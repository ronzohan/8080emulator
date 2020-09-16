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
  uint8_t f;
  uint8_t h;
  uint8_t l;
  uint16_t sp;
  uint16_t pc;
  uint8_t *memory;
  struct ConditionCodes cc;
  uint8_t int_enable;

  uint16_t af()
  {
    return (a << 8) | (f & 0xff);
  }

  uint16_t bc()
  {
    return (b << 8) | (c & 0xff);
  }

  void setBC(uint16_t r)
  {
    b = r >> 8 & 0xff;
    c = r & 0xff;
  }

  uint16_t de()
  {
    return (d << 8) | (e & 0xff);
  }

  void setDE(uint16_t r)
  {
    d = r >> 8 & 0xff;
    e = r & 0xff;
  }

  uint16_t hl()
  {
    return (h << 8) | (l & 0xff);
  }

  void setHL(uint16_t r)
  {
    h = r >> 8 & 0xff;
    l = r & 0xff;
  }

  void push(uint16_t r)
  {
    memory[(sp - 2)] = r >> 8;
    memory[(sp - 1)] = r & 0xff;
    sp -= 2;
  }

  uint16_t pop()
  {
    uint16_t result = (memory[sp + 1] & 0xff) | (memory[sp] << 8);
    sp += 2;
    return result;
  }

  uint16_t nextWord(uint16_t pc)
  {
    unsigned char *opcode = &memory[pc];
    pc += 2;
    return (opcode[1] << 8) | opcode[0];
  }

  uint16_t nextWord()
  {
    unsigned char *opcode = &memory[pc];
    pc += 2;

    return (opcode[1] << 8) | opcode[0];
  }

  uint16_t getWord(uint16_t pc)
  {
    unsigned char *opcode = &memory[pc];
    return (opcode[1] << 8) | opcode[0];
  }

  uint16_t getWord()
  {
    unsigned char *opcode = &memory[pc];
    return (opcode[1] << 8) | opcode[0];
  }

  uint8_t nextByte(uint16_t pc)
  {
    unsigned char *opcode = &memory[pc];
    pc += 1;
    return opcode[0];
  }

  uint8_t nextByte()
  {
    unsigned char *opcode = &memory[pc];
    pc += 1;
    return opcode[0];
  }

  void setFlagsZSPAC(uint16_t result)
  {
    cc.z = ((result & 0xff) == 0);
    cc.s = ((result & 0x80) != 0);
    cc.p = parity(result & 0xff);
    setPSW();
  }

  void setFlagsZSPACCY(uint16_t result)
  {
    setFlagsZSPAC(result);
    setFlagCY(result);
  }

  void setFlagCY(uint16_t result)
  {
    cc.cy = (result > 0xff);
    setPSW();
  }

  void setPSW() 
  {
    f = ((cc.s << 7) & 0x0080) +
     ((cc.p << 2) & 0x0004) +
      ((cc.z << 6) & 0x0040) +
       (cc.cy & 0x0001);
  }

  void setPSW(uint16_t result)
  { 
    a = result >> 8 & 0xff;
    f = result & 0xff;
  }

  uint16_t addWord(uint16_t lhs, uint16_t rhs)
  {
    uint32_t result = lhs + rhs;
    cc.cy = result > 0xffff;
    return result;
  }

  int parity(int x)
  {
    int i, size = 16;
    int p = 0;
    x = (x & ((1 << size) - 1));
    for (i = 0; i < size; i++)
    {
      if (x & 0x1)
        p++;
      x = x >> 1;
    }
    return (0 == (p & 0x1));
  }

  void aXOR(uint8_t val) 
  {
    uint8_t result = a ^ val;
    a = result;
    setFlagsZSPACCY(result);
  }

  void aAND(uint8_t val)
  {
    uint8_t result = a & val;
    a = result;
    setFlagsZSPACCY(result);
  }
};

#endif