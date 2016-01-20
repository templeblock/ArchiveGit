/* (WT)    TEXTURE.C */
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

/* GIF87a code from Thomas Hamren (d0hampe@dtek.chalmers.se) */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wt.h"
#include "wtmem.h"
#include "error.h"
#include "texture.h"


static TexFormat check_texture_file(FILE *fp, char *filename);
static int log2(int x);


Texture *new_texture(int width, int height)
{
     Texture *t;

     if (height <= 0 || width <= 0)
	  fatal_error("new_texture:  bad texture dimensions");

     t = wtmalloc(sizeof(Texture));
     t->texels = wtmalloc(height * width);

     t->width = width;
     t->height = height;
     t->log2width = log2(width);
     
     return t;
}


Texture *read_texture_file(char *filename)
{
     FILE *fp;
     Texture *t;
     int height, width;


     if (!(fp = fopen(filename, "rb")))
	  fatal_error("Cannot open texture file %s.", filename);

     switch (check_texture_file(fp, filename)) {
	case WTTEX:
	  break;

	case GIF87a:
	  return LoadGIF(fp, filename);
	  break;

	default:
          fclose(fp);
	  fprintf(stderr,"Unknown texture format\n");
	  exit(1);
	  break;
     }

     if (fscanf(fp, "%d %d\n", &width, &height) != 2)
	  fatal_error("Texture file %s has a bad header.", filename);

     /* The height and width should be powers of two for efficient
     **   texture mapping.  Here, we enforce this.
     */
     if (log2(width) == -1 || log2(height) == -1)
	  fatal_error("Dimensions texture %s are not powers of two.", 
		      filename);
     t = new_texture(width, height);

     if (fread(t->texels, 1, t->height * t->width, fp) !=
	 t->height * t->width)
	  fatal_error("Error reading texture file %s.", filename);

     fclose(fp);

     return t;
}


/* in what format are the texturefile */
static TexFormat check_texture_file( FILE *fp, char *filename )
{
     char magic[TEXTURE_MAGIC_LENGTH];

     if (fread(magic, 1, TEXTURE_MAGIC_LENGTH, fp) != TEXTURE_MAGIC_LENGTH)
	  fatal_error("Error reading texture file %s.", filename);

     if (strncmp(magic, TEXTURE_MAGIC, TEXTURE_MAGIC_LENGTH) == 0)
	  return WTTEX;

     if (strncmp(magic, GIF_MAGIC, TEXTURE_MAGIC_LENGTH) == 0)
	  return GIF87a;

     return Unknown;
}

/* Return the log base 2 of the argument, or -1 if the argument is not
**   an integer power of 2.
*/
static int log2(int x)
{
     int i;
     unsigned int n;

     if (x <= 0)
	  return -1;
     else
	  n = (unsigned int) x;

     
     for (i = 0; (n & 0x1) == 0; i++, n >>= 1);

     if (n == 1)
	  return i;
     else
	  return -1;
}
