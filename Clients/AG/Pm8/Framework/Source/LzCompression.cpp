// ****************************************************************************
//
//  File Name:			lzCompression.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				B. Rosenblum
//
//  Description:		lz compression and decompression
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/LzCompression.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "lzCompression.h"

#define kLzMinString			3		// lower limit for string length - Minimum key value
#define kLzMaxLengthBits	8		// upper limit +1 of length bits value - Maximum key value +1
//	Note: we do not fully understand why 7 is the largest valid value for LengthBits, however
//		in testing we have found that value 8 or larger seem to corrupt memory. Further
//		research is left as an exercise to the reader. In the mean time, our code is asserted
//		to make sure that we don't exceed the valid range of 3 to 7. - BDR 1/24/96
#define kLzLimit 				16		// upper limit (plus 1) of LZ compression string

typedef struct							//	used for LzPack routine
{
	int		iMaxString;
	int		iCBufferSize;
	int		iMatchLength;
	int		iMatchPosition;
	sWORD*	pswDad;
	sWORD*	pswLson;
	sWORD*	pswRson;
	PUBYTE	pubCBuffer;				// circular buffer containing the dictionary
} PackDict, *pPackDict;

static void _InsertNode(int r, pPackDict ppackDict);
static void _DeleteNode(int p, pPackDict ppackDict);

// ****************************************************************************
//
// Function Name:		LZunpack
//
// Description:		Unpack a buffer of bytes packed with LZPack
//
//	Inputs:				PUBYTE TheInputData: pointer to compressed data
//							PUBYTE TheOutputData: pointer to buffer of at least OutputDataSize bytes
//							sLONG OutputDataSize: size of output data (not input data size!)
//							sWORD TheLengthBits: length of best compression string for this data
//
// Returns:				OutputDataSize
//
// Exceptions:			throws exception if internal memory allocation new fails
//
//	Notes:				Tests of PSD3 graphics have shown that they average saving
//							about 1 byte in 4 with this compression. In other words, graphics
//							compressed with this scheme are about 75% of their original size.
//
// ****************************************************************************
//
sLONG LZunpack( const uBYTE* TheInputData, PUBYTE TheOutputData,
							sLONG OutputDataSize, sWORD TheLengthBits)
{
	PUBYTE	pCBufferObj;
	PUBYTE	pCBuffer;
	uBYTE	uc;
	uWORD	uwMatchLength, uwMatchPosition, uwFlags, uwRingOffset;
	sWORD	swPosBits, swMaxString, swCBufferSize;
	sLONG	slBytesOut;

	TpsAssert( NULL != TheInputData, "Bad input pointer" );
	TpsAssert( NULL != TheOutputData, "Bad output pointer" );
	TpsAssert( 0xFFFF > OutputDataSize, "Bad output data size" );
	TpsAssert( (0 < TheLengthBits) && (kLzMaxLengthBits > TheLengthBits), "Bad length bits" );

	slBytesOut = 0;
	swPosBits = static_cast<sWORD>( kLzLimit - TheLengthBits );
	swMaxString = static_cast<sWORD>( (1 << TheLengthBits) + kLzMinString - 1 ); // upper limit for string length 
	swCBufferSize = static_cast<sWORD>( 1 << swPosBits );			// size of the circular buffer 
	uwRingOffset = static_cast<uWORD>( swCBufferSize - swMaxString );		// begin near end of buffer 
	uwFlags = 0;									// clear out flags int so that it will be refilled (with real flags) as soon as we enter the loop 

	pCBufferObj = new uBYTE [swCBufferSize + swMaxString];
	pCBuffer = pCBufferObj;
	memset (pCBuffer, 0, swCBufferSize + swMaxString);	// requires buffer to be zeroed

	while (slBytesOut < OutputDataSize)
	{
		uwFlags >>= 1;
		if (!(uwFlags & 0x0100))				// get new set of 8 flags (a single char) 
			uwFlags = static_cast<uWORD>( *TheInputData++ | 0xFF00 );	// uses higher byte cleverly to count eight 

		if (uwFlags & 1)
		{
			uc = *TheInputData++;				// get unencoded character 
			*TheOutputData++ = uc;
			pCBuffer[uwRingOffset++] = uc;
			++slBytesOut;
			uwRingOffset &= (swCBufferSize - 1);
		}
		else
		{
			uWORD uwIndex, j;
			
			uwIndex = *TheInputData++;			// get encoded character

			uwMatchPosition = *TheInputData++;
			uwMatchPosition = static_cast<uWORD>( ((uwIndex << 8) | uwMatchPosition) );
			uwMatchPosition <<= TheLengthBits;
			uwMatchPosition >>= TheLengthBits;

			uwMatchLength = static_cast<uWORD>( (uwIndex >> (swPosBits-8)) + kLzMinString );
			for (j = 0; j < uwMatchLength; j++)
			{							// copy string to output, and to new position in circular buffer 
				uc = pCBuffer[(uwMatchPosition + j) & (swCBufferSize - 1)];
				*TheOutputData++ = uc;
				if (++slBytesOut == OutputDataSize)
					break;
				pCBuffer[uwRingOffset++] = uc;
				uwRingOffset &= (swCBufferSize - 1);
			}
		}
	}
	delete pCBufferObj;
	return slBytesOut;
}

