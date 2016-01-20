/****************************************************************************
Contains:	textline functions
****************************************************************************/

#include "sctextli.h"
#include "sccolumn.h"
#include "scstcach.h"
#include "scglobda.h"
#include "scpubobj.h"
#include "scmem.h"
#include "scparagr.h"
#include "scspcrec.h"
#include "sccallbk.h"
#include "scctype.h"

#ifdef _RUBI_SUPPORT
	#include "scrubi.h"
#endif

#define kMaxCharArray	200

#ifndef LETTERSPACE
	#define LETTERSPACE( ch ) ( (ch)->character != scWordSpace && ((ch)+1)->character != scWordSpace )
#endif

/* ==================================================================== */
// structure to handle different baseline definitions
struct CBaselinePositions {
	union {
		MicroPoint	fTop;
		MicroPoint	fLeft;
	};
	
	MicroPoint		fMiddle;
		
	MicroPoint		fRoman;
	
	union {
		MicroPoint	fBottom;
		MicroPoint	fRight;
	};
	
	void			Init( const scMuPoint&, MicroPoint, TypeSpec, const scFlowDir& );
};

void CBaselinePositions::Init( const scMuPoint& org,
							   MicroPoint		vjOffset,
							   TypeSpec 		spec,
							   const scFlowDir& fd )
{
	scAssert( spec != 0 );
	
	MicroPoint	ptsize = scCachedStyle::GetCachedStyle( spec ).GetPtSize();
	
	if ( fd.IsHorizontal() ) {
		fTop	= org.y - scRoundMP( (REAL)ptsize * RLU_BASEfmTop / scBaseRLUsystem );
		fMiddle = org.y - scRoundMP( (REAL)ptsize * ( RLU_BASEfmTop - (scBaseRLUsystem/2)) / scBaseRLUsystem );
		fRoman	= org.y;
		fBottom = org.y + scRoundMP( (REAL)ptsize * RLU_BASEfmBottom / scBaseRLUsystem );
		
		fTop	+= vjOffset;
		fMiddle += vjOffset;
		fRoman	+= vjOffset;
		fBottom += vjOffset;
		
	}
	else {
		fLeft	= org.x - ptsize / 2;
		fMiddle = org.x;
		fRight	= org.x + ptsize / 2;
		
		fLeft	-= vjOffset;
		fMiddle -= vjOffset;
		fRight	-= vjOffset;
	}
}

/* ==================================================================== */
// handle japanese counter flow numerical strings
struct scRenMojiStr {
	TypeSpec	fSpec;
	CharRecord	fRenMojiStr[8];
	scMuPoint	fPosition;	
};

/* ==================================================================== */
class scDrawLine {
public:
							scDrawLine( scTextline& 	txl, 
									   const scFlowDir& flowDir,
									   const APPDrwCtx	appMat,
									   const scMuPoint& transPt );
							~scDrawLine();
	void					Draw( void );
	
private:
	enum caseState {
		noCase,
		lowerCase,
		upperCase
	};

	
	scTextline& 			txl_;
	scContUnit& 			fPara;				// paragraph we are drawing
	scFlowDir				fFlowDir;
	const APPDrwCtx 		fDrawContext;
	const long				fStartOffset;
	const long				fEndOffset;
	const scMuPoint 		fOrigin;			// the original origin of line
	const GlyphSize 		fLetterSpace;	
	const short 			fLineCount;
	
		// the glyph array for drawing	
	scGlyphArray			fChArray[kMaxCharArray];
	scGlyphArray			*fChA;

		// data to pass to appdrawstring	
	scGlyphInfo 			fGlyphInfo; 		

		// paragraph spec
	TypeSpec				pspec_;
	
		// local state variables
	CharRecordP 			fChRec; 		// backing store
	scRubiArray 			*fRubiArray;		// rubi - if present
		
	scSpecRecord			*fSpecRec;
	TypeSpec				fSpec;	

		// this is the reference spec for baselines 
	TypeSpec				fReferenceSpec;

		// the baseline positions based upon the reference character	
	CBaselinePositions		fBaselinePositions;

	
	enum eStreamPosition {
		eNoEnd,
		eEndOfPara,
		eEndOfStream
	};
	
	eStreamPosition 		fStreamPosition;
	
	
	ushort					fCount; 			// glyphs to image
	ushort					fToDraw;			// characters left to process
		
	scMuPoint				fVJOffset;			// the vj correction
	scMuPoint				fDrawOrigin;		// the drawing origin	
	scMuPoint				fCurPos;			// the currect position 		
	scMuPoint				fBaseline;			// new computed baseline
		
	scXRect 				fMaxBounds;
	long					fOffset;
	
	void					SetCharData( void );
	void					SetCaseCharData( caseState );	
	void					SetCharData( GlyphSize,
										 GlyphSize,
										 const scFlowDir&,
										 eFntBaseline );
	
		// we pass in the character so we can override what is in
		// the backing store
	void					SetCharArray( const CharRecord&, BOOL lsp );
	
	scCachedStyle&			UpdateSpec();
	scCachedStyle&			UpdateCaseSpec( caseState = noCase );	
	
	void					IncrementBackingStore( void )
								{ 
									if ( fToDraw > 0 )
										fToDraw--;
	//								fToDraw = fToDraw > 0 ? fToDraw-- : fToDraw; 
									fChRec++; 
									fOffset++; 
								}
	void					IncrementDrawing( void )		
								{
									fChA++, fCount++;
									if ( fCount >= kMaxCharArray )
										OutputLine();
								}
	
	void					OutputLine( void );

	void					ComputeFillCharInfo( scTabInfo& tabInfo );
	void					ComputeTabInfo( scTabInfo& tabInfo );
	void					DrawTabSpace( void );
	void					DrawRule( void );
	void					DrawBullet( void );
	void					DrawField( void );
	

#ifdef _RUBI_SUPPORT
	void					SetRubiCharData( const scRubiData&	rd, 
											 const scFlowDir&	fd, 
											 eTSJustTypes		rubiJust,
											 int				count,
											 MicroPoint&		letterspace );
											 
	void					DrawRubi( const scRubiData& rd );
	void					ProcessRubi( void );


	void					ProcessRenMoji( void ); 
	void					DrawRenMoji( void );

	scMemArray				fRenMojiArray;
#else
	void					DrawRenMoji( void ) 	{}
	void					ProcessRubi( void ) 	{}
	void					ProcessRenMoji( void )	{}
#endif
	
