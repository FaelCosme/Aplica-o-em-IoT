#include "config.h"
#include "hardware/motor.h"
#include "hardware/botoes.h"
#include "logica/posicoes.h"
#include "rede/comunicacao.h"

#include <Arduino.h>
#include <string.h>

static unsigned long ultimaPubHA = 0;

// -------- Conversão passos <-> porcentagem --------
// 0 = fechada (0%), passosTotais = aberta (100%).
static int passosParaPorcento(long pos) {
  long total = posicoesPassosTotais();
  if (total <= 0) return 0;
  long pct = pos * 100L / total;
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;
  return (int)pct;
}

static long porcentoParaPassos(int pct) {
  if (pct < 0)   pct = 0;
  if (pct > 100) pct = 100;
  return posicoesPassosTotais() * pct / 100;
}

// -------- Comandos vindos do HA --------
static void onComandoHA(const char* cmd, long valor) {
  if (strcmp(cmd, "ABRIR") == 0) {
    motorIrPara(posicoesPassosTotais());
    // comunicacaoPublicarEstado("CORTINA ABRINDO");
  } else if (strcmp(cmd, "FECHAR") == 0) {
    motorIrPara(0);
    // comunicacaoPublicarEstado("CORTINA FECHANDO");
  } else if (strcmp(cmd, "PARAR") == 0) {
    motorParar();
    comunicacaoPublicarEstado("CORTINA PARADA");
  } else if (strcmp(cmd, "IR_PARA") == 0) {
    motorIrPara(porcentoParaPassos((int)valor));
    Serial.printf("[HA] IR_PARA %ld%%\r\n", valor);
  } else if (strcmp(cmd, "SET_TOTAL") == 0) {
    Serial.printf("[HA] PASSOS_TOTAIS = %ld\r\n", valor);
  } else if (strcmp(cmd, "RESTART") == 0) {
    if (motorEmMovimento()) {
      Serial.println("[HA] RESTART ignorado: motor em movimento");
    } else {
      Serial.println("[HA] Reiniciando ESP32...");
      delay(200);          // tempo para o log sair
      ESP.restart();
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\r\n--- Trabalho-IoT: Cortina ---");

  motorInit();
  botoesInit();
  posicoesInit();

  // Assume cortina FECHADA na energização
  motorSetPosicao(0);

  comunicacaoSetCallback(onComandoHA);
  comunicacaoInit();

  Serial.printf("Boot: PASSOS_TOTAIS=%ld\r\n", posicoesPassosTotais());
}

void loop() {
  motorUpdate();
  botoesUpdate();
  comunicacaoUpdate();

  // ==== Botões físicos ====
  if (btnAbrirApertou()) {
    motorIrPara(posicoesPassosTotais());
    Serial.printf("[ABRIR] alvo=%ld\r\n", posicoesPassosTotais());
  }
  if (btnFecharApertou()) {
    motorIrPara(0);
    Serial.println("[FECHAR] alvo=0");
  }

  // ==== Detecta quando o motor para e publica estado final ====
  static bool estavaMovendo = false;
  static unsigned long parouEm = 0;

  bool movendoAgora = motorEmMovimento();

  if (estavaMovendo && !movendoAgora) {
    parouEm = millis();   // acabou de parar
  }

  if (!movendoAgora && parouEm != 0 && (millis() - parouEm >= 300)) {
    parouEm = 0;
    long pos   = motorPosicaoAtual();
    long total = posicoesPassosTotais();

    if (pos >= total - 50) {
      comunicacaoPublicarEstado("ABERTO");
      Serial.println("[ESTADO] ABERTO");
    } else if (pos <= 50) {
      comunicacaoPublicarEstado("FECHADO");
      Serial.println("[ESTADO] FECHADO");
    } else {
      comunicacaoPublicarEstado("PARADO");
      Serial.println("[ESTADO] PARADO (parcial)");
    }
  }

  estavaMovendo = movendoAgora;

  // ==== Publica posição no HA a cada INTERVALO_PUB_HA ms ====
  unsigned long agora = millis();
  if (agora - ultimaPubHA >= INTERVALO_PUB_HA) {
    ultimaPubHA = agora;
    int pct = passosParaPorcento(motorPosicaoAtual());
    comunicacaoPublicarPosicao(pct);
  }
}