#include "LEDController.h"

LEDController::LEDController() {
  initializeSegments();
}

void LEDController::begin() {
  // WS2818 uses WS2812B protocol with GRB color order
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  clear();
  show();
  Serial.println("LED Controller initialized");
}

void LEDController::initializeSegments() {
  // Define the three segments based on the back-and-forth layout
  // For 209 LEDs: 70 + 70 + 69
  // Segment 0: LEDs 0-69 (forward)
  segments[0].start = 0;
  segments[0].end = 69;
  segments[0].length = 70;
  segments[0].reversed = false;

  // Segment 1: LEDs 70-139 (backward - folded back)
  segments[1].start = 70;
  segments[1].end = 139;
  segments[1].length = 70;
  segments[1].reversed = true;

  // Segment 2: LEDs 140-208 (forward again)
  segments[2].start = 140;
  segments[2].end = 208;
  segments[2].length = 69;
  segments[2].reversed = false;
}

void LEDController::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(constrain(brightness, 0, MAX_BRIGHTNESS));
}

void LEDController::clear() {
  fill(CRGB::Black);
}

void LEDController::show() {
  FastLED.show();
}

void LEDController::setPixel(uint16_t index, CRGB color) {
  if (index < NUM_LEDS) {
    leds[index] = color;
  }
}

void LEDController::setPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  setPixel(index, CRGB(r, g, b));
}

CRGB LEDController::getPixel(uint16_t index) const {
  if (index < NUM_LEDS) {
    return leds[index];
  }
  return CRGB::Black;
}

void LEDController::setSegmentPixel(uint8_t segment, uint16_t position, CRGB color) {
  if (segment >= NUM_SEGMENTS) return;

  uint16_t actualIndex = mapSegmentPosition(segment, position);
  setPixel(actualIndex, color);
}

uint16_t LEDController::mapSegmentPosition(uint8_t segment, uint16_t position) const {
  if (segment >= NUM_SEGMENTS) return 0;

  const Segment& seg = segments[segment];

  if (position >= seg.length) {
    position = seg.length - 1;
  }

  // If segment is reversed, map position from the end
  if (seg.reversed) {
    return seg.end - position;
  } else {
    return seg.start + position;
  }
}

Segment LEDController::getSegment(uint8_t segment) const {
  if (segment < NUM_SEGMENTS) {
    return segments[segment];
  }
  return segments[0];
}

void LEDController::fill(CRGB color) {
  fill_solid(leds, NUM_LEDS, color);
}

void LEDController::fillSegment(uint8_t segment, CRGB color) {
  if (segment >= NUM_SEGMENTS) return;

  const Segment& seg = segments[segment];
  fillRange(seg.start, seg.end, color);
}

void LEDController::fillRange(uint16_t start, uint16_t end, CRGB color) {
  if (start >= NUM_LEDS) return;
  if (end >= NUM_LEDS) end = NUM_LEDS - 1;

  for (uint16_t i = start; i <= end; i++) {
    leds[i] = color;
  }
}

void LEDController::fillGradient(CRGB startColor, CRGB endColor) {
  fill_gradient_RGB(leds, 0, startColor, NUM_LEDS - 1, endColor);
}

void LEDController::fillGradientSegment(uint8_t segment, CRGB startColor, CRGB endColor) {
  if (segment >= NUM_SEGMENTS) return;

  const Segment& seg = segments[segment];

  // For reversed segments, swap colors to maintain visual direction
  if (seg.reversed) {
    fill_gradient_RGB(leds, seg.start, endColor, seg.end, startColor);
  } else {
    fill_gradient_RGB(leds, seg.start, startColor, seg.end, endColor);
  }
}

// Color wheel: input 0-255 to get rainbow colors
CRGB LEDController::wheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return CRGB(255 - pos * 3, 0, pos * 3);
  } else if (pos < 170) {
    pos -= 85;
    return CRGB(0, pos * 3, 255 - pos * 3);
  } else {
    pos -= 170;
    return CRGB(pos * 3, 255 - pos * 3, 0);
  }
}

// Heat color: input 0-255 to get fire colors (black -> red -> orange -> yellow -> white)
CRGB LEDController::heatColor(uint8_t temperature) {
  // Scale 'heat' from 0-255 to 0-191 for better color progression
  uint8_t t192 = scale8_video(temperature, 191);

  // Calculate ramp up from black to red
  uint8_t heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  if (t192 > 0x80) {
    // Hottest: yellow to white
    return CRGB(255, 255, heatramp);
  } else if (t192 > 0x40) {
    // Medium: red to yellow
    return CRGB(255, heatramp, 0);
  } else {
    // Coolest: black to red
    return CRGB(heatramp, 0, 0);
  }
}
