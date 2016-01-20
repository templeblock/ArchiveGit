/*
// $Header: /PM8/App/FMTBAR.CPP 1     3/03/99 6:05p Gbeddow $
//
// barmenu.cpp : implementation file
//
// $Log: /PM8/App/FMTBAR.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 3     3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 2     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.0   14 Aug 1997 15:20:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:30   Fred
// Initial revision.
// 
//    Rev 1.7   03 Apr 1997 09:30:54   Fred
// More text cleanup
// 
//    Rev 1.6   13 Mar 1997 15:17:42   Jay
// Fixed format bar for warp text editing
// 
//    Rev 1.5   15 Jan 1997 10:49:20   Jay
// New toolbars
// 
//    Rev 1.4   29 Jul 1996 18:13:38   Fred
//  
// 
//    Rev 1.3   28 Jul 1996 10:18:52   Jay
// Text color, new color dialog
// 
//    Rev 1.2   23 Jul 1996 10:40:30   Jay
// Added newsletters and brochures; removed backgrounds
// 
//    Rev 1.1   14 Jun 1996 17:06:40   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:33:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:34   FRED
// Initial revision.
// 
//    Rev 1.12   26 Dec 1995 10:29:12   FRED
// Project structure changes for mac port
// 
//    Rev 1.11   20 Dec 1995 09:56:30   JAY
// 32-bit fixes
// 
//    Rev 1.10   18 Oct 1995 12:05:18   FRED
// Takes scroller width into account when sizing toolbar combos
// 
//    Rev 1.9   06 Sep 1995 11:27:38   JAY
// Added flip background buttons.
// 
//    Rev 1.8   04 Sep 1995 09:00:24   JAY
// Sound fixes.
// 
//    Rev 1.7   30 Aug 1995 15:13:20   FRED
// Added CBS_AUTOHSCROLL to typeface and pointsize combo boxes
// 
//    Rev 1.6   22 Aug 1995 19:21:42   FRED
//  
// 
//    Rev 1.5   20 Aug 1995 15:39:30   FRED
// Rotate object control
// 
//    Rev 1.4   14 Aug 1995 17:19:28   JAY
// Cut/paste/undo from warp text dialog.
// 
//    Rev 1.3   11 Aug 1995 12:51:32   FRED
//  
// 
//    Rev 1.2   10 Aug 1995 17:06:18   JAY
// Added new format bars.
// 
//    Rev 1.1   05 May 1995 17:09:54   FRED
//  
// 
//    Rev 1.0   19 Apr 1995 12:38:04   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "fmtbar.h"

#include "typeface.h"

#include <afxpriv.h>

#define TID_HIDDEN_CONTROL -3

/////////////////////////////////////////////////////////////////////////////
// CFormatBar

/*
// This array indicates the id that corresponds to each bitmap in the
// styles bitmap (the one the format bar contains).
// All cells which can possibly show up on the format bar must exist
// in the bitmap. The id array for a specific incarnation of a format bar
// will show the actual order of the cells on the screen (this array shows
// the order of the cells in the bitmap).
*/

UINT CFormatBar::m_uBitmapIds[] =
{
// same order as in the bitmap 'styles.bmp'
	ID_TEXT_BOLD,
	ID_TEXT_ITALIC,
	ID_TEXT_UNDERLINE,
	ID_ALIGN_LEFT,
	ID_ALIGN_CENTER,
	ID_ALIGN_RIGHT,
	ID_ALIGN_JUSTIFY,
	ID_ALIGN_TOP,
	ID_ALIGN_MIDDLE,
	ID_ALIGN_BOTTOM,
	IDC_TEXT_STRETCHING,
	ID_YFLIP,
	ID_XFLIP,
	ID_POSITION_STANDARD,
	ID_FILE_FREE_FONTS,
	ID_TEXT_EDIT_FRAME,
	ID_CALENDAR_PICTURE,
	ID_CALENDAR_TEXT,
	ID_EDIT_GROUP,
	ID_EDIT_UNGROUP,
	ID_EDIT_UNDO,
	ID_EDIT_REDO,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
	ID_TEXT_CHOOSECOLOR,
	ID_POSITION_VCENTER,
	ID_POSITION_HCENTER,
	ID_POSITION_CENTERONPAGE,
	ID_POSITION_FULLPAGE,
	0
};

