/*
//
// List handling routines.
//
// Oct. 22, 1992 - Jay N. : Initial creation.
//
*/

#include "stdafx.h"

/****************/
/* List methods */
/****************/

/*
// Return the first element that satisfies a certain criterion.
*/

ListElementPtr List::first_that(SEARCH_CRITERION criterion, VOIDPTR data, ListElementPtr start, ListElementPtr end)
{
	if (start == NULL)
	{
		start = first;
	}

	while (start != NULL && !criterion(start, data))
	{
		if (start == end)
		{
			return NULL;
		}
		start = start->next;
	}
	return start;
}

/*
// Return the last element that satisfies a certain criterion.
*/

ListElementPtr List::last_that(SEARCH_CRITERION criterion, VOIDPTR data, ListElementPtr start, ListElementPtr end)
{
	if (end == NULL)
	{
		end = last;
	}

/* Search backwards. */

	while (end != NULL && !criterion(end, data))
	{
		if (end == start)
		{
			return NULL;
		}
		end = end->previous;
	}
	return end;
}

/*
// insert()
//
// Insert an element according to some criterion.
//
// If the criterion function does not return ABORT, the element will fall
// off and be attached to the end of the list. So an element will ALWAYS be
// inserted into an empty list regardless of the criterion.
*/

BOOL List::insert(ListElementPtr element, INSERT_CRITERION criterion, VOIDPTR data)
{
	ListElementPtr search_element;
	LIST_INSERT_ENUM where = INSERT_BEFORE;			// Before end if fall through.

/* Run through all the elements in the list. */

	for (search_element = first; search_element != NULL; search_element = search_element->next)
	{
		LIST_INSERT_ENUM this_where;

	/* Invoke the criterion function. */

		this_where = criterion(element, search_element, data);

	/* See what we got. */

		if (this_where == INSERT_ABORT)
		{
			return FALSE;
		}
		else if (this_where != INSERT_NOT_YET)
		{
			where = this_where;
			break;
		}
	}

/* Do the insertion. */

	if (where == INSERT_BEFORE)
	{
		insert_before(element, search_element);
	}
	else
	{
		insert_after(element, search_element);
	}

	return TRUE;
}

/*
// Insert an element before another element.
// The "before" element of NULL is assumed to be the end of the list.
*/

VOID List::insert_before(ListElementPtr element, ListElementPtr before_element)
{
	if (element != NULL)
	{
		if (before_element == NULL)
		{
			append(element);
		}
		else
		{
		/* Hook us in. */

			if ((element->previous = before_element->previous) == NULL)
			{
				first = element;
			}
			else
			{
				element->previous->next = element;
			}

			before_element->previous = element;
			element->next = before_element;

		/* Got another element. */

			element_count++;
		}
	}
}

/*
// Insert an element after another element.
// The "after" element of NULL is assumed to be the beginning of the list.
*/

VOID List::insert_after(ListElementPtr element, ListElementPtr after_element)
{
	if (element != NULL)
	{
		if (after_element == NULL)
		{
			prepend(element);
		}
		else
		{
		/* Hook us in. */

			if ((element->next = after_element->next) == NULL)
			{
				last = element;
			}
			else
			{
				element->next->previous = element;
			}

			after_element->next = element;
			element->previous = after_element;

		/* Got another element. */

			element_count++;
		}
	}
}

/*
// Append an element to the end of the list.
*/

VOID List::append(ListElementPtr element)
{
/* Check for a good element. */

	if (element != NULL)
	{
	/* None will be after. */

		element->next = NULL;

	/* Check for an empty list. */

		if ((element->previous = last) == NULL)
		{
		/* Start the list. */
			first = element;
		}
		else
		{
		/* Continue the list. */

			last->next = element;
		}

	/* We are the last element. */

		last = element;

	/* One more in the list. */

		element_count++;
	}
}

/*
// Prepend an element to the front of the list.
*/

VOID List::prepend(ListElementPtr element)
{
/* Check for a good element. */

	if (element != NULL)
	{
	/* None will be before. */

		element->previous = NULL;

	/* Check for an empty list. */

		if ((element->next = first) == NULL)
		{
		/* Start the list. */
			last = element;
		}
		else
		{
		/* Continue the list. */

			first->previous = element;
		}

	/* We are the first element. */

		first = element;

	/* One more in the list. */

		element_count++;
	}
}

/*
// Detach an element from the list.
*/

VOID List::detach(ListElementPtr element)
{
/* Check for a good element. */

	if (element != NULL)
	{
//#ifdef INTEGRITY_CHECK
		ListElementPtr elem;

	/* Make sure we're in the list. Find the first element in the list. */

		for (elem = element; elem->previous != NULL; elem = elem->previous)
			;

		if (first != elem)
		{
			return;
		}
//#endif

	/* Do the detach. */

		ListElementPtr previous = element->previous;
		ListElementPtr next = element->next;

	/* Detach before. */

		if (previous == NULL)
		{
			first = next;
		}
		else
		{
			previous->next = next;
		}

	/* Detach after. */

		if (next == NULL)
		{
			last = previous;
		}
		else
		{
			next->previous = previous;
		}

	/* One less element. */

		element_count--;
	}
}

/*
// Return the specified element in the list.
// If the passed index is invalid, NULL is returned.
*/

ListElementPtr List::element_in_list(SHORT which)
{
/* Adjust for the base. */

	which -= element_base;

/* Validate the element passed. */

	if (which < 0 || which >= element_count)
	{
		return NULL;
	}

/* Look up the entry. */

	SHORT other_which = element_count-1 - which;
	ListElementPtr element;

/* See which way is faster, from the front or back. */

	if (other_which < which)
	{
	/* Start from the back of the list. */

		for (element = last; other_which-- && element != NULL; element = element->previous)
			;
	}
	else
	{
	/* Start from the front of the list. */

		for (element = first; which-- && element != NULL; element = element->next)
			;
	}

	return element;
}

/*
// Empty a list.
*/

VOID List::empty()
{
/* If we own the list, delete all elements in it. */

	if (owns_list)
	{
		ListElementPtr element, next;

	/* Delete all elements in the list. */

		for (element = first; element != NULL; element = next)
		{
			next = element->next;
			delete element;
		}
	}

/* Reset the list. */

	first = last = NULL;
	element_count = 0;
}

#ifdef WANT_STRING_LIST
/***********************/
/* String list methods */
/***********************/

StringListElement::~StringListElement()
{
	if (string != NULL)
	{
		heap->free(string);
	}
}
#endif

