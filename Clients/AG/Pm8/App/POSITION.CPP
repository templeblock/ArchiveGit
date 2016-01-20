/*
// $Header: /PM8/App/POSITION.CPP 1     3/03/99 6:10p Gbeddow $
//
// Position dialog.
//
// $Log: /PM8/App/POSITION.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 2     5/25/98 12:59p Cboggio
// Converted Inches to centimetres where required for localization
// 
//    Rev 1.1   02 Oct 1997 10:36:48   johno
// Removed stray character in //{{AFX_DATA_INIT(CPageRelative)
// that ClassWizard didn't like
// 
//    Rev 1.0   14 Aug 1997 15:24:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:00   Fred
// Initial revision.
// 
//    Rev 1.12   22 Jul 1997 15:53:54   johno
// Added string "Reset" to resources
// 
//    Rev 1.11   21 Jul 1997 16:03:14   Fred
// Fixed bug with rotated objects
// 
//    Rev 1.10   31 Mar 1997 12:59:38   Fred
// Turned optimizations around a function to get past internal compiler error
// 
//    Rev 1.9   12 Mar 1997 15:52:16   Jay
// Rotation, etc.
// 
//    Rev 1.8   11 Mar 1997 16:13:56   Jay
// Extensions to the workspace
// 
//    Rev 1.7   29 Jan 1997 11:40:58   johno
// Do Space Equally AFTER grow or shrink
// 
//    Rev 1.6   24 Jan 1997 15:06:58   johno
// Fixes per Fred
// 
//    Rev 1.5   23 Jan 1997 14:52:16   johno
// Changes, fixes per Fred
// (not finished)
// 
//    Rev 1.4   06 Jan 1997 13:57:34   johno
// Move "Preview" static text
// 
//    Rev 1.3   24 Dec 1996 10:37:18   johno
// 
//    Rev 1.2   23 Dec 1996 17:36:26   johno
// 
//    Rev 1.1   23 Dec 1996 11:41:42   johno
// 
//    Rev 1.0   19 Dec 1996 14:44:30   johno
// Initial revision.
// 
*/

// position.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "position.h"
#include "utils.h"			// For RAD2DEGC

#ifdef LOCALIZE
#include "clocale.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/******************************************************************************
   Property Page - Base Class
******************************************************************************/
CPositionDlgPageBase::CPositionDlgPageBase(int ID)
   : CPmwPropertyPage (ID)
{
   m_pObjectListCurrent = m_pObjectListSave = NULL;
}

CPositionDlgPageBase::~CPositionDlgPageBase()
{
	delete m_pObjectListSave;
}

void CPositionDlgPageBase::DoDataExchange(CDataExchange* pDX)
{
   CPmwPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPositionDlgPageBase)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPositionDlgPageBase, CPmwPropertyPage)
   //{{AFX_MSG_MAP(CPositionDlgPageBase)
   ON_WM_DRAWITEM()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL 
CPositionDlgPageBase::OnSetActive ()
{
   BOOL  b;
   
   b = CPmwPropertyPage::OnSetActive ();
   //// Get the current settings and save them for "no change" selection
   if (m_pObjectListCurrent != NULL)
   {
      if (m_pObjectListSave != NULL)
         delete m_pObjectListSave;

      m_pObjectListSave = new TPositionList ();
      m_pObjectListSave->Assign (m_pObjectListCurrent);
   }

   return b;
}

BOOL 
CPositionDlgPageBase::OnKillActive ()
{
   BOOL  b;
   
   b = CPmwPropertyPage::OnKillActive ();
   //// Throw away saved settings
   if (m_pObjectListSave != NULL)
   {
      delete m_pObjectListSave;
      m_pObjectListSave = NULL;
   }

   return b;
}

void CPositionDlgPageBase::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   // TODO: Add your message handler code here and/or call default
   
   if (nIDCtl == IDC_POSITION_PREVIEW)
   {
      if (m_pObjectListCurrent != NULL)
      {
         CRect Rect (lpDrawItemStruct->rcItem);
         CDC   *pDC = CDC::FromHandle (lpDrawItemStruct->hDC);

         m_pObjectListCurrent->Display (&Rect, pDC);
         
         HBRUSH   hBrush, hOldBrush;
         hBrush = (HBRUSH)GetStockObject (HOLLOW_BRUSH);
         hOldBrush = (HBRUSH)SelectObject (lpDrawItemStruct->hDC, hBrush);

         pDC->Rectangle (Rect);

         SelectObject (lpDrawItemStruct->hDC, hOldBrush);
      }
   }
   else
      CPmwPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

