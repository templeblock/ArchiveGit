// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern int image_read (LPTR ptr, unsigned size_of_ptr, int count, int ifh);

typedef struct	{
	int code;
	int chr;
	int first;
	int next;
} ENTRY;
typedef ENTRY far *LPENTRY;

static int	fpC, CurrentCode, CodeLengthBump, NextCodeIndex;
static LPENTRY	CodeTable;
static LPTR	Outbuf, lpOutbuf, lpOutLast;
static LPTR	Inbuf, lpInbuf, lpInLast;
static DWORD	Accum, CodesWritten;
static BYTE	nAccumBits, CodeLength, fError;

#define MAXCODES 4097 // Just so we can read a single 13 bit EOI code
#define OUTBUFSIZE 16384
#define INBUFSIZE 16384 /* at least MAX_IMAGE_LINE */
#define CODETABLESIZE ((long)sizeof(ENTRY) * MAXCODES)

static int iClrCode;
static int iEoiCode;
static int iCodeLength;
static int iCodeLenBumpFudge;
static int iMaxCode;

/***********************************************************************/
int CompressLZW_2 (ofp, lpIn, InCount, WriteCode, minCodeLength)
/***********************************************************************/
int	ofp;
LPTR	lpIn;
int	InCount;
int	(*WriteCode)();
int	minCodeLength;
{
BYTE	c;
int	Code, Count;

iCodeLength = minCodeLength;
iClrCode = 1 << (minCodeLength - 1);
iEoiCode = iClrCode+1;

CodesWritten = 0;
fpC = ofp;
fError = NO;

if ( lpIn == NULL || InCount == 0 ) /* Special start/end Compress call */
	{
	if ( !CodeLength )	/* Time to initialize */
		{
		if ( !(CodeTable = (LPENTRY)Alloc( CODETABLESIZE )) )
			{
			Message( IDS_EMEMALLOC );
			return( -1 );
			}
		if ( !(Outbuf = Alloc( (long)OUTBUFSIZE )) )
			{
			FreeUp( (LPTR)CodeTable );
			Message( IDS_EMEMALLOC );
			return( -1 );
			}
		lpOutbuf = Outbuf;
		lpOutLast = Outbuf + OUTBUFSIZE;
		InitCodeTable(); /* Initialize the table */
		Accum = 0;
		nAccumBits = 0;
		(*WriteCode) ( iClrCode ); /* ClearCode */
		}
	else	{		/* Time to terminate */
		if ( CurrentCode >= 0 )
			(*WriteCode) ( CurrentCode );
		(*WriteCode) ( iEoiCode ); /* EndOfInfoCode */
		FreeUp( (LPTR)CodeTable );
		FreeUp( Outbuf );
		CodeLength = 0;
		}
	return( fError ? -1 : CodesWritten );
	}

while (	--InCount >= 0 && !fError )
	{
	c = *lpIn++;
	if ( (Code = FindCode( CurrentCode, c )) < 0 )
		{
		(*WriteCode) ( CurrentCode );
		CheckoutCodeLength();
		AddCode( CurrentCode, c );
		if ( NextCodeIndex == iMaxCode )
			{ /* Ran out of 12 bit codes */
			(*WriteCode) ( iClrCode ); /* ClearCode */
			InitCodeTable(); /* Initialize */
			}
		CurrentCode = c;
		}
	else	CurrentCode = Code;
	}

return( fError ? -1 : CodesWritten );
}


