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

AnimationMode currentMode = MODE_FIRE;
unsigned long lastModeChange = 0;
int lastEncoderModePosition = 0;  // Track encoder position for mode changes

// Timing
unsigned long lastFrame = 0;
unsigned long lastMotionUpdate = 0;
unsigned long frameCount = 0;

// Mode switching button (optional - connect to pin 2)
const int MODE_BUTTON_PIN = 2;
bool lastButtonState = HIGH;
int lastEnc2Pos = 0;  // Track encoder 2 changes for logging

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
      printEffectParams();
    }

    // Check encoder 1 button for mode reset
    if (encoderHandler.getEncoder1Button()) {
      currentMode = MODE_KALEIDOSCOPE;  // Reset to default mode
      Serial.println("Mode reset to Kaleidoscope");
      lastModeChange = currentTime;
      printEffectParams();
    }

    // Update virtual rotation from encoder 2
    motionProcessor.setVirtualRotation(encoderHandler.getVirtualRotation());

    // Log effect params when encoder 2 changes
    int enc2Pos = encoderHandler.getEncoder2Position();
    if (enc2Pos != lastEnc2Pos) {
      lastEnc2Pos = enc2Pos;
      printEffectParams();
    }
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
  Serial.print("%)  Pitch: ");
  Serial.print(motion.pitch);
  Serial.print("° Roll: ");
  Serial.print(motion.roll);
  Serial.print("° Pan: ");
  Serial.print(motion.pan);
  Serial.println("°");

  // Raw per-axis data to identify which axis = which physical movement
  // Try: 1) tilt tube up/down  2) pan left/right  3) rotate around tube axis
  Serial.print("Accel: X=");
  Serial.print(motion.accelX, 1);
  Serial.print(" Y=");
  Serial.print(motion.accelY, 1);
  Serial.print(" Z=");
  Serial.println(motion.accelZ, 1);

  Serial.print("Gyro(°/s): X=");
  Serial.print(motion.gyroX * 57.3, 1);  // rad/s to deg/s
  Serial.print(" Y=");
  Serial.print(motion.gyroY * 57.3, 1);
  Serial.print(" Z=");
  Serial.println(motion.gyroZ * 57.3, 1);

  Serial.print("RotSpeed: ");
  Serial.print(motion.rotationSpeed, 1);
  Serial.print("°/s  Shake: ");
  Serial.print(motion.shakeNormalized * 100, 0);
  Serial.print("%  FPS: ");
  Serial.println(1000.0 / FRAME_DELAY);
  Serial.println();
}

void printEffectParams() {
  MotionData motion = motionProcessor.getMotionData();
  float enc2 = motion.rotationNormalized;
  float tilt = motion.tiltNormalized;
  float pan = abs(motion.pan);

  Serial.println("--- Effect Params ---");
  Serial.print("Mode: ");
  Serial.print(getModeName(currentMode));
  Serial.print(" | Enc2(rotNorm)=");
  Serial.print(enc2, 2);
  Serial.print(" tilt=");
  Serial.print(tilt, 2);
  Serial.print(" pan=");
  Serial.println(pan, 1);

  switch (currentMode) {
    case MODE_RAINBOW: {
      float speed = 1.0 + enc2 * 5.0 + tilt * 10.0 + pan / 45.0 * 5.0;
      Serial.print("  speed=");
      Serial.print(speed, 1);
      Serial.println(" (enc2 adds 0-5)");
      break;
    }
    case MODE_SPARKLE: {
      float density = 0.1 + enc2 * 0.4 + tilt * 0.5 + pan / 90.0 * 0.3;
      Serial.print("  density=");
      Serial.print(density, 2);
      Serial.println(" (enc2 adds 0-0.4)");
      break;
    }
    case MODE_WAVE: {
      float speed = 1.0 + enc2 * 4.0 + tilt * 5.0 + pan / 45.0 * 3.0;
      uint8_t waveWidth = 50 - enc2 * 30 - tilt * 15;
      Serial.print("  speed=");
      Serial.print(speed, 1);
      Serial.print(" waveWidth=");
      Serial.print(waveWidth);
      Serial.println(" (enc2: +speed, -width)");
      break;
    }
    case MODE_FIRE: {
      uint8_t cooling = 60 - enc2 * 40 + tilt * 30;
      uint8_t sparking = 40 + enc2 * 160 + tilt * 50 + pan / 45.0 * 30;
      uint8_t hueShift = enc2 * 160;
      Serial.print("  cooling=");
      Serial.print(cooling);
      Serial.print(" sparking=");
      Serial.print(sparking);
      Serial.print(" hue=");
      Serial.print(hueShift);
      Serial.println(" (enc2: embers->inferno->blue fire)");
      break;
    }
    case MODE_PULSE: {
      float speed = 1.0 + enc2 * 3.0 + tilt * 5.0 + pan / 30.0 * 3.0;
      Serial.print("  speed=");
      Serial.print(speed, 1);
      Serial.println(" (enc2 adds 0-3)");
      break;
    }
    case MODE_KALEIDOSCOPE: {
      float speed = 1.0 + enc2 * 4.0 + tilt * 8.0 + pan / 30.0 * 5.0;
      float waveFreq1 = 4.0 + enc2 * 8.0;
      float waveFreq2 = 2.0 + enc2 * 6.0;
      float hueSpread = 60 + enc2 * 120;
      Serial.print("  speed=");
      Serial.print(speed, 1);
      Serial.print(" waveFreq=");
      Serial.print(waveFreq1, 1);
      Serial.print("/");
      Serial.print(waveFreq2, 1);
      Serial.print(" hueSpread=");
      Serial.println(hueSpread, 0);
      break;
    }
    default: break;
  }
  Serial.println();
}

void startupAnimation() {
  // Test: fill all red
  Serial.println("Testing both strips - filling RED");
  ledController.fill(CRGB::Red);
  ledController.show();
  delay(2000);
  
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
