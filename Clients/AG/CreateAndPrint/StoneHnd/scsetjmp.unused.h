/****************************************************************
Contains:	take care of setjmp/longjmp
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

#endif _H_SCSETJMP
