
#ifndef __EVENLESS__TYPES_H
#define __EVENLESS__TYPES_H


#include <notmuch.h>

typedef struct {
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


#if 0
typedef struct {
  char key;
  void (*func) (Client* c);
} Action;
#endif

#endif /* __EVENLESS__TYPES_H */
