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
      display("[Oops!] Persona name cannot be empty or contain spaces.", "\nPlease use short names like: moody_boss, ChadBoss69, Karen.exe, etc");
      suggest_help_manual("persona");
      return 1;
    }

    if (persona_description == NULL || strlen(persona_description) <= 0)
    {
      display("[Oops!] Describe your persona.", "\nGive a short description of this persona! e.g Thinks she's the CEO because she's been here 'since day one'.");
      suggest_help_manual("persona");
      return 1;
    }

    int result = insert_persona(persona_name, persona_description);

    if (result == VAULT_SUCCESS)
    {
      printf("\n[SUCCESS] Persona '%s' added to scream list. Ready to vent?\n\n", persona_name);
      suggest_help_manual("new");
      return 0;
    }

    display("[Oops!] Something went wrong while adding your persona.", "\nPlease ensure your persona name is unique and try again.");
    return 1;
  }
  else if (strcmp(action, "remove") == 0 && argc == 4)
  {
    int result = delete_persona(persona_id);

    if (result == VAULT_SUCCESS)
    {
      printf("\n[GONE!] Persona ID '%d' removed from your scream list.\n\n", persona_id);
      return _list_personas();
    }

    display("[Oops!] Something went wrong.", "\nPlease to try to add them again.");
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
    display_table(personas);
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

static void print_table_header(void)
{
  printf("┌─────────┬─────────────────┬────────────────────────────────────────────────────┐\n");
  printf("│ %-7s │ %-15s │ %-50s │\n", "ID", "Username", "Their Persona");
  printf("├─────────┼─────────────────┼────────────────────────────────────────────────────┤\n");
}

static void print_table_footer(void)
{
  printf("└─────────┴─────────────────┴────────────────────────────────────────────────────┘\n");
}

static void print_table_row(const Persona *persona)
{
  // Truncate message if too long then add ...
  char truncated_message[51];
  if (persona->persona && strlen(persona->persona) > 50)
  {
    strncpy(truncated_message, persona->persona, 47);
    truncated_message[47] = '.';
    truncated_message[48] = '.';
    truncated_message[49] = '.';
    truncated_message[50] = '\0';
  }
  else
  {
    strncpy(truncated_message, persona->persona ? persona->persona : "", sizeof(truncated_message) - 1);
    truncated_message[sizeof(truncated_message) - 1] = '\0';
  }

  printf("│ %-7d │ %-15s │ %-50s │\n", persona->id, persona->username, truncated_message);
}

void display_table(const PersonaList *list)
{
  if (!list || list->count == 0)
  {
    printf("No personas found to display.\n");
    return;
  }

  print_table_header();

  for (int i = 0; i < list->count; i++)
  {
    print_table_row(&list->personas[i]);
  }

  print_table_footer();
}
