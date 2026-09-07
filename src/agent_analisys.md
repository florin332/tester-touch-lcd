# Analiză cod — tester touch lcd / service_box

## Context

Fișier analizat: `src/main.cpp`.  
Fișier hardware disponibil: `hardware_map.md`.

**Hardware real (Waveshare RP2350 Touch LCD):**
- Microcontroler: RP2350
- Display: **ST7789T3**, 320×240, SPI1
  - CS = GP13, DC = GP14, RST = GP15, BL = GP16
  - MOSI = GP11, MISO = GP12, SCLK = GP10
- Touch: **CST328 capacitiv**, I2C0
  - SDA = GP6, SCL = GP7, RST = GP17, INT = GP18
- IMU: QMI8658 tot pe I2C0 (SDA=GP6, SCL=GP7)
- MicroSD: SPI1, CS = GP8
- UART1: TX=GP4, RX=GP5

Observație: analiza este făcută pe varianta sumarizată a `main.cpp` (unele blocuri apar cu `…`). Concluziile se referă la structura și la maparea hardware vizibile în cod.

---

## 1. Constatare capitală: codul nu se potrivește hardware-ului real

`main.cpp` pare scris pentru un alt kit (ILI9341 + XPT2046 pe SPI), nu pentru Waveshare RP2350 Touch LCD.

| Componentă reală | Ce folosește codul | Problemă |
|---|---|---|
| **Display ST7789T3 320×240 SPI1** | `Adafruit_ILI9341` | Controller greșit. Dimensiuni 240×319 în loc de 320×240. |
| **Touch CST328 I2C0** | `XPT2046_Touchscreen` | Controller și bus complet greșit. Codul face SPI, nu I2C. |
| **TFT_DC = GP14** | `#define TFT_DC 6` | GP6 este SDA (I2C0). Se produce conflict cu touch/IMU. |
| **TFT_RST = GP15** | `#define TFT_RST 14` | GP14 este DC. Se inversează cu DC și creează conflict. |
| **TFT_LED = GP16** | `#define TFT_LED 4` | Backlight nu va funcționa pe pinul corect. |
| **Touch INT = GP18** | `#define TOUCH_IRQ 8` | GP8 este CS pentru MicroSD. Configurat ca INPUT_PULLUP poate bloca cardul SD. |
| **Touch RST = GP17** | nedefinit | CST328 nu va putea fi resetat din cod. |
| **UART1 RX = GP5** | `#define RECALIB_BUTTON 5` | GP5 este UART1 RX; butonul poate interfera cu serial. |
| **SD card CS = GP8** | `#define TOUCH_CS 9` | Pin nefolosit corespunzător; dacă se adaugă SD-ul, va trebui gestionat separat. |

### 1.1 Consecințe imediate

1. **Codul nu compilează** așa cum este:
   - include `#include <Adafruit_st7735.h>` nu corespunde obiectului `Adafruit_ILI9341`;
   - `#include <circuitstate/CSE_CST328.h` este scris greșit (lipsă `>` și nu este folosit);
   - `XPT2046_Touchscreen` lipsește ca include, dar nici nu este biblioteca corectă.
2. **Dacă s-ar forța compilarea cu ILI9341**, ecranul ST7789T3 ar afișa culori/offset greșite sau nimic.
3. **Touch-ul nu va răspunde** pentru că este pe I2C, nu pe SPI, și folosește alt protocol (CST328 vs XPT2046).
4. **Pinii GP6, GP8, GP14, GP15 sunt folosiți greșit**, ceea ce poate deteriora comunicația cu touch/IMU/SD.

---

## 2. Alte constatări din cod

### 2.1 Include-uri și obiecte nepotrivite

- `#include <Adafruit_st7735.h>` este pentru ST7735 (128×160); nu are legătură cu obiectul `Adafruit_ILI9341` declarat.
- `#include <circuitstate/CSE_CST328.h` este cel mai apropiat de hardware, dar:
  - este scris cu newline înainte de `>`;
  - clasa nu este instanțiată;
  - nu este folosit `Wire` / I2C.
- Lipsește `#include <XPT2046_Touchscreen.h>`, dar și așa XPT2046 nu este ceea ce trebuie.

### 2.2 Gestiune manuală a liniilor CS

Funcția `getIsolatedTouchPoint()` și funcțiile de randare fac `digitalWrite(TFT_CS, LOW/HIGH)` și `digitalWrite(TOUCH_CS, LOW/HIGH)` manual.

- Pentru **SPI1**, CS-ul display-ului ar trebui gestionat de biblioteca ST7789.
- Pentru **I2C**, conceptul de CS nu există; `TOUCH_CS` este irrelevant.
- Dacă se va adăuga **MicroSD pe SPI1** (CS=GP8), va fi nevoie de o logică clară de arbiter SPI, nu de comutare manuală în funcții de touch.

### 2.3 Inițializare SPI inutilă

```cpp
SPI1.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
SPI1.endTransaction();
```

Această pereche nu transmite nimic și poate fi eliminată. Frecvența de 1 MHz este foarte mică pentru un display modern (ST7789 suportă zeci de MHz).

### 2.4 Logică de calibrare — concept OK, implementare legată de XPT2046