	void					ProcessHyphen( scCharFlags );
};


/* ==================================================================== */
scDrawLine::scDrawLine( scTextline& 		txl,
						const scFlowDir&	flowDir,
						const APPDrwCtx 	drwCtx,
						const scMuPoint&	trans ) :
							txl_( txl ),
							fPara( *txl.GetPara() ),
							fFlowDir( flowDir ),
							fDrawContext( drwCtx ),
							fStartOffset( txl.fStartOffset ),
							fEndOffset( txl.fEndOffset ),
							fOrigin( txl.fOrigin.x + trans.x,
									 txl.fOrigin.y + trans.y ),
							fLetterSpace( txl.fLspAdjustment ),
							fLineCount( txl.fLineCount )
#ifdef _RUBI_SUPPORT
							, fRenMojiArray( sizeof( scRenMojiStr ) )
#endif
{	
	fChA			= fChArray;
	
	fChRec			= (CharRecordP)fPara.GetCharArray().Lock();
	fChRec			+= txl.fStartOffset;
	
	fSpecRec		= fPara.GetSpecRun().ptr() + fPara.GetSpecRun().indexAtOffset( txl.fStartOffset );
	fSpec			= fSpecRec++->spec();

	pspec_			=  fPara.GetDefaultSpec();
	
	scCachedStyle::SetParaStyle( &fPara, pspec_ );

	fReferenceSpec	= txl.fMaxLeadSpec.ptr() ? txl.fMaxLeadSpec : fSpec;
	
	fBaselinePositions.Init( fOrigin, txl.fVJOffset, fReferenceSpec, flowDir );
	
	scCachedStyle::GetCachedStyle( fSpec );

#ifdef _RUBI_SUPPORT
	fRubiArray		= fPara.GetRubiArray();
#endif
	
	fOffset 		= txl.fStartOffset;
	
	fCurPos 		= fOrigin;
	
	fMaxBounds		= txl.fInkExtents;
	
	fToDraw 		= (ushort)(fEndOffset - fStartOffset);
	fCount			= 0;

	fStreamPosition = eNoEnd; 

	if ( fFlowDir.IsVertical() ) {
		fCurPos.Translate( -txl.fVJOffset, 0 );
		fMaxBounds.Translate( -txl.fVJOffset, 0 );
		fVJOffset.Set( -txl.fVJOffset, 0 );
	}
	else {
		fCurPos.Translate( 0, txl.fVJOffset );
		fMaxBounds.Translate( 0, txl.fVJOffset );
		fVJOffset.Set( 0, txl.fVJOffset );		
	}
	
	fDrawOrigin 	= fCurPos;

	APPDrawStartLine( fDrawContext, fDrawOrigin.x, fDrawOrigin.y, fMaxBounds );
}

/* ==================================================================== */
scDrawLine::~scDrawLine()
{
	APPDrawEndLine( fDrawContext );
	
#ifdef _RUBI_SUPPORT
	if ( fRubiArray && fRubiArray->IsRubiData( fStartOffset, fEndOffset ) )
		ProcessRubi();
		
	if ( fRenMojiArray.GetNumItems() )
		DrawRenMoji();
#endif
	
	fPara.GetCharArray().Unlock( );
}

/* ==================================================================== */
inline void scDrawLine::SetCharArray( const CharRecord& ch, BOOL addLSP )
{
	fChA->fGlyphID	= ch.character;

#ifdef _RUBI_SUPPORT
	fChA->flags = ch.flags;
#endif
	
	if ( fFlowDir.IsVertical() ) {
		fChA->hEscapement = 0;
		fChA->vEscapement = ch.escapement;
		if ( addLSP )
			fChA->vEscapement += fLetterSpace;
	}
	else { 
		fChA->hEscapement	= ch.escapement;
		if ( addLSP )
			fChA->hEscapement += fLetterSpace;
		fChA->vEscapement	= 0;		
	}	
	
	fCurPos.Translate( fChA->hEscapement, fChA->vEscapement );
	
	IncrementDrawing();
}

/* ==================================================================== */
scCachedStyle& scDrawLine::UpdateSpec( )
{
	OutputLine();
	fSpec = (fSpecRec++)->spec();
	scCachedStyle& cachedStyle = scCachedStyle::GetCachedStyle( fSpec );
	SetCharData( );

	return cachedStyle;
}

/* ==================================================================== */
scCachedStyle& scDrawLine::UpdateCaseSpec( caseState cs )
{
	OutputLine();
	
	scCachedStyle& cachedStyle = scCachedStyle::GetCachedStyle( fSpec );
	SetCharData( );

	return cachedStyle;
}

/* ==================================================================== */
void scDrawLine::ComputeFillCharInfo( scTabInfo& tabInfo )
{
	scMuPoint	pt( fCurPos );
	
	if ( fFlowDir.IsVertical() )
		TSfillCharInfo( fSpec, 
						tabInfo.fillChar,
						tabInfo.fillCharAlign, 
						pt.y, pt.x,
						fLineCount );
	else
		TSfillCharInfo( fSpec, 
						tabInfo.fillChar,
						tabInfo.fillCharAlign, 
						pt.x, pt.y,
						fLineCount );	
}

/* ==================================================================== */
void scDrawLine::ComputeTabInfo( scTabInfo& tabInfo)
{	
	if ( fFlowDir.IsVertical() )
		TSTabInfo( pspec_, fSpec, tabInfo, fCurPos.y, 0L, 0L );
	else
		TSTabInfo( pspec_, fSpec, tabInfo, fCurPos.x, 0L, 0L ); 		
}

/* ==================================================================== */
void scDrawLine::DrawRule()
{
	scMuPoint end( fOrigin );
	end.x += fChRec->escapement;
	APPDrawRule( fCurPos, end, fGlyphInfo, fDrawContext );
}

/***************************************************************************/
/* set the ComputedCharData structure */

