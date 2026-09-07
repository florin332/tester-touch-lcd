# Hardware Map — Waveshare RP2350 Touch LCD 2.8"

## MCU

[x] RP2350

## LCD — ST7789T3

[x] SCLK     — GP10
[x] MOSI     — GP11
[x] CS       — GP13
[x] DC       — GP14
[x] RST      — GP15
[x] BL       — GP16
[x] MISO     — not used

[x] SPI1
[x] 240 × 320
[x] Rotation 0
[x] 80 MHz
[x] CPOL 1
[x] CPHA 1
[x] MSB first

## Touch — CST328

[x] SDA      — GP6
[x] SCL      — GP7
[x] RST      — GP17
[x] INT      — GP18

[x] I2C1
[x] Address 0x1A
[x] 400 kHz
[x] 240 × 320
[x] Rotation 0

## MicroSD

[ ] CS       — GP8
[ ] MOSI     — GP11
[ ] MISO     — GP12
[ ] SCLK     — GP10

## IMU — QMI8658

[ ] SDA      — GP6
[ ] SCL      — GP7
[ ] INT1     — GP23
[ ] INT2     — GP24

## UART1

[ ] TX       — GP4
[ ] RX       — GP5



The IMU is present on the hardware but is **NOT USED by the
current LCD tester**.
Do not implement IMU functionality in this branch.

Any proposed change to a CONFIRMED pin requires explicit
verification before implementation.
