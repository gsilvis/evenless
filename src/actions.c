
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <notmuch.h>
#include <ncurses.h>

#include "types.h"
#include "consts.h"

#include "utils.h"
#include "get.h"

#include "actions.h"




int
action_quit (Client *c) {
  exit(0);
  return 0;
}



/* Steps the user through the action of adding some tag to the thread 'thread' */
int
action_tag_thread (Client *c) {

  get_selected(c);

  char *tag = get_string_input("Please type the tag you would like to add.");

  if (tag==NULL) {
    return 1;
  }
  int returnee = tag_entire_thread(c->database, (c->entries)[c->selected], tag);
  
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
  int returnee = untag_entire_thread(c->database, (c->entries)[c->selected], tag);
  
  free(tag);
  return returnee;
}

/* Specifically removes the 'inbox' tag from a thread */
int
action_archive (Client *c) {

  get_selected(c);

  return untag_entire_thread(c->database, (c->entries)[c->selected], "inbox");
}

/* Specifically removes the 'unread' tag from a thread */
int
action_read (Client *c) {

  get_selected(c);

  return untag_entire_thread(c->database, (c->entries)[c->selected], "unread");
}

int
action_read_archive (Client *c) {

  get_selected(c);

  untag_entire_thread(c->database, (c->entries)[c->selected], "unread");
  untag_entire_thread(c->database, (c->entries)[c->selected], "inbox");
  return 0;
}

/* Shells out to mutt, so the user can compose a new email */
int
action_compose (Client *c) {

  def_prog_mode();
  endwin();
  int returnee = system("mutt -i /dev/null");
  reset_prog_mode();
  refresh();
  return returnee;

}

/* Shells out to mutt, so the user can view the thread 'thread' */
int
action_view (Client *c) {

  get_selected(c);


  const char* threadid = notmuch_thread_get_thread_id((c->entries)[c->selected]);
  const char* prefix = "notmuch show --format=mbox thread:";
  const char* suffix = " > ~/.evenless/tmp.mbox";
  char* command = malloc(snprintf(NULL, 0, "%s%s%s", prefix, threadid, suffix) + 1);
  sprintf(command, "%s%s%s", prefix, threadid, suffix);





  def_prog_mode();
  endwin();

  system(command); /* notmuch show --format=mbox thread:[THREADID] > ~/.evenless/tmp.mbox */

  free(command);

  system("mutt -f ~/.evenless/tmp.mbox");
  system("rm ~/.evenless/tmp.mbox");

  reset_prog_mode();
  refresh();



  return untag_entire_thread(c->database, (c->entries)[c->selected], "unread");
}

int
action_fetch (Client* c) {
  notmuch_threads_destroy(c->threads);
  c->threads = NULL;
  notmuch_query_destroy(c->query);
  c->query = NULL;
  notmuch_database_close(c->database);
  c->database = NULL;
  c->selected = EVENLESS_NULL_SELECTION;

  

  def_prog_mode();
  endwin();

  system("offlineimap -o");
  system("notmuch new");

  reset_prog_mode();
  refresh();

  return 1;
}


int
action_move (Client *c, int delta) {
  c->selected += delta;
  
  if (c->selected < 0) {
    c->selected = 0;
  }
  if (c->selected >= c->num_entries) {
    c->selected = c->num_entries-1;
  }

  return 1;
}
