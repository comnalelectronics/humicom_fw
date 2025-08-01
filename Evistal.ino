#define ledPinRgb1  32
#define ledPinRgb2  33
#define ledPinRgb3  27
#define diffuserPin 12
#define buttonControl 26

unsigned long buttonPressStart = 0;
bool lastButtonState = HIGH;

int shortPressMode = 0;
int longPressMode = 0;
bool justPressed = false;

void setup() {
  pinMode(diffuserPin, OUTPUT);
  digitalWrite(diffuserPin, LOW); // Başlangıçta kapalı

  pinMode(ledPinRgb1, OUTPUT);
  pinMode(ledPinRgb2, OUTPUT);
  pinMode(ledPinRgb3, OUTPUT);

  pinMode(buttonControl, INPUT_PULLUP);
}

void loop() {
  handleButtonPress();        // 👈 buton kontrolü
  handleShortPressModes();    // 👈 difüzör kontrolü
  handleLongPressModes();     // 👈 RGB kontrolü
}

// === Buton Basma Kontrolü ===
void handleButtonPress() {
  bool buttonState = digitalRead(buttonControl);

  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressStart = millis();
  }

  if (buttonState == HIGH && lastButtonState == LOW) {
    unsigned long pressDuration = millis() - buttonPressStart;

    if (pressDuration >= 2000) {
      longPressMode = (longPressMode + 1) % 4;
    } else if (pressDuration <= 500) {
      shortPressMode = (shortPressMode + 1) % 3;
    }

    justPressed = true;
  }

  lastButtonState = buttonState;
}

// === Difüzör Modları ===
void handleShortPressModes() {
  static unsigned long lastToggleTime = 0;
  static bool isOn = false;
  static int lastMode = -1;

  if (shortPressMode != lastMode || justPressed) {
    lastToggleTime = millis();
    isOn = false;
    digitalWrite(diffuserPin, LOW);
    lastMode = shortPressMode;
    justPressed = false;
  }

  unsigned long currentMillis = millis();

  switch (shortPressMode) {
    case 0:
      digitalWrite(diffuserPin, LOW); // Kapalı
      break;

    case 1:
      digitalWrite(diffuserPin, HIGH); // Sabit açık
      break;

    case 2:
      if (isOn && currentMillis - lastToggleTime >= 3000) {
        digitalWrite(diffuserPin, LOW);
        isOn = false;
        lastToggleTime = currentMillis;
      } else if (!isOn && currentMillis - lastToggleTime >= 2000) {
        digitalWrite(diffuserPin, HIGH);
        isOn = true;
        lastToggleTime = currentMillis;
      }
      break;
  }
}

// === RGB Modları ===
void handleLongPressModes() {
  static unsigned long lastColorChange = 0;
  static int currentColor = 0;
  static int lastMode = -1;
  static unsigned long rainbowTimer = 0;
  static int step = 0;

  if (longPressMode != lastMode || justPressed) {
    lastColorChange = millis();
    currentColor = 0;
    rainbowTimer = millis();
    step = 0;
    lastMode = longPressMode;
    justPressed = false;
  }

  switch (longPressMode) {
    case 0:
      setRGBColor(false, false, false); // Kapalı
      break;

    case 1: // Rainbow
      if (millis() - rainbowTimer > 300) {
        switch (step) {
          case 0: setRGBColor(true, false, false); break;
          case 1: setRGBColor(true, true, false); break;
          case 2: setRGBColor(false, true, false); break;
          case 3: setRGBColor(false, true, true); break;
          case 4: setRGBColor(false, false, true); break;
          case 5: setRGBColor(true, false, true); break;
          case 6: setRGBColor(true, true, true); break;
        }
        step = (step + 1) % 7;
        rainbowTimer = millis();
      }
      break;

    case 2:
      if (millis() - lastColorChange > 1000) {
        currentColor = (currentColor + 1) % 3;
        setRGBColor(currentColor == 0, currentColor == 1, currentColor == 2);
        lastColorChange = millis();
      }
      break;

    case 3:
      setRGBColor(true, true, true); // Beyaz
      break;
  }
}

void setRGBColor(bool r, bool g, bool b) {
  digitalWrite(ledPinRgb1, r ? HIGH : LOW);
  digitalWrite(ledPinRgb2, g ? HIGH : LOW);
  digitalWrite(ledPinRgb3, b ? HIGH : LOW);
}
