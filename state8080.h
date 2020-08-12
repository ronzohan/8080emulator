#ifndef STATE8080_H
#define STATE8080_H

#include <iostream>
#include <stdio.h>
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

  void push(uint16_t r) {
	  memory[(sp - 2)] = r >> 8;
	  memory[(sp - 1)] = r & 0xff;
	  sp -= 2;
  }

  void pop() {
	  pc = (memory[sp + 1] & 0xff) | (memory[sp] << 8);
	  sp += 2;
  }

  uint16_t nextWord(uint16_t pc) {
	  unsigned char* opcode = &memory[pc];
	  pc += 2;
	  return (opcode[1] << 8) | opcode[0];
  }

  uint16_t nextWord() {
	  unsigned char* opcode = &memory[pc];
	  pc += 2;

	  return (opcode[1] << 8) | opcode[0];
  }

  uint16_t getWord(uint16_t pc) {
	  unsigned char* opcode = &memory[pc];
	  return (opcode[1] << 8) | opcode[0];
  }

  uint16_t getWord() {
	  unsigned char* opcode = &memory[pc];
	  return (opcode[1] << 8) | opcode[0];
  }

  uint8_t nextByte(uint16_t pc) {
	  unsigned char* opcode = &memory[pc];
	  pc += 1;
	  return opcode[0];
  }

  uint8_t nextByte() {
	  unsigned char* opcode = &memory[pc];
	  pc += 1;
	  return opcode[0];
  }

  void setFlagsZSPAC(uint16_t result)
  {
	  cc.z = ((result & 0xff) == 0);
	  cc.s = ((result & 0x80) != 0);
	  cc.p = parity(result & 0xff);
  }

  void setFlagsZSPACCY(uint16_t result)
  {
	  setFlagsZSPAC(result);
	  setFlagCY(result);
  }

  void setFlagCY(uint16_t result)
  {
	  cc.cy = (result > 0xff);
  }

  // Returns 1 when the bits has even parity, 0 when odd parity
  int parity(int x)
  {
	  x ^= x >> 8;
	  x ^= x >> 4;
	  x ^= x >> 2;
	  x ^= x >> 1;

	  return (~x) & 1;
  }
};

#endif