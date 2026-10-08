#pragma once

void aprendizadoInit();
void aprendizadoEntrar();
void aprendizadoSair();
bool aprendizadoAtivo();
int  aprendizadoEtapa();                  // 0 = próxima gravação é ABERTA; 1 = FECHADA
void aprendizadoSalvar(long posicaoAtual); // grava e, se era FECHADA, sai do modo