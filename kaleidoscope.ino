/*
 * Kaleidoscope - Motion-Reactive LED Art Installation
 *
 * Hardware:
 * - ELEGOO MEGA R3 Board ATmega 2560
 * - MPU-6050 3-Axis Accelerometer/Gyroscope
 * - BTF-LIGHTING WS2812B LED Strip (300 LEDs)
 *
 * Features:
 * - Motion-reactive animations that respond to tilt, rotation, and shake
 * - Multiple animation modes that can be switched
 * - Smooth transitions and motion processing
 */

#include "Config.h"
#include "MotionProcessor.h"
#include "LEDController.h"
#include "Animations.h"
#include "EncoderHandler.h"

// Global objects
MotionProcessor motionProcessor;
LEDController ledController;
Animations animations(ledController);
EncoderHandler encoderHandler;

// Animation state
enum AnimationMode {
  MODE_RAINBOW,
  MODE_SPARKLE,
  MODE_WAVE,
  MODE_FIRE,
  MODE_PULSE,
  MODE_KALEIDOSCOPE,
  MODE_COUNT  // Number of modes
};

AnimationMode currentMode = MODE_KALEIDOSCOPE;
unsigned long lastModeChange = 0;
int lastEncoderModePosition = 0;  // Track encoder position for mode changes

// Timing
unsigned long lastFrame = 0;
unsigned long lastMotionUpdate = 0;
unsigned long frameCount = 0;

// Mode switching button (optional - connect to pin 2)
const int MODE_BUTTON_PIN = 2;
bool lastButtonState = HIGH;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait up to 3 seconds for Serial
  Serial.println("\n=== Kaleidoscope Startup ===");

  // Initialize LED controller
  Serial.println("Initializing LEDs...");
  ledController.begin();

  // Show startup animation
  startupAnimation();

  // Initialize motion processor
  Serial.println("Initializing motion sensor...");
  if (!motionProcessor.begin()) {
    Serial.println("ERROR: Failed to initialize MPU6050!");
    Serial.println("Check wiring and restart.");
    // Show error pattern
    while (1) {
      ledController.fill(CRGB::Red);
      ledController.show();
      delay(500);
      ledController.clear();
      ledController.show();
      delay(500);
    }
  }

  // Calibrate motion sensor
  Serial.println("Calibrating...");
  ledController.fill(CRGB::Blue);
  ledController.show();
  motionProcessor.calibrate();

  // Setup mode button (optional)
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);

  // Initialize encoders
  if (USE_ENCODERS) {
    Serial.println("Initializing encoders...");
    encoderHandler.begin();
  }

  // Random startup mode if enabled
  if (RANDOM_START_MODE) {
    randomSeed(analogRead(0) + millis());  // Seed with analog noise + time
    currentMode = (AnimationMode)random(MODE_COUNT);
    Serial.print("Starting with random mode: ");
    Serial.println(getModeName(currentMode));
  } else {
    Serial.println("Starting with: Kaleidoscope");
  }

  Serial.println("=== Kaleidoscope Ready! ===");
  if (AUTO_CYCLE_MODES) {
    Serial.print("Auto-cycling modes every ");
    Serial.print(MODE_DURATION_MS / 1000);
    Serial.println(" seconds");
  }
  Serial.print("Animation FPS: ");
  Serial.println(TARGET_FPS);
  Serial.print("Motion sensor rate: ");
  Serial.print(MPU_UPDATE_RATE);
  Serial.println(" Hz");
  Serial.println();

  lastFrame = millis();
  lastMotionUpdate = millis();
  lastModeChange = millis();
}

void loop() {
  unsigned long currentTime = millis();

  // Update motion sensor at defined rate
  if (currentTime - lastMotionUpdate >= (1000 / MPU_UPDATE_RATE)) {
    motionProcessor.update();
    lastMotionUpdate = currentTime;

    // Print motion data periodically for debugging (adjusted for higher FPS)
    if (frameCount % (TARGET_FPS * 2) == 0) {  // Every 2 seconds
      printMotionData();
    }
  }

  // Update encoders
  if (USE_ENCODERS) {
    encoderHandler.update();

    // Check encoder 1 for mode changes
    int encoderModePos = encoderHandler.getEncoder1Position();
    if (encoderModePos != lastEncoderModePosition) {
      int modeDiff = encoderModePos - lastEncoderModePosition;
      lastEncoderModePosition = encoderModePos;

      // Change mode based on encoder rotation
      for (int i = 0; i < abs(modeDiff); i++) {
        if (modeDiff > 0) {
          nextMode();
        } else {
          previousMode();
        }
      }
    }

    // Check encoder 1 button for mode reset
    if (encoderHandler.getEncoder1Button()) {
      currentMode = MODE_KALEIDOSCOPE;  // Reset to default mode
      Serial.println("Mode reset to Kaleidoscope");
      lastModeChange = currentTime;
    }

    // Update virtual rotation from encoder 2
    motionProcessor.setVirtualRotation(encoderHandler.getVirtualRotation());
  }

  // Check for mode button press (if connected and encoders not in use)
  if (!USE_ENCODERS) {
    checkModeButton();
  }

  // Auto-cycle modes if enabled (disabled when using encoders)
  if (AUTO_CYCLE_MODES && !USE_ENCODERS && (currentTime - lastModeChange >= MODE_DURATION_MS)) {
    nextMode();
  }

  // Frame rate control
  if (currentTime - lastFrame >= FRAME_DELAY) {
    // Get motion data
    MotionData motion = motionProcessor.getMotionData();

    // Run current animation
    runCurrentAnimation(motion, currentTime);

    // Update LED strip
    ledController.show();

    lastFrame = currentTime;
    frameCount++;
  }
}

