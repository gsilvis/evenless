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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <notmuch.h>

#include "evenless.h"

/* Throughout this file, if not mentioned otherwise, any function that returns an int
 * returns 0 if successful, and 1 if unsuccessful
 */

void
get_config_path (Client *c) {

  if (c->config_path) {
    return;
  }


  char* home_dir = getenv("HOME");
  if (home_dir==NULL) {
    printf("Home environment variable not set, and config not explicitly given. Giving up.\n");
    printf("Either set HOME, or explicitly pass '--config /full/path/to/config'\n");
    exit(1);
  }
  char* ending = ".evenless/evenless.conf";
  
  c->config_path = malloc(snprintf(NULL, 0, "%s/%s", home_dir, ending) + 1);
  sprintf(c->config_path, "%s/%s", home_dir, ending);
  

  if (c->config_path) {
    return;
  } else {
    printf("Failed to get config path. Exiting.\n");
    exit(1);
  }
}

void
get_database_path (Client*c) {

  if (c->database_path) {
    return;
  }

  get_config_path(c);

    
  FILE* config_file = fopen(c->config_path, "r");
  if (config_file==NULL) {
    printf("Failed to open config file.\n");
    exit(1);
  }
  unsigned int line_buffer_length = 5;
  char* line_buffer = (char*) malloc(line_buffer_length+1);
  while (getline(&line_buffer, &line_buffer_length, config_file) != -1) {
    if (line_buffer[0]=='#' ||
        line_buffer[0]==' ' ||
        line_buffer[0]=='\n' ||
        line_buffer[0]=='\t') {
      continue;
    }
    line_buffer[strlen(line_buffer)-1] = '\0'; /* strip trailing newline */
    
    c->database_path = malloc(snprintf(NULL, 0, "%s", line_buffer) + 1);
    sprintf(c->database_path, "%s", line_buffer);
    
  }
  fclose(config_file);


  if (c->database_path) {
    return;
  } else {
    printf("Failed to get database path. Exiting.\n");
  }
}

void
get_database (Client *c) {

  if (c->database) {
    return;
  }

  get_database_path(c);

  c->database = notmuch_database_open(c->database_path,
                                      NOTMUCH_DATABASE_MODE_READ_WRITE);


  if (c->database) {
    return;
  } else {
    printf("Failed to get database. Exiting.\n");
  }
}

void
get_query (Client *c) {

  if (c->query) {
    return;
  }

  get_database(c);

  c->query = notmuch_query_create(c->database,
                                  c->search_string);

  if (c->query) {
    return;
  } else {
    printf("Failed to get query. Exiting.\n");
    exit(1);
  }
}

void
get_threads (Client *c) {

  if (c->threads) {
    notmuch_threads_destroy(c->threads);
    c->threads = NULL;
  }

  get_query(c);

  c->threads = notmuch_query_search_threads(c->query);

  if (c->threads) {
    return;
  } else {
    printf("Failed to get threads. Exiting.\n");
    exit(1);
  }
}

void
get_entries (Client *c) {

  /* Do it even if it already exists. */

  get_threads(c);

  if (c->num_entries == -1) {
    c->num_entries = 20;
    c->entries = malloc(20 * sizeof(void*));
  } /* c->entries is now *not* null */

  notmuch_thread_t **ptr = c->entries;
  int i = 0;
  

  /* Copy each thread into the array */
  for (;
       notmuch_threads_valid(c->threads);
       notmuch_threads_move_to_next(c->threads), i++) {
    if (i==c->num_entries) {
      c->num_entries *= 2;
      c->entries = realloc(c->entries, c->num_entries * sizeof(void*));
    } /* If we've run out of room, double length of array */
    (c->entries)[i] = notmuch_threads_get(c->threads);
  }


  /* trim array to desired length */
  c->num_entries = i+1;
  c->entries = realloc(c->entries, c->num_entries * sizeof(void*));

  return;
}

void
get_selected (Client* c) {

  /* For sanity reasons, does not call get_entries */

  if (c->evenless_mode == EVENLESS_MODE_TTYUI) {

    c->selected = get_int_input("What thread?");

    while (c->selected < 1 || c->selected > c->num_entries) {
      printf("Try again.\n");
      c->selected = get_int_input("What thread?");
    }

    return;

  } else { /* EVENLESS_MODE_CURSES */

    if (c->selected < 1) {
      c->selected = 1;
      return;
    } else if (c->selected > c->num_entries) {
      c->selected = c->num_entries;
      return;
    }
 
  }
}

