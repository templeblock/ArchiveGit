/*
//	LZW.CPP
//
//		Copyright (c) 1999 The Learning Company, Inc.
//
// LZW decompression routines.
//
// $Header: /PM8/Framework/Source/LzwBuffer.cpp 1     3/03/99 6:17p Gbeddow $
//
// $Log: /PM8/Framework/Source/LzwBuffer.cpp $
// 
// 1     3/03/99 6:17p Gbeddow
// 
// 1     1/25/99 3:36p Rgrenfell
// New file for support of GIF animation
// 
*/

#include "FrameworkIncludes.h"

#include <stdlib.h>
#include <string.h>

#include "lzwBuffer.h"

/* The buffer area. */

#define	DATA_BUFFER_SIZE 128
#define	LZW_BUFFER_SIZE 32767

#define	TABLE_SIZE			4096

/* Our string table. And which code will be added next. */

SHORT next_code_added;

SHORT code_width;
SHORT code_limit;

SHORT clear_code;
SHORT eoi_code;
SHORT first_code;
SHORT start_bits;
SHORT start_limit;

SHORT far *prefix_code;
BYTE far *append_character;
BYTE far *decode_stack;
BYTE far *decode_end;
BYTE far *byte_buffer;

BYTE far *current_byte;
USHORT buffered_bytes;

// PRIVATE StorageDevicePtr lzw_device;

BOOL reading_tiff;

SHORT last_code;
SHORT old_code;
UCHAR character;

SHORT remainder_width;
UCHAR shift_buffer[sizeof(LONG)];

PSTRING current_string;

/*
// Code reading variables.
*/

#define	ERROR_CODE	(-1)

#if 0
#define	CLEAR_CODE	(256)
#define	EOI_CODE		(257)
#define	FIRST_CODE	(258)
#define	START_BITS	(9)
#define	START_LIMIT	(512)
#endif

/* Construct the buffer stream handler for GIF image data.
	The raw data buffer supplied must be pointed directly to the first image data
	sub-block so that the block size can be read.  The size of the buffer
	specified must be relative to the current location of the stream up to the
	end of the stream, not the streams entire length.

	Returns : TRUE on success, FALSE upon failure.
 */
LZWDataBufferStream::LZWDataBufferStream(
	LPBYTE pRawData,		// The raw data stream pointed to the first image data sub-block.
	long lBufferSize )	// The length of the buffer from its current location to the end of the stream.
{
	m_pRawDataBuffer = pRawData;
	m_lBufferSize = lBufferSize;
	m_lDataRead = 0l;

	m_nBlockSize = 0;
	m_nBlockDataRead = 0;

	m_bHitImageTerminator = FALSE;
}

/* Routine to read a block of data of the requested size from the data stream.
	This will read as much data as it can from the data stream, but will not
	read past the end of the image data sub-blocks, or the end of the stream,
	which ever comes first.

	Returns : TRUE if at least partial data could be read,
				 FALSE if no data could be read at all.
 */
bool LZWDataBufferStream::Read(
	LPBYTE pDataBuffer,		// The data buffer to fill in.
	long lBufferSize,			// The size of the data block to read.  (The size of the buffer?)
	USHORT *puBytesRead )	// Supplies to the caller the number of actual bytes read.
{
	ASSERT( pDataBuffer != NULL );
	ASSERT( puBytesRead != NULL );

	// Initialize to a valid read state.
	bool bValidRead = TRUE;

	// Initialize the bytes read to 0 in case we error early.
	*puBytesRead = 0;

	// Create a buffer pointer that can be moved forward as we read blocks...
	LPBYTE pLocalBufferPtr = pDataBuffer;

	// Read data from the raw data buffer until we've read the requested amount
	// or we have run out of data.
	long lBufferDataFilled = 0;
	bool bOutOfData = FALSE;
	while (((lBufferSize - lBufferDataFilled) > 0) && !bOutOfData)
	{
		// Do we need to start a new image block?
		if ((m_nBlockSize - m_nBlockDataRead) <= 0)
		{
			// Yep, so find out how big the current block is...
			if (m_lDataRead < m_lBufferSize)
			{
				// Get a new block size, and clear the counter of how much of it has been read.
				m_nBlockSize = m_pRawDataBuffer[m_lDataRead];
				m_nBlockDataRead = 0;
				m_lDataRead++;	// Mark the data as read.
			}
			else
			{
				// We are out of data, so mark that and drop out of the loop.
				bOutOfData = TRUE;
				continue;
			}

			// If the size of the data read was zero, then we have found the image
			// data sub-block terminator, so there is no further image data to read.
			if (m_nBlockSize == 0)
			{
				bOutOfData = TRUE;
				m_bHitImageTerminator = TRUE;
				continue;
			}
		}

		// OK, let's read a little data out of the current image data sub-block.
		BYTE nSizeToRead = (BYTE)__min( (lBufferSize - lBufferDataFilled), (long)(m_nBlockSize - m_nBlockDataRead) );

		// Don't read past the end of the file.
		nSizeToRead = (BYTE)__min((m_lBufferSize - m_lDataRead), nSizeToRead);

		// Put the data into the buffer.		
		memcpy( pLocalBufferPtr, &(m_pRawDataBuffer[m_lDataRead]), nSizeToRead );
		// Advance the pointer to past the data just read.
		pLocalBufferPtr += nSizeToRead;
		// Update the value of how much has been read for the caller.
		*puBytesRead += nSizeToRead;
		// Update the data block read amounts
		m_nBlockDataRead += nSizeToRead;
		m_lDataRead += nSizeToRead;
		lBufferDataFilled += nSizeToRead;

		// Check for an out of data condition.
		if ((m_lBufferSize - m_lDataRead) == 0)
		{
			bOutOfData = TRUE;
		}
	}

	if (*puBytesRead == 0)
	{
		// We didn't get any data, this is an error state.
		bValidRead = FALSE;
	}

	return bValidRead;
}


