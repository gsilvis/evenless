# Copyright (C) 2011 Liam Wang <lzwbraid@gmail.com>

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



#required for all CMakeList files.
cmake_minimum_required(VERSION 2.6)

#project name
project(evenless)

set(CMAKE_C_FLAGS -g)

set(CMAKE_MAJOR_VERSION 0)
set(CMAKE_MINOR_VERSION 0)
set(CMAKE_PATCH_VERSION 4-rc1)


add_executable(evenless evenless.c)

target_link_libraries(evenless notmuch)

# gcc -o evenless evenless.c -lnotmuch
