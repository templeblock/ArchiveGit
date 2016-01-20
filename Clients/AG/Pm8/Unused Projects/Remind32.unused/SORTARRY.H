/*
// $Workfile: SORTARRY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/SORTARRY.H $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:58   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:30   Fred
// Initial revision.
// 
//    Rev 1.6   01 Sep 1996 16:45:04   Jay
// Added FindName()
// 
//    Rev 1.5   15 Aug 1996 16:00:52   Jay
//  
// 
//    Rev 1.4   14 Aug 1996 11:43:08   Fred
// Added CMlsStringArray::GetNames()
// 
//    Rev 1.3   24 Jul 1996 13:12:56   Fred
// Moved CMlsStringArray to SORTARRY
// 
//    Rev 1.2   01 Jul 1996 07:32:04   Fred
// Receiver list
// 
//    Rev 1.1   18 Jun 1996 15:21:56   Fred
// Some fixes
// 
//    Rev 1.0   18 Jun 1996 08:28:18   Fred
// Initial revision.
*/

class CSortedPtrArrayElement;
class CSortedPtrArrayElementString;
class CSortedPtrArrayElementSigned32;
class CSortedPtrArrayElementUnsigned32;
class CSortedPtrArray;
class CSortedDWordArray;
class CMlsStringArray;

#ifndef __SORTARRY_H__
#define __SORTARRY_H__

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArrayElement

class CSortedPtrArrayElement
{
public:
	CSortedPtrArrayElement();
	virtual ~CSortedPtrArrayElement();

public:
	// Comparison function (to same kind).
	virtual int Compare(CSortedPtrArrayElement* pElement) = 0;

	// Get/Set as string.
	virtual LPCSTR GetAsString(void) = 0;
	virtual void SetAsString(LPCSTR pszString) = 0;

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void) = 0;
	virtual void SetAsSigned32(long lNumber) = 0;

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void) = 0;
	virtual void SetAsUnsigned32(DWORD dwNumber) = 0;

	// Assignment operator.
	CSortedPtrArrayElement& operator=(CSortedPtrArrayElement& Field)
		{ SetAsString((LPCSTR)Field); return *this; }
	CSortedPtrArrayElement& operator=(LPCSTR pszString)
		{ SetAsString(pszString); return *this; }
	CSortedPtrArrayElement& operator=(long lNumber)
		{ SetAsSigned32(lNumber); return *this; }
	CSortedPtrArrayElement& operator=(DWORD dwNumber)
		{ SetAsUnsigned32(dwNumber); return *this; }
	CSortedPtrArrayElement& operator=(int nNumber)
		{ SetAsSigned32((long)nNumber); return *this; }
	CSortedPtrArrayElement& operator=(unsigned int uNumber)
		{ SetAsUnsigned32((DWORD)uNumber); return *this; }

	// Conversion operator.
	operator LPCSTR ()
		{ return GetAsString(); }
	operator long ()
		{ return GetAsSigned32(); }
	operator DWORD ()
		{ return GetAsUnsigned32(); }
	operator int ()
		{ return (int)GetAsSigned32(); }
	operator unsigned int ()
		{ return (unsigned int)GetAsUnsigned32(); }

protected:
	// Local storage for string representation.
	CString m_csValue;
};

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArrayElementString

class CSortedPtrArrayElementString : public CSortedPtrArrayElement
{
	INHERIT(CSortedPtrArrayElementString, CSortedPtrArrayElement)

public:
	CSortedPtrArrayElementString(LPCSTR pszValue);
	virtual ~CSortedPtrArrayElementString();

public:
	// Comparison function (to same kind).
	virtual int Compare(CSortedPtrArrayElement* pElement);

	// Get/Set as string.
	virtual LPCSTR GetAsString(void);
	virtual void SetAsString(LPCSTR pszValue);

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void);
	virtual void SetAsSigned32(long lValue);

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwValue);
};

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArrayElementSigned32

class CSortedPtrArrayElementSigned32 : public CSortedPtrArrayElement
{
	INHERIT(CSortedPtrArrayElementSigned32, CSortedPtrArrayElement)

public:
	CSortedPtrArrayElementSigned32(long lValue);
	virtual ~CSortedPtrArrayElementSigned32();

public:
	// Comparison function (to same kind).
	virtual int Compare(CSortedPtrArrayElement* pElement);

