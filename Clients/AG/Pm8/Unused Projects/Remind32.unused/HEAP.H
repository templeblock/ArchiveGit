#ifndef _HEAP_H
#define _HEAP_H

/*
// Classes for memory heap management.
//
// Oct. 26, 1992 - Jay N. : Initial definitions
*/

#include "error.h"

/*
// The size of a heap element.
*/

typedef USHORT HEAP_ELEMENT_SIZE;

/*
// A function for reclaiming memory in the heap when space is insufficient.
// This function is called repeatedly by the heap until either the memory
// allocation succeeds or the purge functions run out of purgable memory.
//
// The data passed to the purge function is the data passed when the function
// is registered.
//
// The purge function should not attempt to allocate any memory (to prevent
// infinite recursion). When the purge function is unable to purge any
// more memory, it should return ERRORCODE_Memory.
*/

typedef ERRORCODE (*PURGE_FUNCTION)(LPVOID data);

/*
// A heap for memory allocation.
// This class merely defines an interface for heap managers.
// No code is defined for this class.
*/

FARCLASS Heap
{
protected:
/* Variables for handling the last error. */

	ERRORCODE 			last_error;
	HEAP_ELEMENT_SIZE	last_error_size;

public:

/*
// Standard constructor.
*/

	Heap() : last_error(ERRORCODE_None), last_error_size(0) {}

/*
//	The destructor for a heap.
// This is virtual so it can be overridden by derived classes.
*/

	virtual ~Heap() { }

/*
// allocate()
//
// Allocate some memory in the heap.
*/

	virtual LPVOID allocate(HEAP_ELEMENT_SIZE size) = 0;

/*
// reallocate()
//
// Reallocate some memory (change its size).
// The memory may change location. If NULL is returned, the original pointer
// passed is NOT freed.
*/

	virtual LPVOID reallocate(LPVOID memory, HEAP_ELEMENT_SIZE new_size, HEAP_ELEMENT_SIZE old_size) = 0;

/*
// free()
//
// Free some memory allocated in the heap.
*/

	virtual VOID free(LPVOID memory) = 0;

/*
// last_memory_error()
//
// Return the last memory error. This is only valid if the last call to
// a memory routine resulted in a NULL pointer being returned.
// If "last_requested_size" is not NULL, the size of the last memort request
// is also returned.
*/

	virtual ERRORCODE last_memory_error(HEAP_ELEMENT_SIZE far *last_requested_size = NULL) = 0;

/*
// register_purge_function()
//
// Register a memory reclamation function.
// The data passed at this point is passed to the function when it is invoked
// later.
*/

	virtual ERRORCODE register_purge_function(PURGE_FUNCTION function, LPVOID data) = 0;

/*
// busy()
//
// Return whether the heap is busy or not.
// This basically prevents recursion.
*/

	virtual BOOL busy() = 0;

/*
// dup_string()
//
// Duplicate a string.
*/

	virtual LPSTR dup_string(LPCSTR string);

/*
// free_string()
//
// Frees a (possibly const) string.
// Note that this will indeed modify the string (since it's being freed!).
*/

	void free_string(LPCSTR s)
		{ free((LPVOID)s); }

/*
// compact()
//
// Do any compaction necessary.
*/

	virtual VOID compact(VOID) = 0;

};

typedef Heap far *HeapPtr;

/*
// The standard derived heap class.
*/

#define MAX_PURGE_FUNCTIONS 20

class StandardHeap : public Heap
{
private:
	BOOL freed;

/* Variables for handling the purging of memory. */

	SHORT					purge_function_count;
	struct STANDARD_HEAP_PURGE
	{
		PURGE_FUNCTION		function;
		LPVOID				data;
	} purge[MAX_PURGE_FUNCTIONS];

/* Are we allocating? */

	BOOL purging;

public:

/*
// The constructor for the heap.
*/

	StandardHeap() : Heap(), freed(FALSE), purge_function_count(0), purging(FALSE) { }

/*
// allocate()
//
// Allocate some memory.
*/

	LPVOID allocate(HEAP_ELEMENT_SIZE size);

/*
// reallocate()
//
// Reallocate some memory.
*/

	LPVOID reallocate(LPVOID memory, HEAP_ELEMENT_SIZE size, HEAP_ELEMENT_SIZE old_size);

/*
// free()
//
// Free some memory.
*/

	VOID free(LPVOID memory);

/*
// last_memory_error()
//
// Return the last memory error.
*/

	ERRORCODE last_memory_error(HEAP_ELEMENT_SIZE far *last_requested_size = NULL);

/*
// register_purge_function()
//
// Register a memory reclamation function.
// The data passed at this point is passed to the function when it is invoked
// later.
*/

	ERRORCODE register_purge_function(PURGE_FUNCTION function, LPVOID data);

/*
// busy()
//
// Return whether we're busy or not.
// This basically prevents recursion.
*/

	BOOL busy() { return purging; }

/*
// compact()
//
// Do any compaction necessary.
*/

	virtual VOID compact(VOID);

};

#endif						/* #ifndef _HEAP_H */
