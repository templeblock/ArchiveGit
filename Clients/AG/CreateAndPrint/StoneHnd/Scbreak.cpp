#include "scbreak.h"
#include "sccolumn.h"
#include "scglobda.h"
#include "scstcach.h"
#include "scctype.h"
#include "scmem.h"
#include "screfdat.h"
#include "sccallbk.h"

// TOOLBOX BEHAVIOR - force first word on line to break if it does not fit
#define gHiliteSpaces false
#define scForceBreakFirstWord

#define MAXLEADVALS 		50
#define MAXBREAKVALS		100

#ifndef LETTERSPACE
#define LETTERSPACE( ch ) ( (ch)->character!=scWordSpace\
								&& ((ch)+1)->character!=scWordSpace )
#endif	

static MicroPoint	BRKNextTokenWidth( CharRecordP, UCS2 );
static BOOL 		TabBreakChar( UCS2 theCh, UCS2 breakCh );

static eBreakEvent		BRKLoopBody( void );
static eBreakEvent		BRKTheLoop( void );

static BOOL 		BRKStillMoreChars( CharRecordP, long );

#ifdef JAPANESE //j
static void 		BRKCharJapanese( void );
#endif JAPANESE

static eBreakEvent		bmBRKWordSpace( void );
static eBreakEvent		bmBRKFixSpace( void );
static eBreakEvent		bmBRKRelSpace( void );
static eBreakEvent		bmBRKEndStream( void );
static eBreakEvent		bmBRKChar( void );
static eBreakEvent		bmBRKHardReturn( void );
static eBreakEvent		bmBRKQuad( void );
static eBreakEvent		bmBRKField( void );
static eBreakEvent		bmBRKVertTab( void );
static eBreakEvent		bmBRKTab( void );
static eBreakEvent		bmBRKFillSpace( void );
static eBreakEvent		bmBRKRule( void );
static eBreakEvent		bmBRKHyphen( void );

static void 		BRKPlaceLine( scMuPoint&, MicroPoint&, const scFlowDir& );
static void 		BRKJustifyLine( void );
static MicroPoint	BRKRagControl(	CharRecordP,
									MicroPoint, MicroPoint,
									MicroPoint, TypeSpec,	ushort, short );

static CandBreak*	BRKLineDecisionJust( void );
static CandBreak*	BRKLineDecisionRag( void );
static void 		BRKSetCharIndent( CharRecordP, long, long, MicroPoint );

static CandBreak*	BRKHyphenateRag( void );
static CandBreak*	BRKHyphenateJust( void );

static void 		BRKAddHyphens( CandBreak*, CandBreak* );
#ifdef scForceBreakFirstWord
static void 		BRKForceHyphens( CandBreak*, CandBreak* );
#endif
static short		BRKPerformDiscHyphen( CharRecordP, CharRecordP, Hyphen* );

static void 		BRKAdjustWordSpace( CharRecordP, GlyphSize, long, long );
							
static void 		BRKRepairLastSpace(  CharRecordP, long );
static void 		BRKRepairFinalSpace( void );
static CharRecordP	BRKLastCharOnLine( CharRecordP );
static MicroPoint	BRKHangPuncRightAdjust( void );

static TypeSpec 	BRKUpdateSpec( scSpecRecord* );

static MicroPoint	BRKKernCorrection( CharRecordP );

static void 		BRKMaxLineVals( scLINERefData&, MicroPoint, eFntBaseline, MicroPoint );

/**********************************************************************/
CandBreak::CandBreak()
{
	Init();
}

/************************************************************************/

void CandBreak::Init()
{
	breakCount		= 0;
	startCount		= 0;
	streamCount 	= 0;
	wsSpaceCount	= 0;
	spaceCount		= 0;
	trailingSpaces	= 0;
	chCount 		= 0;
	fillSpCount 	= 0;
	lineVal 		= 0;
	breakVal		= eUndefinedBreak;
	minGlue 		= 0;
	optGlue 		= 0;
	maxGlue 		= 0;
	curBox			= 0;
	fHangable		= 0;
	theChRec		= 0;
	specChanged 	= 0;
	spec.clear();
	specRec 		= 0;
}

/************************************************************************/
// if the break candidates fill we remove the first entry and shuffle the 
// candidates down since the first candidate is no longer a real candidate

static void ShuffleBreakCandidates()
{
	gbrS.candBreak[0].Init();  // force the clearing of a spec
		
		// shuffle the array down into the spot we just cleared
	memmove( gbrS.candBreak, gbrS.candBreak+1, sizeof(CandBreak) * (MAXBREAKVALS-1L) );

		// zero out the last entry which is now doubled because we copied it into
		// the next to last entry
	memset( gbrS.candBreak + ( MAXBREAKVALS - 1 ), 0, sizeof(CandBreak) );

		// initialize the last entry
	gbrS.candBreak[MAXBREAKVALS-1].Init();
}

/************************************************************************/
/* put the current break values into the candidate break array */

static void BRKSetCandBreak( eBreakType breakType )
{
	CandBreak			*theBreak	= gbrS.candBreak + gbrS.cB.breakCount;

	*theBreak			= gbrS.cB;
	theBreak->breakVal	= breakType;

	if ( breakType == eHyphBreak )
		theBreak->curBox += scCachedStyle::GetCurrentCache().GetEscapement( '-' );

	if ( gbrS.cB.breakCount >= (MAXBREAKVALS-1) )
		ShuffleBreakCandidates();	
	else
		gbrS.cB.breakCount++;
}

/************************************************************************/

static eBreakEvent BRKExitLoop(  )
{
	return measure_exceeded;
}

/**********************************************************************/
/* we are passed in some characters & line attributes, break the line,
 * performing justification, hyphenation, indents, etc.
 */

eBreakType
BRKRomanLineBreak( CharRecordP		chRec,		// the character array
				   long 			startCount, // # into char array to start the linebreak
				   long&			count,		// count into char array of end of line
				   scLINERefData&	lineData,
				   short			lineCount,
				   short&			linesHyphenated,
				   scSpecRecord**	specRec,
				   scXRect&,
				   GlyphSize&		letterSpace )
{
	/* not a good idea to use register or auto variables that are used
	 * across a call to setjmp/longjmp, they alter them when the previous
	 * stack is restored
	 */
	MicroPoint		initialLead;
	eFntBaseline	initialBaseline;

	gbrS.Init();	

	/* set up state variables */
	gbrS.gStartRec = gbrS.cB.theChRec = chRec + startCount;

	gbrS.fMaxLineVals[0] = gbrS.fZeroMaxLineVals;

	lineData.fInkExtents.Translate( lineData.fOrg );  

			/* zero out the char indent at the start of every paragraph */
	if ( startCount == 0 )		
		gbrS.charIndent = LONG_MIN;
	gbrS.foundCharIndent = true;

	gbrS.cB.startCount		= gbrS.cB.streamCount		= (long)startCount;
	gbrS.theSpecRec 		= *specRec;
	gbrS.cB.spec			= gbrS.theSpecRec->spec();
	gbrS.originalMeasure	= lineData.fComputedLen = lineData.fMeasure;

	/* this test is somewhat arbitrary, but we are reaching the
	 * outer limits of our unit system, and what we probably
	 * have encountered is a horizontally flexible column,
	 * which should not be justified!! 
	 */
	gbrS.allowJustification = ( gbrS.originalMeasure < (LONG_MAX-one_pica) );

	/* a little bullet proofing */
	if (  gbrS.originalMeasure < 0 )
		 gbrS.originalMeasure = one_pica * 2;

	if ( lineData.IsHorizontal() )
		gbrS.colShapeRag		= (eTSJust)( lineData.fColShapeType & eHorzFlex ? eRagLeft : -1 );
	else 
		gbrS.colShapeRag		= (eTSJust)( lineData.fColShapeType & eVertFlex ? eRagLeft : -1 );

	gbrS.lastLineLen		= lineData.fLastLineLen;

	gbrS.theLineCount		= lineCount;
	gbrS.cB.breakVal		= eCharBreak;
	gbrS.cB.lineVal 		= 0;
	gbrS.cB.breakCount		= 0;
	gbrS.cB.spaceCount		= 0;
	gbrS.cB.trailingSpaces	= 0;
	gbrS.cB.wsSpaceCount	= 0;
	gbrS.cB.fillSpCount 	= 0;
	gbrS.cB.chCount 		= 0;

	gbrS.letterSpaceAdj 	= 0;
	gbrS.minRelPosition 	= 0;
	gbrS.cB.curBox			= 0;

	initialLead 	= lineData.fInitialLead.GetLead();
	initialBaseline = scCachedStyle::GetCurrentCache().GetBaselineType( );

	gbrS.cB.optGlue 		= gbrS.cB.minGlue = gbrS.cB.maxGlue = 0L;
	gbrS.tmpMinGlue 		= gbrS.tmpOptGlue	= gbrS.tmpMaxGlue = 0L;

	gbrS.cB.specChanged 	= false;
	gbrS.firstBox			= gbrS.firstGlue		= true;
	gbrS.fNoStartline		= false;
	gbrS.fLastHangable		= 0;
	gbrS.numTargetChars 	= 0;
	gbrS.totalTrailingSpace = 0;


	gbrS.desiredMeasure = ::BRKRagControl( gbrS.cB.theChRec, lineData.fOrg.x, lineData.fOrg.y,
										   lineData.fMeasure, gbrS.cB.spec, lineCount, linesHyphenated );

	if (  gbrS.desiredMeasure <= 0 ) {
		count = 0;
#if _DEBUG
	gbrS.Init();
#endif
		return eCharBreak;
	}


	if ( gbrS.cB.theChRec->character == scEndStream ) {
		if ( startCount==0 || (gbrS.cB.theChRec-1)->character!=scHardReturn ) {
			BRKPlaceLine( lineData.fOrg, lineData.fComputedLen, scCachedStyle::GetCurrentCache().GetFlowdir() );
			count = gbrS.cB.streamCount - startCount; 
			BRKMaxLineVals( lineData, initialLead, initialBaseline, 0 );
#if _DEBUG
	gbrS.Init();
#endif

			return eEndStreamBreak;
		}
	}

	switch ( BRKTheLoop() ) {
		case start_of_line: 		
			break;
		case end_of_stream_reached:
				/* the end of paragraph has been detected */
				/* force justify the line,
				 * NOTE: we should probably justify the line
				 * if it is close to the desired measure
				 * I THINK WE DO
				 */
			BRKRepairFinalSpace(  );
			lineData.fRagSetting = gbrS.effectiveRag;
			if ( gbrS.colShapeRag != (eTSJust)-1 )
				gbrS.effectiveRag = eRagRight;

			if ( (gbrS.effectiveRag & eRagFlag) == eRagJustified && !gbrS.cB.fillSpCount ) {
				if ( !(gbrS.effectiveRag & (int)eLastLineJust) )
					lineData.fComputedLen = gbrS.cB.curBox + gbrS.cB.optGlue;
				else {
					if ( gbrS.allowJustification )
						BRKJustifyLine(   );
					lineData.fComputedLen =  gbrS.desiredMeasure;
				}
				lineData.fOrg.x += gbrS.brkLeftMargin;
				if ( gHiliteSpaces )
					lineData.fComputedLen += gbrS.totalTrailingSpace;
			}
			else
				BRKPlaceLine( lineData.fOrg, lineData.fComputedLen, scCachedStyle::GetCurrentCache().GetFlowdir() );
			count		= gbrS.cB.streamCount - startCount;
			letterSpace = gbrS.letterSpaceAdj;
			break;
		default:
		case measure_exceeded:
				// the line measure has been exceeded, there are still more
				// characters in the paragraph
			lineData.fRagSetting = gbrS.effectiveRag;
			if ( gbrS.colShapeRag != (eTSJust)-1 )
				gbrS.effectiveRag = eRagRight;
		 
			if ( (gbrS.effectiveRag & eRagFlag) == eRagJustified && !gbrS.cB.fillSpCount ) {
				if ( gbrS.allowJustification )
					BRKJustifyLine(  );
				lineData.fOrg.x += gbrS.brkLeftMargin;
				lineData.fComputedLen = gbrS.desiredMeasure;
				if ( gHiliteSpaces )
					lineData.fComputedLen += gbrS.totalTrailingSpace;
			}
			else
				BRKPlaceLine( lineData.fOrg, lineData.fComputedLen, scCachedStyle::GetCurrentCache().GetFlowdir() );
			count		= gbrS.cB.streamCount - startCount;
			letterSpace = gbrS.letterSpaceAdj;
			break;
	}
	/* increment the line hyphenation count, used to prevent to many
	 * consecutive lines hyphenated
	 */
	if (  gbrS.lineHyphenated )
		linesHyphenated += 1;
	else
		linesHyphenated = 0;

	/* set these - the leading may force the line to be replaced & rebroken */
	BRKMaxLineVals( lineData, initialLead, initialBaseline, (gbrS.cB.theChRec-1)->character>=scWordSpace?(gbrS.cB.theChRec-1)->escapement:0 );

	for ( ; (long)gbrS.cB.streamCount > (*specRec+1)->offset(); (*specRec)++ )
		;

	if (  gbrS.foundCharIndent )
		BRKSetCharIndent(  chRec, startCount, count,  gbrS.letterSpaceAdj  );

	(gbrS.cB.theChRec-1)->flags.SetLineBreak();
	scAssert( count >= 0 );


#if _DEBUG
	eBreakType ret = gbrS.cB.breakVal;
	gbrS.Init();
	return ret;
#else
	return gbrS.cB.breakVal;
#endif

}

