#ifndef EMULATE_8080_H
#define EMULATE_8080_H
#include "state8080.h"

void Emulate8080p(State8080 *state);
int Disassemble8080p(unsigned char *codebuffer, int pc);

#endif