/* From the original Pascal

   Numerator := PreviewPanel.Width;
   Denominator := DialogData.PageWidth;

   if (PreviewPanel.Width*DialogData.PageHeight) > (PreviewPanel.Height*DialogData.PageWidth) then
   begin
      Numerator := PreviewPanel.Height;
      Denominator := DialogData.PageHeight;
   end;

   Width := (DialogData.PageWidth*Numerator) div Denominator;
   Height := (DialogData.PageHeight*Numerator) div Denominator;

   PreviewPanel.SetBounds(
      PreviewPanel.Left+(PreviewPanel.Width-Width) div 2,
      PreviewPanel.Top+(PreviewPanel.Height-Height) div 2,
      Width,
      Height);

   PreviewPanelLabel.Left := PreviewPanel.Left;
   PreviewPanelLabel.Top := PreviewPanel.Top-PreviewPanelLabel.Height;
*/
BOOL CPositionDlgPageBase::OnInitDialog() 
{
   //int      Numerator, Denominator, Width, Height;
#if 0
   int      n;
   double   pw, ph;
#endif   
   CRect    Rect;
   CWnd     *pWindow;
   
   CPmwPropertyPage::OnInitDialog();
   
   if (m_pObjectListCurrent != NULL)
   {
      if ((pWindow = GetDlgItem (IDC_POSITION_PREVIEW)) != NULL)
      {
         pWindow->GetWindowRect (&Rect);
         ScreenToClient (Rect);

         double fScale = (double)Rect.Width()/m_pObjectListCurrent->NormalizedPageWidth ();

         if (fScale > (double)Rect.Height()/m_pObjectListCurrent->NormalizedPageHeight ())
         {
            fScale = (double)Rect.Height()/m_pObjectListCurrent->NormalizedPageHeight ();
         }

         // Compute dimensions of new rectangle.
         int nWidth = (int)(m_pObjectListCurrent->NormalizedPageWidth ()*fScale);
         int nHeight = (int)(m_pObjectListCurrent->NormalizedPageHeight ()*fScale);

         // Set new rectangle size and center in old rectangle.
         Rect.SetRect(
            Rect.left+(Rect.Width()-nWidth)/2,
            Rect.top+(Rect.Height()-nHeight)/2,
            Rect.left+(Rect.Width()-nWidth)/2+nWidth,
            Rect.top+(Rect.Height()-nHeight)/2+nHeight);

#if 0
         pw = m_pObjectListCurrent->NormalizedPageWidth ();
         ph = m_pObjectListCurrent->NormalizedPageHeight ();

         if (pw > ph)   // ph = 0.75
         {
            n = (int)((double)Rect.Width () * ph);
            Rect.bottom = Rect.top + n;
            Rect.OffsetRect (0, (n / 2));
         }
         else  // ph = 1.25
         {
            n = (int)((double)Rect.Height () / ph);
            Rect.right = Rect.left + n;
            Rect.OffsetRect (-(n / 2), 0);
         }
#endif
         
         pWindow->MoveWindow (Rect);

         if ((pWindow = GetDlgItem (IDC_POSITION_STATIC_PREVIEW)) != NULL)
         {
            int   w, h;
            CRect Rect2;

            pWindow->GetWindowRect (&Rect2);
            w = Rect2.Width ();
            h = Rect2.Height ();
            
            Rect2.top = Rect.top - h - 2;
            Rect2.bottom = Rect2.top + h;
            
            Rect2.left = Rect.left;
            Rect2.right = Rect2.left + w;

            pWindow->MoveWindow (Rect2);
         }
      }
   }
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
/******************************************************************************
   Property Page - Position
******************************************************************************/
// CPagePosition property page

//IMPLEMENT_DYNCREATE(CPagePosition, CPmwPropertyPage)

CPagePosition::CPagePosition() 
   : PAGE_BASE(CPagePosition::IDD)
{
   //{{AFX_DATA_INIT(CPagePosition)
   m_nPositionSelectionHoriz = 0;
   m_nPositionSelectionVert = 0;
   //}}AFX_DATA_INIT
   
}

CPagePosition::~CPagePosition()
{
}

void CPagePosition::DoDataExchange(CDataExchange* pDX)
{
   CPmwPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPagePosition)
   DDX_Radio(pDX, IDC_POSITION_NCH, m_nPositionSelectionHoriz);
   DDX_Radio(pDX, IDC_POSITION_NCV, m_nPositionSelectionVert);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPagePosition, CPmwPropertyPage)
   //{{AFX_MSG_MAP(CPagePosition)
	ON_EN_CHANGE(IDC_POSITION_INCHV, OnChangePositionInchv)
	ON_EN_CHANGE(IDC_POSITION_INCHH, OnChangePositionInchh)
   ON_BN_CLICKED(IDC_POSITION_ATH, OnClickedPositionAth)
   ON_BN_CLICKED(IDC_POSITION_ATV, OnClickedPositionAtv)
	ON_EN_SETFOCUS(IDC_POSITION_INCHH, OnSetfocusPositionInchh)
	ON_EN_SETFOCUS(IDC_POSITION_INCHV, OnSetfocusPositionInchv)
   ON_WM_DRAWITEM()
   ON_BN_CLICKED(IDC_POSITION_NCH, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_LEFT, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_CENTER, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_RIGHT, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_SEH, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_NCV, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_TOP, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_MID, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_BOTTOM, OnPositionChange)
   ON_BN_CLICKED(IDC_POSITION_SEV, OnPositionChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPagePosition message handlers

int  CPagePosition::ButtonIDLeft [] = {IDC_POSITION_NCH,
                                       IDC_POSITION_LEFT,
                                       IDC_POSITION_CENTER,
                                       IDC_POSITION_RIGHT,
                                       IDC_POSITION_SEH,
                                       IDC_POSITION_ATH,
                                       0};

int  CPagePosition::ButtonIDRight [] = {IDC_POSITION_NCV,
                                       IDC_POSITION_TOP,
                                       IDC_POSITION_MID,
                                       IDC_POSITION_BOTTOM,
                                       IDC_POSITION_SEV,
                                       IDC_POSITION_ATV,
                                       0};

BOOL 
CPagePosition::OnInitDialog()
{ 
   CPositionEdit        *pWindow;

   BOOL  b = CPositionDlgPageBase::OnInitDialog();

   if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_INCHH)) != NULL)
      pWindow->LimitText (10);

   if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_INCHV)) != NULL)
      pWindow->LimitText (10);

   return b;
}

BOOL CPagePosition::OnSetActive() 
{
   BOOL     b = CPositionDlgPageBase::OnSetActive();
   
   UncheckAll ();

   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_POSITION_NCH)) != NULL)
      pBut->SetCheck (1);

   if ((pBut = (CButton*)GetDlgItem (IDC_POSITION_NCV)) != NULL)
      pBut->SetCheck (1);
   
   SetEditText ();
   
   return b;
}

void
CPagePosition::SetEditText (void)
{
   //// Fill in edit boxes (maybe)
	int                  i;
   double               d;
   CPositionEdit        *pWindow;
   TPositionListElement *pListElement;
   if (m_pObjectListCurrent != NULL)
   {
      if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_INCHH)) != NULL)
      {
         for (i = 0; i < m_pObjectListCurrent->GetSize (); i ++)
         {
            pListElement = (TPositionListElement*)m_pObjectListCurrent->GetAt (i);
            if (i == 0)
               d = pListElement->Bounds.Left;
            else
               if (d != pListElement->Bounds.Left)
                  break;
         }

         if (i >= m_pObjectListCurrent->GetSize ())
#ifdef LOCALIZE
            pWindow->SetText (CurrentLocale.ConvertFromInches(m_pObjectListCurrent->XRelativeToInches (d)));
#else
            pWindow->SetText (m_pObjectListCurrent->XRelativeToInches (d));
#endif
         else  
            pWindow->SetWindowText ("");
      }

      if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_INCHV)) != NULL)
      {
         for (i = 0; i < m_pObjectListCurrent->GetSize (); i ++)
         {
            pListElement = (TPositionListElement*)m_pObjectListCurrent->GetAt (i);
            if (i == 0)
               d = pListElement->Bounds.Top;
            else
               if (d != pListElement->Bounds.Top)
                  break;
         }

         if (i >= m_pObjectListCurrent->GetSize ())
#ifdef LOCALIZE
            pWindow->SetText (CurrentLocale.ConvertFromInches(m_pObjectListCurrent->YRelativeToInches (d)));
#else
            pWindow->SetText (m_pObjectListCurrent->YRelativeToInches (d));
#endif
         else  
            pWindow->SetWindowText ("");
      }
   }
}

