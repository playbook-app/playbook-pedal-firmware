/**
 * PlayBook Pedal Firmware v1.2 (ESP32-S3)
 * MIT License · https://github.com/playbook-app/playbook-pedal-firmware
 *
 * Teclado Bluetooth (BLE HID) pra ESP32-WROOM-32 — vira página no PlayBook
 * (e em qualquer app, já que é um teclado genérico).
 *
 * Manda SEMPRE teclas de teclado limpas (PgUp/PgDn/setas), uma por pisada.
 * Nada de teclas de mídia (MediaTrackNext e afins): pedais que misturam
 * mídia + seta confundem o remapeamento dos apps. Aqui cada botão emite um
 * código só, estável, que casa com o mapa padrão do PlayBook:
 *   PgDn → rolar pra baixo     ←  → música anterior/próxima (variante 4b)
 *   PgUp → rolar pra cima
 *
 * Hardware oficial: ESP32-S3 (WROOM-1, 44 pinos, duplo USB-C). Placa ÚNICA
 * do projeto — o mesmo módulo faz BLE (este firmware) e, no futuro, o modo
 * cabeado por USB HID nativo (firmware em desenvolvimento). O ESP32 clássico
 * foi descartado: não faz USB HID nativo e custa mais.
 *
 * Chaves entre o GPIO e o GND (pull-up interno; pisado = LOW), alimentação
 * pelo pino 5V0 — nunca ligar bateria direto no 3V3.
 *
 * GPIOs a EVITAR no S3: 0, 3, 45, 46 (afetam o boot), 19 e 20 (USB) e 26-37
 * (memória do módulo). Livres pra sobra: 8, 16, 17, 18, 21.
 *
 * Requer a biblioteca ESP32-BLE-Keyboard (T-vK):
 *   https://github.com/T-vK/ESP32-BLE-Keyboard
 */

#include <BleKeyboard.h>

// ============ CONFIGURAÇÃO ============
#define NUM_BUTTONS 2 // 2 (este protótipo) ou 4
#define DEVICE_NAME "PlayBook Pedal"
#define MANUFACTURER "PlayBook"
#define DEBOUNCE_MS 25 // janela de estabilização do contato (borda)
// LED de status EXTERNO (3mm + resistor 220R) -> GND. Não usamos o LED
// onboard do S3: ele é RGB endereçável (WS2812) no GPIO 48 e exigiria outra
// biblioteca só pra piscar.
#define LED_PIN 15

BleKeyboard bleKeyboard(DEVICE_NAME, MANUFACTURER, 100);

// ---- 2 botões · vira página ----
// GPIO 4 = volta (PgUp) · GPIO 5 = avança (PgDn/rola pra baixo). Ordem casada
// com o layout físico do enclosure (o botão de avançar é o do 5). Se um dia
// inverter, é só trocar a ordem aqui de novo.
#if NUM_BUTTONS == 2
const int BUTTON_PINS[] = {4, 5};
const uint8_t KEYS[] = {KEY_PAGE_UP, KEY_PAGE_DOWN};
const char *NAMES[] = {"PgUp", "PgDn"};
#endif

// ---- 4 botões · vira página + muda música ----
// GPIO 6 = próxima música · GPIO 7 = música anterior. (Os pinos antigos
// 32/33/25/26 eram do ESP32 clássico e caem na faixa 26-37, reservada à
// memória no S3 — não servem nesta placa.)
#if NUM_BUTTONS == 4
const int BUTTON_PINS[] = {4, 5, 6, 7};
const uint8_t KEYS[] = {KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_RIGHT_ARROW,
                        KEY_LEFT_ARROW};
const char *NAMES[] = {"PgUp", "PgDn", "Right", "Left"};
#endif

// Estado de debounce por borda: dispara UMA tecla na transição
// solto→apertado. Segurar NÃO repete (page-turner vira 1 página por toque).
struct ButtonState {
  bool stable;        // último estado estável (true = apertado)
  bool lastReading;   // última leitura crua
  unsigned long tChange; // quando a leitura crua mudou pela última vez
};
ButtonState buttons[NUM_BUTTONS];

// pisca o LED N vezes (feedback de boot/eventos), bloqueante e curto
void blinkLed(int times, int ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(ms);
    digitalWrite(LED_PIN, LOW);
    delay(ms);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("PlayBook Pedal v1.2 (ESP32-S3) — iniciando...");

  pinMode(LED_PIN, OUTPUT);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    // INPUT_PULLUP: solto = HIGH, apertado = LOW. Guardamos "apertado" como
    // true, então invertemos a leitura crua.
    buttons[i] = {false, false, 0};
  }

  bleKeyboard.begin();
  Serial.printf("BLE HID pronto (%d botões). Aguardando emparelhamento...\n",
                NUM_BUTTONS);
  blinkLed(3, 120); // 3 piscadas = boot ok
}

void loop() {
  static bool wasConnected = false;
  const bool connected = bleKeyboard.isConnected();

  // borda de conexão: log + piscada
  if (connected != wasConnected) {
    wasConnected = connected;
    Serial.println(connected ? "Conectado." : "Desconectado.");
    if (connected) blinkLed(2, 80);
  }

  if (connected) {
    digitalWrite(LED_PIN, HIGH); // aceso fixo = conectado e pronto
    scanButtons();
  } else {
    // pulso lento = anunciando/procurando host
    digitalWrite(LED_PIN, (millis() / 500) % 2 ? HIGH : LOW);
  }

  delay(5);
}

void scanButtons() {
  const unsigned long now = millis();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    // apertado = LOW no pino (pull-up) → inverte pra lógica "pressed"
    const bool pressed = (digitalRead(BUTTON_PINS[i]) == LOW);

    // reinicia o cronômetro de estabilização a cada mudança de leitura
    if (pressed != buttons[i].lastReading) {
      buttons[i].lastReading = pressed;
      buttons[i].tChange = now;
    }

    // leitura estável por tempo suficiente e diferente do estado atual
    if ((now - buttons[i].tChange) > DEBOUNCE_MS &&
        pressed != buttons[i].stable) {
      buttons[i].stable = pressed;

      // dispara só na descida (solto→apertado); soltar não manda nada
      if (pressed) {
        bleKeyboard.write(KEYS[i]);
        Serial.printf("Botão %d (%s) -> tecla 0x%02X\n", i + 1, NAMES[i],
                      KEYS[i]);
      }
    }
  }
}
