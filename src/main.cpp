#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include <LcdConfig.h>   // HAL: selectare model + API comun

// Culori generice RGB565 (independente de controller)
#define HAL_COLOR_BLACK      0x0000
#define HAL_COLOR_WHITE      0xFFFF
#define HAL_COLOR_RED        0xF800
#define HAL_COLOR_GREEN      0x07E0
#define HAL_COLOR_BLUE       0x001F
#define HAL_COLOR_CYAN       0x07FF
#define HAL_COLOR_YELLOW     0xFFE0
#define HAL_COLOR_ORANGE     0xFD20
#define HAL_COLOR_MAROON     0x7800
#define HAL_COLOR_DARKGREEN  0x03E0
#define HAL_COLOR_DARKGREY   0x7BEF

// Dimensiunile display-ului activ (din HAL)
#define TFT_W (LCD.width)
#define TFT_H (LCD.height)

// ============================================================
// Prototipuri de funcții
// ============================================================
int safeMap(int v, int fl, int fh, int tl, int th);
void loadCalib();
bool saveCalib();
void forceReboot();
void processRawTouch(int32_t rx, int32_t ry, int32_t &cx, int32_t &cy, bool useConfig);
void mapTouch(int32_t rx, int32_t ry, int &ox, int &oy);
void drawButton(int x, int y, int w, int h, const char *l, uint16_t fg, uint16_t bg);
void drawCrosshair(int cx, int cy, uint16_t clr);
void renderPageInfo();
void renderPageDesen();
void renderPageRgb();
void renderPageRaw();
void renderPageOrientX();
void renderPageOrientY();
void renderCalibPointScreen(int idx);

// ============================================================
// Definiții hardware și constante
// ============================================================
// Pinii display/touch sunt acum in HAL (LcdModel*.cpp, structura LCD)

#define RECALIB_BUTTON 5

#define Z_TOUCH_MIN   (LCD.zTouchMin)
#define Z_SAMPLE_MIN  (LCD.zSampleMin)

// Magic-ul de calibrare este per model (din HAL): la schimbarea
// modelului de display se declanseaza automat recalibrarea
#define CALIB_MAGIC (LCD.calibMagic)
#define CALIB_VER   6

// ============================================================
// Structuri de date și variabile globale
// ============================================================
// Structură impachetată: layout determinist în EEPROM, fără padding
struct __attribute__((packed)) CalibData {
    uint32_t magic;
    uint16_t version;
    int32_t xmin, xmax, ymin, ymax;
    uint8_t swapXY, invX, invY, stage;
};

CalibData calib;

enum Page {
    PAGE_INFO,
    PAGE_DESEN,
    PAGE_RGB,
    PAGE_RAW,
    PAGE_ORIENT_X,
    PAGE_ORIENT_Y,
    PAGE_CALIB
};

Page currentPage = PAGE_INFO;

int calibPointIndex = 0;
int32_t calibX[4], calibY[4];  // Coordonatele celor 4 colțuri de calibrare

int32_t lastRawX = 0;
int32_t lastRawY = 0;
int32_t lastRawZ = 0;

int lastMapX = 0;
int lastMapY = 0;

int lastX = -1;
int lastY = -1;
bool lastValid = false;

// Poartă non-blocantă: ignoră touch-ul până la eliberarea completă
bool awaitingRelease = false;
unsigned long releaseSince = 0;

// Stare non-blocantă pentru colectarea eșantioanelor de calibrare
enum CollectState { COLLECT_IDLE, COLLECT_SAMPLING };
CollectState collectState = COLLECT_IDLE;
int32_t calibSamplesX[12], calibSamplesY[12];
int sampleCount = 0;

// Timeout pentru așteptarea atingerii unui punct de calibrare
unsigned long calibIdleSince = 0;
const unsigned long CALIB_POINT_TIMEOUT_MS = 10000;

// Eșantioane pentru detecția direcției de swipe (etapele de orientare):
// primele 2 și ultimele 2 eșantioane ale întregii mișcări
int32_t swipeStartX[2], swipeStartY[2];
int32_t swipeEndX[2], swipeEndY[2];
int swipeCount = 0;

