/***************************************************************************/

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

/***************************************************************************/

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "clrmng.h"

/***************************************************************************/
// GLOGAL data
/***********************************************************************/
LPKODAKATTRIB	KCMSGetAttribute;

/***********************************************************************/
// LOCAL data
/***********************************************************************/
static	HINSTANCE				hDLL;
static	LPKODAKOPEN				KCMSOpen;
static	LPKCMSSIZEPROC			lpKCMSSizeProc;
static	LPKCMSDATAPROC			lpKCMSDataProc;
static	LPKCMSGAMMPROC			lpKCMSGammProc;
static	LPKCMSCOPYGAMMAXFORM	lpKCMSCopyGammaXform;
static	FARPROC					lpCMSProgress;
static 	char						CRCtoCRC[] = "CRCtoCRC";
static 	char						CRCtoUID[] = "CRCtoUID";
static	char						pScanSetupSrcName[] = "ScanSetupSrcName";
static	char						pScanSetupDstName[] = "ScanSetupDstName";
static	char						pFileOpenSrcName[] = "FileOpenSrcName";
static	char						pFileOpenDstName[] = "FileOpenDstName";
static	char						pConvertToSrcName[] = "ConvertToSrcName";
static	char						pConvertToDstName[] = "ConvertToDstName";
static	char						pPasteSrcName[]    = "PasteSrcName";
static	char						pPCDPasteSrcName[]    = "PCDPasteSrcName";
static	char						pPrintPreviewDstName[] = "PrintPreviewDstName";
static	char						pFileNewSetupDstName[] = "FileNewSetupDstName";
static	char						pConvertCMYKDstName[] = "ConvertCMYKDstName";
static	char						pConvertRGBDstName[] = "ConvertRGBDstName";
static	char						pPrintSetupDstName[] = "PrintSetupDstName";
static	char						pPrintSetupDstProfile[] = "PrintSetupDstProfile";
static	char						pMonitorSetupDstName[] = "MonitorSetupDstName";
static	char						pMonitorSetupDstProfile[] = "MonitorSetupDstProfile";
static	char						pPhotoCDOpenSrcName[] = "PhotoCDOpenSrcName";
static	char						pPhotoCDOpenSrcProfile[] = "PhotoCDOpenSrcProfile";
static	char						pPhotoCDOpenDstName[] = "PhotoCDOpenDstName";
static	char						pPhotoCDOpenDstProfile[] = "PhotoCDOpenDstProfile";

static  char						pMonitorDstDataTypeHi[] = "MonitorDstDataTypeHi";
static  char						pMonitorDstDataTypeLo[] = "MonitorDstDataTypeLo";
static  char						pMonitorDstToRCSHi[] = "MonitorDstToRCSHi";
static  char						pMonitorDstToRCSLo[] = "MonitorDstToRCSLo";
static  char						pMonitorDstFrRCSHi[] = "MonitorDstFrRCSHi";
static  char						pMonitorDstFrRCSLo[] = "MonitorDstFrRCSLo";

static	char						pScanSrcDataTypeHi[] = "ScanSrcDataTypeHi";
static	char						pScanSrcDataTypeLo[] = "ScanSrcDataTypeLo";
static	char						pScanSrcToRCSHi[] = "ScanSrcToRCSHi";
static	char						pScanSrcToRCSLo[] = "ScanSrcToRCSLo";
static	char						pScanSrcFrRCSHi[] = "ScanSrcFrRCSHi";
static	char						pScanSrcFrRCSLo[] = "ScanSrcFrRCSLo";

static	char						pScanDstDataTypeHi[] = "ScanDstDataTypeHi";
static	char						pScanDstDataTypeLo[] = "ScanDstDataTypeLo";
static	char						pScanDstToRCSHi[] = "ScanDstToRCSHi";
static	char						pScanDstToRCSLo[] = "ScanDstToRCSLo";
static	char						pScanDstFrRCSHi[] = "ScanDstFrRCSHi";
static	char						pScanDstFrRCSLo[] = "ScanDstFrRCSLo";

static	char						pPhotoCDSrcDataTypeHi[] = "PhotoCDSrcDataTypeHi";
static	char						pPhotoCDSrcDataTypeLo[] = "PhotoCDSrcDataTypeLo";
static	char						pPhotoCDSrcToRCSHi[] = "PhotoCDSrcToRCSHi";
static	char						pPhotoCDSrcToRCSLo[] = "PhotoCDSrcToRCSLo";
static	char						pPhotoCDSrcFrRCSHi[] = "PhotoCDSrcFrRCSHi";
static	char						pPhotoCDSrcFrRCSLo[] = "PhotoCDSrcFrRCSLo";

static	char						pPhotoCDDstDataTypeHi[] = "PhotoCDDstDataTypeHi";
static	char						pPhotoCDDstDataTypeLo[] = "PhotoCDDstDataTypeLo";
static	char						pPhotoCDDstToRCSHi[] = "PhotoCDDstToRCSHi";
static	char						pPhotoCDDstToRCSLo[] = "PhotoCDDstToRCSLo";
static	char						pPhotoCDDstFrRCSHi[] = "PhotoCDDstFrRCSHi";
static	char						pPhotoCDDstFrRCSLo[] = "PhotoCDDstFrRCSLo";

static	char						szDefaultSrc[] = "(None)";
static	char						szDefaultDst[] = "(None - Default Monitor if Source selected)";
static  CMSINFO						SaveSrcInfo;
static  CMSINFO						SaveDstInfo;
static  char						SrcDevice[256];
static  char						SrcProfile[256];
static  char						DstDevice[256];
static  char						DstProfile[256];

static	FNAME						szCms;  // CMS.INI w/path

COLORMANAGER			ColorManager;
LPKCMSCNCTTHREEPROC	lpKCMSCnctThreeProc;

__declspec( dllimport ) LPKCMSCNCTPROC lpKCMSCnctProc;
__declspec( dllimport ) LPKCMSEVALPROC lpKCMSEvalProc;
__declspec( dllimport ) LPKCMSFREEPROC lpKCMSFreeProc;
__declspec( dllimport ) LPKCMSUIDMPROC lpKCMSUidmProc;
/***********************************************************************/
// LOCAL prototypes
/***********************************************************************/
LOCAL BOOL CmsPrintSetupGetDst( HWND hDlg, LPFRMTYPEINFO lpDstInfo );
LOCAL void DeleteMap( CMapStringToPtr *pMap );
BOOL FAR EXPORT CMSProgress( int percent, ITEMID Id );
LOCAL void CmsPCDSaveSettings(HWND hDlg);
LOCAL void CmsMonitorSelect( HWND hDlg );

/***************************************************************************/
void CMSInit( void )
/***************************************************************************/
{
hDLL 		= 0;
KCMSOpen = 0;
}

/***************************************************************************/
BOOL CMSOpen( BOOL bCMSEnable )
/***************************************************************************/
{
int					uErrorMode;

if( !bCMSEnable )
	{
	// CMS Open ?
	if( !( KCMSOpen ) )
		return( FALSE );

	// Close The Color Manager
	( *KCMSOpen )( bCMSEnable, ( FARPROC )0, NULL );

	// Free the CMS DLL
	FreeProcInstance( lpCMSProgress );

	FreeLibrary( hDLL );

	KCMSOpen 							= NULL;
	KCMSGetAttribute 					= NULL;
	lpKCMSCnctProc 					= NULL;
	lpKCMSEvalProc 					= NULL;
	lpKCMSFreeProc 					= NULL;
	lpKCMSUidmProc						= NULL;
	lpKCMSSizeProc 					= NULL;
	lpKCMSDataProc 					= NULL;
	lpKCMSCnctThreeProc 				= NULL;
	lpKCMSCopyGammaXform				= NULL;

	hDLL 									= NULL;

	// Free the PT Maps
	DeleteMap( ColorManager.srcMap );
	DeleteMap( ColorManager.dstMap );
	DeleteMap( ColorManager.monMap );
	DeleteMap( ColorManager.pcdMap );
	DeleteMap( ColorManager.scnMap );

	delete( ColorManager.crcMap );
	delete( ColorManager.uidMap );
	delete( ColorManager.spcMap );

	LPFRAME	  lpFrame;
   CServerDoc *pDoc;
   POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
   while (TemplatePos)
   {
       CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);
       POSITION DocPos = pTemplate->GetFirstDocPosition();
       while (DocPos)
       {
           pDoc = (CServerDoc*)pTemplate->GetNextDoc (DocPos);
           if (pDoc && pDoc->m_lpImage )
           {
					pDoc->m_lpImage->m_cmsXform = NULL;
					pDoc->m_lpImage->m_bDoCmsGamma = 0;
					pDoc->m_lpImage->SaveInfo.toRCS =
					pDoc->m_lpImage->SaveInfo.frRCS =
					pDoc->m_lpImage->PtInfo.toRCS = 
					pDoc->m_lpImage->PtInfo.frRCS = 0;
					lpFrame = ImgGetBaseEditFrame( pDoc->m_lpImage );
					if( lpFrame )
						lpFrame->ptInfo.toRCS = lpFrame->ptInfo.frRCS = 0;
           }
       }
   }
	return( FALSE );
	}
