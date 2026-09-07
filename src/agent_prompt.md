# AGENT PROMPT — PORT LCD TESTER TO WAVESHARE RP2350 TOUCH LCD 2.8

## 1. OBJECTIVE

Port the currently functional LCD tester from the `main` branch to the dedicated Waveshare branch:

`wv-2350-touch2.8`

The goal is to obtain a fully functional LCD tester running on the Waveshare RP2350 Touch LCD 2.8 hardware.

This is a HARDWARE PORT, not a redesign of the tester architecture.

The existing functional tester behavior, UI, test sequence, calibration concept and general application logic from `main` must be preserved as much as reasonably possible.

The Waveshare branch is intentionally independent from `main`.

DO NOT merge this work into `main`.

---

# 2. SOURCE OF TRUTH

The existing `main` branch is the behavioral source of truth.

Use it to determine:

- tester behavior
- page structure
- menus
- test sequence
- LCD test patterns
- RGB test
- drawing test
- raw touch test
- orientation tests
- calibration procedure
- EEPROM calibration storage
- recalibration button behavior
- timing
- user interaction
- error handling
- general application flow

Do NOT redesign these features unless a hardware difference makes the original implementation impossible.

The task is primarily to replace the hardware-specific LCD and touchscreen implementation.

---

# 3. TARGET HARDWARE

Target:

Waveshare RP2350 Touch LCD 2.8"

MCU:

RP2350

Framework:

Arduino

Arduino core:

Earle Philhower

Display:

ST7789T3

Resolution:

320 x 240

Touch controller:

CST328

Touch interface:

I2C

The target hardware is NOT an XPT2046 resistive touchscreen.

---

# 4. FIXED WAVESHARE PIN MAP

These pins are authoritative.

Do NOT invent alternative GPIO assignments.

Do NOT change these pins unless explicitly instructed.

## LCD — ST7789T3

TFT_CS  = GP13
TFT_DC  = GP14
TFT_RST = GP15
TFT_BL  = GP16

TFT_MOSI = GP11
TFT_MISO = GP12
TFT_SCLK = GP10

LCD interface:

SPI1

## TOUCH — CST328

TOUCH_SDA = GP6
TOUCH_SCL = GP7

TOUCH_RST = GP17
TOUCH_INT = GP18

Touch interface:

I2C

IMPORTANT:

GP16 is the LCD backlight.

GP16 MUST NOT be used as touch reset or for any other function.

GP6/GP7 are the I2C bus used by the CST328.

Do not introduce XPT2046 pins such as TOUCH_CS, TOUCH_MISO or TOUCH_IRQ.

The CST328 does not use the XPT2046 SPI interface.

---

# 5. CURRENT BRANCH MUST BE INSPECTED FIRST

Before modifying files:

1. Inspect the current `wv-2350-touch2.8` branch.
2. Inspect `platformio.ini`.
3. Inspect `src/main.cpp`.
4. Inspect all relevant headers/source files.
5. Inspect `hardware_map.md`.
6. Compare the branch with `main`.
7. Identify all remaining Marble/XPT2046/ILI9341-specific code.

Do not immediately rewrite the whole project.

First understand what is already present.

---

# 6. PLATFORMIO

The Waveshare branch must remain a PlatformIO project.

Use the existing RP2350/Pico 2 target and Earle Philhower Arduino core unless a concrete compilation issue requires a correction.

The build must use:

- C++17
- warnings enabled
- Arduino framework
- RP2350 board definition already used by the branch

Keep the configuration simple.

Do not introduce unnecessary frameworks.

Do not migrate the project to ESP-IDF.

Do not introduce a new build system.

---

# 7. LIBRARIES

The target needs:

- Adafruit GFX
- appropriate ST7789/ST77xx display support
- CST328 touch support

The XPT2046 library is NOT required for the Waveshare branch.

Remove XPT2046-specific dependencies and code from the Waveshare implementation.

Do not keep dead XPT2046 code merely because it existed in `main`.

However:

DO NOT modify the functional `main` branch.

Only the Waveshare branch is being ported.

---

