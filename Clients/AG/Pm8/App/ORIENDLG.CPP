//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ORIENDLG.CPP 1     3/03/99 6:08p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ORIENDLG.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 13    2/20/99 9:20p Psasse
// Support for Vertical Banners
// 
// 12    2/05/99 2:49p Johno
// Projects of type PROJECT_TYPE_BlankPage are changed to
// PROJECT_TYPE_Poster in COrientationDialog::get_exit_info()
// 
// 11    2/04/99 3:05p Johno
// Changes for new "Blank Page" (poster) project type
// 
// 10    7/12/98 5:32p Jayn
// Brochure previews were reversed (tall vs. wide)
// 
// 9     6/01/98 10:44a Rlovejoy
// Adjusted frame drawing.
// 
// 8     5/31/98 6:14p Hforman
// 
// 7     5/31/98 5:18p Hforman
// added flags to erase full area and show 3d border
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "oriendlg.h"
#include "colordef.h"
#include "calinfo.h"
#include "util.h"
#include "filcmprs.h"

//#define  JTRACE_ON
#include "jtrace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectMakePreview

WNDPROC CProjectMakePreview::m_pfnSuperWndProc = NULL;

CProjectMakePreview::CProjectMakePreview(BOOL fEraseArea/*=TRUE*/, BOOL fDrawBorder/*=TRUE*/)
{
	m_fEraseArea = fEraseArea;
	m_fDrawBorder = fDrawBorder;
   m_pCBitmap = NULL;
}

CProjectMakePreview::~CProjectMakePreview()
{
   FreeCBitmap();
}

void CProjectMakePreview::FreeCBitmap()
{
	if (m_pCBitmap != NULL)
	{
		CBitmapFileCompress Destroyer;
		Destroyer.FreeData((LPBYTE)m_pCBitmap);
		m_pCBitmap = NULL;
	}
}

BEGIN_MESSAGE_MAP(CProjectMakePreview, CButton)
   //{{AFX_MSG_MAP(CProjectMakePreview)
   ON_WM_ERASEBKGND()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectMake message handlers