void CPagePosition::OnPositionChange() 
{
   ApplyPosition ();
}

void CPagePosition::OnClickedPositionAth() 
{
   CWnd  *pWindow;
   
   if ((pWindow = GetDlgItem (IDC_POSITION_INCHH)) != NULL)
      pWindow->SetFocus ();
   
   ApplyPosition ();
}

void CPagePosition::OnClickedPositionAtv() 
{
   CWnd  *pWindow;
   
   if ((pWindow = GetDlgItem (IDC_POSITION_INCHV)) != NULL)
      pWindow->SetFocus ();

   ApplyPosition ();
}

void CPagePosition::OnChangePositionInchh() 
{
   ApplyPosition ();
}

void CPagePosition::OnChangePositionInchv() 
{
   ApplyPosition ();
}

void CPagePosition::OnSetfocusPositionInchh() 
{
   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_POSITION_ATH)) != NULL)
   {
      if (pBut->GetCheck () != 1)
      {
         UncheckButtons (ButtonIDLeft);
         pBut->SetCheck (1);
      }
   }
}

void CPagePosition::OnSetfocusPositionInchv() 
{
   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_POSITION_ATV)) != NULL)
   {
      if (pBut->GetCheck () != 1)
      {
         UncheckButtons (ButtonIDRight);
         pBut->SetCheck (1);
      }
   }
}

BOOL
CPagePosition::DoInchH () 
{
   BOOL     ret = FALSE;
   double   d;

   CPositionEdit  *pWindow;
   
   pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_INCHH);
      
   if (pWindow != NULL)
   {
      if (pWindow->GetTypeIn (&d) == TRUE)
      {
#ifdef LOCALIZE
		 d = CurrentLocale.ConvertToInches(d);
#endif
         //XDotsToRelative := ((X-DialogData.PageLeft)/DialogData.PageWidth)*NormalizedPageWidth;
         d = ((d - m_pObjectListCurrent->GetPageRectLeft ()) / m_pObjectListCurrent->GetPageRectWidth () * m_pObjectListCurrent->NormalizedPageWidth ());
         
         m_pObjectListCurrent->AlignToLeft (d);

         ret = TRUE;
      }
   }

   return ret;
}

BOOL
CPagePosition::DoInchV ()
{
   BOOL     ret = FALSE;
   double   d;

   CPositionEdit  *pWindow;
   
   pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_INCHV);
      
   if (pWindow != NULL)
   {
      if (pWindow->GetTypeIn (&d) == TRUE)
      {
#ifdef LOCALIZE
		 d = CurrentLocale.ConvertToInches(d);
#endif
		  //YDotsToRelative := ((Y-DialogData.PageTop)/DialogData.PageHeight)*NormalizedPageHeight;
         d = ((d - m_pObjectListCurrent->GetPageRectTop ()) / m_pObjectListCurrent->GetPageRectWidth () * m_pObjectListCurrent->NormalizedPageWidth ());
         
         m_pObjectListCurrent->AlignToTop (d);

         ret = TRUE;
      }
   }

   return ret;
}

void
CPagePosition::ApplyPosition ()
{
   CWnd  *pWindow;
   
   if ((m_pObjectListCurrent != NULL) && (m_pObjectListSave != NULL))
   {
      m_pObjectListCurrent->Assign (m_pObjectListSave);
////
      if (IsDlgButtonChecked (IDC_POSITION_ATH) == TRUE)
         DoInchH ();
      else
////      
      if (IsDlgButtonChecked (IDC_POSITION_LEFT) == TRUE)
      {
         m_pObjectListCurrent->AlignToLeftOfPage ();
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_CENTER) == TRUE)
      {
         m_pObjectListCurrent->AlignToCenterOfPage ();
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_RIGHT) == TRUE)
      {
         m_pObjectListCurrent->AlignToRightOfPage ();
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SEH) == TRUE)
      {
         m_pObjectListCurrent->SpaceWidthsEvenlyInPage ();
      }
////
      if (IsDlgButtonChecked (IDC_POSITION_ATV) == TRUE)
         DoInchV ();
      else
////
      if (IsDlgButtonChecked (IDC_POSITION_TOP) == TRUE)
      {
         m_pObjectListCurrent->AlignToTopOfPage ();
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_MID) == TRUE)
      {
         m_pObjectListCurrent->AlignToMiddleOfPage ();
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_BOTTOM) == TRUE)
      {
         m_pObjectListCurrent->AlignToBottomOfPage ();
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SEV) == TRUE)
      {
         m_pObjectListCurrent->SpaceHeightsEvenlyInPage ();
      }
////
////
      if ((pWindow = GetDlgItem (IDC_POSITION_PREVIEW)) != NULL)
      {
         pWindow->Invalidate ();
      }
   }
}

/******************************************************************************
   Property Page - Size
******************************************************************************/
// CPageSize property page

//IMPLEMENT_DYNCREATE(CPageSize, CPmwPropertyPage)

CPageSize::CPageSize() 
   : PAGE_BASE(CPageSize::IDD)
{
   //{{AFX_DATA_INIT(CPageSize)
   m_nSizeSelectionWidth = 0;
   m_nSizeSelectionHeight = 0;
   //}}AFX_DATA_INIT

   m_bLocalUpdate = FALSE;   
}

CPageSize::~CPageSize()
{
}

