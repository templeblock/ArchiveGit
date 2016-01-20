/* (WT)      TABLE.H */
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

typedef struct {
     size_t entry_size;
     int current_entries;
     int max_entries;
     void *table;
} Table;


#define TABLE_ELEMENTS(t, type)   ((type *) (t)->table)
#define TABLE_SIZE(t)             ((t)->current_entries)

extern Table *new_table(size_t entry_size);
extern void init_table(Table *t, size_t entry_size);
extern void add_table_entry(Table *t, void *entry);
extern int table_size(Table *t);
#ifdef WIN32
//GCT minor addition to support the reloading of Worlds and their components.
extern int free_table(Table *t);
#endif
