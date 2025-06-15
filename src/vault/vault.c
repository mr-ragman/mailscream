// vault.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for file operations like, access(), unlink()
#include <sqlite3.h>

#include "vault.h"
#include "../utils/helper.h"

char *MIGRATION_FILE = "/migrations/schema.sql";
const char *DB_PATH = "/mailscream.db";

sqlite3 *db_connection()
{
  char *path = get_full_path(DB_PATH);
  if (!path || path == NULL)
  {
    free(path);
    fprintf(stderr, "\n[ERROR] Oops! Unable to build sqlite database file\n\n");
    return NULL;
  }

  sqlite3 *db;
  int rc = sqlite3_open(path, &db);

  // free path
  free(path);

  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db); // Clean up even on failure
    return NULL;       // Return NULL to indicate failure
  }

  return db;
}

int vault_exists()
{
  char *path = get_full_path(DB_PATH);
  if (!path || path == NULL)
  {
    free(path);
    fprintf(stderr, "\n[ERROR] Oops! Unable to build sqlite database file\n\n");
    return 0;
  }

  int result = access(path, F_OK) == 0;
  free(path);

  return result;
}

int vault_init()
{
  if (vault_exists())
  {
    puts("\n[OK] Vault already exists");
    puts("\nUse `mailscream help` for more options. Enjoy! \n\n");
    return 0;
  }

  char *path = get_full_path(MIGRATION_FILE);
  if (!path || path == NULL)
  {
    free(path);
    fprintf(stderr, "\n[ERROR] Oops! Unable to identify database schema file\n\n");
    return 1;
  }

  FILE *file = fopen(path, "r");
  if (!file)
  {
    fprintf(stderr, "\n[ERROR] Oops! Failed to open migration file%s\n\n", path);
    free(path);
    return 1;
  }

  // we don't need path anymore
  free(path);

  // get file size
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);

  // load migration into memory
  char *sql = malloc(length + 1);
  fread(sql, 1, length, file);
  sql[length] = '\0';
  fclose(file);

  sqlite3 *db = db_connection();

  if (db == NULL)
  {
    puts("Error: Database connection failed!");
    free(sql);
    return 1;
  }

  char *errmsg = 0;
  int rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "Error: Migration failed: %s\n", errmsg);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    free(sql);
    return 1;
  }

  printf("\n[SUCCESS]: Your local vault has been initialized successfully.\n\n");
  suggest_help_manual(NULL);
  sqlite3_close(db);
  free(sql);
  return 0;
}

int vault_drop()
{
  char *path = get_full_path(DB_PATH);
  if (!path || path == NULL)
  {
    free(path);
    fprintf(stderr, "\n[ERROR] Oops! Unable to identify database file\n\n");
    return 1;
  }

  puts("[WARNING] Are you sure you want to drop your local vault? This cannot be undone. [y/N]: ");
  char confirm;
  scanf(" %c", &confirm);
  if (confirm != 'y' && confirm != 'Y')
  {
    puts("\n[ERROR] Drop aborted.\n");
    return 1;
  }

  if (unlink(path) == 0)
  {
    puts("\n[DROPPED] Vault dropped successfully!\n");
    free(path);
    return 0;
  }

  perror("\n[ERROR] Failed to drop vault");
  vault_help();
  puts("");
  free(path);
  return 1;
}

void ensure_vault_ready()
{
  if (!vault_exists())
  {
    puts("\n[ERROR] Oops, You have not yet initialized your local Vault (Database).\n");
    vault_help();
    exit(1);
  }
}

void vault_help()
{
  suggest_help_manual("vault");
}

// ====================
//    Manage Persona
// ====================

/**
 * client must free the list or call free_personas_list(PersonaList $personalist) for safety
 */
PersonaList *get_personas(void)
{
  sqlite3 *db = db_connection();
  if (db == NULL)
    return NULL;

  sqlite3_stmt *stmt;
  const char *sql = "SELECT id, username, persona FROM bosses ORDER BY id DESC;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return NULL;
  }

  // Allocate initial space
  PersonaList *list = malloc(sizeof(PersonaList));
  list->personas = calloc(MAX_PERSONAS, sizeof(Persona));
  list->count = 0;

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    // populate the memory space
    Persona *persona = &list->personas[list->count];
    persona->id = sqlite3_column_int(stmt, 0);
    strcpy(persona->username, (char *)sqlite3_column_text(stmt, 1));
    strcpy(persona->persona, (char *)sqlite3_column_text(stmt, 2));
    list->count++;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return list;
}

/**
 * client must free the list or call free_persona(Persona $persona) for safety
 */
Persona *get_persona(const char *persona_name)
{
  sqlite3 *db = db_connection();
  if (db == NULL)
    return NULL;

  sqlite3_stmt *stmt;
  const char *sql = "SELECT id, username, persona FROM bosses WHERE username = ?;";

  Persona *persona = malloc(sizeof(Persona));

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
  {
    sqlite3_bind_text(stmt, 2, persona_name, strlen(persona_name), SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      persona->id = sqlite3_column_int(stmt, 0);
      strcpy(persona->username, (char *)sqlite3_column_text(stmt, 1));
      strcpy(persona->persona, (char *)sqlite3_column_text(stmt, 2));
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return persona;
}

/**
 * client must free the list or call free_persona(Persona $persona) for safety
 */
Persona *get_persona_by_id(const int persona_id)
{
  sqlite3 *db = db_connection();
  if (db == NULL)
    return NULL;

  sqlite3_stmt *stmt;
  const char *sql = "SELECT id, username, persona FROM bosses WHERE id = ?;";

  Persona *persona = malloc(sizeof(Persona));

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
  {
    sqlite3_bind_int(stmt, 1, persona_id);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      persona->id = sqlite3_column_int(stmt, 0);
      strcpy(persona->username, (char *)sqlite3_column_text(stmt, 1));
      strcpy(persona->persona, (char *)sqlite3_column_text(stmt, 2));
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return persona;
}

int insert_persona(char *persona_name, char *persona)
{
  sqlite3 *db = db_connection();
  sqlite3_stmt *stmt;

  const char *sql = "INSERT INTO bosses (username, persona) VALUES (?, ?);";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK)
  {
    // bind params
    sqlite3_bind_text(stmt, 1, persona_name, strlen(persona_name), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, persona, -1, SQLITE_STATIC);

    // execute
    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return VAULT_SUCCESS;
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return VAULT_FAILURE;
}

int delete_persona(int persona_id)
{
  sqlite3 *db = db_connection();
  sqlite3_stmt *stmt;

  const char *sql = "DELETE FROM bosses WHERE id = ?;";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK)
  {
    sqlite3_bind_int(stmt, 1, persona_id);
    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return VAULT_SUCCESS;
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return VAULT_FAILURE;
}

void free_personas_list(PersonaList *list)
{
  if (list)
  {
    free(list->personas);
    free(list);
  }
}

void free_persona(Persona *persona)
{
  if (persona)
  {
    free(persona);
  }
}

