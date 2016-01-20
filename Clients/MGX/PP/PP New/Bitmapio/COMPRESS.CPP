//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "stdafx.h"
#include "compress.h"
#include "tiffio.h"

// Static prototypes
static  void CheckoutCodeLength( LPLZW_STUFF lpLZW );
static  void InitCodeTable( LPLZW_STUFF lpLZW );
static  int FindCode(int Code,unsigned char Char,LPLZW_STUFF lpLZW);
static  int AddCode(int Code,unsigned char Char,LPLZW_STUFF lpLZW);
static  unsigned char FirstChar(int Code, LPLZW_STUFF lpLZW);
static  void BufferString(int Code, LPLZW_STUFF lpLZW);
static  int TiffReadCode(LPTIFFHEADER lpTH, LPLZW_STUFF lpLZW);
static  int GifReadCode( LPTIFFHEADER lpTH, LPLZW_STUFF lpLZW );
static  void TiffWriteCode(int Code, LPLZW_STUFF lpLZW);
static  void GifWriteCode(int Code, LPLZW_STUFF lpLZW);

#define MAXCODES 4097 // Just so we can read a single 13 bit EOI code
#define OUTBUFSIZE 16384
#define INBUFSIZE 16384 /* at least MAX_IMAGE_LINE */
#define CODETABLESIZE ((long)sizeof(CTENTRY) * MAXCODES)

/***********************************************************************/
LPLZW_STUFF CompressLZW_2( CFile *pFile, LPTR lpIn, int InCount,
                              void ( *WriteCode )( int Code, LPLZW_STUFF lpLZW ),
                              int minCodeLength, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
BYTE        c;
int	      Code;
BOOL		fError;

if( lpLZW )
   {   
   lpLZW->iCodeLength = minCodeLength;
   lpLZW->iClrCode = 1 << (minCodeLength - 1);
   lpLZW->iEoiCode = lpLZW->iClrCode+1;

   lpLZW->CodesWritten = 0;
   lpLZW->pFile = pFile;
   lpLZW->fError = NO;
   }

if ( lpIn == NULL || InCount == 0 ) /* Special start/end Compress call */
	{
	if ( !lpLZW )	/* Time to initialize */
		{
		if ( !(lpLZW = ( LPLZW_STUFF )Alloc( sizeof( LZW_STUFF ))) )
			{
			//Message( IDS_EMEMALLOC );
			return( FALSE );
			}
         lpLZW->iCodeLength = minCodeLength;
         lpLZW->iClrCode = 1 << (minCodeLength - 1);
         lpLZW->iEoiCode = lpLZW->iClrCode+1;

         lpLZW->CodesWritten = 0;
         lpLZW->pFile = pFile;
         lpLZW->fError = NO;
		if ( !(lpLZW->CodeTable = (LPCTENTRY)Alloc( CODETABLESIZE )) )
			{
			FreeUp( (LPTR)lpLZW );
			//Message( IDS_EMEMALLOC );
			return( FALSE );
			}
		if ( !(lpLZW->Outbuf = Alloc( (long)OUTBUFSIZE )) )
			{
			FreeUp( (LPTR)lpLZW->CodeTable );
			FreeUp( (LPTR)lpLZW );
			//Message( IDS_EMEMALLOC );
			return( FALSE );
			}
		lpLZW->lpOutbuf = lpLZW->Outbuf;
		lpLZW->lpOutLast = lpLZW->Outbuf + OUTBUFSIZE;
		InitCodeTable( lpLZW ); /* Initialize the table */
		lpLZW->Accum = 0;
		lpLZW->nAccumBits = 0;
		(*WriteCode) ( lpLZW->iClrCode, lpLZW ); /* ClearCode */
		fError = lpLZW->fError;
		}
	else	{		/* Time to terminate */
		if ( lpLZW->CurrentCode >= 0 )
			(*WriteCode) ( lpLZW->CurrentCode, lpLZW );
		(*WriteCode) ( lpLZW->iEoiCode, lpLZW ); /* EndOfInfoCode */
		FreeUp( (LPTR)lpLZW->CodeTable );
		FreeUp( lpLZW->Outbuf );
		fError = lpLZW->fError;
	  	FreeUp( (LPTR)lpLZW );
// tedr - why was this here?		lpLZW->CodeLength = 0;
		}
	return( fError ? NULL : lpLZW );
	}

while (	--InCount >= 0 && !lpLZW->fError )
	{
	c = *lpIn++;
	if ( (Code = FindCode( lpLZW->CurrentCode, c, lpLZW )) < 0 )
		{
		(*WriteCode) ( lpLZW->CurrentCode, lpLZW );
		CheckoutCodeLength(lpLZW);
		AddCode( lpLZW->CurrentCode, c, lpLZW );
		if ( lpLZW->NextCodeIndex == lpLZW->iMaxCode )
			{ /* Ran out of 12 bit codes */
			(*WriteCode) ( lpLZW->iClrCode, lpLZW ); /* ClearCode */
			InitCodeTable( lpLZW ); /* Initialize */
			}
		lpLZW->CurrentCode = c;
		}
	else	lpLZW->CurrentCode = Code;
	}

return( lpLZW->fError ? NULL : lpLZW );
}