void CProjectMakePreview::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   CString  csName;
   CBrush   cbBackground;
   CDC      *pDC = NULL;
   int      nContext = 0;
      
   TRY
   {
      if ((lpDrawItemStruct->itemAction & ODA_DRAWENTIRE) != 0)
      {
         // Get the DC to draw into.        
         CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
            
         if (pDC != NULL)
         {
            // Save the current DC so we can do all sorts of things to it.
            if ((nContext = pDC->SaveDC()) != 0)
            {
               // Compute the area to draw the preview.
               CRect crPreview;
               crPreview = lpDrawItemStruct->rcItem;
               
               if (m_pCBitmap != NULL)
               {
						// Copy bitmap to control
						LPBITMAPINFOHEADER pBitmap = (LPBITMAPINFOHEADER)m_pCBitmap; 
                  
						// Compute the aligned destination rectangle.      
                  int nImageWidth = pBitmap->biWidth;
                  int nImageHeight = pBitmap->biHeight;
                                       
                  int nTargetWidth = crPreview.Width();
                  int nTargetHeight = crPreview.Height();
                                       
                  int nNumerator = nTargetWidth;
                  int nDenominator = nImageWidth;
                                       
                  if (((long)nTargetWidth*(long)nImageHeight) > ((long)nTargetHeight*(long)nImageWidth))
                  {
                     nNumerator = nTargetHeight;
                     nDenominator = nImageHeight;
                  }
                                          
                  nImageWidth = MulDiv(nImageWidth, nNumerator, nDenominator);
                  nImageHeight = MulDiv(nImageHeight, nNumerator, nDenominator);

                  CRect crWmf(0, 0, nImageWidth, nImageHeight);
                  
                  crWmf.OffsetRect(
                     (nTargetWidth-nImageWidth)/2+crPreview.left,
                     (nTargetHeight-nImageHeight)/2+crPreview.top);
                     
                  // Erase the areas of the preview window that are not covered
                  // by the actual preview.
                  if (m_fEraseArea)
                  {
							cbBackground.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
                     if (crPreview.left < crWmf.left)
                     {
                        CRect crErase(crPreview.left, crPreview.top, crWmf.left, crPreview.bottom);
                        pDC->FillRect(crErase, &cbBackground);
                     }
                        
                     if (crPreview.top < crWmf.top)
                     {
                        CRect crErase(crWmf.left, crPreview.top, crWmf.right, crWmf.top);
                        pDC->FillRect(crErase, &cbBackground);
                     }
                        
                     if (crPreview.right > crWmf.right)
                     {
                        CRect crErase(crWmf.right, crPreview.top, crPreview.right, crPreview.bottom);
                        pDC->FillRect(crErase, &cbBackground);
                     }
                        
                     if (crPreview.bottom > crWmf.bottom)
                     {
                        CRect crErase(crWmf.left, crWmf.bottom, crWmf.right, crPreview.bottom);
                        pDC->FillRect(crErase, &cbBackground);
                     }
                  }

                  // Draw a simple 3D border around the preview area.                  
						if (m_fDrawBorder)
						{
							Util::Draw3dBorder(
								pDC,
								crWmf,
								GetSysColor(COLOR_BTNHIGHLIGHT),
								GetSysColor(COLOR_BTNSHADOW),
								GetSysColor(COLOR_BTNFACE),
								GetSysColor(COLOR_BTNTEXT),
								RGB(255,255,255),
								2,
								TRUE,
								TRUE,
								FALSE);

							crWmf.InflateRect(-3,-3);
						}
						else
						{
							// just draw a black border
							int nBorderWidth = pBitmap->biWidth+2;
							int nBorderHeight = pBitmap->biHeight+2;
							CRect crBorder(CPoint(crWmf.left, crWmf.top), CSize(nBorderWidth, nBorderHeight));
							crBorder.OffsetRect(
								(crWmf.Width() - nBorderWidth) / 2,
								(crWmf.Height() - nBorderHeight) / 2);
							CPen* pPen = CPen::FromHandle((HPEN)GetStockObject(BLACK_PEN));
							pDC->SelectObject(pPen);
							pDC->Rectangle(crBorder);
	
							crWmf.InflateRect(-1,-1);
						}

                  // Set the viewport to be our rectangle.                  
                  pDC->SetViewportOrg(crWmf.left, crWmf.top);
                  pDC->SetViewportExt(crWmf.Width(), crWmf.Height());
						
						// Use our global PrintMaster palette.
						extern CPalette* pOurPal;
						CPalette* pOldPal = pDC->SelectPalette(pOurPal, FALSE);
						pDC->RealizePalette();

						// Compute a centered placement
						CPoint ptPlace;
						ptPlace.x = (crWmf.Width() - pBitmap->biWidth) / 2;
						ptPlace.y = (crWmf.Height() - pBitmap->biHeight) / 2;

						LPBYTE pBitmapData = (LPBYTE)pBitmap + sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD);
						::SetDIBitsToDevice(pDC->GetSafeHdc(),
												  ptPlace.x, ptPlace.y,	// dest x, y
												  pBitmap->biWidth,
												  pBitmap->biHeight,
												  0, 0,              // source x, y
												  0,                 // first scan line
												  pBitmap->biHeight, // # of scan lines
												  pBitmapData,
												  (LPBITMAPINFO)pBitmap,
												  DIB_RGB_COLORS);

						pDC->SelectPalette(pOldPal, FALSE);
					}
						                        
               pDC->RestoreDC(nContext);
               nContext = NULL;
            }
         }
      }
   }
   CATCH_ALL(e)
   {
      if (pDC != NULL)
      {
         if (nContext != 0)
         {
            pDC->RestoreDC(nContext);
            nContext = 0;
         }
      }
      
		FreeCBitmap();      
      m_csCBitmapName.Empty();
   }
   END_CATCH_ALL
}

void CProjectMakePreview::UpdateCBitmap(const char *cpName)
{
	// We have never loaded a bitmap or the name has changed, so load it now
   if ((m_pCBitmap == NULL) || (m_csCBitmapName != cpName))
   {
      // Free any existing CBitmap
		FreeCBitmap();
      
      // Save the name
		// The address must be valid for the life of the CWindowsMetaFile object
      m_csCBitmapName = cpName;
      
      if (!m_csCBitmapName.IsEmpty())
      {
 			// Load & decompress the bitmap
			CBitmapFileCompress Decompressor;
			LPBYTE pCBitmap;
			DWORD dwBitmapSize;
			BOOL fSuccess = FALSE;
			if (Decompressor.Decompress(NULL, (LPCTSTR)m_csCBitmapName, &pCBitmap, &dwBitmapSize))
			{
				if (pCBitmap)
				{
					m_pCBitmap = (LPBITMAPINFO)pCBitmap;
					fSuccess = TRUE;
				}
			}

			// PMGTODO: Remove the following once we have all the preview bitmaps.
			if (!fSuccess)
			{
				if (Decompressor.Decompress(NULL, "UNDERCONSTRUCTION_BM", &pCBitmap, &dwBitmapSize))
				{
					if (pCBitmap)
					{
						m_pCBitmap = (LPBITMAPINFO)pCBitmap;
					}
				}
			}

		}
   }
}
               
