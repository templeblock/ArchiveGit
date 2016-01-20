/*
// $Workfile: ADDRDATA.H $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDRDATA.H $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 2     5/20/98 2:53p Cboggio
// Put country names in the resource file
// 
//    Rev 1.0   14 Aug 1997 15:18:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:36   Fred
// Initial revision.
// 
//    Rev 1.3   12 Nov 1996 09:55:20   johno
// Removed string declarations
// (moved to resource DLL)
// 
//    Rev 1.2   18 Jul 1996 07:28:40   Fred
// Fixed picture bug; Moved countries to ADDRDATA
// 
//    Rev 1.1   17 Jul 1996 19:08:34   Fred
// Picture blobs now have compression type
// 
//    Rev 1.0   17 Jul 1996 13:25:32   FRED
// Initial revision.
*/

#ifndef __ADDRDATA_H__
#define __ADDRDATA_H__

#include "flatdb.h"

// File extension for address books.

#define	ADDRESS_BOOK_EXTENSION			".ABK"

// The blobs in an address book database have a leading DWORD which identifies their type.

#define	BLOB_TYPE_MEMO							(1)		// Memo text
#define	BLOB_TYPE_PICTURE						(2)		// Picture (Name/Packed DIB Thumbnail)

// Picture blobs in the address book have a compression type.

#define	BLOB_COMPRESS_NONE					(0)		// No compression
#define	BLOB_COMPRESS_DEFLATE				(1)		// CDeflate

// The following definitions are used to build and parse the user data
// for a PrintMaster address book database field.

// defined field editors
#define	FIELD_EDIT_NONE						(0)		// cannot be edited
#define	FIELD_EDIT_NORMAL						(1)		// type in string value
#define	FIELD_EDIT_LIST						(2)		// select from list or type in new value
#define	FIELD_EDIT_LIST_FIXED				(3)		// select from fixed list
#define	FIELD_EDIT_PICTURE					(4)		// choose picture (bmp)
#define	FIELD_EDIT_MEMO						(5)		// edit variable length text

// defined lists
#define	FIELD_LIST_NONE						(0)		// no specific list
#define	FIELD_LIST_RELATIONSHIP				(1)		// relationship list
#define	FIELD_LIST_STATE						(2)		// state list
#define	FIELD_LIST_COUNTRY					(3)		// Country list

// construct field user data
#define	MAKE_FIELD_USER_DATA(e, w)		MAKELONG(e, w)

// predefined common values
#define	FNOEDIT			MAKE_FIELD_USER_DATA(FIELD_EDIT_NONE, 0)
#define	FNORMAL			MAKE_FIELD_USER_DATA(FIELD_EDIT_NORMAL, 0)
#define	FLIST				MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST, FIELD_LIST_NONE)
#define	FLISTFIXED		MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST_FIXED, FIELD_LIST_NONE)
#define	FRELATIONSHIP	MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST, FIELD_LIST_RELATIONSHIP)
#define	FSTATE			MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST, FIELD_LIST_STATE)
#define	FCOUNTRY			MAKE_FIELD_USER_DATA(FIELD_EDIT_LIST, FIELD_LIST_COUNTRY)
#define	FPICTURE			MAKE_FIELD_USER_DATA(FIELD_EDIT_PICTURE, 0)
#define	FMEMO				MAKE_FIELD_USER_DATA(FIELD_EDIT_MEMO, 0)

// parse field user data
#define	FIELD_USER_DATA_EDITOR(d)	LOWORD(d)
#define	FIELD_USER_DATA_WORD(d)		HIWORD(d)

// The following definitions are used to build and parse the user data
// for a PrintMaster address book database index.

// defined index display types
#define	INDEX_DISPLAY_NONE					(0)		// do not display index
#define	INDEX_DISPLAY_ALPHA					(1)		// display index in alphabetic format
#define	INDEX_DISPLAY_ALPHA_LIST			(2)		// display index in alphabetic list format
#define	INDEX_DISPLAY_NUMERIC				(3)		// display index in numeric format
#define	INDEX_DISPLAY_ZIP						(4)		// display index in zip format

// defined alphabetic lists
#define	INDEX_LIST_NONE						(0)		// no specific list
#define	INDEX_LIST_RELATIONSHIP				(1)		// relationship list
#define	INDEX_LIST_STATE						(2)		// state list
#define	INDEX_LIST_COUNTRY					(3)		// country list

// construct index user data
#define	MAKE_INDEX_USER_DATA(d, w)			MAKELONG(d, w)

// predefined common values
#define	INODISPLAY		MAKE_INDEX_USER_DATA(INDEX_DISPLAY_NONE, 0)
#define	IALPHA(l)		MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA, l)
#define	IALPHA1			IALPHA(1)
#define	IALPHA2			IALPHA(2)
#define	IALPHA3			IALPHA(3)
#define	INUMERIC			MAKE_INDEX_USER_DATA(INDEX_DISPLAY_NUMERIC, 0)
#define	IZIP(l)			MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ZIP, l)
#define	IZIP1				IZIP(1)
#define	IZIP2				IZIP(2)
#define	IZIP3				IZIP(3)
#define	ILIST				MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA_LIST, INDEX_LIST_NONE)
#define	IRELATIONSHIP	MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA_LIST, INDEX_LIST_RELATIONSHIP)
#define	ISTATE			MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA_LIST, INDEX_LIST_STATE)
#define	ICOUNTRY			MAKE_INDEX_USER_DATA(INDEX_DISPLAY_ALPHA_LIST, INDEX_LIST_COUNTRY)

// parse index user data
#define	INDEX_USER_DATA_DISPLAY(d)	LOWORD(d)
#define	INDEX_USER_DATA_WORD(d)		HIWORD(d)

// JRO extern char szStandardDatabaseDescription[];
extern CFlatFileFieldDescription StandardDatabaseFieldDescriptions[];
extern CFlatFileIndexDescription StandardDatabaseIndexDescriptions[];

// JRO extern char szStandardSenderDatabaseDescription[];
extern CFlatFileFieldDescription StandardSenderDatabaseFieldDescriptions[];
extern CFlatFileIndexDescription StandardSenderDatabaseIndexDescriptions[];

//extern LPCSTR AddressBookCountries[];
extern UINT AddressBookCountries[];

#endif