- `collectCalibPoint()` citește `TS_Point p = touch.getPoint()` — API XPT2046.
- Pentru CST328, API-ul este diferit (de regulă `getTouched()` + `getPoint(int16_t &x, int16_t &y)`).
- Detectarea orientării (`CAL_ST`, `CAL_DR`, `CAL_DJ`, `CAL_SJ`) și extrapolarea colțurilor sunt gândite pentru un touch rezistiv 4096×4096. CST328 returnează deja coordonate logice (de obicei 0…319 / 0…239 sau similar), deci calibrarea poate fi mult simplificată.

### 2.5 Bucla principală

- `pixelX` / `pixelY` nu sunt resetate când `pressed == false`; risc de „apăsare fantomă”.
- `armRecalib` este setat `true`, dar nu este verificat ulterior — cod mort.
- Pagina RAW folosește mapare hardcodată `3700, 370, 300, 3700` în loc de structura `calib`.
- Butoanele tactile sunt verificate uneori cu `lastMapX/Y`, alteori cu `pixelX/Y` — inconsistență.

### 2.6 Stări de calibrare neclare

Ordinea `1 → 4 → 2 → 3` este neobișnuită și nedocumentată. Fără comentarii, este greu de întreținut.

---

## 3. Riscuri funcționale

1. **Imposibilitatea compilării** pe hardwareul țintă.
2. **Lipsa comunicației cu touch-ul** (bus/protocol greșit).
3. **Ecran nefuncțional sau cu artefacte** (controller și pini greșiți).
4. **Conflict electric / logic pe GP6 și GP14** (I2C SDA vs TFT_DC).
5. **Blocarea MicroSD-ului** prin configurarea greșită a GP8.
6. **Buton de recalibrare pe UART1 RX** — poate corupe date seriale sau nu va funcționa dacă UART1 este activ.
7. **Date de calibrare incorecte** pentru CST328 dacă se păstrează logica XPT2046.

---

## 4. Propuneri de refactorizare (fără cod încă)

### 4.1 Alegerea bibliotecilor corecte

- Display: `Adafruit_ST7789` (sau o bibliotecă mai performantă precum `LovyanGFX` / `TFT_eSPI`) pentru ST7789T3 320×240.
- Touch: `CSE_CST328` (din pachetul `circuitstate`) pentru CST328 pe I2C0.
- I2C: `Wire` (I2C0 pe GP6/GP7).

### 4.2 Corectarea hărții de pini

Actualizați `#define`-urile conform `hardware_map.md`:

| Semnal | Pin corect |
|---|---|
| TFT_CS | 13 |
| TFT_DC | 14 |
| TFT_RST | 15 |
| TFT_LED | 16 |
| TFT_MOSI | 11 |
| TFT_MISO | 12 |
| TFT_SCK | 10 |
| TOUCH_SDA | 6 |
| TOUCH_SCL | 7 |
| TOUCH_RST | 17 |
| TOUCH_INT | 18 |
| SD_CS | 8 |
| RECALIB_BUTTON | alt pin liber, nu GP5 |

### 4.3 Refacerea inițializării

- Ștergeți inițializarea SPI inutilă.
- Inițializați `SPI1` pentru display (și eventual SD).
- Inițializați `Wire` pentru touch și IMU.
- Configurați pinii de reset și interrupt ai CST328.
- Dacă se folosește MicroSD, asigurați-vă că `TFT_CS` este high înainte de accesul SD.

### 4.4 Refacerea driverului de touch

- Înlocuiți `getIsolatedTouchPoint()` și `TS_Point` cu API-ul CST328.
- Adaptați `processRawTouch`, `mapTouch`, `collectCalibPoint` la coordonatele returnate de CST328.
- Evaluați dacă mai este necesară calibrarea complexă în 4 puncte; pentru multe panouri CST328, o simplă mapare liniară și inversare/swap sunt suficiente.

### 4.5 Curățarea buclei principale

- Resetați `pixelX` / `pixelY` la `-1` când nu este atingere.
- Eliminați sau folosiți `armRecalib`.
- Folosiți `pixelX/Y` consistent pentru toate butoanele.
- Faceți pagina RAW să reflecte valorile din `calib`.

### 4.6 Separarea codului

- `hardware_map.h` — toți pinii și constantele hardware.
- `display_manager.cpp/.h` — inițializare și primitive de desenare.
- `touch_manager.cpp/.h` — inițializare și citire touch CST328.
- `calibration.cpp/.h` — logica de calibrare.
- `pages.cpp/.h` — randarea celor 6 pagini.
- `main.cpp` — doar `setup()` / `loop()`.

### 4.7 Robustete

- Debouncing pentru butonul hardware.
- Timeout și retry la citirea touch.
- Verificare returnată `saveCalib()`.
- Gestionare corectă a întreruperii I2C dacă IMU este adăugat ulterior pe același bus.

---

## 5. Întrebări care trebuie clarificate înainte de generarea codului

1. Este confirmat kitul **Waveshare RP2350 Touch LCD** cu ST7789T3 + CST328?
2. Se dorește să se folosească biblioteca **Adafruit_ST7789** sau altă bibliotecă (ex. `LovyanGFX`)?
3. Este necesar suportul pentru **MicroSD** pe SPI1 sau rămâne neatins?
4. Este necesar suportul pentru **IMU QMI8658**? Dacă da, pe același I2C0 trebuie gestionată coexistența.
5. Ce pin fizic ar trebui folosit pentru **RECALIB_BUTTON**?
6. Ce rotație a display-ului se dorește implicit? (320×240 peisaj vs portret influențează touch mapping.)

Rezolvarea celor 6 puncte elimină majoritatea ambiguităților și permite generarea unui cod curat, funcțional.