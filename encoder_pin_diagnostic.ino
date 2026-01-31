// ENCODER PIN DIAGNOSTIC
// This will show exactly what's happening with each pin

#define ENCODER_2_CLK 18  // Clock (interrupt capable)
#define ENCODER_2_DT  19  // Data/Direction
#define ENCODER_2_SW  17  // Switch/Button

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=====================================");
  Serial.println("    ENCODER 2 PIN DIAGNOSTIC");
  Serial.println("=====================================");
  Serial.println();
  Serial.println("This will show the real-time state of each pin.");
  Serial.println("Rotate Encoder 2 SLOWLY and watch the pin states.");
  Serial.println();

  // Configure pins with pullups
  pinMode(ENCODER_2_CLK, INPUT_PULLUP);
  pinMode(ENCODER_2_DT, INPUT_PULLUP);
  pinMode(ENCODER_2_SW, INPUT_PULLUP);

  Serial.println("Pin Configuration:");
  Serial.println("  Pin 18 (CLK): INPUT_PULLUP");
  Serial.println("  Pin 19 (DT):  INPUT_PULLUP");
  Serial.println("  Pin 17 (SW):  INPUT_PULLUP");
  Serial.println();
  Serial.println("Expected behavior when rotating:");
  Serial.println("  CLOCKWISE:    CLK falls before DT");
  Serial.println("  COUNTER-CW:   DT falls before CLK");
  Serial.println();
  Serial.println("Pin states (1=HIGH/unpressed, 0=LOW/pressed):");
  Serial.println("-------------------------------------");
}

void loop() {
  static int lastCLK = HIGH;
  static int lastDT = HIGH;
  static int lastSW = HIGH;

  int currentCLK = digitalRead(ENCODER_2_CLK);
  int currentDT = digitalRead(ENCODER_2_DT);
  int currentSW = digitalRead(ENCODER_2_SW);

  // Only print if something changed
  if (currentCLK != lastCLK || currentDT != lastDT || currentSW != lastSW) {

    // Clear line and print current state
    Serial.print("\rCLK:");
    Serial.print(currentCLK);
    Serial.print(" DT:");
    Serial.print(currentDT);
    Serial.print(" SW:");
    Serial.print(currentSW);

    // Detect rotation direction
    if (lastCLK == HIGH && currentCLK == LOW) {
      // CLK falling edge detected
      Serial.print("  → CLK FALLING, DT=");
      Serial.print(currentDT);
      if (currentDT == LOW) {
        Serial.print(" [CLOCKWISE]     ");
      } else {
        Serial.print(" [COUNTER-CW]    ");
      }
    }

    // Button press
    if (currentSW == LOW && lastSW == HIGH) {
      Serial.print(" [BUTTON PRESSED]");
    }

    lastCLK = currentCLK;
    lastDT = currentDT;
    lastSW = currentSW;
  }

  // Continuous monitoring display
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 2000) {
    Serial.println();
    Serial.println();
    Serial.print("Current readings - CLK:");
    Serial.print(digitalRead(ENCODER_2_CLK));
    Serial.print(" DT:");
    Serial.print(digitalRead(ENCODER_2_DT));
    Serial.print(" SW:");
    Serial.println(digitalRead(ENCODER_2_SW));

    // Check for common issues
    if (digitalRead(ENCODER_2_CLK) == LOW) {
      Serial.println("⚠️  CLK stuck LOW - check wiring!");
    }
    if (digitalRead(ENCODER_2_DT) == LOW) {
      Serial.println("⚠️  DT stuck LOW - check wiring!");
    }
    if (digitalRead(ENCODER_2_CLK) == digitalRead(ENCODER_2_DT)) {
      Serial.println("ℹ️  CLK and DT same state - normal when not rotating");
    }

    Serial.println();
    lastStatus = millis();
  }

  delay(1);  // Small delay for stability
}