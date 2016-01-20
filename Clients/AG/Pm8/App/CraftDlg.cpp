//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CraftDlg.cpp 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/CraftDlg.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 8     10/22/98 12:39p Mwilson
// added code to handle not finding image in collection
// 
// 7     9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 6     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 5     8/27/98 5:22p Hforman
// 
// 4     8/26/98 8:12p Hforman
// added code for getting thumbnails out of project collection
// 
// 3     8/20/98 12:51p Hforman
// added m_CraftType, changed contents of CrTypes.ini
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "util.h"
#include "utils.h"
#include "pmwcoll.h"
#include "browser.h"
#include "CraftDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCraftTypeDlg dialog


CCraftTypeDlg::CCraftTypeDlg(CWnd* pParent /*=NULL*/, DWORD WizFlags, COLORREF cr /*INVALID_DIALOG_COLOR*/)
	: CPmwDialogColor(CCraftTypeDlg::IDD, pParent, cr)
{
	//{{AFX_DATA_INIT(CCraftTypeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_WizFlags = WizFlags;
	m_CraftType = CRAFT_TYPE_Generic;
	m_pThumbnailData = NULL;
}

CCraftTypeDlg::~CCraftTypeDlg()
{
	if (m_pThumbnailData != NULL)
	{
		Util::HugeFree(m_pThumbnailData);
		m_pThumbnailData = NULL;
	}
}

void CCraftTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCraftTypeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCraftTypeDlg, CPmwDialogColor)
	//{{AFX_MSG_MAP(CCraftTypeDlg)
	ON_LBN_SELCHANGE(IDC_CRAFT_LIST, OnSelchangeCraftList)
	ON_WM_DRAWITEM()
	ON_LBN_DBLCLK(IDC_CRAFT_LIST, OnDblclkCraftList)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCraftTypeDlg message handlers

// TODO: move this to central location
int GetCraftTypeFromStr(const CString& strCraft)
{
	CMlsStringArray mlsCrafts(IDS_CRAFT_TYPES);
	int nType = mlsCrafts.FindName(strCraft, CMlsStringArray::IgnoreCase);

	return nType;
}

void CCraftTypeDlg::BuildList(void)
{
	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_CRAFT_LIST)) != NULL)
	{
		int nNewSel = 0;
		char sectionNames[512];
		GetPrivateProfileSectionNames(sectionNames, 512, m_csIniFile);

		char* pSection = sectionNames;
		while(strlen(pSection) != 0)
		{
			int index = pList->AddString(pSection);
			if (index != LB_ERR)
			{
				CString csType = m_iniFile.GetString(pSection, "CraftType");
				int type = GetCraftTypeFromStr(csType);
				pList->SetItemData(index, type);
			}

			pSection += strlen(pSection) + 1;
		}
		// Always start with the first craft.
		pList->SetCurSel(nNewSel);
		pList->SetRedraw(TRUE);
		OnSelchangeCraftList();
	}
}

void CCraftTypeDlg::OnOK()
{
	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_CRAFT_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			CString csSection;
			pList->GetText(nCurSel, csSection);
			m_csSelectedProject = m_iniFile.GetString(csSection, "FileName");
			int nType = pList->GetItemData(nCurSel);
			m_CraftType = (CRAFT_TYPE)nType;
		}
	}
	CPmwDialogColor::OnOK();
}


