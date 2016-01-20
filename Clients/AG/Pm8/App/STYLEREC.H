/*
// $Header: /PM8/App/STYLEREC.H 1     3/03/99 6:12p Gbeddow $
//
// Style record definitions.
//
// $Log: /PM8/App/STYLEREC.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:08   Fred
// Initial revision.
// 
//    Rev 1.1   25 Apr 1996 12:16:24   Jay
//  
// 
//    Rev 1.0   23 Apr 1996 08:08:20   Jay
// Initial revision.
*/

#ifndef __STYLEREC_H__
#define __STYLEREC_H__

#include "text.h"

/*
// The text style data record.
// This is used by both character and paragraph named styles. The "type"
// member differentiates them.
*/

class CTextStyleRecord : public DatabaseRecord
{
public:

	struct CCreationData
	{
		LPCSTR m_pszName;
		WORD m_wType;
		CTextStyle* m_pStyle;			// May be NULL.
	};

/*
// The destructor.
*/

	virtual ~CTextStyleRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Different text style types.
*/

	enum
	{
		TEXT_STYLE_Character = 1,
		TEXT_STYLE_Paragraph
	};

/*
// Different text style flags.
*/

	enum
	{
		FLAG_Builtin = 1,				// Non-deletable
		FLAG_Default = 2				// Default style
	};

/*
// Increment the style reference count.
*/

	DWORD Increment(void);

/*
// Decrement the style reference count.
*/

	DWORD Decrement(void);

/*
// Return the reference count.
*/

	DWORD ReferenceCount(void) const
		{ return m_Record.m_dwReferenceCount; }

/*
// Style type access.
*/

	WORD Type(void) const
		{ return m_Record.m_wType; }
	void Type(WORD wType)
		{ m_Record.m_wType = wType; modified(); }

/*
// Flags access.
*/

	DWORD Flags(void) const
		{ return m_Record.m_dwFlags; }
	void Flags(DWORD dwFlags)
		{ m_Record.m_dwFlags = dwFlags; modified(); }

/*
// Style name access.
*/

	LPCSTR Name(void) const
		{ return m_csName; }
	void Name(LPCSTR pszName)
		{ m_csName = pszName; modified(); }

/*
// Style access.
*/
	CTextStyle& Style(void)
		{ return m_Style; }

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE ReadData(StorageDevice* pDevice);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE WriteData(StorageDevice* pDevice);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevice* pDevice);

protected:

	struct
	{
	// The type of text style this is. (See above enum)
		WORD	m_wType;

	// Flags. (like built-in, read-only, etc.)
		DWORD	m_dwFlags;

	// The reference count for this text style.
		DWORD	m_dwReferenceCount;

	} m_Record;

/*
// The style this represents.
*/

	CTextStyle m_Style;

/*
// The name of this style.
*/

	CString m_csName;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CTextStyleRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);
};

#endif					// #ifndef __STYLEREC_H__
