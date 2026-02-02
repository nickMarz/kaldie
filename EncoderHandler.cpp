#include "EncoderHandler.h"

// Static instance pointer for ISR access
EncoderHandler* EncoderHandler::instance = nullptr;

EncoderHandler::EncoderHandler() :
  encoder1Pos(0),
  encoder2Pos(0),
  encoder1ButtonPressed(false),
  encoder2ButtonPressed(false),
  encoder1LastButton(HIGH),
  encoder2LastButton(HIGH),
  encoder1LastInterrupt(0),
  encoder2LastInterrupt(0),
  encoder1ButtonTime(0),
  encoder2ButtonTime(0),
  encoder1LastChange(0),
  encoder2LastChange(0),
  encoder1Speed(0),
  encoder2Speed(0) {
  instance = this;
}

void EncoderHandler::begin() {
  if (!USE_ENCODERS) return;

  // Configure encoder 1 pins
  pinMode(ENCODER_1_CLK, INPUT);
  pinMode(ENCODER_1_DT, INPUT);
  pinMode(ENCODER_1_SW, INPUT_PULLUP);

  // Configure encoder 2 pins
  pinMode(ENCODER_2_CLK, INPUT);
  pinMode(ENCODER_2_DT, INPUT);
  pinMode(ENCODER_2_SW, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_1_CLK), encoder1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_2_CLK), encoder2ISR, FALLING);

  Serial.println("Encoders initialized");
  Serial.print("Encoder 1: CLK=");
  Serial.print(ENCODER_1_CLK);
  Serial.print(" DT=");
  Serial.print(ENCODER_1_DT);
  Serial.print(" SW=");
  Serial.println(ENCODER_1_SW);

  Serial.print("Encoder 2: CLK=");
  Serial.print(ENCODER_2_CLK);
  Serial.print(" DT=");
  Serial.print(ENCODER_2_DT);
  Serial.print(" SW=");
  Serial.println(ENCODER_2_SW);
}

void EncoderHandler::update() {
  if (!USE_ENCODERS) return;

  updateButtons();

  // Calculate speed for acceleration
  unsigned long now = millis();
  if (now - encoder1LastChange > 500) {
    encoder1Speed = 0;
  }
  if (now - encoder2LastChange > 500) {
    encoder2Speed = 0;
  }
}

void EncoderHandler::updateButtons() {
  // Read encoder 1 button
  bool button1 = digitalRead(ENCODER_1_SW);
  if (button1 == LOW && encoder1LastButton == HIGH) {
    if (millis() - encoder1ButtonTime > 50) { // Debounce
      encoder1ButtonPressed = true;
      encoder1ButtonTime = millis();
      Serial.println("Encoder 1 button pressed");
    }
  } else {
    encoder1ButtonPressed = false;
  }
  encoder1LastButton = button1;

  // Read encoder 2 button
  bool button2 = digitalRead(ENCODER_2_SW);
  if (button2 == LOW && encoder2LastButton == HIGH) {
    if (millis() - encoder2ButtonTime > 50) { // Debounce
      encoder2ButtonPressed = true;
      encoder2ButtonTime = millis();
      Serial.println("Encoder 2 button pressed");
    }
  } else {
    encoder2ButtonPressed = false;
  }
  encoder2LastButton = button2;
}

// ISR for encoder 1
void EncoderHandler::encoder1ISR() {
  if (instance) {
    instance->handleEncoder1();
  }
}

// ISR for encoder 2
void EncoderHandler::encoder2ISR() {
  if (instance) {
    instance->handleEncoder2();
  }
}

void EncoderHandler::handleEncoder1() {
  unsigned long now = millis();
  if (now - encoder1LastInterrupt < ENCODER_DEBOUNCE_MS) return;
  encoder1LastInterrupt = now;

  // Read direction
  int direction = digitalRead(ENCODER_1_DT) ? 1 : -1;

  // Calculate acceleration
  if (ENCODER_ACCELERATION) {
    unsigned long timeDiff = now - encoder1LastChange;
    if (timeDiff < 100) {
      encoder1Speed = min(encoder1Speed + 1, 10);
    } else {
      encoder1Speed = 1;
    }
    encoder1Pos += direction * encoder1Speed;
  } else {
    encoder1Pos += direction;
  }

  encoder1LastChange = now;
}

void EncoderHandler::handleEncoder2() {
  unsigned long now = millis();
  if (now - encoder2LastInterrupt < ENCODER_DEBOUNCE_MS) return;
  encoder2LastInterrupt = now;

  // Read direction
  int direction = digitalRead(ENCODER_2_DT) ? 1 : -1;

  // Calculate acceleration
  if (ENCODER_ACCELERATION) {
    unsigned long timeDiff = now - encoder2LastChange;
    if (timeDiff < 100) {
      encoder2Speed = min(encoder2Speed + 1, 10);
    } else {
      encoder2Speed = 1;
    }
    encoder2Pos += direction * encoder2Speed;
  } else {
    encoder2Pos += direction;
  }

  encoder2LastChange = now;
}

float EncoderHandler::getVirtualRotation() const {
  // Map encoder position to 0-100 range
  float virtualRot = constrain(encoder2Pos * VIRTUAL_ROTATION_STEP, 0, VIRTUAL_ROTATION_MAX);
  return virtualRot;
}

float EncoderHandler::getEffectIntensity() const {
  // Map encoder position to 0-1 range for effect intensity
  float intensity = getVirtualRotation() / VIRTUAL_ROTATION_MAX;
  return constrain(intensity, 0.0, 1.0);
}

int EncoderHandler::getModeOffset() const {
  // Return how many modes to shift (positive or negative)
  // This will be used to change animation modes
  return encoder1Pos;
}