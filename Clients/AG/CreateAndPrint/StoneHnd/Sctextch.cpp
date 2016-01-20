#include "scapptex.h"
#include "scctype.h"
#include "scstcach.h"
#include "scmem.h"
#include "scparagr.h"
#include "scfileio.h"

#ifdef _RUBI_SUPPORT
	#include "scrubi.h"
#endif

//#define TextRunDebug

#if defined( TextRunDebug  ) && defined( scAssert )

	static long 	TXTCharCount( CharRecordP );
	static long 	txtCharCount;

	#define TXTValidate(_chRec,_len)	{scAssert( ((_chRec)+(_len))->character == scEndStream\
										&& _len == TXTCharCount( _chRec ) );}

#else

	#define TXTValidate(_chRec,_len)	{;}

#endif	/*TextRunDebug*/

/*======================================================================*/
#define CharacterPad		10

/*======================================================================*/
void SetKerning( UCS2			prevCh,
				 CharRecordP	prevChRec,
				 UCS2			ch,
				 CharRecordP	chRec )
{
	GlyphSize	kernVal;

	kernVal = scCachedStyle::GetCurrentCache().GetKernValue( prevCh, ch );
						  
	if ( kernVal ) {
		prevChRec->flags.SetKernBits();
		prevChRec->escapement += kernVal;
	}
}

/****************************************************************************/
/* 'repair' the text array, rehypenate, rekern and recompute widths */
static BOOL SetCharacterWidth( UCS2 ch, CharRecordP chRec )
{
	switch ( ch ) {
		default:
			if ( ch > 256 )
				chRec->escapement = scCachedStyle::GetCurrentCache().GetSetSize();
			else
				chRec->escapement = scCachedStyle::GetCurrentCache().GetEscapement( ch );
			break;

		case scFixRelSpace:
		case scFixAbsSpace:
		case scFillSpace:
			break;
		case scQuadCenter:
		case scQuadLeft:
		case scQuadRight:
		case scQuadJustify:
		case scHardReturn:
		case scTabSpace:
		case scVertTab:
				chRec->escapement = 0L;
				break;
		case scWordSpace:
			chRec->escapement = scCachedStyle::GetCurrentCache().GetOptWord();
			break;

	}
	
	return ch < 256;
}

/****************************************************************************/
static void TXT_IncRetabulate( CharRecordP	chRec,
							   scSpecRun&	specRun,
							   long 		start,
							   long 		end,
							   TypeSpec 	changedSpec )
{
	CharRecordP 	prevChRec = 0;
	CharRecordP 	stopChRec;
	UCS2			prevCh = 0;
	UCS2			ch;
	scSpecRecord*	specRec;
	long			chCount;
	BOOL			doit			= false;
	TypeSpec		rtSpec;
	int 			kerning;
		
	stopChRec	= chRec + end;
	chRec		+= start;
	
	specRec 		= specRun.ptr( ) + specRun.indexAtOffset( start );
	rtSpec			= specRec->spec();
	scCachedStyle*	cachedStyle = &scCachedStyle::GetCachedStyle( rtSpec );

	kerning = cachedStyle->GetKern();

	chCount = start; 
	for ( ; chRec <= stopChRec; prevChRec = chRec, chRec++, chCount++ ) {

		/* maintain spec */
		if ( chCount >= specRec->offset() ) {
			rtSpec = (specRec++)->spec();				/* set the retab spec */
			
			/* do we retab this spec ? */			
			doit = (changedSpec == NULL || rtSpec == changedSpec );
			
			cachedStyle = &scCachedStyle::GetCachedStyle( rtSpec ); 		/* set the metrics */
			kerning = cachedStyle->GetKern();

			prevChRec	= NULL;
		}

		ch = chRec->character;
		if ( prevChRec )
			prevCh = prevChRec->character;

		if (doit) {
			chRec->flags.ClrAutoBits();

			if ( ::SetCharacterWidth( ch, chRec ) && kerning && prevChRec )
				::SetKerning( prevCh, prevChRec, ch, chRec );
		}
		if ( ch == 0 )
			break;
	}
}

/************************************************************************/
/* locate the beginning of a word */
static long TXT_StartSpace( CharRecordP chRec,
							long		start )
{
	for ( ; start && !CTIsSpace(chRec[ start - 1 ].character); start-- )
		;
	return start;
}

/************************************************************************/
/* locate the end of a word */
static long TXT_EndSpace( CharRecordP	chRec,
						 long			end )
{
	if	(end >= 0L) {
		chRec += end;

		for ( ; chRec->character != scEndStream && !CTIsSpace(	chRec->character  );
					chRec++, end++ )
			;
	}
	return end;
}

/************************************************************************/
static void TXT_RepairDamage( CharRecordP	startChRec,
							 scSpecRun& specRun,
							 long			offset,
							 long			charSize )
{
	long	startOffset,
			endOffset;

	startOffset = offset - 2;
	startOffset = MAX( startOffset, 0 );

	startOffset = TXT_StartSpace( startChRec, startOffset );

	offset		= MIN( offset + 1, charSize - 1 );
	endOffset	= TXT_EndSpace( startChRec, offset );

	TypeSpec nullSpec;

	if ( endOffset > 0 && startOffset != endOffset )
		TXT_IncRetabulate( startChRec, specRun, startOffset, endOffset, nullSpec );
}


/* ==================================================================== */

