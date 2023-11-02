#include <M5Stack.h>
#include <cmath>

#include "ble.h"
#include "phoenixwan.h"

USB Usb;
PhoenixWanUSB PhoenixWan(&Usb);
BeatbleBLEServer *BleServer;

namespace {
namespace color {
const uint32_t background = TFT_DARKGREY;
const uint32_t scratch = TFT_BLACK;
const uint32_t scratchAngle = RED;
const uint32_t frontButton = TFT_WHITE;
const uint32_t frontButtonPressed = TFT_BLUE;
const uint32_t backButton = TFT_BLACK;
const uint32_t backButtonPressed = TFT_BLUE;
const uint32_t optionButton = TFT_WHITE;
const uint32_t optionButtonPressed = TFT_RED;
const uint32_t text = TFT_RED;
} // namespace color
} // namespace

static bool isRight = true;
static int32_t buttonBaseX = 160;
static int32_t scratchBaseX = 80;
void togglePlaySide() {
  auto nextIsRight = !isRight;
  isRight = nextIsRight;

  if (nextIsRight) {
    buttonBaseX = 160;
    scratchBaseX = 80;
  } else {
    buttonBaseX = 10;
    scratchBaseX = 240;
  }
}

void clearPlayArea() { M5.Lcd.fillRect(0, 45, 320, 151, color::background); }

void updateScratch() {
  M5.Lcd.fillCircle(scratchBaseX, 120, 70, color::scratch);
}

void drawAngleLine(uint8_t angle) {
  double theta = (M_PI / 128) * angle;
  auto r = 70;
  auto dx = (int32_t)(r * std::cos(theta));
  auto dy = (int32_t)(-r * std::sin(theta));
  auto bx = scratchBaseX;

  M5.Lcd.drawLine(bx, 120, bx + dx, 120 + dy, color::scratchAngle);
}

void updateOptionButtons(bool e1, bool e2, bool e3, bool e4) {
  auto option = color::optionButton;
  auto pressed = color::optionButtonPressed;

  auto bx = buttonBaseX;
  auto w = 29;
  auto r = 2;

  M5.Lcd.fillRoundRect(bx + 0, 45, w, w, r, e1 ? pressed : option);
  M5.Lcd.fillRoundRect(bx + 41, 45, w, w, r, e2 ? pressed : option);
  M5.Lcd.fillRoundRect(bx + 82, 45, w, w, r, e3 ? pressed : option);
  M5.Lcd.fillRoundRect(bx + 123, 45, w, w, r, e4 ? pressed : option);
}

void updateButtons(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6,
                   bool b7) {
  auto front = color::frontButton;
  auto frontPressed = color::frontButtonPressed;
  auto back = color::backButton;
  auto backPressed = color::backButtonPressed;

  auto bx = buttonBaseX;
  auto w = 29;
  auto h = 46;
  auto r = 2;

  M5.Lcd.fillRoundRect(bx, 150, w, h, r, b1 ? frontPressed : front);
  M5.Lcd.fillRoundRect(bx + 41, 150, w, h, r, b3 ? frontPressed : front);
  M5.Lcd.fillRoundRect(bx + 82, 150, w, h, r, b5 ? frontPressed : front);
  M5.Lcd.fillRoundRect(bx + 123, 150, w, h, r, b7 ? frontPressed : front);
  M5.Lcd.fillRoundRect(bx + 21, 94, w, h, r, b2 ? backPressed : back);
  M5.Lcd.fillRoundRect(bx + 62, 94, w, h, r, b4 ? backPressed : back);
  M5.Lcd.fillRoundRect(bx + 103, 94, w, h, r, b6 ? backPressed : back);
}

void setup() {
  M5.begin();
  // Serial.begin(115200);

#if !defined(__MIPSEL__)
  while (!Serial)
    ; // Wait for serial port to connect - used on Leonardo, Teensy and other
      // boards with built-in USB CDC serial connection
#endif

  if (Usb.Init() == -1) {
    Serial.println("OSC did not start");
    while (1)
      ; // Halt
  }

  M5.Lcd.fillScreen(color::background);

  updateScratch();
  updateOptionButtons(false, false, false, false);
  updateButtons(false, false, false, false, false, false, false);

  BleServer = BeatbleBLEServer::crateServer();
  Serial.println("start advertising");
}