/***********************************************************************/
int DecompressLZW_2 (ifp, lpOut, OutCount, ReadCode, minCodeLength)
/***********************************************************************/
int	ifp;
LPTR	lpOut;
int	OutCount;
int	(*ReadCode)();
int	minCodeLength;
{
int	Code, NewCode;

iCodeLength = minCodeLength;
iClrCode = 1 << (minCodeLength - 1);
iEoiCode = iClrCode+1;

CodesWritten = 0;
fpC = ifp;
fError = NO;

if ( lpOut == NULL || OutCount == 0 ) /* Special start/end Decompress call */
	{
	if ( !CodeLength )	/* Time to initialize */
		{
		if ( !(CodeTable = (LPENTRY)Alloc( CODETABLESIZE )) )
			{
			Message( IDS_EMEMALLOC );
			return( -1 );
			}
		if ( !(Outbuf = Alloc( (long)OUTBUFSIZE )) )
			{
			FreeUp( (LPTR)CodeTable );
			Message( IDS_EMEMALLOC );
			return( -1 );
			}
		lpOutbuf = Outbuf;
		lpOutLast = Outbuf + OUTBUFSIZE;
		if ( !(Inbuf = Alloc( (long)INBUFSIZE )) )
			{
			FreeUp( (LPTR)CodeTable );
			FreeUp( Outbuf );
			Message( IDS_EMEMALLOC );
			return( -1 );
			}
		lpInbuf = lpInLast = Inbuf + INBUFSIZE;
		InitCodeTable(); /* Initialize the table */
		Accum = 0;
		nAccumBits = 0;
		}
	else	{		/* Time to terminate */
		FreeUp( (LPTR)CodeTable );
		FreeUp( Outbuf );
		FreeUp( Inbuf );
		CodeLength = 0;
		}
	return( fError ? -1 : 1 );
	}

while( (Code = (*ReadCode) ()) >= 0 && !fError )
	{
	if ( Code == iEoiCode ) /* EndOfInformation */
		{
		CodeLength = iCodeLength;
		if ( nAccumBits % 8 ) /* Ensure next read is a byte boundary */
			nAccumBits = 0;
		Code = (*ReadCode) ();
		if ( Code != iClrCode ) /* ClearCode */
			break;
		InitCodeTable();
		continue;
		}
	if ( Code == iClrCode ) /* ClearCode */
		{
		InitCodeTable();
		continue;
		}
	if ( CurrentCode < 0 )
		/* Only occurs after an Init/Clear */
		BufferString( Code );
	else
	if ( Code < NextCodeIndex )
		{ /* Code is already in the table */
		AddCode( CurrentCode, FirstChar( Code ) );
		BufferString( Code );
		}
	else	{ /* Code is not yet in the table */
		NewCode = AddCode( CurrentCode, FirstChar( CurrentCode ));
		BufferString( NewCode );
		}

	CheckoutCodeLength();
	CurrentCode = Code;

	/* If we have accumulated more than the caller needs, get out */
	if ( (lpOutbuf - Outbuf) >= OutCount )
		break;
	}

/* Give the caller his/her data */
lmemcpy( lpOut, Outbuf, OutCount );
/* Move any remaining data to the top of the buffer */
lmemcpy( Outbuf, Outbuf + OutCount, (lpOutbuf - Outbuf) - OutCount );
lpOutbuf -= OutCount;
return( fError ? -1 : CodesWritten );
}


/***********************************************************************/
static void InitCodeTable()
/***********************************************************************/
{
LPENTRY lpEntry;
int index;

lpEntry = CodeTable;
for ( index=0; index < MAXCODES; index++ )
	{
	lpEntry->code = -1;
	lpEntry->chr = ( index < iClrCode ? index : -1 );
	lpEntry->first = -1;
	lpEntry->next  = -1;
	lpEntry++;
	}
NextCodeIndex = iClrCode+2; /* Don't use iClrCode and iEoiCode */
CodeLength = iCodeLength;
CodeLengthBump = (1 << CodeLength) - 1;
CurrentCode = -1;
}


/***********************************************************************/
static int FindCode( Code, Char )
/***********************************************************************/
int	Code;
BYTE	Char;
{
int	TheCode;
LPENTRY	lpEntry;

if ( Code < 0 ) /* Special case for the initial codes */
	return( Char );

if ( Code >= NextCodeIndex )
	{
	Print("Error finding LZW code");
	fError = YES;
	return( -1 );
	}

lpEntry = &CodeTable[ Code ];
TheCode = lpEntry->first;

while( TheCode >= 0 )
	{
	lpEntry = &CodeTable[ TheCode ];
	if ( lpEntry->code != Code )
		{
		Print("Bad LZW chain for code %d", Code );
		fError = YES;
		return( -1 );
		}
	if ( lpEntry->chr == Char )
		return( TheCode );
	TheCode = lpEntry->next;
	}

return( -1 );
}


/***********************************************************************/
static int AddCode( Code, Char )
/***********************************************************************/
int	Code;
BYTE	Char;
{
LPENTRY	lpEntry, lpCur;

if ( NextCodeIndex < 0 || NextCodeIndex >= MAXCODES )
	{
	Print("Error in LZW; next code index %d is out of bounds",
		NextCodeIndex );
	fError = YES;
	return( -1 );
	}

if ( Code < 0 || Code >= NextCodeIndex )
	{
	Print("Error in LZW; add code %d beyond current code %d",
		Code, NextCodeIndex );
	fError = YES;
	return( -1 );
	}

lpCur = &CodeTable[ Code ];
lpEntry = &CodeTable[ NextCodeIndex ];
lpEntry->code = Code;
lpEntry->chr = Char;
lpEntry->next = lpCur->first;
lpCur->first  = NextCodeIndex;
NextCodeIndex++;
return( NextCodeIndex-1 );
}


