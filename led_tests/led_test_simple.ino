// SIMPLE LED TEST - Minimal code to verify LEDs work
// This will light up the first 10 LEDs in red

#include <FastLED.h>

#define LED_PIN 4
#define NUM_LEDS 209

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=================================");
  Serial.println("    SIMPLE LED TEST");
  Serial.println("=================================");
  Serial.println();
  Serial.println("This should light the first 10 LEDs in RED");
  Serial.println();

  // Initialize LEDs - try both protocols
  Serial.println("Initializing with WS2812B protocol, GRB order...");
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  // Set safe brightness
  FastLED.setBrightness(20);

  // Clear all LEDs first
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(500);

  Serial.println("Setting first 10 LEDs to RED...");

  // Light up first 10 LEDs in red
  for(int i = 0; i < 10; i++) {
    leds[i] = CRGB::Red;
    Serial.print("LED ");
    Serial.print(i);
    Serial.println(" set to RED");
  }

  Serial.println("Calling FastLED.show()...");
  FastLED.show();

  Serial.println("\nIf you see RED LEDs: Hardware is working!");
  Serial.println("If no LEDs are lit, check:");
  Serial.println("  1. External 5V power connected to LED strip?");
  Serial.println("  2. LED strip GND connected to Arduino GND?");
  Serial.println("  3. Data pin 4 connected to LED DIN?");
  Serial.println("  4. Power supply turned on?");
}

void loop() {
  // Flash the first LED to show we're running
  static unsigned long lastBlink = 0;
  static bool ledOn = true;

  if (millis() - lastBlink > 1000) {
    if (ledOn) {
      leds[0] = CRGB::Blue;
      Serial.println("LED 0: BLUE");
    } else {
      leds[0] = CRGB::Red;
      Serial.println("LED 0: RED");
    }
    FastLED.show();
    ledOn = !ledOn;
    lastBlink = millis();
  }
}