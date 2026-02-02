# LED Troubleshooting Guide

## Quick Checklist

### 1. Power Issues (Most Common)
- [ ] **External 5V power supply connected to LED strip?**
  - LEDs need separate power, Arduino can't power 209 LEDs
- [ ] **Power supply turned ON?**
- [ ] **GND connection between Arduino and power supply?**
  - This is CRITICAL - Arduino GND and LED strip GND must connect

### 2. Wiring Check
```
LED Strip → Arduino Mega
DIN      → Pin 4
GND      → GND (also connect to power supply GND)
5V       → External Power Supply 5V (NOT Arduino 5V)

Power Supply
5V+      → LED strip 5V
GND      → LED strip GND AND Arduino GND
```

### 3. Test with Simple Sketch

Upload `led_test_simple.ino` to verify hardware:
1. This lights first 10 LEDs in red
2. Then blinks first LED red/blue
3. If this works, hardware is good

### 4. Check Serial Monitor

Open Serial Monitor (115200 baud) with main sketch running:
- Should see "LEDs initialized"
- Should see mode changes
- Look for any error messages

### 5. Common Issues & Fixes

| Problem | Cause | Solution |
|---------|-------|----------|
| No LEDs light | No power | Connect/turn on power supply |
| No LEDs light | Bad GND | Connect Arduino GND to PS GND |
| No LEDs light | Wrong pin | Verify pin 4 connection |
| First LED flickers | Data issue | Check pin 4 wire |
| Random colors | Protocol wrong | Try WS2811 instead of WS2812B |
| Only first few work | Power drop | Add power injection |

### 6. Debug Steps

**Step 1**: Test with `led_test_simple.ino`
- Simplest possible test
- Should see red LEDs

**Step 2**: Check brightness setting
```cpp
// In Config.h, temporarily increase:
#define DEFAULT_BRIGHTNESS 50  // Was 20
```

**Step 3**: Add debug output to main sketch
In `kaleidoscope.ino` after `FastLED.show()`:
```cpp
static unsigned long lastDebug = 0;
if (millis() - lastDebug > 1000) {
  Serial.print("Mode: ");
  Serial.print(currentMode);
  Serial.print(" | First LED: ");
  Serial.print(leds[0].r);
  Serial.print(",");
  Serial.print(leds[0].g);
  Serial.print(",");
  Serial.println(leds[0].b);
  lastDebug = millis();
}
```

### 7. Power Calculation

For 209 WS2818 LEDs:
- **Minimum needed**: 5V, 3A (assuming not full white)
- **Recommended**: 5V, 10A power supply
- **Arduino alone**: Can only power ~10 LEDs safely

### 8. Test Individual Components

**Test Power Supply**:
- Measure voltage with multimeter (should be 5V)
- Check current capability (printed on label)

**Test First LED**:
- Connect just first LED segment
- Use only Arduino 5V (for testing 1 LED only)

**Test Data Line**:
- Check continuity from Pin 4 to LED DIN
- Look for loose connections

### 9. Alternative Test

```cpp
// Super simple test - just first LED
void setup() {
  FastLED.addLeds<WS2812B, 4, GRB>(leds, 1);
  FastLED.setBrightness(50);
  leds[0] = CRGB::Red;
  FastLED.show();
}
void loop() {}
```

### 10. If Nothing Works

Try different protocol:
```cpp
// Instead of:
FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

// Try:
FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
// or
FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
```

## Most Likely Issue

**80% of "LEDs won't light" problems are power-related:**
1. No external power supply
2. Power supply not turned on
3. Missing GND connection between Arduino and power supply

Check these three things first!