void CPageSize::DoDataExchange(CDataExchange* pDX)
{
   CPmwPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPageSize)
   DDX_Radio(pDX, IDC_SIZE_NCW, m_nSizeSelectionWidth);
   DDX_Radio(pDX, IDC_SIZE_NCH, m_nSizeSelectionHeight);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageSize, CPmwPropertyPage)
   //{{AFX_MSG_MAP(CPageSize)
   ON_BN_CLICKED(IDC_SIZE_NCW, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_1_10W, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_1_4W, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_1_3W, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_1_2W, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_3_4W, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_FULLW, OnSizeRadioChange)

   ON_BN_CLICKED(IDC_SIZE_NCH, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_1_10H, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_1_4H, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_1_3H, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_1_2H, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_3_4H, OnSizeRadioChange)
   ON_BN_CLICKED(IDC_POSITION_SIZE_FULLH, OnSizeRadioChange)

   ON_WM_DRAWITEM()

	ON_EN_SETFOCUS(IDC_POSITION_SIZE_W_EDIT, OnSetfocusPositionSizeWEdit) 
   ON_EN_CHANGE(IDC_POSITION_SIZE_W_EDIT, OnChangePositionSizeWEdit)
   ON_BN_CLICKED(IDC_POSITION_SIZE_WIDTH, OnClickedWidth)
	
   ON_EN_SETFOCUS(IDC_POSITION_SIZE_W_SCALE_EDIT, OnSetfocusPositionSizeWScaleEdit)
   ON_EN_CHANGE(IDC_POSITION_SIZE_W_SCALE_EDIT, OnChangePositionSizeWScaleEdit)
   ON_BN_CLICKED(IDC_POSITION_SIZE_SCALEW, OnClickedScaleW)

	ON_EN_SETFOCUS(IDC_POSITION_SIZE_H_EDIT, OnSetfocusPositionSizeHEdit) 
	ON_EN_CHANGE(IDC_POSITION_SIZE_H_EDIT, OnChangePositionSizeHEdit)
   ON_BN_CLICKED(IDC_POSITION_SIZE_HEIGHT, OnClickedHeight)
	
	ON_EN_SETFOCUS(IDC_POSITION_SIZE_H_SCALE_EDIT, OnSetfocusPositionSizeHScaleEdit)
   ON_EN_CHANGE(IDC_POSITION_SIZE_H_SCALE_EDIT, OnChangePositionSizeHScaleEdit)
   ON_BN_CLICKED(IDC_POSITION_SIZE_SCALEH, OnClickedScaleH)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageSize message handlers

int  CPageSize::ButtonIDLeft [] = {IDC_SIZE_NCW,
                                 IDC_POSITION_SIZE_1_10W,
                                 IDC_POSITION_SIZE_1_4W,
                                 IDC_POSITION_SIZE_1_3W,
                                 IDC_POSITION_SIZE_1_2W,
                                 IDC_POSITION_SIZE_3_4W,
                                 IDC_POSITION_SIZE_FULLW,
                                 IDC_POSITION_SIZE_WIDTH,
                                 IDC_POSITION_SIZE_SCALEW,
                                 0};

int  CPageSize::ButtonIDRight [] = {IDC_SIZE_NCH,
                                 IDC_POSITION_SIZE_1_10H,
                                 IDC_POSITION_SIZE_1_4H,
                                 IDC_POSITION_SIZE_1_3H,
                                 IDC_POSITION_SIZE_1_2H,
                                 IDC_POSITION_SIZE_3_4H,
                                 IDC_POSITION_SIZE_FULLH,
                                 IDC_POSITION_SIZE_HEIGHT,
                                 IDC_POSITION_SIZE_SCALEH,
                                 0};

BOOL 
CPageSize::OnInitDialog()
{ 
   CPositionEdit  *pWindow;

   BOOL  b = CPositionDlgPageBase::OnInitDialog();

   if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_W_EDIT)) != NULL)
      pWindow->LimitText (10);

   if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_W_SCALE_EDIT)) != NULL)
      pWindow->LimitText (10);

   if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_H_EDIT)) != NULL)
      pWindow->LimitText (10);

   if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_H_SCALE_EDIT)) != NULL)
      pWindow->LimitText (10);

   return b;
}

BOOL CPageSize::OnSetActive() 
{
   BOOL b = CPositionDlgPageBase::OnSetActive();

   UncheckAll ();

   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_SIZE_NCW)) != NULL)
      pBut->SetCheck (1);

   if ((pBut = (CButton*)GetDlgItem (IDC_SIZE_NCH)) != NULL)
      pBut->SetCheck (1);

   SetEditTextAll ();
   
   return b;
}

void
CPageSize::SetEditTextAll (void)
{
   //// Fill in edit boxes (maybe)
	int                  NumSelected;
   double               d, Original;
   CPositionEdit        *pWindow;
   TPositionListElement *pListElement;
   
   BOOL u = GetLocalUpdate ();
   if (u == FALSE)
   {
      SetLocalUpdate (TRUE);

   if (m_pObjectListCurrent != NULL)
   {  // Multiple objects selected, blank non active type ins
      if ((NumSelected = m_pObjectListCurrent->GetSize ()) > 1)   
      {
         if (IsDlgButtonChecked (IDC_POSITION_SIZE_WIDTH) == FALSE)
            SetControlText (IDC_POSITION_SIZE_W_EDIT, "");

         if (IsDlgButtonChecked (IDC_POSITION_SIZE_SCALEW) == FALSE)
            SetControlText (IDC_POSITION_SIZE_W_SCALE_EDIT, "");
      
         if (IsDlgButtonChecked (IDC_POSITION_SIZE_HEIGHT) == FALSE)
            SetControlText (IDC_POSITION_SIZE_H_EDIT, "");
      
         if (IsDlgButtonChecked (IDC_POSITION_SIZE_SCALEH) == FALSE)
            SetControlText (IDC_POSITION_SIZE_H_SCALE_EDIT, "");
      }
      else
      if (NumSelected > 0)
      {
         if (IsDlgButtonChecked (IDC_POSITION_SIZE_WIDTH) == FALSE)
         {
            if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_W_EDIT)) != NULL)
            {
               pListElement = (TPositionListElement*)m_pObjectListCurrent->GetAt (0);
               d = pListElement->Bounds.Width;
#ifdef LOCALIZE
			   pWindow->SetText (CurrentLocale.ConvertFromInches(m_pObjectListCurrent->XRelativeToInches (d)));
#else
			   pWindow->SetText (m_pObjectListCurrent->XRelativeToInches (d));
#endif
            }
         }

         if (IsDlgButtonChecked (IDC_POSITION_SIZE_HEIGHT) == FALSE)
         {
            if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_H_EDIT)) != NULL)
            {
               pListElement = (TPositionListElement*)m_pObjectListCurrent->GetAt (0);
               d = pListElement->Bounds.Height;
#ifdef LOCALIZE
               pWindow->SetText (CurrentLocale.ConvertFromInches(m_pObjectListCurrent->YRelativeToInches (d)));
#else
               pWindow->SetText (m_pObjectListCurrent->YRelativeToInches (d));
#endif

            }
         }

         if (m_pObjectListSave != NULL)
         {
            if (IsDlgButtonChecked (IDC_POSITION_SIZE_SCALEW) == FALSE)
            {
               if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_W_SCALE_EDIT)) != NULL)
               {
                  pListElement = (TPositionListElement*)m_pObjectListSave->GetAt (0);
                  Original = pListElement->Bounds.Width;
                  if (Original == 0.0)
                     Original = 1.0;
                  
                  pListElement = (TPositionListElement*)m_pObjectListCurrent->GetAt (0);
                  d = (pListElement->Bounds.Width / Original);
                  
                  pWindow->SetTextPerCent (d);
               }
            }
            
            if (IsDlgButtonChecked (IDC_POSITION_SIZE_SCALEH) == FALSE)
            {
               if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_H_SCALE_EDIT)) != NULL)
               {
                  pListElement = (TPositionListElement*)m_pObjectListSave->GetAt (0);
                  Original = pListElement->Bounds.Height;
                  if (Original == 0.0)
                     Original = 1.0;
                  
                  pListElement = (TPositionListElement*)m_pObjectListCurrent->GetAt (0);
                  d = (pListElement->Bounds.Height / Original);
                  
                  pWindow->SetTextPerCent (d);
               }
            }
         }
      }
   }

   SetLocalUpdate (u);
   }
}

