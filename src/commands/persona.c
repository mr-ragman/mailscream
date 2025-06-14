#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../utils/helper.h"
#include "persona.h"

int manage_persona(int argc, char **argv)
{
  char *action = argv[2];
  char *persona_name = NULL;
  char *persona_description = NULL;
  int persona_id = 0;

  if (strcmp(action, "add") == 0)
  {
    persona_name = argv[3];

    if (argc == 5)
      persona_description = argv[4];
  }

  else if (strcmp(action, "remove") == 0)
  {
    persona_id = atoi(argv[3]);
    if (persona_id == 0)
    {
      fprintf(stderr, "\nError: Ooops, Persona ID can not be 0. Please specify the correct ID\n");
      return 1;
    }
  }

  if (strcmp(action, "list") == 0)
  {
    return _list_personas();
  }
  else if (strcmp(action, "add") == 0)
  {
    if (persona_name == NULL || strchr(persona_name, ' '))
    {
      display("Oops! Persona name cannot be empty or contain spaces.", "\nPlease use short names like: moody_boss, ChadBoss69, Karen.exe, etc");
      suggest_help_manual("persona");
      return 1;
    }

    if (persona_description == NULL || strlen(persona_description) <= 0)
    {
      display("Oops! Describe your persona.", "\nGive a short description of this persona! e.g Thinks she's the CEO because she's been here 'since day one'.");
      suggest_help_manual("persona");
      return 1;
    }

    int result = insert_persona(persona_name, persona_description);

    if (result == VAULT_SUCCESS)
    {
      printf("\n***Boom!*** Persona '%s' has joined the scream list. Ready to vent?\n\n", persona_name);
      suggest_help_manual("new");
      return 0;
    }

    display("Oops! Something went wrong.", "\nPlease to try to add them again.");
    return 1;
  }
  else if (strcmp(action, "remove") == 0 && argc == 4)
  {
    int result = delete_persona(persona_id);

    if (result == VAULT_SUCCESS)
    {
      printf("\n***Gone!*** Persona ID '%d' removed from your scream list.\n\n", persona_id);
      return _list_personas();
    }

    display("Oops! Something went wrong.", "\nPlease to try to add them again.");
    return 1;
  }
  else
  {
    suggest_help_manual("persona");
  }

  return 0;
}

/**
 * @ private
 */
int _list_personas(void)
{
  PersonaList *personas = get_personas();

  if (personas == NULL)
  {
    fprintf(stderr, "\nWarning: Ooops, you currently have no personas. Let's change that..\n");
    suggest_help_manual("persona");
    free_personas_list(personas);
    return 1;
  }

  puts("\n---------------------------------");
  printf(" You currently have %d personas\n", personas->count);
  puts("---------------------------------");

  if (personas->count > 0)
  {
    for (int i = 0; i < personas->count; i++)
    {
      Persona *p = &personas->personas[i];
      printf("%s (ID: %d)\n", p->username, p->id);
      printf("%s\n\n", p->persona);
      if (i < personas->count - 1)
        printf("-------\n");
    }
  }
  else
  {
    puts("\nCreate a new persona and let the venting begin!");
    suggest_help_manual("persona");
    puts("");
  }

  free_personas_list(personas);

  return 0;
}
