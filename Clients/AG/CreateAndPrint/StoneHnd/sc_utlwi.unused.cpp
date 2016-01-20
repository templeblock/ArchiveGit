/****************************************************************
Contains:	WINDOWS versions of low level debugging stuff
****************************************************************/

#include "sctypes.h"
#include "scexcept.h"

void SCDebugStr ( const scChar* cstr )
{
	OutputDebugString( cstr );
}

void SCAssertFailed ( const scChar* assertStr, const scChar* file, int lineNum )
{
	scChar buf[256];
	
	if ( scStrlen( assertStr ) + scStrlen( file ) + 4 < 256 )
		wsprintf( buf, scString( "ASSERT FAILED \"%s\" file \"%s\" line #%d\n" ), assertStr, file, lineNum );
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

void SCDebugBreak( void )
{
#if SCDEBUG > 1
	DebugBreak();
#else
	MessageBeep( -1 );
#endif
}
