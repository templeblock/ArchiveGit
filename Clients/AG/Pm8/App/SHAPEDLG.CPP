/*
// $Workfile: SHAPEDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SHAPEDLG.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:25:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:24   Fred
// Initial revision.
// 
//    Rev 1.3   21 Jul 1997 16:30:56   dennisp
// Fixed m_pFile ASSERT() in wrong place.
// 
//    Rev 1.2   28 Jan 1997 14:10:34   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.1   22 Aug 1996 08:52:28   Jay
// Sets initial index.
// 
//    Rev 1.0   14 Mar 1996 13:34:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:50   FRED
// Initial revision.
// 
//    Rev 1.3   06 Jul 1995 17:26:42   JAY
// New binary caching technology
// 
//    Rev 1.2   18 May 1995 13:20:46   FRED
// Shape file/dialog stuff
// 
//    Rev 1.1   16 May 1995 11:59:58   JAY
// 
//    Rev 1.0   10 May 1995 15:17:44   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "shapedlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShapeDialog dialog

CShapeDialog::CShapeDialog(CShapeFile* pFile, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CShapeDialog::IDD, pParent)
{
	m_pFile = pFile;
	ASSERT(m_pFile != NULL);
	//{{AFX_DATA_INIT(CShapeDialog)
	m_nChosen = 0;
	//}}AFX_DATA_INIT
}

void CShapeDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShapeDialog)
	DDX_Control(pDX, IDC_SHAPES, m_List);
	DDX_LBIndex(pDX, IDC_SHAPES, m_nChosen);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShapeDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CShapeDialog)
	ON_LBN_DBLCLK(IDC_SHAPES, OnDblclkShapes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShapeDialog message handlers

BOOL CShapeDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();

	BuildShapeList();

	m_List.SetCurSel(m_nChosen);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CShapeDialog::BuildShapeList(void)
{
	// initialize the shape list box
	m_List.Shapes(m_pFile);
	m_List.SizeItems();
}

void CShapeDialog::OnDblclkShapes()
{
	OnOK();	
}
