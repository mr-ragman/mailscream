#include <stdio.h>
#include <string.h>
#include "utils/helper.h"

int main(int argc, char *argv[])
{
  // we must have arguments or show help screen
  if (argc < 2 || (argc > 1 && is_help_command(argv[1])))
  {
    show_help();
    return 1;
  }

  printf("Unknown command: %s\n", argv[1]);
  show_help();

  return 0;
}