// ****************************************************************************
//
// Function Name:		FindBestLZPackKey
//
// Description:		Find best key size for packing a buffer with LZ Compression
//
//	Inputs:				PUBYTE puSource: pointer to data to compress
//							sLONG slInputSize: size of data to compress
//
// Outputs:				PSLONG pslCompressedSize: length of best compressed data for this key
//
// Returns:				best value for length bits to get maximum compression
//
// Exceptions:			none 
//
//	Notes:				Call this routine to find out the TheLengthBits value to use
//							when calling PackLZ for a given buffer
//
// ****************************************************************************
//
sWORD FindBestLZPackKey ( const uBYTE* puSource, sLONG slInputSize, PSLONG pslCompressedSize)
{
	int	i;
	sWORD	swBestKey = 0;
	sLONG	slCompressedSize, slBestCompressedSize;

	slBestCompressedSize = LONG_MAX;
	for(i = kLzMinString; i < kLzMaxLengthBits; i++)
	{
		slCompressedSize = PackLZ(NULL,
											puSource,
											slInputSize,
											static_cast<sWORD>( i ),
											FALSE);

		if(slCompressedSize < slBestCompressedSize)
		{
			slBestCompressedSize = slCompressedSize;
			swBestKey = static_cast<sWORD>( i );
		}
	}
	*pslCompressedSize = slBestCompressedSize;
	return swBestKey;
}

