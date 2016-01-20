// agcvtDlg.h : header file
//

#if !defined(AFX_AGCVTDLG_H__68EA4396_A8D3_11D1_BCE8_00A0246D41C3__INCLUDED_)
#define AFX_AGCVTDLG_H__68EA4396_A8D3_11D1_BCE8_00A0246D41C3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "pmw.h"
#include "cwmf.h"
#include "resource.h"
#include "pagedefs.h"
#include "afxtempl.h"
#include "pmgobj.h"
#include "frameobj.h"


/////////////////////////////////////////////////////////////////////////////
// CAgcvtDlg dialog
struct BmpData
{
	CRect rcBmpDst;
	BITMAPINFO* pBmpInfo;
	BYTE* pBits;
};

struct PolyData
{
	CPoint ptOrigin;
	CPoint ptExt;
	int nCount;
	CMetaFileDC* pDC;
};


class CTextInfo : public CObject
{
public:
	CTextInfo(){;}
	CPoint m_ptLoc;
	LOGFONT m_lf;
	CString m_strText;
	COLORREF m_crTextColor;
	int m_nPS4Panel;
};

class CAgcvtDlg : public CDialog
{
// Construction
public:
	CAgcvtDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAgcvtDlg)
	enum { IDD = IDD_AGCVT_DIALOG };
	CEdit	m_editFile;
	int		m_nFontNorm;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgcvtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	NEWINFO m_projectInfo;

	// Generated message map functions
	//{{AFX_MSG(CAgcvtDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void ParseWmf(const CString& csPathName);


};


class CMyMetaFile : public CWindowsMetaFile
{
public:

	CMyMetaFile();
	virtual ~CMyMetaFile();
	//create a new project
	BOOL CreateNewProject(CString strText);
	//if there isn't a text string describing the project type try to guess at the type
	//based on the metafiles window ext.  This is not always accurate.
	BOOL SetupProjectFromWindowExt();
	//process the metafile
   void ProcessMetafile(CDC* pDC, BOOL fMask = FALSE, LPARAM lParam = 0);
	//override to handle opening a non aldus metafile
   virtual BOOL GetMetaFile(void);
  
	virtual BOOL Init(LPCSTR pszFileName, CString strDefaultFontName);

protected:
	//Sets the printmaster panel to the correct panel but it returns the .ds4 panel
	int SetPanel( CPoint ptLoc );
	//returns the .ds4 panel
	int GetPanel( CPoint ptLoc );
	//determines if we are starting a new bmp.  Every bmp comes in one line at a time so we want 
	//to combine all the lines into a single bmp
	BOOL IsNewBmp(METARECORD* pMetaRecord);
	//returns the index into the bmp array
	int GetBmpDataIndex(METARECORD* pMetaRecord);
	//takes a rect in the input coordinates and converts it into a pbox in printmaster coords.
	PBOX ComputeOutputRect(CRect rcInput, int nPS4Panel);
	//handles the stretch dib metafile record
	void StretchDIBRecord(METARECORD* pMetaRecord);

	//handles the create pen metafile record
	void CreatePenRecord(METARECORD* pMetaRecord);
	//handles the create brush metafile record
	void CreateBrushRecord(METARECORD* pMetaRecord);
	//handles the text align metafile record
	void SetTextAlignRecord(METARECORD* pMetaRecord);
	//handles the text color metafile record
	void SetTextColorRecord(METARECORD* pMetaRecord);
	//handles the create font metafile record
	void CreateFontRecord(METARECORD* pMetaRecord);
	//handles the textout metafile record
	void TextOutRecord(METARECORD* pMetaRecord);
	//handles the polygon, polyline, and polypolygon records.
	void PolyRecord(HANDLETABLE* pObject, METARECORD* pMetaRecord, int nObject, BOOL bPolyPoly = FALSE);