/***********************************************************************/
LPLZW_STUFF DecompressLZW_2 ( CFile *pFile, LPTIFFHEADER lpTH,
                              LPTR lpOut, int OutCount,
                              int ( *ReadCode )( LPTIFFHEADER lpTH, LPLZW_STUFF lpLZW ),
                              int minCodeLength, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
int	Code, NewCode;

if( lpLZW )
   {   
   lpLZW->iCodeLength = minCodeLength;
   lpLZW->iClrCode = 1 << (minCodeLength - 1);
   lpLZW->iEoiCode = lpLZW->iClrCode+1;

   lpLZW->CodesWritten = 0;
   lpLZW->pFile = pFile;
   lpLZW->fError = NO;
   }

if ( lpOut == NULL || OutCount == 0 ) /* Special start/end Decompress call */
	{
	if ( !lpLZW )	/* Time to initialize */
		{
		if ( !(lpLZW = ( LPLZW_STUFF )Alloc( sizeof( LZW_STUFF ))) )
			{
			//Message( IDS_EMEMALLOC );
			return( FALSE );
			}
      lpLZW->iCodeLength = minCodeLength;
      lpLZW->iClrCode = 1 << (minCodeLength - 1);
      lpLZW->iEoiCode = lpLZW->iClrCode+1;

      lpLZW->CodesWritten = 0;
      lpLZW->pFile = pFile;
      lpLZW->fError = NO;

      lpLZW->iCodeLenBumpFudge = 0;
      lpLZW->iMaxCode = 4095;

		if ( !(lpLZW->CodeTable = (LPCTENTRY)Alloc( CODETABLESIZE )) )
			{
			FreeUp( (LPTR)lpLZW );
			//Message( IDS_EMEMALLOC );
			return( FALSE );
			}
		if ( !(lpLZW->Outbuf = Alloc( (long)OUTBUFSIZE )) )
			{
			FreeUp( (LPTR)lpLZW->CodeTable );
			FreeUp( (LPTR)lpLZW );
			//Message( IDS_EMEMALLOC );
			return( FALSE );
			}
		lpLZW->lpOutbuf = lpLZW->Outbuf;
		lpLZW->lpOutLast = lpLZW->Outbuf + OUTBUFSIZE;
		if ( !(lpLZW->Inbuf = Alloc( (long)INBUFSIZE )) )
			{
			FreeUp( (LPTR)lpLZW->CodeTable );
			FreeUp( lpLZW->Outbuf );
			FreeUp( (LPTR)lpLZW );
			//Message( IDS_EMEMALLOC );
			return( FALSE );
			}
		lpLZW->lpInbuf = lpLZW->lpInLast = lpLZW->Inbuf + INBUFSIZE;
		InitCodeTable( lpLZW ); /* Initialize the table */
		lpLZW->Accum = 0;
		lpLZW->nAccumBits = 0;
	   lpLZW->fIncomplete = NO;
		}
	else	{		/* Time to terminate */
		FreeUp( (LPTR)lpLZW->CodeTable );
		FreeUp( lpLZW->Outbuf );
		FreeUp( lpLZW->Inbuf );
		FreeUp( (LPTR)lpLZW );
		}
	return( lpLZW ? lpLZW : FALSE );
	}

// May already have enough data, if so, give the caller his/her data 
if ((lpLZW->lpOutbuf - lpLZW->Outbuf) >= OutCount )
{
	lmemcpy( lpOut, lpLZW->Outbuf, OutCount );
	/* Move any remaining data to the top of the buffer */
	lmemcpy( lpLZW->Outbuf, lpLZW->Outbuf + OutCount, (lpLZW->lpOutbuf - lpLZW->Outbuf) - OutCount );
	lpLZW->lpOutbuf -= OutCount;
 	return( lpLZW );
}

// finish last if there was one (usually not) 
if (lpLZW->fIncomplete)
	BufferString(lpLZW->CodeToFinish,lpLZW);
if(lpLZW->fIncomplete) // still incomplete
	return( lpLZW );

while( (Code = (*ReadCode) (lpTH, lpLZW)) >= 0 && !lpLZW->fError)
	{
	if ( Code == lpLZW->iEoiCode ) /* EndOfInformation */
		{
		lpLZW->CodeLength = lpLZW->iCodeLength;
		if ( lpLZW->nAccumBits % 8 ) /* Ensure next read is a byte boundary */
			lpLZW->nAccumBits = 0;
		Code = (*ReadCode) (lpTH, lpLZW);
		if ( Code != lpLZW->iClrCode ) /* ClearCode */
			break;
		InitCodeTable( lpLZW );
		continue;
		}
	if ( Code == lpLZW->iClrCode ) /* ClearCode */
		{
		InitCodeTable( lpLZW );
		continue;
		}
	if ( lpLZW->CurrentCode < 0 )
		/* Only occurs after an Init/Clear */
		BufferString( Code, lpLZW );
	else
	if ( Code < lpLZW->NextCodeIndex )
		{ /* Code is already in the table */
		AddCode( lpLZW->CurrentCode, FirstChar( Code, lpLZW ), lpLZW );
		BufferString( Code, lpLZW );
		}
	else	{ /* Code is not yet in the table */
		NewCode = AddCode( lpLZW->CurrentCode, FirstChar( lpLZW->CurrentCode, lpLZW ), lpLZW);
		BufferString( NewCode, lpLZW );
		}

	CheckoutCodeLength(lpLZW);
	lpLZW->CurrentCode = Code;

	/* If we have accumulated more than the caller needs, get out */
	if ( (lpLZW->lpOutbuf - lpLZW->Outbuf) >= OutCount )
		break;
	}

	if (!lpLZW->fError)
	{
		int iCount;

		/* Give the caller his/her data */
		lmemcpy( lpOut, lpLZW->Outbuf, OutCount );

		/* Move any remaining data to the top of the buffer */
		iCount = (lpLZW->lpOutbuf - lpLZW->Outbuf) - OutCount;

		if (iCount > 0)
			lmemcpy( lpLZW->Outbuf, lpLZW->Outbuf + OutCount, iCount );
		lpLZW->lpOutbuf -= OutCount;
	}

	return( lpLZW->fError ? FALSE : lpLZW );
}


