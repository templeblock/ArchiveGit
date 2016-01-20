/*
$Header: /PM8/TT/FNTJMP.H 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/FNTJMP.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:14:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:14   Fred
// Initial revision.
// 
//    Rev 1.2   01 Jul 1997 09:08:22   Fred
// More changes for PMG 4.0
// 
//    Rev 1.1   18 Apr 1997 12:49:56   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:16   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:14:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:40   FRED
// Initial revision.
// 
//    Rev 1.0   29 Sep 1995 13:37:04   FRED
// Initial revision.
 * 
 *    Rev 1.2   01 Sep 1992 09:24:16   FRED
 * Added PVCS variables.
 * 
*/
/*
    File:       fntjmp.h

    Written by: Lenox Brassell

    Contains:   definitions for jmp_buf[], setjmp(), longjmp(), and the
                aliases fs_setjmp() and fs_longjmp().

    Copyright:  c 1989-1990 by Microsoft Corp., all rights reserved.

    Change History (most recent first):
        <1>      6/18/91    LB      Created file.
*/

#ifndef __FNTJMP_H__
#define __FNTJMP_H__

#ifdef WIN32
	  #include "setjmp32.h"
#else
	  #include "setjmp16.h"
#endif
	  #define fs_setjmp(a)    setjmp(a)
	  #define fs_longjmp(a,b) longjmp(a,b)

#if 0
#ifndef fs_setjmp

	#ifndef PC_OS
     
     #undef __cplusplus
	  #include <setjmp.h>
     #define __cplusplus
	  #define fs_setjmp(a)    setjmp(a)
	  #define fs_longjmp(a,b) longjmp(a,b)

	#else

		/***
		 *setjmp.h - definitions/declarations for setjmp/longjmp routines
		 *
		 *       Copyright (c) 1985-1990, Microsoft Corporation.  All rights reserved.
		 *
		 *Purpose:
		 *       This file defines the machine-dependent buffer used by
		 *       setjmp/longjmp to save and restore the program state, and
		 *       declarations for those routines.
		 *       [ANSI/System V]
		 *
		 ****/

		 #if defined(_DLL) && !defined(_MT)
		 #error Cannot define _DLL without _MT
		 #endif

		 #ifdef _MT
		 #define _FAR_ _far
		 #else
		 #define _FAR_
		 #endif

		 /* define the buffer type for holding the state information */

		 #define _JBLEN  9  /* bp, di, si, sp, ret addr, ds */

		 #ifndef _JMP_BUF_DEFINED
		 typedef  int  jmp_buf[_JBLEN];
		 #define _JMP_BUF_DEFINED
		 #endif


		 /* function prototypes */

		 int  fs_setjmp(jmp_buf);
		 void fs_longjmp(jmp_buf, int);

	#endif /* PC_OS */
#endif
#endif

#endif
