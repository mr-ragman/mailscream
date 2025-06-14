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

  sqlite3 *db;
  int rc = sqlite3_open(DB_PATH, &db);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "❌ Can't open DB: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    free(sql);
    return 1;
  }

  char *errmsg = 0;
  rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "❌ Migration failed: %s\n", errmsg);
    sqlite3_free(errmsg);
    sqlite3_close(db);
    free(sql);
    return 1;
  }

  printf("✅ Vault initialized at: %s\n\n", DB_PATH);
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
