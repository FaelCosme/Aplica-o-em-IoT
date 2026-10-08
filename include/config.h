#pragma once

// ===== Motor =====
#define PINO_STEP        4
#define PINO_DIR         5

// ===== Botões =====
#define PINO_BTN1        21
#define PINO_BTN2        22
#define PINO_BTNSAVE     23

// ===== Parâmetros do motor =====
#define VEL_JOG          600.0
#define VEL_AUTO         1000.0
#define ACCEL_AUTO       800.0
#define ACCEL_JOG        1500.0

// ===== Tempos =====
#define TEMPO_LONGO      2000      // ms de pressão no btn1 para virar "longo"
#define DEBOUNCE_MS      50        // ms de estabilização dos botões
#define INTERVALO_SAVE   800       // ms mínimo entre dois salvamentos
#define INTERVALO_PUB_HA 500       // ms entre publicações no Home Assistant

// ===== NVS =====
#define NVS_NAMESPACE    "cortina"

// ===== Wi-Fi =====
#define WIFI_SSID        "Wokwi-GUEST"
#define WIFI_PASSWORD    ""

// ===== MQTT / Home Assistant =====
<<<<<<< HEAD
#define MQTT_BROKER_ADDR IPAddress(0,0,0,0) //IP do broker MQTT
#define MQTT_USERNAME    "SeuUsuarioMQTT"
#define MQTT_PASSWORD    "SuaSenhaMQTT"
=======
#define MQTT_BROKER_ADDR IPAddress(0,0,0,0) // IP do broker MQTT
#define MQTT_USERNAME    "SeuUsuarioMQTT"
#define MQTT_PASSWORD    "SeuPasswordMQTT"
>>>>>>> 0000f28 (fix: usuário, senha e IP do broker)

#define HA_DEVICE_ID     "esp32_cortina_01"
#define HA_DEVICE_NAME   "ESP32 Cortina"