// tbardlgs.cpp : implementation file
//

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "tbardlgs.h"
#include "pptbars.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
/////////////////////////////////////////////////////////////////////////////
// CMacroListBox dialog
/////////////////////////////////////////////////////////////////////////////

CMacroListBox::CMacroListBox()
{
	m_pDlg = NULL;
}

CMacroListBox::~CMacroListBox()
{
}

BEGIN_MESSAGE_MAP(CMacroListBox, CListBox)
	//{{AFX_MSG_MAP(CMacroListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMacroListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDown(nFlags, point);
    FORWARD_WM_LBUTTONUP(GetSafeHwnd(), point.x, point.y, nFlags, ::SendMessage);
	m_pDlg->DoMacroDrag(point);
}

void CMacroListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRect	r;
	CPoint	Pt = point;

	if (OCToolList->IsDraging())
	{
		m_pDlg->GetWindowRect(r);
		ClientToScreen(&Pt);
		if (r.PtInRect(Pt))
			OCToolList->EndDrag(CPoint(0,0), TRUE);
		else
			m_pDlg->pButtonSelector->EndDrag(point);
	}
	CListBox::OnLButtonUp(nFlags, point);
}

void CMacroListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (OCToolList->IsDraging())
		OCToolList->MouseMove(point);
	CListBox::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// TBarDlgList dialog
/////////////////////////////////////////////////////////////////////////////


#define		CUSTOM_VERTICALGAP	3

TBarDlgList::TBarDlgList(CWnd* pParent /*=NULL*/)
	: CDialog(TBarDlgList::IDD, pParent)
{
	//{{AFX_DATA_INIT(TBarDlgList)
	m_ColorIcons 	= FALSE;
	m_ToolTips 	= FALSE;
	//}}AFX_DATA_INIT
}


