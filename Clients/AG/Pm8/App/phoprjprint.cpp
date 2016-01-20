/*
// $Workfile: phoprjprint.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/phoprjprint.cpp $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     1/30/99 11:32p Psasse
// Updated and fairly stable EasyPrints code
// 
//  
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "phoprjprint.h"

#include "macrostd.h"
#include "paper.h"
#include "namerec.h"
#include "phoprjprvw.h"
#include "lbldoc.h"
#include "easyprintsdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectPrintDialog dialog
CPhotoProjectPrintDialog::CPhotoProjectPrintDialog(CPmwView* pView)
	: CPmwPrint(pView, NULL, CPhotoProjectPrintDialog::IDD)
{
	//{{AFX_DATA_INIT(CPhotoProjectPrintDialog)
	m_fCollate = FALSE;
	m_fPageBreak = FALSE;
	m_nPhotoCopies = 0;
	//}}AFX_DATA_INIT

	// Set the number of photo copies to the full count unless
	// we have names chosen (in which case we default to 1 each).
	NameListRecord* pNameList = m_pDoc->GetMacroServer()->GetNameList();
	m_nPhotoCopies = (pNameList->Names() == 0)
							? m_pDoc->GetTotalSlots()
							: 1;

	m_nStartingPhoto = 0;
}

void CPhotoProjectPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhotoProjectPrintDialog)
	DDX_Check(pDX, IDC_COLLATE, m_fCollate);
	DDX_Check(pDX, IDC_PAGE_BREAK, m_fPageBreak);
	DDX_Text(pDX, IDC_LABEL_COPIES, m_nPhotoCopies);
	DDV_MinMaxInt(pDX, m_nPhotoCopies, 1, 99);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPhotoProjectPrintDialog, CPmwPrint)
	//{{AFX_MSG_MAP(CPhotoProjectPrintDialog)
	ON_BN_CLICKED(IDC_STARTING_LABEL, OnStartingPhoto)
	ON_EN_CHANGE(IDC_LABEL_COPIES, OnChangePhotoCopies)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectPrintDialog message handlers


BOOL CPhotoProjectPrintDialog::OnInitDialog()
{
	CPmwPrint::OnInitDialog();

	CButton* pButton = (CButton*)GetDlgItem(IDC_STARTING_LABEL);
	pButton->SetWindowText ("Starting Photo...");

	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_NUM_LABELS_STATIC);
	pStatic->SetWindowText ("Number of Photos:");

	UpdateStartingPhoto();
	UpdateCollate();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPhotoProjectPrintDialog::ComputeTotals(void)
{
	CPaperInfo* pPaperInfo = m_pDoc->GetPaperInfo();
	CStdMacroServer* pMacroServer = m_pDoc->GetMacroServer();
	NameListRecord* pNameList = pMacroServer->GetNameList();

	int nPhotosPerPage = ((CEasyPrintsDoc*)m_pDoc)->GetTotalSlots();

	
	int StartingPhoto = 0;
	for(int x = 0; x < m_nStartingSubSection; x++)
	{
		StartingPhoto += m_pDoc->GetPhotoProjectInfo()->m_pSubAreaData[x]->Slots();
	}

	StartingPhoto += (m_nStartingPhoto + 0);
	
	
	int nPhotosOnFirstPage = nPhotosPerPage - StartingPhoto;

	int nNames = pNameList->Names();
	if (nNames == 0)
	{
		nNames++;
	}

/*
// Compute the total number of photos.
*/

	m_nTotalPhotos = m_nPhotoCopies*nNames;

/*
// Compute the total number of pages.
*/

	int nPhotosPerSet;
	int nSets;

	if (nNames > 1
		 && m_nPhotoCopies > 1
		 && m_fPageBreak)
	{
	/*
	// We have a multi-name and multi-copy case with page breaks between sets.
	*/
		if (m_fCollate)
		{
		/*
		// Each set consists of one of each name.
		// There is one set printed for each copy requested.
		*/
			nPhotosPerSet = nNames;
			nSets = m_nPhotoCopies;
		}
		else
		{
		/*
		// Each set consists of all copies of one name.
		// There is one set per name requested.
		*/
			nPhotosPerSet = m_nPhotoCopies;
			nSets = nNames;
		}
	}
	else
	{
	/*
	// This is a case where all photos are contiguous on the pages.
	*/
		nPhotosPerSet = m_nTotalPhotos;
		nSets = 1;
	}

/*
// Compute the number of pages per set.
*/

	m_nTotalPages = 1;

	int nPhotosOnThisPage = nPhotosOnFirstPage;

	while (nPhotosPerSet > nPhotosOnThisPage)
	{
		m_nTotalPages++;
		nPhotosPerSet -= nPhotosOnThisPage;

		nPhotosOnThisPage = nPhotosPerPage;
	}

