#include <iostream>
#include <string>
#include <cstdio>
#include <stdlib.h>
#include "state8080.h"
#include "emulate.h"

using namespace std;

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
  case 0x00: printf("%02x\t NOP", code[0]); break;
  case 0x05: printf("%02x\t DCR B", code[0]); break;
  case 0x06: printf("%02x %02x\t MVI B, #$%02x", code[0], code[1], code[1]); opbytes = 2; break;
  case 0x09: printf("%02x\t DAD B", code[0]); break;
  case 0x0d: printf("%02x\t DCR C", code[0]); break;
  case 0x0e: printf("%02x %02x\t MVI C, #$%02x", code[0], code[1], code[1]); opbytes = 2; break;
  case 0x0f: printf("%02x\t RRC", code[0]); break;
  case 0x11: printf("%02x %02x %02x\t LXI D, #$%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0x13: printf("%02x\t INX D", code[0]); break;
  case 0x19: printf("%02x\t DAD D", code[0]); break;
  case 0x1a: printf("%02x\t LDAX D", code[0]); break;
  case 0x21: printf("%02x %02x %02x\t LXI H, #$%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0x23: printf("%02x\t INX H", code[0]); break;
  case 0x26: printf("%02x %02x %02x\t MVI H, #$%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0x27: printf("%02x\t DAA", code[0]); break;
  case 0x29: printf("%02x\t DAD H", code[0]); break;
  case 0x31: printf("%02x %02x %02x\t LXI SP, #$%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0x32: printf("%02x %02x %02x\t STA $%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0x33: printf("%02x\t INX SP", code[0]); break;
  case 0x35: printf("%02x\t DCR M", code[0]); break;
  case 0x36: printf("%02x %02x\t MVI M, #$%02x", code[0], code[1], code[1]); opbytes = 2; break;
  case 0x3a: printf("%02x %02x %02x\t LDA $%02x%02x", code[0], code[1], code[2] ,code[2], code[1]); opbytes = 3; break;
  case 0x3e: printf("%02x %02x\t MVI A, #$%02x", code[0], code[1], code[1]); opbytes = 2; break;
  case 0x56: printf("%02x\t MOV D, M", code[0]); break;
  case 0x5e: printf("%02x\t MOV E, M", code[0]); break;
  case 0x66: printf("%02x\t MOV H, M", code[0]); break;
  case 0x6f: printf("%02x\t MOV L, A", code[0]); break;
  case 0x77: printf("%02x\t MOV M, A", code[0]); break;
  case 0x7a: printf("%02x\t MOV A, D", code[0]); break;
  case 0x7b: printf("%02x\t MOV A, E", code[0]); break;
  case 0x7c: printf("%02x\t MOV A, H", code[0]); break;
  case 0x7e: printf("%02x\t MOV A, M", code[0]); break;
  case 0x80: printf("%02x\t ADD B", code[0]); break;
  case 0xa7: printf("%02x\t ANA A", code[0]); break;
  case 0xaf: printf("%02x\t XRA A", code[0]); break;
  case 0xc1: printf("%02x\t POP B", code[0]); break;
  case 0xc2: printf("%02x %02x %02x\t JNZ $%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0xc3: printf("%02x %02x %02x\t JMP $%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0xc5: printf("%02x\t PUSH B", code[0]); break;
  case 0xc6: printf("%02x %02x\t ADI $%02x", code[0], code[1], code[1]); opbytes = 2; break;
  case 0xc9: printf("%02x\t RET", code[0]); break;
  case 0xca: printf("%02x %02x %02x\t JZ $%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0xcd: printf("%02x %02x %02x\t CALL $%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0xd1: printf("%02x\t POP D", code[0]); break;
  case 0xd3: printf("%02x %02x\t OUT #$%02x", code[0], code[1], code[1]); opbytes = 2; break;
  case 0xd5: printf("%02x\t PUSH D", code[0]); break;
  case 0xda: printf("%02x %02x %02x\t JC $%02x%02x", code[0], code[1], code[2], code[2], code[1]); opbytes = 3; break;
  case 0xdb: printf("%02x\t IN #$%02x", code[0], code[1]); opbytes = 2; break;
  case 0xe1: printf("%02x\t POP H", code[0]); break;
  case 0xe5: printf("%02x\t PUSH H", code[0]); break;
  case 0xe6: printf("%02x %02x\t ANI #$%02x", code[0], code[1], code[1]); opbytes = 2; break;
  case 0xeb: printf("%02x\t XCHG", code[0]); break;
  case 0xf1: printf("%02x\t POP PSW", code[0]); break;
  case 0xf5: printf("%02x\t PUSH PSW", code[0]); break;
  case 0xfb: printf("%02x\t EI", code[0]); break;
  case 0xfe: printf("%02x %02x\t CPI #$%02x", code[0], code[1], code[1]); opbytes = 2; break;
  default:
    break;
  }

  cout << "\n";

  return opbytes;
}

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
  int limit = 10;
  
  state->pc = 0;
  while (true)
  {
    Disassemble8080p(buffer, state->pc);
    Emulate8080p(state);
  }

  return 0;
}
