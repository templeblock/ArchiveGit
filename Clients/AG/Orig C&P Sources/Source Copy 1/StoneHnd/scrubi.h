/*=================================================================
 
	File:		crubi.h

	$Header: /Projects/Toolbox/ct/scrubi.h 2     5/30/97 8:45a Wmanis $

	Contains:	rubi data

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

=================================================================*/

#ifndef _H_CRUBI
#define _H_CRUBI

#include "scmemarr.h"


struct scRubiData {
	UCS2		fCh[18];			// the rubi characters, NULL terminated
	long		fStartOffset;		// start offset in stream
	long		fEndOffset;			// end offset in stream
	scMuPoint	fOrg;				// drawing origin
	MicroPoint	fLetterSpace;		// letterspace for justification
	scXRect		fExtents;			// extents of chars
	TypeSpec	fRubiSpec;			// spec of rubi
	
			scRubiData(){}
			scRubiData( const UCS2 *, long, long, TypeSpec );
	
	void	TransOffsets( long );
	
	void	Read( APPCtxPtr, IOFuncPtr );
	void	PtrRestore( void );	
	void	Write( APPCtxPtr, IOFuncPtr );
};


/* ==================================================================== */

class scRubiArray : public scMemArray {
public:
			scRubiArray( );
			~scRubiArray();

			// is there an annotation at this location
	Bool	IsRubiData( long );
	Bool	IsRubiData( long, long );
	
			// get the nth rubi data that occur between the 
			// specified offset setting its position and the rubidata
	Bool	GetRubiAt( scRubiData&, long );
	Bool	GetNthRubi( int&, scRubiData&, int, long, long );
			
			// as we edit text change the annotations
	void	BumpRubiData( long, long );
	
			// place the rubiarray into the existing rubi array at the
			// indicated offset with it covering the number of chars
			// indicated
	void	Paste( const scRubiArray&, long offset, int size );
	
			// apply the style to the rubidata found within the bounds
	void	ApplyStyle( long start, long end, TypeSpec ts );
	
			// add rubi data			
	Bool	AddRubiData( scRubiData& );
	
			// delete rubi data
	void	DeleteRubiData( long );

			// delete rubi data between the indicated offsets
	void	DeleteRubiData( long, long );
		
	scRubiArray&	operator=( const scRubiArray& );
	
	long	ExternalSize( void );
	void	Read( APPCtxPtr, IOFuncPtr, int numread );
	void	PtrRestore( void );	
	void	Write( APPCtxPtr, IOFuncPtr );
};

#endif
