/*
// $Header: /PM8/App/LAYREC.H 1     3/03/99 6:06p Gbeddow $
//
// Object layout record definitions.
//
// $Log: /PM8/App/LAYREC.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:22:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:10   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:48   Jay
// Changes in 2.1
// 
//    Rev 2.2   27 Feb 1996 16:44:12   JAY
// 32-bit fix for record (RECTS, POINTS)
// 
//    Rev 2.1   19 Apr 1995 12:31:50   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:42   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:33:50   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   01 Jun 1994 16:35:22   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:08:22   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:52   JAY
//  
*/

#ifndef __LAYREC_H__
#define __LAYREC_H__

#define	ELEMENT_XFLIPPED			0x0001
#define	ELEMENT_YFLIPPED			0x0002
#define	ELEMENT_RELATIVE_SIZE	0x0004
#define	ELEMENT_REPEATED_X		0x0008
#define	ELEMENT_REPEATED_Y		0x0010
#define	ELEMENT_CENTERED_X		0x0020
#define	ELEMENT_CENTERED_Y		0x0040
#define	ELEMENT_ALIGN_LEFT		0x0080
#define	ELEMENT_ALIGN_RIGHT		0x0100
#define	ELEMENT_ALIGN_TOP			0x0200
#define	ELEMENT_ALIGN_BOTTOM		0x0400
#define	ELEMENT_FLOW_LEFT			0x0800
#define	ELEMENT_FLOW_RIGHT		0x1000

#define	ELEMENT_HIDDEN				0x8000

#define	ELEMENT_X_UNIT				1000
#define	ELEMENT_Y_UNIT				1000

/*
// A function to call when layout elements are generated.
*/

typedef BOOL (*ELEM_RECT_FUNC)(PBOX_PTR, VOIDPTR);

/*
// A layout element.
*/

FARCLASS LayoutElement : public ListElement
{
private:
public:

	struct LErecord
	{
		FLAGS			flags;
		RECTS			where;
		POINTS		offset;		/* for repeated elements */
	} record;

/*
// Generate real rectangles from a layout element.
// "world" defines the coordinate system
// "size" gives the size of the object which will be fit in the element
// "func" is the function to pass rectangles to
// "user" is user supplied data which is passed to the rect function
*/

	VOID generate_rects(PBOX world, POINT size, FLAGS obj_flags,
									ELEM_RECT_FUNC func, VOIDPTR user);

/*
// I/O routines.
*/

	ERRORCODE read(StorageDevicePtr device);
	ERRORCODE write(StorageDevicePtr device);
	static ST_DEV_IO_SIZE size(StorageDevicePtr device);	/* Constant! */
};

typedef LayoutElement far *LayoutElementPtr;

/*
// The object layout record.
*/

FARCLASS ObjectLayoutRecord : public DatabaseRecord
{
private:

/* Data */

	CString m_csName;			/* the name of this layout. */
	List elements;				/* layout elements */

	struct
	{
		USHORT reference_count;
		SHORT type;
	} record;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	ObjectLayoutRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

public:

	virtual ~ObjectLayoutRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Set the reference count.
*/

	VOID set_reference_count(USHORT count)
		{ record.reference_count = count; }

/*
// Get the reference count.
*/

	USHORT get_reference_count(VOID)
		{ return record.reference_count; }

/*
// Increment the reference count.
*/

	USHORT inc_reference_count(VOID)
		{ return ++record.reference_count; }

/*
// Decrement the reference count.
*/

	USHORT dec_reference_count(VOID)
		{ return --record.reference_count; }

/*
// Set the layout type.
*/

	VOID set_type(SHORT t)
		{ record.type = t; }

/*
// Get the layout type.
*/

	SHORT get_type(VOID)
		{ return record.type; }

/*
// Set the name for this layout record.
// The passed name is duplicated.
*/

	void set_name(LPCSTR name);

/*
// Get the name.
*/

	LPCSTR get_name(void) const
		{ return m_csName; }

/*
// Get the elements.
*/

	ListPtr get_elements(VOID)
		{ return &elements; }

/*
// Add an element to the layout record.
*/

	VOID add_element(LayoutElementPtr element)
		{ elements.append(element); }

/*
// Return the number of elements in the list.
*/

	SHORT element_count(VOID)
		{ return elements.count(); }

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};

#endif					// #ifndef __LAYREC_H__
