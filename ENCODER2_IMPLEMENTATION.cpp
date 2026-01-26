// ENCODER 2 BUTTON - COLOR MODE IMPLEMENTATION EXAMPLE
// This file shows how to integrate the color mode toggle feature

// ============================================
// Step 1: Include ColorModes.h in your main sketch
// ============================================
#include "ColorModes.h"

// Create global color mode manager
ColorModeManager colorModes;

// ============================================
// Step 2: Add button handling to EncoderHandler
// ============================================
// In EncoderHandler.h, add:
class EncoderHandler {
private:
  // Existing encoder variables...

  // Button state tracking
  bool encoder2ButtonState = false;
  bool encoder2ButtonPressed = false;
  unsigned long encoder2ButtonPressTime = 0;

public:
  // Add button check method
  bool getEncoder2ButtonPressed() {
    bool pressed = encoder2ButtonPressed;
    encoder2ButtonPressed = false;  // Clear flag after reading
    return pressed;
  }

  // In update() method, add:
  void update() {
    // Check encoder 2 button
    bool currentButton = digitalRead(ENCODER_2_SW) == LOW;
    if (currentButton && !encoder2ButtonState) {
      // Button just pressed
      encoder2ButtonPressed = true;
      encoder2ButtonPressTime = millis();
    }
    encoder2ButtonState = currentButton;

    // Existing encoder update code...
  }
};

// ============================================
// Step 3: Integrate into main loop
// ============================================
// In kaleidoscope.ino main loop:
void loop() {
  // Existing motion and encoder updates...
  motionProcessor.update();
  encoderHandler.update();

  // Handle encoder 2 button press for color mode
  if (encoderHandler.getEncoder2ButtonPressed()) {
    colorModes.nextMode();

    // Visual feedback - flash the new palette
    showColorModePreview();
  }

  // Run animation with color mode applied
  runCurrentAnimation();

  // Show LEDs
  ledController.show();
}

// ============================================
// Step 4: Apply color modes in animations
// ============================================
// Example: Update motionRainbow animation
void motionRainbow(const MotionData& motion, unsigned long time) {
  // Calculate speed from motion
  float speed = 1.0 + (motion.tiltNormalized * 4.0);
  uint8_t offset = (uint32_t)(time / (30 / speed)) % 256;

  // Apply to each segment
  for (int seg = 0; seg < 3; seg++) {
    for (int i = 0; i < LEDS_PER_SEGMENT; i++) {
      // Calculate base hue
      uint8_t hue = (i * 256 / LEDS_PER_SEGMENT) + offset;

      // APPLY COLOR MODE TRANSFORMATION HERE!
      CRGB color = colorModes.applyColorMode(hue, 255, 255);

      // Set the LED
      ledController.setPixel(seg, i, color);
    }
  }
}

// ============================================
// Step 5: Visual feedback function
// ============================================
void showColorModePreview() {
  // Brief preview of the new color palette
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t hue = i * 256 / NUM_LEDS;
    leds[i] = colorModes.applyColorMode(hue, 255, 255);
  }

  FastLED.show();
  delay(100);  // Show for 100ms

  // Optional: Sweep effect
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(200);
    if (i % 10 == 0) FastLED.show();
  }
}

// ============================================
// Step 6: Update ALL animations similarly
// ============================================
// For each animation function:
// 1. Calculate your base color/hue as normal
// 2. Pass it through colorModes.applyColorMode()
// 3. Use the returned color for the LED

// Example for motionSparkle:
void motionSparkle(const MotionData& motion, unsigned long time) {
  // Existing sparkle logic...

  if (random8() < sparkleChance) {
    int pos = random16(NUM_LEDS);
    uint8_t hue = random8();

    // Apply color mode to sparkle color
    CRGB sparkleColor = colorModes.applyColorMode(hue, 255, 255);
    leds[pos] = sparkleColor;
  }
}

