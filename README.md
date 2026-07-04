# PlayBook Pedal · Firmware

Firmware open-source do pedal Bluetooth do [PlayBook](https://playbook.mus.br) —
app de cifras e setlists pra músicos.

Roda em **ESP32-WROOM-32** e emula um teclado HID via Bluetooth. Funciona com
qualquer app que aceite pedal HID (não só o PlayBook).

## Variantes

| Variante | Botões | Teclas enviadas | Ação padrão no PlayBook |
|---|---|---|---|
| **2-botões** | 2 | `PgUp` · `PgDn` | Vira página anterior / próxima |
| **4-botões** | 4 | `PgUp` · `PgDn` · `←` · `→` | Vira página + música anterior / próxima |

Mude `NUM_BUTTONS` em [`playbook_pedal/playbook_pedal.ino`](playbook_pedal/playbook_pedal.ino)
pra selecionar sua variante.

## Compilar e flashar

### 1. Instalar Arduino IDE
Baixa em [arduino.cc/en/software](https://www.arduino.cc/en/software) (v2.x).

### 2. Adicionar suporte ESP32
Em `Arquivo → Preferências → URLs adicionais do gerenciador de placas`, cola:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

Depois `Ferramentas → Placa → Boards Manager` → busca "esp32" → Install.

### 3. Instalar biblioteca ESP32-BLE-Keyboard
`Sketch → Include Library → Manage Libraries` → busca `ESP32-BLE-Keyboard` (T-vK) → Install.

Ou clone manualmente: https://github.com/T-vK/ESP32-BLE-Keyboard

### 4. Selecionar board
`Ferramentas → Placa → ESP32 Arduino → ESP32 Dev Module` · Porta → COM correta.

### 5. Compilar + Upload
Abre `playbook_pedal/playbook_pedal.ino`, ajusta `NUM_BUTTONS` se necessário,
clica **Upload** (seta pra direita).

## Bill of materials

Ver [tutorial de montagem completo](https://playbook.mus.br/pedal) com fotos,
lista de peças e links de fornecedores (Mercado Livre + Aliexpress).

Curto:
- ESP32 DOIT DevKit (WROOM-32, 30 pinos)
- Enclosure metal 2 ou 4 switches (FS2CX ou equivalente)
- Chaves SPST momentâneas grade pedal
- Suporte 2×AAA + interruptor
- LED 3mm + resistor 220Ω (opcional, indicador de status)

## Emparelhar com o PlayBook

1. Ligue o pedal — LED verde pisca (procurando conexão)
2. No celular: `Ajustes → Bluetooth`
3. Toque em `PlayBook Pedal` na lista
4. LED verde fica sólido = conectado
5. Abra o app PlayBook → `Ajustes → Pedal → Learn Mode`
6. Pise em cada botão quando o app pedir — mapeamento automático

## Comprar pronto

Se você prefere não montar, vendemos versão pronta no Mercado Livre:
- 2-botões: R$ 259
- 4-botões: R$ 349

Anúncios linkados em [playbook.mus.br/pedal](https://playbook.mus.br/pedal).

## Contribuir

Issues e PRs são bem-vindos. Pra debugging, use o serial monitor em 115200 baud
— o firmware imprime status de conexão e cada tecla enviada.

## Licença

MIT — veja [LICENSE](LICENSE). Sinta-se livre pra usar como base pro seu próprio
pedal ou variante.