/***********************************************************************/
static void InitCodeTable( LPLZW_STUFF lpLZW)
/***********************************************************************/
{
LPCTENTRY lpEntry;
int index;

lpEntry = lpLZW->CodeTable;
for ( index=0; index < MAXCODES; index++ )
	{
	lpEntry->code = -1;
	lpEntry->chr = ( index < lpLZW->iClrCode ? index : -1 );
	lpEntry->first = -1;
	lpEntry->next  = -1;
	lpEntry++;
	}
lpLZW->NextCodeIndex = lpLZW->iClrCode+2; /* Don't use iClrCode and iEoiCode */
lpLZW->CodeLength = lpLZW->iCodeLength;
lpLZW->CodeLengthBump = (1 << lpLZW->CodeLength) - 1;
lpLZW->CurrentCode = -1;
}


/***********************************************************************/
static int FindCode( int Code, BYTE Char, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
int	TheCode;
LPCTENTRY	lpEntry;

if ( Code < 0 ) /* Special case for the initial codes */
	return( Char );

if ( Code >= lpLZW->NextCodeIndex )
	{
//	dbg("Error finding LZW code");
	//Message( IDS_ECOMPRESS );
	lpLZW->fError = YES;
	return( -1 );
	}

lpEntry = &lpLZW->CodeTable[ Code ];
TheCode = lpEntry->first;

while( TheCode >= 0 )
	{
	lpEntry = &lpLZW->CodeTable[ TheCode ];
	if ( lpEntry->code != Code )
		{
//		dbg("Bad LZW chain for code %d", Code );
		//Message( IDS_ECOMPRESS );
		lpLZW->fError = YES;
		return( -1 );
		}
	if ( lpEntry->chr == Char )
		return( TheCode );
	TheCode = lpEntry->next;
	}

return( -1 );
}


