/*
**  wt -- a 3d game engine
**
**  Copyright (C) 1994 by Chris Laurel
**  email:  claurel@mr.net
**  snail mail:  Chris Laurel, 5700 W Lake St #208,  St. Louis Park, MN  55416
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdlib.h>
#include <string.h>
#include "wtmem.h"
#include "table.h"


#define STARTING_TABLE_SIZE  16


Table *new_table(size_t entry_size)
{
     Table *t;
     
     t = wtmalloc(sizeof(Table));
     init_table(t, entry_size);

     return t;
}


void init_table(Table *t, size_t entry_size)
{
     t->entry_size = entry_size;
     t->current_entries = 0;
     t->max_entries = 0;
     t->table = NULL;
}


void add_table_entry(Table *t, void *entry)
{
     if (t->current_entries >= t->max_entries) {
	  if (t->max_entries == 0) {
	       t->max_entries = STARTING_TABLE_SIZE;
	       t->table = wtmalloc(t->max_entries * t->entry_size);
	  } else {
	       t->max_entries = (t->current_entries * 3) / 2;
	       t->table = wtrealloc(t->table,
				    t->max_entries * t->entry_size);
	  }
     }

     memcpy((char *) t->table + t->entry_size * t->current_entries,
	    entry, t->entry_size);
     t->current_entries++;
}
