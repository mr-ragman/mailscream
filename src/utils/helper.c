#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include "helper.h"

void show_help()
{
  puts("\n___  ___      _ _ _____                               ");
  puts("|  \\/  |     (_) /  ___|                              ");
  puts("| .  . | __ _ _| \\ `--.  ___ _ __ ___  __ _ _ __ ___  ");
  puts("| |\\/| |/ _` | | |`--. \\/ __| '__/ _ \\/ _` | '_ ` _ \\");
  puts("| |  | | (_| | | /\\__/ / (__| | |  __/ (_| | | | | | |");
  puts("\\_|  |_/\\__,_|_|_\\____/ \\___|_|  \\___|\\__,_|_| |_| |_|");
  puts("------------------------------------------------------");
  puts("When your boss can't hear your scream but you write it anyway!.\n\n");

  printf("📬 \033[1;33mCommands:\033[0m\n\n");
  printf("  \033[1;36mscream, s [bossname] [message]\033[0m          Flush your emotions. Let the boss reply hysterically!\n");
  printf("  \033[1;36mreply, rp <id>\033[0m                          Reply to an earlier conversation/email with a given ID\n");
  printf("  \033[1;36mread, r <id>\033[0m                            Read the full email thread with the given ID\n");
  printf("  \033[1;36mlist, l\033[0m                                 Show all your stored emails\n");
  printf("  \033[1;36mvault [init | drop] \033[0m                    Initialize or drop your local scream vault\n");
  printf("  \033[1;36mhelp, -h, --help\033[0m                        Show this help screen\n");

  // options
  printf("\n\n📬 \033[1;33mOptions:\033[0m\n\n");
  printf("  \033[1;36m--no-reply, -nr\033[0m    Scream without reply from the mad boss. All power to you!!\n\n");
  printf("\n🔥 \033[3mWrite it. Send it. Feel better.\033[0m\n\n");
}

int is_help_command(const char *arg)
{
  return strcmp(arg, "help") == 0 || strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0;
}