BEGIN_MESSAGE_MAP(CFormatBar, CPmwToolBar)
	//{{AFX_MSG_MAP(CFormatBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFormatBar::CFormatBar()
{
	m_uBitmap = IDB_EDIT_TEXT_FORMAT;
}

BOOL CFormatBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	m_uID = nID;
	if (!CPmwToolBar::Create(pParentWnd, dwStyle))
	{
		return FALSE;
	}

   CSize czButton = CSize(16+7, 15+7);
   CSize czImage = CSize(16, 15);
   SetSizes(czButton, czImage);

/*
// Create the two "edit text" combo boxes.
*/

	CRect rect;

	rect.top = 3;
	rect.bottom = rect.top + DropHeight;
	rect.left = 0;
	rect.right = 150;

	if (!m_TypefaceBox.Create(CBS_DROPDOWN
										| CBS_SORT
										| CBS_AUTOHSCROLL
										| WS_VSCROLL
										| WS_CLIPSIBLINGS,
									  rect,
									  this,
									  IDC_TEXT_TYPEFACE))
	{
		TRACE0("Failed to create typeface combo-box\n");
		return FALSE;
	}

	m_TypefaceBox.LimitText(100);

	if (!m_SizeBox.Create(CBS_DROPDOWN
									| CBS_AUTOHSCROLL
									| WS_VSCROLL
									| WS_CLIPSIBLINGS,
								 rect,
								 this,
								 IDC_TEXT_SIZE))
	{
		TRACE0("Failed to create size combo-box\n");
		return FALSE;
	}

	m_SizeBox.LimitText(4);

	if (!m_RotateBox.Create(CBS_DROPDOWN
									| WS_VSCROLL
									| WS_CLIPSIBLINGS,
								 rect,
								 this,
								 IDC_ROTATE))
	{
		TRACE0("Failed to rotate combo-box\n");
		return FALSE;
	}
	
	m_RotateBox.LimitText(8);

	//  Create a font for the comboboxes
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));

	if (!::GetSystemMetrics(SM_DBCSENABLED))
	{
		// Since design guide says toolbars are fixed height so is the font.
		logFont.lfHeight = -10;
		logFont.lfWeight = FW_BOLD;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		CString strDefaultFont;
//		strDefaultFont.LoadString(IDS_DEFAULT_FONT);
		strDefaultFont = "MS Sans Serif";			// Cheat a little (for now!)
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		if (!m_font.CreateFontIndirect(&logFont))
		{
			TRACE0("Could not create font for combo boxes\n");
		}
		else
		{
			m_TypefaceBox.SetFont(&m_font);
			m_SizeBox.SetFont(&m_font);
			m_RotateBox.SetFont(&m_font);
		}
	}
	else
	{
		m_font.Attach(::GetStockObject(SYSTEM_FONT));
		m_TypefaceBox.SetFont(&m_font);
		m_SizeBox.SetFont(&m_font);
		m_RotateBox.SetFont(&m_font);
	}
	
	m_nFontAverageWidth = 8;
	
	if (m_font.GetObject(sizeof(logFont), &logFont) == sizeof(logFont))
	{
		m_nFontAverageWidth = logFont.lfHeight;
		if (m_nFontAverageWidth < 0)
		{
			m_nFontAverageWidth = -m_nFontAverageWidth;
		}
		m_nFontAverageWidth = (m_nFontAverageWidth*3)/4;
		if (m_nFontAverageWidth < 4)
		{
			m_nFontAverageWidth = 4;
		}
	}

	BuildTypefaceList();
	BuildPointSizeList();
	BuildRotateList();

	return TRUE;
}

/*
// Does the format bar have the focus?
*/

BOOL CFormatBar::HasFocus(void)
{
	CWnd* pFocus = GetFocus();

	return (pFocus == m_TypefaceBox.GetEdit()) || (pFocus == m_SizeBox.GetEdit()) || (pFocus == m_RotateBox.GetEdit());
}

/*
// Build the typeface list.
*/

