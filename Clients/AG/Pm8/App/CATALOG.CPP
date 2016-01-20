//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CATALOG.CPP 1     3/03/99 6:03p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/CATALOG.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 6     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     4/17/98 6:09p Hforman
// finishing up Win95-style open dialog
// 
//////////////////////////////////////////////////////////////////////////////
//
// PMWTODO: .pmo graphics are not printing!
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwprint.h"

#include "objlist.h"
#include "catalog.h"

#include "system.h"
#include "utils.h"
#include "textflow.h"
#include "ctxp.h"

#include "pararec.h"
#include "framerec.h"
#include "frameobj.h"
#include "grafobj.h"
#include "patobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#define CATALOG_BACKGROUND

/////////////////////////////////////////////////////////////////////////////
// CPictureCatalogDialog dialog

CPictureCatalogDialog::CPictureCatalogDialog(CPmwDoc *pDoc, CWnd* pParent, LPCSTR filters)
	: CCustomOpenDlg(FALSE,	// show graphics
						  NULL, NULL,
 						  filters,
						  IDD,
						  OFN_ALLOWMULTISELECT,
 						  pParent)
{
	m_ofn.lpstrTitle = "Print Pictures";
	m_pDoc = pDoc;

	//{{AFX_DATA_INIT(CPictureCatalogDialog)
	//}}AFX_DATA_INIT
}

CPictureCatalogDialog::~CPictureCatalogDialog()
{
}

void CPictureCatalogDialog::DoDataExchange(CDataExchange* pDX)
{
	CCustomOpenDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPictureCatalogDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPictureCatalogDialog, CCustomOpenDlg)
	//{{AFX_MSG_MAP(CPictureCatalogDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureCatalogDialog message handlers

