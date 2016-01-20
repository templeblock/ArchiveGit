/* (WT)      WT.H */
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

/* directory and file configuration */
#define TEXTURE_PATH "textures"
#ifdef WIN32
#define DEFAULT_WORLD_FILENAME "worlds\\newcastl.wor"
#define DEFAULT_PALETTE_FILENAME "wt.pal"
#else
#define DEFAULT_WORLD_FILE "d:\\misc\\gamedev\\wtwin\\worlds\\newcastl.wor"
#define DEFAULT_PALETTE_FILE "d:\\misc\\gamedev\\wtwin\\wt.pal"
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef False
typedef enum { False, True } Boolean;
#else
#ifndef Boolean
#define Boolean int
#endif
#endif

#ifndef __GNUC__
#define inline
#endif

/* For compilers without NULL defined in stdlib.h */
#ifndef NULL
#include <stdio.h>
#endif

/* More compatibility stuff . . . */
#ifndef PATH_MAX
#ifdef _POSIX_PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX
#else
#define PATH_MAX 1024
#endif
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCES 0
#endif
