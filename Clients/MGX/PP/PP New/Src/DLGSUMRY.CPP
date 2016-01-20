// info.cpp : implementation file
//

#include "pp.h"
#include "dlgsumry.h"
#include "ppafx.h"
#include "svrview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSummaryStreamPage property page

IMPLEMENT_DYNCREATE(CSummaryStreamPage, CPPPropertyPage)
 
CSummaryStreamPage::CSummaryStreamPage() : 
    CPPPropertyPage(CSummaryStreamPage::IDD)
{
	//{{AFX_DATA_INIT(CSummaryStreamPage)
		// NOTE: the ClassWizard will add member initialization here
	m_SummAuthor = _T("");
	m_SummComments = _T("");
	m_SummKeywords = _T("");
	m_SummSubject = _T("");
	m_SummTitle = _T("");
    m_SummLastAuthor = _T("");
	//}}AFX_DATA_INIT
}

CSummaryStreamPage::~CSummaryStreamPage()
{
	if (PageEverActive()) // have we ever used this page
	{
	}
}

void CSummaryStreamPage::DoDataExchange(CDataExchange* pDX)
{
	CPPPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSummaryStreamPage)
	DDX_Text(pDX, IDC_SUMM_AUTHOR, m_SummAuthor);
	DDX_Text(pDX, IDC_SUMM_LASTAUTHOR, m_SummLastAuthor);
	DDX_Text(pDX, IDC_SUMM_COMMENTS, m_SummComments);
	DDX_Text(pDX, IDC_SUMM_KEYWORDS, m_SummKeywords);
	DDX_Text(pDX, IDC_SUMM_SUBJECT, m_SummSubject);
	DDX_Text(pDX, IDC_SUMM_TITLE, m_SummTitle);
	DDX_Text(pDX, IDC_SUMM_CREATED, m_SummCreated);
	DDX_Text(pDX, IDC_SUMM_LASTSAVED, m_SummLastSaved);
//	DDX_Text(pDX, IDC_SUMM_LASTPRINT, m_SummLastPrinted);
    //}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
// CSummaryStreamPage message handlers


