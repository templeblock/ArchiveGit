// pictattr.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pictattr.h"

#include "grafrec.h"
#include "grafobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPictureAttrDlg dialog

CPictureAttrDlg::CPictureAttrDlg(GraphicObjectPtr object, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CPictureAttrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPictureAttrDlg)
	m_coloring_book = FALSE;
	m_render_method = -1;
	//}}AFX_DATA_INIT
	m_object = object;
}

void CPictureAttrDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPictureAttrDlg)
	DDX_Check(pDX, IDC_COLORING_BOOK, m_coloring_book);
	DDX_Radio(pDX, IDC_ORDERED_DITHER, m_render_method);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		if (!m_Color.Color(&m_ChosenColor))
		{
			m_ChosenColor = COLOR_BLACK;
		}
	}
}

BEGIN_MESSAGE_MAP(CPictureAttrDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CPictureAttrDlg)
	ON_BN_CLICKED(IDC_COLOR_MORE, OnClickedColorMore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureAttrDlg message handlers

VOID CPictureAttrDlg::enable_window(int id, BOOL enable)
{
	HWND hWnd;

	if ((hWnd = ::GetDlgItem(GetSafeHwnd(), id)) != 0)
	{
		::EnableWindow(hWnd, enable);
	}
}

BOOL CPictureAttrDlg::OnInitDialog()
{
/* Get the object type to test whether outline or bitmap. */

	GraphicPtr graphic;
	PMGDatabasePtr database = m_object->get_database();

	if ((graphic = (GraphicPtr)database->get_record(m_object->get_graphic_record(),
						NULL,
						RECORD_TYPE_Graphic)) == NULL)
	{
		EndDialog(IDABORT);
		return TRUE;
	}

	switch (graphic->record.type)
	{
		case GRAPHIC_TYPE_CGM:
		case GRAPHIC_TYPE_WPG:
		case GRAPHIC_TYPE_WMF:
		{
			m_outline = TRUE;
			break;
		}
		default:
		{
			m_outline = FALSE;
			break;
		}
	}
	graphic->release();

	if (m_outline)
	{
		enable_window(IDC_IMAGE_GROUP, FALSE);
		enable_window(IDC_ORDERED_DITHER, FALSE);
		enable_window(IDC_ERROR_DIFFUSION, FALSE);
		m_coloring_book = !!(m_object->get_graphic_flags() & GRAPHIC_FLAG_coloring_book);
	}
	else
	{
		enable_window(IDC_COLORING_BOOK, FALSE);
		m_render_method = m_object->get_render_method();
	}

	m_ChosenColor = m_object->get_bcolor();
	m_Color.SubclassDlgItem(IDC_COLOR, this);
	m_Color.SizeItems();
	// JRO m_Color.Colors(NULL, "Original Colors");
   m_Color.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_ORG_COLORS));
	m_Color.Color(m_ChosenColor, m_ChosenColor != COLOR_BLACK);

	GET_PMWAPP()->ShowHelpfulHint("Picture attributes dialog", this);

	return CPmwDialog::OnInitDialog();
}

void CPictureAttrDlg::OnClickedColorMore()
{
	m_Color.ChooseCustomColor();
}

