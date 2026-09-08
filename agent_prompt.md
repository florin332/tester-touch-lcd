# Prompt pentru agent — Extragere modul LCD + Touch + Calibrare

## Context

Proiectul sursă este un **tester LCD + touchscreen** pentru RP2040 (Marble Pico, core earlephilhower, PlatformIO). Din acest proiect trebuie extras **doar modulul de afișare + touch + calibrare**, fără funcționalitatea de tester (paginile INFO/DESEN/RGB/RAW). Proiectul țintă va avea propria aplicație care folosește display-ul și touchscreen-ul calibrat.

**Hardware țintă (identic cu sursa):**
- MCU: RP2040 (GroundStudio Marble Pico)
- Display: ILI9341 240×320 pe SPI1
- Touch: XPT2046 rezistiv, pe aceeași magistrală SPI1
- Buton recalibrare: **GP29** (în sursă este GP5 — **de modificat**), INPUT_PULLUP, activ LOW

## Fișiere de extras din proiectul sursă

1. **`lib/TesterHAL/` complet** (LcdHal.h, LcdHal.cpp, LcdConfig.h, LcdModel1_BlueTab.cpp) — HAL-ul display/touch. Se copiază integral, neschimbat. Celelalte modele (2, 3) pot fi omise sau păstrate — nu afectează funcționarea.
2. **Din `src/main.cpp`, doar secțiunile marcate `[PRELUARE]`** (marcajele sunt comentarii în cod):
   - Constantele de culori `HAL_COLOR_*` și macro-urile `TFT_W`/`TFT_H`
   - `RECALIB_BUTTON` → **redefinit la 29** (GP29) în proiectul țintă
   - `Z_TOUCH_MIN`, `Z_SAMPLE_MIN`, `CALIB_MAGIC`, `CALIB_VER`
   - Structura `CalibData` (packed) + variabila globală `calib`
   - Variabilele de stare: `calibPointIndex`, `calibX[4]`/`calibY[4]`, `awaitingRelease`/`releaseSince`, `CollectState` + `calibSamplesX/Y` + `sampleCount`, `calibIdleSince` + `CALIB_POINT_TIMEOUT_MS`, `swipeStartX/Y[2]`/`swipeEndX/Y[2]` + `swipeCount`
   - Funcțiile: `safeMap` (cu protecția la diviziune prin zero), `loadCalib`, `saveCalib`, `forceReboot`, `processRawTouch`, `mapTouch`, `drawCrosshair`, `renderPageOrientX`, `renderPageOrientY`, `renderCalibPointScreen`
   - Din `setup()`: `pinMode(RECALIB_BUTTON, INPUT_PULLUP)`, `loadCalib()`, `halDisplayInit()`, `halTouchInit()`, **flush-ul touch de la pornire (5 citiri + delay 20 ms — CRITIC, nu se omite)**, și logica de reluare a etapei de calibrare după reboot (stage 1/4/2)
   - Din `loop()`: citirea `halTouchRead()`, poarta non-blocantă `awaitingRelease`, maparea `mapTouch()` la `stage == 3`, mașina de stări a butonului de recalibrare (2 s hold → ștergere calibrare → reboot) și **întregul bloc al etapelor de calibrare (stage 1, 4, 2)**

## Ce NU se preia (marcate `[NU PRELUA]`)

- `drawButton`, `renderPageInfo`, `renderPageDesen`, `renderPageRgb`, `renderPageRaw`
- Variabilele `lastRawX/Y/Z`, `lastMapX/Y`, `lastX/Y`, `lastValid` (specifice paginilor testerului)
- Paginile `PAGE_INFO`, `PAGE_DESEN`, `PAGE_RGB`, `PAGE_RAW` din enum-ul `Page`
- Blocul `if (calib.stage == 3) { ... pagini ... }` din loop — se înlocuiește cu logica aplicației țintă

## Dependențe (platformio.ini țintă)

```
lib_deps =
    adafruit/Adafruit GFX Library
    adafruit/Adafruit ILI9341
    https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
```

## Modificări obligatorii în proiectul țintă

1. `RECALIB_BUTTON` = **29** (GP29), `INPUT_PULLUP`.
2. Enum-ul `Page`: păstrează `PAGE_ORIENT_X`, `PAGE_ORIENT_Y`, `PAGE_CALIB` și adaugă pagina principală a aplicației țintă (ex. `PAGE_MAIN`).
3. În `setup()`, ramura `else` finală (stage 3, calibrat) randează pagina principală a aplicației țintă în loc de `renderPageInfo()`.
4. În `loop()`, la `stage == 3` rulează logica aplicației țintă, folosind `mapTouch(pt.x, pt.y, pixelX, pixelY)` pentru coordonate. Folosește poarta `awaitingRelease` pentru acțiuni de tip buton.
5. Mesajul grafic „ELIBERATI BUTONUL..." din secvența de recalibrare poate fi eliminat dacă UI-ul țintă nu permite suprapuneri — **dar logica non-blocantă cu 3 faze (repaus → cronometrare 2 s → așteaptă eliberarea) se păstrează**.

## Atenție — detalii critice

- **Flush-ul touch la pornire** (5 citiri + 20 ms) este obligatoriu; fără el apar atingeri fantomă care strică detecția etapei.
- **Izolarea CS** display/touch e gestionată de HAL (`halTouchRead`/`halDisplaySelect/Deselect`) — nu ocoli aceste funcții.
- Calibrarea trece prin **reboot-uri** între etape (stage salvat în EEPROM) — nu încerca s-o faci fără reboot.
- Pozițiile crosshair-urilor de calibrare (25/215 pe X, 85/300 pe Y) sunt legate de formula de extrapolare din finalul etapei 2 — dacă le schimbi, ajustează și coeficienții (25/190, 85/215, 20/215).
- Procedura de calibrare: etapa 1 = swipe orizontal stânga→dreapta (detectează swapXY/invX), etapa 4 = swipe vertical sus→jos (detectează invY), etapa 2 = atingerea a 4 puncte în colțuri (calculează xmin/xmax/ymin/ymax), stage 3 = funcționare normală.

## Rezultat așteptat

Un `main.cpp` în proiectul țintă care: pornește display-ul + touch-ul prin HAL, verifică calibrarea din EEPROM, execută procedura de calibrare la nevoie (inclusiv declanșată de GP29 ținut 2 s), apoi rulează aplicația țintă cu coordonate mapate corect.