# 8. LCD PORT

Replace the Marble/ILI9341 display implementation with the Waveshare ST7789T3 implementation.

The display must use:

SPI1

with:

MOSI = GP11
MISO = GP12
SCLK = GP10
CS  = GP13
DC  = GP14
RST = GP15
BL  = GP16

The exact display library/API may be adapted to what the Waveshare hardware and installed Arduino libraries require.

Do not force the ILI9341 driver onto the ST7789T3.

Do not keep incorrect Marble pin definitions.

Do not silently invert byte order or color order unless required by the actual ST7789T3 configuration.

The original tester already has known-working visual test patterns.

Use those patterns to verify that:

- red is red
- green is green
- blue is blue
- white is white
- black is black
- gradients/patterns are correct
- orientation is correct
- no unexpected byte-order inversion has been introduced

---

# 9. BACKLIGHT

Use:

TFT_BL = GP16

The backlight must be initialized correctly.

Do not reuse GP16 for any other purpose.

If the existing tester simply turns the backlight on permanently, preserve that behavior unless there is a hardware reason not to.

Do not introduce PWM/backlight control unless necessary.

---

# 10. TOUCH PORT

The current Marble tester uses XPT2046.

The Waveshare hardware uses CST328.

Therefore the touch implementation must be ported.

Remove/replace code such as:

- XPT2046_Touchscreen
- TOUCH_CS
- TOUCH_MISO
- TOUCH_IRQ as an XPT2046 SPI interface
- touch.begin(SPI1)
- touch.getPoint()
- TS_Point
- XPT2046 rotation functions

Use CST328 through I2C.

I2C:

SDA = GP6
SCL = GP7

Touch reset:

GP17

Touch interrupt:

GP18

Use the installed CST328 library if it provides a reliable Arduino interface.

If the library API differs from the XPT2046 API, adapt the tester's internal touch handling rather than trying to emulate XPT2046 hardware.

---

# 11. TOUCH DATA MODEL

The tester internally needs touch coordinates.

Create the minimum adaptation necessary to convert CST328 readings into the coordinate representation expected by the existing tester logic.

Do not redesign the entire application.

The internal application should still be able to work with:

- X
- Y
- touch pressed/released state

Pressure/Z values from XPT2046 are not applicable to CST328.

Do not invent pressure values.

If existing code uses pressure only to determine whether a touch exists, replace that condition with the CST328 touch/point status.

---

# 12. TOUCH COORDINATE SYSTEM

The Waveshare display resolution is:

320 x 240

The CST328 coordinate system may require transformation.

Determine experimentally and/or from the controller/library behavior:

- raw X range
- raw Y range
- X/Y orientation
- possible axis inversion
- rotation relationship between touch and display

Implement the minimum required coordinate transformation so that:

- touching the upper-left corresponds to the upper-left
- touching the upper-right corresponds to the upper-right
- touching the lower-left corresponds to the lower-left
- touching the lower-right corresponds to the lower-right

Do not guess the final transformation.

Use the raw touch test and calibration screen to validate it.

---

# 13. CALIBRATION

Preserve the tester's existing calibration concept.

The calibration must remain a real touchscreen calibration, not a hardcoded coordinate mapping.

The tester should be able to determine the relationship between physical touch coordinates and display coordinates.

Preserve EEPROM storage of calibration data if that is part of the existing tester behavior.

The calibration flow must remain usable on the 320x240 Waveshare display.

Adapt only:

- coordinate ranges
- axis direction
- rotation
- touch API

Do not redesign the calibration algorithm unless required.

---

# 14. RECALIBRATION BUTTON

The existing tester has a hardware recalibration button.

Preserve the functionality.

For the Waveshare branch, verify the correct GPIO before using it.

Do not blindly copy a Marble-only GPIO assignment if that pin has a different function on the Waveshare board.

If the existing branch already defines the correct hardware button, use it.

If the branch does not define a valid Waveshare recalibration button, do not invent one.

Instead clearly report that the hardware button mapping requires a decision.

---

# 15. EXISTING TESTER FEATURES TO PRESERVE

