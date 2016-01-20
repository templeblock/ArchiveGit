/*
//	LZW.CPP
//
//		Copyright (c) 1993 MicroLogic Software, Inc.
//
// LZW decompression routines.
//
// $Header: /PM8/App/LZW.CPP 1     3/03/99 6:07p Gbeddow $
//
// $Log: /PM8/App/LZW.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:22:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:06   Fred
// Initial revision.
// 
//    Rev 1.1   19 Jul 1996 18:01:16   Fred
//  
// 
//    Rev 1.0   14 Mar 1996 13:43:28   Jay
// Initial revision.
// 
//    Rev 1.1   27 Feb 1996 17:06:06   VSI
// _NEWPORTABLE 'C' code replaces .asm -- not yet working
// 
*/

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "lzw.h"

/*
// A "Pascal" string. We must use this because we can have zeros in the string.
*/

typedef struct
{
	USHORT		length;
	UCHAR			far *data;
} PSTRING;

/* The buffer area. */

#define	DATA_BUFFER_SIZE 128
#define	LZW_BUFFER_SIZE 32767

#define	TABLE_SIZE			4096

/* Our string table. And which code will be added next. */

PRIVATE	SHORT next_code_added;

PRIVATE SHORT code_width;
PRIVATE SHORT code_limit;

PRIVATE SHORT clear_code;
PRIVATE SHORT eoi_code;
PRIVATE SHORT first_code;
PRIVATE SHORT start_bits;
PRIVATE SHORT start_limit;

PRIVATE SHORT far *prefix_code;
PRIVATE BYTE far *append_character;
PRIVATE BYTE far *decode_stack;
PRIVATE BYTE far *decode_end;
PRIVATE BYTE far *byte_buffer;

PRIVATE BYTE far *current_byte;
PRIVATE USHORT buffered_bytes;

PRIVATE StorageDevicePtr lzw_device;

PRIVATE BOOL reading_tiff;

PRIVATE SHORT (near *get_next_code)(VOID);

PRIVATE SHORT last_code;
PRIVATE SHORT old_code;
PRIVATE UCHAR character;

PRIVATE SHORT near remainder_width;
PRIVATE UCHAR near shift_buffer[sizeof(LONG)];

PRIVATE PSTRING near current_string;

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

/*
// "Write" a string to the buffer.
*/

VOID near LZWParser::write_current_string(VOID)
{
	while (current_string.length != 0 && (skip != 0 || size != 0))
	{
	/*
 	// We have string data and we have need.
 	*/

	/* Handle any skip data. */

		if (skip != 0)
		{
		/* We need to skip some data. */
			if (skip >= current_string.length)
			{
			/* Skipped all data! */
				skip -= current_string.length;
				current_string.length = 0;
			}
			else
			{
				current_string.length -= (USHORT)skip;
				current_string.data += (USHORT)skip;
				skip = 0;
			}

			if (current_string.length == 0)
			{
				return;
			}
		}

	/* Handle any store data. */

		if (size != 0)
		{
			USHORT copy_size = __min(size, current_string.length);

		/* Copy the data. */

			memcpy(buffer, current_string.data, copy_size);

		/* Update the pointers. */

			size -= copy_size;
			buffer += copy_size;
			current_string.length -= copy_size;
			current_string.data += copy_size;
		}
	}
}

/*
// Clear the string table.
*/

PRIVATE VOID near
init_lzw_table(VOID)
{
	code_width = start_bits;
	code_limit = start_limit;
	next_code_added = first_code;
}

/*
// Reset the lzw parser.
*/

VOID LZWParser::reset(VOID)
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

PRIVATE SHORT near
get_next_rcode(VOID)
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
			    ERRORCODE error;

			    if ((error = lzw_device->read(byte_buffer, 
				    DATA_BUFFER_SIZE, &buffered_bytes)) !=
				    ERRORCODE_None || ((error = 
				    ERRORCODE_Read), buffered_bytes == 0))
			    {
				last_code = error;
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
					(CHAR _near *)&shift_buffer[2]))
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

