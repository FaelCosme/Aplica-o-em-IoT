#include "AccelStepper.h"
#include <Preferences.h>

#define stepPin 4
#define dirPin  5
#define motorInterfaceType 1

const int pinoBtn1    = 21;
const int pinoBtn2    = 22;
const int pinoBtnSave = 23;

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
Preferences prefs;

const float velJog    = 600.0; // velocidade de jog (botão pressionado)
const float velAuto   = 1000.0; // velocidade de movimento automático (apertou botão e soltou)
const float accelAuto = 800.0; // aceleração de movimento automático (apertou botão e soltou)
const float accelJog  = 1500.0; // aceleração de jog (botão pressionado)

const unsigned long TEMPO_LONGO     = 5000;   // 5s btn1 = aprendizado
const unsigned long DEBOUNCE_MS     = 50;     // tempo de estabilização do botão
const unsigned long INTERVALO_SAVE  = 800;    // tempo mínimo entre salvamentos

long posicaoAberta  = 0; 
long posicaoFechada = 0;

bool modoAprendizado = false;
int  etapaSalvar     = 0;

// --- estados de debounce (por botão) ---
struct Debounce {
  bool estadoEstavel;      // estado "firmado" (true = pressionado)
  bool ultimaLeitura;      // ultima leitura crua
  unsigned long ultimaMudanca;
};

Debounce d1    = { false, false, 0 };
Debounce d2    = { false, false, 0 };
Debounce dSave = { false, false, 0 };

// Retorna true somente na transição "soltou -> pressionou" (após debounce)
bool leBotaoDebounced(int pino, Debounce &d) {
  bool leitura = (digitalRead(pino) == LOW);
  if (leitura != d.ultimaLeitura) {
    d.ultimaMudanca = millis();
    d.ultimaLeitura = leitura;
  }
  if ((millis() - d.ultimaMudanca) >= DEBOUNCE_MS) {
    if (d.estadoEstavel != leitura) {
      bool pressaoNova = (leitura == true);
      d.estadoEstavel = leitura;
      return pressaoNova;   // true apenas na borda de pressão
    }
  }
  return false;
}

// Retorna o estado "pressionado" (já debounced), útil para o jog
bool estaPressionado(Debounce &d) {
  return d.estadoEstavel;
}

unsigned long b1PressTime = 0;
bool b1LongFired = false;
unsigned long ultimoSave = 0;
unsigned long ultimoPrint = 0;

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(velAuto);
  stepper.setAcceleration(accelAuto);

  pinMode(pinoBtn1, INPUT_PULLUP);
  pinMode(pinoBtn2, INPUT_PULLUP);
  pinMode(pinoBtnSave, INPUT_PULLUP);

  prefs.begin("cortina", false);
  posicaoAberta  = prefs.getLong("aberta", 0);
  posicaoFechada = prefs.getLong("fechada", 0);

  stepper.setCurrentPosition(posicaoFechada);

  Serial.printf("Boot: ABERTA=%ld FECHADA=%ld\n", posicaoAberta, posicaoFechada);
  Serial.println("Modo NORMAL");
}