MicroPoint	UnivStringWidth( USTR&	ustr, 
							 MicroPoint 	escapements[], 
							 TypeSpec&		ts )
{
	scSpecRun	specrun( ts );
	CharRecordP run = new CharRecord[ ustr.len + 1 ];

	for ( UINT i = 0; i < ustr.len; i++ )
		run[i].character = ustr.ptr[i];
	run[ustr.len].character = 0;

	TypeSpec nullSpec;
	TXT_IncRetabulate( run, specrun, 0, ustr.len, nullSpec );

	MicroPoint width = 0;
	for ( UINT i = 0; i < ustr.len; i++ ) {
		width += run[i].escapement;
		if ( escapements )
			escapements[i] = run[i].escapement;
	}

	delete run;
	run = NULL;
	return width;
}

/* ==================================================================== */

/* locate the beginning of a word for selection */

long TXTStartSelectableWord( CharRecordP	chRec,
							 long			start )
{
	UCS2 ch;
	
	for ( ; start; start-- ) {
		ch = chRec[ start - 1 ].character;
		if ( !CTIsSelectable( ch ) )
			break;
	}
		
	return start;
}

/************************************************************************/
/* locate the end of a word for selection */
long TXTEndSelectableWord( CharRecordP	chRec,
						   long 		end )
{
	UCS2 ch;
	
	if	(end >= 0L) {
		chRec +=  end;

		ch =  chRec->character ;
		if ( ch != scEndStream && (CTIsSelectable( ch ) || CTIsPunc(ch))) {
			for ( ; ch != scEndStream; chRec++, end++ ) {
				ch =  chRec->character ;
				if ( !CTIsSelectable( ch ) && !CTIsPunc(ch))
					break;
			}
		}
	}
	return end;
}

/************************************************************************/
long TXTStartWord( CharRecordP	chRec,
				   long 		start,
				   int			eleminateLeadingSpaces )
{
	if ( eleminateLeadingSpaces ) {
		for ( ; CTIsSpace( chRec[ start ].character ); start++ )
			;
	}

	for ( ; start; start-- ) {
		UCS2 ch = chRec[ start - 1 ].character;
		if ( CTIsSpace( ch ) )
			break;
	}
		
	return start;
}

/************************************************************************/
/* locate the end of a word for selection for spell checking */
long TXTEndWord( CharRecordP	chRec,
				 long			end )
{
	UCS2 ch;
	
	if	(end >= 0L) {
		chRec +=  end;

		ch =  chRec->character ;
		if ( ch != scEndStream && !CTIsSpace( ch ) ) {
			for ( ; true; chRec++, end++ ) {
				ch =  chRec->character ;
				if ( CTIsSpace( ch ) || ch == scEndStream )
					break;
			}
		}
	}
	return end;
}

/************************************************************************/
/* after insertion of character(s) we must fix the damage, caused by insertion,
 * this entails rehyphenating, rekerning, etc. Fixes that rebreaking will
 * not accomplish.
 */
void scCharArray::RepairText( scSpecRun&	specRun,
							  long			offset1,
							  long			offset2 )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;
	
	if ( offset1 != offset2 )
		TXT_RepairDamage( chRec, specRun, offset2, GetContentSize() );
		
	TXT_RepairDamage( chRec, specRun, offset1, GetContentSize() );
}

/* ==================================================================== */

static BOOL Capitalize( UCS2 lastCh )
{
	if ( lastCh >= 256 )
		return true;
	
	if ( lastCh == 0x27 )
		return false;
	
	return !CTIsAlpha( lastCh );
}

/* ==================================================================== */
// count the number of characters in the text array

#ifdef TextRunDebug
static long TXTCharCount( CharRecordP chRec )
{
	long i;

	for ( i = 0; chRec->character; chRec++, i++ )
		;
	txtCharCount = i;
	return i;
}
#endif

/* ==================================================================== */

void scCharArray::Retabulate( scSpecRun&	specRun,
							  long			start,
							  long			end,
							  TypeSpec		changedSpec,
							  long			charSize )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;

	TXT_IncRetabulate( chRec, specRun, start, end, changedSpec );

	/* fix edges */
	if ( start > 0 )
		TXT_RepairDamage( chRec, specRun, start, charSize );
	if ( (chRec+end)->character != scEndStream )
		TXT_RepairDamage( chRec, specRun, end, charSize );
}

/* ==================================================================== */


#include "scdbcsdt.h"

static BOOL scAPPReadCharacter( APPCtxPtr				ctxPtr,
								IOFuncPtr				readFunc,
								UCS2&					character,
								const scDBCSDetector&	detector )
{
	long	size;
	uchar	buf[8];
	
	size = (*readFunc)( ctxPtr, buf, 1 );

	if ( size && detector.ByteType( buf[0] ) == scDBCSDetector::eFirstByte ) {
		size += (*readFunc)( ctxPtr, buf + 1, 1 );
	}	
	
	if ( size == 1 )	
		character = buf[0];
	else if ( size == 2 )
		character = (UCS2)((unsigned int)buf[1] | (unsigned int)buf[0]<<8);
		
	return size > 0;
}

/*======================================================================*/
BOOL scAPPWriteCharacter( APPCtxPtr ctxPtr,
						  IOFuncPtr writeFunc,
						  UCS2* 	character,
						  TypeSpec )
{
	uchar	*ptr		= (uchar *)character;
	long	size	= 0;
	
	if ( *character > 255 )
		size = (*writeFunc)( ctxPtr, ptr + 1, 1 );
	size += (*writeFunc)( ctxPtr, ptr, 1);
	return size > 0;
}

/* ==================================================================== */

#ifdef jis4051
// find the start of the ren moji character

static CharRecordP StartRenMoji( CharRecordP start, CharRecordP ch )
{
	unsigned	count	= ch->flags.GetRenMoji();
	CharRecordP ptr 	= ch;
	
	while ( ptr->character && ptr->flags.GetRenMoji() == count )
		ptr++;
	
		// i should now be past all renmoji of the count i am testing
	while ( ( ptr - count ) > ch )
		ptr -= count;
	
	return MAX( start, ptr );
}

