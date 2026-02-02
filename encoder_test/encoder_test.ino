/*
 * Encoder Test Sketch
 * Tests KY-040 rotary encoders with Arduino Mega
 *
 * Wiring:
 * Encoder 1: CLK=3, DT=5, SW=7
 * Encoder 2: CLK=18, DT=19, SW=17
 */

// Encoder 1 pins
#define ENCODER_1_CLK 3
#define ENCODER_1_DT 5
#define ENCODER_1_SW 7

// Encoder 2 pins
#define ENCODER_2_CLK 18
#define ENCODER_2_DT 19
#define ENCODER_2_SW 17

// Encoder positions
volatile int encoder1Pos = 0;
volatile int encoder2Pos = 0;

// Button states
bool encoder1Button = false;
bool encoder2Button = false;
bool encoder1LastButton = HIGH;
bool encoder2LastButton = HIGH;

// Timing
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== KY-040 Encoder Test ===");
  Serial.println("Rotate encoders to see position changes");
  Serial.println("Press encoder buttons to see button events\n");

  // Configure encoder 1
  pinMode(ENCODER_1_CLK, INPUT);
  pinMode(ENCODER_1_DT, INPUT);
  pinMode(ENCODER_1_SW, INPUT_PULLUP);

  // Configure encoder 2
  pinMode(ENCODER_2_CLK, INPUT);
  pinMode(ENCODER_2_DT, INPUT);
  pinMode(ENCODER_2_SW, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_1_CLK), encoder1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_2_CLK), encoder2ISR, FALLING);

  Serial.println("Setup complete!");
  Serial.println("Encoder 1: Mode Control (CLK=3, DT=5, SW=7)");
  Serial.println("Encoder 2: Effect Control (CLK=18, DT=19, SW=17)");
  Serial.println();
}

void loop() {
  // Check buttons
  checkButtons();

  // Print status every 100ms
  if (millis() - lastPrint > 100) {
    lastPrint = millis();

    Serial.print("Enc1 Pos: ");
    Serial.print(encoder1Pos);
    Serial.print("\tMode: ");
    Serial.print(abs(encoder1Pos) % 6);  // Simulate 6 modes

    Serial.print("\t|\tEnc2 Pos: ");
    Serial.print(encoder2Pos);
    Serial.print("\tEffect: ");
    Serial.print(constrain(encoder2Pos * 5, 0, 100));  // 0-100% effect
    Serial.print("%");

    if (encoder1Button) {
      Serial.print("\t[ENC1 BUTTON!]");
    }
    if (encoder2Button) {
      Serial.print("\t[ENC2 BUTTON!]");
    }

    Serial.println();
  }
}

void checkButtons() {
  // Check encoder 1 button
  bool btn1 = digitalRead(ENCODER_1_SW);
  if (btn1 == LOW && encoder1LastButton == HIGH) {
    encoder1Button = true;
    Serial.println("\n>>> Encoder 1 Button Pressed - Mode Reset!");
    encoder1Pos = 0;  // Reset position
  } else {
    encoder1Button = false;
  }
  encoder1LastButton = btn1;

  // Check encoder 2 button
  bool btn2 = digitalRead(ENCODER_2_SW);
  if (btn2 == LOW && encoder2LastButton == HIGH) {
    encoder2Button = true;
    Serial.println("\n>>> Encoder 2 Button Pressed - Effect Toggle!");
  } else {
    encoder2Button = false;
  }
  encoder2LastButton = btn2;
}

// Interrupt service routine for encoder 1
void encoder1ISR() {
  static unsigned long lastInterrupt = 0;
  unsigned long now = millis();

  // Debounce
  if (now - lastInterrupt < 5) return;
  lastInterrupt = now;

  // Read direction
  if (digitalRead(ENCODER_1_DT)) {
    encoder1Pos++;
    Serial.println("Enc1: CW (Next Mode)");
  } else {
    encoder1Pos--;
    Serial.println("Enc1: CCW (Previous Mode)");
  }
}

// Interrupt service routine for encoder 2
void encoder2ISR() {
  static unsigned long lastInterrupt = 0;
  unsigned long now = millis();

  // Debounce
  if (now - lastInterrupt < 5) return;
  lastInterrupt = now;

  // Read direction
  if (digitalRead(ENCODER_2_DT)) {
    encoder2Pos++;
    Serial.println("Enc2: CW (More Effect)");
  } else {
    encoder2Pos--;
    Serial.println("Enc2: CCW (Less Effect)");
  }
}