/************************************************************************/

static void BRKMaxLineVals( scLINERefData&		lineData,
							MicroPoint		initialLead,
							eFntBaseline	baseline,
							MicroPoint		lastChWidth )
{
	for (int i = gbrS.cB.lineVal; i--; ) {
		if ( initialLead < gbrS.fMaxLineVals[i].fMaxLead.GetLead() ) {
			if ( lineData.fEndLead.GetLead() < gbrS.fMaxLineVals[i].fMaxLead.GetLead() ) {
				lineData.fEndLead		= gbrS.fMaxLineVals[i].fMaxLead;
				lineData.SetMaxLeadSpec( gbrS.fMaxLineVals[i].fSpecRec->spec() );
			}
		}	

		scXRect inkExtents( gbrS.fMaxLineVals[i].fMaxInkExtents );
		inkExtents.Translate( lineData.fOrg );
		lineData.fInkExtents.Union( inkExtents );
	}

	
	if ( lineData.IsHorizontal() ) {
		lineData.fInkExtents.x1 += gbrS.minRelPosition;
		lineData.fInkExtents.x2 += ( lineData.fComputedLen - lastChWidth );
	}
	else {
		lineData.fInkExtents.y1 += gbrS.minRelPosition;
		lineData.fInkExtents.y2 += ( lineData.fComputedLen - lastChWidth );
	}

	
#if SCDEBUG > 1
	SCDebugTrace( 4, scString( "BRKMaxLineVals: (%d %d %d %d)\n" ),
				  muPoints( lineData.fInkExtents.x1 ), muPoints( lineData.fInkExtents.y1 ),
				  muPoints( lineData.fInkExtents.x2 ), muPoints( lineData.fInkExtents.y2 ) );	
#endif
}

/************************************************************************/

static inline BOOL BRKExceedVals(  MicroPoint adjustableSpace )
{
	return gbrS.cB.minGlue + gbrS.tmpMinGlue > adjustableSpace;
}

/************************************************************************/
/* this is the routine that effectively does the quality line breaking
 * up to this point we have simply been looking for a condition to
 * have been exceeded, now we may search to find a good break point
 */

static UCS2 BRKLineDecision( MicroPoint )
{
	CandBreak	*choice;
	
	if ( ( gbrS.effectiveRag & eRagFlag ) == eRagJustified )
		choice	= BRKHyphenateJust( );
	else
		choice	= BRKHyphenateRag( );

	if ( choice->breakCount == 0	&& 
			gbrS.cB.breakCount > 1	&& 
			choice->streamCount == choice->startCount )
		choice++;

	choice->spaceCount		= (ushort)(choice->spaceCount - choice->trailingSpaces);
	choice->wsSpaceCount	= (ushort)(choice->wsSpaceCount - choice->trailingSpaces);
	gbrS.cB = *choice;

	/* this is a fix for a bug in the character loop,
	 * in the loop the spec is incremenented before the character is
	 * called, if the character forces a line break, the spec at the
	 * end of the line ( stored in choice->spec ) is invalid, the following
	 * fixes this
	 */

	while ( gbrS.cB.theChRec->character && 
			gbrS.cB.lineVal 			&& 
			gbrS.cB.specRec->offset() >= (long)gbrS.cB.streamCount ) {
		gbrS.cB.specRec--;
		gbrS.cB.spec = gbrS.cB.specRec->spec();
		gbrS.cB.lineVal--;
	}

	scCachedStyle::GetCachedStyle( gbrS.cB.spec );

	if ( gbrS.cB.breakVal == eHyphBreak ) {
		gbrS.lineHyphenated = true;
		if ( gbrS.cB.theChRec->flags.IsKernPresent() )
			gbrS.cB.curBox += BRKKernCorrection(  gbrS.cB.theChRec );
	}
	else {
		gbrS.lineHyphenated = false;
		BRKRepairLastSpace( gbrS.cB.theChRec, gbrS.cB.trailingSpaces );
	}

	return gbrS.cB.theChRec->character;
}


/************************************************************************/
/* we have tripped on a word space, if it is the first word space we
 * must do some housekeeping, the first word space test performs two
 * operations: 1. it tests to see if we have exceeded the measure and
 * 2. it counts actual interword spaces areas, we need to know that
 * for microjustification
 */

static inline eBreakEvent bmBRKWordSpace(  )
{
	BOOL bFirstGlue = gbrS.firstGlue;

	if ( gbrS.firstGlue ) {
		gbrS.firstBox = true;
		gbrS.firstGlue = false;
		gbrS.cB.wsSpaceCount++;
	}

	gbrS.fNoStartline = false;
	gbrS.fLastHangable = 0;

	gbrS.tmpOptGlue += scCachedStyle::GetCurrentCache().GetOptWord();
	gbrS.tmpMinGlue += scCachedStyle::GetCurrentCache().GetMinWord();
	gbrS.tmpMaxGlue += scCachedStyle::GetCurrentCache().GetMaxWord();

	gbrS.cB.theChRec->escapement = scCachedStyle::GetCurrentCache().GetOptWord();

	gbrS.cB.trailingSpaces++;
	gbrS.cB.spaceCount++;
	gbrS.cB.streamCount++;
	gbrS.cB.theChRec++;

	if (!bFirstGlue)
	{
		MicroPoint adjustableSpace = gbrS.desiredMeasure - gbrS.cB.curBox;
		BRKSetCandBreak( eCharBreak );
		if (BRKExceedVals (adjustableSpace))
		{
			BRKLineDecision (0);
			return BRKExitLoop( );
		}
	}

	return in_line;
}

/************************************************************************/
/* does much the same as the word space break, except these are characters,
 * it also checks for hyphens
 */

static inline void BRKSetFirstBox(	)
{
	 gbrS.firstGlue 		= true;
	 gbrS.firstBox			= false;
	 gbrS.cB.minGlue		+=	gbrS.tmpMinGlue;
	 gbrS.cB.optGlue		+=	gbrS.tmpOptGlue;
	 gbrS.cB.maxGlue		+=	gbrS.tmpMaxGlue;
	 gbrS.tmpMinGlue		=  gbrS.tmpOptGlue	=  gbrS.tmpMaxGlue	= 0;
	 gbrS.cB.trailingSpaces = 0;
}

