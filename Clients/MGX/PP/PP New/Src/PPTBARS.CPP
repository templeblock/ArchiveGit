#include "pp.h"
#include "afxpriv.h"
#include "id.h"
#include "ppafx.h"
#include "pptbars.h"
#include "ipframe.h"
#include "ctxhelp.h"
#include "pickpal.h"
#include "ctllib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

void MeasurePopupMenu( LPMEASUREITEMSTRUCT lpMeasure );
void DrawPopupMenu( const DRAWITEMSTRUCT FAR * lpDraw );

static HMENU InitPopupTools( void );
static void PopupTools( HWND hWnd, ITEMID idTool, HWND hTool);
static void NotifyOfActiveTool(int cat, int id); // will be replaced with a Tool Forum
static void SendToRibbon(int id);
static void StretchBottombar( HWND hWindow , CRect Newr);

static HMENU hPopup;
static HMENU hMenu;
static WORD idMaskFunction, idRetouchFunction, idFilterFunction,
	    idFillFunction, idDrawFunction;
/////////////////////////////////////////////////////////////////////////////
// COCWinButton
/////////////////////////////////////////////////////////////////////////////

COCWinButton::COCWinButton()
{
}

void COCWinButton::Serialize(CArchive &ar)
{
	COCButton::Serialize(ar);
}

COCWinButton::COCWinButton(UINT id, CString Str, CString CName, DWORD Style, CSize S)
{
	nID			= id;
	nStyle		= TBBS_CONTROL;
	x			= 0;
	y			= 0;
	IDMasterBM	= Style | WS_CHILD | WS_VISIBLE;
	Size		= S;
	iImage		= 0;
	Text		= Str;
	Origin      = CPoint(0,0);
	ClassName	= CName;
	hWnd		= NULL;
}

COCButton* COCWinButton::Duplicate()
{
	COCWinButton*	pBt = new COCWinButton(nID, Text, ClassName, IDMasterBM, Size);
	return(pBt);
}

BOOL COCWinButton::Create(CWnd *pParent)
{
	int		h = (ClassName=="COMBOBOX")? 100 : Size.cy;
	CRect	r;

	hWnd = CreateWindow(ClassName, Text, IDMasterBM, 0, 0, Size.cx-WINDOWSEPARATOR*2, h, pParent->GetSafeHwnd(), (HMENU)nID, PictPubApp.GetResourceHandle(), NULL);
	GetWindowRect(hWnd, &r);
	Size.cy = r.Height();
	SendMessage(hWnd, WM_SETFONT, (WPARAM)OCToolList->hfontSys.GetSafeHandle(), TRUE);
	Init(pParent);

	return(TRUE);
}

void  COCWinButton::CalcSize(BOOL Large)
{
}

void COCWinButton::Move(CPoint Pt, BOOL Horz)
{
	BOOL	Combo = ClassName=="COMBOBOX";
	int		h = Combo? 100 : Size.cy;
	int		x = Pt.x;
	int		y = Pt.y;
	int		w = Size.cx;

	Origin = Pt;
	if (Horz)
	{
		if (!(nStyle & TBBS_NEWLINE) && Combo)
		{
			x += WINDOWSEPARATOR;
			w -= WINDOWSEPARATOR * 2;
		}
	}
	SetWindowPos(hWnd, NULL, x, y, w, h, SWP_NOZORDER|SWP_NOACTIVATE);
}

BOOL COCWinButton::Notify(int nCode)
{
	((CPPToolList*)OCToolList)->SetDlgCtrl(GetParent(hWnd), hWnd, nCode);
	switch(nID)
	{
	case IDC_TEXT_FACES:
		FontCombo.NotifyFontChanged(nCode);
		break;
	case IDC_ZOOM_COMBO:
		m_ZoomCombo.NotifyChanged(nCode);
		break;
	default:
		return (FALSE);
	}
	return(TRUE);
}

BOOL COCWinButton::Init(CWnd *pParent)
{
	switch(nID)
	{
	case IDC_STATUSTEXT:
		m_StatusMonitor.Init(hWnd);
		break;
	case IDC_TEXT_FACES:
		FontCombo.Init(hWnd);
		break;
	case IDC_ZOOM_COMBO:
		m_ZoomCombo.Init(hWnd);
		break;
	default:
		break;
	}
return(TRUE);
}

COCWinButton::~COCWinButton()
{
	if (hWnd)
		FORWARD_WM_CLOSE(hWnd, ::SendMessage);
	switch(nID)
	{
	case IDC_STATUSTEXT:
		m_StatusMonitor.Init(NULL);
		break;
	case IDC_TEXT_FACES:
		FontCombo.Init(NULL);
		break;
	case IDC_ZOOM_COMBO:
		m_ZoomCombo.Init(NULL);
		break;
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPPToolList
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CPPToolList, CCmdTarget)
BEGIN_MESSAGE_MAP(CPPToolList, CCmdTarget)
   ON_BN_CLICKED(IDC_MASK,          OnMaskClick)
   ON_BN_DOUBLECLICKED(IDC_MASK,    OnMaskDblClk)
   ON_BN_CLICKED(IDC_RETOUCH,       OnRetouchClick)
   ON_BN_DOUBLECLICKED(IDC_RETOUCH, OnRetouchDblClk)
   ON_BN_CLICKED(IDC_FILTER,        OnFilterClick)
   ON_BN_DOUBLECLICKED(IDC_FILTER,  OnFilterDblClk)
   ON_BN_CLICKED(IDC_FILL,          OnFillClick)
   ON_BN_DOUBLECLICKED(IDC_FILL,    OnFillDblClk)
   ON_BN_CLICKED(IDC_DRAW,          OnDrawClick)
   ON_BN_DOUBLECLICKED(IDC_DRAW,    OnDrawDblClk)

   ON_BN_CLICKED(IDC_SELECTOR,      OnSelector)
   ON_BN_CLICKED(IDC_ZOOM,    	    OnZoom)
   ON_BN_CLICKED(IDC_PROBE,    	    OnProbe)
   ON_BN_CLICKED(IDC_TEXT,          OnText)

   ON_BN_CLICKED(IDC_FREE,          OnMaskFree)
   ON_BN_CLICKED(IDC_SQUARE,        OnMaskSquare)
   ON_BN_CLICKED(IDC_CIRCLE,        OnMaskCircle)
   ON_BN_CLICKED(IDC_PAINTON,       OnMaskPainton)
   ON_BN_CLICKED(IDC_MAGICWAND,     OnMaskMagic)
   ON_BN_CLICKED(IDC_SHIELD,        OnMaskShield)
   ON_BN_CLICKED(IDC_TRANSFORMER,   OnMaskTransformer)
   ON_BN_CLICKED(IDC_POINTER,       OnMaskPointer)
   ON_BN_CLICKED(IDC_CROP,          OnMaskCrop)

   ON_BN_CLICKED(IDC_PAINT,         OnRetouchPaint)
   ON_BN_CLICKED(IDC_SPRAY,         OnRetouchSpray)
   ON_BN_CLICKED(IDC_CLONE,         OnRetouchClone)
   ON_BN_CLICKED(IDC_TEXTURE,       OnRetouchTexture)
   ON_BN_CLICKED(IDC_SMEAR,         OnRetouchSmear)
   ON_BN_CLICKED(IDC_ERASER,        OnRetouchEraser)

   ON_BN_CLICKED(IDC_SHARP,         OnFilterSharp)
   ON_BN_CLICKED(IDC_SMOOTH,        OnFilterSmooth)
   ON_BN_CLICKED(IDC_LIGHTEN,       OnFilterLighten)
   ON_BN_CLICKED(IDC_DARKEN,        OnFilterDarken)

   ON_BN_CLICKED(IDC_VIGNETTE,      OnFillVignette)
   ON_BN_CLICKED(IDC_TEXTUREFILL,   OnFillTexture)
   ON_BN_CLICKED(IDC_TINTFILL,      OnFillTintFill)
   ON_BN_CLICKED(IDC_FLOOD,         OnFillFlood)

   ON_BN_CLICKED(IDC_PENCIL,        OnDrawPencil)
   ON_BN_CLICKED(IDC_DRAWSQUARE,    OnDrawSquare)
   ON_BN_CLICKED(IDC_DRAWFREE,      OnDrawFree)
   ON_BN_CLICKED(IDM_LASTTOOL,      OnLastTool)

	// for ribbon stuff
	ON_UPDATE_COMMAND_UI(IDC_FREEHAND	,OnUpdateRMCheck)
	ON_UPDATE_COMMAND_UI(IDC_AUTOMASK	,OnUpdateRMCheck)
	ON_UPDATE_COMMAND_UI(IDC_POINTMODE	,OnUpdateRMCheck)
	ON_UPDATE_COMMAND_UI(IDC_DRAW_LINE	,OnUpdateRMEnableAndCheck)
	ON_UPDATE_COMMAND_UI(IDC_DRAW_BEZIER,OnUpdateRMEnableAndCheck)
	ON_UPDATE_COMMAND_UI(IDC_IS_BEZIER,OnUpdateRMEnable)
	ON_UPDATE_COMMAND_UI(IDC_IS_POINT ,OnUpdateRMEnable)
	ON_UPDATE_COMMAND_UI(IDC_MOVEPOINTS,OnUpdateRMEnableAndCheck)
	ON_UPDATE_COMMAND_UI(IDC_ADDPOINTS ,OnUpdateRMEnableAndCheck)
	ON_UPDATE_COMMAND_UI(IDC_DELPOINTS ,OnUpdateRMEnableAndCheck)
	ON_UPDATE_COMMAND_UI(IDC_ZOOM_COMBO, OnUpdateZoomCombo)

	ON_COMMAND(IDC_FREEHAND	  ,OnFreeHand  )
	ON_COMMAND(IDC_AUTOMASK	  ,OnAutoMask  )
	ON_COMMAND(IDC_DRAW_LINE  ,OnDrawLine  )
	ON_COMMAND(IDC_DRAW_BEZIER,OnDrawBezier)
	ON_COMMAND(IDC_IS_BEZIER  ,OnIsBezier  )
	ON_COMMAND(IDC_IS_POINT   ,OnIsPoint   )
	ON_COMMAND(IDC_MOVEPOINTS ,OnMovePoints)
	ON_COMMAND(IDC_ADDPOINTS  ,OnAddPoints )
	ON_COMMAND(IDC_DELPOINTS  ,OnDelPoint  )
	ON_COMMAND(IDC_POINTMODE  ,OnPointMode )
END_MESSAGE_MAP()

CPPToolList::CPPToolList()
{
	m_pStatus = NULL;
	m_pRibbon = NULL;
	m_pMain = NULL;
	m_hDlg = m_hControl = NULL;
}

CPPToolList::~CPPToolList()
{
	Save();
}

BOOL CPPToolList::IsMainUp()
{
   	if (m_pMain && m_pMain->IsToolBarUp())
    	return(TRUE);
   	return(FALSE);
}

void CPPToolList::ShowBars(BOOL bShow)
{
	POSITION      Pos;
   	CPPToolBar   *pToolBar;

	for (Pos = m_List.GetHeadPosition(); Pos != NULL ;)
	{
		pToolBar = (CPPToolBar *)m_List.GetNext(Pos);
		if (pToolBar && /*!pToolBar->m_IsInsitue && Nezar */ pToolBar->GetSafeHwnd())
			pToolBar->ShowWindow (bShow ? SW_SHOW : SW_HIDE);
	}
}

void CPPToolList::NotifyOfActiveTool(int cat, int id) // will be replaced with a Tool Forum
{
	POSITION        Pos;
   	CPPToolBar*		pToolBar;

   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
   {
      pToolBar = (CPPToolBar *)m_List.GetNext(Pos);
      if (pToolBar->GetSafeHwnd())
      {
         pToolBar->UpdateActiveTool(cat);
         pToolBar->UpdateActiveTool(id);
      }
   }
}

void CPPToolList::UpdateStatusButton(int id, BOOL b)
{
	POSITION        Pos;
   	CPPToolBar   *pToolBar;

   	for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
    {
    	pToolBar = (CPPToolBar *)m_List.GetNext(Pos);
      	if (pToolBar->GetSafeHwnd())
	    	pToolBar->CheckDlgButton(id, b);
    }
}

BOOL CPPToolList::Remove(CPPToolBar *pBox)
   {
   	POSITION        Pos;
   	POSITION        Prev;
   	CPPToolBar   *pToolBar;

   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
      {
	  Prev = Pos;
      pToolBar = (CPPToolBar *)m_List.GetNext(Pos);
      if (pToolBar == pBox)
	  	 {
      	 m_List.RemoveAt(Prev);
         delete pBox;
         return(TRUE);
		 }
      }
   return(FALSE);
   }

void CPPToolList::InvalidateColorPatches(BOOL Erase)
   {
   POSITION        Pos;
   CPPToolBar   *pToolBar;

   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
      {
      pToolBar = (CPPToolBar *)m_List.GetNext(Pos);
	  pToolBar->InvalidateColorPatches(Erase);
      }
   }

void CPPToolList::NotifyColorPatches(int what)
   {
   POSITION        Pos;
   CPPToolBar   *pToolBar;

   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
      {
      pToolBar = (CPPToolBar *)m_List.GetNext(Pos);
	  pToolBar->NotifyColorPatches(what);
      }
   }

afx_msg void CPPToolList::OnLastTool()
    {
    switch (Tool.idLast)
        {
        case IDC_FREE:
            OnMaskTool(IDC_FREE);
            break;
        case IDC_SQUARE:
            OnMaskTool(IDC_SQUARE);
            break;
        case IDC_CIRCLE:
            OnMaskTool(IDC_CIRCLE);
            break;
        case IDC_PAINTON:
            if( Control.Retail )
                OnMaskTool(IDC_PAINTON);
            break;
        case IDC_MAGICWAND:
            OnMaskTool(IDC_MAGICWAND);
            break;
        case IDC_SHIELD:
            OnMaskTool(IDC_SHIELD);
            break;
        case IDC_TRANSFORMER:
            OnMaskTool(IDC_TRANSFORMER);
            break;
        case IDC_POINTER:
            OnMaskTool(IDC_POINTER);
            break;
        case IDC_CROP:
            OnMaskTool(IDC_CROP);
            break;
        case IDC_PAINT:
            OnRetouchTool(IDC_PAINT);
            break;
        case IDC_SPRAY:
            OnRetouchTool(IDC_SPRAY);
            break;
        case IDC_CLONE:
            OnRetouchTool(IDC_CLONE);
            break;
        case IDC_TEXTURE:
            OnRetouchTool(IDC_TEXTURE);
            break;
        case IDC_SMEAR:
            OnRetouchTool(IDC_SMEAR);
            break;
        case IDC_ERASER:
            OnRetouchTool(IDC_ERASER);
            break;
        case IDC_SHARP:
            OnFilterTool(IDC_SHARP);
            break;
        case IDC_SMOOTH:
            OnFilterTool(IDC_SMOOTH);
            break;
        case IDC_LIGHTEN:
            OnFilterTool(IDC_LIGHTEN);
            break;
        case IDC_DARKEN:
            OnFilterTool(IDC_DARKEN);
            break;
        case IDC_VIGNETTE:
            OnFillTool(IDC_VIGNETTE);
            break;
        case IDC_TEXTUREFILL:
            OnFillTool(IDC_TEXTUREFILL);
            break;
        case IDC_TINTFILL:
            OnFillTool(IDC_TINTFILL);
            break;
        case IDC_FLOOD:
            OnFillTool(IDC_FLOOD);
            break;
        case IDC_PENCIL:
            OnDrawTool(IDC_PENCIL);
            break;
        case IDC_DRAWSQUARE:
            OnDrawTool(IDC_DRAWSQUARE);
            break;
        case IDC_DRAWFREE:
            OnDrawTool(IDC_DRAWFREE);
            break;
        default:
            OnSelViewTextProbe(Tool.idLast);
            break;
        }
    }

void CPPToolList::SetDlgCtrl(HWND hDlg, HWND hControl, UINT codeNotify)
   {
   m_hDlg         = hDlg;
   m_hControl     = hControl;
   m_codeNotify   = codeNotify;
   }

afx_msg void CPPToolList::OnMaskClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnMaskDblClk();
   else
      PopupTools(m_hDlg, IDC_MASK, m_hControl);
   }

