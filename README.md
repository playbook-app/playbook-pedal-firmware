# PlayBook Pedal · Firmware

Firmware Arduino (ESP32) do pedal Bluetooth do
[PlayBook](https://github.com/playbook-app) — página-vira-página pra
músicos. Código aberto, MIT.

## Hardware

- ESP32 DOIT DevKit (ESP32-WROOM-32)
- 2 ou 4 chaves SPST momentâneas (footswitch)
- Alimentação 2×AAA (deep sleep entre pisadas)
- Emula **teclado Bluetooth (BLE HID)** — funciona com qualquer app,
  inclusive o PlayBook

## Mapa de teclas

| Variante | Botões | Teclas |
|---|---|---|
| 2-botões | vira página | `PgUp` `PgDn` |
| 4-botões | página + música | `PgUp` `PgDn` `←` `→` |

## Status

🚧 Firmware em desenvolvimento — o `.ino` chega aqui junto com o tutorial
de montagem completo (lista de materiais, fotos passo a passo, flash pelo
Arduino IDE). Enquanto isso, qualquer pedal BLE HID do mercado já funciona
com o app.

## Licença

MIT
