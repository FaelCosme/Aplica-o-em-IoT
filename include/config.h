#pragma once

// ===== Motor =====
#define PINO_STEP        4
#define PINO_DIR         5

// ===== Botões =====
#define PINO_BTN_ABRIR        21
#define PINO_BTN_FECHAR        22
// (btnSave/GPIO 23 não é mais usado, mas pode ficar ligado sem problema)

// ===== Parâmetros do motor =====
#define VEL_JOG          600.0
#define VEL_AUTO         1000.0
#define ACCEL_AUTO       800.0
#define ACCEL_JOG        1500.0

// ===== Tempos =====
#define DEBOUNCE_MS      50
#define INTERVALO_PUB_HA 500

// ===== Tamanho padrão do percurso =====
// Ajuste conforme sua mecânica. Se preferir, você pode mudar isso
// depois direto pelo HA (slider "Passos Totais").
#define PASSOS_TOTAIS_DEFAULT  1600

// ===== NVS =====
#define NVS_NAMESPACE    "cortina"

// ===== Wi-Fi =====
#define WIFI_SSID        "Wokwi-GUEST"
#define WIFI_PASSWORD    ""

// ===== MQTT / Home Assistant =====
#define MQTT_BROKER_ADDR IPAddress(0,0,0,0) // IP do broker MQTT
#define MQTT_USERNAME    "SeuUsuarioMQTT"
#define MQTT_PASSWORD    "SuaSenhaMQTT"

#define HA_DEVICE_ID     "esp32_cortina_01"
#define HA_DEVICE_NAME   "ESP32 Cortina"