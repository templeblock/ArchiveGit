/***************************************************************************

	File:		SCAPI.C


	$Header: /Projects/Toolbox/ct/SCAPI.CPP 3     5/30/97 8:45a Wmanis $
	
	Contains:	Application Program Interface for the
				Stonehand Composition Toolbox. For the most part 
				this file is simply a bottle nect module. All
				documentation for the functions contained within 
				are found in scappint.h.

	Written by: Manis


	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


****************************************************************************/
                     

#include "scappint.h"
#include "scpubobj.h"


#include "scannota.h"
#include "scapptex.h"
#include "sccolumn.h"
#include "scexcept.h"
#include "scstcach.h"
#include "scglobda.h"
#include "scmem.h"
#include "scparagr.h"
#include "scregion.h"
#include "scset.h"
#include "scstream.h"
#include "sctextli.h"


static int 		gInited;
static int 		gBaseError;
int				scDebugTrace = 0;

/* ==================================================================== */


#if 0
	static int gInputLevel;
	
		// if scDebugTrace is set to a value >0 all calls into
		// the toolbox will be traced, may be useful for understanding
		// a behavior or pointing the finger!
	
	inline void EnterMonitor( const scChar *str )
	{
		scAssert( gInited );
		SCDebugTrace( 0, scString( "\n+%s\n" ), str );
	}

	inline void ExitMonitor( const scChar *str )
	{
		SCDebugTrace( 0, scString( "-%s\n" ), str );	
	}
#else
	#define EnterMonitor( x )
	#define ExitMonitor( x )
#endif

	

/* ==================================================================== */

void		BRKInitMach( void );
void		BRKFreeMach( void );


char* stoneVersion =  __DATE__" - "__TIME__;