void TBarDlgList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TBarDlgList)
	DDX_Control(pDX, IDC_TBARLIST, m_tbarlist);
	DDX_Check(pDX, IDC_COLORBUTTON, m_ColorIcons);
	DDX_Check(pDX, IDC_TBARTOOLTIPS, m_ToolTips);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TBarDlgList, CDialog)
	//{{AFX_MSG_MAP(TBarDlgList)
	ON_BN_CLICKED(IDC_TBARNEW, OnNew)
	ON_BN_CLICKED(IDC_TBARCUST, OnCustomize)
	ON_BN_CLICKED(IDC_TBAROPTIONS, OnOption)
	ON_BN_CLICKED(IDC_TBARDELRESET, OnDelReset)
	ON_LBN_SELCHANGE(IDC_TBARLIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// TBarDlgList message handlers

BOOL TBarDlgList::OnInitDialog() 
{
    LPTSTR      	lp;
	COCToolList*	pList = (COCToolList*)PictPubApp.GetCustomBoxes();

	CDialog::OnInitDialog();
	
    CenterPopup(GetSafeHwnd());
    AstralStr(IDS_TBARRESET, &lp);
	m_Reset   	= lp;
    AstralStr(IDS_TBARDELETE, &lp);
	m_Delete 	= lp;
	m_ColorIcons = pList->GetColorIcons();
	m_ToolTips   = pList->GetToolTips();
	((CButton*)GetDlgItem(IDC_COLORBUTTON))->SetCheck(m_ColorIcons);
	((CButton*)GetDlgItem(IDC_TBARTOOLTIPS))->SetCheck(m_ToolTips);
	pList->FillListBox(&m_tbarlist, 0);
	OnSelchangeList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TBarDlgList::OnSelchangeList() 
{
	int			i = m_tbarlist.GetCurSel();
	CString*	Str = i < 3? &m_Reset : &m_Delete;
	
	GetDlgItem(IDC_TBARDELRESET)->SetWindowText(*Str);

	// Option, Del/Reset buttons do not apply for Ribbon
	GetDlgItem(IDC_TBAROPTIONS)->EnableWindow(i != 1);
	GetDlgItem(IDC_TBARDELRESET)->EnableWindow(i != 1);
}

void TBarDlgList::OnCancel()
{
	CDialog::OnCancel();
}
 
void TBarDlgList::OnOK() 
{
	int		i,c = m_tbarlist.GetCount();
	COCToolList*	pList = (COCToolList*)PictPubApp.GetCustomBoxes();
	COCToolBar*		pBar;
	
	pList->SetColorIcons(m_ColorIcons);
	pList->SetToolTips(m_ToolTips);
	for (i=0; i < c; i++)
	{
		pBar = (COCToolBar*)m_tbarlist.GetItemData(i);

		if (pBar->GetUpState())
		{
			if (!pBar->IsToolBarUp())
				pBar->ShowToolBar((CFrameWnd *)AfxGetMainWnd());
		}
		else if (pBar->IsToolBarUp())
			pBar->ShowToolBar((CFrameWnd *)AfxGetMainWnd());
	}
	CDialog::OnOK();
}

void TBarDlgList::OnCustomize() 
{
	OnOK();
	AfxGetMainWnd()->PostMessage(WM_COMMAND, IDM_OCCustom, 0);
}

void TBarDlgList::OnDelReset() 
{
	CPPToolList*	pList = (CPPToolList*)PictPubApp.GetCustomBoxes();
	COCToolBar*		pBar;
	int			i = m_tbarlist.GetCurSel();
	BOOL		Del = i > 2;
	
	pBar = (COCToolBar*)m_tbarlist.GetItemData(i);
	if (Del)
	{
		pList->Delete(pBar);
		i--;
	}
	else if (pBar == pList->GetMain())
		pList->ResetMain((CFrameWnd *)AfxGetMainWnd());
	else if (pBar == pList->GetStatus())
		pList->ResetStatus((CFrameWnd *)AfxGetMainWnd());
	pList->FillListBox(&m_tbarlist, i);
	OnSelchangeList();
	m_tbarlist.SetFocus();
}

void TBarDlgList::OnOption() 
{
	int				i = m_tbarlist.GetCurSel();
	CString			Str;
	COCToolList*	pList = (COCToolList*)PictPubApp.GetCustomBoxes();
	COCToolBar*		pBar;

	pBar = (COCToolBar*)m_tbarlist.GetItemData(i);
	Str = pBar->GetName();
    if (CNameToolBar(&Str, pBar).DoModal() != IDCANCEL)
    {
        FixExtName(&Str);
		pBar->SetName(Str);
		pList->FillListBox(&m_tbarlist, i);
	}
	m_tbarlist.SetFocus();
}

BOOL PromptForToolBarName(CString &Str)
{
	COCToolList*	pList = (COCToolList*)PictPubApp.GetCustomBoxes();
    if (CNameToolBar(&Str).DoModal() != IDCANCEL)
	{
        FixExtName(&Str);
		return(TRUE);
	}
	return(FALSE);
}

void TBarDlgList::OnNew() 
{
	int				i;
	CString			Str;
	COCToolList*	pList = (COCToolList*)PictPubApp.GetCustomBoxes();

	if (PromptForToolBarName(Str))
    {
		COCToolBar*		pBar = pList->AddToolBar();
		pBar->SetName(Str);
		i = m_tbarlist.AddString(Str);
		pList->FillListBox(&m_tbarlist, i);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTBList
/////////////////////////////////////////////////////////////////////////////

CTBList::CTBList()
{
	m_ItemHeight = 0;
}

CTBList::~CTBList()
{
}


BEGIN_MESSAGE_MAP(CTBList, CListBox)
	//{{AFX_MSG_MAP(CTBList)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTBList message handlers
/////////////////////////////////////////////////////////////////////////////

void CTBList::DrawItem(LPDRAWITEMSTRUCT lpDraw) 
{
COCToolBar*	pBar;
CDC		*pDC = CDC::FromHandle(lpDraw->hDC);
CBrush	 br;
CString	 Str;
int      iOldMode;
COLORREF lOldColor;
HPEN     OldPen;
CRect	 Box, ClientRect;
BOOL	 bFocus		= lpDraw->itemState & ODS_FOCUS;
BOOL	 bHilighted	= lpDraw->itemState & ODS_SELECTED;

// Check to see if its focus draw only
if ( lpDraw->itemAction == ODA_FOCUS )
	{
	pDC->DrawFocusRect(&lpDraw->rcItem);
	return;
	}
Box = lpDraw->rcItem;
ClientRect = Box;
if (!m_ItemHeight)
	m_ItemHeight = Box.bottom - Box.top;
ClientRect.left += m_ItemHeight;
Box.left++;
Box.top++;
Box.bottom--;
Box.right = Box.left + m_ItemHeight - 2;
OldPen = (HPEN)pDC->SelectObject(GetStockObject(BLACK_PEN));
pDC->Rectangle(&Box);
pBar = (COCToolBar*)lpDraw->itemData;
if (pBar->GetUpState())
	{
	pDC->MoveTo(Box.left,  Box.top);
	pDC->LineTo(Box.right, Box.bottom);
	pDC->MoveTo(Box.left,  Box.bottom);
	pDC->LineTo(Box.right, Box.top);
	}
pDC->SelectObject(OldPen);
GetText(lpDraw->itemID, Str);
if (Str.GetLength())
	{
	// Draw the text
	iOldMode = pDC->SetBkMode(TRANSPARENT);
	br.CreateSolidBrush(GetSysColor(bHilighted ? COLOR_HIGHLIGHT : COLOR_WINDOW));
	pDC->FillRect(&ClientRect, &br);
	lOldColor = pDC->SetTextColor(GetSysColor( bHilighted ? COLOR_HIGHLIGHTTEXT :	COLOR_WINDOWTEXT ) );

	pDC->DrawText(Str, -1, &ClientRect,	DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	pDC->SetBkMode(iOldMode );
	pDC->SetTextColor(lOldColor );
	}
if ( bFocus )
	pDC->DrawFocusRect(&ClientRect );
}

void CTBList::OnLButtonDown(UINT nFlags, CPoint point) 
{
int		j,i = GetTopIndex();
CRect	r;

CListBox::OnLButtonDown(nFlags, point);
GetClientRect(&r);
j = min(GetCount(), i + r.Height() / m_ItemHeight);
for (;i < j; i++)
	{
	GetItemRect(i, &r);
	r.left++;
	r.top++;
	r.bottom--;
	r.right = r.left + m_ItemHeight - 2;
	if (r.PtInRect(point))
		break;
	}
if (i < j)
	{
	BOOL	UpState = TRUE;
	COCToolBar*	pBar = (COCToolBar*)GetItemData(i);

	if (pBar->GetUpState())
		UpState = FALSE;
	pBar->SetUpState(UpState);
	InvalidateRect(&r, FALSE);
	}
}

void CTBList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
OnLButtonDown(nFlags, point);	
}

////////////////////////////////////////////////////////////////////////
// OCButtonSelector
///////////////////////////////////////////////////////////////////////

OCButtonSelector::OCButtonSelector(CRect r, CSize Size):m_OCBM(Size)
{
m_Area 		= r;
m_Size 		= Size;
m_pData 	= NULL;
m_nCount 	= 0;
m_Selected  = NULL;
}

OCButtonSelector::~OCButtonSelector()
{
	if (m_pData)
		DeleteButtons();
	m_pData 	= NULL;
	m_nCount 	= 0;
}

inline COCButton** OCButtonSelector::GetButtonPtrPtr(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nCount);
	ASSERT(m_pData != NULL);
	return (&m_pData[nIndex]);
}

inline COCButton* OCButtonSelector::GetButtonPtr(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nCount);
	ASSERT(m_pData != NULL);
	return (m_pData[nIndex]);
}

void OCButtonSelector::StartDrag(CWnd *pOwner, CPoint Pt)
{
	CRect	r;

	if (HitTest(Pt, r))
		OCToolList->StartDrag(pOwner, Pt, r);
}

void OCButtonSelector::StartMacroDrag(int Sel, CWnd *pOwner, CPoint Pt, CRect r)
{
	m_Selected = GetButtonPtr(Sel);
	OCToolList->StartDrag(pOwner, Pt, r);
}

void OCButtonSelector::MoveDrag(CPoint Pt)
{
	if (OCToolList->IsDraging())
		OCToolList->MouseMove(Pt);
}

void OCButtonSelector::ExecuteDrag()
{
	COCToolBar	*pBar = OCToolList->m_pDragOverWnd;
	CPoint		 Pt   = OCToolList->m_DropPoint;
	CRect		 r;
	BOOL		 After;
	CString		Str;

	if (pBar)		// insert
	{
		BOOL 		 bHorz = (pBar->m_dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE;
		int			 loc  = pBar->GetInsertLoc(Pt);
		
		if (loc != -1)
		{
			pBar->GetItemRect(loc, &r);
			After = ( bHorz && Pt.x > (r.left + r.Width() /2)) ||
				(!bHorz && Pt.y > (r.top  + r.Height()/2));
			if (After)
				loc++;
		}
		pBar->InsertButton(m_Selected->Duplicate(), loc);
	}
	else if (PromptForToolBarName(Str))
		OCToolList->CreateToolBar(m_Selected, Pt, Str);// create new COCToolBar
}

void OCButtonSelector::EndDrag(CPoint Pt)
{
	if (OCToolList->EndDrag(Pt))
		ExecuteDrag();
}

BOOL OCButtonSelector::HitTest(CPoint Pt, CRect &r)
{
	CRect		BtRect;
	CPoint		DstPt;
	COCButton* pTBB;

	r.left 	= m_Area.left;
	r.top  	= m_Area.top;
	for (int i = 0; i < m_nCount; i++)
	{
		pTBB = GetButtonPtr(i);
		if ((r.left + pTBB->Size.cx) > m_Area.right)
		{
			r.left  = m_Area.left;
			r.top  += pTBB->Size.cy + CUSTOM_VERTICALGAP;
		}
		r.right   = r.left + pTBB->Size.cx;
		r.bottom  = r.top  + pTBB->Size.cy;
		if (r.PtInRect(Pt))
		{
			m_Selected = pTBB;
			return(TRUE);
		}
		r.left += CUSTOM_VERTICALGAP + pTBB->Size.cx;
	}
return(FALSE);
}

BOOL OCButtonSelector::Draw(CDC	*pPaintDC)
{
	COCButton* pTBB;

	m_OCBM.StartDraw();
	for (int i = 0; i < m_nCount; i++)
	{
		pTBB = GetButtonPtr(i);
		if (!(pTBB->nStyle & TBBS_WINDOW))		// not Window
			pTBB->DrawButton(pPaintDC, 16);
	}
	m_OCBM.EndDraw();
return(TRUE);
}

void OCButtonSelector::DeleteButtons()
{
	COCButton* pTBB;
	for (int i = 0; i < m_nCount; i++)
	{
		pTBB = GetButtonPtr(i);
		if (pTBB->nStyle & TBBS_WINDOW)
			DestroyWindow(pTBB->hWnd);
		delete pTBB;
	}
	free(m_pData);
}

BOOL OCButtonSelector::AllocElements(int nElements)
{
	ASSERT(nElements > 0);
	if (m_pData != NULL)
		DeleteButtons();

	if ((m_pData = (COCButton**)calloc(nElements, sizeof(COCButton*))) == NULL)
		return FALSE;

	m_nCount = nElements;
	return TRUE;
}


static OCCONTROL  Controls[]   = 
{
	{IDC_ZOOM_COMBO, IDS_OCZOOM, "COMBOBOX", CBS_DROPDOWN|WS_VSCROLL|ES_WANTRETURN,150, 100},
	{IDC_STATUSTEXT, IDS_OCSTATUS, "icon", 0x0010 | WS_DISABLED,128, 24},
	{IDC_ACTIVECOLOR, 0, "color2", 0,32, 32},
};

COCButton* OCButtonSelector::CreateWinButton(int index, CWnd* pParent)
{
	OCCONTROL*		pDef = &Controls[index];
	COCButton* 	pTBB;
	LPTSTR		lp;

	AstralStr(pDef->CaptionID, &lp);
	pTBB = new COCWinButton(pDef->nID,lp, pDef->ClassName, pDef->Style, CSize(pDef->w,pDef->h));
	pTBB->Create(pParent);
	EnableWindow(pTBB->hWnd, FALSE);
	return(pTBB);
}

BOOL OCButtonSelector::FillMacro(CListBox*	pList)
{
	CString			Str;
	COCButton** 	pTBB;
	int				len = pList->GetCount();

	// first allocate array for panes and copy initial data
	if (!AllocElements(len))
		return FALSE;
	// go through them adding buttons
	for (int i = 0; i < len; i++)
	{
		pList->GetText(i, Str);
		pTBB = GetButtonPtrPtr(i);
		*pTBB = new COCButton(IDM_MACROPLAY, Str, Str);
		(*pTBB)->CalcSize(FALSE);
	}
	if (!BuildBitmap())
		return FALSE;
	return TRUE;
}

BOOL OCButtonSelector::SetButtons(CWnd* pParent, const OCBUTTON* lpIDArray, int nIDCount)
{
	ASSERT(nIDCount >= 1);  // must be at least one of them
	ASSERT(lpIDArray == NULL ||
		AfxIsValidAddress(lpIDArray, sizeof(OCBUTTON) * nIDCount, FALSE));

	// first allocate array for panes and copy initial data
	if (!AllocElements(nIDCount))
		return FALSE;
	if (lpIDArray != NULL)
	{
		// go through them adding buttons
		CPoint		DstPt;
		COCButton** pTBB;
		DstPt = CPoint(m_Area.left,m_Area.top);
		for (int i = 0; i < nIDCount; i++)
		{
			pTBB = GetButtonPtrPtr(i);
			if (lpIDArray->nID != -1)
			{
				*pTBB  = new COCButton();
				**pTBB = *lpIDArray;
				(*pTBB)->CalcSize(FALSE);
			}
			else
				*pTBB = CreateWinButton(lpIDArray->x, pParent);
			if ((DstPt.x + (*pTBB)->Size.cx) > m_Area.right)
			{
				DstPt.x  = m_Area.left;
				DstPt.y += (*pTBB)->Size.cy + CUSTOM_VERTICALGAP;
			}
			(*pTBB)->Move(DstPt, TRUE);
			DstPt.x += CUSTOM_VERTICALGAP + (*pTBB)->Size.cx;
			lpIDArray++;
		}
		if (!BuildBitmap())
			return FALSE;
	}
	return TRUE;
}

extern LPTSTR TBarIconDLL[4];
BOOL OCButtonSelector::BuildBitmap()
{
	HINSTANCE	hOrigRes = AfxGetResourceHandle();
	HINSTANCE	hRes;
	int			i    = OCToolList->m_ColorIcons? 0 : 1;
	HBITMAP		hbm;

	hRes = LoadLibrary(TBarIconDLL[i]); 
	AfxSetResourceHandle(hRes);
	hbm = ::BuildBitmap(m_pData, m_nCount, m_Size);
	AfxSetResourceHandle(hOrigRes);
	FreeLibrary(hRes);
	if (hbm)
		m_OCBM.SafeAttach(hbm);
	return(hbm != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// OCCustom dialog
/////////////////////////////////////////////////////////////////////////////

OCCustom::OCCustom(CWnd* pParent /*=NULL*/)
	: CDialog(OCCustom::IDD, pParent)
{
	//{{AFX_DATA_INIT(OCCustom)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Macros.m_pDlg = this;
	pButtonSelector = NULL;
}

OCCustom::~OCCustom()
{
if (pButtonSelector)
	delete pButtonSelector;
}


void OCCustom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OCCustom)
	DDX_Control(pDX, IDC_OCCat, m_Cat);
	DDX_Control(pDX, IDC_OCMacro, m_Macros);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OCCustom, CDialog)
	//{{AFX_MSG_MAP(OCCustom)
	ON_LBN_SELCHANGE(IDC_OCCat, OnSelchangeCat)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// OCCustom message handlers
/////////////////////////////////////////////////////////////////////////////

int OCCustom::GetDragMode()
{
	int		i = m_Cat.GetCurSel();
	int		j = m_Cat.GetItemData(i);
	return(j);
}

void OCCustom::DoMacroDrag(CPoint Pt)
{
	CRect	r;
	int		i = m_Macros.GetCurSel();

	if (i != -1)
	{
		m_Macros.GetItemRect(i, r);
		pButtonSelector->StartMacroDrag(i, &m_Macros, Pt, r);
	}
}

void OCCustom::OnLButtonDown(UINT nFlags, CPoint point) 
{
	pButtonSelector->StartDrag(this, point);
	CDialog::OnLButtonDown(nFlags, point);
}

void OCCustom::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRect	r;
	CPoint	Pt = point;

	GetWindowRect(r);
	ClientToScreen(&Pt);
	if (r.PtInRect(Pt))
		OCToolList->EndDrag(CPoint(0,0), TRUE);
	else
		pButtonSelector->EndDrag(point);
	CDialog::OnLButtonUp(nFlags, point);
}

void OCCustom::OnPaint() 
{
	CPaintDC 	dc(this);
	pButtonSelector->Draw(&dc);
}

void OCCustom::OnOK() 
{
	OCToolList->SetCustomizing(FALSE);
	CDialog::OnOK();
}

void OCCustom::OnCancel() 
{
	if (OCToolList->IsDraging())
		OCToolList->EndDrag(CPoint(0,0), TRUE);
	else
	{
		CDialog::OnCancel();
		OCToolList->SetCustomizing(FALSE);
	}
}

static OCBUTTON  buttons[] =
{
	// File Menu
	{IDM_NEW,  				0,0,IDR_FILEMENU},
	{IDM_OPEN, 				1,0,IDR_FILEMENU},
	{IDM_SAVE, 				2,0,IDR_FILEMENU},
	{IDM_SAVEAS, 			3,0,IDR_FILEMENU},
	{IDM_REVERT,			4,0,IDR_FILEMENU},
	{IDM_IMPORT,			5,0,IDR_FILEMENU},
	{IDM_ACQUIRE,			6,0,IDR_FILEMENU},
	{IDM_MONITORMAP,		7,0,IDR_FILEMENU},
	{IDM_PRINTSETUP, 		8,0,IDR_FILEMENU},
	{IDM_CALIBRATESCAN,		9,0,IDR_FILEMENU},
	{IDM_CALIBRATEPRINT,	10,0,IDR_FILEMENU},
	{IDM_PRINTPREVIEW,		11,0,IDR_FILEMENU},
	{IDM_PRINT,				12,0,IDR_FILEMENU},
	{IDM_SENDMAIL,			13,0,IDR_FILEMENU},

	// 14 edit menu
	{IDM_CMDLIST_UNDO,		0,0,IDR_EDITMENU},
	{IDM_CMDLIST_REDO,		1,0,IDR_EDITMENU},
	{IDM_APPLY,				2,0,IDR_EDITMENU},
	{IDM_CMDLIST_EDIT,		3,0,IDR_EDITMENU},
	{IDM_CUT,				4,0,IDR_EDITMENU},
	{IDM_COPY,				5,0,IDR_EDITMENU},
	{IDM_COPYTOFILE,		6,0,IDR_EDITMENU},
	{IDM_PASTE,				7,0,IDR_EDITMENU},
	{IDM_PASTEASNEW,		8,0,IDR_EDITMENU},
	{IDM_DELETE,			9,0,IDR_EDITMENU},
	{IDM_PASTEFROMFILE,		10,0,IDR_EDITMENU},

	// 25 view menu
	{IDC_STATUSPALETTE,		0,0,IDR_VIEWMENU},
	{IDM_SHOWINFO,			1,0,IDR_VIEWMENU},
	{IDM_OBJ_MANAGER,		2,0,IDR_VIEWMENU},
	{IDM_RULERSHOW,			3,0,IDR_VIEWMENU},
	{IDM_TASKMANSHOW,		4,0,IDR_VIEWMENU},
	{IDC_STATUSINFO,		5,0,IDR_VIEWMENU},

	// 31 tools menu
	{IDM_SCRATCHPAD,		0,0,IDR_TOOLMENU},
	{IDM_MACROPLAY,			1,0,IDR_TOOLMENU},
	{IDM_MACROPLAYBATCH,	2,0,IDR_TOOLMENU},
	{IDM_MACRORECORD,		3,0,IDR_TOOLMENU},
	{IDM_MACRO_EDIT,		4,0,IDR_TOOLMENU},
	{IDM_MACROSTOP,			5,0,IDR_TOOLMENU},
	{IDM_PREF,				6,0,IDR_TOOLMENU},

	// 38 mask menu
	{IDM_MASKUNDO,			0,0,IDR_MASKMENU},
	{IDM_MASKUNDO,			1,0,IDR_MASKMENU},  // shoud be redo
	{IDM_MASKREMOVE,		2,0,IDR_MASKMENU},
	{IDM_MASKLOAD,			3,0,IDR_MASKMENU},
	{IDM_MASKSAVE,			4,0,IDR_MASKMENU},
	{IDM_CHROMA_MASK,		5,0,IDR_MASKMENU},
	{IDM_MASKFROMOBJECT,	6,0,IDR_MASKMENU},
	{IDM_MASKINVERT,		7,0,IDR_MASKMENU},
	{IDM_FEATHER,			8,0,IDR_MASKMENU},
	{IDM_REMOVE_HOLES,		9,0,IDR_MASKMENU},
	{IDM_MASK_SMOOTH,		10,0,IDR_MASKMENU},
	{IDM_CROP,				11,0,IDR_MASKMENU},
	{IDM_MASKSTROKE,		12,0,IDR_MASKMENU},
	{IDM_HIDEMARQUEE,		13,0,IDR_MASKMENU},

	// 52 mapping menu
	{IDM_MAP,				0,0,IDR_MAPMENU},
	{IDM_CONTBRIT_STICK,	1,0,IDR_MAPMENU},
	{IDM_CONTBRIT_VISUAL,	2,0,IDR_MAPMENU},
	{IDM_COLORBAL_STICK,	3,0,IDR_MAPMENU},
	{IDM_COLORBAL_VISUAL,	4,0,IDR_MAPMENU},
	{IDM_QUARTERTONE,		5,0,IDR_MAPMENU},
	{IDM_POSTERIZE,			6,0,IDR_MAPMENU},
	{IDM_THRESHOLD,			7,0,IDR_MAPMENU},
	{IDM_HUE_SHIFT,			8,0,IDR_MAPMENU},
	{IDM_HUE,				9,0,IDR_MAPMENU},
	{IDM_HISTOGRAM,			10,0,IDR_MAPMENU},
	{IDM_CALIBRATEAPPLY,	11,0,IDR_MAPMENU},
	{IDM_PALETTEEDIT,		12,0,IDR_MAPMENU},

	// 65 image menu
	{IDM_SIZE,				0,0,IDR_IMAGEMENU},
	{IDM_EXPAND,			1,0,IDR_IMAGEMENU},
	{IDM_ROTATE90,			2,0,IDR_IMAGEMENU},
	{IDM_ROTATE270,			3,0,IDR_IMAGEMENU},
	{IDM_ROTATE180,			4,0,IDR_IMAGEMENU},
	{IDM_ROTATEANY,			5,0,IDR_IMAGEMENU},
	{IDM_MIRRORX,			6,0,IDR_IMAGEMENU},
	{IDM_MIRRORY,			7,0,IDR_IMAGEMENU},
	{IDM_MIRRORXY,			8,0,IDR_IMAGEMENU},
	{IDM_SPLITRGB,			9,0,IDR_IMAGEMENU},
	{IDM_SPLITHSL,			10,0,IDR_IMAGEMENU},
	{IDM_SPLITCMYK,			11,0,IDR_IMAGEMENU},
	{IDM_CHANNELCOMBINE,	12,0,IDR_IMAGEMENU},
	{IDM_CONVLA,			13,0,IDR_IMAGEMENU},
	{IDM_CONVSP,			14,0,IDR_IMAGEMENU},
	{IDM_CONVCT,			15,0,IDR_IMAGEMENU},
	{IDM_CONVPALETTE,		16,0,IDR_IMAGEMENU},
	{IDM_CONVRGBCOLOR,		17,0,IDR_IMAGEMENU},
	{IDM_CONVCMYKCOLOR,		18,0,IDR_IMAGEMENU},
	{IDM_COLORMANAGED,		19,0,IDR_IMAGEMENU},
	{IDM_INVERT,			20,0,IDR_IMAGEMENU},
	{IDC_STITCH,			21,0,IDR_IMAGEMENU},
	{IDM_SPECIALFX,			22,0,IDR_IMAGEMENU},

	// 88 object menu
	{IDM_OBJ_ALIGN,			0,0,IDR_OBJMENU},
	{IDM_OBJ_POSITION,		1,0,IDR_OBJMENU},
	{IDM_OBJ_GROUP,			2,0,IDR_OBJMENU},
	{IDM_OBJ_UNGROUP,		3,0,IDR_OBJMENU},
	{IDM_OBJ_UNLOCK,		4,0,IDR_OBJMENU},
	{IDM_OBJ_LOCK,			5,0,IDR_OBJMENU},
	{IDM_OBJ_DOWNLEVEL,		6,0,IDR_OBJMENU},
	{IDM_OBJ_UPLEVEL,		7,0,IDR_OBJMENU},
	{IDM_OBJ_MOVETOBOTTOM,	8,0,IDR_OBJMENU},
	{IDM_OBJ_MOVETOTOP,		9,0,IDR_OBJMENU},
	{IDM_OBJ_COMBOBJS,		10,0,IDR_OBJMENU},
	{IDM_COMBINE,			11,0,IDR_OBJMENU},
	{IDM_COMBINE_SEL,		12,0,IDR_OBJMENU},
	{IDM_OBJ_FEATHER,		13,0,IDR_OBJMENU},
	{IDM_OBJ_APPLYMASK,		14,0,IDR_OBJMENU},
	{IDM_OBJECTFROMMASK,	15,0,IDR_OBJMENU},
	{IDM_OBJ_DELETE,		16,0,IDR_OBJMENU},
	{IDM_OBJ_SELECTALL,		17,0,IDR_OBJMENU},
	{IDM_OBJ_HIDEMARQUEE,	18,0,IDR_OBJMENU},

	// 107 window menu
	{ID_WINDOW_NEW,			0,0,IDR_WINDOWMENU},
	{ID_WINDOW_CASCADE,		1,0,IDR_WINDOWMENU},
	{ID_WINDOW_TILE_HORZ,	2,0,IDR_WINDOWMENU},
	{IDM_CLOSEALL,			3,0,IDR_WINDOWMENU},

	// 111 help menu
	{IDM_HELP_CONTEXT,		0,0,IDR_HELPMENU},
	{IDM_HELP_INDEX,		1,0,IDR_HELPMENU}, // should be	IDM_HELP_SEARCH
	{IDM_HELP_INDEX,		2,0,IDR_HELPMENU},
	{IDM_ABOUT,				3,0,IDR_HELPMENU},

	// TOOLS

	// 115 selector tool
	{IDC_SELECTOR,			0,0,IDR_SELECTOR},

	// 116 mask tools
	{IDC_SQUARE,			0,0,IDR_MASKTOOL},
	{IDC_FREE,				1,0,IDR_MASKTOOL},
	{IDC_PAINTON,			2,0,IDR_MASKTOOL},
	{IDC_MAGICWAND,			3,0,IDR_MASKTOOL},
	{IDC_TRANSFORMER,		4,0,IDR_MASKTOOL},
	{IDC_POINTER,			5,0,IDR_MASKTOOL},
	{IDC_CROP,				6,0,IDR_MASKTOOL},
	{IDC_MASKEDITALPHA,		7,0,IDR_MASKTOOL},
	{IDC_VIEWSLIME,			8,0,IDR_MASKTOOL},
	{IDC_STATUSSHIELD,		9,0,IDR_MASKTOOL},

	// 126 retouch tools
	{IDC_PAINT,				0,0,IDR_PAINTTOOL},
	{IDC_SPRAY,				1,0,IDR_PAINTTOOL},
	{IDC_CLONE,				2,0,IDR_PAINTTOOL},
	{IDC_TEXTURE,			3,0,IDR_PAINTTOOL},
	{IDC_SMEAR,				4,0,IDR_PAINTTOOL},
	{IDC_ERASER,			5,0,IDR_PAINTTOOL},

	// 132 filter tools
	{IDC_SHARP,				0,0,IDR_FILTERTOOL},
	{IDC_SMOOTH,			1,0,IDR_FILTERTOOL},
	{IDC_LIGHTEN,			2,0,IDR_FILTERTOOL},
	{IDC_DARKEN,			3,0,IDR_FILTERTOOL},

	// 136 fill tools
	{IDC_VIGNETTE,			0,0,IDR_FILLTOOL},
	{IDC_TEXTUREFILL,		1,0,IDR_FILLTOOL},
	{IDC_TINTFILL,			2,0,IDR_FILLTOOL},
	{IDC_FLOOD,				3,0,IDR_FILLTOOL},

	// 140 draw tools
	{IDC_DRAWSQUARE,		0,0,IDR_DRAWTOOL},
	{IDC_DRAWFREE,			1,0,IDR_DRAWTOOL},
	{IDC_PENCIL,			2,0,IDR_DRAWTOOL},

	// 143 zoom tools
	{IDC_ZOOMIN,			0,0,IDR_ZOOMTOOL},
	{IDC_ZOOMOUT,			1,0,IDR_ZOOMTOOL},
	{IDC_VIEWLAST,			2,0,IDR_ZOOMTOOL},
	{IDC_VIEW1TO1,			3,0,IDR_ZOOMTOOL},
	{IDC_VIEWALL,			4,0,IDR_ZOOMTOOL},
	{IDC_VIEWFULL,			5,0,IDR_ZOOMTOOL},
	{IDM_SHOWZOOMBOX,		6,0,IDR_ZOOMTOOL},
	{0xFFFFFFFF,			0,0,0},

	// 151 text tools
	{IDC_TEXT,				0,0,IDR_TEXTTOOL},

	// 152 probe tools
	{IDC_PROBEPOINT,		0,0,IDR_PROPETOOL},
	{IDC_PROBERECT,			1,0,IDR_PROPETOOL},

	// 154 controls
	{0xFFFFFFFF, 1, 0, 0},
	{0xFFFFFFFF, 2, 0, 0},
};

static int  CatLimits[]  = {0, 14, 25, 31, 38, 52, 65, 88, 107, 111, 115, 
							116, 126, 132, 136, 140, 143, 151, 152, 154, 156};
static int	Categories[] = 
	{
	IDS_CATFILEMENU,
	IDS_CATEDITMENU,
	IDS_CATVIEWMENU,
	IDS_CATTOOLSMENU,
	IDS_CATMASKMENU,
	IDS_CATMAPMENU,
	IDS_CATIMAGEMENU,
	IDS_CATOBJECTMENU,
	IDS_CATWINDOWMENU,
	IDS_CATHELPMENU,
	IDS_CATSELECTORTOOL,
	IDS_CATMASKTOOLS,
	IDS_CATRETOUCHTOOLS,
	IDS_CATFILTERTOOLS,
	IDS_CATFILLTOOLS,
	IDS_CATDRAWTOOLS,
	IDS_CATZOOMTOOLS,
	IDS_CATTEXTTOOL,
	IDS_CATPROPETOOLS,
	IDS_CATCONTROLS,
	};

void OCCustom::FillMacroList(int Sel)
{
	m_Macros.ResetContent();
    InitExtName(GetSafeHwnd(), IDC_OCMacro, NULL, Names.Macro, IDN_MACRO);
	m_Macros.AddString("Nezar");
	m_Macros.SetCurSel(Sel);
}

void OCCustom::EnableMacros(BOOL f)
{
	if (!f)
	{
		m_Macros.ResetContent();
		m_Macros.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OCMACRO_STEXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OCBUTTONS)->ShowWindow(SW_SHOW);
	}
	else
	{
		m_Macros.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_OCMACRO_STEXT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_OCBUTTONS)->ShowWindow(SW_HIDE);
		FillMacroList(0);
	}
	
}

