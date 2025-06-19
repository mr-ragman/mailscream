#ifndef FLUSH_H
#define FLUSH_H

#define CONFIRMATION_COUNT 7

// create a new scream/email
int create_scream(int argc, char **argv);

// create a new scream/email
int list_screams(void);

// create a new scream/email
int flush_screams(int argc, char **argv);

void display_scream_table(const ScreamList *scream_list);

void print_scream_row(const Scream *scream);

void print_table_header(void);

void print_table_footer(void);

#endif // FLUSH_H
