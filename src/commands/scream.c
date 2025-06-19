#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "../utils/helper.h"
#include "../vault/vault.h"
#include "scream.h"

// random success/confirmation messages
static const char *confirmation_messages[CONFIRMATION_COUNT] = {
    "\n  BOOM. Emotional payload deployed. Breathe in… and out.",
    "\n  Scream added! Deep breath… time to pretend you're okay.",
    "\n  Message delivered! The scream echoes through the void.",
    "\n  Filed under: catharsis > productive outbursts.",
    "\n  Scream stored. May it haunt your persona forever.",
    "\n  You screamed. The universe didn't flinch, but you feel lighter.",
    "\n  Therapy might be cheaper, but this is louder.",
};

const char *_get_random_confirmation()
{
  int index = rand() % CONFIRMATION_COUNT;
  return confirmation_messages[index];
}

int create_scream(int argc, char **argv)
{
  bool should_ai_reply = true;
  int persona_index = 1; // without options

  // if we have options
  if (strcmp(argv[1], "--no-reply") == 0 || strcmp(argv[1], "-nr") == 0)
  {
    should_ai_reply = false;
    persona_index++;
  }

  int persona_id = 0;
  char *persona_name = NULL;

  if (atoi(argv[persona_index]) > 0)
    persona_id = atoi(argv[persona_index]);
  else
    persona_name = argv[persona_index];

  const char *message = argv[persona_index + 1];

  if (persona_index + 1 >= argc)
  {
    fprintf(stderr, "\n[Error]: Invalid input. Missing message.\n");
    suggest_help_manual("new");
    return 1;
  }

  // Step 1: sanity check :: check if persona exists
  Persona *persona;
  if (persona_id > 0)
  {
    persona = get_persona_by_id(persona_id);
  }
  else
    persona = get_persona(persona_name);

  if (persona == NULL || persona->id <= 0)
  {
    display("[Oops!] This persona does not exit", "\n  Double check your persona's name or ID and try again.");
    return 1;
  }

  persona_id = persona->id;

  //
  char *username = malloc(strlen(persona->username) + 1);
  strcpy(username, persona->username);

  free_persona(persona);

  // Step 3: insert email
  int result = add_scream(persona_id, message);

  if (result == VAULT_SUCCESS)
  {
    int required_size = snprintf(NULL, 0, "[SUCCESS] Your scream was successfully sent to %s!", username) + 1;
    char *confirm_msg = malloc(required_size);
    if (!confirm_msg)
    {
      perror("Memory allocation failed");
      exit(EXIT_FAILURE);
    }

    snprintf(confirm_msg, required_size, "[SUCCESS] Your scream was successfully sent to %s!", username);

    // Step 4: maybe generate reply
    if (should_ai_reply)
    {
      display(confirm_msg, "");

      /**
       * ping LLM
       * TODO: Hook in to MCP/LLM APIs
       */
      const char *ai_response = _get_random_confirmation();

      printf("   " BOLD "[You]" RESET ": %s \n", message);
      printf("\n   " BOLD "[%s]" RESET ": %s \n\n", username, ai_response);
    }
    else
    {
      // random reply
      display(confirm_msg, _get_random_confirmation());
    }

    free(confirm_msg);
    free(username);
    return 0;
  }

  free(username);

  // issue with database insert
  display("Oops! Problem registering your scream", "Please try again.");
  return 1;
}

int list_screams(void)
{
  ScreamList *screams = get_screams();

  if (screams == NULL)
  {
    display("[Hey!] No vents to see here!", "Got someone to screan at today?\n");
    suggest_help_manual("new");
    free_screams_list(screams);
    return 1;
  }

  puts("\n ------------------------------------");
  printf(" List of your %d most recent screams!\n", screams->count);
  puts(" ------------------------------------");

  if (screams->count > 0)
  {
    display_scream_table(screams);
  }
  else
  {
    display("[Hey!] No vents to see here!", "Got someone to screan at today?\n");
    suggest_help_manual("new");
    puts("");
  }

  free_screams_list(screams);

  return 0;
}

void print_table_header(void)
{
  printf("┌─────────┬─────────────────┬────────────────────────────────────────┬─────────┬─────────────────────┐\n");
  printf("│ %-7s │ %-15s │ %-38s │ %-7s │ %-19s │\n",
         "ID", "Sent To", "Message", "Replies", "Created At");
  printf("├─────────┼─────────────────┼────────────────────────────────────────┼─────────┼─────────────────────┤\n");
}

void print_table_footer(void)
{
  printf("└─────────┴─────────────────┴────────────────────────────────────────┴─────────┴─────────────────────┘\n");
}

void print_scream_row(const Scream *scream)
{
  // Truncate message if too long then add ...
  char truncated_message[39];
  if (scream->message && strlen(scream->message) > 38)
  {
    strncpy(truncated_message, scream->message, 35);
    truncated_message[35] = '.';
    truncated_message[36] = '.';
    truncated_message[37] = '.';
    truncated_message[38] = '\0';
  }
  else
  {
    strncpy(truncated_message, scream->message ? scream->message : "", sizeof(truncated_message) - 1);
    truncated_message[sizeof(truncated_message) - 1] = '\0';
  }

  printf("│ %-7d │ %-15s │ %-38s │ %-7d │ %-19s │\n",
         scream->scream_id,
         scream->username,
         truncated_message,
         scream->total_replies,
         scream->created_at);

  // got to respect the contract!
  free(scream->message);
}

void display_scream_table(const ScreamList *scream_list)
{
  if (!scream_list || scream_list->count == 0)
  {
    printf("No screams to display.\n");
    return;
  }

  print_table_header();

  for (int i = 0; i < scream_list->count; i++)
  {
    print_scream_row(&scream_list->screams[i]);
  }

  print_table_footer();
}
