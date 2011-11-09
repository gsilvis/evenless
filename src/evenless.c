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
#include <ncurses.h>

#include "consts.h"
#include "types.h"

#include "config.h"

#include "get.h"
#include "utils.h"
#include "actions.h"


#include "evenless.h"

/* Throughout this file, if not mentioned otherwise, any function that returns an int
 * returns 0 if successful, and 1 if unsuccessful
 */


void
print_thread (Client *c,
              int i) {

  if (i >= c->num_entries) {
    clrtoeol();
    printw("\n");
    return;
  }
  
  notmuch_thread_t *thread = c->entries[i];
    
  notmuch_tags_t *tags;
  int unread = FALSE;
  for (tags = notmuch_thread_get_tags(thread);
       notmuch_tags_valid(tags);
       notmuch_tags_move_to_next(tags)) {
    const char *tag = notmuch_tags_get(tags);
    if (!strcmp(tag, "unread")) {
      unread = TRUE;
      break;
    }
  }
  notmuch_tags_destroy(tags);
  
  int total_messages = notmuch_thread_get_total_messages(thread);
  const char *authors = notmuch_thread_get_authors(thread);
  const char *subject = notmuch_thread_get_subject(thread);
  /* printf("\e[%im%i: %i messages. %s.  %s\e[m\n", style, i, total_messages, subject, authors); */
  if (unread) {
    attron(A_BOLD);
  }
  if (i==c->selected) {
    attron(A_REVERSE);
  }
  printw("%i: %i messages. %s. %s \n", i, total_messages, subject, authors);
  clrtoeol();
  attroff(A_BOLD);
  attroff(A_REVERSE);
  return;

}



void
print_ttyui (Client* c) {

  /*  get_entries(c); */
  int i;

  int y = getmaxy(stdscr);

  move(0, 0);

  if (c->top_displayed > c->selected) {
    c->top_displayed -= y/3;
  }

  if (c->top_displayed + y - 2 < c->selected) {
    c->top_displayed += y/3;
  }


  /* fix top_displayed */
  if (c->top_displayed < 0) {
    c->top_displayed = 0;
  } else if (c->num_entries <= y+1) {
    c->top_displayed = 0;
  } else if (c->top_displayed + y >= c->num_entries) {
    c->top_displayed = c->num_entries - y - 1;
  }



  for (i = c->top_displayed;
       i < y+(c->top_displayed)-1 /* && i < c->num_entries */;
       i++) {
    print_thread(c, i);
  }
  refresh();

}


void
run_ttyui (Client *c) {

  char command;

  get_entries(c);

  while (1) {
    print_ttyui(c);
    command = getch();


    switch (command) {
      /* GLOBAL COMMANDS */
    case 'c': case 'm': /* _c_ompose new _m_ail */
      action_compose(c);
      break;
    case 'u': case 'f':
      action_fetch(c);
      get_entries(c);
      break;
    case 'q': case 'x': /* e_x_it */
      return;
      /* THREAD-SPECIFIC COMMANDS */
    case 't': /* select and _t_ag a thread */
      action_tag_thread(c);
      get_entries(c);
      break;
    case 'T': /* select and un_T_ag a thread */
      action_untag_thread(c);
      get_entries(c);
      break;
    case 'a': /* select and archive a thread */
      action_archive(c);
      get_entries(c);
      break;
    case 'r': /* mark a thread as read */
      action_read(c);
      break;
    case 'A': /* marh a thread as read and archive it */
      action_read_archive(c);
      get_entries(c);
      break;
    case 'v': case '\n': /* select and _v_iew a thread */
      action_view(c);
      break;
    case 'j':
      action_move(c, 1);
      break;
    case 'k':
      action_move(c, -1);
      break;
    default:
      output_message("Unknown command.");
    }


  }




}







void
initialize_client (Client *c) {
  c->config_path = NULL;
  c->database_path = NULL;
  c->search_string = "tag:inbox";
  c->database = NULL;
  c->query = NULL;
  c->threads = NULL;
  c->num_entries = 4;
  c->entries = malloc(4 * sizeof(void*));
  c->top_displayed = 1;
  c->selected = 0;
}




void
parse_args (Client *c,
            int argc,
            char **argv) {

  int i;
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--version")) {
      printf("%s\n", PACKAGE_STRING);
      printf("Copyright (C) 2011 George Silvis, III\n");
      printf("This is free software; see the source for copying conditions.  There is NO\n");
      printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");

      cleanup_client(c);
      free(c);

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
  if (c->entries) {
    free(c->entries);
  }
  if (c->threads) {
    notmuch_threads_destroy(c->threads);
  }
  if (c->query) {
    notmuch_query_destroy(c->query);
  }
  if (c->database) {
    notmuch_database_close(c->database);
  }
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

  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);

  run_ttyui(c);

  endwin();

  cleanup_client(c);
  free(c);

  return 0;
}
