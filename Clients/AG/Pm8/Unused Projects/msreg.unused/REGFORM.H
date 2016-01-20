/*
// $Workfile: REGFORM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REGFORM.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   01 Oct 1996 10:38:50   FRED
// Initial revision.
*/

class CRegistrationFormMetaFile;
class CRegistrationForm;

#ifndef __REGFORM_H__
#define __REGFORM_H__

#include "cwmf.h"
#include "mlsprint.h"

class CRegistrationFormMetaFile : public CWindowsMetaFile
{

protected:

	virtual BOOL EnumMetaFileProc(HANDLETABLE* pObject, METARECORD* pMetaRecord, int nObject, CWindowsMetaFile::EnumData* pData);
};

class CRegistrationForm : public CMlsPrint
{

public:

	enum StringAlignment
	{
		ALIGN_Left,
		ALIGN_Center,
		ALIGN_Right
	};
						
public:

	CRegistrationForm();
	virtual ~CRegistrationForm();
	
	virtual void OnPrepareDC();
	
	virtual BOOL Print(CWnd* pParent);
	
	virtual void Wmf(LPCSTR pszFileNameTop, LPCSTR pszFileNameMiddle, LPCSTR pszFileNameBottom);
	virtual void Wmf(HINSTANCE hInstance, LPCSTR pszResourceTop, LPCSTR pszResourceMiddle, LPCSTR pszResourceBottom, LPCSTR pszType);
	
	virtual void AddStringMapping(LPCSTR pszFrom, LPCSTR pszTo);
	virtual void AddIndexedStringMapping(LPCSTR pszFrom, int nIndex, LPCSTR pszTo);
	virtual void RemoveStringMapping(LPCSTR pszFrom);
	virtual BOOL LookupStringMapping(LPCSTR pszFrom, CString& csTo);
	
	virtual BOOL EnumMetaFileProc(HANDLETABLE* pObject, METARECORD* pMetaRecord, METARECORD** ppNewRecord, int nObject, CWindowsMetaFile::EnumData* pData);

protected:

	virtual void OnPrint();
	virtual void GetTitle(CString& csTitle);
	
	virtual int NumberOfPages(void);
	
	void PrintWmf(CRegistrationFormMetaFile& MetaFile, const CRect& crRect);

	CMapStringToString m_Map;
	
	LPCSTR m_pszFileNameTop;
	LPCSTR m_pszFileNameMiddle;
	LPCSTR m_pszFileNameBottom;
	
	HINSTANCE m_hInstance;
	LPCSTR m_pszResourceTop;
	LPCSTR m_pszResourceMiddle;
	LPCSTR m_pszResourceBottom;
	LPCSTR m_pszType;
};

#endif
