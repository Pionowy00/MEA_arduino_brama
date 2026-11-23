
#include <Servo.h>

// ==========================================
// PINY
// ==========================================

#define LED_PIN 7
#define SERVO_LEFT_PIN 9
#define SERVO_RIGHT_PIN 8
#define PIR_PIN 12
#define BUTTON_PIN 10

// ==========================================
// USTAWIENIA
// ==========================================

#define OPEN_TIME 10000UL
#define CLOSE_TIME 10000UL
#define BLINK_TIME 400UL
#define DEBOUNCE_TIME 50UL
#define UPDATE_TIME 20UL

#define LEFT_CLOSED 90
#define LEFT_OPEN 0

#define RIGHT_CLOSED 90
#define RIGHT_OPEN 180

// ==========================================
// STANY
// ==========================================

#define OTWARTE 0
#define ZAMKNIETE 1
#define ZAMYKA_SIE 2
#define OTWIERA_SIE 3
#define ZATRZYMANA_PRZY_OTWARCIU 4
#define ZATRZYMANA_PRZY_ZAMKNIECIU 5

// ==========================================
// OBIEKTY
// ==========================================

Servo servoLeft;
Servo servoRight;

// ==========================================
// ZMIENNE
// ==========================================

int stan = ZAMKNIETE;

float leftPosition = LEFT_CLOSED;
float rightPosition = RIGHT_CLOSED;

float leftStartPosition = LEFT_CLOSED;
float rightStartPosition = RIGHT_CLOSED;

unsigned long movementStartTime = 0;
unsigned long lastServoUpdate = 0;
unsigned long lastBlinkTime = 0;
unsigned long lastDebounceTime = 0;

bool ledState = false;

bool buttonState = HIGH;
bool lastButtonReading = HIGH;
bool buttonPressed = false;

// ==========================================
// NAZWY STANOW
// ==========================================

const char* getStateName(int s) {

  switch (s) {

    case OTWARTE: return "OTWARTE";
    case ZAMKNIETE: return "ZAMKNIETE";
    case ZAMYKA_SIE: return "ZAMYKA SIE";
    case OTWIERA_SIE: return "OTWIERA SIE";
    case ZATRZYMANA_PRZY_OTWARCIU:
      return "ZATRZYMANA PRZY OTWARCIU";
    case ZATRZYMANA_PRZY_ZAMKNIECIU:
      return "ZATRZYMANA PRZY ZAMKNIECIU";
  }

  return "NIEZNANY";
}

// ==========================================
// ZMIANA STANU
// ==========================================

void setState(int newState) {

  if (stan == newState) return;

  Serial.print("[STAN] ");
  Serial.print(getStateName(stan));
  Serial.print(" -> ");
  Serial.println(getStateName(newState));

  stan = newState;

  digitalWrite(LED_PIN, LOW);
  ledState = false;
  lastBlinkTime = millis();

  if (stan == OTWIERA_SIE || stan == ZAMYKA_SIE) {

    movementStartTime = millis();
    lastServoUpdate = 0;

    leftStartPosition = leftPosition;
    rightStartPosition = rightPosition;

    Serial.println("[RUCH] Rozpoczynam ruch.");
  }

  if (stan == OTWARTE) {

    leftPosition = LEFT_OPEN;
    rightPosition = RIGHT_OPEN;

    servoLeft.write(LEFT_OPEN);
    servoRight.write(RIGHT_OPEN);

    Serial.println("[RUCH] Brama otwarta.");
  }

  if (stan == ZAMKNIETE) {

    leftPosition = LEFT_CLOSED;
    rightPosition = RIGHT_CLOSED;

    servoLeft.write(LEFT_CLOSED);
    servoRight.write(RIGHT_CLOSED);

    Serial.println("[RUCH] Brama zamknieta.");
  }
}

// ==========================================
// PRZYCISK
// ==========================================

void readButton() {

  buttonPressed = false;

  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime >= DEBOUNCE_TIME) {

    if (reading != buttonState) {

      buttonState = reading;

      if (buttonState == LOW) {

        buttonPressed = true;
        Serial.println("[BUTTON] Wcisnieto przycisk.");
      }
    }
  }

  lastButtonReading = reading;
}

// ==========================================
// LED
// ==========================================

void blinkLED() {

  if (millis() - lastBlinkTime >= BLINK_TIME) {

    lastBlinkTime = millis();

    ledState = !ledState;

    digitalWrite(LED_PIN, ledState);
  }
}

// ==========================================
// SERWA
// ==========================================

