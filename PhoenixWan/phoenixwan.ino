#include <M5Stack.h>
#include <SPI.h>

#include "ble.h"
#include "phoenixwan.h"
#include <cmath>

USB Usb;
PhoenixWanUSB PhoenixWan(&Usb);
BeatbleBLEServer *BleServer;

void updateScratch(bool changed) {
  M5.Lcd.fillCircle(80, 120, 70, changed ? TFT_WHITE : TFT_BLACK);
}

void drawAngleLine(uint8_t angle) {
  double theta = (M_PI / 128) * angle;
  auto r = 70;
  auto dx = (int32_t)(r * std::cos(theta));
  auto dy = (int32_t)(-r * std::sin(theta));

  M5.Lcd.drawLine(80, 120, 80 + dx, 120 + dy, RED);
}

void updateOptionButtons(bool e1, bool e2, bool e3, bool e4) {
  M5.Lcd.fillRoundRect(160, 45, 29, 29, 2, e1 ? TFT_RED : TFT_WHITE);
  M5.Lcd.fillRoundRect(201, 45, 29, 29, 2, e2 ? TFT_RED : TFT_WHITE);
  M5.Lcd.fillRoundRect(242, 45, 29, 29, 2, e3 ? TFT_RED : TFT_WHITE);
  M5.Lcd.fillRoundRect(283, 45, 29, 29, 2, e4 ? TFT_RED : TFT_WHITE);
}

void updateButtons(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6,
                   bool b7) {
  M5.Lcd.fillRoundRect(160, 150, 29, 46, 2, b1 ? TFT_BLUE : TFT_WHITE);
  M5.Lcd.fillRoundRect(181, 94, 29, 46, 2, b2 ? TFT_BLUE : TFT_BLACK);
  M5.Lcd.fillRoundRect(201, 150, 29, 46, 2, b3 ? TFT_BLUE : TFT_WHITE);
  M5.Lcd.fillRoundRect(222, 94, 29, 46, 2, b4 ? TFT_BLUE : TFT_BLACK);
  M5.Lcd.fillRoundRect(242, 150, 29, 46, 2, b5 ? TFT_BLUE : TFT_WHITE);
  M5.Lcd.fillRoundRect(263, 94, 29, 46, 2, b6 ? TFT_BLUE : TFT_BLACK);
  M5.Lcd.fillRoundRect(283, 150, 29, 46, 2, b7 ? TFT_BLUE : TFT_WHITE);
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

  M5.Lcd.fillScreen(TFT_DARKGREY);

  updateScratch(false);
  updateOptionButtons(false, false, false, false);
  updateButtons(false, false, false, false, false, false, false);

  BleServer = BeatbleBLEServer::crateServer();
  Serial.println("start advertising");
}

static uint8_t previousScratch = 0;
static bool previousDeviceConnected = false;

void loop() {
  Usb.Task();

  // USB State
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextSize(2);
  if (PhoenixWan.connected()) {
    M5.Lcd.setTextColor(RED);
  } else {
    M5.Lcd.setTextColor(DARKGREY);
  }
  M5.Lcd.print("PhoenixWan Connected");

  if (PhoenixWan.connected()) {

    if (previousScratch != PhoenixWan.getScratch()) {
      previousScratch = PhoenixWan.getScratch();
    }
    updateScratch(false);
    drawAngleLine(PhoenixWan.getScratch());

    updateOptionButtons(PhoenixWan.getButtonPress(PButton::E_1),
                        PhoenixWan.getButtonPress(PButton::E_2),
                        PhoenixWan.getButtonPress(PButton::E_3),
                        PhoenixWan.getButtonPress(PButton::E_4));

    updateButtons(PhoenixWan.getButtonPress(PButton::B_1),
                  PhoenixWan.getButtonPress(PButton::B_2),
                  PhoenixWan.getButtonPress(PButton::B_3),
                  PhoenixWan.getButtonPress(PButton::B_4),
                  PhoenixWan.getButtonPress(PButton::B_5),
                  PhoenixWan.getButtonPress(PButton::B_6),
                  PhoenixWan.getButtonPress(PButton::B_7));
  }

  // Show BLE State
  M5.Lcd.setCursor(10, 210);
  M5.Lcd.setTextSize(2);
  if (BleServer->isConnected()) {
    M5.Lcd.setTextColor(RED);
  } else {
    M5.Lcd.setTextColor(DARKGREY);
  }
  M5.Lcd.print("BLE Connected");

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
