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

void updateScratch() { M5.Lcd.fillCircle(80, 120, 70, color::scratch); }

void drawAngleLine(uint8_t angle) {
  double theta = (M_PI / 128) * angle;
  auto r = 70;
  auto dx = (int32_t)(r * std::cos(theta));
  auto dy = (int32_t)(-r * std::sin(theta));

  M5.Lcd.drawLine(80, 120, 80 + dx, 120 + dy, color::scratchAngle);
}

void updateOptionButtons(bool e1, bool e2, bool e3, bool e4) {
  auto option = color::optionButton;
  auto pressed = color::optionButtonPressed;

  auto w = 29;
  auto r = 2;

  M5.Lcd.fillRoundRect(160, 45, w, w, r, e1 ? pressed : option);
  M5.Lcd.fillRoundRect(201, 45, w, w, r, e2 ? pressed : option);
  M5.Lcd.fillRoundRect(242, 45, w, w, r, e3 ? pressed : option);
  M5.Lcd.fillRoundRect(283, 45, w, w, r, e4 ? pressed : option);
}

void updateButtons(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6,
                   bool b7) {
  auto front = color::frontButton;
  auto frontPressed = color::frontButtonPressed;
  auto back = color::backButton;
  auto backPressed = color::backButtonPressed;

  auto w = 29;
  auto h = 46;
  auto r = 2;

  M5.Lcd.fillRoundRect(160, 150, w, h, r, b1 ? frontPressed : front);
  M5.Lcd.fillRoundRect(201, 150, w, h, r, b3 ? frontPressed : front);
  M5.Lcd.fillRoundRect(242, 150, w, h, r, b5 ? frontPressed : front);
  M5.Lcd.fillRoundRect(283, 150, w, h, r, b7 ? frontPressed : front);
  M5.Lcd.fillRoundRect(181, 94, w, h, r, b2 ? backPressed : back);
  M5.Lcd.fillRoundRect(222, 94, w, h, r, b4 ? backPressed : back);
  M5.Lcd.fillRoundRect(263, 94, w, h, r, b6 ? backPressed : back);
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

  // scratch
  if (previousScratch != PhoenixWan.getScratch()) {
    previousScratch = PhoenixWan.getScratch();
    updateScratch();
    drawAngleLine(PhoenixWan.getScratch());
  }

  if (previousOptionButton != PhoenixWan.getOptionButtonValue()) {
    previousOptionButton = PhoenixWan.getOptionButtonValue();
    updateOptionButtons(PhoenixWan.getButtonPress(PButton::E_1),
                        PhoenixWan.getButtonPress(PButton::E_2),
                        PhoenixWan.getButtonPress(PButton::E_3),
                        PhoenixWan.getButtonPress(PButton::E_4));
  }

  if (previousButton != PhoenixWan.getButtonValue()) {
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

void loop() {
  Usb.Task();

  auto currentMillis = millis();

  if (currentMillis - lastMillis >= 1000 / 60) {
    lastMillis = currentMillis;
    updateDisplay();
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
