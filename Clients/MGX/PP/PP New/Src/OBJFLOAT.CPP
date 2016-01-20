// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "objfloat.h"

#define TITLE_HEIGHT	    9
#define BORDER_WIDTH	    1
#define SYS_MENU_WIDTH	    12
#define SYS_COOKIE_OFFSET	3
#define SYS_COOKIE_WIDTH	7
#define SYS_COOKIE_HEIGHT	4
#define COLOR_SYSMENU		RGB(192, 192, 192)
#define COLOR_SYSSHADOW		RGB(128, 128, 128)
#define COLOR_COOKIEFILL	RGB(255, 255, 255)
#define COLOR_BORDER	    RGB(0,   0,   0)

CObjFloat   ObjFloat;
/***************************************************************************/
/*                      CObjFloat                                       */
/***************************************************************************/
IMPLEMENT_DYNAMIC(CObjFloat, CDialog)
BEGIN_MESSAGE_MAP(CObjFloat, CDialog)
   ON_WM_CLOSE()
   ON_WM_NCHITTEST()
   ON_WM_NCCALCSIZE()
   ON_WM_NCACTIVATE()
   ON_WM_NCPAINT()
   ON_WM_SHOWWINDOW()
   ON_WM_PAINT()
   ON_WM_NCLBUTTONDOWN()
   ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BOOL CObjFloat::Create()
   {
   BOOL  ret = CDialog::Create(IDD_OBJFLOAT,CWnd::FromHandle(PictPubApp.Get_hWndAstral()));
   return(ret);
   }

CObjFloat::CObjFloat()
   {
   }

CObjFloat::~CObjFloat()
   {
   }

extern HACCEL		hAccelTable;
BOOL CObjFloat::PreTranslateMessage(MSG* pMsg)
   {
   if (TranslateAccelerator( AfxGetMainWnd()->GetSafeHwnd(), hAccelTable, (LPMSG)pMsg ) )
      return(TRUE);

	if (CDialog::PreTranslateMessage(pMsg))
		return(TRUE);

   return(FALSE); // FALSE if the message should be processed in the normal way
   }

BOOL CObjFloat::OnInitDialog()
   {
   CRect        r,rr;
   int          diff;

   if (Control.fNewLook)
   		SetSmallCaption(GetSafeHwnd());
   RestorePopup( GetSafeHwnd(), "xyObjects", RightPopup );
   GetWindowRect(&r);
	SetWindowPos(NULL, 0, 0, r.Width(), r.Height()/*-1*/, SWP_SHOWWINDOW | SWP_NOMOVE);
   GetWindowRect(&r);
   GetDlgItem(IDC_VIEWOBJ5)->GetWindowRect(&rr);
   diff = r.bottom - rr.bottom;
   SetWindowPos(NULL, 0, 0, r.Width(), r.Height()/* - diff*/, SWP_SHOWWINDOW | SWP_NOMOVE);
   CDialog::OnInitDialog();
   SetupMiniViews(GetSafeHwnd(), NO);
   return(TRUE);
   }

void CObjFloat::OnSetFocus(CWnd* pOldWnd)
   {
   AfxGetMainWnd()->SetFocus();
   }

void CObjFloat::OnShowWindow(BOOL bShow, UINT Status)
   {
   if ( bShow )
	   SetupMiniViews(GetSafeHwnd(), NO);
	CDialog::OnShowWindow(bShow, Status);
   }

void CObjFloat::OnPaint()
{
   	if (Control.fNewLook)
		CDialog::OnPaint();
	else
	{
		PAINTSTRUCT ps;
		CDC      *pDC;
		CWnd     *pWnd;
		RECT     rClient;
		CBrush   Brush;

		if (!(pWnd = GetDlgItem(IDC_VIEWOBJUP)))
   		return;
		pDC = BeginPaint(&ps);
		Brush.CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
		pWnd->GetClientRect(&rClient);
		pWnd->ClientToScreen((LPPOINT)&rClient.left);
		pWnd->ClientToScreen((LPPOINT)&rClient.right);
		ScreenToClient((LPPOINT)&rClient.left);
		ScreenToClient((LPPOINT)&rClient.right);
		rClient.bottom = rClient.top;
		rClient.top = ps.rcPaint.top;
		rClient.left = ps.rcPaint.left;
		rClient.right = ps.rcPaint.right;
		pDC->FillRect(&rClient, &Brush);
		EndPaint(&ps);
	}
}

