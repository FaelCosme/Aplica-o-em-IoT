#pragma once

void motorInit();
void motorUpdate();

void motorIrPara(long posicao);
void motorJog(int direcao);
void motorJogParar();
void motorParar();

long motorPosicaoAtual();
void motorSetPosicao(long pos);
bool motorEmMovimento();