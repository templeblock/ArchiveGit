/*
// $Header: /PM8/App/MACROREC.H 1     3/03/99 6:07p Gbeddow $
//
// Macro list record definitions.
// A macro list record contains the list of macros for a document.
//
// $Log: /PM8/App/MACROREC.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 2     3/18/98 11:14a Jayn
// Page numbers
// 
//    Rev 1.0   14 Aug 1997 15:22:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:32   Fred
// Initial revision.
// 
//    Rev 1.3   13 Mar 1997 13:17:18   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.2   16 Jul 1996 17:29:02   Fred
// More address/sender book stuff
// 
//    Rev 1.1   16 Jul 1996 08:47:04   Fred
// Changes for new address book
// 
//    Rev 1.0   14 Mar 1996 13:44:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:46   FRED
// Initial revision.
// 
//    Rev 1.6   14 Sep 1995 09:32:20   JAY
// 32-bit changes
// 
//    Rev 1.5   25 Aug 1995 15:01:32   JAY
// New user type.
// 
//    Rev 1.4   02 Aug 1995 12:56:46   FRED
// Added types for receiver and sender address book macros.
// 
// 
//    Rev 1.3   01 May 1995 15:24:52   JAY
// Restructured the macro stuff
// 
//    Rev 1.2   01 May 1995 08:44:22   JAY
// New AssignData() method.
// 
//    Rev 1.1   19 Apr 1995 12:31:36   JAY
//  
// 
//    Rev 1.0   29 Mar 1995 08:59:48   JAY
// Initial revision.
*/

#ifndef __MACROREC_H__
#define __MACROREC_H__

#define MACRO_FLAG_PermanentName	1
#define MACRO_Max (MACRO_CHARACTER_MAX-MACRO_CHARACTER)

class CMacro
{
public:
	CMacro();
	virtual ~CMacro();

	// Type of macros.
	enum Type
	{
		MACRO_TYPE_User = 1,						// Standard value.
		MACRO_TYPE_Receiver = 2,				// Receiver address book field
		MACRO_TYPE_Sender = 3,					// Sender address book field
		MACRO_TYPE_Global = 4,					// Global value.
		MACRO_TYPE_Private = (int)0xc000		// Here and up are private macro types.
	};

	// Type of values.
	enum ValueType
	{
		VALUE_TYPE_String = 1,					// Value is literal string.
		VALUE_TYPE_OldField,						// Value is old database field.
		VALUE_TYPE_Field							// Value is database field.
	};

/*
// Type access.
*/

	WORD MacroType(void) const
		{ return m_Record.m_wType; }
	void MacroType(WORD wType)
		{ m_Record.m_wType = wType; }

/*
// Id access.
*/

	WORD Id(void) const
		{ return m_Record.m_wID; }
	void Id(WORD wID)
		{ m_Record.m_wID = wID; }

/*
// Flags access.
*/

	WORD Flags(void) const
		{ return m_Record.m_wFlags; }
	void Flags(WORD wFlags)
		{ m_Record.m_wFlags = wFlags; }

/*
// Name access.
*/

	const CString& Name(void) const
		{ return m_csName; }
	void Name(const CString& csName)
		{ m_csName = csName; }

/*
// Value access.
*/

	const CString& Value(void) const
		{ return m_csValue; }
	void Value(const CString& csValue)
		{ m_csValue = csValue; }

/*
// Value type access.
*/

	WORD MacroValueType(void) const
		{ return m_Record.m_wValueType; }
	void MacroValueType(WORD wValueType)
		{ m_Record.m_wValueType = wValueType; }

/*
// Field class access.
*/

	const CString& FieldClass(void) const
		{ return m_csFieldClass; }
	void FieldClass(const CString& csFieldClass)
		{ m_csFieldClass = csFieldClass; }

/*
// Assign data. This will not change our id.
*/

	virtual void AssignData(const CMacro* pMacro);

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device);

protected:

/*
// Data.
*/

	struct
	{
		WORD m_wType;					// Macro type.
		WORD m_wID;						// Macro ID (used to construct text character).
		WORD m_wFlags;					// Macro flags.
		WORD m_wValueType;			// Type of data (string, database field, etc).
	} m_Record;

	CString m_csName;				// Human-readable name of macro.
	CString m_csValue;			// The value for this macro (may be field).
	CString m_csFieldClass;		// The field class for this macro.
};

