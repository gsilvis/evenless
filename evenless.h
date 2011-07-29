/* Copyright (C) 2010 George Silvis, III */

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


/* Throughout this file, if not mentioned otherwise, any function that returns an int
 * returns 0 if successful, and 1 if unsuccessful */

/* returns a query that matches all messages in a thread. Free the query yourself. */
notmuch_query_t*
get_query_for_thread (notmuch_database_t* database,
                      notmuch_thread_t* thread);

/* Adds the tag 'tag' to all messages in the thread 'thread' */
int
tag_entire_thread (notmuch_database_t* database,
                   notmuch_thread_t* thread,
                   const char* tag);

/* remove the tag 'tag' from all messages in thread 'thread' */
int
untag_entire_thread (notmuch_database_t* database,
                     notmuch_thread_t* thread,
                     const char* tag);

/* Asks the user to input a string. You must free it yourself.
 * 'prompt' is how you want to prompt the user.
 * It should not end with a newline. */
char*
get_string_input (const char* prompt);

/* Asks the user to input an integer. 'prompt' is how you want to prompt the user. */
int
get_int_input (const char* prompt);

/* Steps the user through the action of adding some tag to the thread 'thread' */
int
action_tag_thread (notmuch_database_t* database,
                   notmuch_thread_t* thread);

/* As above, but removes a tag */
int
action_untag_thread (notmuch_database_t* database,
                     notmuch_thread_t* thread);

/* Specifically removes the 'inbox' tag from a thread */
int
action_archive (notmuch_database_t* database,
                notmuch_thread_t* thread);

/* Specifically removes the 'unread' tag from a thread */
int
action_read (notmuch_database_t* database,
             notmuch_thread_t* thread);

/* Prompts the user to select a thread. */
/* Used by all functions below with 'select' in their name */
notmuch_thread_t*
action_select_thread (notmuch_database_t* database,
                      notmuch_threads_t* threads);

/* Prompts user to select a thread, and then to enter a tag to add to the thread */
int
action_select_tag (notmuch_database_t* database,
                   notmuch_query_t* query);

/* Select a thread, then enter a tag to remove from it. */
int
action_select_untag (notmuch_database_t* database,
                     notmuch_query_t* query);

/* Shells out to mutt, so the user can compose a new email */
int
action_compose (void);

/* Shells out to mutt, so the user can view the thread 'thread' */
int
action_view (notmuch_thread_t* thread);

/* Prompts user to select a thread, then shows it in mutt */
int
action_select_view (notmuch_database_t* database,
                    notmuch_query_t* query);

/* Prompts user to select a thread, then removes the 'inbox' tag from it */
int
action_select_archive (notmuch_database_t* database,
                       notmuch_query_t* query);

/* Prompts user to select a thread, then removes the 'unread' tag from it */
int
action_select_read (notmuch_database_t* database,
                    notmuch_query_t* query);

/* Prompts user to select a thread, then removes 'inbox' and 'unread' from it */
int
action_select_read_archive (notmuch_database_t* database,
                            notmuch_query_t* query);

/* Prints out the results of a query, returns the number of threads. */
int
printquery (notmuch_database_t* database,
            notmuch_query_t* query);

/* Runs a query and lets the user interact with it */
int
runquery (notmuch_database_t* database,
          char* string);

/* Main */
int
main (int argc,
      char** argv);

#endif /* __EVENLESS_H */