// ****************************************************************************
//
// Function Name:		PackLZ
//
// Description:		Pack a buffer of bytes with LZ Compression scheme
//
//	Inputs:				PUBYTE Dest: pointer to compressed data of at least InputSize bytes
//							PUBYTE Source: pointer to data to compress
//							sLONG InputSize: size of data to compress
//							sWORD TheLengthBits: length of best compression string for this data
//							BOOLEAN PackFlag: flag to actually write output data
//
// Returns:				length of compressed data
//
// Exceptions:			throws exception if internal memory allocation new fails
//
//	Notes:				Note that if this routine is called with PackFlag = false,
//							then Dest can be NULL. This allows calling this routine
//							with several different values of TheLengthBits to determine
//							the best value for compression. Then call this routine with
//							PackFlag = true and a valid Dest pointer.
//
// ****************************************************************************
//
sLONG PackLZ (PUBYTE Dest, const uBYTE* Source, sLONG InputSize,
							sWORD TheLengthBits, BOOLEAN PackFlag)
{
	uBYTE 			uc;
	uBYTE				mask, code_flags, code_buf[kLzLimit];
	int				i, s, r;
	int				iPosBits, iLength, iLastMatchLength, iCodeBufPtr;
	unsigned int	iDadStart, iLSonStart, iRSonStart, iTotal;
	unsigned int	code;
	sLONG 			input_position, output_position;
	PackDict			packDict;

	//	next assert is warning only
	TpsAssert( 0x7FFF > InputSize, "Warning: Untested for size > 32767" );

	TpsAssert( NULL != Source, "Bad input pointer" );
	TpsAssert( (NULL != Dest) || (!PackFlag), "Bad output pointer" );
	TpsAssert( 0xFFFF > InputSize, "Bad input data size" );
	TpsAssert( (0 < TheLengthBits) && (kLzMaxLengthBits > TheLengthBits), "Bad length bits" );

	input_position = 0;
	output_position = 0;

	iPosBits	= kLzLimit - TheLengthBits;
	packDict.iMaxString  = (1 << TheLengthBits) + kLzMinString - 1; // upper limit for string length
	packDict.iCBufferSize	= 1 << iPosBits;						// size of the circular buffer

	iDadStart  = packDict.iCBufferSize + packDict.iMaxString; // numbers of elements for arrays
	iLSonStart = iDadStart + (packDict.iCBufferSize + 1) * 2; //	...comprising binary tree structure
	iRSonStart = iLSonStart + (packDict.iCBufferSize + 1) * 2;
	iTotal = iRSonStart + (packDict.iCBufferSize + 257) * 2;
	TpsAssert(0xFFFF > iTotal, "Illegal buffer size");

	packDict.pubCBuffer = new uBYTE[iTotal];
	packDict.pswDad = (sWORD*)&packDict.pubCBuffer[iDadStart];
	packDict.pswLson = (sWORD*)&packDict.pubCBuffer[iLSonStart];
	packDict.pswRson = (sWORD*)&packDict.pubCBuffer[iRSonStart];

	for (i = packDict.iCBufferSize + 1; i <= packDict.iCBufferSize + 256; i++)
		packDict.pswRson[i] = static_cast<sWORD>( packDict.iCBufferSize );				// Fill pswRson and pswDad arrays with #4096

	for (i=0; i<packDict.iCBufferSize; i++)
		packDict.pswDad[i] = static_cast<sWORD>( packDict.iCBufferSize );
	
	code_flags = 0;
	iCodeBufPtr = 0;
	mask = 1;
	s = 0;
	r = packDict.iCBufferSize-packDict.iMaxString;		// Start near end of buffer
	for (i=s; i<r; i++)				 
		packDict.pubCBuffer[i] = 0;
	
	for (iLength = 0; iLength < packDict.iMaxString; iLength++)	// Read data into the last iMaxString buffer positions
	{
		if (input_position == InputSize)
			break;
		else
		{
			packDict.pubCBuffer[r + iLength] = Source[input_position];
			input_position++;
		}
	}
	
	// Insert the iMaxString strings, each of which begins with one or 
	// more 'space' characters.  Note the order in which these strings 
	// are inserted.  This way, degenerate trees will be less likely 
	// to occur.
	for (i = 1; i <= packDict.iMaxString; i++)
		_InsertNode(r - i, &packDict);
	
	_InsertNode(r, &packDict);									// Insert the whole string just read
																		// iMatchLength and iMatchPosition get set
	do
		{	
		if (packDict.iMatchLength > iLength)				// iMatchLength may be spuriously long
			packDict.iMatchLength = iLength;					// ...near the end of text
		
		if (packDict.iMatchLength < kLzMinString)
		{	
			packDict.iMatchLength = 1;							// Can't send a string (match too int)
			code_flags |= mask;									// 'Send one byte' flag
			code_buf[iCodeBufPtr++] = packDict.pubCBuffer[r]; // Send unencoded character
		}
		else
		{	
			code = packDict.iMatchPosition | ((packDict.iMatchLength - kLzMinString) << iPosBits);
			code_buf[iCodeBufPtr++] = (uBYTE) (code >> 8);
			code_buf[iCodeBufPtr++] = (uBYTE) code;
		}
		//	Note that 17 is allowed for iCodeBufPtr since next loop checks for <, not <=
		TpsAssertL( (0 <= iCodeBufPtr) && (17 > iCodeBufPtr), "Bad iCodeBufPtr", &iCodeBufPtr, 1);
	
		mask <<= 1;													// Shift mask left one bit
		if (! mask)
		{	
			if (PackFlag)
				Dest[output_position] = code_flags;
			output_position++;
			
			for (i = 0; i < iCodeBufPtr; i++) 				// Send at most 8 units of
			{
				if (PackFlag)
					Dest[output_position] = code_buf[i];
				output_position++;
			}
			code_flags = 0;										// Reset flags, output buffer position and flag mask
			iCodeBufPtr = 0;
			mask = 1;
		}
		iLastMatchLength = packDict.iMatchLength;
		for (i = 0; i < iLastMatchLength; i++)
		{	
			if (input_position == InputSize)
				break;
			else
			{
				uc = Source[input_position];
				input_position++;
			}
			_DeleteNode(s, &packDict);							// Delete old strings and read new bytes
			packDict.pubCBuffer[s] = uc;
			if (s < packDict.iMaxString - 1)					// If near end of buffer, extend buffer
				packDict.pubCBuffer[s + packDict.iCBufferSize] = uc;	// ...to make string comparison easier
			s = (s + 1) & (packDict.iCBufferSize - 1);	// Increment position in buffer "modulo iCBufferSize"
			r = (r + 1) & (packDict.iCBufferSize - 1);			
			_InsertNode(r, &packDict);							// Register the string in pubCBuffer[R..R+iMaxString-1]
		}
		while (i++ < iLastMatchLength)						// After the end of text, no need to read more data,
		{																//...but buffer may not be empty
			_DeleteNode(s, &packDict);
			s = (s + 1) & (packDict.iCBufferSize - 1);
			r = (r + 1) & (packDict.iCBufferSize - 1);
			if (--iLength)
				_InsertNode(r, &packDict);
		}
		
	} while (iLength > 0);										// Until length of string to be processed is zero
	
	if (iCodeBufPtr)
	{	
		if (PackFlag)
			Dest[output_position] = code_flags;
		output_position++;
		
		for (i = 0; i < iCodeBufPtr; i++)
		{
			if (PackFlag)
				Dest[output_position] = code_buf[i];
			output_position++;
		}
	}
	delete packDict.pubCBuffer;
	return(output_position);
}

