#pragma once

void motorInit();
void motorUpdate();            // chamar dentro do loop()

void motorIrPara(long posicao); // move suave até a posição
void motorJog(int direcao);     // +1 ou -1 — movimento contínuo (jog)
void motorJogParar();           // freia no lugar
void motorParar();              // aborta o movimento atual

long motorPosicaoAtual();
void motorSetPosicao(long pos);
bool motorEmMovimento();