void CFormatBar::BuildTypefaceList(void)
{
	m_TypefaceBox.SetRedraw(FALSE);
	m_TypefaceBox.ResetContent();

	BOOL is_registered = GET_PMWAPP()->user_is_registered();

/* Add all the chooseable faces to the list. */

	FaceEntry* entry;
	int i;

	for (entry = typeface_server.face_list.first_face(), i = 0;
					entry != NULL;
					entry = entry->next_face(), i++)
	{
		if (entry->is_chooseable(is_registered))
		{
			int nIndex = m_TypefaceBox.InsertString(-1, entry->get_name());

			if (nIndex != -1)
			{
				m_TypefaceBox.SetItemData(nIndex, (DWORD)i);
			}
		}
	}

	m_TypefaceBox.SetRedraw(TRUE);
	if (m_TypefaceBox.IsWindowVisible())
	{
		m_TypefaceBox.InvalidateRect(NULL);
	}
}

/*
// Build the pointsize list.
*/

void CFormatBar::BuildPointSizeList(void)
{
	m_SizeBox.SetRedraw(FALSE);
	m_SizeBox.ResetContent();

	m_SizeBox.AddString("6");
	m_SizeBox.AddString("8");
	m_SizeBox.AddString("10");
	m_SizeBox.AddString("12");
	m_SizeBox.AddString("14");
	m_SizeBox.AddString("18");
	m_SizeBox.AddString("24");
	m_SizeBox.AddString("36");
	m_SizeBox.AddString("48");
	m_SizeBox.AddString("60");
	m_SizeBox.AddString("72");

	m_SizeBox.SetRedraw(TRUE);
	if (m_SizeBox.IsWindowVisible())
	{
		m_SizeBox.InvalidateRect(NULL);
	}
}

void CFormatBar::BuildRotateList(void)
{
	m_RotateBox.SetRedraw(FALSE);
	m_RotateBox.ResetContent();

	m_RotateBox.AddString("0");
	m_RotateBox.AddString("45");
	m_RotateBox.AddString("90");
	m_RotateBox.AddString("135");
	m_RotateBox.AddString("180");
	m_RotateBox.AddString("225");
	m_RotateBox.AddString("270");
	m_RotateBox.AddString("315");

	m_RotateBox.SetRedraw(TRUE);
	if (m_RotateBox.IsWindowVisible())
	{
		m_RotateBox.InvalidateRect(NULL);
	}
}

/*
// Set the format bar using the passed ids.
*/

void CFormatBar::SetFormat(UINT* pIds, int nIds)
{
//	return;

	BOOL fShowedTypefaceBox = FALSE;
	BOOL fShowedSizeBox = FALSE;
	BOOL fShowedRotateBox = FALSE;
	
/* Allocate memory for our buttons. */

	if (SetButtons(NULL, nIds))
	{
	/* Now go through and set all buttons. */
		UINT* pId = pIds;

		for (int nId = 0; nId < nIds; nId++, pId++)
		{
			switch (*pId)
			{
				case ID_SEPARATOR:
				{
				// width of separator includes 2 pixel overlap
					SetButtonInfo(nId, *pId, TBBS_SEPARATOR, m_cxDefaultGap + 2);
					break;
				}
				case IDC_TEXT_TYPEFACE:
				{
				// We want the typeface combo box here.
					SetupCombo(nId, *pId, m_nFontAverageWidth*18, m_TypefaceBox);
					fShowedTypefaceBox = TRUE;
					break;
				}
				case IDC_TEXT_SIZE:
				{
				// We want the point size combo box here.
					SetupCombo(nId, *pId, m_nFontAverageWidth*10, m_SizeBox);
					fShowedSizeBox = TRUE;
					break;
				}
				case IDC_ROTATE:
				{
				// We want the point size combo box here.
					SetupCombo(nId, *pId, m_nFontAverageWidth*10, m_RotateBox);
					fShowedRotateBox = TRUE;
					break;
				}
#ifdef WIN32
				case TID_HIDDEN_CONTROL:
				{
				/* Our special hidden control. */
					SetButtonInfo(nId, *pId, TBBS_BUTTON, -1);
					break;
				}
#endif
				default:
				{
				/* A regular button. */
//					SetButtonInfo(nId, *pId, TBBS_BUTTON, IndexFromId(*pId));
					break;
				}
			}
		}
	}

/*
// Make sure everything redraws.
*/

	CRect crWindow;

	GetClientRect(&crWindow);
	InvalidateRect(crWindow);

/*
// Hide the controls we aren't using.
*/

	if (!fShowedTypefaceBox)
	{
		HideCombo(m_TypefaceBox);
	}
	if (!fShowedSizeBox)
	{
		HideCombo(m_SizeBox);
	}
	if (!fShowedRotateBox)
	{
		HideCombo(m_RotateBox);
	}

	SendMessage(WM_IDLEUPDATECMDUI, TRUE, 0L);
}

