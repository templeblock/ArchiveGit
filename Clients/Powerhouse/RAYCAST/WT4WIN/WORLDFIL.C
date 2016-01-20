/* (WT)   WORLDFIL.C */
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#include "wt.h"
#include "error.h"
#include "fixed.h"
#include "wtmem.h"
#include "list.h"
#include "table.h"
#include "texture.h"
#include "world.h"
#include "worldfile.h"

#ifdef WIN32
//GCT minor change to load textures using WT's path.
extern char szWTpath[];
#endif

#define TEXTURE_NAME_MAX_LENGTH 32
#define STRING_TOKEN_MAX_LENGTH 256

#define IS_STRING_TOKEN_CHAR(c) (isalpha(c) || isdigit(c) || \
				 ((c) == '/') || ((c) == '.') || ((c) == '_'))
#define COMMENT_CHAR ';'

typedef struct {
     char name[TEXTURE_NAME_MAX_LENGTH + 1];
     int index;
} Texture_node;

typedef enum {
     Token_string,
     Token_real,
     Token_integer,
     Token_EOF
} Token_type;


static void parse_world(FILE *fp, World *w);
static void parse_vertex(FILE *fp, World *w);
static void parse_wall(FILE *fp, World *w);
static void parse_region(FILE *fp, World *w);
static void parse_texture(FILE *fp, World *w);
static int get_texture_index(char *name);
static List_result find_list_name(void *node, void *data);
static Token_type get_string_token(FILE *fp, char *tokenbuf);
static Token_type get_real_token(FILE *fp, fixed *f);
static Token_type get_integer_token(FILE *fp, int *i);
static int skip_whitespace(FILE *fp);
static void parse_error(char *message);

static List *texture_list;
static int line_number;


World *read_world_file(FILE *fp)
{
     World *w;

     w = new_world();
     texture_list = new_list();
     parse_world(fp, w);
     delete_list(texture_list);

     return w;
}


void parse_world(FILE *fp, World *w)
{
     char tokenbuf[STRING_TOKEN_MAX_LENGTH];

     while (get_string_token(fp, tokenbuf) != Token_EOF) {
	  if (strcmp(tokenbuf, "texture") == 0)
	       parse_texture(fp, w);
	  else if (strcmp(tokenbuf, "wall") == 0)
	       parse_wall(fp, w);
	  else if (strcmp(tokenbuf, "region") == 0)
	       parse_region(fp, w);
	  else if (strcmp(tokenbuf, "vertex") == 0)
	       parse_vertex(fp, w);
	  else
	       parse_error("unknown structure type");
     }
}


void parse_vertex(FILE *fp, World *w)
{
     Vertex v;


     if (get_real_token(fp, &v.x) != Token_real)
	  parse_error("number expected");
     if (get_real_token(fp, &v.y) != Token_real)
	  parse_error("number expected");

     add_vertex(w, &v);
}


void parse_region(FILE *fp, World *w)
{
     char texture_name[STRING_TOKEN_MAX_LENGTH];
     int texture_index;
     Region r;

     if (get_real_token(fp, &r.floor) != Token_real)
	  parse_error("number expected");
     if (get_real_token(fp, &r.ceiling) != Token_real)
	  parse_error("number expected");

     /* floor texture */
     if (get_string_token(fp, texture_name) != Token_string)
	  parse_error("texture name expected");
     texture_index = get_texture_index(texture_name);
     if (texture_index < 0 || texture_index > TABLE_SIZE(w->textures))
	  parse_error("non-existent texture");
     else
	  r.floor_tex = WORLD_TEXTURE(w, texture_index);

     /* ceiling texture */
     if (get_string_token(fp, texture_name) != Token_string)
	  parse_error("texture name expected");
     if (strcmp(texture_name, "sky") == 0)
	  r.ceiling_tex = NULL;
     else {
	  texture_index = get_texture_index(texture_name);
	  if (texture_index < 0 || texture_index > TABLE_SIZE(w->textures))
	       parse_error("non-existent texture");
	  else
	       r.ceiling_tex = WORLD_TEXTURE(w, texture_index);
     }

     add_region(w, &r);
}


