/*
// Heap management routines.
// This file defines the methods of the StandardHeap class.
//
// Oct. 26, 1992 - Jay N. : Initial creation
*/

#include "stdafx.h"

//#include <stdio.h>
//#include <stdlib.h>
#include <malloc.h>						// for _heapmin()
//#include <string.h>

#define DEBUG_HEAPx		/* Don't use this anymore! _msize() is not valid! */

#ifdef DEBUG_HEAP
HEAP_ELEMENT_SIZE the_magic_size = 0x290;
static DWORD dwAllocated = 0L;
#endif

/*
// Duplicate a string.
*/

LPSTR Heap::dup_string(LPCSTR string)
{
	LPSTR new_string;
	SHORT len;

/* Like to like... */

	if (string == NULL)
	{
		last_error = ERRORCODE_None;			/* This is OK! */
		return NULL;
	}

/* Get the length. */

	len = strlen(string);

	if ((new_string = (LPSTR)allocate(len+1)) != NULL)
	{
		strcpy(new_string, string);
	}
	return new_string;
}

/*
// Allocate some memory.
*/

LPVOID StandardHeap::allocate(HEAP_ELEMENT_SIZE size)
{
	LPVOID memory;
	SHORT current_purge = 0;
	ERRORCODE error;

/* See if we're busy. */

	if (busy())
	{
		last_error = ERRORCODE_Busy;
		last_error_size = size;

		return NULL;
	}

/* Try to allocate memory. */

	for (;;)
	{
	/* Try to allocate the memory. */

		if ((memory = new BYTE[size]) != NULL)
		{
#ifdef DEBUG_HEAP
		/* Finish up the allocation. */

			size = ::_msize(memory);

			dwAllocated += size;
			od("Allocated %x bytes (%lx)\r\n", size, dwAllocated);
#endif
			break;
		}

	/* We're now busy. */

		purging = TRUE;

	/* Call the next purge function. */

		error = ERRORCODE_Memory;

	/* Call the purge functions while the purge functions can't purge. */

		while (current_purge < purge_function_count &&
					(error = purge[current_purge].function(purge[current_purge].data)) == ERRORCODE_Memory)
		{
		/* This one failed. Move to the next. */
			current_purge++;
		}

	/* We're not busy. */

		purging = FALSE;

	/* Check our success. */

		if (error != ERRORCODE_None)
		{
			last_error = error;
			last_error_size = size;
			break;
		}

	/* Some purge function succeeded. Try the allocation again. */
	}

//	od("alloc: %u got %lx\r\n", size, memory);
	return memory;
}

/*
// Reallocate some memory.
*/

LPVOID StandardHeap::reallocate(LPVOID memory, HEAP_ELEMENT_SIZE new_size, HEAP_ELEMENT_SIZE old_size)
{
	LPVOID new_mem;

#ifdef DEBUG_HEAP
	HEAP_ELEMENT_SIZE debug_old_size;
	if (memory == NULL)
	{
		debug_old_size = 0L;
	}
	else
	{
		debug_old_size = ::_msize(memory);
	}
#endif

//	if ((new_mem = ::realloc(memory, new_size)) == NULL)
	{
//		HEAP_ELEMENT_SIZE old_size = ::_msize(memory);

	/* Couldn't reallocate in place. Try to reallocate by moving. */

		if ((new_mem = allocate(new_size)) != NULL)
		{
		/* Success! Copy the data over. */

			if (memory != NULL)
			{
				memcpy(new_mem, memory, __min(new_size, old_size));
	
			/* Free the old memory since we succeeded. */
	
				free(memory);
			}
		}
	}
	
#ifdef DEBUG_HEAP
	HEAP_ELEMENT_SIZE debug_new_size = ::_msize(new_mem);
/* Do NOT combine these next two lines! SIGNED/UNSIGNED problems! */
	dwAllocated -= debug_old_size;
	dwAllocated += debug_new_size;
	od("Realloc'ed %x to %x bytes (%lx)\r\n",
			debug_old_size, debug_new_size, dwAllocated);
#endif

	freed = TRUE;
//	od("realloc %u got %lx\r\n", new_size, new_mem);
	return new_mem;
}

/*
// Free some memory.
*/

VOID StandardHeap::free(LPVOID memory)
{
#ifdef DEBUG_HEAP
	if (memory != NULL)
	{
		HEAP_ELEMENT_SIZE size = ::_msize(memory);
		dwAllocated -= size;
		od("free: %x @ %08lx (%lx)\r\n", size, memory, dwAllocated);
	}
#endif
//	::free(memory);
	delete [] (LPBYTE)memory;
	freed = TRUE;
}

/*
// Return the last memory error encountered.
// This is not valid if the last memory call was successful.
*/

ERRORCODE StandardHeap::last_memory_error(HEAP_ELEMENT_SIZE far *size)
{
	if (size != NULL)
	{
		*size = last_error_size;
	}
	return last_error;
}

/*
// Register a purge function.
*/

ERRORCODE StandardHeap::register_purge_function(PURGE_FUNCTION function, LPVOID data)
{
/* See if we have room. */

	if (purge_function_count == MAX_PURGE_FUNCTIONS)
	{
		return ERRORCODE_Full;
	}

/* Do the register. */

	purge[purge_function_count].function = function;
	purge[purge_function_count].data = data;

	return ERRORCODE_None;
}

/*
// compact()
//
// Do any compaction necessary.
*/

VOID StandardHeap::compact(VOID)
{
	if (freed)
	{
		freed = FALSE;
		_heapmin();
	}
}
