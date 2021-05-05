#include "ble.h"

static bool deviceConnected = false;
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) { deviceConnected = true; };
  void onDisconnect(BLEServer *server) { deviceConnected = false; }
};

BeatbleBLEServer::BeatbleBLEServer() {
  BLEDevice::init(ADVERTISING_NAME);
  this->server = BLEDevice::createServer();
  this->service = server->createService(SERVICE_UUID);

  this->characteristic = service->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  this->characteristic->addDescriptor(new BLE2902());

  this->service->start();

  this->advertising = BLEDevice::getAdvertising();
  this->advertising->addServiceUUID(SERVICE_UUID);
  this->advertising->setScanResponse(true);

  BLEDevice::startAdvertising();

  this->connected = false;

  this->server->setCallbacks(new MyServerCallbacks());
}

BeatbleBLEServer *BeatbleBLEServer::crateServer() {
  return new BeatbleBLEServer();
}

bool BeatbleBLEServer::isConnected() { return deviceConnected; }

static uint16_t counter = 0;
void BeatbleBLEServer::notifyState(uint8_t scratch, uint8_t button,
                                   uint8_t optionButton) {
  uint8_t temp[10] = {
      scratch, 0x00, button, optionButton, counter & 0xFF,
      scratch, 0x00, button, optionButton, (counter + 1) & 0xFF,
  };

  this->characteristic->setValue(temp, 10);
  this->characteristic->notify();

  counter = (counter + 2) & 0xFF;
}

void BeatbleBLEServer::startAdvertising() { this->server->startAdvertising(); }