/* ==================================================================== */

BOOL TXTSameRenMoji( CharRecordP start, CharRecordP ch1, CharRecordP ch2 )
{
	unsigned rm1 = ch1->flags.GetRenMoji();
	unsigned rm2 = ch2->flags.GetRenMoji();
	if ( rm1 && rm2 && rm1 == rm2 )
		return StartRenMoji( start, ch1 ) == StartRenMoji( start, ch2 );
	return false;
}

/* ==================================================================== */

#endif

int scCharArray::IsEqual( const scObject& obj ) const
{
	scCharArray& arr2 = (scCharArray&)obj;

	if ( fNumItems != arr2.GetNumItems() )
		return 0;

	scHandleArrayLock	h1( (scHandleArray*)this );
	scHandleArrayLock	h2( (scHandleArray*)&arr2 );	

	CharRecordP ch1 = (CharRecordP)*h1;
	CharRecordP ch2 = (CharRecordP)*h2;

	int i;

	for ( i = 0; i < fNumItems; i++, ch1++, ch2++ ) {
		if ( ch1->character != ch2->character )
			return 0;
	}
	return ch1->character == ch2->character;
}

/* ==================================================================== */

void scCharArray::RemoveBetweenOffsets( long	startOffset,
										long	endOffset )
{
	{
		scHandleArrayLock	h( this );
		CharRecordP chRec = (CharRecordP)*h;

		memmove( chRec + startOffset,
				 chRec + endOffset,
				 ( fNumItems - endOffset ) * sizeof( CharRecord ) );

		fNumItems -= ( endOffset - startOffset );
	}
	SetContentSize( fNumItems );
}

/* ==================================================================== */

void scCharArray::Copy( scCharArray&	charArray,
						long			startOffset,
						long			endOffset ) const
{
	charArray.SetContentSize( endOffset - startOffset );
	
	scHandleArrayLock	h( (scHandleArray*)this );
	const CharRecordP chRec = (const CharRecordP)*h;
	
	charArray.Insert( chRec + startOffset, 0, endOffset - startOffset );
}

/* ==================================================================== */

void scCharArray::Paste( scCharArray&	charArray,
						 long			startOffset )
{
	scHandleArrayLock	h( &charArray );
	CharRecordP chRec = (CharRecordP)*h;

	Insert( chRec, startOffset, charArray.GetContentSize() );
}

/* ==================================================================== */

void scCharArray::Cut( scCharArray& charArray,
					   long 		startOffset,
					   long 		endOffset )
{
	Copy( charArray, startOffset, endOffset );
	RemoveBetweenOffsets( startOffset, endOffset );
}

/* ==================================================================== */

void scCharArray::SetContentSize( long size )
{
	SetNumSlots( ( ( (size+1) / fBlockSize ) + 1 ) * fBlockSize );	
}

/* ==================================================================== */

long scCharArray::ExternalSize( void ) const
{
	return 0;
}

/* ==================================================================== */

void scCharArray::Read( APPCtxPtr	ctxPtr,
						IOFuncPtr	readFunc )
{
	long			numItems,
					leftToRead;
	uchar			buf[256];
	const uchar*	pbuf = buf;

	ReadLong( numItems, ctxPtr, readFunc, kIntelOrder );
	SetNumSlots( numItems );

	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;

	leftToRead = numItems;
		
	while ( leftToRead > 0 ) {
		ulong readin;

		if ( leftToRead > ( sizeof( buf ) / sizeof( long ) ) ) {
			ReadBytes( buf, ctxPtr, readFunc, sizeof( buf ) );
			readin = sizeof( buf );
		}
		else {
			ReadBytes( buf, ctxPtr, readFunc, leftToRead * sizeof ( long ) );
			readin = leftToRead * sizeof ( long );
		}
							 
		leftToRead -= ( readin / sizeof( long ) );

		pbuf = buf;
			
		while ( readin > 0 ) {
			pbuf = BufGet_long( pbuf, chRec->charflags, kIntelOrder );
			chRec++, readin -= sizeof( long );
		}
	}
	fNumItems = numItems;
	Validate();
}

/* ==================================================================== */

void scCharArray::Write( APPCtxPtr	ctxPtr,
						 IOFuncPtr	writeFunc )
{
	uchar	buf[256];
	uchar*	pbuf = buf;
	
	Validate();

	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;

	int i,
		bufIndex;
		
	WriteLong( GetNumItems(), ctxPtr, writeFunc, kIntelOrder );
		
	for ( i = bufIndex = 0; i < GetNumItems(); i++, chRec++ ) {
		if ( bufIndex == 0 )
			pbuf = buf;
			
		pbuf = BufSet_long( pbuf, chRec->charflags, kIntelOrder );
		bufIndex += sizeof( long );
			
		if ( bufIndex == 256 ) {
			WriteBytes( buf, ctxPtr, writeFunc, bufIndex );
			bufIndex = 0;
		}
	}
	if ( bufIndex )
		WriteBytes( buf, ctxPtr, writeFunc, bufIndex ); 	
}

/* ==================================================================== */
// insert the "len" characters in the srcCh (source characters)
// into the character array at "offset"

void scCharArray::Insert( const CharRecordP srcCh,
						  long				offset,
						  long				len )
{
	long	oldNumItems = fNumItems;
	
	SetNumSlots( GetNumItems() + len );
	
	scHandleArrayLock	h( this );
	CharRecordP 		dstCh = (CharRecordP)*h;

	memmove( dstCh + offset + len,
			 dstCh + offset,
			 ( oldNumItems - offset ) * fElemSize );

	memcpy( dstCh + offset, srcCh, len * fElemSize );

	fNumItems += len;
}