// ****************************************************************************
//
// Function Name:		_InsertNode
//
// Description:		Inserts string of length iMaxString, pubCBuffer[r..r+iMaxString-1], 
//							into one of the	trees (pubCBuffer[r]'th tree) and returns the 
//							longest-match position and length via the global variables 
//							iMatchPosition and iMatchLength.
//							If iMatchLength = iMaxString, then removes the old node in favor of 
//							the new	one, because the old one will be deleted sooner.
//							Note r plays double role, as tree node and position in buffer.
//
//	Inputs:				REVIEW 
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
static void _InsertNode(int r, pPackDict ppackDict)
{
	int		i;
	sWORD		p, cmp;
	PUBYTE 	pubKey;
	
	cmp = 1;
	pubKey = &ppackDict->pubCBuffer[r];
	p = static_cast<sWORD>( ppackDict->iCBufferSize + 1 + pubKey[0] );
	ppackDict->pswRson[r] = ppackDict->pswLson[r] = static_cast<sWORD>( ppackDict->iCBufferSize );
	ppackDict->iMatchLength = 0;
	
	for ( ; ; )
	{
		if (cmp >= 0)
		{	
			if (ppackDict->pswRson[p] != ppackDict->iCBufferSize)
				p = ppackDict->pswRson[p];
			else
			{
				ppackDict->pswRson[p] = static_cast<sWORD>( r );
				ppackDict->pswDad[r] = p;
				return;
			}
		}
		else
		{
			if (ppackDict->pswLson[p] != ppackDict->iCBufferSize)
				p = ppackDict->pswLson[p];
			else
			{
				ppackDict->pswLson[p] = static_cast<sWORD>( r );
				ppackDict->pswDad[r] = p;
				return;
			}
		}
	
		for (i = 1; i < ppackDict->iMaxString; i++)
			if ((cmp = static_cast<sWORD>( pubKey[i] - ppackDict->pubCBuffer[p + i] )) != 0)
				break;
		
		if (i > ppackDict->iMatchLength)
		{
			ppackDict->iMatchPosition = p;
			if ((ppackDict->iMatchLength = i) >= ppackDict->iMaxString)
				break;
		}
	}
	
	ppackDict->pswDad[r] = ppackDict->pswDad[p];
	ppackDict->pswLson[r] = ppackDict->pswLson[p];
	ppackDict->pswRson[r] = ppackDict->pswRson[p];
	
	ppackDict->pswDad[ppackDict->pswLson[p]] = static_cast<sWORD>( r );
	ppackDict->pswDad[ppackDict->pswRson[p]] = static_cast<sWORD>( r );
	
	if (ppackDict->pswRson[ppackDict->pswDad[p]] == p)
		ppackDict->pswRson[ppackDict->pswDad[p]] = static_cast<sWORD>( r );
	else
		ppackDict->pswLson[ppackDict->pswDad[p]] = static_cast<sWORD>( r );
	ppackDict->pswDad[p] = static_cast<sWORD>( ppackDict->iCBufferSize );	/* remove p */
}

// ****************************************************************************
//
// Function Name:		_DeleteNode
//
// Description:		Deletes node p from tree 
//
//	Inputs:				REVIEW 
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
static void _DeleteNode(int p, pPackDict ppackDict)
{
	sWORD	q;

	if (ppackDict->pswDad[p] != ppackDict->iCBufferSize)
	{

		if (ppackDict->pswRson[p] == ppackDict->iCBufferSize)
			q = ppackDict->pswLson[p];
		else if (ppackDict->pswLson[p] == ppackDict->iCBufferSize)
			q = ppackDict->pswRson[p];
		else
		{
			q = ppackDict->pswLson[p];
			if (ppackDict->pswRson[q] != ppackDict->iCBufferSize) 
			{
				do
				{
					q = ppackDict->pswRson[q];
				} while (ppackDict->pswRson[q] != ppackDict->iCBufferSize);
			ppackDict->pswRson[ppackDict->pswDad[q]] = ppackDict->pswLson[q];
			ppackDict->pswDad[ppackDict->pswLson[q]] = ppackDict->pswDad[q];
			ppackDict->pswLson[q] = ppackDict->pswLson[p];
			ppackDict->pswDad[ppackDict->pswLson[p]] = q;
			}
			ppackDict->pswRson[q] = ppackDict->pswRson[p];
			ppackDict->pswDad[ppackDict->pswRson[p]] = q;
		}		
		ppackDict->pswDad[q] = ppackDict->pswDad[p];
		if (ppackDict->pswRson[ppackDict->pswDad[p]] == p)
			ppackDict->pswRson[ppackDict->pswDad[p]] = q;
		else
			ppackDict->pswLson[ppackDict->pswDad[p]] = q;
		ppackDict->pswDad[p] = static_cast<sWORD>( ppackDict->iCBufferSize );
	}
}

