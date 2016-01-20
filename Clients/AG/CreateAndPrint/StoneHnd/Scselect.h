/***************************************************************************
Contains:	scSelection definition
****************************************************************************/

#pragma once

#include "sccharex.h"
#include "scexcept.h"
#include "sctbobj.h"

class scColumn;
class scContUnit;
class scTextline;
class scAnnotation;
class scTypeSpecList;
class scSpecLocList;
class scRedispList;
class stTextImportExport;
class clField;

enum eSelectMovement {
	eBeforeSelect = 1,
	eEqualSelect,
	eAfterSelect 
};

class TextMarker {
public:
		TextMarker() :
			fCol( 0 ),
			fPara( 0 ),
			fTxl( 0 ),
			fColCount( 0 ),
			fParaCount( 0 ),
			fLineCount( 0 ),
			fOffset( 0 ),
			fHLoc( 0 ),
			fSelMaxX( 0 ),
			fEndOfLine( false ) {}

		TextMarker( scContUnit* cu, int32 poffset, int32 offset ) :
			fCol( 0 ),
			fPara( cu ),
			fTxl( 0 ),
			fColCount( 0 ),
			fParaCount( poffset ),
			fLineCount( 0 ),
			fOffset( offset ),
			fHLoc( 0 ),
			fSelMaxX( 0 ),
			fEndOfLine( false ) {}

		TextMarker( const TextMarker& tm )
			{
				fCol		= tm.fCol;
				fPara		= tm.fPara;
				fTxl		= tm.fTxl;
				fColCount	= tm.fColCount;
				fParaCount	= tm.fParaCount;
				fLineCount	= tm.fLineCount;
				fOffset 	= tm.fOffset;
				fHLoc		= tm.fHLoc;
				fSelMaxX	= tm.fSelMaxX;
				fEndOfLine	= tm.fEndOfLine;
			}
							
	scColumn*		fCol;
	scContUnit* 	fPara;
	scTextline* 	fTxl;
	long			fColCount;
	long			fParaCount;
	long			fLineCount;
	long			fOffset;
	MicroPoint		fHLoc;
	MicroPoint		fSelMaxX;
	BOOL			fEndOfLine;

	TextMarker& 	operator=( const TextMarker& tm )
							 {
								fCol		= tm.fCol;
								fPara		= tm.fPara;
								fTxl		= tm.fTxl;
								fColCount	= tm.fColCount;
								fParaCount	= tm.fParaCount;
								fLineCount	= tm.fLineCount;
								fOffset 	= tm.fOffset;
								fHLoc		= tm.fHLoc;
								fSelMaxX	= tm.fSelMaxX;
								fEndOfLine	= tm.fEndOfLine;
								return *this;
							 }

	int 			operator>( const TextMarker& ) const;
	int 			operator<( const TextMarker& ) const;
	int 			operator>=( const TextMarker& ) const;
	int 			operator<=( const TextMarker& ) const;
	int 			operator==( const TextMarker& ) const;
	int 			operator!=( const TextMarker& ) const;

	void			Zero( int, BOOL );
	void			Invalidate( void )
							 {
								fCol		= 0;
								fPara		= 0;
								fTxl		= 0;
								fColCount	= -1;
								fParaCount	= -1;
								fLineCount	= -1;
								fOffset 	= -1;
								fHLoc		= kInvalMP;
								fSelMaxX	= kInvalMP;
								fEndOfLine	= 0;
							 }
	

	BOOL			Update( eContentMovement, scColumn* flowset );


	void			UpdateInfo( int setMax );

	int 			SelectPrevCharInPara( void );
	int 			SelectNextCharInPara( void );
	
	int 			SelectPrevChar( void );
	int 			SelectNextChar( void );

	int 			SelectPrevLine( void );
	int 			SelectNextLine( void );

	int 			SelectStartLine( void );
	int 			SelectEndLine( void );

	BOOL			SelectPrevWord( void  );
	BOOL			SelectNextWord( void );

	BOOL			SelectPrevSpellWord( void );
	BOOL			SelectNextSpellWord( void );

	int 			SelectStartWord( void );
	int 			SelectEndWord( void );

	int 			SelectStartSpellWord( int eleminateLeadingSpaces = 0 );
	int 			SelectEndSpellWord( void );

	int 			SelectStartPara( void );
	int 			SelectEndPara( void );

	int 			SelectPrevPara( void );
	int 			SelectNextPara( void );
	
	int 			SelectStartColumn( void );
	int 			SelectEndColumn( void );

	int 			SelectStartStream();
	int 			SelectEndStream();
	
#if SCDEBUG > 1
	void			DbgPrintInfo( int debugLevel = 0 ) const;
	void			scAssertValid( void );
#else
	void			scAssertValid( void ){} 
#endif
};

class scSelection : public scObject {
public:
	enum Scope {
		inContUnit,
		inStream,
		inLine,
		inColumn
	};
					scSelection( scColumn* flowset = 0 ) :
						fFlowset( flowset ){}
						
					scSelection( const scSelection& sel )
						{
							fFlowset	= sel.fFlowset;
							fMark		= sel.fMark;
							fPoint		= sel.fPoint;
						}

	void			NthPara( scStream* stream, long nthPara );
	void			SetParaSelection( scContUnit*	p,
									  long			start,
									  long			end );


	void			GetContUnits( scContUnit*& mark, scContUnit*& point ) const;
	
