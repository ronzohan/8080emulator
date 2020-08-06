#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "emulate.h"
#include "condition_codes.h"
#include "state8080.h"

/*    
  *codebuffer is a valid pointer to 8080 assembly code    
  pc is the current offset into the code

  returns the number of bytes of the op    
*/
int Disassemble8080p(unsigned char *codebuffer, int pc)
{
  unsigned char *code = &codebuffer[pc];
  int opbytes = 1;

  printf("%04x: ", pc);

  switch (*code)
  {
  case 0x00: printf("%02x NOP", code[0]); break;
  case 0x05: printf("DCR B"); break;
  case 0x11: printf("LXI D, %02x%02x", code[2], code[1]); break;
  case 0x13: printf("INX D"); break;
  case 0x1a: printf("LDAX D"); break;
  case 0x21: printf("LXI H, %02x%02x", code[2], code[1]); break;
  case 0x23: printf("INX H"); break;
  case 0x77: printf("MOV M, A"); break;
  case 0xc3: printf("JMP %02x%02x", code[2], code[1]); break;
  case 0xcd: printf("CALL %02x%02x", code[2], code[1]); break;
  default:
    printf("%02x\t", code[0]);
    break;
  }

  printf("\n");

  return opbytes;
}

void unimplementedInstruction(State8080 *state)
{
  //pc will have advanced one, so undo that
  std::printf("Error: Unimplemented instruction ");
  Disassemble8080p(state->memory, state->pc);
  exit(1);
}

