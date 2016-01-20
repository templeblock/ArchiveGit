/****************************************************************************
Contains:	Debugging routines for composition toolkit.
*****************************************************************************/

#include <windowsx.h>
#include <stdarg.h>
#include "scexcept.h"

void DbgVPrintf( const scChar*	fmt,
				 va_list		args )
{
	scChar	buf[256];
	wvsprintf( buf, fmt, args );
	int len = strlen( buf );
	
	if ( buf[len - 1] == '\n' )
	{
		buf[len - 1] = 0;
		strcat( buf, scString( "\r\n" ) );
	}
	
	OutputDebugString( buf );
}

void SCDebugTrace( int level, const scChar* fmt, ... )
{
	extern int scDebugTrace;

	if ( level > scDebugTrace )
		return;
	
	va_list args;

	
	if ( fmt && *fmt ) {
		va_start( args, fmt );
		DbgVPrintf( fmt, args );
		va_end( args );
	}	
}

void SCDebugStr ( const scChar* cstr )
{
	OutputDebugString( cstr );
}

void SCAssertFailed ( const scChar* assertStr, const scChar* file, int lineNum )
{
	scChar buf[256];
	
	if ( strlen( assertStr ) + strlen( file ) + 4 < 256 )
		wsprintf( buf, scString( "ASSERT FAILED \"%s\" file \"%s\" line #%d\n" ), assertStr, file, lineNum );
	else
		strcpy( buf, scString( "ASSERT STRING TOO LONG!!!!\n" ) );
			
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

void SCDebugBreak( void )
{
#if SCDEBUG > 1
	DebugBreak();
#else
	MessageBeep( -1 );
#endif
}

void AssertFailed( const scChar *exp, const char *file, int line )
{
	SCDebugTrace( 0, scString( "(%s,%d): assert failed: \"%s\"\n" ), file, line, exp ); 
	raise( scERRassertFailed );
}
