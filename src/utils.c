
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <notmuch.h>
#include <ncurses.h>

#include "utils.h"





int
output_message (const char *message) {
  int y = getmaxy(stdscr);

  move(y,0);
  clrtoeol();
  printw("%s", message);

  return 1;
}

/* returns a query that matches all messages in a thread. Free the query yourself. */
notmuch_query_t*
get_query_for_thread (notmuch_database_t *database,
                      notmuch_thread_t *thread) {
  
  const char *threadid = notmuch_thread_get_thread_id(thread);
  const char *prefix = "thread:";
  char *search_term = malloc(snprintf(NULL, 0, "%s%s", prefix, threadid) + 1);
  sprintf(search_term, "%s%s", prefix, threadid);

  notmuch_query_t *query = notmuch_query_create(database, search_term);

  free(search_term);
  return query;
}


/* Adds the tag 'tag' to all messages in the thread 'thread' */
int
tag_entire_thread (notmuch_database_t* database,
                   notmuch_thread_t* thread,
                   const char* tag) {
  notmuch_query_t* query = get_query_for_thread(database, thread);
  notmuch_messages_t* messages;
  for (messages = notmuch_query_search_messages(query);
       notmuch_messages_valid(messages);
       notmuch_messages_move_to_next(messages)) {
    notmuch_message_t* message = notmuch_messages_get(messages);
    if (message==NULL) {
      return 1; /* out of memory */
    }
    switch (notmuch_message_add_tag(message, tag)) {
    case NOTMUCH_STATUS_SUCCESS:
      output_message("Tagging succeeded.");
      break;
    case NOTMUCH_STATUS_NULL_POINTER:
      output_message("Bad tag pointer.");
      break;
    case NOTMUCH_STATUS_TAG_TOO_LONG:
      output_message("Very long tag name.");
      break;
    case NOTMUCH_STATUS_READ_ONLY_DATABASE:
      output_message("Database read only.");
      break;
    default:
      output_message("What the fuck, notmuch.");
      break;
    }
    notmuch_message_destroy(message);
  }
  notmuch_messages_destroy(messages);
  notmuch_query_destroy(query);
  return 0;
}


/* remove the tag 'tag' from all messages in thread 'thread' */
int
untag_entire_thread (notmuch_database_t *database,
                     notmuch_thread_t *thread,
                     const char *tag) {
  notmuch_query_t *query = get_query_for_thread(database, thread);
  notmuch_messages_t *messages;
  for (messages = notmuch_query_search_messages(query);
       notmuch_messages_valid(messages);
       notmuch_messages_move_to_next(messages)) {
    notmuch_message_t *message = notmuch_messages_get(messages);
    if (message==NULL) {
      return 1; /* out of memory */
    }
    notmuch_message_remove_tag(message, tag);
    notmuch_message_destroy(message);
  }
  notmuch_messages_destroy(messages);
  notmuch_query_destroy(query);
  return 0;
}


/* Asks the user to input a string. You must free it yourself.
 * 'prompt' is how you want to prompt the user.
 * It should not end with a newline. */
char*
get_string_input (const char *prompt) {
  char *input_string;

  output_message(prompt);

  echo();
  scanw("%a[a-zA-Z0-9]", &input_string);
  noecho();

  return input_string;
}

/* Asks the user to input an integer. 'prompt' is how you want to prompt the user. */
int
get_int_input (const char *prompt) {
  char *input_number;

  output_message(prompt);

  echo();

  scanw("%a[0-9]", &input_number);

  if (input_number == NULL) {
    return 0;
  }

  noecho();

  int returnee = atoi(input_number);
  free(input_number);
  return returnee;
}
