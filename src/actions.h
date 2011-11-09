#include "types.h"


#ifndef __EVENLESS_ACTIONS_H
#define __EVENLESS_ACTIONS_H

int
action_quit (Client *c);

int
action_tag_thread (Client *c);

int
action_untag_thread (Client *c);

int
action_archive (Client *c);

int
action_read (Client *c);

int
action_read_archive (Client *c);

int
action_compose (Client *c);

int
action_view (Client *c);

int
action_fetch (Client *c);

int
action_move (Client *c, int delta);

#endif