/*
// "Write" a string to the buffer.
*/

void LZWBufferParser::write_current_string()
{
	while (current_string.length != 0 && (m_lSkip != 0 || m_nSize != 0))
	{
	/*
 	// We have string data and we have need.
 	*/

	/* Handle any skip data. */

		if (m_lSkip != 0)
		{
		/* We need to skip some data. */
			if (m_lSkip >= current_string.length)
			{
			/* Skipped all data! */
				m_lSkip -= current_string.length;
				current_string.length = 0;
			}
			else
			{
				current_string.length -= (USHORT)m_lSkip;
				current_string.data += (USHORT)m_lSkip;
				m_lSkip = 0;
			}

			if (current_string.length == 0)
			{
				return;
			}
		}

	/* Handle any store data. */

		if (m_nSize != 0)
		{
			USHORT copy_size = __min(m_nSize, current_string.length);

		/* Copy the data. */

			memcpy(m_pBuffer, current_string.data, copy_size);

		/* Update the pointers. */

			m_nSize -= copy_size;
			m_pBuffer += copy_size;
			current_string.length -= copy_size;
			current_string.data += copy_size;
		}
	}
}

/*
// Clear the string table.
*/

void LZWBufferParser::init_lzw_table()
{
	code_width = start_bits;
	code_limit = start_limit;
	next_code_added = first_code;
}

/*
// Reset the lzw parser.
*/

void LZWBufferParser::reset()
{
/* Reset the code parser. */

	current_string.length = 0;		// No data accumulated
	remainder_width = 0;
	buffered_bytes = 0;

	init_lzw_table();

	last_code = 0;
}



/*
// Get the next LZW code.
// Code lengths vary from 9 to 11 bits.
// Thus, we can use a return code of -1 as an error condition (which we do).
*/



/*
// The asm code below was converted to C code by Roger House 96/02/22.
//
// Note that in the "#else" part of the "#if 1" below there is C code 
// which does almost the same thing as the asm code.  However the C and
// the asm are not really functionally identical, so the asm has been
// converted to C from scratch.
//
// Get the next code by shifting right.
// The shift buffer is 4 UCHARs wide and is used as follows (in rcode):
//
// |unused except for shift | incoming CHAR | resultant SHORT |
// |          3             |       2       |      1,0        |
//
*/

