//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "afxpriv.h"
#include "ppafx.h"
#include "id.h"
#include "pickpal.h"
#include "custools.h"
#include "editcust.h"
#include "svrview.h"
#include "macro.h"
#include "ctxhelp.h"
#include "ipframe.h"
 
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define CUSTFILENAME    "Customtl.ini"
#define BOX_POSITION    "Position"
#define BOX_WIDTH       "Width"
#define COLORPATCH      "ColorPatch"
static STRING szCustomToolFile;

static HMENU hPopup;
static HMENU hMenu;
static WORD idMaskFunction, idRetouchFunction, idFilterFunction,
	    idFillFunction, idDrawFunction;

extern BOOL    CreatingCustom;
extern CString NewToolBoxName;
extern BOOL WINPROC EXPORT DlgSaveToolBoxProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam);
// Static prototypes
static HMENU InitPopupTools( void );
static void PopupTools( HWND hWnd, ITEMID idTool, HWND hControl);
void MeasurePopupMenu( LPMEASUREITEMSTRUCT lpMeasure );
void DrawPopupMenu( const DRAWITEMSTRUCT FAR * lpDraw );

/***************************************************************************/
/*                      CCustTool                                          */
/***************************************************************************/

#define ICON_HEIGHT  34

INT            CCustTool::ButtonHeight = 20;
CCustToolBox *CurCustomToolBox = NULL;

IMPLEMENT_DYNAMIC(CCustTool, CObject)
CCustTool::CCustTool()
   {
   m_Cmd       = 0;
   m_MacroName = "";
   m_ShortName = "";
   m_Type      = CUSTOOL_CMD;
   m_Wnd       = NULL;
   }

CCustTool::CCustTool(CUSTOOL_TYPE Type, UINT Cmd, CString &SName)
   {
   m_Cmd       = Cmd;
   m_MacroName = "";
   m_ShortName = SName;
   m_Type      = Type;
   m_Wnd       = NULL;
   }

CCustTool::CCustTool(CString &SName, CString &MName)
   {
   m_Cmd       = IDM_MACROPLAY;
   m_MacroName = MName;
   m_ShortName = SName;
   m_Type      = CUSTOOL_MACRO;
   m_Wnd       = NULL;
   }

CCustTool::CCustTool(const CCustTool &Src)
   {
   m_Cmd       = Src.m_Cmd;
   m_MacroName = Src.m_MacroName;
   m_ShortName = Src.m_ShortName;
   m_Type      = Src.m_Type;
   m_Wnd       = NULL;
   }

CCustTool::~CCustTool()
   {
   }

BOOL   CCustTool::Create (CWnd *pParent, UINT Width)
   {
   LPSTR lpbuf;

   if (m_Wnd)
      return(TRUE);

   switch (m_Type)
      {
      case CUSTOOL_ICON:
         m_Wnd = CreateWindow("icon","",WS_CHILD | WS_VISIBLE | WS_TOOL, 0, 0,
         ICON_HEIGHT, ICON_HEIGHT, pParent->GetSafeHwnd(), (HMENU)m_Cmd, PictPubApp.Get_hInstAstral(), NULL);
         break;
      default:
         lpbuf = m_ShortName.GetBuffer(256);
         m_Wnd = CreateWindow("Button", lpbuf, WS_CHILD | WS_VISIBLE, 0, 0, ICON_HEIGHT * Width,
         CCustTool::ButtonHeight, pParent->GetSafeHwnd(), (HMENU)m_Cmd, PictPubApp.Get_hInstAstral(), NULL);
         m_ShortName.ReleaseBuffer();
         CWnd::FromHandle(m_Wnd)->SetFont(CFont::FromHandle((HFONT)GetStockObject(Window.iLabelFont)));
      }
   
   return(TRUE);
   }

BOOL CCustTool::PlayMacro()
   {
   if (m_Type == CUSTOOL_MACRO)
      {
      FNAME    szFileName;
      LPSTR    lpbuf = m_MacroName.GetBuffer(256);

	   lstrcpy( Names.Macro, lpbuf);
      m_MacroName.ReleaseBuffer();
	   Macro.nRepeat = 1;
      if (LookupExtFile(Names.Macro, szFileName, IDN_MACRO))
         {
         if (Macro.fJournalMacro)
            {
            int i = Macro.nRepeat;
            while (i--)
               PlayJournalMacro(szFileName);
            }
         else
            ::PlayMacro(NULL, szFileName, Macro.nRepeat, NO, NULL, NULL, 0, Names.Macro);
         }
      }
   return(TRUE);
   }


UINT   CCustTool::RePosition(UINT NonIconTools, UINT Order, UINT Width)
   {
   UINT     ret = CCustTool::ButtonHeight;
   UINT     Y   = NonIconTools * ret;
   UINT     X   = 0;

   if (m_Type == CUSTOOL_ICON)
      {
      X   = ICON_HEIGHT * (Order % Width);
      Y  += (Order / Width) * ICON_HEIGHT;
      ret = ICON_HEIGHT;
      }
   
	CWnd::FromHandle(m_Wnd)->SetWindowPos(NULL, X, Y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
   return(ret);
   }

BOOL CCustTool::Load(UINT Order, LPSTR SectionName)
   {
   char           digits[16];
   LPSTR          lpstr, Entry;
   CString        Tool, Val;

   itoa(Order, digits, 10);
   Tool  = "Tool";
   Tool += digits;
   Entry = Tool.GetBuffer(256);
   lpstr = Val.GetBuffer(256);
   GetPrivateProfileString(SectionName, Entry, "", lpstr, 256, szCustomToolFile);
   Val.ReleaseBuffer();
   Tool.ReleaseBuffer();
   if (!Val.IsEmpty())
      {
      m_Type = (CUSTOOL_TYPE)(Val[0] - '0');
      Val = Val.Right(Val.GetLength() - 2);
      Tool = Val.Left(Val.Find(','));
      if (m_Type == CUSTOOL_MACRO)
         m_MacroName = Tool;
      else
         m_Cmd = atoi(Tool);
      m_ShortName = Val.Right(Val.GetLength() - Val.Find(',') - 1);
      return(TRUE);
      }
   return(FALSE);
   }

BOOL CCustTool::Save(UINT Order, LPSTR SectionName)
   {
   char           digits[16];
   LPSTR          Entry, lpstr2;
   CString        Tool, Val;

   itoa(Order, digits, 10);
   Tool   = "Tool";
   Tool  += digits;

   itoa((UINT)m_Type, digits, 10);
   Val    = digits;
   Val   += ',';
   if (m_Type != CUSTOOL_MACRO)
      {
      itoa(m_Cmd, digits, 10);
      Val   += digits;
      }
   else
      Val += m_MacroName;
   Val   += ',';
   Val   += m_ShortName;
   Entry  = Tool.GetBuffer(256);
   lpstr2 = Val.GetBuffer(256);
   WritePrivateProfileString(SectionName, Entry, lpstr2, szCustomToolFile);
   Val.ReleaseBuffer();
   Tool.ReleaseBuffer();
   return(TRUE);
   }

#ifdef _DEBUG
void CCustTool::AssertValid() const
   {
	CObject::AssertValid();
   }

void CCustTool::Dump(CDumpContext& dc) const
   {
	CObject::Dump(dc);
   AFX_DUMP1(dc,"m_MacroName=",m_MacroName);
   AFX_DUMP1(dc,"m_ShortName=",m_ShortName);
   AFX_DUMP1(dc,"m_Cmd=",m_Cmd);
   }

#endif

/***************************************************************************/
/*                      CCustToolList                                      */
/***************************************************************************/

IMPLEMENT_DYNAMIC(CCustToolList, CObList)
BEGIN_MESSAGE_MAP(CCustToolList,CCmdTarget)
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
   ON_BN_CLICKED(IDC_CUSTOMVIEW,    OnCustomView)
   ON_BN_CLICKED(IDC_TEXT,          OnText)
   ON_BN_CLICKED(IDC_PROBE,         OnProbe)

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

	ON_COMMAND(IDM_CUSTBOXLOAD, OnCustomLoad)
		ON_UPDATE_COMMAND_UI(IDM_CUSTBOXLOAD, OnUpdateCustomLoad)
	ON_COMMAND(IDM_CUSTBOXCREATE, OnCustomCreate)
		ON_UPDATE_COMMAND_UI(IDM_CUSTBOXCREATE, OnUpdateCustomCreate)

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

afx_msg void CCustToolList::OnUpdateRMCheck(CCmdUI* pCmdUI)
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

afx_msg void CCustToolList::OnUpdateRMEnable(CCmdUI* pCmdUI)
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

afx_msg void CCustToolList::OnUpdateRMEnableAndCheck(CCmdUI* pCmdUI)
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


afx_msg void CCustToolList::OnFreeHand()
   {
   SendToRibbon(IDC_FREEHAND);
   }
afx_msg void CCustToolList::OnAutoMask()
   {
   SendToRibbon(IDC_AUTOMASK);
   }
afx_msg void CCustToolList::OnDrawLine()
   {
   SendToRibbon(IDC_DRAW_LINE);
   }
afx_msg void CCustToolList::OnDrawBezier()
   {
   SendToRibbon(IDC_DRAW_BEZIER);
   }
afx_msg void CCustToolList::OnIsBezier()
   {
   SendToRibbon(IDC_IS_BEZIER);
   }
afx_msg void CCustToolList::OnIsPoint()
   {
   SendToRibbon(IDC_IS_POINT);
   }
afx_msg void CCustToolList::OnMovePoints()
   {
   SendToRibbon(IDC_MOVEPOINTS);
   }
afx_msg void CCustToolList::OnAddPoints()
   {
   SendToRibbon(IDC_ADDPOINTS);
   }
afx_msg void CCustToolList::OnDelPoint()
   {
   SendToRibbon(IDC_DELPOINTS);
   }
afx_msg void CCustToolList::OnPointMode()
   {
   SendToRibbon(IDC_POINTMODE);
   }

afx_msg void CCustToolList::OnMaskClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnMaskDblClk();
   else
      PopupTools(m_hDlg, IDC_MASK, m_hControl);
   }

