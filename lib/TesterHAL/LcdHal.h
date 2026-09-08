#pragma once

// ============================================================
// HAL — Tester Touch LCD
// Abstractizare display + touch. Partea de aplicatie (main.cpp)
// foloseste exclusiv acest API; detaliile de controller/rezolutie
// se afla in fisierele LcdModel<ID>_<Nume>.cpp, selectate prin
// LCD_MODEL_ACTIVE din LcdConfig.h (sau -DLCD_MODEL=<id>).
// ============================================================

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>

// ------------------------------------------------------------
// Identificatorii modelelor suportate
// ------------------------------------------------------------
#define LCD_MODEL_1_BLUE_TAB    1   // ILI9341 240x320, XPT2046
#define LCD_MODEL_2_ST7789      2   // ST7789  240x320, XPT2046
#define LCD_MODEL_3_ST7735      3   // ST7735  128x160, XPT2046

// ------------------------------------------------------------
// Structura cu tot ce difera intre modele (pini, rezolutie, etc.)
// instantele display/touch NU sunt aici — sunt definite in .cpp
// ------------------------------------------------------------
struct LcdBoardConfig {
    const char *name;       // ex. "Model 1 - Blue Tab"

    // Rezolutie si orientare
    uint16_t width;         // latime activa (dupa setRotation)
    uint16_t height;        // inaltime activa
    uint8_t  rotation;      // rotatia GFX
    bool     invertColors;  // inversare culori la init

    // Pini display (partajeaza SPI1 cu touch-ul)
    int8_t cs, rst, dc, mosi, sck, miso, led;

    // Pini touch
    int8_t touchCs, touchIrq;

    // Valori brute default de calibrare touch (pana la prima calibrare)
    int32_t defXmin, defXmax, defYmin, defYmax;

    // Praguri presiune touch (Z)
    int zTouchMin, zSampleMin;

    // Identificator unic pentru datele de calibrare din EEPROM:
    // fiecare model are propria calibrare; schimbarea modelului
    // declanseaza automat recalibrarea.
    uint32_t calibMagic;
};

// Configul activ (definit in fisierul .cpp al modelului selectat)
extern const LcdBoardConfig LCD;

// Obiectul grafic — tip de baza Adafruit, functioneaza pentru orice
// controller derivat (ILI9341, ST7789, ST7735, ...)
extern Adafruit_GFX &display;

// ------------------------------------------------------------
// API HAL
// ------------------------------------------------------------
void halDisplayInit();     // pini, SPI1, init display, backlight
void halTouchInit();       // pini + init controller touch

// Selectie/deselectie chip display (izolare pe magistrala comuna)
void halDisplaySelect();
void halDisplayDeselect();

// Citire punct touch, cu izolarea display-ului gestionata intern.
// x, y = valori brute (0..4095), z = presiune, pressed = validitate
struct HalTouchPoint {
    int32_t x, y, z;
    bool pressed;
};
HalTouchPoint halTouchRead();

// Starea pinului IRQ al touch-ului (LOW = atins)
bool halTouchIrqActive();
