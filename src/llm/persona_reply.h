#ifndef AI_REPLY_H
#define AI_REPLY_H

#define MAX_PROMPT_SIZE 4096

#if !defined(_SSIZE_T) && !defined(_SSIZE_T_DEFINED)
#include <sys/types.h> // POSIX
#endif

// Structure for API responses
typedef struct
{
  char *data;
  size_t size;
} APIResponse;

char *generate_ai_response(const char *scream_text, const char *persona_name, const char *persona_description);

#endif