afx_msg void CCustToolList::OnMaskDblClk()
   {
   NotifyOfActiveTool(IDC_MASK, idMaskFunction);
	ActivateTool(idMaskFunction);
   }

afx_msg void CCustToolList::OnRetouchClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnRetouchDblClk();
   else
      PopupTools(m_hDlg, IDC_RETOUCH, m_hControl);
   }

afx_msg void CCustToolList::OnRetouchDblClk()
   {
   NotifyOfActiveTool(IDC_RETOUCH, idRetouchFunction);
	ActivateTool(idRetouchFunction);
   }

afx_msg void CCustToolList::OnFilterClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnFilterDblClk();
   else
      PopupTools(m_hDlg, IDC_FILTER, m_hControl);
   }

afx_msg void CCustToolList::OnFilterDblClk()
   {
   NotifyOfActiveTool(IDC_FILTER, idFilterFunction);
	ActivateTool(idFilterFunction);
   }

afx_msg void CCustToolList::OnFillClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnFillDblClk();
   else
      PopupTools(m_hDlg, IDC_FILL, m_hControl);
   }

afx_msg void CCustToolList::OnFillDblClk()
   {
   NotifyOfActiveTool(IDC_FILL, idFillFunction);
	ActivateTool(idFillFunction);
   }

afx_msg void CCustToolList::OnDrawClick()
   {
   BOOL  fDoubleClick = (m_codeNotify == 1);
   if (fDoubleClick)
      OnDrawDblClk();
   else
      PopupTools(m_hDlg, IDC_DRAW, m_hControl);
   }

afx_msg void CCustToolList::OnDrawDblClk()
   {
   NotifyOfActiveTool(IDC_DRAW, idDrawFunction);
	ActivateTool(idDrawFunction);
   }

afx_msg void CCustToolList::OnLastTool()
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

afx_msg void CCustToolList::OnSelector()
   {OnSelViewTextProbe(IDC_SELECTOR);}
afx_msg void CCustToolList::OnCustomView()
   {OnSelViewTextProbe(IDC_CUSTOMVIEW);}
afx_msg void CCustToolList::OnText()
   {OnSelViewTextProbe(IDC_TEXT);}
afx_msg void CCustToolList::OnProbe()
   {OnSelViewTextProbe(IDC_PROBE);}
afx_msg void CCustToolList::OnSelViewTextProbe(int ID)
{
	switch (ID)
	{
		case IDC_SELECTOR:
		case IDC_CUSTOMVIEW:
		case IDC_TEXT:
		case IDC_PROBE:
			NotifyOfActiveTool(ID, 0);
			ActivateTool(ID);
		break;

		default:
		break;
	}
}

afx_msg void CCustToolList::OnMaskFree()
   {OnMaskTool(IDC_FREE);}
afx_msg void CCustToolList::OnMaskSquare()
   {OnMaskTool(IDC_SQUARE);}
afx_msg void CCustToolList::OnMaskCircle()
   {OnMaskTool(IDC_CIRCLE);}
afx_msg void CCustToolList::OnMaskPainton()
   {OnMaskTool(IDC_PAINTON);}
afx_msg void CCustToolList::OnMaskMagic()
   {OnMaskTool(IDC_MAGICWAND);}
afx_msg void CCustToolList::OnMaskShield()
   {OnMaskTool(IDC_SHIELD);}
afx_msg void CCustToolList::OnMaskTransformer()
   {OnMaskTool(IDC_TRANSFORMER);}
afx_msg void CCustToolList::OnMaskPointer()
   {OnMaskTool(IDC_POINTER);}
afx_msg void CCustToolList::OnMaskCrop()
   {OnMaskTool(IDC_CROP);}
