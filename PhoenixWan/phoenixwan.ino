#include "phoenixwan.h"

#include <SPI.h>

USB Usb;
PhoenixWanUSB PhoenixWan(&Usb);

void setup() {
  Serial.begin(115200);

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

  Serial.println("Started");
}

void loop() {
  Usb.Task();

  if (PhoenixWan.connected()) {
    if (PhoenixWan.getButtonPress(PButton::B_3)) {
      Serial.println("B3");
    }
    if (PhoenixWan.getButtonPress(PButton::E_1)) {
      Serial.println("E1");
    }
  }
}
