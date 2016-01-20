/*
// Array handling routines.
//
// Oct. 26, 1992 - Jay N. : Initial creation.
*/

#include "stdafx.h"
#include "array.h"
#include "dbstore.h"

//#include <string.h>

void Array::free_data(void)
{
	delete [] (BYTE*) data;
	data = NULL;
}

/*
// Set the size of an array.
// An error generally will not be returned in the event of contraction.
*/

ERRORCODE Array::set_array_size(SHORT new_count)
{
	int nElementSize = ElementSize();

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
		free_data();
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
			// Allocate new data.
			LPVOID new_data = NULL;
			TRY
				new_data = (LPVOID)(new BYTE [nElementSize*new_count]);
			END_TRY

			if (new_data == NULL)
			{
				return ERRORCODE_Memory;
			}

			// If we have old data, copy it over now.
			if (data != NULL)
			{
				// Copy over the data from the old block.
				int nCount = __min(new_count, m_nBlockCount);
				memcpy(new_data, data, nCount*nElementSize);
				free_data();
			}
			// Plug in the new data.
			data = new_data;
			m_nBlockCount = new_count;
		}
	}

	// Set the new count.
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

ERRORCODE Array::insert_element(LPVOID element, SHORT where)
{
	SHORT old_count = element_count;
	int nElementSize = ElementSize();		// promote to int.
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

		LPSTR new_slot = ((LPSTR)data) + where*nElementSize,
				dst_slot = new_slot + nElementSize;

		if (where != old_count)
		{
		/* Must move some data. Use the function which handles overlap. */
			memmove(dst_slot, new_slot, (old_count - where)*nElementSize);
		}

	/* Insert the new element. */

		memcpy(new_slot, element, nElementSize);
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
	int nElementSize = ElementSize();		// promote to int.

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
		LPSTR dst = ((LPSTR)data) + which*nElementSize,
				src = dst + nElementSize;

	/* "which" is not the last one. */

		memcpy(dst, src, (new_count-which)*nElementSize);
	}

/* Resize the array to take up less space. */

	return set_array_size(new_count);
}

/*
// Return the n'th element.
*/

LPVOID Array::get_element(SHORT which)
{
/* Normalize. */

	which -= first;

/* Validate the argument. */

	if (which < 0 || which >= element_count)
	{
		return NULL;
	}

/* Return the element. */

	return ((LPSTR)data) + ElementSize()*which;
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
		free_data();
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

USHORT Array::size(StorageDevicePtr device)
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
