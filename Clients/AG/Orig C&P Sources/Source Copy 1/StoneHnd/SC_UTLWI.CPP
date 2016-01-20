/****************************************************************

	File:		SC_UTLWI.C

	$Header: /Projects/Toolbox/ct/SC_UTLWI.CPP 2     5/30/97 8:45a Wmanis $
		
	Contains:	WINDOWS versions of low level debugging stuff

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	

****************************************************************/

#include "sctypes.h"
#include "scexcept.h"

/*============================================================*/

void SCDebugStr ( const scChar* cstr )
{
    OutputDebugString( cstr );
}

/*============================================================*/

void SCAssertFailed ( const scChar*	assertStr, 
					  const scChar*	file, 
					  int			lineNum )
{
    scChar buf[256];
    
	if ( scStrlen( assertStr ) + scStrlen( file ) + 4 < 256 )
		wsprintf( buf, scString( "ASSERT FAILED \"%s\" file \"%s\" line #%d\n" ), 
				  assertStr, file, lineNum );
	else
		scStrcpy( buf, scString( "ASSERT STRING TOO LONG!!!!\n" ) );
			
	SCDebugStr( buf );

#if SCDEBUG < 1
	raise( scERRassertFailed );
#else
	SCDebugBreak();

		// set doit to true if you want to raise an exception
	int doit = 0;
	if ( doit )
		raise( scERRassertFailed );
#endif
}

/*============================================================*/

void SCDebugBreak( void )
{
#if SCDEBUG > 1
	DebugBreak();
#else
	#ifdef _WIN32
			Beep( 500, 100 );  
	#else   
			MessageBeep( -1 );
	#endif
#endif
}

/*============================================================*/

void SCSysBeep( long duration )
{
#ifdef _WIN32
		Beep( 500, duration );  
#else   
		MessageBeep( -1 );
#endif
}

/*============================================================*/
