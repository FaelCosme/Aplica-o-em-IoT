#include "logica/posicoes.h"
#include "config.h"
#include <Preferences.h>

static Preferences prefs;
static long _aberta  = 0;
static long _fechada = 0;

void posicoesInit() {
  prefs.begin(NVS_NAMESPACE, false);
  _aberta  = prefs.getLong("aberta",  0);
  _fechada = prefs.getLong("fechada", 0);
}

long posicoesAberta()  { return _aberta;  }
long posicoesFechada() { return _fechada; }

void posicoesSalvarAberta(long v) {
  _aberta = v;
  prefs.putLong("aberta", v);
}

void posicoesSalvarFechada(long v) {
  _fechada = v;
  prefs.putLong("fechada", v);
}