// ============================================
// Step 7: Status display (optional)
// ============================================
void showStatusIndicator() {
  // Use first LED to show current color mode
  ColorMode mode = colorModes.getCurrentMode();
  CRGB indicator;

  switch(mode) {
    case COLOR_FULL_SPECTRUM: indicator = CRGB::White; break;
    case COLOR_WARM: indicator = CRGB::OrangeRed; break;
    case COLOR_COOL: indicator = CRGB::Blue; break;
    case COLOR_FIRE: indicator = CRGB::Red; break;
    case COLOR_OCEAN: indicator = CRGB::Cyan; break;
    case COLOR_PARTY: indicator = CRGB::Magenta; break;
    case COLOR_MONOCHROME: indicator = CRGB::Gray; break;
  }

  leds[0] = indicator;
}

// ============================================
// ALTERNATIVE BUTTON FUNCTIONS
// ============================================

// Option A: Brightness Presets
class BrightnessPresets {
  enum Preset {
    AMBIENT = 20,
    NORMAL = 50,
    BRIGHT = 100,
    DEMO = 150,
    BATTERY_SAVE = 10
  };

  uint8_t presets[5] = {AMBIENT, NORMAL, BRIGHT, DEMO, BATTERY_SAVE};
  uint8_t currentPreset = 1;  // Start with NORMAL

  void nextPreset() {
    currentPreset = (currentPreset + 1) % 5;
    FastLED.setBrightness(presets[currentPreset]);

    // Flash to show new brightness
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(50);
  }
};

// Option B: Effect Speed Control
class SpeedControl {
  float speedMultipliers[5] = {0.25, 0.5, 1.0, 2.0, 4.0};
  uint8_t currentSpeed = 2;  // Start at 1.0x

  void nextSpeed() {
    currentSpeed = (currentSpeed + 1) % 5;
  }

  float getMultiplier() {
    return speedMultipliers[currentSpeed];
  }
};

// Option C: Freeze Frame
class FreezeFrame {
  bool frozen = false;
  CRGB snapshot[NUM_LEDS];

  void toggle() {
    if (!frozen) {
      // Capture current frame
      memcpy(snapshot, leds, sizeof(CRGB) * NUM_LEDS);
      frozen = true;
    } else {
      frozen = false;
    }
  }

  void apply() {
    if (frozen) {
      // Restore frozen frame
      memcpy(leds, snapshot, sizeof(CRGB) * NUM_LEDS);

      // Optional: Add subtle breathing effect
      uint8_t breath = beatsin8(12, 200, 255);
      FastLED.setBrightness(breath);
    }
  }
};

// ============================================
// ADVANCED: Context-Aware Button
// ============================================
void handleEncoder2Press() {
  // Different function based on current animation
  switch(currentMode) {
    case MODE_RAINBOW:
      colorModes.nextMode();  // Change colors
      break;

    case MODE_SPARKLE:
      // Toggle sparkle density
      static uint8_t densities[3] = {10, 50, 100};
      static uint8_t densityIndex = 0;
      densityIndex = (densityIndex + 1) % 3;
      sparkleChance = densities[densityIndex];
      break;

    case MODE_FIRE:
      // Toggle fire intensity
      static bool intensefire = false;
      intensefire = !intensefire;
      // Adjust fire parameters
      break;

    case MODE_KALEIDOSCOPE:
      // Change symmetry mode
      static uint8_t symmetryMode = 0;
      symmetryMode = (symmetryMode + 1) % 4;
      // Apply different symmetry patterns
      break;
  }
}

// ============================================
// BUTTON HOLD DETECTION
// ============================================
void checkEncoder2Hold() {
  if (encoderHandler.encoder2ButtonState &&
      millis() - encoderHandler.encoder2ButtonPressTime > 1000) {
    // Button held for 1 second

    // Example: Save current settings as preset
    saveCurrentPreset();

    // Flash confirmation
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
    delay(200);
  }
}

// ============================================
// COMBO ACTIONS
// ============================================
void checkComboActions() {
  // Both encoder buttons pressed together
  if (digitalRead(ENCODER_1_SW) == LOW &&
      digitalRead(ENCODER_2_SW) == LOW) {

    // Reset all settings to defaults
    colorModes.setMode(COLOR_FULL_SPECTRUM);
    FastLED.setBrightness(DEFAULT_BRIGHTNESS);
    currentMode = MODE_KALEIDOSCOPE;

    // Rainbow sweep to confirm
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(i * 256 / NUM_LEDS, 255, 255);
    }
    FastLED.show();
    delay(500);
  }
}