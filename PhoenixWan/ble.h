#ifndef _ble_h_
#define _ble_h_

#include <memory>

#include <NimBLEDevice.h>

namespace {
const std::string ADVERTISING_NAME = "IIDX Entry model";
// BASE_UUID: 0000XXXX-0000-1000-8000-00805F9B34FB
const std::string SERVICE_UUID = "0000FF00-0000-1000-8000-00805F9B34FB";
const std::string CHARACTERISTIC_UUID = "0000FF01-0000-1000-8000-00805F9B34FB";
} // namespace

class BeatbleBLEServer {
public:
  static BeatbleBLEServer *crateServer();

  bool isConnected();
  void notifyState(uint8_t scratch, uint8_t button, uint8_t optionButton);

  void startAdvertising();

private:
  BeatbleBLEServer();

  BLEServer *server;
  BLEService *service;
  BLECharacteristic *characteristic;
  BLEAdvertising *advertising;

  bool connected;
};

#endif