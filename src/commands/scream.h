#ifndef FLUSH_H
#define FLUSH_H

#define CONFIRMATION_COUNT 7

int manage_screams(int argc, char **argv);

// create a new scream/email
int list_screams(void);

// create a new scream/email
int flush_screams(int argc, char **argv);

void display_screams_list_table(const ScreamList *scream_list);

void print_scream_and_replies(Scream *scream);

#endif // FLUSH_H