BOOL CSummaryStreamPage::OnInitDialog() 
{
 	CPPPropertyPage::OnInitDialog();

    BOOL bResult = FALSE;
	// TODO: Add extra initialization here
    CSummaryPropSheet *pParentSheet = (CSummaryPropSheet*)GetParent();
    if (pParentSheet)
    {
        CServerDoc *pDocument = pParentSheet->GetDocument();
        if (pDocument)
        {
            m_SummAuthor = 	pDocument->m_SummaryInfo.GetAuthor();
            m_SummLastAuthor = pDocument->m_SummaryInfo.GetLastAuthor();
          	m_SummComments = pDocument->m_SummaryInfo.GetComments();
        	m_SummKeywords = pDocument->m_SummaryInfo.GetKeywords();
        	m_SummSubject = pDocument->m_SummaryInfo.GetSubject();
        	m_SummTitle = pDocument->m_SummaryInfo.GetTitle();
            m_SummCreated = pDocument->m_SummaryInfo.GetCreated();
            m_SummLastSaved = pDocument->m_SummaryInfo.GetLastSaved();
//            m_SummLastPrinted = pDocument->m_SummaryInfo.GetLastPrinted();

            UpdateData(FALSE);
            bResult = TRUE;
        }
    }

    return bResult;

//	return TRUE;  // return TRUE unless you set the focus to a control
//	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSummaryStreamPage::HandleOK()
{
    // TODO handle the okay
    CSummaryPropSheet *pParentSheet = (CSummaryPropSheet*)GetParent();
    if (pParentSheet)
    {
        CServerDoc *pDocument = pParentSheet->GetDocument();
        if (pDocument)
        {
        	pDocument->m_SummaryInfo.SetAuthor(m_SummAuthor);
        	pDocument->m_SummaryInfo.SetComments(m_SummComments);
        	pDocument->m_SummaryInfo.SetKeywords(m_SummKeywords);
        	pDocument->m_SummaryInfo.SetSubject(m_SummSubject);
        	pDocument->m_SummaryInfo.SetTitle(m_SummTitle);
        }
    }

	CPPPropertyPage::HandleOK();
}
   
void CSummaryStreamPage::HandleCancel()
{
    // TODO handle the cancel here

	CPPPropertyPage::HandleCancel();
}

void CSummaryStreamPage::OnChangeSummary() 
{
	// TODO: Add your control notification handler code here
    SetModified(FALSE,TRUE);
}
   
/////////////////////////////////////////////////////////////////////////////
// CGeneralSummaryPage property page

IMPLEMENT_DYNCREATE(CGeneralSummaryPage, CPPPropertyPage)

CGeneralSummaryPage::CGeneralSummaryPage() : CPPPropertyPage(CGeneralSummaryPage::IDD)
{
	//{{AFX_DATA_INIT(CGeneralSummaryPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGeneralSummaryPage::~CGeneralSummaryPage()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGeneralSummaryPage message handlers
BOOL CGeneralSummaryPage::OnInitDialog() 
{
	CPPPropertyPage::OnInitDialog();

	// TODO init the dialog here
    LPSTR lpString, lpUnit;
    LPOBJECT lpObject;
    char szString[256], szValue[256], szFormat[256];
    int iType, nEdit, nUndo;
    long lSize, lFileSize, lEdit, lUndo, lObjects;
    long l;
    LPMASK lpMask;
    LPFRAME	lpFrame;
    LPIMAGE lpImage = NULL;
	FRMTYPEINFO TypeInfo;
    char szTitle[MAX_STR_LEN];
	ITEMID idDataType;
	HWND hDlg = GetSafeHwnd();
    CSummaryPropSheet *pParentSheet = (CSummaryPropSheet*)GetParent();

    if (pParentSheet)
        lpImage = pParentSheet->GetDocument()->m_lpImage;
    if (!lpImage)
    	return(TRUE);

    lpFrame = ImgGetBaseEditFrame(lpImage);
    lstrcpy(szTitle, lpImage->CurFile );
    if (Control.WinVer == WV_WINDOWS95)
        stripext(szTitle);
    SetDlgItemText(IDC_INFOFILENAME, szTitle);
    iType = IDI_TIFF + (lpImage->FileType - IDN_TIFF);
    if (AstralStr(iType, &lpString))
	    SetDlgItemText(IDC_INFOFILETYPE, lpString);

	idDataType = lpImage->DataType;
	lpImage->GetTypeInfo(&TypeInfo);
	if ((TypeInfo.DataType == FDT_PALETTECOLOR) && (TypeInfo.ColorMap->NumEntries <= 16))
		idDataType = IDC_SAVE4BITCOLOR;
    if (AstralStr(idDataType, &lpString))
	    SetDlgItemText(IDC_INFODATATYPE, lpString);

    if (AstralStr(ImgChanged(lpImage) ? IDS_YES : IDS_NO, &lpString))
    	SetDlgItemText(IDC_INFOMODIFIED, lpString);
    if (AstralStr(IDS_INFOSIZE, &lpString))
	{
	    int OldUnits = Control.Units;

    	if (Control.Units == IDC_PREF_UNITPIXELS)
		{
	    	Control.Units = IDC_PREF_UNITINCHES;
    		SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), 
                Control.UnitRes, Control.Points);
		}

	    lstrcpy(szFormat, lpString);
    	if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES,
	        &lpUnit ) )
		{
    		Unit2String(FGET(FrameXSize(lpFrame), FrameResolution(lpFrame)),
	    		szValue);
    		wsprintf(szString, szFormat, FrameXSize(lpFrame),
    			(LPSTR)szValue, lpUnit);
    		SetDlgItemText(IDC_INFOWIDTH, szString);
    		Unit2String(FGET(FrameYSize(lpFrame), FrameResolution(lpFrame)),
    			szValue);
    		wsprintf(szString, szFormat, FrameYSize(lpFrame),
    			(LPSTR)szValue, lpUnit);
    		SetDlgItemText(IDC_INFOHEIGHT, szString);
	    }

    	Control.Units = OldUnits;
    	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	}
    SetDlgItemInt(IDC_INFORESOLUTION, FrameResolution(lpFrame), NO);
    lObjects = 0;
    lpObject = ImgGetBase(lpImage);
    while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
    	lpFrame = ObjGetEditFrame(lpObject);
    	if (lpFrame)
    		lObjects += (long)FrameByteWidth(lpFrame) * (long)FrameYSize(lpFrame);
    	if (lpObject->lpAlpha)
		{
    		lpFrame = lpObject->lpAlpha->Pixmap.EditFrame;
    		if (lpFrame)
    			lObjects += (long)FrameByteWidth(lpFrame) * (long)FrameYSize(lpFrame);
		}
	}
    if (AstralStr(IDS_INFOIMAGEMEMORY, &lpString))
	{
    	lpFrame = ImgGetBaseEditFrame(lpImage);
    	lstrcpy(szFormat, lpString);
    	l = (long)FrameByteWidth(lpFrame) * (long)FrameYSize(lpFrame);
    	l += lObjects; 
    	lSize = (l + 512L) / 1024L;
	    lFileSize = (FileSize( lpImage->CurFile ) + 512L) / 1024L;
    	wsprintf( szString, szFormat, lSize, lFileSize );
    	SetDlgItemText(IDC_INFOIMAGEMEMORY, szString);
	}

    lEdit = lUndo = 0;
    nEdit = nUndo = 0;
    lpObject = NULL;
    while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
    	if (lpFrame = ObjGetEditFrame(lpObject))
		{
    		lEdit += (long)FrameInMemory(lpFrame);
    		++nEdit;
		}
    	if (lpFrame = ObjGetUndoFrame(lpObject))
		{
    		lUndo += (long)FrameInMemory(lpFrame);
    		++nUndo;
		}
    	if (lpObject->lpAlpha &&
    		(lpFrame = lpObject->lpAlpha->Pixmap.EditFrame))
		{
    		lEdit += (long)FrameInMemory(lpFrame);
    		++nEdit;
		}
    	if (lpObject->lpAlpha &&
    		(lpFrame = lpObject->lpAlpha->Pixmap.UndoFrame))
		{
    		lUndo += (long)FrameInMemory(lpFrame);
    		++nUndo;
		}
	}
    if (nEdit)
    	lEdit /= nEdit;
    if (nUndo)
    	lUndo /= nUndo;
    if (AstralStr(IDS_INFOEDITPERCENT, &lpString))
	{
    	lstrcpy(szFormat, lpString);
    	wsprintf( szString, szFormat, (int)lEdit );
    	SetDlgItemText(IDC_INFOEDITPERCENT, szString);
	}
    if (AstralStr(IDS_INFOUNDOPERCENT, &lpString))
	{
    	lstrcpy(szFormat, lpString);
    	wsprintf( szString, szFormat, (int)lUndo );
    	SetDlgItemText(IDC_INFOUNDOPERCENT, szString);
	}
    lEdit = lUndo = 0;
    if (lpMask = ImgGetMask(lpImage))
	{
    	if (lpMask->Pixmap.EditFrame)
    		lEdit += FrameInMemory(lpMask->Pixmap.EditFrame);
	    if (lpMask->Pixmap.UndoFrame)
    		lUndo += FrameInMemory(lpMask->Pixmap.UndoFrame);
    }
    if (AstralStr(IDS_INFOMASKEDITPERCENT, &lpString))
	{
    	lstrcpy(szFormat, lpString);
    	wsprintf( szString, szFormat, (int)lEdit );
    	SetDlgItemText(IDC_INFOMASKEDITPERCENT, szString);
	}
    if (AstralStr(IDS_INFOMASKUNDOPERCENT, &lpString))
	{
    	lstrcpy(szFormat, lpString);
    	wsprintf( szString, szFormat, (int)lUndo );
    	SetDlgItemText(IDC_INFOMASKUNDOPERCENT, szString);
	}
    if (AstralStr(IDS_INFOMEMORY, &lpString))
	{
    	lstrcpy(szFormat, lpString);
    	l = (AvailableMemory()+512) / 1024L;
    	lpString = szString;
    	wsprintf( lpString, szFormat, l );
    	SetDlgItemText(IDC_INFOMEMORY, szString);
	}
    SetDlgItemInt(IDC_INFONUMOBJECTS, ImgCountObjects(lpImage)-1, NO);
    if (AstralStr(IDS_INFOMEMORY, &lpString))
	{
    	lstrcpy(szFormat, lpString);
    	lObjects = (lObjects + 512L) / 1024L;
    	wsprintf(szString, szFormat, lObjects);
    	SetDlgItemText(IDC_INFOOBJECTSMEMORY, szString);
	}
    SetDlgItemText( IDC_CMS_SRCNAME,
        CmsGetPTName( ImgGetBaseEditFrame(lpImage), szString ) );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGeneralSummaryPage::HandleOK()
{
    // TODO handle the okay

	CPPPropertyPage::HandleOK();
}

