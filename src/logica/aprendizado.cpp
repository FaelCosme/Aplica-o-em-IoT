#include "logica/aprendizado.h"
#include "logica/posicoes.h"
#include <Arduino.h>

static bool ativo = false;
static int  etapa = 0;

void aprendizadoInit() {
  ativo = false;
  etapa = 0;
}

void aprendizadoEntrar() {
  ativo = true;
  etapa = 0;
  Serial.println(">> APRENDIZADO ativo");
  Serial.println("   SEGURE btn1 ou btn2 para mover");
  Serial.println("   Aperte btnSave para gravar");
}

void aprendizadoSair() {
  ativo = false;
  Serial.println(">> MODO NORMAL");
}

bool aprendizadoAtivo() { return ativo; }
int  aprendizadoEtapa() { return etapa; }

void aprendizadoSalvar(long posicaoAtual) {
  if (etapa == 0) {
    posicoesSalvarAberta(posicaoAtual);
    Serial.printf("Salvou ABERTA = %ld\r\n", posicaoAtual);
    etapa = 1;
  } else {
    posicoesSalvarFechada(posicaoAtual);
    Serial.printf("Salvou FECHADA = %ld\r\n", posicaoAtual);
    etapa = 0;
    ativo = false;
    Serial.println(">> MODO NORMAL (saida automatica)");
  }
}