void runCurrentAnimation(const MotionData& motion, unsigned long time) {
  switch (currentMode) {
    case MODE_RAINBOW:
      animations.motionRainbow(motion, time);
      break;

    case MODE_SPARKLE:
      animations.motionSparkle(motion);
      break;

    case MODE_WAVE:
      animations.motionWave(motion, time);
      break;

    case MODE_FIRE:
      animations.motionFire(motion);
      break;

    case MODE_PULSE:
      animations.motionPulse(motion, time);
      break;

    case MODE_KALEIDOSCOPE:
      animations.motionKaleidoscope(motion, time);
      break;

    default:
      animations.motionKaleidoscope(motion, time);
      break;
  }
}

void checkModeButton() {
  bool buttonState = digitalRead(MODE_BUTTON_PIN);

  // Button pressed (LOW because of pull-up)
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50); // Debounce
    if (digitalRead(MODE_BUTTON_PIN) == LOW) {
      nextMode();
    }
  }

  lastButtonState = buttonState;
}

void nextMode() {
  currentMode = (AnimationMode)((currentMode + 1) % MODE_COUNT);
  lastModeChange = millis();

  // Clear LEDs on mode change
  ledController.clear();
  ledController.show();

  // Print mode name
  Serial.print("Mode changed to: ");
  Serial.println(getModeName(currentMode));

  // Brief flash to indicate mode change
  ledController.fill(CRGB::White);
  ledController.setBrightness(100);
  ledController.show();
  delay(100);
  ledController.clear();
  ledController.show();
  ledController.setBrightness(DEFAULT_BRIGHTNESS);
}

void previousMode() {
  currentMode = (AnimationMode)((currentMode - 1 + MODE_COUNT) % MODE_COUNT);
  lastModeChange = millis();

  // Clear LEDs on mode change
  ledController.clear();
  ledController.show();

  // Print mode name
  Serial.print("Mode changed to: ");
  Serial.println(getModeName(currentMode));

  // Brief flash to indicate mode change
  ledController.fill(CRGB::Blue);  // Different color for going backwards
  ledController.setBrightness(100);
  ledController.show();
  delay(100);
  ledController.clear();
  ledController.show();
  ledController.setBrightness(DEFAULT_BRIGHTNESS);
}

const char* getModeName(AnimationMode mode) {
  switch (mode) {
    case MODE_RAINBOW: return "Rainbow";
    case MODE_SPARKLE: return "Sparkle";
    case MODE_WAVE: return "Wave";
    case MODE_FIRE: return "Fire";
    case MODE_PULSE: return "Pulse";
    case MODE_KALEIDOSCOPE: return "Kaleidoscope";
    default: return "Unknown";
  }
}

void printMotionData() {
  MotionData motion = motionProcessor.getMotionData();

  Serial.println("=== Motion Data ===");
  Serial.print("Tilt: ");
  Serial.print(motion.tiltAngle);
  Serial.print("° (");
  Serial.print(motion.tiltNormalized * 100);
  Serial.println("%)");

  Serial.print("Rotation: ");
  Serial.print(motion.rotationSpeed);
  Serial.print("°/s (");
  Serial.print(motion.rotationNormalized * 100);
  Serial.println("%)");

  Serial.print("Shake: ");
  Serial.print(motion.shakeIntensity);
  Serial.print(" (");
  Serial.print(motion.shakeNormalized * 100);
  Serial.println("%)");

  Serial.print("Pitch: ");
  Serial.print(motion.pitch);
  Serial.print("° Roll: ");
  Serial.print(motion.roll);
  Serial.println("°");

  Serial.print("FPS: ");
  Serial.println(1000.0 / FRAME_DELAY);
  Serial.println();
}

void startupAnimation() {
  // Quick rainbow sweep on startup
  for (int i = 0; i < 256; i += 4) {
    for (uint16_t j = 0; j < ledController.numLeds(); j++) {
      uint8_t hue = (j * 256 / ledController.numLeds() + i) % 256;
      ledController.setPixel(j, CHSV(hue, 255, 255));
    }
    ledController.show();
    delay(5);
  }

  ledController.clear();
  ledController.show();
  delay(500);
}