The Waveshare version should retain the functional behavior of the original tester, including as applicable:

- startup
- information page
- drawing test
- RGB test
- raw touch test
- X orientation test
- Y orientation test
- calibration
- EEPROM calibration storage
- navigation
- test timing
- touch interaction
- visual test patterns

Do not remove working functionality just to make the port easier.

---

# 16. DO NOT IMPLEMENT THESE FEATURES NOW

This is extremely important.

DO NOT implement:

- generic HAL
- display abstraction layer
- touchscreen abstraction layer
- multiple LCD profiles
- runtime display selection
- automatic hardware detection
- Marble/Waveshare unified driver architecture
- SD card support
- SD configuration
- firmware/configuration files on SD
- Service Box integration
- Service Box UI
- generalized hardware configuration system
- automatic calibration profile selection
- dynamic board detection
- unnecessary refactoring
- architectural redesign

These are future tasks.

The current task is ONLY:

FUNCTIONAL WAVESHARE LCD TESTER.

Keep the implementation simple.

---

# 17. DO NOT MODIFY MAIN

The `main` branch is the stable Marble tester.

Do not:

- merge Waveshare code into main
- change main because of the Waveshare port
- refactor main
- rename main functionality
- introduce Waveshare dependencies into main

All work for this task must remain on:

`wv-2350-touch2.8`

---

# 18. CODE QUALITY

Do not use hacks to bypass compilation errors.

Do not:

- comment out large sections just to obtain a build
- add fake functions
- create dummy touch coordinates
- suppress compiler errors
- disable warnings
- ignore initialization failures
- add arbitrary delays as a substitute for understanding the hardware
- invent GPIO assignments
- leave incompatible driver code active

Prefer small, understandable changes.

If a library API is incompatible, adapt the code properly.

---

# 19. STATIC CODE CHECK BEFORE BUILD

Before compiling, search the Waveshare branch for remnants of the Marble/XPT2046 implementation.

Search for:

XPT2046
XPT2046_Touchscreen
TOUCH_CS
TOUCH_MISO
TS_Point
touch.getPoint
touch.begin(SPI1)
ILI9341
TFT_DC 6
TFT_RST 14
TFT_LED 4

Any remaining occurrence must be evaluated.

Some symbols may legitimately remain only if they are unrelated, but hardware-specific remnants must not survive accidentally.

Also verify:

TFT_CS  = 13
TFT_DC  = 14
TFT_RST = 15
TFT_BL  = 16
TFT_MOSI = 11
TFT_MISO = 12
TFT_SCLK = 10

and:

TOUCH_SDA = 6
TOUCH_SCL = 7
TOUCH_RST = 17
TOUCH_INT = 18

---

# 20. BUILD VALIDATION

Run a complete PlatformIO build.

The project must compile without errors.

Do not consider the task complete merely because a library was accepted by PlatformIO.

Also check compiler warnings.

Warnings related to:

- wrong types
- unused variables
- incompatible APIs
- implicit conversions
- invalid initialization
- dead hardware code

must be investigated.

Do not simply suppress them.

---

# 21. HARDWARE VALIDATION — LCD

After a successful build and flash to the Waveshare board:

Verify:

1. Display initializes.
2. Backlight works.
3. Resolution is correct.
4. Orientation is correct.
5. Black screen is correct.
6. White screen is correct.
7. Red is correct.
8. Green is correct.
9. Blue is correct.
10. Existing RGB test works.
11. Existing drawing test works.
12. No visible corruption occurs.
13. No byte-order/color-order regression occurs.

If the display is wrong, investigate driver configuration before changing application logic.

---

# 22. HARDWARE VALIDATION — TOUCH

Verify:

1. CST328 initializes.
2. Touch is detected.
3. Touch release is detected.
4. Coordinates change when touching different areas.
5. Raw coordinates are stable.
6. Upper-left maps correctly.
7. Upper-right maps correctly.
8. Lower-left maps correctly.
9. Lower-right maps correctly.
10. Drawing follows the finger.
11. Menu buttons respond correctly.

Use the raw touch page to debug the coordinate system.

