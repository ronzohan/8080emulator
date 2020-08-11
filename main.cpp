#include "emulator.h"
#include <string>

using namespace std;

int main(int argc, char **argv)
{
  Emulator emulator = Emulator();
  string filename = "invaders/invaders";
  emulator.emulate(filename);
  return 0;
}
