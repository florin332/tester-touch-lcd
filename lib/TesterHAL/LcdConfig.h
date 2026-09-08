#pragma once

// ============================================================
// Configurare HAL — Tester Touch LCD
// ------------------------------------------------------------
// Aici se alege modelul de display/touch la COMPILARE.
// Ordinea de rezolvare a modelului:
//   1. -DLCD_MODEL=<id> din build_flags (platformio.ini) — va fi
//      folosit pe viitor pentru selectia externa (butoane + meniu).
//   2. LCD_MODEL_DEFAULT de mai jos — editeaza o singura linie.
//
// Modele disponibile:
//   1 = ILI9341 240x320 SPI, XPT2046      ("Model 1 - Blue Tab")
//   2 = ST7789  240x320 SPI, XPT2046      ("Model 2 - ST7789 240x320")
//   3 = ST7735  128x160 SPI, XPT2046      ("Model 3 - ST7735 128x160")
//   + modele noi: adauga aici un ID si creeaza LcdModel<ID>_<Nume>.cpp
// ============================================================

#include "LcdHal.h"

// Modelul implicit (folosit daca nu exista -DLCD_MODEL in build_flags)
#define LCD_MODEL_DEFAULT LCD_MODEL_1_BLUE_TAB

// Rezolvare model: flag extern (viitor) are prioritate
#if defined(LCD_MODEL)
    #define LCD_MODEL_ACTIVE LCD_MODEL
#else
    #define LCD_MODEL_ACTIVE LCD_MODEL_DEFAULT
#endif