void scDrawLine::SetCharData(  )
{
	fGlyphInfo.fontname 	= scCachedStyle::GetCurrentCache().GetFont();
	fGlyphInfo.emphasis_	= scCachedStyle::GetCurrentCache().GetEmphasis();
	fGlyphInfo.fontRender	= scCachedStyle::GetCurrentCache().GetRender();
	fGlyphInfo.typespec 	= scCachedStyle::GetCurrentCache().GetSpec();
	fGlyphInfo.color		= scCachedStyle::GetCurrentCache().GetColor( );
	fGlyphInfo.pointsize	= scCachedStyle::GetCurrentCache().GetGlyphHeight();
	fGlyphInfo.setsize		= scCachedStyle::GetCurrentCache().GetGlyphWidth();
	fGlyphInfo.oblique		= 0;
	fGlyphInfo.rotation 	= 0;

	fGlyphInfo.flowDir		= fFlowDir;

	if ( fFlowDir.IsVertical() ) {
		fGlyphInfo.fBaseline	= scCachedStyle::GetCurrentCache().GetVertBaseline( );
		fBaseline.Set( scCachedStyle::GetCurrentCache().GetBaseline(), 0 );
	}
	else {
		fGlyphInfo.fBaseline	= scCachedStyle::GetCurrentCache().GetHorzBaseline( );
		fBaseline.Set( 0, -scCachedStyle::GetCurrentCache().GetBaseline() );
	}
}

/* ==================================================================== */
void scDrawLine::SetCaseCharData( caseState cs )
{
	OutputLine();

	fGlyphInfo.fontname 	= scCachedStyle::GetCurrentCache().GetFont();
	fGlyphInfo.emphasis_	= scCachedStyle::GetCurrentCache().GetEmphasis();
	fGlyphInfo.fontRender	= scCachedStyle::GetCurrentCache().GetRender();
	fGlyphInfo.typespec 	= scCachedStyle::GetCurrentCache().GetSpec();
	fGlyphInfo.color		= scCachedStyle::GetCurrentCache().GetColor( );

	if ( cs == lowerCase ) {
		fGlyphInfo.pointsize	= scRoundGS( scCachedStyle::GetCurrentCache().GetGlyphHeight() * kSmallCapCorrection );
		fGlyphInfo.setsize		= scRoundGS( scCachedStyle::GetCurrentCache().GetGlyphWidth() * kSmallCapCorrection );
	}
	else {
		fGlyphInfo.pointsize	= scCachedStyle::GetCurrentCache().GetGlyphHeight();
		fGlyphInfo.setsize		= scCachedStyle::GetCurrentCache().GetGlyphWidth();
	}
		
	fGlyphInfo.oblique		= 0;
	fGlyphInfo.rotation 	= 0;

	fGlyphInfo.flowDir		= fFlowDir;

	if ( fFlowDir.IsVertical() ) {
		fGlyphInfo.fBaseline	= scCachedStyle::GetCurrentCache().GetVertBaseline( );
		fBaseline.Set( scCachedStyle::GetCurrentCache().GetBaseline(), 0 );
	}
	else {
		fGlyphInfo.fBaseline	= scCachedStyle::GetCurrentCache().GetHorzBaseline( );
		fBaseline.Set( 0, -scCachedStyle::GetCurrentCache().GetBaseline() );
	}
}

/* ==================================================================== */
void scDrawLine::SetCharData( GlyphSize 		ptsize,
							  GlyphSize 		setsize,
							  const scFlowDir&	fd,
							  eFntBaseline		baseline )
{
	fGlyphInfo.fontname 	= scCachedStyle::GetCurrentCache().GetFont();
	fGlyphInfo.emphasis_	=  scCachedStyle::GetCurrentCache().GetEmphasis();
	fGlyphInfo.fontRender	= scCachedStyle::GetCurrentCache().GetRender();
	fGlyphInfo.typespec 	= scCachedStyle::GetCurrentCache().GetSpec();
	fGlyphInfo.color		= scCachedStyle::GetCurrentCache().GetColor();
	fGlyphInfo.pointsize	= ptsize;
	fGlyphInfo.setsize		= setsize;
	fGlyphInfo.oblique		= 0;
	fGlyphInfo.rotation 	= 0;

	fGlyphInfo.flowDir		= fd;

	if ( fFlowDir.IsVertical() ) {
		fGlyphInfo.fBaseline	= baseline;
		fBaseline.Set( scCachedStyle::GetCurrentCache().GetBaseline(), 0 );
	}
	else {
		fGlyphInfo.fBaseline	= baseline; 
		fBaseline.Set( 0, -scCachedStyle::GetCurrentCache().GetBaseline() );
	}
}

/* ==================================================================== */
void scDrawLine::OutputLine( )
{
	if ( fCount ) {
		scAssert( fGlyphInfo.typespec != 0 );
	
		scMuPoint	origin( fDrawOrigin );
		
		origin.Translate( fBaseline );
		
		if ( fGlyphInfo.flowDir.IsHorizontal() ) {
			switch ( fGlyphInfo.fBaseline ) {
				case kTopBaseline:
					origin.y = fBaselinePositions.fTop;
					break;
				case kMiddleBaseline:
					origin.y = fBaselinePositions.fMiddle;
					break;
				case kBottomBaseline:
					origin.y = fBaselinePositions.fBottom;
					break;
				default:
				case kRomanBaseline:
						// no change
					break;
			}
		}
		else {
			switch ( fGlyphInfo.fBaseline ) {
				case kLeftBaseline:
					origin.x = fBaselinePositions.fLeft;
					break;
				default:
				case kCenterBaseline:
						// no change
					break;
				case kRightBaseline:
					origin.x = fBaselinePositions.fRight;
					break;
			}			
		}
		APPDrawString( fDrawContext, fChArray, fCount, origin.x, origin.y, fGlyphInfo );
	}
	
	fDrawOrigin 	= fCurPos;
	fCount			= 0;
	fChA			= fChArray;
}