/************************************************************************/

static inline eBreakEvent bmBRKChar(  )
{
	MicroPoint		adjustableSpace;

	if ( gbrS.cB.theChRec->character >= 256 ) {
		adjustableSpace = gbrS.desiredMeasure - gbrS.cB.curBox;
		BRKSetCandBreak( eCharBreak );

		if ( BRKExceedVals( adjustableSpace ) ) {
			BRKLineDecision( 0 );
			return BRKExitLoop( );
		}
		gbrS.cB.curBox	+=	gbrS.cB.theChRec ->escapement;

		gbrS.cB.chCount++;
		gbrS.cB.streamCount++;
		gbrS.cB.theChRec++;

		return in_line;
	}

	if ( gbrS.firstBox ) {
		adjustableSpace =  gbrS.desiredMeasure	-  gbrS.cB.curBox ;

			/* at the start of every word set a potential break point */
		BRKSetCandBreak(  eCharBreak );
		if ( BRKExceedVals(  adjustableSpace ) ) {
			BRKLineDecision( 0 );
			return BRKExitLoop(  );
		}

		BRKSetFirstBox(  );
	}

	 gbrS.cB.curBox  +=  gbrS.cB.theChRec ->escapement;

	 gbrS.cB.chCount++;
	 gbrS.cB.streamCount++;
	 gbrS.cB.theChRec++;

	 return in_line;
}

/* ==================================================================== */

static inline eBreakEvent bmBRKField()
{
	USTR	ustr;
	TypeSpec		spec = gbrS.cB.spec;
	
	clField& field = APPCreateField(gbrS.theBreakColH->GetStream(), gbrS.cB.theChRec->flags.GetField());
	field.content(ustr, gbrS.theBreakColH->GetAPPName(), spec );
	field.release();

	if ( gbrS.firstBox ) {
		MicroPoint adjustableSpace = gbrS.desiredMeasure - gbrS.cB.curBox;

			/* at the start of every word set a potential break point */
		BRKSetCandBreak(  eCharBreak );
		if ( BRKExceedVals(  adjustableSpace ) ) {
			BRKLineDecision( 0 );
			return BRKExitLoop(  );
		}

		BRKSetFirstBox(  );
	}
	
	if ( ustr.len )
		gbrS.cB.theChRec->escapement = UnivStringWidth( ustr, 0, spec );

	gbrS.cB.curBox	+=	gbrS.cB.theChRec ->escapement;
	
	gbrS.cB.chCount++;
	gbrS.cB.streamCount++;
	gbrS.cB.theChRec++;

	return in_line;
}

/************************************************************************/

static inline eBreakEvent BRKLoopBody(	)
{
	register ushort theCharacter = gbrS.cB.theChRec->character;
	register eBreakEvent breaktype = in_line;

	/* increment the spec counter if necessary,
	 * NOTE - this increments the spec too soon!!!
	 * If the next character is the break char then
	 * the spec will be inaccurate, there is a fix
	 * for this in the line ending decision logic,
	 * it is commented as such, the cleaner fix
	 * would slow the code down too much
	 * (sorry for any confusion WAM)
	 */
	if ( gbrS.cB.streamCount >= gbrS.theSpecRec->offset() ) {
		if ( theCharacter != scEndStream ) {
			/* do not advance spec unless we have characters */
			gbrS.cB.spec = BRKUpdateSpec( gbrS.theSpecRec );
			gbrS.theSpecRec++;
		}
	}

	/* dispatch the character to the appropriate routine */

	gbrS.cB.theChRec->flags.ClrVarious();

	
	if ( ! ( theCharacter & 0xFFC0 ) ) {
		if ( theCharacter == scWordSpace )
			breaktype = bmBRKWordSpace();
		else if ( theCharacter & 0x0030 ) {
			switch ( theCharacter ) {
			case scBreakingHyphen:
				breaktype = bmBRKHyphen();
				break;
			default:
				breaktype = bmBRKChar();
				break;
			}
		} 
		else
			breaktype = (*gbrS.breakMach[theCharacter])();
	} 
	else
		breaktype = bmBRKChar();

	return breaktype;
}

/************************************************************************/

static eBreakEvent BRKTheLoop(	)
{
	eBreakEvent bt;
	while ( ( bt = BRKLoopBody() ) == in_line )
		;
	return bt;
}

/************************************************************************/

static short BRKPerformDiscHyphen( CharRecordP	theChar,
								   CharRecordP	lastChRec,
								   Hyphen		*hyphens )
{
	UCS2		ch;
	short		charCount		= 1,
				numBreaks		= 0;
	int 		j;
	BOOL		hitLowerCase	= false;
	BOOL		hitUpperCase	= false;

	for ( ; theChar <= lastChRec; theChar++, charCount++ ) {

		ch =  theChar->character;

		if ( ch == scBreakingHyphen )
			return 0;

		if (  theChar->flags.IsDiscHyphen() ) {
			if ( charCount >= scCachedStyle::GetCurrentCache().GetPreHyph() ) {
				hyphens[numBreaks].offset	= charCount;
				hyphens[numBreaks].rank 	= eDiscHyphRank;
				numBreaks++;
			}
		}

		if ( ch < 256 && CTIsAlpha( ch ) ) {
			if ( CTIsLowerCase( ch ) )
				hitLowerCase	= true;
			else if ( CTIsUpperCase( ch ) )
				hitUpperCase	= true;
		}
		else if ( CTIsSpace( ch ) )
			if ( ch != scFixRelSpace && ch != scFixAbsSpace )
				break;
	}

	if ( ( ! scCachedStyle::GetCurrentCache().GetAcronymHyphs() && ! hitLowerCase ) || ( ! scCachedStyle::GetCurrentCache().GetCaseHyphs() && hitUpperCase ) )
		return 0;

	charCount = (short)(charCount - scCachedStyle::GetCurrentCache().GetPostHyph());
	for ( j = numBreaks - 1; j >= 0 && numBreaks > 0; j-- )
		if ( hyphens[j].offset >= charCount )
			numBreaks--;

	return numBreaks;
}

/************************************************************************/

static short BRKPerformHyphenation( CharRecordP firstChRec,
									CharRecordP lastChRec,
									Hyphen* 	hyphens )
{
	CharRecordP theChar;
	UCS2	ch;
	UCS2	hyphWord[64];
	short		hyphArray[64];
	short		hLen			= 0;
	BOOL		hitLowerCase	= false;
	BOOL		hitUpperCase	= false;
	short		numBreaks		= 0;
	short		charCount		= 1;

	memset( hyphArray, 0, sizeof( short ) * 64 );
	memset( hyphWord, 0, sizeof( UCS2 ) * 64 );

	for ( theChar = firstChRec; theChar <= lastChRec; theChar++ ) {
		ch	=  theChar->character;
		if ( ch < 256 && CTIsAlpha( ch ) )
			break;
	}

	for ( ; theChar <= lastChRec; theChar++, charCount++ ) {

		ch =  theChar->character;

		if ( ch == scBreakingHyphen ) {
			return 0;
		}
		else if ( theChar->flags.IsDiscHyphen() ) {
			return BRKPerformDiscHyphen(  firstChRec, lastChRec, hyphens );
		}
		else if ( ch < 256 && CTIsAlpha( ch ) ) {
			if ( hLen < 63 ) {
				if ( CTIsLowerCase( ch ) )
					hitLowerCase	= true;
				else if ( CTIsUpperCase( ch ) )
					hitUpperCase	= true;
				hyphWord[ hLen++ ]	= CTToLower( ch );
			}
		}
		else if ( ch != scFixRelSpace && ch != scFixAbsSpace ) {	/* hit delimiter */
			break;
		}
	}

	if ( hLen < scCachedStyle::GetCurrentCache().GetMaxWordHyph()
			|| ( ! scCachedStyle::GetCurrentCache().GetAcronymHyphs() && ! hitLowerCase )
			|| ( ! scCachedStyle::GetCurrentCache().GetCaseHyphs() && hitUpperCase ) )
		return 0;

	return numBreaks;
}

/************************************************************************/
/* This is called only in the case where even the first word (or the	*/
/* first legal portion of it before a hyphen) will not fit on the line. */
/* Therefore, we add a hyphen after every character to force a break.	*/

#ifdef scForceBreakFirstWord

static void BRKForceHyphens( CandBreak	*startBreak,
							 CandBreak	*endBreak )
{
	CandBreak		savedEndBreak;

	savedEndBreak	= *endBreak;

	/* We are resetting the break machine to the start of the word. */
	/* The values from this point on in the machine will be over-	*/
	/* written. 													*/

	gbrS.tmpMinGlue = (startBreak+1)->minGlue - startBreak->minGlue;
	gbrS.tmpOptGlue = (startBreak+1)->optGlue - startBreak->optGlue;
	gbrS.tmpMaxGlue = (startBreak+1)->maxGlue - startBreak->maxGlue;
	gbrS.cB 		= *startBreak;
	gbrS.firstBox	= true; 		/* signal start of word */

	while ( gbrS.theSpecRec->offset() > gbrS.cB.streamCount ) // reset the spec to the start of the word
		gbrS.theSpecRec--;

	while ( true ) {

		BRKLoopBody(  );

		if ( gbrS.cB.chCount == savedEndBreak.chCount )
			break;

		BRKSetCandBreak(  eHyphBreak );
		if ( scCachedStyle::GetCurrentCache().GetHyphLanguage() != Japanese )
			(gbrS.cB.theChRec - 1)->flags.SetAutoHyphen( eGoodHyphRank );

		if ( ( gbrS.effectiveRag & eRagFlag ) == eRagJustified ) {
			if ( gbrS.cB.curBox + gbrS.cB.minGlue >  gbrS.desiredMeasure )
				break;
		}
		else {
			if ( gbrS.cB.curBox + gbrS.cB.optGlue >  gbrS.desiredMeasure )
				break;
		}
	}

	savedEndBreak.breakCount	= gbrS.cB.breakCount;
	gbrS.candBreak[gbrS.cB.breakCount]	= savedEndBreak;

	if ( gbrS.cB.breakCount >= (MAXBREAKVALS-1) )
		ShuffleBreakCandidates();
	else
		gbrS.cB.breakCount++;
}

