#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "LEDController.h"
#include "MotionProcessor.h"
#include <Arduino.h>

// Base animation parameters
struct AnimationParams {
  float speed;        // Animation speed multiplier (0-10)
  float intensity;    // Effect intensity (0-1)
  uint8_t hue;        // Primary hue (0-255)
  uint8_t saturation; // Color saturation (0-255)
  bool reverse;       // Reverse direction
};

class Animations {
public:
  Animations(LEDController& ledController);

  // Animation primitives
  void rainbow(uint8_t offset, float speed = 1.0);
  void rainbowSegmented(uint8_t offset, float speed = 1.0);
  void sparkle(CRGB baseColor, CRGB sparkleColor, float density = 0.1);
  void twinkle(CRGB color, float speed = 1.0, float density = 0.2);
  void chase(CRGB color, uint8_t chaseSize, uint16_t position);
  void wave(uint8_t hue, uint8_t waveWidth, float position);
  void gradient(CRGB color1, CRGB color2, CRGB color3, float position = 0);
  void fire(uint8_t cooling = 55, uint8_t sparking = 120);
  void pulse(CRGB color, float phase);
  void strobe(CRGB color, uint8_t onFrames, uint8_t offFrames, uint16_t counter);
  void meteor(CRGB color, uint8_t meteorSize, uint8_t trailDecay, uint16_t position);
  void fillNoise(uint8_t hue, uint8_t scale, uint16_t offset);

  // Motion-reactive animations
  void motionRainbow(const MotionData& motion, unsigned long time);
  void motionSparkle(const MotionData& motion);
  void motionWave(const MotionData& motion, unsigned long time);
  void motionFire(const MotionData& motion);
  void motionPulse(const MotionData& motion, unsigned long time);
  void motionKaleidoscope(const MotionData& motion, unsigned long time);

  // Utility functions
  void fadeToBlackBy(uint8_t fadeAmount);
  void blur(uint8_t blurAmount);

private:
  LEDController& leds;

  // Fire effect heat array
  byte heat[NUM_LEDS];

  // Helper functions
  CRGB lerpColor(CRGB a, CRGB b, float t);
  uint8_t beatSin8(uint16_t bpm, uint8_t lowest, uint8_t highest, unsigned long timebase, uint16_t phaseOffset);
};

#endif
