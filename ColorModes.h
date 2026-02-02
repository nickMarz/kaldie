#ifndef COLOR_MODES_H
#define COLOR_MODES_H

#include <FastLED.h>

enum ColorMode {
  COLOR_FULL_SPECTRUM,  // All colors (normal operation)
  COLOR_WARM,          // Reds, oranges, yellows (cozy)
  COLOR_COOL,          // Blues, greens, purples (calm)
  COLOR_FIRE,          // Fire colors (dramatic)
  COLOR_OCEAN,         // Ocean blues and greens (serene)
  COLOR_PARTY,         // Random vibrant colors (fun)
  COLOR_MONOCHROME,    // Single color with brightness variations
  COLOR_MODE_COUNT     // Total number of modes
};

class ColorModeManager {
public:
  ColorModeManager() : currentMode(COLOR_FULL_SPECTRUM), modeHue(0) {}

  // Apply color mode to a base hue
  CRGB applyColorMode(uint8_t baseHue, uint8_t sat = 255, uint8_t val = 255) {
    switch(currentMode) {
      case COLOR_FULL_SPECTRUM:
        return CHSV(baseHue, sat, val);

      case COLOR_WARM:
        // Map to warm colors (red to yellow: 0-60)
        return CHSV(map(baseHue, 0, 255, 0, 60), sat, val);

      case COLOR_COOL:
        // Map to cool colors (cyan to purple: 120-280)
        return CHSV(map(baseHue, 0, 255, 120, 280) % 256, sat, val);

      case COLOR_FIRE:
        // Fire palette: black -> red -> orange -> yellow
        if (baseHue < 85) return CRGB::Black;
        else if (baseHue < 170) return CRGB(val, 0, 0);  // Red
        else if (baseHue < 213) return CRGB(val, val/4, 0);  // Orange
        else return CRGB(val, val/2, 0);  // Yellow

      case COLOR_OCEAN:
        // Ocean colors: deep blue to turquoise
        return CHSV(map(baseHue, 0, 255, 140, 180), sat, val);

      case COLOR_PARTY:
        // Random saturated colors
        return CHSV(random8(), 255, val);

      case COLOR_MONOCHROME:
        // Single hue with brightness variations
        return CHSV(modeHue, sat, baseHue);  // Use baseHue as brightness

      default:
        return CHSV(baseHue, sat, val);
    }
  }

  // Toggle to next color mode
  void nextMode() {
    currentMode = (ColorMode)((currentMode + 1) % COLOR_MODE_COUNT);
    if (currentMode == COLOR_MONOCHROME) {
      modeHue = random8();  // Pick random hue for monochrome
    }
    showModePreview();
  }

  // Get current mode name
  const char* getModeName() {
    switch(currentMode) {
      case COLOR_FULL_SPECTRUM: return "Full Spectrum";
      case COLOR_WARM: return "Warm Colors";
      case COLOR_COOL: return "Cool Colors";
      case COLOR_FIRE: return "Fire";
      case COLOR_OCEAN: return "Ocean";
      case COLOR_PARTY: return "Party";
      case COLOR_MONOCHROME: return "Monochrome";
      default: return "Unknown";
    }
  }

  // Flash preview of color mode
  void showModePreview() {
    // This will be called to show a brief preview
    // Implementation depends on your LED controller
    Serial.print("Color Mode: ");
    Serial.println(getModeName());
  }

  ColorMode getCurrentMode() { return currentMode; }
  void setMode(ColorMode mode) { currentMode = mode; }

private:
  ColorMode currentMode;
  uint8_t modeHue;  // For monochrome mode
};

#endif // COLOR_MODES_H