#endif /* forceBreakFirstWord */

/************************************************************************/

#define NotHyphBreak( c )		( ((c)!=eHyphBreak) && ((c)!=eHardHyphBreak ) )
#define useBadHyphens			false

static void BRKAddHyphens( CandBreak	*startBreak,
						   CandBreak	*endBreak )
{
	CandBreak	savedEndBreak;
	CharRecordP startChRec;
	CharRecordP stopChRec;
	Hyphen		hyphens[64];
	short		i,
				offset,
				prevOffset		= 0,
				numHyphens;

	if ( startBreak == endBreak || endBreak->curBox + endBreak->optGlue <=	gbrS.desiredMeasure )
		return;

	if ( ! gbrS.allowHyphens || ! scCachedStyle::GetCurrentCache().GetHyphenate() ) {
		
#ifdef scForceBreakFirstWord
		if ( startBreak == &gbrS.candBreak[0] && startBreak->streamCount == startBreak->startCount )
			BRKForceHyphens( startBreak, endBreak );
#endif
		
		return;
	}

	savedEndBreak	= *endBreak;
	startChRec		= startBreak->theChRec;
	stopChRec		= endBreak->theChRec;

	numHyphens	= BRKPerformHyphenation(  startChRec, stopChRec, hyphens );
	if ( numHyphens == 0 ) {
		
#ifdef scForceBreakFirstWord
		if ( startBreak == &gbrS.candBreak[0] && startBreak->streamCount == startBreak->startCount )
			BRKForceHyphens(  startBreak, endBreak );
#endif
		
		return;
	}

	/* We are resetting the break machine to the start of the word. */
	/* The values from this point on in the machine will be over-	*/
	/* written. The end of word break, since it is past the measure,*/
	/* will never be restored, and the final break will now be the	*/
	/* last hyphen break.											*/
	/* Use endBreak to return the last hyphen breakpoint we find.	*/

	gbrS.tmpMinGlue = (startBreak+1)->minGlue - startBreak->minGlue;
	gbrS.tmpOptGlue = (startBreak+1)->optGlue - startBreak->optGlue;
	gbrS.tmpMaxGlue = (startBreak+1)->maxGlue - startBreak->maxGlue;
	gbrS.cB 		= *startBreak;
	gbrS.firstBox	= true; 		/* signal start of word */

	for ( i = 0; i < numHyphens; i++ ) {
		if ( !useBadHyphens && hyphens[i].rank == eBadHyphRank )
			continue;

		offset	= hyphens[i].offset;
		for ( ; prevOffset < offset; prevOffset++ )
			BRKLoopBody(  );

		BRKSetCandBreak(  eHyphBreak );

		if ( !(gbrS.cB.theChRec-1)->flags.IsDiscHyphen() )
			(gbrS.cB.theChRec-1)->flags.SetAutoHyphen( hyphens[i].rank );

		if ( ( gbrS.effectiveRag & eRagFlag ) == eRagJustified ) {
			if ( gbrS.cB.curBox + gbrS.cB.minGlue >  gbrS.desiredMeasure )
				break;
		}
		else {
			if ( gbrS.cB.curBox + gbrS.cB.optGlue >  gbrS.desiredMeasure )
				break;
		}
	}

#ifdef scForceBreakFirstWord
		/* if the first hyphen exceeded the measure, we may */
		/* want to force a break.							*/
	if ( i == 0 && startBreak == &gbrS.candBreak[ 0 ]
				&& startBreak->streamCount == startBreak->startCount ) {
		BRKForceHyphens(  startBreak, endBreak );
		return;
	}
#endif

	savedEndBreak.breakCount	= gbrS.cB.breakCount;
	gbrS.candBreak[gbrS.cB.breakCount]	= savedEndBreak;

	if ( gbrS.cB.breakCount >= (MAXBREAKVALS-1) )
		ShuffleBreakCandidates();
	else
		gbrS.cB.breakCount++;
}

/************************************************************************/

static CandBreak *BRKHyphenateRag(	)
{
	CandBreak	*theBreak,
				*choice 		= NULL;
	CandBreak	*startWord		= NULL,
				*endWord;
	long	bCount			= gbrS.cB.breakCount;
	MicroPoint	diff,
				bestDiff		= LONG_MAX,
				lineSpace;
	BOOL		lineDiff		= true; /* irrelevant unless otherwise set */

	/* If there is a hyphenation zone, first try to find a non-hyphen break.
	 * If that fails, find the start and end of the word straddling the
	 * line break, call hyphenation routine, and call line decision to
	 * find the best break.
	 */

	if ( gbrS.hyphenationZone ) {
		theBreak	= gbrS.candBreak + gbrS.cB.breakCount - 1;

		for ( ; bCount-- > 0; theBreak-- ) {

			lineSpace	= theBreak->curBox + theBreak->optGlue;
			if ( scCachedStyle::GetCurrentCache().GetDiffRagZone() )
				lineDiff	= ABS(	gbrS.lastLineLen - lineSpace ) > scCachedStyle::GetCurrentCache().GetDiffRagZone();

			/* look for a non-hyphen break and a line with a sufficient diff zone */
			if ( NotHyphBreak( theBreak->breakVal )
					&&	gbrS.desiredMeasure >= lineSpace
					&&	gbrS.desiredMeasure <= lineSpace + gbrS.hyphenationZone
					&& lineDiff )
			{
				choice		= theBreak;
				break;
			}			
		}
	}

	if ( choice )
		return choice;

	/* First, get closest word break greater than desired measure */

	bCount		= gbrS.cB.breakCount;
	endWord 	= theBreak	= gbrS.candBreak + gbrS.cB.breakCount - 1;
	for ( ; bCount-- > 0; theBreak-- ) {

		diff	= theBreak->curBox + theBreak->optGlue - gbrS.desiredMeasure;
		if ( diff <= 0 )
			break;

		if ( NotHyphBreak( theBreak->breakVal ) && diff < bestDiff ) {
			bestDiff	= diff;
			endWord 	= theBreak;
		}
	}

	/* Next, get closest word break less than desired measure */

	startWord	= endWord;
	bCount++;				/* reset it to what it should be */
	for ( ; bCount-- > 0; theBreak-- ) {
		if ( NotHyphBreak( theBreak->breakVal ) ) {
			startWord	= theBreak;
			break;
		}
	}

	BRKAddHyphens(	startWord, endWord );
	return BRKLineDecisionRag(	);
}

/************************************************************************/

static CandBreak *BRKHyphenateJust(  )
{
	CandBreak		*theBreak;
	CandBreak				*choice 		= NULL,
							*startWord		= NULL,
							*endWord;
	long				bCount		= gbrS.cB.breakCount;
	long						endCount = 0;
	MicroPoint		adjustableSpace,
								diff,
								bestDiff		= LONG_MAX;

	endWord 	= theBreak	= gbrS.candBreak + bCount - 1;
	for ( ; bCount-- > 0; theBreak-- ) {

		/* amount of space we have to play with */
		adjustableSpace =  gbrS.desiredMeasure + theBreak->fHangable - theBreak->curBox;

		diff = adjustableSpace - theBreak->optGlue;
		if ( diff <= 0 && NotHyphBreak( theBreak->breakVal ) ) {
			endWord 	= theBreak;
			endCount	= bCount + 1;
		}

		diff = ABS( diff );

		if ( NotHyphBreak( theBreak->breakVal ) && diff < bestDiff ) {
			if ( adjustableSpace <= theBreak->maxGlue && adjustableSpace >= theBreak->minGlue ) {
				choice		= theBreak;
				bestDiff	= diff;
			}
		}
		if ( diff > bestDiff )
			break;
	}

	if ( choice )
		return choice;

	theBreak	= startWord = endWord;
	bCount		= endCount; 			/* restore its last value */
	for ( ; bCount-- > 0; theBreak-- ) {
		if ( NotHyphBreak( theBreak->breakVal )
				&& theBreak->curBox + theBreak->optGlue <  gbrS.desiredMeasure	+ theBreak->fHangable ) {
			startWord	= theBreak;
			break;
		}
	}

	BRKAddHyphens(	startWord, endWord );
	return BRKLineDecisionJust(  );
}

/************************************************************************/

static CandBreak *BRKLineDecisionRag(  )
{
	CandBreak	*theBreak,
				*choice 		= NULL;
	long	bCount			= gbrS.cB.breakCount;
	MicroPoint	lineSpace;

	/* there is a very strange bug here in that the char plus hyphen may be
	 * chosen instead of the entire word because the hyphen may be wider than
	 * the trailing letter(s)
	 *
	 * Note: We only get here if hyphenationZone is off or we failed to find
	 * a good non-hyphen break.
	 */
	 

	if ( scCachedStyle::GetCurrentCache().GetDiffRagZone() ) {	
		theBreak	= gbrS.candBreak + gbrS.cB.breakCount - 1;

		for ( ; bCount-- > 0; theBreak-- ) {

			lineSpace	= theBreak->curBox + theBreak->optGlue;
			/* this reflects space changed by hyphenation spelling changes */

			if (  gbrS.desiredMeasure >= lineSpace && ABS( gbrS.lastLineLen - lineSpace ) > scCachedStyle::GetCurrentCache().GetDiffRagZone() ) {
				choice		= theBreak;
				break;
			}			
		}
	}

	/* resort to worst case if necessary */
	/* if !GetDiffRagZone() && !hyphenationZone, we will fall through to here */

	if ( choice == NULL ) {
		bCount		= gbrS.cB.breakCount;
		choice		= theBreak		= gbrS.candBreak + gbrS.cB.breakCount - 1;

		for ( ; bCount-- > 0; theBreak-- ) {

			lineSpace		= theBreak->curBox + theBreak->optGlue;
			/* this reflects space changed by hyphenation spelling changes */

			if (  gbrS.desiredMeasure + theBreak->fHangable >= lineSpace ) {
				choice		= theBreak;
				break;
			}			
		}
	}

	return choice;
}

