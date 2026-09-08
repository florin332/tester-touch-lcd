// ============================================================
// Model 3 - ST7735 128x160
// Display: ST7735 128x160 pe SPI1 | Touch: XPT2046
// Atentie: la rezolutia 128x160 UI-ul aplicatiei nu incape
// identic — coordonatele butoanelor din main.cpp sunt pentru
// 240x320 si vor trebui adaptate intr-o iteratie viitoare.
// Pinii se ajusteaza aici, la conectarea hardware.
// ============================================================

#include "LcdConfig.h"

#if LCD_MODEL_ACTIVE == LCD_MODEL_3_ST7735

#include <Adafruit_ST7735.h>
#include <XPT2046_Touchscreen.h>

// ------------------------------------------------------------
// Pinout (ajusteaza la modulul tau!)
// ------------------------------------------------------------
static const int8_t PIN_TFT_CS   = 13;
static const int8_t PIN_TFT_RST  = 14;
static const int8_t PIN_TFT_DC   = 6;
static const int8_t PIN_TFT_MOSI = 11;
static const int8_t PIN_TFT_LED  = 4;
static const int8_t PIN_TFT_SCK  = 10;
static const int8_t PIN_TFT_MISO = 12;
static const int8_t PIN_TCH_CS   = 9;
static const int8_t PIN_TCH_IRQ  = 8;

// ------------------------------------------------------------
// Configul modelului
// ------------------------------------------------------------
const LcdBoardConfig LCD = {
    "Model 3 - ST7735 128x160",
    128, 160,             // width, height
    0,                    // rotation
    false,                // invertColors
    PIN_TFT_CS, PIN_TFT_RST, PIN_TFT_DC,
    PIN_TFT_MOSI, PIN_TFT_SCK, PIN_TFT_MISO, PIN_TFT_LED,
    PIN_TCH_CS, PIN_TCH_IRQ,
    3700, 370,            // defXmin, defXmax
    300, 3700,            // defYmin, defYmax
    200, 300,             // zTouchMin, zSampleMin
    0x544C4346            // calibMagic — calibrare separata
};

// ------------------------------------------------------------
// Instante drivere
// ------------------------------------------------------------
static Adafruit_ST7735 tft(&SPI1, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
static XPT2046_Touchscreen ts(PIN_TCH_CS);

Adafruit_GFX &display = tft;

// ------------------------------------------------------------
// Implementari HAL
// ------------------------------------------------------------
void halDisplayInit() {
    pinMode(PIN_TFT_CS, OUTPUT);
    digitalWrite(PIN_TFT_CS, HIGH);

    pinMode(PIN_TFT_LED, OUTPUT);
    digitalWrite(PIN_TFT_LED, HIGH);

    SPI1.setTX(PIN_TFT_MOSI);
    SPI1.setSCK(PIN_TFT_SCK);
    SPI1.setRX(PIN_TFT_MISO);
    SPI1.begin();

    SPI1.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    SPI1.endTransaction();

    tft.initR(INITR_BLACKTAB);   // varianta comuna de ST7735
    tft.invertDisplay(LCD.invertColors);
    tft.setRotation(LCD.rotation);
}

void halTouchInit() {
    pinMode(PIN_TCH_CS, OUTPUT);
    digitalWrite(PIN_TCH_CS, HIGH);

    pinMode(PIN_TCH_IRQ, INPUT_PULLUP);

    ts.begin(SPI1);
    ts.setRotation(LCD.rotation);
}

HalTouchPoint halTouchRead() {
    HalTouchPoint hp;

    digitalWrite(PIN_TFT_CS, HIGH);
    digitalWrite(PIN_TCH_CS, LOW);

    TS_Point p = ts.getPoint();

    digitalWrite(PIN_TCH_CS, HIGH);

    hp.x = p.x;
    hp.y = p.y;
    hp.z = p.z;
    hp.pressed = (p.z > LCD.zTouchMin);
    return hp;
}

bool halTouchIrqActive() {
    return digitalRead(PIN_TCH_IRQ) == LOW;
}

#endif // LCD_MODEL_3_ST7735
