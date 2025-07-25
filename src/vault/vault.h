#ifndef VAULT_H
#define VAULT_H

#define MAX_PERSONAS 20

#define MAX_LATEST_EMAILS 25

#define VAULT_SUCCESS 200

#define VAULT_FAILURE 500

// icludes
#include <stdbool.h>
#include <sqlite3.h>

// ======================================
// vault :: local DB
// ======================================
int vault_init();

int vault_drop();

void vault_help();

int vault_exists();

void ensure_vault_ready();

// safely copy text from stmt to dest/struct item
void _safe_column_text_copy(sqlite3_stmt *stmt, int col_index, char *dest, size_t dest_size);

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

Persona *get_persona_by_id(const int persona_id);

void free_personas_list(PersonaList *list);

void free_persona(Persona *persona);

char *get_migration_path(void);

// ======================================
// Screams/Messages
// ======================================

typedef struct
{
  int scream_id;
  char username[100]; // too much but, hey!
  char *message;
  char created_at[20];
} ScreamReply;

typedef struct
{
  int scream_id;
  char username[100]; // too much but, hey!
  char *message;
  int total_replies;
  char created_at[20];
  ScreamReply *replies;
} Scream;

typedef struct
{
  Scream *screams;
  int count;
} ScreamList;

/**
 * @internal
 * TODO: validate args
 * 
 * This function adds a new scream.
 * 
 * @returns the last inserted ID of the scream (sqlite3_int64). -1 means failure
 */
sqlite3_int64 add_scream(const int persona_id, const char *message);

/**
 * @internal
 * TODO: validate args
 * 
 * This function will add a reply from AI and associate it with the original scream
 * 
 * @returns status of the operation [VAULT_SUCCESS | VAULT_FAILURE]
 */
int add_scream_reply(const int persona_id, const int parent_message_id, const char *reply);

ScreamList *get_screams(void);

void free_screams_list(ScreamList *list);

Scream *get_scream_and_replies(int scream_id);

void free_scream(Scream *scream);

#endif
