/*
// $Workfile: PMWCOLOR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWCOLOR.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:50   Fred
// Initial revision.
// 
//    Rev 1.3   10 Jun 1997 15:29:48   Jay
// New color dialog; saves custom colors.
// 
//    Rev 1.2   03 Apr 1997 14:31:22   Jay
// Resource cleanup
// 
//    Rev 1.1   28 Jul 1996 10:18:54   Jay
// Text color, new color dialog
*/

#include "stdafx.h"
#include "pmw.h"

#include "pmwcolor.h"
#include "pmwdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static char BASED_CODE SECTION_CustomColor[] = "CustomColor";
static char BASED_CODE cbColorEntryFormat[] = "Color%d";

/////////////////////////////////////////////////////////////////////////////
// CPmwColorDialog dialog

CPmwColorDialog::CPmwColorDialog(COLORREF Color, DWORD dwFlags /*=CC_FULLOPEN*/, CWnd* pParent /*=NULL*/)
	: CColorDialog(Color, dwFlags, pParent)
{
	//{{AFX_DATA_INIT(CPmwColorDialog)
	//}}AFX_DATA_INIT
}

void CPmwColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CColorDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPmwColorDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPmwColorDialog, CColorDialog)
	//{{AFX_MSG_MAP(CPmwColorDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
// DoModal for the color dialog.
*/

int CPmwColorDialog::DoModal(void)
{
	// Load the custom colors from the user INI.
	m_cc.lpCustColors = m_CustomColors;
	for (int nColor = 0; nColor < 16; nColor++)
	{
		CString csEntry;
		csEntry.Format(cbColorEntryFormat, nColor);
		m_CustomColors[nColor] = atol(GET_PMWAPP()->OurGetProfileString(SECTION_CustomColor, csEntry, ""));
	}

	// Run the dialog.
	int nResult = CColorDialog::DoModal();

	// If the user said OK, save the custom colors.
	if (nResult == IDOK)
	{
		// Save the custom colors from the user INI.
		for (int nColor = 0; nColor < 16; nColor++)
		{
			CString csEntry;
			csEntry.Format(cbColorEntryFormat, nColor);
			CString csValue;
			csValue.Format("%ld", m_CustomColors[nColor]);
			AfxGetApp()->WriteProfileString(SECTION_CustomColor, csEntry, csValue);
		}
	}
	return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwColorDialog message handlers

BOOL CPmwColorDialog::OnInitDialog()
{
	CColorDialog::OnInitDialog();

	standard_dialog_startup(this, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