void
print_ttyui (Client* c) {

  get_entries(c);
  int i;
  notmuch_thread_t **ptr;

  for (i = 1, ptr = c->entries;
       i < c->num_entries;
       i++, ptr++) {

    notmuch_thread_t* thread = *ptr;

    notmuch_tags_t* tags;
    int unread = FALSE;
    for (tags = notmuch_thread_get_tags(thread);
         notmuch_tags_valid(tags);
         notmuch_tags_move_to_next(tags)) {
      const char* tag = notmuch_tags_get(tags);
      if (!strcmp(tag, "unread")) {
        unread = TRUE;
        break;
      }
    }
    notmuch_tags_destroy(tags);

    int style;
    if (unread) {
      style = 1;
    } else {
      style = 0;
    }

    int total_messages = notmuch_thread_get_total_messages(thread);
    const char* authors = notmuch_thread_get_authors(thread);
    const char* subject = notmuch_thread_get_subject(thread);
    printf("\e[%im%i: %i messages. %s.  %s\e[m\n", style, i, total_messages, subject, authors);


  }
}



void
run_ttyui (Client* c) {

  unsigned int command_length = 1;
  char* command = (char*) malloc(command_length+1);

  while (1) {
    print_ttyui(c);
    printf("\nType your command here: ");
    if (getline(&command, &command_length, stdin)==-1) {
      printf("Command input failed. Try again.\n");
      continue;
    }
    switch (command[0]) {
      /* GLOBAL COMMANDS */
    case 'c': case 'm': /* _c_ompose new _m_ail */
      action_compose(c);
      break;
    case 'q': case 'x': /* e_x_it */
      return;
      /* THREAD-SPECIFIC COMMANDS */
    case 't': /* select and _t_ag a thread */
      action_tag_thread(c);
      break;
    case 'T': /* select and un_T_ag a thread */
      action_untag_thread(c);
      break;
    case 'a': /* select and archive a thread */
      action_archive(c);
      break;
    case 'r': /* mark a thread as read */
      action_read(c);
      break;
    case 'A': /* marh a thread as read and archive it */
      action_read_archive(c);
      break;
    case 'v': case '\n': /* select and _v_iew a thread */
      action_view(c);
      break;
    default:
      printf("Unknown command.\n");
    }
  }

  free(command);




}






/* returns a query that matches all messages in a thread. Free the query yourself. */
notmuch_query_t*
get_query_for_thread (notmuch_database_t* database,
                      notmuch_thread_t* thread) {
  
  const char* threadid = notmuch_thread_get_thread_id(thread);
  const char* prefix = "thread:";
  char* search_term = malloc(snprintf(NULL, 0, "%s%s", prefix, threadid) + 1);
  sprintf(search_term, "%s%s", prefix, threadid);

  notmuch_query_t* query = notmuch_query_create(database, search_term);

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
      printf("Tagging succeeded.\n");
      break;
    case NOTMUCH_STATUS_NULL_POINTER:
      printf("Bad tag pointer.\n");
      break;
    case NOTMUCH_STATUS_TAG_TOO_LONG:
      printf("Very long tag name.\n");
      break;
    case NOTMUCH_STATUS_READ_ONLY_DATABASE:
      printf("Database read only.\n");
      break;
    default:
      printf("What the fuck, notmuch.\n");
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
  unsigned int input_string_length = 15;
  char *input_string = (char*) malloc(input_string_length+1);

  printf("%s\n", prompt);
  if (getline(&input_string, &input_string_length, stdin) == -1) {
    printf("Error (getline failed)\n");
    return NULL;
  }

  input_string[strlen(input_string)-1]='\0'; /* strip trailing newline */

  return input_string;
}

/* Asks the user to input an integer. 'prompt' is how you want to prompt the user. */
int
get_int_input (const char *prompt) {
  unsigned int input_number_length = 3;
  char *input_number = (char*) malloc(input_number_length+1);

  printf("%s\n", prompt);
  if (getline(&input_number, &input_number_length, stdin) == -1) {
    printf("Error (getline failed)\n");
    return -1;
  }

  int returnee = atoi(input_number);
  free(input_number);
  return returnee;
}