SHORT LZWBufferParser::get_next_rcode()
{
   SHORT	   r;

#if 1
	SHORT bits_needed = code_width;

	*(SHORT *)&shift_buffer[0] = 0;		// Init the result to 0

	// Grab bits until done

	do
	{
		// Make sure we have source

		if (remainder_width == 0)
		{
			// Must refill the current source byte

			if (buffered_bytes == 0)
			{
				if ((m_lzwDataStream.Read(byte_buffer, DATA_BUFFER_SIZE, &buffered_bytes) == FALSE) ||
					 ( buffered_bytes == 0))
				{
					last_code = -7;	// ERRORCODE_Read - defined in PMW, not framework.
					return(last_code);
				}
				current_byte = byte_buffer;
			}
			shift_buffer[2] = *current_byte++;
			buffered_bytes--;
			
//			printf("{%x}", shift_buffer[2]);
			remainder_width = 8;
		}

#ifdef _NEWPORTABLE

		SHORT	bits_grabbed;
		ULONG	ul;

		bits_grabbed = remainder_width;
		if (bits_needed < remainder_width)
			bits_grabbed = bits_needed;

		// Now bits_grabbed == min(bits_needed, remainder_width)

		ul = *((ULONG *)shift_buffer);
		ul >>= bits_grabbed;

		*((ULONG *)shift_buffer) = ul;

		remainder_width -= bits_grabbed;
		bits_needed     -= bits_grabbed;

	} while (bits_needed > 0);

	// Shift the word the rest of the way down

	USHORT us = *((USHORT *)shift_buffer);
	us >>= (16 - code_width);

	last_code = SHORT(us);
   r = last_code;

	return(r);

#else // _NEWPORTABLE

		_asm
		{
			mov	di, bits_needed
			mov	si, remainder_width
			cmp	di, si							; bits_needed > remainder_width?
			jae	lp3
			mov	si, di							; bits_grabbed = bits_needed
		lp3:

		; Now si = bits_grabbed.

			mov	ax, word ptr shift_buffer+0
			mov	dx, word ptr shift_buffer+2
			mov	cx, si

		;
		; Shift is always <= 8
		;

		; Shift is 8?

			cmp	cl, 8
			jb		lp4

			mov	al, ah
			mov	ah, dl
			mov	dl, dh

			jmp	short lp5

		; Shift less than eight.

		lp4:
			mov	dh, ah
			ror	dx, cl
			shr	ax, cl
			mov	ah, dh
		lp5:

			mov	word ptr shift_buffer+0, ax
			mov	word ptr shift_buffer+2, dx

			sub	remainder_width, si
			sub	bits_needed, si
		}
	} while (bits_needed > 0);

	_asm
	{
	; Shift the word the rest of the way down.
		mov	ax, word ptr shift_buffer+0
		mov	cx, 16
		sub	cx, code_width

		cmp	cl, 8
		jb		lp6
		mov	al, ah
		xor	ah, ah
		sub	cl, 8
		jz		lpdone

	lp6:
		shr	ax, cl

	lpdone:

		mov	r, ax
		mov	last_code, ax
	}

	return r;

#endif	// _NEWPORTABLE

#else // 1

	SHORT bits_needed, bits_grabbed;

	// We need the number of bits in the code word

	bits_needed = code_width;



	*(SHORT *)&shift_buffer[0] = 0;		// Init the result to 0

	// Grab bits until done

	do
	{
		// Make sure we have source

		if (remainder_width == 0)
		{
			// Must refill the current source byte

			if (!local_next_byte(
					(CHAR *)&shift_buffer[2]))
			{
				// Error reading
				return ERROR_CODE;
			}
//			printf("{%x}", shift_buffer[2]);
			remainder_width = 8;
		}

		// Compute how many bits to grab on this pass

		if ((bits_grabbed = bits_needed) > remainder_width)
		{
			bits_grabbed = remainder_width;
		}

		// Shift the desired number of bits into the destination

		(*(ULONG *)shift_buffer) >>= bits_grabbed;

		// Adjust the values of what is left

		remainder_width -= bits_grabbed;
		bits_needed -= bits_grabbed;

	} while (bits_needed > 0);

	// Adjust the values the rest of the way down

	*(USHORT *)&shift_buffer[0] >>= 16-code_width;

	// Return the accumulated number

//	printf("[[%x]]", *(USHORT *)shift_buffer);

	return(*(SHORT *)&shift_buffer[0]);

#endif	// else 1

} // end get_next_rcode



/*
// The asm code below was converted to C code by Roger House 96/02/22.
//
// Note that in the "#else" part of the "#if 1" below there is C code 
// which does almost the same thing as the asm code.  However the C and
// the asm are not really functionally identical, so the asm has been
// converted to C from scratch.
//
// Get the next code by shifting left.
// The shift buffer is 4 UCHARs wide and is used as follows (in lcode):
//
// |unused except for shift | resultant SHORT | incoming UCHAR |
// |          3             |     2, 1        |      0         |
//
*/