BOOL CCraftTypeDlg::OnInitDialog()
{
	CPmwDialogColor::OnInitDialog();

	m_preview.Initialize(this, IDC_PREVIEW, IDC_PREVIEW_DESCRIPTION);
	m_csIniFile = GetGlobalPathManager()->ExpandPath("[[S]]\\CrTypes.INI");
	m_iniFile.Name(m_csIniFile);

	// Save initial size of Preview window
	CWnd* pWnd = GetDlgItem(IDC_PREVIEW);
	if (pWnd != NULL)
	{
		CRect rect;
		pWnd->GetClientRect(&rect);
		m_nPreviewWidth = rect.Width() - 2;
		m_nPreviewHeight = rect.Height() - 2;
	}

	// Build the list box.
	BuildList();

   CreateWizzardButtons ();
   EnableWizardButtons (m_WizFlags);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CCraftTypeDlg::OnSelchangeCraftList()
{

	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_CRAFT_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			CString csSection;
			pList->GetText(nCurSel, csSection);

			CString csProject = m_iniFile.GetString(csSection, "FileName");

			// Find the collection
			CString csName;
			CString csCollection;
			GetGlobalPathManager()->BindPath(csProject, csName, &csCollection);
			CPMWCollection* pCollection = GetGlobalCollectionManager()->FindCollection(csCollection);
			if (pCollection)
			{
				// Get item number in collection.
				CString csPath, csFilename;
				Util::SplitPath(csProject, &csPath, &csFilename);
				DWORD dwItem;
				if (pCollection->FindItem(csFilename, &dwItem) == ERRORCODE_None)
				{
					CFileThumbnails* pFileThumbnails = NULL;
					if (pCollection->NewThumbnailStream(&pFileThumbnails) == ERRORCODE_None)
					{
						CThumbnailDataEntry Thumbnail;
						if (pFileThumbnails->Find(dwItem, &Thumbnail) == ERRORCODE_None)
						{
							DWORD dwDataSize = Thumbnail.GetItemSize();
							void* pData = NULL;
							if ((pData = Util::HugeAlloc(dwDataSize)) != NULL)
							{
								Thumbnail.Attach(pData, dwDataSize);

								if (pFileThumbnails->Read(dwDataSize) == ERRORCODE_None)
								{
									CSize size;
									GetFinalThumbnailSize(Thumbnail.m_DataHeader.GetWidth(),
																 Thumbnail.m_DataHeader.GetHeight(),
																 &size);

									CRect crThumbnail(0, 0, size.cx, size.cy);
									BOOL bTransparent;
									if (m_pThumbnailData != NULL)
									{
										Util::HugeFree(m_pThumbnailData);
										m_pThumbnailData = NULL;
									}
									m_pThumbnailData = (LPBITMAPINFO)
										CBrowserDialog::MakeThumbnail(pCollection->GetID(), dwItem,
																				crThumbnail, Thumbnail, pData, 
																				&bTransparent);
									if (m_pThumbnailData != NULL)
									{
										m_preview.NewGraphicPreview(NULL, NULL, m_pThumbnailData);
									}
								}

								Thumbnail.Detach();
								Util::HugeFree(pData);
							}
						}

						pCollection->ReleaseThumbnailStream(pFileThumbnails);
					}
				}
				else
				{
					AfxMessageBox(IDS_ERR_IMAGE_NOT_FOUND);
					return;
				}
			}
		}
	}

	UpdatePreview();
}

BOOL CCraftTypeDlg::GetFinalThumbnailSize(int nThumbWidth, int nThumbHeight, CSize* pSize)
{
	int nWidth;
	int nHeight = m_nPreviewHeight;
	if ((nWidth = scale_pcoord(nHeight, nThumbWidth, nThumbHeight)) > m_nPreviewWidth)
	{
		nWidth = m_nPreviewWidth;
		nHeight = scale_pcoord(nWidth, nThumbHeight, nThumbWidth);
	}

	pSize->cx = nWidth;
	pSize->cy = nHeight;
	return TRUE;
}


CString CCraftTypeDlg::GetChosenCraft(void)
{
	return m_csSelectedProject;
}

void CCraftTypeDlg::UpdatePreview(void)
{
	CWnd* pPreview = GetDlgItem(IDC_PREVIEW);
	if (pPreview != NULL)
	{
		pPreview->InvalidateRect(NULL);
	}
}

void CCraftTypeDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_PREVIEW:
		{
			m_preview.DrawPreview(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwDialogColor::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
		}
	}
}


void CCraftTypeDlg::OnDblclkCraftList()
{
	OnOK();
}

void CCraftTypeDlg::OnBack() 
{
   EndDialog (IDC_BACK);
}