void OCCustom::OnSelchangeCat()
{
	int				Mode = GetDragMode();

	if (!Mode)
	{
		int		i = m_Cat.GetCurSel();
		int		Start = CatLimits[i];
		pButtonSelector->SetButtons(this, &buttons[Start], CatLimits[i+1] - Start);
		EnableMacros(FALSE);
	}
	else			// Macro
	{
		EnableMacros(TRUE);
		pButtonSelector->FillMacro(&m_Macros);
	}
	InvalidateRect(pButtonSelector->m_Area);
}

void OCCustom::FillCategory(int Sel)
{
	int				i;
    LPTSTR    		lp;

	m_Cat.ResetContent();
	for (i = 0; i < (sizeof(Categories) / sizeof(int)); i++)
	{
		AstralStr(Categories[i], &lp);
		m_Cat.AddString(lp);
		m_Cat.SetItemData(i, 0);
	}
	AstralStr(IDS_CATMACROTOOLS, &lp);
	i = m_Cat.AddString(lp);
	m_Cat.SetItemData(i, 1);
	m_Cat.SetCurSel(Sel);
}

BOOL OCCustom::OnInitDialog() 
{
	CRect	r;
	CDialog::OnInitDialog();
	UINT swpflags = SWP_NOSIZE | SWP_DRAWFRAME | SWP_SHOWWINDOW;

	SetWindowPos(NULL, 100, 100, 0, 0, swpflags);
    CenterPopup(GetSafeHwnd());
	GetDlgItem(IDC_OCBUTTONS)->GetWindowRect(&r);
	ScreenToClient(&r);
	r.top  += 15;
	r.left += 5;
	FillCategory(0);
	pButtonSelector = new OCButtonSelector(&r, CSize(16,16));
	OnSelchangeCat();
	OCToolList->SetCustomizing(TRUE);
	
	return TRUE;
}

