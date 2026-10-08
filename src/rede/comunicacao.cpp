#include "rede/comunicacao.h"
#include "config.h"
#include <WiFi.h>
#include <ArduinoHA.h>

static WiFiClient client;
static HADevice device(HA_DEVICE_ID);
static HAMqtt mqtt(client, device);

static HACover cover("esp32_cortina_01_cover");
static HASwitch switchAprendizado("esp32_cortina_01_learning");

static CallbackComandoHA callbackExterno = nullptr;

// ---------- Callbacks do HA ----------

static void onCoverCommand(HACover::CoverCommand cmd, HACover* sender) {
  if (!callbackExterno) return;
  switch (cmd) {
    case HACover::CommandOpen:  callbackExterno("OPEN",  -1); break;
    case HACover::CommandClose: callbackExterno("CLOSE", -1); break;
    case HACover::CommandStop:  callbackExterno("STOP",  -1); break;
  }
  (void)sender;
}

// onPosition removido — não existe nesta versão da ArduinoHA.
// A posição é publicada periodicamente via comunicacaoPublicarPosicao().

static void onLearningSwitch(bool state, HASwitch* sender) {
  if (callbackExterno) {
    callbackExterno(state ? "LEARNING_ON" : "LEARNING_OFF", -1);
  }
  sender->setState(state);
}

// ---------- API pública ----------

void comunicacaoInit() {
  // ----- Wi-Fi -----
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Wi-Fi OK. IP: ");
  Serial.println(WiFi.localIP());

  // ----- Device -----
  device.setName(HA_DEVICE_NAME);
  device.setManufacturer("DIY");
  device.setModel("ESP32-Cortina");
  device.setSoftwareVersion("1.0.0");
  device.enableSharedAvailability();
  device.setAvailability(true);
  device.enableLastWill();

  // ----- Cover (cortina) -----
  cover.setName("Cortina");
  cover.setDeviceClass("curtain");   // string em vez da constante
  cover.onCommand(onCoverCommand);
  // cover.onPosition(onCoverPosition);  // <-- removido

  // ----- Switch de aprendizado -----
  switchAprendizado.setName("Modo Aprendizado");
  switchAprendizado.setIcon("mdi:cog");
  switchAprendizado.onCommand(onLearningSwitch);

  // ----- MQTT -----
  mqtt.onConnected([]() {
  Serial.println("✅ Conectado ao broker MQTT!");
});
  mqtt.begin(MQTT_BROKER_ADDR, MQTT_USERNAME, MQTT_PASSWORD);
}

void comunicacaoUpdate() {
  mqtt.loop();
}

void comunicacaoPublicarEstado(const char* estado) {
  if      (strcmp(estado, "open")    == 0) cover.setCurrentState(HACover::StateOpen);
  else if (strcmp(estado, "closed")  == 0) cover.setCurrentState(HACover::StateClosed);
  else if (strcmp(estado, "opening") == 0) cover.setCurrentState(HACover::StateOpening);
  else if (strcmp(estado, "closing") == 0) cover.setCurrentState(HACover::StateClosing);
  else                                     cover.setCurrentState(HACover::StateStopped);
}

void comunicacaoPublicarPosicao(int posicao) {
  if (posicao < 0)   posicao = 0;
  if (posicao > 100) posicao = 100;
  cover.setPosition(posicao);   // método disponível nesta versão
}

void comunicacaoSetCallback(CallbackComandoHA cb) {
  callbackExterno = cb;
}