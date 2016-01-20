/* (WT)       LIST.C */
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
#include "wt.h"
#include "wtmem.h"
#include "list.h"


List *new_list(void)
{
     List *l;

     l = wtmalloc(sizeof(List));
     l->node = NULL;
     l->next = NULL;

     return l;
}


void add_node(List *l, void *node)
{
     List *t;

     t = l->next;
     l->next = new_list();
     l->next->node = node;
     l->next->next = t;
}


void delete_node(List *l)
{
     List *t;

     t = l->next->next;
     if (l->next->node != NULL)
	  free(l->next->node);
     wtfree(l->next);
     l->next = t;
}


void delete_list(List *l)
{
     while (l->next)
	  delete_node(l);

     wtfree(l);
}


List *scan_list(List *l, void *data,
		List_result (*scan_function)(void *node, void *data))
{
     List *t = l;

     while (t->next != NULL) {
	  if (scan_function(t->next->node, data) == List_success)
	       break;
	  else
	       t = t->next;
     }

     return t;
}