PRIVATE SHORT near
get_next_lcode(VOID)
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
			    ERRORCODE error;

			    if ((error = lzw_device->read(byte_buffer, 
				    DATA_BUFFER_SIZE, &buffered_bytes)) != 
				    ERRORCODE_None || ((error = 
				    ERRORCODE_Read), buffered_bytes == 0))
			    {
				last_code = error;
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
			if (!local_next_byte((CHAR _near *)&shift_buffer[0]))
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

PRIVATE VOID near
string_from_code(SHORT code, PSTRING near * string)
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

LZWParser::LZWParser(StorageDevicePtr device, SHORT bit_width, BOOL dir)
{
/* No data yet. */

/* Not allocated yet. */

	data_block = NULL;

/* Validate the number of bits. */

	if ((USHORT)bit_width > 8)
	{
	/* Illegal! */
		return;
	}

	reading_tiff = !dir;
	m_pDevice = device;

/* Allocate our string block. */

	TRY
		data_block = new BYTE[LZW_BUFFER_SIZE];
	END_TRY

/* Set our pointers. */

	byte_buffer = (BYTE far *)data_block;
	prefix_code = (SHORT far *)(byte_buffer + DATA_BUFFER_SIZE);
	append_character = (BYTE far *)(prefix_code + TABLE_SIZE);
	decode_stack = (append_character + TABLE_SIZE);
	decode_end = ((BYTE far *)data_block) + LZW_BUFFER_SIZE;

/* Set the codes based on the number of bits. */

	clear_code = 1 << bit_width;			/* for 8 bits - 256 */
	eoi_code = clear_code+1;				/* for 8 bits - 257 */
	first_code = eoi_code+1;				/* for 8 bits - 258 */

	start_bits = bit_width+1;				/* for 8 bits - 9   */
	start_limit = clear_code << 1;		/* for 8 bits - 512 */

#if 0
	printf("Decode %d bits (first:%d, clear:%d, eoi:%d)\n",
						bit_width, first_code, clear_code, eoi_code);
#endif

	get_next_code = dir ? get_next_rcode : get_next_lcode;

/* Reset the code parser. */

	reset();
}

/*
// Destructor for the LZW parser.
*/

LZWParser::~LZWParser()
{
	delete [] data_block;
}

/*
// Decompress an LZW strip using the next_byte function passed.
// Place the data into the buffer passed.
*/

ERRORCODE LZWParser::read(ULONG skip_size, LPBYTE read_buffer, USHORT read_size)
{
	SHORT code;

/* Store here. */

	skip = skip_size;
	buffer = read_buffer;
	size = read_size;

/* Handle any previous left-over string. */

	if (current_string.length != 0)
	{
		write_current_string();
	}

/* See if we already hit the end. */

	if (last_code == eoi_code)
	{
		return ERRORCODE_Read;			/* No more data! */
	}
	else if (last_code < 0)
	{
		return (ERRORCODE)last_code;
	}

	lzw_device = m_pDevice;

	while (size != 0 || skip != 0)
	{
		if ((code = get_next_code()) == eoi_code)
		{
		/* Couldn't satisfy the request. */
			last_code = ERRORCODE_Read;
			return ERRORCODE_Read;
		}

//		od("(%d, %d, %d)\r\n", code, next_code_added, character);

	/* Check for errors. */

		if (code < 0)
		{
		/* We got an error reading. Return the bad news. */
			return (ERRORCODE)code;
		}

	/* Check for the clear code. */

		if (code == clear_code)
		{
		/* Reinit the string table. */

//			printf("(CLEAR)");
			init_lzw_table();

		/* Get the next (first) code. */

			if ((code = get_next_code()) == eoi_code)
			{
			/* Couldn't satisfy the request. */
				last_code = ERRORCODE_Read;
				return ERRORCODE_Read;
			}

//			od("<%d, %d, %d>\r\n", code, next_code_added, character);

		/* Integrity check. */

			if (code >= next_code_added)
			{
			/* LZW file is bad. */
				return ERRORCODE_InvalidRecord;
			}

		/* Check for errors. */

			if (code < 0)
			{
			/* Error time. */
				return (ERRORCODE)code;
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
					return ERRORCODE_InvalidRecord;
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

	return ERRORCODE_None;
}
