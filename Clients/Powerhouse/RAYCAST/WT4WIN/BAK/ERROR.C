/* (WT)      ERROR.C */
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

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#include <stdarg.h>
#endif
#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <stdarg.h>
#include "wt.h"
#include "framebuf.h"
#include "graphics.h"
#endif
#include "error.h"

#ifdef WIN32
HWND hwndWT;
WORD msgWT;
#endif

#ifndef WIN32
void fatal_error(char *message, ...)
{
     va_list args;

     end_graphics();

     va_start(args, message);
     vfprintf(stderr, message, args);
     putc('\n', stderr);
     va_end(args);

     exit(1);
}
#else

void fatal_error(char *message, ...)
{
   va_list args;
	char szMsg[100];

   va_start(args, message);
   vsprintf(szMsg, message, args);
	MessageBox(0,szMsg,"WTWIN Error!",MB_ICONSTOP);

   va_end(args);
}

void DebugMsg(char *message, ...)
{
	va_list args;
	char szMsg[100];

   va_start(args, message);
   vsprintf(szMsg, message, args);
	MessageBox(0,szMsg,"WTWIN Debug",MB_OK);

	va_end(args);
}

void DebugOut(char *message, ...)
{
	va_list args;
	char szMsg[100];

   va_start(args, message);
   vsprintf(szMsg, message, args);
	OutputDebugString(szMsg);

	va_end(args);
}

void wt_setlink(HWND hwnd, WORD msg)
{
	hwndWT = hwnd;
	msgWT = msg;
}

void UpdateWT(int wParam)
{
	SendMessage(hwndWT,msgWT,wParam,0L);
}


////////////////////////////////////////////////////////////////////////////
//I was using these for timing various parts of WT.  They work ok.  
//Call wt_enable_time before timing, bracket desired code with
//wt_start_time() and wt_end_time(), then call wt_get_time() to get 
//the elapsed time, display using DebugMsg() or DebugOut(), then call
//wt_disable_time().  I'm sure these will go away soon - just a quick hack.

static DWORD Time;
static DWORD TimeTotal;
BOOL bTime = FALSE;

void wt_enable_time(void)
{
	bTime = TRUE;
	Time = 0;
	TimeTotal = 0;
}

void wt_disable_time(void)
{
	bTime = FALSE;
	Time = 0;
	TimeTotal = 0;
}

void wt_start_time(void)
{
	if (bTime) 
		Time = timeGetTime();
}

void wt_end_time(void)
{
	if (bTime) {	
		Time = timeGetTime() - Time;
		TimeTotal += Time;
		Time = 0;
	}
}

long wt_get_time(void)
{
	return TimeTotal;
}
#endif

