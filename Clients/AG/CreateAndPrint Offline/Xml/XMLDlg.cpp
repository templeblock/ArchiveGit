// XMLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "XMLDlg.h"
#include "CXMLDocument.h"
#include "PngToDib.h"
#include "scappint.h"
#include "Image.h"

#include <fstream>

#define NLAYERS 2

/////////////////////////////////////////////////////////////////////////////
// CXMLDlg dialog

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CXMLDlg, CDialog)
	//{{AFX_MSG_MAP(CXMLDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnLoad)
	ON_BN_CLICKED(IDC_STREAM, OnStreamClicked)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowseClicked)
	ON_BN_CLICKED(IDC_CREATE, OnCreateOutput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CXMLDlg::CXMLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXMLDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXMLDlg)
	m_strFileName = _T("");
	m_bAsync = false;
	m_bStream = false;
	m_szProjectPath = _T("");
	m_pAGDoc = NULL;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXMLDlg)
	DDX_Control(pDX, IDC_ASYNC, m_btnAsync);
	DDX_Control(pDX, IDC_TREE, m_treeOutput);
	DDX_Text(pDX, IDC_FILENAME, m_strFileName);
	DDX_Check(pDX, IDC_ASYNC, m_bAsync);
	DDX_Check(pDX, IDC_STREAM, m_bStream);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CXMLDlg message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CXMLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, true); 		// Set big icon
	SetIcon(m_hIcon, false);		// Set small icon
	
	return true; // return true unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//	to draw the icon.  For MFC applications using the document/view model,