afx_msg void CCustToolList::OnMaskTool(int ID)
   {
   NotifyOfActiveTool(IDC_MASK, ID);
	CheckMenuItem(hPopup, idMaskFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idMaskFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CCustToolList::OnRetouchPaint()
   {OnRetouchTool(IDC_PAINT);}
afx_msg void CCustToolList::OnRetouchSpray()
   {OnRetouchTool(IDC_SPRAY);}
afx_msg void CCustToolList::OnRetouchClone()
   {OnRetouchTool(IDC_CLONE);}
afx_msg void CCustToolList::OnRetouchTexture()
   {OnRetouchTool(IDC_TEXTURE);}
afx_msg void CCustToolList::OnRetouchSmear()
   {OnRetouchTool(IDC_SMEAR);}
afx_msg void CCustToolList::OnRetouchEraser()
   {OnRetouchTool(IDC_ERASER);}
afx_msg void CCustToolList::OnRetouchTool(int ID)
   {
   NotifyOfActiveTool(IDC_RETOUCH, ID);
	CheckMenuItem(hPopup, idRetouchFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idRetouchFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CCustToolList::OnFilterSharp()
   {OnFilterTool(IDC_SHARP);}
afx_msg void CCustToolList::OnFilterSmooth()
   {OnFilterTool(IDC_SMOOTH);}
afx_msg void CCustToolList::OnFilterLighten()
   {OnFilterTool(IDC_LIGHTEN);}
afx_msg void CCustToolList::OnFilterDarken()
   {OnFilterTool(IDC_DARKEN);}
afx_msg void CCustToolList::OnFilterTool(int ID)
   {
   NotifyOfActiveTool(IDC_FILTER, ID);
	CheckMenuItem(hPopup, idFilterFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idFilterFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CCustToolList::OnFillVignette()
   {OnFillTool(IDC_VIGNETTE);}
afx_msg void CCustToolList::OnFillTexture()
   {OnFillTool(IDC_TEXTUREFILL);}
afx_msg void CCustToolList::OnFillTintFill()
   {OnFillTool(IDC_TINTFILL);}
afx_msg void CCustToolList::OnFillFlood()
   {OnFillTool(IDC_FLOOD);}
afx_msg void CCustToolList::OnFillTool(int ID)
   {
   NotifyOfActiveTool(IDC_FILL, ID);
	CheckMenuItem(hPopup, idFillFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idFillFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }

afx_msg void CCustToolList::OnDrawPencil()
   {OnDrawTool(IDC_PENCIL);}
afx_msg void CCustToolList::OnDrawSquare()
   {OnDrawTool(IDC_DRAWSQUARE);}
afx_msg void CCustToolList::OnDrawFree()
   {OnDrawTool(IDC_DRAWFREE);}
afx_msg void CCustToolList::OnDrawTool(int ID)
   {
   NotifyOfActiveTool(IDC_DRAW, ID);
	CheckMenuItem(hPopup, idDrawFunction, MF_UNCHECKED);
	CheckMenuItem(hPopup, idDrawFunction = ID, MF_CHECKED);
	ActivateTool(ID);
   }



afx_msg void CCustToolList::OnCustomLoad()
   {
   if( Control.Retail )
       AstralDlg(NO, PictPubApp.Get_hInstAstral(), PictPubApp.Get_hWndAstral(), IDD_TOOLBOX_LOAD, DlgSaveToolBoxProc);
   }

afx_msg void CCustToolList::OnUpdateCustomLoad(CCmdUI* pCmdUI)
   {
   if( Control.Retail )
       pCmdUI->Enable(TRUE);
   }

afx_msg void CCustToolList::OnCustomCreate()
{
    if( !Control.Retail )
        return;

    CCustToolBox      *pctb;
    NewToolBoxName = "";
    CreatingCustom = TRUE;

    if (CRenameCustTool(&NewToolBoxName).DoModal() != IDCANCEL)
    {
        FixExtName( &NewToolBoxName );
        if( lstrlen( NewToolBoxName ))
        {
            Add( NewToolBoxName, FALSE );
            pctb = ( CCustToolBox * )GetHead();
            CEditCustTool( pctb ).DoModal();
        }
    }
}

afx_msg void CCustToolList::OnUpdateCustomCreate(CCmdUI* pCmdUI)
{
    if( Control.Retail )
        pCmdUI->Enable(TRUE);
}

CCustToolList::CCustToolList()
{
    m_pMain = NULL;
}

void CCustToolList::SetDlgCtrl(HWND hDlg, HWND hControl, UINT codeNotify)
   {
   m_hDlg         = hDlg;
   m_hControl     = hControl;
   m_codeNotify   = codeNotify;
   }

CCustToolList::~CCustToolList()
   {
   CCustToolBox   *pctb;
   POSITION        Pos;
   COLORSPACE   ColorSpace;
   STRING       szString;
   Save();

   for (Pos = GetHeadPosition(); Pos != NULL;)
      {
      pctb = (CCustToolBox *)GetNext(Pos);
      delete pctb;
      }
   // Save the active color preference
   ColorSpace = GetActiveColorString( szString );
   PutDefaultString( "Color1", szString );
   PutDefaultInt( "Space1", ColorSpace );
   // Save the alternate color preference
   ColorSpace = GetAlternateColorString( szString );
   PutDefaultString( "Color2", szString );
   PutDefaultInt( "Space2", ColorSpace );
   if ( hPopup )
	   {
	   DestroyMenu(hPopup);
	   hPopup = NULL;
	   }
   }

POSITION CCustToolList::Add(CCustToolBox *pBox)
   {
   if (pBox->m_IsMain)
      return m_List.AddTail(pBox);
   else
      return m_List.AddHead(pBox);
   }

BOOL CCustToolList::Remove(CCustToolBox *pBox)
   {
   POSITION        Pos;
   POSITION        Prev;
   CCustToolBox   *pctb;

   for (Pos = GetHeadPosition(); Pos != NULL;)
      {
	  Prev = Pos;
      pctb = (CCustToolBox *)GetNext(Pos);
      if (pctb == pBox)
	  	 {
      	 RemoveAt(Prev);
         delete pBox;
         return(TRUE);
		 }
      }
   return(FALSE);
   }

void CCustToolList::UpdateStatusButton(int id, BOOL b)
   {
   POSITION        Pos;
   CCustToolBox   *pctb;

   for (Pos = GetHeadPosition(); Pos != NULL;)
      {
      pctb = (CCustToolBox *)GetNext(Pos);
      if (pctb->GetSafeHwnd())
	      pctb->CheckDlgButton(id, b);
      }
   }

void CCustToolList::NotifyOfActiveTool(int cat, int id)
   {
   POSITION        Pos;
   CCustToolBox   *pctb;

   for (Pos = GetHeadPosition(); Pos != NULL;)
      {
      pctb = (CCustToolBox *)GetNext(Pos);
      if (pctb->GetSafeHwnd())
         {
         pctb->UpdateActiveTool(cat);
         pctb->UpdateActiveTool(id);
         }
      }
   }

void CCustToolList::ShowMain()
   {
   CCustToolBox   *pctb = FindMain();
   if (IsMainUp())
      pctb->OnCommand(SC_CLOSE, (LPARAM) 0);
   else
      pctb->Create();
   }

BOOL CCustToolList::IsMainUp()
   {
   CCustToolBox   *pctb = FindMain();

   if (pctb->GetSafeHwnd())
      return(TRUE);
   return(FALSE);
   }

CCustToolBox *CCustToolList::FindMain()
   {
   POSITION        Pos;
   CCustToolBox   *pctb;

   for (Pos = GetHeadPosition(); Pos != NULL;)
      {
      pctb = (CCustToolBox *)GetNext(Pos);
      if (pctb->m_IsMain && !pctb->m_IsInsitue)
         return(pctb);
      }
   return(NULL);
   }

POSITION CCustToolList::FindToolBox(CString &Name)
   {
   POSITION        NPos,Pos;
   CCustToolBox   *pctb;

   NPos = GetHeadPosition();
   for (; NPos != NULL;)
      {
      Pos = NPos;
      pctb = (CCustToolBox *)GetNext(NPos);
      if (pctb->GetName() == Name)
         return(Pos);
      }
   return(NULL);
   }

void CCustToolList::InvalidateColorPatches(BOOL Erase)
   {
   POSITION        Pos;
   CCustToolBox   *pctb;

   for (Pos = GetHeadPosition(); Pos != NULL;)
      {
      pctb = (CCustToolBox *)GetNext(Pos);
      if (pctb->IsPatchUp() && pctb->GetSafeHwnd())
         InvalidateRect(pctb->GetPatchWnd(), NULL, Erase);
      }
   }

void CCustToolList::NotifyColorPatches(int what)
   {
   POSITION        Pos;
   CCustToolBox   *pctb;

   for (Pos = GetHeadPosition(); Pos != NULL;)
      {
      pctb = (CCustToolBox *)GetNext(Pos);
      if (pctb->IsPatchUp())
		   FORWARD_WM_COMMAND(pctb->GetSafeHwnd(), IDC_ACTIVECOLOR, NULL, what, SendMessage);
      }
   }

BOOL CCustToolList::Save()
   {
   CCustToolBox   *pctb;
   LPSTR           lpbuf;
   LPSTR           lpbuf2;
   BOOL            ret = TRUE;
   int             i;
   CString         Name, Val;
   char            digits[4];
   POSITION        Pos;


   if (!m_List.GetCount())    // safety check. Do not save if it wasn't loaded yet
      return(TRUE);
   PutDefaultInt("NumCustomToolBoxes", m_List.GetCount());
   for (i = 0,Pos = GetHeadPosition(); Pos != NULL; i++)
      {
      pctb = (CCustToolBox *)GetNext(Pos);
   	  if (!pctb->m_IsInsitue)
		 {
	     itoa(i, digits, 10);
	     Name  = "CustToolBox";
	     Name += digits;
	     if (pctb->GetUp())         // was Dialog Up
	        Val = "1,";
	     else
	        Val = "0,";
	     Val  += pctb->GetName();
	     lpbuf = Name.GetBuffer(256);
	     lpbuf2 = Val.GetBuffer(256);
	     PutDefaultString(lpbuf, lpbuf2);
	     Val.ReleaseBuffer();
	     Name.ReleaseBuffer();
	     pctb->Save();
		 }	
      }
   return(ret);
   }

// this routines assumes that the main toolbar is the last one on the list,
// so load him first

BOOL CCustToolList::Load(CWnd *pParent, BOOL MainOnly)
   {
   LPSTR           lpbuf;
   LPSTR           lpbuf2;
   BOOL            ret = TRUE;
   int             i, num;
   CString         Name, Val;
   char            digits[4];
   STRING          szString;
   COLORSPACE      ColorSpace;

   hPopup = InitPopupTools();

   // Read the active color preference
   GetDefaultString("Color1", "0,0,0,0", szString, sizeof(STRING));
   ColorSpace = (COLORSPACE)GetDefaultInt( "Space1", CS_GRAY);
   SetActiveColorString(szString, ColorSpace);

   // Read the alternate color preference
   GetDefaultString("Color2", "255,255,255,255", szString, sizeof(STRING));
   ColorSpace = (COLORSPACE)GetDefaultInt("Space2", CS_GRAY);
   SetAlternateColorString(szString, ColorSpace);

	lstrcpy(szCustomToolFile, CUSTFILENAME);
   GetIniPath(szCustomToolFile);

   num = GetDefaultInt("NumCustomToolBoxes", 1);
   if (!num)
      {
      Name = "Main";
      Add(Name, TRUE, pParent);
      }
   else for (i=num - 1; i >= 0; i--)
      {
      itoa(i, digits, 10);
      Name  = "CustToolBox";
      Name += digits;
      lpbuf  = Val.GetBuffer(256);
      lpbuf2 = Name.GetBuffer(256);
      if (i == (num - 1))
         GetDefaultString(lpbuf2, "1,Main", lpbuf, 256);
      else
         GetDefaultString(lpbuf2, "0,", lpbuf, 256);
      Val.ReleaseBuffer();
      Name.ReleaseBuffer();
      Name = Val.Right(Val.GetLength() - 2);
      Add(Name, MainOnly? FALSE : Val[0] == '1', pParent);
      }
   NotifyOfActiveTool(IDC_MASK, IDC_SQUARE);
   ActivateTool(IDC_SQUARE);
   return(ret);
   }


CCustToolBox *CCustToolList::CreateInsitueBox(CWnd *pParent)
   {
   CCustToolBox *pBox = new CCustToolBox;

   pBox->Create(pParent);
   Add(pBox);
   return(pBox);
   }

BOOL CCustToolList::Add(CString &Name, BOOL Open, CWnd *pParent)
   {
   CCustToolBox   *pctb;
   
   pctb = new CCustToolBox(Name);
   if (Open || pctb->m_IsMain)
      pctb->Create(pParent);
   if (pctb->m_IsMain)
      m_List.AddTail(pctb);
   else
      m_List.AddHead(pctb);
   return(TRUE);
   }

BOOL CCustToolList::Delete(POSITION Pos)
   {
   CCustToolBox  *pctb;
   LPSTR          lpstr1;
   CString        Name;

   if (!Pos)
      return(FALSE);
   pctb = (CCustToolBox *)GetAt(Pos);
   if (pctb->GetSafeHwnd())
      pctb->OnCommand(SC_CLOSE, (LPARAM) 0);
   m_List.RemoveAt(Pos);

   Name   = pctb->GetName();
   lpstr1 = Name.GetBuffer(256);
   WritePrivateProfileString(lpstr1, NULL, NULL, szCustomToolFile); // delete section
   Name.ReleaseBuffer();

   delete pctb;
   return(TRUE);
   }

BOOL CCustToolList::Delete(UINT i)
   {
   return(Delete(FindIndex(i)));
   }

BOOL CCustToolList::ReName(UINT i, CString &Name)
   {
   POSITION       Pos  = FindIndex(i);
   CCustToolBox  *pctb;

   pctb = (CCustToolBox *)GetAt(Pos);
   pctb->ReName(Name);
   
   return(TRUE);
   }

/***************************************************************************/
/*                      CCustToolBox                                       */
/***************************************************************************/
IMPLEMENT_DYNAMIC(CCustToolBox, CDialog)
BEGIN_MESSAGE_MAP(CCustToolBox, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
   ON_WM_NCACTIVATE()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

CCustToolBox::CCustToolBox()
   {
   m_Name            = "Main";
   m_WasUp           = FALSE;
   m_NumNonIconTools = 0;
   m_Pos             = CPoint(-1,-1);
   m_Width           = 1;
   m_Patch           = FALSE;
   m_IsMain          = TRUE;
   m_IsInsitue       = TRUE;
   LoadMain();
   }

CCustToolBox::CCustToolBox(CString &Name)
   {
   m_Name            = Name;
   m_WasUp           = FALSE;
   m_NumNonIconTools = 0;
   m_Pos             = CPoint(-1,-1);
   m_Width           = 2;
   m_Patch           = FALSE;
   m_IsMain          = FALSE;
   m_IsInsitue       = FALSE;
   Load();
   }

CCustToolBox::CCustToolBox(const CCustToolBox &Src)
   {
   CCustTool   *pct;
   POSITION     Pos;

   m_Name            = Src.m_Name;
   m_WasUp           = Src.m_WasUp;
   m_NumNonIconTools = Src.m_NumNonIconTools;
   m_Pos             = Src.m_Pos;
   m_Width           = Src.m_Width;
   m_Patch           = Src.m_Patch;
   m_IsMain          = Src.m_IsMain;
   m_IsInsitue       = Src.m_IsInsitue;

   for (Pos = Src.m_List.GetHeadPosition(); Pos != NULL;)
      {
      pct = (CCustTool *)Src.m_List.GetNext(Pos);
      pct = new CCustTool(*pct);
      m_List.AddTail(pct);
      }
   }
   
LRESULT CCustToolBox::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
	if (GetStyle() & WS_VISIBLE)        // ignore if child is invisible
	{
		CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
		if (pTarget == NULL || !pTarget->IsFrameWnd())
			pTarget = GetParentFrame();
		if (pTarget != NULL)
			OnUpdateCmdUI(pTarget, (BOOL)wParam);
	}
	return 0L;
}

void CCustToolBox::OnUpdateCmdUI(CFrameWnd *pTarget, BOOL bDisableIfNoHndler)
{
	// update the dialog controls in the toolbox
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

void CCustToolBox::ReLoad()
   {
   if (GetSafeHwnd())
      OnCommand(SC_CLOSE, (LPARAM) 0);
   DeleteAll();
   Load(FALSE);
   Create();
   }

CCustToolBox::~CCustToolBox()
   {
   DeleteAll();
   }

void CCustToolBox::DeleteAll()
   {
   CCustTool   *pct;
   POSITION     Pos;

   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
      {
      pct = (CCustTool *)m_List.GetNext(Pos);
      delete pct;
      }
   m_List.RemoveAll();
   m_NumNonIconTools = 0;
   m_ColorPatch      = NULL;
   }
   
void CCustToolBox::Delete(UINT i, BOOL Icon)
   {
   CCustTool      *pct;
   POSITION        Pos;

   if (i != -1)
      {
      if (Icon)
         i += m_NumNonIconTools;
      Pos  = m_List.FindIndex(i);
      pct = (CCustTool *)m_List.GetAt(Pos);
	   m_List.RemoveAt(Pos);
      if (pct->GetType() != CUSTOOL_ICON)
         m_NumNonIconTools--;
      delete pct;
      }
   }

void CCustToolBox::ReName(CString &Name)
   {
   LPSTR          lpstr1;

   lpstr1 = m_Name.GetBuffer(256);
   WritePrivateProfileString(lpstr1, NULL, NULL, szCustomToolFile); // delete section
   m_Name.ReleaseBuffer();

   m_Name = Name;
   Save();
   }

void CCustToolBox::Add(CCustTool *pct, POSITION Pos)
   {
   if (!Pos)
      {
      if (pct->GetType() == CUSTOOL_ICON)
         m_List.AddTail(pct);
      else if (!m_NumNonIconTools)
         m_List.AddHead(pct);
      else
         {
         Pos  = m_List.FindIndex(m_NumNonIconTools - 1);
         m_List.InsertAfter(Pos, pct);
         }
      }
   else
      m_List.InsertBefore(Pos, pct);
   if (pct->GetType() != CUSTOOL_ICON)
      m_NumNonIconTools++;
   }

void CCustToolBox::UpdateActiveTool(int id)
   {
   if (!GetSafeHwnd())
      return;
   if (id >= IDC_FIRSTTOOL && id <= IDC_LASTTOOL)
      CheckRadioButton(IDC_FIRSTTOOL, IDC_LASTTOOL, id);
   else
      CheckRadioButton(IDC_FIRSTICON, IDC_LASTICON, id);
   }

BOOL CCustToolBox::Create(CWnd *pParent)
   {
   CCustTool   *pct;
   CFont       *pFont;
   BOOL         ret;
   POSITION     Pos;
   LOGFONT      fnt;

   if (!pParent)
      pParent = CWnd::FromHandle(PictPubApp.Get_hWndAstral());
   ret = CDialog::Create(IDD_CUSTOM_TOOLBOX, pParent);
   pFont = GetFont();
   pFont->GetObject(sizeof(LOGFONT), &fnt);
   CCustTool::ButtonHeight = fnt.lfHeight * 2;
   if (CCustTool::ButtonHeight < 0)
      CCustTool::ButtonHeight = -CCustTool::ButtonHeight;
   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
      {
      pct = (CCustTool *)m_List.GetNext(Pos);
      pct->Create(this, m_Width);
      }
   if (m_Patch)
      {
      m_ColorPatch = CreateWindow("color2","",WS_CHILD | WS_VISIBLE, 0, 0,
         ICON_HEIGHT, ICON_HEIGHT * 1.2, GetSafeHwnd(), (HMENU)IDC_ACTIVECOLOR, PictPubApp.Get_hInstAstral(), NULL);
      }

   ReSize();
   if (m_Pos.x == -1 && m_Pos.y == -1)
      {
      CRect  r;
      
      ::GetWindowRect(PictPubApp.Get_hWndAstral(), &r);
      m_Pos.x = r.Width()  / 2;
      m_Pos.y = r.Height() / 2;
      }

	UINT swpflags = SWP_NOSIZE | SWP_DRAWFRAME;
	if (!m_IsInsitue)
		swpflags |= SWP_SHOWWINDOW;
	SetWindowPos(NULL, m_Pos.x, m_Pos.y, 0, 0, swpflags);

   return(ret);
   }

BOOL CCustToolBox::Save()
   {
   BOOL           ret = TRUE;
   LPSTR          lpstr1, lpstr2;
   CCustTool     *lpcs;
   POSITION       Pos;
   int            i;
   CString        Val;
   char           digits[16];

   if (m_IsInsitue)
      return(TRUE);

   lpstr1 = m_Name.GetBuffer(256);
   WritePrivateProfileString(lpstr1, NULL, NULL, szCustomToolFile); // delete section

   itoa((UINT)m_Pos.x, digits, 10);
   Val    = digits;
   Val   += ',';
   itoa((UINT)m_Pos.y, digits, 10);
   Val   += digits;
   lpstr2 = Val.GetBuffer(256);
   WritePrivateProfileString(lpstr1, BOX_POSITION, lpstr2, szCustomToolFile);
   Val.ReleaseBuffer();

   WritePrivateProfileString(lpstr1, COLORPATCH, m_Patch?"1":"0", szCustomToolFile);

   itoa((UINT)m_Width, digits, 10);
   Val    = digits;
   lpstr2 = Val.GetBuffer(256);
   WritePrivateProfileString(lpstr1, BOX_WIDTH, lpstr2, szCustomToolFile);
   Val.ReleaseBuffer();

   for (i = 0,Pos = m_List.GetHeadPosition(); Pos != NULL; i++)
         {
         lpcs   = (CCustTool *)m_List.GetNext(Pos);
         lpcs->Save(i, lpstr1);
         }
   m_Name.ReleaseBuffer();
   return(ret);
   }

BOOL CCustToolBox::Load(BOOL LoadPos)
   {
   BOOL           ret = TRUE;
   LPSTR          lpstr1,lpstr;
   CCustTool     *lpcs;
   CString        SName, LName;
   CString        Val, v;

   lpstr1 = m_Name.GetBuffer(256);

   if (LoadPos)
      {
      lpstr = Val.GetBuffer(256);
      GetPrivateProfileString(lpstr1, BOX_POSITION, "-1,-1", lpstr, 256, szCustomToolFile);
      Val.ReleaseBuffer();
      v = Val.Left(Val.Find(','));
      m_Pos.x = atoi(v);
      v = Val.Right(Val.GetLength() - Val.Find(',') - 1);
      m_Pos.y = atoi(v);
      }

   lpstr = Val.GetBuffer(256);
   GetPrivateProfileString(lpstr1, COLORPATCH, "0", lpstr, 256, szCustomToolFile);
   Val.ReleaseBuffer();
   m_Patch = atoi(Val);

   if (!m_IsInsitue)
      m_IsMain = m_Name == "Main"? 1 : 0;

   lpstr = Val.GetBuffer(256);
   GetPrivateProfileString(lpstr1, BOX_WIDTH, "2", lpstr, 256, szCustomToolFile);
   Val.ReleaseBuffer();
   m_Width = atoi(Val);

   if (m_IsMain)
      LoadMain();
   else for (int i=0; ; i++)
      {
      lpcs = new CCustTool;
      if (lpcs->Load(i, lpstr1))
         Add(lpcs);
      else
         {
         delete lpcs;
         break;
         }
      }
   m_Name.ReleaseBuffer();
   return(ret);
   }

#define TITLE_HEIGHT		   9
#define BORDER_WIDTH		   1
#define SYS_MENU_WIDTH	   12
#define SYS_COOKIE_OFFSET	3
#define SYS_COOKIE_WIDTH	7
#define SYS_COOKIE_HEIGHT	4
#define COLOR_SYSMENU		RGB(192, 192, 192)
#define COLOR_SYSSHADOW		RGB(128, 128, 128)
#define COLOR_COOKIEFILL	RGB(255, 255, 255)
#define COLOR_BORDER		   RGB(0,   0,   0)

void CCustToolBox::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp)
   {
	if (bCalcValidRects)
	   {
		lpncsp->rgrc[0].top    += TITLE_HEIGHT;
		lpncsp->rgrc[0].left   += BORDER_WIDTH;
		lpncsp->rgrc[0].right  -= BORDER_WIDTH;
		lpncsp->rgrc[0].bottom -= BORDER_WIDTH;
	   }
	else
      CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);
   }

UINT CCustToolBox::OnNcHitTest(CPoint pt)
   {
	CRect    r;

	CalcSysMenuRect(FALSE, r);
	if (r.PtInRect(pt))
		return HTSYSMENU;
   return(HTCAPTION);
   }

void CCustToolBox::OnNcLButtonDown(UINT nHitTest, CPoint pt)
   {
	if (nHitTest == HTSYSMENU)
	   {
      if (!m_IsMain)
         {
	      CRect     r;
         CMenu     *Menu;

		   InvertSysMenu();
	      GetWindowRect(r);
         Menu = GetSystemMenu(FALSE);
	      SetKeyState(VK_LBUTTON, YES, NO);
         Menu->TrackPopupMenu(0, r.left, r.top + TITLE_HEIGHT, this,NULL);
		   InvertSysMenu();
         }
	   }
	else 
		CDialog::OnNcLButtonDown(nHitTest, pt);
   }

void CCustToolBox::OnNcLButtonUp(UINT nHitTest, CPoint pt)
   {
	CDialog::OnNcLButtonUp(nHitTest, pt);
   }

void CCustToolBox::InvertSysMenu()
   {
	CRect    rMenu;

	CalcSysMenuRect(TRUE, rMenu);

	rMenu.InflateRect(-1, -1);
	CDC *pDC = GetWindowDC();
	if (!pDC) 
      return;

	pDC->InvertRect(rMenu);	
	ReleaseDC(pDC);
   }

BOOL CCustToolBox::OnNcActivate(BOOL f)
   {
   OnNcPaint();
   return(TRUE);
   }

void CCustToolBox::OnNcPaint()
   {
	CRect       r, rTitle, rMenu, rCookie;

	GetWindowRect(r);
	r.OffsetRect(-r.left, -r.top);
	CalcTitleRect(TRUE, rTitle);
	CalcSysMenuRect(TRUE, rMenu);
	rCookie.SetRect(
		rMenu.left + SYS_COOKIE_OFFSET + 1,
		rMenu.top  + SYS_COOKIE_OFFSET + 1,
		rMenu.left + SYS_COOKIE_OFFSET + SYS_COOKIE_WIDTH,
		rMenu.top  + SYS_COOKIE_OFFSET + SYS_COOKIE_HEIGHT);
	
// Draw the title bar, sys menu cookie, and border	
	CDC *pDC = GetWindowDC();
	if (!pDC)
      return;
	
	CBrush TitleBrush(GetSysColor(COLOR_ACTIVECAPTION));
	CBrush FrameBrush(COLOR_BORDER);
	CBrush MenuBrush(COLOR_SYSMENU);
	CBrush CkBrush(COLOR_COOKIEFILL);
	CBrush CkBrushShadow(COLOR_SYSSHADOW);
	
	pDC->FillRect(rTitle, &TitleBrush);
	pDC->FillRect(rMenu,  &MenuBrush);
	
	pDC->FrameRect(rTitle,  &FrameBrush);
	pDC->FrameRect(r,       &FrameBrush);
	pDC->FrameRect(rMenu,   &FrameBrush);
	pDC->FrameRect(rCookie, &CkBrushShadow);
	rCookie.OffsetRect(-1, -1);
	pDC->FillRect(rCookie,  &CkBrush);
	pDC->FrameRect(rCookie, &FrameBrush);
	
   if (!m_IsMain)
      {
	   CFont fnt;
	   int FntHeight = 12;
	   if (fnt.CreateFont(FntHeight,0,0,0,FW_NORMAL,FALSE,FALSE,0,0,OUT_DEFAULT_PRECIS,
		   CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
		   DEFAULT_PITCH|FF_DONTCARE|TMPF_TRUETYPE,NULL))
	      {
         LPSTR    lp   = m_Name.GetBuffer(256);
         rTitle.left += rMenu.right - rTitle.left;
		   CFont   *pfont = pDC->SelectObject(&fnt);
		   pDC->SetBkMode(TRANSPARENT);
		   pDC->SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
		   pDC->DrawText((LPCSTR)lp,-1,&rTitle,DT_CENTER|DT_VCENTER);
		   pDC->SelectObject(pfont);
		   fnt.DeleteObject();
         m_Name.ReleaseBuffer();
	      }
      }

	ReleaseDC(pDC);
   }

void CCustToolBox::CalcSysMenuRect(BOOL bClient, CRect &rMenu)
   {
	CRect r;

	GetWindowRect(&r);
	if (bClient)
		r.OffsetRect(-r.left, -r.top);
	rMenu = r;
	rMenu.bottom = rMenu.top  + TITLE_HEIGHT;
	rMenu.right  = rMenu.left + SYS_MENU_WIDTH;
   }

void CCustToolBox::CalcTitleRect(BOOL bClient, CRect &rTitle)
   {
	CRect r;

	GetWindowRect(&r);
	if (bClient)
		r.OffsetRect(-r.left, -r.top);
	rTitle = r;
	rTitle.bottom = rTitle.top + TITLE_HEIGHT;
   }

void CCustToolBox::FindTool(HWND hDlg, int id, HWND hControl, UINT codeNotify)
   {
   POSITION        Pos;
   CCustTool      *pct = NULL;
   CCustToolList  *CBList    = PictPubApp.GetCustomBoxes();

   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
      {
      pct = (CCustTool *)m_List.GetNext(Pos);
      if (pct->GetHWnd() == hControl)
         break;
      pct = NULL;
      }
   if (pct && pct->GetType() == CUSTOOL_MACRO)
      pct->PlayMacro();
   else
      CBList->SetDlgCtrl(hDlg, hControl, codeNotify);
   return;
   }

extern BOOL WINPROC EXPORT DlgSaveToolBoxProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam);
BOOL CCustToolBox::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (GET_WM_COMMAND_ID(wParam, lParam))
    {
        case SC_MOVE:
            FORWARD_WM_SYSCOMMAND(GetSafeHwnd(),SC_MOVE, 0, 0, ::SendMessage);
            break;
        case SC_CLOSE:
		    if (!m_IsInsitue)
         	    FORWARD_WM_CLOSE(GetSafeHwnd(), ::SendMessage);
            break;
        case IDM_CUSTBOXEDIT:
            if( Control.Retail )
                CEditCustTool(this).DoModal();
            break;
        case IDM_CUSTBOXLOAD:
            if( Control.Retail )
            {
                CurCustomToolBox = this;
		        FORWARD_WM_COMMAND( PictPubApp.Get_hWndAstral(),
			    			        IDM_CUSTBOXLOAD, NULL, 0, ::SendMessage );
            }
            CurCustomToolBox = NULL;
            break;
	    case IDC_ACTIVECOLOR:
            HANDLE_WM_COMMAND(GetSafeHwnd(), wParam, lParam, OnActiveColor);
		    break;
        default:
            AfxGetMainWnd()->SetFocus();
            if (GET_WM_COMMAND_ID(wParam, lParam) == IDC_QUICKZOOM)
                wParam = IDM_SHOWZOOMBOX;
            HANDLE_WM_COMMAND(GetSafeHwnd(), wParam, lParam, FindTool);
		    FORWARD_WM_COMMAND( PictPubApp.Get_hWndAstral(),
							    wParam, NULL, 0, ::SendMessage );
            return(TRUE);
    }
    CDialog::OnCommand(wParam, lParam);
    return(TRUE);
}

void CCustToolBox::OnActiveColor(HWND hDlg, int id, HWND hControl, UINT codeNotify)
   {
   BOOL        fDoubleClick = (codeNotify == 1);
   COLORINFO   ColorInfo, AltColorInfo;
   LPIMAGE     lpImage;
   FRMTYPEINFO TypeInfo;

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
         ::InvalidateRect(m_ColorPatch, NULL, FALSE);
         break;
      default:
         break;
      }
   }

extern HACCEL		hAccelTable;

BOOL CCustToolBox::PreTranslateMessage(MSG* pMsg)
   {
   if (TranslateAccelerator( AfxGetMainWnd()->GetSafeHwnd(), hAccelTable, (LPMSG)pMsg ) )
      return(TRUE);

	if (CDialog::PreTranslateMessage(pMsg))
		return(TRUE);

   return(FALSE); // FALSE if the message should be processed in the normal way
   }

BOOL CCustToolBox::OnInitDialog()
{
    CMenu      *Menu;
    LPSTR       lp;
    if( Control.Retail )
    {
        Menu = GetSystemMenu(FALSE);
        Menu->AppendMenu(MF_SEPARATOR, 0);
        AstralStr(IDS_LOADCUSTTOOL, &lp);
        Menu->AppendMenu(MF_ENABLED | MF_STRING,IDM_CUSTBOXLOAD, lp);
        AstralStr(IDS_EDITCUSTTOOL, &lp);
        Menu->AppendMenu(MF_ENABLED | MF_STRING,IDM_CUSTBOXEDIT, lp);

        CDialog::OnInitDialog();
    }
    return(FALSE);
}

BOOL CCustToolBox::ReSize()
   {
   UINT         X = ICON_HEIGHT * m_Width + BORDER_WIDTH * 2;
   UINT         Y = TITLE_HEIGHT;
   UINT         h;
   POSITION     Pos;
   CCustTool   *pct;
   UINT         i = 0;
   UINT         Order = 0;

   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
      {
      pct = (CCustTool *)m_List.GetNext(Pos);
      h   = pct->RePosition(i, Order, m_Width);
      if (pct->GetType() == CUSTOOL_ICON)
         Order++;
      else
         {
         Y += h;
         i++;
         }
      }
   
   Y += ICON_HEIGHT * ((Order + m_Width - 1) / m_Width);
   if (m_Patch)
      {
      int   xx = ICON_HEIGHT * m_Width;
      int   yy = ICON_HEIGHT * 1.2;
	   CWnd::FromHandle(m_ColorPatch)->SetWindowPos(NULL, 0, Y - TITLE_HEIGHT, xx, yy, SWP_SHOWWINDOW | SWP_NOZORDER);
      Y += (UINT)ICON_HEIGHT * 1.2;
      }
	UINT swpflags = SWP_NOMOVE | SWP_NOZORDER;
	if (!m_IsInsitue)
		swpflags |= SWP_SHOWWINDOW;
	SetWindowPos(NULL, 0, 0, X, Y, swpflags);
   return(TRUE);
   }

BOOL CCustToolBox::OnEraseBkgnd(CDC* pDC)
   {
   CBrush   br;
   CRect    r;

   GetClientRect(&r);
   br.CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
   pDC->FillRect(&r, &br);
   return(TRUE);
   }

void CCustToolBox::OnClose()
   {
   DestroyWindow();
   m_WasUp = FALSE;
   }

void CCustToolBox::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItem)
   {
   DrawPopupMenu(lpDrawItem);
   }

void CCustToolBox::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItem)
   {
   MeasurePopupMenu(lpMeasureItem);
   }

void CCustToolBox::OnDestroy()
   {
   CCustTool   *pct;
   CWnd        *pParent;
   CRect        r,rr;
   POSITION     Pos;

   GetWindowRect(&r);
   pParent = GetParent();
   pParent->GetClientRect(&rr);
   pParent->ClientToScreen(&rr);
   m_Pos.x = r.left - rr.left;
   m_Pos.y = r.top  - rr.top;
   CDialog::OnDestroy();
   for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
      {
      pct = (CCustTool *)m_List.GetNext(Pos);
      pct->SetHWnd();
      }
   m_WasUp = TRUE;
   }

afx_msg BOOL CCustToolBox::OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT msg)
    {
    CPoint pt;
    HWND hChild;

    // check for disabled control
    GetCursorPos( &pt );
    if( hChild = MyChildWindowFromPoint( pWnd->GetSafeHwnd(), pt ))
        {
        return( SetupCursor( hChild, nHitTest, 0 ) );
        }
	return ( SetupCursor( pWnd->GetSafeHwnd(), nHitTest, 0 ));
    }