/***********************************************************************/
static int AddCode( int Code, BYTE Char, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
LPCTENTRY	lpEntry, lpCur;

if ( lpLZW->NextCodeIndex < 0 || lpLZW->NextCodeIndex >= MAXCODES )
	{
//	dbg("Error in LZW; next code index %d is out of bounds",
//		lpLZW->NextCodeIndex );
	//Message( IDS_ECOMPRESS );
	lpLZW->fError = YES;
	return( -1 );
	}

if ( Code < 0 || Code >= lpLZW->NextCodeIndex )
	{
//	dbg("Error in LZW; add code %d beyond current code %d",
//		Code, lpLZW->NextCodeIndex );
	//Message( IDS_ECOMPRESS );
	lpLZW->fError = YES;
	return( -1 );
	}

lpCur = &lpLZW->CodeTable[ Code ];
lpEntry = &lpLZW->CodeTable[ lpLZW->NextCodeIndex ];
lpEntry->code = Code;
lpEntry->chr = Char;
lpEntry->next = lpCur->first;
lpCur->first  = lpLZW->NextCodeIndex;
lpLZW->NextCodeIndex++;
return( lpLZW->NextCodeIndex-1 );
}


/***********************************************************************/
void CheckoutCodeLength( LPLZW_STUFF lpLZW )
/***********************************************************************/
{

/* If the next code index we're going to use is too big, update code length */
if ( lpLZW->NextCodeIndex == lpLZW->CodeLengthBump + lpLZW->iCodeLenBumpFudge )
	{
	/* Don't let the code length exceed 12 bits (GIF) */
	if ((lpLZW->iCodeLenBumpFudge) && (lpLZW->CodeLength == 12))
		return;

	lpLZW->CodeLengthBump <<= 1;
	lpLZW->CodeLengthBump++;
	lpLZW->CodeLength++;
	}
}


/***********************************************************************/
static BYTE FirstChar( int Code, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
int	TheCode;
LPCTENTRY	lpEntry;

if ( Code < 0 || Code >= lpLZW->NextCodeIndex )
	{
//	dbg("Error finding first LZW character");
	//Message( IDS_ECOMPRESS );
	lpLZW->fError = YES;
	return('.');
	}

lpEntry = &lpLZW->CodeTable[ Code ];
while ( (TheCode = lpEntry->code) >= 0 )
	lpEntry = &lpLZW->CodeTable[ TheCode ];

return( lpEntry->chr );
}


/***********************************************************************/
static void BufferString( int Code, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
int	TheCode, Count;
LPTR lpBegin, lpEnd;
LPCTENTRY	lpEntry;

lpLZW->fIncomplete = NO;
if ( Code < 0 || Code >= lpLZW->NextCodeIndex)
	{
//	dbg("Error buffering LZW string");
	//Message( IDS_ECOMPRESS );
	lpLZW->fError = YES;
	return;
	}

/* Traverse the code list starting at Code and output the characters */
lpBegin = lpLZW->lpOutbuf;
TheCode = Code;
do	{
  	if ( lpLZW->lpOutbuf >= lpLZW->lpOutLast )
		{
		lpLZW->fIncomplete = YES;
		lpLZW->CodeToFinish = TheCode;
		break;
	  	}
	lpEntry = &lpLZW->CodeTable[TheCode];
	*(lpLZW->lpOutbuf++) = lpEntry->chr;
	}
	while ( (TheCode = lpEntry->code) >= 0 );

Count = lpLZW->lpOutbuf-lpBegin;
lpLZW->CodesWritten += Count;

// data id read in reverse order.. so reverse this new part of the buffer
lpEnd = lpLZW->lpOutbuf;
lpBegin--;
while ((++lpBegin) < (--lpEnd))
	{
	TheCode = *lpBegin;
	*lpBegin = *lpEnd;
	*lpEnd = TheCode;
	}
}
		 

