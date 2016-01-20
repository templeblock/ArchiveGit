/****************************************************************************
Contains:	scStream definition.
****************************************************************************/

#pragma once

#include "scparagr.h"

class stTextImportExport;
class scRedispList;
class scTypeSpecList;
class scSpecLocList;

// the ticks in the following 2 functions describes how long
// the operation should go before calling it quits - ticks
// is completely system dependent - so you are on your own
// in terms of the recommended value
extern scTicks scReformatTimeSlice;
extern scTicks scInteractiveTimeSlice;

eRefEvent	STRReformat( scColumn*, 
						 scContUnit*,
						 scTicks,
						 scRedispList* );

eRefEvent	STRIncReformat( scContUnit*,
							scTicks,
							scRedispList* );

class scStream : public scContUnit
{
public:
	void		ResetStream( scStream* );

	// does the stream contain the following paragraph */
	BOOL		Contains( scContUnit* );

	scContUnit* First( void ) const;
	scContUnit* Last( void ) const;

	BOOL		Compare( const scStream* ) const;
	void		CopyAPPText( stTextImportExport& );
	void		STRMark( const scLogBits& );
	scContUnit* NthPara( long );

	void		STRGetTSList( scTypeSpecList& );
	void		GetParaTSList( scTypeSpecList& );
	void		GetCharSpecList( scSpecLocList& );
	void		SetCharSpecList( const scSpecLocList&, scRedispList* );
	void		SetParaSpecList( const scSpecLocList&, scRedispList* );
	void		GetParaSpecList( scSpecLocList& );
	
	void		ChCount( long& );

	BOOL		FindColumn( scColumn *& );

	// remove all references to layout from the stream
	void		STRDeformat( void );
	void		RemoveEmptyTrailingParas( scColumn* );	
	scContUnit* LastVisiblePara( void );
	scTextline* LastVisibleLine( void );
	scStream*	Split( scContUnit* );

	status		Iter( SubstituteFunc	func,
					  scRedispList* 	damage );

	void		Search( const UCS2* findString,
						SubstituteFunc	func,
						scRedispList*	damage );

	void		DeepEnumerate( long& ); 
	void		STRFree( void );

	void		STRCopy( scStream*& ) const;
	void		STRWriteMemText( BOOL, SystemMemoryObject& );
	void		STRWriteTextFile( APPCtxPtr, IOFuncPtr, BOOL );
	
	void		STRToFile( APPCtxPtr, IOFuncPtr );
	void		STRWriteToFile( APPCtxPtr, IOFuncPtr );

	static scStream*	STRFromFile( scSet*, APPCtxPtr, IOFuncPtr );
	
	void		STRRestorePointers( scSet* );

	void		STRZeroEnumeration( void ); 	

	static scStream*	ReadAPPText( stTextImportExport& );
	static scStream*	ReadTextFile( TypeSpec, APPCtxPtr, IOFuncPtr, long );
	static scStream*	ReadMemText( TypeSpec, const scChar* );
	
#if SCDEBUG > 1
	long		STRDebugSize( void );
	void		STRDbgPrintInfo( void );
#endif	
};