/*
// A list of macros.
*/

class CMacroList : public CPtrArray
{
public:
	~CMacroList();

/*
// Assignment operator.
*/

	const CMacroList& operator=(const CMacroList& List);

/*
// Add a macro.
*/

	CMacro* AddMacro(LPCSTR pName, LPCSTR pValue);
	CMacro* AddMacroAt(int nIndex, LPCSTR pName, LPCSTR pValue);

/*
// Delete a macro by name.
*/

	void DeleteMacro(LPCSTR pName);

/*
// Delete a macro by index.
*/

	void DeleteMacro(int nMacro);

/*
// Delete all macros.
*/

	void DeleteAll(void);

/*
// Get the macro count.
*/

	int Macros(void) const
		{ return GetSize(); }

/*
// Get a macro.
*/

	CMacro* Macro(int nMacro) const
		{ return (CMacro*)GetAt(nMacro); }

/*
// Find a macro by id.
*/

	CMacro* FindMacro(WORD wID);

/*
// Find a macro by name.
*/

	CMacro* FindMacro(LPCSTR pName);

/*
// Get the title for a macro.
*/

	BOOL GetMacroTitle(int nMacro, CString& csTitle);

/*
// Get the value for the n'th macro.
*/

	BOOL GetMacroValue(int nMacro, CString& csValue);

/*
// Set the value for the n'th macro.
*/

	BOOL SetMacroValue(int nMacro, const CString& csNewValue);

/*
// Import a macro from another macro list.
*/

	WORD ImportMacro(WORD wMacroId, CMacroList* pList);

protected:

/*
// Generate a unique ID for a macro.
*/

	WORD UniqueID(void);
};

/*
// The macro list data record.
*/

class MacroListRecord : public DatabaseRecord
{
public:

/*
// The destructor.
*/

	~MacroListRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Get version.
*/
	WORD GetVersion(void)
		{ return m_Record.m_wVersion; }

/*
// Add a macro.
*/

	CMacro* AddMacro(LPCSTR pName, LPCSTR pValue);

/*
// Delete a macro.
*/

	void DeleteMacro(int nMacro);

/*
// Get the macro count.
*/

	int Macros(void) const
		{ return m_Macros.Macros(); }

/*
// Get a macro.
*/

	CMacro* Macro(int nMacro) const
		{ return m_Macros.Macro(nMacro); }

/*
// Find a macro by id.
*/

	CMacro* FindMacro(WORD wID)
		{ return m_Macros.FindMacro(wID); }

/*
// Find a macro by name.
*/

	CMacro* FindMacro(LPCSTR pName)
		{ return m_Macros.FindMacro(pName); }

/*
// Do we show values?
*/

	BOOL ShowValues(void) const
		{ return m_Record.m_fShowValues; }

/*
// Set whether we show values or not.
*/

	void ShowValues(BOOL fShowValues)
		{ m_Record.m_fShowValues = fShowValues; modified(); }

/*
// Return the macro list for this record.
*/

	CMacroList& MacroList(void)
		{ return m_Macros; }

/*
// Import a macro from another macro list.
*/

	WORD ImportMacro(WORD wMacroId, MacroListRecord* pRecord)
		{ return m_Macros.ImportMacro(wMacroId, &pRecord->m_Macros); }

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	virtual ERRORCODE ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	virtual ERRORCODE WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	virtual ST_MAN_SIZE SizeofData(StorageDevicePtr device);

protected:

	struct
	{
		WORD m_wCount;			// For transportation purposes only.
									// Will mirror the array size, but only set and
									// used at read and write time.
		WORD m_fShowValues;	// A place to store this state (used to be BOOL).
		WORD m_wVersion;		// 0 = 2.0,2.1, 3 = 3.0
	} m_Record;

/*
// The list of macros.
*/

	CMacroList m_Macros;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	MacroListRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

//	virtual ERRORCODE assign(DatabaseRecordRef record);
};

#endif					// #ifndef __MACROREC_H__