static bool previousIsRight = isRight;
static uint8_t previousScratch = 0;
static uint8_t previousButton = 0;
static uint8_t previousOptionButton = 0;

void updateDisplay() {
  // USB State
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextSize(2);
  if (PhoenixWan.connected()) {
    M5.Lcd.setTextColor(color::text);
  } else {
    M5.Lcd.setTextColor(color::background);
  }
  M5.Lcd.print("PhoenixWan Connected");

  // BLE State
  M5.Lcd.setCursor(10, 210);
  M5.Lcd.setTextSize(2);
  if (BleServer->isConnected()) {
    M5.Lcd.setTextColor(color::text);
  } else {
    M5.Lcd.setTextColor(color::background);
  }
  M5.Lcd.print("BLE Connected");

  bool playSideChanged = false;
  // play side changed
  if (previousIsRight != isRight) {
    previousIsRight = isRight;
    playSideChanged = true;

    clearPlayArea();
  }

  // scratch
  if (previousScratch != PhoenixWan.getScratch() || playSideChanged) {
    previousScratch = PhoenixWan.getScratch();
    updateScratch();
    drawAngleLine(PhoenixWan.getScratch());
  }

  if (previousOptionButton != PhoenixWan.getOptionButtonValue() ||
      playSideChanged) {
    previousOptionButton = PhoenixWan.getOptionButtonValue();
    updateOptionButtons(PhoenixWan.getButtonPress(PButton::E_1),
                        PhoenixWan.getButtonPress(PButton::E_2),
                        PhoenixWan.getButtonPress(PButton::E_3),
                        PhoenixWan.getButtonPress(PButton::E_4));
  }

  if (previousButton != PhoenixWan.getButtonValue() || playSideChanged) {
    previousButton = PhoenixWan.getButtonValue();
    updateButtons(PhoenixWan.getButtonPress(PButton::B_1),
                  PhoenixWan.getButtonPress(PButton::B_2),
                  PhoenixWan.getButtonPress(PButton::B_3),
                  PhoenixWan.getButtonPress(PButton::B_4),
                  PhoenixWan.getButtonPress(PButton::B_5),
                  PhoenixWan.getButtonPress(PButton::B_6),
                  PhoenixWan.getButtonPress(PButton::B_7));
  }
}

static auto lastMillis = millis();
static bool previousDeviceConnected = false;

class DisplaySleeper {
public:
  void toggle() {
    if (this->isSleep) {
      this->wakeup();
      return;
    }

    this->sleep();
  }

  void sleep() {
    this->isSleep = true;
    M5.Lcd.setBrightness(0);
    M5.Lcd.sleep();
  }

  void wakeup() {
    this->isSleep = false;
    M5.Lcd.setBrightness(200);
    M5.Lcd.wakeup();
  }

private:
  bool isSleep = false;
};

DisplaySleeper displaySleeper;

void loop() {
  M5.update();
  Usb.Task();

  auto currentMillis = millis();

  if (currentMillis - lastMillis >= 1000 / 60) {
    lastMillis = currentMillis;
    updateDisplay();
  }

  if (M5.BtnA.pressedFor(20, 500)) {
    Serial.println("Left Button was pressed");
    togglePlaySide();
  }

  if (M5.BtnB.pressedFor(20, 500)) {
    Serial.println("Middle Button was pressed");
    displaySleeper.toggle();
  }

  if (BleServer->isConnected()) {
    BleServer->notifyState(PhoenixWan.getScratch(), PhoenixWan.getButtonValue(),
                           PhoenixWan.getOptionButtonValue());
    delay(3);
  }

  // disconnecting
  if (!BleServer->isConnected() && previousDeviceConnected) {
    Serial.println("ble: disconnecting");
    delay(500); // wait for bluetooth stack
    BleServer->startAdvertising();
    Serial.println("start advertising");
    previousDeviceConnected = false;
  }

  // connecting
  if (BleServer->isConnected() && !previousDeviceConnected) {
    Serial.println("ble: connecting");
    previousDeviceConnected = true;
  }
}