status scIMPL_EXPORT	SCENG_Init( int baseError )
{
		// The following are pool definitions that are passed
		// to the initiailization of the memory manager.
		// The last pool is the default memory allocation pool
		// all others are fixed size pools, these are not sorted
		// at this time
	static scPoolInfo	objPools[] = {
		{ sizeof( scTextline ),				0 },
		{ sizeof( scContUnit ),				0 },
		{ sizeof( scAbstractArray ),		0 },
		{ 0,								0 }
	};
	
#ifndef useCPLUSEXCEPTIONS
		// if we are not using C++ exceptions - initialize our own
		// exception manager.
	scExceptContext::Initialize( 0 );
#endif
	
	status stat = scSuccess;
	
	gBaseError	= baseError;

	try {
		MEMInit( objPools );				// initialize memory manager
//		scAssert( sizeof( CharRecord ) == ( sizeof( long ) * 2 ) );
		BRKInitMach();						// initialize breaking machine
		scCachedStyle::BuildCache( 16 );	// build internal spec cache
		gInited					= true;
	} 
	IGNORE_RERAISE;

	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCENG_Fini( void )
{  
	status stat = scSuccess;   
	
	try {
		scAssert( gInited );
		BRKFreeMach( );
		scColumn::FiniCTXList();
		scCachedStyle::DeleteCache( );
		
		gInited = false;
		MEMFini();
	} 
	IGNORE_RERAISE;
		
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCENG_RetainMemory ( void )
{
	//	MEMSetRestrictions( memRetain );
	return scSuccess;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCENG_UseRetainedMemory ( void )
{
	//	MEMSetRestrictions( memUseRetained );
	return scSuccess;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCENG_ReleaseMemory ( void )
{
	//	MEMSetRestrictions( memNoRestrictions );
	return scSuccess;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCENG_ChangedTS ( TypeSpec		ts,
										  eSpecTask		task,
										  scRedispList*	redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCENG_ChangedTS" ) );

	scCachedStyle::StyleInvalidateCache ( ts );
	
	try {
		if ( task & eSCDoAll )
			scColumn::ChangedTS( ts, task, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCENG_ChangedTS" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCHTS_Alloc( scSpecLocList*&	cslist,
									  scStream*			stream )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCCHTS_Alloc" ) );
	
	cslist = 0;
	
	try {
		cslist = SCNEW scSpecLocList( stream );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCCHTS_Alloc" ) );
	
	return stat;	
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCHTS_Delete( scSpecLocList*& cslist )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCCHTS_Delete" ) );
	
	try {
		delete cslist, cslist = 0;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCCHTS_Delete" ) );
	
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCTSL_Alloc( scTypeSpecList*& tslist )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCTSL_Alloc" ) );
	
	tslist = 0;
	
	try {
		tslist = SCNEW scTypeSpecList;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCTSL_Alloc" ) );

	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCTSL_Delete( scTypeSpecList*& tslist )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCTSL_Delete" ) );
	
	try {
		delete tslist, tslist = 0;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCTSL_Delete" ) );

	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCRDL_Alloc( scRedispList*& rdlist )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCRDL_Alloc" ) );
	
	rdlist = 0;
	
	try {
		rdlist = SCNEW scRedispList;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCRDL_Alloc" ) );

	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCRDL_Delete( scRedispList*& rdlist )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCRDL_Delete" ) );
	
	try {
		delete rdlist, rdlist = 0;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCRDL_Delete" ) );

	return stat;
}

/* ==================================================================== */
/* Recompose a single column with extreme prejudice */

status scIMPL_EXPORT	SCCOL_Recompose( scColumn*			col,
										 scRedispList*	redispList )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCCOL_Recompose" ) );

	try {
		col->Rebreak( redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_Recompose" ) );
	return stat;
}

/* ==================================================================== */
/* Recompose a single column with extreme prejudice */

status scIMPL_EXPORT	SCRebreakCol ( scColumn*		col, 
									   scRedispList*	redispList )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCRebreakCol" ) );

	try {
		col->Rebreak2( redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCRebreakCol" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCFS_SetRecompose( scColumn* col, Bool tf )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCFS_SetRecompose" ) );
	
	try {
		if ( col )
			col->SetRecomposition( tf );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCFS_SetRecompose" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCFS_GetRecompose( scColumn*	col,
										   Bool&	tf )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCFS_GetRecompose" ) );

	try {
		tf = col->GetRecomposition();
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCFS_GetRecompose" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCFS_Recompose( scColumn*			col,
										scRedispList*	redispList )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCFS_Recompose" ) );

	try {
		col->RecomposeFlowset( LONG_MAX, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCFS_Recompose" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCFS_Recompose( scColumn*			col,
										long			ticks,
										scRedispList*	redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCFS_Recompose" ) );

	try {
		col->RecomposeFlowset( ticks, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCFS_Recompose" ) );
	return stat;
}

/* ==================================================================== */


status scIMPL_EXPORT	SCCOL_New( APPColumn	appName,
								   scColumn*& 	col,
								   MicroPoint	width,
								   MicroPoint	depth )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_New" ) );

	try {	
		raise_if( width < 0 || depth < 0, scERRinput );
		col = SCNEW scColumn( appName, width, depth );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_New" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSCR_Write( scScrapPtr		scrap,
									 APPCtxPtr		ctxPtr,
									 IOFuncPtr		writeFunc )
{	
	status	stat = scSuccess;
	EnterMonitor( scString( "SCSCR_Write" ) );
	
	try {  
		if ( scrap->IsClass( "scColumn" ) ) {
			scColumn* col = (scColumn*)scrap;
			col->ZeroEnumeration();
			col->StartWrite( ctxPtr, writeFunc );
		}
		else if ( scrap->IsClass( "scStream" ) ) {
			scStream* stream = (scStream*)scrap;
			stream->STRZeroEnumeration();
			stream->STRWriteToFile( ctxPtr, writeFunc );
		}
		else
			raise( scERRidentification );
		scTBObj::WriteNullObject( ctxPtr, writeFunc );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSCR_Write" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSCR_ReadCol( scScrapPtr&	scrap,
									   scSet*		enumTable,
									   APPCtxPtr	ctxPtr,
									   IOFuncPtr	readFunc )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSCR_ReadCol" ) );

	try {
		scColumn* col;
		col = (scColumn*)scTBObj::StartRead( enumTable, ctxPtr, readFunc );
		scAssert( scTBObj::StartRead( enumTable, ctxPtr, readFunc ) == 0 );
		scrap = col;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSCR_ReadCol" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSCR_ReadStream( scScrapPtr&	scrapH,
										  scSet*		enumTable,
										  APPCtxPtr		ctxPtr,
										  IOFuncPtr		readFunc )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSCR_ReadStream" ) );
	
	scrapH = 0;

	try {
		scStream* stream = scStream::STRFromFile( enumTable, ctxPtr, readFunc );
		scAssert( scTBObj::StartRead( enumTable, ctxPtr, readFunc ) == 0 );		
		scrapH	= stream;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString(  "SCSCR_ReadStream" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSCR_TSList( scScrapPtr		scrap,
									  scTypeSpecList&	tsList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSCR_TSList" ) );
	
	try {
		if ( scrap->IsClass( "scColumn" ) )
			((scColumn *)scrap)->GetTSList( tsList );
		else if ( scrap->IsClass( "scContUnit" ) )
			((scStream*)scrap)->GetTSList ( tsList );
		else
			stat = scERRidentification;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString(  "SCSCR_TSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSCR_Free( scScrapPtr scrap )
{
	status	stat = scSuccess;
	long	bytesFreed;
	
	EnterMonitor( scString( "SCSCR_Free" ) );

	try {
		if ( !scrap )
			;
		else if ( scrap->IsClass( "scColumn" ) )
			((scColumn*)scrap)->FreeScrap();
		else if ( scrap->IsClass( "scContUnit" ) )
			((scContUnit*)scrap)->FreeScrap( bytesFreed );
		else
			raise( scERRidentification );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSCR_Free" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_Delete( scColumn* 	col,
									  scRedispList*	redispList )

{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCCOL_Delete" ) );

	try {
		col->Delete( redispList );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCCOL_Delete" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_Read( scStream*&	stream,
									scSet*		enumTable,
									APPCtxPtr	ctxPtr,										 
									IOFuncPtr	readFunc )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_Read" ) );

	try {
		stream = scStream::STRFromFile( enumTable, ctxPtr, readFunc );
		scAssert( scTBObj::StartRead( enumTable, ctxPtr, readFunc ) == 0 );		
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_Read" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_Write( scStream*	stream,
									 APPCtxPtr	ctxPtr,
									 IOFuncPtr	writeFunc )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_Write" ) );

	try {
		stream->STRWriteToFile( ctxPtr, writeFunc );
		scTBObj::WriteNullObject( ctxPtr, writeFunc );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSTR_Write" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_Cut ( scStream*	streamID,
									scRedispList*	redispList )
{
	status		stat = scSuccess;
	scColumn*	col;
	scTextline*	txl;

	EnterMonitor( scString( "SCSTR_Cut" ) );

	try {
		txl = streamID->GetFirstline();
		if	( txl )
			col =  txl->GetColumn( );
		else
			col = scColumn::FindFlowset( streamID );

		if ( col )
			col->FlowsetCutStream ( streamID, redispList );
		else
			raise( scERRstructure );
	}
	IGNORE_RERAISE;

	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_Copy( const scStream*	stream,
									scStream*&		newStream )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_Copy" ) );
	
	try {
		stream->STRCopy( newStream );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_Copy" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCFS_PasteStream ( scColumn*			col,
										   scStream*		streamID,
										   scRedispList*	redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCFS_PasteStream" ) );

	try {
		col->FlowsetPasteStream( streamID, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCFS_PasteStream" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_Clear ( scStream*		stream,
									  scRedispList*	redispList )
{
	status 		stat = scSuccess;
	scColumn*	col;
	scTextline*	txl;

	EnterMonitor( scString( "SCSTR_Clear" ) );

	try {
		if ( stream ) {
			txl = stream->GetFirstline();
			if	( txl ) {
				col = txl->GetColumn( );
				if	( col )
					col->FlowsetClearStream ( redispList );
				else
					raise( scERRstructure );
			}
			else if ( stream->FindColumn( col ) )
				col->FlowsetClearStream( redispList );
			else
					/* if no layout structure associated with stream */		
				stream->STRFree();		 
		}
		else
			raise( scNoAction );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_Clear" ) );
	return stat;
}

/* ==================================================================== */
// Extracts a scContUnit from a scStreamLocation for use with SCSTR_Split

status scIMPL_EXPORT	SCSEL_GetContUnit( scContUnit*& mark,
										   scContUnit*& point,										  
										   const scSelection* sl )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_GetContUnit" ) );
	
	try {
		sl->GetContUnits( mark, point );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_GetContUnit" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_Split( scStream*		stream1,
									 scContUnit*	cu,
									 scStream*&		stream2 )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_Split" ) );
	
	try {
		stream2 = stream1->Split( cu );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_Split" ) );
	return stat;
}

/* ==================================================================== */
/* compare streams for equality, this tests content and specs
 * scSuccess == equality
 */
 
status scIMPL_EXPORT	SCSTR_Compare( const scStream* str1,
									   const scStream* str2 )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCSTR_Compare" ) );
	
	try {
		stat = str1->Compare( str2 ) ? scSuccess : scNoAction;
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_Compare" ) );
	
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_SetSize( scColumn* 		col,
									   MicroPoint		width,
									   MicroPoint		depth,
									   scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_SetSize" ) );


	try {
		if	( width < 0 || depth < 0 )
			raise( scERRinput );
		
		col->Resize( width, depth, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_SetSize" ) );
	return stat;
}

/* ==================================================================== */
// is there any text associated with this column

status scIMPL_EXPORT	SCCOL_HasText( scColumn* col )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_HasText" ) );
	
	try {
		stat = col->HasText( ) ? scSuccess : scNoAction;
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCCOL_HasText" ) );
	return stat;
}

/* ==================================================================== */

// tests to see if there is more text than is in this column
// this would set the flag to true if:
//			there is text in subsequent linked columns
//			there is unformatted text that will not fit in this column

status scIMPL_EXPORT	SCCOL_MoreText( scColumn*	col,
										Bool&		flag )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_MoreText" ) );

	try {
		flag = col->MoreText( );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_MoreText" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_LinePositions ( scColumn*			col,
											  scLineInfoList*	lineInfo,
											  long&				nLines,
											  Bool&				moreText )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_LinePositions" ) );

	try {
		col->LineInfo( lineInfo, nLines, moreText );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_LinePositions" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_SetVertFlex ( scColumn*		col,
											scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_SetVertFlex" ) );
	
	try {
		col->SetVertFlex ( true, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_SetVertFlex" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_ClearVertFlex ( scColumn* 		col,
											  scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_ClearVertFlex" ) );
	
	try {
		col->SetVertFlex ( false, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_ClearVertFlex" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_SetHorzFlex ( scColumn*		col,
											scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_SetHorzFlex" ) );
	
	try {
		col->SetHorzFlex ( true, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_SetHorzFlex" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_ClearHorzFlex ( scColumn* 	col,
											  scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_ClearHorzFlex" ) );
	
	try {
		col->SetHorzFlex ( false, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_ClearHorzFlex" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_GetVertFlex( scColumn*	col,
										   Bool&		tf )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_GetVertFlex" ) );
	
	try {
		tf = col->GetVertFlex( );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_GetVertFlex" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_GetHorzFlex ( scColumn*	col,
											Bool&		tf )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_GetHorzFlex" ) );
	
	try {
		tf = col->GetHorzFlex( );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_GetHorzFlex" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_GetFlowDirection( scColumn*	col,
												scFlowDir&	flodir )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_GetFlowDirection" ) );
	
	try {
		flodir = col->GetFlowdir();
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_GetFlowDirection" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_SetFlowDirection( scColumn*			col,
												const scFlowDir&	flodir )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_SetFlowDirection" ) );
	
	try {
		col->FlowsetSetFlowdir( flodir );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_SetFlowDirection" ) );
	return stat;
}

/* ==================================================================== */

#if defined( scColumnShape )
status scIMPL_EXPORT	SCCOL_PastePoly ( scColumn*			col,
										  const scVertex*	vert,
										  scRedispList*		redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_PastePoly" ) );

	try {
//		col->PastePoly ( vert, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_PastePoly" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_ClearPoly ( scColumn*		col,
										  scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_ClearPoly" ) );
	
	try {
		col->ClearShape( redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_ClearPoly" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_CopyPoly ( scColumn*	col,
										 scVertex*& vert )
{
	status stat = scSuccess;
	/*CLIPSTUFF*/
	EnterMonitor( scString( "SCCOL_CopyPoly" ) );

	try {
//		col->CopyPoly( vert );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_CopyPoly" ) );
	return stat;
}

/*================================ REGIONS ===========================*/

status scIMPL_EXPORT	SCCOL_PasteRgn ( scColumn* 			col,
										 const HRgnHandle	rgnH,
										 scRedispList*		redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_PasteRgn" ) );

	try {
		col->PasteRgn( rgnH, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_PasteRgn" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_ClearRgn ( scColumn* 		col,
										 scRedispList 	*redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_ClearRgn" ) );
	
	try {
		col->ClearShape( redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_ClearRgn" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_CopyRgn ( scColumn* 	col,
										HRgnHandle&	rgnH )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_CopyRgn" ) );
	
	try {
		col->CopyRgn( rgnH );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_CopyRgn" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_New( HRgnHandle&	hrgH,
									MicroPoint	sliverSize )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_New" ) );
	
	try {
		hrgH = NewHRgn( sliverSize );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_New" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Dispose( HRgnHandle hrgH )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Dispose" ) );
	
	try {
		DisposeHRgn( hrgH );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Dispose" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Empty( HRgnHandle hrgH )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Empty" ) );
	
	try {
		stat = EmptyHRgn( hrgH ) ? scSuccess : scNoAction;
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Empty" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Equal( const HRgnHandle	a,
									  const HRgnHandle b )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Equal" ) );
	
	try {
		stat = EqualHRgn( a, b ) ? scSuccess : scNoAction;
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Equal" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_PtIn( const HRgnHandle	hrgH,
									 const scMuPoint&	pt )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_PtIn" ) );
	
	try {
		stat = PtInHRgn( hrgH, pt ) ? scSuccess : scNoAction;
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_PtIn" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Rect( HRgnHandle		hrgH,
									 const scXRect&	xrect )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Rect" ) );
	
	try {
		RectHRgn( hrgH, xrect );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Rect" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Poly( HRgnHandle			hrgH,
									 const scVertex*	verts )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Poly" ) );
	
	try {
		PolyHRgn( hrgH, verts );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Poly" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Copy( HRgnHandle			dstRgn,
									 const HRgnHandle	srcRgn )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Copy" ) );
	
	try {
		CopyHRgn( dstRgn, srcRgn );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Copy" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Sect( const HRgnHandle	a,
									 const HRgnHandle	b,
									 HRgnHandle			dstRgnH )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Sect" ) );

	try {
		SectHRgn( a, b, dstRgnH );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Sect" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Union( const HRgnHandle	a,
									  const HRgnHandle	b,
									  HRgnHandle		dstRgnH )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Union" ) );

	try {
		UnionHRgn( a, b, dstRgnH );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Union" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Diff( const HRgnHandle	a,
									 const HRgnHandle	b,
									 HRgnHandle			dstRgnH )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Diff" ) );

	try {
		DiffHRgn( a, b, dstRgnH );		
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Diff" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Xor( const HRgnHandle	a,
									const HRgnHandle	b,
									HRgnHandle			dstRgnH )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Xor" ) );

	try {
		XorHRgn( a, b, dstRgnH );				
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Xor" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Translate( HRgnHandle	hrgH,
										  MicroPoint	x,
										  MicroPoint	y )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Translate" ) );

	try {
		TranslateHRgn( hrgH, x, y );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Translate" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_Inset( HRgnHandle	hrgH,
									  MicroPoint	x,
									  MicroPoint	y )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_Inset" ) );

	try {
		InsetHRgn( hrgH, x, y, true );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_Inset" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_SetEmpty( HRgnHandle hrgH )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_SetEmpty" ) );
	
	try {
		SetEmptyHRgn( hrgH );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_SetEmpty" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_SliverSize( HRgnHandle		hrgH,
										   MicroPoint&	sliverSize )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_SliverSize" ) );
	
	try {
		sliverSize = RGNSliverSize( hrgH );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_SliverSize" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCHRGN_RectIn( const HRgnHandle	hrgH,
									   const scXRect&	xrect )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCHRGN_RectIn" ) );
	
	try {
		stat = RectInHRgn( hrgH, xrect ) ? scSuccess : scNoAction;
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCHRGN_RectIn" ) );
	return stat;
}


/* ==================================================================== */

#endif


/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_Update( scColumn*			col,
									  const scXRect&	xrect,
									  APPDrwCtx			mat )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_Update" ) );
	
	try {
		col->Draw ( xrect, mat );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_Update" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_UpdateLine( scColumn*			col,
										  scImmediateRedisp&	lineDamage,	
										  APPDrwCtx			mat )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_UpdateLine" ) );

	try {
		col->UpdateLine( lineDamage, mat );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_UpdateLine" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_TSList ( scColumn*			col,
									   scTypeSpecList&	tsList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_TSList" ) );
	
	try {
		col->GetTSList( tsList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_TSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_TSList ( scStream*		stream,
									   scTypeSpecList&	tsList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_TSList" ) );
	
	try {
		stream->STRGetTSList( tsList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_TSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_ParaTSList ( scStream*		stream,
									   scTypeSpecList&	tsList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_ParaTSList" ) );
	
	try {
		stream->GetParaTSList( tsList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_ParaTSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_CHTSList ( scStream*		stream,
										 scSpecLocList&	csList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_CHTSList" ) );
	
	try {
		stream->GetCharSpecList( csList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_CHTSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_SetDepthNVJ( scColumn*			col,
										   MicroPoint		depth,
										   scRedispList 	*redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_SetDepthNVJ" ) );

	try {
		col->SetDepthNVJ ( depth, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_SetDepthNVJ" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_FlowJustify( scColumn*		col,
										   eVertJust	attributes )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_FlowJustify" ) );
	
	try {
		col->SetVJ( attributes );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_FlowJustify" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_ChCount( scStream*	stream,
									   long&		count )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_ChCount" ) );
	
	try {
		stream->ChCount( count );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_ChCount" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_TSList ( scSelection*		selection,
									   scTypeSpecList&	tsList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_TSList" ) );
	
	try {
		selection->GetTSList( tsList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_TSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_CHTSList( scSelection*		selection,
										scSpecLocList& 	csList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_CHTSList" ) );
	
	try {
		selection->GetCharSpecList( csList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_CHTSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_PARATSList( scSelection*		sel,
										  scSpecLocList&   cslist )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_PARATSList" ) );
	
	try {
		sel->GetParaSpecList( cslist );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_PARATSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_PARATSList( scSelection*		sel,
										  scTypeSpecList&	tsList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_PARATSList" ) );
	
	try {
		sel->GetParaSpecList( tsList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_PARATSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_PARATSList( scStream*			stream,
										  scSpecLocList&	cslist )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_PARATSList" ) );
	
	try {
		stream->GetParaSpecList( cslist );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_PARATSList" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_CHTSListSet ( scStream*				str,
											const scSpecLocList&	csList,
											scRedispList*			redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_CHTSListSet" ) );
	
	try {
		scAssert( str == csList.GetStream() );
		str->SetCharSpecList( csList, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_CHTSListSet" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_PARATSListSet( scStream*				str,	
											 const scSpecLocList&	cslist,
											 scRedispList*			rInfo )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_PARATSListSet" ) );
	
	try {
		scAssert( str == cslist.GetStream() );
		str->SetParaSpecList( cslist, rInfo );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_PARATSListSet" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCExternalSize ( scColumn* 	col,
										 long&		size )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCExternalSize" ) );
	
	try {
		col->ExternalSize( size );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCExternalSize" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCTB_ZeroEnumeration( void )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCTB_ZeroEnumeration" ) );
	
	try {
		scColumn*	col = scColumn::GetBaseContextList();
		for ( ; col; col = col->GetContext() )
			col->ZeroEnumeration();
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCTB_ZeroEnumeration" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSET_InitRead( scSet*&	enumTable, 
										long	maxsize )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSET_InitRead" ) );
	
	try {
		enumTable = SCNEW scSet;
		enumTable->SetNumSlots( maxsize );
		enumTable->SetRetainMem( true );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSET_InitRead" ) );	
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSET_FiniRead( scSet*			enumTable,
										scRedispList*	redispList )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCSET_FiniRead" ) );

	try {
		delete enumTable, enumTable = 0;
		scColumn::Update( redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSET_FiniRead" ) );
	
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSET_Abort( scSet*& enumTable )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSET_Abortt" ) );
	try {
		enumTable->DeleteAll();
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSET_Abortt" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCOBJ_PtrRestore( scTBObj*	obj,
										  scSet*	enumTable )

{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCOBJ_PtrRestore" ) );

	try {
		long	i,
				limit = enumTable->GetNumItems();

		for ( i = 0; i < limit; i++ ) {
			scTBObj* ptr = (scTBObj*)enumTable->Get( i );
			if ( ptr )
				ptr->RestorePointers( enumTable );
		}
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCOBJ_PtrRestore" ) );
	
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_QueryInkExtents( scColumn*	col,
											   scXRect&	xrect )
{
	status	stat = scSuccess;
	
	EnterMonitor( scString( "SCCOL_QueryInkExtents" ) );

	try {
		col->ComputeInkExtents();
		xrect = col->GetInkExtents();
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_QueryInkExtents" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_QueryMargins( scColumn*	col,
											scXRect&	xrect )
{
	status	stat = scSuccess;
	
	EnterMonitor( scString( "SCCOL_QueryMargins" ) );

	try {
		col->QueryMargins( xrect );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_QueryMargins" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_Size( scColumn* 	col,
									scSize&		size )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_Size" ) );
	
	try {
		col->QuerySize( size );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_Size" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCOBJ_Enumerate( scTBObj*	obj,
										 long&		objEnumerate )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCOBJ_Enumerate" ) );

	try {
		if ( obj->IsClass( "scColumn" ) )
			((scColumn*)obj)->Enumerate( objEnumerate );
		else if ( obj->IsClass( "scContUnit" ) )
			((scStream*)obj)->DeepEnumerate( objEnumerate );
		else
			raise( scERRstructure );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCOBJ_Enumerate" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_Link( scColumn*		col1,
									scColumn*		col2,
									scRedispList*	redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_Link" ) );

	try {
		col1->Link( col2, true, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_Link" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_Unlink ( scColumn*		col,
									   scRedispList* 	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_Unlink" ) );
	
	try {
		col->Unlink( redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_Unlink" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCFS_Split( scColumn*	col1, 
									scColumn*	col2 )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCFS_Split" ) );
	
	try {
		col1->BreakChain( col2 );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCFS_Split" ) );
	return stat;

}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_GetStream ( scColumn*		col,
										  scStream*&	stream )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_GetStream" ) );
	
	try {
		stream = col->GetStream ();
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_GetStream" ) );
	return stat;
}

/* ==================================================================== */


status scIMPL_EXPORT	SCFS_ReadTextFile ( scColumn* 		col,
											TypeSpec		spec,
											APPCtxPtr		ctxPtr,
											IOFuncPtr		readFunc,
											scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCFS_ReadTextFile" ) );

	try {
		col->ReadTextFile( spec, ctxPtr, readFunc, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCFS_ReadTextFile" ) );
	return stat;
}

/* ==================================================================== */


status scIMPL_EXPORT	SCWriteTextFile ( scStream*	stream,
										  APPCtxPtr	ctxPtr,
										  IOFuncPtr	writeFunc )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCWriteTextFile" ) );
	
	try {
		stream->STRWriteTextFile( ctxPtr, writeFunc, false );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCWriteTextFile" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCTextFileToScrap ( scScrapPtr&	scrapH,
											APPCtxPtr	ctxPtr,
											IOFuncPtr	readFunc )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCTextFileToScrap" ) );

	try {
		TypeSpec nullSpec;

		scStream* stream = scStream::ReadTextFile( nullSpec, ctxPtr, readFunc, 0 );
		scrapH	= stream; 
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCTextFileToScrap" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCAPPTXT_Alloc( stTextImportExport*& apptext )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCAPPTXT_Alloc" ) );
	
	apptext = 0;
	
	try {
		apptext = &stTextImportExport::MakeTextImportExport( 1 );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCAPPTXT_Alloc" ) );

	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCAPPTXT_Delete( stTextImportExport* apptext )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCAPPTXT_Delete" ) );
	
	try {
		apptext->release();
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCAPPTXT_Delete" ) );

	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCFS_PasteAPPText( scColumn*			col,
										   stTextImportExport&		appText,
										   scRedispList*	redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCFS_PasteAPPText" ) );

	try {
		col->PasteAPPText( appText, redispList );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCFS_PasteAPPText" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_PasteAPPText( scSelection*			sel,
										   stTextImportExport&		appText,
										   scRedispList*	redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_PasteAPPText" ) );

	try {
		sel->PasteAPPText( appText, redispList );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_PasteAPPText" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_GetAPPText( scStream*		stream,
										  stTextImportExport&	appText )
{
	status stat = scSuccess;
	/*CLIPSTUFF*/
	EnterMonitor( scString( "SCSTR_GetAPPText" ) );

	try {
		stream->CopyAPPText ( appText );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSTR_GetAPPText" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_GetAPPText( scSelection*			selection,
										  stTextImportExport&	appText )
{
	status stat = scSuccess;
	/*CLIPSTUFF*/
	EnterMonitor( scString( "SCSEL_GetAPPText" ) );

	try {
		selection->CopyAPPText ( appText );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_GetAPPText" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_Write( scColumn*	col,
									 APPCtxPtr	ctxPtr,
									 IOFuncPtr	writeFunc )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_Write" ) );

	try {
		col->StartWrite( ctxPtr, writeFunc );
		scTBObj::WriteNullObject( ctxPtr, writeFunc );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_Write" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_Read( APPColumn		appName,
									scColumn*&		col,
									scSet*			enumTable,
									APPCtxPtr		ctxPtr,
									IOFuncPtr		readFunc )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_Read" ) );
	
	try {
		col = (scColumn*)scTBObj::StartRead( enumTable, ctxPtr, readFunc );
		scAssert( scTBObj::StartRead( enumTable, ctxPtr, readFunc ) == 0 );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCCOL_Read" ) );
	return stat;
}

/* ==================================================================== */
/*================== SELECTION MESSAGES ==================*/

status scIMPL_EXPORT	SCCOL_ClickEvaluate ( scColumn*			col,
											  const scMuPoint&	pt,
											  REAL&				dist )
{	
	status		stat = scSuccess;
	scMuPoint	cmpt = pt;
	
	EnterMonitor( scString( "SCCOL_ClickEvaluate" ) );

	try {
		col->ClickEvaluate( cmpt, dist );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_ClickEvaluate" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_StartSelect( scColumn* 		col,
										   const scMuPoint&	pt,
										   HiliteFuncPtr DrawRect,
										   APPDrwCtx		mat,
										   scSelection*&	selectID )
{
	status		stat = scSuccess;
	EnterMonitor( scString( "SCCOL_StartSelect" ) );

#if SCDEBUG > 1
	SCDebugTrace( 2, scString( "SCCOLStartSelect %d %d\n" ), pt.x, pt.y );
#endif

	try {
		col->StartClick( pt, DrawRect, mat, selectID );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_StartSelect" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_StartSelect( scColumn*			col,
										   scStreamLocation&	mark,
										   const scMuPoint&		pt,
										   HiliteFuncPtr		DrawRect,
										   APPDrwCtx			mat,
										   scSelection*&		selectID )
{
	status 		stat = scSuccess;
	
	EnterMonitor( scString( "SCCOL_StartSelect" ) );

	try {
		col->StartShiftClick( mark, pt, DrawRect, mat, selectID );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_StartSelect" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_ExtendSelect( scColumn* 			col,
											const scMuPoint&	pt,
											HiliteFuncPtr 		DrawRect,
											APPDrwCtx,
											scSelection*		select )
{
	status		stat = scSuccess;
	
	EnterMonitor( scString( "SCCOL_ExtendSelect" ) );

//	SCDebugTrace( 0, scString( "SCCOLExtendSelect ENTER %d %d\n" ), pt.x, pt.y );

	try {
		raise_if( select == 0, scERRinput );
		col->ContinueClick( pt, DrawRect, select );
	}
	IGNORE_RERAISE;

//	SCDebugTrace( 0, scString( "SCCOLExtendSelect EXIT %d %d\n" ), pt.x, pt.y );
	
	ExitMonitor( scString( "SCCOL_ExtendSelect" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_InitialSelect ( scColumn* 	col,
											  TypeSpec&		typespec,
											  scSelection*&	selectID )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_InitialSelect" ) );

	try {
		col->InitialSelection( typespec, selectID );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_InitialSelect" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_Decompose ( scSelection*		select,
										  scStreamLocation&	mark,
										  scStreamLocation&	point )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_Decompose" ) );

	try {
		select->Decompose( mark, point );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_Decompose" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_Decompose2( scSelection*		select,
										  scStreamLocation&	mark,
										  scStreamLocation&	point )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_Decompose" ) );

	try {
		select->Decompose2( mark, point );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_Decompose" ) );
	return stat;
}

/* ==================================================================== */
status scIMPL_EXPORT	SCSEL_Invalidate( scSelection*& select )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_Invalidate" ) );
	
	try {
		if ( select )
			select->Invalidate();
	}
	IGNORE_RERAISE;
	
	select = 0;
	
	ExitMonitor( scString( "SCSEL_Invalidate" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_Restore( const scStream*			stream,
									   const scStreamLocation&	mark,
									   const scStreamLocation&	point,
									   scSelection*&			select,
									   Bool						geometryChange )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_Restore" ) );

	try {
		scColumn* col = scColumn::FindFlowset( stream );

			// we cannot create a selection if there is no layout
		raise_if( col == 0, scERRstructure );
		   
		select = col->FlowsetGetSelection();

		select->Restore( &mark, &point, stream, geometryChange );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_Restore" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCCOL_SelectSpecial( scColumn*			col,
											 const scMuPoint&	pt,
											 eSelectModifier	selectMod,
											 scSelection*&		selectID )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCCOL_SelectSpecial" ) );

	try {
		col->SelectSpecial( pt, selectMod, selectID );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCCOL_SelectSpecial" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_Move( scSelection*	select,
									eSelectMove		moveSelect )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCSEL_Move" ) );

	try {
		select->MoveSelect( moveSelect );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_Move" ) );
	
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_Extend( scSelection*	select,
									  eSelectMove	moveSelect )
{
	status stat = scSuccess;
	
	EnterMonitor( scString( "SCSEL_Move" ) );

	try {
		select->Extend( moveSelect );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_Move" ) );
	
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_Hilite( scSelection*	select,
									  HiliteFuncPtr	DrawRect )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_Hilite" ) );
	
	try {
		select->ValidateHilite( DrawRect );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_Hilite" ) );
	return stat;
}

/* ==================================================================== */
/*================== EDITING MESSAGES ==================*/

status scIMPL_EXPORT	SCSEL_InsertKeyRecords( scSelection*	select,
												short			keyCount,
												scKeyRecord*	keyRecords, /* array of key recds */
												scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_InsertKeyRecords" ) );

	try {
		select->KeyArray( keyCount, keyRecords, redispList );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_InsertKeyRecords" ) );	
	return stat;
}


/* ==================================================================== */

status SCSEL_InsertField( scSelection*		sel,
						  const clField&	field,
						  TypeSpec&			spec,
						  scRedispList*		redisplist )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_InsertAnnotation" ) );

	try {
		sel->InsertField( field, spec, redisplist );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_InsertAnnotation" ) );	
	return stat;
}

/* ==================================================================== */
		
status scIMPL_EXPORT	SCSEL_SetTextStyle ( scSelection*	selection,
											 TypeSpec		style,
											 scRedispList*	redispList )
{	
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_SetTextStyle" ) );

	try {
		selection->SetStyle( style, redispList );
	}
	IGNORE_RERAISE;
		
	ExitMonitor( scString( "SCSEL_SetTextStyle" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_TextTrans ( scSelection*	select,
										  eChTranType	trans,
										  int			numChars,		// this is a modifier for the string
										  scRedispList	*redispList )

{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_TextTrans" ) );

	try {
		select->TextTrans( trans, numChars, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_TextTrans" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_CutText ( scSelection*	selection,
										scScrapPtr&		scrap,
										scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_CutText" ) );

	try {
		selection->CutText( scrap, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_CutText" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_ClearText( scSelection*	selection,
										 scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_ClearText" ) );
	
	try {
		selection->ClearText( redispList, true );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_ClearText" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_CopyText ( scSelection*	selection,
										 scScrapPtr&	scrap )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_CopyText" ) );
	
	try {
		selection->CopyText( scrap );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_CopyText" ) );
	return stat;
}


/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_PasteText ( scSelection*	selection,
										  scScrapPtr	scrap,
										  TypeSpec		style,
										  scRedispList*	redispList )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_PasteText" ) );

	try {
		selection->PasteText( (scStream*)scrap, style, redispList );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSEL_PasteText" ) );
	return stat;
}

/* ==================================================================== */


status scIMPL_EXPORT	SCSCR_ConToSys ( scScrapPtr				scrap,
										 SystemMemoryObject&	pSysConBlock )
{
	status		stat = scSuccess;
	scContUnit*	para = (scContUnit*)scrap;
	
	EnterMonitor( scString( "SCSCR_ConToSys" ) );

	try {
		if ( scrap->IsClass( "scColumn" ) )
			para = ((scColumn*)scrap)->GetStream();

		if ( para->IsClass( "scContUnit" ) )
			((scStream*)para)->STRWriteMemText( false, pSysConBlock );
		else
			raise( scERRidentification );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSCR_ConToSys" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSCR_SysToCon( scScrapPtr&		scrapP,
										const scChar*	sysScrapPtr,
										TypeSpec		ts )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSCR_SysToCon" ) );

	try {
		scrapP = scStream::ReadMemText( ts, sysScrapPtr );
	}
	IGNORE_RERAISE;
	
	ExitMonitor( scString( "SCSCR_SysToCon" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_Iter( scSelection*	select,
									SubstituteFunc	func,
									scRedispList*	damage )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_Iter" ) );

	try {
		select->Iter( func, damage );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_Iter" ) );
	return stat;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_Iter( scStream*		stream,
									SubstituteFunc	func,
									scRedispList*	damage )

{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_Iter" ) );

	try {
		stream->Iter( func, damage ); 
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSTR_Iter" ) );
	return stat;
	
}

/* ==================================================================== */
// deprecated

status scIMPL_EXPORT	SCSTR_Search( scStream*			stream,
									  const UCS2*		findString,
									  SubstituteFunc	func,
									  scRedispList*		damage )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_Search" ) );

	try {
		throw( scERRnotImplemented );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSTR_Search" ) );	
	return scSuccess;
}

/* ==================================================================== */
// deprecated

status scIMPL_EXPORT	SCSEL_FindString( scSelection*	select,
										  const UCS2*	findString )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_FindString" ) );

	try {
		throw( scERRnotImplemented );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_FindString" ) );	
	return scSuccess;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_GetStream( const scSelection* selection,
										 scStream*&			stream, 
										 TypeSpec&			ts )

{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_GetStream" ) );

	try {
		stream	= selection->GetStream();
		ts		= selection->GetSpecAtStart();
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_GetStream" ) );	
	return scSuccess;
}

/* ==================================================================== */

status scIMPL_EXPORT	SCSTR_NthParaSelect( scStream*		streamID, 
											 long			nthPara, 
											 scSelection*	select )

{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSTR_NthParaSelect" ) );

	try {
		select->NthPara( streamID, nthPara );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSTR_NthParaSelect" ) );
	return stat;
}

/* ==================================================================== */

#ifdef _RUBI_SUPPORT

status scIMPL_EXPORT	SCSEL_GetAnnotation( scSelection*	select,
											 int			nth, 
											 scAnnotation&	annotation )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_GetAnnotation" ) );

	try {
		select->GetAnnotation( nth, annotation );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_GetAnnotation" ) );
	return stat;
}						 

/* ==================================================================== */

status scIMPL_EXPORT	SCSEL_ApplyAnnotation( scSelection*			select, 
											   const scAnnotation&	annotation, 
											   scRedispList*		redisplayListH )
{
	status stat = scSuccess;
	EnterMonitor( scString( "SCSEL_ApplyAnnotation" ) );

	try {
		select->ApplyAnnotation( annotation, redisplayListH );
	}
	IGNORE_RERAISE;

	ExitMonitor( scString( "SCSEL_ApplyAnnotation" ) );
	return stat;

}						  

#endif

/* ==================================================================== */
/* ==================================================================== */
/* ==================================================================== */

void scIMPL_EXPORT		SCCOL_InvertExtents( scColumn*		col,
										 	 HiliteFuncPtr	func,
											 APPDrwCtx		drawCtx )
{
	status stat = scSuccess;
	try {
		col->InvertExtents( func, drawCtx );
	}
	IGNORE_RERAISE;
}

/* ==================================================================== */

#if SCDEBUG > 1

long scIMPL_EXPORT		SCCOL_DebugSize( scColumn* col )
{
	return sizeof( scColumn ) + ( col->GetLinecount( ) * sizeof( scTextline ) );
}

/* ==================================================================== */

long scIMPL_EXPORT		SCSTR_DebugSize( scStream* stream )
{
	return stream->STRDebugSize(  );
}

/* ==================================================================== */

void scIMPL_EXPORT		SCCOL_InvertRegion( scColumn*		col,
										    HiliteFuncPtr	func,
										    APPDrwCtx		drawCtx )
{
	status stat = scSuccess;
	try {
		scFlowDir fd( col->GetFlowdir( ) );
		if ( col->GetRgn() ) 
			RGNInvertSlivers( col->GetRgn(), drawCtx, func, col->GetSize(), fd.IsVertical() );
	}
	IGNORE_RERAISE;
}

/* ==================================================================== */

void scIMPL_EXPORT		SCDebugColumnList( void )
{
	scColumn* col;

	SCDebugTrace( 0, scString( "Toolbox Column list start\n" ) );
	
	for ( col = scColumn::GetBaseContextList( ); col; col = col->GetContext() ) {
		SCDebugTrace( 0, scString( "\tcol 0x%08x appname 0x%08x\n" ), col, col->GetAPPName() );
	}
	SCDebugTrace( 0, scString( "Toolbox Column list end\n" ) );	
	
}

/* ==================================================================== */

void scIMPL_EXPORT	SCDebugColumn( scColumn*	col,
								   int		contentLevel )
{
	SCDebugTrace( 0, scString( "Column 0x%08x appname 0x%08x %d %d %s\n" ), 
					col, col->GetAPPName(),
					col->Width(), col->Depth(),
					col->GetVertFlex() ? "VFLEX" : "noflex" );

	if ( contentLevel ) {
		scContUnit* p;
		
		for ( p = col->GetStream(); p; p = p->GetNext() )
			SCDebugTrace( 0, scString( "\tpara 0x%08x\n" ), p );
	}
}

/* ==================================================================== */

void scIMPL_EXPORT SCDebugParaSpecs( scSelection* sel )
{
#if 1
	scSelection sorted( *sel );

	sorted.Sort();
	
	scContUnit* cu = sorted.GetMark().fPara;
	for ( ; cu && cu != sorted.GetPoint().fPara->GetNext(); cu = cu->GetNext() )
		cu->DebugParaSpecs();
#else
	if ( sel->IsSliverCursor() )
		sel->GetMark().fPara->DebugParaSpecs();
#endif
}

/* ==================================================================== */

void scIMPL_EXPORT SCSTR_Debug( scStream* str )
{
	str->STRDbgPrintInfo( );
}

/* ==================================================================== */

#endif /* DEBUG */

/* ==================================================================== */
/* ==================================================================== */
/* ==================================================================== */
