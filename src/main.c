#include <stdio.h>
#include <time.h>
#include <stdlib.h>                   // for srand(), rand()
#include <string.h>

#include "utils/helper.h"
#include "vault/vault.h"
#include "commands/scream.h"
#include "commands/persona.h"

int main(int argc, char *argv[])
{
  // seed the RNG once. Ensures rand() functions well! #include <time.h>
  srand(time(NULL));

  // main menu: mailsream command alone
  if (argc < 2 || (argc > 1 && is_help_command(argv[1])))
  {
    show_help();
    return 1;
  }

  // ping
  if (argc == 2 && (strcmp(argv[1], "ping") == 0))
  {
    puts("\n    [ PONG ]\n");
    return 0;
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

  // manage or list personas
  if (argc >= 2 && argc <= 5 && (strcmp(argv[1], "persona") == 0 || strcmp(argv[1], "p") == 0))
  {
    // mailscream p = mailscream p list
    if (argc == 2)
      argv[2] = "list";

    return manage_persona(argc, argv);
  }
  // manage or list personas
  else if (argc == 2 && (strcmp(argv[1], "list") == 0 || strcmp(argv[1], "l") == 0))
  {
    return list_screams();
  }
  /**
   * Create new email
   * !!NOTE: LEAVE THIS BLOCK LAST!!
   */
  else if (argc >= 3 && argc <= 4)
  {
    return manage_screams(argc, argv);
  }

  printf("\nUnknown command [%s] or incorrect number of arguments passed.\n", argv[1]);
  suggest_help_manual(NULL);

  return 0;
}