else
	{
	// Print Preview
	ColorManager.CmsXform = 0;

	// Get location of CMS.INI is, regardless of installation type.
	lstrcpy( szCms, "CMS.INI" );
    GetIniPath( szCms );    

	// Turn off Windows' Error Box
	uErrorMode = SetErrorMode( SEM_NOOPENFILEERRORBOX );

	// Load the color manager's DLL
	hDLL = ( HINSTANCE )AstralLoadLibrary( "CMS.DLL" );

	// Turn Windows' Error Box back on
	SetErrorMode( uErrorMode );

	// Give up if no dll loaded
	if ( !( hDLL ) )
		return( FALSE );

	// Get CMS DLL Proc Addrs
	KCMSOpen =	( LPKODAKOPEN )GetProcAddress( hDLL, "KCMSOpen" );
	KCMSGetAttribute =	( LPKODAKATTRIB )GetProcAddress( hDLL, "KCMSGetAttribute" );
	lpKCMSCnctProc = ( LPKCMSCNCTPROC )GetProcAddress( hDLL, "KCMSCnctProc" );
	lpKCMSEvalProc = ( LPKCMSEVALPROC )GetProcAddress( hDLL, "KCMSEvalProc" );
	lpKCMSFreeProc = ( LPKCMSFREEPROC )GetProcAddress( hDLL, "KCMSFreeProc" );
	lpKCMSSizeProc = ( LPKCMSSIZEPROC )GetProcAddress( hDLL, "KCMSSizeProc" );
	lpKCMSDataProc = ( LPKCMSDATAPROC )GetProcAddress( hDLL, "KCMSDataProc" );
	lpKCMSCnctThreeProc = ( LPKCMSCNCTTHREEPROC )GetProcAddress( hDLL, "KCMSCnctThreeProc" );
	lpKCMSGammProc = ( LPKCMSGAMMPROC )GetProcAddress( hDLL, "KCMSGammProc" );
	lpKCMSCopyGammaXform = ( LPKCMSCOPYGAMMAXFORM )GetProcAddress( hDLL, "KCMSCopyGammaXform" );
	lpKCMSUidmProc = ConvertCRCtoUID;
	lpBltScreen->lpCmsGammaProc = CmsSetSuperBltGamma;

	if( !( KCMSOpen ) || /*!( KCMSGetAttribute ) || */!( lpKCMSCnctProc ) ||
		 !( lpKCMSEvalProc ) || !( lpKCMSFreeProc ) || !( lpKCMSUidmProc ) ||
		 !( lpKCMSSizeProc ) || !( lpKCMSDataProc ) || !( lpKCMSGammProc ) ||
		 !( lpKCMSCopyGammaXform ) )
		{
		FreeLibrary( hDLL );
		return( FALSE );
		}

	// Open The Color Manager
	lpCMSProgress = MakeProcInstance( ( FARPROC )CMSProgress,
		PictPubApp.Get_hInstAstral() );

	if( !( ( *KCMSOpen )( bCMSEnable, lpCMSProgress, szCms ) ) )
		{
		FreeLibrary( hDLL );
		KCMSOpen 							= NULL;
		KCMSGetAttribute 					= NULL;
		lpKCMSCnctProc 					= NULL;
		lpKCMSEvalProc 					= NULL;
		lpKCMSFreeProc 					= NULL;
		lpKCMSUidmProc						= NULL;
		lpKCMSSizeProc 					= NULL;
		lpKCMSDataProc 					= NULL;
		lpKCMSCnctThreeProc 				= NULL;
	   lpKCMSCopyGammaXform				= NULL;
		hDLL 									= NULL;
		return( FALSE );
		}

	// Open the data base
	if( !( DBOpen( &ColorManager ) )	)
		{
		FreeLibrary( hDLL );
		KCMSOpen 							= NULL;
		KCMSGetAttribute 					= NULL;
		lpKCMSCnctProc 					= NULL;
		lpKCMSEvalProc 					= NULL;
		lpKCMSFreeProc 					= NULL;
		lpKCMSUidmProc						= NULL;
		lpKCMSSizeProc 					= NULL;
		lpKCMSDataProc 					= NULL;
		lpKCMSCnctThreeProc 				= NULL;
	   lpKCMSCopyGammaXform				= NULL;
		hDLL 									= NULL;
		return( FALSE );
		}

	LoadScanSetup();
	LoadPhotoCDSetup();
	LoadMonitorSetup();

	CmsSetSuperBltGamma( TRUE );
	}

return( TRUE );
}

/****************************************************************************/
LPVOID CmsCopyGammaXform( LPVOID xform )
/****************************************************************************/
{
return( lpKCMSCopyGammaXform ? ( *lpKCMSCopyGammaXform )( xform ) : FALSE );
}
/****************************************************************************/
LONG GetXformDataSize( LONG lCrc , LPSTR pMandM )
/****************************************************************************/
{

return( lpKCMSSizeProc ? ( *lpKCMSSizeProc )( ConvertCRC( lCrc ),
	pMandM ) : FALSE );
}

/****************************************************************************/
BOOL GetXformData( LONG lCrc, LPTR pData )
/****************************************************************************/
{

return( lpKCMSDataProc ? ( *lpKCMSDataProc )( ConvertCRC( lCrc ),
	pData ) : FALSE );
}

/****************************************************************************/
void DeleteMap( CMapStringToPtr *pMap )
/****************************************************************************/
{
POSITION				pos;
CMapStringToPtr	*pPt;
CString				key;

// Free the PT Maps
for( pos = pMap->GetStartPosition(); pos != NULL; )
	{
	pMap->GetNextAssoc( pos, key, ( LPVOID & )pPt );
	delete( pPt );
	}

delete( pMap );

}

#define BUFFER_SIZE 4096
/****************************************************************************/
BOOL DBOpen( LPCOLORMANAGER pColorManager )
/****************************************************************************/
{

if( !( pColorManager->uidMap = DBBuildList( "CRCtoUID" ) ) ||
	 !( pColorManager->crcMap = DBBuildList( "CRCtoCRC" ) ) ||
	 !( pColorManager->spcMap = DBBuildList( "CRCtoSPC" ) ) ||
	 !( pColorManager->monMap = DBOpenSection( "Monitor" ) ) ||
	 !( pColorManager->pcdMap = DBOpenSection( "PhotoCD" ) ) ||
	 !( pColorManager->scnMap = DBOpenSection( "Scanner" ) ) ||
	 !( pColorManager->srcMap = DBOpenSection( "Input" ) ) ||
	 !( pColorManager->dstMap = DBOpenSection( "Output" ) ) )
	return( FALSE );

return( TRUE );
}

/****************************************************************************/
CMapPtrToPtr *DBBuildList( LPSTR pSection )
/****************************************************************************/
{
char					aDesc[256];
LPSTR					pIDs, pIDsH;
DWORD					lLen, lID, lCRC;
CMapPtrToPtr		*pMap;

if( !( pIDs = pIDsH = ( LPSTR )Alloc( BUFFER_SIZE ) ) )
	return( FALSE );

GetPrivateProfileString( pSection, NULL, "", pIDs, BUFFER_SIZE, szCms );

pMap = new CMapPtrToPtr;

while( lLen = lstrlen( pIDs ) )
	{
	lCRC = atol( pIDs );

	GetPrivateProfileString( pSection, pIDs, "", aDesc, sizeof( aDesc ),
		szCms );
	lID = atol( aDesc );
	pMap->SetAt( ( LPVOID )lCRC, ( LPVOID &)lID );
	pIDs += ( lLen + 1 );
	}
FreeUp( pIDsH );

return( pMap );
}

