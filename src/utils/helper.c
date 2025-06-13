#include <stdio.h>
#include <string.h>

#include "helper.h"

void show_help()
{
  puts("");

  puts("___  ___      _ _ _____                               ");
  puts("|  \\/  |     (_) /  ___|                              ");
  puts("| .  . | __ _ _| \\ `--.  ___ _ __ ___  __ _ _ __ ___  ");
  puts("| |\\/| |/ _` | | |`--. \\/ __| '__/ _ \\/ _` | '_ ` _ \\");
  puts("| |  | | (_| | | /\\__/ / (__| | |  __/ (_| | | | | | |");
  puts("\\_|  |_/\\__,_|_|_\\____/ \\___|_|  \\___|\\__,_|_| |_| |_|");
  puts("------------------------------------------------------");
  puts("When your boss can't hear your scream but you write it anyway!.\n\n");

  printf("📬 \033[1;33mCommands:\033[0m\n\n");
  printf("  \033[1;36mnew\033[0m              Start a new emotional email\n");
  printf("  \033[1;36mlist\033[0m             Show all your stored emails\n");
  printf("  \033[1;36mread <id>\033[0m        Read the email with the given ID\n");
  printf("  \033[1;36mreply <id>\033[0m       Get a hilarious reply from your Boss (AI)\n");
  printf("  \033[1;36msettings\033[0m         Run config, migrations, or wipe the vault\n");
  printf("  \033[1;36mhelp\033[0m             Show this help screen\n");

  printf("\n🔥 \033[3mWrite it. Send it. Feel better.\033[0m\n\n");
}

int is_help_command(const char *arg)
{
  return strcmp(arg, "help") == 0 || strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0;
}
