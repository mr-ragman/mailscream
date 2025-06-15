#include <stdio.h>
#include <string.h>
#include "utils/helper.h"
#include "vault/vault.h"
#include "commands/persona.h"

int main(int argc, char *argv[])
{
  // we must have arguments or show help screen
  if (argc < 2 || (argc > 1 && is_help_command(argv[1])))
  {
    show_help();
    return 1;
  }

  // vault/db management
  if (strcmp(argv[1], "vault") == 0 || strcmp(argv[1], "db") == 0)
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

  // add personas
  if (argc >= 2 && argc <= 5 && (strcmp(argv[1], "persona") == 0 || strcmp(argv[1], "p") == 0))
  {
    // automatically list current personas
    if (argc == 2)
      argv[2] = "list";

    return manage_persona(argc, argv);
  }

  printf("\nUnknown command [%s] or incorrect number of arguments passed.\n", argv[1]);
  suggest_help_manual(NULL);

  return 0;
}