/************************************************************************/

static CandBreak *BRKLineDecisionJust(	)
{
	CandBreak	*theBreak,
				*choice;
	long	bCount			= gbrS.cB.breakCount;
	MicroPoint	lineSpace,
				diff,
				bestDiff		= LONG_MAX;

	choice		= theBreak	= gbrS.candBreak + bCount - 1;

	for ( ; bCount-- > 0; theBreak-- ) {
		lineSpace	= theBreak->curBox + theBreak->minGlue;
		/* this reflects space changed by hyphenation spelling changes */

		diff		= lineSpace - gbrS.desiredMeasure;
		if ( diff < theBreak->fHangable && ABS( diff ) < bestDiff ) {
			choice		= theBreak;
			bestDiff	= ABS( diff );
		}
		if ( diff > bestDiff )
			break;
	}

	return choice;
}

/************************************************************************/
/* if we break on a hyphenation point and the character is kerned with
 * the next character we have an incorrect line length, because of the
 * kern built into the characters escapement, here we get that correction
 */

static MicroPoint BRKKernCorrection( CharRecordP aChRec )
{
	return ( scCachedStyle::GetCurrentCache().GetEscapement( aChRec->character ) - aChRec->escapement );
}

/************************************************************************/
/* for non-justified lines this places the line */

static void BRKPlaceLine( scMuPoint&			lineOrigin,
						  MicroPoint&		measure,
						  const scFlowDir&	fd )
{
	MicroPoint	actualMeasure,
				translation;

	actualMeasure = gbrS.cB.curBox + gbrS.cB.optGlue;
	if ( gbrS.cB.fillSpCount ) {
		MicroPoint	fill;
		long		count;
		CharRecordP tmpChRec;

		fill = gbrS.desiredMeasure - actualMeasure;
		fill = scRoundMP( (double)fill / gbrS.cB.fillSpCount );
		
		actualMeasure =  gbrS.desiredMeasure;
		for ( tmpChRec = gbrS.cB.theChRec, count = gbrS.cB.fillSpCount;
					tmpChRec >= gbrS.gStartRec && count; tmpChRec-- ) {
			if ( tmpChRec->character == scFillSpace )
				tmpChRec->escapement = (GlyphSize)fill;
		}
	}

	switch ( gbrS.effectiveRag & eRagFlag ) {
		default:
		case eRagRight:
			translation = gbrS.brkLeftMargin;
			break;
		case eRagLeft:
			if ( gbrS.effectiveRag & (int)eHangPuncRight )
				 gbrS.desiredMeasure += BRKHangPuncRightAdjust(  );
			translation =  gbrS.desiredMeasure - actualMeasure + gbrS.brkLeftMargin;

			if ( !gbrS.lineHyphenated ) {
				/* this accounts for any track kerning - no need to worry about with
				 * hyphenation because, we want trackkerning between the hyphen
				 * and the last character - and the hyphen escapement does not
				 * include any track kerning
				 */
				translation += scCachedStyle::GetCurrentCache().GetOptLSP();
			}
			break;
		case eRagCentered:
			if ( gbrS.lineHyphenated )
				translation = scRoundMP( (double)( gbrS.desiredMeasure - actualMeasure) / 2 ) + gbrS.brkLeftMargin;
			else
				translation = scRoundMP( (double)( gbrS.desiredMeasure - actualMeasure + scCachedStyle::GetCurrentCache().GetOptLSP()) / 2 ) + gbrS.brkLeftMargin;
			break;
	}

	if ( fd.IsHorizontal() )
		lineOrigin.Translate( translation, 0 );
	else
		lineOrigin.Translate( 0, translation );

	if ( gHiliteSpaces )
		actualMeasure += gbrS.totalTrailingSpace;

	measure = actualMeasure;
}

/************************************************************************/
/* handle adjustment for hanging punctuation on the right */

static MicroPoint BRKHangPuncRightAdjust(  )
{
	CharRecordP lastCharOnLine;

	if ( gbrS.lineHyphenated )
		return scCachedStyle::GetCurrentCache().GetRightHangValue( scCachedStyle::GetCurrentCache().GetHyphChar() );

	lastCharOnLine = BRKLastCharOnLine( gbrS.cB.theChRec - 1 );
	if ( CTIsPunc(	lastCharOnLine->character  ) )
		return scCachedStyle::GetCurrentCache().GetRightHangValue( lastCharOnLine->character );
	else
		return 0L;
}

/************************************************************************/

#define COMP_LETTERSPACES( charCount, spaceCount, chRec ) \
	((long)( charCount-spaceCount-1 + ((chRec-1)->flags.IsHyphPresent() ? 1:0) ))

static void BRKJustifyLine(  )
{
	GlyphSize	glueSpace,
				adjustableSpace;
	long		lspSpaces;

	if ( gbrS.effectiveRag & (int)eHangPuncRight )
		 gbrS.desiredMeasure += BRKHangPuncRightAdjust(  );

	glueSpace =  (GlyphSize)(gbrS.desiredMeasure - gbrS.cB.curBox);

	if ( glueSpace < 0 ) {
		gbrS.desiredMeasure += gbrS.cB.fHangable;
		glueSpace =  (GlyphSize)(gbrS.desiredMeasure - gbrS.cB.curBox);
	}

	if ( !gbrS.lineHyphenated )
		glueSpace += scCachedStyle::GetCurrentCache().GetOptLSP();
	
	if ( gbrS.cB.spaceCount ) {
	
		gbrS.justSpace = scRoundGS( (double)glueSpace / gbrS.cB.spaceCount );

		if ( gbrS.justSpace < 0 ) {
			lspSpaces		= COMP_LETTERSPACES( gbrS.cB.chCount, 0, gbrS.cB.theChRec);
			if ( lspSpaces )
				gbrS.letterSpaceAdj = scRoundGS( (double)glueSpace / lspSpaces ); 
			else
				gbrS.letterSpaceAdj = glueSpace;				
			gbrS.justSpace		= 0;
		}
		else if ( gbrS.justSpace > scCachedStyle::GetCurrentCache().GetMaxWord() ) {
			lspSpaces		 = COMP_LETTERSPACES( gbrS.cB.chCount, gbrS.cB.spaceCount, gbrS.cB.theChRec);
			adjustableSpace = MPtoGS( glueSpace - gbrS.cB.maxGlue );
			if ( lspSpaces )
				gbrS.letterSpaceAdj = scRoundGS( (double)adjustableSpace / lspSpaces );
			else
				gbrS.letterSpaceAdj = adjustableSpace;										
			if ( gbrS.letterSpaceAdj < scCachedStyle::GetCurrentCache().GetMinLSP() ) {
				gbrS.letterSpaceAdj = scCachedStyle::GetCurrentCache().GetMinLSP();
				adjustableSpace = MPtoGS( glueSpace - scRoundMP( (double)gbrS.letterSpaceAdj * lspSpaces ) );
				gbrS.justSpace = scRoundGS( (double)adjustableSpace / gbrS.cB.spaceCount );
			}
			else if ( gbrS.letterSpaceAdj > scCachedStyle::GetCurrentCache().GetMaxLSP() ) {
				gbrS.letterSpaceAdj = scCachedStyle::GetCurrentCache().GetMaxLSP();
				adjustableSpace = MPtoGS( glueSpace - scRoundMP( (double)gbrS.letterSpaceAdj * lspSpaces ) );
				gbrS.justSpace = scRoundGS( (double)adjustableSpace / gbrS.cB.spaceCount );
			}
			else
				gbrS.justSpace = scCachedStyle::GetCurrentCache().GetMaxWord();
		}
		else if ( gbrS.justSpace < scCachedStyle::GetCurrentCache().GetMinWord() ) {
			lspSpaces		= COMP_LETTERSPACES( gbrS.cB.chCount, gbrS.cB.spaceCount, gbrS.cB.theChRec);
			adjustableSpace = MPtoGS( glueSpace - gbrS.cB.minGlue );
			
			if ( lspSpaces )
				gbrS.letterSpaceAdj = scRoundGS( (double)adjustableSpace / lspSpaces );
			else
				gbrS.letterSpaceAdj = adjustableSpace;			
				
			if ( gbrS.letterSpaceAdj < scCachedStyle::GetCurrentCache().GetMinLSP() ) {
				gbrS.letterSpaceAdj = scCachedStyle::GetCurrentCache().GetMinLSP();
				adjustableSpace = MPtoGS( glueSpace - scRoundMP( (double)gbrS.letterSpaceAdj * lspSpaces ) );
				gbrS.justSpace = scRoundGS( (double)adjustableSpace / gbrS.cB.spaceCount );
			}
			else if ( gbrS.letterSpaceAdj > scCachedStyle::GetCurrentCache().GetMaxLSP() ) {
				gbrS.letterSpaceAdj = scCachedStyle::GetCurrentCache().GetMaxLSP();
				adjustableSpace = MPtoGS( glueSpace - scRoundMP( (double)gbrS.letterSpaceAdj / lspSpaces ) );
				gbrS.justSpace	= scRoundGS( (double)adjustableSpace / gbrS.cB.spaceCount );
			}
			else
				gbrS.justSpace = scCachedStyle::GetCurrentCache().GetMinWord();
		}
		BRKAdjustWordSpace( gbrS.cB.theChRec, 
							gbrS.justSpace, gbrS.cB.spaceCount,
							gbrS.cB.trailingSpaces );
	}
	else {
		lspSpaces		= COMP_LETTERSPACES( gbrS.cB.chCount, 0, gbrS.cB.theChRec);
		if ( lspSpaces )
			gbrS.letterSpaceAdj = scRoundGS( (double)glueSpace / lspSpaces );
		else
			gbrS.letterSpaceAdj = glueSpace;
			
#ifdef LimitLetterSpace
		/* should we constrain this to min/max letterspace */
		gbrS.letterSpaceAdj = MIN( gbrS.letterSpaceAdj, scCachedStyle::GetCurrentCache().GetMaxLSP() );
		gbrS.letterSpaceAdj = MAX( gbrS.letterSpaceAdj, scCachedStyle::GetCurrentCache().GetMinLSP() ); 			
#endif
	}
}