/****************************************************************************/
CMapStringToPtr *DBOpenSection( LPSTR pSection )
/****************************************************************************/
{
char					aDesc[256], pWork[128];
LPSTR					pIDs, pIDsH;
DWORD					lLen, lID;
CMapStringToPtr	*pPTs, *pMap, *lDummy;

if( !( pIDs = pIDsH = ( LPSTR )Alloc( BUFFER_SIZE ) ) )
	return( FALSE );

GetPrivateProfileString( pSection, NULL, "", pIDs, BUFFER_SIZE, szCms );

pMap = new CMapStringToPtr;

while( lLen = lstrlen( pIDs ) )
	{
	lID = atol(	pIDs );

	GetPrivateProfileString( pSection, pIDs, "", aDesc, sizeof( aDesc ),
		szCms );
	if( !( pMap->Lookup(	aDesc, ( LPVOID & )pPTs ) ) )
		pMap->SetAt( aDesc, pPTs = new CMapStringToPtr );
	GetPrivateProfileString( lstrcat( lstrcpy( pWork, pSection ), "IDs" ),
		pIDs, "", aDesc, sizeof( aDesc ), szCms );
	if( pPTs->Lookup( aDesc, ( LPVOID & )lDummy ) )
		lstrcat( aDesc, "##" );
	pPTs->SetAt( aDesc, ( LPVOID &)lID );
	pIDs += ( lLen + 1 );
	}

FreeUp( pIDsH );
return( pMap );
}

/***************************************************************************/
BOOL FAR EXPORT CMSProgress( int percent, ITEMID Id )
/***************************************************************************/
{

if( Id != 0 )
	{	
	if( percent == 0 )
		{	
		MessageStatus( Id );
		AstralBeachCursor( ID_BALL1 );
		}
	else
		{	
		if( percent == 100 )
			{
//			AstralCursor( NULL );
			return( FALSE );
			}
		else
			AstralBeachCursor( ID_BALL1 );
		}
	if( Id == IDS_CMS_BUILDING )
		{
		if( CANCEL )
			return( TRUE );
		else
			return( FALSE );
		}
	}
return( FALSE );
}

/***********************************************************************/
// LOCAL prototypes
/***********************************************************************/
LOCAL BOOL DlgCmsPrintSetupInfo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void DlgCmsPrintSetupInfo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgCmsPrintSetupInfoProc( HWND hDlg, UINT msg, WPARAM wParam,
                                          LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, DlgCmsPrintSetupInfo_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, InfoMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, InfoDrawItem);
	HANDLE_DLGMSG(hDlg, WM_COMMAND, DlgCmsPrintSetupInfo_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CMS_DSTINFO);
	}
}

/************************************************************************/
LOCAL BOOL DlgCmsPrintSetupInfo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
char					*pkey;
CString				key;
CMapStringToPtr	*pPTs;
DWORD					nID, nCRC;
int					nIndex;
HWND					parent;

pkey = key.GetBuffer( 256 );

parent = ( HWND )lParam;

CenterPopup( hDlg );

ComboBox_GetText( GetDlgItem( parent, IDC_CMS_DST ), pkey, BUFFER_SIZE );

ListBox_ResetContent( GetDlgItem( hDlg, IDC_CMS_DSTINFO ) );

if( !( ColorManager.dstMap->Lookup( key, ( void *& )pPTs ) ) )
	return( FALSE );

ListBox_GetText( GetDlgItem( parent, IDC_CMS_DSTLIST ),
	ListBox_GetCurSel( GetDlgItem( parent, IDC_CMS_DSTLIST ) ), pkey );

pPTs->Lookup( key, ( LPVOID & )nID );

if( !ColorManager.uidMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
	return( FALSE );

Static_SetText( GetDlgItem( hDlg, IDC_CMS_DSTNAME ), key );

nIndex = 0;

while( ( *KCMSGetAttribute )( pkey, nIndex++, nCRC, PRT ) )
	ListBox_AddString( GetDlgItem( hDlg, IDC_CMS_DSTINFO ), pkey );

return(TRUE);
}


/************************************************************************/
LOCAL void DlgCmsPrintSetupInfo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDOK:
	case IDCANCEL:
		AstralDlgEnd( hDlg, TRUE );
   }
}

/***********************************************************************/
// LOCAL prototypes
/***********************************************************************/
LOCAL BOOL DlgCmsMonitorSetupInfo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void DlgCmsMonitorSetupInfo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgCmsMonitorSetupInfoProc( HWND hDlg, UINT msg, WPARAM wParam,
                                          LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, DlgCmsMonitorSetupInfo_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, InfoMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, InfoDrawItem);
	HANDLE_DLGMSG(hDlg, WM_COMMAND, DlgCmsMonitorSetupInfo_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CMS_DSTINFO);
	}
}

/************************************************************************/
LOCAL BOOL DlgCmsMonitorSetupInfo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
char					*pkey;
CString				key;
CMapStringToPtr	*pPTs;
DWORD					nID, nCRC;
int					nIndex;
HWND					parent;

pkey = key.GetBuffer( 256 );

parent = ( HWND )lParam;

CenterPopup( hDlg );

ComboBox_GetText( GetDlgItem( parent, IDC_CMS_DST ), pkey, BUFFER_SIZE );

ListBox_ResetContent( GetDlgItem( hDlg, IDC_CMS_DSTINFO ) );

if( !( ColorManager.monMap->Lookup( key, ( void *& )pPTs ) ) )
	return( FALSE );

ListBox_GetText( GetDlgItem( parent, IDC_CMS_DSTLIST ),
	ListBox_GetCurSel( GetDlgItem( parent, IDC_CMS_DSTLIST ) ), pkey );

pPTs->Lookup( key, ( LPVOID & )nID );

if( !ColorManager.uidMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
	return( FALSE );

Static_SetText( GetDlgItem( hDlg, IDC_CMS_DSTNAME ), key );

nIndex = 0;

while( ( *KCMSGetAttribute )( pkey, nIndex++, nCRC, MON ) )
	ListBox_AddString( GetDlgItem( hDlg, IDC_CMS_DSTINFO ), pkey );

return(TRUE);
}

/************************************************************************/
LOCAL void DlgCmsMonitorSetupInfo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDOK:
	case IDCANCEL:
		AstralDlgEnd( hDlg, TRUE );
   }
}

// LOCAL prototypes
LOCAL BOOL PrintPreview_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PrintPreview_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL BOOL PrintPreview_OnEraseBkgnd(HWND hWindow, HDC hDC);
LOCAL void PrintPreview_OnPaint(HWND hWindow);
LOCAL void PrintPreview_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPrintPreviewProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
    {
    HANDLE_DLGRET(hDlg, WM_INITDIALOG, PrintPreview_OnInitDialog);  
    HANDLE_DLGMSG(hDlg, WM_COMMAND, PrintPreview_OnCommand);
    HANDLE_DLGRET(hDlg, WM_ERASEBKGND, PrintPreview_OnEraseBkgnd);
    HANDLE_DLGMSG(hDlg, WM_PAINT, PrintPreview_OnPaint);
    HANDLE_DLGMSG(hDlg, WM_KEYDOWN, PrintPreview_OnKeyDown);

    HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
    HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Dialog_OnPaletteChanged);

    default:
    return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PRINTPREVIEW);
    }
}

/************************************************************************/
LOCAL BOOL PrintPreview_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
RECT 		ClientRect;
int 		npix, nlin, cx, cy, dx, dy;
HWND 		hControl, hWndImage;
LPOBJECT	lpBase;
LPFRAME 	lpFrame;
BOOL 		bInplaceActive;

if (!(hWndImage = GetActiveDoc()))
    return(TRUE);

// Make the window as big as the screen
dx = GetSystemMetrics( SM_CXSCREEN );
dy = GetSystemMetrics( SM_CYSCREEN );
MoveWindow( hDlg, 0, 0, dx, dy, NO );

