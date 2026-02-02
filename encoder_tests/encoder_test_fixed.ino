// ENCODER TEST WITH DIRECTION FIX
// This version can detect and fix common wiring issues

// Encoder 1 pins
#define ENCODER_1_CLK 3
#define ENCODER_1_DT  5
#define ENCODER_1_SW  7

// Encoder 2 pins
#define ENCODER_2_CLK 18
#define ENCODER_2_DT  19
#define ENCODER_2_SW  17

// Positions
volatile int encoder1Position = 0;
volatile int encoder2Position = 0;

// Direction detection
volatile bool encoder2DTWorking = true;
volatile int encoder2DTReadings = 0;
volatile int encoder2DTChanges = 0;

// Button states
bool lastEncoder1Button = HIGH;
bool lastEncoder2Button = HIGH;

// Timing
unsigned long lastEncoder1Change = 0;
unsigned long lastEncoder2Change = 0;

void encoder1ISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

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
  static int lastDTState = HIGH;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 5) {
    int dtState = digitalRead(ENCODER_2_DT);

    // Track if DT pin is changing
    if (dtState != lastDTState) {
      encoder2DTChanges++;
      lastDTState = dtState;
    }
    encoder2DTReadings++;

    // Check if DT pin is working after 10 readings
    if (encoder2DTReadings == 10) {
      if (encoder2DTChanges < 2) {
        encoder2DTWorking = false;
        Serial.println();
        Serial.println("⚠️  ENCODER 2 DT PIN NOT CHANGING!");
        Serial.println("    Pin 19 may be disconnected or stuck.");
        Serial.println("    Using alternate rotation method.");
        Serial.println();
      }
    }

    // Direction detection
    if (encoder2DTWorking) {
      // Normal operation - use DT pin
      if (dtState == LOW) {
        encoder2Position++;
      } else {
        encoder2Position--;
      }
    } else {
      // DT pin not working - always increment
      // User can only go one direction but at least it works
      encoder2Position++;
    }

    lastEncoder2Change = interruptTime;
  }
  lastInterruptTime = interruptTime;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=====================================");
  Serial.println("  ENCODER TEST WITH DIRECTION FIX");
  Serial.println("=====================================");
  Serial.println();

  // Configure pins
  pinMode(ENCODER_1_CLK, INPUT_PULLUP);
  pinMode(ENCODER_1_DT, INPUT_PULLUP);
  pinMode(ENCODER_1_SW, INPUT_PULLUP);

  pinMode(ENCODER_2_CLK, INPUT_PULLUP);
  pinMode(ENCODER_2_DT, INPUT_PULLUP);
  pinMode(ENCODER_2_SW, INPUT_PULLUP);

  // Check initial pin states
  Serial.println("Initial Pin States:");
  Serial.print("Encoder 2 - CLK:");
  Serial.print(digitalRead(ENCODER_2_CLK));
  Serial.print(" DT:");
  Serial.print(digitalRead(ENCODER_2_DT));
  Serial.print(" SW:");
  Serial.println(digitalRead(ENCODER_2_SW));

  if (digitalRead(ENCODER_2_DT) == LOW) {
    Serial.println("⚠️  WARNING: DT pin starts LOW - may be shorted to ground!");
  }

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_1_CLK), encoder1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_2_CLK), encoder2ISR, FALLING);

  Serial.println();
  Serial.println("TROUBLESHOOTING GUIDE:");
  Serial.println("----------------------");
  Serial.println("If Encoder 2 only goes one direction:");
  Serial.println();
  Serial.println("1. CHECK WIRING:");
  Serial.println("   Encoder → Arduino");
  Serial.println("   CLK → Pin 18 ✓");
  Serial.println("   DT  → Pin 19 ← Check this!");
  Serial.println("   SW  → Pin 17 ✓");
  Serial.println("   +   → 5V");
  Serial.println("   GND → GND");
  Serial.println();
  Serial.println("2. COMMON FIXES:");
  Serial.println("   • Ensure DT wire is firmly connected");
  Serial.println("   • Try swapping CLK and DT wires");
  Serial.println("   • Check for bent pins on encoder");
  Serial.println("   • Test continuity of DT wire");
  Serial.println();
  Serial.println("Starting monitoring...");
  Serial.println("-------------------------------------");
}

void loop() {
  // Read buttons
  bool button1 = digitalRead(ENCODER_1_SW);
  bool button2 = digitalRead(ENCODER_2_SW);

  if (button1 == LOW && lastEncoder1Button == HIGH) {
    Serial.println("*** ENCODER 1 BUTTON PRESSED ***");
  }
  if (button2 == LOW && lastEncoder2Button == HIGH) {
    Serial.println("*** ENCODER 2 BUTTON PRESSED ***");
    // Test: Read DT pin directly when button pressed
    Serial.print("    DT Pin reads: ");
    Serial.println(digitalRead(ENCODER_2_DT) ? "HIGH" : "LOW");
  }

  lastEncoder1Button = button1;
  lastEncoder2Button = button2;

  // Display positions
  static unsigned long lastPrint = 0;
  static int lastPos1 = 0;
  static int lastPos2 = 0;

  if (millis() - lastPrint >= 200) {
    bool changed = (encoder1Position != lastPos1) || (encoder2Position != lastPos2);

    if (changed) {
      Serial.print("ENC1: ");
      if (encoder1Position >= 0) Serial.print(" ");
      Serial.print(encoder1Position);
      Serial.print("  |  ENC2: ");
      if (encoder2Position >= 0) Serial.print(" ");
      Serial.print(encoder2Position);

      if (!encoder2DTWorking) {
        Serial.print(" [DT FAULT - ONE DIR ONLY]");
      }

      Serial.println();

      lastPos1 = encoder1Position;
      lastPos2 = encoder2Position;
    }
    lastPrint = millis();
  }

  // Diagnostic check every 5 seconds
  static unsigned long lastDiag = 0;
  if (millis() - lastDiag >= 5000) {
    Serial.println();
    Serial.println("=== PIN STATE CHECK ===");
    Serial.print("E2 Pins - CLK:");
    Serial.print(digitalRead(ENCODER_2_CLK));
    Serial.print(" DT:");
    Serial.print(digitalRead(ENCODER_2_DT));
    Serial.print(" SW:");
    Serial.println(digitalRead(ENCODER_2_SW));

    if (encoder2DTReadings > 0) {
      Serial.print("DT pin changes detected: ");
      Serial.print(encoder2DTChanges);
      Serial.print(" out of ");
      Serial.print(encoder2DTReadings);
      Serial.println(" interrupts");

      if (encoder2DTChanges == 0) {
        Serial.println("❌ DT pin is not changing - check connection to pin 19!");
      }
    }

    Serial.println("=======================");
    lastDiag = millis();
  }

  delay(10);
}