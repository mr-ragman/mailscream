// vault.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for file operations like, access(), unlink()
#include <sqlite3.h>

#include "vault.h"
#include "../utils/helper.h"

const char *MIGRATION_FILE = "./vault/migration.sql";

const char *DB_PATH = "./mailscream.db";

sqlite3 *db_connection()
{
  sqlite3 *db;
  int rc = sqlite3_open(DB_PATH, &db);

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
  return access(DB_PATH, F_OK) == 0;
}

int vault_init()
{
  if (vault_exists())
  {
    printf("\n📦 Vault already exists at: %s\n", DB_PATH);
    puts("Use `mailscream help` for more options. Enjoy! \n\n");
    return 0;
  }

  FILE *file = fopen(MIGRATION_FILE, "r");
  if (!file)
  {
    fprintf(stderr, "❌ Failed to open %s\n", MIGRATION_FILE);
    return 1;
  }

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

  printf("OK: Vault initialized at: %s\n\n", DB_PATH);
  sqlite3_close(db);
  free(sql);
  return 0;
}

int vault_drop()
{
  if (!vault_exists())
  {
    printf("ℹ️ No vault to drop at: %s\n", DB_PATH);
    return 0;
  }

  printf("⚠️ Are you sure you want to drop the vault? This cannot be undone. [y/N]: ");
  char confirm;
  scanf(" %c", &confirm);
  if (confirm != 'y' && confirm != 'Y')
  {
    printf("❌ Drop aborted.\n");
    return 1;
  }

  if (unlink(DB_PATH) == 0)
  {
    printf("🗑️ Vault dropped from: %s\n", DB_PATH);
    return 0;
  }
  else
  {
    perror("❌ Failed to drop vault");
    return 1;
  }
}

void ensure_vault_ready()
{
  if (!vault_exists())
  {
    printf("🚨 Vault not found.\n");
    vault_help();
    exit(1);
  }
}

void vault_help()
{
  puts("Use: vault [init | drop] or run `mailscream help` for more options.\n");
}

// Manage Persona
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
 * @internal
 */
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

/**
 * @internal
 */
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