/***********************************************************************/
void CheckoutCodeLength()
/***********************************************************************/
{

/* If the next code index we're going to use is too big, update code length */
if ( NextCodeIndex == CodeLengthBump + iCodeLenBumpFudge )
	{
	/* Don't let the code length exceed 12 bits (GIF) */
	if ((iCodeLenBumpFudge) && (CodeLength == 12))
		return;

	CodeLengthBump <<= 1;
	CodeLengthBump++;
	CodeLength++;
	}
}


/***********************************************************************/
static BYTE FirstChar( Code )
/***********************************************************************/
int	Code;
{
int	TheCode;
LPENTRY	lpEntry;

if ( Code < 0 || Code >= NextCodeIndex )
	{
	Print("Error finding first LZW character");
	fError = YES;
	return('.');
	}

lpEntry = &CodeTable[ Code ];
while ( (TheCode = lpEntry->code) >= 0 )
	lpEntry = &CodeTable[ TheCode ];

return( lpEntry->chr );
}


/***********************************************************************/
static void BufferString( Code )
/***********************************************************************/
int	Code;
{
#define BUFSIZE 1024
int	TheCode, Count;
PTR	Ptr;
BYTE	Buffer[BUFSIZE];
LPENTRY	lpEntry;

if ( Code < 0 || Code >= NextCodeIndex )
	{
	Print("Error buffering LZW string");
	fError = YES;
	return;
	}

/* Traverse the code list starting at Code and output the characters */
Count = 0;
TheCode = Code;
Ptr = &Buffer[BUFSIZE];
do	{
	lpEntry = &CodeTable[ TheCode ];
	*(--Ptr) = lpEntry->chr;
	Count++;
	if ( Count >= BUFSIZE )
		Print("Oops! buffer output %d > %d", Count, BUFSIZE);
	}
	while ( (TheCode = lpEntry->code) >= 0 );

CodesWritten += Count;
lmemcpy( lpOutbuf, Ptr, Count );
lpOutbuf += Count;
if ( lpOutbuf >= lpOutLast )
	Print("Oops! Exceeded the output buffer");
}


/***********************************************************************/
static int TiffReadCode()
/***********************************************************************/
{
int Code, Count;

while (1)
	{
	if ( lpInbuf >= lpInLast )	/* If the input buffer is empty */
		{			/* then fill it */
		lpInbuf = Inbuf;
		if (!(Count = image_read(Inbuf, 1, INBUFSIZE, fpC )))
			{
			return( -1 );
			}
		lpInLast = Inbuf + Count;
		}
	nAccumBits += 8;
	Accum |= ( (DWORD)(*lpInbuf++) << (32-nAccumBits) );
	if ( nAccumBits >= CodeLength )
		{ /* We have enough to return a full code */
		Code = (Accum >> (32-CodeLength));
		Accum <<= CodeLength;
		nAccumBits -= CodeLength;
		if (Code == iEoiCode)
			{
			image_read_startstrip ();
			lpInbuf = lpInLast = Inbuf + INBUFSIZE;
			nAccumBits = 0;
			Accum = 0;
			}
		return( Code );
		}
	}
}


/***********************************************************************/
int DecompressLZW( ifh, lpOut, OutCount )
/***********************************************************************/
int	ifh;
LPTR	lpOut;
int	OutCount;
{
    iCodeLenBumpFudge = 0;
    iMaxCode = 4095;
    return (DecompressLZW_2 (ifh, lpOut, OutCount, TiffReadCode, 9));
}