/***********************************************************************/
static int TiffReadCode( LPTIFFHEADER lpTH, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
int Code;
UINT Count;

while (1)
	{
	if ( lpLZW->lpInbuf >= lpLZW->lpInLast )	/* If the input buffer is empty */
		{			/* then fill it */
		lpLZW->lpInbuf = lpLZW->Inbuf;
		if (!(Count = image_read(lpTH, lpLZW->Inbuf, INBUFSIZE, lpLZW->pFile, YES )))
			{
			return( -1 );
			}
		lpLZW->lpInLast = lpLZW->Inbuf + Count;
		}
	lpLZW->nAccumBits += 8;
	lpLZW->Accum |= ( (DWORD)(*lpLZW->lpInbuf++) << (32-lpLZW->nAccumBits) );
	if ( lpLZW->nAccumBits >= lpLZW->CodeLength )
		{ /* We have enough to return a full code */
		Code = (lpLZW->Accum >> (32-lpLZW->CodeLength));
		lpLZW->Accum <<= lpLZW->CodeLength;
		lpLZW->nAccumBits -= lpLZW->CodeLength;
		if (Code == lpLZW->iEoiCode)
			{
			image_read_startstrip (lpTH);
			lpLZW->lpInbuf = lpLZW->lpInLast = lpLZW->Inbuf + INBUFSIZE;
			lpLZW->nAccumBits = 0;
			lpLZW->Accum = 0;
			}
		return( Code );
		}
	}
return(-1);
}


/***********************************************************************/
LPLZW_STUFF DecompressLZW( CFile *pFile, LPTIFFHEADER lpTH, LPTR lpOut,
                              int OutCount, LPLZW_STUFF lpLZW )
/***********************************************************************/
{

if( lpLZW )
   {   
   lpLZW->iCodeLenBumpFudge = 0;
   lpLZW->iMaxCode = 4095;
   }

return (DecompressLZW_2 (pFile, lpTH, lpOut, OutCount, TiffReadCode, 9, lpLZW));
}


/***********************************************************************/
static int GifReadCode( LPTIFFHEADER lpTH, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
int Code, Count;
BYTE	blkSize;

/* bit mask for extracting code bits */
static WORD wCodeMask [] = {0x000, 0x001, 0x003, 0x007, 0x00F, 0x01F,
		     0x03F, 0x07F, 0x0FF, 0x1FF, 0x3FF, 0x7FF, 0xFFF};

TRY
	{
	while (1)
		{
		if ( lpLZW->lpInbuf >= lpLZW->lpInLast )	/* If the input buffer is empty */
			{			/* then fill it */

		/* Get number of bytes in the block */
			if (lpLZW->pFile->Read ( &blkSize, 1) != 1)
				return( -1 );

		/* check for zero length block (should not happen) */
			if (blkSize == 0)
				return( -1 );

			lpLZW->lpInbuf = lpLZW->Inbuf;
			Count = lpLZW->pFile->Read( lpLZW->Inbuf, blkSize );
			if (Count != blkSize )
				return( -1 );
			lpLZW->lpInLast = lpLZW->Inbuf + Count;
			}
		if ( lpLZW->nAccumBits > lpLZW->CodeLength )
			{ /* We have enough to return a full code */
			Code = (lpLZW->Accum & wCodeMask [lpLZW->CodeLength] );
			lpLZW->Accum >>= lpLZW->CodeLength;
			lpLZW->nAccumBits -= lpLZW->CodeLength;
			return( Code );
			}
		else	{
			lpLZW->Accum |= (DWORD)(*lpLZW->lpInbuf++) << lpLZW->nAccumBits;
			lpLZW->nAccumBits += 8;
			}
		}
	}
END_TRY
return(-1);
}


/***********************************************************************/
LPLZW_STUFF DecompressLZW_GIF( CFile *pFile, LPTR lpOut, int OutCount,
                                 int codeSize, LPLZW_STUFF lpLZW )
/***********************************************************************/
{

if( lpLZW )
   {   
   lpLZW->iCodeLenBumpFudge = 1;
   lpLZW->iMaxCode = 4095;
   }
return (DecompressLZW_2 (pFile, NULL, lpOut, OutCount, GifReadCode, codeSize+1, lpLZW));
}