BOOL OCCustom::Create() 
{
	return CDialog::Create(IDD_OCCustom);
}

void OCCustom::OnDestroy() 
{
	CDialog::OnDestroy();	
	delete this;			// this is not good but it is ok for now.
}

void OCCustom::OnMouseMove(UINT nFlags, CPoint point) 
{
	pButtonSelector->MoveDrag(point);
	CDialog::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CNameToolBar dialog
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CNameToolBar, CDialog)
	//{{AFX_MSG_MAP(CNameToolBar)
   	ON_WM_SETCURSOR()
   	ON_WM_CTLCOLOR()
  	ON_BN_CLICKED(IDC_LARGEBUTTON, OnLargeButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNameToolBar::OnLargeButton()
{
	m_LargeIcons = !m_LargeIcons;
	CheckDlgButton(IDC_LARGEBUTTON, m_LargeIcons);
}

CNameToolBar::CNameToolBar(CString *Name, COCToolBar *pBar)
	: CDialog(CNameToolBar::IDD, NULL)
{
	m_pNameVal = Name;
	if (pBar)
	{
		m_pBar	= pBar; // true is option dialog box, FALSE if Naming Dialog
		m_LargeIcons = m_pBar->GetLargeIcons();
	}
	else
		m_pBar = NULL;
}

CNameToolBar::~CNameToolBar()
{
}

BOOL CNameToolBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   return Dialog_OnSetCursor(GetSafeHwnd(), pWnd->GetSafeHwnd(), nHitTest, message);
}