////////
void
CPageSize::OnClickedWidth ()
{
   CWnd  *pWindow;
   
   if ((pWindow = GetDlgItem (IDC_POSITION_SIZE_W_EDIT)) != NULL)
      pWindow->SetFocus ();

   ApplySize ();
}

void
CPageSize::OnClickedHeight ()
{
   CWnd  *pWindow;
   
   if ((pWindow = GetDlgItem (IDC_POSITION_SIZE_H_EDIT)) != NULL)
      pWindow->SetFocus ();

   ApplySize ();
}

void
CPageSize::OnClickedScaleW ()
{
   CWnd  *pWindow;
   
   if ((pWindow = GetDlgItem (IDC_POSITION_SIZE_W_SCALE_EDIT)) != NULL)
      pWindow->SetFocus ();

   ApplySize ();
}

void
CPageSize::OnClickedScaleH ()
{
   CWnd  *pWindow;
   
   if ((pWindow = GetDlgItem (IDC_POSITION_SIZE_H_SCALE_EDIT)) != NULL)
      pWindow->SetFocus ();

   ApplySize ();
}
////////
void CPageSize::OnSetfocusPositionSizeWEdit() 
{
   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_POSITION_SIZE_WIDTH)) != NULL)
   {
      if (pBut->GetCheck () != 1)
      {
         UncheckButtons (ButtonIDLeft);
         pBut->SetCheck (1);
      }

      SetEditTextAll ();
   }
}

void CPageSize::OnSetfocusPositionSizeWScaleEdit() 
{
   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_POSITION_SIZE_SCALEW)) != NULL)
   {
      if (pBut->GetCheck () != 1)
      {
         UncheckButtons (ButtonIDLeft);
         pBut->SetCheck (1);
      }

      SetEditTextAll ();
   }
}

void CPageSize::OnSetfocusPositionSizeHEdit() 
{
   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_POSITION_SIZE_HEIGHT)) != NULL)
   {
      if (pBut->GetCheck () != 1)
      {
         UncheckButtons (ButtonIDRight);
         pBut->SetCheck (1);
      }

      SetEditTextAll ();
   }
}

void CPageSize::OnSetfocusPositionSizeHScaleEdit() 
{
   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_POSITION_SIZE_SCALEH)) != NULL)
   {
      if (pBut->GetCheck () != 1)
      {
         UncheckButtons (ButtonIDRight);
         pBut->SetCheck (1);
      }

      SetEditTextAll ();
   }
}
////////
void CPageSize::OnSizeRadioChange() 
{
   ApplySize ();
   SetEditTextAll ();
}

void CPageSize::OnChangePositionSizeWEdit() 
{
   JTRACE ("Size width changed\n");
   ApplySize ();
   //SetEditTextScale ();
   SetEditTextAll ();
}

void CPageSize::OnChangePositionSizeWScaleEdit() 
{
   JTRACE ("Scale width changed\n");
   ApplySize ();
   //SetEditTextSize ();
   SetEditTextAll ();
}

void CPageSize::OnChangePositionSizeHEdit() 
{
   JTRACE ("Size Height changed\n");
   ApplySize ();
   //SetEditTextScale ();
   SetEditTextAll ();
}

void CPageSize::OnChangePositionSizeHScaleEdit() 
{
   JTRACE ("Scale Height changed\n");
   ApplySize ();
   //SetEditTextSize ();
   SetEditTextAll ();   
}

void
CPageSize::ApplySize ()
{
   double         d;
   CPositionEdit  *pWindow;
   
   if ((m_pObjectListCurrent != NULL) && ((m_pObjectListSave != NULL)))
   {
      m_pObjectListCurrent->Assign (m_pObjectListSave);

      if (IsDlgButtonChecked (IDC_POSITION_SIZE_1_10W) == TRUE)
      {
         m_pObjectListCurrent->SetWidth (m_pObjectListCurrent->NormalizedPageWidth () / 10.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_1_4W) == TRUE)
      {
         m_pObjectListCurrent->SetWidth (m_pObjectListCurrent->NormalizedPageWidth () / 4.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_1_3W) == TRUE)
      {
         m_pObjectListCurrent->SetWidth (m_pObjectListCurrent->NormalizedPageWidth () / 3.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_1_2W) == TRUE)
      {
         m_pObjectListCurrent->SetWidth (m_pObjectListCurrent->NormalizedPageWidth () / 2.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_3_4W) == TRUE)
      {
			m_pObjectListCurrent->SetWidth ((3.0 * m_pObjectListCurrent->NormalizedPageWidth ()) / 4.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_FULLW) == TRUE)
      {
         m_pObjectListCurrent->SetWidth (m_pObjectListCurrent->NormalizedPageWidth ());
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_WIDTH) == TRUE)
      {
         if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_W_EDIT)) != NULL)
            if (pWindow->GetTypeIn (&d) == TRUE)
            {
#ifdef LOCALIZE
			   d = CurrentLocale.ConvertToInches(d);
#endif
			   d = ((d - m_pObjectListCurrent->GetPageRectLeft ()) / m_pObjectListCurrent->GetPageRectWidth () * m_pObjectListCurrent->NormalizedPageWidth ());
               m_pObjectListCurrent->SetWidth (d);
            }
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_SCALEW) == TRUE)
      {
         if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_W_SCALE_EDIT)) != NULL)
            if (pWindow->GetTypeInPerCent (&d) == TRUE)
               m_pObjectListCurrent->ScaleWidth (d, m_pObjectListSave);
      }
