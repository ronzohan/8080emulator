#include "emulator.h"
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

Emulator::Emulator()
{
	state = (State8080 *)calloc(1, sizeof(State8080));
	state->memory = (uint8_t *)malloc(0x10000); //16Ks
	instructionCount = 0;
}

void Emulator::emulate(string filename)
{
	FILE *f = fopen(filename.c_str(), "r");
	if (f == NULL)
	{
		cout << "error: Couldn't open";
		return;
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
		// if (instructionCount > 42033) {
		// 	printCurrentState();
		// 	break;
		// }

		//std::cout << "Count: " << instructionCount << "\n";
		//disassemble8080p(buffer, state->pc);
		emulate8080p();
		
		instructionCount++;
	}
}

void Emulator::emulate8080p()
{
	unsigned char *opcode = &state->memory[state->pc];
	state->pc += 1;

	switch (*opcode)
	{
		// NOP
	case 0x00:
		break;
	case 0x01:
		state->setBC(state->nextWord());
		break;
	case 0x05:
	{
		uint16_t result = state->b - 1;
		state->setFlagsZSPAC(result);
		state->b = result;
		break;
	}
	case 0x06:
		state->b = state->nextByte();
		break;
	case 0x09:
	{
		uint16_t result = state->addWord(state->hl(), state->bc());
		state->setHL(result);
		break;
	}
	case 0x0d:
	{
		uint8_t result = state->c - 1;
		state->setFlagsZSPAC(result);
		state->c = result;
		break;
	}
	case 0x0e:
		state->c = state->nextByte();
		break;
	case 0x0f:
	{
		uint8_t x = state->a;
		state->a = (x >> 1) | (x & 1) << 7;
		uint16_t cy_result = (x & 1) << 8;
		state->setFlagCY(cy_result);
		break;
	}
	case 0x11:
		state->setDE(state->nextWord());
		break;
	case 0x13:
		state->setDE(state->de() + 1);
		break;
	case 0x1a:
		state->a = state->memory[state->de()];
		break;
	/*case 0x1c:
		state->e += 1;
		break;*/
	case 0x19:
	{
		uint16_t result = state->addWord(state->hl(), state->de());
		state->setHL(result);
		break;
	}
	case 0x21:
		state->setHL(state->nextWord());
		break;
	case 0x23:
		state->setHL(state->hl() + 1);
		break;
	case 0x26:
		state->h = state->nextByte();
		break;
	case 0x29:
	{
		uint16_t result = state->addWord(state->hl(), state->hl());
		state->setHL(result);
		break;
	}
	case 0x31:
		state->sp = state->nextWord();
		break;
	case 0x32:
	{
		uint16_t address = state->nextWord();
		state->memory[address] = state->a;
		break;
	}
		
	case 0x36:
		state->memory[state->hl()] = opcode[1];
		state->pc++;
		break;
	case 0x3a:
		state->a = state->memory[state->nextWord()];
		break;
	case 0x3e:
		state->a = state->nextByte();
		break;
	case 0x56:
		state->d = state->memory[state->hl()];
		break;
	case 0x5e:
		state->e = state->memory[state->hl()];
		break;
	case 0x66:
		state->h = state->memory[state->hl()];
		break;
	case 0x6f:
		state->l = state->a;
		break;
	case 0x7a:
		state->a = state->d;
		break;
	case 0x77:
		state->memory[state->hl()] = state->a;
		break;
	case 0x7b:
		state->a = state->e;
		break;
	case 0x7c:
		state->a = state->h;
		break;
	case 0x7e:
		state->a = state->memory[state->hl()];
		break;
	case 0xa7:
		state->aAND(state->a);
		break;
	case 0xaf:
		state->aXOR(state->a);
		break;
	case 0xc1:
		state->setBC(state->pop());
		break;
	case 0xc2: // JNZ
	{
		if (state->cc.z == 0)
			state->pc = state->getWord();
		else
			state->pc += 2;
		break;
	}
	case 0xc3:
		state->pc = state->getWord();
		break;
	case 0xc5:
		state->push(state->bc());
		break;
	case 0xc6:
	{
		uint16_t result = state->a + state->nextByte();
		state->a =  result & 0xff;
		state->setFlagsZSPACCY(result);
		break;
	}
	case 0xc9:
		state->pc = state->pop();
		break;
	case 0xcd:
	{
		uint16_t w = state->nextWord();
		state->push(state->pc);
		state->pc = w;
		break;
	}
	case 0xd1:
		state->setDE(state->pop());
		break;
	case 0xd3:
		// TODO: OUT Special
		state->pc++;
		break;
	case 0xd5:
		state->push(state->de());
		break;
	case 0xe1:
	{
		uint16_t result = state->pop();
		state->setHL(result);
		break;
	}
	case 0xe5:
		state->push(state->hl());
		break;
	case 0xe6:
	{
		uint8_t result = state->a & state->nextByte();;
		state->a = result;
		state->setFlagsZSPACCY(result);
		break;
	}
	case 0xeb:
	{
		uint16_t de = state->de();
		state->setDE(state->hl());
		state->setHL(de);
		break;
	}
	case 0xf1:
	{
		state->setPSW(state->pop());
		printf("here");
		break;
	}
	case 0xf5:
		state->push(state->af());
		break;
	case 0xfb:
		break;
	case 0xfe:
	{
		uint16_t result = state->a - opcode[1];
		state->setFlagsZSPACCY(result);
		state->pc++;
		break;
	}
	default:
		unimplementedInstruction();
		break;
	}
}

