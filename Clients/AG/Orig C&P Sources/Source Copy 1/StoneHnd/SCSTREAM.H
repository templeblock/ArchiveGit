/****************************************************************************

	File:		SCSTREAM.H


	$Header: /Projects/Toolbox/ct/SCSTREAM.H 2     5/30/97 8:45a Wmanis $
	
	Contains:	scStream definition.

	Written by:	Manis


	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


****************************************************************************/

#ifndef _H_SCSTREAM
#define _H_SCSTREAM

#ifdef SCMACINTOSH
#pragma once
#endif

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


class scStream : public scContUnit {
public:
	void		ResetStream( scStream* );

		// does the stream contain the following paragraph */
	Bool		Contains( scContUnit* );

	scContUnit* First( void ) const;
	scContUnit* Last( void ) const;

	Bool		Compare( const scStream* ) const;
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

	Bool		FindColumn( scColumn *& );

		// remove all references to layout from the stream
	void		STRDeformat( void );
	void		RemoveEmptyTrailingParas( scColumn* );	
	scContUnit*	LastVisiblePara( void );
	scTextline*	LastVisibleLine( void );
	scStream*	Split( scContUnit* );

	void		Iter( SubstituteFunc	func,
					  scRedispList*		damage );

	void		Search( const UCS2*	findString,
						SubstituteFunc	func,
						scRedispList*	damage );


	void		DeepEnumerate( long& );	
	void		STRFree( void );


	void	    STRCopy( scStream*& ) const;
	void		STRWriteMemText( Bool, SystemMemoryObject& );
	void	    STRWriteTextFile( APPCtxPtr, IOFuncPtr, Bool );
	
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







	

#endif /* _H_SCSTREAM */


