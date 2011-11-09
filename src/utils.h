
#ifndef __EVENLESS__UTILS_H
#define __EVENLESS__UTILS_H

#include <notmuch.h>


int
output_message (const char *string);

notmuch_query_t*
get_query_for_thread (notmuch_database_t* database,
                      notmuch_thread_t* thread);


int
tag_entire_thread (notmuch_database_t* database,
                   notmuch_thread_t* thread,
                   const char* tag);

int
untag_entire_thread (notmuch_database_t *database,
                     notmuch_thread_t *thread,
                     const char *tag);

char*
get_string_input (const char *prompt);

int
get_int_input (const char *prompt);


#endif /* __EVENLESS__UTILS_H */