// Instanta display este expusa de HAL ca referinta Adafruit_GFX &display

// ============================================================
// Funcții de bază pentru touch și mapare
// ============================================================
int safeMap(int v, int fl, int fh, int tl, int th) {
    if (fh == fl) {
        return tl;  // Protecție la diviziune cu zero (calibrare coruptă)
    }
    return tl + (v - fl) * (th - tl) / (fh - fl);
}

void loadCalib() {
    EEPROM.begin(64);
    EEPROM.get(0, calib);

    if (calib.magic != CALIB_MAGIC || calib.version != CALIB_VER) {
        calib.magic   = 0;
        calib.xmin    = LCD.defXmin;
        calib.xmax    = LCD.defXmax;
        calib.ymin    = LCD.defYmin;
        calib.ymax    = LCD.defYmax;
        calib.swapXY  = 0;
        calib.invX    = 0;
        calib.invY    = 0;
        calib.stage   = 3;
    }
}

bool saveCalib() {
    calib.magic   = CALIB_MAGIC;
    calib.version = CALIB_VER;
    EEPROM.put(0, calib);
    return EEPROM.commit();
}

void forceReboot() {
    delay(500);
    rp2040.reboot();
}

void processRawTouch(int32_t rx, int32_t ry, int32_t &cx, int32_t &cy, bool useConfig) {
    cx = rx;
    cy = ry;

    if (useConfig && calib.swapXY) {
        int32_t t = cx;
        cx = cy;
        cy = t;
    }

    if (useConfig && calib.invX) {
        cx = 4095 - cx;
    }

    if (useConfig && calib.invY) {
        cy = 4095 - cy;
    }
}

void mapTouch(int32_t rx, int32_t ry, int &ox, int &oy) {
    int32_t cx, cy;
    processRawTouch(rx, ry, cx, cy, true);

    ox = constrain(safeMap(cx, calib.xmin, calib.xmax, 0, TFT_W - 1), 0, TFT_W - 1);
    oy = constrain(safeMap(cy, calib.ymin, calib.ymax, 0, TFT_H - 1), 0, TFT_H - 1);
}

// ============================================================
// Funcții de desenare
// ============================================================
void drawButton(int x, int y, int w, int h, const char *l, uint16_t fg, uint16_t bg) {
    halDisplaySelect();

    display.fillRoundRect(x, y, w, h, 4, bg);
    display.drawRoundRect(x, y, w, h, 4, fg);

    display.setCursor(x + (w - (int16_t)strlen(l) * 12) / 2, y + (h - 14) / 2);
    display.setTextColor(fg);
    display.setTextSize(2);
    display.print(l);

    halDisplayDeselect();
}

void drawCrosshair(int cx, int cy, uint16_t clr) {
    halDisplaySelect();

    display.drawCircle(cx, cy, 10, clr);
    display.drawLine(cx - 16, cy, cx - 6, cy, clr);
    display.drawLine(cx + 6, cy, cx + 16, cy, clr);
    display.drawLine(cx, cy - 16, cx, cy - 6, clr);
    display.drawLine(cx, cy + 6, cx, cy + 16, clr);
    display.fillCircle(cx, cy, 2, clr);

    halDisplayDeselect();
}