/* ==================================================================== */
// insert the univstring between start and end 

int scCharArray::Insert( const USTR& str,
						  int32 			  start,
						  int32 			  end )
{
	long	oldNumItems = fNumItems;
	scAssert( end < fNumItems );

	int32	diff = ( str.len - ( end - start ) );
	
	if ( diff > 0 )
		SetNumSlots( GetNumItems() + diff );

	{
		scHandleArrayLock	h( this );
		CharRecordP 		dstCh = (CharRecordP)*h;

		memmove( dstCh + end + diff,
				 dstCh + end,
				 ( oldNumItems - end ) * fElemSize );


	
		dstCh += start;

		for ( UINT i = 0; i < str.len; i++ ) {
			dstCh[i].ClearFlags();
			dstCh[i].character = str.ptr[i];
		}
	}

	if ( diff < 0 )
		SetNumSlots( GetNumItems() + diff );

	fNumItems += diff;

	return diff;
}

/* ==================================================================== */

void scCharArray::CopyChars( CharRecordP dstCh, long offset, long len )
{
	scHandleArrayLock	h( this );
	CharRecordP 		srcCh = (CharRecordP)*h;

	memcpy( dstCh, srcCh + offset, len * sizeof( CharRecord ) );
}

/* ==================================================================== */

void scCharArray::Validate( void ) const
{
	CharRecordP chRec = (CharRecordP)GetMem();
	scAssert( chRec[ fNumItems - 1 ].character == 0 );
}

/* ==================================================================== */

ElementPtr scCharArray::Lock( void )
{
	Validate();
	return scHandleArray::Lock();
}

/* ==================================================================== */

void scCharArray::Unlock( void )
{
	Validate();
	scHandleArray::Unlock();
}

/* ==================================================================== */

void scCharArray::Transform( long			startOffset,
							 long			endOffset,
							 eChTranType	trans,
							 int			numChars )
{
	CharRecordP chRec;
	CharRecordP chStop;
	UCS2	lastChar,
				thisChar;

	scHandleArrayLock	h( this );
	CharRecordP 		startChRec = (CharRecordP)*h;

	chStop		= startChRec + endOffset;
	chRec		= startChRec + startOffset;

	if ( startOffset == 0 )
		lastChar = scWordSpace;
	else
		lastChar = (chRec-1)->character;



#ifdef jis4051
		// remove the special japanese stuff beyond the indicated boundary,
		// because the boundaries maynot be accurate
	if ( trans == eRemoveJapTran ) {
		CharRecordP chRec2;
		
		for ( chRec2 = chRec; --chRec2 >= startChRec; ) {
			if ( chRec2->flags.IsSpecialNihon() ) {
				chRec2->flags.ClrSpecialNihon();
				chRec2->flags.ClrNoBreak();
			}
			else 
				break;
		}
		
		for ( chRec2 = chStop; chRec2->character && chRec2->flags.IsSpecialNihon(); chRec2++ ) {
			chRec2->flags.ClrSpecialNihon();
			chRec2->flags.ClrNoBreak(); 	
		}
	}
#endif

	for ( ; chRec < chStop; chRec++ ) {
		thisChar = chRec->character;
		if ( thisChar >= ' ' ) {
			switch ( trans ) {
				default:
				case eNormalTran:
					break;
				case eLowerTran:
					chRec->character = CTToLower( thisChar );
					break;
				case eUpperTran:
					chRec->character = CTToUpper( thisChar );
					break;
					
				case eCapitalizeTran:
					if ( Capitalize( lastChar ) )
						chRec->character = CTToUpper( thisChar );
					else						
						chRec->character = CTToLower( thisChar );
					lastChar = thisChar;
					break;
					
				case eChangeCaseTran:
					chRec->character = CTToggleCase( thisChar );
					break;
					
				case eNoBreakTran:
					chRec->flags.SetNoBreak();
					break;
					
				case eAllowBreakTran:
					chRec->flags.ClrNoBreak();
					break;

#ifdef jis4051
				case eWarichuTran:
					chRec->flags.ClrSpecialNihon(); 
					scAssert( numChars < 4 );
					chRec->flags.SetWarichu( numChars );
					break;		
								
				case eRenMojiTran:
					chRec->flags.ClrSpecialNihon(); 
					scAssert( numChars < 8 );
					chRec->flags.SetRenMoji( numChars );
					break;
					
				case eRubiTran:
					chRec->flags.ClrSpecialNihon(); 
					chRec->flags.SetRubi( );
					chRec->flags.SetNoBreak();
					break;
					
				case eRemoveJapTran:		// remove the japanese transformations
					chRec->flags.ClrSpecialNihon(); 
					chRec->flags.ClrNoBreak();
					break;
#endif
			}
		}
	}
}

/* ==================================================================== */

void scCharArray::SelectWord( long	offset, 
							  long& startWord,
							  long& endWord )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;
	
	startWord	= TXTStartSelectableWord( chRec, offset );
	endWord 	= TXTEndSelectableWord( chRec, offset );
}

/* ==================================================================== */

#ifdef jis4051 

static void PatchRenMoji( CharRecordP	startChRec,
						  CharRecordP	delChRec )
{
	CharRecordP chRec = delChRec;
	unsigned	i;
	unsigned	numChars;
	unsigned	rmset;
	
	numChars = delChRec->flags.GetRenMoji();

	while ( chRec > startChRec && (chRec-1)->flags.GetRenMoji() )
		chRec--;
	
	rmset = 0;
	do {
		chRec += rmset;
		rmset = chRec->flags.GetRenMoji(); 
	} while ( chRec + rmset < delChRec ); 
	
	for ( i = 0; i < numChars; i++, chRec++ )
		chRec->flags.ClrRenMoji();
}

