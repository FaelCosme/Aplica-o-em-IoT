#pragma once

void comunicacaoInit();
void comunicacaoUpdate();

// Publica no HA
void comunicacaoPublicarEstado(const char* estado);   // "open" | "closed" | "opening" | "closing" | "stopped"
void comunicacaoPublicarPosicao(int posicao);         // 0..100

// Callback que o main registra para receber comandos:
//   cmd = "OPEN" | "CLOSE" | "STOP" | "POSITION" | "LEARNING_ON" | "LEARNING_OFF"
//   valor = 0..100 quando cmd = "POSITION"; -1 nos demais
typedef void (*CallbackComandoHA)(const char* cmd, int valor);
void comunicacaoSetCallback(CallbackComandoHA cb);