HBRUSH CNameToolBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
   return Dialog_OnCtlColor(GetSafeHwnd(), pDC->GetSafeHdc(), pWnd->GetSafeHwnd(), nCtlColor);
}

extern STRING szAppName;
void CNameToolBar::OnOK()
{
	CPPToolList*	pList = PictPubApp.GetCustomBoxes();
	CDataExchange   dx(this, TRUE);
   	POSITION        Pos;
	COCToolBar*		pBar;

	DDX_Text(&dx, IDC_TOOLNAME, *m_pNameVal);
	DDV_MaxChars(&dx, *m_pNameVal, 256);
   	Pos = pList->FindToolBox(*m_pNameVal);
	if (Pos)
		pBar = pList->m_List.GetAt(Pos);
   	if ((!Pos || pBar == m_pBar) && m_pNameVal->GetLength())
	{
		CDialog::OnOK();
		if (m_pBar)
			m_pBar->SetLargeIcon(m_LargeIcons);
	}
   	else
    {
    	LPTSTR    lp;
      	BOOL     ret;

		if (pBar == pList->GetMain())
		{
	    	AstralStr(IDS_CANTREPLACEMAIN, &lp);
         	::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, MB_ICONSTOP | MB_OK | MB_TASKMODAL);
		}
		else if (pBar == pList->GetStatus())
		{
	    	AstralStr(IDS_CANTREPLACESTATUS, &lp);
         	::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, MB_ICONSTOP | MB_OK | MB_TASKMODAL);
		}
		else if (pBar == pList->GetRibbon())
		{
	    	AstralStr(IDS_CANTREPLACERIBBON, &lp);
         	::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, MB_ICONSTOP | MB_OK | MB_TASKMODAL);
		}
      	else if (Pos)
        {
	    	AstralStr(IDS_CUSTNAMEDUP, &lp);
         	ret = ::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, 
            	MB_ICONQUESTION | MB_OKCANCEL | MB_TASKMODAL) == IDOK;
         	if (ret)
            {
	        	CDialog::OnOK();
				if (pBar)
					pBar->SetLargeIcon(m_LargeIcons);
            	pList->Delete(Pos);
            }
        }
      	else
        {
	    	AstralStr(IDS_CUSTNAMEINVALID, &lp);
         	::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, MB_OK |
                      MB_TASKMODAL);
        }
    }
}

void CNameToolBar::OnCancel()
{
	CDialog::OnCancel();
}

BOOL CNameToolBar::OnInitDialog()
{

    CenterPopup(GetSafeHwnd());
    CEdit *eptr = ( CEdit *)GetDlgItem( IDC_TOOLNAME );
    if( eptr )
        eptr->LimitText( MAX_EXTNAME_LEN-1 );

    if (m_pBar)
    {
    	CString  Caption;
		LPTSTR	 lp;

		GetDlgItem(IDC_LARGEBUTTON)->ShowWindow(SW_SHOW);
        AstralStr(IDS_RENAMECUST, &lp);
		Caption = lp;
        SetWindowText(Caption);
		eptr->SetWindowText(*m_pNameVal);
		CheckDlgButton(IDC_LARGEBUTTON, m_LargeIcons);
    }
	else
		GetDlgItem(IDC_LARGEBUTTON)->ShowWindow(SW_HIDE);
		
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNameToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditCustTool)
	DDX_Control(pDX, IDC_TOOLNAME, m_ToolName);
	DDX_Text(pDX, IDC_TOOLNAME, *m_pNameVal);
	DDV_MaxChars(pDX, *m_pNameVal, 256);
	//}}AFX_DATA_MAP
}