/************************************************************************/

static eBreakEvent bmBRKFixSpace(  )
{
	MicroPoint adjustableSpace;

	if ( gbrS.firstBox ) {
		adjustableSpace =  gbrS.desiredMeasure - gbrS.cB.curBox;

			/* at the start of every word set a potential break point */
		BRKSetCandBreak(  eCharBreak );
		if ( BRKExceedVals(  adjustableSpace ) ) {
			BRKLineDecision( 0 );
			return BRKExitLoop(  );
		}
		BRKSetFirstBox();
		
//		gbrS.firstGlue			= true;
//		gbrS.firstBox			= false;
//		gbrS.cB.minGlue 		+= gbrS.tmpMinGlue;
//		gbrS.cB.optGlue 		+= gbrS.tmpOptGlue;
//		gbrS.cB.maxGlue 		+= gbrS.tmpMaxGlue;
//		gbrS.tmpMinGlue 		= gbrS.tmpOptGlue = gbrS.tmpMaxGlue = 0;
//		gbrS.cB.trailingSpaces	= 0;
	}

	gbrS.fNoStartline = false;
	gbrS.fLastHangable = 0;

	gbrS.cB.curBox += gbrS.cB.theChRec->escapement;
	gbrS.cB.theChRec++;
	gbrS.cB.streamCount++;

	return in_line;
}

/************************************************************************/

static eBreakEvent bmBRKRelSpace(  )
{
	MicroPoint adjustableSpace;

	if ( gbrS.firstBox ) {
		adjustableSpace =  gbrS.desiredMeasure - gbrS.cB.curBox;

		/* at the start of every word set a potential break point */
		BRKSetCandBreak(  eCharBreak );
		if ( BRKExceedVals(  adjustableSpace ) ) {
			BRKLineDecision( 0 );
			return BRKExitLoop(  );
		}
		BRKSetFirstBox();
//		gbrS.firstGlue			= true;
//		gbrS.firstBox			= false;
//		gbrS.cB.minGlue 		+= gbrS.tmpMinGlue;
//		gbrS.cB.optGlue 		+= gbrS.tmpOptGlue;
//		gbrS.cB.maxGlue 		+= gbrS.tmpMaxGlue;
//		gbrS.tmpMinGlue 		= gbrS.tmpOptGlue = gbrS.tmpMaxGlue = 0;
//		gbrS.cB.trailingSpaces	= 0;
	}

	gbrS.fNoStartline = false;
	gbrS.fLastHangable = 0;

	gbrS.cB.curBox += SCRLUCompMP( scCachedStyle::GetCurrentCache().GetGlyphWidth(), (RLU) gbrS.cB.theChRec->escapement );
	gbrS.minRelPosition = MIN( gbrS.cB.curBox, gbrS.minRelPosition );
	gbrS.cB.theChRec++;
	gbrS.cB.streamCount++;

	return in_line;
}

/************************************************************************/

static eBreakEvent bmBRKHardReturn(  )
{
	gbrS.cB.theChRec->escapement = 0;
	gbrS.cB.theChRec++;
	gbrS.cB.streamCount++;

	BRKSetCandBreak(  eSpaceBreak );
	BRKLineDecision( 0 );
	return BRKExitLoop(  );
}

/************************************************************************/

static eBreakEvent bmBRKQuad(  )
{
	gbrS.cB.theChRec->escapement = 0;
	gbrS.cB.theChRec++;
	gbrS.cB.streamCount++;
	if ( gbrS.cB.theChRec->character == scEndStream )
		BRKSetCandBreak(  eEndStreamBreak );
	else
		BRKSetCandBreak( eSpaceBreak );

	BRKLineDecision( 0 );
	return BRKExitLoop(  );
}

/************************************************************************/

static eBreakEvent bmBRKVertTab(  )
{
	gbrS.cB.theChRec->escapement = 0;
	gbrS.cB.theChRec++;
	gbrS.cB.streamCount++;
	BRKSetCandBreak( eColumnBreak );

	BRKLineDecision( 0 );
	return BRKExitLoop(  );
}

/************************************************************************/

	static BOOL TabBreakChar( UCS2 theCh,
								UCS2 breakCh )
	{
		if ( breakCh == theCh )
			return true;
		else {
			switch (  theCh  ) {
				default:
					return false;
				case scEndStream:
				
/*				case wordSpace:*/
				case scTabSpace:
				case scFillSpace:
				/* vetical breaks */
				case scVertTab:
				/* horizontal breaks */
				case scHardReturn:				
				case scQuadCenter:
				case scQuadLeft:
				case scQuadRight:
				case scQuadJustify:
					return true;
			}
		}
	}

/************************************************************************/

	static MicroPoint	BRKNextTokenWidth( CharRecordP	chRec,
										   UCS2 		breakCh )
	{
		MicroPoint tokenWidth = 0;
		MicroPoint charWidth;
		UCS2	theCh;
		long			tStreamCount	= gbrS.cB.streamCount;
		scSpecRecord *		curSpecRec		= gbrS.theSpecRec;

		for ( theCh = chRec->character; 
				!TabBreakChar(theCh,breakCh); 
				chRec++,theCh = chRec->character ) {

			if ( (long)tStreamCount >= gbrS.theSpecRec->offset() ) {
				gbrS.cB.spec 
					= BRKUpdateSpec(  gbrS.theSpecRec );
				gbrS.theSpecRec++;
			}
			switch ( theCh ) {
				default:
					charWidth = chRec->escapement;
					break;
				case scWordSpace:
					charWidth = scCachedStyle::GetCurrentCache().GetOptWord();
					break;
				case scFixRelSpace:
					charWidth = SCRLUCompGS( scCachedStyle::GetCurrentCache().GetSetSize(),(RLU)chRec->escapement );
					break;					
			}
			
			tokenWidth += charWidth;
		}

		if ( curSpecRec != gbrS.theSpecRec ) {
			gbrS.theSpecRec = curSpecRec;
			gbrS.cB.spec
				= BRKUpdateSpec(  gbrS.theSpecRec );
		}

		return tokenWidth;
	}


/************************************************************************/

static void BRKSetCharIndent( 
	
	CharRecordP chRec,	/* the character array */
	long	startCount, /* count into char array that starts line */
	long	count,		/* count into char array of end of line */
	MicroPoint	letterSpace )
{
	MicroPoint indent;
	
	for ( indent = 0, chRec += startCount; count--; chRec++ ) {
		switch ( chRec->character ) {
			default:
				if ( LETTERSPACE( chRec ) )
					indent += (chRec->escapement + letterSpace);
				else
					indent += chRec->escapement;
				break;
			case scFixRelSpace:
				indent += SCRLUCompMP( scCachedStyle::GetCurrentCache().GetGlyphWidth(), (RLU)chRec->escapement );
				break;
		}
	}
	gbrS.foundCharIndent = false;
}

/************************************************************************/

static eBreakEvent bmBRKTab(  )
{
	scTabInfo	tabInfo;
	MicroPoint	currentPosition,
				nextTokenWidth	= 0,
				alignTokenWidth = 0;
	CharRecordP tabChRec		= gbrS.cB.theChRec;

	BRKSetCandBreak(  eCharBreak );

	tabChRec->escapement = 0;
	currentPosition = gbrS.cB.curBox +
					  gbrS.cB.optGlue + gbrS.tmpOptGlue +
					  gbrS.brkLeftMargin + gbrS.theLineOrg;
					
	TSTabInfo( gbrS.pspec_,
			   gbrS.cB.spec,
			   tabInfo,
			   currentPosition,
			   0,
			   gbrS.theLineCount );

	switch ( tabInfo.tabAlign ) {
		default:
		case eTBLeftAlign:
			break;
		case eTBRightAlign:
			alignTokenWidth = nextTokenWidth = BRKNextTokenWidth( gbrS.cB.theChRec+1,'\0');
			break;
		case eTBDecimalAlign:
			alignTokenWidth = BRKNextTokenWidth( gbrS.cB.theChRec + 1, scCachedStyle::GetCurrentCache().GetDecimalChar() );
						
			nextTokenWidth = BRKNextTokenWidth( gbrS.cB.theChRec+1,'\0');
			break;
		case eTBCenterAlign:
			nextTokenWidth = BRKNextTokenWidth( gbrS.cB.theChRec + 1, '\0' );
			alignTokenWidth = scRoundMP( (double)nextTokenWidth / 2 );
			break;
	}
	
	tabChRec->escapement = (GlyphSize)(tabInfo.xPosition - currentPosition - alignTokenWidth);
	
	if ( tabChRec->escapement < 0 ) {
		alignTokenWidth 	+= tabChRec->escapement;		/* wam added 7/22 */
		tabChRec->escapement = 0;
	}

	if ( gbrS.desiredMeasure < currentPosition + tabChRec->escapement + nextTokenWidth ) {
		if ( gbrS.cB.curBox + gbrS.cB.optGlue + gbrS.tmpOptGlue > 0 ) {
			BRKLineDecision( 0 );
			return BRKExitLoop(  );
		}
	}

	gbrS.cB.curBox = tabInfo.xPosition - (	gbrS.brkLeftMargin +  gbrS.theLineOrg  ) - alignTokenWidth;

	gbrS.cB.theChRec++;
	gbrS.cB.streamCount++;

	gbrS.firstGlue		= true;
	gbrS.firstBox		= true; 	/* sil to true on 6/12/92 */
	gbrS.fNoStartline	= false;
	gbrS.fLastHangable	= 0;

	gbrS.cB.minGlue = gbrS.cB.optGlue = gbrS.cB.maxGlue = 0;
	gbrS.tmpMinGlue = gbrS.tmpOptGlue = gbrS.tmpMaxGlue = 0;
	gbrS.cB.trailingSpaces	= 0;
	gbrS.cB.wsSpaceCount	= 0;
	gbrS.cB.spaceCount		= 0;

		// define noLeftAlignTabbedLines and this
		// will allow tabbed lines to be none left aligned,
		// the manager of the spec system had better
		// guarantee that the values are reasonable
#ifndef noLeftAlignTabbedLines
	gbrS.cB.fillSpCount++;
#endif

	return in_line;
}

