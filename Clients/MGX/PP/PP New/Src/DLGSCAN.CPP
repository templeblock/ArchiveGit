//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#define SCANMAIN
#include "pp.h"
#include "ppafx.h"
#include <time.h>
#include "id.h"
#include "twain.h"
#include "dca_glue.h"
#include "dca_acq.h"
#include "calib.h"
#include "defaults.h"
#include "dlgscan.h"


extern "C"
{
#include "dyndlg.h"
}

// Static prototypes
BOOL Global_fScanUseCMS;
static int Global_ScanSetup_iTabTable[] =
{
    IDD_SCANCAL,
    IDD_CMSCAL
};


CScanSetupDlg::CScanSetupDlg( CWnd *pParentWnd ) :
    CPPModalDlg ( IDD_SCANSETUP, pParentWnd)
{
}

int CScanSetupDlg::DoModal()
{
    int ret = -1; // -1 == couldn't create dialog successfully
    if (PreDoModal(NULL))
    {
        ret = CPPModalDlg::DoModal();
        PostDoModal();
    }
    return ret;
}

BOOL CScanSetupDlg::OnInitDialog()
{
    STRING szString;

    CenterPopup( GetSafeHwnd() );

    m_bTwainOK = CheckTwainOK();

    SetTwainStrings( );
    ControlEnable( GetSafeHwnd(), IDM_ACQUIRESETUP, m_bTwainOK );
    
    CheckDlgButton(IDC_SCANUSECMS, m_fScanUseCMS);
    ControlEnable( GetSafeHwnd(), IDC_SCANUSECMS, m_fCMSEnabled);
    
    // Initialise the dialog section.
    InitDlgSection(&m_scansec, ID_SECTION0, GetSafeHwnd(), IDC_PANE0, NULL);
    
    // Set threshold so that a category is always discarded when the user
    // selects another.
    SetDlgSectionThreshold(&m_scansec, 1, 1);
    
    int nItems = sizeof(Global_ScanSetup_iTabTable)/sizeof(int);
    for (int i = 0; i < nItems; ++i)
    {
    	// add the halftone category
    	if (Global_ScanSetup_iTabTable[i] != DLGSEC_ERR)
    		AddDlgSectionCategory(&m_scansec,
    							PictPubApp.GetResourceHandle(),
    							MAKEINTRESOURCE(Global_ScanSetup_iTabTable[i]),
    							Global_ScanSetup_iTabTable[i],
    					    	DSCF_DISCARDABLE,
    							NULL);
    }
    
    ScanSetup_Update();
    
    return(TRUE);
}

BOOL CScanSetupDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
   BOOL bReturn = TRUE;
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);
   CMSINFO	cmsInfo;
	CCmsScanSetupDlg	ScanDlg( &cmsInfo, CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );


    char szPort[5];

    switch (nID)
	{
    	case IDC_CMS_ENABLED:
			ScanDlg.DoModal();
    		ColorManager.Scan = cmsInfo;
    		CmsScanSetupInit( GetSafeHwnd() );
    		break;
    		
    	case IDC_SCANUSECMS:
    		m_fScanUseCMS = !m_fScanUseCMS;
    		CheckDlgButton(IDC_SCANUSECMS, m_fScanUseCMS);
    		ScanSetup_Update();
        	break;
    
    	case IDM_ACQUIRESETUP:
    	    DCTerminate();
        	if ( !DCOpenDSM() )
        		break;
        	DCSelectDS();
        	DCCloseDSM();
        	SetTwainStrings( );
        	break;
    
    	case IDC_EXTNAMES:
        	if ( nCode != CBN_SELCHANGE )
    	    	break;
//        	GetExtNameN( GetSafeHwnd(), nID, IDC_FILENAME, m_Scanner,
//       		IDN_SCANNER, nCode, IDS_NONE, NO );
    	    break;
    
    	case IDC_CALSCANNAMES:
        	if ( nCode != CBN_SELCHANGE )
        		break;
        	GetExtNameN( GetSafeHwnd(), nID, IDC_CALSCANFILE, m_ScanMap,
        		IDN_SCANMAP, nCode, IDS_NONE, NO);
    	    break;
    
    	case IDC_EXTMANAGE:
        	m_idListToManage = nID;
        	PopupMenuEx( GetSafeHwnd(), nID, IDC_EXTMANAGE, IDC_FILENAME );
        	break;
        
    	case IDC_CALSCANMGR:
        	m_idListToManage = nID;
        	PopupMenuEx( GetSafeHwnd(), nID, IDC_EDITEXTMANAGE, IDC_CALSCANFILE );
        	break;
    
    	case IDC_ADDEXT:
    	case IDC_DELETEEXT:
    	case IDC_RENAMEEXT:
    	    switch (m_idListToManage )
    		{
    		case IDC_EXTMANAGE:
//        		ExtNameManager( GetSafeHwnd(), IDC_EXTNAMES, IDC_FILENAME,
//        			m_Scanner, IDN_SCANNER, nID, NO );
    	    	break;
    
    		case IDC_CALSCANMGR:
    		    ExtNameManager( GetSafeHwnd(), IDC_CALSCANNAMES, IDC_CALSCANFILE,
        			m_ScanMap, IDN_SCANMAP, nID, NO );
        		break;
    		}
    	    break;
    
    	case IDC_EDITEXT:
            {        
                CScanEditDlg *pDlg = new 
                    CScanEditDlg(IDD_CALSCANEDIT,this);
                if (pDlg)
                {
                    pDlg->DoModal();
                    delete pDlg;
                }
            	InitExtNameN( GetSafeHwnd(), IDC_CALSCANNAMES, IDC_CALSCANFILE, 
                    m_ScanMap,
        	    	IDN_SCANMAP, IDS_NONE, NO );
            }
    	    break;

        case ID_SECTION0:
        case ID_SECTION1:
        case ID_SECTION2:
        case ID_SECTION3:
        case ID_SECTION4:
        case ID_SECTION5:
        case ID_SECTION6:
        case ID_SECTION7:
        case ID_SECTION8:
        case ID_SECTION9:
            // NOTE: This is to prevent and Nasty ASSERT in MFC Code.
            if (!::IsWindow(hWndCtrl))
                break;
            // <<<<fall thru>>>>>
        default:
            bReturn = FALSE;
  	        break;
    }

    return bReturn ? bReturn : CPPModalDlg::OnCommand(wParam, lParam);
}

