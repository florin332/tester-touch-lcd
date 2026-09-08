// ============================================================
// HAL — functiile comune, independente de model.
// Detaliile per model (instante, pinmap, LCD) sunt in
// LcdModel<ID>_<Nume>.cpp; aici avem doar selectia modelului.
// ============================================================

#include "LcdConfig.h"

// ------------------------------------------------------------
// API comun (implementat cu primitivele din fisierele de model)
// ------------------------------------------------------------
void halDisplaySelect()   { digitalWrite(LCD.cs, LOW); }
void halDisplayDeselect() { digitalWrite(LCD.cs, HIGH); }

// ------------------------------------------------------------
// Selectia modelului la compilare
// ------------------------------------------------------------
#if LCD_MODEL_ACTIVE == LCD_MODEL_1_BLUE_TAB
    // LcdModel1_BlueTab.cpp
#elif LCD_MODEL_ACTIVE == LCD_MODEL_2_ST7789
    #include "LcdModel2_ST7789.cpp"
#elif LCD_MODEL_ACTIVE == LCD_MODEL_3_ST7735
    #include "LcdModel3_ST7735.cpp"
#else
    #error "LCD_MODEL necunoscut! Vezi LcdConfig.h pentru lista modelelor."
#endif
