// PMWCompressView.cpp : implementation of the CPMWCompressView class
//

#include "stdafx.h"
#include "PMWCompress.h"

#include "PMWCompressDoc.h"
#include "PMWCompressView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPMWCompressView

IMPLEMENT_DYNCREATE(CPMWCompressView, CScrollView)

BEGIN_MESSAGE_MAP(CPMWCompressView, CScrollView)
	//{{AFX_MSG_MAP(CPMWCompressView)
	ON_COMMAND(ID_COMPRESS, OnCompress)
	ON_COMMAND(ID_DECOMPRESS, OnDecompress)
	ON_COMMAND(ID_WRITE_HEADER, OnWriteHeader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPMWCompressView construction/destruction

CPMWCompressView::CPMWCompressView()
{
	// TODO: add construction code here

}

CPMWCompressView::~CPMWCompressView()
{
}

BOOL CPMWCompressView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPMWCompressView drawing

void CPMWCompressView::OnDraw(CDC* pDC)
{
	CPMWCompressDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// Set up font & increment
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	int nDrop = tm.tmHeight;

	// Draw all the process strings
	CString csDisplayString;
	int nCnt = 0;
	CPoint TextLoc(0, 0);
	do
	{
		m_FileCompress.GetProcessString(nCnt, csDisplayString);
		if (!csDisplayString.IsEmpty())
		{
			pDC->TextOut(TextLoc.x, TextLoc.y, csDisplayString);
			TextLoc.y += nDrop;
			nCnt++;
		}
	}
	while (!csDisplayString.IsEmpty());

	// See if we're done
	if (!m_csComplete.IsEmpty())
	{
		TextLoc.y += nDrop;
		pDC->TextOut(TextLoc.x, TextLoc.y, m_csComplete);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPMWCompressView diagnostics

#ifdef _DEBUG
void CPMWCompressView::AssertValid() const
{
	CView::AssertValid();
}

void CPMWCompressView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPMWCompressDoc* CPMWCompressView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPMWCompressDoc)));
	return (CPMWCompressDoc*)m_pDocument;
}
#endif //_DEBUG

// Implementation
BOOL CPMWCompressView::DoFileDialog(enum OperateType operation)
{
	const MAX_FILE_CHARS = 8192;
	static char szFileBuffer[MAX_FILE_CHARS];

	// Put up a standard file dialog box
	CFileDialog FileDlg(
		TRUE,		// open files
		(operation == COMPRESS) ? ".bmp" : ".bmx"	// extension
		);

	// Set up file buffer
	FileDlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;
	FileDlg.m_ofn.nMaxFile = MAX_FILE_CHARS;
	FileDlg.m_ofn.lpstrFile = (LPTSTR)szFileBuffer;	// big enough

	// Run the dialog
	if (FileDlg.DoModal() == IDOK)
	{
		// Build the list of filenames
		CString csPath(FileDlg.m_ofn.lpstrFile);
		const char *c = (const char*)FileDlg.m_ofn.lpstrFile + csPath.GetLength() + 1;
		int nString = 0;
		CStringArray aFilenames;
		aFilenames.SetSize(12);	// good start

		// Special case-- one file selected
		if (*c == 0)
		{
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];
			_splitpath((const char*)csPath, drive, dir, fname, ext);
			csPath = drive;
			csPath += dir;
			CString csFile(fname);
			csFile += ext;
			aFilenames.SetAtGrow(nString++, csFile);
		}
		else {
			while (*c != 0)
			{
				CString csNextFile = c;
				aFilenames.SetAtGrow(nString++, csNextFile);
				c += csNextFile.GetLength() + 1;
			}
		}

		// Init file compression object
		m_FileCompress.InitFileList(operation, csPath, aFilenames, nString);
		return TRUE;
	}

	return FALSE;
}

void CPMWCompressView::PressFiles()
{
	m_csComplete.Empty();

	// While there are files left to process, etc.
	int nIndex = 0;
	while (m_FileCompress.StartOperation(nIndex))
	{
		Invalidate();
		SendMessage(WM_PAINT);
		m_FileCompress.PerformOperation(nIndex);
		nIndex++;
	}
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CPMWCompressView message handlers

void CPMWCompressView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	SetScrollSizes(MM_TEXT, CSize(0, 500));
}

void CPMWCompressView::OnCompress() 
{
	if (DoFileDialog(COMPRESS))
	{
		if(AfxMessageBox(IDS_WRITE_HEADER, MB_YESNO) == IDYES)
			m_FileCompress.SetCompressionHeader(TRUE);

		PressFiles();
		m_csComplete = "Compression complete.";
	}
}

void CPMWCompressView::OnDecompress() 
{
	if (DoFileDialog(DECOMPRESS))
	{
		if(AfxMessageBox(IDS_HAS_HEADER, MB_YESNO) == IDYES)
			m_FileCompress.SetCompressionHeader(TRUE);

		PressFiles();
		m_csComplete = "Decompression complete.";
	}
}

void CPMWCompressView::OnWriteHeader() 
{

	if (DoFileDialog(WRITE_HEADER))
	{
		m_FileCompress.SetCompressionHeader(TRUE);
		
		PressFiles();
		m_csComplete = "Header inclusion complete.";
	}
	
}

