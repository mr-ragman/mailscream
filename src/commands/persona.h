#ifndef PERSONA_H
#define PERSONA_H

#include "../vault/vault.h"

int manage_persona(int argc, char **argv);

/**
 * @private
 */
int _list_personas(void);

void display_table(const PersonaList *list);

#endif
