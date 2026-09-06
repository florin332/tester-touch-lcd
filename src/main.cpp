#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
// Prototipuri de funcții necesare pentru compilatorul PlatformIO C++
TS_Point getIsolatedTouchPoint();
int safeMap(int v, int fl, int fh, int tl, int th);
void loadCalib();
bool saveCalib();
void forceReboot();
void processRawTouch(int32_t rx, int32_t ry, int32_t &cx, int32_t &cy, bool useConfig);
void mapTouch(int32_t rx, int32_t ry, int &ox, int &oy);
void waitForRelease();
bool collectCalibPoint(int32_t &ox, int32_t &oy, bool useConfig);
void drawButton(int x, int y, int w, int h, const char *l, uint16_t fg, uint16_t bg);
void drawCrosshair(int cx, int cy, uint16_t clr);
void renderPageInfo();
void renderPageDesen();
void renderPageRgb();
void renderPageRaw();
void renderPageOrientX();
void renderPageOrientY();
void renderCalibPointScreen(int idx);


#define INVERT_COLORS true
#define TFT_CS 22
#define TFT_RST 20
#define TFT_DC 21
#define TFT_MOSI 19
#define TFT_LED 4
#define TFT_SCK 18
#define TOUCH_CS 26
#define TOUCH_MISO 16
#define TOUCH_IRQ 27
#define RECALIB_BUTTON 5   
#define Z_TOUCH_MIN 200
#define Z_SAMPLE_MIN 300
#define CALIB_MAGIC 0x544C4344
#define CALIB_VER 6
struct CalibData { uint32_t magic; uint16_t version; int32_t xmin, xmax, ymin, ymax; uint8_t swapXY, invX, invY, stage; };
CalibData calib;
enum Page { PAGE_INFO, PAGE_DESEN, PAGE_RGB, PAGE_RAW, PAGE_ORIENT_X, PAGE_ORIENT_Y, PAGE_CALIB };
Page currentPage = PAGE_INFO;
enum CalibState { CAL_ST, CAL_DR, CAL_DJ, CAL_SJ };
CalibState calibState = CAL_ST;
int calibPointIndex = 0;
int32_t calibX[4], calibY[4]; // Array-uri declarate explicit local pentru colțuri   
int32_t lastRawX = 0, lastRawY = 0, lastRawZ = 0;
int lastMapX = 0, lastMapY = 0;
int lastX = -1, lastY = -1; bool lastValid = false;
bool armRecalib = false;
Adafruit_ILI9341 display((int8_t)TFT_CS, (int8_t)TFT_DC, (int8_t)TFT_RST);
XPT2046_Touchscreen touch(TOUCH_CS);
TS_Point getIsolatedTouchPoint() {
    digitalWrite(TFT_CS, HIGH); digitalWrite(TOUCH_CS, LOW);
    TS_Point p = touch.getPoint(); digitalWrite(TOUCH_CS, HIGH); return p;
}
int safeMap(int v, int fl, int fh, int tl, int th) { return tl + (v - fl) * (th - tl) / (fh - fl); }
void loadCalib() {
    EEPROM.begin(64); EEPROM.get(0, calib);
    if (calib.magic != CALIB_MAGIC || calib.version != CALIB_VER) {
        calib.magic = 0; calib.xmin = 3700; calib.xmax = 370; calib.ymin = 300; calib.ymax = 3700;
        calib.swapXY = 0; calib.invX = 0; calib.invY = 0; calib.stage = 3; 
    }
}
bool saveCalib() { calib.magic = CALIB_MAGIC; calib.version = CALIB_VER; EEPROM.put(0, calib); return EEPROM.commit(); }
void forceReboot() { delay(500); rp2040.reboot(); }
void processRawTouch(int32_t rx, int32_t ry, int32_t &cx, int32_t &cy, bool useConfig) {
    cx = rx; cy = ry;
    if (useConfig && calib.swapXY) { int32_t t = cx; cx = cy; cy = t; }
    if (useConfig && calib.invX) cx = 4095 - cx;
    if (useConfig && calib.invY) cy = 4095 - cy;
}
void mapTouch(int32_t rx, int32_t ry, int &ox, int &oy) {
    int32_t cx, cy; processRawTouch(rx, ry, cx, cy, true);
    ox = constrain(safeMap(cx, calib.xmin, calib.xmax, 0, 239), 0, 239);
    oy = constrain(safeMap(cy, calib.ymin, calib.ymax, 0, 319), 0, 319);
}
void waitForRelease() { while (getIsolatedTouchPoint().z > Z_TOUCH_MIN) delay(10); delay(50); }
bool collectCalibPoint(int32_t &ox, int32_t &oy, bool useConfig) {
    unsigned long to = millis() + 10000; TS_Point p;
    do { p = getIsolatedTouchPoint(); if (millis() > to) return false; delay(10); } while (p.z < Z_SAMPLE_MIN);
    int32_t xs[12], ys[12]; int n = 0; 
    while (n < 12) { p = getIsolatedTouchPoint(); if (p.z < Z_SAMPLE_MIN) break; xs[n] = p.x; ys[n] = p.y; n++; delay(25); }
    if (n < 5) return false;
    int32_t sx = 0, sy = 0; for (int i = 2; i < n - 2; i++) { sx += xs[i]; sy += ys[i]; }
    processRawTouch(sx / (n - 4), sy / (n - 4), ox, oy, useConfig); waitForRelease(); return true;
}
void drawButton(int x, int y, int w, int h, const char *l, uint16_t fg, uint16_t bg) {
    digitalWrite(TFT_CS, LOW); display.fillRoundRect(x, y, w, h, 4, bg); display.drawRoundRect(x, y, w, h, 4, fg);
    display.setCursor(x + (w - (int16_t)strlen(l) * 12) / 2, y + (h - 14) / 2); display.setTextColor(fg); display.setTextSize(2); display.print(l); digitalWrite(TFT_CS, HIGH);
}
void drawCrosshair(int cx, int cy, uint16_t clr) {
    digitalWrite(TFT_CS, LOW); display.drawCircle(cx, cy, 10, clr); display.drawLine(cx - 16, cy, cx - 6, cy, clr);
    display.drawLine(cx + 6, cy, cx + 16, cy, clr); display.drawLine(cx, cy - 16, cx, cy - 6, clr);
    display.drawLine(cx, cy + 6, cx, cy + 16, clr); display.fillCircle(cx, cy, 2, clr); digitalWrite(TFT_CS, HIGH);
}
void renderPageInfo() {
    digitalWrite(TFT_CS, LOW); display.fillScreen(ILI9341_BLACK); display.setCursor(10, 12); display.setTextColor(ILI9341_CYAN); display.setTextSize(2); display.println("TESTER LCD SPI");
    display.setTextSize(1); display.setTextColor(ILI9341_YELLOW); display.setCursor(10, 40); display.print("Xmin="); display.print(calib.xmin); display.print(" Xmax="); display.print(calib.xmax);
    display.setCursor(10, 52); display.print("Ymin="); display.print(calib.ymin); display.print(" Ymax="); display.print(calib.ymax);
    display.setCursor(10, 64); display.print("SwapXY="); display.print(calib.swapXY); display.print(" InvX="); display.print(calib.invX); display.print(" InvY="); display.print(calib.invY);
    display.setCursor(10, 76); display.setTextColor(ILI9341_GREEN); display.print("Status Touch: CALIBRAT COMPLET");
    display.drawFastHLine(0, 280, 240, ILI9341_DARKGREY);
    digitalWrite(TFT_CS, HIGH); drawButton(10, 100, 105, 45, "DESEN", ILI9341_WHITE, 0x001F); drawButton(125, 100, 105, 45, "RGB", ILI9341_WHITE, 0x7800);
    drawButton(10, 160, 105, 45, "RAW", ILI9341_WHITE, 0x03E0); drawButton(125, 160, 105, 45, "CALIBRARE", ILI9341_BLACK, ILI9341_YELLOW);
}
void renderPageDesen() { digitalWrite(TFT_CS, LOW); display.fillScreen(ILI9341_BLACK); display.drawFastHLine(0, 55, 240, ILI9341_DARKGREY); display.drawFastHLine(0, 292, 240, ILI9341_DARKGREY); digitalWrite(TFT_CS, HIGH); drawButton(10, 10, 90, 35, "INAPOI", ILI9341_WHITE, 0x000F); drawButton(140, 10, 90, 35, "STERGE", ILI9341_BLACK, ILI9341_RED); lastValid = false; }
void renderPageRgb() { digitalWrite(TFT_CS, LOW); display.fillRect(0, 0, 240, 107, ILI9341_RED); display.fillRect(0, 107, 240, 106, ILI9341_GREEN); display.fillRect(0, 213, 240, 107, ILI9341_BLUE); display.setTextSize(2); display.setTextColor(ILI9341_WHITE); display.setCursor(95, 45); display.print("RED"); display.setCursor(85, 151); display.print("GREEN"); display.setCursor(90, 257); display.print("BLUE"); display.fillRect(10, 10, 80, 30, ILI9341_BLACK); display.drawRect(10, 10, 80, 30, ILI9341_WHITE); display.setCursor(18, 17); display.print("INAPOI"); digitalWrite(TFT_CS, HIGH); }
void renderPageRaw() { 
    digitalWrite(TFT_CS, LOW); display.fillScreen(ILI9341_BLACK); display.drawFastHLine(0, 215, 240, ILI9341_DARKGREY); display.setTextSize(1); display.setTextColor(ILI9341_WHITE);
    display.drawCircle(26, 235, 4, ILI9341_CYAN); display.setCursor(35, 232); display.print("T1(26,235)"); display.drawCircle(150, 235, 4, ILI9341_CYAN); display.setCursor(160, 232); display.print("T2(150,235)");
    display.drawCircle(148, 273, 4, ILI9341_CYAN); display.setCursor(158, 270); display.print("T3(148,273)"); display.drawCircle(27, 272, 4, ILI9341_CYAN); display.setCursor(37, 269); display.print("T4(27,272)");
    display.setCursor(90, 48); display.setTextColor(ILI9341_YELLOW); display.print("LIVE"); display.setCursor(160, 48); display.setTextColor(ILI9341_ORANGE); display.print("LAST");
    digitalWrite(TFT_CS, HIGH); drawButton(10, 10, 220, 32, "INAPOI", ILI9341_WHITE, 0x000F); 
}
void renderPageOrientX() { digitalWrite(TFT_CS, LOW); display.fillScreen(ILI9341_BLACK); display.setCursor(10, 12); display.setTextColor(ILI9341_CYAN); display.setTextSize(2); display.println("LINIA 1/2: ORIZONTALA"); display.setTextSize(1); display.setTextColor(ILI9341_WHITE); display.setCursor(10, 45); display.println("Trageti o linie dreapta continuu"); display.setCursor(10, 58); display.println("de la STANGA la DREAPTA pe ecran."); display.setTextSize(4); display.setTextColor(ILI9341_YELLOW); display.setCursor(40, 140); display.println("======>>"); digitalWrite(TFT_CS, HIGH); }
void renderPageOrientY() { digitalWrite(TFT_CS, LOW); display.fillScreen(ILI9341_BLACK); display.setCursor(10, 12); display.setTextColor(ILI9341_CYAN); display.setTextSize(2); display.println("LINIA 2/2: VERTICALA"); display.setTextSize(1); display.setTextColor(ILI9341_WHITE); display.setCursor(10, 45); display.println("Trageti o linie dreapta continuu"); display.setCursor(10, 58); display.println("de SUS in JOS pe ecran."); display.drawFastVLine(120, 100, 80, ILI9341_YELLOW); display.drawLine(110, 160, 120, 180, ILI9341_YELLOW); display.drawLine(130, 160, 120, 180, ILI9341_YELLOW); digitalWrite(TFT_CS, HIGH); }
void renderCalibPointScreen(int idx) { digitalWrite(TFT_CS, LOW); display.fillScreen(ILI9341_BLACK); display.setCursor(10, 10); display.setTextColor(ILI9341_CYAN); display.setTextSize(2); display.print("PAS 2: COORDONATE "); display.print(idx + 1); display.println("/4"); digitalWrite(TFT_CS, HIGH); const int TX[] = {25, 215, 215, 25}; const int TY[] = {85, 85, 300, 300}; for (int i = 0; i < idx; i++) drawCrosshair(TX[i], TY[i], ILI9341_DARKGREY); drawCrosshair(TX[idx], TY[idx], ILI9341_YELLOW); }
void setup() {
    Serial.begin(115200); pinMode(RECALIB_BUTTON, INPUT_PULLUP); loadCalib();
    pinMode(TFT_CS, OUTPUT); digitalWrite(TFT_CS, HIGH); pinMode(TOUCH_CS, OUTPUT); digitalWrite(TOUCH_CS, HIGH); pinMode(TFT_LED, OUTPUT); digitalWrite(TFT_LED, HIGH);
    SPI.setTX(TFT_MOSI); SPI.setSCK(TFT_SCK); SPI.setRX(TOUCH_MISO); SPI.begin();
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); SPI.endTransaction();
    display.begin(); display.invertDisplay(INVERT_COLORS); display.setRotation(0);
    pinMode(TOUCH_IRQ, INPUT_PULLUP); touch.begin(); touch.setRotation(0); delay(50);
    
    // CRITIC: Curățarea forțată (Flush) a bufferului tactil rezidual înainte de a evalua stadiul
    for (int i = 0; i < 5; i++) { getIsolatedTouchPoint(); delay(20); }
    
    if (calib.stage == 1) { currentPage = PAGE_ORIENT_X; renderPageOrientX(); }
    else if (calib.stage == 4) { currentPage = PAGE_ORIENT_Y; renderPageOrientY(); }
    else if (calib.stage == 2) { currentPage = PAGE_CALIB; calibState = CAL_ST; calibPointIndex = 0; renderCalibPointScreen(0); }
    else { currentPage = PAGE_INFO; renderPageInfo(); }
}


