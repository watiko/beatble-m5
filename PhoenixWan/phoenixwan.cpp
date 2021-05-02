#include "phoenixwan.h"

bool PhoenixWanParser::getButtonPress(PButton::Button button) {
  switch (button) {
  case PButton::B_1:
    return this->state.btn1.b1 == 1;
  case PButton::B_2:
    return this->state.btn1.b2 == 1;
  case PButton::B_3:
    return this->state.btn1.b3 == 1;
  case PButton::B_4:
    return this->state.btn1.b4 == 1;
  case PButton::B_5:
    return this->state.btn1.b5 == 1;
  case PButton::B_6:
    return this->state.btn1.b6 == 1;
  case PButton::B_7:
    return this->state.btn1.b7 == 1;
  case PButton::E_1:
    return this->state.btn2.e1 == 1;
  case PButton::E_2:
    return this->state.btn2.e2 == 1;
  case PButton::E_3:
    return this->state.btn2.e3 == 1;
  case PButton::E_4:
    return this->state.btn2.e4 == 1;
  default:
    return false;
  }
}

uint8_t PhoenixWanParser::getScratch() { return this->state.scratch; }

void PhoenixWanParser::Parse(uint8_t len, uint8_t *buf) {
  if (len < 0 || !buf)
    return;

  if (len != 5)
    return;

  // update
  this->state.scratch = buf[0];
  this->state.btn1.value = buf[2];
  this->state.btn2.value = buf[3];

  if (this->state.scratch != this->previousState.scratch) {
    this->previousState.scratch = this->state.scratch;
    // detect change
  }

  if (this->state.btn1.value != this->previousState.btn1.value) {
    this->previousState.btn1.value = this->state.btn1.value;
    // detect change
  }

  if (this->state.btn1.value != this->previousState.btn1.value) {
    this->previousState.btn1.value = this->state.btn1.value;
    // detect change
  }
}

void PhoenixWanParser::Reset() {
  this->state.scratch = 0;
  this->state.btn1.value = 0;
  this->state.btn2.value = 0;

  this->previousState.scratch = 0;
  this->previousState.btn1.value = 0;
  this->previousState.btn2.value = 0;
}