	void AddRotatedText();
	int GetFontSize(LOGFONT& lf);
	void AddText();
	//takes a polygon metafile record and determines if it is a normal rectangle.  If it is this
	//function converts the polygon into a printmaster rectangle object
	BOOL PolygonToRect(METARECORD* pMetaRecord);
	//Used to determine the number of vector graphics we will add and the extents of each one
	void GetPolyData(METARECORD* pMetaRecord, BOOL bPolyPoly = FALSE);
	//returns the index of the array that contains that should contain the actual polygon
	int GetPolyIndex(METARECORD* pMetaRecord, BOOL bPolyPoly = FALSE);
	//determine if the polygon is within the panel bounds.  Needed for projects that have more than
	//one panel because these projects have polygons that describe the project and we don't want to add 
	//them
	BOOL PolyWithinPanel(METARECORD* pMetaRecord, BOOL bPolyPoly = FALSE);
	//adds a metafile object to the project.
	void AddMetafile(PolyData* pPolyData );

	CTypedPtrList<CObList, CTextInfo*> m_TextInfoList;
	CTypedPtrList<CObList, CTextInfo*> m_RotatedTextInfoList;

	BOOL m_bGetTextInfo;
	//stores the project info
	NEWINFO* m_pInfo;
	//pointer to the created document
	CPmwDoc* m_pDoc;
	//flag for project creation
   BOOL m_bProjectCreated;
	//used to flag the first pass through metafile which is used to gather info about the metafile
   BOOL m_bGetData;
   BYTE* m_pTempBits;
   int m_nScanLine;
   //stores the panels in rects.  If it is a card the panel order is Front, Inside Front, Inside Back, Back
   CArray<CRect, CRect> m_PanelArray;
   //stores the bitmap info
   CArray<BmpData, BmpData> m_BmpDataArray;
   //stores the polygon info
   CArray<PolyData, PolyData> m_PolyDataArray;
   //current polygon in progress
   int m_nCurPolyIndex;
   //current bmp in progress
   int m_nCurBmpIndex;
   //stores the last created font.
   LOGFONT m_lf;
	//stores the position of the last text out
   CPoint m_ptLastTextLine;

   int m_nLastRightBoundary;
   //current text frame object
   CFrameObject* m_pCurTextFrame;
	//the last panel that was altered
   int m_nLastPS4Panel;
   //horizontal algignmet for text
   ALIGN_TYPE m_nHorizTextAlign;
   //vertical alignment for text
   UINT m_nVertTextAlign;
   //stores the text color
   COLORREF m_crTextColor;
   //used to flag that any polygons that are encontered when this is true are to be added to the project
   BOOL m_bOkToAddPoly;
   int m_nPenWidth;
   //if the project creation this is set to true so we don't get a crash
   BOOL m_bBail;
   //the window ext of the metafile
   CSize m_szWindowExt;
   // the text bounds
   PBOX m_pbTextBounds;
   //stores the save extention.  This determines the project type
   CString m_strSaveExt;
   //stores the logpen from the create pen record
   LOGPEN m_lgPen;
   //stores the logbrush from the create brush record
   LOGBRUSH m_lgBrush;
   //flags that a brush or pen was created so when we get the next SelectObject record we know that a brush
   //or pen was created
	BOOL m_bBrushCreated;
	BOOL m_bPenCreated;
	//stores the index of the currently selected pen or brush in the metafiles dc
   int m_nCurPenObjSel;
   int m_nCurBrushObjSel;
   //stores the default font name to substitute for arial
   CString m_strDefaultFontName;
	//arrays used for keeping track of which brush and pen are selected into the metafiles dc.
   CArray<LOGBRUSH, LOGBRUSH>m_brushArray;
   CArray<LOGPEN, LOGPEN>m_penArray;


	virtual BOOL EnumMetaFileProc(HANDLETABLE FAR* pObject, METARECORD FAR* pMetaRecord, int nObject, EnumData* pData);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCVTDLG_H__68EA4396_A8D3_11D1_BCE8_00A0246D41C3__INCLUDED_)
