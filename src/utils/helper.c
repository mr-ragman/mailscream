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
  puts("They may not care, but your catharsis doesn't need permission!.\n\n");

  puts("mailscream <command> [--options] [arguments]\n");
  printf("📬 \033[1;33mCommands:\033[0m\n\n");
  printf("  \033[1;36mflush, f \033[0m               Flush your emotions and Let your boss reply hysterically!\n");
  puts("                          Usage: mailscream f [--options] [bossname] [message]\n");
  printf("  \033[1;36mreply, rp\033[0m               Reply to an earlier conversation/email with a given ID\n");
  puts("                          Usage: mailscream rp <ID> [message]\n");
  printf("  \033[1;36mread, r\033[0m                 Read the full email thread with the given ID\n");
  puts("                          Usage: mailscream r <ID>\n");
  printf("  \033[1;36mlist, l\033[0m                 Show all your stored emails\n");
  puts("                          Usage: mailscream l\n");
  printf("  \033[1;36mvault \033[0m                  Initialize or drop your local scream vault\n");
  puts("                          Usage: mailscream vault [init | drop]\n");
  printf("  \033[1;36mhelp, -h, --help\033[0m        Show this help screen\n");
  puts("                          Usage: mailscream help\n");

  // options
  printf("\n📬 \033[1;33mOptions:\033[0m\n\n");
  printf("  \033[1;36m--no-reply, -nr\033[0m    Your quiet emotional flush - No replies today!\n");
  puts("                     Usage: mailscream s -nr [bossname] [message]\n");
  printf("\n🔥 \033[3mWrite it. Send it. Feel better.\033[0m\n\n");
}

int is_help_command(const char *arg)
{
  return strcmp(arg, "help") == 0 || strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0;
}
