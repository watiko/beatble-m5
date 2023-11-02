#ifndef _phoenixwan_h_
#define _phoenixwan_h_

#include <Usb.h>
#include <hiduniversal.h>

#define PHOENIX_WAN_VID 0x1CCF
#define PHOENIX_WAN_PID 0x8048

enum class PButton {
  B_1,
  B_2,
  B_3,
  B_4,
  B_5,
  B_6,
  B_7,
  E_1,
  E_2,
  E_3,
  E_4,
};

union PhoenixWanButtons {
  struct {
    uint8_t b1 : 1;
    uint8_t b2 : 1;
    uint8_t b3 : 1;
    uint8_t b4 : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;
  } __attribute__((packed));
  uint8_t value : 8;
} __attribute__((packed));

union PhoenixWanOptionButtons {
  struct {
    uint8_t e1 : 1;
    uint8_t e2 : 1;
    uint8_t e3 : 1;
    uint8_t e4 : 1;
  } __attribute__((packed));
  uint8_t value : 8;
} __attribute__((packed));

struct PhoenixWanState {
  uint8_t scratch;
  PhoenixWanButtons buttons;
  PhoenixWanOptionButtons optionButtons;
};

class PhoenixWanParser {
public:
  PhoenixWanParser(){};

  bool getButtonPress(PButton button);
  uint8_t getScratch();
  uint8_t getButtonValue();
  uint8_t getOptionButtonValue();

protected:
  void Parse(uint8_t len, uint8_t *buf);
  void Reset();

private:
  PhoenixWanState state;
  PhoenixWanState previousState;
};

class PhoenixWanUSB : public HIDUniversal, public PhoenixWanParser {
public:
  PhoenixWanUSB(USB *p) : HIDUniversal(p){};

  bool connected() {
    return this->HIDUniversal::isReady() && this->isPhoenixWan();
  }

protected:
  virtual void ParseHIDData(USBHID *hid __attribute__((unused)),
                            bool is_rpt_id __attribute__((unused)), uint8_t len,
                            uint8_t *buf) {
    if (this->isPhoenixWan())
      this->PhoenixWanParser::Parse(len, buf);
  };

  virtual uint8_t OnInitSuccessful() {
    if (this->isPhoenixWan()) {
      this->PhoenixWanParser::Reset();
    };
    return 0;
  };

  virtual bool VIDPIDOK(uint16_t vid, uint16_t pid) {
    return vid == PHOENIX_WAN_VID && pid == PHOENIX_WAN_PID;
  };

private:
  bool isPhoenixWan() {
    return this->VIDPIDOK(HIDUniversal::VID, HIDUniversal::PID);
  };
};

#endif