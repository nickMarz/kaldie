// MINIMAL LED TEST - Brightness 10
// Safe to run on Arduino power temporarily

#include <FastLED.h>

#define LED_PIN 4
#define NUM_LEDS 209

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(2000);  // Extra delay for serial monitor

  Serial.println("=====================================");
  Serial.println("  MINIMAL POWER LED TEST");
  Serial.println("  Brightness: 10/255 (safe)");
  Serial.println("=====================================\n");

  // Try WS2812B with GRB
  Serial.println("Initializing LEDs...");
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);  // Very low brightness

  // Clear all
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(500);

  // Light pattern for testing
  Serial.println("\nLighting test pattern:");
  Serial.println("  LED 0: RED");
  Serial.println("  LED 1: GREEN");
  Serial.println("  LED 2: BLUE");
  Serial.println("  LED 3: WHITE");
  Serial.println("  LEDs 4-9: YELLOW\n");

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Blue;
  leds[3] = CRGB::White;
  for(int i = 4; i < 10; i++) {
    leds[i] = CRGB::Yellow;
  }

  Serial.println("Sending data to LEDs...");
  FastLED.show();

  Serial.println("\n✓ Setup complete!");
  Serial.println("\nIf you see NO LEDs:");
  Serial.println("1. Check: LED DIN → Arduino Pin 4");
  Serial.println("2. Check: LED GND → Arduino GND");
  Serial.println("3. Check: LED 5V → Power Supply (or Arduino 5V for test)");
  Serial.println("\nPower at brightness 10:");
  Serial.println("  ~0.4-0.8A total (safe for testing)");
}

void loop() {
  // Cycle first LED through colors slowly
  static uint8_t hue = 0;
  leds[0] = CHSV(hue++, 255, 255);
  FastLED.show();

  // Debug output every second
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    Serial.print("Running... First LED HSV: ");
    Serial.print(hue);
    Serial.print(" | RGB: ");
    Serial.print(leds[0].r);
    Serial.print(",");
    Serial.print(leds[0].g);
    Serial.print(",");
    Serial.println(leds[0].b);
    lastPrint = millis();
  }

  delay(20);
}