afx_msg void CPPToolList::OnMaskDblClk()
   {
   NotifyOfActiveTool(IDC_MASK, idMaskFunction);
	ActivateTool(idMaskFunction);
   }

afx_msg void CPPToolList::OnRetouchClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnRetouchDblClk();
   else
      PopupTools(m_hDlg, IDC_RETOUCH, m_hControl);
   }

afx_msg void CPPToolList::OnRetouchDblClk()
   {
   NotifyOfActiveTool(IDC_RETOUCH, idRetouchFunction);
	ActivateTool(idRetouchFunction);
   }

afx_msg void CPPToolList::OnFilterClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnFilterDblClk();
   else
      PopupTools(m_hDlg, IDC_FILTER, m_hControl);
   }

afx_msg void CPPToolList::OnFilterDblClk()
   {
   NotifyOfActiveTool(IDC_FILTER, idFilterFunction);
	ActivateTool(idFilterFunction);
   }

afx_msg void CPPToolList::OnFillClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnFillDblClk();
   else
      PopupTools(m_hDlg, IDC_FILL, m_hControl);
   }

afx_msg void CPPToolList::OnFillDblClk()
   {
   NotifyOfActiveTool(IDC_FILL, idFillFunction);
	ActivateTool(idFillFunction);
   }

afx_msg void CPPToolList::OnDrawClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnDrawDblClk();
   else
      PopupTools(m_hDlg, IDC_DRAW, m_hControl);
   }

afx_msg void CPPToolList::OnDrawDblClk()
   {
   NotifyOfActiveTool(IDC_DRAW, idDrawFunction);
	ActivateTool(idDrawFunction);
   }

afx_msg void CPPToolList::OnZoom()
{
	PopupTools(m_hDlg, IDC_ZOOM, m_hControl);
}

afx_msg void CPPToolList::OnProbe()
{
	PopupTools(m_hDlg, IDC_PROBE, m_hControl);
}

afx_msg void CPPToolList::OnSelector()
   {OnSelViewTextProbe(IDC_SELECTOR);}
afx_msg void CPPToolList::OnText()
   {OnSelViewTextProbe(IDC_TEXT);}
afx_msg void CPPToolList::OnSelViewTextProbe(int ID)
{
	switch (ID)
	{
		case IDC_SELECTOR:
		case IDC_TEXT:
			NotifyOfActiveTool(ID, 0);
			ActivateTool(ID);
		break;

		default:
		break;
	}
}

afx_msg void CPPToolList::OnMaskFree()
   {OnMaskTool(IDC_FREE);}
afx_msg void CPPToolList::OnMaskSquare()
   {OnMaskTool(IDC_SQUARE);}
afx_msg void CPPToolList::OnMaskCircle()
   {OnMaskTool(IDC_CIRCLE);}
afx_msg void CPPToolList::OnMaskPainton()
   {OnMaskTool(IDC_PAINTON);}
afx_msg void CPPToolList::OnMaskMagic()
   {OnMaskTool(IDC_MAGICWAND);}
afx_msg void CPPToolList::OnMaskShield()
   {OnMaskTool(IDC_SHIELD);}
afx_msg void CPPToolList::OnMaskTransformer()
   {OnMaskTool(IDC_TRANSFORMER);}
afx_msg void CPPToolList::OnMaskPointer()
   {OnMaskTool(IDC_POINTER);}
afx_msg void CPPToolList::OnMaskCrop()
   {OnMaskTool(IDC_CROP);}