// ============================================================
// Funcții de randare a paginilor
// ============================================================
void renderPageInfo() {
    halDisplaySelect();

    display.fillScreen(HAL_COLOR_BLACK);

    display.setCursor(10, 12);
    display.setTextColor(HAL_COLOR_CYAN);
    display.setTextSize(2);
    display.println("TESTER LCD");

    display.setTextSize(1);
    display.setTextColor(HAL_COLOR_YELLOW);

    display.setCursor(10, 32);
    display.print(LCD.name);

    display.setCursor(10, 48);
    display.print("Xmin=");
    display.print(calib.xmin);
    display.print(" Xmax=");
    display.print(calib.xmax);

    display.setCursor(10, 60);
    display.print("Ymin=");
    display.print(calib.ymin);
    display.print(" Ymax=");
    display.print(calib.ymax);

    display.setCursor(10, 72);
    display.print("SwapXY=");
    display.print(calib.swapXY);
    display.print(" InvX=");
    display.print(calib.invX);
    display.print(" InvY=");
    display.print(calib.invY);

    display.setCursor(10, 84);
    display.setTextColor(HAL_COLOR_GREEN);
    display.print("Status Touch: CALIBRAT COMPLET");

    display.drawFastHLine(0, 280, TFT_W, HAL_COLOR_DARKGREY);

    halDisplayDeselect();

    drawButton(10, 100, 105, 45, "DESEN", HAL_COLOR_WHITE, 0x001F);
    drawButton(125, 100, 105, 45, "RGB", HAL_COLOR_WHITE, 0x7800);
    drawButton(10, 160, 105, 45, "RAW", HAL_COLOR_WHITE, 0x03E0);
    drawButton(125, 160, 105, 45, "Calibr.", HAL_COLOR_RED, HAL_COLOR_YELLOW);
}

void renderPageDesen() {
    halDisplaySelect();

    display.fillScreen(HAL_COLOR_BLACK);
    display.drawFastHLine(0, 55, TFT_W, HAL_COLOR_DARKGREY);
    display.drawFastHLine(0, 292, TFT_W, HAL_COLOR_DARKGREY);

    halDisplayDeselect();

    drawButton(10, 10, 90, 35, "INAPOI", HAL_COLOR_WHITE, 0x000F);
    drawButton(140, 10, 90, 35, "STERGE", HAL_COLOR_BLACK, HAL_COLOR_RED);

    lastValid = false;
}

void renderPageRgb() {
    halDisplaySelect();

    display.fillRect(0, 0, TFT_W, 107, HAL_COLOR_RED);
    display.fillRect(0, 107, TFT_W, 106, HAL_COLOR_GREEN);
    display.fillRect(0, 213, TFT_W, 107, HAL_COLOR_BLUE);

    display.setTextSize(2);
    display.setTextColor(HAL_COLOR_WHITE);

    display.setCursor(95, 45);
    display.print("RED");
    display.setCursor(85, 151);
    display.print("GREEN");
    display.setCursor(90, 257);
    display.print("BLUE");

    display.fillRect(10, 10, 80, 30, HAL_COLOR_BLACK);
    display.drawRect(10, 10, 80, 30, HAL_COLOR_WHITE);

    display.setCursor(18, 17);
    display.print("INAPOI");

    halDisplayDeselect();
}

void renderPageRaw() {
    halDisplaySelect();

    display.fillScreen(HAL_COLOR_BLACK);
    display.drawFastHLine(0, 215, TFT_W, HAL_COLOR_DARKGREY);

    display.setTextSize(1);
    display.setTextColor(HAL_COLOR_WHITE);

    display.drawCircle(26, 235, 4, HAL_COLOR_CYAN);
    display.setCursor(35, 232);
    display.print("T1(26,235)");

    display.drawCircle(150, 235, 4, HAL_COLOR_CYAN);
    display.setCursor(160, 232);
    display.print("T2(150,235)");

    display.drawCircle(148, 273, 4, HAL_COLOR_CYAN);
    display.setCursor(158, 270);
    display.print("T3(148,273)");

    display.drawCircle(27, 272, 4, HAL_COLOR_CYAN);
    display.setCursor(37, 269);
    display.print("T4(27,272)");

    display.setCursor(90, 48);
    display.setTextColor(HAL_COLOR_YELLOW);
    display.print("LIVE");

    display.setCursor(160, 48);
    display.setTextColor(HAL_COLOR_ORANGE);
    display.print("LAST");

    halDisplayDeselect();

    drawButton(10, 10, 220, 32, "INAPOI", HAL_COLOR_WHITE, 0x000F);
}

