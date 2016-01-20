/*
// $Workfile: DsnCheck.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DsnCheck.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 19    2/19/99 2:53p Johno
// Improved listbox auto - resizing
// 
// 18    1/04/99 5:12p Johno
// Moved CWebView / CWebDoc code into CPmwDoc / CPmwView.
// 
// 17    7/23/98 5:41p Cboggio
// Split up IDS_WEBCHK_MINSEC and IDS_WEBCHK_SEC into multiple strings
// because simply appending an 's' to the end of a word to make it plural
// does NOT work in foreign languages.
// 
// 16    7/22/98 6:15p Johno
// Minor changes
// 
// 15    6/16/98 2:33p Johno
// Dialog box and listbox re-size to ensure that the strings are fully
// visible
// 
// 14    6/15/98 5:48p Johno
// Changes for new list box strings (split into multiple strings)
// 
// 13    6/03/98 2:46p Johno
// Changes for "run design checker" menu option
// 
// 12    6/02/98 7:04p Johno
// Simplifications / changes / debugs to find HTML positioning bug
// 
// 11    6/02/98 12:00p Johno
// Tweaks
// 
// 10    6/01/98 6:34p Johno
// Internationalized, simple owner drawn list box
// 
// 9     5/31/98 5:27p Johno
// Functional HTML warning rectangle
// 
// 8     5/30/98 6:27p Johno
// Changes for HTML warning rect
// 
// 7     5/28/98 4:57p Johno
// Changes for HTML compliance groups
// 
// 6     5/28/98 11:49a Johno
// Print pagination
// 
// 5     5/28/98 10:21a Johno
// Compile update
// 
// 4     5/28/98 9:08a Johno
// Disign checker printing
// 
// 3     5/26/98 6:52p Johno
// 
// 2     5/22/98 3:42p Johno
// Created
//
*/
// DsnCheck.cpp : implementation file

#include "stdafx.h"
#include "pmw.h"
#include "DsnCheck.h"

#include "pmwdoc.h"     // For printing
#include "webdoc.h"
#include "pmwview.h"
#include "webview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CListDesignChecker::~CListDesignChecker()
{
	for(int i = 0;
		i < mItems.GetSize();
		i ++)
	{
		CListBoxThing *p = mItems.GetAt(i);
		delete p;	
	}
}

CHTMLCompliantInfo*
CListDesignChecker::GetItemFor(int idx)
{
	if ((idx >= 0) && (idx < mItems.GetSize()))
	{
		CListBoxThing *p = mItems.GetAt(idx);
		return p->GetInfoPointer();
	}

	return NULL;
}

int 
CListDesignChecker::AddString(LPCTSTR lpszItem, enum CListBoxThing::ListBoxThingType Type, CHTMLCompliantInfo *pInfo)
{
   int		ret, idx = 0;
	CString	Temp;
	enum CListBoxThing::ListBoxThingType lType = Type;
	CHTMLCompliantInfo *lpInfo = pInfo;

	do
	{
		Temp.Empty();

		for (idx; lpszItem [idx] != 0; idx ++)
		{
			if (lpszItem [idx] == '\n')
			{
				idx ++;
				break;
			}
			Temp += lpszItem [idx];
		}

		ret = CListBox::AddString (Temp);

		if ((ret != LB_ERR) && (ret != LB_ERRSPACE))
		{
			TRY
			{
				CListBoxThing *p = new CListBoxThing(lType, lpInfo);	
				mItems.Add(p);
			}
			CATCH_ALL(e)
			{
				DeleteString (ret);
				ret = LB_ERR;
			}
			END_CATCH_ALL
		}

		lType = CListBoxThing::Plain;
		lpInfo = NULL;
	} while ((ret != LB_ERR) && (ret != LB_ERRSPACE) && (lpszItem [idx] != 0));

   return ret;
}

