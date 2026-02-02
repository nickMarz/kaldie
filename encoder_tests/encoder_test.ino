// ENCODER WIRING TEST SKETCH
// Tests both KY-040 rotary encoders without LEDs
// Shows rotation counts and button presses in Serial Monitor

// Encoder 1 pins (Mode selection)
#define ENCODER_1_CLK 3   // Clock (interrupt capable)
#define ENCODER_1_DT  5   // Data
#define ENCODER_1_SW  7   // Switch/Button

// Encoder 2 pins (Virtual rotation)
#define ENCODER_2_CLK 18  // Clock (interrupt capable)
#define ENCODER_2_DT  19  // Data
#define ENCODER_2_SW  17  // Switch/Button

// Encoder positions
volatile int encoder1Position = 0;
volatile int encoder2Position = 0;

// Button states
bool encoder1ButtonState = HIGH;
bool encoder2ButtonState = HIGH;
bool lastEncoder1Button = HIGH;
bool lastEncoder2Button = HIGH;

// Timing
unsigned long lastPrintTime = 0;
unsigned long lastEncoder1Change = 0;
unsigned long lastEncoder2Change = 0;

// Setup interrupt service routines
void encoder1ISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce
  if (interruptTime - lastInterruptTime > 5) {
    if (digitalRead(ENCODER_1_DT) == LOW) {
      encoder1Position++;
    } else {
      encoder1Position--;
    }
    lastEncoder1Change = interruptTime;
  }
  lastInterruptTime = interruptTime;
}

void encoder2ISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce
  if (interruptTime - lastInterruptTime > 5) {
    if (digitalRead(ENCODER_2_DT) == LOW) {
      encoder2Position++;
    } else {
      encoder2Position--;
    }
    lastEncoder2Change = interruptTime;
  }
  lastInterruptTime = interruptTime;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=================================");
  Serial.println("    ENCODER WIRING TEST");
  Serial.println("=================================");
  Serial.println();
  Serial.println("Wiring Check:");
  Serial.println("ENCODER 1 - Mode Selection:");
  Serial.println("  CLK -> Pin 3");
  Serial.println("  DT  -> Pin 5");
  Serial.println("  SW  -> Pin 7");
  Serial.println("  +   -> 5V");
  Serial.println("  GND -> GND");
  Serial.println();
  Serial.println("ENCODER 2 - Virtual Rotation:");
  Serial.println("  CLK -> Pin 18");
  Serial.println("  DT  -> Pin 19");
  Serial.println("  SW  -> Pin 17");
  Serial.println("  +   -> 5V");
  Serial.println("  GND -> GND");
  Serial.println();
  Serial.println("---------------------------------");
  Serial.println("TEST INSTRUCTIONS:");
  Serial.println("1. Rotate each encoder");
  Serial.println("2. Press each button");
  Serial.println("3. Watch values change below");
  Serial.println("---------------------------------");
  Serial.println();

  // Configure encoder pins
  pinMode(ENCODER_1_CLK, INPUT_PULLUP);
  pinMode(ENCODER_1_DT, INPUT_PULLUP);
  pinMode(ENCODER_1_SW, INPUT_PULLUP);

  pinMode(ENCODER_2_CLK, INPUT_PULLUP);
  pinMode(ENCODER_2_DT, INPUT_PULLUP);
  pinMode(ENCODER_2_SW, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_1_CLK), encoder1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_2_CLK), encoder2ISR, FALLING);

  Serial.println("Setup complete! Starting monitoring...");
  Serial.println();
}

void loop() {
  // Read button states
  encoder1ButtonState = digitalRead(ENCODER_1_SW);
  encoder2ButtonState = digitalRead(ENCODER_2_SW);

  // Check for button presses
  if (encoder1ButtonState == LOW && lastEncoder1Button == HIGH) {
    Serial.println("*** ENCODER 1 BUTTON PRESSED! ***");
  }
  if (encoder1ButtonState == HIGH && lastEncoder1Button == LOW) {
    Serial.println("*** ENCODER 1 BUTTON RELEASED ***");
  }

  if (encoder2ButtonState == LOW && lastEncoder2Button == HIGH) {
    Serial.println("*** ENCODER 2 BUTTON PRESSED! ***");
  }
  if (encoder2ButtonState == HIGH && lastEncoder2Button == LOW) {
    Serial.println("*** ENCODER 2 BUTTON RELEASED ***");
  }

  // Store button states
  lastEncoder1Button = encoder1ButtonState;
  lastEncoder2Button = encoder2ButtonState;

  // Print status every 200ms if there's activity
  unsigned long currentTime = millis();
  bool hasActivity = (currentTime - lastEncoder1Change < 2000) ||
                     (currentTime - lastEncoder2Change < 2000) ||
                     encoder1ButtonState == LOW ||
                     encoder2ButtonState == LOW;

  if (currentTime - lastPrintTime >= 200 && hasActivity) {
    Serial.print("ENC1: ");
    Serial.print(encoder1Position);
    Serial.print(" [");
    Serial.print(encoder1ButtonState == LOW ? "BTN" : "---");
    Serial.print("]  |  ");

    Serial.print("ENC2: ");
    Serial.print(encoder2Position);
    Serial.print(" [");
    Serial.print(encoder2ButtonState == LOW ? "BTN" : "---");
    Serial.println("]");

    // Show rotation direction hints
    static int lastEnc1 = 0;
    static int lastEnc2 = 0;

    if (encoder1Position > lastEnc1) {
      Serial.println("  Encoder 1: Rotating CLOCKWISE →");
    } else if (encoder1Position < lastEnc1) {
      Serial.println("  Encoder 1: Rotating COUNTER-CLOCKWISE ←");
    }

    if (encoder2Position > lastEnc2) {
      Serial.println("  Encoder 2: Rotating CLOCKWISE →");
    } else if (encoder2Position < lastEnc2) {
      Serial.println("  Encoder 2: Rotating COUNTER-CLOCKWISE ←");
    }

    lastEnc1 = encoder1Position;
    lastEnc2 = encoder2Position;
    lastPrintTime = currentTime;
  }

  // Diagnostic info every 5 seconds
  static unsigned long lastDiagnostic = 0;
  if (currentTime - lastDiagnostic >= 5000) {
    Serial.println();
    Serial.println("=== DIAGNOSTIC CHECK ===");
    Serial.print("Pin states: ");
    Serial.print("E1_CLK:");
    Serial.print(digitalRead(ENCODER_1_CLK));
    Serial.print(" E1_DT:");
    Serial.print(digitalRead(ENCODER_1_DT));
    Serial.print(" E1_SW:");
    Serial.print(digitalRead(ENCODER_1_SW));
    Serial.print(" | E2_CLK:");
    Serial.print(digitalRead(ENCODER_2_CLK));
    Serial.print(" E2_DT:");
    Serial.print(digitalRead(ENCODER_2_DT));
    Serial.print(" E2_SW:");
    Serial.println(digitalRead(ENCODER_2_SW));

    if (encoder1Position == 0 && encoder2Position == 0) {
      Serial.println("⚠ No rotation detected - check CLK/DT connections");
    }
    if (encoder1ButtonState == HIGH && encoder2ButtonState == HIGH) {
      Serial.println("ℹ Buttons ready (HIGH = not pressed)");
    }
    Serial.println("========================");
    Serial.println();

    lastDiagnostic = currentTime;
  }

  delay(10);
}