// Compute the center of the window
GetClientRect( hDlg, &ClientRect );
cx = ( ClientRect.right + ClientRect.left ) / 2;
cy = ( ClientRect.bottom + ClientRect.top ) / 2;

// Link all of the images to image controls
hControl = GetDlgItem( hDlg, IDC_VIEWFULL );
hWndImage = GetActiveDoc();

//if (hWndImage)
//	bInplaceActive = (PictPubApp.GetActiveView())->GetDocument()->IsInPlaceActive();

lpBase = ImgGetBase(GetImagePtr(hWndImage));
//if ( !lpBase || !hControl )
//    continue;
lpFrame = ObjGetEditFrame(lpBase);

// Link the frame to the image control
SetWindowLong( hControl, GWL_IMAGE, (long)lpBase);

// The destination size can be no bigger the the screen
npix = FrameXSize(lpFrame);
nlin = FrameYSize(lpFrame);
if ( npix > dx || nlin > dy )
    {
    npix = dx;
    nlin = dy;
    ScaleToFit( &npix, &nlin,
        FrameXSize(lpFrame), FrameYSize(lpFrame));
    }

// Position the control in the center of the window
ClientRect.left = cx - npix/2;
ClientRect.right = ClientRect.left + npix;
ClientRect.top = cy - nlin/2;
ClientRect.bottom = ClientRect.top + nlin;
MoveWindow( hControl,
    ClientRect.left, ClientRect.top, // New position
    RectWidth( &ClientRect ),
    RectHeight( &ClientRect ), // New size
    NO /* No repaint*/ );
if (!bInplaceActive)
{
    // Check to see if these get destroyed when the window does
	hWndImage = GetParent(hWndImage); // get the View's Parent MDIChild hWnd
/*    do  
	{
        hWndImage = GetWindow( hWndImage, GW_HWNDNEXT );
		if (hWndImage)
			pView = (CServerView *)
			((CPPMDIChildWnd*)CWnd::FromHandle(hWndImage))->GetActiveView();
		else
			pView = NULL;
    } 
	while (pView && !IsDoc(pView->GetSafeHwnd()));

	hWndImage = (pView && IsDoc(pView->GetSafeHwnd())) ? 
		pView->GetSafeHwnd() : NULL;
*/
	if (!hWndImage)
        return(TRUE);
}
else
    return(TRUE);

 // Get another image control
 hControl = CopyWindow( hControl );
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL PrintPreview_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
return(TRUE);
}

/***********************************************************************/
LOCAL void PrintPreview_OnPaint(HWND hDlg)
/***********************************************************************/
{
PAINTSTRUCT ps;
RECT ClientRect;
HDC hDC;

hDC = BeginPaint( hDlg, &ps );
//  if ( ps.fErase )
    { // Just draw the background
    ClientRect = ps.rcPaint;
    DrawColorPatch( hDC, &ClientRect, GetActiveRGB(NULL), YES );
    }
EndPaint( hDlg, &ps );
}

/***********************************************************************/
LOCAL void PrintPreview_OnKeyDown(HWND hDlg, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
AstralDlgEnd( hDlg, FALSE );
}

/************************************************************************/
LOCAL void PrintPreview_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
AstralDlgEnd( hDlg, FALSE );
}

/***********************************************************************/
void CmsSetSuperBltGamma( BOOL bIniting )
/***********************************************************************/
{

if( !Control.CMSEnabled && !bIniting)
	return;

CmsGetMonitorGamma( ColorManager.Monitor.dst.ptInfo.frRCS );

BuildGammaLut( FIXDIV( lpBltScreen->RGamma, lpBltScreen->CmsRGamma ),
	( LPTR )&lpBltScreen->CmsRGammaLut );
BuildGammaLut( FIXDIV( lpBltScreen->GGamma, lpBltScreen->CmsGGamma ),
	( LPTR )&lpBltScreen->CmsGGammaLut );
BuildGammaLut( FIXDIV( lpBltScreen->BGamma, lpBltScreen->CmsBGamma ),
	( LPTR )&lpBltScreen->CmsBGammaLut );
}

/***********************************************************************/
void CmsGetMonitorGamma( LONG Crc )
/***********************************************************************/
{
double	d;
LFIXED	e;

if( Crc )
	d = ( *lpKCMSGammProc )( ConvertCRC( Crc ) );
else
	d = 1.0;

e = DBL2FIX( d );

lpBltScreen->CmsRGamma = lpBltScreen->CmsGGamma = lpBltScreen->CmsBGamma = e;

// This should work but doesn't;
//lpBltScreen->CmsRGamma = lpBltScreen->CmsGGamma = lpBltScreen->CmsBGamma =
//	Crc ? DBL2FIX( ( *lpKCMSGammProc )( ConvertCRC( Crc ) ) ) : TOFIXED( 1 );
}

/***********************************************************************/
void CmsPrintPreview( LPIMAGE lpImage )
/***********************************************************************/
{
CMSINFO	CmsInfo;
FRMTYPEINFO	TypeInfo, PrevType, MonType;

FrameGetTypeInfo( ImgGetBaseEditFrame( lpImage ), &TypeInfo );

if( !IsDstPTSelected( &TypeInfo.ptInfo ) )
	{
	CmsInfo.src.DataType = TypeInfo.DataType;
	CCmsPasteDlg	PasteDlg( &CmsInfo,
		CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );
	PasteDlg.DoModal();
	if( !IsSrcPTSelected( &CmsInfo.src.ptInfo ) )
		return;
	else
		TypeInfo = CmsInfo.src;
	}

PrevType = Separate.cmsInfo.dst;
MonType  = ColorManager.Monitor.dst;

MonType.ptInfo.frRCS = ConvertCRC( MonType.ptInfo.frRCS );

if( 	IsDstPTSelected( &PrevType.ptInfo )	&&
		lpKCMSCnctThreeProc &&
		ConvertCRCtoUID( &TypeInfo, &PrevType ) )
	ColorManager.CmsXform = ( *lpKCMSCnctThreeProc )( &TypeInfo, &PrevType,
		IsDstPTSelected( &MonType.ptInfo ) ? &MonType :	NULL ); 
else
	ColorManager.CmsXform = NULL;

AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(),
	IDD_PRINTPREVIEW, DlgPrintPreviewProc );

if( ColorManager.CmsXform )
	{
	( *lpKCMSFreeProc )( ColorManager.CmsXform );
	ColorManager.CmsXform = NULL;
	}

}

/************************************************************************/
void CmsPhotoCD_OnInitDialog( HWND hDlg )
/************************************************************************/
{
	GetRegStr( pPhotoCDOpenSrcName,  SrcDevice, "" );
	GetRegStr( pPhotoCDOpenSrcProfile,  SrcProfile, "" );
	GetRegStr( pPhotoCDOpenDstName,  DstDevice, "" );
	GetRegStr( pPhotoCDOpenDstProfile,  DstProfile, "" );
}

/************************************************************************/
void CmsPhotoCDInit( HWND hDlg )
/************************************************************************/
{
Static_SetText( GetDlgItem( hDlg, IDC_CMS_SRCNAME ), SrcProfile );
Static_SetText( GetDlgItem( hDlg, IDC_CMS_DSTNAME ), DstDevice );
}

/************************************************************************/
void CmsScanSetupInit( HWND hDlg )
/************************************************************************/
{
CString				key;
char					*pkey;

pkey  = key.GetBuffer( BUFFER_SIZE );

GetRegStr( pScanSetupSrcName,  pkey, "" );
Static_SetText( GetDlgItem( hDlg, IDC_CMS_SRCNAME ), key );

GetRegStr( pScanSetupDstName,  pkey, "" );
Static_SetText( GetDlgItem( hDlg, IDC_CMS_DSTNAME ), key );

}

/************************************************************************/
void CmsPrintSetup_OnInitDialog( HWND hDlg )
/************************************************************************/
{
	SaveDstInfo = Separate.cmsInfo;	
	GetRegStr( pPrintSetupDstName, DstDevice, "" );
	GetRegStr( pPrintSetupDstProfile, DstProfile, "" );
}

