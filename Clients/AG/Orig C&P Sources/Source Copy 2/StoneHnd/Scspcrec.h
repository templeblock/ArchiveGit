/*=================================================================

	File:		SCSPECRU.H

	$Header: /Projects/Toolbox/ct/SCSPCREC.H 2	   5/30/97 8:45a Wmanis $

	Contains:	scSpecRecord - spec plus content unit offset

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

=================================================================*/

#ifndef _H_SCSPCREC
#define _H_SCSPCREC

#include "sctypes.h"
#include "scarray.h"

#include <limits.h>

/*======================================================================*/
class scSpecRecord;


							
class scSpecRun : public scSizeableArrayD<scSpecRecord> {
public:
					scSpecRun();
					scSpecRun( const scSpecRun& );
					scSpecRun( TypeSpec firstspec );
					~scSpecRun();

		// get the last valid spec in the run
	TypeSpec	GetFirstSpec( void );
	TypeSpec	SpecAtOffset( int32 );	
	TypeSpec	GetLastSpec( void );

		// get the offset of the last valid spec
	int32			GetLastOffset( void );

	int 			Includes( TypeSpec );

	void			AppendSpec( TypeSpec, int32 );
	void			ApplySpec( TypeSpec, int32, int32 );
	void			Clear( int32, int32  );
	void			Copy( scSpecRun&, int32, int32 ) const;
	void			InsertRun( int32 offset, int32 len, const scSpecRun& );
	
	void			BumpOffset( int32, int32 );
	void			SetContentSize( int32 );
	
		// return the record of the spec rec at an offset
	scSpecRecord&		SpecRecAtOffset( int32 );
	const scSpecRecord& SpecRecAtOffset( int32 ) const;

	scSpecRun&		operator=( const scSpecRun& );
	int 			operator==(const scSpecRun& ) const;
	int 			operator!=(const scSpecRun& ) const;

	int 			isTerminated() const;

#ifdef SCDEBUG
	void			DebugRun( const char* ) const;
	void			PrintRun( const char* ) const;
#else
	void			DebugRun( const char* ) const {}
	void			PrintRun( const char* ) const {}
#endif

	void			Read( APPCtxPtr, IOFuncPtr );
	void			Write( APPCtxPtr, IOFuncPtr );
	void			RestorePointers( void );
	
	long			ExternalSize( void ) const;
	int 			indexAtOffset( int32 ) const;

	static int32	terminate_;
	
private:

	void			insertSpecRec( const scSpecRecord&, int32 );
	int32			removeOffsets( int32, int32 );
	void			removeIndicies( int32, int32 );
	void			bumpIndicies( int32, int32 );
	
	void			backwardCleanUpRun( int );

	void			consolidate( int outer = 1 );

	void			terminate();

};

class scSpecRecord {
friend class scSpecRun;
public:
				scSpecRecord() :
						spec_(0), 
						offset_(scSpecRun::terminate_)
						{
						}
	
				scSpecRecord( TypeSpec& ts, int32 offset ) :
							spec_(ts), offset_(offset)
						{
						}
				~scSpecRecord()
						{
						}
	void		set( TypeSpec& ts, int32 offset )
					{
						spec_	= ts;
						offset_ = offset;
					}
	int 		isTerminator( void ) const
					{
						return offset_ == scSpecRun::terminate_;
					}
	void		bumpOffset( int32 bump )
					{
						offset_ += (offset_!=scSpecRun::terminate_ ? bump : 0);
						offset_ = ( offset_ < 0 ) ? 0 : offset_;
					}

	int 		operator==(const scSpecRecord& sr ) const
					{
						return spec_==sr.spec_ && offset_==sr.offset_;
					}
	int 		operator!=(const scSpecRecord& sr ) const
					{
						return spec_!=sr.spec_ || offset_!=sr.offset_;		
					}

	TypeSpec	spec() const
					{
						return spec_;
					}
	TypeSpec	spec()
					{
						return spec_;
					}
	int32			offset() const
					{
						return offset_;
					}
	int32&			offset()
					{
						return offset_;
					}


	void			restorePointer();
	
private:
	TypeSpec	spec_;		// the spec to apply
	int32		offset_;	// character offset to start appling spec
};


/*======================================================================*/

#endif /* _H_SCSPECRU */
