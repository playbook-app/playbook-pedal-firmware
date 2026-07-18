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

Manda **só teclas de teclado limpas**, uma por pisada — nada de teclas de
mídia (`MediaTrackNext` etc.). Bate com o mapa padrão do app PlayBook, então
funciona sem remapear.

**Protótipo atual (2 botões):** ESP32-WROOM-32 DevKit (DOIT), chaves em
`D4`/`D5` (comum no GND), alimentado por USB. `D4` = volta (`PgUp`), `D5` =
avança (`PgDn`). Pra inverter, troque a ordem em `KEYS[]`.

## Compilar

Requer a lib **ESP32-BLE-Keyboard** (T-vK) e o core **esp32** no Arduino IDE.

⚠️ **Core ESP32 3.x**: a lib 0.3.2 do T-vK foi feita pro core 2.x. No core
3.x ela não compila (a API `BLECharacteristic::setValue`/`BLEDevice::init`
passou a usar `String` no lugar de `std::string`). Dois caminhos:

- **Patch (2 linhas)** em `BleKeyboard.cpp` — converta com `.c_str()`:
  - `BLEDevice::init(String(deviceName.c_str()));`
  - `hid->manufacturer()->setValue(String(deviceManufacturer.c_str()));`
- **ou** instale o core **esp32 2.0.x** no Boards Manager (a lib compila
  direto).

> A lib patchada ainda **não** está vendorizada aqui — fica pra quando a
> placa definitiva estiver decidida.

Board no IDE: **DOIT ESP32 DEVKIT V1**.

## Status

✅ Firmware funcional (v1.1) — protótipo de 2 botões montado, gravado e
validado com o app. Pendente: tutorial de montagem (materiais, fotos, flash
passo a passo) e a definição da placa/enclosure definitivos.

## Licença

MIT