void renderPageOrientX() {
    halDisplaySelect();

    display.fillScreen(HAL_COLOR_BLACK);

    display.setCursor(10, 12);
    display.setTextColor(HAL_COLOR_CYAN);
    display.setTextSize(2);
    display.println("LINIA 1/2: ORIZONTALA");

    display.setTextSize(1);
    display.setTextColor(HAL_COLOR_WHITE);

    display.setCursor(10, 45);
    display.println("Trageti o linie dreapta continuu");

    display.setCursor(10, 58);
    display.println("de la STANGA la DREAPTA pe ecran.");

    display.setTextSize(2);
    display.setTextColor(HAL_COLOR_YELLOW);

    // Săgeată orizontală (dublu contur pentru a părea mai groasă)
    display.drawLine(40,  150, 180, 150, HAL_COLOR_YELLOW);
    display.drawLine(40,  151, 180, 151, HAL_COLOR_YELLOW);
    display.drawLine(160, 130, 180, 151, HAL_COLOR_YELLOW);
    display.drawLine(160, 131, 180, 152, HAL_COLOR_YELLOW);
    display.drawLine(160, 171, 180, 151, HAL_COLOR_YELLOW);
    display.drawLine(160, 170, 180, 150, HAL_COLOR_YELLOW);

    halDisplayDeselect();
}

void renderPageOrientY() {
    halDisplaySelect();

    display.fillScreen(HAL_COLOR_BLACK);

    display.setCursor(10, 12);
    display.setTextColor(HAL_COLOR_CYAN);
    display.setTextSize(2);
    display.println("LINIA 2/2: VERTICALA");

    display.setTextSize(1);
    display.setTextColor(HAL_COLOR_WHITE);

    display.setCursor(10, 45);
    display.println("Trageti o linie dreapta continuu");

    display.setCursor(10, 58);
    display.println("de SUS in JOS pe ecran.");

    // Săgeată verticală (dublu contur pentru a părea mai groasă)
    display.drawLine(120, 100, 120, 220, HAL_COLOR_YELLOW);
    display.drawLine(121, 100, 121, 220, HAL_COLOR_YELLOW);
    display.drawLine(100, 200, 120, 220, HAL_COLOR_YELLOW);
    display.drawLine(101, 200, 121, 220, HAL_COLOR_YELLOW);
    display.drawLine(140, 200, 120, 220, HAL_COLOR_YELLOW);
    display.drawLine(139, 200, 119, 220, HAL_COLOR_YELLOW);

    halDisplayDeselect();
}

void renderCalibPointScreen(int idx) {
    halDisplaySelect();

    display.fillScreen(HAL_COLOR_BLACK);

    display.setCursor(10, 10);
    display.setTextColor(HAL_COLOR_CYAN);
    display.setTextSize(2);
    display.print("PAS 2: COORDONATE ");
    display.print(idx + 1);
    display.println("/4");

    halDisplayDeselect();

    const int TX[] = {25, 215, 215, 25};
    const int TY[] = {85, 85, 300, 300};

    for (int i = 0; i < idx; i++) {
        drawCrosshair(TX[i], TY[i], HAL_COLOR_DARKGREY);
    }

    drawCrosshair(TX[idx], TY[idx], HAL_COLOR_YELLOW);
}

// ============================================================
// Setup și Loop principal
// ============================================================
void setup() {
    Serial.begin(115200);

    pinMode(RECALIB_BUTTON, INPUT_PULLUP);

    loadCalib();

    // Init hardware prin HAL (pini, SPI1, display, touch)
    halDisplayInit();
    halTouchInit();
    delay(50);

    // CRITIC: Curățarea forțată (Flush) a bufferului tactil rezidual
    // înainte de a evalua stadiul
    for (int i = 0; i < 5; i++) {
        halTouchRead();
        delay(20);
    }

    if (calib.stage == 1) {
        currentPage = PAGE_ORIENT_X;
        renderPageOrientX();
    }
    else if (calib.stage == 4) {
        currentPage = PAGE_ORIENT_Y;
        renderPageOrientY();
    }
    else if (calib.stage == 2) {
        currentPage = PAGE_CALIB;
        calibPointIndex = 0;
        renderCalibPointScreen(0);
    }
    else {
        currentPage = PAGE_INFO;
        renderPageInfo();
    }
}

