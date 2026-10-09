#pragma once

// Módulo que guarda o tamanho do percurso (número total de passos).
// Convenção: 0 = fechada, passosTotais = aberta.

void posicoesInit();
long posicoesPassosTotais();
void posicoesSalvarPassosTotais(long v);