//	this is automatically done for you by the framework.
void CXMLDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
HCURSOR CXMLDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::OnBrowseClicked() 
{
	CFileDialog FileDialog(true, "*.xml", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		"XML Files (*.xml)|*.xml|All Files (*.*)|*.*||", AfxGetMainWnd());

	if (FileDialog.DoModal() != IDOK)
		return;
	
	m_strFileName = FileDialog.GetPathName();
	UpdateData(false);
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::OnStreamClicked() 
{
	UpdateData();

	m_btnAsync.EnableWindow(!m_bStream);
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::OnLoad() 
{
	CWaitCursor Wait;

	UpdateData();

	HRESULT hr = S_OK;

	HTREEITEM hFirstItem = m_treeOutput.GetChildItem(TVI_ROOT);
	if (hFirstItem)
		m_treeOutput.Expand(hFirstItem, TVE_COLLAPSE);
	m_treeOutput.DeleteAllItems();

	CoInitialize(NULL);

	// Create an empty XML document
	CXMLDocument* pXMLDoc = new CXMLDocument;
	hr = pXMLDoc->RegisterCallback(&MyXMLCallback, (LPARAM)this);

	if (m_strFileName.IsEmpty())
	{
		hr = pXMLDoc->Build(); // load a document from in-memory string.
	}
	else
	{
		if (m_bStream)
			hr = pXMLDoc->LoadStream(m_strFileName);
		else
			hr = pXMLDoc->Load(m_strFileName, !!m_bAsync);
	}

	// Now walk the loaded XML document dumping the name-value pairs
	pXMLDoc->WalkTree(CString(""));

//j	// Test persistence of encoded XML doc in memory
//j	PBYTE pData = NULL;
//j	ULONG ulLen;
//j	hr = pXMLDoc->PersistToMemory(&pData, &ulLen);
//j	hr = pXMLDoc->LoadFromMemory(pData, ulLen);
//j	delete pData;
	
	delete pXMLDoc;
	CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::MyXMLCallback(int iLevel, LPCSTR pNameString, LPCSTR pValueString, LPARAM lParam) 
{
	CXMLDlg* pWnd = (CXMLDlg*)lParam;
	if (!pNameString || !pWnd)
		return;

	static int iLastLevel = -1;
	static HTREEITEM hLastItem;

	// If the tree has been emptied, reinitialize these variables
	if (!pWnd->m_treeOutput.GetCount())
	{
		iLastLevel = -1;
		hLastItem = NULL;
	}

	// Figure out the parent for this new name-value pair
	HTREEITEM hParent = hLastItem;
	for (int i=iLevel; i<=iLastLevel; i++)
		hParent = (hParent ? pWnd->m_treeOutput.GetParentItem(hParent) : NULL);

	// Prepare the name-value string to add to the tree
	CString strOutput;
	strOutput += pNameString;
	if (pValueString && pValueString[0])
	{
		strOutput += "=";
		strOutput += pValueString;
	}

	hLastItem = pWnd->m_treeOutput.InsertItem(strOutput, hParent, TVI_LAST/*hTreeItemInsertAfter*/);

	// Automatically expand new items in the tree for the user's convenience
	//if (hParent && pWnd->m_treeOutput.GetParentItem(hParent))
	//	pWnd->m_treeOutput.Expand(hParent, TVE_EXPAND);

	iLastLevel = iLevel;
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::OnCreateOutput() 
{
	CWaitCursor Wait;

	SCENG_Init();

	// Get the <#document> item
	HTREEITEM pItem = m_treeOutput.GetRootItem();
	if (!pItem)
		return;

	// Get the <?xml version="1.0" encoding="iso-8859-1" ?> item
	pItem = m_treeOutput.GetChildItem(pItem);
	if (!pItem)
		return;

	// Get the <xml> item
	pItem = m_treeOutput.GetNextItem(pItem, TVGN_NEXT);
	if (!pItem)
		return;

	// Get the first <project> item
	pItem = m_treeOutput.GetChildItem(pItem);
	if (!pItem)
		return;

	// Process all <project> items
	while (pItem)
	{
		if (!m_treeOutput.GetItemText(pItem).CompareNoCase(_T("project")))
			CreateNewProject(pItem);

		pItem = m_treeOutput.GetNextItem(pItem, TVGN_NEXT);
	}

	SCENG_Fini();
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::CreateNewProject(HTREEITEM pItem)
{
	AGDOCTYPE ProjectType = GetCurrentDocType(pItem);
	if (ProjectType == DOC_CALENDAR)
		return;

	ASSERT(!m_pAGDoc);

	if (!(m_pAGDoc = new CAGDoc()))
	{
		ASSERT(false);
		return;
	}

	m_pAGDoc->SetDocType(ProjectType);
	m_pAGDoc->SetPortrait(GetCurrentPortrait(pItem));

	CString strFileName = GetCurrentObjectFile(pItem);
	m_szProjectPath = strFileName.Left(strFileName.ReverseFind('\\')+1);
	CString szName = strFileName.Mid(strFileName.ReverseFind('\\')+1);
	m_pAGDoc->SetFileName(CString(szName)); // Filename and extension only; no path

	int nPages = CreatePages(pItem);

	m_pAGDoc->SetCurrentPageNum(0);
	m_pAGDoc->SetModified(false);

	// Create Dir
	CString szDir = _T("D:\\content\\"); // m_szProjectPath;
	CreateDirectory(szDir, NULL);
	szDir += szName.Left(2);
	szDir += '\\';
	CreateDirectory(szDir, NULL);

	// Create File
	// If you want to remove the old extension first, uncomment the line below
	//szName = szName.Left(szName.ReverseFind('.'));
	CString szFile = szDir + szName + CString(_T("ctp"));
	
	// Keep the tree relative current
	bool bSaveOK = SaveDoc(szFile);

	delete m_pAGDoc;
	m_pAGDoc = NULL;
}

/////////////////////////////////////////////////////////////////////////////
int CXMLDlg::CreatePages(HTREEITEM pItem)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return 0;
	}

	int nPages = 0;
	while (pChildItem)
	{
		CString szItemType = m_treeOutput.GetItemText(pChildItem);
		if (!szItemType.CompareNoCase(_T("page")) )
		{
			POINT Size = {0,0};
			GetCurrentObjectSize(pChildItem, Size);
			int iPageWidth = Size.x;
			int iPageHeight = Size.y;

			AGDOCTYPE Type = m_pAGDoc->GetDocType();
			POINT ptSplitOffset = {0,0};
			int iPagesToMake = 1;
			if ((Type == DOC_QUARTERCARD || Type == DOC_HALFCARD || Type == DOC_NOTECARD) && nPages == 1)
				iPagesToMake = 2;
			else
			if (Type == DOC_CALENDAR && (iPageWidth > 11*1800 || iPageHeight > 11*1800))
				iPagesToMake = 2;

			if (iPagesToMake > 1)
			{ // Split the larger dimension
				if (iPageWidth > iPageHeight)
				{
					iPageWidth /= iPagesToMake;
					ptSplitOffset.x = iPageWidth;
				}
				else
				{
					iPageHeight /= iPagesToMake;
					ptSplitOffset.y = iPageHeight;
				}
			}

			for (int i=0; i<iPagesToMake; i++)
			{
				if (Type == DOC_CALENDAR && iPagesToMake > 1)
				{
					// Special case to skip the top part of a calendar cover
					if (nPages == 0 && i == 0)
						continue;
					// Special case to skip the bottom part of a calendar back
					if (nPages == 13 && i == 1)
						continue;
				}
				CAGPage* pPage = new CAGPage(iPageWidth, iPageHeight);
				if (!pPage)
				{
					ASSERT(false);
					return nPages;
				}

				m_pAGDoc->AddPage(pPage);
				CString strPageName = GetPageName(nPages);
				pPage->SetPageName(strPageName);
				for (int j = 0; j < NLAYERS; j++)
					pPage->AddLayer(new CAGLayer());

				POINT ptOffset = {0,0};
				if (i > 0)
					ptOffset = ptSplitOffset;
				HandlePageItems(pChildItem, pPage, ptOffset);
				nPages++;
			}
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}

	return nPages;
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::HandlePageItems(HTREEITEM pItem, CAGPage* pPage, POINT& ptOffset)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return;
	}

	CRect PageRect;
	GetPageRect(pPage, &PageRect);
	PageRect.OffsetRect(ptOffset);

	while (pChildItem)
	{
		CRect ItemRect;
		GetCurrentObjectRect(pChildItem, ItemRect);

		CString szItemType = m_treeOutput.GetItemText(pChildItem);
		if (!szItemType.CompareNoCase(_T("image")) )
		{
			if (Intersect(ItemRect, PageRect))
				CreateImageItem(pChildItem, pPage, ptOffset);
		}
		else
		if (!szItemType.CompareNoCase(_T("text")) ||
			!szItemType.CompareNoCase(_T("date")) )
		{
			if (Intersect(ItemRect, PageRect))
				CreateTextItem(pChildItem, pPage, ptOffset);
		}
		else
		if (!szItemType.CompareNoCase(_T("rectangle")) ||
			!szItemType.CompareNoCase(_T("drawing")) ||
			!szItemType.CompareNoCase(_T("line")) ||
			!szItemType.CompareNoCase(_T("ellipse")) )
		{
			if (Intersect(ItemRect, PageRect))
				CreateGraphicItem(pChildItem, pPage, ptOffset, szItemType);
		}
		else
		if (!szItemType.CompareNoCase(_T("border")) ||
			!szItemType.CompareNoCase(_T("hyperlink")) )
		{
			if (Intersect(ItemRect, PageRect))
				CreateGraphicItem(pChildItem, pPage, ptOffset, szItemType);
		}
		else
		if (!szItemType.CompareNoCase(_T("group")) ||
			!szItemType.CompareNoCase(_T("calendar")) ||
			!szItemType.CompareNoCase(_T("table")) ||
			!szItemType.CompareNoCase(_T("cell")) )
		{
			HandlePageItems(pChildItem, pPage, ptOffset);
		}
		else
		{
			CString szTitle;
			CString szValue;
			Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

			if (!szTitle.CompareNoCase(_T("dx")) )
			{
				long lHeight = INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("dy")) )
			{
				long lWidth = INCHES(atof(szValue));
			}
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CXMLDlg::Intersect(const RECT& ItemRect, const RECT& PageRect)
{
	// In order to avoid using an object on both pages when it only intersects 
	// by a small amount, make sure at least one quarter of the object intersects the page
	RECT ResultRect;
	bool bIntersect = !!::IntersectRect(&ResultRect, &ItemRect, &PageRect);
	if (bIntersect)
	{
		if (CRect(ResultRect).Width() <= CRect(ItemRect).Width()/4)
			bIntersect = false;
	}
	return bIntersect;
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::GetPageRect(CAGPage* pPage, RECT* pRect)
{
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	pRect->bottom = PageSize.cy;
	pRect->right = PageSize.cx;
	pRect->top = 0;
	pRect->left = 0;
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::CreateImageItem(HTREEITEM pItem, CAGPage* pPage, POINT& ptOffset)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return;
	}

	CString strFileName;
	CRect DestRect;
	double fAngle = 0.0;
	while (pChildItem)
	{
		CString szTitle;
		CString szValue;
		Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

		if (!szTitle.CompareNoCase(_T("x")) )
		{
			DestRect.left = INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("y")) )
		{
			DestRect.top = INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("dx")) )
		{
			DestRect.right = DestRect.left + INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("dy")) )
		{
			DestRect.bottom = DestRect.top + INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("angle")) )
		{
			fAngle = atof(szValue);
		}
		else
		if (!szTitle.CompareNoCase(_T("file")) )
		{
			if (szValue.GetAt(1) == ':')
				strFileName = szValue;
			else
				strFileName = m_szProjectPath + szValue;
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}	

	DestRect.OffsetRect(-ptOffset.x, -ptOffset.y);

	if (!strFileName.IsEmpty())
	{
		bool bOK = HandleAddImage(pPage, &DestRect, strFileName, fAngle);
	}
//j	else
//j		ASSERT(false);
}

/////////////////////////////////////////////////////////////////////////////
bool CXMLDlg::HandleAddImage(CAGPage* pPage, RECT* pDestRect, LPCTSTR strFileName, double fAngle)
{
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		char strMsg[256];
		wsprintf(strMsg, "Can't find image file '%s'.  Try again...", strFileName);
		::MessageBox(::GetActiveWindow(), strMsg, "Create & Print", MB_OK);
		hFile = ::CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			wsprintf(strMsg, "Can't find image file '%s'.", strFileName);
			::MessageBox(::GetActiveWindow(), strMsg, "Create & Print", MB_OK);
			return false;
		}
	}
	::CloseHandle(hFile);

	bool bResult = false;

	CString strExtension = strFileName;
	strExtension = strExtension.Right(4);
	strExtension.MakeLower();

	if (strExtension == ".jpg" || strExtension == ".wmf" || strExtension == ".emf")
	{
		CImage Image(strFileName);
		HGLOBAL hMemory = Image.GetNativeImageHandle();
		if (!hMemory)
			return false;

		bResult = AddImage(hMemory, false/*bIsDIB*/, pPage, pDestRect);
	}
	else
	if (strExtension == ".bmp")
	{
		CImage Image(strFileName);
		HGLOBAL hMemory = Image.GetNativeImageHandle();
		if (!hMemory)
			return false;

		bResult = AddImage(hMemory, true/*bIsDIB*/, pPage, pDestRect);
	}
	else
	if (strExtension == ".png")
	{
		HGLOBAL hMemory = CPng::ReadPNG(strFileName);
		if (!hMemory)
			return false;

		bResult = AddImage(hMemory, true/*bIsDIB*/, pPage, pDestRect);
		::GlobalFree(hMemory);
	}
	else
	{ // gif, ico
		CImage Image(strFileName);
		HGLOBAL hMemory = Image.GetDibHandle();
		if (!hMemory)
			return false;

		bResult = AddImage(hMemory, true/*bIsDIB*/, pPage, pDestRect);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
bool CXMLDlg::AddImage(HGLOBAL hMemory, bool bIsDIB, CAGPage* pPage, RECT* pDestRect)
{
	if (!pPage || !pDestRect)
	{
		ASSERT(false);
		return false;
	}

	RECT MaxBounds = {0,0,0,0};
	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	::SetRect(&MaxBounds, 0, 0, PageSize.cx, PageSize.cy);

	CAGSym* pSym = NULL;
	if (bIsDIB)
	{
		char* pMemory = (char*)::GlobalLock(hMemory);
		if (!pMemory)
			return false;

		// do we have a bitmap file header preceding the DIB?
		BITMAPFILEHEADER* pBMPFile = (BITMAPFILEHEADER*)pMemory;
		#define BFT_BITMAP	0x4d42	/* 'BM' */
		if (pBMPFile->bfType == BFT_BITMAP)
			pMemory += sizeof(BITMAPFILEHEADER);

		BITMAPINFOHEADER* pDIB = (BITMAPINFOHEADER*)pMemory;
		if (pDIB->biCompression == BI_RGB)
		{
			CAGSymImage* pSymImage = new CAGSymImage(pDIB, MaxBounds);
			pSym = pSymImage;
		}

		::GlobalUnlock(hMemory);
	}
	else
	{
		CAGSymImage* pSymImage = new CAGSymImage(hMemory, MaxBounds);
		pSym = pSymImage;
	}

	if (!pSym)
	{
		ASSERT(false);
		return false;
	}
		
	pSym->SetDestRect(*pDestRect);

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (pActiveLayer)
		pActiveLayer->AddSymbol(pSym);
	else
		ASSERT(false);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::CreateTextItem(HTREEITEM pItem, CAGPage* pPage, POINT& ptOffset)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return;
	}

	// Initialize the text properties: Color, Valign, HAlign, Angle, Font
	COLORREF Color = RGB(0,0,0);
	eTSJustTypes HAlignment = eRagCentered;
	eVertJust VAlignment = eVertTop;
	LOGFONT Font;
	memset(&Font, 0, sizeof(Font));
	Font.lfWeight = FW_NORMAL;
	Font.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
	Font.lfItalic = false;
	Font.lfUnderline = false;
	Font.lfStrikeOut = false;
	lstrcpy(Font.lfFaceName, "Arial");
	Font.lfHeight = -POINTUNITS(14);

	#define MAX_SPECS 1000
	int pCharOffsets[MAX_SPECS];
	CAGSpec* pSpecs[MAX_SPECS];
	int nSpecs = 0;

	char* pTextBuffer = new char[10000];
	if (!pTextBuffer)
	{
		ASSERT(false);
		return;
	}
	*pTextBuffer = '\0';

	CRect DestRect;
	double fAngle = 0.0;
	while (pChildItem)
	{
		CString szItemType = m_treeOutput.GetItemText(pChildItem);
		if (!szItemType.CompareNoCase(_T("style")) )
		{
			int iOffset = lstrlen(pTextBuffer);
			CAGSpec* pSpec = HandleAddText(pChildItem, Font, Color, HAlignment, (BYTE*)pTextBuffer + iOffset);
			if (pSpec)
			{
				pSpecs[nSpecs] = pSpec;
				pCharOffsets[nSpecs] = iOffset;
				nSpecs++;
			}
		}
		else
		{
			CString szTitle;
			CString szValue;
			Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

			if (!szTitle.CompareNoCase(_T("x")) )
			{
				DestRect.left = INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("y")) )
			{
				DestRect.top = INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("dx")) )
			{
				DestRect.right = DestRect.left + INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("dy")) )
			{
				DestRect.bottom = DestRect.top + INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("angle")) )
			{
				fAngle = atof(szValue);
			}
			else
			if (!szTitle.CompareNoCase(_T("valign")) )
			{
				if (!szValue.CompareNoCase(_T("Top")))
					VAlignment = eVertTop;
				else
				if (!szValue.CompareNoCase(_T("Middle")))
					VAlignment = eVertCentered;
				else
				if (!szValue.CompareNoCase(_T("Bottom")))
					VAlignment = eVertBottom;

			}
		}
		
		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}
	
	if (!nSpecs)
	{
		delete [] pTextBuffer;
		return;
	}

	DestRect.OffsetRect(-ptOffset.x, -ptOffset.y);

	CAGSymText* pText = new CAGSymText(ST_TEXT);
	if (pText)
	{
		pText->Create(DestRect);
		pText->SetVertJust(VAlignment);
		if (fAngle)
		{
			CAGMatrix Matrix;
			Matrix.Rotate(-fAngle, -fAngle, ((DestRect.left + DestRect.right) / 2), ((DestRect.top + DestRect.bottom) / 2));
			pText->SetMatrix(Matrix);
		}

		// Set the text and specs
		pText->SetText(pTextBuffer, nSpecs, pSpecs, pCharOffsets);

		// Add the text symbol to the page/layer
		CAGLayer* pActiveLayer = pPage->GetActiveLayer();
		if (pActiveLayer)
			pActiveLayer->AddSymbol(pText);	
	}
	else
		ASSERT(false);

	delete [] pTextBuffer;
}

