#include "state8080.h"

class Emulator
{
private:
  State8080 *state;
  uint16_t nextWord(uint16_t pc);
  void push(uint16_t r);
public:
  Emulator();
};