void CObjFloat::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp)
{
	if (Control.fNewLook)
		CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);
	else
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
}

UINT CObjFloat::OnNcHitTest(CPoint pt)
{
	if (Control.fNewLook)
		return CDialog::OnNcHitTest(pt);
	else
	{
		CRect    r;

		CalcSysMenuRect(FALSE, r);
		if (r.PtInRect(pt))
			return HTSYSMENU;
		return(HTCAPTION);
	}
}

void CObjFloat::OnNcLButtonDown(UINT nHitTest, CPoint pt)
{
	if (Control.fNewLook)
		CDialog::OnNcLButtonDown(nHitTest, pt);
	else
	{
		if (nHitTest != HTSYSMENU)
			CDialog::OnNcLButtonDown(nHitTest, pt);
	}
}

BOOL CObjFloat::OnNcActivate(BOOL f)
{
	if (Control.fNewLook)
		return CDialog::OnNcActivate(f);
	else
	{
		OnNcPaint();
		return(TRUE);
	}
}

void CObjFloat::OnNcPaint()
{
	if (Control.fNewLook)
		CDialog::OnNcPaint();
	else
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

		ReleaseDC(pDC);
	}
}

void CObjFloat::CalcSysMenuRect(BOOL bClient, CRect &rMenu)
   {
	CRect r;

	GetWindowRect(&r);
	if (bClient)
		r.OffsetRect(-r.left, -r.top);
	rMenu = r;
	rMenu.bottom = rMenu.top  + TITLE_HEIGHT;
	rMenu.right  = rMenu.left + SYS_MENU_WIDTH;
   }

void CObjFloat::CalcTitleRect(BOOL bClient, CRect &rTitle)
   {
	CRect r;

	GetWindowRect(&r);
	if (bClient)
		r.OffsetRect(-r.left, -r.top);
	rTitle = r;
	rTitle.bottom = rTitle.top + TITLE_HEIGHT;
   }

void CObjFloat::OnClose()
   {
//   AstralDlgEnd( GetSafeHwnd(), TRUE );
   DestroyWindow();
   }

