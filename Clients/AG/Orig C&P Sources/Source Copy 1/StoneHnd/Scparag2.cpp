/*=================================================================
 
	File:		scparag2.c

	$Header: /Projects/Toolbox/ct/Scparag2.cpp 4     5/30/97 8:45a Wmanis $

	Contains:	content unit implementations

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

=================================================================*/


#include "scparagr.h"
#include "scctype.h"
#include "scspcrec.h"
#include "scmem.h"
#include "scannota.h"

#ifdef _RUBI_SUPPORT
#include "scrubi.h"
#endif

#define INLINE static

/***********************************************************************/ 

INLINE void LoadNewWord( CharRecordP	ch,
						 const UCS2*	replaceCh,
						 long			size )
{
	for ( ; size--; )
		(ch++)->character = *replaceCh++;
}

/***********************************************************************/ 


INLINE void BuildTestWord( UCS2*		ch,
						   CharRecordP	charRec,
						   long			size )
{	

	for ( ; size--; ) 
		*ch++ = (charRec++)->character;
	*ch = 0;
}

/***********************************************************************/

INLINE long FindNextSpellingWord( CharRecordP	startChRec, 
								  long			endOffset, 
								  long			limitOffset )
{
	UCS2 ch = startChRec[endOffset].character;
	
	while ( !CTIsAlpha( ch ) && endOffset < limitOffset )
		ch = startChRec[endOffset++].character;
	return endOffset;
}

/***********************************************************************/

void scContUnit::Iter( SubstituteFunc	func,
					   long				startLocation,
					   long&			limitOffset )
{
	UCS2			chBuf[64];
	UCS2*			chP;
	CharRecordP		startChRec;
	scSpecRecord*	specRec;
	long			startOffset,
					endOffset,
					wordLen;
	
	LockMem( startChRec, specRec );
	
	startOffset = startLocation;
	endOffset 	= startOffset;
	
	for ( ; endOffset < limitOffset; ) {
		startOffset = TXTStartSelectableWord( startChRec, endOffset );
		endOffset 	= TXTEndSelectableWord( startChRec, endOffset );
		wordLen = endOffset - startOffset;
		
		if ( wordLen > 1 ) {
			BuildTestWord( chBuf, startChRec + startOffset, wordLen );
			status stat = (*func)( &chP, chBuf, NULL );
			
			if ( stat == scSuccess || stat == scUserAbort ) {
				if ( !ReplaceWord( startChRec, specRec, startOffset, endOffset,
								   limitOffset, chBuf, chP ) ) {
						UnlockMem( );
						return;
				}
				
				fCharArray.RepairText( fSpecRun, startOffset, endOffset );
				Mark( scREBREAK );
				
				if ( stat == scUserAbort )
					goto exit;
			}
		}
		endOffset = FindNextSpellingWord( startChRec, endOffset, limitOffset );
	}

exit:
	UnlockMem( );
}

/***********************************************************************/

int scContUnit::ReplaceToken( const stUnivString& ustr, 
							  int32				  start, 
							  int32&			  end )
{
	ForceRepaint( start, end );	
	Mark( scREBREAK );

	if ( ustr.len == (ulong)(end - start) && fCharArray.ReplaceToken( ustr, start, end ) ) {
		fCharArray.RepairText( fSpecRun, start, end );
	}
	else if ( fCharArray.Insert( ustr, start, end ) ) {
		int32	diff = ( ustr.len - ( end - start ) );
		fSpecRun.BumpOffset( start, diff );

		end += diff;
		TypeSpec ts;
		fCharArray.Retabulate( fSpecRun, start, end, ts, 
							   fCharArray.GetContentSize() );
	}
	else {
		scAssert( 0 );
	}
#if SCDEBUG > 0
	fCharArray.Validate();
	fSpecRun.DebugRun( "ReplaceToken" );
#endif
	return 1;
}

/* ===================================================================== */

int scContUnit::GetToken( stUnivString&	ustr, 
						  int32			start, 
						  int32			end ) const
{
	return fCharArray.GetToken( ustr, start, end );	
}

/* ===================================================================== */
// memory comes in here locked