SHORT LZWBufferParser::get_next_lcode()
{
	SHORT	r;

#if 1
	SHORT bits_needed = code_width;

	*(SHORT *)&shift_buffer[1] = 0;		// Init the result to 0

	// Grab bits until done

	do
	{
		// Make sure we have source

		if (remainder_width == 0)
		{
			// Must refill the current source byte

			if (buffered_bytes == 0)
			{
				if ((m_lzwDataStream.Read(byte_buffer, DATA_BUFFER_SIZE, &buffered_bytes) == FALSE) ||
					 ( buffered_bytes == 0))
				{
					last_code = -7;
					return(last_code);
				}
				current_byte = byte_buffer;
			}
			shift_buffer[0] = *current_byte++;
			buffered_bytes--;

//			printf("{%u}", shift_buffer[0]);
			remainder_width = 8;
		}

#ifdef _NEWPORTABLE

		SHORT	bits_grabbed;
		ULONG	ul;

		bits_grabbed = remainder_width;
		if (bits_needed < remainder_width)
			bits_grabbed = bits_needed;

		// Now bits_grabbed == min(bits_needed, remainder_width)

		ul = *((ULONG *)shift_buffer);
		ul <<= bits_grabbed;

		*((ULONG *)shift_buffer) = ul;

		remainder_width -= bits_grabbed;
		bits_needed     -= bits_grabbed;

	} while (bits_needed > 0);

	// Grab the finished word

	last_code = *((SHORT *)&shift_buffer[1]);

	return(last_code);

#else

		_asm
		{
			mov	di, bits_needed
			mov	si, remainder_width
			cmp	di, si							; bits_needed > remainder_width?
			jae	lp3
			mov	si, di							; bits_grabbed = bits_needed
		lp3:

		; Now si = bits_grabbed.

			mov	ax, word ptr shift_buffer+0
			mov	dx, word ptr shift_buffer+2
			mov	cx, si

		;
		; Shift is always <= 8
		;

		; Shift is 8?

			cmp	cl, 8
			jb	lp4

			mov	dl, ah
			mov	ah, al
			xor	al, al

			jmp	short lp5

		; Shift less than eight.

		lp4:

			mov	dh, ah
			shl	ax, cl
			rol	dx, cl

		lp5:

			mov	word ptr shift_buffer+0, ax
			mov	word ptr shift_buffer+2, dx

			sub	remainder_width, si
			sub	bits_needed, si
		}
	} while (bits_needed > 0);

	_asm
	{
	; Grab the finished word.

		mov	ax, word ptr shift_buffer+1

		mov	r, ax
		mov	last_code, ax
	}
	return r;

#endif	//  _NEWPORTABLE

#else
//	SHORT bits_needed, bits_grabbed;

/* We need the number of bits in the code word. */

	bits_needed = code_width;

/* Init the result to 0. */

	*(SHORT *)&shift_buffer[1] = 0;

/* Grab bits until done. */

	do
	{
	/* Make sure we have source. */

		if (remainder_width == 0)
		{
		/* Must refill the current source byte. */
			if (!local_next_byte((CHAR *)&shift_buffer[0]))
			{
			/* Error reading. */
				return ERROR_CODE;
			}
//			printf("{%u}", shift_buffer[0]);
			remainder_width = 8;
		}

	/* Compute how many bits to grab on this pass. */

		if ((bits_grabbed = bits_needed) > remainder_width)
		{
			bits_grabbed = remainder_width;
		}

	/* Shift the desired number of bits into the destination. */

		(*(ULONG *)shift_buffer) <<= bits_grabbed;

	/* Adjust the values of what is left. */

		remainder_width -= bits_grabbed;
		bits_needed -= bits_grabbed;

	} while (bits_needed > 0);

/* Return the accumulated number. */

//	printf("{{%x}}", *(USHORT *)&shift_buffer[1]);
	return(*(SHORT *)&shift_buffer[1]);

#endif	// 1

} // end get_next_lcode



/*
// Return the string corresponding to a code.
*/

void LZWBufferParser::string_from_code(SHORT code, PSTRING * string)
{
#if 0
	_asm
	{
		
	}
#else
	while (code >= first_code)
	{
		*(--string->data) = append_character[code];
		string->length++;
		code = prefix_code[code];
	}

	*(--string->data) = (UCHAR)code;
	string->length++;
#endif
}