/* ==================================================================== */
// compute the fill character parameters
static void ComputeFillChar( eFCAlignment	alignment,	// fill character type
							 MicroPoint 	startX, 	// starting point
							 MicroPoint 	endX,		// ending point
							 long			lineNum,	// line number, for
														// even/odd fill chars
							 GlyphSize& 	fillChWid,	// fill character width
							 GlyphSize& 	headPad,	// pad before starting
														// to set chars
							 GlyphSize& 	tailPad 	// pad after finishing
														// setting chars
							)
{
	MicroPoint	run,				/* length of run for fill characters */
				extraSpace;

	run = endX - startX;

	if ( run < fillChWid || fillChWid == 9 ) {
		/* we cannot fit a fill character in the run */
		headPad = MPtoGS( run );
		tailPad = 0;
	}
	else {
		switch ( alignment ) {
			case eFCAltAlign:
				if ( startX % fillChWid == 0 )
					headPad = 0;
				else
					headPad = fillChWid - (GlyphSize)(startX % fillChWid);

				tailPad = (GlyphSize)(endX % fillChWid);

				if ( lineNum & 1 ) {
					headPad += ( fillChWid / 2 );
					tailPad -= ( fillChWid / 2 );
					if ( tailPad < 0 )
						tailPad += fillChWid;
				}
				break;
				
			case eFCJustAlign:
				if ( fillChWid ) {
					MicroPoint newFillChWid = 0;
					
					extraSpace = ( run % fillChWid ) / ( run / fillChWid + 1 );
					if ( run > fillChWid )
						newFillChWid = fillChWid + extraSpace;
						
					fillChWid	= MPtoGS( newFillChWid );
					headPad 	= MPtoGS( extraSpace );
					tailPad 	= 0;
				}
				break;
				
			case eFCLeftAlign:
				headPad = 0;
				tailPad = (GlyphSize)( run % fillChWid );
				break;
				
			case eFCNormalAlign:
				if ( startX % fillChWid == 0 )
					headPad = 0;
				else
					headPad = fillChWid - (GlyphSize)(startX % fillChWid);

				tailPad = (GlyphSize)(endX % fillChWid);
				break;
				
			default:
			case eFCRightAlign:
				headPad = (GlyphSize)( run % fillChWid );
				tailPad = 0;
				break;
		}
		if ( headPad + tailPad > run ) {
			headPad = MPtoGS( run );
			tailPad = 0;
		}
	}
}

/* ==================================================================== */
// draw the tab glyph(?) using the appropriate fill glyphs if needed
static BOOL GlyphAvail( UCS2 glyph )
{
	switch ( glyph ) {
		default:
			break;
		case scEndStream:
		case scEmptySpace:
		case scTabSpace:
		case scHardReturn:
		case scVertTab:
		case scField:
		case scRulePH:
		case scParaStart:
		case scParaEnd:
		case scQuadCenter:
		case scQuadLeft:
		case scQuadRight:
		case scQuadJustify:
		case scFixAbsSpace:
		case scFixRelSpace:
		case scFillSpace:
		case scNoBreakHyph:
		case scDiscHyphen:
		case scFigureSpace:
		case scThinSpace:
		case scEnSpace:
		case scEmSpace:
			return 0;
	}
	return 1;
}

void scDrawLine::DrawTabSpace()
{
	scTabInfo		tabInfo;
	GlyphSize		fillChWid;
	CharRecord		chRec;

	if ( fChRec->character == scFillSpace )
		ComputeFillCharInfo( tabInfo );
	else
		ComputeTabInfo( tabInfo );

	if ( fFlowDir.IsVertical() )
		tabInfo.xPosition = fCurPos.y + fChRec->escapement;
	else
		tabInfo.xPosition = fCurPos.x + fChRec->escapement;
		
	fillChWid = scCachedStyle::GetCurrentCache().GetEscapement( tabInfo.fillChar );

	if ( !GlyphAvail( tabInfo.fillChar ) || fChRec->escapement < 0 || fillChWid <= 0 ) {
		chRec.Set( CMctToAPP( fChRec->character ), fChRec->escapement );
		SetCharArray( chRec, false );
	}
	else {
		MicroPoint	endX;
		GlyphSize	headPad,
					tailPad;
					
		if ( fFlowDir.IsVertical() )
			ComputeFillChar( tabInfo.fillCharAlign,
							 fCurPos.y,
							 tabInfo.xPosition,
							 fLineCount,
							 fillChWid,
							 headPad,
							 tailPad );
		else
			ComputeFillChar( tabInfo.fillCharAlign,
							 fCurPos.x,
							 tabInfo.xPosition,
							 fLineCount,
							 fillChWid,
							 headPad,
							 tailPad );

		endX = tabInfo.xPosition;

			// for show invisibles
		if ( CMctToAPP( scTabSpace ) ) {
			chRec.Set( CMctToAPP( scTabSpace ), 0 );
			SetCharArray( chRec, false );
		}

		if ( headPad ) {
			chRec.Set( CMctToAPP( scEmptySpace ), headPad );
			SetCharArray( chRec, false );
		}

		if ( fFlowDir.IsVertical() ) {
			while ( fCurPos.y + fillChWid <= ( endX - tailPad ) ) {
				chRec.Set( CMctToAPP( tabInfo.fillChar ), fillChWid );
				SetCharArray( chRec, false );
			}
		}
		else {
			while ( fCurPos.x + fillChWid <= ( endX - tailPad ) ) {
				chRec.Set( CMctToAPP( tabInfo.fillChar ), fillChWid );
				SetCharArray( chRec, false );
			}		
		}
		
		if ( tailPad ) {
			chRec.Set( CMctToAPP( scEmptySpace ), tailPad );
			SetCharArray( chRec, false );
		}
	}
}

#ifdef _RUBI_SUPPORT

/* ==================================================================== */
// set the ComputedCharData structure
void scDrawLine::SetRubiCharData( const scRubiData& rd, 
								 const scFlowDir&	fd, 
								 eTSJustTypes		rubiJust,
								 int				count,
								 MicroPoint&		letterspace )
{
	REAL	rubifactor = ( (REAL)TSRubiSizeFactor( rd.fRubiSpec ) ) / 10000;
	
	fGlyphInfo.fontname 	= TSRubiFont( rd.fRubiSpec );
	fGlyphInfo.emphasis_	=  scCachedStyle::GetCurrentCache().GetEmphasis();
	fGlyphInfo.fontRender	= scCachedStyle::GetCurrentCache().GetFontRender();
	fGlyphInfo.typespec 	= scCachedStyle::GetCurrentCache().GetSpec();
	fGlyphInfo.color			= TSRubiColor( rd.fRubiSpec );
	
	fGlyphInfo.pointsize		= ROUND( scCachedStyle::GetCurrentCache().GetPtSize() * rubifactor );
	fGlyphInfo.setsize		= ROUND( scCachedStyle::GetCurrentCache().GetSetSize() * rubifactor );

	if ( fd.IsHorizontal() )
		fGlyphInfo.fBaseline		= kRomanBaseline;
	else
		fGlyphInfo.fBaseline		= kLeftBaseline;
		
	if ( rubiJust & lastLineJust && count > 1 ) {
		letterspace = rd.fLetterSpace * ( count - 1 );	
		if ( fd.IsHorizontal() ) {
			fGlyphInfo.setsize	+= ( letterspace / count );
			letterspace = 0;
		}
		else {
			fGlyphInfo.pointsize	+= ( letterspace / count );
			letterspace = 0;
		}
	}
	else
		letterspace =  rd.fLetterSpace;
	
	fGlyphInfo.oblique		= 0;
	fGlyphInfo.rotation 	= 0;
	fGlyphInfo.flowDir		= fFlowDir;
}