void loop() {
  // Borda de pressão (debounced) de cada botão
  bool novoBtn1    = leBotaoDebounced(pinoBtn1, d1);
  bool novoBtn2    = leBotaoDebounced(pinoBtn2, d2);
  bool novoBtnSave = leBotaoDebounced(pinoBtnSave, dSave);

  // Estado "está pressionado" (para o jog)
  bool b1    = estaPressionado(d1);
  bool b2    = estaPressionado(d2);

  unsigned long agora = millis();

  if (!modoAprendizado) {
    // ---------------- MODO NORMAL ----------------

    if (novoBtn1) {
      b1PressTime = agora;
      b1LongFired = false;
    }

    // Curto no btn1 = abrir (mas só se não virou longo)
    if (!b1 && !b1LongFired && (b1PressTime != 0) &&
        (agora - b1PressTime < TEMPO_LONGO)) {
      // Já soltou e não completou o tempo longo
      // (checamos via transição de "não pressionado" após ter pressionado)
    }

    // Detecta soltura: se não está mais pressionado e houve um início de pressão
    // Usaremos uma flag simples para saber que foi soltura
    static bool b1JaEstavaPressionado = false;
    if (b1) {
      b1JaEstavaPressionado = true;
    } else if (b1JaEstavaPressionado) {
      b1JaEstavaPressionado = false;
      if (!b1LongFired) {
        stepper.moveTo(posicaoAberta);
        Serial.println("[ABRIR]");
        Serial.print(" atual=");
        Serial.println(stepper.currentPosition());
        Serial.print(" alvo=");
        Serial.println(posicaoAberta);
      }
    }

    // Segurou tempo suficiente = aprendizado
    if (b1 && !b1LongFired && (b1PressTime != 0) &&
        (agora - b1PressTime >= TEMPO_LONGO)) {
      b1LongFired = true;
      modoAprendizado = true;
      etapaSalvar = 0;
      Serial.println(">> APRENDIZADO ativo");
      Serial.println("   SEGURE btn1 ou btn2 para mover");
      Serial.println("   Aperte btnSave para gravar");
    }

    if (novoBtn2) {
      stepper.moveTo(posicaoFechada);
      Serial.println("[FECHAR]");
      Serial.print(" atual=");
      Serial.println(stepper.currentPosition());
      Serial.print(" alvo=");
      Serial.println(posicaoFechada);
    }

    stepper.run();

  } else {
    // ------------- MODO APRENDIZADO -------------

    if (b1 && !b2) {
      if (stepper.distanceToGo() < 200) {
        stepper.moveTo(stepper.currentPosition() + 100000);
      }
      stepper.setMaxSpeed(velJog);
      stepper.setAcceleration(accelJog);
      stepper.run();

      if (agora - ultimoPrint > 300) {
        Serial.println("[JOG+]");
        Serial.print(" pos=");
        Serial.println(stepper.currentPosition());
        Serial.print(" dist=");
        Serial.println(stepper.distanceToGo());
        ultimoPrint = agora;
      }
    } else if (b2 && !b1) {
      if (stepper.distanceToGo() > -200) {
        stepper.moveTo(stepper.currentPosition() - 100000);
      }
      stepper.setMaxSpeed(velJog);
      stepper.setAcceleration(accelJog);
      stepper.run();

      if (agora - ultimoPrint > 300) {
        Serial.println("[JOG-]");
        Serial.print(" pos=");
        Serial.println(stepper.currentPosition());
        Serial.print(" dist=");
        Serial.println(stepper.distanceToGo());
        ultimoPrint = agora;
      }
    } else {
      if (stepper.distanceToGo() != 0) {
        stepper.moveTo(stepper.currentPosition());
      }
      stepper.setMaxSpeed(velJog);
      stepper.setAcceleration(accelJog);
      stepper.run();
    }

    // Salvar: só na borda + respeita intervalo mínimo entre salvamentos
    if (novoBtnSave && (agora - ultimoSave >= INTERVALO_SAVE)) {
      ultimoSave = agora;
      long pos = stepper.currentPosition();
      if (etapaSalvar == 0) {
        posicaoAberta = pos;
        prefs.putLong("aberta", posicaoAberta);
        Serial.printf("Salvou ABERTA = %ld\n", posicaoAberta);
        etapaSalvar = 1;
      } else {
        posicaoFechada = pos;
        prefs.putLong("fechada", posicaoFechada);
        Serial.printf("Salvou FECHADA = %ld\n", posicaoFechada);
        etapaSalvar = 0;
        modoAprendizado = false;
        Serial.println(">> MODO NORMAL");
        Serial.printf("Finais: ABERTA=%ld FECHADA=%ld\n",
                      posicaoAberta, posicaoFechada);
      }
    }
  }
}