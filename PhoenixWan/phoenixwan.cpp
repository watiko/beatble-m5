#include "phoenixwan.h"

bool PhoenixWanParser::getButtonPress(PButton::Button button) {
  switch (button) {
  case PButton::B_1:
    return this->state.buttons.b1 == 1;
  case PButton::B_2:
    return this->state.buttons.b2 == 1;
  case PButton::B_3:
    return this->state.buttons.b3 == 1;
  case PButton::B_4:
    return this->state.buttons.b4 == 1;
  case PButton::B_5:
    return this->state.buttons.b5 == 1;
  case PButton::B_6:
    return this->state.buttons.b6 == 1;
  case PButton::B_7:
    return this->state.buttons.b7 == 1;
  case PButton::E_1:
    return this->state.optionButtons.e1 == 1;
  case PButton::E_2:
    return this->state.optionButtons.e2 == 1;
  case PButton::E_3:
    return this->state.optionButtons.e3 == 1;
  case PButton::E_4:
    return this->state.optionButtons.e4 == 1;
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
  this->state.buttons.value = buf[2];
  this->state.optionButtons.value = buf[3];

  if (this->state.scratch != this->previousState.scratch) {
    this->previousState.scratch = this->state.scratch;
    // detect change
  }

  if (this->state.buttons.value != this->previousState.buttons.value) {
    this->previousState.buttons.value = this->state.buttons.value;
    // detect change
  }

  if (this->state.buttons.value != this->previousState.buttons.value) {
    this->previousState.buttons.value = this->state.buttons.value;
    // detect change
  }
}

void PhoenixWanParser::Reset() {
  this->state.scratch = 0;
  this->state.buttons.value = 0;
  this->state.optionButtons.value = 0;

  this->previousState.scratch = 0;
  this->previousState.buttons.value = 0;
  this->previousState.optionButtons.value = 0;
}
