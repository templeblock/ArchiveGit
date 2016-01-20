// PsBaseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "LaunchResource.h"
#include "CommonTypes.h"

#include "PSBaseDialog.h"
#include "PSButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPSBaseDialog dialog

CPSBaseDialog::CProjectMap CPSBaseDialog::m_cProjectMap;

CPSBaseDialog::CPSBaseDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
	, m_nIDTemplate( nIDTemplate )
{
	//{{AFX_DATA_INIT(CPSBaseDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPSBaseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSBaseDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPSBaseDialog, CDialog)
	//{{AFX_MSG_MAP(CPSBaseDialog)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSBaseDialog message handlers

BOOL CPSBaseDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_cProjectMap.IsEmpty())
	{
		LoadProjectMap();
	}

	LoadButtonData();
	LoadListData();
	LoadPaperList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPSBaseDialog::OnBack() 
{
	GetParent()->PostMessage( UM_POP_DIALOG );
}

void CPSBaseDialog::LoadProjectMap()
{
	// Load the project map
	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hRes = ::FindResource( hModule, MAKEINTRESOURCE( ID_PROJECT_MAP ), "SYSTEMDATA" );

	if (hRes)
	{
		HGLOBAL hData = ::LoadResource( hModule, hRes );

		if (hData)
		{
			int nCount = SizeofResource( hModule, hRes ) / sizeof( SProjectMapEntry );
			SProjectMapEntry *m_pData = (SProjectMapEntry *) hData;

			for (int i = 0; i < nCount; i++)
			{
				m_cProjectMap.SetAt( m_pData[i].m_wType, m_pData[i] );
			}

			::FreeResource( hData );
		}
	}
}

void CPSBaseDialog::LoadButtonData()
{
	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hRes = ::FindResource( hModule, MAKEINTRESOURCE( m_nIDTemplate ), "BUTTONDATA" );

	if (hRes)
	{
		HGLOBAL hData = ::LoadResource( hModule, hRes );

		if (hData)
		{
			try
			{
				DWORD dwSize = SizeofResource( hModule, hRes );
				CMemFile cFile( (BYTE *) hData, dwSize );
				CArchive ar( &cFile, CArchive::load );

				WORD wControlId = 0;
				ar >> wControlId;

				while (wControlId > 0)
				{
					CWnd* pWnd = GetDlgItem( wControlId );

					if (pWnd)
					{
						// Create a self-deleting button
						CPSButton* pButton = new CPSButton( TRUE );
						pButton->Initialize( ar, 0L );

						pButton->SubclassWindow( pWnd->GetSafeHwnd() );
					}

					ar >> wControlId;
				} 

				ar.Close();
			}
			catch (...)
			{
			}

			::FreeResource( hData );
		}
	}
}

void CPSBaseDialog::LoadListData()
{
	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hRes = ::FindResource( hModule, MAKEINTRESOURCE( m_nIDTemplate ), "LISTDATA" );

	if (hRes)
	{
		HGLOBAL hData = ::LoadResource( hModule, hRes );

		if (hData)
		{
			WORD *pData = (WORD *) hData;

			while (*pData)
			{
				WORD wResID = *pData++;

				CListBox* pWnd = (CListBox *) GetDlgItem( wResID );
				ASSERT( pWnd );

				CString strItem;

				while (*pData)
				{
					try
					{
						strItem.LoadString( *pData++ );
						int nItem = pWnd->AddString( strItem );

						WORD wLow = *pData++; WORD wHigh = *pData++;
						pWnd->SetItemData( nItem, MAKELONG( wLow, wHigh ) );
					}
					catch (CMemoryException&)
					{
						// TODO: add alert message
					}
				}

				pData++;
			}

			::FreeResource( hData );
		}
	}
}

void CPSBaseDialog::LoadPaperList()
{
	CListBox* pWnd = static_cast<CListBox *>( GetDlgItem( IDC_PAPER_LIST ) );

	if (pWnd)
	{
		UINT uiPaperMap = kPaperMapDefault;

		// Find the paper map.  Note: project type is needed here.
		//
		if (_ProjectInfo.nProjectType >= 0)
		{
			SProjectMapEntry mapEntry;
			if (m_cProjectMap.Lookup( _ProjectInfo.nProjectType, mapEntry ) && mapEntry.m_wPaperMapID)
				uiPaperMap = mapEntry.m_wPaperMapID;

#if 0
			HMODULE hModule = ::AfxGetResourceHandle();
			HRSRC hResInfo  = ::FindResource( hModule, MAKEINTRESOURCE( 
				_ProjectInfo.nProjectType ), "PROJECT_MAP" );

			if (hResInfo)
			{
				HGLOBAL hData = ::LoadResource( hModule, hResInfo );
				WORD* pData = (WORD *) hData;

				uiPaperMap = pData[1];

				::FreeResource( hData );
			}
#endif
		}

		// Load the paper map
		//
		HMODULE hModule = ::AfxGetResourceHandle();
		HRSRC hResInfo  = ::FindResource( hModule, MAKEINTRESOURCE( 
			uiPaperMap ), "PAPER_MAP" );

		if (hResInfo)
		{
			HGLOBAL hData = ::LoadResource( hModule, hResInfo );
			WORD* pData = (WORD *) hData;
			CString strPaperType;

			pWnd->SetRedraw( FALSE );
			pWnd->ResetContent();

			for (int i = 0; pData[i]; i++)
			{
				if (strPaperType.LoadString( pData[i] ))
				{
					int nItem = pWnd->AddString( strPaperType );
					pWnd->SetItemData( nItem, pData[i] - IDS_PAPER_USLETTER );
				}
			}

			pWnd->SetCurSel( 0 );
			pWnd->SetRedraw( TRUE );
			pWnd->Invalidate();

			PostMessage( WM_COMMAND, MAKEWPARAM( pWnd->GetDlgCtrlID(), LBN_SELCHANGE ), (LPARAM) pWnd->m_hWnd );

			::FreeResource( hData );
		}
	}
}

UINT CPSBaseDialog::GetBaseDialogID( UINT nDialogID )
{
	//
	// The dialog id can contain two parts.  The first part is
	// the base dialog which is the thousands part of the value.
	// For example, with an id of 2010, the base dialog id is 2000. 
	// The base dialog id is used to determine which dialog template
	// to use while the entire number is used to determine what
	// configuration data to use.
	//
	// Note: If the hundreds part of the id is greater then or
	// equal to 500 then it is a special dialog that cannot use 
	// the base dialog template.
	//

	UINT nBaseDialog = nDialogID / 1000 * 1000;

	if (nDialogID - nBaseDialog >= 500)
	{
		// The hundreds portion of the value is >= 500, so
		// don't use the base dialog template.  (See note above).
		nBaseDialog = nDialogID;
	}

	return nBaseDialog;
}