void updateServos(bool opening) {

  unsigned long currentTime = millis();

  if (currentTime - lastServoUpdate < UPDATE_TIME) {
    return;
  }

  lastServoUpdate = currentTime;

  unsigned long duration = opening ? OPEN_TIME : CLOSE_TIME;

  unsigned long elapsed = currentTime - movementStartTime;

  float progress = (float)elapsed / duration;

  if (progress > 1.0) {
    progress = 1.0;
  }

  int leftTarget = opening ? LEFT_OPEN : LEFT_CLOSED;
  int rightTarget = opening ? RIGHT_OPEN : RIGHT_CLOSED;

  leftPosition = leftStartPosition +
    (leftTarget - leftStartPosition) * progress;

  rightPosition = rightStartPosition +
    (rightTarget - rightStartPosition) * progress;

  servoLeft.write((int)(leftPosition + 0.5));
  servoRight.write((int)(rightPosition + 0.5));
}

// ==========================================
// STAN: OTWARTE
// ==========================================

void stateOTWARTE() {

  digitalWrite(LED_PIN, LOW);

  if (buttonPressed) {

    Serial.println("[INFO] Rozpoczynam zamykanie.");

    setState(ZAMYKA_SIE);
  }
}

// ==========================================
// STAN: ZAMKNIETE
// ==========================================

void stateZAMKNIETE() {

  digitalWrite(LED_PIN, LOW);

  if (buttonPressed) {

    Serial.println("[INFO] Rozpoczynam otwieranie.");

    setState(OTWIERA_SIE);
  }
}

// ==========================================
// STAN: ZAMYKA SIE
// ==========================================

void stateZAMYKA_SIE() {

  blinkLED();

  if (digitalRead(PIR_PIN) == HIGH) {

    Serial.println("[PIR] Wykryto ruch! Otwieranie.");

    updateServos(false);

    setState(OTWIERA_SIE);
    return;
  }

  if (buttonPressed) {

    Serial.println("[BUTTON] Zatrzymano zamykanie.");

    updateServos(false);

    setState(ZATRZYMANA_PRZY_ZAMKNIECIU);
    return;
  }

  updateServos(false);

  if (millis() - movementStartTime >= CLOSE_TIME) {
    setState(ZAMKNIETE);
  }
}

// ==========================================
// STAN: OTWIERA SIE
// ==========================================

void stateOTWIERA_SIE() {

  blinkLED();

  if (buttonPressed) {

    Serial.println("[BUTTON] Zatrzymano otwieranie.");

    updateServos(true);

    setState(ZATRZYMANA_PRZY_OTWARCIU);
    return;
  }

  updateServos(true);

  if (millis() - movementStartTime >= OPEN_TIME) {
    setState(OTWARTE);
  }
}

// ==========================================
// STAN: ZATRZYMANA PRZY OTWARCIU
// ==========================================

void stateZATRZYMANA_PRZY_OTWARCIU() {

  digitalWrite(LED_PIN, LOW);

  if (buttonPressed) {

    Serial.println("[BUTTON] Zmiana kierunku na zamykanie.");

    setState(ZAMYKA_SIE);
  }
}

// ==========================================
// STAN: ZATRZYMANA PRZY ZAMKNIECIU
// ==========================================

void stateZATRZYMANA_PRZY_ZAMKNIECIU() {

  digitalWrite(LED_PIN, LOW);

  if (digitalRead(PIR_PIN) == HIGH) {

    Serial.println("[PIR] Wykryto ruch! Otwieranie.");

    setState(OTWIERA_SIE);
    return;
  }

  if (buttonPressed) {

    Serial.println("[BUTTON] Zmiana kierunku na otwieranie.");

    setState(OTWIERA_SIE);
  }
}

// ==========================================
// SETUP
// ==========================================

void setup() {

  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  servoLeft.attach(SERVO_LEFT_PIN);
  servoRight.attach(SERVO_RIGHT_PIN);

  servoLeft.write(LEFT_CLOSED);
  servoRight.write(RIGHT_CLOSED);

  digitalWrite(LED_PIN, LOW);

  Serial.println("==============================");
  Serial.println(" SYSTEM BRAMY URUCHOMIONY");
  Serial.println("==============================");
  Serial.println("[INIT] Stan: ZAMKNIETE");
}

// ==========================================
// LOOP
// ==========================================

void loop() {

  readButton();

  switch (stan) {

    case OTWARTE:
      stateOTWARTE();
      break;

    case ZAMKNIETE:
      stateZAMKNIETE();
      break;

    case ZAMYKA_SIE:
      stateZAMYKA_SIE();
      break;

    case OTWIERA_SIE:
      stateOTWIERA_SIE();
      break;

    case ZATRZYMANA_PRZY_OTWARCIU:
      stateZATRZYMANA_PRZY_OTWARCIU();
      break;

    case ZATRZYMANA_PRZY_ZAMKNIECIU:
      stateZATRZYMANA_PRZY_ZAMKNIECIU();
      break;
  }
}
