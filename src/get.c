

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "consts.h"
#include "types.h"

#include "get.h"

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
  c->num_entries = i; /* it's been incremented beyond the last thread */
  c->entries = realloc(c->entries, c->num_entries * sizeof(void*));

  return;
}

void
get_selected (Client* c) {

  /* For sanity reasons, does not call get_entries */
  if (c->selected < 1) {
    c->selected = 0;
    return;
  } else if (c->selected >= c->num_entries) {
    c->selected = c->num_entries - 1;
    return;
  }
  
}
