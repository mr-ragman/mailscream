#include <stdlib.h>
#include <stdio.h>     // PATH_MAX, readlink, strdup all available
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>    // POSIX - For readlink()
#include <stdint.h>    // uint32_t and other fixed-width types

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
  puts("They may not care, but your catharsis needs no permission!\n\n");

  puts(" =====================");
  printf(" " BOLD "[ Scream instantly! ]" RESET "\n");
  puts(" =====================\n");
  puts(" - mailscream [--options] <persona_name> \"your message\"\n");
  puts(" - mailscream [--options] <persona_id> \"your message\"\n");

  printf("\n " BOLD "[ More actions: ]" RESET "\n\n");
  puts("  mailscream <command> [--options] <arguments, ...>\n");
  printf("\n  \033[1;33mCommands:\033[0m\n\n");
  printf("  \033[1;36mpersona, p\033[0m              Manage your scream personas\n");
  puts("                          Usage: mailscream p [list | add <persona_name> \"short description\" | remove <ID>]\n");
  printf("  \033[1;36mreply, rp\033[0m               Reply to an earlier conversation/email with a given ID\n");
  puts("                          Usage: mailscream rp <ID> \"your reply message\"\n");
  printf("  \033[1;36mread, r\033[0m                 Read the full email thread with the given ID\n");
  puts("                          Usage: mailscream r <ID>\n");
  printf("  \033[1;36mlist, l\033[0m                 Show all your stored emails\n");
  puts("                          Usage: mailscream l\n");
  printf("  \033[1;36mvault, db \033[0m              Initialize or drop your local scream vault\n");
  puts("                          Usage: mailscream db [init | drop]\n");
  printf("  \033[1;36mhelp, -h, --help\033[0m        Show this help screen\n");
  puts("                          Usage: mailscream help\n");

  // options
  printf("  \033[1;33mOptions:\033[0m\n\n");
  printf("  \033[1;36m--no-reply, -nr\033[0m    Your quiet emotional flush - No replies today!\n");
  puts("                     Usage: mailscream s -nr <name> \"your message\"\n");
}

/**
 * @internal
 *
 * Displays special messages with a header/title
 */
void display(const char *header, const char *message)
{
  int len = strlen(header);

  puts("");
  // Print top dotted line
  for (int i = 0; i < len + 4; i++)
  {
    printf("-");
  }
  printf("\n");

  // Print header with dots on sides
  printf(" %s \n", header);

  // Print bottom dotted line
  for (int i = 0; i < len + 4; i++)
  {
    printf("-");
  }
  printf("\n %s\n", message);
}

int is_help_command(const char *arg)
{
  return strcmp(arg, "help") == 0 || strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0;
}

void suggest_help_manual(char *command_name)
{
  if (command_name == NULL)
  {
    puts(" Run `mailscream help` for more options.\n");
  }
  else if (strcmp(command_name, "new") == 0)
  {
    puts(" Usage:\n");
    puts(" - mailscream persona_name \"your scream message\"");
    puts(" - mailscream persona_id \"your scream message\"\n");
  }
  else if (strcmp(command_name, "persona") == 0)
  {
    puts("\n Usage:\n");
    puts(" - New Persona:     mailscream p add <persona_name> \"describe your persona briefly. Do them justice!\"");
    puts(" - List Personas:   mailscream p list");
    puts(" - Remove Persona:  mailscream p remove <id>\n");
    puts(" For more options run `mailscream help`\n");
  }
  else if (strcmp(command_name, "vault") == 0)
  {
    puts("\n Usage:\n");
    puts(" - Use: mailscream vault init - to create new local vault.");
    puts(" - Use: mailscream vault drop - to remove your local vault.\n");
    puts(" For more options run `mailscream help`\n");
  }
  else
  {
    puts(" Run `mailscream help` for more options.\n");
  }
}

/**
 * Caller must free() the result!
 */
char *get_executable_dir()
{
  char *path = malloc(PATH_MAX);
  if (!path)
    return NULL;

  ssize_t count = readlink("/proc/self/exe", path, PATH_MAX - 1);
  if (count == -1)
  {
    free(path);
    return NULL;
  }
  path[count] = '\0';

  // Step 2: Extract directory (strip filename)
  char *dir = dirname(path); // dirname() modifies the input string!
  if (!dir)
  {
    free(path);
    return NULL;
  }

  // Step 3: Return a copy (dirname() may return static/stack memory)
  char *result = strdup(dir);
  free(path); // Free the original path
  return result;
}

/**
 * Caller must free the result
 */
char *get_full_path(const char *suffix)
{
  char *exe_dir = get_executable_dir();
  if (!exe_dir)
    return NULL;

  // Calculate total path length + null terminator
  size_t total_len = strlen(exe_dir) + strlen(suffix) + 1;
  char *migration_path = malloc(total_len);

  if (!migration_path)
  {
    free(exe_dir);
    return NULL;
  }

  // Build the full path
  strcpy(migration_path, exe_dir);
  strcat(migration_path, suffix);

  free(exe_dir);
  return migration_path;
}