/*
// And multuply by the number of sets.
*/

	m_nTotalPages *= nSets;
}

void CPhotoProjectPrintDialog::OnStartingPhoto()
{
	CPhotoProjectStartDialog Dialog(m_pDoc->GetPhotoProjectInfo(), this);

	Dialog.m_nStartingPhoto = m_nStartingPhoto;
	Dialog.m_nStartingSubSection = m_nStartingSubSection;

	if (Dialog.DoModal() == IDOK)
	{
		m_nStartingPhoto = Dialog.m_nStartingPhoto;
		m_nStartingSubSection = Dialog.m_nStartingSubSection;
		UpdateStartingPhoto();
	}
}

void CPhotoProjectPrintDialog::UpdateStartingPhoto(void)
{
	char buffer[10];
	
	int StartingPhoto = 0;

	for(int x = 0; x < m_nStartingSubSection; x++)
	{
		StartingPhoto += m_pDoc->GetPhotoProjectInfo()->m_pSubAreaData[x]->Slots();
	}

	StartingPhoto += (m_nStartingPhoto + 1);
	
	sprintf(buffer, "%d", StartingPhoto);

	SetDlgItemText(IDC_STARTING_LABEL_CHOSEN, buffer);
}

void CPhotoProjectPrintDialog::UpdateCollate(void)
{
	BOOL fEnable = FALSE;
	CWnd* pChild = GetDlgItem(IDC_LABEL_COPIES);
	if (pChild != NULL)
	{
		CString csText;
		pChild->GetWindowText(csText);
		if (atoi(csText) > 1)
		{
			CStdMacroServer* pMacroServer = m_pDoc->GetMacroServer();
			NameListRecord* pNameList = pMacroServer->GetNameList();

			if (pNameList->Names() > 1)
			{
				fEnable = TRUE;
			}
		}
	}
	pChild = GetDlgItem(IDC_COLLATE);
	if (pChild != NULL)
	{
		pChild->EnableWindow(fEnable);
	}
	pChild = GetDlgItem(IDC_PAGE_BREAK);
	if (pChild != NULL)
	{
		pChild->EnableWindow(fEnable);
	}
}

void CPhotoProjectPrintDialog::update_names_chosen(void)
{
	CPmwPrint::update_names_chosen();
	UpdateCollate();

	// If the user has chosen names, we do not want to use the number
	// of photos in the sheet. Even though the user may have chosen that
	// number (rare), it is more likely that it was set by the code.
	// So we correct it.

	CWnd* pChild = GetDlgItem(IDC_LABEL_COPIES);
	if (pChild != NULL)
	{
		NameListRecord* pNameList = m_pDoc->GetMacroServer()->GetNameList();
		int nNames = pNameList->Names();
		int nSlots = m_pDoc->GetTotalSlots();

		CString csText;
		pChild->GetWindowText(csText);
		int nPhotos = atoi(csText);
		int nNewPhotos = nPhotos;
		if (nPhotos == nSlots && nNames > 0)
		{
			nNewPhotos = 1;
		}
		else if (nPhotos == 1 && nNames == 0)
		{
			nNewPhotos = nSlots;
		}

		if (nPhotos != nNewPhotos)
		{
			CString cs;
			cs.Format("%d", nNewPhotos);
			pChild->SetWindowText(cs);
		}
	}
}

void CPhotoProjectPrintDialog::OnChangePhotoCopies()
{
	UpdateCollate();
}

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectStartDialog dialog

CPhotoProjectStartDialog::CPhotoProjectStartDialog(CPhotoPrjData* pPhotoInfo, CWnd* pParent /*=NULL*/, int nID /*=IDD*/)
	: CPmwDialog(nID, pParent)
{
	//{{AFX_DATA_INIT(CPhotoProjectStartDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPhotoPrjData = pPhotoInfo;
	m_nStartingPhoto = 0;
	m_nStartingSubSection = 0;
}

void CPhotoProjectStartDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhotoProjectStartDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPhotoProjectStartDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CPhotoProjectStartDialog)
	ON_BN_DOUBLECLICKED(IDC_PREVIEW, OnDoubleclickedPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectStartDialog message handlers

BOOL CPhotoProjectStartDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	m_pPreview = (CPhotoProjectPreview*)GetDlgItem(IDC_PREVIEW);
	m_pPreview->WantSelect();
	m_pPreview->SetPhotoProject(m_pPhotoPrjData);
	m_pPreview->SelectPhotoProject(m_nStartingSubSection, m_nStartingPhoto);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPhotoProjectStartDialog::OnOK()
{
	m_nStartingPhoto = m_pPreview->SelectedPhoto();
	m_nStartingSubSection = m_pPreview->SelectedSubSection();
	CPmwDialog::OnOK();
}

void CPhotoProjectStartDialog::OnDoubleclickedPreview()
{
	OnOK();
}