////////
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_1_10H) == TRUE)
      {
         m_pObjectListCurrent->SetHeight (m_pObjectListCurrent->NormalizedPageHeight () / 10.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_1_4H) == TRUE)
      {
         m_pObjectListCurrent->SetHeight (m_pObjectListCurrent->NormalizedPageHeight () / 4.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_1_3H) == TRUE)
      {
         m_pObjectListCurrent->SetHeight (m_pObjectListCurrent->NormalizedPageHeight () / 3.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_1_2H) == TRUE)
      {
         m_pObjectListCurrent->SetHeight (m_pObjectListCurrent->NormalizedPageHeight () / 2.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_3_4H) == TRUE)
      {
			m_pObjectListCurrent->SetHeight ((3.0 * m_pObjectListCurrent->NormalizedPageHeight ()) / 4.0);
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_FULLH) == TRUE)
      {
			m_pObjectListCurrent->SetHeight (m_pObjectListCurrent->NormalizedPageHeight ());
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_HEIGHT) == TRUE)
      {
         if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_H_EDIT)) != NULL)
            if (pWindow->GetTypeIn (&d) == TRUE)
            {
#ifdef LOCALIZE
			   d = CurrentLocale.ConvertToInches(d);
#endif
			   d = ((d - m_pObjectListCurrent->GetPageRectTop ()) / m_pObjectListCurrent->GetPageRectWidth () * m_pObjectListCurrent->NormalizedPageWidth ());
               m_pObjectListCurrent->SetHeight (d);
            }
      }
      else
      if (IsDlgButtonChecked (IDC_POSITION_SIZE_SCALEH) == TRUE)
      {
         if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_SIZE_H_SCALE_EDIT)) != NULL)
            if (pWindow->GetTypeInPerCent (&d) == TRUE)
               m_pObjectListCurrent->ScaleHeight (d, m_pObjectListSave);
      }
   }

   if ((pWindow = (CPositionEdit*)GetDlgItem (IDC_POSITION_PREVIEW)) != NULL)
   {
      pWindow->Invalidate ();
   }
}

/******************************************************************************
   Property Page - Relative
******************************************************************************/
// CPageRelative property page

//IMPLEMENT_DYNCREATE(CPageRelative, CPmwPropertyPage)

CPageRelative::CPageRelative() 
   : PAGE_BASE(CPageRelative::IDD)
{
   //{{AFX_DATA_INIT(CPageRelative)
   m_nRelativeSelectionHoriz = 0;
   m_nRelativeSelectionVert = 0;
   m_nRelativeSelectionHeight = 0;
   m_nRelativeSelectionWidth = 0;
   //}}AFX_DATA_INIT
   
}

CPageRelative::~CPageRelative()
{
}

