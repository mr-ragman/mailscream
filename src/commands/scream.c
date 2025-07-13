#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sqlite3.h>

#include "../utils/helper.h"
#include "../vault/vault.h"
#include "../llm/persona_reply.h"
#include "scream.h"

// random success/confirmation messages
static const char *confirmation_messages[CONFIRMATION_COUNT] = {
    "\n  BOOM. Emotional payload deployed. Breathe in… and out.\n",
    "\n  Scream saved into the void! Deep breath… time to pretend you're okay.\n",
    "\n  Message delivered! The scream echoes through the void.\n",
    "\n  Filed under: catharsis > productive outbursts.\n",
    "\n  Scream stored into the void. May it haunt your persona forever.\n",
    "\n  You screamed. The universe didn't flinch, but you feel lighter.\n",
    "\n  Therapy might be cheaper, but this is louder.\n",
};

const char *_get_random_confirmation()
{
  int index = rand() % CONFIRMATION_COUNT;
  return confirmation_messages[index];
}

static void print_indented(const char *message)
{
  int max_width = get_terminal_width();
  if (max_width <= 0)
    max_width = 80;

  // if interactive terminal
  if (isatty(STDOUT_FILENO))
  {
    print_wrapped(message, max_width, "    ");
  }
  else
  {
    printf("    %s", message);
  }

  printf("\n\n");
}

static int create_scream(int argc, char **argv)
{
  bool should_ai_reply = true;
  int persona_index = 1;

  // if we have command options/flags
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
  {
    persona = get_persona(persona_name);
  }

  if (persona == NULL || persona->id <= 0)
  {
    display("Oops, This persona does not exit", "\n Please, double check your persona's username or ID and try again.\n\n");
    free_persona(persona);
    return 1;
  }

  persona_id = persona->id;  
  char *username = malloc(strlen(persona->username) + 1);
  strcpy(username, persona->username);

  // Step 3: insert email
  sqlite3_int64 last_id = add_scream(persona_id, message);

  if (last_id > 0)
  {
    int required_size = snprintf(NULL, 0, "SENT! Your scream was successfully delivered to %s!", username) + 1;
    char *confirm_msg = malloc(required_size);
    if (!confirm_msg)
    {
      perror("Memory allocation failed");
      exit(EXIT_FAILURE);
    }

    snprintf(confirm_msg, required_size, "SENT! %s knows exactly how you feel now!", username);

    // Step 4: maybe generate reply
    if (should_ai_reply)
    {
      display(confirm_msg, "");
      printf(" ...let's see what %s has to say about that... please wait.\n", username);

      // ping LLM
      char *summary = generate_ai_response(
          message,
          username,
          persona->persona);

      if (summary != NULL)
      {
        (void)add_scream_reply(persona_id, last_id, summary);

        printf("\n   " BOLD "[%s]" RESET ": \n", username);
        print_indented(summary);
      }
      else
      {
        printf("\n   " BOLD "[Ooops, LLM Failed To Reply. But hey...]" RESET ": %s \n\n", _get_random_confirmation());
      }

      free(summary);
    }
    else
    {
      // random reply
      display(confirm_msg, _get_random_confirmation());
    }

    free(confirm_msg);
    free(username);
    free_persona(persona);
    return 0;
  }

  free_persona(persona);
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
    display("[Hey!] No vents to see here!", "Got someone to scream at today?\n");
    suggest_help_manual("new");
    free_screams_list(screams);
    return 1;
  }

  if (screams->count > 0)
  {
    puts("\n ------------------------------------");
    printf(" >>   Your %d most recent screams!\n", screams->count);
    puts(" ------------------------------------\n");

    display_screams_list_table(screams);
  }
  else
  {
    display("Oops! No vents to see here.", "=> Got someone to scream at today?\n");
    suggest_help_manual("new");
    puts("");
  }

  free_screams_list(screams);

  return 0;
}

int read_scream(int argc, char **argv)
{
  if (argc != 3 || atoi(argv[2]) <= 0)
  {
    display("[Oops] Invalid number of arguments", "Please check the manual and try again\n");
    suggest_help_manual("read");
    return 1;
  }

  Scream *scream = get_scream_and_replies(atoi(argv[2]));

  if (scream == NULL)
  {
    display("Oops! No vents to see here!", "Got someone to scream at today?\n");
    suggest_help_manual("new");
    free_scream(scream);
    return 1;
  }

  if (scream->scream_id > 0)
  {
    puts("\n ------------------------------------");
    printf(" >> You are viewing a scream ID: %d!\n", atoi(argv[2]));
    puts(" ------------------------------------");

    print_scream_and_replies(scream);
  }
  else
  {
    printf("\n Sorry, no scream found with ID %s. Want to create a new one?\n\n", argv[2]);
    suggest_help_manual("new");
    puts("");
  }

  free_scream(scream);

  return 0;
}

static void print_table_header(void)
{
  printf("┌─────────┬─────────────────┬────────────────────────────────────────┬─────────┬─────────────────────┐\n");
  printf("│ %-7s │ %-15s │ %-38s │ %-7s │ %-19s │\n",
         "Scr. ID", "Sent To", "Message", "Replies", "Created At");
  printf("├─────────┼─────────────────┼────────────────────────────────────────┼─────────┼─────────────────────┤\n");
}

static void print_table_footer(void)
{
  printf("└─────────┴─────────────────┴────────────────────────────────────────┴─────────┴─────────────────────┘\n");
}

static void print_scream_row(const Scream *scream)
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

int manage_screams(int argc, char **argv)
{
  if (strcmp(argv[1], "read") == 0 || strcmp(argv[1], "r") == 0)
  {
    return read_scream(argc, argv);
  }
  else
  {
    return create_scream(argc, argv);
  }

  return 0;
}

void display_screams_list_table(const ScreamList *scream_list)
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

static void print_scream_reply(ScreamReply *scream_reply)
{
  printf("  ↳  " BOLD "[From: %s]" RESET " on %s\n", scream_reply->username, scream_reply->created_at);
  print_indented(scream_reply->message);
  free(scream_reply->message);
}

static void print_main_scream(Scream *scream)
{
  puts("");
  printf("  To: %s\n", scream->username);
  printf("  Date: %s\n", scream->created_at);
  printf("  Message:\n  ->  %s\n\n", scream->message);

  free(scream->message);
}

void print_scream_and_replies(Scream *scream)
{
  // Print main scream
  print_main_scream(scream);

  if (scream->total_replies > 0)
  {
    for (int j = 0; j < scream->total_replies; j++)
    {
      if ((&scream->replies[j])->scream_id > 0)
      {
        print_scream_reply(&scream->replies[j]);
      }
    }
  }
  else
  {
    printf("(No replies)\n");
  }
}
