/*
  Clap-Based Servo Motor Control
  --------------------------------
  Hardware:
    - Arduino Uno
    - HL-009 Sound Sensor (Digital OUT pin used)
    - SG90 Servo Motor
    - Potentiometer (onboard on HL-009 for sensitivity adjustment)

  Behavior:
    - First clap  → Servo sweeps to 180°
    - Second clap → Servo sweeps back to 0°
    - Each subsequent clap toggles between 0° and 180°

  Pin Connections:
    - HL-009 DO     → Digital Pin 2
    - SG90 Signal   → Digital Pin 9 (PWM pin)
    - SG90 VCC      → 5V
    - SG90 GND      → GND
    - HL-009 VCC    → 5V
    - HL-009 GND    → GND

  Note:
    - If servo jitters, power it from external 5V supply (not Arduino 5V)
    - Adjust DEBOUNCE_MS if false triggers occur in noisy environments
*/

#include <Servo.h>

// ── Pin Definitions ──────────────────────────────────────────
const int SOUND_PIN = 2;   // HL-009 digital output
const int SERVO_PIN = 9;   // SG90 signal pin (must be PWM)

// ── Servo Config ─────────────────────────────────────────────
Servo myServo;
const int POS_START = 0;    // Starting position
const int POS_END   = 180;  // End position
const int SWEEP_DELAY = 15; // Delay (ms) between each degree — controls sweep speed

// ── State Variables ──────────────────────────────────────────
bool servoState     = false; // false = at 0°, true = at 180°
bool lastSoundState = HIGH;  // Previous sensor reading

// ── Debounce Config ──────────────────────────────────────────
unsigned long lastClapTime  = 0;
const unsigned long DEBOUNCE_MS = 300; // Ignore re-triggers within 300ms

// ────────────────────────────────────────────────────────────
void setup() {
  pinMode(SOUND_PIN, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(POS_START);   // Initialize servo at 0°

  Serial.begin(9600);
  Serial.println("Clap Servo Control Ready.");
  Serial.println("Clap to sweep servo between 0° and 180°.");
}

// ── Smooth Sweep Function ────────────────────────────────────
void sweepTo(int fromAngle, int toAngle) {
  if (fromAngle < toAngle) {
    // Sweep forward: 0° → 180°
    for (int angle = fromAngle; angle <= toAngle; angle++) {
      myServo.write(angle);
      delay(SWEEP_DELAY);
    }
  } else {
    // Sweep backward: 180° → 0°
    for (int angle = fromAngle; angle >= toAngle; angle--) {
      myServo.write(angle);
      delay(SWEEP_DELAY);
    }
  }
}

// ────────────────────────────────────────────────────────────
void loop() {
  bool currentSoundState = digitalRead(SOUND_PIN);
  unsigned long currentTime = millis();

  // Detect falling edge: HIGH → LOW means sound/clap detected
  if (lastSoundState == HIGH && currentSoundState == LOW) {

    // Debounce: ignore if too soon after last clap
    if (currentTime - lastClapTime > DEBOUNCE_MS) {

      // Toggle servo state
      servoState = !servoState;

      if (servoState) {
        Serial.println("Clap! Sweeping to 180°...");
        sweepTo(POS_START, POS_END);
        Serial.println("Reached 180°.");
      } else {
        Serial.println("Clap! Sweeping back to 0°...");
        sweepTo(POS_END, POS_START);
        Serial.println("Reached 0°.");
      }

      lastClapTime = currentTime; // Update last clap time
    }
  }

  lastSoundState = currentSoundState; // Update previous state
}