/////////////////////////////////////////////////////////////////////////////
CAGSpec* CXMLDlg::HandleAddText(HTREEITEM pItem, LOGFONT& Font, COLORREF& Color, eTSJustTypes& HAlignment, BYTE* pTextBuffer)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return NULL;
	}

	while (pChildItem)
	{
		CString szTitle;
		CString szValue;
		Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

		if (!szTitle.CompareNoCase(_T("align")) )
		{
			if (!szValue.CompareNoCase(_T("Left")))
				HAlignment = eFlushLeft;
			else
			if (!szValue.CompareNoCase(_T("Center")))
				HAlignment = eRagCentered;
			else
			if (!szValue.CompareNoCase(_T("Right")))
				HAlignment = eFlushRight;
			else
			if (!szValue.CompareNoCase(_T("Justify")))
				HAlignment = eRagJustified;
		}
		else
		if (!szTitle.CompareNoCase(_T("face")) )
		{
			lstrcpy(Font.lfFaceName, szValue);
		}
		else
		if (!szTitle.CompareNoCase(_T("size")) )
		{
			Font.lfHeight = -POINTUNITS(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("color")) )
		{
			Color = ParseColor(szValue);
		}
		else
		if (!szTitle.CompareNoCase(_T("b")) )
		{
			Font.lfWeight = (!szValue.CompareNoCase(_T("on")) ? FW_BOLD : FW_NORMAL);
		}
		else
		if (!szTitle.CompareNoCase(_T("i")) )
		{
			Font.lfItalic = !szValue.CompareNoCase(_T("on"));
		}
		else
		if (!szTitle.CompareNoCase(_T("u")) )
		{
			Font.lfUnderline = !szValue.CompareNoCase(_T("on"));
		}
		else
		if (!szTitle.CompareNoCase(_T("t")) )
		{
			for (int i=0; i<szValue.GetLength(); i++)
			{
				char c = szValue.GetAt(i);
				if (c == '\\')
				{
					c = szValue.GetAt(++i);
					if (c == 't')
						c = '\t';
					else
					if (c == 'n')
						c = '\n';
					else
					if (c == 'r')
						c = '\r';
					else
					{
						c = '\\';
						i--;
					}
				}

				*pTextBuffer++ = (char)c;
				*pTextBuffer = '\0';
			}
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}

	CAGSpec* pSpec = new CAGSpec(
		Font,			// LOGFONT& Font
		HAlignment,		// eTSJust HorzJust
		0,				// short sLineLeadPct
		LT_None,		// int LineWidth
		RGB(0,0,0),		// COLORREF LineColor
		FT_Solid,		// FillType Fill
		Color,			// COLORREF FillColor
		RGB(0,0,0))		// COLORREF FillColor2
		;
	ASSERT(pSpec);
	return pSpec;
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::CreateGraphicItem(HTREEITEM pItem, CAGPage* pPage, POINT& ptOffset, CString& szParentItemType)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return;
	}

	COLORREF LineColor = RGB(0,0,0);
	int iLineWidth = LT_None;
	FillType tFillType = FT_None;
	COLORREF FillColor = RGB(0,0,0);
	COLORREF FillColor2 = RGB(0,0,0);
	LineStart tStart = LS_LeftTop;

	POINT* pPoints = new POINT[5000];
	int nPoints = 0;

	CRect DestRect;
	double fAngle = 0.0;
	while (pChildItem)
	{
		CString szItemType = m_treeOutput.GetItemText(pChildItem);
		if (!szItemType.CompareNoCase(_T("pt")) )
		{
			POINT Location = {0,0};
			GetCurrentObjectLocation(pChildItem, Location);
			pPoints[nPoints++] = Location;
		}
		else
		{
			CString szTitle;
			CString szValue;
			Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

			if (!szTitle.CompareNoCase(_T("x")) )
			{
				DestRect.left = INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("y")) )
			{
				DestRect.top = INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("dx")) )
			{
				DestRect.right = DestRect.left + INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("dy")) )
			{
				DestRect.bottom = DestRect.top + INCHES(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("angle")) )
			{
				fAngle = atof(szValue);
			}
			else
			if (!szTitle.CompareNoCase(_T("filltype")) )
			{
				if (!szValue.CompareNoCase(_T("Solid")))
					tFillType = FT_Solid;
				else
				if (!szValue.CompareNoCase(_T("SweepRight")))
					tFillType = FT_SweepRight;
				else
				if (!szValue.CompareNoCase(_T("SweepDown")))
					tFillType = FT_SweepDown;
				else
				if (!szValue.CompareNoCase(_T("RadialCenter")))
					tFillType = FT_RadialCenter;
				else
				if (!szValue.CompareNoCase(_T("RadialCorner")))
					tFillType = FT_RadialCorner;
			}
			else
			if (!szTitle.CompareNoCase(_T("fillcolor")) )
			{
				FillColor = ParseColor(szValue);
			}
			else
			if (!szTitle.CompareNoCase(_T("fillblendcolor")) )
			{
				FillColor2 = ParseColor(szValue);
			}
			else
			if (!szTitle.CompareNoCase(_T("linetype")) )
			{
				if (!szValue.CompareNoCase(_T("Hairline")))
					iLineWidth = LT_Hairline;
				else
				if (!szValue.CompareNoCase(_T("Normal")))
					iLineWidth = POINTUNITS(1);
			}
			else
			if (!szTitle.CompareNoCase(_T("linecolor")) )
			{
				LineColor = ParseColor(szValue);
			}
			else
			if (!szTitle.CompareNoCase(_T("linewidth")) )
			{
				iLineWidth = POINTUNITS(atof(szValue));
			}
			else
			if (!szTitle.CompareNoCase(_T("start")) )
			{
				if (!szValue.CompareNoCase(_T("UpperLeft")))
					tStart = LS_LeftTop;
				else
				if (!szValue.CompareNoCase(_T("UpperRight")))
					tStart = LS_RightTop;
				else
				if (!szValue.CompareNoCase(_T("LowerRight")))
					tStart = LS_RightBottom;
				else
				if (!szValue.CompareNoCase(_T("LowerLeft")))
					tStart = LS_LeftBottom;
			}
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}	

	CAGSymGraphic* pSym = NULL;

	if (!szParentItemType.CompareNoCase(_T("rectangle")))
	{
		CAGSymRectangle* pSym1 = new CAGSymRectangle();
		pSym = pSym1;
	}
	else
	if (!szParentItemType.CompareNoCase(_T("ellipse")))
	{
		CAGSymEllipse* pSym1 = new CAGSymEllipse();
		pSym = pSym1;
	}
	else
	if (!szParentItemType.CompareNoCase(_T("drawing")))
	{
		CAGSymDrawing* pSym1 = new CAGSymDrawing();

		// Due to a conversion bug, flip the y value of each point within the drawing
		int iOffset = DestRect.bottom + DestRect.top;
		for (int i = 0; i < nPoints; i++)
			pPoints[i].y = iOffset - pPoints[i].y;

		pSym1->SetPoints(pPoints, nPoints);
		pSym = pSym1;
	}
	else
	if (!szParentItemType.CompareNoCase(_T("line")))
	{
		CAGSymLine* pSym1 = new CAGSymLine();

		pSym1->SetLineStart(tStart);
		pSym = pSym1;
	}
	else
	if (!szParentItemType.CompareNoCase(_T("border")))
	{
		CAGSymRectangle* pSym1 = new CAGSymRectangle();
		pSym = pSym1;
		// Ignore everything because the data is bad; make it a black, 5 point, unfilled rectangle
		LineColor = RGB(0,0,0);
		iLineWidth = POINTUNITS(5);
		tFillType = FT_None;
		FillColor = RGB(0,0,0);
		FillColor2 = RGB(0,0,0);
	}
	else
	if (!szParentItemType.CompareNoCase(_T("hyperlink")))
	{
	}

	delete [] pPoints;

	if (!pSym)
	{
		ASSERT(false);
		return;
	}

	DestRect.OffsetRect(-ptOffset.x, -ptOffset.y);

	pSym->SetLineColor(LineColor);
	pSym->SetLineWidth(iLineWidth);

	pSym->SetFillType(tFillType);
	pSym->SetFillColor(FillColor);
	pSym->SetFillColor2(FillColor2);

	pSym->SetDestRect(DestRect);
	if (fAngle)
	{
		CAGMatrix Matrix;
		Matrix.Rotate(-fAngle, -fAngle, ((DestRect.left + DestRect.right) / 2), ((DestRect.top + DestRect.bottom) / 2));
		pSym->SetMatrix(Matrix);
	}

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (pActiveLayer)
		pActiveLayer->AddSymbol(pSym);
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CXMLDlg::ParseColor(CString& strColor)
{
	int nComma = strColor.Find('x');
	int nComma1 = strColor.Find(',');
	int nComma2 = strColor.ReverseFind(',');
	if (nComma1 < 0 || nComma2 < 0)
	{
		ASSERT(false);
		return RGB(0,0,0);
	}

	CString szR = strColor.Left(nComma1);
	CString szG = strColor.Mid(nComma1+1, nComma2-nComma1-1);
	CString szB = strColor.Mid(nComma2+1);

	int r = atoi(szR);
	int g = atoi(szG);
	int b = atoi(szB);
	
	return RGB(r,g,b);
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::GetCurrentObjectLocation(HTREEITEM pItem, POINT& Location)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return;
	}

	while (pChildItem)
	{
		CString szTitle;
		CString szValue;
		Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

		if (!szTitle.CompareNoCase(_T("x")) )
		{
			Location.x = INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("y")) )
		{
			Location.y = INCHES(atof(szValue));
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::GetCurrentObjectSize(HTREEITEM pItem, POINT& Size)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return;
	}

	while (pChildItem)
	{
		CString szTitle;
		CString szValue;
		Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

		if (!szTitle.CompareNoCase(_T("dx")))
		{
			Size.x = INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("dy")))
		{
			Size.y = INCHES(atof(szValue));
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::GetCurrentObjectRect(HTREEITEM pItem, RECT& Rect)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
		return;

	while (pChildItem)
	{
		CString szTitle;
		CString szValue;
		Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

		if (!szTitle.CompareNoCase(_T("x")) )
		{
			Rect.left = INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("y")) )
		{
			Rect.top = INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("dx")) )
		{
			Rect.right = Rect.left + INCHES(atof(szValue));
		}
		else
		if (!szTitle.CompareNoCase(_T("dy")) )
		{
			Rect.bottom = Rect.top + INCHES(atof(szValue));
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}
}

