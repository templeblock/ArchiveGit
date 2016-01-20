/****************************************************************************

	File:		DEBUG.C

	$Header: /Projects/Toolbox/ct/SCDEBUG.CPP 2     5/30/97 8:45a Wmanis $

	Contains:	Debugging routines for composition toolkit.

	Written by:	Sealy

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	

*****************************************************************************/

#if defined( SCWINDOWS )
	#include <windowsx.h>
#else
	#include <stdio.h>
#endif

#include <stdarg.h>

#include "scexcept.h"

// Debugger output/interrupts

/* ======================================================================== */

void DbgVPrintf( const scChar*	fmt,
				 va_list		args )
{
#if defined( SCWINDOWS )

	scChar	buf[256];
	int		len;
	
	wvsprintf( buf, fmt, args );
	len = scStrlen( buf );
	
	if ( buf[len - 1] == '\n' ) {
		buf[ len - 1] = 0;
		scStrcat( buf, scString( "\r\n" ) );
	}
	
	OutputDebugString( buf );

#elif defined( SCMACINTOSH )
	
	scChar	buf[256];
	vsprintf( buf, fmt, args );
	fputs( buf,  stderr );
	
#endif
	
}

/* ======================================================================== */

void SCDebugTrace( int level, const scChar* fmt, ... )
{
	extern int scDebugTrace;

	if ( level > scDebugTrace )
		return;
	
	va_list	args;

	
	if ( fmt && *fmt ) {
		va_start( args, fmt );
		DbgVPrintf( fmt, args );
		va_end( args );
	}	
}

/* ======================================================================== */
/* Asserts */

void AssertFailed( const scChar *exp, const char *file, int line )
{
#ifdef SCMACINTOSH	
	SCDebugTrace( 0, scString( "(%s,%ld): assert failed: \"%s\"\n" ), file, line, exp );
#else
	SCDebugTrace( 0, scString( "(%s,%d): assert failed: \"%s\"\n" ), file, line, exp );	
#endif
		
	raise( scERRassertFailed );
//	throw( new scException( scERRassertFailed, file, line ) );
}

/* ======================================================================== */
