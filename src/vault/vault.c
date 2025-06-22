// vault.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // File operations like, access(), unlink()

#include "vault.h"
#include "../utils/helper.h"

char *MIGRATION_FILE = "/src/migrations/schema.sql";
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
    sqlite3_close(db);
    return NULL;
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
  
  free(path);

  fseek(file, 0, SEEK_END);
  size_t length = ftell(file);
  rewind(file);

  char *sql = malloc(length + 1);
  size_t bytes_read = fread(sql, 1, length, file);

  // I/O error?
  if (bytes_read != length)
  {
    free(sql);
    fclose(file);
    return 1;
  }

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
  int result = scanf(" %c", &confirm);
  if (result != 1)
  {
    // EOF or invalid input?
    exit(EXIT_FAILURE);
  }

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
 * !! Client must FREE the list or call free_personas_list(PersonaList $personalist) for safety
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
  
  PersonaList *list = malloc(sizeof(PersonaList));
  list->personas = calloc(MAX_PERSONAS, sizeof(Persona));
  list->count = 0;

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
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
 * !! Client must free the list or call free_persona(Persona $persona) for safety
 */
Persona *get_persona(const char *persona_name)
{
  sqlite3 *db = db_connection();
  if (db == NULL)
    return NULL;

  sqlite3_stmt *stmt;
  const char *sql = "SELECT id, username, persona FROM bosses WHERE username = ?;";

  Persona *persona = malloc(sizeof(Persona));

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK)
  {
    sqlite3_bind_text(stmt, 1, persona_name, strlen(persona_name), SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
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
 * !! Client must free the list or call free_persona(Persona $persona) for safety
 */
Persona *get_persona_by_id(const int persona_id)
{
  sqlite3 *db = db_connection();
  if (db == NULL)
    return NULL;

  sqlite3_stmt *stmt;
  const char *sql = "SELECT id, username, persona FROM bosses WHERE id = ?;";

  Persona *persona = malloc(sizeof(Persona));

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK)
  {
    sqlite3_bind_int(stmt, 1, persona_id);

    if (sqlite3_step(stmt) == SQLITE_ROW)
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
    sqlite3_bind_text(stmt, 1, persona_name, strlen(persona_name), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, persona, -1, SQLITE_STATIC);

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

// ====================
//    Manage Messages
// ====================

int add_scream(int persona_id, const char *message)
{
  sqlite3 *db = db_connection();
  sqlite3_stmt *stmt;

  const char *sql = "INSERT INTO emails (boss_id, body) VALUES (?, ?);";
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK)
  {
    sqlite3_bind_int(stmt, 1, persona_id);
    sqlite3_bind_text(stmt, 2, message, -1, SQLITE_STATIC);

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

ScreamList *get_screams(void)
{
  sqlite3 *db = db_connection();
  if (db == NULL)
    return NULL;

  sqlite3_stmt *stmt;
  const char *sql = "SELECT "
                    "e.id as scream_id, username, e.body as message, strftime('%Y-%m-%d %H:%M', e.created_at) as created_at,"
                    "COUNT(r.id) as total_replies "
                    " FROM emails e"
                    " INNER JOIN bosses b ON e.boss_id = b.id"
                    " LEFT JOIN emails r ON e.id = r.parent_id"
                    " WHERE e.parent_id IS NULL"
                    " GROUP BY e.id"
                    " ORDER BY e.id DESC"
                    " LIMIT 25;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
  {
    fprintf(stderr, "\n  [SQL ERROR]: %s\n", sqlite3_errmsg(db));

    // Cleanup
    if (stmt)
      sqlite3_finalize(stmt);

    sqlite3_close(db);
    return NULL;
  }

  // Allocate initial space
  ScreamList *list = malloc(sizeof(ScreamList));
  list->screams = calloc(MAX_LATEST_EMAILS, sizeof(Scream));
  list->count = 0;

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    Scream *scream = &list->screams[list->count];
    scream->scream_id = sqlite3_column_int(stmt, 0);
    scream->total_replies = sqlite3_column_int(stmt, 4);
    strcpy(scream->username, (char *)sqlite3_column_text(stmt, 1));
    strcpy(scream->created_at, (char *)sqlite3_column_text(stmt, 3));

    // message needs mem location
    const char *msg = (const char *)sqlite3_column_text(stmt, 2);
    scream->message = msg ? strdup(msg) : NULL;

    list->count++;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return list;
}

Scream *get_scream_and_replies(int scream_id)
{
  sqlite3 *db = db_connection();
  if (db == NULL || scream_id < 1)
    return NULL;

  sqlite3_stmt *stmt;
  const char *sql = "WITH RECURSIVE email_thread AS ("
                    "  SELECT id, boss_id, parent_id, body, created_at, 0 as level "
                    "  FROM emails WHERE id = ? "
                    "  UNION ALL "
                    "  SELECT e.id, e.boss_id, e.parent_id, e.body, e.created_at, et.level + 1 "
                    "  FROM emails e"
                    "  JOIN email_thread et ON e.parent_id = et.id"
                    ") "
                    "SELECT t.id, b.username, CASE WHEN parent_id IS NOT NULL THEN 1 ELSE 0 END AS is_reply, body as message, strftime('%Y-%m-%d %H:%M', t.created_at) as created_at "
                    " FROM email_thread t "
                    " LEFT JOIN bosses b ON t.boss_id = b.id"
                    " WHERE t.parent_id IS NULL OR t.parent_id IN (SELECT id FROM email_thread)"
                    " ORDER BY level, created_at";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
  {
    fprintf(stderr, "\n  [SQL ERROR]: %s\n", sqlite3_errmsg(db));

    // Cleanup
    if (stmt)
      sqlite3_finalize(stmt);
    sqlite3_close(db);
    return NULL;
  }

  // Allocate initial space based on data type
  Scream *scream = malloc(sizeof(Scream));
  scream->replies = calloc(MAX_LATEST_EMAILS, sizeof(ScreamReply));
  scream->total_replies = 0;

  // bind params
  sqlite3_bind_int(stmt, 1, scream_id);

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    int is_reply = sqlite3_column_int(stmt, 2);

    if (is_reply > 0)
    {
      ScreamReply *reply = &scream->replies[scream->total_replies];
      reply->scream_id = sqlite3_column_int(stmt, 0);
      strcpy(reply->username, (char *)sqlite3_column_text(stmt, 1));
      strcpy(reply->created_at, (char *)sqlite3_column_text(stmt, 4));

      // message needs mem location
      const char *reply_msg = (const char *)sqlite3_column_text(stmt, 3);
      reply->message = reply_msg ? strdup(reply_msg) : NULL;
    }
    else
    {
      scream->scream_id = sqlite3_column_int(stmt, 0);
      strcpy(scream->username, (char *)sqlite3_column_text(stmt, 1));
      strcpy(scream->created_at, (char *)sqlite3_column_text(stmt, 4));

      // message needs mem location
      const char *msg = (const char *)sqlite3_column_text(stmt, 3);
      scream->message = msg ? strdup(msg) : NULL;
    }

    scream->total_replies += 1;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return scream;
}

void free_screams_list(ScreamList *list)
{
  if (list)
  {
    free(list->screams);
    free(list);
  }
}

void free_scream(Scream *scream)
{
  if (scream)
  {
    free(scream->replies);
    free(scream);
  }
}