/* ==================================================================== */
// draw japanese rubi
void scDrawLine::DrawRubi( const scRubiData& rd )
{
	int 			i,
					count		= CharacterBufLen( rd.fCh );
	MicroPoint		hEscapement;
	MicroPoint		vEscapement;
	scXRect 		xrect( rd.fExtents );
	scMuPoint		org 		= rd.fOrg;
	scMuPoint		trans;
	TSJust			rubiJust	= TSRubiJust(  rd.fRubiSpec );
	MicroPoint		letterspace;
	
	scCachedStyle::GetCachedStyle( rd.fRubiSpec );
	SetRubiCharData( rd, fFlowDir, rubiJust, count, letterspace );
	
	trans = fOrigin;
	if ( fFlowDir.IsHorizontal() ) {
		switch ( scCachedStyle::GetCachedStyle().GetHorzBaseline() ) {
			case kTopBaseline:
				trans.y = fBaselinePositions.fTop;
				break;
			case kMiddleBaseline:
				trans.y = fBaselinePositions.fMiddle;
				break;
			case kBottomBaseline:
				trans.y = fBaselinePositions.fBottom;
				break;
			default:
			case kRomanBaseline:
				trans.y = fBaselinePositions.fRoman;
				break;
		}
	}
	else {
		switch ( scCachedStyle::GetCachedStyle().GetVertBaseline() ) {		
			case kLeftBaseline:
				trans.x = fBaselinePositions.fLeft;
				break;
			default:
			case kCenterBaseline:
				trans.x = fBaselinePositions.fMiddle;
				break;
			case kRightBaseline:
				trans.x = fBaselinePositions.fRight;
				break;
		}			
	}

	org.Translate( trans );
	fCurPos = org;

	if ( fFlowDir.IsVertical() ) {
		hEscapement = 0;
		vEscapement = fGlyphInfo.pointsize + letterspace;		
	}
	else {
		hEscapement = fGlyphInfo.setsize + letterspace;
		vEscapement = 0;
	}
	
	fChA	= fChArray;
	for ( i = 0; i < count; i++ ) {
		fChA[i].ch			= rd.fCh[i];
		fChA[i].flags.ClearFlags();
		fChA[i].hEscapement = hEscapement;
		fChA[i].vEscapement = vEscapement;		
	}

	APPDrawStartLine( fDawContext, org.x, org.y, hrect );
	APPDrawString( fDrawContext, fChArray, count, org.x, org.y, fGlyphInfo );
	APPDrawEndLine( fDrawContext ); 
}

/* ==================================================================== */
void scDrawLine::ProcessRubi( )
{	
	int 		nth,
				index;
	scRubiData	rd;
	
	for ( nth = 1; fRubiArray->GetNthRubi( index, rd, nth, fStartOffset, fEndOffset ); nth++ )
		DrawRubi( rd );
}

/* ==================================================================== */						
static MicroPoint ComputeRenMojiMeasure( CharRecordP	chRec,
										 int&			numChars )
{
	MicroPoint	meas;
	unsigned	n = chRec->flags.GetRenMoji();
	
	for ( meas = 0, numChars = 0; n--; chRec++, numChars++ ) {
		meas += chRec->escapement;
	}
	
	return meas;
}

/* ==================================================================== */
void scDrawLine::ProcessRenMoji()
{
	scRenMojiStr	rm;
	CharRecord		fauxChar;
	unsigned		i;
	unsigned		numChars = fChRec->flags.GetRenMoji();

		// save spec	
	rm.fSpec	= fSpec;
	
	scAssert( numChars < 7 );
	
		// process characters
	for ( i = 0; i < numChars; i++ ) {
		rm.fRenMojiStr[i] = *fChRec;
		rm.fRenMojiStr[i+1].character = 0;
		IncrementBackingStore();
	}
	
	fauxChar.Set( emSpace,	(fChRec-1)->escapement );
	SetCharArray( fauxChar, false );
	
	rm.fPosition = fCurPos;
	
	if ( fFlowDir.IsHorizontal() ) {
		switch ( scCachedStyle::GetCachedStyle().GetHorzBaseline() ) {
			case kTopBaseline:
				rm.fPosition.y = fBaselinePositions.fTop;
				break;
			case kMiddleBaseline:
				rm.fPosition.y = fBaselinePositions.fMiddle;
				break;
			case kBottomBaseline:
				rm.fPosition.y = fBaselinePositions.fBottom;
				break;
			default:
			case kRomanBaseline:
				rm.fPosition.y = fBaselinePositions.fRoman;
				break;
		}
	}
	else {
		switch ( scCachedStyle::GetCachedStyle().GetVertBaseline() ) {		
			case kLeftBaseline:
				rm.fPosition.x = fBaselinePositions.fLeft;
				break;
			default:
			case kCenterBaseline:
				rm.fPosition.x = fBaselinePositions.fMiddle;
				break;
			case kRightBaseline:
				rm.fPosition.x = fBaselinePositions.fRight;
				break;
		}			
	}
	
	fRenMojiArray.AppendData( (ElementPtr)&rm );
}

