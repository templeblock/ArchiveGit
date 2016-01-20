// slpicdlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "slpicdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectPictureDlg dialog

CSelectPictureDlg::CSelectPictureDlg(CPmwDoc *pDoc, CWordArray* pcwaTypes, SHORT id, CWnd* pParent /*=NULL*/)
	: CPmwDialog(id, pParent)
{
	//{{AFX_DATA_INIT(CSelectPictureDlg)
	m_list_index = -1;
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_pcwaTypes = pcwaTypes;
}

void CSelectPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectPictureDlg)
	DDX_Control(pDX, IDC_PICTURE_LIST, m_picture_list);
	DDX_LBIndex(pDX, IDC_PICTURE_LIST, m_list_index);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectPictureDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CSelectPictureDlg)
	ON_BN_CLICKED(IDC_PICTURE_ADD, OnPictureAdd)
	ON_LBN_DBLCLK(IDC_PICTURE_LIST, OnDblclkPictureList)
	ON_LBN_SELCHANGE(IDC_PICTURE_LIST, OnSelchangePictureList)
	ON_BN_CLICKED(IDC_REMOVE_OBJECTS, OnClickedRemove)
	ON_BN_CLICKED(IDC_PICTURE_ADD_OTHER, OnPictureAddOther)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectPictureDlg message handlers

void CSelectPictureDlg::OnPictureAdd()
{
	EndDialog(IDC_PICTURE_ADD);
}

BOOL CSelectPictureDlg::IsShowableObject(PMGPageObject* pObject)
{
	if (pObject->get_panel() < m_pDoc->number_of_panels())
	{
		int nType = pObject->type();
		int nTypes = m_pcwaTypes->GetSize();
		for (int i = 0; i < nTypes; i++)
		{
			if (m_pcwaTypes->GetAt(i) == (WORD)nType)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CSelectPictureDlg::build_object_list()
{
/* Start off empty. */

	m_picture_list.ResetContent();

/* Run through the selected list. */

	PMGPageObjectPtr object;
	PMGPageObjectPtr next;

	for (object = m_pDoc->selected_objects(); object != NULL; object = next)
	{
		next = object->next_selected_object();
		
		if (!IsShowableObject(object))
		{
		/* Make sure it's not selected. */
			m_pDoc->deselect_object(object);
		}
	}

/* Run through the object list and build the listbox list. */

	BOOL something_selected = FALSE;

	for (object = (PMGPageObjectPtr)m_pDoc->objects();
					object != NULL;
					object = (PMGPageObjectPtr)object->next_object())
	{
		if (IsShowableObject(object))
		{
			CHAR buffer[64];

		/* We want to add this name to the list. */

			if (object->get_name(buffer, sizeof(buffer), -1))
			{
				m_picture_list.InsertString(0, buffer);
				if (m_pDoc->object_selected(object))
				{
				/* This is selected. */
					m_picture_list.SetSel(0, TRUE);
					something_selected = TRUE;
				}
			}
		}
	}

/* Update the remove button. */

	update_remove_button(something_selected);
}

BOOL CSelectPictureDlg::OnInitDialog()
{
	CPmwDialog::OnInitDialog();

	build_object_list();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSelectPictureDlg::OnDblclkPictureList()
{
	OnOK();
}

void CSelectPictureDlg::OnSelchangePictureList()
{
	objects_from_list();	
}

VOID CSelectPictureDlg::update_remove_button(BOOL something_selected)
{
	CWnd *remove_button = GetDlgItem(IDC_REMOVE_OBJECTS);
	if (remove_button != NULL)
	{
		remove_button->EnableWindow(something_selected);
	}
}

/*
// Set the selection state of the objects from the list.
*/

void CSelectPictureDlg::objects_from_list(void)
{
	SHORT i;
	PMGPageObjectPtr object;
	BOOL something_selected = FALSE;

	for (i = m_picture_list.GetCount(),
 				object = (PMGPageObjectPtr)m_pDoc->objects();
							object != NULL;
							object = (PMGPageObjectPtr)object->next_object())
	{
		if (IsShowableObject(object))
		{
			BOOL selected = m_pDoc->object_selected(object);

			i--;

			if (m_picture_list.GetSel(i))
			{
				if (!selected)
				{
					m_pDoc->select_object(object);
				}
				something_selected = TRUE;
			}
			else
			{
				if (selected)
				{
					m_pDoc->deselect_object(object);
				}
			}
		}
	}

/* Update the remove button. */

	update_remove_button(something_selected);
}

void CSelectPictureDlg::OnClickedRemove()
{
	m_pDoc->delete_selects();

	build_object_list();
}

void CSelectPictureDlg::OnCancel()
{
	m_pDoc->deselect_all();
	
	CPmwDialog::OnCancel();
}

void CSelectPictureDlg::OnPictureAddOther()
{
	EndDialog(IDC_PICTURE_ADD_OTHER);
}
