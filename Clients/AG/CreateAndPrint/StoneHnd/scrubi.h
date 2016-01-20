/*=================================================================
Contains:	rubi data
=================================================================*/

#pragma once

#include "scmemarr.h"

struct scRubiData
{
	UCS2		fCh[18];			// the rubi characters, NULL terminated
	long		fStartOffset;		// start offset in stream
	long		fEndOffset; 		// end offset in stream
	scMuPoint	fOrg;				// drawing origin
	MicroPoint	fLetterSpace;		// letterspace for justification
	scXRect 	fExtents;			// extents of chars
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
	BOOL	IsRubiData( long );
	BOOL	IsRubiData( long, long );
	
			// get the nth rubi data that occur between the 
			// specified offset setting its position and the rubidata
	BOOL	GetRubiAt( scRubiData&, long );
	BOOL	GetNthRubi( int&, scRubiData&, int, long, long );
			
			// as we edit text change the annotations
	void	BumpRubiData( long, long );
	
			// place the rubiarray into the existing rubi array at the
			// indicated offset with it covering the number of chars
			// indicated
	void	Paste( const scRubiArray&, long offset, int size );
	
			// apply the style to the rubidata found within the bounds
	void	ApplyStyle( long start, long end, TypeSpec ts );
	
			// add rubi data			
	BOOL	AddRubiData( scRubiData& );
	
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