/************************************************************************/
void CmsPrintSetupInit( HWND hDlg )
/************************************************************************/
{
POSITION				pos;
CString				key;
char					*pkey;
CMapStringToPtr	*pPTs;
DWORD					nID;
HWND hWnd;
int iSel;

pkey  = key.GetBuffer( BUFFER_SIZE );

// Build DST's list
//ComboBox_AddString( GetDlgItem( hDlg, IDC_CMS_DST ), "(None)" );

for( pos = ColorManager.dstMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos, key, ( void* &)pPTs );
	ComboBox_AddString( GetDlgItem( hDlg, IDC_CMS_DST ), pkey );
	}

ComboBox_SetCurSel( GetDlgItem( hDlg, IDC_CMS_DST ),
	ComboBox_FindStringExact( GetDlgItem( hDlg, IDC_CMS_DST ),
	-1, lstrlen( DstDevice ) ? DstDevice : "(None)" ) );

ComboBox_GetText( GetDlgItem( hDlg,	IDC_CMS_DST ),	DstDevice, 256 );

if( ColorManager.dstMap->Lookup( DstDevice, ( void *& )pPTs ) )
	{
	hWnd = GetDlgItem(hDlg, IDC_CMS_DSTLIST);
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, key, ( void *& )nID );
		ListBox_AddString( hWnd, pkey );
		}
	iSel = ListBox_FindStringExact( hWnd, -1, DstProfile );
 	ListBox_SetCurSel( hWnd, iSel == LB_ERR ? 0 : iSel );
	ListBox_GetText( hWnd, ListBox_GetCurSel(hWnd), DstProfile); 
	}
else
	DstProfile[0] = '\0';

// initialize the lut control
Lut_Init(GetDlgItem(hDlg, IDC_MAP), 64, 64);
//CmsInitBlackLuts(hDlg);
if( !CmsPrintSetupGetDst( hDlg, &Separate.cmsInfo.dst ) )
	cmsInfoInit( &Separate.cmsInfo );
}

/************************************************************************/
void CmsPrintSetupProcessMsg( HWND hDlg, ITEMID id )
/************************************************************************/
{
POSITION				pos;
CString				key;
char					*pkey;
CMapStringToPtr	*pPTs;
DWORD					nID;
int 					iSel;

pkey  = key.GetBuffer( BUFFER_SIZE );

switch ( id )
	{
	case IDC_CMS_DSTLIST:
		if( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if( !CmsPrintSetupGetDst( hDlg, &Separate.cmsInfo.dst ) )
			cmsInfoInit( &Separate.cmsInfo );
		Lut_SetLut(GetDlgItem(hDlg, IDC_MAP), 0, NULL);
	//	CmsInitBlackLuts(hDlg);
	break;

	case IDC_CMS_DST:
		if( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		
		ComboBox_GetText( GetDlgItem( hDlg, IDC_CMS_DST ), DstDevice, sizeof(DstDevice) );

		ListBox_ResetContent( GetDlgItem( hDlg, IDC_CMS_DSTLIST ) );

		if( ColorManager.dstMap->Lookup( DstDevice, ( void *& )pPTs ) )
			{
			HWND hWnd = GetDlgItem(hDlg, IDC_CMS_DSTLIST);
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, key, ( LPVOID & )nID );
				ListBox_AddString( hWnd, pkey );
				}
			iSel = ListBox_FindStringExact( hWnd, -1, DstProfile );
 			ListBox_SetCurSel( hWnd, iSel == LB_ERR ? 0 : iSel );
			ListBox_GetText( hWnd, ListBox_GetCurSel(hWnd), DstProfile); 
			}
		Lut_SetLut(GetDlgItem(hDlg, IDC_MAP), 0, NULL);
		if( !CmsPrintSetupGetDst( hDlg, &Separate.cmsInfo.dst ) )
			cmsInfoInit( &Separate.cmsInfo );
	//	CmsInitBlackLuts(hDlg);
	break;

	case IDC_PREVIEW_DST:
		CmsInitBlackLuts(hDlg);
	break;

	case IDOK:
		PutRegStr( pPrintSetupDstName,  DstDevice );
		PutRegStr( pPrintSetupDstProfile,  DstProfile );
		// Reset Print Style CMS switch if no PT Selected
	break;

	case IDCANCEL:
		Separate.cmsInfo = SaveDstInfo;
	break;
   default:

	break;
   }
}

/***********************************************************************/
void CmsInitBlackLuts(HWND hDlg)
/***********************************************************************/
{
	int i, outDepth;
	CFrameTypeConvert TypeConvert;
	FRMTYPEINFO inType, outType;
	RGBS rgb;
	LPRGB lpIn = NULL;
	LPTR lpOut = NULL;

	inType = ColorManager.Monitor.dst;
	if (inType.DataType != FDT_RGBCOLOR)
		return;
	if (!CmsPrintSetupGetDst(hDlg, &outType))
		return;
	if (outType.DataType != FDT_RGBCOLOR && outType.DataType != FDT_CMYKCOLOR)
		return;

	lpIn = (LPRGB)Alloc(256L*3L);
	if (outType.DataType == FDT_CMYKCOLOR)
		outDepth = 4;
	else
		outDepth = 3;
	lpOut = Alloc(256L*outDepth);

	if (lpIn && lpOut && 
		TypeConvert.Init(inType, outType, 256))
	{
		for (i = 0; i < 256; ++i)
			lpIn[i].red = lpIn[i].green = lpIn[i].blue = 255-i;
		TypeConvert.ConvertData((LPTR)lpIn, lpOut, 0, 256);
		Lut_SetLut(GetDlgItem(hDlg, IDC_MAP), outDepth, lpOut);
		if (outType.DataType == FDT_CMYKCOLOR)
		{
			rgb.red = 0; rgb.green = rgb.blue = 255;
			Lut_SetColor(GetDlgItem(hDlg, IDC_MAP), 0, &rgb); 
			rgb.red = rgb.blue = 255; rgb.green = 0;
			Lut_SetColor(GetDlgItem(hDlg, IDC_MAP), 1, &rgb); 
			rgb.red = rgb.green = 255; rgb.blue = 0;
			Lut_SetColor(GetDlgItem(hDlg, IDC_MAP), 2, &rgb); 
			rgb.red = rgb.green = rgb.blue = 0;
			Lut_SetColor(GetDlgItem(hDlg, IDC_MAP), 3, &rgb); 
		}
		else
		{
			rgb.red = 255; rgb.green = rgb.blue = 0;
			Lut_SetColor(GetDlgItem(hDlg, IDC_MAP), 0, &rgb); 
			rgb.red = rgb.blue = 0; rgb.green = 255;
			Lut_SetColor(GetDlgItem(hDlg, IDC_MAP), 1, &rgb); 
			rgb.red = rgb.green = 0; rgb.blue = 255;
			Lut_SetColor(GetDlgItem(hDlg, IDC_MAP), 2, &rgb); 
		}
	}
	if (lpIn)
		FreeUp(lpIn);
	if (lpOut)
		FreeUp(lpOut);
}

