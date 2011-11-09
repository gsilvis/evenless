
#ifndef __EVENLESS__GET_H
#define __EVENLESS__GET_H


#include "types.h"


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


#endif /* __EVENLESS__GET_H */
