#include "hardware/botoes.h"
#include "config.h"
#include <Arduino.h>

struct Debounce {
  bool estado;
  bool ultimaLeitura;
  unsigned long ultimaMudanca;
  bool edgeDownPending;
};

static Debounce d1 = { false, false, 0, false };
static Debounce d2 = { false, false, 0, false };

static void updateDebounce(int pino, Debounce &d) {
  bool leitura = (digitalRead(pino) == LOW);
  if (leitura != d.ultimaLeitura) {
    d.ultimaMudanca = millis();
    d.ultimaLeitura = leitura;
  }
  if ((millis() - d.ultimaMudanca) >= DEBOUNCE_MS) {
    if (d.estado != leitura) {
      if (leitura) d.edgeDownPending = true;
      d.estado = leitura;
    }
  }
}

static bool consumeEdgeDown(Debounce &d) {
  if (d.edgeDownPending) { d.edgeDownPending = false; return true; }
  return false;
}

void botoesInit() {
  pinMode(PINO_BTN_ABRIR, INPUT_PULLUP);
  pinMode(PINO_BTN_FECHAR, INPUT_PULLUP);
}

void botoesUpdate() {
  updateDebounce(PINO_BTN_ABRIR, d1);
  updateDebounce(PINO_BTN_FECHAR, d2);
}

bool btnAbrirApertou()     { return consumeEdgeDown(d1); }
bool btnAbrirPressionado() { return d1.estado; }

bool btnFecharApertou()     { return consumeEdgeDown(d2); }
bool btnFecharPressionado() { return d2.estado; }