BOOL CScanSetupDlg::PreDoModal(LPVOID lpVoid)
{
    m_fCMSEnabled = Control.CMSEnabled;
    m_fScanUseCMS = Scans.fScanUseCMS;

    GetDefaultString(ScanMap_Str,Empty_Str,m_ScanMap,sizeof(m_ScanMap));

    return CPPModalDlg::PreDoModal(lpVoid);
}

LPVOID CScanSetupDlg::PostDoModal()
{
    //TODO
    Control.CMSEnabled = m_fCMSEnabled;
    Scans.fScanUseCMS = m_fScanUseCMS;
    PutDefaultInt( fScanUseCMS_Str, m_fScanUseCMS );

	lstrcpy( Names.ScanMap, m_ScanMap );
    PutDefaultString( ScanMap_Str, m_ScanMap );

    return CPPModalDlg::PostDoModal();
}

void CScanSetupDlg::OnOK()
{
    HMODULE hDLL;
    STRING szString;

	if ( StringsEqualID( m_ScanMap, IDS_NONE ) )
		m_ScanMap[0] = 0;

    CPPModalDlg::OnOK();
}

void CScanSetupDlg::OnCancel()
{
    // TODO
	Global_fScanUseCMS = m_fScanUseCMS;
    CPPModalDlg::OnCancel();
}

void CScanSetupDlg::ScanSetup_Update()
{
	ITEMID idCategory;

	idCategory = Global_ScanSetup_iTabTable[m_fScanUseCMS];
	SelectDlgSectionCategory(&m_scansec, idCategory);

	if (idCategory == IDD_SCANCAL)
	{
		InitExtNameN( GetSafeHwnd(), IDC_CALSCANNAMES, IDC_CALSCANFILE, m_ScanMap,
			IDN_SCANMAP, IDS_NONE, NO);
		SetWindowWord( ::GetDlgItem(GetSafeHwnd(),IDC_CALSCANMGR), GWW_ICONID,
			IDC_EXTMANAGE );
	}
	else
	if (idCategory == IDD_CMSCAL)
    	CmsScanSetupInit( GetSafeHwnd() );
}

void CScanSetupDlg::SetTwainStrings( )
{
    FNAME szFileName;
    STRING szTime, szSize;
    long lSize;
    int length;
    FILETIMES time;
    
    GetProfileString( "TWAIN32", "Default Source", "", szFileName, sizeof(FNAME) );
    
    if ( !lstrlen(szFileName) )
    	return;
    SetDlgItemText(IDC_GRABFILE, Lowercase(szFileName) );
    if ( !FileDate(szFileName, &time) )
    	return;
    FileTimeToString(&time, szTime);
    length = lstrlen(szTime);
    while (szTime[length-1] == '\r' || szTime[length-1] == '\n')
    	szTime[--length] = '\0';
    lSize = FileSize(szFileName);
    wsprintf(szSize, "   %ld", lSize);
    lstrcat(szTime, szSize);
    SetDlgItemText(IDC_GRABDATE, szTime);
}
