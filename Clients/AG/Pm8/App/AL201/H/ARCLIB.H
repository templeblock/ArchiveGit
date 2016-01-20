/*
 * ARCLIB.H
 *
 *  Master header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 * This is the master header file for Archive Library.  It will include
 * all header files that define base classes, but no derived classes.  For
 * people who don't want the massive flood of include files provided by
 * AL.H, this is the answer.  However, you will have to include some
 * other headers, leading to some extra work.
 *
 * This file must be included by any module that is either a part of
 * Archive Library, or part of a project that uses archive library.
 *
 * Defines that you should be creating in your program when using this
 * product include:
 *
 *  #define AL_USING_DLL       Every module calling the AL DLL *must*
 *                             have this defined.  (Unless you are using
 *                             VC++ 4.x and MFC).
 *
 *  #define NDEBUG             Turns off assertions.  Note that assertions
 *                             defined in the debug version of the library
 *                             won't go away just because you define this
 *
 *  #define _DEBUG             Turn this on to turn on some of the debugging
 *                             features of the library.  Doesn't do much
 *                             at this time.
 *
 *  We set up the rest of the definitions in ALDEFS.H, based on compiler
 *  settings that can be detected using your compiler's predefined
 *  macros.
 * 
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  July 8, 1994  1.0B  : I stuck a function prototype needed by SUN UNIX
 *                        in here.  The function, strcasecmp() is a replacement
 *                        for the ANSI stricmp(), but it isn't prototyped in
 *                        strings.h
 *
 *  January 1, 1995 1.01A : Changed include of storage.h to stor.h.  Had
 *                          clash with the new header file
 *                          STORAGE.H used by compiler vendors for
 *                          OLE implementation.
 *
 *  February 14, 1995  2.0A : New release.
 *
 *  March 20, 1996     2.01A : Minor changes to accomodate test drive.
 */

#ifndef _ARCLIB_H
#define _ARCLIB_H

/*
 * Want to customize the workings of our library?  Just define AL_CUSTOM,
 * and then create your own personal version of ALCUSTOM.H. (No, we don't
 * ship a copy of this header file with the library, it is for you to
 * define.  This is a really good way to use products like MEMCHECK or
 * SmartHeap that want to insert an included file in every one of
 * our source files.
 */

/*
 * I used to define STRICT right before I included windows.h.  But...
 * I found out Microsoft has a stealth include of windows.h from
 * iostream.h.  So, this is a preemptive strike.
 */

#ifndef STRICT
#define STRICT
#endif

#if defined( AL_CUSTOM )
#include "alcustom.h"
#endif

#if defined( __cplusplus )
#include "iostream.h"
#endif

/* All these includes needed for various library features */

#if defined( AL_MFC )
#include <afxwin.h>         /* MFC core and standard components */
#include <afxext.h>         /* MFC extensions                   */
#endif

#include "string.h"
#include "aldefs.h"
#include "_debug.h"

/* Base classes */

#include "status.h"
#include "objname.h"
#include "stor.h"
#include "engn.h"
#include "monitor.h"
#include "toolkit.h"
#include "arcentry.h"
#include "arclist.h"
/*
 * If I include this header, VC++ insists on including
 * some objects that aren't needed in the test drive.
 */
#if !defined( AL_DEMO )
#include "cmpobj.h"
#endif
#include "arc.h"

#endif /* ARCLIB_H */
