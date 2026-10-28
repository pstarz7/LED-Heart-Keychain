/*
  16-LED Heart Controller — ULTIMATE EDITION
  Board: Arduino Nano (ATmega328P)
  LEDs: 16 | Button: D2 to GND | Modes: 16

  Mode Map (ordered for maximum visual impact):
   1. Standby OFF
   2. Breathing Glow        ← smooth PWM fade
   3. Waterfall Drop        ← classic sweep
   4. Ripple Heartbeat      ← center-out double-beat
   5. Comet Tail            ← clockwise with fading trail
   6. Zebra Morph           ← alternating with speed warp
   7. Candle Flicker        ← random warm shimmer
   8. Accelerating Chase    ← dot speeds up dramatically
   9. Pulsing Rings         ← continuous outward ripples
  10. Build & Burst         ← fill then explode
  11. Power Up              ← bottom-to-top brightening
  12. Dual Converge         ← two dots meet and flash
  13. Rain Drops            ← falling drops
  14. ECG Monitor           ← heartbeat waveform trace
  15. Shatter & Rebuild     ← explode then reform
  16. All ON Steady         ← full glow finale

  Hardware: Same as original. No changes needed.
*/

// ====================================================
// 1. PIN DEFINITIONS (UNCHANGED)
// ====================================================

const byte buttonPin = 2;
const byte topLed    = A5;
const byte bottomLed = A2;

const byte leftSide[]  = {11, 7, 8, 9, 6, 5, 12};
const byte rightSide[] = {13, A4, A3, 10, 4, A1, A0};

const byte clockwisePath[] = {
  A5, 13, A4, A3, 10, 4, A1, A0,
  A2, 12, 5, 6, 9, 8, 7, 11
};

const byte allLeds[] = {
  A2, 4, 5, 6, 7, 8, 9, 10,
  11, 12, 13, A0, A1, A3, A4, A5
};

const byte NUM_LEDS  = 16;
const byte NUM_MODES = 16;

// ====================================================
// 2. STATE
// ====================================================

byte stage = 1;
bool modeChanged = false;

byte lastReading = HIGH;
byte stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceMs = 30;

// ====================================================
// 3. SETUP
// ====================================================

void setup() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    pinMode(allLeds[i], OUTPUT);
    digitalWrite(allLeds[i], LOW);
  }
  pinMode(buttonPin, INPUT_PULLUP);
  randomSeed(analogRead(A6));
}

// ====================================================
// 4. MAIN LOOP
// ====================================================

void loop() {
  modeChanged = false;

  switch (stage) {
    case  1: mode1();  break;
    case  2: mode2();  break;
    case  3: mode3();  break;
    case  4: mode4();  break;
    case  5: mode5();  break;
    case  6: mode6();  break;
    case  7: mode7();  break;
    case  8: mode8();  break;
    case  9: mode9();  break;
    case 10: mode10(); break;
    case 11: mode11(); break;
    case 12: mode12(); break;
    case 13: mode13(); break;
    case 14: mode14(); break;
    case 15: mode15(); break;
    case 16: mode16(); break;
    default: stage = 1; break;
  }
}

// ====================================================
// 5. BUTTON (POLLING — NO INTERRUPT)
// ====================================================

void checkButton() {
  byte reading = digitalRead(buttonPin);
  if (reading != lastReading) {
    lastDebounceTime = millis();
    lastReading = reading;
  }
  if (millis() - lastDebounceTime >= debounceMs) {
    if (reading != stableButtonState) {
      stableButtonState = reading;
      if (stableButtonState == LOW) {
        stage++;
        if (stage > NUM_MODES) stage = 1;
        modeChanged = true;
      }
    }
  }
}

void smartDelay(unsigned long duration) {
  unsigned long start = millis();
  while (millis() - start < duration && !modeChanged) {
    checkButton();
    delay(1);
  }
}

// ====================================================
// 6. HELPERS
// ====================================================

void turnAllOff() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    // analogWrite(0) clears any lingering PWM state
    analogWrite(allLeds[i], 0);
  }
}

void turnAllOn() {
  for (byte i = 0; i < NUM_LEDS; i++) {
    digitalWrite(allLeds[i], HIGH);
  }
}

// Returns true if the pin supports hardware PWM on Nano.
bool isPWM(byte pin) {
  return (pin == 5 || pin == 6 || pin == 9 ||
          pin == 10 || pin == 11);
}