/* Steps the user through the action of adding some tag to the thread 'thread' */
int
action_tag_thread (Client *c) {

  get_selected(c);

  char *tag = get_string_input("Please type the tag you would like to add.");

  if (tag==NULL) {
    return 1;
  }
  int returnee = tag_entire_thread(c->database, (c->entries)[c->selected-1], tag);
  
  free(tag);
  return returnee;
}

/* As above, but removes a tag */
int
action_untag_thread (Client *c) {

  get_selected(c);

  char *tag = get_string_input("Please type the tag you would like to remove.");

  if (tag==NULL) {
    return 1;
  }
  int returnee = untag_entire_thread(c->database, (c->entries)[c->selected-1], tag);
  
  free(tag);
  return returnee;
}

/* Specifically removes the 'inbox' tag from a thread */
int
action_archive (Client *c) {

  get_selected(c);

  return untag_entire_thread(c->database, (c->entries)[c->selected-1], "inbox");
}

/* Specifically removes the 'unread' tag from a thread */
int
action_read (Client *c) {

  get_selected(c);

  return untag_entire_thread(c->database, (c->entries)[c->selected-1], "unread");
}

int
action_read_archive (Client *c) {

  get_selected(c);

  untag_entire_thread(c->database, (c->entries)[c->selected-1], "unread");
  untag_entire_thread(c->database, (c->entries)[c->selected-1], "inbox");
  return 0;
}

/* Shells out to mutt, so the user can compose a new email */
int
action_compose (Client *c) {
  return system("mutt -i /dev/null");
}

/* Shells out to mutt, so the user can view the thread 'thread' */
int
action_view (Client *c) {

  get_selected(c);

  const char* threadid = notmuch_thread_get_thread_id((c->entries)[c->selected-1]);
  const char* prefix = "notmuch show --format=mbox thread:";
  const char* suffix = " > ~/.evenless/tmp.mbox";
  char* command = malloc(snprintf(NULL, 0, "%s%s%s", prefix, threadid, suffix) + 1);
  sprintf(command, "%s%s%s", prefix, threadid, suffix);

  system(command); /* notmuch show --format=mbox thread:[THREADID] > ~/.evenless/tmp.mbox */
  system("mutt -f ~/.evenless/tmp.mbox");
  system("rm ~/.evenless/tmp.mbox");

  free(command);
  return 0;
}


void
initialize_client (Client* c) {
  c->evenless_mode = EVENLESS_MODE_TTYUI;
  c->config_path = NULL;
  c->database_path = NULL;
  c->search_string = "tag:inbox";
  c->database = NULL;
  c->query = NULL;
  c->threads = NULL;
  c->num_entries = 4;
  c->entries = malloc(4 * sizeof(void*));
  c->top_displayed = 1;
  c->selected = EVENLESS_NULL_SELECTION;
}




void
parse_args (Client* c,
            int argc,
            char** argv) {

  int i;
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--version")) {
      printf("evenless %s\n", EVENLESS_VERSION);
      printf("Copyright (C) 2011 George Silvis, III\n");
      printf("This is free software; see the source for copying conditions.  There is NO\n");
      printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");

      exit(0);
    } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--search")) {
      if (i+1==argc) {
        printf("Bad arguments. %s expects a search string (in quotes if necessary).\n", argv[i]);
        exit(1);
      }
      c->search_string = argv[++i];
    } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config")) {
      if (i+1==argc) {
        printf("Bad arguments. %s expects a path to a file.\n", argv[i]);
        exit(1);
      }
      c->config_path = argv[++i];
    } else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--database")) {
      if (i+1==argc) {
        printf("Bad arguments. %s expects a path to a notmuch database.\n", argv[i]);
        exit(1);
      }
      c->database_path = argv[++i];
    } else {
      printf("Bad arguments. (unrecognized argument)\n");
      exit(1);
    }
  }
}

int
cleanup_client (Client *c) {
  free(c->entries);
  notmuch_threads_destroy(c->threads);
  notmuch_query_destroy(c->query);
  notmuch_database_close(c->database);
  c->entries = NULL;
  c->num_entries = 0;
  c->threads = NULL;
  c->query = NULL;
  c->database = NULL;

  return 0;
}


int
main (int argc,
      char **argv) {

  Client *c = malloc(sizeof(Client));
  initialize_client(c);

  parse_args(c, argc, argv);

  run_ttyui(c);

  cleanup_client(c);
  free(c);

  return 0;
}
