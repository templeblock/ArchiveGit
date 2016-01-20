/****************************************************************

	File:		SCSETJMP.H

	$Header: /Projects/Toolbox/ct/SCSETJMP.H 2     5/30/97 8:45a Wmanis $

	Contains:	take care of setjmp/longjmp

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************/

#ifndef _H_SCSETJMP
#define _H_SCSETJMP

#if defined( SCMACINTOSH )

	#include <setjmp.h>

	#define scJMPBUF					jmp_buf	
	#define scTHROW( lpbuf, numback)	longjmp( lpbuf, numback )
	#define scCATCH( lpbuf )			setjmp( lpbuf )

#elif defined( SCWINDOWS )

	#include <windows.h>
	#include <setjmp.h> 

	#ifdef _JMP_BUF_DEFINED
		#define scJMPBUF					jmp_buf	
		#define scTHROW( lpbuf, numback)	longjmp( lpbuf, numback )
		#define scCATCH( lpbuf )			setjmp( lpbuf )
	#else
		#define scJMPBUF					CATCHBUF	
		#define scTHROW( lpbuf, numback)	Throw( lpbuf, numback )
		#define scCATCH( lpbuf )			Catch( lpbuf )
	#endif

#endif


#endif /* _H_SCSETJMP */
