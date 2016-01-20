#ifndef _ARRAY_H
#define _ARRAY_H

/*
// An array class.
//
// Oct. 26, 1992 - Jay N. : Initial definitions.
*/

#include "storeman.h"					/* We need storage devices */

FARCLASS Array
{
	friend class ArrayRecord; 
	friend class SplitArray;
protected:
	SHORT		first;						/* Index of first element. */
	SHORT		element_size;				/* The size of an element. */
	SHORT		element_count;				/* The number of elements. */
	int		m_nBlockCount;				/* Size of data block (elements). */
	VOIDPTR	data;							/* The elements themselves. */

/*
// set_array_size()
//
// Set the array size to the specified count.
// After this call, element_count will be the passed count.
*/

	ERRORCODE	set_array_size(SHORT new_count);

/*
// Return the size in bytes of the array.
*/

	USHORT array_byte_size(VOID)
		{ return element_size*element_count; }

public:

/*
// The constructor for the array.
*/

	Array(SHORT elem_size, SHORT first_element = 0)
		:	first(first_element),
			element_size(elem_size),
			m_nBlockCount(0),
			element_count(0),
			data(NULL)
		{ }

/*
// The assignment operator.
*/

	ERRORCODE assign(const Array &array);
	Array& operator = (const Array &array)
		{ assign(array); return *this; }

/*
// Copy constructor.
*/

	Array(const Array far &array)
		:	m_nBlockCount(0),
			element_count(0),
			data(NULL)
		{ *this = array; }

/*
// The destructor for the array.
*/

	~Array();

/*
// Return the number of elements in the array.
*/

	SHORT count(VOID) { return element_count; }

/*
// insert_element()
//
//	Insert an element at a position in the array.
*/

	ERRORCODE insert_element(VOIDPTR element, SHORT where);

/*
// add_element()
//
// Add an element to the end of the array.
*/

	ERRORCODE add_element(VOIDPTR element) { return insert_element(element, element_count); }

/*
// delete_element()
//
// Delete an element in the array.
*/

	ERRORCODE delete_element(SHORT which);

/*
// get_element()
//
// Return a pointer to the n'th element.
// If NULL is returned, the index was out of range.
// This pointer should be considered safe to increment to get further objects.
// This may not be good programming practice, but it will facilitate speed.
*/

	VOIDPTR get_element(SHORT which);

/*
// empty_array()
//
// Make the array be empty.
*/

	VOID empty_array();

/*
// read()
//
// Read an array from a StorageDevice.
*/

	ERRORCODE read(StorageDevicePtr device);

/*
// write()
//
// Write an array to a StorageDevice.
*/

	ERRORCODE write(StorageDevicePtr device);

/*
// size()
//
// Size an array for a StorageDevice.
*/

	ST_MAN_SIZE size(StorageDevicePtr device);

};

#endif					/* #ifndef _ARRAY_H */