/*
  *codebuffer is a valid pointer to 8080 assembly code
  pc is the current offset into the code

  returns the number of bytes of the op
*/
int Emulator::disassemble8080p(unsigned char *codebuffer, int pc)
{
	unsigned char *code = &codebuffer[pc];
	int opbytes = 1;

	printf("%04x: ", pc);

	switch (*code)
	{
	case 0x00:
		printf("%02x NOP", code[0]);
		break;
	case 0x01:
		printf("LXI BC, %02x%02x", code[2], code[1]);
		break;
	case 0x05:
		printf("DCR B");
		break;
	case 0x09:
		printf("DAD B");
		break;
	case 0x0d:
		printf("DCR C");
		break;
	case 0x0e:
		printf("MVI C, %02x", code[1]);
		break;
	case 0x11:
		printf("LXI D, %02x%02x", code[2], code[1]);
		break;
	case 0x13:
		printf("INX D");
		break;
	case 0x1a:
		printf("LDAX D");
		break;
	case 0x1c:
		printf("INC E");
		break;
	case 0x19:
		printf("DAD D");
		break;
	case 0x21:
		printf("LXI H, %02x%02x", code[2], code[1]);
		break;
	case 0x23:
		printf("INX H");
		break;
	case 0x26:
		printf("MVI H, %02x", code[1]);
		break;
	case 0x29:
		printf("DAD H");
		break;
	case 0x32:
		printf("STA %02x%02x", code[2], code[1]);
		break;
	case 0x36:
		printf("MVI M, %02x", code[1]);
		break;
	case 0x3a:
		printf("LDA %02x%02x", code[2], code[1]);
		break;
	case 0x3e:
		printf("MVI A, %02x%02x", code[2], code[1]);
		break;
	case 0x5e:
		printf("MOV E, M");
		break;
	case 0x6f:
		printf("MOV L, A");
		break;
	case 0x77:
		printf("MOV M, A");
		break;
	case 0x7b:
		printf("MOV A, E");
		break;
	case 0x7c:
		printf("MOV A, H");
		break;
	case 0xa7:
		printf("ANA A");
		break;
	case 0xaf:
		printf("XRA A");
		break;
	case 0xc1:
		printf("POP BC");
		break;
	case 0xc2:
		printf("JNZ %02x%02x", code[2], code[1]);
		break;
	case 0xc5:
		printf("PUSH BC");
		break;
	case 0xc3:
		printf("JMP %02x%02x", code[2], code[1]);
		break;
	case 0xc9:
		printf("RET");
		break;
	case 0xcd:
		printf("CALL %02x%02x", code[2], code[1]);
		break;
	case 0xd3:
		printf("OUT %02x", code[1]);
		break;
	case 0xd5:
		printf("PUSH D");
		break;
	case 0xe5:
		printf("PUSH H");
		break;
	case 0xeb:
		printf("XCHG");
		break;
	case 0xf1:
		printf("POP PSW");
		break;
	case 0xfb:
		printf("EI");
		break;
	case 0xfe:
		printf("CPI %02x", code[1]);
		break;
	default:
		printf("%02x\t", code[0]);
		break;
	}

	printf("\n");

	return opbytes;
}

void Emulator::unimplementedInstruction()
{
	state->pc--;
	printCurrentState();

	//pc will have advanced one, so undo that
	cout << "Error: Unimplemented instruction\n";
	disassemble8080p(state->memory, state->pc);
	exit(1);
}

void Emulator::printCurrentState()
{
	printf("Instruction executed count: %d\n", instructionCount);
	/* print out processor state */
	printf("C=%d, P=%d, S=%d, Z=%d AC=%d", state->cc.cy, state->cc.p, state->cc.s, state->cc.z, state->cc.ac);
	printf("\nA:$%02x F:$%02x B:$%02x C:$%02x D:$%02x E:$%02x H:$%02x L:$%02x PC:%04x SP:%04x \n\n",
				 state->a, state->f, state->b, state->c, state->d,
				 state->e, state->h, state->l, state->pc, state->sp);
}