#endif

/* ==================================================================== */

void scCharArray::DoBackSpace( long&		streamSize,
							   long&		cursorSize,
							   scSpecRun&	specRun,
#ifdef _RUBI_SUPPORT
							   scRubiArray* rubiArray,
#endif							   
							   long 		offset,
							   scKeyRecord& keyRec,
							   BOOL 		textCleared )
{
	scHandleArrayLock	h( this );
	CharRecordP 		startChRec	= (CharRecordP)*h;
	CharRecordP 		chRec		= startChRec;
	
	if ( offset > 0 ) {
		chRec +=  offset - 1;
		if ( chRec->flags.IsDiscHyphen() && chRec->flags.IsLineBreak() ) {
				keyRec.replacedchar() = scDiscHyphen;
				chRec->flags.ClrDiscHyphen();
		}
		else if ( !textCleared ) {
			if ( chRec->character == scFixRelSpace || chRec->character == scFixAbsSpace )
				keyRec.escapement() = chRec->escapement;

#ifdef jis4051
			if ( chRec->flags.GetRenMoji() > 1 )
				PatchRenMoji( startChRec, chRec );
#endif

			keyRec.replacedchar()	= chRec->character;
			keyRec.replacedfield()	= chRec->flags.GetField();
			keyRec.spec()			= specRun.SpecAtOffset( offset -1 );

			memmove( chRec, chRec + 1, (GetNumItems() - offset) * sizeof(CharRecord) );
			cursorSize = streamSize = -1;
		}
	}
}


/* ==================================================================== */

void scCharArray::DoForwardDelete( long&		streamSize,
								   long&		cursorSize,
								   scSpecRun&	specRun,
#ifdef _RUBI_SUPPORT
								   scRubiArray* rubiArray,
#endif								   
								   long 		offset,
								   scKeyRecord& keyRec,
								   BOOL 		textCleared )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;
	
	if ( offset < GetContentSize() ) {
		chRec +=  offset;
		if ( !textCleared ) {
			if ( chRec->character == scFixRelSpace || chRec->character == scFixAbsSpace )
				keyRec.escapement() = chRec->escapement;
			keyRec.replacedchar() = chRec->character;
			keyRec.replacedfield() = chRec->flags.GetField();
			keyRec.spec() = specRun.SpecAtOffset( offset );
			memmove( chRec, chRec + 1, (GetContentSize() - offset) * sizeof(CharRecord) );
			streamSize	= -1;
			cursorSize	= 0; /* don't reposition this backwards */
			keyRec.restoreselect() = true; /* used as forward delete flag */

		}
	}
}

/* ==================================================================== */

void scCharArray::DoDiscHyphen( long&			streamSize,
								long&			cursorSize,
								scSpecRun&		specRun,
#ifdef _RUBI_SUPPORT
								scRubiArray*	rubiArray,
#endif								
								long			offset,
								scKeyRecord&		keyRec,
								BOOL			textCleared )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;
	
	if ( offset ) {
		chRec += offset - 1;
		chRec->flags.SetDiscHyphen();
	}
	streamSize = cursorSize = 0;
	if ( keyRec.restoreselect() )	/* used as forward delete flag */
		keyRec.replacedchar() = scForwardDelete;
	else
		keyRec.replacedchar() = scBackSpace;
}

/* ==================================================================== */

void scCharArray::DoFixSpace( long& 		streamSize,
							  long& 		cursorSize,
							  scSpecRun&	specRun,
#ifdef _RUBI_SUPPORT
							  scRubiArray*	rubiArray,
#endif							  
							  long			offset,
							  scKeyRecord&	keyRec,
							  BOOL			textCleared )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;
	
	chRec +=  offset;
	if ( chRec->character == keyRec.keycode() ) {
		if ( (chRec->escapement + keyRec.escapement()) == 0 ) {
				// if the resulting value is to be zero (ie no kern) remove the kern character
			memmove( chRec, chRec + 1, (GetNumItems() - offset) * sizeof(CharRecord) );
			cursorSize = -1;
			streamSize = -1;
		}
		else
			chRec->escapement += keyRec.escapement();

	}
	else if ( offset && (chRec-1)->character == keyRec.keycode() ) {
		if ( ((chRec-1)->escapement + keyRec.escapement()) == 0 ) {
				// if the resulting value is to be zero (ie no kern) remove the kern character */
			memmove( chRec - 1, chRec, (GetNumItems() - offset + 1) * sizeof(CharRecord) );
			cursorSize = -1;
			streamSize = -1;
		}
		else
			(chRec-1)->escapement += keyRec.escapement();
	}
	else {
		memmove( chRec + 1, chRec, (GetNumItems() - offset + 1) * sizeof(CharRecord) );
		chRec->character = keyRec.keycode();
		chRec->ClearFlags();
		chRec->escapement = keyRec.escapement();
		streamSize = 1;
		if ( ! keyRec.restoreselect() )
			cursorSize = 1;
	}
	if ( keyRec.keycode() == scFixRelSpace ) {
		keyRec.replacedchar()	= scFixRelSpace;
		keyRec.escapement() = -keyRec.escapement();
	}
	else if ( keyRec.restoreselect() )
		keyRec.replacedchar() = scForwardDelete;
	else
		keyRec.replacedchar() = scBackSpace;
}


/* ==================================================================== */
	
