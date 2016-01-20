/*
// $Workfile: ORDERFRM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/ORDERFRM.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:48   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:56   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:44:38   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:59:08   JAY
// Initial revision.
// 
//    Rev 1.2   06 Jan 1995 18:07:52   FRED
//  
// 
//    Rev 1.1   06 Dec 1994 13:38:44   FRED
//  
// 
//    Rev 1.0   10 Nov 1994 11:49:54   FRED
// Initial revision.
*/

class COrderFormMetaFile;
class COrderFormItem;
class COrderForm;

#ifndef __ORDERFRM_H__
#define __ORDERFRM_H__

#include "cwmf.h"
#include "mlsprint.h"

// orderfrm.h : header file
//

class COrderFormMetaFile : public CWindowsMetaFile
{

protected:

	virtual BOOL EnumMetaFileProc(HANDLETABLE* pObject, METARECORD* pMetaRecord, int nObject, CWindowsMetaFile::EnumData* pData);
};

class COrderFormItem
{

public:

	COrderFormItem(int nNumber, const CString& csName, const CString& csPrice, BOOL fHighlight);
	virtual ~COrderFormItem();
	
public:
   
   int Number(void)
   	{ return m_nNumber; }
	const CString& Name(void)
		{ return m_csName; }
	const CString& Price(void)
		{ return m_csPrice; }
	BOOL Highlight(void)
		{ return m_fHighlight; }

protected:

	int m_nNumber;
	CString m_csName;
	CString m_csPrice;
	BOOL m_fHighlight;
};

class COrderFormItems : public CPtrArray
{

public:

	COrderFormItems();
	virtual ~COrderFormItems();

	void RemoveAll(void);
	
	virtual int ItemCount(void) const
		{ return GetSize(); }
	virtual COrderFormItem* Item(int nIndex) const
		{ return (COrderFormItem*)(GetAt(nIndex)); }
};

class COrderForm : public CMlsPrint
{

public:

	enum StringAlignment
	{
		ALIGN_Left,
		ALIGN_Center,
		ALIGN_Right
	};
						
public:

	COrderForm();
	virtual ~COrderForm();
	
	virtual void OnPrepareDC();
	
	virtual BOOL Print(CWnd* pParent);
	
	virtual void Wmf(LPCSTR pszFileName);
	virtual void Wmf(HINSTANCE hInstance, LPCSTR pszResource, LPCSTR pszType);
	
	virtual void Bounds(int nTop, int nMiddle, int nBottom, int nLeft, int nRight, int nTextX, int nTextY);

	virtual void AddStringMapping(LPCSTR pszFrom, LPCSTR pszTo);
	virtual void AddIndexedStringMapping(LPCSTR pszFrom, int nIndex, LPCSTR pszTo);
	virtual void RemoveStringMapping(LPCSTR pszFrom);
	virtual BOOL LookupStringMapping(LPCSTR pszFrom, CString& csTo);
	
	virtual BOOL EnumMetaFileProc(HANDLETABLE* pObject, METARECORD* pMetaRecord, METARECORD** ppNewRecord, int nObject, CWindowsMetaFile::EnumData* pData);
	
	virtual void AddItem(int nNumber, const CString& csName, const CString& csPrice, BOOL fHighlight);
	
protected:

	virtual void OnPrint();
	virtual void GetTitle(CString& csTitle);
	
	virtual BOOL EnumerateRecord(METARECORD* pRecord);
	virtual BOOL IsPenColor(CDC* pDC, COLORREF clColor);
	
	virtual BOOL PlaySeparatorLine(HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData);
	virtual BOOL PlayItemTitle(HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData);
	virtual BOOL PlayItemLine(int nNumber, LPCSTR pszDescription, LPCSTR pszPrice, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData);
	virtual BOOL PlaySummaryLine(int nNumber, LPCSTR pszDescription, LPCSTR pszPrice, BOOL fHighlight, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData);
	virtual BOOL PlayHighlightRectangle(BOOL fHighlight, int nX0, int nY0, int nX1, int nY1, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData);
	virtual BOOL PlayTextOutRecord(LPCSTR pszString, int nX, int nY, StringAlignment nAlignment, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData);
	virtual BOOL PlayLineRecord(int nX0, int nY0, int nX1, int nY1, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData);
	virtual BOOL PlayRectangleRecord(int nX0, int nY0, int nX1, int nY1, CPen* pcpPen, CBrush* pcbBrush, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData);
	
	virtual BOOL NumberOfPages(void);
	
	CMapStringToString m_Map;
	
	LPCSTR m_pszFileName;
	
	HINSTANCE m_hInstance;
	LPCSTR m_pszResource;
	LPCSTR m_pszType;
	
	int m_nBoundsTop;
	int m_nBoundsMiddle;
	int m_nBoundsBottom;
	int m_nBoundsLeft;
	int m_nBoundsRight;
	int m_nBoundsTextX;
	int m_nBoundsTextY;
	
	int m_nItemX0;
	int m_nItemX1;
	int m_nItemX2;
	int m_nItemX3;
	int m_nItemX4;
	int m_nItemX5;
	int m_nItemX6;
	int m_nItemX7;
	int m_nItemX8;
	int m_nItemX9;
	int m_nItemX10;
	int m_nItemX11;
	int m_nItemX12;
	
	int m_nSeparatorSpacing;
	int m_nTextSpacing;
	int m_nCollectionSpacing;
	int m_nHighlightThickness;
	
	int m_nCurrentY;
	int m_nSummaryY;
	int m_nBottomY;
	
	COrderFormItems m_Items;
	int m_nItem;
	
	int m_nPage;
	int m_nNumberOfPages;
	int m_fShowPage;
};

#endif
