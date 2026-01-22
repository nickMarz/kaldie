#include "Animations.h"

Animations::Animations(LEDController& ledController)
  : leds(ledController) {
  // Initialize heat array for fire effect
  memset(heat, 0, sizeof(heat));
}

// Rainbow effect across all LEDs
void Animations::rainbow(uint8_t offset, float speed) {
  for (uint16_t i = 0; i < leds.numLeds(); i++) {
    uint8_t hue = (i * 256 / leds.numLeds() + offset) % 256;
    leds.setPixel(i, CHSV(hue, 255, 255));
  }
}

// Rainbow effect with each segment having a different color
void Animations::rainbowSegmented(uint8_t offset, float speed) {
  for (uint8_t seg = 0; seg < leds.numSegments(); seg++) {
    uint8_t hue = (seg * 85 + offset) % 256; // 85 = 256/3 for even distribution
    leds.fillSegment(seg, CHSV(hue, 255, 255));
  }
}

// Sparkle effect
void Animations::sparkle(CRGB baseColor, CRGB sparkleColor, float density) {
  // Fill with base color
  leds.fill(baseColor);

  // Add random sparkles
  uint16_t numSparkles = leds.numLeds() * density;
  for (uint16_t i = 0; i < numSparkles; i++) {
    uint16_t pos = random(leds.numLeds());
    leds.setPixel(pos, sparkleColor);
  }
}

// Twinkling stars effect
void Animations::twinkle(CRGB color, float speed, float density) {
  // Fade all pixels
  fadeToBlackBy(20 * speed);

  // Add new twinkles
  if (random(100) < density * 100) {
    uint16_t pos = random(leds.numLeds());
    leds.setPixel(pos, color);
  }
}

// Chase effect
void Animations::chase(CRGB color, uint8_t chaseSize, uint16_t position) {
  leds.clear();

  for (uint8_t i = 0; i < chaseSize; i++) {
    uint16_t pos = (position + i) % leds.numLeds();
    leds.setPixel(pos, color);
  }
}

// Wave effect
void Animations::wave(uint8_t hue, uint8_t waveWidth, float position) {
  for (uint16_t i = 0; i < leds.numLeds(); i++) {
    float wave = sin((i + position * 10) * 2 * PI / waveWidth);
    uint8_t brightness = (wave + 1.0) * 127.5; // Map -1..1 to 0..255
    leds.setPixel(i, CHSV(hue, 255, brightness));
  }
}

// Three-color gradient that can be animated
void Animations::gradient(CRGB color1, CRGB color2, CRGB color3, float position) {
  uint16_t numLeds = leds.numLeds();

  for (uint16_t i = 0; i < numLeds; i++) {
    float adjustedPos = fmod(i + position * numLeds, numLeds);
    float t = adjustedPos / numLeds;

    CRGB color;
    if (t < 0.33) {
      color = lerpColor(color1, color2, t * 3.0);
    } else if (t < 0.66) {
      color = lerpColor(color2, color3, (t - 0.33) * 3.0);
    } else {
      color = lerpColor(color3, color1, (t - 0.66) * 3.0);
    }

    leds.setPixel(i, color);
  }
}