void 
CListDesignChecker::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);
   
   pDC->SaveDC();

   CRect Rect(lpDIS->rcItem);

   if (lpDIS->itemState & ODS_SELECTED)
   {
      pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
      pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
   }
   else
   {
      pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
      pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
   }

   pDC->ExtTextOut(0, 0, ETO_CLIPPED | ETO_OPAQUE, &Rect, NULL, 0, NULL);

   CFont FontBold;
   CString csText;
   GetText(lpDIS->itemID, csText);
   if (csText.IsEmpty() == FALSE)
   {
      CListBoxThing *p;
		if ((int)lpDIS->itemID < mItems.GetSize())
		{
			p = mItems.GetAt(lpDIS->itemID);
			
			int h = Rect.Height();
			int h2 = h / 4;
				
			if (p->Type() == CListBoxThing::Data)
			{
				HBRUSH	hBrushOld, hBrush = (HBRUSH)::GetStockObject(BLACK_BRUSH);
				HPEN		hPenOld, hPen = (HPEN)::GetStockObject(BLACK_PEN);

				hBrushOld = (HBRUSH)::SelectObject(pDC->GetSafeHdc(), hBrush);
				hPenOld = (HPEN)::SelectObject(pDC->GetSafeHdc(), hPen);
				
				pDC->Ellipse(Rect.left + h2, Rect.top + h2, Rect.left + h - h2, Rect.bottom - h2);
				
				::SelectObject(pDC->GetSafeHdc(), hBrushOld);
				::SelectObject(pDC->GetSafeHdc(), hPenOld);
			}
			// Indent the same as height
			Rect.left += h;

			if ((p->Type() == CListBoxThing::Bold) || (p->Type() == CListBoxThing::Data))
			{
				CFont *pFont = NULL;
				if ((pFont = GetFont()) != NULL)
				{
					LOGFONT logfont;
					pFont->GetLogFont(&logfont);
					logfont.lfWeight = FW_BOLD;
					FontBold.CreateFontIndirect(&logfont);
					pDC->SelectObject(&FontBold);
				}
			}
		}

      pDC->DrawText(csText, -1, &Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
   }

   pDC->RestoreDC(-1);
}

void 
CListDesignChecker::MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
   if (pMeasureItemStruct->CtlType == ODT_LISTBOX)
   {
      CRect Rect;
      GetClientRect(&Rect);
      pMeasureItemStruct->itemWidth = Rect.Width();

      CClientDC dc (this);
      
      CString csText;

      GetText(pMeasureItemStruct->itemID, csText);
      CSize    Size = dc.GetTextExtent(csText);
      
      pMeasureItemStruct->itemHeight = Size.cy;
   }
}
 