BOOL CPictureCatalogDialog::OnInitDialog() 
{
	CCustomOpenDlg::OnInitDialog();

	SetControlText(1, "Print");
	return TRUE;  // return TRUE unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CPictureCatalogDialog implementation handlers

void CPictureCatalogDialog::do_print()
{
	// Set our title so that the user will know what is being printed.
   m_pDoc->SetTitle(GET_PMWAPP()->GetResourceStringPointer (IDS_PICTURES));

	CPrintInfo pInfo;

	CPrintDialog *dlg_save = pInfo.m_pPD;

	CCatalogPrint pdlg(m_pDoc);

	pInfo.m_pPD = &pdlg;

	// Bring over the list.
	if (AfxGetApp()->DoPrintDialog(&pdlg) == IDOK)
	{
		// get all the filenames they selected
		POSITION pos = GetStartPosition();
		CStringArray files;
		while (pos != NULL)
		{
			CString file = GetNextPathName(pos);
			files.Add(file);
		}

		CString strDir = GetPathName();
		CCatalogView view(files, strDir);

		m_pDoc->m_bAutoDelete = FALSE;  // don't destroy document while closing views
		m_pDoc->size_to_paper();

		// Hook the view into the document.
		((CCatalogDoc *)m_pDoc)->add_hidden_view(&view);

		// Print the project.
		view.PrintProject(&pInfo);

		// Remove the view before its destructor happens.
		m_pDoc->RemoveView(&view);
	}

	pInfo.m_pPD = dlg_save;
}

SHORT CCatalogView::OnPrintDocument(PrintContext &pc,
												int32 aCurrentDocument, int32 aNumDocuments)
{
	CPmwDoc *pDoc = GetDocument();

	SHORT bError = FALSE;

	SHORT x, y;
	PCOORD grid_width, grid_height, step_width, step_height, text_height, new_x;
	PBOX pbox;
	int32 current_page, num_pages;
#ifdef SPECIAL_CATALOG
	PCOORD x0 = 0, y0 =  (PAGE_RESOLUTION*3)/4;
#endif

	CCatalogPrint *pdlg = (CCatalogPrint *)pc.m_pPD;

#ifdef OLD_CATALOG_STYLE
	CString csNameBuffer;
#endif

/* Compute the size of the grid and the step. */

	PPNT dims = pDoc->get_dimensions();
	PMGDatabasePtr const database = pDoc->get_database();
	CTextStyle Style(database);

#ifdef CATALOG_BACKGROUND
	PATTERN_CREATE_STRUCT pcs;

	pcs.bound.x0 = 0;
	pcs.bound.y0 = 0;
	pcs.bound.x1 = dims.x;
	pcs.bound.y1 = dims.y;
	pcs.pattern = 0;
	pcs.color = COLOR_WHITE;
	pcs.bcolor = MAKE_COLOR(0, 0, 0, 32);
#endif


#ifdef SPECIAL_CATALOG
	dims.x -= x0;
	dims.y -= y0;

	SHORT const across = 10;
	SHORT const down = 5;
#else
	SHORT const across = pdlg->m_x_count;
	SHORT const down = pdlg->m_y_count;
#endif

	grid_width = dims.x/across;
	grid_height = dims.y/down;

	step_width = scale_pcoord(grid_width, 9, 10);

	Style.SetDefault();
	SHORT base_size;

#ifdef DOUBLE_NAMES
	step_height = scale_pcoord(grid_height, 7, 10);
	text_height = (grid_height - step_height)/2;
	base_size = (SHORT)scale_pcoord(text_height, (72*6)/10, PAGE_RESOLUTION);
#else
	step_height = scale_pcoord(grid_height, 9, 10);
	text_height = grid_height - step_height;
	base_size = (SHORT)scale_pcoord(text_height, (72*2)/3, PAGE_RESOLUTION);
#endif

/* Set the text style. */

	if (base_size > 12)
	{
		base_size = 12;
	}

	Style.Size(MakeFixed(base_size));
	Style.BaseSize(MakeFixed(base_size));
#ifdef DOUBLE_NAMES
	Style.Bold(TRUE);
#endif

	Style.UpdateFontMetrics();

/* Figure out number of pages we will print. */
	num_pages = 
#ifdef SPACE_ON_FIRST
					across +
#endif
					m_picture_list.GetSize();
	{
		SHORT n;

		n = across * down;

		num_pages = (num_pages + n - 1) / n;
	}
	current_page = 0;

/* Run through all the graphics. */

	ObjectListPtr list = pDoc->object_list();

	int nMaxNames = m_picture_list.GetSize();
	BOOL broken;
	int nEntry;
	for (broken = FALSE, nEntry = 0; !broken && nEntry < nMaxNames ;)
	{
		// Tell the user what we're doing.
      printer_update_name(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_BUILDING_PAGE), (VOIDPTR)&pc);

		// Make sure the current page is blank.
		list->destroy_objects();

		// Build the next page.
		BeginWaitCursor();

#ifdef CATALOG_BACKGROUND
		PatternObjectPtr patobj;
		if ((patobj = database->create_pattern_object(&pcs)) != NULL)
		{
			pDoc->append_object(patobj);
		}
#endif

#ifdef SPECIAL_CATALOG
		for (pbox.y0 = y0, y = 0;
#else
		for (pbox.y0 = 0, y = 0;
#endif
						y < down && !broken;
						y++, pbox.y0 += grid_height)
		{
#ifdef SPACE_ON_FIRST
			if (current_page == 0 && y == 0)
			{
				continue;
			}
#endif

			pbox.y1 = pbox.y0 + step_height;

#ifdef SPECIAL_CATALOG
			for (pbox.x0 = x0, x = 0;
#else
			for (pbox.x0 = 0, x = 0;
#endif
					x < across;
					x++,
 					pbox.x0 += grid_width,
					nEntry++)
			{
				CFrameObject* pFrame;
				PPNT p;

				// Make sure we are on a good node.
				if (nEntry == nMaxNames)
				{
				/* All done! */
					y = down;
					break;
				}

				CString strName = m_picture_list.GetAt(nEntry);

			/* Show the name. */

//				multi_context.current_name = node->name;

			/* We have a selected node. Make a graphic for it. */

				pbox.x1 = pbox.x0 + step_width;

			/* Create the graphic object. */

#ifdef OLD_CATALOG_STYLE
				csNameBuffer = m_chosen_dir;
				csNameBuffer += strName;
#endif

				GraphicObjectPtr object;

				GRAPHIC_CREATE_STRUCT gcs;
#ifdef OLD_CATALOG_STYLE
				gcs.m_csFileName = csNameBuffer;
#else
				gcs.m_csFileName = strName;
#endif

				if ((object = database->create_graphic_object(&gcs)) != NULL)
				{
				/* Get the size of the graphic. */

					if (object->original_dims(&p))
					{
						pDoc->append_object(object);

						dims.x = (PCOORD)step_width;
						dims.y = (PCOORD)step_height;

					/* Fit the graphic to its box. */

						if ((new_x = scale_pcoord(dims.y, p.x, p.y)) > dims.x)
						{
							dims.y = scale_pcoord(dims.x, p.y, p.x);
						}
						else
						{
							dims.x = new_x;
						}

					/* Set the bound. */

						PBOX bound;

						bound.x0 = (pbox.x0 + pbox.x1 - dims.x)/2;
						bound.x1 = bound.x0 + dims.x;
						bound.y0 = (pbox.y0 + pbox.y1 - dims.y)/2;
						bound.y1 = bound.y0 + dims.y;
						object->set_bound(bound);

						object->calc();
					}
					else
					{
						delete object;
						object = NULL;
					}
				}

			/* Add a text frame for the name. */

				FRAMEOBJ_CREATE_STRUCT fcs;
				LPCSTR text = strName;

				fcs.bound.x0 = pbox.x0;
				fcs.bound.x1 = pbox.x1;
				fcs.bound.y0 = pbox.y1;

				fcs.alignment = ALIGN_center;
				fcs.vert_alignment = ALIGN_top;
//				fcs.text = NULL;
				fcs.flags = 0;
				fcs.pStyle = NULL;

#ifdef SPECIAL_CATALOG
				CHAR buffer[100];

				LPCSTR ptr;

				if ((ptr = strrchr(text, '\\')) == NULL)
				{
					ptr = text;
				}
				else
				{
				/* Move off of '\'. */
					ptr++;
				}

				strcpy(buffer, ptr);
				LPSTR foop;
				if ((foop = strrchr(buffer, '.')) != NULL)
				{
				/* Remove extension. */
					*foop = '\0';
				}

				fcs.bound.y1 = fcs.bound.y0+text_height;
				text = buffer;

#elif defined(DOUBLE_NAMES)

				fcs.bound.y1 = fcs.bound.y0+text_height*2;

				CHAR name[64];
				CHAR buffer[100];
				LPCSTR ptr;

				if ((ptr = strrchr(text, '\\')) == NULL)
				{
					ptr = text;
				}
				else
				{
				/* Move off of '\'. */
					ptr++;
				}

				BOOL blanket = FALSE;				/* porpoise-ly  (just for pun) */

				if (object == NULL)
				{
					blanket = TRUE;
				}
				else
				{
					object->get_name(name, sizeof(name), -1);
					if (stricmp(name, ptr) == 0)
					{
						blanket = TRUE;
						strcpy(name, " ");
					}
				}

				if (blanket)
				{
				/* Blank it. */
					strcpy(name, " ");
				}

//				sprintf(buffer, "%s\n%s", name, p);
				strcpy(buffer, name);
				strcat(buffer, "\n");
				strcat(buffer, ptr);

				text = buffer;
#else
				fcs.bound.y1 = fcs.bound.y0+text_height;
#endif
				if ((pFrame = pDoc->create_frame_object(&fcs)) == NULL)
				{
					broken = TRUE;
					break;
				}

				int nCharacters = strlen(text);
				if (nCharacters != 0)
				{
					CTxp Txp(database);
					Txp.Init(pFrame, 0);
					Txp.SetHorizontalAlignment(fcs.alignment);
					Txp.Style(Style);

					TRY
					{
						CHARACTER* pText = new CHARACTER[nCharacters];
						for (int n = 0; n < nCharacters; n++)
						{
							pText[n] = (CHARACTER)text[n];
						}
#ifdef DOUBLE_NAMES
						// First line is name + CR...
						int nLine1 = strlen(name)+1;
						// ...second line is the rest.
						int nLine2 = nCharacters - nLine1;

						// Insert line 1.
						Txp.InsertText(pText, nLine1);

						// Compute style for line 2:
						// no bold, and 8/10 the size.
						Txp.Style().Bold(FALSE);
						CFixed lSize = Txp.Style().BaseSize();
						lSize = MulDivFixed(lSize, MakeFixed(8), MakeFixed(10));
						Txp.Style().BaseSize(lSize);
						Txp.Style().Size(lSize);
						Txp.Style().UpdateFontMetrics();

						// Insert line 2.
						Txp.InsertText(pText+nLine1, nLine2);
#else
						Txp.InsertText(pText, nCharacters);
#endif
						delete [] pText;
					}
					END_TRY
				}

				pDoc->append_object(pFrame);
			}
		}

		EndWaitCursor();

	/* See if we broke! */

		if (broken)
		{
			break;
		}

	/* Print the page. */

		if (list->count() != 0)
		{
		/* We have objects! Print them. */
			++current_page;

			if (bError = CPmwView::OnPrintDocument(pc, current_page, num_pages))
			{
				break;
			}
		}
	}

	return broken ? -1 : bError;
}

/////////////////////////////////////////////////////////////////////////////
// CCatalogPrint dialog

CCatalogPrint::CCatalogPrint(CPmwDoc *pDoc)
	: CPmwPrint(NULL, pDoc, CCatalogPrint::IDD)
{
	//{{AFX_DATA_INIT(CCatalogPrint)
	m_per_page_index = 0;
	m_x_count = 1;
	m_y_count = 1;
	//}}AFX_DATA_INIT
}

void CCatalogPrint::DoDataExchange(CDataExchange* pDX)
{
	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCatalogPrint)
	DDX_Radio(pDX, IDC_CATALOG_1x2, m_per_page_index);
	DDX_Text(pDX, IDC_CATALOG_X, m_x_count);
	DDV_MinMaxInt(pDX, m_x_count, 1, 8);
	DDX_Text(pDX, IDC_CATALOG_Y, m_y_count);
	DDV_MinMaxInt(pDX, m_y_count, 1, 8);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCatalogPrint, CPmwPrint)
	//{{AFX_MSG_MAP(CCatalogPrint)
	ON_BN_CLICKED(IDC_CATALOG_CUSTOM, OnClickedCatalogCustom)
	ON_EN_CHANGE(IDC_CATALOG_X, OnChangeCustom)
	ON_BN_CLICKED(IDC_CATALOG_1x2, OnClickedPerPage)
	ON_EN_CHANGE(IDC_CATALOG_Y, OnChangeCustom)
	ON_BN_CLICKED(IDC_CATALOG_2x2, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_2x3, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_3x3, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_3x4, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_4x5, OnClickedPerPage)
	ON_BN_CLICKED(IDC_CATALOG_6x8, OnClickedPerPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCatalogPrint message handlers

void CCatalogPrint::OnClickedCatalogCustom()
{
	GotoDlgCtrl(GetDlgItem(IDC_CATALOG_X));
}

void CCatalogPrint::OnChangeCustom()
{
	if (GetCheckedRadioButton(IDC_CATALOG_1x2, IDC_CATALOG_CUSTOM) != IDC_CATALOG_CUSTOM)
	{
		CWnd *focused = GetFocus();

		if (focused != NULL)
		{
			int id = focused->GetDlgCtrlID();

			if (id == IDC_CATALOG_X || id == IDC_CATALOG_Y)
			{
				CheckRadioButton(IDC_CATALOG_1x2, IDC_CATALOG_CUSTOM, IDC_CATALOG_CUSTOM);
			}
		}
	}
}

void CCatalogPrint::update_custom_values()
{
	int checked = GetCheckedRadioButton(IDC_CATALOG_1x2, IDC_CATALOG_CUSTOM);

	if (checked != 0)
	{
		char buffer[64];

		GetDlgItemText(checked, buffer, sizeof(buffer)-1);
		char *p;

		if ((p = strchr(buffer, '(')) != NULL)
		{
			int x, y;

			if (sscanf(p, "(%dx%d)", &x, &y) == 2)
			{
				SetDlgItemInt(IDC_CATALOG_X, x);
				SetDlgItemInt(IDC_CATALOG_Y, y);
			}
		}
	}
}

void CCatalogPrint::OnClickedPerPage()
{
	update_custom_values();
}

BOOL CCatalogPrint::OnInitDialog()
{
	CPmwPrint::OnInitDialog();

	CEdit *edit;

	if ((edit = (CEdit *)GetDlgItem(IDC_CATALOG_X)) != NULL)
	{
		edit->LimitText(1);
	}
	if ((edit = (CEdit *)GetDlgItem(IDC_CATALOG_Y)) != NULL)
	{
		edit->LimitText(1);
	}

	update_custom_values();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CCatalogView

CCatalogView::CCatalogView(const CStringArray& picture_list, LPCSTR chosen_dir)
	: m_picture_list(picture_list)
{
	m_chosen_dir = chosen_dir;
}

CCatalogView::~CCatalogView()
{
}


BEGIN_MESSAGE_MAP(CCatalogView, CPmwView)
	//{{AFX_MSG_MAP(CCatalogView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCatalogView drawing

#if 0
void CCatalogView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CCatalogView message handlers
/////////////////////////////////////////////////////////////////////////////
// CCatalogDoc

IMPLEMENT_SERIAL(CCatalogDoc, CPmwDoc, 0 /* schema number*/ )

CCatalogDoc::CCatalogDoc()
{
}

CCatalogDoc::~CCatalogDoc()
{
}

BEGIN_MESSAGE_MAP(CCatalogDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CCatalogDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

VOID CCatalogDoc::add_hidden_view(CCatalogView *pView)
{
	m_viewList.AddTail(pView);
	pView->set_document(this);
}

/////////////////////////////////////////////////////////////////////////////
// CCatalogDoc commands
