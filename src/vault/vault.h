#ifndef VAULT_H
#define VAULT_H

#define MAX_PERSONAS 20

#define VAULT_SUCCESS 200

#define VAULT_FAILURE 500

#include <sqlite3.h>

typedef struct
{
  int id;
  char username[256];
  char persona[256];
} Persona;

typedef struct
{
  Persona *personas;
  int count;
} PersonaList;

// vault :: local DB
int vault_init();

int vault_drop();

void vault_help();

int vault_exists();

void ensure_vault_ready();

// personas
PersonaList *get_personas(void);

int insert_persona(char *persona_name, char *persona);

int delete_persona(int persona_id);

void free_personas_list(PersonaList *list);

#endif
