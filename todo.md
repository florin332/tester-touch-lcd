# TODO — Waveshare RP2350 Touch LCD 2.8"

Branch: `wv-2350-touch2.8`

## Obiectiv

Portarea testerului funcțional din `tester-touch-lcd/main`
pe hardware-ul Waveshare RP2350 Touch LCD 2.8".

### Surse de adevăr

  - [`tester-touch-lcd/main`](https://github.com/florin332/tester-touch-lcd)
  - sursa de adevăr pentru FUNCȚIONALITATEA testerului
  - meniuri și UI
  - secvența testelor
  - calibrare
  - EEPROM
  - comportamentul butoanelor
  - afișarea rezultatelor

  - [`wv_2350_lcd`](https://github.com/florin332/wv_2350_lcd)
  - sursa de adevăr pentru HARDWARE-ul Waveshare
  - ST7789T3
  - SPI1
  - inițializarea LCD
  - parametrii SPI
  - byte order
  - CST328
  - I2C
  - BSP touch

Nu se portează aplicația/demo din `wv_2350_lcd`.
Se portează implementarea hardware funcțională în aplicația testerului.

---

# 1. Analiză inițială

  - [ ] Analizează complet `tester-touch-lcd/main/src/main.cpp`.
  - [ ] Identifică toate funcțiile dependente de:
  - [ ] ILI9341
  - [ ] XPT2046
  - [ ] Marble Pico
  - [ ] pini specifici Marble
  - [ ] Analizează [Analizează `wv_2350_lcd/src/main.cpp](https://github.com/florin332/wv_2350_lcd).
  - [ ] Analizează implementarea BSP:
  - [ ] `bsp_i2c.*`
  - [ ] `bsp_cst328.*`
  - [ ] orice fișier necesar pentru LCD/ST7789T3
  - [ ] Identifică exact ce trebuie preluat din `wv_2350_lcd`
  pentru ca LCD-ul și touch-ul să funcționeze.

**Nu modifica încă codul în această etapă.**

---

# 2. LCD — ST7789T3

Portarea LCD-ului trebuie făcută pe baza implementării
care este deja funcțională în `wv_2350_lcd`.

## Pinout

- [ ] CS = GP13
- [ ] DC = GP14
- [ ] RST = GP15
- [ ] BL = GP16
- [ ] MOSI = GP11
- [ ] MISO = GP12
- [ ] SCLK = GP10
- [ ] SPI = SPI1

## Parametri confirmați

- [ ] SPI1
- [ ] 80 MHz
- [ ] 8 bit
- [ ] CPOL = 1
- [ ] CPHA = 1
- [ ] MSB first
- [ ] display = 240 × 320
- [ ] rotation = 0
- [ ] portrait

- [ ] Preia secvența de inițializare ST7789T3
  din `wv_2350_lcd`.
- [ ] Păstrează reset-ul hardware al LCD-ului.
- [ ] Păstrează controlul backlight-ului pe GP16.
- [ ] Păstrează byte order-ul din implementarea funcțională.
- [ ] NU modifica byte order-ul pe baza comentariilor din cod;
      implementarea existentă este referința funcțională.

---

# 3. Integrarea LCD cu testerul

Obiectivul este să păstrăm cât mai mult din codul testerului original.

- [ ] Păstrează logica UI existentă.
- [ ] Păstrează funcțiile de desen existente.
- [ ] Înlocuiește numai stratul necesar pentru ca acestea
      să lucreze cu ST7789T3.
- [ ] Evită rescrierea meniurilor și a logicii testerului.
- [ ] Nu introduce un HAL generic doar pentru această portare.
- [ ] Nu face refactorizare structurală dacă nu este necesară.

Dacă este necesar un wrapper/adaptor pentru API-ul grafic existent,
acesta trebuie să fie local branch-ului și cât mai simplu posibil.

---

# 4. Touch — CST328

Înlocuiește XPT2046 cu implementarea deja funcțională
din `wv_2350_lcd`.

## Pinout

- [ ] SDA = GP6
- [ ] SCL = GP7
- [ ] RST = GP17
- [ ] INT = GP18
- [ ] Interfață = I2C

## Implementare

Folosește API-ul BSP verificat:

- [ ] `bsp_i2c_init()`
- [ ] `bsp_cst328_init(...)`
- [ ] `bsp_cst328_read()`
- [ ] `bsp_cst328_get_touch_data(...)`

Datele de touch trebuie preluate din:

- [ ] `points`
- [ ] `coords[0].x`
- [ ] `coords[0].y`
- [ ] `coords[0].pressure`

- [ ] Elimină dependența de XPT2046.
- [ ] Elimină CS/MISO/IRQ specifice XPT2046.
- [ ] Nu trata CST328 ca touchscreen SPI.
- [ ] Nu folosi GP16 pentru touch reset.

---

# 5. Calibrare

Calibrarea trebuie păstrată funcțional ca în testerul original.

- [ ] Păstrează procedura existentă de calibrare.
- [ ] Păstrează salvarea în EEPROM.
- [ ] Păstrează încărcarea calibrației la boot.
- [ ] Adaptează numai sursa coordonatelor raw pentru CST328.
- [ ] Verifică orientarea X/Y.
- [ ] Verifică eventualele inversări X/Y.
- [ ] Verifică limitele 0...239 / 0...319.
- [ ] Verifică atingerea celor patru colțuri.
- [ ] Verifică repetabilitatea coordonatelor.

Nu modifica algoritmul de calibrare decât dacă hardware-ul
CST328 o impune în mod demonstrabil.

---

# 6. Buton recalibrare

- [ ] Identifică pinul disponibil ales pentru recalibrare.
- [ ] Nu folosi pini deja rezervați LCD/touch.
- [ ] Păstrează comportamentul existent al butonului.
- [ ] Testează apăsare scurtă/lungă conform comportamentului
      existent în tester.

Orice alegere de GPIO care nu este deja confirmată trebuie
semnalată înainte de implementare.

---

# 7. Funcționalități tester

După ce LCD + touch funcționează:

- [ ] PAGE_INFO
- [ ] PAGE_DESEN
- [ ] PAGE_RGB
- [ ] PAGE_RAW
- [ ] PAGE_ORIENT_X
- [ ] PAGE_ORIENT_Y
- [ ] PAGE_CALIB

Pentru fiecare:

- [ ] afișare corectă
- [ ] touch funcțional
- [ ] navigare corectă
- [ ] coordonate corecte
- [ ] fără artefacte LCD
- [ ] fără resetări

---

# 8. Test LCD

Verifică implementarea hardware înainte de testarea
întregii aplicații.

- [ ] RED
- [ ] GREEN
- [ ] BLUE
- [ ] WHITE
- [ ] BLACK

- [ ] 2 cicluri
- [ ] 3 secunde / culoare

Rezultatul trebuie să fie comparabil cu testul funcțional
din `wv_2350_lcd`.

---

# 9. Test Touch

- [ ] Detectare touch
- [ ] coordonate X
- [ ] coordonate Y
- [ ] pressure
- [ ] release
- [ ] touch repetat
- [ ] touch în toate cele patru colțuri
- [ ] touch în centrul ecranului
- [ ] desen continuu / tracking

Verifică explicit că valorile nu rămân blocate după release.

---

# 10. Build

- [ ] `pio run`
- [ ] Rezolvă erorile de compilare.
- [ ] Nu introduce biblioteci inutile.
- [ ] Verifică toate include-urile.
- [ ] Verifică dependențele PlatformIO.
- [ ] Verifică faptul că build-ul este pentru RP2350.

Agentul NU execută build-ul hardware final ca validare fizică.

Dacă este necesar un build pentru verificarea compilării,
agentul îl poate solicita utilizatorului.

---

# 11. Test hardware

După build:

- [ ] Flash pe Waveshare RP2350 Touch LCD 2.8"
- [ ] Verificare boot
- [ ] Verificare LCD
- [ ] Verificare touch
- [ ] Verificare calibrare
- [ ] Verificare toate paginile testerului
- [ ] Verificare RGB
- [ ] Verificare RAW
- [ ] Verificare orientări
- [ ] Verificare EEPROM

Problemele constatate la testarea fizică se notează aici înainte
de modificarea următoare.

---

# 12. Reguli de lucru

- [ ] Lucrează EXCLUSIV pe `wv-2350-touch2.8`.
- [ ] NU modifica `main`.
- [ ] NU face merge în `main`.
- [ ] NU copia aplicația demo din `wv_2350_lcd` peste tester.
- [ ] NU rescrie testerul de la zero.
- [ ] NU modifica funcționalitatea testerului fără motiv.
- [ ] NU modifica pinout-ul confirmat.
- [ ] NU introduce HAL generic.
- [ ] NU introduce suport SD în această etapă.
- [ ] NU introduce suport IMU în această etapă.
- [ ] NU introduce alte profile LCD în această etapă.

---

# 13. Criteriu de finalizare

Branch-ul `wv-2350-touch2.8` este considerat finalizat
când:

- [ ] testerul original este funcțional pe Waveshare RP2350;
- [ ] ST7789T3 funcționează stabil;
- [ ] CST328 funcționează stabil;
- [ ] calibrarea funcționează;
- [ ] EEPROM funcționează;
- [ ] toate paginile testerului funcționează;
- [ ] testul RGB funcționează;
- [ ] touch-ul este corect pe întreg ecranul;
- [ ] nu există dependențe de XPT2046;
- [ ] nu există dependențe de hardware-ul Marble;
- [ ] `main` a rămas neschimbat.

## Surse de referință

Tester funcțional:
`tester-touch-lcd`

Hardware Waveshare funcțional:
`wv_2350_lcd`