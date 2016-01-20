/*
// $Workfile: CheckArtExists.h $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $ 
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CheckArtExists.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     8/05/98 12:00p Mwilson
// added ability to check for art in a spreadsheet
// 
// 2     5/14/98 11:50a Mwilson
// moved include of pmwdoc.h out of header and into cpp file
// 
// 1     5/14/98 10:32a Mwilson
// Utility for checking to make sure the art is all there for a directory
// of pmw files
// 
// Initial revision.
*/

#ifndef __CHECKART_H__
#define __CHECKART_H__

class ObjectList;
class CArtExistsDlg;

class CCheckArtExists
{
public:
	CCheckArtExists();
	virtual ~CCheckArtExists();

public:
	// Run the show.
	void Run(void);
protected:
	virtual CString GetNextProjName() = 0;
	virtual BOOL SetupProjectIterator(CString csArtSrc, CString csProjSrc) = 0;
	virtual BOOL CheckArtFile(CString csArtFilePath) = 0;
	void CheckObjectList(CListBox* pListBox, ObjectList *pList);
	CString m_csArtDir;
	CString m_csProjDir;

	CFile m_errorFile;
	CStdioFile m_cfOutFileNames;
	CArray<CString, CString>m_errorList;
	CArtExistsDlg* m_pListDlg;
	BOOL m_bSrcIsDir;
	char m_szCurDir[MAX_PATH];
	BOOL m_bCheckArt;

};

class CCheckArtExistsDir : public CCheckArtExists
{
public:
	CCheckArtExistsDir(){m_bSrcIsDir = TRUE;}
protected:
	virtual CString GetNextProjName();
	virtual BOOL SetupProjectIterator(CString csArtSrc, CString csProjSrc);
	virtual BOOL CheckArtFile(CString csArtFilePath);
	CFileFind m_fileFinder;
};


class CCheckArtExistsSS : public CCheckArtExists
{
public:
	CCheckArtExistsSS(){m_bSrcIsDir = FALSE;}
protected:
	virtual CString GetNextProjName();
	virtual BOOL SetupProjectIterator(CString csArtSrc, CString csProjSrc);
	virtual BOOL CheckArtFile(CString csArtFilePath);

	BOOL CreateArtFileList(CString csArtSSPath);

	int m_nProjDirIndex;
	int m_nProjFileNameIndex;
	CString m_csProjSS;
	CStdioFile m_cfProjFile;
	CStringArray m_csaArtFileList;
	CMapStringToPtr m_StringMap;
};



#endif
