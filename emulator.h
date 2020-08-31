#include "state8080.h"
#include <string>

class Emulator
{
private:
  State8080 *state;
  void unimplementedInstruction();
  void printCurrentState();
  void emulate8080p();
  int instructionCount;
public:
  Emulator();
  void emulate(std::string filename);
  int disassemble8080p(unsigned char* codebuffer, int pc);
};
