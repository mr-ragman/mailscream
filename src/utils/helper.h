#ifndef HELP_H
#define HELP_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L // Required for PATH_MAX on some systems
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>  // Required for _NSGetExecutablePath
#include <sys/param.h>    // For PATH_MAX on macOS
#endif

#ifdef __linux__
#include <unistd.h>       // For readlink()
#endif

// fallback
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Ensure ssize_t is defined
#if !defined(_SSIZE_T) && !defined(_SSIZE_T_DEFINED) && !defined(_WIN32)
#include <sys/types.h> // POSIX
#endif

int is_help_command(const char *arg);

void show_help();

void suggest_help_manual(char *command_name);

void display(const char *header, const char *message);

char *get_executable_dir();

char *get_full_path(const char *suffix);

#endif
