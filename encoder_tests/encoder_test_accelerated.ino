// ACCELERATED ENCODER TEST
// Fast rotation = bigger jumps in values
// Slow rotation = fine control

// Encoder 1 pins (Mode selection)
#define ENCODER_1_CLK 3
#define ENCODER_1_DT  5
#define ENCODER_1_SW  7

// Encoder 2 pins (Virtual rotation/intensity)
#define ENCODER_2_CLK 18
#define ENCODER_2_DT  19
#define ENCODER_2_SW  17

// For mode selection (0-5 for 6 modes)
volatile int encoder1Mode = 0;
const int MAX_MODES = 6;

// For intensity control (0-100)
volatile int encoder2Value = 50;  // Start at 50%
const int MIN_VALUE = 0;
const int MAX_VALUE = 100;

// Acceleration tracking
volatile unsigned long lastEncoder1Time = 0;
volatile unsigned long lastEncoder2Time = 0;
volatile int encoder1Speed = 1;
volatile int encoder2Speed = 1;

// Button states
bool encoder1ButtonState = HIGH;
bool encoder2ButtonState = HIGH;
bool lastEncoder1Button = HIGH;
bool lastEncoder2Button = HIGH;

// Animation mode names
const char* modeNames[] = {
  "RAINBOW",
  "SPARKLE",
  "WAVE",
  "FIRE",
  "PULSE",
  "KALEIDOSCOPE"
};

void encoder1ISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce
  if (interruptTime - lastInterruptTime > 5) {
    // Calculate time between clicks for acceleration
    unsigned long timeDiff = interruptTime - lastEncoder1Time;

    // Acceleration: faster rotation = bigger jumps
    if (timeDiff < 50) {
      encoder1Speed = 3;  // Very fast - jump by 3
    } else if (timeDiff < 100) {
      encoder1Speed = 2;  // Fast - jump by 2
    } else {
      encoder1Speed = 1;  // Normal speed
    }

    // Change mode with wrapping
    if (digitalRead(ENCODER_1_DT) == LOW) {
      encoder1Mode = (encoder1Mode + encoder1Speed) % MAX_MODES;
    } else {
      encoder1Mode = encoder1Mode - encoder1Speed;
      if (encoder1Mode < 0) encoder1Mode = MAX_MODES - 1;
    }

    lastEncoder1Time = interruptTime;
  }
  lastInterruptTime = interruptTime;
}

void encoder2ISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce
  if (interruptTime - lastInterruptTime > 5) {
    // Calculate time between clicks for acceleration
    unsigned long timeDiff = interruptTime - lastEncoder2Time;

    // Acceleration for value control
    if (timeDiff < 30) {
      encoder2Speed = 10;  // Very fast - jump by 10
    } else if (timeDiff < 60) {
      encoder2Speed = 5;   // Fast - jump by 5
    } else if (timeDiff < 100) {
      encoder2Speed = 2;   // Medium - jump by 2
    } else {
      encoder2Speed = 1;   // Fine control
    }

    // Change value with limits
    if (digitalRead(ENCODER_2_DT) == LOW) {
      encoder2Value = min(MAX_VALUE, encoder2Value + encoder2Speed);
    } else {
      encoder2Value = max(MIN_VALUE, encoder2Value - encoder2Speed);
    }

    lastEncoder2Time = interruptTime;
  }
  lastInterruptTime = interruptTime;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=================================");
  Serial.println("  ACCELERATED ENCODER TEST");
  Serial.println("=================================");
  Serial.println();
  Serial.println("FEATURES:");
  Serial.println("• Encoder 1: Select mode (0-5)");
  Serial.println("• Encoder 2: Adjust value (0-100%)");
  Serial.println("• FAST rotation = BIG jumps");
  Serial.println("• SLOW rotation = FINE control");
  Serial.println();
  Serial.println("TIP: Spin fast to jump quickly,");
  Serial.println("     turn slowly for precision!");
  Serial.println("---------------------------------");
  Serial.println();

  // Configure pins
  pinMode(ENCODER_1_CLK, INPUT_PULLUP);
  pinMode(ENCODER_1_DT, INPUT_PULLUP);
  pinMode(ENCODER_1_SW, INPUT_PULLUP);

  pinMode(ENCODER_2_CLK, INPUT_PULLUP);
  pinMode(ENCODER_2_DT, INPUT_PULLUP);
  pinMode(ENCODER_2_SW, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_1_CLK), encoder1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_2_CLK), encoder2ISR, FALLING);

  Serial.println("Ready! Try rotating at different speeds...");
  Serial.println();
}

void loop() {
  // Read buttons
  encoder1ButtonState = digitalRead(ENCODER_1_SW);
  encoder2ButtonState = digitalRead(ENCODER_2_SW);

  // Encoder 1 button - reset to mode 0
  if (encoder1ButtonState == LOW && lastEncoder1Button == HIGH) {
    encoder1Mode = 0;
    Serial.println("*** MODE RESET TO RAINBOW ***");
  }

  // Encoder 2 button - cycle through color modes (preview feature)
  static int colorMode = 0;
  if (encoder2ButtonState == LOW && lastEncoder2Button == HIGH) {
    colorMode = (colorMode + 1) % 7;
    Serial.print("*** COLOR MODE: ");
    switch(colorMode) {
      case 0: Serial.println("FULL SPECTRUM ***"); break;
      case 1: Serial.println("WARM ***"); break;
      case 2: Serial.println("COOL ***"); break;
      case 3: Serial.println("FIRE ***"); break;
      case 4: Serial.println("OCEAN ***"); break;
      case 5: Serial.println("PARTY ***"); break;
      case 6: Serial.println("MONOCHROME ***"); break;
    }
  }

  lastEncoder1Button = encoder1ButtonState;
  lastEncoder2Button = encoder2ButtonState;

  // Display status
  static unsigned long lastDisplayTime = 0;
  static int lastMode = -1;
  static int lastValue = -1;

  if (millis() - lastDisplayTime >= 100) {
    if (encoder1Mode != lastMode || encoder2Value != lastValue) {
      // Clear line and print new status
      Serial.print("\r");
      Serial.print("MODE: ");
      Serial.print(modeNames[encoder1Mode]);

      // Pad mode name to align
      int padding = 15 - strlen(modeNames[encoder1Mode]);
      for(int i = 0; i < padding; i++) Serial.print(" ");

      Serial.print(" | INTENSITY: ");
      if (encoder2Value < 10) Serial.print(" ");
      if (encoder2Value < 100) Serial.print(" ");
      Serial.print(encoder2Value);
      Serial.print("%");

      // Show visual bar graph
      Serial.print("  [");
      int barLength = encoder2Value / 5;  // 20 character bar
      for(int i = 0; i < 20; i++) {
        if (i < barLength) {
          Serial.print("█");
        } else {
          Serial.print("·");
        }
      }
      Serial.print("]");

      // Show acceleration indicator
      if (encoder1Speed > 1 || encoder2Speed > 1) {
        Serial.print(" ACCEL!");
      } else {
        Serial.print("       ");
      }

      lastMode = encoder1Mode;
      lastValue = encoder2Value;
    }
    lastDisplayTime = millis();
  }

  // Show tips periodically
  static unsigned long lastTipTime = 0;
  if (millis() - lastTipTime >= 10000) {
    Serial.println();
    Serial.println();
    Serial.println("💡 TIP: Rotate FAST for big jumps, SLOW for fine control!");
    Serial.println("        Press buttons to test features.");
    Serial.println();
    lastTipTime = millis();
  }

  delay(10);
}