// Fire effect
void Animations::fire(uint8_t cooling, uint8_t sparking) {
  // Cool down every cell a little
  for (uint16_t i = 0; i < leds.numLeds(); i++) {
    uint8_t cooldown = random(0, ((cooling * 10) / leds.numLeds()) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Heat from each cell drifts up and diffuses
  for (uint16_t k = leds.numLeds() - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Randomly ignite new sparks near the bottom
  if (random(255) < sparking) {
    uint8_t y = random(7);
    heat[y] = heat[y] + random(160, 255);
  }

  // Convert heat to LED colors
  for (uint16_t j = 0; j < leds.numLeds(); j++) {
    leds.setPixel(j, LEDController::heatColor(heat[j]));
  }
}

// Pulse effect
void Animations::pulse(CRGB color, float phase) {
  uint8_t brightness = (sin(phase * 2 * PI) + 1.0) * 127.5;

  CHSV hsvColor = rgb2hsv_approximate(color);
  hsvColor.v = brightness;

  leds.fill(hsvColor);
}

// Strobe effect
void Animations::strobe(CRGB color, uint8_t onFrames, uint8_t offFrames, uint16_t counter) {
  uint16_t cycle = counter % (onFrames + offFrames);

  if (cycle < onFrames) {
    leds.fill(color);
  } else {
    leds.clear();
  }
}

// Meteor effect
void Animations::meteor(CRGB color, uint8_t meteorSize, uint8_t trailDecay, uint16_t position) {
  // Fade existing pixels
  fadeToBlackBy(trailDecay);

  // Draw meteor
  for (uint8_t i = 0; i < meteorSize; i++) {
    uint16_t pos = (position + i) % leds.numLeds();
    leds.setPixel(pos, color);
  }
}

// Perlin noise fill
void Animations::fillNoise(uint8_t hue, uint8_t scale, uint16_t offset) {
  for (uint16_t i = 0; i < leds.numLeds(); i++) {
    uint8_t noise = inoise8(i * scale, offset);
    uint8_t brightness = noise;
    leds.setPixel(i, CHSV(hue + (noise >> 2), 255, brightness));
  }
}

// === MOTION-REACTIVE ANIMATIONS ===

// Rainbow that changes speed and segments based on motion
void Animations::motionRainbow(const MotionData& motion, unsigned long time) {
  // Rotation changes speed, tilt changes whether it's segmented or continuous
  float speed = 1.5 + motion.rotationNormalized * 8.0;  // Faster and more responsive
  uint8_t offset = (uint32_t)(time / (30 / speed)) % 256;  // Smoother with higher FPS

  if (motion.tiltNormalized > 0.3) {
    rainbowSegmented(offset, speed);
  } else {
    rainbow(offset, speed);
  }
}

// Sparkles that react to shake and rotation
void Animations::motionSparkle(const MotionData& motion) {
  // Shake controls density, tilt controls color
  float density = 0.05 + motion.shakeNormalized * 0.3;
  uint8_t hue = motion.tiltAngle * 2; // 0-180 degrees maps to 0-255 hue

  CRGB baseColor = CHSV(hue, 200, 50);
  CRGB sparkleColor = CHSV(hue, 255, 255);

  sparkle(baseColor, sparkleColor, density);
}

// Wave that changes based on tilt and rotation
void Animations::motionWave(const MotionData& motion, unsigned long time) {
  // Tilt angle controls hue, rotation controls wave speed
  uint8_t hue = map(motion.pitch + 90, 0, 180, 0, 255); // Map -90 to 90 degrees to hue
  float speed = 1.0 + motion.rotationNormalized * 3.0;
  uint8_t waveWidth = 30 - motion.tiltNormalized * 20; // Narrower waves with more tilt

  float position = (time / (100.0 / speed));
  wave(hue, waveWidth, position);
}

// Fire effect that reacts to shake (intensity) and tilt (color)
void Animations::motionFire(const MotionData& motion) {
  // Shake increases sparking, tilt affects cooling
  uint8_t cooling = 55 + motion.tiltNormalized * 30;
  uint8_t sparking = 100 + motion.shakeNormalized * 100;

  fire(cooling, sparking);

  // Shift hue based on rotation
  if (motion.rotationNormalized > 0.1) {
    uint8_t hueShift = motion.rotationSpeed / 2;
    for (uint16_t i = 0; i < leds.numLeds(); i++) {
      CRGB color = leds.getPixel(i);
      CHSV hsv = rgb2hsv_approximate(color);
      hsv.h += hueShift;
      leds.setPixel(i, hsv);
    }
  }
}

// Pulse that reacts to all motion types
void Animations::motionPulse(const MotionData& motion, unsigned long time) {
  // Rotation controls pulse speed, tilt controls color, shake controls brightness
  float speed = 1.0 + motion.rotationNormalized * 3.0;
  uint8_t hue = motion.tiltAngle * 2;
  uint8_t baseBrightness = 100 + motion.shakeNormalized * 155;

  float phase = (time / 1000.0) * speed;
  uint8_t brightness = (sin(phase * 2 * PI) + 1.0) * 127.5;
  brightness = map(brightness, 0, 255, baseBrightness / 2, baseBrightness);

  leds.fill(CHSV(hue, 255, brightness));
}

// Kaleidoscope effect - unique pattern for this project
void Animations::motionKaleidoscope(const MotionData& motion, unsigned long time) {
  // Each segment mirrors/relates to the others, creating kaleidoscope effect
  float speed = 1.5 + motion.rotationNormalized * 4.0;  // More responsive
  uint8_t hueBase = (uint32_t)(time / (20 / speed)) % 256;  // Faster color cycling

  // Add tilt-based hue shift
  hueBase += motion.tiltAngle;

  for (uint8_t seg = 0; seg < leds.numSegments(); seg++) {
    Segment segment = leds.getSegment(seg);
    uint8_t hueOffset = seg * 85; // 120 degree hue offset for each segment

    for (uint16_t pos = 0; pos < segment.length; pos++) {
      // Create symmetrical pattern
      float normalizedPos = (float)pos / segment.length;

      // Wave pattern based on position
      float wave1 = sin(normalizedPos * 4 * PI + time / 200.0);
      float wave2 = sin(normalizedPos * 2 * PI - time / 300.0);

      // Brightness based on waves and shake
      uint8_t brightness = ((wave1 + wave2) / 2.0 + 1.0) * 127.5;
      brightness = brightness * (0.5 + motion.shakeNormalized * 0.5);

      // Hue based on position, segment, and motion
      uint8_t hue = hueBase + hueOffset + (normalizedPos * 60);

      leds.setSegmentPixel(seg, pos, CHSV(hue, 255, brightness));
    }
  }
}

// === UTILITY FUNCTIONS ===

void Animations::fadeToBlackBy(uint8_t fadeAmount) {
  for (uint16_t i = 0; i < leds.numLeds(); i++) {
    leds.setPixel(i, leds.getPixel(i).fadeToBlackBy(fadeAmount));
  }
}

void Animations::blur(uint8_t blurAmount) {
  // Manual blur implementation since we don't have direct access to leds array
  // This creates a simple averaging blur effect
  uint16_t numLeds = leds.numLeds();
  if (numLeds < 2) return;

  // Store first LED color for wrap-around
  CRGB firstLed = leds.getPixel(0);
  CRGB prev = firstLed;

  for (uint16_t i = 0; i < numLeds - 1; i++) {
    CRGB cur = leds.getPixel(i);
    CRGB next = leds.getPixel(i + 1);

    // Average with neighbors
    leds.setPixel(i, CRGB(
      (prev.r + cur.r + next.r) / 3,
      (prev.g + cur.g + next.g) / 3,
      (prev.b + cur.b + next.b) / 3
    ).nscale8(255 - blurAmount / 2));

    prev = cur;
  }

  // Handle last LED
  CRGB lastLed = leds.getPixel(numLeds - 1);
  leds.setPixel(numLeds - 1, CRGB(
    (prev.r + lastLed.r + firstLed.r) / 3,
    (prev.g + lastLed.g + firstLed.g) / 3,
    (prev.b + lastLed.b + firstLed.b) / 3
  ).nscale8(255 - blurAmount / 2));
}

CRGB Animations::lerpColor(CRGB a, CRGB b, float t) {
  t = constrain(t, 0.0, 1.0);
  return CRGB(
    a.r + (b.r - a.r) * t,
    a.g + (b.g - a.g) * t,
    a.b + (b.b - a.b) * t
  );
}

uint8_t Animations::beatSin8(uint16_t bpm, uint8_t lowest, uint8_t highest, unsigned long timebase, uint16_t phaseOffset) {
  uint16_t beat = (timebase * bpm) / 60;
  uint8_t sinValue = sin8(beat + phaseOffset);
  return map(sinValue, 0, 255, lowest, highest);
}
