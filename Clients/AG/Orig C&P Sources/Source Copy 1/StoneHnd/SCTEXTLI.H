/****************************************************************************

	File:		SCTEXTLI.H


	$Header: /Projects/Toolbox/ct/SCTEXTLI.H 2     5/30/97 8:45a Wmanis $

	Contains:	scTextline definition.

	Written by: Manis


	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

***************************************************************************/

#ifndef _H_SCTEXTLI
#define _H_SCTEXTLI

#ifdef SCMACINTOSH
#pragma once
#endif

#include "sctbobj.h"
#include "screfdat.h"
#include "sccolumn.h"
		 
/* THE TEXTLINE OBJECT */

class scLINERefData;
class scLEADRefData;
class scTypeSpecList;
class TextMarker;
class scDrawLine;
class scStreamChangeInfo;

class scTextline : public scTBObj {
	scDECLARE_RTTI;
	
	friend scDrawLine;		
public:
						// use this to allocate new lines where the line
						// has been overridden on the outside.
	static scTextline* Allocate( scContUnit*	p = 0,
								 scColumn*		col = 0,
								 scTextline*	prev = 0 );	
	
					scTextline( scContUnit* p = 0, scColumn* col = 0, scTextline* prev = 0 );
					~scTextline();

	void			Delete( scXRect& damage );

						// the last two arguments, paragraph and offset,
						// allow us to test accurately for mono-spaced
						// characters that are inserted. we test anything
						// after the offset and can tell if the line
						// has changed even though none of its visusal
						// characteristics has change save the glyph indicies
	Bool			Compare( scTextline*, const scStreamChangeInfo& );
	Bool			TestOffsets( scTextline*, const scStreamChangeInfo& );
					
	void			Set( short,				// line number
						 eBreakType,		// breaktype
						 scLINERefData& );	// linedata


	void			InitForReuse( scContUnit* p )
						{
							fPara			= p;
							fBits			= 0;
							fLineCount 		= 0;
							fVJOffset		= 0;
							fMaxLeadSpec.clear();
							fMaxLead		= 0;
						}

	Bool			IsHyphenated( void ) const;


	void			GetTSList( scTypeSpecList& );
	Bool			ContainTS( TypeSpec );
	void			GetLineInfo( scLineInfo& ) const;
	
	TypeSpec		SpecAtStart( void ) const;

	short			GetLinecount( void ) const
						{
							return fLineCount;
						}
	
	long			GetStartOffset( void ) const
						{
							return fStartOffset;
						}
	long			GetEndOffset( void ) const
						{
							return fEndOffset;
						}
	long			GetCharCount( void ) const
						{
							return fEndOffset - fStartOffset;
						}
	
	void			SetStartOffset( long st )
						{
							fStartOffset = st;
						}
	void			SetEndOffset( long end )
						{
							fEndOffset = end;
						}	
	void			SetOffsets( long st, long end )
						{
							fStartOffset = st, fEndOffset = end;
						}

	Bool			OffsetOnLine( long offset ) const
						{
							return offset >= fStartOffset && offset <= fEndOffset;
						}
			
	MicroPoint		GetBaseline( void ) const;
	
						// returns true if the selection is a sliver cursor
	Bool			SortMarkers( const TextMarker*&	startTM,
								 MicroPoint&		startLoc, 
								 int32&				startOffset,
								 const TextMarker*&	endTM,
								 MicroPoint&		endLoc, 
								 int32&				endOffset,
								 Bool				isVertical );
	
	scMuPoint&		Locate( long&, scMuPoint&, eContentMovement );

	scContUnit*		GetPara( void ) const
						{
							return fPara;
						}
	void			SetPara( scContUnit* cu )
						{
							fPara = cu;
						}

	Bool			IsLastLinePara( void ) const;

	UCS2			CharAtStart( void ) const;	
	UCS2			CharAtEnd( void ) const;
	
	const scFlowDir&	GetFlowdir( void ) const;


	MicroPoint		MaxLead( TypeSpec& );
	
	MicroPoint		ParaLead( scLEADRefData&, const scFlowDir& );
	
	scXRect& 		QueryExtents( scXRect&, int dcextended = 0 ) const;
	void			SetInkExtents( const scXRect& xrect )
					{
						fInkExtents = xrect;
					}

					// translates all the line data
	void			Translate( const scMuPoint& );

	void			SetVJ( MicroPoint vj )
						{
							fVJOffset = vj;
						}
					// remove the effects of vert justifying
	void			RemoveVJ( void )
						{
							fVJOffset = 0;
						}

	void			Reposition( MicroPoint );

	void			Hilite( const TextMarker*, MicroPoint,
							const TextMarker*, MicroPoint, 
							APPDrwCtx, HiliteFuncPtr,
							const scSelection& selection );

	void			Draw( APPDrwCtx, 
						  const scFlowDir&,
						  const scMuPoint& translation );

	void			InvertExtents( HiliteFuncPtr, APPDrwCtx ) const;