extern void ShowChromaProc(HWND hWindow, UINT msg, int x, int y, UINT32 Option);

/***********************************************************************/
ITEMID ActivateTool( ITEMID idTool )
/***********************************************************************/
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


/***********************************************************************/
void DeactivateTool()
/***********************************************************************/
{
HWND hActiveWnd;

if ( Tool.bActive && Tool.lpToolProc )
	{
	if (hActiveWnd = GetActiveDoc())
		DestroyProc( hActiveWnd, 0L );
	}
}

extern BOOL fAppActive;
/***********************************************************************/
void InstallTool( BOOL fAlternate )
/***********************************************************************/
{
WORD idCursor, idRibbon;
BOOL fDisplaysInfo, fSetCursor, fConstrain, fAutoScrollSetPos;
DLGPROC lpRibbon;
LPTOOLPROC lpTool;
// HWND	hWnd;

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
//		AstralDlg( YES, PictPubApp.Get_hInstAstral(), PictPubApp.Get_hWndAstral(), 
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


/***********************************************************************/
static HMENU InitPopupTools()
/***********************************************************************/
{
HMENU hPopup;
WORD wState;
#define MENUFLAGS1 (MF_OWNERDRAW | MF_BYCOMMAND)
#define MENUFLAGS2 (MF_OWNERDRAW | MF_BYCOMMAND | MF_MENUBARBREAK)
#define MENUFLAGS3 (MF_OWNERDRAW | MF_BYCOMMAND | MF_MENUBREAK)

/* Get the menu for the popup from the resource file. */
if ( !(hMenu = LoadMenu( PictPubApp.Get_hInstAstral(), "POPUPTOOLS" )) )
	return( NULL );

// Set the ownerdraw flags; if this could happen if the RC file, it would
hPopup = GetSubMenu( hMenu, IDC_MASK - IDC_FIRSTTOOL );
wState = GetMenuState(hPopup, IDC_SQUARE, MF_BYCOMMAND ) & MF_GRAYED;
ModifyMenu( hPopup, IDC_SQUARE,     MENUFLAGS1|wState, IDC_SQUARE, NULL );
//wState = GetMenuState(hPopup, IDC_CIRCLE, MF_BYCOMMAND ) & MF_GRAYED;
//ModifyMenu( hPopup, IDC_CIRCLE,     MENUFLAGS2|wState, IDC_CIRCLE, NULL );
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

CheckMenuItem( hMenu, idMaskFunction = IDC_SQUARE, MF_CHECKED );
CheckMenuItem( hMenu, idRetouchFunction = IDC_PAINT, MF_CHECKED );
CheckMenuItem( hMenu, idFilterFunction = IDC_SHARP, MF_CHECKED );
CheckMenuItem( hMenu, idFillFunction = IDC_VIGNETTE, MF_CHECKED );
CheckMenuItem( hMenu, idDrawFunction = IDC_PENCIL, MF_CHECKED );
return( hMenu );
}

/***********************************************************************/
void SaveRetouchBrush( int brush )
/***********************************************************************/
{
if (brush == NO_BRUSH)
	return;

lpBrushSettings [brush].BrushShape = Retouch.BrushShape;
lpBrushSettings [brush].BrushSize = Retouch.BrushSize;
lpBrushSettings [brush].Opacity = Retouch.Opacity;
lpBrushSettings [brush].Pressure = Retouch.Pressure;
lpBrushSettings [brush].MergeMode = Retouch.MergeMode;
lpBrushSettings [brush].Feather = Retouch.Feather;
}

/***********************************************************************/
void SelectRetouchBrush( int brush )
/***********************************************************************/
{
//DisplayBrush(0, 0, 0, OFF);
if (brush == NO_BRUSH)
	return;

SaveRetouchBrush( RetouchBrushCurrent );

Retouch.BrushShape = lpBrushSettings [brush].BrushShape;
Retouch.BrushSize = lpBrushSettings [brush].BrushSize;
Retouch.Opacity = lpBrushSettings [brush].Opacity;
Retouch.Pressure = lpBrushSettings [brush].Pressure;
Retouch.MergeMode = lpBrushSettings [brush].MergeMode;
Retouch.Feather = lpBrushSettings [brush].Feather;

RetouchBrushCurrent = brush;
}


#define OUTER_LEFT 0
#define OUTER_TOP 0
#define OUTER_RIGHT 0
#define OUTER_BOTTOM 0

/***********************************************************************/
void StretchTopbar( HWND hWindow )
/***********************************************************************/
{
int flag, Height, iHalfXBorder, iHalfYBorder;
RECT rect, rClient;

return;
GetWindowRect( hWindow, &rect );
GetClientRect( hWindow, &rClient );
iHalfXBorder = ( RectWidth(&rect) - RectWidth(&rClient) ) / 2;
iHalfYBorder = ( RectHeight(&rect) - RectHeight(&rClient) ) / 2;
Height = RectHeight( &rect );

GetClientRect( PictPubApp.Get_hWndAstral(), &rect );
InflateRect( &rect, iHalfXBorder, iHalfYBorder );
rect.left += OUTER_LEFT;
rect.right -= OUTER_RIGHT;
rect.top += OUTER_TOP;
flag = ( IsWindowVisible(hWindow) ? 0: SWP_NOREDRAW );
SetWindowPos( hWindow, NULL, rect.left, rect.top,
	RectWidth( &rect ) - 1, Height - 1,
	flag | SWP_NOZORDER | SWP_NOACTIVATE );
}


/***********************************************************************/
void StretchBottombar( HWND hWindow , CRect Newr)
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


#ifdef ID_MAGIC
/***********************************************************************/
void StretchLeftbar( HWND hWindow, int dyRibbon, int dyStatus )
/***********************************************************************/
{
int flag, Width, iHalfXBorder, iHalfYBorder;
RECT rect, rClient;
HWND hWnd;

if ( !dyRibbon || !dyStatus )
	{
	GetClientRect( PictPubApp.Get_hWndAstral(), &rClient );
	ClientToScreen( PictPubApp.Get_hWndAstral(), (LPPOINT)&rClient.left );
	ClientToScreen( PictPubApp.Get_hWndAstral(), (LPPOINT)&rClient.right );
	if ( !dyRibbon && (hWnd = AstralDlgGet( Tool.idRibbon )) )
		{
		if ( IsWindowVisible(hWnd) )
			{
			GetWindowRect( hWnd, &rect );
			rect.right--; rect.bottom--;
			dyRibbon = rect.bottom - rClient.top + 1;
			}
		}
	if ( !dyStatus && (hWnd = AstralDlgGet( IDD_STATUS )) )
		{
		if ( IsWindowVisible(hWnd) )
			{
			GetWindowRect( hWnd, &rect );
			rect.right--; rect.bottom--;
			dyStatus = rClient.bottom - rect.top + 1;
			}
		}
	}

GetWindowRect( hWindow, &rect );
GetClientRect( hWindow, &rClient );
iHalfXBorder = ( RectWidth(&rect) - RectWidth(&rClient) ) / 2;
iHalfYBorder = ( RectHeight(&rect) - RectHeight(&rClient) ) / 2;
Width = RectWidth( &rect );

GetClientRect( PictPubApp.Get_hWndAstral(), &rect );
InflateRect( &rect, iHalfXBorder, iHalfYBorder );
rect.left += OUTER_LEFT;
rect.top += dyRibbon;
rect.bottom -= dyStatus;
rect.bottom++;
flag = ( IsWindowVisible(hWindow) ? 0: SWP_NOREDRAW );
SetWindowPos( hWindow, NULL, rect.left, rect.top,
	Width - 1, RectHeight( &rect ) - 1,
	flag | SWP_NOZORDER | SWP_NOACTIVATE );
}

/***********************************************************************/
void StretchRightbar( HWND hWindow, int dyRibbon, int dyStatus )
/***********************************************************************/
{
int flag, Width, iHalfXBorder, iHalfYBorder;
RECT rect, rClient;
HWND hWnd;

if ( !dyRibbon || !dyStatus )
	{
	GetClientRect( PictPubApp.Get_hWndAstral(), &rClient );
	ClientToScreen( PictPubApp.Get_hWndAstral(), (LPPOINT)&rClient.left );
	ClientToScreen( PictPubApp.Get_hWndAstral(), (LPPOINT)&rClient.right );
	if ( !dyRibbon && (hWnd = AstralDlgGet( Tool.idRibbon )) )
		{
		if ( IsWindowVisible(hWnd) )
			{
			GetWindowRect( hWnd, &rect );
			rect.right--; rect.bottom--;
			dyRibbon = rect.bottom - rClient.top + 1;
			}
		}
	if ( !dyStatus && (hWnd = AstralDlgGet( IDD_STATUS )) )
		{
		if ( IsWindowVisible(hWnd) )
			{
			GetWindowRect( hWnd, &rect );
			rect.right--; rect.bottom--;
			dyStatus = rClient.bottom - rect.top + 1;
			}
		}
	}

GetWindowRect( hWindow, &rect );
GetClientRect( hWindow, &rClient );
iHalfXBorder = ( RectWidth(&rect) - RectWidth(&rClient) ) / 2;
iHalfYBorder = ( RectHeight(&rect) - RectHeight(&rClient) ) / 2;
Width = RectWidth( &rect );

GetClientRect( PictPubApp.Get_hWndAstral(), &rect );
InflateRect( &rect, iHalfXBorder, iHalfYBorder );
rect.left = rect.right - Width + 1 - OUTER_RIGHT;
rect.top += dyRibbon;
rect.bottom -= dyStatus;
rect.bottom++;
flag = ( IsWindowVisible(hWindow) ? 0: SWP_NOREDRAW );
SetWindowPos( hWindow, NULL, rect.left, rect.top,
	Width - 1, RectHeight( &rect ) - 1,
	flag | SWP_NOZORDER | SWP_NOACTIVATE );
}
#endif

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


/***********************************************************************/
void MeasurePopupMenu( LPMEASUREITEMSTRUCT lpMeasure )
/***********************************************************************/
{
DWORD dSize;

dSize = GetMenuCheckMarkDimensions();
lpMeasure->itemWidth = 32 - LOWORD(dSize);
lpMeasure->itemHeight = 32;
}


/***********************************************************************/
void DrawPopupMenu( const DRAWITEMSTRUCT FAR * lpDraw )
/***********************************************************************/
{
int x, y;
RECT ClientRect;
HICON hResource;
HDC hDC;
BOOL bFocus, bDown, bGrayed;
WORD idIcon;
int offset;

if (Control.fNewLook)
	offset = 1;
else
	offset = 2;

// (lpDraw->itemAction == ODA_SELECT)
// (lpDraw->itemAction == ODA_DRAWENTIRE)

bFocus = NO;
bDown = (lpDraw->itemState & ODS_SELECTED);
bGrayed = (lpDraw->itemState & ODS_GRAYED);
hDC = lpDraw->hDC;
ClientRect = lpDraw->rcItem;
idIcon = lpDraw->itemID;

if (Control.fNewLook)
	{
	ClientRect.left -= 2;
	ClientRect.right += 2;
	}
// Draw the box interior
if ( !bDown )
	FillRect( hDC, &ClientRect, Window.hButtonBrush );
else
if ( Control.fNewLook )
	{
    HBRUSH hBrush = CreateSolidBrush( GetSysColor(COLOR_HIGHLIGHT) );
	FillRect( hDC, &ClientRect, hBrush );
	DeleteObject(hBrush);
	}
else
	HilightRect( hDC, &ClientRect, Window.hHilightBrush );

if (Control.fNewLook)
	{
	ClientRect.left += 2;
	ClientRect.right -= 2;
	}
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
	MgxGrayArea(hDC, ClientRect.left, ClientRect.top,
			ClientRect.right-ClientRect.left,
			ClientRect.bottom-ClientRect.top);
	}
if ( bDown )
    {
    HelpContext.OnHints( NULL, 0 );
	HintLine( idIcon );
    }
}