extern int nTopObject;
extern int nMaxMinis;
BOOL CObjFloat::OnCommand(WPARAM wParam, LPARAM lParam)
   {
   int      id = wParam;
   UINT     codeNotify = HIWORD(lParam);
   BOOL     fDoubleClick;
   int      nObject,nObject2;
   LPOBJECT lpObject, lpToObject;
   LPIMAGE  lpImage;

   lpImage = GetActiveImage();
   fDoubleClick = (codeNotify == 1);
   switch (id)
	   {
      case SC_CLOSE:
         FORWARD_WM_CLOSE(GetSafeHwnd(), ::SendMessage);
         break;
	   case IDC_VIEWOBJ0:
	   case IDC_VIEWOBJ1:
	   case IDC_VIEWOBJ2:
	   case IDC_VIEWOBJ3:
	   case IDC_VIEWOBJ4:
	   case IDC_VIEWOBJ5:
	   case IDC_VIEWOBJ6:
	   case IDC_VIEWOBJ7:
	   case IDC_VIEWOBJ8:
	   case IDC_VIEWOBJ9:
	      if ( !lpImage )
		      break;
	      nObject = id - IDC_VIEWOBJFIRST;
	      nObject += nTopObject;
	      if ( nObject >= ImgCountFloaters(lpImage) )
		      break;
	      if ( !(lpObject = ImgGetObject(lpImage, nObject)) )
		      break;
	      if (codeNotify >= IDC_VIEWOBJFIRST && codeNotify <= IDC_VIEWOBJLAST)
		      {
            OBJECTLAYERING_PARMS Parms;

		      nObject2 = codeNotify - IDC_VIEWOBJFIRST;
		      nObject2 += nTopObject;
		      if ( nObject2 >= ImgCountFloaters(lpImage) )
			      break;
		      if ( !(lpToObject = ImgGetObject(lpImage, nObject2)) )
			      break;
		      if (lpToObject == lpObject)
			      break;
            Parms.Offset = nObject - nObject2;
            if (Parms.Offset < 0 && (id > (WORD)codeNotify))
               Parms.Offset--;   
            lpImage->fChanged = TRUE;
            ProcessCommand(lpImage->lpCmdList, IDS_CMD_OBJECTLAYERING, &Parms);
		      break;
		      }
         SELECTOBJRELATIVE_PARMS Parms;
         Parms.Pos = nObject;
         Parms.Shift = SHIFT;
         lpImage->fChanged = TRUE;
         ProcessCommand(lpImage->lpCmdList, IDS_CMD_SELECTOBJRELATIVE, &Parms);
         AfxGetMainWnd()->SetFocus();
	      break;

	   case IDC_VIEWOBJUP:
	      if ( !lpImage )
		      break;
	      if ( !nTopObject )
		      break;
	      nTopObject--;
	      SetupMiniViews(GetSafeHwnd(), NO);
         AfxGetMainWnd()->SetFocus();
	      break;

	   case IDC_VIEWOBJDOWN:
	      if ( !lpImage )
		      break;
	      if ( ImgCountFloaters(lpImage) <= (nTopObject + nMaxMinis) )
		      break;
	      nTopObject++;
	      SetupMiniViews(GetSafeHwnd(), NO);
         AfxGetMainWnd()->SetFocus();
	      break;
      case IDCANCEL:
         return(TRUE);

      default:
	      break;
      }
   CDialog::OnCommand(wParam, lParam);
   return(TRUE);
   }

/***********************************************************************/
BOOL SelectObjectsRelativeImage(LPIMAGE lpImage, LPSELECTOBJRELATIVE_PARMS lpParms)
/***********************************************************************/
{
RECT rUpdate;
LPOBJECT lpObject;

GetObjectMarqueeRect(lpImage, &rUpdate);
if ( lpParms->Pos < ImgCountFloaters(lpImage) )
   {
   if (lpObject = ImgGetObject(lpImage, lpParms->Pos))
      {
      RemoveObjectMarquee(lpImage);
      ImgChangeSelection(lpImage, lpObject, !lpObject->fSelected, lpParms->Shift);
      if ( Tool.hRibbon )
      ::SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );

	   lpParms->Common.UpdateType = UT_ACTIVEAREA;
	   lpParms->Common.rUpdateArea = rUpdate;
      }
	}
return(TRUE);
}


/***********************************************************************/
BOOL ObjectLayerImage(LPIMAGE lpImage, LPOBJECTLAYERING_PARMS lpParms)
/***********************************************************************/
{
RECT     rUpdate;
LPOBJECT lpObject, lpObject2;
LPOBJECT lpBase;
int      Offset = lpParms->Offset;
BOOL     fToTop = Offset > 0;

lpBase = ImgGetBase(lpImage);
lpObject = ImgGetSelObject(lpImage, lpBase);
if (lpObject)
   {
   if (Offset < 0)
      Offset = -Offset;
   while (Offset--)
      ImgAdjustLayerSelObj(lpImage, fToTop, TRUE);

   GetObjectMarqueeRect(lpImage, &rUpdate);

   ImgGetSelObjectRect(lpImage, &rUpdate, YES);
	lpParms->Common.UpdateType = UT_ACTIVEAREA;
	lpParms->Common.rUpdateArea = rUpdate;
	}
return(TRUE);
}