/* ==================================================================== */
void scDrawLine::DrawRenMoji()
{
	int 		i,
				j;
	scRenMojiStr	rm;
	MicroPoint	microMeasure;
	MicroPoint	romanBaselineShift;
	MicroPoint	maxMeasure;
	REAL		ptsizeAdjust;
	REAL		setwidthAdjust;
	scFlowDir	antiFlowDir;
	scFlowDir	saveFlowDir( fFlowDir );
	TypeSpec	saveSpec	= scCachedStyle::GetCurrentCache().GetSpec();
	
	antiFlowDir.CounterFlow( fFlowDir );

	for ( i = 0; i < fRenMojiArray.GetNumItems(); i++ ) {
		fRenMojiArray.GetDataAt( i, (ElementPtr)&rm );

		scCachedStyle::SetFlowdir( antiFlowDir );
		scCachedStyle::GetCachedStyle( rm.fSpec );

		microMeasure = BRKComposeRenMoji( rm.fRenMojiStr, rm.fSpec, antiFlowDir, true );
		
		maxMeasure = scCachedStyle::GetCurrentCache().GetPtSize() * 2;

#ifdef kMakimotoRenMoji
		int doMakimotoRenMoji = 1;
#else
		int doMakimotoRenMoji = 0;
#endif
		if ( doMakimotoRenMoji && microMeasure > maxMeasure ) {
				// first try to reduce setwidth
			setwidthAdjust = (REAL)maxMeasure / microMeasure;
			
			if ( setwidthAdjust < 0.60 )
				ptsizeAdjust = setwidthAdjust;		// adjust pointsize to
			else
				ptsizeAdjust = 1.0;
			
			microMeasure = ROUND( microMeasure * setwidthAdjust );
			
			for ( j = 0; rm.fRenMojiStr[j].character; j++ )
				rm.fRenMojiStr[j].escapement = ROUND( rm.fRenMojiStr[j].escapement * setwidthAdjust );
		}
		else {
			setwidthAdjust	= 1.0;
			ptsizeAdjust	= 1.0;
		}
		
		MicroPoint ptsize = ROUND( ptsizeAdjust * scCachedStyle::GetCurrentCache().GetPtSize() );
		MicroPoint setsize = ROUND( setwidthAdjust * scCachedStyle::GetCurrentCache().GetSetSize() );

		if ( fFlowDir.IsVertical() ) {
			MicroPoint	midPoint,
						left;

			switch ( scCachedStyle::GetCachedStyle().GetVertBaseline() ) {					
				case kLeftBaseline:
					midPoint	= ROUND( scCachedStyle::GetCurrentCache().GetPtSize() * (scBaseRLUsystem/2) / scBaseRLUsystem );
					break;
				case kCenterBaseline:
					midPoint = 0;
					break;
				case kRightBaseline:
					midPoint	= ROUND( -scCachedStyle::GetCurrentCache().GetPtSize() * (scBaseRLUsystem/2) / scBaseRLUsystem );
					break;
			}
			romanBaselineShift = -(scCachedStyle::GetCurrentCache().GetPtSize()/2) + (ptsize/2) - (ptsize*RLU_BASEfmBottom/scBaseRLUsystem);
			left		= midPoint	- microMeasure / 2;
			rm.fPosition.Translate( left, romanBaselineShift );
		}
		else {
			MicroPoint	midPoint,
						top;
			switch ( scCachedStyle::GetCachedStyle().GetHorzBaseline() ) {
				case kTopBaseline:
					midPoint	= ROUND( scCachedStyle::GetCurrentCache().GetPtSize() * (scBaseRLUsystem/2) / scBaseRLUsystem );
					break;
				case kMiddleBaseline:
					midPoint = 0;
					break;
				case kBottomBaseline:
					midPoint	= ROUND( -scCachedStyle::GetCurrentCache().GetPtSize() * (scBaseRLUsystem/2) / scBaseRLUsystem );
					break;
				case kRomanBaseline:
					midPoint	= ROUND(-scCachedStyle::GetCurrentCache().GetPtSize() * (scBaseRLUsystem/2-RLU_BASEfmBottom) / scBaseRLUsystem);
					break;
			}
			top 		= midPoint - microMeasure/2;			
			rm.fPosition.Translate( -scCachedStyle::GetCurrentCache().GetSetSize() / 2, top );
		}
		
		fDrawOrigin = rm.fPosition;
		fFlowDir	= antiFlowDir;

		ptsize		= ROUND( ptsizeAdjust * scCachedStyle::GetCurrentCache().GetPtSize() );
		setsize 	= ROUND( setwidthAdjust * scCachedStyle::GetCurrentCache().GetSetSize() );
		
		if ( fFlowDir.IsHorizontal() )
			SetCharData( ptsize, setsize, antiFlowDir, kRomanBaseline  );
		else
			SetCharData( setsize, ptsize, antiFlowDir, kCenterBaseline );		
		
		for ( j = 0; rm.fRenMojiStr[j].character; j++ )
			SetCharArray( rm.fRenMojiStr[j], true );
			
		OutputLine();
		
		fFlowDir	= saveFlowDir;
	}
	
	scCachedStyle::SetFlowdir( saveFlowDir );
	scCachedStyle::GetCachedStyle( saveSpec );
}

#endif

/* ==================================================================== */
// process a hyphen at the end of a line if found
void scDrawLine::ProcessHyphen( scCharFlags flags )
{
	if ( flags.IsKernPresent() ) {
		if ( fFlowDir.IsVertical() )
			(fChA-1)->vEscapement = scCachedStyle::GetCurrentCache().GetEscapement( (fChRec-1)->character ) + fLetterSpace;
		else
			(fChA-1)->hEscapement = scCachedStyle::GetCurrentCache().GetEscapement( (fChRec-1)->character ) + fLetterSpace;
	}
	
	fChA->fGlyphID		=  scCachedStyle::GetCurrentCache().GetHyphChar();
	
	if ( fFlowDir.IsVertical() ) {
		fChA->vEscapement	= scCachedStyle::GetCurrentCache().GetEscapement( '-' );
		fChA->hEscapement	= 0;
	}
	else {
		fChA->hEscapement	= scCachedStyle::GetCurrentCache().GetEscapement( '-' );
		fChA->vEscapement	= 0;
	}
	fCount++;
}

/* ==================================================================== */
void::scDrawLine::DrawBullet()
{
	fChArray[0].fGlyphID	= scCachedStyle::GetParaStyle().GetBulletChar();
	fChArray[0].hEscapement = 0;
	fChArray[0].vEscapement = 0;

	scMuPoint origin = fOrigin + fVJOffset;
	//origin.x -= MIN( ( fGlyphInfo.pointsize + fGlyphInfo.pointsize / 2 ), 
	//				 scCachedStyle::GetParaStyle().GetBulletIndent() );
	// origin.x -= ( fGlyphInfo.pointsize + fGlyphInfo.pointsize / 2 );
	origin.x -= scCachedStyle::GetParaStyle().GetBulletIndent();

	scGlyphInfo gi	= fGlyphInfo;
	gi.fontRender	= scCachedStyle::GetParaStyle().GetBulletRender();
	gi.color		= scCachedStyle::GetParaStyle().GetBulletColor();
	gi.fontname 	= scCachedStyle::GetParaStyle().GetBulletFontFamily();
	gi.emphasis_	= scCachedStyle::GetParaStyle().GetBulletEmphasis();

	APPDrawString( fDrawContext, fChArray, 1, origin.x, origin.y, gi ); 
}

