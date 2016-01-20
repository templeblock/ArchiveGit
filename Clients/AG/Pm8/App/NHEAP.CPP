/*#define DEBUG_HEAP*/

/*
// NHEAP.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Routines to manage a small ( < 64K) local heap.
//
*/

#include "stdafx.h"

//#define DEBUG_HEAP

#ifdef DEBUG_HEAP
#define DASSERTVALID(h)	ASSERT(heap_valid(h))
#else
#define DASSERTVALID(h)
#endif

#include "nheap.h"

#define ALLOCATED		0x8000

typedef struct
{
	uint16 total_words;
	uint16 words_available;
} tHeapHeader;

#define HEADER_WORDS		(sizeof (tHeapHeader) / sizeof (uint16))

int heap_init(void FAR *heap, uint16 size)
{
	uint16 FAR *wp;
	tHeapHeader FAR *header;

	header = (tHeapHeader FAR *)heap;
	wp = (uint16 FAR *)&header[1];

	size /= 2;
	if (size > 0x7ff7)
	{
		size = 0x7ff7;
	}

	header->total_words = size - HEADER_WORDS;
	header->words_available = header->total_words - 1;
	*wp = header->words_available;
	DASSERTVALID(heap);
	return 0;
}

void FAR *heap_alloc(void FAR *heap, uint16 size)
{
	uint16 FAR *wp, FAR *endp;
	tHeapHeader FAR *header;
	DASSERTVALID(heap);

	if (heap == (void FAR *)0 || size == 0)
	{
		return (void FAR *)0;
	}

	header = (tHeapHeader FAR *)heap;
	wp = (uint16 FAR *)heap;
	endp = wp + *wp + HEADER_WORDS - 1;

	size = (size + 1) / 2;

	wp += HEADER_WORDS;

	while (wp < endp)
	{
		if (!(*wp & ALLOCATED))
		{
			uint16 FAR *wp2;
			uint16 size_found;

			size_found = *wp;
			wp2 = wp + *wp + 1;
			while (wp2 <= endp && !(*wp2 & ALLOCATED))
			{
				/* Join two blocks */
				uint16 len = *wp2 + 1;
				size_found += len;
				wp2 += len;
			}

			*wp = size_found;
			DASSERTVALID(heap);
			if (size_found >= size)
			{
				*wp = ALLOCATED | size;
				header->words_available -= size;
					
				if (wp > (uint16 FAR *)(header + 1))
				{
					--header->words_available;
				}

				if (size != size_found)
				{
					/* Break up a block */
					*(wp + size + 1) = size_found - size - 1;
				}
				DASSERTVALID(heap);
				return ((void FAR *)(wp + 1));
			}
		}
		wp += (*wp & ~ALLOCATED) + 1;
	}
	DASSERTVALID(heap);
	return (void FAR *)0;
}

void heap_free(void FAR *heap, void FAR *block)
{
	uint16 FAR *wp;
	tHeapHeader FAR *header;
	DASSERTVALID(heap);

	header = (tHeapHeader FAR *)heap;
	wp = ((uint16 FAR *)block - 1);
	ASSERT((*wp & ALLOCATED) != 0);
	*wp &= ~ALLOCATED;
	if (wp > (uint16 FAR *)(header + 1))
	{
		header->words_available++;
	}
	header->words_available += *wp;
	DASSERTVALID(heap);
}


uint16 heap_available(void FAR *heap)
{
	return ((*(tHeapHeader FAR *)heap).words_available * 2);
}


BOOL heap_valid(void FAR *heap)
{
	uint16 FAR *wp, FAR *endp;
	tHeapHeader FAR *header;
	uint16 words_allocated;
 	uint16 blocks_allocated;

	header = (tHeapHeader FAR *)heap;
	wp = (uint16 FAR *)heap + HEADER_WORDS;
	endp = wp + header->total_words;

	if (header->words_available >= header->total_words)
	{
		return (FALSE);
	}

	words_allocated = 0;
	blocks_allocated = 0;

	while (wp < endp)
	{
		uint16 words;

		words = *wp & ~ALLOCATED;

		if ((*wp & ALLOCATED) != 0)
		{
			if (wp > (uint16 FAR *)(header + 1))
				++blocks_allocated;
			words_allocated += words;
		}
		wp += words + 1;
	}
	if (wp != endp)
	{
		return (FALSE);
	}
	if (words_allocated + 1 + blocks_allocated + header->words_available != header->total_words)
	{
		return FALSE;
	}
	return TRUE;
}
