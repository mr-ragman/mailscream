#ifndef HELP_H
#define HELP_H

int is_help_command(const char *arg);

void show_help();

void suggest_help_manual(char *command_name);

#endif