	REAL			Select( scMuPoint&, 
							long&, 
							const scMuPoint&, 
							eContentMovement,
							Bool& );
	
		//////////////////////////////
		//////////////////////////////

	scTextline&		operator=( const scTextline& );
	
	scTextline*		GetPrev( void ) const
						{
							return (scTextline*)Prev();
						}	
	scTextline*		GetNext( void ) const
						{
							return (scTextline*)Next();
						}

	scTextline*		GetPrevLogical( void ) const;
	scTextline*		GetNextLogical( void ) const;

					// mark the para for deletion
	void			MarkForDeletion( void );

	scColumn*		GetColumn( void ) const		
						{ 
							return fColumn; 
						}
	void			SetColumn( scColumn* col )			
						{ 
							fColumn = col; 
						}

	const scMuPoint&	GetOrigin( void ) const		
						{ 
							return fOrigin; 
						}


	MicroPoint		GetVJOffset( void ) const
						{
							return fVJOffset;
						}
	MicroPoint		GetLength( void ) const
						{
							return fLength;
						}
	MicroPoint		GetMeasure( void ) const
						{
							return fLength;
						}	
	MicroPoint		GetCursorY1( void ) const
						{
							return fCursorY1;
						}
	MicroPoint		GetCursorY2( void ) const
						{
							return fCursorY2;
						}

	TypeSpec		GetMaxLeadSpec( void ) const
						{
							return fMaxLeadSpec;
						}
	MicroPoint		GetMaxLead( void ) const
						{
							return fMaxLead;
						}
	short			GetLineCount( void ) const
						{
							return fLineCount;
						}

	MicroPoint		GetLSP( void ) const
						{
							return fLspAdjustment;
						}
	
	eTSJust			GetFlexLineAdjustment( void ) const
						{
							return (eTSJust)fLayBits.fLayAdjustment;
						}
	void			SetFlexLineAdjustment( eTSJust rag )
						{
							fLayBits.fLayAdjustment = rag;
						}

#if SCDEBUG > 1
	virtual void	scAssertValid( Bool recurse = true ) const;
	void			DbgPrintInfo( int debugLevel = 0 ) const;
#else
	virtual void	scAssertValid( Bool = true ) const{}
#endif

	int				operator==( const scTextline& tl ) const
						{
							return fPara		== tl.fPara			&&
								   fStartOffset	== tl.fStartOffset	&&
								   fEndOffset	== tl.fEndOffset	&&
								   fColumn		== tl.fColumn		&&
								   fOrigin		== tl.fOrigin		&&
								   fInkExtents	== tl.fInkExtents	&&
								   fVJOffset	== tl.fVJOffset		&&
								   fLength		== tl.fLength		&&
								   fCursorY1	== tl.fCursorY1		&&
								   fCursorY2	== tl.fCursorY2		&&
								   fMaxLeadSpec	== tl.fMaxLeadSpec	&&
								   fMaxLead		== tl.fMaxLead		&&
								   fLineCount	== tl.fLineCount	&&
								   fLspAdjustment == tl.fLspAdjustment;
						}
										   
									
		//////////////////////////////
		//////////////////////////////

protected:
	MicroPoint		TopHilite( void ) const;
	MicroPoint		BottomHilite( void ) const;
	
	MicroPoint		TopLead( void ) const;
	MicroPoint		BottomLead( void ) const;	
	
	Bool			DropCapHilite( APPDrwCtx, HiliteFuncPtr	) const;
	void			GetDropCapExtent( scXRect& ) const;

	scContUnit*		fPara; 				/* parent para				*/
 	long			fStartOffset;		/* hooks into parastream		*/
	long			fEndOffset;			/* hooks into parastream		*/

 	scColumn*		fColumn;			/* parent column			*/
	scMuPoint		fOrigin;			// origin of line in containter coordintates
	
	scXRect			fInkExtents;		/* ink extents of character		*/

	MicroPoint		fVJOffset;			/* baseline after VJ			*/	
	MicroPoint		fLength;			/* the length of this line		*/
	MicroPoint		fCursorY1;			/* extents for use by cursors	*/
	MicroPoint		fCursorY2;			/* extents for use by cursors	*/ 
	// the above three may be  combined into logicalextents

	TypeSpec		fMaxLeadSpec;		// reference spec
	MicroPoint		fMaxLead;			// max lead from previous line
	
	short			fLineCount;			/* line number in this para		*/

	GlyphSize		fLspAdjustment;		/* letterspace adjust for line	*/
	
};

		
#define LNNext( t ) 		( t->GetNext() )
#define LNPrev( t ) 		( t->GetPrev() )


/***************************************************************************/
/***************************************************************************/

scTextline*		LNInsertNew( scContUnit *, scColumn*, scTextline* );


void			LNSetNext( scTextline*, scTextline* );


void			LNPtrRestore( scTBObj*, scSet* );
void			LNAbort( scTBObj * );

Bool			LNFromFile( APPCtxPtr, 
							IOFuncPtr, 
							scSet*, 
							scTBObj&, scTBObj*& );





#endif /* _H_SCTEXTLI */