/***********************************************************************/
static int GifReadCode()
/***********************************************************************/
{
int Code, Count;
BYTE	blkSize;

/* bit mask for extracting code bits */
static WORD wCodeMask [] = {0x000, 0x001, 0x003, 0x007, 0x00F, 0x01F,
		    0x03F, 0x07F, 0x0FF, 0x1FF, 0x3FF, 0x7FF, 0xFFF};

while (1)
	{
	if ( lpInbuf >= lpInLast )	/* If the input buffer is empty */
		{			/* then fill it */

	/* Get number of bytes in the block */
		if (_lread (fpC, &blkSize, 1) == -1)
			return( -1 );

	/* check for zero length block (should not happen) */
		if (blkSize == 0)
			return( -1 );

		lpInbuf = Inbuf;
		Count = _lread( fpC, Inbuf, blkSize );
		if (Count == -1)
			return( -1 );
		lpInLast = Inbuf + Count;
		}
	if ( nAccumBits > CodeLength )
		{ /* We have enough to return a full code */
		Code = (Accum & wCodeMask [CodeLength] );
		Accum >>= CodeLength;
		nAccumBits -= CodeLength;
		return( Code );
		}
	else	{
		Accum |= (DWORD)(*lpInbuf++) << nAccumBits;
		nAccumBits += 8;
		}
	}
}


/***********************************************************************/
int DecompressLZW_GIF( ifh, lpOut, OutCount, codeSize )
/***********************************************************************/
int	ifh;
LPTR	lpOut;
int	OutCount;
int	codeSize;
{
    iCodeLenBumpFudge = 1;
    iMaxCode = 4095;
    return (DecompressLZW_2 (ifh, lpOut, OutCount, GifReadCode, codeSize+1));
}


/***********************************************************************/
static void TiffWriteCode( Code )
/***********************************************************************/
int Code;
{
int Count;

nAccumBits += CodeLength;
Accum |= (DWORD)Code << (32-nAccumBits);
CodesWritten++;
if ( nAccumBits > 16 )
	{ /* There is output */
	*lpOutbuf++ = HIBYTE(HIWORD(Accum));
	*lpOutbuf++ = LOBYTE(HIWORD(Accum));
	if ( lpOutbuf >= lpOutLast )
		{
		Count = lpOutbuf - Outbuf;
		if ( _lwrite( fpC, Outbuf, Count ) != Count )
			Message( IDS_EWRITEFILE );
		lpOutbuf = Outbuf;
		}
	Accum <<= 16;
	nAccumBits -= 16;
	}
if (Code == iEoiCode)
	{
	TiffWriteCode( NULL ); /* Make sure the EOI is flushed from */
	TiffWriteCode( NULL ); /* the Accumulator */
	Count = lpOutbuf - Outbuf;
	if ( _lwrite( fpC, Outbuf, Count ) != Count )
		Message( IDS_EWRITEFILE );
	}
}


/***********************************************************************/
int CompressLZW( ofp, lpIn, InCount )
/***********************************************************************/
int	ofp;
LPTR	lpIn;
int	InCount;
{
	iCodeLenBumpFudge = 0;
	iMaxCode = 4094;
	return ( CompressLZW_2 (ofp, lpIn, InCount, TiffWriteCode, 9));
}


/***********************************************************************/
static void GifWriteCode( Code )
/***********************************************************************/
int Code;
{
int	Count;
BYTE	blkSize;

Accum |= (DWORD)Code << nAccumBits;
nAccumBits += CodeLength;
CodesWritten++;
while ( nAccumBits > 8 )
	{ /* There is output */
	*lpOutbuf++ = LOBYTE(Accum);
	blkSize = lpOutbuf - Outbuf;
	if ( blkSize == 255 )
		{
		if ( _lwrite( fpC, &blkSize, 1 ) != 1 )
			Message( IDS_EWRITEFILE );
		if ( _lwrite( fpC, Outbuf, blkSize ) != blkSize )
			Message( IDS_EWRITEFILE );
		lpOutbuf = Outbuf;
		}
	Accum >>= 8;
	nAccumBits -= 8;
	}
if (Code == iEoiCode)
	{
	if (nAccumBits)
	    *lpOutbuf++ = LOBYTE(Accum);

	blkSize = lpOutbuf - Outbuf;
	if (blkSize)
		{
		if ( _lwrite( fpC, &blkSize, 1 ) != 1 )
			Message( IDS_EWRITEFILE );
		if ( _lwrite( fpC, Outbuf, blkSize ) != blkSize )
			Message( IDS_EWRITEFILE );
		}
	}
}


/***********************************************************************/
int CompressLZW_GIF ( ofp, lpIn, InCount, codeSize )
/***********************************************************************/
int	ofp;
LPTR	lpIn;
int	InCount;
int	codeSize;
{
    iCodeLenBumpFudge = 1;
    iMaxCode = 4094;
    return ( CompressLZW_2 (ofp, lpIn, InCount, GifWriteCode, codeSize+1));
}
