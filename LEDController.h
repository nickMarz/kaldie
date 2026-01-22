#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <FastLED.h>
#include "Config.h"

// Segment definition
struct Segment {
  uint16_t start;   // Starting LED index
  uint16_t end;     // Ending LED index
  uint16_t length;  // Number of LEDs in segment
  bool reversed;    // Whether segment is reversed in physical layout
};

class LEDController {
public:
  LEDController();

  void begin();
  void setBrightness(uint8_t brightness);
  void clear();
  void show();

  // Direct LED access
  void setPixel(uint16_t index, CRGB color);
  void setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
  CRGB getPixel(uint16_t index) const;

  // Segment access
  void setSegmentPixel(uint8_t segment, uint16_t position, CRGB color);
  Segment getSegment(uint8_t segment) const;

  // Fill operations
  void fill(CRGB color);
  void fillSegment(uint8_t segment, CRGB color);
  void fillRange(uint16_t start, uint16_t end, CRGB color);

  // Gradient operations
  void fillGradient(CRGB startColor, CRGB endColor);
  void fillGradientSegment(uint8_t segment, CRGB startColor, CRGB endColor);

  // Utility functions
  uint16_t numLeds() const { return NUM_LEDS; }
  uint8_t numSegments() const { return NUM_SEGMENTS; }

  // Color utilities
  static CRGB wheel(uint8_t pos);
  static CRGB heatColor(uint8_t temperature);

private:
  CRGB leds[NUM_LEDS];
  Segment segments[NUM_SEGMENTS];

  void initializeSegments();
  uint16_t mapSegmentPosition(uint8_t segment, uint16_t position) const;
};

#endif