void CPageRelative::DoDataExchange(CDataExchange* pDX)
{
   CPmwPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPageRelative)
   DDX_Radio(pDX, IDC_RELATIVE_NCE, m_nRelativeSelectionHoriz);
   DDX_Radio(pDX, IDC_RELATIVE_NCV, m_nRelativeSelectionVert);
   DDX_Radio(pDX, IDC_RELATIVE_NCH, m_nRelativeSelectionHeight);
   DDX_Radio(pDX, IDC_RELATIVE_NCW, m_nRelativeSelectionWidth);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageRelative, CPmwPropertyPage)
   //{{AFX_MSG_MAP(CPageRelative)
   ON_BN_CLICKED(IDC_RELATIVE_NCE, OnRelativeChange)
   ON_WM_DRAWITEM()
   ON_BN_CLICKED(IDC_RELATIVE_NCH, OnRelativeChange)
   ON_BN_CLICKED(IDC_RELATIVE_NCV, OnRelativeChange)
   ON_BN_CLICKED(IDC_RELATIVE_NCW, OnRelativeChange)
   ON_BN_CLICKED(IDC_RELATIVE_RIGHT, OnRelativeChange)
   ON_BN_CLICKED(IDC_RELATIVE_SEH, OnRelativeChange)
   ON_BN_CLICKED(IDC_RELATIVE_SEV, OnRelativeChange)
   ON_BN_CLICKED(IDC_RELATIVE_SMALLE, OnRelativeChange)
   ON_BN_CLICKED(IDC_RELATIVE_SMALLW, OnRelativeChange)
   ON_BN_CLICKED(IDC_RELATIVE_TOP, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_LEFT, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_CENTERH, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_CENTERV, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_GROWE, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_BOTTOM, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_GROWW, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_HEIGHT, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_HORIZONTAL, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_VERTICAL, OnRelativeChange)
	ON_BN_CLICKED(IDC_RELATIVE_WIDTH, OnRelativeChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageRelative message handlers

int  CPageRelative::ButtonIDs [] = {IDC_RELATIVE_NCH,
                                    IDC_RELATIVE_LEFT,
                                    IDC_RELATIVE_CENTERH,
                                    IDC_RELATIVE_RIGHT,
                                    IDC_RELATIVE_SEH,
                                    IDC_RELATIVE_NCW,
                                    IDC_RELATIVE_SMALLW,
                                    IDC_RELATIVE_GROWW,
                                    IDC_RELATIVE_NCV,
                                    IDC_RELATIVE_TOP,
                                    IDC_RELATIVE_CENTERV,
                                    IDC_RELATIVE_BOTTOM,
                                    IDC_RELATIVE_SEV,
                                    IDC_RELATIVE_NCE,
                                    IDC_RELATIVE_SMALLE,
                                    IDC_RELATIVE_GROWE,
                                    0};

BOOL CPageRelative::OnSetActive() 
{
   BOOL b = CPositionDlgPageBase::OnSetActive();

   UncheckButtons (ButtonIDs);

   CButton  *pBut;

   if ((pBut = (CButton*)GetDlgItem (IDC_RELATIVE_NCH)) != NULL)
      pBut->SetCheck (1);

   if ((pBut = (CButton*)GetDlgItem (IDC_RELATIVE_NCW)) != NULL)
      pBut->SetCheck (1);

   if ((pBut = (CButton*)GetDlgItem (IDC_RELATIVE_NCV)) != NULL)
      pBut->SetCheck (1);

   if ((pBut = (CButton*)GetDlgItem (IDC_RELATIVE_NCE)) != NULL)
      pBut->SetCheck (1);

   return b;
}

void CPageRelative::OnRelativeChange() 
{
   ApplyAlignment ();
}

void
CPageRelative::ApplyAlignment (void)
{
   CWnd  *pWindow;

   if ((m_pObjectListCurrent != NULL) && (m_pObjectListSave != NULL))
   {
      m_pObjectListCurrent->Assign (m_pObjectListSave);
////      
      if (IsDlgButtonChecked (IDC_RELATIVE_SMALLW) == TRUE)
      {
         m_pObjectListCurrent->SetWidthToSmallest ();
      }
      else
      if (IsDlgButtonChecked (IDC_RELATIVE_GROWW) == TRUE)
      {
         m_pObjectListCurrent->SetWidthToLargest ();
      }
////
      if (IsDlgButtonChecked (IDC_RELATIVE_SMALLE) == TRUE)
      {
         m_pObjectListCurrent->SetHeightToSmallest ();
      }
      else
      if (IsDlgButtonChecked (IDC_RELATIVE_GROWE) == TRUE)
      {
         m_pObjectListCurrent->SetHeightToLargest ();
      }
////      
      if (IsDlgButtonChecked (IDC_RELATIVE_TOP) == TRUE)
      {
         m_pObjectListCurrent->AlignToTopOfBounds ();
      }
      else
      if (IsDlgButtonChecked (IDC_RELATIVE_CENTERV) == TRUE)
      {
         m_pObjectListCurrent->AlignToMiddleOfBounds ();
      }
      else
      if (IsDlgButtonChecked (IDC_RELATIVE_BOTTOM) == TRUE)
      {
         m_pObjectListCurrent->AlignToBottomOfBounds ();
      }
      else
      if (IsDlgButtonChecked (IDC_RELATIVE_SEV) == TRUE)
      {
         m_pObjectListCurrent->SpaceHeightsEvenlyInBounds ();
      }
////
      if (IsDlgButtonChecked (IDC_RELATIVE_LEFT) == TRUE)
      {
         m_pObjectListCurrent->AlignToLeftOfBounds ();
      }
      else
      if (IsDlgButtonChecked (IDC_RELATIVE_CENTERH) == TRUE)
      {
         m_pObjectListCurrent->AlignToCenterOfBounds ();
      }
      else
      if (IsDlgButtonChecked (IDC_RELATIVE_RIGHT) == TRUE)
      {
         m_pObjectListCurrent->AlignToRightOfBounds ();
      }
      else
      if (IsDlgButtonChecked (IDC_RELATIVE_SEH) == TRUE)
      {
         m_pObjectListCurrent->SpaceWidthsEvenlyInBounds ();
      }
   }

   if ((pWindow = GetDlgItem (IDC_POSITION_PREVIEW)) != NULL)
   {
      pWindow->Invalidate ();
   }
}

/******************************************************************************
   Property Page - Sheet
******************************************************************************/
// CPositionObjectDialog dialog

IMPLEMENT_DYNAMIC(CPositionObjectDialog, CPmwPropertySheet)

CPositionObjectDialog::CPositionObjectDialog (UINT nIDCaption, CWnd* pParent, UINT iSelectPage)
   : CPmwPropertySheet (nIDCaption, pParent, iSelectPage)
{
   //{{AFX_DATA_INIT(CPositionObjectDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   m_pList = m_pListOriginal = NULL;
}

void CPositionObjectDialog::DoDataExchange(CDataExchange* pDX)
{
   CPmwPropertySheet::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPositionObjectDialog)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}
/* From the original Pascal

   Bounds.Left := ((Element^.Rect.Left-PageLeft)/PageWidth)*NormalizedPageWidth;
   Bounds.Top := ((Element^.Rect.Top-PageTop)/PageHeight)*NormalizedPageHeight;
   Bounds.Width := ((Element^.Rect.Right-Element^.Rect.Left)/PageWidth)*NormalizedPageWidth;
   Bounds.Height := ((Element^.Rect.Bottom-Element^.Rect.Top)/PageHeight)*NormalizedPageHeight;
*/
void
CPositionObjectDialog::ToPositionList (CPositionData* pData)
{
   int                  i;
   double               d, left, right, top, bottom;
   double               pageleft, pagetop, pagewidth, pageheight;
   TPositionListElement *pListElement;
   TRealBounds          Bounds;
   CObjectElement       *pElement;

   NormalizedPageWidth  = 1.0;

   NormalizedPageHeight = pData->PageBound.y1 - pData->PageBound.y0;
   NormalizedPageHeight = NormalizedPageHeight / (pData->PageBound.x1 - pData->PageBound.x0);

   TRY
   {
      if (m_pList != NULL)
         delete m_pList;

      if (m_pListOriginal != NULL)
         delete m_pListOriginal;
      
      pageleft   = (double)pData->PageBound.x0;
      pagetop    = (double)pData->PageBound.y0;
      pagewidth  = (double)(pData->PageBound.x1 - pData->PageBound.x0);
      pageheight = (double)(pData->PageBound.y1 - pData->PageBound.y0);

      
      Bounds.Left   = pageleft;
      Bounds.Top    = pagetop;
      Bounds.Width  = pagewidth;
      Bounds.Height = pageheight;

      m_pList = new TPositionList (NormalizedPageWidth, NormalizedPageHeight, Bounds);
   
      for (i = 0; i < pData->lObjectCount; i ++)
      {
         pElement = &pData->pObjects[i];

         d = (double)(pElement->lAngle) / 65536.0 / RAD2DEGC;

         left   = (double)pElement->ObjectBound.x0;
         right  = (double)pElement->ObjectBound.x1;
         top    = (double)pElement->ObjectBound.y0;
         bottom = (double)pElement->ObjectBound.y1;

         Bounds.Left   = ((left - pageleft) / pagewidth)  * NormalizedPageWidth;
         Bounds.Top    = ((top - pagetop)   / pageheight) * NormalizedPageHeight;
         Bounds.Width  = ((right - left)    / pagewidth)  * NormalizedPageWidth;
         Bounds.Height = ((bottom - top)    / pageheight) * NormalizedPageHeight;

         pListElement = new TPositionListElement (Bounds, d, pData->pObjects[i].lFlags);

         m_pList->Add (pListElement);
      }
      
      m_pListOriginal = new TPositionList ();
      m_pListOriginal->Assign (m_pList);
   }
   END_TRY
}

