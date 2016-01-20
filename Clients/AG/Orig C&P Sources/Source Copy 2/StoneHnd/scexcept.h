/*=================================================================

	File:		EXCEPT.H

	$Header: /Projects/Toolbox/ct/SCEXCEPT.H 2	   5/30/97 8:45a Wmanis $

	Contains:	exception code

	Written by: Sealy

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


=================================================================*/

#ifndef _H_EXCEPT
#define _H_EXCEPT

#include "sctypes.h"

#if SCDEBUG > 1
	#undef new
#endif

/* ======================================================================== */

class scException {
public:

#if SCDEBUG > 1
				scException( status 	errCode = scSuccess,
							 const char* file = 0,
							 int		line = 0 ) :
								fFile( file ),
								fLine( line ),
								fCode( errCode ){ SCDebugBreak(); }

#else
				scException( status 	errCode = scSuccess ) :
								fCode( errCode ){}
#endif

	status		GetValue(void) const			{ return fCode; }

#if SCDEBUG > 1
	const char* 	fFile;
	const int		fLine;
#endif
		
private:
	const status	fCode;
};


#if 0

	#if SCDEBUG > 1
		#define raise(err)						throw( scException( err, __FILE__, __LINE__ ) )
		#define raise_if(exp, err)				((exp) ? (throw( scException( err, __FILE__, __LINE__ )),0) : 0)
	#else
		#ifndef MSVCBUG_1A
			#define raise( scerr )						throw( scException( scerr ) )
			#define raise_if(exp, scerr )				((exp) ? (throw( scException( scerr )),0) : 0)
		#else
			#define raise(err)						throw( scException( (status)err ) )
			#define raise_if(exp, err)				((exp) ? (throw( scException( (status)err )),0) : 0)
		#endif
	#endif

#else

	#define raise(err)						throw( err )
	#define raise_if(exp, err)				((exp) ? (throw( err ), 0 ) : 0)

#endif

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */	

#endif /* _H_EXCEPT */


