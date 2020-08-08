#ifndef STATE8080_H
#define STATE8080_H

#include <iostream>
#include "condition_codes.h"

struct State8080
{
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  uint16_t sp;
  uint16_t pc;
  uint8_t *memory;
  struct ConditionCodes cc;
  uint8_t int_enable;

  uint16_t de() {
    return (d << 8) | (e & 0xff);
  }

  void setDE(uint16_t r) {
    d = r >> 8 & 0xff;
    e = r & 0xff;
  }

  uint16_t hl() {
    return (h << 8) | (l & 0xff);
  }

  void setHL(uint16_t r) {
    h = r >> 8 & 0xff;
    l = r & 0xff;
  }
};

#endif