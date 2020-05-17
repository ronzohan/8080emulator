#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "emulate.h"
#include "condition_codes.h"
#include "state8080.h"

void unimplementedInstruction(State8080 *state)
{
  //pc will have advanced one, so undo that
  std::printf("Error: Unimplemented instruction\n");
  exit(1);
}

// Returns 1 when the bits has even parity, 0 when odd parity
int parity(int bit)
{
  return __builtin_parity(bit);
}

void setArithmeticFlags(uint16_t result, State8080 *state)
{
  state->cc.z = ((result & 0xff) == 0);
  state->cc.s = ((result & 0x80) != 0);
  state->cc.cy = (result > 0xff);
  state->cc.p = parity(result & 0xff);
}

void setFlagsZSP(uint8_t result, State8080 *state) 
{
  state->cc.z = ((result & 0xff) == 0);
  state->cc.s = ((result & 0x80) != 0);
  state->cc.p = parity(result & 0xff);
}

void Emulate8080p(State8080 *state)
{
  
  unsigned char *opcode = &state->memory[state->pc];
  state->pc += 1;

  switch (*opcode)
  {
  // NOP ez
  case 0x00:
    break;
  case 0x05:
  {
    uint8_t result = state->b - 1;
    setFlagsZSP(result, state);
    state->b = result;
    break;
  }
  case 0x11: 
  {
    state->d = opcode[2] & 0xff;
    state->e = opcode[1] & 0xff;
    state->pc += 2;
    break;
  }
  case 0x13:
  {
    state->e++;
    if (state->e == 0)
      state->d++;
    break;
  }
  case 0x1a:
  {
    uint16_t offset = (state->d << 8) | (state->e & 0xff);
    state->a = state->memory[offset];
    break;
  };
  case 0x21:
  {
    state->h = opcode[2] & 0xff;
    state->l = opcode[1] & 0xff;
    state->pc += 2;
    break;
  }
  case 0x23:
  {
    state->l++;

    if (state->l == 0 )
      state->h++;

    break;
  }
  case 0x31: 
  {
    state->sp = (opcode[2] << 8) | opcode[1];
    state->pc += 2;
    break;
  }
  case 0x06:
  {
    state->b = opcode[1];
    state->pc += 1;
    break;
  }
  case 0x77:
  {
    uint8_t offset = (state->h << 8) | (state->l & 0xff);
    state->memory[offset] = state->a;
    break;
  }
  // ADD B
  case 0x80:
  {
    uint16_t answer = (uint16_t)state->a + (uint16_t)state->b;

    // Zero flag: if the result is zero,
    // set the flag to zero
    // else clear the flag
    if ((answer & 0xff) == 0)
      state->cc.z = 1;
    else
      state->cc.z = 0;

    // Sign Flag: if bit 7 is set,
    // set the sign flag
    // else clear the sign flag
    if (answer & 0x80)
      state->cc.s = 1;
    else
      state->cc.s = 0;

    // Carry Flag
    if (answer > 0xff)
      state->cc.cy = 1;
    else
      state->cc.cy = 0;

    // Parity is handled by a subroutine
    state->cc.p = parity(answer & 0xff);

    state->a = answer & 0xff;

    break;
  }
  // ADD C
  case 0x81:
  {
    unimplementedInstruction(state);
    uint16_t answer = (uint16_t)state->a + (uint16_t)state->c;
    setArithmeticFlags(answer, state);
    state->a = answer  & 0xff;
    break;
  }
  // ADD M
  case 0x86:
  {
    unimplementedInstruction(state);
    uint16_t offset = (state->h << 8) | (state->l);
    uint16_t answer = (uint16_t)state->a + state->memory[offset];
    setArithmeticFlags(answer, state);
    break;
  }
  case 0xc3:
  {
    state->pc = (opcode[2] << 8) | opcode[1];
    break;
  }
  case 0xcd:
  {
    state->memory[state->sp - 1] = (opcode[2]) & 0xff;
    state->memory[state->sp - 2] = 0x01; //& 0xff;
    state->memory[0] = 0x01;
    state->sp = state->sp - 2;
    state->pc = (opcode[2] << 8) | opcode[1];
    break;
  }
  // ADI D8
  case 0xC6:
  {
    unimplementedInstruction(state);
    uint16_t answer = (uint16_t)state->a + (uint16_t)opcode[1];
    setArithmeticFlags(answer, state);
    break;
  }
  default:
    unimplementedInstruction(state);
    break;
  }

  /* print out processor state */
  std::printf("\nC=%d, P=%d, S=%d, Z=%d", state->cc.cy, state->cc.p, state->cc.s, state->cc.z);
  std::printf("\nA:$%02x B:$%02x C:$%02x D:$%02x E:$%02x H:$%02x L:$%02x SP:%04x\n\n",
         state->a, state->b, state->c, state->d,
         state->e, state->h, state->l, state->sp);
}