void scCharArray::DoCharacter( long&		streamSize,
							   long&		cursorSize,
							   scSpecRun&	specRun,
#ifdef _RUBI_SUPPORT
							   scRubiArray* rubiArray,
#endif							   
							   long 		offset,
							   scKeyRecord& keyRec,
							   BOOL 		textCleared )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;
	
	chRec +=  offset;

	if ( keyRec.type() == scKeyRecord::overstrike && chRec->character ) {
		keyRec.replacedchar() = chRec->character;
		keyRec.replacedfield() = chRec->flags.GetField();
	}
	else {
		memmove( chRec + 1, chRec, (GetNumItems() - offset) * sizeof(CharRecord) );
		streamSize	= 1;
		keyRec.replacedchar() = 0;
	}

	chRec->character =	keyRec.keycode();
	chRec->ClearFlags();
	chRec->flags.SetField( keyRec.field() );

	if ( offset == 0 ) {
			// why is this here
		switch ( chRec->character ) {
			case scNoBreakSpace:
			case scFigureSpace:
			case scThinSpace:
			case scEnSpace:
			case scEmSpace:
				{ 
					TypeSpec ts = specRun.SpecAtOffset( 0 );
					scCachedStyle::GetCachedStyle( ts );
					chRec->escapement = scCachedStyle::GetCurrentCache().GetEscapement( chRec->character );
				}
				break;
			default:
				break;
		}
	}

	if ( !keyRec.restoreselect() ) { /* if not replacing forward deletion */
		cursorSize = 1;
		if ( !keyRec.replacedchar() )
			keyRec.replacedchar() = scBackSpace;
	}
	else
		keyRec.replacedchar() = scForwardDelete;
}


/* ==================================================================== */
	
void scCharArray::CharInsert( long& 		cursorSize,
							  scSpecRun&	specRun,
#ifdef _RUBI_SUPPORT
							  scRubiArray*	rubiArray,
#endif							  
							  long			offset,
							  scKeyRecord&	keyRec,
							  BOOL			textCleared,
							  TypeSpec		clearedSpec )
{
		/* can we accept a spec with this character */
	BOOL		specInsert	= false;
	long		streamSize	= 0;


#if SCDEBUG > 1
	static int doit;
	if ( doit )
		specRun.PrintRun( "scCharArray::CharInsert" );
#endif

	cursorSize	= 0;

#ifdef _RUBI_SUPPORT
		// handle character insertion
	switch ( keyRec.keycode() ) {

		case scBackSpace:
			DoBackSpace( streamSize, cursorSize, specRun, rubiArray, offset, keyRec, textCleared );
			break;

		case scForwardDelete:
			DoForwardDelete( streamSize, cursorSize, specRun, rubiArray, offset, keyRec, textCleared );
			break;
			
		case scDiscHyphen:
			DoDiscHyphen( streamSize, cursorSize, specRun, rubiArray, offset, keyRec, textCleared );
			break;
			
		case scFixAbsSpace:
		case scFixRelSpace:
			DoFixSpace( streamSize, cursorSize, specRun, rubiArray, offset, keyRec, textCleared );
			break;
			
		case scFillSpace:
		default:
			DoCharacter( streamSize, cursorSize, specRun, rubiArray, offset, keyRec, textCleared );
			specInsert = true;
			break;
	}
#else
		// handle character insertion
	switch ( keyRec.keycode() ) {

		case scBackSpace:
			DoBackSpace( streamSize, cursorSize, specRun, offset, keyRec, textCleared );
			specRun.BumpOffset( MAX( offset-1, 0 ), streamSize );
			break;

		case scForwardDelete:
			DoForwardDelete( streamSize, cursorSize, specRun, offset, keyRec, textCleared );
			specRun.BumpOffset( MAX( offset, 0 ), streamSize );
			break;
			
		case scDiscHyphen:
			DoDiscHyphen( streamSize, cursorSize, specRun, offset, keyRec, textCleared );
			specRun.BumpOffset( MAX( offset-1, 0 ), streamSize );
			break;
			
		case scFixAbsSpace:
		case scFixRelSpace:
			DoFixSpace( streamSize, cursorSize, specRun, offset, keyRec, textCleared );
			specRun.BumpOffset( MAX( offset-1, 0 ), streamSize );
			break;
			
		case scFillSpace:
		default:
			DoCharacter( streamSize, cursorSize, specRun, offset, keyRec, textCleared );
			specRun.BumpOffset( MAX( offset-1, 0 ), streamSize );
			specInsert = true;
			break;
	}
	
#endif
	
	if ( streamSize ) {
		
			// we have changed the size of the stream and now
			// we must move the specRec stuff around
		fNumItems += streamSize;

#ifdef _RUBI_SUPPORT
		if ( rubiArray )
			rubiArray->BumpRubiData( MAX( offset, 0 ), streamSize );
#endif

		/* if we inserted a spec split the specrun and insert the new one */
		if ( specInsert && keyRec.spec().ptr() ) {
			specRun.ApplySpec( keyRec.spec(), offset, offset + 1 ); 	
			specRun.SetContentSize( GetContentSize() );
		}
		else if ( textCleared && clearedSpec.ptr() ) {
			specRun.ApplySpec( clearedSpec, offset, offset + 1 );		
			specRun.SetContentSize( GetContentSize() );
		}

	}
	
	RepairText( specRun, offset, offset );
	
	Validate( );
}


/* ==================================================================== */
// determine wordspace value closest to offset, in some cases this will be 
// bogus when there are lots of spec changes on a line -- but then what 
// the hell does that mean anyway

void scCharArray::WordSpaceInfo( long			offset,
								 MicroPoint&	escapement )