Bool scContUnit::ReplaceWord( CharRecordP&		startChRec,
							  scSpecRecord*&	specRec,
							  long				startOffset,
							  long&				endOffset,
							  long&				limitOffset,
							  UCS2*				chBuf,
							  UCS2*				replaceBuf )
{
	long	deltaLen;
			
	deltaLen = CharacterBufLen( replaceBuf ) - CharacterBufLen( chBuf );
	
	if ( !deltaLen ) {
		LoadNewWord( startChRec + startOffset,
					 replaceBuf,
					 endOffset - startOffset );
		return false;
	}
	
		// need to grow or shrink memory
//	if ( deltaLen > 0 )	// grow the memory - first resize and then move
//		PARASetChSize( p, startChRec, deltaLen );
		

//	memmove( *startChRec + *endOffset + deltaLen,
//			*startChRec + *endOffset,
//			(size_t)( p->fChSize - *endOffset + 1 ) * sizeof( CharRecord ) );
//	p->fChSize		+= deltaLen;
//	*limitOffset	+= deltaLen;
//	*endOffset		+= deltaLen;

//	scAssert( (*startChRec + p->fChSize)->character == 0 );					

//	if ( deltaLen < 0 ) // shrink the memory - first move and then resize
//		PARASetChSize(  p, startChRec, deltaLen );

	
	LoadNewWord( startChRec + startOffset, replaceBuf, endOffset - startOffset );

	fSpecRun.BumpOffset( endOffset - deltaLen, deltaLen );
	specRec = fSpecRun.ptr( );

	return true;
}

/* ===================================================================== */

int scContUnit::FindString( const stUnivString& ustr,
						    const SearchState&	flags,
							int32				start,
							int32				end,
							int32&				offset )
{
	return fCharArray.FindString( ustr, flags, start, end, offset );
}

/* ===================================================================== */

Bool scContUnit::FindString( const UCS2*	findString, 
							 const SearchState&	flags,
							 long&			startOffset,
							 long&			endOffset )
{
	stUnivString ustr;
	ustr.ptr = findString;
	ustr.len = CharacterBufLen( findString );

	if ( fCharArray.FindString( ustr, flags, startOffset, endOffset, startOffset ) ) {
		endOffset = startOffset + ustr.len;
		return true;
	}
	return false;
}

/* ===================================================================== */

#ifdef _RUBI_SUPPORT

Bool scContUnit::GetAnnotation( int				nth, 
								long			start, 
								long			end, 
								scAnnotation&	annotation )
{
	if ( fRubiArray ) {	
		scRubiData	rd;
		int			index;
		
		if ( fRubiArray->GetNthRubi( index, rd, nth, start, end ) ) {
			annotation.Set( rd.fCh, GetCount(), rd.fStartOffset, rd.fEndOffset );
			return true;
		}
	}
	return false;
}

/* ==================================================================== */

void scContUnit::ApplyAnnotation( long			start,
								  long			end,
								  const scAnnotation&	annot )
{
	eChTranType	chTranType	= eNormalTran;
	
	if ( !fRubiArray ) {
		AllocRubiArray();
	}
	else if ( fRubiArray->IsRubiData( start, end ) ) {
		int			nth;
		int			index;
		scRubiData	rd;	
		
		fCharArray.Transform( start, end, eRemoveJapTran, end - start );
		for ( nth = 1; fRubiArray->GetNthRubi( index, rd, nth, start, end );  )
			fRubiArray->RemoveDataAt( index );
	}
	
	if ( annot.fAnnotate ) {
		scRubiData rd( annot.fCharStr, start, end, SpecAtOffset( start + 1 ) );
	
		fRubiArray->AddRubiData( rd );
		
		chTranType = eRubiTran;
	}
	else {
			// i should have already removed any annotations
		if ( fRubiArray->GetNumItems() == 0 )
			DeleteRubiArray();
		chTranType = eRemoveJapTran;
	}
	Mark( scREBREAK );
	ForceRepaint( start, end );	
	fCharArray.Transform( start, end, chTranType, end - start );
}

#endif

/* ==================================================================== */

int scContUnit::operator==( const scContUnit& p2 ) const
{
	if ( GetContentSize() != p2.GetContentSize() )
		return 0;
	
	if ( fSpecRun != p2.fSpecRun )
		return 0;
  
  	return fCharArray == p2.fCharArray;
}

/* ==================================================================== */