	// Get/Set as string.
	virtual LPCSTR GetAsString(void);
	virtual void SetAsString(LPCSTR pszValue);

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void);
	virtual void SetAsSigned32(long lValue);

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwValue);

protected:
	// Local storage for Signed32 representation.
	long m_lValue;
};

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArrayElementUnsigned32

class CSortedPtrArrayElementUnsigned32 : public CSortedPtrArrayElement
{
	INHERIT(CSortedPtrArrayElementUnsigned32, CSortedPtrArrayElement)

public:
	CSortedPtrArrayElementUnsigned32(DWORD dwValue);
	virtual ~CSortedPtrArrayElementUnsigned32();

public:
	// Comparison function (to same kind).
	virtual int Compare(CSortedPtrArrayElement* pElement);

	// Get/Set as string.
	virtual LPCSTR GetAsString(void);
	virtual void SetAsString(LPCSTR pszValue);

	// Get/Set as 32-bit signed number.
	virtual long GetAsSigned32(void);
	virtual void SetAsSigned32(long lValue);

	// Get/Set as 32-bit unsigned number.
	virtual DWORD GetAsUnsigned32(void);
	virtual void SetAsUnsigned32(DWORD dwValue);

protected:
	// Local storage for Unsigned32 representation.
	DWORD m_dwValue;
};

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArray

class CSortedPtrArray : public CPtrArray
{
	INHERIT(CSortedPtrArray, CPtrArray)

public:
	CSortedPtrArray(BOOL fNoDuplicates = TRUE);
	virtual ~CSortedPtrArray();

public:
	// Delete all element objects.
	void DeleteAll(void);

	// Add a new element with sorting and duplicate checking.
	virtual int Add(CSortedPtrArrayElement* pNewElement);

	CSortedPtrArrayElement* GetAt(int nIndex) const
		{ return (CSortedPtrArrayElement*)(INHERITED::GetAt(nIndex)); }
	CSortedPtrArrayElement*& ElementAt(int nIndex)
		{ return (CSortedPtrArrayElement*&)(INHERITED::ElementAt(nIndex)); }
	CSortedPtrArrayElement* operator[](int nIndex) const
		{ return (CSortedPtrArrayElement*)(INHERITED::GetAt(nIndex)); }
	CSortedPtrArrayElement*& operator[](int nIndex)
		{ return (CSortedPtrArrayElement*&)(INHERITED::ElementAt(nIndex)); }

	// Don't allow duplicates flag.
	BOOL m_fNoDuplicates;
};

//////////////////////////////////////////////////////////////////////////
// CSortedDWordArray

class CSortedDWordArray : public CDWordArray
{
	INHERIT(CSortedDWordArray, CDWordArray)

public:
	CSortedDWordArray(BOOL fNoDuplicates = TRUE);
	virtual ~CSortedDWordArray();

public:
	// Add a new value with sorting and duplicate checking.
	virtual int Add(DWORD dwValue);

	// Delete a value.
	virtual int Delete(DWORD dwValue);

	// Search for a value.
	virtual int Find(DWORD dwValue, int* pnInsertPosition = NULL);

	// Check if a value exists.
	virtual int Exists(DWORD dwValue);

	// Compare in a non-standard way (i.e. not by value).
	virtual int CustomCompare(DWORD dwValue, DWORD dwTry);

	// Don't allow duplicates flag.
	BOOL m_fNoDuplicates;
	BOOL m_fUseCustomCompare;
};

/////////////////////////////////////////////////////////////////////////////
// CMlsStringArray

class CMlsStringArray : public CStringArray
{
public:
	CMlsStringArray(LPCSTR pszNames = NULL, char cSeparator = '|');
	CMlsStringArray(UINT uNames, char cSeparator = '|');
	virtual ~CMlsStringArray();
public:
	void SetNames(LPCSTR pszNames, char cSeparator = '|');
	void SetNames(UINT uNames, char cSeparator = '|');
	void GetNames(CString& csNames, char cSeparator = '|');
	int FindName(LPCSTR pName);
protected:
	BOOL GetNextName(CString& csName, CString& csNames, char cSeparator);
};

#endif
