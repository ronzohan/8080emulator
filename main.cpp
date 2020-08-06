#include <iostream>
#include <string>
#include <cstdio>
#include <stdlib.h>
#include "state8080.h"
#include "emulate.h"

using namespace std;

State8080* init8080(void) 
{
  State8080* state = (State8080*)calloc(1, sizeof(State8080));
	state->memory = (uint8_t*)malloc(0x10000);  //16Ks
	return state;
}

int main(int argc, char **argv)
{
  State8080* state = init8080();
  string fileName = "invaders/invaders";
  FILE *f = fopen(fileName.c_str(), "rb");
  cout << "Heelo";
  if (f == NULL)
  {
    printf("error: Couldn't open %s\n", argv[1]);
  }

  //Get the file size and read it into a memory buffer
  fseek(f, 0L, SEEK_END);
  int fsize = ftell(f);
  fseek(f, 0L, SEEK_SET);

  //unsigned char *buffer = (unsigned char *)malloc(fsize);
  uint8_t *buffer = &state->memory[0];
  fread(buffer, fsize, 1, f);
  fclose(f);

  int pc = 0;
  
  state->pc = 0;
  int count = 0;
  while (true)
  {
    count++;
    Disassemble8080p(buffer, state->pc);
    Emulate8080p(state);
  }

  return 0;
}
