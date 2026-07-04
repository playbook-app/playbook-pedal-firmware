/**
 * PlayBook Pedal Firmware v1.0
 * MIT License · https://github.com/playbook-app/playbook-pedal-firmware
 *
 * Bluetooth HID keyboard emulator for ESP32-WROOM-32.
 * Emula um teclado Bluetooth genérico com 2 ou 4 botões.
 *
 * Suporte a duas variantes:
 *  - 2-BOTÕES: vira página (PgUp / PgDn)
 *  - 4-BOTÕES: vira página + muda música (PgUp / PgDn / ← / →)
 *
 * Mude NUM_BUTTONS abaixo pra bater com o seu enclosure.
 *
 * Requer biblioteca ESP32-BLE-Keyboard (T-vK):
 * https://github.com/T-vK/ESP32-BLE-Keyboard
 */

#include <BleKeyboard.h>

// ============ CONFIGURAÇÃO ============
#define NUM_BUTTONS 2                  // 2 (default) ou 4
#define DEVICE_NAME "PlayBook Pedal"
#define DEBOUNCE_MS 150
#define LED_PIN 2

BleKeyboard bleKeyboard(DEVICE_NAME, "PlayBook", 100);

// ---- 2-BOTÕES · vira página ----
#if NUM_BUTTONS == 2
  const int BUTTON_PINS[] = {32, 33};
  const uint8_t KEYS[]    = {KEY_PAGE_UP, KEY_PAGE_DOWN};
#endif

// ---- 4-BOTÕES · vira página + muda música ----
#if NUM_BUTTONS == 4
  const int BUTTON_PINS[] = {32, 33, 25, 26};
  const uint8_t KEYS[]    = {KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_LEFT_ARROW, KEY_RIGHT_ARROW};
#endif

unsigned long lastPress[NUM_BUTTONS] = {0};

void setup() {
  Serial.begin(115200);
  Serial.println("PlayBook Pedal iniciando...");

  pinMode(LED_PIN, OUTPUT);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  bleKeyboard.begin();
  Serial.println("BLE HID pronto. Aguardando emparelhamento...");
}

void loop() {
  if (bleKeyboard.isConnected()) {
    digitalWrite(LED_PIN, HIGH);      // LED aceso = conectado
    checkButtons();
  } else {
    // Pulso lento indica procurando conexão
    digitalWrite(LED_PIN, (millis() / 500) % 2);
  }

  delay(10);
}

void checkButtons() {
  unsigned long now = millis();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW) {   // pressionado (INPUT_PULLUP invertido)
      if (now - lastPress[i] > DEBOUNCE_MS) {
        bleKeyboard.write(KEYS[i]);
        Serial.printf("Botão %d pressionado -> tecla 0x%02X\n", i + 1, KEYS[i]);
        lastPress[i] = now;
      }
    }
  }
}