void
CDesignCheckerDlg::SizeToWidestString(void)
{
	CRect	RectList;
	m_List1.GetWindowRect(&RectList);
	// Get the widest string width from the list box (width + indent)
	int Widest = m_List1.GetWidest();
	// Figure the difference between current width and desired width
	int dx = (Widest - RectList.Width()) / 2;
	// Are we too small?
	if (dx > 0)
	{
		CRect	RectWind;
		GetWindowRect(&RectWind);
		// Make the main window wider
		SetWindowPos(NULL, 0, 0, RectWind.Width(), RectWind.Height(), SWP_NOZORDER | SWP_NOMOVE);
		// Make the listbox wider
		m_List1.SetWindowPos(NULL, 0, 0, RectList.Width(), RectList.Height(), SWP_NOZORDER | SWP_NOMOVE);
TRACE("Design checker window re-sized\n");
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDesignCheckerDlg dialog


CDesignCheckerDlg::CDesignCheckerDlg(LPCSTR ButtonOKText/*NULL*/, CPmwView* pParent, BOOL ButtonOKEnable /*TRUE*/)
	: CDialog(CDesignCheckerDlg::IDD, (CWnd*)pParent)
{
	//{{AFX_DATA_INIT(CDesignCheckerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   SetButtonOKText(ButtonOKText, ButtonOKEnable);
   ASSERT(pParent != NULL);
   m_pView = pParent;
}


void CDesignCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDesignCheckerDlg)
	DDX_Control(pDX, IDOK, m_ButtonOK);
//	DDX_Control(pDX, IDC_LIST1, m_List1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDesignCheckerDlg, CDialog)
	//{{AFX_MSG_MAP(CDesignCheckerDlg)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
CDesignCheckerDlg::Empty(void)
{
   for (int i = 0; i < PageStats.GetSize(); i ++) 
   {
      CWebPageStats *pStats = PageStats.GetAt(i);
      delete pStats;
   }
}

void
CDesignCheckerDlg::AddPageStats(CWebPageStats &ps)
{
   // Make a copy of this thing and save it
   CWebPageStats *pStats = new CWebPageStats;
   pStats->CopyOf(ps);
   PageStats.Add(pStats);
}
/////////////////////////////////////////////////////////////////////////////
// CDesignCheckerDlg message handlers

BOOL CDesignCheckerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int   s = PageStats.GetSize();

   if (s < 1)
      EndDialog(IDOK); // Nothing to do!
   else
   {
      if (m_pView != NULL)
      {
	      VERIFY(m_List1.SubclassDlgItem(IDC_LIST1, this));
         
         CString Temp, Temp2;
			
			Temp.LoadString(IDS_WEB_INTRO);
			m_List1.AddStringBold(Temp);
			Temp.LoadString(IDS_WEB_COMMENT);
			m_List1.AddString(Temp);
         // Stats for each page
         for (int i = 0; i < s; i ++)
         {
            // "Page 1:"
				Temp.Format(IDS_WEBCHK_PAGE, i + 1);

            m_List1.AddString("");   
            m_List1.AddStringBold(Temp);

            CWebPageStats *pStats = PageStats.GetAt(i);
            int min = 0, sec = pStats->GetDownloadSecondsTotalInt(28800);
            while (sec > 60)
            {
               min ++;
               sec -= 60;
            }
				// "Download time (28800 bps)
            if (min > 0)
			{
				UINT StrID;
				if (min == 1)
				{
					if (sec == 1)
						StrID = IDS_WEBCHK_MIN1SEC1;
					else 
		   				StrID = IDS_WEBCHK_MIN1SEC;
				}
				else if (sec == 1)
		   			StrID = IDS_WEBCHK_MINSEC1;
				else
					StrID = IDS_WEBCHK_MINSEC;
                Temp.Format(StrID, min, sec);
            }
            else
                Temp.Format((sec == 1)?IDS_WEBCHK_SEC1:IDS_WEBCHK_SEC, sec);

            Temp2.LoadString(IDS_WEBCHK_DLTIME);
            Temp2 += Temp;
            m_List1.AddString(Temp2);
            
				// Stats for each object group
            for (int j = 0; j < pStats->NumComplianceGroups(); j ++)
            {
               if (j == 0)
               {
                  // "HTML Conversion Issues"
						m_List1.AddString("");
                  Temp.LoadString(IDS_WEBCHK_COMPLIANCE);
                  m_List1.AddStringBold(Temp);
               }

               CHTMLCompliantInfo *pInfo = pStats->GetComplianceGroup(j);
               ASSERT(pInfo != NULL);
               if (pInfo != NULL)
               {
                  ASSERT(pInfo->HasComplianceErrors());
                  if (pInfo->GetFirstString(Temp2))
                  {
                     //Temp = "   - ";
							Temp = "";
                     Temp += Temp2;
#ifdef _DEBUG
                     if (pInfo->NumPMObjects() > 1)
                     {
                        Temp2.Format(" (%d)", pInfo->NumPMObjects());
                        Temp += Temp2;
                     }
#endif
                     m_List1.AddString(Temp, CListBoxThing::Data, pInfo);
                     while (pInfo->GetNextString(Temp2))
                     {
                        //Temp = "   - ";
								Temp = "";
                        Temp += Temp2;
                        m_List1.AddString(Temp, pInfo);
                     }
                  }
               }
            }
         }
		
			SizeToWidestString();
      }

	   if (m_ButtonOKText.IsEmpty() == FALSE)
         m_ButtonOK.SetWindowText(m_ButtonOKText);

      m_ButtonOK.EnableWindow(m_fButtonOKEnable);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDesignCheckerDlg::OnPrint() 
{
   DWORD f = PD_NOPAGENUMS | PD_NOSELECTION ;
   CString  str;
   CPrintDialog pd (FALSE, f, m_pView);

   if (pd.DoModal() == IDOK)
   {
      CDC   dc;
      if (dc.Attach(pd.GetPrinterDC()))
      {
         // Just to be sure
         dc.SetTextColor(RGB(0,0,0));
         dc.SetMapMode(MM_TEXT);

         int Height = dc.GetDeviceCaps(VERTRES), OneLine; 
         int      DocError;
         CString  Text = " ";;
         CSize    Size = dc.GetTextExtent(Text);
         OneLine = Size.cy;
         
         DOCINFO  di;
         memset(&di, 0, sizeof(DOCINFO));
         di.cbSize = sizeof(DOCINFO);

         str.LoadString(IDS_WEBCHK_PRINTTITLE);
         di.lpszDocName = str;

         for (int i = 0; i < pd.GetCopies(); i++)
         {
            if ((DocError = dc.StartDoc(&di)) >= 0)
            {
               if ((DocError = dc.StartPage()) >= 0)
               {
                  CRect    Rect(60, OneLine, 0, 0);
                  for (int i = 0; i < m_List1.GetCount(); i ++)
                  {
                     m_List1.GetText(i, Text);
                     
                     Size = dc.GetTextExtent(Text);
                     Rect.right = Size.cx + Rect.left;
                     Rect.bottom = Size.cy + Rect.top;
                     // Will this go off the bottom of the page?
                     if (Rect.bottom + OneLine >= Height)
                     {
                        DocError = dc.EndPage();
                        DocError = dc.StartPage();
                        int h = Rect.Height();
                        Rect.top = OneLine;
                        Rect.bottom = Rect.top + h;
                     }

                     dc.DrawText(Text, -1, &Rect, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
                     Rect.OffsetRect(0, Size.cy);
                  }
                  DocError = dc.EndPage();
               }
               DocError = dc.EndDoc();
            }
            // Abort - something went wrong somewhere
            if (DocError < 0)
               break;
         }
      }
   }
}

void CDesignCheckerDlg::OnSelchangeList1() 
{
   int               Sel;
   DB_RECORD_NUMBER  PageRec = -1;
   PBOX              bound;
   bound.Empty();

   if (m_pView != NULL)
   {
      if ((Sel = m_List1.GetCurSel()) != LB_ERR)
      {
			CHTMLCompliantInfo *pInfo = m_List1.GetItemFor(Sel);
			if (pInfo != NULL)
         {
            pInfo->GetObjectsPBOXExpanded(bound);
            PageRec = pInfo->GetObjectsPage();
         }
		}
      // Set (or clear) the warning rect
      m_pView->SetWarningRect(bound, PageRec);
   }
}

void CDesignCheckerDlg::OnOK() 
{
   PBOX  bound;
   bound.Empty();

   if (m_pView != NULL)
   {
      m_pView->SetWarningRect(bound, -1);
   }
	
	CDialog::OnOK();
}

void CDesignCheckerDlg::OnCancel() 
{
   PBOX  bound;
   bound.Empty();

   if (m_pView != NULL)
   {
      m_pView->SetWarningRect(bound, -1);
   }
	
	CDialog::OnCancel();
}