/************************************************************************/
LOCAL BOOL CmsPrintSetupGetDst( HWND hDlg, LPFRMTYPEINFO lpDstInfo )
/************************************************************************/
{
CString				key;
char				*pkey;
CMapStringToPtr		*pPTs;
DWORD				nID, nCRC;

pkey  = key.GetBuffer( BUFFER_SIZE );

ComboBox_GetText( GetDlgItem( hDlg, IDC_CMS_DST ), DstDevice, 256 );

if( ColorManager.dstMap->Lookup( DstDevice, ( void *& )pPTs ) )
	{	
	ListBox_GetText( GetDlgItem( hDlg, IDC_CMS_DSTLIST ), ListBox_GetCurSel(
		GetDlgItem( hDlg, IDC_CMS_DSTLIST ) ), DstProfile );

	if( pPTs->Lookup( DstProfile, ( LPVOID & )nID ) )
		{
		lpDstInfo->ptInfo.frRCS = nID;
		if( ColorManager.spcMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
			lpDstInfo->DataType = ( FRMDATATYPE )nCRC;
		if( ColorManager.crcMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
			lpDstInfo->ptInfo.toRCS = nCRC;
		else
			lpDstInfo->ptInfo.toRCS = 0;
		}
	return(TRUE);
	}
else
	DstProfile[0] = '\0';
return(FALSE);
}

/************************************************************************/
void CmsMonitorSetupInit( HWND hDlg )
/************************************************************************/
{
POSITION				pos;
CString				key;
char					*pkey;
CMapStringToPtr	*pPTs;
int iSel;
DWORD					nID;
HWND hWnd;

pkey  = key.GetBuffer( BUFFER_SIZE );

// Build MON's list
ComboBox_AddString( GetDlgItem( hDlg, IDC_CMS_DST ), "(None)" );

for( pos = ColorManager.monMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.monMap->GetNextAssoc( pos, key, ( void* &)pPTs );
	ComboBox_AddString( GetDlgItem( hDlg, IDC_CMS_DST ), pkey );
	}

ComboBox_SetCurSel( GetDlgItem( hDlg, IDC_CMS_DST ),
	ComboBox_FindStringExact( GetDlgItem( hDlg, IDC_CMS_DST ),
	-1, lstrlen( DstDevice ) ? DstDevice : "(None)" ) );

ComboBox_GetText( GetDlgItem( hDlg,	IDC_CMS_DST ), DstDevice, 256 );

if( ColorManager.monMap->Lookup( DstDevice, ( void *& )pPTs ) )
	{
	hWnd = GetDlgItem(hDlg, IDC_CMS_DSTLIST);
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, key, ( void *& )nID );
		ListBox_AddString( hWnd, pkey );
		}
	iSel = ListBox_FindStringExact( hWnd, -1, DstProfile );
 	ListBox_SetCurSel( hWnd, iSel == LB_ERR ? 0 : iSel );
	ListBox_GetText( hWnd, ListBox_GetCurSel(hWnd), DstProfile); 
	}
CmsMonitorSelect(hDlg);
}

/************************************************************************/
void CmsMonitorSetupProcessMsg( HWND hDlg, ITEMID id )
/************************************************************************/
{
POSITION				pos;
CMapStringToPtr	*pPTs;
DWORD					nID;
int					iSel;
char					*pkey;
CString				key;

pkey = key.GetBuffer( 256 );

switch ( id )
	{
	case IDC_CMS_DSTLIST:
		if( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		CmsMonitorSelect(hDlg);
		break;
		
	case IDC_CMS_DST:
		if( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		
		ComboBox_GetText( GetDlgItem( hDlg, IDC_CMS_DST ), DstDevice, 256 );

		ListBox_ResetContent( GetDlgItem( hDlg, IDC_CMS_DSTLIST ) );

		if( ColorManager.monMap->Lookup( DstDevice, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, key, ( LPVOID & )nID );
				ListBox_AddString( GetDlgItem( hDlg, IDC_CMS_DSTLIST ), pkey );
				}
			iSel = ListBox_FindStringExact( GetDlgItem( hDlg, IDC_CMS_DSTLIST ), -1, DstProfile );
		 	ListBox_SetCurSel( GetDlgItem( hDlg, IDC_CMS_DSTLIST ), iSel == LB_ERR ? 0 : iSel );

			}
		ComboBox_GetText( GetDlgItem( hDlg, IDC_CMS_DST ), DstDevice, 256 );

		CmsMonitorSelect(hDlg);
	break;

	case IDOK:
		// may not be our dialog here
		CmsSaveMonitorSelection();
		SaveMonitorSetup();
		CmsSetSuperBltGamma();
	break;

	case IDCANCEL:
		// may not be our dialog here
		ColorManager.Monitor = SaveSrcInfo;
	break;

   default:

	break;
   }
}

/************************************************************************/
LOCAL void CmsMonitorSelect( HWND hDlg )
/************************************************************************/
{
	DWORD nID, nCRC;
	CMapStringToPtr	*pPTs;

	if( ColorManager.monMap->Lookup( DstDevice, ( void *& )pPTs ) )
		{	
		ListBox_GetText( GetDlgItem( hDlg, IDC_CMS_DSTLIST ), ListBox_GetCurSel(
			GetDlgItem( hDlg, IDC_CMS_DSTLIST ) ), DstProfile );

		if( pPTs->Lookup( DstProfile, ( LPVOID & )nID ) )
			{
			ColorManager.Monitor.dst.ptInfo.frRCS = nID;
			if( ColorManager.spcMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
				ColorManager.Monitor.dst.DataType = ( FRMDATATYPE )nCRC;
			if( ColorManager.crcMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
				ColorManager.Monitor.dst.ptInfo.toRCS = nCRC;
			else
				ColorManager.Monitor.dst.ptInfo.toRCS = 0;
			}
		}
	else
		{
		DstProfile[0] = '\0';
		cmsInfoInit( &ColorManager.Monitor );
		}
}

/************************************************************************/
LPSTR CmsGetPTNameFromFile( LPSTR lpFile, LPSTR lpStr )
/************************************************************************/
{
int				Type;
LPTIFFHEADER	lpHdr = NULL;
FRMTYPEINFO		TypeInfo;

if (!AstralStrEx(IDS_UNKNOWN, lpStr, sizeof(STRING)))
	*lpStr = '\0';

Type = GetPPFileType( lpFile );

if( Type == IDN_TIFF )
	{
	CReadImage read(lpFile);
	lpHdr = read.ReadTiffHeader();
	if( lpHdr )
		{
		if( lpHdr->pXform )
			lstrcpy( lpStr, lpHdr->szXformName );
		read.FreeTiffHeader(lpHdr);
		}
	}
else if( Type == IDN_PP || Type == IDN_PPCOMPOUND)
	{
	CReadImage read(lpFile);
	if (read.ReadHeader())
		{
		read.GetCMSSrcTypeInfo(&TypeInfo);
		CmsGetPTName( NULL, lpStr, &TypeInfo );
		}
	}

return( lpStr );
}

/************************************************************************/
LPSTR CmsGetPTName( LPFRAME lpFrame, LPSTR lpStr, LPFRMTYPEINFO lpTypeInfo )
/************************************************************************/
{
POSITION				pos, pos1;
CMapStringToPtr	*pPTs;
LONG					nID;
CString				szName;
char					*p;
FRMTYPEINFO			TypeInfo;

if (!AstralStrEx(IDS_UNKNOWN, lpStr, sizeof(STRING)))
	*lpStr = '\0';

if( !Control.CMSEnabled )
	{
	return( lpStr );
	}
	
if( !lpFrame )
	TypeInfo = *lpTypeInfo;
else
	FrameGetTypeInfo( lpFrame, &TypeInfo );

p = szName.GetBuffer( 256 );

for( pos1 = ColorManager.dstMap->GetStartPosition(); pos1 != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos1, szName, ( LPVOID & )pPTs );
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, szName, ( LPVOID & )nID );
		if( TypeInfo.ptInfo.frRCS == nID )
			{
			lstrcpy( lpStr, p );
			return( lpStr );
			}
		}
	}
return( lpStr );
}

/************************************************************************/
BOOL CmsGetSrcPTType( LPFRMTYPEINFO lpSrcType, LPCMSINFO pCmsInfo)
/************************************************************************/
{
							  
if( lpSrcType->DataType != FDT_RGBCOLOR &&
		lpSrcType->DataType != FDT_CMYKCOLOR )
	return( FALSE );

pCmsInfo->src = *lpSrcType;

do
	{
	CCmsPasteDlg	PasteDlg( pCmsInfo,
		CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );
	PasteDlg.DoModal();
	if( !PasteDlg.GetReturn() )
		{
		cmsInfoInit( pCmsInfo );
		return( FALSE );
		}
	else
		{
		if( lpSrcType->DataType != pCmsInfo->src.DataType )
			Message( IDS_CMS_EFILEPTMM );
		else
			{
			return( TRUE );
			}
		}
	}
while( 1 );

return( TRUE );
}