void loop() {
    TS_Point pt = getIsolatedTouchPoint(); bool pr = (pt.z > Z_TOUCH_MIN); int pixelX = 0, pixelY = 0;
    if (pr && calib.stage == 3) mapTouch(pt.x, pt.y, pixelX, pixelY);
    if (calib.stage == 3 && digitalRead(RECALIB_BUTTON) == LOW) {
        unsigned long pressStart = millis(); bool validHold = true;
        while (millis() - pressStart < 2000) { if (digitalRead(RECALIB_BUTTON) == HIGH) { validHold = false; break; } delay(20); }
        if (validHold) {
            digitalWrite(TFT_CS, LOW); display.fillScreen(ILI9341_MAROON); display.setCursor(15, 120); display.setTextSize(2); display.setTextColor(ILI9341_WHITE);
            display.print("ELIBERATI BUTONUL..."); digitalWrite(TFT_CS, HIGH);
            while(digitalRead(RECALIB_BUTTON) == LOW) delay(10); 
            armRecalib = true; // Armare sigură
            calib.magic = 0; calib.stage = 1; saveCalib(); forceReboot();
        }
    }
    if (calib.stage == 3) {
        if (currentPage == PAGE_INFO) {
            static int lastInfoX = -1, lastInfoY = -1;
            if (pixelX != lastInfoX || pixelY != lastInfoY) {
                digitalWrite(TFT_CS, LOW); display.fillRect(0, 285, 240, 35, ILI9341_BLACK);
                display.setCursor(5, 290); display.setTextSize(1); display.setTextColor(ILI9341_CYAN);
                display.print("RAW X: "); display.print(pr ? pt.x : 0); display.print(" Y: "); display.print(pr ? pt.y : 0);
                display.setCursor(5, 305); display.setTextColor(ILI9341_GREEN);
                display.print("MAP X: "); display.print(pr ? pixelX : 0); display.print(" Y: "); display.print(pr ? pixelY : 0);
                digitalWrite(TFT_CS, HIGH); lastInfoX = pixelX; lastInfoY = pixelY;
            }
            if (pr) {
                if (pixelX >= 10 && pixelX <= 115 && pixelY >= 100 && pixelY <= 145) { waitForRelease(); currentPage = PAGE_DESEN; renderPageDesen(); }
                else if (pixelX >= 125 && pixelX <= 230 && pixelY >= 100 && pixelY <= 145) { waitForRelease(); currentPage = PAGE_RGB; renderPageRgb(); }
                else if (pixelX >= 10 && pixelX <= 115 && pixelY >= 160 && pixelY <= 205) { waitForRelease(); currentPage = PAGE_RAW; renderPageRaw(); }
                else if (pixelX >= 125 && pixelX <= 230 && pixelY >= 160 && pixelY <= 205) { waitForRelease(); calib.stage = 2; saveCalib(); forceReboot(); }
            }
        }
        else if (currentPage == PAGE_DESEN) {
            if (pr) {
                if (pixelX >= 10 && pixelX <= 100 && pixelY >= 10 && pixelY <= 45) { waitForRelease(); currentPage = PAGE_INFO; renderPageInfo(); }
                else if (pixelX >= 140 && pixelX <= 230 && pixelY >= 10 && pixelY <= 45) { renderPageDesen(); waitForRelease(); }
                else if (pixelY > 60 && pixelY < 292) {
                    digitalWrite(TFT_CS, LOW); if (lastValid) display.drawLine(lastX, lastY, pixelX, pixelY, ILI9341_GREEN); else display.fillRect(pixelX - 1, pixelY - 1, 3, 3, ILI9341_GREEN);
                    digitalWrite(TFT_CS, HIGH); lastX = pixelX; lastY = pixelY; lastValid = true;
                }
            } else { lastValid = false; }
            static int pbx = -1, pby = -1; if (pixelX != pbx || pixelY != pby) {
                digitalWrite(TFT_CS, LOW); display.fillRect(0, 295, 240, 25, ILI9341_BLACK); display.setCursor(10, 300); display.setTextColor(ILI9341_CYAN); display.setTextSize(1);
                display.print("X: "); display.print(pixelX); display.print(" | Y: "); display.print(pixelY); digitalWrite(TFT_CS, HIGH); pbx = pixelX; pby = pixelY;
            }
        }
        else if (currentPage == PAGE_RGB) { if (pr && pixelX >= 10 && pixelX <= 90 && pixelY >= 10 && pixelY <= 40) { waitForRelease(); currentPage = PAGE_INFO; renderPageInfo(); } }
        else if (currentPage == PAGE_RAW) {
            int currentMapX = 0, currentMapY = 0;
            if (pr) {
                lastRawX = pt.x; lastRawY = pt.y; lastRawZ = pt.z;
                lastMapX = constrain(safeMap(pt.x, 3700, 370, 0, 239), 0, 239);
                lastMapY = constrain(safeMap(pt.y, 300, 3700, 0, 319), 0, 319);
                currentMapX = lastMapX; currentMapY = lastMapY;
                if (lastMapX >= 10 && lastMapX <= 230 && lastMapY >= 10 && lastMapY <= 42) { waitForRelease(); currentPage = PAGE_INFO; renderPageInfo(); return; }
            }
            digitalWrite(TFT_CS, LOW); display.setTextSize(2);
            display.setCursor(15, 60); display.setTextColor(ILI9341_WHITE, ILI9341_BLACK); display.print("X: "); display.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); display.print(pr ? pt.x : 0); display.print(" "); display.setCursor(140, 60); display.setTextColor(ILI9341_ORANGE, ILI9341_BLACK); display.print(lastRawX); display.print(" ");
            display.setCursor(15, 82); display.setTextColor(ILI9341_WHITE, ILI9341_BLACK); display.print("Y: "); display.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); display.print(pr ? pt.y : 0); display.print(" "); display.setCursor(140, 82); display.setTextColor(ILI9341_ORANGE, ILI9341_BLACK); display.print(lastRawY); display.print(" ");
            display.setCursor(15, 104); display.setTextColor(ILI9341_WHITE, ILI9341_BLACK); display.print("Z: "); display.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); display.print(pr ? pt.z : 0); display.print(" "); display.setCursor(140, 104); display.setTextColor(ILI9341_ORANGE, ILI9341_BLACK); display.print(lastRawZ); display.print(" ");
            display.setCursor(15, 135); display.setTextColor(ILI9341_WHITE, ILI9341_BLACK); display.print("MX:"); display.setTextColor(ILI9341_GREEN, ILI9341_BLACK); display.print(pr ? currentMapX : 0); display.print(" "); display.setCursor(140, 135); display.setTextColor(ILI9341_DARKGREEN, ILI9341_BLACK); display.print(lastMapX); display.print(" ");
            display.setCursor(15, 157); display.setTextColor(ILI9341_WHITE, ILI9341_BLACK); display.print("MY:"); display.setTextColor(ILI9341_GREEN, ILI9341_BLACK); display.print(pr ? currentMapY : 0); display.print(" "); display.setCursor(140, 157); display.setTextColor(ILI9341_DARKGREEN, ILI9341_BLACK); display.print(lastMapY); display.print(" ");
            display.setTextSize(1); display.setCursor(15, 185); display.setTextColor(digitalRead(TOUCH_IRQ) == LOW ? ILI9341_YELLOW : ILI9341_GREEN, ILI9341_BLACK);
            display.print("PENIRQ HW Stare: "); display.println(digitalRead(TOUCH_IRQ) == LOW ? "ATINS " : "REPAUS"); digitalWrite(TFT_CS, HIGH);
        }
    }
    else if (calib.stage == 1 && currentPage == PAGE_ORIENT_X) {
        if (pr) {
            TS_Point startPt = getIsolatedTouchPoint(); delay(350); TS_Point endPt = getIsolatedTouchPoint();
            long diffX = (long)endPt.x - (long)startPt.x; long diffY = (long)endPt.y - (long)startPt.y;
            calib.swapXY = (abs(diffX) > abs(diffY)) ? 0 : 1;
            long dominantX = calib.swapXY ? diffY : diffX; calib.invX = (dominantX > 0) ? 0 : 1;
            calib.stage = 4; saveCalib(); forceReboot();
        }
    }
    else if (calib.stage == 4 && currentPage == PAGE_ORIENT_Y) {
        if (pr) {
            TS_Point startPt = getIsolatedTouchPoint(); delay(350); TS_Point endPt = getIsolatedTouchPoint();
            long diffX = (long)endPt.x - (long)startPt.x; long diffY = (long)endPt.y - (long)startPt.y;
            long dominantY = calib.swapXY ? diffX : diffY; calib.invY = (dominantY > 0) ? 0 : 1;
            calib.stage = 2; saveCalib(); forceReboot();
        }
    }
    else if (calib.stage == 2 && currentPage == PAGE_CALIB) {
        if (pr) {
            int32_t mx, my; if (collectCalibPoint(mx, my, true)) {
                calibX[calibPointIndex] = mx; calibY[calibPointIndex] = my; calibPointIndex++;
                if (calibPointIndex >= 4) {
                    int32_t loX = calibX[0], hiX = calibX[0], loY = calibY[0], hiY = calibY[0];
                    for(int i=1; i<4; i++) {
                        if(calibX[i] < loX) loX = calibX[i]; if(calibX[i] > hiX) hiX = calibX[i];
                        if(calibY[i] < loY) loY = calibY[i]; if(calibY[i] > hiY) hiY = calibY[i];
                    }
                    int32_t deltaX = hiX - loX; int32_t deltaY = hiY - loY;
                    calib.xmin = loX - (25 * deltaX) / 190; calib.xmax = hiX + (25 * deltaX) / 190;
                    calib.ymin = loY - (85 * deltaY) / 215; calib.ymax = hiY + (20 * deltaY) / 215;
                    calib.stage = 3; saveCalib(); forceReboot();
                } else { renderCalibPointScreen(calibPointIndex); }
            }
        }
    }
    delay(10);
}
