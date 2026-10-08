#include "config.h"
#include "hardware/motor.h"
#include "hardware/botoes.h"
#include "logica/posicoes.h"
#include "logica/aprendizado.h"
#include "rede/comunicacao.h"

#include <Arduino.h>
#include <string.h>

// -------- Estado do botão 1 (toque curto x longo) --------
static bool          btn1Aguardando = false;
static unsigned long btn1PressTime  = 0;
static bool          btn1LongFired  = false;

static unsigned long ultimoSave    = 0;
static unsigned long ultimaPubHA   = 0;

// -------- Conversão passos <-> porcentagem --------
static int posicaoParaPorcento(long pos) {
  long a = posicoesAberta();
  long f = posicoesFechada();
  if (a == f) return 0;
  long pct = (pos - f) * 100L / (a - f);
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;
  return (int)pct;
}

static long porcentoParaPosicao(int pct) {
  long a = posicoesAberta();
  long f = posicoesFechada();
  return f + (a - f) * pct / 100;
}

// -------- Comandos vindos do Home Assistant --------
static void onComandoHA(const char* cmd, int valor) {
  if (strcmp(cmd, "OPEN") == 0) {
    motorIrPara(posicoesAberta());
    comunicacaoPublicarEstado("opening");
  } else if (strcmp(cmd, "CLOSE") == 0) {
    motorIrPara(posicoesFechada());
    comunicacaoPublicarEstado("closing");
  } else if (strcmp(cmd, "STOP") == 0) {
    motorParar();
    comunicacaoPublicarEstado("stopped");
  } else if (strcmp(cmd, "POSITION") == 0) {
    motorIrPara(porcentoParaPosicao(valor));
  } else if (strcmp(cmd, "LEARNING_ON") == 0) {
    aprendizadoEntrar();
  } else if (strcmp(cmd, "LEARNING_OFF") == 0) {
    aprendizadoSair();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\r\n--- Trabalho-IoT: Cortina ---");

  motorInit();
  botoesInit();
  posicoesInit();
  aprendizadoInit();

  // Assume que a cortina começa FECHADA na energização
  motorSetPosicao(posicoesFechada());

  comunicacaoSetCallback(onComandoHA);
  comunicacaoInit();

  Serial.printf("Boot: ABERTA=%ld FECHADA=%ld\r\n",
                posicoesAberta(), posicoesFechada());
  Serial.println("Modo NORMAL");
}

void loop() {
  motorUpdate();
  botoesUpdate();
  comunicacaoUpdate();

  unsigned long agora = millis();

  if (aprendizadoAtivo()) {
    // ================= MODO APRENDIZADO =================
    if (btn1Pressionado() && !btn2Pressionado()) {
      motorJog(+1);
    } else if (btn2Pressionado() && !btn1Pressionado()) {
      motorJog(-1);
    } else {
      motorJogParar();
    }

    if (btnSaveApertou() && (agora - ultimoSave >= INTERVALO_SAVE)) {
      ultimoSave = agora;
      aprendizadoSalvar(motorPosicaoAtual());
    }

  } else {
    // ================== MODO NORMAL =====================

    // btn1 — detecta curto vs longo
    if (btn1Apertou()) {
      btn1Aguardando = true;
      btn1PressTime  = agora;
      btn1LongFired  = false;
    }

    if (btn1Aguardando && !btn1Pressionado()) {
      // Soltou: se não virou "longo", foi toque curto = ABRIR
      if (!btn1LongFired) {
        motorIrPara(posicoesAberta());
        Serial.printf("[ABRIR] atual=%ld alvo=%ld\r\n",
                      motorPosicaoAtual(), posicoesAberta());
      }
      btn1Aguardando = false;
      btn1LongFired  = false;
    }

    if (btn1Aguardando && btn1Pressionado() && !btn1LongFired &&
        (agora - btn1PressTime >= TEMPO_LONGO)) {
      btn1LongFired = true;
      aprendizadoEntrar();
    }

    // btn2 — fechar
    if (btn2Apertou()) {
      motorIrPara(posicoesFechada());
      Serial.printf("[FECHAR] atual=%ld alvo=%ld\r\n",
                    motorPosicaoAtual(), posicoesFechada());
    }
  }

  // Publica posição periódica no HA
  if (agora - ultimaPubHA >= INTERVALO_PUB_HA) {
    ultimaPubHA = agora;
    comunicacaoPublicarPosicao(posicaoParaPorcento(motorPosicaoAtual()));
  }
}