/////////////////////////////////////////////////////////////////////////////
CString CXMLDlg::GetCurrentObjectFile(HTREEITEM pItem)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return CString("");
	}

	while (pChildItem)
	{
		CString szTitle;
		CString szValue;
		Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

		if (!szTitle.CompareNoCase(_T("file")) )
			return szValue;

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}

	ASSERT(false);
	return CString("");
}

/////////////////////////////////////////////////////////////////////////////
bool CXMLDlg::GetCurrentPortrait(HTREEITEM pItem)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return false;
	}

	while (pChildItem)
	{
		CString szTitle;
		CString szValue;
		Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

		if (!szTitle.CompareNoCase(_T("orient")))
		{
			return !szValue.CompareNoCase(_T("portrait"));
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}

	ASSERT(false);
	return false;
}

/////////////////////////////////////////////////////////////////////////////
AGDOCTYPE CXMLDlg::GetCurrentDocType(HTREEITEM pItem)
{
	HTREEITEM pChildItem = m_treeOutput.GetChildItem(pItem);
	if (!pChildItem)
	{
		ASSERT(false);
		return DOC_DEFAULT;
	}

	while (pChildItem)
	{
		CString szTitle;
		CString szValue;
		Separate(m_treeOutput.GetItemText(pChildItem), szTitle, szValue);

		if (!szTitle.CompareNoCase(_T("type")))
		{
			return (ConvertType(szValue));
		}

		pChildItem = m_treeOutput.GetNextItem(pChildItem, TVGN_NEXT);
	}

	ASSERT(false);
	return DOC_DEFAULT;
}