// FF - 3/31/97 - Had to turn off optimizations to get rid of internal compiler error!                       
#pragma optimize("", off)
void CPositionObjectDialog::FromPositionList (CPositionData* pData)
{
   if (m_pList != NULL)
   {
      double pageleft   = (double)pData->PageBound.x0;
      double pagetop    = (double)pData->PageBound.y0;
      double pagewidth  = (double)(pData->PageBound.x1 - pData->PageBound.x0);
      double pageheight = (double)(pData->PageBound.y1 - pData->PageBound.y0);

      for (int i = 0; i < pData->lObjectCount; i ++)
      {
         TPositionListElement* pElementList = (TPositionListElement*)(m_pList->GetAt(i));

         CObjectElement* pDataElement = &(pData->pObjects[i]);
         
         double dAngle = pElementList->GetAngle();
         dAngle *= 65536.0;
         dAngle *= RAD2DEGC;
         long lAngle = RoundD2I(dAngle);
         pDataElement->lAngle = lAngle;
         
//			pDataElement->ObjectBound.x0 = RoundD2I(((pElementList->GetLeft() * pagewidth)  / m_pList->NormalizedPageWidth()) + pageleft);
//			pDataElement->ObjectBound.x1 = RoundD2I((((pElementList->GetLeft() + pElementList->Bounds.Width)  * pagewidth)  / m_pList->NormalizedPageWidth()) + pageleft);
//			pDataElement->ObjectBound.y0 = RoundD2I(((pElementList->GetTop()  * pageheight) / m_pList->NormalizedPageHeight()) + pagetop);
//			pDataElement->ObjectBound.y1 = RoundD2I((((pElementList->GetTop()  + pElementList->Bounds.Height) * pageheight) / m_pList->NormalizedPageHeight()) + pagetop);
			pDataElement->ObjectBound.x0 = RoundD2I(((pElementList->Bounds.Left * pagewidth)  / m_pList->NormalizedPageWidth()) + pageleft);
			pDataElement->ObjectBound.x1 = RoundD2I((((pElementList->Bounds.Left + pElementList->Bounds.Width)  * pagewidth)  / m_pList->NormalizedPageWidth()) + pageleft);
			pDataElement->ObjectBound.y0 = RoundD2I(((pElementList->Bounds.Top  * pageheight) / m_pList->NormalizedPageHeight()) + pagetop);
			pDataElement->ObjectBound.y1 = RoundD2I((((pElementList->Bounds.Top  + pElementList->Bounds.Height) * pageheight) / m_pList->NormalizedPageHeight()) + pagetop);
      }
   }
}
#pragma optimize("", on)


#ifdef WIN32
BOOL CPositionObjectDialog::OnInitDialog() 
{
   // TODO: Add your specialized code here and/or call the base class
   BOOL           ret;
   ret = CPmwPropertySheet::OnInitDialog();
#else
int CPositionObjectDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)    
{
   if (CPmwPropertySheet::OnCreate(lpCreateStruct) == -1)
   {
      return -1;
   }
   
#endif
   //// Borrow the Apply button, use for Reset
   CWnd  *pWnd;
   if ((pWnd = GetDlgItem (ID_APPLY_NOW)) != NULL)
   {
      CString  Temp;
      Temp.LoadString (IDS_RESET);
      pWnd->SetWindowText (Temp);
   }
   //// This will enable the Reset (Apply) button
   CPropertyPage  *Page;
   if ((Page = GetPage (0)) != NULL)
      Page->SetModified (TRUE);

#ifdef WIN32
   return ret;
#else
   return 0;
#endif      
}

int
CPositionObjectDialog::DoModal (CPositionData* pData)
{
   int   ret;
   
   ToPositionList (pData);
   
   AddPage (&m_PagePosition);
   AddPage (&m_PageSize);
   if (pData->lObjectCount > 1)
      AddPage (&m_PageRelative);

   m_PagePosition.SetObjectList (m_pList);
   m_PageSize.SetObjectList (m_pList);
   m_PageRelative.SetObjectList (m_pList);

   ret = CPmwPropertySheet::DoModal ();
   
   while (GetPageCount () > 0)
      RemovePage (0);

   if (ret == IDOK)
   {
      FromPositionList (pData);
   }

   m_PagePosition.SetObjectList (NULL);
   m_PageSize.SetObjectList (NULL);
   m_PageRelative.SetObjectList (NULL);

   if (m_pList != NULL)
      delete m_pList;
   
   if (m_pListOriginal != NULL)
      delete m_pListOriginal;

   return ret;
}

BEGIN_MESSAGE_MAP(CPositionObjectDialog, CPmwPropertySheet)
   //{{AFX_MSG_MAP(CPositionObjectDialog)
	ON_COMMAND(ID_APPLY_NOW, OnReset)
	//}}AFX_MSG_MAP
#ifndef WIN32
   ON_WM_CREATE()
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPositionObjectDialog message handlers

void CPositionObjectDialog::OnReset() 
{
   if ((m_pList != NULL) && (m_pListOriginal != NULL))
   {
      m_pList->Assign (m_pListOriginal);

      CPropertyPage *Page;
      if ((Page = GetActivePage ()) != NULL)
      {
         CWnd  *pPreview;

         Page->OnSetActive ();
         
         if ((pPreview = Page->GetDlgItem (IDC_POSITION_PREVIEW)) != NULL)
            pPreview->Invalidate ();
      }
   }
}

