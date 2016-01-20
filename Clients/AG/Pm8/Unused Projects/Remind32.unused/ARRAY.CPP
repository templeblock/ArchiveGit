/*
// Array handling routines.
//
// Oct. 26, 1992 - Jay N. : Initial creation.
*/

#include "stdafx.h"

#include <string.h>

/*
// Set the size of an array.
// An error generally will not be returned in the event of contraction.
*/

ERRORCODE Array::set_array_size(SHORT new_count)
{
	VOIDPTR new_data;

/* See if there's anything to do. */

	if (new_count == element_count)
	{
		return ERRORCODE_None;
	}

/*
// See if it's time to free the data.
*/

	if (new_count == 0)
	{
	/* No more data. Free the data area. */
		system_heap->free(data);
		data = NULL;
	}
	else
	{
	/*
	// Try to reallocate the array data.
	// If "data" is NULL to begin with, this should allocate memory.
	*/
		/* Only allocate if getting larger. */
		if (new_count > m_nBlockCount || data == NULL)
		{
			if ((new_data = system_heap->reallocate(data, element_size*new_count, element_size*m_nBlockCount)) == NULL)
			{
				return system_heap->last_memory_error();
			}
			else
			{
				m_nBlockCount = new_count;

				/* Plug in the new data. */
				data = new_data;
			}
		}
	}

/* Set the new count. */

	element_count = new_count;

	return ERRORCODE_None;
}

/*
// The destructor for an array.
*/

Array::~Array()
{
	empty_array();
}

/*
// Insert an element in an array at a position.
// Valid positions are 0 to element_count. The element is placed at the
// specified place in the array, adjusting all elements at or following the
// place.
// Positions larger than element_count map or less than 0 result in an error.
*/

ERRORCODE Array::insert_element(VOIDPTR element, SHORT where)
{
	SHORT old_count = element_count;
	ERRORCODE error;

/* Normalize. */

	where -= first;

/* Validate where. */

	if (where < 0 || where > element_count)
	{
		return ERRORCODE_BadParameter;
	}

/*
// Set the new array size.
// After this call, "element_count" will be set to the new size.
// Watch out!
*/

	if ((error = set_array_size(element_count+1)) == ERRORCODE_None)
	{
	/* Move the data if need be. */

		LPSTR new_slot = ((LPSTR)data) + where*element_size,
				dst_slot = new_slot + element_size;

		if (where != old_count)
		{
		/* Must move some data. Use the function which handles overlap. */
			memmove(dst_slot, new_slot, (old_count - where)*element_size);
		}

	/* Insert the new element. */

		memcpy(new_slot, element, element_size);
	}
	return error;
}

/*
// Delete an element in the array.
// The position passed must be in the range of 0 to element_count-1.
// Any value outside this range generates an error.
*/

ERRORCODE Array::delete_element(SHORT which)
{
	SHORT new_count;

/* Normalize. */

	which -= first;

/* Validate the parameter. */

	if (which < 0 || which >= element_count)
	{
		return ERRORCODE_BadParameter;
	}

/* Move data down if we need to. */

	new_count = element_count-1;

	if (new_count != which)
	{
		LPSTR dst = ((LPSTR)data) + which*element_size,
				src = dst + element_size;

	/* "which" is not the last one. */

		memcpy(dst, src, (new_count-which)*element_size);
	}

/* Resize the array to take up less space. */

	return set_array_size(new_count);
}

/*
// Return the n'th element.
*/

VOIDPTR Array::get_element(SHORT which)
{
/* Normalize. */

	which -= first;

/* Validate the argument. */

	if (which < 0 || which >= element_count)
	{
		return NULL;
	}

/* Return the element. */

	return ((LPSTR)data) + element_size*which;
}

/*
// Make the array be empty.
*/

VOID Array::empty_array()
{
	set_array_size(0);
}

/*
// Read an array from a StorageDevice.
*/

ERRORCODE Array::read(StorageDevicePtr device)
{
	ERRORCODE error;

/* Free the data if there is any. */

	if (data != NULL)
	{
		system_heap->free(data);
		data = NULL;
	}

/* Read the element count. */

	if ((error = device->read(&element_count, sizeof(element_count))) != ERRORCODE_None)
	{
		element_count = 0;
		return error;
	}

/* Read the block. */
	
	if (element_count != 0)
	{
		if ((error = device->read_block(array_byte_size(), &data)) != ERRORCODE_None)
		{
			element_count = 0;
		}
	}

	m_nBlockCount = element_count;

	return error;
}

/*
// Write an array to a StorageDevice.
*/

ERRORCODE Array::write(StorageDevicePtr device)
{
	ERRORCODE error;

/* Write the element count. */

	if ((error = device->write(&element_count, sizeof(element_count))) != ERRORCODE_None)
	{
		return error;
	}

/* Write the data. */

	if (element_count != 0)
	{
		error = device->write(data, array_byte_size());
	}

	return error;
}

/*
// Size an array for a StorageDevice.
*/

ST_MAN_SIZE Array::size(StorageDevicePtr device)
{
	return sizeof(element_count) + array_byte_size();
}

/*
// Assignment operator.
// This handles duplicating the memory block.
*/

ERRORCODE Array::assign(const Array far &array)
{
	ERRORCODE error;

/* Free any data we have. */

	set_array_size(0);

/* Copy the static portion. */

	first = array.first;
	element_size = array.element_size;

/* Allocate the new data. */

	if ((error = set_array_size(array.element_count)) == ERRORCODE_None
			&& data != NULL
 			&& array.data != NULL)
	{
	/* Copy the data. */
		memcpy(data, array.data, array_byte_size());
	}
	return error;
}
