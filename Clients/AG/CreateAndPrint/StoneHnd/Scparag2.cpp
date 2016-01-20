/*=================================================================
Contains:	content unit implementations
=================================================================*/

#include "scparagr.h"
#include "scctype.h"
#include "scspcrec.h"
#include "scmem.h"
#include "scannota.h"

#ifdef _RUBI_SUPPORT
	#include "scrubi.h"
#endif

/***********************************************************************/
static void BuildTestWord( UCS2*		ch,
						   CharRecordP	charRec,
						   long 		size )
{	
	while (--size >= 0)
		*ch++ = (charRec++)->character;
	*ch = 0;
}

/***********************************************************************/
static long FindNextSpellingWord( CharRecordP	startChRec, 
								  long			endOffset, 
								  long			limitOffset )
{
	UCS2 ch = startChRec[endOffset].character;
	while ( !CTIsAlpha( ch ) && endOffset < limitOffset )
		ch = startChRec[endOffset++].character;

	return endOffset;
}

/***********************************************************************/
status scContUnit::Iter( SubstituteFunc	func,
					   long 			startOffset,
					   long&			limitOffset )
{
	while (startOffset < limitOffset)
	{
		long endOffset = startOffset;
		fCharArray.SelectWord(startOffset, startOffset, endOffset);

		long wordLen = endOffset - startOffset;
		if ( wordLen > 0 )
		{
			CharRecordP startChRec = (CharRecordP)fCharArray.Lock();
			UCS2 chBuf[64];
			BuildTestWord( chBuf, startChRec + startOffset, wordLen );
			fCharArray.Unlock();

			UCS2* chP;
			status stat = (*func)( &chP, chBuf, NULL );
			if (stat == scUserAbort)
				return scUserAbort;

			if (stat == scSuccess)
			{
				if (!ReplaceWord(startOffset, endOffset, limitOffset, chP))
					return scUserAbort;
			}
		}

		CharRecordP startChRec = (CharRecordP)fCharArray.Lock();
		startOffset = FindNextSpellingWord(startChRec, endOffset, limitOffset);
		fCharArray.Unlock();
	}

	return scSuccess;
}

/***********************************************************************/
int scContUnit::ReplaceToken( const USTR& ustr, 
							  int32 			  start, 
							  int32&			  end )
{
	ForceRepaint( start, end ); 
	Mark( scREBREAK );

	if ( ustr.len == (ulong)(end - start) && fCharArray.ReplaceToken( ustr, start, end ) )
	{
		fCharArray.RepairText( fSpecRun, start, end );
	}
	else
	if ( fCharArray.Insert( ustr, start, end ) )
	{
		int32	diff = ( ustr.len - ( end - start ) );
		fSpecRun.BumpOffset( start, diff );

		end += diff;
		TypeSpec ts;
		fCharArray.Retabulate( fSpecRun, start, end, ts, 
							   fCharArray.GetContentSize() );
	}
	else
		scAssert( 0 );

#if SCDEBUG > 0
	fCharArray.Validate();
	fSpecRun.DebugRun( "ReplaceToken" );
#endif

	return 1;
}

/* ===================================================================== */
// memory comes in here locked

BOOL scContUnit::ReplaceWord( long startOffset,
							  long& endOffset,
							  long& limitOffset,
							  const UCS2* replaceBuf )
{
	long replaceLen = CharacterBufLen(replaceBuf);
	USTR ustr;
	ustr.ptr = replaceBuf;
	ustr.len = replaceLen;

	limitOffset -= endOffset;
	ReplaceToken(ustr, startOffset, endOffset);
	limitOffset += endOffset;
	return true;
}

/* ===================================================================== */

#ifdef _RUBI_SUPPORT

BOOL scContUnit::GetAnnotation( int 			nth, 
								long			start, 
								long			end, 
								scAnnotation&	annotation )
{
	if ( fRubiArray ) { 
		scRubiData	rd;
		int 		index;
		
		if ( fRubiArray->GetNthRubi( index, rd, nth, start, end ) ) {
			annotation.Set( rd.fCh, GetCount(), rd.fStartOffset, rd.fEndOffset );
			return true;
		}
	}
	return false;
}

/***********************************************************************/
void scContUnit::ApplyAnnotation( long			start,
								  long			end,
								  const scAnnotation&	annot )
{
	eChTranType chTranType	= eNormalTran;
	
	if ( !fRubiArray ) {
		AllocRubiArray();
	}
	else if ( fRubiArray->IsRubiData( start, end ) ) {
		int 		nth;
		int 		index;
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

/***********************************************************************/
int scContUnit::operator==( const scContUnit& p2 ) const
{
	if ( GetContentSize() != p2.GetContentSize() )
		return 0;
	
	if ( fSpecRun != p2.fSpecRun )
		return 0;
  
	return fCharArray == p2.fCharArray;
}