{
	scHandleArrayLock	h( this );
	CharRecordP 		theChRec	= (CharRecordP)*h;
	CharRecordP 		chRec		= theChRec + offset;
	long				count;

	escapement	= LONG_MIN;

		// look forward first
	for ( ; chRec->character; chRec++ ) {
		if ( chRec->character == scWordSpace && !chRec->flags.IsLineBreak() ) {
			escapement = chRec->escapement;
			break;
		}
		else if ( chRec->character == scWordSpace && chRec->flags.IsLineBreak() )
			break;	/* we've gone to far */
	}

	if ( escapement == LONG_MIN ) {
			// look backward
		for ( chRec = theChRec +  offset, count = offset; count-- > 0; chRec-- ) {
			if ( chRec->character == scWordSpace && !chRec->flags.IsLineBreak() ) {
				escapement = chRec->escapement;
				break;
			}
			else if ( chRec->character == scWordSpace && chRec->flags.IsLineBreak() )
				break;	/* we've gone to far */
		}
	}

	if ( escapement == LONG_MIN )
		escapement = 0;
}

/* ==================================================================== */

void scCharArray::CharInfo( scSpecRun&	specRun, 
							long		offset, 
							UCS2&	ch, 
							ulong&		flags, 
							MicroPoint& escapement, 
							TypeSpec&	ts, 
							eUnitType&	unitType )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;

	chRec	+= offset - 1;
	ch		= chRec->character;
	flags	= chRec->charflags & 0x0000ffff;
	ts		= specRun.SpecAtOffset( offset - 1);

	if ( !chRec->flags.IsKernPresent() )
		escapement = chRec->escapement;
	else {
		scCachedStyle::GetCachedStyle( ts );
		escapement = scCachedStyle::GetCurrentCache().GetEscapement( ch );
	}

	unitType = ( (ch==scFixRelSpace) ? eRluUnit : eAbsUnit );
}

/* ==================================================================== */

#define kMaxCharBuf 64

long scCharArray::ReadText( scSpecRun&	specRun,
						   APPCtxPtr	ctxPtr,
						   IOFuncPtr	readFunc,
						   int			charset )
{
	UCS2	ch,
				lastCh;
	long		ret;
	long		i;
	CharRecord	chRecBuf[kMaxCharBuf];
	CharRecord	chRec;
	TypeSpec	ts		= specRun.SpecAtOffset( 0 );
	scDBCSDetector detector( ts );

	i = 0;
	
	for ( lastCh = '\0'; (ret = scAPPReadCharacter( ctxPtr, readFunc, ch, detector )) == 1L; ) {
		if ( ch < 256 && !sc_CharType[(ch)+1] )
			continue;
		
		if ( ch == scHardReturn )
			break;
		
#ifdef JAPANESE //j
	if ( IntlScript() != smJapanese )
	{ // this does not work well with japanese fonts
		if ( ch == '"' )
		{
			if ( lastCh == ' ' || lastCh == '\0' )
				ch = 0xD2;
			else
				ch = 0xD3;
		}
		else if ( ch == '-' ) {
			if ( lastCh == '-' ) {
				if ( (chRec-1)->character == '-' ) {
					(chRec-1)->character = emDash;
					continue;
				}
			}
		}
	}
#endif JAPANESE

		chRecBuf[i].ClearFlags();
		chRecBuf[i].character		= ch;
		chRecBuf[i++].escapement	= 0;

		if ( i == kMaxCharBuf ) {
			InsertAt( GetNumItems() - 1, (ElementPtr)chRecBuf, kMaxCharBuf );
			i = 0;
		}
		lastCh				= ch;
	}

	if ( i )
		InsertAt( GetNumItems() - 1, (ElementPtr)chRecBuf, i );
	
	Validate();
	
	return ret;
}


/* ==================================================================== */

void scCharArray::WriteText( scSpecRun& 	specRun,
							 BOOL			addDcr,
							 APPCtxPtr		ctxPtr,
							 IOFuncPtr		writeFunc,
							 int			charset )
{
	TypeSpec nullSpec;
	CharRecordP chRec = (CharRecordP)Lock();

	try {

		UCS2 ch = 0;
		for ( ; chRec->character; chRec++ )
		{
			ch = chRec->character;
			if (ch >= ' ' || ch == scHardReturn || ch == scTabSpace)
				scAPPWriteCharacter(ctxPtr, writeFunc, &ch, nullSpec);
		}

		if (addDcr)
		{
			UCS2 chExtra = '\r';
			scAPPWriteCharacter( ctxPtr, writeFunc, &chExtra, nullSpec);
		}
	}

	catch( ... )
	{
		Unlock();
		throw;
	} 

	Unlock();
}

/* ==================================================================== */
/* read text in from application array */
/* this assumes reading from a buffer and receiving ASCII data,
 * no translation is occuring
 */

long scCharArray::ReadAPPText( scSpecRun&			specRun,
							   stTextImportExport&	appText )

{
	TypeSpec	lastTypespec,
				typespec;
	UCS2	ch;
	UCS2	lastCh;
	CharRecord	chRec;
	long		chCount;
	int 		ret = 0;
	int 		i	= 0;
	CharRecord	chRecBuf[kMaxCharBuf];
	
	for ( lastCh = '\0', chCount = 0; ( ret = appText.GetChar( ch, typespec ) ) > 0; chCount++ ){

		if ( lastTypespec != typespec ) {
			specRun.ApplySpec( typespec, chCount, LONG_MAX ); /* set style to end of run */
			lastTypespec = typespec;
		}

			// PARAGRAPH BREAK
//		if ( ( lastCh == scHardReturn && ch == scHardReturn ) || ch == scParaEnd )
		if ( ch == scParaEnd )
			break;
			
		lastCh = ch;

		ch = CMinputMap( ch );

		chRecBuf[i].character		= ch;
		chRecBuf[i].ClearFlags();
		chRecBuf[i++].escapement	= 0;

		if ( i == kMaxCharBuf ) {
			InsertAt( GetNumItems() - 1, (ElementPtr)chRecBuf, kMaxCharBuf );
			i = 0;
		}
	}
	
	if ( i )
		InsertAt( GetNumItems() - 1, (ElementPtr)chRecBuf, i );

	Validate();
	return ret;
}

