/****************************************************************************

	File:		SCSTCACH.H

	$Header: /Projects/Toolbox/ct/SCSTCACH.H 3     5/30/97 8:45a Wmanis $	

	Contains:	Style cache code.
	
	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************************/
  
#ifndef _H_SCSTCACH
#define _H_SCSTCACH

#include "scstyle.h"  
#include "scmem.h"


inline MicroPoint SCRLUCompMP( MicroPoint size, RLU rlu )
	{ 
		return scRoundMP( (REAL)size * rlu / scBaseRLUsystem ); 
	}

inline GlyphSize SCRLUCompGS( GlyphSize size, RLU rlu )
	{ 
		return scRoundGS( (REAL)size * rlu / scBaseRLUsystem ); 
	}


class scCachedStyle : public scStyle {

public:
	static void				BuildCache( int entries );
	static void				DeleteCache( void );
	
	static scCachedStyle&	GetParaStyle( )
								{
									scAssert( cachedPara_ );
									return cachedParaStyle_;
								}
	static void				SetParaStyle( const scContUnit*, TypeSpec& ts );

								// given a spec, get its cached value,
								// loading the cache in necessary
								//
	static scCachedStyle&	GetCachedStyle( TypeSpec& ts );	
				
	static scCachedStyle&	FindCachedStyle( TypeSpec& ts );


								// this simply returns the last cache we got
								// hold of, TEMPORARY
	static scCachedStyle&	GetCurrentCache( )
								{ 
									return fCachedStyles[fLast]; 
								}

								// invalidate the spec, if NULL all specs
								// will be invalidated
								//
	static void				StyleInvalidateCache( TypeSpec& ts );

								// set and get the flowdir for the cached values
								//
	static void				SetFlowdir( const scFlowDir& fd );
	const scFlowDir&		GetFlowdir( void ) const
								{ 
									return fFlowDir; 
								}
	
private:
							// the cached values
	static scCachedStyle*	fCachedStyles;
	static int				fEntries;			// number of entries in the cache
	static int				fLast;

	
	static scCachedStyle		cachedParaStyle_;
	static const scContUnit*	cachedPara_;

							// a timer that increments only on loading a cache
	static long				fCacheTime;


							// for now we are cacheing only on a flow basis
	static scFlowDir		fFlowDir;

							// get the oldest cached value
	static int				GetOldestIndex( void );
	
public:
					scCachedStyle() :
						fTimeStamp( 0 ),
						fRunAroundBorder( 0 ),
						fPtConv( 0 ),
						fSetConv( 0 )
							{
								SCmemset( fWidths, 0, sizeof( fWidths ) );
								fCursorY1 = 0, fCursorY2 = 0;
							}
						
					~scCachedStyle()
							{
							}


	void			SetSpec( TypeSpec& ts )
						{ 
							fSpec = ts; 
						}
	TypeSpec&		GetSpec( void )
						{
							return fSpec; 
						}

						// return the nominal escapement for this glpyh
	GlyphSize		GetEscapement( UCS2 ch );
	
	GlyphSize		GetLeftHangValue( UCS2 ch )
						{ return scRoundGS( (REAL)GetEscapement( ch ) * GetLeftHang() / 10000 ); }
						
	GlyphSize		GetRightHangValue( UCS2 ch )
						{ return scRoundGS( (REAL)GetEscapement( ch ) * GetRightHang() / 10000 );	}
						
	GlyphSize		GetKernValue( UCS2, UCS2 );

	const scXRect&	GetLogicalExtents( void ) const	{ return fLogicalExtents; }
	
	const scXRect&	GetInkExtents( void ) const		{ return fInkExtents; }

	eFntBaseline	GetBaselineType( void ) const
						{ if ( fFlowDir.IsVertical() )	return GetVertBaseline();
						  else return GetHorzBaseline(); }


	MicroPoint		GetRunAroundBorder( void ) const	{ return fRunAroundBorder; }
	void			SetRunAroundBorder( MicroPoint b )	{ fRunAroundBorder = b; }


	MicroPoint		GetCursorY1( void ) const	{ return fCursorY1; }
	MicroPoint		GetCursorX1( void ) const	{ return fCursorX1; }
	MicroPoint		GetCursorY2( void ) const	{ return fCursorY2; }
	MicroPoint		GetCursorX2( void ) const	{ return fCursorX2; }


	void			GetParaBreak( scParaColBreak& pb )
						{
							pb.Set( GetLinesBefore(), GetLinesAfter(), GetNoBreak(), GetKeepWithNext() );
						}

	static 	MicroPoint GetParaSpace( scContUnit*, scContUnit* );
	static 	MicroPoint GetMaxParaSpace( scContUnit*, scContUnit* );

	MicroPoint		HorzCompute( RLU rlu )	{ return scRoundMP( fSetConv * rlu ); }
	MicroPoint		VertCompute( RLU rlu )	{ return scRoundMP( fPtConv * rlu ); }
	
	eFntBaseline	GetOperativeBaseline( const scFlowDir& fd )
						{ 
							return fd.IsVertical() ? GetVertBaseline() : GetHorzBaseline();
						}
private:

	void			Init( TypeSpec& ts );
	void			InitFlowdir( const scFlowDir& fd );
	void			InitWidths( void );

	void			ComputeExtentsnCursor( void );
	
	long			fTimeStamp;
	
	TypeSpec		fSpec;


	MicroPoint		fRunAroundBorder;
	
	scXRect			fInkExtents;
	scXRect			fLogicalExtents;	

	REAL			fPtConv;
	REAL			fSetConv;

	MicroPoint		fWidths[256];

	union {
		MicroPoint	fCursorY1;
		MicroPoint	fCursorX1;
	};
	union {
		MicroPoint	fCursorY2;
		MicroPoint	fCursorX2;
	};
};

#define gfmS	scCachedStyle::GetCurrentCache( )

inline scCachedStyle& cachedTS( TypeSpec& ts ) 
{
	return scCachedStyle::GetCachedStyle( ts );
}

inline scCachedStyle& currentTS( )
{
	return scCachedStyle::GetCurrentCache( );
}

#endif