afx_msg void CPPToolList::OnMaskTool(int ID)
   {
   NotifyOfActiveTool(IDC_MASK, ID);
	CheckMenuItem(hPopup, idMaskFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idMaskFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CPPToolList::OnRetouchPaint()
   {OnRetouchTool(IDC_PAINT);}
afx_msg void CPPToolList::OnRetouchSpray()
   {OnRetouchTool(IDC_SPRAY);}
afx_msg void CPPToolList::OnRetouchClone()
   {OnRetouchTool(IDC_CLONE);}
afx_msg void CPPToolList::OnRetouchTexture()
   {OnRetouchTool(IDC_TEXTURE);}
afx_msg void CPPToolList::OnRetouchSmear()
   {OnRetouchTool(IDC_SMEAR);}
afx_msg void CPPToolList::OnRetouchEraser()
   {OnRetouchTool(IDC_ERASER);}
afx_msg void CPPToolList::OnRetouchTool(int ID)
   {
   NotifyOfActiveTool(IDC_RETOUCH, ID);
	CheckMenuItem(hPopup, idRetouchFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idRetouchFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CPPToolList::OnFilterSharp()
   {OnFilterTool(IDC_SHARP);}
afx_msg void CPPToolList::OnFilterSmooth()
   {OnFilterTool(IDC_SMOOTH);}
afx_msg void CPPToolList::OnFilterLighten()
   {OnFilterTool(IDC_LIGHTEN);}
afx_msg void CPPToolList::OnFilterDarken()
   {OnFilterTool(IDC_DARKEN);}
afx_msg void CPPToolList::OnFilterTool(int ID)
   {
   NotifyOfActiveTool(IDC_FILTER, ID);
	CheckMenuItem(hPopup, idFilterFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idFilterFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CPPToolList::OnFillVignette()
   {OnFillTool(IDC_VIGNETTE);}
afx_msg void CPPToolList::OnFillTexture()
   {OnFillTool(IDC_TEXTUREFILL);}
afx_msg void CPPToolList::OnFillTintFill()
   {OnFillTool(IDC_TINTFILL);}
afx_msg void CPPToolList::OnFillFlood()
   {OnFillTool(IDC_FLOOD);}
afx_msg void CPPToolList::OnFillTool(int ID)
   {
   NotifyOfActiveTool(IDC_FILL, ID);
	CheckMenuItem(hPopup, idFillFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idFillFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CPPToolList::OnDrawPencil()
   {OnDrawTool(IDC_PENCIL);}
afx_msg void CPPToolList::OnDrawSquare()
   {OnDrawTool(IDC_DRAWSQUARE);}
afx_msg void CPPToolList::OnDrawFree()
   {OnDrawTool(IDC_DRAWFREE);}
afx_msg void CPPToolList::OnDrawTool(int ID)
   {
   NotifyOfActiveTool(IDC_DRAW, ID);
	CheckMenuItem(hPopup, idDrawFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idDrawFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CPPToolList::OnUpdateZoomCombo(CCmdUI* pCmdUI)
{
	CServerView    *pView = PictPubApp.GetActiveView();
	BOOL state = IsCmdEnabled(0, pView);
	if (m_Customize)
		state = FALSE;
	pCmdUI->Enable(state);
}

afx_msg void CPPToolList::OnUpdateRMCheck(CCmdUI* pCmdUI)
   {
   CServerView    *pView = PictPubApp.GetActiveView();
   CInPlaceFrame  *pIPFrame;
   HWND            hRibbon = Tool.hRibbon;

   if (pView && pView->GetDocument()->IsInPlaceActive())
      {
	   pIPFrame = (CInPlaceFrame*)pView->GetParentFrame();
      hRibbon  = pIPFrame->GetCurrRibbon();
      }

	if(hRibbon) 
      {
	   if (IsDlgButtonChecked(hRibbon, pCmdUI->m_nID) > 0)
		   pCmdUI->SetCheck( TRUE );
	   else
		   pCmdUI->SetCheck( FALSE );
	   pCmdUI->Enable( TRUE );
      }
   }

afx_msg void CPPToolList::OnUpdateRMEnable(CCmdUI* pCmdUI)
   {
	HWND            hControl;
   CServerView    *pView = PictPubApp.GetActiveView();
   CInPlaceFrame  *pIPFrame;
   HWND            hRibbon = Tool.hRibbon;

   if (pView && pView->GetDocument()->IsInPlaceActive())
      {
	   pIPFrame = (CInPlaceFrame*)pView->GetParentFrame();
      hRibbon  = pIPFrame->GetCurrRibbon();
      }

	if(hRibbon) 
      {
	   // enable item if corresponding item in ribbon is enabled
	   if (hControl = GetDlgItem(hRibbon, pCmdUI->m_nID))
	      pCmdUI->Enable( IsWindowEnabled(hControl) );
      }
   }

afx_msg void CPPToolList::OnUpdateRMEnableAndCheck(CCmdUI* pCmdUI)
   {
	HWND            hControl;
   CServerView    *pView = PictPubApp.GetActiveView();
   CInPlaceFrame  *pIPFrame;
   HWND            hRibbon = Tool.hRibbon;

   if (pView && pView->GetDocument()->IsInPlaceActive())
      {
	   pIPFrame = (CInPlaceFrame*)pView->GetParentFrame();
      hRibbon  = pIPFrame->GetCurrRibbon();
      }

	if(hRibbon) 
      {
	   // enable item if corresponding item in ribbon is enabled
	   // Check item if corresponding item in ribbon is checked
	   if (hControl = GetDlgItem(hRibbon, pCmdUI->m_nID))
         {
	      pCmdUI->Enable( IsWindowEnabled(hControl) );
	      if (IsDlgButtonChecked(hRibbon, pCmdUI->m_nID) > 0)
		      pCmdUI->SetCheck( TRUE );
	      else
		      pCmdUI->SetCheck( FALSE );
         }
      }
   }

afx_msg void CPPToolList::OnFreeHand()
   {
   SendToRibbon(IDC_FREEHAND);
   }
afx_msg void CPPToolList::OnAutoMask()
   {
   SendToRibbon(IDC_AUTOMASK);
   }
afx_msg void CPPToolList::OnDrawLine()
   {
   SendToRibbon(IDC_DRAW_LINE);
   }
afx_msg void CPPToolList::OnDrawBezier()
   {
   SendToRibbon(IDC_DRAW_BEZIER);
   }
afx_msg void CPPToolList::OnIsBezier()
   {
   SendToRibbon(IDC_IS_BEZIER);
   }
afx_msg void CPPToolList::OnIsPoint()
   {
   SendToRibbon(IDC_IS_POINT);
   }
afx_msg void CPPToolList::OnMovePoints()
   {
   SendToRibbon(IDC_MOVEPOINTS);
   }
afx_msg void CPPToolList::OnAddPoints()
   {
   SendToRibbon(IDC_ADDPOINTS);
   }
afx_msg void CPPToolList::OnDelPoint()
   {
   SendToRibbon(IDC_DELPOINTS);
   }
afx_msg void CPPToolList::OnPointMode()
   {
   SendToRibbon(IDC_POINTMODE);
   }

COCToolBar* CPPToolList::NewToolBar()
{
	COCToolBar*		pBar = new CPPToolBar;
	return(pBar);
}

CPPToolBar* CPPToolList::GetMain()
{
	if (!m_pMain)
		Load((CFrameWnd*)PictPubApp.m_pMainWnd);
	return(m_pMain);
}

COCRibbon*	CPPToolList::GetRibbon()
{
	if (!m_pRibbon)
		Load((CFrameWnd*)PictPubApp.m_pMainWnd);
	return(m_pRibbon);
}

COCStatus*	CPPToolList::GetStatus()
{
	if (!m_pStatus)
		Load((CFrameWnd*)PictPubApp.m_pMainWnd);
	return(m_pStatus);
}

BOOL CPPToolList::Save()
{
	CFile* pFile = GetFile(m_FileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
	if (pFile == NULL)
		return FALSE;
	CArchive Ar(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
	TRY
	{
		m_pMain->Serialize(Ar);
		m_pRibbon->Serialize(Ar);
		m_pStatus->Serialize(Ar);
		m_List.RemoveAt(m_List.Find(m_pMain));
		m_List.RemoveAt(m_List.Find(m_pRibbon));
		m_List.RemoveAt(m_List.Find(m_pStatus));
		Serialize(Ar);     // load me
		// order here is important.  it is assumed that they exist in this order
		m_List.AddHead(m_pStatus);
		m_List.AddHead(m_pRibbon);
		m_List.AddHead(m_pMain);
		Ar.Close();
		pFile->Close();
		delete pFile;
	}
	CATCH_ALL(e)
	{
		pFile->Abort(); // will not throw an exception
		delete pFile;
		return FALSE;
	}
	END_CATCH_ALL
	return(TRUE);
}

void CPPToolList::ResetStatus(CFrameWnd *pFrame, CWnd *pOwner)
{
	COCStatus*		pBar = new COCStatus;
	POSITION		Pos;

	pBar->LoadStatus();
	m_pStatus->CloseToolBar();
	if (pBar->GetStyle2() & OCTOOLS_ISUP)
		pBar->ShowToolBar(pFrame, pOwner);
	if (pBar->IsFloating())
		pBar->GetDockingFrame()->EnableWindow(FALSE);
	Pos = m_List.Find(m_pStatus);
	m_List.SetAt(Pos, pBar);
	delete m_pStatus;
	m_pStatus = pBar;

}

void CPPToolList::ResetMain(CFrameWnd *pFrame, CWnd *pOwner)
{
	CPPToolBar*		pBar = new CPPToolBar;
	POSITION		Pos;

	pBar->LoadMain();
	if (pBar->GetStyle2() & OCTOOLS_ISUP)
		pBar->ShowToolBar(pFrame, pOwner);
	if (pBar->IsFloating())
		pBar->GetDockingFrame()->EnableWindow(FALSE);
#ifdef DEMO
	pBar->EnableDocking(0);
#endif
	m_pMain->CloseToolBar();
	Pos = m_List.Find(m_pMain);
	m_List.SetAt(Pos, pBar);
	delete m_pMain;
	m_pMain = pBar;
}

BOOL CPPToolList::Load(CFrameWnd *pFrame, CWnd *pOwner)
{
	CFile* 	pFile;
	BOOL	FailedLoading = TRUE;

	if (m_pMain)
		return(TRUE);

	m_pFrame = pFrame;
   	hPopup = InitPopupTools();
	pFrame->EnableDocking(CBRS_ALIGN_ANY);
	m_pMain = new CPPToolBar;
	m_pRibbon = new COCRibbon;	
	m_pStatus = new COCStatus;	
	pFile = GetFile(m_FileName, CFile::modeRead | CFile::shareDenyWrite);
	if (pFile)
	{
		CArchive Ar(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
		TRY
		{
			m_pMain->Serialize(Ar);
			m_pRibbon->Serialize(Ar);
			m_pStatus->Serialize(Ar);
			Serialize(Ar);
			Ar.Close();
			pFile->Close();
			delete pFile;
			FailedLoading = FALSE;
		}
		CATCH_ALL(e)
		{
			pFile->Abort(); // will not throw an exception
			delete pFile;
			// error Message Nezar
		}
		END_CATCH_ALL
	}
	if (FailedLoading)
	{
		m_pMain->LoadMain();
		m_pRibbon->LoadRibbon();
		m_pStatus->LoadStatus();
	}
	// order here is important.  it is assumed that they exist in this order
	if (m_pStatus->GetStyle2() & OCTOOLS_ISUP)
		m_pStatus->ShowToolBar(pFrame, pOwner);
	if (m_pRibbon->GetStyle2() & OCTOOLS_ISUP)
		m_pRibbon->ShowToolBar(pFrame, pOwner);
	if (m_pMain->GetStyle2() & OCTOOLS_ISUP)
		m_pMain->ShowToolBar(pFrame, pOwner);
#ifdef DEMO
	m_pMain->EnableDocking(0);
#endif
	m_List.AddHead(m_pStatus);
	m_List.AddHead(m_pRibbon);
	m_List.AddHead(m_pMain);

	return(TRUE);
}

CFile* CPPToolList::GetFile(LPCTSTR lpszFileName, UINT nOpenFlags)
{
	CFile* pFile = new CFile;
	ASSERT(pFile != NULL);
	if (!pFile->Open(lpszFileName, nOpenFlags))
	{
		delete pFile;
		pFile = NULL;
	}
	return pFile;
}

/////////////////////////////////////////////////////////////////////////////
// CPPToolBar
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPPToolBar, COCToolBar)
	//{{AFX_MSG_MAP(CPPToolBar)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_INITMENUPOPUP()
#ifndef DEMO
	ON_WM_RBUTTONDOWN()
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
IMPLEMENT_DYNAMIC(CPPToolBar, COCToolBar)

CPPToolBar::CPPToolBar()
{
	m_Style2 |= OCTOOLS_ISUP;
	m_BarRect = CRect(100, 100, 100, 100);
}

CPPToolBar::~CPPToolBar()
{
}

afx_msg void CPPToolBar::OnOK()
{
	CServerView    *pView = PictPubApp.GetActiveView();
	if (pView)
		pView->SetFocus();
	else
		AfxGetMainWnd()->SetFocus();
}

afx_msg void CPPToolBar::OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex,
										 BOOL bSysMenu )
{
	if (PictPubApp.m_pMainWnd)
	{
		HWND hWnd = PictPubApp.m_pMainWnd->GetSafeHwnd();
		FORWARD_WM_INITMENUPOPUP(hWnd, pPopupMenu->GetSafeHmenu(), nIndex, bSysMenu, ::SendMessage);
	}
}

void CPPToolBar::UpdateActiveTool(int id)
{
   if (!GetSafeHwnd())
      return;
   if (id >= IDC_FIRSTTOOL && id <= IDC_LASTTOOL)
      CheckRadioButton(IDC_FIRSTTOOL, IDC_LASTTOOL, id);
   else
      CheckRadioButton(IDC_FIRSTICON, IDC_LASTICON, id);
}

void CPPToolBar::OnActiveColor(HWND hDlg, int id, UINT codeNotify)
{
	BOOL        fDoubleClick = (codeNotify == 1);
   	COLORINFO   ColorInfo, AltColorInfo;
   	LPIMAGE     lpImage;
   	FRMTYPEINFO TypeInfo;
	HWND		ColorPatch;

   	switch (codeNotify)
    {
    case 1:        // Double Click Bring up the color picker
	    GetActiveColor(&ColorInfo);
	    GetAlternateColor(&AltColorInfo);
	    lpImage = GetActiveImage();
	    if (lpImage)
		    ImgGetTypeInfo(lpImage, &TypeInfo);
	    else
		    FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);

	    if (TypeInfo.DataType == FDT_LINEART) 
          break;
	    else if (TypeInfo.DataType == FDT_GRAYSCALE || TypeInfo.DataType == FDT_PALETTECOLOR) 
		{
			if (!PalettePicker(&ColorInfo, &AltColorInfo, TypeInfo.ColorMap))
				return;
		}
	    else if (!ColorPicker(&ColorInfo, &AltColorInfo))
			return;
	    SetActiveColor( (LPCOLORINFO)&ColorInfo, ColorInfo.ColorSpace, FALSE);
	    SetAlternateColor( (LPCOLORINFO)&AltColorInfo, AltColorInfo.ColorSpace, FALSE);
        break;
    case 3:
        PictPubApp.GetCustomBoxes()->InvalidateColorPatches(TRUE);
        break;
    case 2:              // The "2" message is sent from the probe and the palette.
    case 4:
		ColorPatch = GetColorPatch();
	  	if (ColorPatch)
      		::InvalidateRect(ColorPatch, NULL, FALSE);
        break;
     default:
        break;
    }
}

HWND CPPToolBar::GetColorPatch()
{
	int				i = CommandToIndex(IDC_ACTIVECOLOR);
	HWND			hWnd = NULL;
	if (i != -1)
		hWnd = GetButtonPtr(i)->hWnd;
	return(hWnd);
}

void CPPToolBar::InvalidateColorPatches(BOOL Erase)
{
	HWND	ColorPatch = GetColorPatch();
	if (GetSafeHwnd() && ColorPatch)
    	::InvalidateRect(ColorPatch, NULL, Erase);
}

void CPPToolBar::NotifyColorPatches(int what)
{
	HWND	ColorPatch = GetColorPatch();
	if (ColorPatch)
		FORWARD_WM_COMMAND(GetSafeHwnd(), IDC_ACTIVECOLOR, NULL, what, ::SendMessage);
}

void CPPToolBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItem)
{
   DrawPopupMenu(lpDrawItem);
}

void CPPToolBar::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItem)
{
   MeasurePopupMenu(lpMeasureItem);
}

extern HACCEL		hAccelTable;
BOOL CPPToolBar::PreTranslateMessage(MSG* pMsg)
{
   if (TranslateAccelerator( AfxGetMainWnd()->GetSafeHwnd(), hAccelTable, (LPMSG)pMsg ) )
      return(TRUE);

	if (COCToolBar::PreTranslateMessage(pMsg))
		return(TRUE);

   return(FALSE); // FALSE if the message should be processed in the normal way
}

LRESULT CPPToolBar::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	int		i;
	int		id 		= GET_WM_COMMAND_ID(wParam, lParam);
	UINT	nCode 	= GET_WM_COMMAND_CMD(wParam, lParam);
	switch (nMsg)
	{
	case WM_COMMAND:
		if (id == IDOK)
		{
			OnOK();
			return(0);
		}
		i = CommandToIndex(id);
		if (i != -1 && GetButtonPtr(i)->Notify(nCode))
			return(-1);
        if (id == IDC_QUICKZOOM)
        	wParam = IDM_SHOWZOOMBOX;
		else if (id == IDC_ACTIVECOLOR)
			OnActiveColor(GetSafeHwnd(), id, nCode);
// Nezar         HANDLE_WM_COMMAND(GetSafeHwnd(), wParam, lParam, FindTool);
		break;
	case WM_DRAWITEM:
		OnDrawItem((int)wParam, (LPDRAWITEMSTRUCT)lParam);
		break;
	case WM_MEASUREITEM:
		OnMeasureItem((int)wParam, (LPMEASUREITEMSTRUCT)lParam);
		break;
	}
	return CControlBar::WindowProc(nMsg, wParam, lParam);
}

void CPPToolBar::OnRButtonDown(UINT nFlags, CPoint pt) 
{
    CMenu   Menu;
    BOOL 	Loaded;
    Loaded = Menu.LoadMenu(IDR_OCTOOLMENU);
    if (Loaded)
	{
	    ClientToScreen(&pt);
	    Menu.GetSubMenu(0)->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
	}	
}

BOOL CPPToolBar::LoadMain()
{
	COCButton**		pTBB;
    LPTSTR       	lp;
	CSize			Size(32,32);
	
	m_Style2 |= OCTOOLS_LARGEICONS;
    AstralStr(IDS_TBARMAIN, &lp);
	m_Name = lp;
	AllocElements(10);
	pTBB = GetButtonPtrPtr(0);
	*pTBB = new COCWinButton(IDC_SELECTOR,"", "icon", WS_TOOL, Size);

	pTBB = GetButtonPtrPtr(1);
	*pTBB = new COCWinButton(IDC_MASK,"", "icon", WS_TOOL, Size);
	pTBB = GetButtonPtrPtr(2);
	*pTBB = new COCWinButton(IDC_RETOUCH,"", "icon", WS_TOOL, Size);
	pTBB = GetButtonPtrPtr(3);
	*pTBB = new COCWinButton(IDC_FILTER,"", "icon", WS_TOOL, Size);
	pTBB = GetButtonPtrPtr(4);
	*pTBB = new COCWinButton(IDC_FILL,"", "icon", WS_TOOL, Size);
	pTBB = GetButtonPtrPtr(5);
	*pTBB = new COCWinButton(IDC_DRAW,"", "icon", WS_TOOL, Size);
	pTBB = GetButtonPtrPtr(6);
	*pTBB = new COCWinButton(IDC_TEXT,"", "icon", WS_TOOL, Size);
	pTBB = GetButtonPtrPtr(7);
	*pTBB = new COCWinButton(IDC_ZOOM,"", "icon", WS_TOOL, Size);
	pTBB = GetButtonPtrPtr(8);
	*pTBB = new COCWinButton(IDC_PROBE,"", "icon", WS_TOOL, Size);
	pTBB = GetButtonPtrPtr(9);
	*pTBB = new COCWinButton(IDC_ACTIVECOLOR,"", "color2", 0, Size);
	m_Spliting = 1;
	SetNumRows(10);
	return(TRUE);
}

// toolbar buttons - IDs are command buttons
static OCBUTTON  buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	{IDM_NEW,  	0,0,IDR_MAINFRAME},
	{IDM_OPEN, 	1,0,IDR_MAINFRAME},
	{IDM_SAVE, 	2,0,IDR_MAINFRAME},
	{IDM_CUT, 	3,0,IDR_MAINFRAME},
	{IDM_COPY,	4,0,IDR_MAINFRAME},
	{IDM_PASTE,	5,0,IDR_MAINFRAME},
	{IDM_PRINT,	6,0,IDR_MAINFRAME},
	{IDM_HELP_CONTEXT,	7,0,IDR_MAINFRAME},
	{IDM_ABOUT,	8,0,IDR_MAINFRAME}
};

BOOL CPPToolBar::Load()
{
	COCButton*		pBt;
	COCWinButton*	pWBt = new COCWinButton(IDC_TEXT_FACES, "Fonts", "COMBOBOX", CBS_DROPDOWNLIST, CSize(150, 100));
	SetButtons(buttons, 11);

	pBt = new COCButton(IDM_PRINT, "Print");
	InsertButton(pBt);
	pBt = new COCButton(IDM_SAVE, "Save");
	InsertButton(pBt);
	InsertButton(pWBt, 10);
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// COCStatus
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(COCStatus, COCToolBar)
	//{{AFX_MSG_MAP(COCStatus)
#ifndef DEMO
	ON_WM_LBUTTONDOWN()
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
IMPLEMENT_DYNAMIC(COCStatus, COCToolBar)

COCStatus::COCStatus()
{
	m_Style2 &= ~(OCTOOLS_DOCKALL | OCTOOLS_ISFLOATING);
	m_Style2 |= OCTOOLS_DOCKBOTTOM | OCTOOLS_ISUP;
}

COCStatus::~COCStatus()
{
	m_StatusMonitor.Init(NULL);
}

void COCStatus::LoadStatus()
{
    LPTSTR       lp;
	
    AstralStr(IDS_TBARSTATUS, &lp);
	m_Name = lp;
	AllocElements(1);
	COCButton** pTBB = GetButtonPtrPtr(0);
	*pTBB = new COCButton();
}

void COCStatus::OnLButtonDown(UINT nFlags, CPoint point)
{
	CControlBar::OnLButtonDown(nFlags, point);
}

CSize COCStatus::GetDockedSize(BOOL bHorz, BOOL Update)
{
	CSize	Size = CSize(4,4);
	COCButton* 	pTBB;
	
	if (!m_nCount)
		return(Size);
	pTBB = GetButtonPtr(0);
	if (Update)
	{
		if (pTBB->hWnd)
		{
			CRect	r;
			if (!IsFloating())
				::GetClientRect( PictPubApp.Get_hWndAstral(), &r );
			else
				r = m_rect;
			StretchBottombar(pTBB->hWnd, r);
			*pTBB = COCButton(pTBB->hWnd);
		}
		pTBB->Move(CPoint(2, 2), TRUE);
	}
	Size += pTBB->Size;
	return(Size);
}

BOOL COCStatus::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	BOOL	ret;

	m_Style2 &= ~OCTOOLS_DOCKALL;
	m_Style2 |= OCTOOLS_DOCKBOTTOM;
	ret = COCToolBar::Create(pParentWnd, dwStyle, nID);
	EnableDocking(CBRS_ORIENT_HORZ);
	return(ret);
}

BOOL COCStatus::AttachRibbon(HWND	hWnd)
{
	COCButton	Br(hWnd);
	COCButton* 	pTBB = GetButtonPtr(0);

	*pTBB = Br;
	::GetWindowRect(pTBB->hWnd, &m_rect );
	::SetParent(pTBB->hWnd, GetSafeHwnd());
	m_StatusMonitor.Init(::GetDlgItem(AstralDlgGet(IDD_STATUS), IDC_STATUSTEXT));
	RecalcLayout();
	return(TRUE);
}

BOOL COCStatus::ShowToolBar(CFrameWnd *pFrame, CWnd *pOwner)
{
	if (!GetSafeHwnd())
	{
		HWND	hDlg;
		if (hDlg = AstralDlgGet(IDD_STATUS))
			AstralDlgEnd(hDlg, TRUE);
        AstralDlg( YES, PictPubApp.GetResourceHandle(), pFrame->GetSafeHwnd(), IDD_STATUS, DlgStatusProc );
	}
	COCToolBar::ShowToolBar(pFrame, pOwner);
	AttachRibbon(AstralDlgGet(IDD_STATUS));
    UpdateStatusBar( YES, YES, YES, YES );
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// COCRibbon
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(COCRibbon, COCToolBar)
	//{{AFX_MSG_MAP(COCRibbon)
#ifndef DEMO
	ON_WM_LBUTTONDOWN()
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
IMPLEMENT_DYNAMIC(COCRibbon, COCToolBar)

COCRibbon::COCRibbon()
{
	m_Style2 &= ~(OCTOOLS_DOCKALL | OCTOOLS_ISFLOATING);
	m_Style2 |= OCTOOLS_DOCKTOP | OCTOOLS_ISUP;
}

void COCRibbon::LoadRibbon()
{
    LPTSTR       lp;
	
    AstralStr(IDS_TBARRIBBON, &lp);
	m_Name = lp;
	AllocElements(1);
	COCButton** pTBB = GetButtonPtrPtr(0);
	*pTBB = new COCButton();
}

void COCRibbon::OnLButtonDown(UINT nFlags, CPoint point)
{
	CControlBar::OnLButtonDown(nFlags, point);
}

BOOL COCRibbon::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	BOOL	ret;

	ret = COCToolBar::Create(pParentWnd, dwStyle, nID);
	EnableDocking(CBRS_ORIENT_HORZ);
	return(ret);
}

CSize COCRibbon::GetDockedSize(BOOL bHorz, BOOL Update)
{
	CSize	Size = CSize(4,4);
	COCButton* 	pTBB;
	
	if (!m_nCount)
		return(Size);
	pTBB = GetButtonPtr(0);
	if (Update)
		pTBB->Move(CPoint(2, 2), TRUE);
	Size += pTBB->Size;
	return(Size);
}

BOOL COCRibbon::AttachRibbon(HWND	hWnd)
{
	COCButton	Br(hWnd);
	COCButton* 	pTBB = GetButtonPtr(0);

	*pTBB = Br;
	::SetParent(pTBB->hWnd, GetSafeHwnd());
	RecalcLayout();
	return(TRUE);
}

/***********************************************************************/
static void StretchBottombar( HWND hWindow , CRect Newr)
/***********************************************************************/
{
int flag, iHalfXBorder, iHalfYBorder;
RECT rect, rClient;
int width, height;
HWND hControl, hPrev;

// get client and window rect to get border size
GetWindowRect( hWindow, &rect );
GetClientRect( hWindow, &rClient );

// get size of border on each side to overlap the borders
iHalfXBorder = ( RectWidth(&rect) - RectWidth(&rClient) ) / 2;
iHalfYBorder = ( RectHeight(&rect) - RectHeight(&rClient) ) / 2;
height = rect.bottom - rect.top;

// enlarge to overlap borders
InflateRect( &Newr, iHalfXBorder, iHalfYBorder );

// calc top of status bar from bottom of app client area
Newr.top = Newr.bottom - height;
flag = ( IsWindowVisible(hWindow) ? 0: SWP_NOREDRAW );
SetWindowPos( hWindow, NULL, Newr.left, Newr.top, Newr.right - Newr.left, height,
	flag | SWP_NOZORDER | SWP_NOACTIVATE );


// Move the controls around
GetClientRect( hWindow, &rClient);
InflateRect(&rClient, 1, 1);
hControl = GetWindow(hWindow, GW_CHILD);
if (hControl)
	hControl = GetWindow(hControl, GW_HWNDLAST);
height = rClient.bottom - rClient.top + 2;
while (hControl)
	{
	hPrev = GetWindow(hControl, GW_HWNDPREV);
	GetWindowRect(hControl, &rect);
	width = rect.right - rect.left;

	// make control tight on all sides
	rect.right = rClient.right + 1;
	if (hPrev)
		rect.left = rect.right - width;
	else
		{
		rect.left = rClient.left - 1;
		width = rect.right - rect.left;
		}
	PlaceAndSizeWindow( hControl, rect.left, rClient.top - 1,
			width, height);
	rClient.right = rect.left;
	hControl = hPrev;
	}
}

void SendToRibbon(int id)
   {
   CServerView    *pView = PictPubApp.GetActiveView();
   CInPlaceFrame  *pIPFrame;
   HWND            hRibbon = Tool.hRibbon;

   if (pView && pView->GetDocument()->IsInPlaceActive())
      {
	   pIPFrame = (CInPlaceFrame*)pView->GetParentFrame();
      hRibbon  = pIPFrame->GetCurrRibbon();
      }

   if (hRibbon)
      FORWARD_WM_COMMAND(hRibbon, id, GetDlgItem(hRibbon,id), 0, SendMessage);
   }

/***********************************************************************/
static void PopupTools( HWND hWnd, ITEMID idTool, HWND hTool)
/***********************************************************************/
{
int iAlign, x;
HMENU hPopup;
RECT Rect;

if ( !hMenu )
	return;

if ( !(hPopup = GetSubMenu( hMenu, idTool - IDC_FIRSTTOOL )) )
	return;

/* Get screen coordinates of the tool */
GetWindowRect( hTool, &Rect );

if ( !DelayIfMouseDown( 500L ) ) // pop out if timeout or button up
	SetKeyState( VK_LBUTTON, YES /*fDown*/, NO /*fToggle*/ );

// Figure out where to popup
// TPM_LEFTALIGN 0x0000
// TPM_RIGHTALIGN 0x0008
if ( Rect.left > GetSystemMetrics(SM_CXSCREEN)/2 )
		{ x = Rect.left; iAlign = 8; }
else	{ x = Rect.right; iAlign = 0; }
/* Draw and track the "floating" popup */
TrackPopupMenu( hPopup, iAlign, x, Rect.top, 0, hWnd, NULL );
}

void OnViewSlime(int id)
{
	int      iMode;
   	LPIMAGE  lpImage;
   	HWND     hDlg;
   	CPPToolList  *CBList    = PictPubApp.GetCustomBoxes();

	hDlg = AstralDlgGet(IDD_STATUS);
	lpImage = GetActiveImage();
	if ( !lpImage )
		return;
	iMode = ImgGetViewMode(lpImage);
	if ( iMode == VM_SLIME )
		ImgSetViewMode( lpImage, VM_NORMAL );
	else
	if ( iMode == VM_NORMAL )
		ImgSetViewMode( lpImage, VM_SLIME );
	CheckDlgButton( hDlg, IDC_VIEWSLIME, (ImgGetViewMode(lpImage) == VM_SLIME) );
   	CBList->UpdateStatusButton(IDC_VIEWSLIME, (ImgGetViewMode(lpImage) == VM_SLIME));
}

void OnStatusMaskEditAlpha(int id)
{
	LPIMAGE  lpImage;
   	HWND     hDlg;
   	CPPToolList  *CBList    = PictPubApp.GetCustomBoxes();

	hDlg = AstralDlgGet(IDD_STATUS);
	lpImage = GetActiveImage();
	if ( !lpImage || ImageIsBusy(lpImage))
		return;
	if (Tool.bActive)
		return;
	ImgSetupMaskEditMode(lpImage);
	CheckDlgButton( hDlg, IDC_MASKEDITALPHA, ImgInMaskEditMode(lpImage) );
   	CBList->UpdateStatusButton(IDC_MASKEDITALPHA, ImgInMaskEditMode(lpImage));
}

void OnStatusPalette(int id)
{
   	BOOL Bool;
   	HWND hWnd;
   	HWND hDlg;
   	CPPToolList  *CBList    = PictPubApp.GetCustomBoxes();

	hDlg = AstralDlgGet(IDD_STATUS);
	hWnd = AstralDlgGet( IDD_PALETTE );
	Bool = ( hWnd && IsWindowVisible( hWnd ) );
	CheckDlgButton( hDlg, IDC_STATUSPALETTE, !Bool );
    CBList->UpdateStatusButton(IDC_STATUSPALETTE, !Bool);
	if ( Bool )
	{ // it's up
		if ( hWnd ) // bring it down
			SendMessage( hWnd, WM_CLOSE, 0, 0L );
	}
	else
	{ // it's down
		if (!hWnd) // bring it up
			AstralDlg(YES, PictPubApp.GetResourceHandle(), 
				PictPubApp.Get_hWndAstral(), IDD_PALETTE, DlgPaletteProc);
		else
			ShowWindow( hWnd, SW_NORMAL );
		UpdateStatusBar( NO, NO, NO, YES );
	}
}

void OnStatusInfo()
{
	// this function should go in CServerView
	CServerView *pView = PictPubApp.GetActiveView();
	if (pView)
	{
	 	CServerDoc *pDocument = pView->GetDocument();  
		if (pDocument)
        	pDocument->DoSummaryInfo();
	}
}

extern void TurnOffShieldPreview();
void OnStatusShield(int id) // hDlg has to be dlg handle for status bar
   {
   BOOL Bool;
   HWND hWnd;
   HWND hDlg;
   CPPToolList  *CBList    = PictPubApp.GetCustomBoxes();

	hDlg = AstralDlgGet(IDD_STATUS);
	hWnd = AstralDlgGet( IDD_SHIELDFLOAT );
	Bool = ( hWnd && IsWindowVisible( hWnd ) );
	CheckDlgButton( hDlg, IDC_STATUSSHIELD, !Bool );
   CBList->UpdateStatusButton(IDC_STATUSSHIELD, !Bool);
	if ( Bool )
		{ // it's up
		if ( hWnd ) // bring it down
         {
         TurnOffShieldPreview();
         ColorMask.On = !ColorMask.IfHidden;
			ShowWindow( hWnd, SW_HIDE );
         }
		}
	else
		{ // it's down
		if ( !hWnd ) // bring it up
			AstralDlg( YES, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(),
				IDD_SHIELDFLOAT, DlgShieldFloatProc );
		else
         {
         ColorMask.On = YES;
			ShowWindow( hWnd, SW_NORMAL );
			}
		UpdateStatusBar( NO, NO, NO, YES );
		}
   }

void MeasurePopupMenu( LPMEASUREITEMSTRUCT lpMeasure )
{
	DWORD dSize;

	dSize = GetMenuCheckMarkDimensions();
	lpMeasure->itemWidth = 32 - LOWORD(dSize);
	lpMeasure->itemHeight = 32;
}

void DrawPopupMenu( const DRAWITEMSTRUCT FAR * lpDraw )
{
	int x, y;
	RECT ClientRect;
	HICON hResource;
	HDC hDC;
	BOOL bFocus, bDown, bGrayed;
	WORD idIcon;
	int offset;

	if (Control.fNewLook)
		offset = 0;
	else
		offset = 2;

	bFocus = NO;
	bDown = (lpDraw->itemState & ODS_SELECTED);
	bGrayed = (lpDraw->itemState & ODS_GRAYED);
	hDC = lpDraw->hDC;
	ClientRect = lpDraw->rcItem;
	idIcon = lpDraw->itemID;

	// Draw the box interior
	if ( !bDown )
		FillRect( hDC, &ClientRect, DrawTools.GetBtnFaceBrush() );
	else if ( Control.fNewLook )
	{
    	HBRUSH hBrush = CreateSolidBrush( GetSysColor(COLOR_HIGHLIGHT) );
		FillRect( hDC, &ClientRect, hBrush );
		DeleteObject(hBrush);
	}
	else
		HilightRect( hDC, &ClientRect, DrawTools.GetBtnGrayBrush() );

	// Compute the client rect center
	x = ( ClientRect.right + ClientRect.left + 1 ) / 2;
	if ( bDown ) x += offset;
	y = ( ClientRect.bottom + ClientRect.top + 1 ) / 2;
	if ( bDown ) y += offset;

	// Draw the icon
	if ( hResource = LoadIcon( hInstIconLib, MAKEINTRESOURCE(idIcon) ) )
		DrawIcon( hDC, x-16, y-16, hResource );

	if (!Control.fNewLook)
	{
		InflateRect( &ClientRect, 1, 1 );
		DrawSculptedBox( hDC, &ClientRect, bFocus, bDown, YES );
	}
	if (bGrayed)
	{
		ClientRect = lpDraw->rcItem;
		if (bDown)
			InflateRect( &ClientRect, -1, -1);
		else		
			InflateRect( &ClientRect, -2, -2);
		MgxGrayArea(hDC, ClientRect.left, ClientRect.top, ClientRect.right-ClientRect.left,
			ClientRect.bottom-ClientRect.top);
	}
	if ( bDown )
    {
    	HelpContext.OnHints( NULL, 0 );
		HintLine( idIcon );
    }
}

extern BOOL fAppActive;
void InstallTool( BOOL fAlternate )
{
	WORD idCursor, idRibbon;
	BOOL fDisplaysInfo, fSetCursor, fConstrain, fAutoScrollSetPos;
	DLGPROC lpRibbon;
	LPTOOLPROC lpTool;

	if (fAlternate)
	{
		lpTool   = Tool.Alt.lpToolProc;
		idCursor = Tool.Alt.idCursor;
		idRibbon = Tool.Alt.idRibbon;
		lpRibbon = Tool.Alt.lpRibbonProc;
		fSetCursor = Tool.Alt.fSetCursor;
		fDisplaysInfo = Tool.Alt.fDisplaysInfo;
		fConstrain = Tool.Alt.fConstrain;
		fAutoScrollSetPos = Tool.Alt.fAutoScrollSetPos;
	}
	else
	{
		lpTool   = Tool.Prim.lpToolProc;
		idCursor = Tool.Prim.idCursor;
		idRibbon = Tool.Prim.idRibbon;
		lpRibbon = Tool.Prim.lpRibbonProc;
		fSetCursor = Tool.Prim.fSetCursor;
		fDisplaysInfo = Tool.Prim.fDisplaysInfo;
		fConstrain = Tool.Prim.fConstrain;
		fAutoScrollSetPos = Tool.Prim.fAutoScrollSetPos;
	}

	Tool.bAltTool = fAlternate;
	Tool.lpToolProc = lpTool;
	Tool.fSetCursor = fSetCursor;
	Tool.fDisplaysInfo = fDisplaysInfo;
	Tool.fConstrain = fConstrain;
	Tool.fAutoScrollSetPos = fAutoScrollSetPos;

	SetWindowCursor(idCursor);

// Show the new Ribbon (if any) and if successful,
// close the old Ribbon (if any)
//if ( idRibbon && idRibbon != Tool.idRibbon )
//	{
//	if ( lpRibbon )
//		AstralDlg( YES, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), 
//			idRibbon, lpRibbon);
//   // Close the old ribbon
//	if ( hWnd = AstralDlgGet( Tool.idRibbon ) )
//		AstralDlgEnd(hWnd, TRUE);
//	// Move the new ribbon
//	Tool.idRibbon = idRibbon;
//	if ( Tool.hRibbon = AstralDlgGet( Tool.idRibbon ) )
//		CServerDoc::SetToolbar(PictPubApp.GetActiveView(),TRUE);
//	}
//if ( idRibbon && idRibbon != Tool.idRibbon )
	if (idRibbon)
	{
   		CServerView *pView = PictPubApp.GetActiveView();

   // ActiveView returning inplaceactive TRUE when PP has no open views and
   // it is the active app.
   		if (fAppActive && pView && pView->GetDocument()->IsInPlaceActive())
      		pView = NULL;
		CServerDoc::SetToolbar(pView,TRUE,idRibbon,lpRibbon);
	}
}

extern void ShowChromaProc(HWND hWindow, UINT msg, int x, int y, UINT32 Option);
ITEMID ActivateTool( ITEMID idTool )
{
	// never, and I mean never activate a null tool
	if (!idTool)
		return(Tool.idLast);
	
	DeactivateTool();
	if ( idTool != Tool.id )
	{ // if a different tool, cleanup and save it
		if ( Tool.lpToolProc )
			ActivateProc( NULL, 0L );
		Tool.idLast = Tool.id;
	}

	clr( (LPTR)&Tool.Prim, sizeof(Tool.Prim) );
	clr( (LPTR)&Tool.Alt, sizeof(Tool.Alt) );
	Tool.bPassRButton = NO;

	switch ( Tool.id = idTool )
	{
    case IDC_SELECTOR:
		Tool.Prim.lpRibbonProc = DlgSelectorProc;
		Tool.Prim.idRibbon = IDD_SELECTOR;
		Tool.Prim.lpToolProc   = SelectorProc;
		Tool.Prim.idCursor = ID_ARROW;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_FREE:
		Control.idMaskTool = idTool;
		Tool.Prim.lpRibbonProc = DlgFreeProc;
		Tool.Prim.idRibbon = IDD_FREE;
		Tool.Prim.lpToolProc   = FreeProc;
		Tool.Prim.idCursor = ID_FREE;
		Tool.Prim.fConstrain = NO;
		break;

    case IDC_PAINTON:
    	if( !Control.Retail )
        	break;
		Control.idMaskTool = idTool;
		Tool.Prim.lpRibbonProc = DlgPaintOnProc;
		Tool.Prim.idRibbon = IDD_PAINTON;
		Tool.Prim.lpToolProc   = PaintOnProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(PAINTON_BRUSH);
		break;

    case IDC_SQUARE:
		Control.idMaskTool = idTool;
		Tool.Prim.lpRibbonProc = DlgRectangularProc;
		Tool.Prim.idRibbon = IDD_SQUARE;
		Tool.Prim.lpToolProc   = SquareProc;
		Tool.Prim.idCursor = ID_FREE;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_CROP:
		Control.idMaskTool = idTool;
		Tool.Prim.lpRibbonProc = DlgCropProc;
		Tool.Prim.idRibbon = IDD_CROP;
		Tool.Prim.lpToolProc   = CropProc;
		Tool.Prim.idCursor = ID_CROP;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_MAGICWAND:
		Control.idMaskTool = idTool;
		Tool.Prim.lpRibbonProc = DlgMagicProc;
		Tool.Prim.idRibbon = IDD_MAGICWAND;
		Tool.Prim.lpToolProc   = MagicProc;
		Tool.Prim.idCursor = ID_MAGICWAND;
		Tool.Prim.fConstrain = YES;
		break;

    case IDC_TRANSFORMER:
		Control.idMaskTool = idTool;
		Tool.Prim.lpRibbonProc = DlgTransformerProc;
		Tool.Prim.idRibbon = IDD_TRANSFORMER;
		Tool.Prim.lpToolProc   = TransformerProc;
		Tool.Prim.idCursor = ID_TRANSFORMER;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_OBJECTTRANSFORM:
		Tool.Prim.lpRibbonProc = DlgObjectTransformProc;
		Tool.Prim.idRibbon = IDD_OBJECTTRANSFORM;
		Tool.Prim.lpToolProc   = TransformObjectsProc;
		Tool.Prim.idCursor = ID_ARROW;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_SELECTORTRANSFORM:
		Tool.Prim.lpRibbonProc = DlgObjectTransformProc;
		Tool.Prim.idRibbon = IDD_SELECTORTRANSFORM;
		Tool.Prim.lpToolProc   = TransformObjectsProc;
		Tool.Prim.idCursor = ID_ARROW;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_MASKTRANSFORM:
		Tool.Prim.lpRibbonProc = DlgObjectTransformProc;
		Tool.Prim.idRibbon = IDD_MASKTRANSFORM;
		Tool.Prim.lpToolProc   = TransformObjectsProc;
		Tool.Prim.idCursor = ID_ARROW;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_POINTER:
		Control.idMaskTool = idTool;
		Tool.Prim.lpRibbonProc = DlgPointerProc;
		Tool.Prim.idRibbon = IDD_POINTER;
		Tool.Prim.lpToolProc   = PointerProc;
		Tool.Prim.idCursor = ID_POINTER;
		Tool.Prim.fConstrain = NO;
		break;

    case IDC_PAINT:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgPaintProc;
		Tool.Prim.idRibbon = IDD_PAINT;
		Tool.Prim.lpToolProc   = PaintProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(PAINT_BRUSH);
		break;

    case IDC_SPRAY:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgPaintProc;
		Tool.Prim.idRibbon = IDD_SPRAY;
		Tool.Prim.lpToolProc   = PaintProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(SPRAY_BRUSH);
		break;

    case IDC_CLONE:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgPaintProc;
		Tool.Prim.idRibbon = IDD_CLONE;
		Tool.Prim.lpToolProc   = CloneProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(CLONE_BRUSH);
		break;

    case IDC_TEXTURE:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgTextureProc;
		Tool.Prim.idRibbon = IDD_TEXTURE;
		Tool.Prim.lpToolProc   = PaintProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(TEXTURE_BRUSH);
		break;

    case IDC_SMEAR:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgPaintProc;
		Tool.Prim.idRibbon = IDD_SMEAR;
		Tool.Prim.lpToolProc   = PaintProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(SMEAR_BRUSH);
		break;

    case IDC_ERASER:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgPaintProc;
		Tool.Prim.lpToolProc   = EraserProc;
		Tool.Prim.idRibbon = IDD_ERASER;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		SelectRetouchBrush(ERASER_BRUSH);
		break;

    case IDC_SHARP:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgFilterProc;
		Tool.Prim.idRibbon = IDD_SHARP;
		Tool.Prim.lpToolProc   = FilterProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(SHARPEN_BRUSH);
		break;

    case IDC_SMOOTH:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgFilterProc;
		Tool.Prim.idRibbon = IDD_SMOOTH;
		Tool.Prim.lpToolProc   = FilterProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(SMOOTH_BRUSH);
		break;

    case IDC_LIGHTEN:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgFilterProc;
		Tool.Prim.idRibbon = IDD_LIGHTEN;
		Tool.Prim.lpToolProc   = FilterProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(LIGHTEN_BRUSH);
		break;

    case IDC_DARKEN:
		Control.idRetouchTool = idTool;
		Tool.Prim.lpRibbonProc = DlgFilterProc;
		Tool.Prim.idRibbon = IDD_DARKEN;
		Tool.Prim.lpToolProc   = FilterProc;
		Tool.Prim.idCursor = ID_NULL;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fAutoScrollSetPos = YES;
		Tool.Alt.lpRibbonProc = NULL;
		Tool.Alt.idRibbon = 0;
		Tool.Alt.lpToolProc = AltEraserProc;
		Tool.Alt.idCursor = ID_NULL;
		Tool.Alt.fConstrain = YES;
		Tool.Alt.fAutoScrollSetPos = YES;
		Tool.bPassRButton = YES;
		SelectRetouchBrush(DARKEN_BRUSH);
		break;

    case IDC_STITCH:
		Tool.Prim.lpRibbonProc = DlgStitchProc;
		Tool.Prim.idRibbon = IDD_STITCH;
		Tool.Prim.lpToolProc   = StitchProc;
		Tool.Prim.idCursor = ID_ARROW;
		Tool.Alt.lpRibbonProc = DlgViewProc;
		Tool.Alt.idRibbon = IDD_CUSTOMVIEW;
		Tool.Alt.lpToolProc   = CustomProc;
		Tool.Alt.idCursor = ID_CUSTOMVIEW;
		break;

    case IDC_VIGNETTE:
		Control.idFillTool = idTool;
		Tool.Prim.lpRibbonProc = DlgVignetteProc;
		Tool.Prim.idRibbon = IDD_VIGNETTE;
		Tool.Prim.lpToolProc   = VignetteProc;
		Tool.Prim.idCursor = ID_VIGNETTE;
		Tool.Prim.fConstrain = YES;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_TEXTUREFILL:
		Control.idFillTool = idTool;
		Tool.Prim.lpRibbonProc = DlgTextureFillProc;
		Tool.Prim.idRibbon = IDD_TEXTUREFILL;
		Tool.Prim.lpToolProc   = TextureFillProc;
		Tool.Prim.idCursor = ID_FLOOD;
		break;

    case IDC_TINTFILL:
		Control.idFillTool = idTool;
		Tool.Prim.lpRibbonProc = DlgTintFillProc;
		Tool.Prim.idRibbon = IDD_TINTFILL;
		Tool.Prim.lpToolProc   = TintFillProc;
		Tool.Prim.idCursor = ID_FLOOD;
		break;

    case IDC_FLOOD:
		Control.idFillTool = idTool;
		Tool.Prim.lpRibbonProc = DlgFloodProc;
		Tool.Prim.idRibbon = IDD_FLOOD;
		Tool.Prim.lpToolProc   = FloodProc;
		Tool.Prim.idCursor = ID_MAGICWAND;
		break;

    case IDC_PENCIL:
		Control.idDrawTool = idTool;
		Tool.Prim.lpRibbonProc = DlgDrawProc;
		Tool.Prim.idRibbon = IDD_PENCIL;
		Tool.Prim.lpToolProc   = FreeProc;
		Tool.Prim.idCursor = ID_PENCIL;
		SelectRetouchBrush(DRAW_PENCIL_BRUSH);
		Draw.FillInterior = FALSE;
		Draw.DrawExterior = TRUE;
		Tool.Prim.fConstrain = YES;
		break;

    case IDC_DRAWFREE:
		Control.idDrawTool = idTool;
		Tool.Prim.lpRibbonProc = DlgDrawProc;
		Tool.Prim.idRibbon = IDD_DRAWFREE;
		Tool.Prim.lpToolProc   = FreeProc;
		Tool.Prim.idCursor = ID_PENCIL;
		Tool.Prim.fConstrain = NO;
		SelectRetouchBrush(DRAW_FREE_BRUSH);
		Draw.FillInterior = !(Draw.wFreeStyle == IDC_FREEDGE-IDC_FREEDGE);
		Draw.DrawExterior = !(Draw.wFreeStyle == IDC_FREFILL-IDC_FREEDGE);
		break;

    case IDC_DRAWSQUARE:
		Control.idDrawTool = idTool;
		Tool.Prim.lpRibbonProc = DlgDrawProc;
		Tool.Prim.idRibbon = IDD_DRAWSQUARE;
		Tool.Prim.lpToolProc   = DrawSquareProc;
		Tool.Prim.idCursor = ID_PENCIL;
		Tool.Prim.fDisplaysInfo = YES;
		SelectRetouchBrush(DRAW_RECT_BRUSH);
		break;

    case IDC_CUSTOMVIEW:
		Tool.Prim.lpRibbonProc = DlgViewProc;
		Tool.Prim.idRibbon = IDD_CUSTOMVIEW;
		Tool.Prim.lpToolProc   = CustomProc;
		Tool.Prim.idCursor = ID_CUSTOMVIEW;
		break;

    case IDC_GRABBER:
		Tool.Prim.lpRibbonProc = NULL;
		Tool.Prim.idRibbon = 0;
		Tool.Prim.lpToolProc   = GrabberProc;
		Tool.Prim.idCursor = ID_GRABBER;
		Tool.Prim.fConstrain = YES;
		break;

    case IDC_TEXT:
		Tool.Prim.lpRibbonProc = DlgTextProc;
		Tool.Prim.idRibbon = IDD_TEXT;
		Tool.Prim.lpToolProc   = TextProc;
		Tool.Prim.idCursor = ID_TEXT;
		break;

    case IDC_PROBE:
		Tool.Prim.lpRibbonProc = DlgProbeProc;
		Tool.Prim.idRibbon = IDD_PROBE;
		Tool.Prim.lpToolProc   = ProbeProc;
		Tool.Prim.idCursor = ID_PROBE;
		Tool.Prim.fDisplaysInfo = YES;
		break;

    case IDC_PREVIEW:
		Tool.Prim.lpRibbonProc = NULL; //DlgShowShieldProc;
		Tool.Prim.idRibbon = 0; //IDD_SHOWSHIELD;
		Tool.Prim.lpToolProc   = ShowShieldProc;
		Tool.Prim.idCursor = ID_ARROW;
		break;

    case IDC_CHROMA:
		Tool.Prim.lpRibbonProc = NULL;
		Tool.Prim.idRibbon = 0; 
		Tool.Prim.lpToolProc   = ShowChromaProc;
		Tool.Prim.idCursor = ID_ARROW;
		break;

    default:
		Tool.Prim.lpRibbonProc = NULL;
		Tool.Prim.idRibbon = 0;
		Tool.Prim.lpToolProc   = NULL;
		Tool.Prim.idCursor = ID_ARROW;
		break;
    }

	InstallTool( NO ); // Bank in the primary tool
	if ( Tool.lpToolProc )
		ActivateProc( NULL, 1L );
	return( Tool.idLast );
}

void DeactivateTool()
{
	HWND hActiveWnd;

	if ( Tool.bActive && Tool.lpToolProc )
	{
		if (hActiveWnd = GetActiveDoc())
			DestroyProc( hActiveWnd, 0L );
	}
}

static HMENU InitPopupTools()
{
	HMENU hPopup;
	WORD wState;
	#define MENUFLAGS1 (MF_OWNERDRAW | MF_BYCOMMAND)
	#define MENUFLAGS2 (MF_OWNERDRAW | MF_BYCOMMAND | MF_MENUBARBREAK)
	#define MENUFLAGS3 (MF_OWNERDRAW | MF_BYCOMMAND | MF_MENUBREAK)

	/* Get the menu for the popup from the resource file. */
	if ( !(hMenu = LoadMenu( PictPubApp.GetResourceHandle(), "POPUPTOOLS" )) )
		return( NULL );

	// Set the ownerdraw flags; if this could happen if the RC file, it would
	hPopup = GetSubMenu( hMenu, IDC_MASK - IDC_FIRSTTOOL );
	wState = GetMenuState(hPopup, IDC_SQUARE, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_SQUARE,     MENUFLAGS1|wState, IDC_SQUARE, NULL );
	wState = GetMenuState(hPopup, IDC_FREE, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_FREE,       MENUFLAGS2|wState, IDC_FREE, NULL );
	wState = GetMenuState(hPopup, IDC_MAGICWAND, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_MAGICWAND,  MENUFLAGS2|wState, IDC_MAGICWAND, NULL );
	wState = GetMenuState(hPopup, IDC_SHIELD, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_SHIELD,     MENUFLAGS2|wState, IDC_SHIELD, NULL );
	wState = GetMenuState(hPopup, IDC_TRANSFORMER, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup,IDC_TRANSFORMER,MENUFLAGS2|wState,IDC_TRANSFORMER,NULL);
	wState = GetMenuState(hPopup, IDC_POINTER, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_POINTER,    MENUFLAGS2|wState, IDC_POINTER, NULL );

	if( Control.Retail )
	{
    	wState = GetMenuState(hPopup, IDC_PAINTON, MF_BYCOMMAND ) & MF_GRAYED;
    	ModifyMenu( hPopup, IDC_PAINTON,    MENUFLAGS2|wState, IDC_PAINTON, NULL );
	}

	wState = GetMenuState(hPopup, IDC_CROP, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_CROP,     MENUFLAGS2|wState, IDC_CROP, NULL );

	hPopup = GetSubMenu( hMenu, IDC_RETOUCH - IDC_FIRSTTOOL );
	wState = GetMenuState(hPopup, IDC_PAINT, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_PAINT,	 MENUFLAGS1|wState, IDC_PAINT, NULL );
	wState = GetMenuState(hPopup, IDC_SPRAY, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_SPRAY,	 MENUFLAGS2|wState, IDC_SPRAY, NULL );
	wState = GetMenuState(hPopup, IDC_CLONE, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_CLONE,	 MENUFLAGS2|wState, IDC_CLONE, NULL );
	wState = GetMenuState(hPopup, IDC_TEXTURE, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_TEXTURE, MENUFLAGS2|wState, IDC_TEXTURE, NULL );
	wState = GetMenuState(hPopup, IDC_SMEAR, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_SMEAR,	 MENUFLAGS2|wState, IDC_SMEAR, NULL );
	wState = GetMenuState(hPopup, IDC_ERASER, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_ERASER,	 MENUFLAGS2|wState, IDC_ERASER, NULL );

	hPopup = GetSubMenu( hMenu, IDC_FILTER - IDC_FIRSTTOOL );
	wState = GetMenuState(hPopup, IDC_SHARP, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_SHARP,	 MENUFLAGS1|wState, IDC_SHARP, NULL );
	wState = GetMenuState(hPopup, IDC_SMOOTH, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_SMOOTH,	 MENUFLAGS2|wState, IDC_SMOOTH, NULL );
	wState = GetMenuState(hPopup, IDC_LIGHTEN, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_LIGHTEN, MENUFLAGS2|wState, IDC_LIGHTEN, NULL );
	wState = GetMenuState(hPopup, IDC_DARKEN, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_DARKEN,	 MENUFLAGS2|wState, IDC_DARKEN, NULL );

	hPopup = GetSubMenu( hMenu, IDC_FILL - IDC_FIRSTTOOL );
	wState = GetMenuState(hPopup, IDC_VIGNETTE, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_VIGNETTE,   MENUFLAGS1|wState, IDC_VIGNETTE, NULL );
	wState = GetMenuState(hPopup, IDC_FLOOD, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_FLOOD,      MENUFLAGS2|wState, IDC_FLOOD, NULL );
	wState = GetMenuState(hPopup, IDC_TEXTUREFILL, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_TEXTUREFILL,MENUFLAGS2|wState, IDC_TEXTUREFILL, NULL );
	wState = GetMenuState(hPopup, IDC_TINTFILL, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_TINTFILL,   MENUFLAGS2|wState, IDC_TINTFILL, NULL );

	hPopup = GetSubMenu( hMenu, IDC_DRAW - IDC_FIRSTTOOL );
	wState = GetMenuState(hPopup, IDC_DRAWSQUARE, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_DRAWSQUARE, MENUFLAGS1|wState, IDC_DRAWSQUARE, NULL );
	wState = GetMenuState(hPopup, IDC_PENCIL, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_PENCIL,     MENUFLAGS2|wState, IDC_PENCIL, NULL );
	wState = GetMenuState(hPopup, IDC_DRAWFREE, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_DRAWFREE,   MENUFLAGS2|wState, IDC_DRAWFREE, NULL );

	hPopup = GetSubMenu( hMenu, IDC_ZOOM - IDC_FIRSTTOOL );
	wState = GetMenuState(hPopup, IDC_CUSTOMVIEW, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_CUSTOMVIEW, MENUFLAGS1|wState, IDC_CUSTOMVIEW, NULL );
	wState = GetMenuState(hPopup, IDC_ZOOMOUT, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_ZOOMOUT,     MENUFLAGS2|wState, IDC_ZOOMOUT, NULL );
	wState = GetMenuState(hPopup, IDC_VIEWLAST, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_VIEWLAST,     MENUFLAGS2|wState, IDC_VIEWLAST, NULL );
	wState = GetMenuState(hPopup, IDC_VIEW1TO1, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_VIEW1TO1,     MENUFLAGS2|wState, IDC_VIEW1TO1, NULL );
	wState = GetMenuState(hPopup, IDC_VIEWALL, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_VIEWALL,     MENUFLAGS2|wState, IDC_VIEWALL, NULL );
	wState = GetMenuState(hPopup, IDC_VIEWFULL, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_VIEWFULL,     MENUFLAGS2|wState, IDC_VIEWFULL, NULL );

	hPopup = GetSubMenu( hMenu, IDC_PROBE - IDC_FIRSTTOOL );
	wState = GetMenuState(hPopup, IDC_PROBEPOINT, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_PROBEPOINT, MENUFLAGS1|wState, IDC_PROBEPOINT, NULL );
	wState = GetMenuState(hPopup, IDC_PROBERECT, MF_BYCOMMAND ) & MF_GRAYED;
	ModifyMenu( hPopup, IDC_PROBERECT,     MENUFLAGS2|wState, IDC_PROBERECT, NULL );

	CheckMenuItem( hMenu, idMaskFunction = IDC_SQUARE, MF_CHECKED );
	CheckMenuItem( hMenu, idRetouchFunction = IDC_PAINT, MF_CHECKED );
	CheckMenuItem( hMenu, idFilterFunction = IDC_SHARP, MF_CHECKED );
	CheckMenuItem( hMenu, idFillFunction = IDC_VIGNETTE, MF_CHECKED );
	CheckMenuItem( hMenu, idDrawFunction = IDC_PENCIL, MF_CHECKED );
	return( hMenu );
}

void SaveRetouchBrush( int brush )
{
	if (brush != NO_BRUSH)
	{
		lpBrushSettings [brush].BrushShape = Retouch.BrushShape;
		lpBrushSettings [brush].BrushSize = Retouch.BrushSize;
		lpBrushSettings [brush].Opacity = Retouch.Opacity;
		lpBrushSettings [brush].Pressure = Retouch.Pressure;
		lpBrushSettings [brush].MergeMode = Retouch.MergeMode;
		lpBrushSettings [brush].Feather = Retouch.Feather;
	}
}

void SelectRetouchBrush( int brush )
{
	if (brush != NO_BRUSH)
	{
		SaveRetouchBrush( RetouchBrushCurrent );
		Retouch.BrushShape = lpBrushSettings [brush].BrushShape;
		Retouch.BrushSize = lpBrushSettings [brush].BrushSize;
		Retouch.Opacity = lpBrushSettings [brush].Opacity;
		Retouch.Pressure = lpBrushSettings [brush].Pressure;
		Retouch.MergeMode = lpBrushSettings [brush].MergeMode;
		Retouch.Feather = lpBrushSettings [brush].Feather;
		RetouchBrushCurrent = brush;
	}
}

#undef new
IMPLEMENT_SERIAL(COCWinButton, COCButton, 0);

