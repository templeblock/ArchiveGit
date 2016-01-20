#ifndef __LIST_H__
#define __LIST_H__
/*
// List handling routines.
// Lists are doubly-linked for various reasons.
*/

typedef class ListElement far *ListElementPtr;

FARCLASS ListElement
{
	friend class List;
private:
/* The previous element in the list. */

	ListElementPtr	previous;

/* The next element in the list. */

	ListElementPtr	next;

public:

/*
// The constructor for the list element.
*/

	ListElement() : previous(NULL), next(NULL) {}

/*
// The destructor for the list element.
// This is virtual so it can be overloaded by derived classes.
*/

	virtual ~ListElement() { }

/*
// previous_element()
//
// Return the previous element in the list.
*/

	ListElementPtr previous_element() { return previous; }

/*
// next_element()
//
// Return the next element in the list.
*/

	ListElementPtr next_element() { return next; }

};

enum LIST_INSERT_ENUM
{
	INSERT_ABORT = -1,		/* Yikes! Don't insert after all */
	INSERT_NOT_YET,			/* Don't insert yet. */
	INSERT_BEFORE,				/* Insert before the current element */
	INSERT_AFTER				/* Insert after the current element */
};

/*
// A list of list elements.
*/

FARCLASS List
{
private:

	typedef BOOL (*SEARCH_CRITERION)(ListElementPtr element, VOIDPTR data);
	typedef LIST_INSERT_ENUM (*INSERT_CRITERION)(ListElementPtr insert_element, ListElementPtr list_element, VOIDPTR data);

	ListElementPtr	first;				/* the first element in the list */
	ListElementPtr	last;					/* the last element in the list */
	SHORT element_base;					/* the base index for elements */
	SHORT element_count;					/* number of elements in this list */
	BOOL owns_list;						/* do we own our list elements? */
public:
/*
// The constructor for a list.
*/

	List(SHORT base = 0, BOOL owns = FALSE) : first(NULL), last(NULL), element_base(base), element_count(0), owns_list(owns) { }

/*
// The destructor for a list.
// This is virtual so it can be overloaded by derived classes.
*/

	virtual ~List() { empty(); }

/*
// first_that()
//
// Return the first element that satisfies a certain criterion.
*/

	ListElementPtr first_that(SEARCH_CRITERION criterion, VOIDPTR data = NULL, ListElementPtr start = NULL, ListElementPtr end = NULL);

/*
// last_that()
//
// Return the last element that satisfies a certain criterion.
*/

	ListElementPtr last_that(SEARCH_CRITERION criterion, VOIDPTR data = NULL, ListElementPtr start = NULL, ListElementPtr end = NULL);

/*
// first_element()
//
// Return the first element in the list.
*/

	ListElementPtr first_element() { return first; }

/*
// last_element()
//
// Return the last element in the list.
*/

	ListElementPtr last_element() { return last; }

/*
// size()
//
// Return the size of the list (as expressed in numbers of elements).
*/

	SHORT	count()	{ return element_count; }

/*
// insert()
//
// Insert an element according to some criterion.
//
// If the criterion function does not return ABORT, the element will fall
// off and be attached to the end of the list. So an element will ALWAYS be
// inserted into an empty list regardless of the criterion.
//
// Return FALSE if the insertion criterion returns ABORT; otherwise, TRUE.
*/

	BOOL insert(ListElementPtr element, INSERT_CRITERION criterion, VOIDPTR data = NULL);

/*
// insert_before()
//
// Insert an element before another element already in the list.
// The "before" element of NULL is assumed to be the end of the list.
*/

	VOID insert_before(ListElementPtr element, ListElementPtr before_element);

/*
// insert_after()
//
// Insert an element after another element already in the list.
// The "after" element of NULL is assumed to be the beginning of the list.
*/

	VOID insert_after(ListElementPtr element, ListElementPtr before_element);

/*
// append()
//
// Add an element to the end of the list.
// Nothing happens if NULL is passed.
*/

	VOID append(ListElementPtr element);

/*
// prepend()
//
// Add an element to the front of the list.
// Nothing happens if NULL is passed.
*/

	VOID prepend(ListElementPtr element);

/*
// element_in_list()
//
// Return the desired element in the list.
// The index passed is based on the base used when the list was created.
// NULL is returned if 'which' is out of range.
*/

	ListElementPtr element_in_list(SHORT which);

/*
// detach()
//
// Detach an element from the list.
// Nothing happens if NULL is passed.
*/

	VOID detach(ListElementPtr element);
	VOID detach(SHORT which) { detach(element_in_list(which)); }

/*
// set_ownership()
//
// Sets the ownership flag for this list.
*/

	VOID set_ownership(BOOL o) { owns_list = o; }

/*
// empty()
//
// Make the list be empty.
// If the list owns its objects, they are deleted.
*/

	VOID empty();

};

typedef List far *ListPtr;
typedef List far &ListRef;

#ifdef WANT_STRING_LIST
/***** THIS CODE HAS NOT BEEN COMPILED! *****/

/*
// A string list element.
*/

FARCLASS StringListElement : public ListElement
{
private:
	LPSTR string;
public:
	StringListElement() : string(NULL) {}
	~StringListElement();
};

typedef class StringListElement far *StringListElementPtr;

/*
// A string list.
*/

FARCLASS StringList : public List
{
public:
/*
// string_in_list()
//
// Return the desired string in the list.
// The index passed is based on the base used when the list was created.
// NULL is returned if 'which' is out of range.
*/

	StringListElementPtr string_in_list(SHORT which);
};
#endif // #ifdef WANT_STRING_LIST

#endif							/* #ifndef __LIST_H__ */
