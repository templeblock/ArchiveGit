/****************************************************************
	File:		SC_UTLTC.C

	$Header: /Projects/Toolbox/ct/SC_UTMAC.CPP 2     5/30/97 8:45a Wmanis $
	
	Contains:	Think C untilities

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************/

//#include "scport.h"
//#include "capplica.h"
//#include "constant.h"
//#include "tbutilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Dialogs.h>
#include <SegLoad.h>
#include <QuickDraw.h>
#include <OSUtils.h>

#include "scTypes.h"

#if defined( THINK_CPLUS ) && THINK_CPLUS < 0x0700
	#include <pascal.h>
#else
	#include <Strings.h>
#endif

Boolean gSCUseSysBreak;

/* ======================================================================== */

void SCDebugStr( const char *str )
{
	char buf[256];
	
	strcpy( buf, str );
	
//	if ( gApplication->TestDebuggerPresence() ) {
		if (gSCUseSysBreak)
			SysBreakStr( c2pstr( buf ) );
		else
			DebugStr( c2pstr( buf ) );
//	}
//	else {
//		ParamText( c2pstr( buf ), (StringPtr)"", (StringPtr)"", (StringPtr)"");
//		PositionDialog('ALRT', ALRTgeneral);
//		InitCursor();
//		Alert(ALRTgeneral, NULL);
//	}	
}

/* ======================================================================== */

void SCAssertFailed( const scChar *str, const scChar *file, int line )
{
	char buf[256];
	
	sprintf( buf, "ASSERT FAILED \"%s\" File %s Line %ld", str, file, line );
	SCDebugStr( buf );
}

/* ======================================================================== */

void SCDebugBreak( void )
{
	DebugStr( "\pSCDEBUGBREAK" );
}

/* ======================================================================== */