/************************************************************************/
void CmsGetSrcPT( LPSTR lpImageFile, LPCMSINFO pCmsInfo)
/************************************************************************/
{
LPTIFFHEADER	lpHdr = NULL;
BOOL				bRetry;
ITEMID			idFileType;

idFileType = GetPPFileType(lpImageFile);

cmsInfoInit( pCmsInfo );
switch( idFileType )
	{
	case IDN_PHOTOCD:
			{
			CCmsPCDPasteDlg PasteDlg( pCmsInfo,
				CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );
		  PasteDlg.DoModal();
		  if( !PasteDlg.GetReturn() )
				cmsInfoInit(pCmsInfo);
			}
	break;
	case IDN_PP:
    case IDN_PPCOMPOUND:
		{
			CReadImage read(lpImageFile);
			if (read.ReadHeader())
			{
				FRMTYPEINFO		SrcTypeInfo;

				read.GetCMSSrcTypeInfo(&SrcTypeInfo);
				if (IsSrcPTSelected(&SrcTypeInfo.ptInfo))
				{
					pCmsInfo->src = SrcTypeInfo;
					return;
				}
			}
		}
		// fall thru
	case IDN_TIFF:
	case IDN_PP4:
		{
		CReadImage read(lpImageFile);
		if (idFileType == IDN_TIFF)
			lpHdr = read.ReadTiffHeader();
		else
		if (idFileType == IDN_PP4)
			lpHdr = read.ReadObjHeader();
		if ( lpHdr )
			{
			if( lpHdr->pXform == NULL && ( lpHdr->inType.DataType == FDT_RGBCOLOR ||
				lpHdr->inType.DataType == FDT_CMYKCOLOR ) )
				{
				pCmsInfo->src.DataType = lpHdr->inType.DataType;

				do
					{
					CCmsPasteDlg	PasteDlg( pCmsInfo,
						CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );
					PasteDlg.DoModal();
					if( !PasteDlg.GetReturn() )
						{
						cmsInfoInit( pCmsInfo );
						bRetry = FALSE;
						}
					else
						{
						if( lpHdr->inType.DataType != pCmsInfo->src.DataType )
							Message( IDS_CMS_EFILEPTMM );
						else
							{
							bRetry = FALSE;
							}
						}
					}
				while( bRetry );
				}
			read.FreeTiffHeader(lpHdr);
			}
		else
			return;
		}
	break;

	default:

		pCmsInfo->src.DataType = FDT_RGBCOLOR;

		do
			{
			CCmsPasteDlg	PasteDlg( pCmsInfo,
				CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );
			PasteDlg.DoModal();
			if( !PasteDlg.GetReturn() )
				{
				bRetry = FALSE;
				}
			else
				{
				if( pCmsInfo->src.DataType != FDT_RGBCOLOR )
					Message( IDS_CMS_EFILEPTMM );
				else
					{
					bRetry = FALSE;
					}
				}

			}
		while( bRetry );
	break;
	}
}
	
/************************************************************************/
BOOL CmsGetDstPT(LPCMSINFO pCmsInfo)
/************************************************************************/
{
CCmsConvertDlg	ConvertDlg( pCmsInfo, CWnd::FromHandle( PictPubApp.Get_hWndAstral()	) );

ConvertDlg.DoModal();

return( ConvertDlg.GetReturn() );
}

/************************************************************************/
BOOL CmsGetSrcDstPT( LPCMSINFO pCmsInfo )
/************************************************************************/
{
CCmsConvertToDlg	Dlg( pCmsInfo, CWnd::FromHandlePermanent( PictPubApp.Get_hWndAstral() ) );

Dlg.DoModal();

return( Dlg.GetReturn() );
}

