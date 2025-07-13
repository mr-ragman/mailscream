#ifndef HELP_H
#define HELP_H

#define BOLD "\033[1m"
#define RESET "\033[0m"

// fallback
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Ensure ssize_t is defined
#if !defined(_SSIZE_T) && !defined(_SSIZE_T_DEFINED)
#include <sys/types.h> // POSIX
#include <unistd.h>
#endif

int is_help_command(const char *arg);

void show_help();

void suggest_help_manual(char *command_name);

void display(const char *header, const char *message);

char *get_executable_dir();

char *get_full_path(const char *suffix);

void load_dotenv(const char *filename);

int file_exists(const char *filename);

int get_terminal_width();

// pretty prints.. wraps the text according to window/terminal size
void print_wrapped(const char *message, int max_width, const char *indent);

#endif
