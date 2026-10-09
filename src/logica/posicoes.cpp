#include "logica/posicoes.h"
#include "config.h"
#include <Preferences.h>

static Preferences prefs;
static long _passosTotais = PASSOS_TOTAIS_DEFAULT;

void posicoesInit() {
  prefs.begin(NVS_NAMESPACE, false);
  _passosTotais = prefs.getLong("total", PASSOS_TOTAIS_DEFAULT);
  if (_passosTotais <= 0) _passosTotais = PASSOS_TOTAIS_DEFAULT;
}

long posicoesPassosTotais() {
  return _passosTotais;
}

void posicoesSalvarPassosTotais(long v) {
  if (v <= 0) return;
  _passosTotais = v;
  prefs.putLong("total", v);
}