/***********************************************************************/
static void TiffWriteCode( int Code, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
int Count;

lpLZW->nAccumBits += lpLZW->CodeLength;
lpLZW->Accum |= (DWORD)Code << (32-lpLZW->nAccumBits);
lpLZW->CodesWritten++;
if ( lpLZW->nAccumBits > 16 )
	{ /* There is output */
	*lpLZW->lpOutbuf++ = HIBYTE(HIWORD(lpLZW->Accum));
	*lpLZW->lpOutbuf++ = LOBYTE(HIWORD(lpLZW->Accum));
	if ( lpLZW->lpOutbuf >= lpLZW->lpOutLast )
		{
		Count = lpLZW->lpOutbuf - lpLZW->Outbuf;
		TRY
			{
			lpLZW->pFile->Write( lpLZW->Outbuf, Count );
			}
		CATCH(CFileException, e)
			{
			//Message( IDS_EWRITEFILE );
			}
		END_CATCH

		lpLZW->lpOutbuf = lpLZW->Outbuf;
		}
	lpLZW->Accum <<= 16;
	lpLZW->nAccumBits -= 16;
	}
if (Code == lpLZW->iEoiCode)
	{
	TiffWriteCode( 0, lpLZW ); /* Make sure the EOI is flushed from */
	TiffWriteCode( 0, lpLZW ); /* the Accumulator */
	Count = lpLZW->lpOutbuf - lpLZW->Outbuf;
	TRY
		{
		lpLZW->pFile->Write( lpLZW->Outbuf, Count );
		}
	CATCH(CFileException, e)
		{
		//Message( IDS_EWRITEFILE );
		}
	END_CATCH
	}
}


/***********************************************************************/
LPLZW_STUFF CompressLZW( CFile *pFile, LPTR lpIn, int InCount,
                           LPLZW_STUFF lpLZW )
/***********************************************************************/
{

   if( lpLZW )
      {
	   lpLZW->iCodeLenBumpFudge = 0;
	   lpLZW->iMaxCode = 4094;
      }
	return ( CompressLZW_2 (pFile, lpIn, InCount, TiffWriteCode, 9, lpLZW));
}


/***********************************************************************/
static void GifWriteCode( int Code, LPLZW_STUFF lpLZW )
/***********************************************************************/
{
BYTE blkSize;

TRY
	{
	lpLZW->Accum |= (DWORD)Code << lpLZW->nAccumBits;
	lpLZW->nAccumBits += lpLZW->CodeLength;
	lpLZW->CodesWritten++;
	while ( lpLZW->nAccumBits > 8 )
		{ /* There is output */
		*lpLZW->lpOutbuf++ = LOBYTE(lpLZW->Accum);
		blkSize = lpLZW->lpOutbuf - lpLZW->Outbuf;
		if ( blkSize == 255 )
			{
			lpLZW->pFile->Write( &blkSize, 1 );
			lpLZW->pFile->Write( lpLZW->Outbuf, blkSize );
			lpLZW->lpOutbuf = lpLZW->Outbuf;
			}
		lpLZW->Accum >>= 8;
		lpLZW->nAccumBits -= 8;
		}
	if (Code == lpLZW->iEoiCode)
		{
		if (lpLZW->nAccumBits)
			*lpLZW->lpOutbuf++ = LOBYTE(lpLZW->Accum);

		blkSize = lpLZW->lpOutbuf - lpLZW->Outbuf;
		if (blkSize)
			{
			lpLZW->pFile->Write( &blkSize, 1 );
			lpLZW->pFile->Write( lpLZW->Outbuf, blkSize );
			}
		}
	}
CATCH(CFileException, e)
	{
	//Message( IDS_EWRITEFILE );
	}
END_CATCH
}


/***********************************************************************/
LPLZW_STUFF CompressLZW_GIF ( CFile *pFile, LPTR lpIn, int InCount,
                                 int codeSize, LPLZW_STUFF lpLZW )
/***********************************************************************/
{

if( lpLZW )
   {   
   lpLZW->iCodeLenBumpFudge = 1;
   lpLZW->iMaxCode = 4094;
   }
return ( CompressLZW_2 (pFile, lpIn, InCount, GifWriteCode, codeSize+1, lpLZW));
}