/************************************************************************/

static eBreakEvent bmBRKRule( void )
{
	if ( gbrS.cB.curBox ) {  
		BRKSetCandBreak( eCharBreak );
		BRKLineDecision( 0 );
		return BRKExitLoop(  );
	}
	
	CharRecordP chRec = gbrS.cB.theChRec;

	chRec->escapement = gbrS.desiredMeasure;

	gbrS.cB.curBox	+=	gbrS.cB.theChRec->escapement;
	gbrS.cB.streamCount++;
	gbrS.cB.theChRec++;

	if ( gbrS.cB.theChRec->character ) {
		BRKSetCandBreak(  eSpaceBreak );
		BRKLineDecision( 0 );
		return BRKExitLoop(  ); 
	}
	return in_line;
}

/************************************************************************/

static eBreakEvent bmBRKFillSpace(	)
{
	MicroPoint adjustableSpace;

	if ( gbrS.firstBox ) {
		adjustableSpace =  gbrS.desiredMeasure - gbrS.cB.curBox;

		BRKSetCandBreak(  eCharBreak );
		if ( gbrS.cB.minGlue > adjustableSpace ) {
			BRKLineDecision( 0 );
			return BRKExitLoop(  );
		}
		BRKSetFirstBox();
//		gbrS.firstGlue			= true;
//		gbrS.firstBox			= false;
//		gbrS.cB.minGlue 		+= gbrS.tmpMinGlue;
//		gbrS.cB.optGlue 		+= gbrS.tmpOptGlue;
//		gbrS.cB.maxGlue 		+= gbrS.tmpMaxGlue;
//		gbrS.tmpMinGlue 		= gbrS.tmpOptGlue = gbrS.tmpMaxGlue = 0;
//		gbrS.cB.trailingSpaces	= 0;
	}

	gbrS.fNoStartline = false;
	gbrS.fLastHangable = 0;

	gbrS.cB.streamCount++;
	gbrS.cB.fillSpCount++;
	gbrS.cB.theChRec->escapement = 0;
	gbrS.cB.theChRec++;

	return in_line;
}

/************************************************************************/

static eBreakEvent bmBRKHyphen(  )
{
	MicroPoint adjustableSpace;

	if ( gbrS.firstBox ) {
		adjustableSpace =  gbrS.desiredMeasure - gbrS.cB.curBox;

		BRKSetCandBreak(  eCharBreak );
		if ( gbrS.cB.minGlue > adjustableSpace ) {
			BRKLineDecision( 0 );
			return BRKExitLoop(  );
		}
		BRKSetFirstBox();
//		gbrS.firstGlue			= true;
//		gbrS.firstBox			= false;
//		gbrS.cB.minGlue 		+= gbrS.tmpMinGlue;
//		gbrS.cB.optGlue 		+= gbrS.tmpOptGlue;
//		gbrS.cB.maxGlue 		+= gbrS.tmpMaxGlue;
//		gbrS.tmpMinGlue 		= gbrS.tmpOptGlue = gbrS.tmpMaxGlue = 0;
//		gbrS.cB.trailingSpaces	= 0;
	}

	gbrS.fNoStartline = false;
	gbrS.fLastHangable = 0;

	gbrS.cB.curBox += gbrS.cB.theChRec->escapement;

	gbrS.cB.chCount++;
	gbrS.cB.streamCount++;
	gbrS.cB.theChRec++;

	BRKSetCandBreak(  eHardHyphBreak );

	return in_line;
}

/************************************************************************/
/* we have hit end of stream, check to see if we have exceeded measure,
 * if not longjmp out, otherwise back up to a reasonable break point
 * and get out
 */


static eBreakEvent bmBRKEndStream(	)
{
	MicroPoint adjustableSpace 
					=  gbrS.desiredMeasure - gbrS.cB.curBox;

	if ( gbrS.cB.maxGlue > adjustableSpace ) {
		BRKSetCandBreak(  eEndStreamBreak );
			if ( gbrS.cB.minGlue > adjustableSpace ) {
				BRKLineDecision( 0 );
				return BRKExitLoop(  );
			}
		}

	if ( gbrS.cB.optGlue > adjustableSpace ) {
		BRKSetCandBreak(  eEndStreamBreak );
		BRKLineDecision( 0 );
		return BRKExitLoop(  );
	}
	BRKSetCandBreak(  eEndStreamBreak );

	
	if ( BRKLineDecision( 0 ) == scEndStream )
		return end_of_stream_reached;
	
	return measure_exceeded;
}

/************************************************************************/
/* this sets up the linebreaker by initing the spec, performing indents,
 * rag zone control, etc. returns the desired measure of the line
 */


static MicroPoint BRKRagControl( CharRecordP	chRec,
								 MicroPoint 	x,		
								 MicroPoint 	y,
								 MicroPoint 	measure,
								 TypeSpec		spec,
								 ushort 		lineCount,
								 short			linesHyphenated )
{
	MicroPoint	dMeasure;

	scCachedStyle::GetCachedStyle(	spec );
	gbrS.effectiveRag	= scCachedStyle::GetParaStyle().GetRag();
	gbrS.brkRightMargin = scCachedStyle::GetParaStyle().GetRightBlockIndent();
	gbrS.theLineOrg 	= x;

		// CONSECUTIVE HYPHENATED LINE CONTROL
	if ( scCachedStyle::GetParaStyle().GetHyphenate() && linesHyphenated < scCachedStyle::GetParaStyle().GetMaxConsHyphs() )
		gbrS.allowHyphens = true;
	else
		gbrS.allowHyphens = false;

	gbrS.pspec_ = scCachedStyle::GetParaStyle().GetSpec();

	if ( lineCount == 0 ) {
		gbrS.cB.spec		= ::BRKUpdateSpec( gbrS.theSpecRec );
		gbrS.theSpecRec++;
		gbrS.theBreakColH	= ggcS.theActiveColH;
	}

		// INDENTION CONTROL
	if ( gbrS.effectiveRag & (int)eRagRight && (long)lineCount < scCachedStyle::GetParaStyle().GetLinesToIndent() )
		gbrS.brkLeftMargin = scCachedStyle::GetParaStyle().GetIndentAmount();
	else
		gbrS.brkLeftMargin = 0;

		// HANGING PUNCTUATION CONTROL
		// this computes the actual overhang
	if ( ( gbrS.effectiveRag & (int)eFlushLeft )
			&& ( gbrS.effectiveRag & (int)eHangPuncLeft )
			&& CTIsPunc( chRec->character ) )
		gbrS.brkLeftMargin -= scCachedStyle::GetParaStyle().GetLeftHangValue( chRec->character );


	gbrS.theBreakColH	= ggcS.theActiveColH;

		// compute the desired measure
	dMeasure = measure - gbrS.brkLeftMargin - gbrS.brkRightMargin;

		// compute the hyphenation zone
	if ( scCachedStyle::GetParaStyle().GetRagZone() > dMeasure )
		gbrS.hyphenationZone = dMeasure / 2;
	else
		gbrS.hyphenationZone = scCachedStyle::GetParaStyle().GetRagZone();

	return dMeasure;
}

/************************************************************************/

static TypeSpec BRKUpdateSpec( scSpecRecord *specRecEntry )
{
	TypeSpec	theSpec 	= specRecEntry->spec();
	size_t		mlvIndex;
	
	scCachedStyle::GetCachedStyle( theSpec );

		/* this is to take care of the rag setting on a line */
	if (  gbrS.cB.startCount  == (	gbrS.cB.streamCount  - 1 ) )
		 gbrS.effectiveRag	=  scCachedStyle::GetParaStyle().GetRag() ;

	if (  gbrS.cB.lineVal + 1 < MAXLEADVALS ) {
		gbrS.cB.specChanged++;
		mlvIndex = gbrS.cB.lineVal;
		
		gbrS.fMaxLineVals[mlvIndex].fSpecRec	= specRecEntry;
		gbrS.fMaxLineVals[mlvIndex].fMaxLead.Set( scCachedStyle::GetCurrentCache().GetComputedLead() );
		
		gbrS.fMaxLineVals[mlvIndex].fMaxInkExtents = scCachedStyle::GetCurrentCache().GetInkExtents();
	
		gbrS.fMaxLineVals[mlvIndex++].fOblique	= false;
		
		gbrS.cB.lineVal = mlvIndex;
		
		*( gbrS.fMaxLineVals + gbrS.cB.lineVal ) = gbrS.fZeroMaxLineVals;
		gbrS.cB.specRec = specRecEntry;
	}

	return theSpec;
}

