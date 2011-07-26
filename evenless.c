/* Copyright (C) 2010 George Silvis, III */

/**************************************************************************
 * This program is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/


#include <stdio.h>
#include <stdlib.h>

#include <notmuch.h>


/* Throughout this file, if not mentioned otherwise, any function that returns an int
 * returns 0 if successful, and 1 if unsuccessful */

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
    notmuch_message_add_tag(message, tag);
    notmuch_message_destroy(message);
  }
  notmuch_messages_destroy(messages);
  notmuch_query_destroy(query);
  return 0;
}


/* remove the tag 'tag' from all messages in thread 'thread' */
int
untag_entire_thread (notmuch_database_t* database,
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
get_string_input (const char* prompt) {
  int input_string_length = 15;
  char* input_string = (char*) malloc(input_string_length+1);

  printf("%s\n", prompt);
  if (getline(&input_string, &input_string_length, stdin) == -1) {
    printf("Error (getline failed)\n");
    return NULL;
  }

  return input_string;
}

/* Asks the user to input an integer. 'prompt' is how you want to prompt the user. */
int
get_int_input (const char* prompt) {
  int input_number_length = 3;
  char* input_number = (char*) malloc(input_number_length+1);

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
action_tag_thread (notmuch_database_t* database,
                   notmuch_thread_t* thread) {
  char* tag = get_string_input("Please type the tag you would like to add.");

  if (tag==NULL) {
    return 1;
  }
  int returnee = tag_entire_thread(database, thread, tag);
  
  free(tag);
  return returnee;
}

/* As above, but removes a tag */
int
action_untag_thread (notmuch_database_t* database,
                     notmuch_thread_t* thread) {
  char* tag = get_string_input("Please type the tag you would like to remove.");

  if (tag==NULL) {
    return 1;
  }
  int returnee = untag_entire_thread(database, thread, tag);
  
  free(tag);
  return returnee;
}

/* Specifically removes the 'inbox' tag from a thread */
int
action_archive (notmuch_database_t* database,
                notmuch_thread_t* thread) {
  return untag_entire_thread(database, thread, "inbox");
}

/* Specifically removes the 'unread' tag from a thread */
int
action_read (notmuch_database_t* database,
             notmuch_thread_t* thread) {
  return untag_entire_thread(database, thread, "unread");
}

/* Prompts the user to select a thread. */
/* Used by all functions below with 'select' in their name */
notmuch_thread_t*
action_select_thread (notmuch_database_t* database,
                      notmuch_threads_t* threads) {
  int number = get_int_input("Please select a thread.");
  
  int i;
  for (i = 1;
       i < number && notmuch_threads_valid(threads);
       i++) {
    notmuch_threads_move_to_next(threads);
  }
  if (!notmuch_threads_valid(threads)) {
    printf("Invalid number");
    return NULL;
  }
  return notmuch_threads_get(threads);

}

/* Prompts user to select a thread, and then to enter a tag to add to the thread */
int
action_select_tag (notmuch_database_t* database,
                   notmuch_query_t* query) {
  notmuch_threads_t* threads = notmuch_query_search_threads(query);
  notmuch_thread_t* thread = action_select_thread(database, threads);
  if (thread==NULL) {
    return -1;
  }

  int returnee = action_tag_thread(database, thread);

  notmuch_thread_destroy(thread);
  notmuch_threads_destroy(threads);
  return returnee;
}

/* Select a thread, then enter a tag to remove from it. */
int
action_select_untag (notmuch_database_t* database,
                     notmuch_query_t* query) {
  notmuch_threads_t* threads = notmuch_query_search_threads(query);
  notmuch_thread_t* thread = action_select_thread(database, threads);
  if (thread==NULL) {
    return -1;
  }

  int returnee = action_untag_thread(database, thread);

  notmuch_thread_destroy(thread);
  notmuch_threads_destroy(threads);
  return returnee;
}

/* Shells out to mutt, so the user can compose a new email */
int
action_compose () {
  return system("mutt -i /dev/null");
}

/* Shells out to mutt, so the user can view the thread 'thread' */
int
action_view (notmuch_thread_t* thread) {
  const char* threadid = notmuch_thread_get_thread_id(thread);
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

/* Prompts user to select a thread, then shows it in mutt */
int
action_select_view (notmuch_database_t* database,
                    notmuch_query_t* query) {
  notmuch_threads_t* threads = notmuch_query_search_threads(query);
  notmuch_thread_t* thread = action_select_thread(database, threads);

  action_read(database, thread);
  action_view(thread);

  notmuch_thread_destroy(thread);
  notmuch_threads_destroy(threads);
}

/* Prompts user to select a thread, then removes the 'inbox' tag from it */
int
action_select_archive (notmuch_database_t* database,
                       notmuch_query_t* query) {
  notmuch_threads_t* threads = notmuch_query_search_threads(query);
  notmuch_thread_t* thread = action_select_thread(database, threads);

  action_archive(database, thread);

  notmuch_thread_destroy(thread);
  notmuch_threads_destroy(threads);
}

/* Prompts user to select a thread, then removes the 'unread' tag from it */
int action_select_read (notmuch_database_t* database,
                        notmuch_query_t* query) {
  notmuch_threads_t* threads = notmuch_query_search_threads(query);
  notmuch_thread_t* thread = action_select_thread(database, threads);

  action_read(database, thread);

  notmuch_thread_destroy(thread);
  notmuch_threads_destroy(threads);
}

/* Prints out the results of a query, returns the number of threads. */
int
printquery (notmuch_database_t* database,
            notmuch_query_t* query) {
  notmuch_threads_t* threads;
  int i = 0;
  for (threads = notmuch_query_search_threads(query);
       notmuch_threads_valid(threads);
       notmuch_threads_move_to_next(threads), i++) {
    notmuch_thread_t* thread = notmuch_threads_get(threads);
    int total_messages = notmuch_thread_get_total_messages(thread);
    const char* authors = notmuch_thread_get_authors(thread);
    const char* subject = notmuch_thread_get_subject(thread);
    printf("%i: %i messages. %s.  %s\n", i+1, total_messages, subject, authors);
    notmuch_thread_destroy(thread);
  }
  notmuch_threads_destroy(threads);
  return i;
}

/* Runs a query and lets the user interact with it */
int
runquery (notmuch_database_t* database,
          char* string) {
  notmuch_query_t* query = notmuch_query_create(database, string);

  int command_length = 1;
  char* command = (char*) malloc(command_length+1);

  while (1) {
    printquery(database, query);
    printf("\nType your command here: ");
    if (getline(&command, &command_length, stdin)==-1) {
      printf("Command input failed. Try again.\n");
      continue;
    }
    switch (command[0]) {
      /* GLOBAL COMMANDS */
    case 'c': case 'm': /* _c_ompose new _m_ail */
      action_compose();
      break;
    case 'q': case 'x': /* e_x_it this level */
      notmuch_query_destroy(query);
      return 0;
      /* THREAD-SPECIFIC COMMANDS */
    case 't': /* select and _t_ag a thread */
      action_select_tag(database, query);
      break;
    case 'T': /* select and un_T_ag a thread */
      action_select_untag(database, query);
      break;
    case 'a': /* select and archive a thread */
      action_select_archive(database, query);
      break;
    case 'r': /* mark a thread as read */
      action_select_read(database, query);
      break;
    case 'v': case '\n': /* select and _v_iew a thread */
      action_select_view(database, query);
      break;
    default:
      printf("Unknown command.\n");
    }
  }

  free(command);
  notmuch_query_destroy(query);
  return 0;
}

/* Main */
int
main (int argc,
      char** argv) {

  char* search_string = NULL;
  char* notmuch_database_path = NULL;
  char* config_path = NULL;

  /* parse command-line arguments */
  int i;
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--version")) {
      printf("evenless 0.0.1\n");
      printf("Copyright (C) 2011 George Silvis, III\n");
      exit(0);
    } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--search")) {
      if (i+1==argc) {
        printf("Bad arguments. %s expects a search string (in quotes if necessary).\n", argv[i]);
        exit(1);
      }
      search_string = argv[++i];
    } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config")) {
      if (i+1==argc) {
        printf("Bad arguments. %s expects a path to a file.\n", argv[i]);
        exit(1);
      }
      config_path = argv[++i];
    } else {
      printf("Bad arguments. (unrecognized argument)\n");
      exit(1);
    }
  }


  /* parse configuration file */
  int need_free_config_path = FALSE;

  if (config_path==NULL) {
    char* home_dir = getenv("HOME");
    if (home_dir==NULL) {
      printf("Home environment variable not set. Giving up.\n");
      printf("Either set HOME, or explicitly pass '--config /full/path/to/config'\n");
      exit(1);
    }
    char* ending = ".evenless/evenless.conf";

    config_path = malloc(snprintf(NULL, 0, "%s/%s", home_dir, ending) + 2);
    sprintf(config_path, "%s/%s", home_dir, ending);

    need_free_config_path = TRUE;
  }

  FILE* config_file = fopen(config_path, "r");
  if (config_file==NULL) {
    printf("Failed to open config file.\n");
    exit(1);
  }
  int line_buffer_length = 5;
  char* line_buffer = (char*) malloc(line_buffer_length+1);
  while (getline(&line_buffer, &line_buffer_length, config_file) != -1) {
    if (line_buffer[0]=='#' || line_buffer[0]==' ' || line_buffer[0]=='\n' || line_buffer[0]=='\t') {
      continue;
    }
    line_buffer[strlen(line_buffer)-1] = '\0'; /* strip trailing newline */

    notmuch_database_path = malloc(snprintf(NULL, 0, "%s", line_buffer) + 1);

    sprintf(notmuch_database_path, "%s", line_buffer);

  }
  fclose(config_file);
  if (need_free_config_path) {
    free(config_path);
  }

  /* open database */
  notmuch_database_t* database;
  database = notmuch_database_open(notmuch_database_path,
                                   NOTMUCH_DATABASE_MODE_READ_WRITE);
  free(notmuch_database_path);

  /* do work */
  if (search_string==NULL) {
    search_string = "tag:inbox";
  }
  runquery(database, search_string);

  /* clean up */
  notmuch_database_close(database);
  return 0;
}