/* ==================================================================== */

void scCharArray::WriteAPPText( scSpecRun&			run,
								stTextImportExport& appText )
{
	CharRecordP 	chRec;
	int 			chCount;

	chRec	= (CharRecordP)Lock();

	try {
		for ( chCount = 0; chRec->character; chRec++, chCount++ ) {
			scSpecRecord	specRec;
			specRec = run.SpecRecAtOffset( chCount );
			switch ( chRec->character ) {
				case scEndStream:
					break;
				default:
					{
						TypeSpec ts = specRec.spec();
						appText.PutChar( chRec->character, ts );
					}
					break;
			}
		}
	}

	catch( ... ) {
		Unlock();
		throw;
	} 

	Unlock();
}


/* ==================================================================== */

void scCharArray::Insert( const UCS2* ch, long offset, long len )
{
	long	oldNumItems = fNumItems;
	
	SetNumSlots( GetNumItems() + len );
	
	scHandleArrayLock	h( this );
	CharRecordP 		dstCh = (CharRecordP)*h;

	memmove( dstCh + offset + len,
			 dstCh + offset,
			 ( oldNumItems - offset ) * fElemSize );

	for ( dstCh += offset; len--; dstCh++, ch++ )	{
		dstCh->charflags	= 0;
		dstCh->character	= *ch;
		dstCh->escapement	= 0;
	}

	fNumItems += len;
}


/* ==================================================================== */

void scCharArray::CopyChars( UCS2*	ch,
							 long		start,
							 long		end )
{
	CharRecordP chRec = (CharRecordP)Lock();
	
	for ( ; start < end; start++ )
		*ch++ = chRec[start].character;

	Unlock();
}

/* ==================================================================== */
// this is a brute force method - the boyer-moore algorithm would be much
// better - but it comsumes immense amount of space for 16 bit characters,
// if we were to treat the string a multi-byte sequence we could conceivealby
// implement the BM algorithm

inline int CharRecCmp( CharRecordP			ch,
					   const USTR&	ustr,
					   int					caseSensitive )
{
	int diff; 
	
	if ( caseSensitive ) {
		for ( UINT index = 0; index < ustr.len; index++, ch++ ) {
			diff = ustr.ptr[index] - ch->character;
			if ( diff )
				return diff;
		}
	}
	else {
		for ( UINT index = 0; index < ustr.len; index++, ch++ ) {
			diff = CTToLower( ustr.ptr[index] ) - CTToLower( ch->character );
			if ( diff )
				return diff;
		}
	}
		
	return 0;
}

/* ===================================================================== */
inline int ForwardSearchWord( CharRecordP			ch, 
							  int32 				start, 
							  int32 				end,
							  int32&				offset,
							  int					caseSensitive,
							  const USTR&	ustr )
{	
	for ( offset = start; offset + (int32)ustr.len <= end; offset++ ) {
		if ( !CharRecCmp( ch + offset, ustr, caseSensitive ) )
			return true;
	}
	return false;
}

/* ===================================================================== */
inline int ReverseSearchWord( CharRecordP			ch, 
							  int32 				start, 
							  int32 				end,
							  int32&				offset,
							  int					caseSensitive,
							  const USTR&	ustr )
{	
	for ( offset = end - (int32)ustr.len; offset > start; offset-- ) {
		if ( !CharRecCmp( ch + offset, ustr, caseSensitive ) )
			return true;
	}
	return false;
}

/* ===================================================================== */
int scCharArray::GetToken( USTR& ustr, 
						   int32			start, 
						   int32			end ) const
{
	scAssert( ustr.len > (ulong)(end - start) );
	scAssert( end - start > 0 );

	scHandleArrayLock	h( (scCharArray*)this );
	CharRecordP chRec = (CharRecordP)*h;
	
	chRec += start;

	int len = end - start;	 
	WCHAR* ptr = (WCHAR*)ustr.ptr;

	for ( int i = 0; i < len; i++, chRec++ )
		ptr[i] = chRec->character;
	
	ustr.len = len;
	
	return len;
}

/* ===================================================================== */
int scCharArray::ReplaceToken( const USTR& ustr,
							   int32 start,
							   int32& end )
{
	if ( ustr.len == (ulong)(end - start) ) {
		scHandleArrayLock	h( this );
		CharRecordP chRec = (CharRecordP)*h;

		chRec += start;

		for ( ulong i = 0; i < ustr.len; i++ )
			chRec[i].character = ustr.ptr[i];

		return 1;
	}
	DebugBreak();
	return 0;
}

/* ===================================================================== */
int scCharArray::FindString( const USTR&	ustr,
							 const SearchState& 	flags,
							 int32					start,
							 int32					end,
							 int32& 				offset )
{
	scHandleArrayLock	h( this );
	CharRecordP chRec = (CharRecordP)*h;

	start = MAX( start, 0 );
	end = MIN( end, GetContentSize() );
	
	if ( !flags.reverse() )
		return ForwardSearchWord( chRec, start, end, offset,
								  flags.caseSensitive(), ustr );
	else
		return ReverseSearchWord( chRec, start, end, offset,
								  flags.caseSensitive(), ustr );
}
