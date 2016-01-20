/*
// $Workfile: BMPARRAY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/BMPARRAY.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:44   Fred
// Initial revision.
// 
//    Rev 1.0   28 Jul 1996 13:49:16   Fred
// Initial revision.
*/ 

class CBitmapResourceArray;

#ifndef __BMPARRAY_H__
#define __BMPARRAY_H__

//////////////////////////////////////////////////////////////////////////
// CBitmapResourceArray

class CBitmapResourceArray : public CPtrArray
{
	INHERIT(CBitmapResourceArray, CPtrArray)

	// Array item
	class Item
	{
	public:
		Item();
		virtual ~Item();

	public:
		BOOL Initialize(LPCSTR pszName);
		LPCSTR GetName(void)
			{ return m_csName; }
		BOOL GetSize(int& nWidth, int& nHeight);
		BOOL GetBitmap(CBitmap& Bitmap);

	protected:
		CString m_csName;

		BOOL m_fHeaderValid;
		BITMAPINFOHEADER m_Header;

		BOOL LoadHeader(void);
	};

public:

	// construction and destruction
	CBitmapResourceArray();
	virtual ~CBitmapResourceArray();

public:

	BOOL SetResourceNames(LPCSTR pszResourceNames);
	BOOL GetBitmap(CBitmap& Bitmap, const CRect& crRect);
	
public:

	void DeleteAll(void);
	void DeleteAt(int nIndex, int nCount = 1);

	Item* GetAt(int nIndex) const
		{ return (Item*)(INHERITED::GetAt(nIndex)); }
	Item*& ElementAt(int nIndex)
		{ return (Item*&)(INHERITED::ElementAt(nIndex)); }
	Item* operator[](int nIndex) const
		{ return (Item*)(INHERITED::GetAt(nIndex)); }
	Item*& operator[](int nIndex)
		{ return (Item*&)(INHERITED::ElementAt(nIndex)); }
};

#endif