#ifdef _DEBUG
void CCustToolBox::AssertValid() const
{
	m_List.CObList::AssertValid();
}

void CCustToolBox::Dump(CDumpContext& dc) const
{
	m_List.CObList::Dump(dc);
}
#endif

void OnViewSlime(int id)
   {
   int      iMode;
   LPIMAGE  lpImage;
   HWND     hDlg;
   CCustToolList  *CBList    = PictPubApp.GetCustomBoxes();

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
   CCustToolList  *CBList    = PictPubApp.GetCustomBoxes();

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
   CCustToolList  *CBList    = PictPubApp.GetCustomBoxes();

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
			AstralDlg(YES, PictPubApp.Get_hInstAstral(), 
				PictPubApp.Get_hWndAstral(), IDD_PALETTE, DlgPaletteProc);
		else
			ShowWindow( hWnd, SW_NORMAL );
		UpdateStatusBar( NO, NO, NO, YES );
		}
   }

void OnStatusInfo()
{
    LPIMAGE lpImage = GetActiveImage();
    if (lpImage)
        lpImage->DoSummaryInfo();
}

extern void TurnOffShieldPreview();
void OnStatusShield(int id) // hDlg has to be dlg handle for status bar
   {
   BOOL Bool;
   HWND hWnd;
   HWND hDlg;
   CCustToolList  *CBList    = PictPubApp.GetCustomBoxes();

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
			AstralDlg( YES, PictPubApp.Get_hInstAstral(), PictPubApp.Get_hWndAstral(),
				IDD_SHIELDFLOAT, DlgShieldFloatProc );
		else
         {
         ColorMask.On = YES;
			ShowWindow( hWnd, SW_NORMAL );
			}
		UpdateStatusBar( NO, NO, NO, YES );
		}
   }
