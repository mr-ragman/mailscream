#include <stdio.h>
#include <string.h>
#include "utils/helper.h"
#include "vault/vault.h"

int main(int argc, char *argv[])
{
  // we must have arguments or show help screen
  if (argc < 2 || (argc > 1 && is_help_command(argv[1])))
  {
    show_help();
    return 1;
  }

  if (strcmp(argv[1], "vault") == 0)
  {
    if (argc < 3)
    {
      printf("vault instruction missing\n");
      vault_help();
      return 1;
    }

    if (strcmp(argv[2], "init") == 0)
      return vault_init();
    if (strcmp(argv[2], "drop") == 0)
      return vault_drop();

    printf("\nUnknown vault command.\n");
    vault_help();
    return 1;
  }

  // We should always DB ready
  ensure_vault_ready();

  printf("Unknown command: %s\n", argv[1]);
  show_help();

  return 0;
}
