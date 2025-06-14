#ifndef VAULT_H
#define VAULT_H

#include <sqlite3.h>

// build local vault
int vault_init();

// drop local vault
int vault_drop();

void vault_help();

int vault_exists();

void ensure_vault_ready();

#endif
