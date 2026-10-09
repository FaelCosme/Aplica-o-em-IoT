#include "rede/comunicacao.h"
#include "config.h"
#include "logica/posicoes.h"
#include <WiFi.h>
#include <ArduinoHA.h>

static WiFiClient client;
static HADevice device(HA_DEVICE_ID);
static HAMqtt mqtt(client, device);

static HACover  cover("esp32_cortina_01_cover");
static HANumber passosTotaisHA("esp32_cortina_01_passos_totais");
static HANumber posicaoAlvoHA("esp32_cortina_01_posicao_alvo");
static HAButton botaoReiniciar("esp32_cortina_01_reiniciar");   // <-- novo

static CallbackComandoHA callbackExterno = nullptr;

// ---------- Callbacks do HA ----------

static void onCoverCommand(HACover::CoverCommand cmd, HACover* sender) {
  if (!callbackExterno) return;
  switch (cmd) {
    case HACover::CommandOpen:  callbackExterno("ABRIR",  -1); break;
    case HACover::CommandClose: callbackExterno("FECHAR", -1); break;
    case HACover::CommandStop:  callbackExterno("PARAR",  -1); break;
  }
  (void)sender;
}

static void onPassosTotaisCommand(HANumeric number, HANumber* sender) {
  if (!number.isSet()) return;
  long v = (long)number.toInt32();
  if (v <= 0) return;

  posicoesSalvarPassosTotais(v);
  if (callbackExterno) callbackExterno("SET_TOTAL", v);
  sender->setState(number);
}

static void onPosicaoAlvoCommand(HANumeric number, HANumber* sender) {
  if (!number.isSet()) return;
  long pct = (long)number.toInt32();
  if (callbackExterno) callbackExterno("IR_PARA", pct);
  sender->setState(number);
}

static void onBotaoReiniciarCommand(HAButton* sender) {
  if (callbackExterno) callbackExterno("RESTART", -1);
  (void)sender;
}

// ---------- API pública ----------

void comunicacaoInit() {
  // Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Wi-Fi OK. IP: ");
  Serial.println(WiFi.localIP());

  // Device
  device.setName(HA_DEVICE_NAME);
  device.setManufacturer("DIY");
  device.setModel("ESP32-Cortina");
  device.setSoftwareVersion("1.0.0");
  device.enableSharedAvailability();
  device.setAvailability(true);
  device.enableLastWill();

  // Cover 
  cover.setName("Cortina");
  cover.setDeviceClass("shutter");
  cover.onCommand(onCoverCommand);

  // Number: passos totais
  passosTotaisHA.setName("Passos Totais");
  passosTotaisHA.setIcon("mdi:ruler");
  passosTotaisHA.setMin(100);
  passosTotaisHA.setMax(100000);
  passosTotaisHA.setStep(1);
  passosTotaisHA.setMode(HANumber::ModeBox);
  passosTotaisHA.onCommand(onPassosTotaisCommand);
  passosTotaisHA.setState((float)posicoesPassosTotais());

  // Number: posição alvo
  posicaoAlvoHA.setName("Posição Alvo");
  posicaoAlvoHA.setIcon("mdi:swap-horizontal");
  posicaoAlvoHA.setMin(0);
  posicaoAlvoHA.setMax(100);
  posicaoAlvoHA.setStep(1);
  posicaoAlvoHA.setMode(HANumber::ModeSlider);
  posicaoAlvoHA.setUnitOfMeasurement("%");
  posicaoAlvoHA.onCommand(onPosicaoAlvoCommand);
  posicaoAlvoHA.setState((float)0);

  // Botão de reiniciar
  botaoReiniciar.setName("Reiniciar ESP");
  botaoReiniciar.setIcon("mdi:restart");
  botaoReiniciar.setDeviceClass("restart");
  botaoReiniciar.onCommand(onBotaoReiniciarCommand);

  // MQTT
  mqtt.onConnected([]() {
  Serial.println("[MQTT] Conectado ao broker!");
});

mqtt.onDisconnected([]() {
  Serial.println("[MQTT] DESCONECTADO do broker!");
});

  mqtt.begin(MQTT_BROKER_ADDR, MQTT_USERNAME, MQTT_PASSWORD);
}

void comunicacaoUpdate() {
  mqtt.loop();
}

void comunicacaoPublicarEstado(const char* estado) {
  if      (strcmp(estado, "CORTINA ABERTA")    == 0) cover.setCurrentState(HACover::StateOpen);
  else if (strcmp(estado, "CORTINA FECHADA")  == 0) cover.setCurrentState(HACover::StateClosed);
  else if (strcmp(estado, "CORTINA ABRINDO") == 0) cover.setCurrentState(HACover::StateOpening);
  else if (strcmp(estado, "CORTINA FECHANDO") == 0) cover.setCurrentState(HACover::StateClosing);
  else                                     cover.setCurrentState(HACover::StateStopped);
}

void comunicacaoPublicarPosicao(int posicao) {
  if (posicao < 0)   posicao = 0;
  if (posicao > 100) posicao = 100;
  cover.setPosition(posicao);
  posicaoAlvoHA.setState((float)posicao);
}

void comunicacaoSetCallback(CallbackComandoHA cb) {
  callbackExterno = cb;
}