void loop() {
    HalTouchPoint pt = halTouchRead();
    bool pressed = pt.pressed;

    // Poartă non-blocantă: după o acțiune, ignoră touch-ul până
    // când degetul este ridicat complet (debounce de 50 ms)
    if (awaitingRelease) {
        if (pressed) {
            releaseSince = 0;
        }
        else if (releaseSince == 0) {
            releaseSince = millis();
        }
        else if (millis() - releaseSince >= 50) {
            awaitingRelease = false;
            releaseSince = 0;
        }

        if (awaitingRelease) {
            delay(10);
            return;
        }
    }

    int pixelX = 0;
    int pixelY = 0;

    if (pressed && calib.stage == 3) {
        mapTouch(pt.x, pt.y, pixelX, pixelY);
    }

    // =========================================================
    // Buton hardware de recalibrare
    // =========================================================
    if (calib.stage == 3) {
        static uint8_t recalibPhase = 0;  // 0=repaus, 1=cronometrare, 2=așteaptă eliberarea
        static unsigned long recalibStart = 0;
        bool btnLow = (digitalRead(RECALIB_BUTTON) == LOW);

        if (recalibPhase == 0 && btnLow) {
            recalibPhase = 1;
            recalibStart = millis();
        }
        else if (recalibPhase == 1) {
            if (!btnLow) {
                recalibPhase = 0;  // Eliberat înainte de 2 s
            }
            else if (millis() - recalibStart >= 2000) {
                halDisplaySelect();
                display.fillScreen(HAL_COLOR_MAROON);
                display.setCursor(15, 120);
                display.setTextSize(2);
                display.setTextColor(HAL_COLOR_WHITE);
                display.print("ELIBERATI BUTONUL...");
                halDisplayDeselect();
                recalibPhase = 2;
            }
        }
        else if (recalibPhase == 2 && !btnLow) {
            calib.magic = 0;
            calib.stage = 1;
            saveCalib();
            forceReboot();
        }
    }

    // =========================================================
    // Modul normal de funcționare (calibrare completă)
    // =========================================================
    if (calib.stage == 3) {
        if (currentPage == PAGE_INFO) {
            static int lastInfoX = -1;
            static int lastInfoY = -1;

            if (pixelX != lastInfoX || pixelY != lastInfoY) {
                halDisplaySelect();

                display.fillRect(0, 285, TFT_W, 35, HAL_COLOR_BLACK);

                display.setCursor(5, 290);
                display.setTextSize(1);
                display.setTextColor(HAL_COLOR_CYAN);
                display.print("RAW X: ");
                display.print(pressed ? pt.x : 0);
                display.print(" Y: ");
                display.print(pressed ? pt.y : 0);

                display.setCursor(5, 305);
                display.setTextColor(HAL_COLOR_GREEN);
                display.print("MAP X: ");
                display.print(pressed ? pixelX : 0);
                display.print(" Y: ");
                display.print(pressed ? pixelY : 0);

                halDisplayDeselect();

                lastInfoX = pixelX;
                lastInfoY = pixelY;
            }

            if (pressed) {
                if (pixelX >= 10 && pixelX <= 115 && pixelY >= 100 && pixelY <= 145) {
                    currentPage = PAGE_DESEN;
                    renderPageDesen();
                    awaitingRelease = true;
                }
                else if (pixelX >= 125 && pixelX <= 230 && pixelY >= 100 && pixelY <= 145) {
                    currentPage = PAGE_RGB;
                    renderPageRgb();
                    awaitingRelease = true;
                }
                else if (pixelX >= 10 && pixelX <= 115 && pixelY >= 160 && pixelY <= 205) {
                    currentPage = PAGE_RAW;
                    renderPageRaw();
                    awaitingRelease = true;
                }
                else if (pixelX >= 125 && pixelX <= 230 && pixelY >= 160 && pixelY <= 205) {
                    calib.stage = 2;
                    saveCalib();
                    forceReboot();
                }
            }
        }
        else if (currentPage == PAGE_DESEN) {
            if (pressed) {
                if (pixelX >= 10 && pixelX <= 100 && pixelY >= 10 && pixelY <= 45) {
                    currentPage = PAGE_INFO;
                    renderPageInfo();
                    awaitingRelease = true;
                }
                else if (pixelX >= 140 && pixelX <= 230 && pixelY >= 10 && pixelY <= 45) {
                    renderPageDesen();
                    awaitingRelease = true;
                }
                else if (pixelY > 60 && pixelY < 292) {
                    halDisplaySelect();

                    if (lastValid) {
                        display.drawLine(lastX, lastY, pixelX, pixelY, HAL_COLOR_GREEN);
                    }
                    else {
                        display.fillRect(pixelX - 1, pixelY - 1, 3, 3, HAL_COLOR_GREEN);
                    }

                    halDisplayDeselect();

                    lastX = pixelX;
                    lastY = pixelY;
                    lastValid = true;
                }
            }
            else {
                lastValid = false;
            }

            static int pbx = -1;
            static int pby = -1;

            if (pixelX != pbx || pixelY != pby) {
                halDisplaySelect();

                display.fillRect(0, 295, TFT_W, 25, HAL_COLOR_BLACK);
                display.setCursor(10, 300);
                display.setTextColor(HAL_COLOR_CYAN);
                display.setTextSize(1);
                display.print("X: ");
                display.print(pixelX);
                display.print(" | Y: ");
                display.print(pixelY);

                halDisplayDeselect();

                pbx = pixelX;
                pby = pixelY;
            }
        }
        else if (currentPage == PAGE_RGB) {
            if (pressed && pixelX >= 10 && pixelX <= 90 && pixelY >= 10 && pixelY <= 40) {
                currentPage = PAGE_INFO;
                renderPageInfo();
                awaitingRelease = true;
            }
        }
        else if (currentPage == PAGE_RAW) {
            int currentMapX = 0;
            int currentMapY = 0;

            if (pressed) {
                lastRawX = pt.x;
                lastRawY = pt.y;
                lastRawZ = pt.z;

                // NOTA: aici se folosesc in mod intentionat valorile brute
                // default (nu calib.*) — vezi pct. 4 din analiza; neatins.
                lastMapX = constrain(safeMap(pt.x, 3700, 370, 0, 239), 0, 239);
                lastMapY = constrain(safeMap(pt.y, 300, 3700, 0, 319), 0, 319);

                currentMapX = lastMapX;
                currentMapY = lastMapY;

                if (lastMapX >= 10 && lastMapX <= 230 && lastMapY >= 10 && lastMapY <= 42) {
                    currentPage = PAGE_INFO;
                    renderPageInfo();
                    awaitingRelease = true;
                    return;
                }
            }

            halDisplaySelect();

            display.setTextSize(2);

            display.setCursor(15, 60);
            display.setTextColor(HAL_COLOR_WHITE, HAL_COLOR_BLACK);
            display.print("X: ");
            display.setTextColor(HAL_COLOR_YELLOW, HAL_COLOR_BLACK);
            display.print(pressed ? pt.x : 0);
            display.print("    ");
            display.setCursor(140, 60);
            display.setTextColor(HAL_COLOR_ORANGE, HAL_COLOR_BLACK);
            display.print(lastRawX);
            display.print("    ");

            display.setCursor(15, 82);
            display.setTextColor(HAL_COLOR_WHITE, HAL_COLOR_BLACK);
            display.print("Y: ");
            display.setTextColor(HAL_COLOR_YELLOW, HAL_COLOR_BLACK);
            display.print(pressed ? pt.y : 0);
            display.print("    ");
            display.setCursor(140, 82);
            display.setTextColor(HAL_COLOR_ORANGE, HAL_COLOR_BLACK);
            display.print(lastRawY);
            display.print("    ");

            display.setCursor(15, 104);
            display.setTextColor(HAL_COLOR_WHITE, HAL_COLOR_BLACK);
            display.print("Z: ");
            display.setTextColor(HAL_COLOR_YELLOW, HAL_COLOR_BLACK);
            display.print(pressed ? pt.z : 0);
            display.print("    ");
            display.setCursor(140, 104);
            display.setTextColor(HAL_COLOR_ORANGE, HAL_COLOR_BLACK);
            display.print(lastRawZ);
            display.print("    ");

            display.setCursor(15, 135);
            display.setTextColor(HAL_COLOR_WHITE, HAL_COLOR_BLACK);
            display.print("MX:");
            display.setTextColor(HAL_COLOR_GREEN, HAL_COLOR_BLACK);
            display.print(pressed ? currentMapX : 0);
            display.print("    ");
            display.setCursor(140, 135);
            display.setTextColor(HAL_COLOR_DARKGREEN, HAL_COLOR_BLACK);
            display.print(lastMapX);
            display.print("    ");

            display.setCursor(15, 157);
            display.setTextColor(HAL_COLOR_WHITE, HAL_COLOR_BLACK);
            display.print("MY:");
            display.setTextColor(HAL_COLOR_GREEN, HAL_COLOR_BLACK);
            display.print(pressed ? currentMapY : 0);
            display.print("    ");
            display.setCursor(140, 157);
            display.setTextColor(HAL_COLOR_DARKGREEN, HAL_COLOR_BLACK);
            display.print(lastMapY);
            display.print("    ");

            display.setTextSize(1);
            display.setCursor(15, 185);
            display.setTextColor(
                halTouchIrqActive() ? HAL_COLOR_YELLOW : HAL_COLOR_GREEN,
                HAL_COLOR_BLACK
            );
            display.print("PENIRQ HW Stare: ");
            display.println(halTouchIrqActive() ? "ATINS " : "REPAUS");

            halDisplayDeselect();
        }
    }

    // =========================================================
    // Etapele de calibrare
    // =========================================================
    else if (calib.stage == 1 && currentPage == PAGE_ORIENT_X) {
        if (pressed) {
            // Reține primele 2 eșantioane ca punct de start
            if (swipeCount < 2) {
                swipeStartX[swipeCount] = pt.x;
                swipeStartY[swipeCount] = pt.y;
            }
            // Actualizează în permanență ultimele 2 eșantioane
            swipeEndX[0] = swipeEndX[1];
            swipeEndX[1] = pt.x;
            swipeEndY[0] = swipeEndY[1];
            swipeEndY[1] = pt.y;
            swipeCount++;
        }
        else if (swipeCount > 0) {
            if (swipeCount >= 3) {
                // Media ultimelor 2 vs. media primelor 2 = capetele traseului
                long diffX = (long)((swipeEndX[0] + swipeEndX[1]) / 2)
                           - (long)((swipeStartX[0] + swipeStartX[1]) / 2);
                long diffY = (long)((swipeEndY[0] + swipeEndY[1]) / 2)
                           - (long)((swipeStartY[0] + swipeStartY[1]) / 2);

                calib.swapXY = (abs(diffX) > abs(diffY)) ? 0 : 1;

                long dominantX = calib.swapXY ? diffY : diffX;
                calib.invX = (dominantX > 0) ? 0 : 1;

                calib.stage = 4;
                saveCalib();
                forceReboot();
            }
            // Prea puține eșantioane (atingere scurtă): ignoră și reia
            swipeCount = 0;
        }
    }
    else if (calib.stage == 4 && currentPage == PAGE_ORIENT_Y) {
        if (pressed) {
            // Reține primele 2 eșantioane ca punct de start
            if (swipeCount < 2) {
                swipeStartX[swipeCount] = pt.x;
                swipeStartY[swipeCount] = pt.y;
            }
            // Actualizează în permanență ultimele 2 eșantioane
            swipeEndX[0] = swipeEndX[1];
            swipeEndX[1] = pt.x;
            swipeEndY[0] = swipeEndY[1];
            swipeEndY[1] = pt.y;
            swipeCount++;
        }
        else if (swipeCount > 0) {
            if (swipeCount >= 3) {
                // Media ultimelor 2 vs. media primelor 2 = capetele traseului
                long diffX = (long)((swipeEndX[0] + swipeEndX[1]) / 2)
                           - (long)((swipeStartX[0] + swipeStartX[1]) / 2);
                long diffY = (long)((swipeEndY[0] + swipeEndY[1]) / 2)
                           - (long)((swipeStartY[0] + swipeStartY[1]) / 2);

                long dominantY = calib.swapXY ? diffX : diffY;
                calib.invY = (dominantY > 0) ? 0 : 1;

                calib.stage = 2;
                saveCalib();
                forceReboot();
            }
            // Prea puține eșantioane (atingere scurtă): ignoră și reia
            swipeCount = 0;
        }
    }
    else if (calib.stage == 2 && currentPage == PAGE_CALIB) {
        if (collectState == COLLECT_IDLE) {
            if (pt.z >= Z_SAMPLE_MIN) {
                // A început atingerea: pornește eșantionarea
                collectState = COLLECT_SAMPLING;
                sampleCount = 0;
                calibIdleSince = 0;
            }
            else {
                // Timeout la așteptarea atingerii: reafișează ecranul cu mesaj
                if (calibIdleSince == 0) {
                    calibIdleSince = millis();
                }
                else if (millis() - calibIdleSince >= CALIB_POINT_TIMEOUT_MS) {
                    renderCalibPointScreen(calibPointIndex);

                    halDisplaySelect();
                    display.setCursor(10, 250);
                    display.setTextSize(1);
                    display.setTextColor(HAL_COLOR_RED, HAL_COLOR_BLACK);
                    display.print("Timeout! Atingeti din nou punctul. ");
                    halDisplayDeselect();

                    calibIdleSince = millis();
                }
            }
        }
        else if (pt.z >= Z_SAMPLE_MIN) {
            // Colectează câte un eșantion pe iterația loop-ului (~10 ms)
            if (sampleCount < 12) {
                calibSamplesX[sampleCount] = pt.x;
                calibSamplesY[sampleCount] = pt.y;
                sampleCount++;
            }
        }
        else {
            // Degetul a fost ridicat: finalizează punctul curent
            if (sampleCount >= 5) {
                int32_t sumX = 0;
                int32_t sumY = 0;
                for (int i = 2; i < sampleCount - 2; i++) {
                    sumX += calibSamplesX[i];
                    sumY += calibSamplesY[i];
                }

                int32_t mx, my;
                processRawTouch(sumX / (sampleCount - 4), sumY / (sampleCount - 4), mx, my, true);

                calibX[calibPointIndex] = mx;
                calibY[calibPointIndex] = my;
                calibPointIndex++;

                if (calibPointIndex >= 4) {
                    int32_t loX = calibX[0];
                    int32_t hiX = calibX[0];
                    int32_t loY = calibY[0];
                    int32_t hiY = calibY[0];

                    for (int i = 1; i < 4; i++) {
                        if (calibX[i] < loX) loX = calibX[i];
                        if (calibX[i] > hiX) hiX = calibX[i];
                        if (calibY[i] < loY) loY = calibY[i];
                        if (calibY[i] > hiY) hiY = calibY[i];
                    }

                    int32_t deltaX = hiX - loX;
                    int32_t deltaY = hiY - loY;

                    calib.xmin = loX - (25 * deltaX) / 190;
                    calib.xmax = hiX + (25 * deltaX) / 190;
                    calib.ymin = loY - (85 * deltaY) / 215;
                    calib.ymax = hiY + (20 * deltaY) / 215;

                    calib.stage = 3;
                    saveCalib();
                    forceReboot();
                }
                else {
                    renderCalibPointScreen(calibPointIndex);
                }
            }
            collectState = COLLECT_IDLE;
        }
    }

    delay(10);
}
