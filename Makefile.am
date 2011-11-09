# Copyright (C) 2011 George Silvis, III <george.iii.silvis@gmail.com> 

#
# This file is part of evenless.
#
# evenless is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# evenless is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# evenless.  If not, see <http://www.gnu.org/licenses/>.
#


AUTOMAKE_OPTIONS = subdir-objects

dist_man_MANS = evenless.1

bin_PROGRAMS = evenless

evenless_CFLAGS = -Wall $(CFLAGS) -I$(srcdir)/src

evenless_LDFLAGS = $(LDFLAGS) -lnotmuch -lncurses

evenless_SOURCES = \
	src/evenless.c \
	src/get.c \
	src/utils.c \
	src/actions.c \
	src/evenless.h \
	src/get.h \
	src/utils.h \
	src/actions.h \
	src/consts.h \
	src/types.h
