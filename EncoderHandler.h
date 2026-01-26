#ifndef ENCODER_HANDLER_H
#define ENCODER_HANDLER_H

#include <Arduino.h>
#include "Config.h"

class EncoderHandler {
public:
  EncoderHandler();

  void begin();
  void update();

  // Encoder 1 - Mode control
  int getEncoder1Position() const { return encoder1Pos; }
  bool getEncoder1Button() const { return encoder1ButtonPressed; }
  void resetEncoder1() { encoder1Pos = 0; }

  // Encoder 2 - Virtual rotation/effect control
  int getEncoder2Position() const { return encoder2Pos; }
  bool getEncoder2Button() const { return encoder2ButtonPressed; }
  void resetEncoder2() { encoder2Pos = 0; }

  // Virtual rotation value (0-100)
  float getVirtualRotation() const;

  // Effect intensity (0-1)
  float getEffectIntensity() const;

  // Animation mode control
  int getModeOffset() const;  // How many modes to shift from current
  bool shouldResetMode() const { return encoder1ButtonPressed; }

private:
  // Encoder positions
  volatile int encoder1Pos;
  volatile int encoder2Pos;

  // Button states
  bool encoder1ButtonPressed;
  bool encoder2ButtonPressed;
  bool encoder1LastButton;
  bool encoder2LastButton;

  // Timing for debounce
  unsigned long encoder1LastInterrupt;
  unsigned long encoder2LastInterrupt;
  unsigned long encoder1ButtonTime;
  unsigned long encoder2ButtonTime;

  // Acceleration detection
  unsigned long encoder1LastChange;
  unsigned long encoder2LastChange;
  int encoder1Speed;
  int encoder2Speed;

  // Static callback functions for interrupts
  static void encoder1ISR();
  static void encoder2ISR();

  // Static reference to instance for ISR
  static EncoderHandler* instance;

  // Helper functions
  void handleEncoder1();
  void handleEncoder2();
  void updateButtons();
  int calculateAcceleration(int baseChange, int speed);
};

#endif