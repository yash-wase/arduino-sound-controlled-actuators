/*
  Clap-Based LED Control
  ----------------------
  Hardware:
    - Arduino Uno
    - HL-009 Sound Sensor (Digital OUT pin used)
    - Potentiometer (onboard on HL-009 for sensitivity adjustment)
    - LED with 220-ohm resistor

  Behavior:
    - First clap  → LED turns ON
    - Second clap → LED turns OFF
    - Potentiometer on HL-009 adjusts the clap detection threshold

  Pin Connections:
    - HL-009 DO  → Digital Pin 8
    - LED +ve    → Digital Pin 13 (built-in LED or external)
    - GND        → GND
    - VCC        → 5V
*/

// ── Pin Definitions ──────────────────────────────────────────
const int SOUND_PIN = 2;   
const int LED_PIN   = 13;  

// ── State Variables ──────────────────────────────────────────
bool ledState       = false;  
bool lastSoundState = HIGH;  

// ── Debounce Config ──────────────────────────────────────────
unsigned long lastClapTime  = 0;
const unsigned long DEBOUNCE_MS = 300;  

void setup() {
  pinMode(SOUND_PIN, INPUT);   
  pinMode(LED_PIN, OUTPUT);    
  digitalWrite(LED_PIN, LOW);  
  Serial.begin(9600);
  Serial.println("Clap LED Control Ready.");
  Serial.println("Clap once to turn ON, clap again to turn OFF.");
}

void loop() {
  bool currentSoundState = digitalRead(SOUND_PIN);
  unsigned long currentTime = millis();

  // Detect falling edge: HIGH → LOW means sound detected
  if (lastSoundState == HIGH && currentSoundState == LOW) {

    // Debounce: ignore if too soon after last clap
    if (currentTime - lastClapTime > DEBOUNCE_MS) {

      // Toggle LED state
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);

      // Log to Serial Monitor
      Serial.print("Clap detected! LED is now: ");
      Serial.println(ledState ? "ON" : "OFF");

      lastClapTime = currentTime; 
    }
  }

  lastSoundState = currentSoundState;  
}