// Smooth brightness for PWM pins, threshold for digital pins.
void setBright(byte pin, byte val) {
  if (isPWM(pin)) {
    analogWrite(pin, val);
  } else {
    digitalWrite(pin, val > 100 ? HIGH : LOW);
  }
}

// Turn on a horizontal level (0 = top, 1-7 = sides, 8 = bottom).
void lightLevel(byte level) {
  if (level == 0)      digitalWrite(topLed, HIGH);
  else if (level <= 7) {
    byte idx = level - 1;
    digitalWrite(leftSide[idx], HIGH);
    digitalWrite(rightSide[idx], HIGH);
  }
  else if (level == 8) digitalWrite(bottomLed, HIGH);
}

// Set brightness for an entire level.
void setLevelBright(byte level, byte val) {
  if (level == 0)      setBright(topLed, val);
  else if (level <= 7) {
    byte idx = level - 1;
    setBright(leftSide[idx], val);
    setBright(rightSide[idx], val);
  }
  else if (level == 8) setBright(bottomLed, val);
}

// ====================================================
// MODE 1 — STANDBY OFF
// ====================================================
void mode1() {
  turnAllOff();
  while (!modeChanged) smartDelay(10);
}

// ====================================================
// MODE 2 — BREATHING GLOW
// Entire heart fades in and out smoothly.
// PWM pins fade continuously; digital pins snap at
// the midpoint, creating a "fill then intensify" look.
// ====================================================
void mode2() {
  turnAllOff();

  while (!modeChanged) {
    // Fade IN  (0 → 255)
    for (int b = 0; b <= 255; b += 3) {
      for (byte lv = 0; lv <= 8; lv++) setLevelBright(lv, b);
      smartDelay(18);
      if (modeChanged) return;
    }
    // Fade OUT (255 → 0)
    for (int b = 255; b >= 0; b -= 3) {
      for (byte lv = 0; lv <= 8; lv++) setLevelBright(lv, b);
      smartDelay(18);
      if (modeChanged) return;
    }
    smartDelay(300);
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 3 — WATERFALL DROP
// Single level sweeps top → bottom, clean ON/OFF.
// ====================================================
void mode3() {
  while (!modeChanged) {
    for (byte i = 0; i <= 8; i++) {
      turnAllOff();
      lightLevel(i);
      smartDelay(120);
      if (modeChanged) return;
    }
  }
}

// ====================================================
// MODE 4 — RIPPLE HEARTBEAT
// Realistic double-beat (lub-dub) expanding from the
// center of the heart outward.
//
// "lub"  = strong pulse, full expansion
// "dub"  = weaker pulse, partial expansion
// pause  = rest
// ====================================================
void mode4() {
  while (!modeChanged) {

    // ---- LUB (strong) ----
    // Center ring fires first, then expands outward
    for (byte ring = 0; ring <= 4; ring++) {
      turnAllOff();
      setLevelBright(4 - ring, 255);
      setLevelBright(4 + ring, 255);
      smartDelay(ring == 0 ? 100 : 70);
      if (modeChanged) return;
    }
    turnAllOff();
    smartDelay(120);
    if (modeChanged) return;

    // ---- DUB (weaker, shorter) ----
    for (byte ring = 0; ring <= 2; ring++) {
      turnAllOff();
      byte b = 255 - ring * 60;   // fading brightness
      setLevelBright(4 - ring, b);
      setLevelBright(4 + ring, b);
      smartDelay(60);
      if (modeChanged) return;
    }
    turnAllOff();
    smartDelay(550);   // rest between beats
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 5 — COMET TAIL
// One bright head LED travels clockwise with a 3-LED
// fading tail behind it. Looks like a comet orbiting
// the heart.
// ====================================================
void mode5() {
  const byte trailLen = 3;
  const byte trailBright[3] = {140, 70, 25};

  while (!modeChanged) {
    for (byte i = 0; i < NUM_LEDS; i++) {
      turnAllOff();

      // Head — full brightness
      setBright(clockwisePath[i], 255);

      // Tail — progressively dimmer
      for (byte t = 1; t <= trailLen; t++) {
        byte idx = (i + NUM_LEDS - t) % NUM_LEDS;
        setBright(clockwisePath[idx], trailBright[t - 1]);
      }

      smartDelay(70);
      if (modeChanged) return;
    }
  }
}

// ====================================================
// MODE 6 — ZEBRA MORPH
// Even/odd levels alternate, but the swap speed
// continuously accelerates then decelerates.
// Creates a "morphing" optical illusion.
// ====================================================
void mode6() {
  while (!modeChanged) {
    // Accelerate: 350ms → 80ms
    for (int d = 350; d >= 80; d -= 30) {
      turnAllOff();
      for (byte i = 0; i <= 8; i += 2) lightLevel(i);
      smartDelay(d);
      if (modeChanged) return;

      turnAllOff();
      for (byte i = 1; i <= 7; i += 2) lightLevel(i);
      smartDelay(d);
      if (modeChanged) return;
    }
    // Decelerate: 80ms → 350ms
    for (int d = 80; d <= 350; d += 30) {
      turnAllOff();
      for (byte i = 0; i <= 8; i += 2) lightLevel(i);
      smartDelay(d);
      if (modeChanged) return;

      turnAllOff();
      for (byte i = 1; i <= 7; i += 2) lightLevel(i);
      smartDelay(d);
      if (modeChanged) return;
    }
  }
}

// ====================================================
// MODE 7 — CANDLE FLICKER
// Bottom half flickers randomly like a candle flame.
// Top half glows steadily at low brightness.
// Warm, organic, alive.
// ====================================================
void mode7() {
  while (!modeChanged) {
    // Top 3 levels: steady dim glow
    setLevelBright(0, 70);
    setLevelBright(1, 60);
    setLevelBright(2, 50);

    // Bottom 6 levels: random flicker
    for (byte lv = 3; lv <= 8; lv++) {
      byte flicker = random(20, 220);
      setLevelBright(lv, flicker);
    }

    smartDelay(random(40, 100));
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 8 — ACCELERATING CHASE
// Single dot starts slow, rapidly accelerates around
// the heart, holds at max speed, then resets.
// Like a roller coaster.
// ====================================================
void mode8() {
  while (!modeChanged) {
    unsigned int spd = 250;   // start slow

    for (byte lap = 0; lap < 3; lap++) {
      for (byte i = 0; i < NUM_LEDS; i++) {
        turnAllOff();
        digitalWrite(clockwisePath[i], HIGH);
        smartDelay(spd);
        if (modeChanged) return;

        // Accelerate
        if (spd > 35) spd = (unsigned int)(spd * 0.88);
      }
    }

    // Hold at max speed for one lap
    for (byte i = 0; i < NUM_LEDS; i++) {
      turnAllOff();
      digitalWrite(clockwisePath[i], HIGH);
      smartDelay(35);
      if (modeChanged) return;
    }

    smartDelay(400);   // pause before reset
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 9 — PULSING RINGS
// Continuous concentric ripples expand outward from
// the center. Each ring leaves a fading trail.
// Hypnotic and mesmerizing.
// ====================================================
void mode9() {
  while (!modeChanged) {
    for (byte ring = 0; ring <= 4; ring++) {
      turnAllOff();

      // Current ring — bright
      setLevelBright(4 - ring, 255);
      setLevelBright(4 + ring, 255);

      // Previous ring — fading trail
      if (ring > 0) {
        setLevelBright(4 - ring + 1, 80);
        setLevelBright(4 + ring - 1, 80);
      }

      smartDelay(90);
      if (modeChanged) return;
    }
    smartDelay(120);
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 10 — BUILD & BURST
// LEDs fill clockwise one by one. When full, the
// heart explodes in 3 rapid flashes.
// ====================================================
void mode10() {
  while (!modeChanged) {
    turnAllOff();

    // Build: cumulative clockwise fill
    for (byte i = 0; i < NUM_LEDS; i++) {
      digitalWrite(clockwisePath[i], HIGH);
      smartDelay(70);
      if (modeChanged) return;
    }

    // Burst: 3 rapid full-heart flashes
    for (byte f = 0; f < 3; f++) {
      turnAllOff();
      smartDelay(60);
      if (modeChanged) return;
      turnAllOn();
      smartDelay(60);
      if (modeChanged) return;
    }

    smartDelay(350);
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 11 — POWER UP
// Heart fills from bottom to top with increasing
// brightness. Like a machine charging up.
// Ends with a bright flash.
// ====================================================
void mode11() {
  // Brightness per level: bottom dim → top bright
  const byte levelBright[9] = {
    255, 230, 200, 175, 150, 120, 90, 65, 45
  };
  // levelBright[0]=top(brightest), [8]=bottom(dim)

  while (!modeChanged) {
    turnAllOff();

    // Fill bottom → top
    for (int lv = 8; lv >= 0; lv--) {
      setLevelBright(lv, levelBright[lv]);
      smartDelay(130);
      if (modeChanged) return;
    }

    // Charged! Bright flash
    turnAllOn();
    smartDelay(250);
    if (modeChanged) return;

    turnAllOff();
    smartDelay(400);
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 12 — DUAL CONVERGE
// Two LEDs start at opposite points on the heart and
// race toward each other. When they meet → flash!
// ====================================================
void mode12() {
  while (!modeChanged) {
    for (byte i = 0; i < 8; i++) {
      turnAllOff();

      // LED 1: clockwise from top
      digitalWrite(clockwisePath[i], HIGH);
      // LED 2: counter-clockwise from bottom
      digitalWrite(clockwisePath[(16 - i) % 16], HIGH);

      smartDelay(100);
      if (modeChanged) return;
    }

    // They met! Flash
    turnAllOn();
    smartDelay(200);
    if (modeChanged) return;

    turnAllOff();
    smartDelay(300);
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 13 — RAIN DROPS
// Three "drops" fall from top to bottom at slightly
// different speeds. When a drop reaches the bottom,
// it respawns at the top.
// ====================================================
void mode13() {
  byte drops[3] = {0, 3, 6};

  while (!modeChanged) {
    turnAllOff();

    for (byte d = 0; d < 3; d++) {
      lightLevel(drops[d]);
    }

    smartDelay(110);
    if (modeChanged) return;

    // Move drops down at slightly varied rates
    for (byte d = 0; d < 3; d++) {
      // 70% chance of moving each frame → varied speeds
      if (random(10) < 7) {
        drops[d]++;
        if (drops[d] > 8) drops[d] = 0;
      }
    }
  }
}

// ====================================================
// MODE 14 — ECG MONITOR
// Traces a realistic ECG waveform vertically on the
// heart. Flatline → P-wave → QRS spike → T-wave.
// The most unique and thematic effect.
// ====================================================
void mode14() {
  while (!modeChanged) {

    // Flatline (baseline at center level 4)
    turnAllOff();
    lightLevel(4);
    smartDelay(400);
    if (modeChanged) return;

    // P-wave (small bump up)
    turnAllOff();
    lightLevel(3);
    smartDelay(80);
    if (modeChanged) return;

    // QRS complex (sharp spike!)
    // Q dip
    turnAllOff(); lightLevel(5);
    smartDelay(30); if (modeChanged) return;
    // R spike (tall, fast)
    turnAllOff(); lightLevel(2);
    smartDelay(25); if (modeChanged) return;
    turnAllOff(); lightLevel(1);
    smartDelay(25); if (modeChanged) return;
    turnAllOff(); lightLevel(0);
    smartDelay(25); if (modeChanged) return;
    // S dip
    turnAllOff(); lightLevel(6);
    smartDelay(30); if (modeChanged) return;

    // Return to baseline
    turnAllOff(); lightLevel(4);
    smartDelay(200);
    if (modeChanged) return;

    // T-wave (gentle bump)
    turnAllOff(); lightLevel(3);
    smartDelay(100);
    if (modeChanged) return;

    // Flatline again
    turnAllOff(); lightLevel(4);
    smartDelay(500);
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 15 — SHATTER & REBUILD
// Full heart → explodes outward from center →
// all dark → rebuilds inward → full heart.
// The most dramatic effect.
// ====================================================
void mode15() {
  while (!modeChanged) {

    // Full heart
    turnAllOn();
    smartDelay(500);
    if (modeChanged) return;

    // SHATTER: center → outward, LEDs die
    for (byte ring = 0; ring <= 4; ring++) {
      setLevelBright(4 - ring, 0);
      setLevelBright(4 + ring, 0);
      smartDelay(80);
      if (modeChanged) return;
    }

    // Darkness
    smartDelay(400);
    if (modeChanged) return;

    // REBUILD: outer → center, LEDs revive
    for (int ring = 4; ring >= 0; ring--) {
      setLevelBright(4 - ring, 255);
      setLevelBright(4 + ring, 255);
      smartDelay(100);
      if (modeChanged) return;
    }

    smartDelay(400);
    if (modeChanged) return;
  }
}

// ====================================================
// MODE 16 — ALL ON STEADY
// Full glow. The finale.
// ====================================================
void mode16() {
  turnAllOn();
  while (!modeChanged) smartDelay(10);
}