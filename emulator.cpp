#include "emulator.h"
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

Emulator::Emulator()
{
	state = (State8080*)calloc(1, sizeof(State8080));
	state->memory = (uint8_t*)malloc(0x10000);  //16Ks
}

void Emulator::emulate(string filename) 
{
	FILE* f = fopen(filename.c_str(), "r");
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
	uint8_t* buffer = &state->memory[0];
	fread(buffer, fsize, 1, f);
	fclose(f);

	int pc = 0;

	state->pc = 0;
	int count = 0;
	//1541
	while (true && count < 1560)
	{
		count++;
		std::cout << "Count: " << count << "\n";
		disassemble8080p(buffer, state->pc);
		emulate8080p();
	}
}

void Emulator::emulate8080p()
{
	unsigned char* opcode = &state->memory[state->pc];
	state->pc += 1;

	switch (*opcode)
	{
		// NOP
	case 0x00:
		break;
	case 0x05:
	{
		uint8_t result = state->b - 1;
		state->setFlagsZSPAC(result);
		state->b = result;
		break;
	}
	case 0x06:
		state->b = state->nextByte();
		break;
	case 0x11:
		state->setDE(state->nextWord());
		break;
	case 0x13:
		state->setDE(state->de() + 1);
		break;
	case 0x1a:
		// ERROR IN HERE
		state->a = state->memory[state->de()];
		break;
	case 0x21:
		state->setHL(state->nextWord());
		break;
	case 0x23:
		state->setHL(state->hl() + 1);
		break;
	case 0x31:
		state->sp = state->nextWord();
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
	case 0xc9:
		state->pop();
		break;
	case 0xcd:
	{
		uint16_t w = state->nextWord();
		state->push(state->pc);
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
		unimplementedInstruction();
		break;
	}

	/* print out processor state */
	std::printf("\nC=%d, P=%d, S=%d, Z=%d", state->cc.cy, state->cc.p, state->cc.s, state->cc.z);
	std::printf("\nA:$%02x B:$%02x C:$%02x D:$%02x E:$%02x H:$%02x L:$%02x PC:%04x SP:%04x\n\n",
		state->a, state->b, state->c, state->d,
		state->e, state->h, state->l, state->pc, state->sp);
}

/*
  *codebuffer is a valid pointer to 8080 assembly code
  pc is the current offset into the code

  returns the number of bytes of the op
*/
int Emulator::disassemble8080p(unsigned char* codebuffer, int pc)
{
	unsigned char* code = &codebuffer[pc];
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
	case 0xc2: printf("JNZ %02x%02x", code[2], code[1]); break;
	case 0xc3: printf("JMP %02x%02x", code[2], code[1]); break;
	case 0xc9: printf("RET"); break;
	case 0xcd: printf("CALL %02x%02x", code[2], code[1]); break;
	default:
		printf("%02x\t", code[0]);
		break;
	}

	printf("\n");

	return opbytes;
}

void Emulator::unimplementedInstruction()
{
	//pc will have advanced one, so undo that
	cout << "Error: Unimplemented instruction";
	disassemble8080p(state->memory, state->pc);
	exit(1);
}