BOOL CProjectMakePreview::OnEraseBkgnd(CDC* pDC)
{
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// COrientationDialog dialog

COrientationDialog::COrientationDialog(NEWINFO_REF info, UINT id /*=0*/, WORD wSoundID /*=0*/, CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(id == 0 ? COrientationDialog::IDD : id, pParent, BRAND_NEW_COLOR)
{
   //{{AFX_DATA_INIT(COrientationDialog)
   m_subtype = 0;
   m_orientation = info.orientation;
   m_nPanels = (info.type == PROJECT_TYPE_Brochure) ? info.subtype-2 : 0;
	m_fDoubleSided = info.fDoubleSided;
   //}}AFX_DATA_INIT
   m_subtype = (info.type == PROJECT_TYPE_Card
             || info.type == PROJECT_TYPE_NoteCard)
                  ? info.subtype == CARD_TopFold ? 1 : 0
                  : 0;

   m_subtype = (info.type == PROJECT_TYPE_Banner)
                  ? info.subtype == BANNER_Vertical ? 1 : 0
                  : 0;

	if ((info.type == PROJECT_TYPE_Calendar)
	 && (info.m_pCalendarInfo != NULL))
	{
		m_subtype = info.m_pCalendarInfo->style.type;
	}

   set_sound_id(wSoundID == 0 ? COrientationDialog::IDD : wSoundID);
   m_nType = info.type;
}

void COrientationDialog::DoDataExchange(CDataExchange* pDX)
{
   CPmwDialogColor::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(COrientationDialog)
   DDX_Radio(pDX, IDC_NEW_TALL, m_orientation);
   DDX_Radio(pDX, IDC_NEW_SIDE, m_subtype);
   DDX_Radio(pDX, IDC_NEW_TWO_PANELS, m_nPanels);
   DDX_Check(pDX, IDC_DOUBLE_SIDED, m_fDoubleSided);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COrientationDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(COrientationDialog)
   ON_BN_CLICKED(IDC_NEW_FOUR_PANELS, DoRadioClick)
   ON_BN_CLICKED(IDC_NEW_PANELS, DoRadioClick)
   ON_BN_CLICKED(IDC_NEW_TALL, DoRadioClick)
   ON_BN_CLICKED(IDC_NEW_THREE_PANELS, DoRadioClick)
   ON_BN_CLICKED(IDC_NEW_TOP, DoRadioClick)
   ON_BN_CLICKED(IDC_NEW_SIDE, DoRadioClick)
   ON_BN_CLICKED(IDC_NEW_TWO_PANELS, DoRadioClick)
   ON_BN_CLICKED(IDC_NEW_WIDE, DoRadioClick)
	ON_BN_CLICKED(IDC_DOUBLE_SIDED, DoRadioClick)
	ON_BN_CLICKED(IDC_FINISH, OnFinish)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrientationDialog message handlers
BOOL COrientationDialog::OnInitDialog()
{
   VERIFY (m_Preview.SubclassDlgItem (IDC_MAKE_PREVIEW, this));
/* Do the normal init dialog. */

   CPmwDialogColor::OnInitDialog();

/* Update the orientation buttons. */

   UpdateControls ();
   DoRadioClick ();
   
   return TRUE;
}

void COrientationDialog::OnOK()
{
   UpdateData(TRUE);
   
   CPmwDialogColor::OnOK();
}

VOID COrientationDialog::get_exit_info(NEWINFO_PTR info)
{
   info->orientation = m_orientation;
	info->fDoubleSided = m_fDoubleSided;

   // Handle any overrides.

   switch (info->type)
   {
      case PROJECT_TYPE_BusinessCard:
      {
			// Orientation flipped to real paper paper orientation in PMW.CPP.
         break;
      }
      case PROJECT_TYPE_Banner:
      {
         info->subtype = m_subtype ? BANNER_Vertical : BANNER_Horizontal;
         break;
      }
      case PROJECT_TYPE_Envelope:
      {
         info->orientation = LANDSCAPE;
         break;
      }
      case PROJECT_TYPE_Label:
      {
         info->orientation = PORTRAIT;
         break;
      }
      case PROJECT_TYPE_Card:
      case PROJECT_TYPE_NoteCard:
      {
         info->subtype = m_subtype ? CARD_TopFold : CARD_SideFold;
         break;
      }
      case PROJECT_TYPE_HalfCard:
		{
			// Orientation flipped to real paper paper orientation in PMW.CPP.
         info->subtype = info->orientation == LANDSCAPE ? CARD_TopFold : CARD_SideFold;
         break;
		}
      case PROJECT_TYPE_Brochure:
      {
			// Orientation flipped to real paper paper orientation in PMW.CPP.
         info->subtype = m_nPanels+2;
         break;
      }
      case PROJECT_TYPE_BlankPage:
		{
			// Blank pages don't really exist
			// change the type to poster
			info->type = PROJECT_TYPE_Poster;
		}
		default:
      {
         break;
      }
   }
}

VOID COrientationDialog::UpdateControls()
{
   int   type = m_nType;

   static struct
   {
      BOOL        m_fShowOrientation;
      BOOL        m_fShowFold;
      BOOL        m_fShowPanels;
		BOOL			m_fShowDoubleSided;
   } Show [] =
   {
      { FALSE, FALSE, FALSE, FALSE},         // 
      { TRUE, FALSE, FALSE, FALSE},          // Poster 
      { TRUE, TRUE, FALSE, FALSE},           // Card   
      { TRUE, TRUE, FALSE, FALSE},         // Banner 
      { TRUE, FALSE, FALSE, FALSE},          // Calendar 
      { FALSE, FALSE, FALSE, FALSE},         // Label 
      { FALSE, FALSE, FALSE, FALSE},         // Envelope 
      { TRUE, FALSE, FALSE, FALSE},          // Business Card 
      { TRUE, FALSE, FALSE, FALSE},          // Certificate 
      { TRUE, TRUE, FALSE, FALSE},           // Note Card 
      { TRUE, FALSE, FALSE, FALSE},          // Fax Cover 
      { TRUE, FALSE, FALSE, FALSE},          // Stationery
      { TRUE, FALSE, FALSE, TRUE},           // Newsletter 
      { TRUE, FALSE, TRUE, FALSE},           // Brochure 
      { TRUE, FALSE, FALSE, FALSE},          // Half-fold Card 
      { TRUE, FALSE, FALSE, FALSE},          // Web Publication
      { FALSE, FALSE, FALSE, FALSE},         // Post Card
      { FALSE, FALSE, FALSE, FALSE},         // Sticker
      { FALSE, FALSE, FALSE, FALSE},         // T-Shirt
      { FALSE, FALSE, FALSE, FALSE},         // Craft
      { FALSE, FALSE, FALSE, FALSE},         // PhotoProject
      { TRUE, FALSE, FALSE, FALSE},				// Blank page (poster)
   };

   if (type >= 0 && type < sizeof(Show)/sizeof(Show[0]))
   {
      BOOL fShowOrientation = Show [type].m_fShowOrientation;
      ShowControl(IDC_NEW_ORIENTATION, fShowOrientation);
      ShowControl(IDC_NEW_TALL, fShowOrientation);
      ShowControl(IDC_NEW_WIDE, fShowOrientation);

      BOOL fShowFold = Show [type].m_fShowFold;
      ShowControl(IDC_NEW_FOLD, fShowFold);
      ShowControl(IDC_NEW_SIDE, fShowFold);
      ShowControl(IDC_NEW_TOP, fShowFold);

      BOOL fShowPanels = Show [type].m_fShowPanels;
      ShowControl(IDC_NEW_PANELS, fShowPanels);
      ShowControl(IDC_NEW_TWO_PANELS, fShowPanels);
      ShowControl(IDC_NEW_THREE_PANELS, fShowPanels);
      ShowControl(IDC_NEW_FOUR_PANELS, fShowPanels);
   
      BOOL fShowDoubleSided = Show [type].m_fShowDoubleSided;
      ShowControl(IDC_DOUBLE_SIDED, fShowDoubleSided);

      CreateWizzardButtons ();

		//change the text for banner radio buttons
		//that dialog is already too crowded
		if (type == PROJECT_TYPE_Banner)
		{
			CWnd* pOrientBox = GetDlgItem(IDC_NEW_ORIENTATION);
			pOrientBox->SetWindowText("Paper");

			CButton* pTallRadio = (CButton*)GetDlgItem(IDC_NEW_TALL);
			pTallRadio->SetWindowText("Portrait");

			CButton* pWideRadio = (CButton*)GetDlgItem(IDC_NEW_WIDE);
			pWideRadio->SetWindowText("Landscape");

			CWnd* pFoldBox = GetDlgItem(IDC_NEW_FOLD);
			pFoldBox->SetWindowText("Orientation");

			CButton* pSideRadio = (CButton*)GetDlgItem(IDC_NEW_SIDE);
			pSideRadio->SetWindowText("Horizontal");

			CButton* pTopRadio = (CButton*)GetDlgItem(IDC_NEW_TOP);
			pTopRadio->SetWindowText("Vertical");
		}

      if (type == PROJECT_TYPE_Calendar)
		{
         EnableWizardButtons (WIZBUT_ALL);
		}
      else
		{
         EnableWizardButtons (WIZBUT_BACK | WIZBUT_FINISH | WIZBUT_CANCEL | WIZBUT_HELP);
		}
   }
}


void
COrientationDialog::DoRadioClick (void)
{
   int      type = m_nType;
   CString  MetaName;

   static struct
   {
      const char *m_MetaName;
      const char *Tall;
      const char *Wide;
      const char *Side;
      const char *Top;
      const char *Fold2;
      const char *Fold3;
      const char *Fold4;
		const char *SingleSided;
		const char *DoubleSided;
   } Meta [] =
   {
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// 
      {"POSTER",      "P", "L", "",  "",  "",  "",  "",  "",  ""  },		// Poster 
      {"CARD",        "T", "W", "S", "T", "",  "",  "",  "",  ""  },		// Card   
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// Banner 
      {"*COMPUTED*",  "P", "L", "",  "",  "",  "",  "",  "",  ""  },		// Calendar 
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// Label 
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// Envelope 
      {"BIZCARD",     "P", "L", "",  "",  "",  "",  "",  "",  ""  },		// Business Card 
      {"CERTIFICATE", "P", "L", "",  "",  "",  "",  "",  "",  ""  },		// Certificate 
      {"NOTECARD",    "T", "W", "S", "T", "",  "",  "",  "",  ""  },		// Note Card 
      {"FAXCOVER",    "P", "L", "",  "",  "",  "",  "",  "",  ""  },		// Fax Cover 
      {"STATIONERY",  "P", "L", "",  "",  "",  "",  "",  "",  ""  },		// Stationery
      {"NEWSLETTER",  "P", "L", "",  "",  "",  "",  "",  "S", "D" },		// Newsletter 
      {"BROCHURE",    "P", "L", "",  "",  "2", "3", "4", "",  ""  },		// Brochure 
      {"HCARD",       "T", "W", "",  "",  "",  "",  "",  "",  ""  },		// Half-fold Card 
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// Web Publication
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// Post Card
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// Sticker
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// T-Shirt
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// Craft
      {"",            "",  "",  "",  "",  "",  "",  "",  "",  ""  },		// Photo project
      {"BLANK",       "P", "L", "",  "",  "",  "",  "",  "",  ""  }		// Blank page (poster)
   };

   if (type >= 0 && type < sizeof (Meta) / sizeof(Meta [0]))
   {
		if (type == PROJECT_TYPE_Calendar)
		{
			// Base calendar name depends on type (yearly, monthly, weekly.)
			switch (m_subtype)
			{
				case YEARLY:
				{
					MetaName = "CALENDARYN";
					break;
				}
				case MONTHLY:
				{
					MetaName = "CALENDARMN";
					break;
				}
				case WEEKLY:
				{
					MetaName = "CALENDARWN";
					break;
				}
				default:
				{
					ASSERT(FALSE);
					break;
				}
			}
		}
		else
		{
			MetaName = Meta [type].m_MetaName;
		}

		MetaName += IsDlgButtonChecked (IDC_NEW_TALL) ? Meta [type].Tall : Meta [type].Wide;
		MetaName += IsDlgButtonChecked (IDC_NEW_SIDE) ? Meta [type].Side : Meta [type].Top;
      
      if (IsDlgButtonChecked (IDC_NEW_TWO_PANELS))
         MetaName += Meta [type].Fold2;
      else
      if (IsDlgButtonChecked (IDC_NEW_THREE_PANELS))
         MetaName += Meta [type].Fold3;
      else
      if (IsDlgButtonChecked (IDC_NEW_FOUR_PANELS))
         MetaName += Meta [type].Fold4;

		if (IsDlgButtonChecked(IDC_DOUBLE_SIDED))
		{
			MetaName += Meta[type].DoubleSided;
		}
		else
		{
			MetaName += Meta[type].SingleSided;
		}

      MetaName += "_BM";
   }

   m_Preview.UpdateCBitmap(MetaName);
   m_Preview.Invalidate();
}

void COrientationDialog::ShowControl(UINT nID, BOOL fShow)
{
   CWnd* pChild = GetDlgItem(nID);
   if (pChild != NULL)
   {
      pChild->ShowWindow(fShow ? SW_SHOW : SW_HIDE);
      pChild->EnableWindow(fShow);
   }
}

void COrientationDialog::OnFinish() 
{
   OnOK ();   
}

void COrientationDialog::OnBack() 
{
   EndDialog (IDC_BACK);
}
