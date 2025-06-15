#ifndef VAULT_H
#define VAULT_H

#define MAX_PERSONAS 20

#define VAULT_SUCCESS 200

#define VAULT_FAILURE 500

#include <sqlite3.h>

// ======================================
// vault :: local DB
// ======================================
int vault_init();

int vault_drop();

void vault_help();

int vault_exists();

void ensure_vault_ready();

// ======================================
// personas
// ======================================
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

PersonaList *get_personas(void);

/**
 * @internal
 * TODO: validate args
 */
int insert_persona(char *persona_name, char *persona);

/**
 * @internal
 * TODO: validate args
 */
int delete_persona(int persona_id);

Persona *get_persona(const char *persona_name);

Persona *get_persona_by_id(const int persona_name);

void free_personas_list(PersonaList *list);

void free_persona(Persona *persona);

char *get_migration_path(void);
