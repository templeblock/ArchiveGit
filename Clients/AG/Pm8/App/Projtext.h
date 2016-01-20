/*
// $Workfile: PROJTEXT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROJTEXT.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     5/05/98 3:51p Mwilson
// added capability to handle sentiment extraction
// 
//    Rev 1.0   14 Aug 1997 15:25:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:38   Fred
// Initial revision.
// 
//    Rev 1.3   14 Jul 1997 11:18:40   Fred
// Change 'export' to 'extract'
// 
//    Rev 1.2   17 Mar 1997 11:43:40   Fred
// Extract project text
// 
//    Rev 1.1   14 Mar 1997 10:23:34   Fred
//  
// 
//    Rev 1.0   14 Mar 1997 10:06:34   Fred
// Initial revision.
*/

#ifndef __PROJTEXT_H__
#define __PROJTEXT_H__

class CTextRecord;
class CExtractProjectText;
class CMlsStringArray;
class PMGPageObject;
class CExtractProjectTextProgressDialog;
class DrawingObject;
class CObjectProperties;
class CFrameObject;
struct FillFormatV2;
struct OutlineFormat;
struct ShadowFormat;
struct PBOX;

class CExtractProjectText
{
public:
	CExtractProjectText(BOOL bAll  = TRUE);
	virtual ~CExtractProjectText();

public:
	void Run(void);
protected:
	void ExamineObject(const CString& strProjectPath, PMGPageObject* pObject, POINT ptOffset, CStdioFile& cfOutput);
	void ExamineFile(const CString& strProjectPath, const CString& strFileName, CExtractProjectTextProgressDialog* m_pProgressDialog, CStdioFile& cfOutput);
	void FixPageRect(PBOX& PageRect, int ProjectType, int i, bool bPortrait);
	void WritePathProperties(CStdioFile& cfOutput, RPath* pPath, int x, int y, int dx, int dy);
	void WriteTextProperties(CStdioFile& cfOutput, CFrameObject* pObj);
	void WriteObjectProperties(CStdioFile& cfOutput, LPCSTR strType, int x, int y, int dx, int dy, float fAngle);
	void WriteDrawObjectProperties(DrawingObject* pObject, bool bFill, CStdioFile& cfOutput);
	void WriteDrawProperties(CObjectProperties& Properties, bool bFill, CStdioFile& cfOutput);
	void DoWriteDrawProperties(FillFormatV2& fill, OutlineFormat& line, ShadowFormat& shadow, bool bFill, CStdioFile& cfOutput);
};


#define	IDC_PROGRESS_MESSAGE_LIST		(666)

/////////////////////////////////////////////////////////////////////////////
// CExtractProjectTextProgressDialog dialog

class CExtractProjectTextProgressDialog : public CDialog
{
// Construction
public:
	CExtractProjectTextProgressDialog(int IDD, CWnd* pParent = NULL);	// standard constructor
	virtual ~CExtractProjectTextProgressDialog();

// Dialog Data
	//{{AFX_DATA(CExtractProjectTextProgressDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CExtractProjectTextProgressDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	BOOL m_fIsAborted;
	BOOL m_fIsDone;
	CListBox* m_pMessageList;
	CWnd* m_pParent;
	
public:
	void AddMessage(LPCSTR pszAction);
	BOOL CheckForAbort(void)
		{ if (!m_fIsAborted) { Breathe(); } return m_fIsAborted; }
	void WaitForDone(void);
	void Finish(void);
	void Breathe(void);
};

#endif
