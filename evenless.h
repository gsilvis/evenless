/* Copyright (C) 2011 George Silvis, III <george.iii.silvis@gmail.com> */

/*
This file is part of evenless.

evenless is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

evenless is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
evenless.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __EVENLESS_H
#define __EVENLESS_H

#include <notmuch.h>

#define EVENLESS_MODE_TTYUI 1
#define EVENLESS_MODE_CURSES 2

#define EVENLESS_NULL_SELECTION -1

#define EVENLESS_VERSION "0.0.4-rc1"


typedef struct {
  int evenless_mode;
  char *config_path;
  char *database_path;
  char *search_string;
  notmuch_database_t *database;
  notmuch_query_t *query;
  notmuch_threads_t *threads;
  int num_entries;
  notmuch_thread_t **entries;
  int top_displayed;
  int selected;
} Client;



/* Throughout this file, if not mentioned otherwise, any function that returns an int
 * returns 0 if successful, and 1 if unsuccessful */


void
get_config_path (Client *c);

void
get_database_path (Client *c);

void
get_database (Client *c);

void
get_query (Client *c);

void
get_threads (Client *c);

void
get_entries (Client *c);

void
get_selected (Client *c);

void
print_ttyui (Client *c);

void
run_ttyui (Client *c);




/* returns a query that matches all messages in a thread. Free the query yourself. */
notmuch_query_t*
get_query_for_thread (notmuch_database_t *database,
                      notmuch_thread_t *thread);

/* Adds the tag 'tag' to all messages in the thread 'thread' */
int
tag_entire_thread (notmuch_database_t *database,
                   notmuch_thread_t *thread,
                   const char *tag);

/* remove the tag 'tag' from all messages in thread 'thread' */
int
untag_entire_thread (notmuch_database_t *database,
                     notmuch_thread_t *thread,
                     const char *tag);

/* Asks the user to input a string. You must free it yourself.
 * 'prompt' is how you want to prompt the user.
 * It should not end with a newline. */
char*
get_string_input (const char *prompt);

/* Asks the user to input an integer. 'prompt' is how you want to prompt the user. */
int
get_int_input (const char *prompt);

/* Steps the user through the action of adding some tag to the thread 'thread' */
int
action_tag_thread (Client *c);

/* As above, but removes a tag */
int
action_untag_thread (Client *c);

/* Specifically removes the 'inbox' tag from a thread */
int
action_archive (Client *c);

/* Specifically removes the 'unread' tag from a thread */
int
action_read (Client *c);

int
action_read_archive (Client *c);

/* Shells out to mutt, so the user can compose a new email */
int
action_compose (Client *c);

/* Shells out to mutt, so the user can view the thread 'thread' */
int
action_view (Client *c);

void
initialize_client (Client *c);

void
parse_args (Client *c,
            int argc,
            char **argv);

int
cleanup_client (Client *c);





/* Prints out the results of a query, returns the number of threads. */
int
printquery (notmuch_database_t* database,
            notmuch_query_t* query);

/* Runs a query and lets the user interact with it */
int
runquery (notmuch_database_t* database,
          char* string);

#endif /* __EVENLESS_H */