	void			CutText( scScrapPtr&, scRedispList* );
	void			ClearText( scRedispList*, BOOL );
	void			CopyText( scScrapPtr& );
	void			PasteText( const scStream*, TypeSpec, scRedispList* );

	void			CopyAPPText( stTextImportExport& );
	void			PasteAPPText( stTextImportExport&, scRedispList* ); 

	void			InsertField( const clField&, 
								 TypeSpec&,
								 scRedispList* );	
	
	void			SetStyle( TypeSpec, scRedispList* );
	void			GetTSList( scTypeSpecList& );
	void			GetCharSpecList( scSpecLocList& );
	void			GetCharSpecListEx( scSpecLocList& );
	void			GetParaSpecList( scSpecLocList& );
	void			GetParaSpecList( scTypeSpecList& );

	
	void			SetFlowset( scColumn* col ) 		{ fFlowset = col; }
	scColumn*		GetFlowset( void ) const			{ return fFlowset; }

	void			SetMark( const TextMarker& mark )	{ fMark = mark; }
	TextMarker& 	GetMark( void ) 					{ return fMark; }

	void			SetPoint( const TextMarker& point ) { fPoint = point; }
	TextMarker& 	GetPoint( void )					{ return fPoint; }


						// is the selection just a caret (i.e. no content selected ) 
	BOOL			IsSliverCursor( void ) const		{ return fPoint == fMark; }
	int32			ContentSize() const;
	
						// we are freeing this paragraph, check to see if we need
						// to invalidate the selection
	void			CheckFreePara( scContUnit* );

	void			UpdateSelection( void );
	
	
						// sort so that the mark occurs before the point
	void			Sort( void );
	
	void			ValidateHilite( HiliteFuncPtr ) const;
	BOOL			ValidateSelection( scSelection& ) const;

	BOOL			ChangeSelection( long	tmMove,
									 BOOL	arrowKey );
	
	void			CorrectSelection( scContUnit*	para1,
									  long		,
									  scContUnit*	para2,
									  long			offset2 );

	void			InteractiveHilite( scSelection&, HiliteFuncPtr );
	
	void			MarkValidatedSelection( const scLayBits& mark );
	
	void			LineHilite( HiliteFuncPtr );

	status			Iter( SubstituteFunc func, scRedispList* );
	void			KeyArray( short, scKeyRecord*, bool* pbOverflow, scRedispList* );
	void			TextTrans( eChTranType, int, scRedispList* );

#ifdef _RUBI_SUPPORT
	BOOL			GetAnnotation( int nth, scAnnotation& );
	void			ApplyAnnotation( const scAnnotation& , scRedispList* );
#endif
	
	scStream*		GetStream( void ) const;

	TypeSpec		GetSpecAtStart( void ) const;

						// this will report the position of the point
						// in relationship to the mark
	eSelectMovement RangeMovement( void ) const;


	void			Decompose( scStreamLocation&, scStreamLocation& );
	void			Decompose2( scStreamLocation&, scStreamLocation& ); 
	void			Restore( const scStreamLocation*,
							 const scStreamLocation*,
							 const scStream*,
							 BOOL );

	scSelection&	operator=( const scSelection& sel )
						{
							fFlowset	= sel.fFlowset;
							fMark		= sel.fMark;
							fPoint		= sel.fPoint;
							return *this;
						}

	int 			operator==( const scSelection& sel ) const
						{
							return (
								fMark.fPara 		== sel.fMark.fPara			&&
								fMark.fParaCount	== sel.fMark.fParaCount 	&&
								fMark.fOffset		== sel.fMark.fOffset		&&
								fMark.fEndOfLine	== sel.fMark.fEndOfLine 	&&
								fPoint.fPara		== sel.fPoint.fPara 		&&
								fPoint.fParaCount	== sel.fPoint.fParaCount	&&
								fPoint.fOffset		== sel.fPoint.fOffset		&&
								fPoint.fEndOfLine	== sel.fPoint.fEndOfLine
							);
						}

						// invalidate the selection
	void			Invalidate( void );
	
	void			MoveSelect( eSelectMove );
	void			Extend( eSelectMove );	
	void			WordSelect( void );
	void			LineSelect( void );
	void			ParaSelect( void );
	void			ColumnSelect( void );
	void			AllSelect( void );

						// mark all the layout objects associated with
						// the selection
	void			MarkLayoutBits( const scLayBits& );
	
#if SCDEBUG > 1
	void			DbgPrintInfo( int debugLevel = 0 ) const;
	void			scAssertValid( void );
#else
	void			scAssertValid( void ){} 
#endif

	int 			PrevWord( Scope scope = inStream );
	int 			NextWord( Scope scope = inStream  );
	int 			PrevSpellWord( Scope scope = inStream );
	int 			NextSpellWord( Scope scope = inStream );

	int 			StartWord( void );
	int 			EndWord( void );

	int 			PrevLine( void );
	int 			NextLine( void );

	int 			PrevEntireLine( void );
	int 			NextEntireLine( void );

	int 			StartLine( void );
	int 			EndLine( void );

	int 			PrevColumn( void );
	int 			NextColumn( void );

	int 			EndColumn( void );
	int 			StartColumn( void );

	int 			BeginPara( void );
	int 			EndPara( void );
	
	int 			Para( eSelectMove moveSelect );
	
	scColumn*		fFlowset;
	TextMarker		fMark;
	TextMarker		fPoint;

protected:

};