const UINT* CFormatBar::GetBitmapIds(void)
{
	return m_uBitmapIds;
}

/////////////////////////////////////////////////////////////////////////////
// Specific incarnations of the format bar.

/*
// Show the text edit format bar.
*/

static UINT BASED_CODE TextEditIds[] =
{
	IDC_TEXT_TYPEFACE,         // for typeface box (placeholder)
		ID_SEPARATOR,
	IDC_TEXT_SIZE,					// for size box (placeholder)
		ID_SEPARATOR,
	ID_TEXT_BOLD,
	ID_TEXT_ITALIC,
	ID_TEXT_UNDERLINE,
		ID_SEPARATOR,
	ID_TEXT_CHOOSECOLOR,
		ID_SEPARATOR,
	ID_ALIGN_LEFT,
	ID_ALIGN_CENTER,
	ID_ALIGN_RIGHT,
	ID_ALIGN_JUSTIFY,
		ID_SEPARATOR,
	ID_ALIGN_TOP,
	ID_ALIGN_MIDDLE,
	ID_ALIGN_BOTTOM,
		ID_SEPARATOR,
	IDC_TEXT_STRETCHING
};

void CFormatBar::ShowTextEdit(void)
{
	SetFormat(TextEditIds, sizeof(TextEditIds)/sizeof(TextEditIds[0]));
}

/*
// Show the warp text format bar.
*/

static UINT BASED_CODE WarpTextIds[] =
{
	IDC_TEXT_TYPEFACE,         // for typeface box (placeholder)
		ID_SEPARATOR,
	ID_TEXT_BOLD,
	ID_TEXT_ITALIC,
		ID_SEPARATOR,
	ID_ALIGN_LEFT,
	ID_ALIGN_CENTER,
	ID_ALIGN_RIGHT,
		ID_SEPARATOR,
	ID_EDIT_UNDO,
	ID_EDIT_REDO,
		ID_SEPARATOR,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE
};

void CFormatBar::ShowWarpText(void)
{
	SetFormat(WarpTextIds, sizeof(WarpTextIds)/sizeof(WarpTextIds[0]));
}

/*
// Show a blank format bar.
*/

static UINT BASED_CODE BlankIds[] =
{
#ifdef WIN32
	ID_FILE_FREE_FONTS			// We need something, I guess...
#else
	ID_SEPARATOR
#endif
};

void CFormatBar::ShowBlank(void)
{
	SetFormat(BlankIds, sizeof(BlankIds)/sizeof(BlankIds[0]));
}

/*
// Show the picture bar.
*/

static UINT BASED_CODE PictureIds[] =
{
	ID_POSITION_STANDARD,
		ID_SEPARATOR,
	ID_POSITION_CENTERONPAGE,
	ID_POSITION_HCENTER,
	ID_POSITION_VCENTER,
	ID_POSITION_FULLPAGE,
		ID_SEPARATOR,
	ID_XFLIP,
	ID_YFLIP,
		ID_SEPARATOR,
	IDC_ROTATE
};

void CFormatBar::ShowPicture(void)
{
	SetFormat(PictureIds, sizeof(PictureIds)/sizeof(PictureIds[0]));
}

/*
// Show the picture bar without rotation.
*/

static UINT BASED_CODE PictureNoRotateIds[] =
{
	ID_POSITION_STANDARD,
		ID_SEPARATOR,
	ID_POSITION_CENTERONPAGE,
	ID_POSITION_HCENTER,
	ID_POSITION_VCENTER,
	ID_POSITION_FULLPAGE,
		ID_SEPARATOR,
	ID_XFLIP,
	ID_YFLIP
};

void CFormatBar::ShowPictureNoRotate(void)
{
	SetFormat(PictureNoRotateIds, sizeof(PictureNoRotateIds)/sizeof(PictureNoRotateIds[0]));
}

/*
// Show the text box bar.
*/