void parse_wall(FILE *fp, World *w)
{
     Wall wall;
     char texture_name[STRING_TOKEN_MAX_LENGTH];
     int texture_index;
     int front_region, back_region;
     int vertex1, vertex2;
     fixed wall_length;


     /* vertices */
     if (get_integer_token(fp, &vertex1) != Token_integer)
	  parse_error("integer expected");
     if (get_integer_token(fp, &vertex2) != Token_integer)
	  parse_error("integer expected");
     if (vertex1 < 0 || vertex1 > TABLE_SIZE(w->vertices))
	  parse_error("invalid vertex number");
     if (vertex2 < 0 || vertex2 > TABLE_SIZE(w->vertices))
	  parse_error("invalid vertex number");
     wall.vertex1 = &WORLD_VERTEX(w, vertex1);
     wall.vertex2 = &WORLD_VERTEX(w, vertex2);

     /* texture */
     if (get_string_token(fp, texture_name) != Token_string)
	  parse_error("texture name expected");
     texture_index = get_texture_index(texture_name);
     if (texture_index < 0 || texture_index > TABLE_SIZE(w->textures))
	  parse_error("non-existent texture");
     else
	  wall.surface_texture = WORLD_TEXTURE(w, texture_index);

     if (strcmp(texture_name, "sky") == 0)
	  wall.sky = True;
     else
	  wall.sky = False;

     /* front and back regions */
     if (get_integer_token(fp, &front_region) != Token_integer)
	  fatal_error("non-existent region");
     if (get_integer_token(fp, &back_region) != Token_integer)
	  fatal_error("non-existent region");
     if (front_region < 0 || front_region > TABLE_SIZE(w->regions))
	  fatal_error("non-existent region");
     if (back_region < 0 || back_region > TABLE_SIZE(w->regions))
	  fatal_error("non-existent region");
     wall.front = &WORLD_REGION(w, front_region);
     wall.back = &WORLD_REGION(w, back_region);

     /* Texture phase and scale.  This code is somewhat more complicated than
     **   you'd expect, since the texture scale must be normalized to the
     **   wall length.
     */
     if (get_real_token(fp, &wall.xscale) != Token_real)
	  parse_error("number expected");
     if (get_real_token(fp, &wall.yscale) != Token_real)
	  parse_error("number expected");
     if (get_real_token(fp, &wall.xphase) != Token_real)
	  parse_error("number expected");
     if (get_real_token(fp, &wall.yphase) != Token_real)
	  parse_error("number expected");
     wall_length =
	  FLOAT_TO_FIXED(sqrt(FIXED_TO_FLOAT(wall.vertex2->x -
					     wall.vertex1->x) *
			      FIXED_TO_FLOAT(wall.vertex2->x -
					     wall.vertex1->x) +
			      FIXED_TO_FLOAT(wall.vertex2->y -
					     wall.vertex1->y) *
			      FIXED_TO_FLOAT(wall.vertex2->y -
					     wall.vertex1->y)));
     wall.yscale = fixmul(wall.yscale,
			  INT_TO_FIXED(wall.surface_texture->height));
     wall.xscale = fixmul(fixmul(wall.xscale,
				 INT_TO_FIXED(wall.surface_texture->width)),
			  wall_length);

     add_wall(w, &wall);
}


void parse_texture(FILE *fp, World *w)
{
     char texture_name[STRING_TOKEN_MAX_LENGTH];
     char filename[STRING_TOKEN_MAX_LENGTH];
     char texture_path[PATH_MAX + STRING_TOKEN_MAX_LENGTH];
     Texture *t;
     Texture_node *tn;


     if (get_string_token(fp, texture_name) != Token_string)
	  parse_error("texture name expected");
     if (strlen(texture_name) >= TEXTURE_NAME_MAX_LENGTH)
	  parse_error("texture name too long");

     if (get_string_token(fp, filename) != Token_string)
	  parse_error("texture file name expected");
#ifdef WIN32
     sprintf(texture_path, "%s%s/%s", szWTpath, TEXTURE_PATH, filename);
#else
     sprintf(texture_path, "%s/%s", TEXTURE_PATH, filename);
#endif
     t = read_texture_file(texture_path);

     tn = wtmalloc(sizeof(Texture_node));
     strcpy(tn->name, texture_name);
     tn->index = TABLE_SIZE(w->textures);

     add_node(texture_list, tn);
     add_texture(w, t);
}
		 

int get_texture_index(char *name)
{
     List *l;

     l = scan_list(texture_list, name, find_list_name);
     if (l == NULL)
	  return -1;
     else
	  return ((Texture_node *) l->next->node)->index;
}


List_result find_list_name(void *node, void *data)
{
     char *name = (char *) data;
     Texture_node *tnode = (Texture_node *) node;
     
     if (strcmp(name, tnode->name) == 0)
	  return List_success;
     else
	  return List_failure;
}

     
Token_type get_string_token(FILE *fp, char *tokenbuf)
{
     int length = 0;
     int c;

     c = skip_whitespace(fp);
     if (c == EOF)
	  return Token_EOF;

     while (c != EOF && IS_STRING_TOKEN_CHAR(c)) {
	  if (length >= STRING_TOKEN_MAX_LENGTH - 1)
	       parse_error("string too long");

	  tokenbuf[length] = c;
	  length++;

	  c = getc(fp);
     }

     ungetc(c, fp);
     tokenbuf[length] = '\0';

     return Token_string;
}


Token_type get_real_token(FILE *fp, fixed *f)
{
     double d;
     int c;

     c = skip_whitespace(fp);
     if (c == EOF)
	  return Token_EOF;

     ungetc(c, fp);
     if (fscanf(fp, "%lf", &d) != 1)
	  parse_error("numeric constant expected");

     *f = FLOAT_TO_FIXED(d);

     return Token_real;
}


Token_type get_integer_token(FILE *fp, int *i)
{
     int c;

     c = skip_whitespace(fp);
     if (c == EOF)
	  return Token_EOF;

     ungetc(c, fp);
     if (fscanf(fp, "%d", i) != 1)
	  parse_error("integer constant expected");

     return Token_integer;
}

	  
int skip_whitespace(FILE *fp)
{
     int c = getc(fp);

     while (c == ' ' || c == '\t' || c == '\n' || c == COMMENT_CHAR) {
	  if (c == COMMENT_CHAR) {
	       while (c != EOF && c != '\n')
		    c = getc(fp);
	  }

	  if (c == '\n')
	       line_number++;
	  c = getc(fp);
     }

     return c;
}


void parse_error(char *message)
{
     fatal_error("Error in world file (line %d):  %s\n", line_number,
		 message);
}
