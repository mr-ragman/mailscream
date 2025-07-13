#include <stdlib.h>
#include <stdio.h> // PATH_MAX, readlink, strdup all available
#include <string.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h> // POSIX - For readlink()
#include <stdint.h> // uint32_t and other fixed-width types
#include <sys/ioctl.h>
#include <ctype.h>

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

  puts(" ===============================================");
  printf(" " BOLD "[ Scream instantly - Caps Lock is encouraged! ]" RESET "\n");
  puts(" ===============================================\n");
  puts(" - mailscream [--options] <persona_username> 'your message'\n");
  puts(" - mailscream [--options] <persona_id> 'your message'\n");

  printf("\n " BOLD "[ More actions: ]" RESET "\n\n");
  puts("  mailscream <command> [--options] <arguments, ...>\n");
  printf("\n  \033[1;33mCommands:\033[0m\n\n");
  printf("  \033[1;36mpersona, p\033[0m              Manage your personas. Personas are the people you want to scream to!\n");
  puts("                          Usage: [list]    mailscream p list");
  puts("                                 [new]     mailscream p add <persona_name> 'short description'");
  puts("                                 [remove]  mailscream p remove <ID>\n");
  printf("  \033[1;36mread, r\033[0m                 Read the email thread with the given ID\n");
  puts("                          Usage: mailscream r <ID>\n");
  printf("  \033[1;36mlist, l\033[0m                 Show all your stored emails (screams)\n");
  puts("                          Usage: mailscream l\n");
  printf("  \033[1;36mvault, db \033[0m              Initialize or drop your local scream vault\n");
  puts("                          Usage: mailscream db [init | drop]\n");
  printf("  \033[1;36mhelp, -h, --help\033[0m        Show this help screen\n");
  puts("                          Usage: mailscream help\n");

  // options
  printf("  \033[1;33mOptions:\033[0m\n\n");
  printf("  \033[1;36m--no-reply, -nr\033[0m         Your quiet emotional flush - No replies today!\n");
  puts("                          Usage: mailscream -nr <persona_username | ID> 'your message'\n");
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
  printf("│ %s │\n", header);

  // Print bottom dotted line
  for (int i = 0; i < len + 4; i++)
  {
    printf("─");
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
    puts(" - mailscream persona_username 'your scream message'");
    puts(" - mailscream persona_id 'your scream message'\n");
  }
  else if (strcmp(command_name, "read") == 0)
  {
    puts(" Usage:\n");
    puts(" - mailscream r <scream_id>\n");
  }
  else if (strcmp(command_name, "persona") == 0)
  {
    puts("\n Usage:\n");
    puts(" - New Persona:     mailscream p add <persona_name> 'describe your persona briefly. Do them justice'");
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

void load_dotenv(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (!file)
    return;

  char line[256];
  while (fgets(line, sizeof(line), file))
  {
    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n')
      continue;

    // Find the = sign
    char *equals = strchr(line, '=');
    if (!equals)
      continue;

    // Split key=value
    *equals = '\0';
    char *key = line;
    char *value = equals + 1;

    // Remove newline from value
    value[strcspn(value, "\r\n")] = '\0';

    // Set environment variable
    setenv(key, value, 1);
  }
  fclose(file);
}

int file_exists(const char *filename) {
  return access(filename, F_OK) != -1;
}

/**
 * Calculate the current terminal window size
 *
 * @return int
 */
int get_terminal_width()
{
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
  {
    return w.ws_col;
  }
  else
  {
    return 80;
  }
}

/**
 * Pretty prints.. ensures the text is nicely wrapped
 */
void print_wrapped(const char *message, int max_width, const char *indent)
{
  int indent_len = strlen(indent);
  int line_len = indent_len;
  printf("\n %s", indent);

  while (*message)
  {
    const char *word_start = message;

    // Skip whitespace
    while (*message && isspace(*message) && *message != '\n')
      message++;

    // Find the end of the word
    while (*message && !isspace(*message) && *message != '\n')
      message++;

    int word_len = message - word_start;

    // If the word won't fit on this line, wrap to next
    if (line_len + word_len + 1 > max_width)
    {
      printf("\n%s", indent);
      line_len = indent_len;
    }
    else if (line_len > indent_len)
    {
      line_len++;
    }

    // Print the word
    fwrite(word_start, 1, word_len, stdout);
    line_len += word_len;

    // Handle explicit newlines
    if (*message == '\n')
    {
      putchar('\n');
      printf("%s", indent);
      line_len = indent_len;
      message++;
    }
  }

  putchar('\n');
}
