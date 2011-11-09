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

#ifndef __EVENLESS_H
#define __EVENLESS_H

#include <notmuch.h>





/* Throughout this file, if not mentioned otherwise, any function that returns an int
 * returns 0 if successful, and 1 if unsuccessful */


void
print_ttyui (Client *c);

void
run_ttyui (Client *c);





void
initialize_client (Client *c);

void
parse_args (Client *c,
            int argc,
            char **argv);

int
cleanup_client (Client *c);





/* Prints out the results of a query, returns the number of threads. */
int
printquery (notmuch_database_t* database,
            notmuch_query_t* query);

/* Runs a query and lets the user interact with it */
int
runquery (notmuch_database_t* database,
          char* string);

#endif /* __EVENLESS_H */
