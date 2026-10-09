#pragma once

void comunicacaoInit();
void comunicacaoUpdate();

void comunicacaoPublicarEstado(const char* estado);
void comunicacaoPublicarPosicao(int posicao);

// Callback que o main registra:
//   cmd = "OPEN" | "CLOSE" | "STOP" | "GO_TO" | "SET_TOTAL" | "RESTART"
//   valor = 0..100 quando cmd = "GO_TO"
//           passos totais quando cmd = "SET_TOTAL"
//           -1 nos demais
typedef void (*CallbackComandoHA)(const char* cmd, long valor);
void comunicacaoSetCallback(CallbackComandoHA cb);