static UINT BASED_CODE TextBoxIds[] =
{
	ID_TEXT_EDIT_FRAME,
		ID_SEPARATOR,
	ID_POSITION_STANDARD,
		ID_SEPARATOR,
	ID_POSITION_CENTERONPAGE,
	ID_POSITION_HCENTER,
	ID_POSITION_VCENTER,
	ID_POSITION_FULLPAGE,
		ID_SEPARATOR,
	ID_XFLIP,
	ID_YFLIP
};

void CFormatBar::ShowTextBox(void)
{
	SetFormat(TextBoxIds, sizeof(TextBoxIds)/sizeof(TextBoxIds[0]));
}

/*
// Show the title text box bar.
*/

static UINT BASED_CODE TitleTextBoxIds[] =
{
	ID_TEXT_EDIT_FRAME,
		ID_SEPARATOR,
	ID_POSITION_STANDARD,
		ID_SEPARATOR,
	ID_POSITION_CENTERONPAGE,
	ID_POSITION_HCENTER,
	ID_POSITION_VCENTER,
	ID_POSITION_FULLPAGE,
		ID_SEPARATOR,
	ID_XFLIP,
	ID_YFLIP,
		ID_SEPARATOR,
	IDC_ROTATE
};

void CFormatBar::ShowTitleTextBox(void)
{
	SetFormat(TitleTextBoxIds, sizeof(TitleTextBoxIds)/sizeof(TitleTextBoxIds[0]));
}

/*
// Show the group bar.
*/

static UINT BASED_CODE GroupIds[] =
{
	ID_EDIT_UNGROUP,
		ID_SEPARATOR,
	ID_POSITION_STANDARD,
		ID_SEPARATOR,
	ID_POSITION_CENTERONPAGE,
	ID_POSITION_HCENTER,
	ID_POSITION_VCENTER,
	ID_POSITION_FULLPAGE,
		ID_SEPARATOR,
	ID_XFLIP,
	ID_YFLIP
};

void CFormatBar::ShowGroup(void)
{
	SetFormat(GroupIds, sizeof(GroupIds)/sizeof(GroupIds[0]));
}

/*
// Show the shape bar.
*/

static UINT BASED_CODE ShapeIds[] =
{
	ID_POSITION_STANDARD,
		ID_SEPARATOR,
	ID_POSITION_CENTERONPAGE,
	ID_POSITION_HCENTER,
	ID_POSITION_VCENTER,
	ID_POSITION_FULLPAGE,
		ID_SEPARATOR,
	ID_XFLIP,
	ID_YFLIP,
		ID_SEPARATOR,
	IDC_ROTATE
};

void CFormatBar::ShowShape(void)
{
	SetFormat(ShapeIds, sizeof(ShapeIds)/sizeof(ShapeIds[0]));
}

/*
// Show the multiple bar.
*/

static UINT BASED_CODE MultipleIds[] =
{
	ID_EDIT_GROUP,
		ID_SEPARATOR,
	ID_POSITION_STANDARD,
		ID_SEPARATOR,
	ID_POSITION_CENTERONPAGE,
	ID_POSITION_HCENTER,
	ID_POSITION_VCENTER,
	ID_POSITION_FULLPAGE,
		ID_SEPARATOR,
	ID_XFLIP,
	ID_YFLIP
};

void CFormatBar::ShowMultiple(void)
{
	SetFormat(MultipleIds, sizeof(MultipleIds)/sizeof(MultipleIds[0]));
}

/*
// Show the calendar date bar.
*/

static UINT BASED_CODE CalendarDateIds[] =
{
	ID_CALENDAR_TEXT,
	ID_CALENDAR_PICTURE
};

void CFormatBar::ShowCalendarDate(void)
{
	SetFormat(CalendarDateIds, sizeof(CalendarDateIds)/sizeof(CalendarDateIds[0]));
}

int CFormatBar::GetDefaultHeight(void)
{
	CRect rectSize;
	rectSize.SetRectEmpty();
#ifdef WIN32
	CalcInsideRect(rectSize, TRUE);       // will be negative size
#else
	CalcInsideRect(rectSize);       // will be negative size
#endif
	return m_sizeButton.cy - rectSize.Height();
}

/////////////////////////////////////////////////////////////////////////////
// CDialogFormatBar
//
// Use this class when you want to use a CFormatBar in a dialog. It overrides
// the OnIdleUpdateCmdUI method to route the update stuff to the format bar
// parent rather than the main frame.

CDialogFormatBar::CDialogFormatBar()
{
	m_fInDialog = TRUE;
}