/////////////////////////////////////////////////////////////////////////////
AGDOCTYPE CXMLDlg::ConvertType(const CString& szType)
{
	AGDOCTYPE DocType = DOC_BLANKPAGE;

	if (!szType.CompareNoCase(_T("Card")) ||
		!szType.CompareNoCase(_T("QuarterCard")))
		DocType = DOC_QUARTERCARD;
	else
	if (!szType.CompareNoCase(_T("HalfCard")))
		DocType = DOC_HALFCARD;	
	else
	if (!szType.CompareNoCase(_T("Banner")))
		DocType = DOC_BANNER;	
	else
	if (!szType.CompareNoCase(_T("BlankPage")))
		DocType = DOC_BLANKPAGE;
	else
	if (!szType.CompareNoCase(_T("Brochure")))
		DocType = DOC_BROCHURE;	
	else
	if (!szType.CompareNoCase(_T("BusinessCard"))) 
		DocType = DOC_BUSINESSCARD;
	else
	if (!szType.CompareNoCase(_T("Calendar")))
		DocType = DOC_CALENDAR;	
	else
	if (!szType.CompareNoCase(_T("CdLabel")))
		DocType = DOC_CDLABEL;	
	else
	if (!szType.CompareNoCase(_T("Certificate")))
		DocType = DOC_CERTIFICATE;
	else
	if (!szType.CompareNoCase(_T("Craft"))) 
		DocType = DOC_CRAFT;	
	else
	if (!szType.CompareNoCase(_T("Envelope")))
		DocType = DOC_ENVELOPE;	
	else
	if (!szType.CompareNoCase(_T("FaxCover")))
		DocType = DOC_FAXCOVER;	
	else
	if (!szType.CompareNoCase(_T("Label"))) 
		DocType = DOC_LABEL;	
	else
	if (!szType.CompareNoCase(_T("NoteCard")))
		DocType = DOC_NOTECARD;	
	else
	if (!szType.CompareNoCase(_T("Newsletter")))
		DocType = DOC_NEWSLETTER;
	else
	if (!szType.CompareNoCase(_T("Poster")))
		DocType = DOC_POSTER;	
	else
	if (!szType.CompareNoCase(_T("PhotoProjects")))
		DocType = DOC_PHOTOS;	
	else
	if (!szType.CompareNoCase(_T("PostCard")))
		DocType = DOC_POSTCARD;	
	else
	if (!szType.CompareNoCase(_T("Stationery")))
		DocType = DOC_STATIONERY;
	else
	if (!szType.CompareNoCase(_T("Sticker")))
		DocType = DOC_STICKER;	
	else
	if (!szType.CompareNoCase(_T("IronOn")))
		DocType = DOC_IRONON;	
	else
	if (!szType.CompareNoCase(_T("Trifold")))
		DocType = DOC_TRIFOLD;
	else
	if (!szType.CompareNoCase(_T("GiftNameCard"))) 
		DocType = DOC_GIFTNAMECARD;
	else
	if (!szType.CompareNoCase(_T("CdBooklet")))
		DocType = DOC_CDBOOKLET;
	else
	if (!szType.CompareNoCase(_T("WebPub")))
		DocType = DOC_WEBPUB;
	else
		ASSERT(false);

	return DocType;
}

