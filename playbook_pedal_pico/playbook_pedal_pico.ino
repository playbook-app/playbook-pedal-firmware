/**
 * PlayBook Pedal Firmware (Pico) v1.0
 * MIT License * https://github.com/playbook-app/playbook-pedal-firmware
 *
 * Teclado USB (HID) pra Raspberry Pi Pico (RP2040) — variante COM FIO do
 * PlayBook Pedal. Plugou no cabo USB, ligou: aparece como um teclado comum e
 * vira pagina no PlayBook (e em qualquer app, ja que e um teclado generico).
 *
 * Manda SEMPRE teclas de teclado limpas (PgUp/PgDn/setas), UMA por pisada —
 * mesmo comportamento e mesmo mapa padrao do firmware ESP32 (sem fio):
 *   PgDn -> rolar pra baixo     <-  -> musica anterior/proxima (variante 4b)
 *   PgUp -> rolar pra cima
 * Nada de teclas de midia: cada botao emite um codigo so, estavel, que casa
 * com o mapa padrao do PlayBook.
 *
 * Por que Pico nesta variante (e nao ESP32): o ESP32 nao faz teclado-USB
 * nativo; o RP2040 faz USB HID direto pela lib Keyboard. Entao SEM FIO usa o
 * ESP32-S3 (BLE, ver playbook_pedal.ino) e COM FIO usa o Pico (USB, este).
 *
 * COM FIO = alimentado pelo proprio cabo USB. Sem pilha, sem suporte de
 * pilha, sem botao liga/desliga: plugou, ligou.
 *
 * ---- Como compilar (Arduino IDE) ----
 * Core arduino-pico (earlephilhower). Em Arquivo > Preferencias, cole no
 * campo "URLs adicionais de gerenciadores de placas":
 *   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
 * Ferramentas > Placa: "Raspberry Pi Pico".
 * Ferramentas > USB Stack: "Pico SDK" (padrao — a lib Keyboard so existe nele).
 * A lib Keyboard ja vem com o core; nao instale outra.
 * Pra entrar em modo de gravacao na 1a vez: segure BOOTSEL ao plugar o USB.
 */

#include <Keyboard.h>

// ============ CONFIGURACAO ============
#define NUM_BUTTONS 2  // 2 (vira pagina) ou 4 (pagina + musica)
#define DEBOUNCE_MS 25 // janela de estabilizacao do contato (borda)
#define LED_PIN 16     // LED de status externo (3mm + resistor 220R) -> GND
#define BLINK_MS 30    // piscada curta de feedback a cada pisada

// ---- 2 botoes * vira pagina ----
// GP2 = volta (PgUp) * GP3 = avanca (PgDn / rola pra baixo). Cada chave liga
// um terminal no GP indicado e o outro no GND (usa o pull-up interno).
#if NUM_BUTTONS == 2
const int BUTTON_PINS[] = {2, 3};
const uint8_t KEYS[] = {KEY_PAGE_UP, KEY_PAGE_DOWN};
const char *NAMES[] = {"PgUp", "PgDn"};
#endif

// ---- 4 botoes * vira pagina + muda musica ----
#if NUM_BUTTONS == 4
const int BUTTON_PINS[] = {2, 3, 4, 5};
const uint8_t KEYS[] = {KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_LEFT_ARROW,
                        KEY_RIGHT_ARROW};
const char *NAMES[] = {"PgUp", "PgDn", "Left", "Right"};
#endif

// Estado de debounce por borda: dispara UMA tecla na transicao solto->apertado.
// Segurar NAO repete (page-turner vira 1 pagina por toque).
struct ButtonState {
  bool stable;           // ultimo estado estavel (true = apertado)
  bool lastReading;      // ultima leitura crua
  unsigned long tChange; // quando a leitura crua mudou pela ultima vez
};
ButtonState buttons[NUM_BUTTONS];

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    // INPUT_PULLUP: solto = HIGH, apertado = LOW. Guardamos "apertado" como
    // true, entao invertemos a leitura crua no scan.
    buttons[i] = {false, false, 0};
  }

  Keyboard.begin(); // teclado USB HID

  // 3 piscadas = boot ok; depois fica aceso (alimentado e pronto).
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(120);
    digitalWrite(LED_PIN, LOW);
    delay(120);
  }
  digitalWrite(LED_PIN, HIGH);

  Serial.print("PlayBook Pedal (Pico) v1.0 — ");
  Serial.print(NUM_BUTTONS);
  Serial.println(" botoes, USB HID pronto.");
}

void loop() {
  scanButtons();
  delay(5);
}

void scanButtons() {
  const unsigned long now = millis();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    // apertado = LOW no pino (pull-up) -> inverte pra logica "pressed"
    const bool pressed = (digitalRead(BUTTON_PINS[i]) == LOW);

    // reinicia o cronometro de estabilizacao a cada mudanca de leitura
    if (pressed != buttons[i].lastReading) {
      buttons[i].lastReading = pressed;
      buttons[i].tChange = now;
    }

    // leitura estavel por tempo suficiente e diferente do estado atual
    if ((now - buttons[i].tChange) > DEBOUNCE_MS &&
        pressed != buttons[i].stable) {
      buttons[i].stable = pressed;

      // dispara so na descida (solto->apertado); soltar nao manda nada
      if (pressed) {
        Keyboard.write(KEYS[i]); // press+release: 1 tecla por toque

        Serial.print("Botao ");
        Serial.print(i + 1);
        Serial.print(" (");
        Serial.print(NAMES[i]);
        Serial.println(") enviado.");

        // piscada curta de feedback (apaga e reacende)
        digitalWrite(LED_PIN, LOW);
        delay(BLINK_MS);
        digitalWrite(LED_PIN, HIGH);
      }
    }
  }
}