---

# 23. HARDWARE VALIDATION — CALIBRATION

Run the complete calibration process.

Verify:

- all calibration targets can be touched
- coordinates are captured correctly
- calibration values are stored
- calibration survives reboot
- calibrated touch corresponds correctly to display coordinates
- no axis is reversed
- no axis is swapped
- no large offset exists
- calibration does not require arbitrary manual constants

If calibration is wrong, determine whether the problem is:

- CST328 raw coordinates
- axis orientation
- coordinate scaling
- display rotation
- calibration storage
- touch API usage

Fix the correct layer.

---

# 24. DEBUGGING PRIORITY

When something fails, debug in this order:

### LCD problems

1. Pin map
2. SPI1 selection
3. ST7789T3 driver
4. CS/DC/RST
5. backlight
6. display rotation
7. color order
8. application code

### Touch problems

1. I2C SDA/SCL
2. CST328 initialization
3. TOUCH_RST
4. TOUCH_INT
5. touch controller detection
6. raw coordinates
7. axis orientation
8. coordinate scaling
9. calibration
10. application integration

Do not change multiple unrelated things at once.

---

# 25. GIT WORKFLOW

Work ONLY on:

`wv-2350-touch2.8`

Do not merge into:

`main`

Keep commits logical.

Recommended progression:

1. Clean Waveshare hardware definitions.
2. Correct PlatformIO dependencies.
3. Port LCD driver.
4. Port CST328 touch.
5. Adapt coordinate handling.
6. Adapt calibration.
7. Build validation.
8. Hardware validation.
9. Cleanup.
10. Final commit.

Do not rewrite Git history unless explicitly instructed.

---

# 26. IMPORTANT ARCHITECTURAL RULE

Do not try to solve future architecture problems now.

There will eventually be a need for cleaner separation between:

- Marble hardware
- Waveshare hardware
- LCD driver
- touch driver
- calibration
- possible SD configuration

But that is NOT this task.

For now, the simplest reliable implementation is preferred.

A little duplication is acceptable if it keeps the Waveshare branch clear and independent.

Do not introduce abstraction merely because it may be useful later.

---

# 27. FINAL SUCCESS CRITERIA

The task is complete when:

- `wv-2350-touch2.8` builds successfully
- no XPT2046 dependency remains in the Waveshare implementation
- the LCD uses ST7789T3
- the LCD uses the correct Waveshare SPI1 pins
- the touch uses CST328 over I2C
- the touch uses GP6/GP7
- touch reset uses GP17
- touch interrupt uses GP18
- backlight uses GP16
- the existing tester UI works
- LCD test patterns work
- RGB test works
- drawing test works
- raw touch test works
- calibration works
- calibration values persist as intended
- touchscreen coordinates are correctly mapped
- no accidental Marble pin mapping remains
- no unnecessary architecture has been introduced
- `main` remains untouched

---

# 28. FINAL REPORT

At the end, report clearly:

## Files changed

List every modified file.

## LCD

State:

- driver/library used
- SPI bus
- pin mapping
- rotation
- color configuration

## Touch

State:

- CST328 library/API used
- I2C pins
- reset pin
- interrupt pin
- coordinate transformation

## Calibration

State:

- whether the existing calibration logic was preserved
- what coordinate adaptations were required
- whether EEPROM persistence works

## Build

State:

- PlatformIO build result
- warnings
- remaining warnings, if any

## Hardware test

Explicitly mark each as:

- PASS
- FAIL
- NOT TESTED

for:

- LCD initialization
- RGB
- drawing
- raw touch
- touch coordinates
- calibration
- EEPROM persistence

## Remaining issues

List only real remaining issues.

Do not hide problems.

---

# 29. FINAL INSTRUCTION



Keep the implementation focused, minimal and hardware-specific.

Do not merge it into `main`.

Do not redesign the project.

Do not implement Service Box functionality.

The only objective of this task is:

PORT THE EXISTING FUNCTIONAL LCD TESTER TO THE WAVESHARE RP2350 TOUCH LCD 2.8 AND MAKE IT WORK.