/////////////////////////////////////////////////////////////////////////////
bool CXMLDlg::SaveDoc(LPCTSTR strFileName)
{
	if (!m_pAGDoc)
	{
		ASSERT(false);
		return false;
	}

	ofstream output(strFileName, ios::out | ios::binary);

	if (!m_pAGDoc->Write(output))
	{
		ASSERT(false);
		return false;
	}		
	
	return true;
}

//////////////////////////////////////////////////////////////////////
CString CXMLDlg::GetPageName(int nPage)
{
	AGDOCTYPE Type = m_pAGDoc->GetDocType();
	bool bIsPortrait = m_pAGDoc->IsPortrait();

	// Convert from 0-based to 1-based
	nPage++;

	CString strPageName;
	switch (Type)
	{
		case DOC_BANNER:
		{
			strPageName = "Banner %d";
			break;
		}
		case DOC_BLANKPAGE: 
		{
			strPageName = "Page %d";
			break;
		}
		case DOC_BUSINESSCARD:
		{
			strPageName = "Business Card %d";
			break;
		}
		case DOC_BROCHURE:
		{
			if (nPage == 1)
				strPageName = "Brochure Front";
			else
			if (nPage == 2)
				strPageName = "Brochure Back";
			break;
		}
		case DOC_CALENDAR:
		{
			strPageName = "Calendar %d";
#ifdef NOTUSED //j
			if (nPage == 1)
				strPageName = "Calendar Cover";
			else
			if (nPage >= 26)
				strPageName = "Calendar Back";
			else
			{
				static const LPCSTR Months[] = { "", 
					"Jan ", "Feb ", "Mar ", "Apr ", "May ", "Jun ",
					"Jul ", "Aug ", "Sep ", "Oct ", "Nov ", "Dec " };
				bool bOdd = (nPage & 1);
				nPage = (nPage / 2);
				if (nPage > 12) nPage = 12;
				strPageName = CString(Months[nPage]);
				if (!bOdd)
					strPageName += (bIsPortrait ? "Left" : "Top");
				else
					strPageName += (bIsPortrait ? "Right" : "Bottom");
			}
#endif NOTUSED //j
			break;
		}
		case DOC_CDLABEL:
		{
			strPageName = "CD Label %d";
			break;
		}
		case DOC_QUARTERCARD:
		case DOC_HALFCARD:
		case DOC_CDBOOKLET:
		case DOC_NOTECARD:
		{
			if (nPage == 1)
				strPageName = "Front";
			else
			if (nPage == 2)
			{
				strPageName = "Inside ";
				strPageName += (bIsPortrait ? "Left" : "Top");
			}
			else
			if (nPage == 3)
			{
				strPageName = "Inside ";
				strPageName += (bIsPortrait ? "Right" : "Bottom");
			}
			else
			if (nPage == 4)
				strPageName = "Back";
			break;
		}
		case DOC_CERTIFICATE:
		{
			strPageName = "Certificate %d";
			break;
		}
		case DOC_CRAFT: 
		{
			strPageName = "Craft %d";
			break;
		}
		case DOC_ENVELOPE:
		{
			strPageName = "Envelope %d";
			break;
		}
		case DOC_FAXCOVER:
		{
			strPageName = "Fax Cover %d";
			break;
		}
		case DOC_GIFTNAMECARD:
		{
			strPageName = "Gift/Name Card %d";
			break;
		}
		case DOC_LABEL: 
		{
			strPageName = "Label %d";
			break;
		}
		case DOC_NEWSLETTER:
		{
			if (nPage == 1)
				strPageName = "Newsletter Cover";
			else
				strPageName = "Page %d";
			break;
		}
		case DOC_PHOTOS:
		{
			strPageName = "Photo Layout %d";
			break;
		}
		case DOC_POSTCARD:
		{
			if (nPage == 1)
				strPageName = "Postcard Front";
			else
			if (nPage == 2)
				strPageName = "Postcard Back";
			else
				strPageName = "Page %d";
			break;
			break;
		}
		case DOC_POSTER:
		{
			strPageName = "Poster %d";
			break;
		}
		case DOC_STATIONERY:
		{
			strPageName = "Stationery %d";
			break;
		}
		case DOC_STICKER:
		{
			strPageName = "Sticker %d";
			break;
		}
		case DOC_IRONON:
		{
			strPageName = "Iron-on Transfer %d";
			break;
		}
		case DOC_WEBPUB:
		{
			strPageName = "Web Page %d";
			break;
		}
		default:
		{
			strPageName = "Page %d";
			break;
		}
	}

	CString strOutput;
	strOutput.Format(strPageName, nPage);
	return strOutput;
}

/////////////////////////////////////////////////////////////////////////////
void CXMLDlg::Separate(const CString& szItemType, CString& szTitle, CString& szValue)
{
	szTitle = szItemType.SpanExcluding(_T("="));
	szTitle.TrimLeft();
	szTitle.TrimRight();
	szValue = szItemType.Mid(szItemType.Find('=')+1);
}
