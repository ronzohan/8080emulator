#include "state8080.h"
#include "emulator.h"

Emulator::Emulator()
{
  State8080* state = (State8080*)calloc(1, sizeof(State8080));
	state->memory = (uint8_t*)malloc(0x10000);  //16Ks
}

void Emulator::push(uint16_t r) {
  state->memory[(state->sp - 2)] = r << 8;
  state->memory[(state->sp - 1)] = r & 0xff;
  state->sp -= 2;
}

uint16_t Emulator::nextWord(uint16_t pc) {
  unsigned char *opcode = &state->memory[state->pc];
  state->pc += 2;

  return (opcode[1] << 8) | opcode[0];
}