LZWBufferParser::LZWBufferParser(LPBYTE pInputBuffer, long lBufferSize, SHORT nBitWidth, BOOL bDir)
	: m_lzwDataStream( pInputBuffer, lBufferSize )
{
/* No data yet. */

/* Not allocated yet. */

	m_pDataBlock = NULL;

/* Validate the number of bits. */

	if ((USHORT)nBitWidth > 8)
	{
	/* Illegal! */
		return;
	}

	reading_tiff = !bDir;
//	m_pDevice = device;

/* Allocate our string block. */

	TRY
		m_pDataBlock = new BYTE[LZW_BUFFER_SIZE];
	END_TRY

/* Set our pointers. */

	byte_buffer = (BYTE far *)m_pDataBlock;
	prefix_code = (SHORT far *)(byte_buffer + DATA_BUFFER_SIZE);
	append_character = (BYTE far *)(prefix_code + TABLE_SIZE);
	decode_stack = (append_character + TABLE_SIZE);
	decode_end = ((BYTE far *)m_pDataBlock) + LZW_BUFFER_SIZE;

/* Set the codes based on the number of bits. */

	clear_code = 1 << nBitWidth;			/* for 8 bits - 256 */
	eoi_code = clear_code+1;				/* for 8 bits - 257 */
	first_code = eoi_code+1;				/* for 8 bits - 258 */

	start_bits = nBitWidth+1;				/* for 8 bits - 9   */
	start_limit = clear_code << 1;		/* for 8 bits - 512 */

#if 0
	printf("Decode %d bits (first:%d, clear:%d, eoi:%d)\n",
						nBitWidth, first_code, clear_code, eoi_code);
#endif

	m_bGIFTranslationDirection = (bDir != FALSE);
//	get_next_code = bDir ? get_next_rcode : get_next_lcode;

/* Reset the code parser. */

	reset();
}

/*
// Destructor for the LZW parser.
*/

LZWBufferParser::~LZWBufferParser()
{
	delete [] m_pDataBlock;
}

/*
// Decompress an LZW strip using the next_byte function passed.
// Place the data into the buffer passed.
*/

int LZWBufferParser::read(ULONG skip_size, LPBYTE read_buffer, USHORT read_size)
{
	SHORT code;

/* Store here. */

	m_lSkip = skip_size;
	m_pBuffer = read_buffer;
	m_nSize = read_size;

/* Handle any previous left-over string. */

	if (current_string.length != 0)
	{
		write_current_string();
	}

/* See if we already hit the end. */

	if (last_code == eoi_code)
	{
		return -7;			/* No more data! */
	}
	else if (last_code < 0)
	{
		return last_code;
	}

//	lzw_device = m_pDevice;

	while (m_nSize != 0 || m_lSkip != 0)
	{
		if ((code = (m_bGIFTranslationDirection) ? get_next_rcode() : get_next_lcode()) == eoi_code)
		{
		/* Couldn't satisfy the request. */
			last_code = -7;
			return last_code;
		}

//		od("(%d, %d, %d)\r\n", code, next_code_added, character);

	/* Check for errors. */

		if (code < 0)
		{
		/* We got an error reading. Return the bad news. */
			return code;
		}

	/* Check for the clear code. */

		if (code == clear_code)
		{
		/* Reinit the string table. */

//			printf("(CLEAR)");
			init_lzw_table();

		/* Get the next (first) code. */

			if ((code = (m_bGIFTranslationDirection) ? get_next_rcode() : get_next_lcode()) == eoi_code)
			{
			/* Couldn't satisfy the request. */
				last_code = -7;
				return last_code;
			}

//			od("<%d, %d, %d>\r\n", code, next_code_added, character);

		/* Integrity check. */

			if (code >= next_code_added)
			{
			/* LZW file is bad. */
				return -11;	// Defined in PMW not in framework.
			}

		/* Check for errors. */

			if (code < 0)
			{
			/* Error time. */
				return code;
			}

		/* Output the string for this code. */

			current_string.data = decode_end;
			current_string.length = 0;

			string_from_code(code, &current_string);

			write_current_string();

			character = (UCHAR)code;
		}
		else
		{
			current_string.data = decode_end;
			current_string.length = 0;

			if (code >= next_code_added)
			{
			/* We can back our pointer up because strings are built from the end. */

				*--current_string.data = character;
				current_string.length++;

			/* Integrity check. */

				if (old_code >= next_code_added)
				{
				/* LZW file is bad. */
					return -11;
				}
				string_from_code(old_code, &current_string);
			}
			else
			{
				string_from_code(code, &current_string);
			}

		/* The append character is the last one in the string. */

			character = *current_string.data;

		/* Write the string. */

			write_current_string();

		/* Add the string to the table. */

		/* Inline code for add_string_to_table() */
			prefix_code[next_code_added] = old_code;
			append_character[next_code_added] = character;

//			printf("add string [%d] (%d, %d)\n", next_code_added, old_code, character);

		/*
		// Bump the code value.
		// Bump the code width if we reach the code limit.
		*/

			next_code_added++;

			if ((!reading_tiff && next_code_added == code_limit
					&& code_limit != 4096)
					|| (reading_tiff && next_code_added == code_limit-1))
			{
//				printf("++");
				code_width++;
				code_limit <<= 1;
			}
		}
		old_code = code;
	}

	return 0;
}
