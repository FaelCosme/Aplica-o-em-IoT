#include "hardware/botoes.h"
#include "config.h"
#include <Arduino.h>

struct Debounce {
  bool estado;                 // estado estável (true = pressionado)
  bool ultimaLeitura;
  unsigned long ultimaMudanca;
  bool edgeDownPending;
};

static Debounce d1    = { false, false, 0, false };
static Debounce d2    = { false, false, 0, false };
static Debounce dSave = { false, false, 0, false };

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
  pinMode(PINO_BTN1,    INPUT_PULLUP);
  pinMode(PINO_BTN2,    INPUT_PULLUP);
  pinMode(PINO_BTNSAVE, INPUT_PULLUP);
}

void botoesUpdate() {
  updateDebounce(PINO_BTN1,    d1);
  updateDebounce(PINO_BTN2,    d2);
  updateDebounce(PINO_BTNSAVE, dSave);
}

bool btn1Apertou()     { return consumeEdgeDown(d1); }
bool btn1Pressionado() { return d1.estado; }

bool btn2Apertou()     { return consumeEdgeDown(d2); }
bool btn2Pressionado() { return d2.estado; }

bool btnSaveApertou()  { return consumeEdgeDown(dSave); }