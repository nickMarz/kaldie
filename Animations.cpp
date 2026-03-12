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

// Fire effect - runs same direction across all segments (end-to-end)
void Animations::fire(uint8_t cooling, uint8_t sparking) {
  // Simulate fire per segment so it flows the same visual direction
  // Each segment uses its slice of the heat array
  for (uint8_t seg = 0; seg < leds.numSegments(); seg++) {
    Segment segment = leds.getSegment(seg);
    uint16_t offset = segment.start;
    uint16_t len = segment.length;

    // Cool down every cell
    for (uint16_t i = 0; i < len; i++) {
      uint8_t cooldown = random(0, ((cooling * 10) / len) + 2);
      if (cooldown > heat[offset + i]) {
        heat[offset + i] = 0;
      } else {
        heat[offset + i] -= cooldown;
      }
    }

    // Heat drifts "up" (toward higher segment positions = same physical direction)
    for (uint16_t k = len - 1; k >= 2; k--) {
      heat[offset + k] = (heat[offset + k - 1] + heat[offset + k - 2] + heat[offset + k - 2]) / 3;
    }

    // Sparks ignite near the "bottom" of each segment (position 0 = same end of tube)
    if (random(255) < sparking) {
      uint8_t y = random(min((uint16_t)7, len));
      heat[offset + y] = qadd8(heat[offset + y], random(160, 255));
    }

    // Render using segment-aware addressing (handles reversed segment 1)
    for (uint16_t j = 0; j < len; j++) {
      leds.setSegmentPixel(seg, j, LEDController::heatColor(heat[offset + j]));
    }
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
  // Encoder 2 boosts speed
  float tiltEffect = motion.tiltNormalized * 10.0;
  float panEffect = motion.panNormalized * 5.0;
  float speed = 1.0 + motion.rotationNormalized * 5.0 + tiltEffect + panEffect;

  uint8_t offset = (uint32_t)(time / (30 / speed)) % 256;

  // Pan direction changes color offset
  offset += (int)(motion.pan * 50);  // Pan shifts the rainbow

  if (motion.tiltNormalized > 0.3) {
    rainbowSegmented(offset, speed);
  } else {
    rainbow(offset, speed);
  }
}

// Sparkles that react to shake and rotation
void Animations::motionSparkle(const MotionData& motion) {
  // Much more reactive to tilt and pan
  float density = 0.1 + motion.rotationNormalized * 0.4 + motion.tiltNormalized * 0.5 + motion.panNormalized * 0.3;

  // Color changes with both tilt AND pan
  uint8_t hue = motion.tiltAngle * 3 + (int)(motion.pan * 50);

  CRGB baseColor = CHSV(hue, 200, 150);
  CRGB sparkleColor = CHSV(hue + 30, 255, 255);  // Sparkles offset in hue

  sparkle(baseColor, sparkleColor, density);
}

// Wave that changes based on tilt and rotation
void Animations::motionWave(const MotionData& motion, unsigned long time) {
  // Much more reactive to tilt and pan
  uint8_t hue = map(motion.pitch + 90, 0, 180, 0, 255) + (int)(motion.pan * 50);

  // Use encoder2 virtual rotation AND motion for speed
  float speed = 1.0 + motion.rotationNormalized * 4.0 + motion.tiltNormalized * 5.0 + motion.panNormalized * 3.0;

  // Encoder 2 also tightens wave width
  uint8_t waveWidth = 50 - motion.rotationNormalized * 30 - motion.tiltNormalized * 15;

  float position = (time / (100.0 / speed));
  wave(hue, waveWidth, position);
}

// Fire effect that reacts to shake (intensity) and tilt (color)
// Encoder 2: controls fire intensity and color temperature
//   0.0 = smoldering embers (high cooling, low sparking, deep red)
//   0.5 = normal fire (orange/yellow)
//   1.0 = inferno (low cooling, max sparking, white-hot with blue tips)
void Animations::motionFire(const MotionData& motion) {
  float enc2 = motion.rotationNormalized;

  // Encoder 2 controls the fire regime
  // Low enc2 = more cooling, fewer sparks (dying fire)
  // High enc2 = less cooling, more sparks (raging inferno)
  uint8_t cooling = 60 - enc2 * 40 + motion.tiltNormalized * 30;
  uint8_t sparking = 40 + enc2 * 160 + motion.tiltNormalized * 50 + motion.panNormalized * 30;

  fire(cooling, sparking);

  // Encoder 2 shifts fire color temperature
  // 0 = deep red/orange (normal fire)
  // 0.5 = yellow/white hot
  // 1.0 = blue/purple inferno
  uint8_t hueShift = enc2 * 160;  // 0=red, ~80=blue, ~160=purple

  // Pan also shifts color
  hueShift += motion.panNormalized * 40;

  // Apply color shift and brightness boost from encoder
  for (uint16_t i = 0; i < leds.numLeds(); i++) {
    CRGB color = leds.getPixel(i);
    if (color.r > 0 || color.g > 0 || color.b > 0) {
      CHSV hsv = rgb2hsv_approximate(color);
      hsv.h += hueShift;
      // Boost saturation at low enc2 (rich reds), reduce at high (white-hot)
      hsv.s = 255 - enc2 * 100;
      // Boost brightness with encoder
      hsv.v = qadd8(hsv.v, enc2 * 60);
      leds.setPixel(i, hsv);
    }
  }
}

// Pulse that reacts to all motion types
void Animations::motionPulse(const MotionData& motion, unsigned long time) {
  // Much more reactive to tilt and pan
  float speed = 1.0 + motion.rotationNormalized * 3.0 + motion.tiltNormalized * 5.0 + motion.panNormalized * 3.0;

  // Color dramatically affected by motion
  uint8_t hue = motion.tiltAngle * 3 + (int)(motion.pan * 100);

  // Brightness range based on tilt
  uint8_t baseBrightness = 150 + motion.tiltNormalized * 105;  // Visible at rest, brighter with motion

  float phase = (time / 1000.0) * speed;
  uint8_t brightness = (sin(phase * 2 * PI) + 1.0) * 127.5;
  brightness = map(brightness, 0, 255, baseBrightness / 4, baseBrightness);

  leds.fill(CHSV(hue, 255, brightness));
}

// Kaleidoscope effect - unique pattern for this project
void Animations::motionKaleidoscope(const MotionData& motion, unsigned long time) {
  // Encoder 2 controls pattern complexity and speed (0 = slow/simple, 1 = fast/complex)
  float enc2 = motion.rotationNormalized;

  float speed = 1.0 + enc2 * 4.0 + motion.tiltNormalized * 8.0 + motion.panNormalized * 5.0;
  uint8_t hueBase = (uint32_t)(time / (20 / speed)) % 256;

  hueBase += motion.tiltAngle * 2 + (int)(motion.pan * 50);

  // Encoder 2 adds extra wave layers for more complex patterns
  float waveFreq1 = 4.0 + enc2 * 8.0;   // 4-12 cycles (more = tighter pattern)
  float waveFreq2 = 2.0 + enc2 * 6.0;   // 2-8 cycles
  float hueSpread = 60 + enc2 * 120;     // 60-180 hue range per segment

  for (uint8_t seg = 0; seg < leds.numSegments(); seg++) {
    Segment segment = leds.getSegment(seg);
    uint8_t hueOffset = seg * 85;

    for (uint16_t pos = 0; pos < segment.length; pos++) {
      float normalizedPos = (float)pos / segment.length;

      float wave1 = sin(normalizedPos * waveFreq1 * PI + time / 200.0);
      float wave2 = sin(normalizedPos * waveFreq2 * PI - time / 300.0);
      // Third wave layer kicks in as encoder 2 increases
      float wave3 = sin(normalizedPos * 6 * PI + time / 150.0) * enc2;

      uint8_t brightness = ((wave1 + wave2 + wave3) / (2.0 + enc2) + 1.0) * 127.5;
      brightness = brightness * (0.7 + motion.tiltNormalized * 0.3 + motion.panNormalized * 0.3);

      uint8_t hue = hueBase + hueOffset + (uint8_t)(normalizedPos * hueSpread);

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
  // Manual blur implementation
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