void CGeneralSummaryPage::HandleCancel()
{
    //TODO handle the cancel here

	CPPPropertyPage::HandleCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CSummaryPropSheet

IMPLEMENT_DYNAMIC(CSummaryPropSheet, CPPPropertySheet)

int CSummaryPropSheet::m_nLastPage = 0;

CSummaryPropSheet::CSummaryPropSheet(CServerDoc *pDocument,UINT nIDCaption, 
    CWnd* pParentWnd, UINT nIDHelp)	:
    CPPPropertySheet(nIDCaption, pParentWnd, nIDHelp, GetSelectPage())
{
    BOOL bTempStg;
    ASSERT(pDocument);
    m_pDocument = pDocument;
    m_bHasSummaryStream = (m_pDocument->GetRootCompoundStorage(&bTempStg) && 
        !bTempStg);
	CommonConstruct();
}

CSummaryPropSheet::CSummaryPropSheet(CServerDoc *pDocument,LPCTSTR pszCaption, 
    CWnd* pParentWnd, UINT nIDHelp)	: 
    CPPPropertySheet(pszCaption, pParentWnd, nIDHelp, GetSelectPage())
{
    BOOL bTempStg;
    ASSERT(pDocument);
    m_pDocument = pDocument;
    m_bHasSummaryStream = (m_pDocument->GetRootCompoundStorage(&bTempStg) && 
        !bTempStg);
	CommonConstruct();
}

void CSummaryPropSheet::CommonConstruct()
{
	if (m_bHasSummaryStream)
		AddPage(&m_SummaryStreamPage);
	AddPage(&m_GeneralSummaryPage);
}

CSummaryPropSheet::~CSummaryPropSheet()
{
//	m_nLastPage = m_nCurPage;
    m_nLastPage = 0;
}

UINT CSummaryPropSheet::GetSelectPage()
{
	int nPageCount = 2;
	if (!m_bHasSummaryStream)
		nPageCount -= 1;
	if (nPageCount <= m_nLastPage)
		m_nLastPage = nPageCount - 1;
	return(m_nLastPage);
}

BEGIN_MESSAGE_MAP(CSummaryPropSheet, CPPPropertySheet)
	//{{AFX_MSG_MAP(CSummaryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSummaryStreamPage, CPPPropertyPage)
	//{{AFX_MSG_MAP(CSummaryStreamPage)
	ON_EN_CHANGE(IDC_SUMM_AUTHOR, OnChangeSummary)
	ON_EN_CHANGE(IDC_SUMM_KEYWORDS, OnChangeSummary)
	ON_EN_CHANGE(IDC_SUMM_SUBJECT, OnChangeSummary)
	ON_EN_CHANGE(IDC_SUMM_TITLE, OnChangeSummary)
	ON_EN_CHANGE(IDC_SUMM_COMMENTS, OnChangeSummary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSummaryPropSheet message handlers