/************************************************************************/
void cmsGetSrcPT( HWND hDlg, LPCMSINFO pcmsInfo, CMapStringToPtr *pMap )
/************************************************************************/
{
CString				key;
char					*pkey;
CMapStringToPtr	*pPTs;
DWORD					nID, nCRC;

pkey  = key.GetBuffer( BUFFER_SIZE );

ComboBox_GetText( GetDlgItem( hDlg, IDC_CMS_SRC ), pkey, 256 );

if( pMap->Lookup( key, ( void *& )pPTs ) )
	{	
	ListBox_GetText( GetDlgItem( hDlg, IDC_CMS_SRCLIST ), ListBox_GetCurSel(
		GetDlgItem( hDlg, IDC_CMS_SRCLIST ) ), pkey );

	if( pPTs->Lookup( key, ( LPVOID & )nID ) )
		{
		pcmsInfo->src.ptInfo.toRCS = nID;
		if( ColorManager.spcMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
			pcmsInfo->src.DataType = ( FRMDATATYPE )nCRC;
		if( ColorManager.crcMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
			pcmsInfo->src.ptInfo.frRCS = nCRC;
		else
			pcmsInfo->src.ptInfo.frRCS = 0;
		}
	}

}

/************************************************************************/
void cmsGetDstPT( HWND hDlg, LPCMSINFO pcmsInfo )
/************************************************************************/
{
CString				key;
char					*pkey;
CMapStringToPtr	*pPTs;
DWORD					nID, nCRC;

pkey  = key.GetBuffer( BUFFER_SIZE );

ComboBox_GetText( GetDlgItem( hDlg, IDC_CMS_DST ), pkey, 256 );

if( ColorManager.dstMap->Lookup( key, ( void *& )pPTs ) )
	{	
	ListBox_GetText( GetDlgItem( hDlg, IDC_CMS_DSTLIST ), ListBox_GetCurSel(
		GetDlgItem( hDlg, IDC_CMS_DSTLIST ) ), pkey );

	if( pPTs->Lookup( key, ( LPVOID & )nID ) )
		{
		pcmsInfo->dst.ptInfo.frRCS = nID;
		if( ColorManager.spcMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
			pcmsInfo->dst.DataType = ( FRMDATATYPE )nCRC;
		if( ColorManager.crcMap->Lookup( ( LPVOID )nID, ( LPVOID & )nCRC ) )
			pcmsInfo->dst.ptInfo.toRCS = nCRC;
		else
			pcmsInfo->dst.ptInfo.toRCS = 0;
		}
	}

}

/************************************************************************/
void CmsThreadInit()
/************************************************************************/
{
}

/************************************************************************/
void CmsThreadTerm()
/************************************************************************/
{
}

/************************************************************************/
BOOL CmsGetMenuState( LPIMAGE lpImage )
/************************************************************************/
{
FRMTYPEINFO	TypeInfo;

ImgGetTypeInfo( lpImage, &TypeInfo );

return( Control.CMSEnabled &&
			!IsDstPTSelected( &TypeInfo.ptInfo ) &&
			lpImage );
}

/************************************************************************/
BOOL ConvertCRCtoUID( LPFRMTYPEINFO SrcType, LPFRMTYPEINFO DstType )
/************************************************************************/
{

if( SrcType != NULL )
	{
	if( !ColorManager.uidMap->Lookup( ( LPVOID )SrcType->ptInfo.toRCS,
		  		( LPVOID & )SrcType->ptInfo.toRCS ) )
		return( FALSE );
	}	

if( DstType != NULL )
	{	
	return( ColorManager.uidMap->Lookup( ( LPVOID )DstType->ptInfo.toRCS,
		( LPVOID & )DstType->ptInfo.toRCS ) &&
	ColorManager.uidMap->Lookup( ( LPVOID )DstType->ptInfo.frRCS,
		( LPVOID & )DstType->ptInfo.frRCS ) );
	}
else
	return( TRUE );
}

/************************************************************************/
LONG	ConvertCRC( LONG lCrc )
/************************************************************************/
{
LONG	UID;

ColorManager.uidMap->Lookup( ( LPVOID )lCrc, ( LPVOID	& )UID );

return( UID );
}

/************************************************************************/
void CmsMonitorSetup_OnInitDialog( HWND hDlg )
/************************************************************************/
{
	SaveSrcInfo = ColorManager.Monitor;	
	GetRegStr( pMonitorSetupDstName, DstDevice, "" );
	GetRegStr( pMonitorSetupDstProfile, DstProfile, "" );
}

/************************************************************************/
void CmsSaveMonitorSelection( )
/************************************************************************/
{
	PutRegStr( pMonitorSetupDstName,  DstDevice );
	PutRegStr( pMonitorSetupDstProfile,  DstProfile );
}

/************************************************************************/
void CmsSavePhotoCDSelection( HWND hDlg )
/************************************************************************/
{
	PutRegStr( pPhotoCDOpenSrcName,  SrcDevice );
	PutRegStr( pPhotoCDOpenSrcProfile,  SrcProfile );
	PutRegStr( pPhotoCDOpenDstName,  DstDevice );
	PutRegStr( pPhotoCDOpenDstProfile,  DstProfile );
}

/***********************************************************************/
void LoadScanSetup( void )
/***********************************************************************/
{
int	Hi, Lo;

GetRegInt( pScanSrcDataTypeHi,	Hi, -1 );
ColorManager.Scan.src.DataType = ( FRMDATATYPE )Hi;

GetRegInt( pScanSrcToRCSHi, 		Hi, 0 );
GetRegInt( pScanSrcToRCSLo, 		Lo, 0 );
ColorManager.Scan.src.ptInfo.toRCS = MAKELONG( Lo, Hi );

GetRegInt( pScanSrcFrRCSHi, 		Hi, 0 );
GetRegInt( pScanSrcFrRCSLo, 		Lo, 0 );
ColorManager.Scan.src.ptInfo.frRCS = MAKELONG( Lo, Hi );

GetRegInt( pScanDstDataTypeHi,	Hi, -1 );
ColorManager.Scan.dst.DataType = ( FRMDATATYPE )Hi;

GetRegInt( pScanDstToRCSHi, 		Hi, 0 );
GetRegInt( pScanDstToRCSLo, 		Lo, 0 );
ColorManager.Scan.dst.ptInfo.toRCS = MAKELONG( Lo, Hi );

GetRegInt( pScanDstFrRCSHi, 		Hi, 0 );
GetRegInt( pScanDstFrRCSLo, 		Lo, 0 );
ColorManager.Scan.dst.ptInfo.frRCS = MAKELONG( Lo, Hi );

}

/***********************************************************************/
void LoadPhotoCDSetup( void )
/***********************************************************************/
{
int	Hi, Lo;

GetRegInt( pPhotoCDSrcDataTypeHi,	Hi, -1 );
GetRegInt( pPhotoCDSrcDataTypeLo,	Lo, -1 );
ColorManager.PhotoCD.src.DataType = ( FRMDATATYPE )MAKELONG( Lo, Hi );

GetRegInt( pPhotoCDSrcToRCSHi, 		Hi, 0 );
GetRegInt( pPhotoCDSrcToRCSLo, 		Lo, 0 );
ColorManager.PhotoCD.src.ptInfo.toRCS = MAKELONG( Lo, Hi );

GetRegInt( pPhotoCDSrcFrRCSHi, 		Hi, 0 );
GetRegInt( pPhotoCDSrcFrRCSLo, 		Lo, 0 );
ColorManager.PhotoCD.src.ptInfo.frRCS = MAKELONG( Lo, Hi );

GetRegInt( pPhotoCDDstDataTypeHi,	Hi, -1 );
GetRegInt( pPhotoCDDstDataTypeLo,	Lo, -1 );
ColorManager.PhotoCD.dst.DataType = ( FRMDATATYPE )MAKELONG( Lo, Hi );

GetRegInt( pPhotoCDDstToRCSHi, 		Hi, 0 );
GetRegInt( pPhotoCDDstToRCSLo, 		Lo, 0 );
ColorManager.PhotoCD.dst.ptInfo.toRCS = MAKELONG( Lo, Hi );

GetRegInt( pPhotoCDDstFrRCSHi, 		Hi, 0 );
GetRegInt( pPhotoCDDstFrRCSLo, 		Lo, 0 );
ColorManager.PhotoCD.dst.ptInfo.frRCS = MAKELONG( Lo, Hi );

}

/***********************************************************************/
void LoadMonitorSetup( void )
/***********************************************************************/
{
int	Hi, Lo;

GetRegInt( pMonitorDstDataTypeHi,	Hi, -1 );
GetRegInt( pMonitorDstDataTypeLo,	Lo, -1 );
ColorManager.Monitor.dst.DataType = ( FRMDATATYPE )MAKELONG( Lo, Hi );

GetRegInt( pMonitorDstToRCSHi, 		Hi, 0 );
GetRegInt( pMonitorDstToRCSLo, 		Lo, 0 );
ColorManager.Monitor.dst.ptInfo.toRCS = MAKELONG( Lo, Hi );

GetRegInt( pMonitorDstFrRCSHi, 		Hi, 0 );
GetRegInt( pMonitorDstFrRCSLo, 		Lo, 0 );
ColorManager.Monitor.dst.ptInfo.frRCS = MAKELONG( Lo, Hi );

}

/***********************************************************************/
void SaveScanSetup( void )
/***********************************************************************/
{

// Scanner settings
PutRegInt( pScanSrcDataTypeHi,	ColorManager.Scan.src.DataType );
PutRegInt( pScanSrcToRCSHi, 		HIWORD( ColorManager.Scan.src.ptInfo.toRCS ) );
PutRegInt( pScanSrcToRCSLo, 		LOWORD( ColorManager.Scan.src.ptInfo.toRCS ) );
PutRegInt( pScanSrcFrRCSHi, 		HIWORD( ColorManager.Scan.src.ptInfo.frRCS ) );
PutRegInt( pScanSrcFrRCSLo, 		LOWORD( ColorManager.Scan.src.ptInfo.frRCS ) );

PutRegInt( pScanDstDataTypeHi,	ColorManager.Scan.dst.DataType );
PutRegInt( pScanDstToRCSHi, 		HIWORD( ColorManager.Scan.dst.ptInfo.toRCS ) );
PutRegInt( pScanDstToRCSLo, 		LOWORD( ColorManager.Scan.dst.ptInfo.toRCS ) );
PutRegInt( pScanDstFrRCSHi, 		HIWORD( ColorManager.Scan.dst.ptInfo.frRCS ) );
PutRegInt( pScanDstFrRCSLo, 		LOWORD( ColorManager.Scan.dst.ptInfo.frRCS ) );

}

/***********************************************************************/
void SavePhotoCDSetup( void )
/***********************************************************************/
{

// PhotoCD settings
PutRegInt( pPhotoCDSrcDataTypeHi,		HIWORD( ColorManager.PhotoCD.src.DataType ) );
PutRegInt( pPhotoCDSrcDataTypeLo,		LOWORD( ColorManager.PhotoCD.src.DataType ) );
PutRegInt( pPhotoCDSrcToRCSHi, 		HIWORD( ColorManager.PhotoCD.src.ptInfo.toRCS ) );
PutRegInt( pPhotoCDSrcToRCSLo, 		LOWORD( ColorManager.PhotoCD.src.ptInfo.toRCS ) );
PutRegInt( pPhotoCDSrcFrRCSHi, 		HIWORD( ColorManager.PhotoCD.src.ptInfo.frRCS ) );
PutRegInt( pPhotoCDSrcFrRCSLo, 		LOWORD( ColorManager.PhotoCD.src.ptInfo.frRCS ) );

PutRegInt( pPhotoCDDstDataTypeHi,		HIWORD( ColorManager.PhotoCD.dst.DataType ) );
PutRegInt( pPhotoCDDstDataTypeLo,		LOWORD( ColorManager.PhotoCD.dst.DataType ) );
PutRegInt( pPhotoCDDstToRCSHi, 		HIWORD( ColorManager.PhotoCD.dst.ptInfo.toRCS ) );
PutRegInt( pPhotoCDDstToRCSLo, 		LOWORD( ColorManager.PhotoCD.dst.ptInfo.toRCS ) );
PutRegInt( pPhotoCDDstFrRCSHi, 		HIWORD( ColorManager.PhotoCD.dst.ptInfo.frRCS ) );
PutRegInt( pPhotoCDDstFrRCSLo, 		LOWORD( ColorManager.PhotoCD.dst.ptInfo.frRCS ) );

}

/***********************************************************************/
void SaveMonitorSetup( void )
/***********************************************************************/
{

// Monitor settings
PutRegInt( pMonitorDstDataTypeHi,  	HIWORD( ColorManager.Monitor.dst.DataType ) );
PutRegInt( pMonitorDstDataTypeLo,  	LOWORD( ColorManager.Monitor.dst.DataType ) );
PutRegInt( pMonitorDstToRCSHi, 		HIWORD( ColorManager.Monitor.dst.ptInfo.toRCS ) );
PutRegInt( pMonitorDstToRCSLo, 		LOWORD( ColorManager.Monitor.dst.ptInfo.toRCS ) );
PutRegInt( pMonitorDstFrRCSHi, 		HIWORD( ColorManager.Monitor.dst.ptInfo.frRCS ) );
PutRegInt( pMonitorDstFrRCSLo, 		LOWORD( ColorManager.Monitor.dst.ptInfo.frRCS ) );

}

