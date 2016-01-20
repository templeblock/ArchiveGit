/*
// $Workfile: WARPSHAP.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPSHAP.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:27:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:02   Fred
// Initial revision.
// 
//    Rev 1.3   17 Mar 1997 14:48:22   Jay
// Warp shape palette and support.
// 
//    Rev 1.2   16 Jan 1997 09:01:26   Fred
// Switch to warp field data record
// 
//    Rev 1.1   22 Aug 1996 08:52:18   Jay
// Initializes index when objects is assigned.
// 
//    Rev 1.0   14 Mar 1996 13:35:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:38   FRED
// Initial revision.
// 
//    Rev 1.9   04 Sep 1995 09:03:42   FRED
// Got rid of GetParent() error.
// 
//    Rev 1.8   03 Sep 1995 17:58:00   FRED
// Warp text dialog works with new tabbed dialog code
// 
//    Rev 1.7   02 Sep 1995 15:04:52   FRED
// More changes to make help work
// 
//    Rev 1.6   13 May 1995 11:10:08   FRED
// More warp preview stuffwarpshap.h
// 
//    Rev 1.5   12 May 1995 14:55:28   FRED
// More warp effects preview stuff
// 
//    Rev 1.4   12 May 1995 10:35:14   FRED
// Warp text preview
// 
//    Rev 1.3   11 May 1995 09:39:18   JAY
// Made CreateStruct an external class.
// 
//    Rev 1.2   28 Apr 1995 17:36:42   FRED
//  
// 
//    Rev 1.1   28 Apr 1995 13:50:44   FRED
// 
//    Rev 1.0   27 Apr 1995 13:30:06   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "warpobj.h"
#include "warpfld.h"
#include "warpshap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarpTextShapePage property page

CWarpTextShapePage::CWarpTextShapePage(CPmwView* pView)
	: CPmwPropertyPage(CWarpTextShapePage::IDD)
{
	//{{AFX_DATA_INIT(CWarpTextShapePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_pObject = NULL;
	m_hPreview = NULL;
}

CWarpTextShapePage::~CWarpTextShapePage()
{
}

void CWarpTextShapePage::DoDataExchange(CDataExchange* pDX)
{
	CPmwPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarpTextShapePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CWarpTextShapePage, CPmwPropertyPage)
	//{{AFX_MSG_MAP(CWarpTextShapePage)
	ON_LBN_SELCHANGE(IDC_SHAPE_LIST, OnSelchangeShapeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWarpTextShapePage::Object(WarpTextObject* pObject)
{
	m_pObject = pObject;
	
	if (m_pObject != NULL)
	{
		m_WarpShape.WarpField(m_pObject->GetWarpField());
	}

	if (GetSafeHwnd() != NULL)
	{	
		// get the preview window
		
		CWnd* pParent = GetParent();
		
		if (pParent != NULL)
		{
			m_hPreview = pParent->GetDlgItem(IDC_PREVIEW)->GetSafeHwnd();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWarpTextShapePage message handlers

BOOL CWarpTextShapePage::OnInitDialog()
{
	CPmwPropertyPage::OnInitDialog();
	
	// initialize the warp shape list box
	
	m_WarpShape.SubclassDlgItem(IDC_SHAPE_LIST, this);

	if (m_WarpFile.Init(GetGlobalPathManager()->ExpandPath("PMW.WRP")))
	{
		m_WarpShape.WarpFields(&m_WarpFile, "Rectangle");
	}
	
	m_WarpShape.SizeItems();
	
	if (m_pObject != NULL)
	{
		m_WarpShape.WarpField(m_pObject->GetWarpField());
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWarpTextShapePage::OnSelchangeShapeList()
{
	// the list box selection changed, update the object and the preview
	
	if (m_pObject != NULL)
	{
		DB_RECORD_NUMBER lRecord = 0;
		CWarpField* pWarpField = m_WarpShape.WarpField();
		
		PMGDatabase* pDatabase = m_pObject->get_database();

		if (pWarpField != NULL)
		{
			WarpField_CreateStruct CreateStruct;
						
			CreateStruct.m_pszName = pWarpField->Name();
			pWarpField->OriginalBounds()->ToData(CreateStruct.m_BoundsData);
			
			lRecord = pDatabase->new_warp_field_record(&CreateStruct);
		}

		m_pObject->set_new_warp_field(lRecord);

		if (lRecord > 0)
		{
			// Decrement our creation reference count.
			pDatabase->free_warp_field_record(lRecord);
		}

		UpdatePreview();
	}
}

void CWarpTextShapePage::UpdatePreview(void)
{
	if (m_hPreview != NULL)
	{
		::InvalidateRect(m_hPreview, NULL, TRUE);
	}
}