/* ==================================================================== */
void scDrawLine::DrawField()
{
	OutputLine();

	USTR	ustr;
	TypeSpec		spec;
	MicroPoint* 	widths = 0;

	clField& field = APPCreateField(txl_.GetColumn()->GetStream(), fChRec->flags.GetField());
	field.content( ustr, txl_.GetColumn()->GetAPPName(), fSpec );
	field.release();

	if ( ustr.len )
		widths = new MicroPoint[ ustr.len ];

	UnivStringWidth( ustr, widths, fSpec );

	for ( UINT i = 0; i < ustr.len; i++ ) { 	
		fChA->fGlyphID	= CMctToAPP( ustr.ptr[i] );

		fChA->hEscapement	= widths[i];
		fChA->hEscapement	+= fLetterSpace;
		fChA->vEscapement	= 0;
		
		fCurPos.Translate( fChA->hEscapement, fChA->vEscapement );

		IncrementDrawing();
	}	
	OutputLine();
	delete widths;
	widths = NULL;
}

/* ==================================================================== */
// run over the glyphs collecting information before calling client
// with APPDrawString
void scDrawLine::Draw()
{
	caseState	prevCase = noCase;
	caseState	curCase  = noCase;
	
	CharRecord chRec;

	SetCharData( );

	if ( !fOffset && scCachedStyle::GetParaStyle().GetNumbered() )
		DrawBullet();

	scCachedStyle& cachedStyle = scCachedStyle::GetCurrentCache();

	for ( ; fToDraw > 0; ) {

			// if there is a spec change dump any characters in the buffer
		if ( fOffset >= fSpecRec->offset()	) {
			cachedStyle = UpdateSpec(); 
			prevCase = noCase;
		}

#ifdef jis4051
		if ( fChRec->flags.GetRenMoji() > 0 )
			ProcessRenMoji();
		else 
#endif
		{  
			UCS2 ch = CMctToAPP( fChRec->character );

			switch ( fChRec->character ) {
				case scField:
					DrawField();
					break;

				case scRulePH:
					DrawRule();
					break;

				case scFillSpace:
				case scTabSpace:
					DrawTabSpace();
					break;
					
				default:
					if (ch)
					{
						chRec.Set( CMctToAPP( ch ), fChRec->flags, fChRec->escapement );
						SetCharArray( chRec, LETTERSPACE( fChRec ) );
					}
					break;
	
				case scFixRelSpace:
					chRec.Set( CMctToAPP( ch ), SCRLUCompGS( cachedStyle.GetGlyphWidth() , (RLU)fChRec->escapement ) );
					SetCharArray( chRec, false );
					break;
	
				case scFixAbsSpace:
					chRec.Set( CMctToAPP( ch ), fChRec->escapement );
					SetCharArray( chRec, false );
					break;
			}	
			IncrementBackingStore();
		}
		prevCase = curCase;
	}

	if ( fEndOffset == fPara.GetContentSize() ) {
		UCS2 ch = fCount ? (fChRec-1)->character : (UCS2)0;

		switch ( ch ) {
			case scHardReturn:
			case scVertTab:
				break;
			default:
				if ( fPara.Next() )
					fStreamPosition = eEndOfPara;
				else
					fStreamPosition = eEndOfStream;
				break;
		}
	}

	if ( fCount || fStreamPosition != eNoEnd ) {
		if ( fCount ) {
			scCharFlags flags	= (fChRec-1)->flags;

			if ( flags.IsHyphPresent() )
				ProcessHyphen( flags );
		}
			
		if ( fStreamPosition == eEndOfPara ) {
			if ( CMctToAPP( scParaEnd ) ) { 
				chRec.Set( CMctToAPP( scParaEnd ),	scCachedStyle::GetCurrentCache().GetPtSize() );
				SetCharArray( chRec, false );
			}
		}
		else if ( fStreamPosition == eEndOfStream ) {
			if ( CMctToAPP( scEndStream ) ) {
				chRec.Set( CMctToAPP( scEndStream ),  scCachedStyle::GetCurrentCache().GetPtSize() );
				SetCharArray( chRec, false );
			}
		}
				
		OutputLine();
	}
}

/* ==================================================================== */
/* draw the line */
void scTextline::Draw( APPDrwCtx		appMat,
					   const scFlowDir& flowDir,
					   const scMuPoint& transpt )
{
	if ( !fPara->Marked( scREBREAK | scRETABULATE ) ) {
		if ( GetCharCount() || Marked( scLASTLINE ) ) {
			scDrawLine ld( *this, flowDir, appMat, transpt );
			ld.Draw();
		}
		Unmark( scREPAINT );  
	}
}

/* ==================================================================== */
// select spec on line
static scSpecRecord*	LNSpecSelect( CharRecordP	chRec, 
									  CharRecordP	chStop,
									  scXRect&		exrect,
									  BOOL			vertical,
									  long& 		offset, 							
									  scSpecRecord* specRec,
									  MicroPoint	letterSpace,
									  scMuPoint&	curPos,
									  const scMuPoint&	hitPt )
{
	MicroPoint	chEscapement;
	
	if ( vertical ) {
		MicroPoint	width = exrect.Width();
		exrect.x1	-= width;
		exrect.x2	+= width;
	}
	else {
		MicroPoint	depth = exrect.Depth();
		exrect.y1	-= depth;
		exrect.y2	+= depth;	
	}
	
	TypeSpec ts;

		// walk the character buffer
	for ( ; chRec < chStop;  chRec++, offset++ ) {

		while ( offset >= specRec->offset() ) {
			ts = specRec->spec();
			scCachedStyle::GetCachedStyle( ts );
			specRec++;
		}

		if ( LETTERSPACE( chRec ) )
			chEscapement = chRec->escapement + letterSpace;
		else
			chEscapement = chRec->escapement;

		if ( chRec->character == scFixRelSpace )
			chEscapement = SCRLUCompGS( scCachedStyle::GetCurrentCache().GetGlyphWidth(), (RLU)chRec->escapement );

		if ( vertical ) {
			exrect.y2 = curPos.y + ( chEscapement / 2 );
			if ( exrect.y2 < exrect.y1 )
				exrect.Set( exrect.x1, exrect.y2, exrect.x2, exrect.y1 );	
		}
		else {
			exrect.x2 = curPos.x + ( chEscapement / 2 );
			if ( exrect.x2 < exrect.x1 )
				exrect.Set( exrect.x2, exrect.y1, exrect.x1, exrect.y2 );
		}
			
		if ( exrect.PinRect( hitPt ) )
			break;
		
		if ( chRec->character == scHardReturn)
			break;

		if ( vertical )
			curPos.Translate( 0, chEscapement );
		else
			curPos.Translate( chEscapement, 0 );
	}
	return specRec;
}