/************************************************************************/
/* find the last non-space character on the line, given that what is passed in
 * is the last character on the line
 */

static CharRecordP BRKLastCharOnLine( CharRecordP tmpChRec )
{
	for ( ; CTIsSpace( tmpChRec->character ); tmpChRec-- )
		;
	return tmpChRec;
}

/************************************************************************/

static void BRKRepairLastSpace( CharRecordP tmpChRec,
								long		numberToNull )
{
	switch ( (tmpChRec-1)->character ) {
		case scQuadCenter:
			gbrS.effectiveRag = eRagCentered;
			tmpChRec -= 2;
			break;
		case scQuadLeft:
			gbrS.effectiveRag = eRagRight;
			tmpChRec -= 2;
			break;
		case scQuadRight:
			gbrS.effectiveRag = eRagLeft;
			tmpChRec -= 2;
			break;
		case scQuadJustify:
			gbrS.effectiveRag = eRagJustified;
			tmpChRec -= 2;
			break;
		case scHardReturn:
		case scVertTab:
			tmpChRec -= 2;
			break;
		default:
			tmpChRec--;
			break;
	}

	gbrS.totalTrailingSpace = 0;
	for ( ; numberToNull && tmpChRec->character == scWordSpace; tmpChRec--, numberToNull-- ) {
		if ( gHiliteSpaces )
			gbrS.totalTrailingSpace += tmpChRec->escapement;
	}
	
	scAssert( !numberToNull );
}

/************************************************************************/

static void BRKAdjustWordSpace( CharRecordP prevChar,
								GlyphSize	adjustment,
								long		numSpaces,
								long		endSpaces )
{
	/* when we come in here prevchar points to the first word of the next line
	 * we need to ignore it if it is a wordspace
	 */
	if (  prevChar->character  == scWordSpace )
		prevChar--;

	for ( ; endSpaces && prevChar > gbrS.gStartRec; prevChar-- ) {
		if (  prevChar->character  == scWordSpace )
			endSpaces--;
	}
	scAssert( endSpaces == 0 );
	for ( ; numSpaces && prevChar >= gbrS.gStartRec; prevChar-- ) {
		if (  prevChar->character  == scWordSpace ) {
			prevChar->escapement = adjustment;
			numSpaces--;
		}
	}
	scAssert( numSpaces == 0 );
}

/************************************************************************/

static void BRKRepairFinalSpace(  )
{
	scAssert( gbrS.cB.theChRec->character == 0 );
	BRKRepairLastSpace( gbrS.cB.theChRec, gbrS.cB.trailingSpaces );
}

/************************************************************************/

BreakStruct::BreakStruct()
{
}

/************************************************************************/

BreakStruct::~BreakStruct()
{
}

/************************************************************************/

void BreakStruct::Init()
{
	pspec_.clear();
	cB.Init();
	for ( int i = 0; i < MAXBREAKVALS; i++ )
		gbrS.candBreak[i].Init();
}

/************************************************************************/
/* free the memory associated with the breaking machine */

void BRKFreeMach(  )
{
	delete [] gbrS.breakMach;
	gbrS.breakMach = NULL;
	delete [] gbrS.fMaxLineVals;
	gbrS.fMaxLineVals = NULL;
	delete [] gbrS.candBreak;
	gbrS.candBreak = NULL;
}

/************************************************************************/
/* init the breaking machine */

void BRKInitMach(  )
{
	int i;

	gbrS.breakMach		= new BrFunc[ SIZE_OF_MACHINE ];
	gbrS.fMaxLineVals	= new scMaxLineVals[ MAXLEADVALS ];
	gbrS.candBreak		= new CandBreak[ MAXBREAKVALS ];

	for ( i = 0; i < SIZE_OF_MACHINE; i++ ) {
		switch ( i ) {
			case scTabSpace:
				gbrS.breakMach[i] = bmBRKTab;
				break;
			case scWordSpace:
				gbrS.breakMach[i] = bmBRKWordSpace;
				break;
			case scEndStream:
				gbrS.breakMach[i] = bmBRKEndStream;
				break;
			case scEnDash:
			case scEmDash:
			case scBreakingHyphen:
			case '=':
				gbrS.breakMach[i] = bmBRKHyphen;
				break;
			case scFillSpace:
				gbrS.breakMach[i] = bmBRKFillSpace;
				break;
			case scRulePH:
				gbrS.breakMach[i] = bmBRKRule;
				break;
			case scFixAbsSpace:
			case scFigureSpace:
			case scThinSpace:
			case scEnSpace:
			case scEmSpace:
				gbrS.breakMach[i] = bmBRKFixSpace;
				break;
			case scFixRelSpace:
				gbrS.breakMach[i] = bmBRKRelSpace;
				break;
			case scVertTab:
				gbrS.breakMach[i] = bmBRKVertTab;
				break;
			case scQuadCenter:
			case scQuadLeft:
			case scQuadRight:
			case scQuadJustify:
				gbrS.breakMach[i] = bmBRKQuad;
				break;
			case scHardReturn:
				gbrS.breakMach[i] = bmBRKHardReturn;
				break;
			case scField:
				gbrS.breakMach[i] = bmBRKField;
				break;
			default:
				gbrS.breakMach[i] = bmBRKChar;
				break;
		}
	}
}

/************************************************************************/

static BOOL BRKStillMoreChars( CharRecordP	chRec,
								long		count )
{
	for ( ; count--; chRec++ ) {
		if ( CTIsVisible( chRec->character ) )
			return true;
	}
	return false;
}

/************************************************************************/

BOOL BRKJustify( CharRecordP	chRec,		/* the character array */
				 long	start,		/* count into ch array to start the linebreak */
				 long	stop,		/* count into ch array of end of line */
				 MicroPoint measure )	/* measure to justify to */
{
	long		spaces,
				count;
	MicroPoint	delta;
	MicroPoint	boxWidth;
	CharRecordP holdChRec;
	BOOL		changed = false;
	
	chRec		+= start;
	holdChRec	= chRec;
	
	boxWidth = 0;
	for ( spaces = 0, count = stop - start; count; chRec++, count-- ) {
		switch ( chRec->character ) {
			case scWordSpace:	
				if ( BRKStillMoreChars( chRec, (long)count ) )
					spaces++;
				break;
			default:
				boxWidth += chRec->escapement;
				break;
		}
	}

	delta = measure - boxWidth; 
	if ( spaces ) {
		delta = scRoundMP((double)delta / spaces);
		
		for ( chRec=holdChRec, count = stop-start; count; chRec++,count-- ) {
			switch ( chRec->character ) {
				case scWordSpace:
					if ( spaces ) {
						spaces--;
						if ( !changed && chRec->escapement != delta )
							changed = true;
						chRec->escapement = (GlyphSize)delta;
					}
					break;
				default:
					break;
			}
		}
	}
	return changed;
}


/************************************************************************/

#ifdef JAPANESE //j
static void BRKCharJapanese( )
{
	MicroPoint		adjustableSpace;
	UCS2		theCharacter;
	CharBits		cb;
	BOOL			noStartline,
					noEndline;


	adjustableSpace = gbrS.desiredMeasure - gbrS.cB.curBox;
	theCharacter	= gbrS.cB.theChRec->character;

	cb = TSCharBits( scCachedStyle::GetCurrentCache().fmTheSpec, theCharacter );

	if ( cb.fTheBits.fCharClass )
		noStartline = true;
	else
		noStartline = false;

	if ( cb.fTheBits.fCharClass )
		noEndline	= true;
	else
		noEndline	= false;
				

	// ValidateBits( theCharacter, cb );

	if ( gbrS.numTargetChars > 0 ) {	/* inhibit breaks in	*/
		gbrS.numTargetChars--;				/* target sequence		*/
	}
	else {
			/* set a potential break before every character */
		if ( !( noStartline || gbrS.fNoStartline ) ) {
			BRKSetCandBreak( eCharBreak );
				
			if ( BRKExceedVals( adjustableSpace ) ) {
				BRKLineDecision( 0 );
				return BRKExitLoop( );
			}
		}
	
		if ( gbrS.firstBox )
			BRKSetFirstBox( );
	}
				
	gbrS.cB.curBox += gbrS.cB.theChRec->escapement;


	if ( noEndline || ( !scCachedStyle::GetCurrentCache().fmBreakableNumbers && cb.fTheBits.fDigit ) )
		gbrS.fNoStartline	= true;
	else
		gbrS.fNoStartline	= false;
		
	if ( cb.fTheBits.fHangable ) 
		gbrS.cB.fHangable = gbrS.cB.theChRec->escapement;
	else
		gbrS.cB.fHangable = 0;
		
	gbrS.cB.chCount++;
	gbrS.cB.streamCount++;
	gbrS.cB.theChRec++;
}
#endif JAPANESE

/************************************************************************/

CandBreak& CandBreak::operator=( const CandBreak& cb )
{
	breakCount		= cb.breakCount;
	startCount		= cb.startCount;
	streamCount 	= cb.streamCount;
	wsSpaceCount	= cb.wsSpaceCount;
	spaceCount		= cb.spaceCount;
	trailingSpaces	= cb.trailingSpaces;
	chCount 		= cb.chCount;
	fillSpCount 	= cb.fillSpCount;
	lineVal 		= cb.lineVal;
	breakVal		= cb.breakVal;
	minGlue 		= cb.minGlue;
	optGlue 		= cb.optGlue;
	maxGlue 		= cb.maxGlue;
	curBox			= cb.curBox;
	fHangable		= cb.fHangable;
	theChRec		= cb.theChRec;
	specChanged 	= cb.specChanged;
	spec			= cb.spec;
	specRec 		= cb.specRec;

	return *this;
}