// Returns 1 when the bits has even parity, 0 when odd parity
int parity(int bit)
{
  return __builtin_parity(bit) == 0;
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

void push(uint16_t r, State8080 *state) {
  state->memory[(state->sp - 2)] = r << 8;
  state->memory[(state->sp - 1)] = r & 0xff;
  state->sp -= 2;
}

uint16_t nextWord(uint16_t pc, State8080 *state) {
  unsigned char *opcode = &state->memory[state->pc];
  state->pc += 2;

  return (opcode[1] << 8) | opcode[0];
}

uint16_t getWord(uint16_t pc, State8080 *state) {
  unsigned char *opcode = &state->memory[state->pc];
  return (opcode[1] << 8) | opcode[0];
}

uint8_t nextByte(uint16_t pc, State8080 *state) {
  unsigned char *opcode = &state->memory[state->pc];
  state->pc += 1;
  return opcode[0];
}

void Emulate8080p(State8080 *state)
{
  unsigned char *opcode = &state->memory[state->pc];
  state->pc += 1;

  switch (*opcode)
  {
  // NOP
  case 0x00:
    break;
  case 0x06:
    state->b = nextByte(state->pc, state);
    break;
  case 0x11:
    state->e = nextByte(state->pc, state);
    state->d = nextByte(state->pc, state);
    break;
  case 0x13:
    state->setDE(state->de() + 1);
    break;
  case 0x1a:
    state->a = state->memory[state->de()];
    break;
  case 0x21:
    state->l = nextByte(state->pc, state);
    state->h = nextByte(state->pc, state);
    break;
  case 0x23:  
    state->setHL(state->hl() + 1);
    break;
  case 0x31:
    state->sp = nextWord(state->pc, state);
    break;
  case 0xc3:
    state->pc = getWord(state->pc, state);
    break;
  case 0xcd:
  {
    uint16_t w = getWord(state->pc, state);
    push(w, state);
    state->pc = w;
    break;
  }
  case 0x77:
    state->memory[state->hl()] = state->a;
    break;
  // case 0x05:
  // {
  //   uint8_t result = state->b - 1;
  //   setFlagsZSP(result, state);
  //   state->b = result;
  //   break;
  // }
  // case 0x0e:
  // {
  //   state->c = opcode[1] & 0xff;
  //   state->pc++;
  //   break;
  // }
  // case 0x11: 
  // {
  //   state->d = opcode[2] & 0xff;
  //   state->e = opcode[1] & 0xff;
  //   state->pc += 2;
  //   break;
  // }
  // case 0x13:
  // {
  //   state->e++;
  //   if (state->e == 0)
  //     state->d++;
  //   break;
  // }
  // case 0x1a:
  // {
  //   uint16_t offset = (state->d << 8) | (state->e & 0xff);
  //   state->a = state->memory[offset];
  //   break;
  // };
  // case 0x21:
  // {
  //   state->h = opcode[2] & 0xff;
  //   state->l = opcode[1] & 0xff;
  //   state->pc += 2;
  //   break;
  // }
  // case 0x23:
  // {
  //   state->l++;

  //   if (state->l == 0 )
  //     state->h++;

  //   break;
  // }
  // case 0x31: 
  // {
  //   state->sp = (opcode[2] << 8) | opcode[1];
  //   state->pc += 2;
  //   break;
  // }
  // case 0x36:
  // {
  //   uint16_t hl = (state->h << 8) | (state->l & 0xff);
  //   state->memory[hl] = opcode[1];
  //   state->pc++;
  //   break;
  // }
  // case 0x06:
  // {
  //   state->b = opcode[1];
  //   state->pc += 1;
  //   break;
  // }
  // case 0x7c:
  // {
  //   state->a = state->h & 0xff;
  //   break;
  // }
  // case 0x77:
  // {
  //   uint8_t offset = (state->h << 8) | (state->l & 0xff);
  //   state->memory[offset] = state->a;
  //   break;
  // }
  // // ADD B
  // case 0x80:
  // {
  //   uint16_t answer = (uint16_t)state->a + (uint16_t)state->b;

  //   // Zero flag: if the result is zero,
  //   // set the flag to zero
  //   // else clear the flag
  //   if ((answer & 0xff) == 0)
  //     state->cc.z = 1;
  //   else
  //     state->cc.z = 0;

  //   // Sign Flag: if bit 7 is set,
  //   // set the sign flag
  //   // else clear the sign flag
  //   if (answer & 0x80)
  //     state->cc.s = 1;
  //   else
  //     state->cc.s = 0;

  //   // Carry Flag
  //   if (answer > 0xff)
  //     state->cc.cy = 1;
  //   else
  //     state->cc.cy = 0;

  //   // Parity is handled by a subroutine
  //   state->cc.p = parity(answer & 0xff);

  //   state->a = answer & 0xff;

  //   break;
  // }
  // // ADD C
  // case 0x81:
  // {
  //   unimplementedInstruction(state);
  //   uint16_t answer = (uint16_t)state->a + (uint16_t)state->c;
  //   setArithmeticFlags(answer, state);
  //   state->a = answer  & 0xff;
  //   break;
  // }
  // // ADD M
  // case 0x86:
  // {
  //   unimplementedInstruction(state);
  //   uint16_t offset = (state->h << 8) | (state->l);
  //   uint16_t answer = (uint16_t)state->a + state->memory[offset];
  //   setArithmeticFlags(answer, state);
  //   break;
  // }
  // case 0xc2: // JNZ
  // {
  //   if (state->cc.z == 0)
  //     state->pc = (opcode[2] << 8) | opcode[1];
  //   else 
  //     state->pc += 2;
  //   break;
  // }
  // case 0xc3:
  // {
  //   state->pc = (opcode[2] << 8) | opcode[1];
  //   break;
  // }
  // case 0xc9: // ret
  // {
  //   state->pc = (state->memory[state->sp] & 0xff) | (state->memory[state->sp + 1] << 8);
  //   state->sp += 2;
  //   break;
  // }
  // case 0xcd: // Call
  // {
  //   uint16_t ret = state->pc + 2;
  //   state->memory[state->sp - 1] = (ret >> 8) & 0xff;
  //   state->memory[state->sp - 2] = ret & 0xff;
  //   state->sp = state->sp - 2;
  //   state->pc = (opcode[2] << 8) | opcode[1];
  //   break;
  // }
  // // ADI D8
  // case 0xC6:
  // {
  //   unimplementedInstruction(state);
  //   uint16_t answer = (uint16_t)state->a + (uint16_t)opcode[1];
  //   setArithmeticFlags(answer, state);
  //   break;
  // }
  // case 0xd5:
  // {
  //   break;
  // }
  // case 0xe5:
  // {
  //   uint16_t reg = (state->h << 8) | (state->l & 0xff);
  //   push(reg, state);
  //   break;
  // }
  // case 0xfe:
  // {
  //   uint16_t answer = (uint16_t)state->a - (uint16_t)opcode[1];
  //   setArithmeticFlags(answer, state);
  //   state->pc++;
  //   break;
  // }
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