/* ==================================================================== */
// select on glyph on line
REAL scTextline::Select( scMuPoint& 		charLoc,
						 long&				count,
						 const scMuPoint&	hitPt,
						 eContentMovement,
						 BOOL&				endOfLine )
{
	CharRecordP 	chStop;
	CharRecordP 	chStart;
	CharRecordP 	chRec;
	scSpecRecord	*specRec;
	scMuPoint		curPos( fOrigin );
	scXRect 		extents( fInkExtents ); 
	long			offset;
	MicroPoint		letterSpace;
	BOOL			searchDone	= false;
	BOOL			vertical;
	
	endOfLine		= false;

	vertical		= fColumn->GetFlowdir().IsVertical();

	chStart 		= (CharRecordP)fPara->GetCharArray().Lock();
	
	specRec 		= fPara->GetSpecRun().ptr( );

	letterSpace 	= fLspAdjustment;

	chStop			= chStart + fEndOffset;
	offset			= fStartOffset;
	chRec			= chStart + offset; 
		
	if ( vertical ) {
		curPos.Translate( -fVJOffset, 0 );
		extents.Translate( -fVJOffset, 0 );
	}	
	else {
		curPos.Translate( 0, fVJOffset );
		extents.Translate( 0, fVJOffset );
	}
	
	TypeSpec ts = specRec->spec();
	scCachedStyle::GetCachedStyle( ts );

		// now start the computation
	if ( !searchDone )
		specRec = LNSpecSelect( chRec, chStop, extents, vertical, offset, specRec, letterSpace, curPos, hitPt );		

	if ( offset >= fEndOffset ) {
		endOfLine = true;
		if ( offset == fEndOffset && IsHyphenated() ) {
			count	= offset;
			chRec	= chStart + ( offset - 1 );
			if ( chRec ->character != scBreakingHyphen )
				curPos.x += scCachedStyle::GetCurrentCache().GetEscapement( '-' );
		}
		else
			count = fEndOffset;
	}
	else
		count = offset;

	charLoc = curPos;

	fPara->GetCharArray().Unlock();

		// the hit is within the extents of the line now
		// determine how far from the selected point it
		// is and return the square of the distance
	REAL	x,
			y;

	if ( vertical ) {
		x = (REAL)(hitPt.y - curPos.x);
		y = (REAL)(hitPt.x - curPos.y);
	}
	else {
		x = (REAL)(hitPt.x - curPos.x);
		y = (REAL)(hitPt.y - curPos.y);
	}
	x = x * x;
	y = y * y;
	return( x + y );
}

/****************************************************************************/
// given an offset determine its location on the line

scMuPoint& scTextline::Locate( long&			chOffset, 
							   scMuPoint&		charOrg, 
							   eContentMovement cursDirection )
{
	CharRecordP 	chStart;
	CharRecordP 	chStop;
	CharRecordP 	chRec;
	scSpecRecord*	specRec;
	scContUnit* 	para;
	long			offset;
	MicroPoint		chEscapement;
	MicroPoint		letterSpace;
	BOOL			vertical;

	para		= fPara;	
	specRec 	= para->GetSpecRun().ptr( );
	
	chStart 	= (CharRecordP)para->GetCharArray().Lock();
	chRec		= chStart;

	TypeSpec ts = specRec->spec();
	scCachedStyle::GetCachedStyle( ts );
	
	chStop		= chRec + fEndOffset;
		
	offset		= fStartOffset;
	chRec		+= offset;

	vertical	= fColumn->GetFlowdir().IsVertical();

	charOrg 	= fOrigin;
	
	letterSpace = fLspAdjustment;

		// walk the line adding the escapements
	for ( ;  chRec	< chStop;  chRec++, offset++ ) {

		if ( offset == chOffset )
			break;

		switch ( chRec->character ) {
			default:
				if ( LETTERSPACE(  chRec  ) )
					chEscapement =	chRec ->escapement + letterSpace;
				else
					chEscapement =	chRec ->escapement;
				break;

			case scFixRelSpace:
				chEscapement = SCRLUCompGS( scCachedStyle::GetCurrentCache().GetGlyphWidth(), (RLU) chRec ->escapement );
				break;
		}
		if ( vertical )
			charOrg.Translate( 0, chEscapement );
		else
			charOrg.Translate( chEscapement, 0 );			
	}

	long preRenMojiOffset = offset;

	switch ( cursDirection ) {
		default:
		case eCursBackward:
			while ( offset > 0 && TXTSameRenMoji( chStart, chRec-1, chRec ) ) {
				chRec--, offset--;
				chEscapement =	chRec->escapement;
				if ( vertical )
					charOrg.Translate( 0, -chEscapement );
				else
					charOrg.Translate( -chEscapement, 0 );			
			}
			break;
		case eCursForward:
			while ( chRec < chStop && TXTSameRenMoji( chStart, chRec-1, chRec ) ) {
				chEscapement =	chRec->escapement;
				if ( vertical )
					charOrg.Translate( 0, chEscapement );
				else
					charOrg.Translate( chEscapement, 0 );	
				chRec++, offset++;							
			}
			break;
	}

	if ( preRenMojiOffset != offset )
		chOffset = offset;

	if ( chOffset == fEndOffset && IsHyphenated() && (chRec-1)->character != scBreakingHyphen ) {
		if ( vertical )
			charOrg.Translate( 0, scCachedStyle::GetCurrentCache().GetEscapement( '-' ) );		
		else
			charOrg.Translate( scCachedStyle::GetCurrentCache().GetEscapement( '-' ), 0 );
	}

	para->GetCharArray().Unlock();

	return charOrg;
}

/*======================================================================*/
// get glyph at start of line

UCS2 scTextline::CharAtStart() const
{
	return fPara->GetCharArray().GetCharAtOffset( fStartOffset );
}

/*======================================================================*/
// get glyph at end of line

UCS2 scTextline::CharAtEnd() const
{
	return fPara->GetCharArray().GetCharAtOffset( fEndOffset - 1 );
}

/*======================================================================*/
