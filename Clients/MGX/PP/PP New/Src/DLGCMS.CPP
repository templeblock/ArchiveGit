/***************************************************************************/

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

/***************************************************************************/

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "dlgcms.h"

/***************************************************************************/

/***********************************************************************/
// LOCAL data
/***********************************************************************/
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

/************************************************************************/
CCmsDlg::CCmsDlg( int nDialogId, CWnd *pParentWnd ) :
	CPPModalDlg( nDialogId, pParentWnd )
/************************************************************************/
{
}

/************************************************************************/
void CCmsDlg::ControlGetText( UINT nId )
/************************************************************************/
{

GetDlgItemText( nId, ( LPTSTR )LPCTSTR( string ), 256 );
}

/************************************************************************/
void CCmsDlg::ControlGetText( HWND hControl )
/************************************************************************/
{

::GetWindowText( hControl, ( LPTSTR )LPCTSTR( string ), 256 );
}

/************************************************************************/
void CCmsDlg::ControlGetParentText( int nId )
/************************************************************************/
{

::GetDlgItemText( GetParent()->m_hWnd, nId, ( LPTSTR )LPCTSTR( string ), 256 );
}

/************************************************************************/
void CCmsDlg::ComboBoxAddString( UINT nId )
/************************************************************************/
{

SendDlgItemMessage( nId, CB_ADDSTRING, 0, ( LPARAM )( LPCTSTR )string );
}

/************************************************************************/
void CCmsDlg::ComboBoxAddString( UINT nId, LPCTSTR string )
/************************************************************************/
{

SendDlgItemMessage( nId, CB_ADDSTRING, 0, ( LPARAM )( LPCTSTR )string );
}

/************************************************************************/
void CCmsDlg::ComboBoxSelectString( UINT nId, LPCTSTR string )
/************************************************************************/
{

SendDlgItemMessage( nId, CB_SETCURSEL, SendDlgItemMessage( nId,
	CB_FINDSTRINGEXACT, ( WPARAM )-1, ( LPARAM )( LPCTSTR )string ), 0 );
}

/************************************************************************/
void CCmsDlg::ListBoxAddString( UINT nId )
/************************************************************************/
{

SendDlgItemMessage( nId, LB_ADDSTRING, 0, ( LPARAM )( LPCTSTR )string );
}

/************************************************************************/
int CCmsDlg::ListBoxFindStringExact( int nId )
/************************************************************************/
{

return( SendDlgItemMessage( nId, CB_FINDSTRINGEXACT, 0,
	( LPARAM )( LPTSTR )LPCTSTR( string ) ) );
}

/************************************************************************/
void CCmsDlg::ListBoxSetCurSel( UINT nId, int index )
/************************************************************************/
{

SendDlgItemMessage( nId, LB_SETCURSEL, ( WPARAM )index, 0 );
}

/************************************************************************/
void CCmsDlg::ListBoxResetContent( UINT nId )
/************************************************************************/
{

SendDlgItemMessage( nId, LB_RESETCONTENT, 0, 0 );
}

/************************************************************************/
int CCmsDlg::ComboBoxFindStringExact( int nId )
/************************************************************************/
{

return( SendDlgItemMessage( nId, CB_FINDSTRINGEXACT, 0,
	( LPARAM )( LPTSTR )LPCTSTR( string ) ) );
}

/************************************************************************/
void CCmsDlg::ComboBoxSetCurSel( UINT nId, int index )
/************************************************************************/
{

SendDlgItemMessage( nId, CB_SETCURSEL, ( WPARAM )index, 0 );
}

/************************************************************************/
void CCmsDlg::StaticSetText( UINT nId )
/************************************************************************/
{

SetDlgItemText( nId, LPCTSTR( string ) );
}

/************************************************************************/
void CCmsDlg::ListBoxGetSelectedString( UINT nId )
/************************************************************************/
{

::SendMessage( ::GetDlgItem( GetParent()->m_hWnd, IDC_CMS_SRCLIST ), LB_GETTEXT,
	::SendDlgItemMessage( GetParent()->m_hWnd, IDC_CMS_SRCLIST, LB_GETCURSEL, 0, 0 ),
	( LPARAM )( LPTSTR )LPCTSTR( string ) );
}

/************************************************************************/
void CCmsDlg::PCDSaveSettings()
/************************************************************************/
{

ControlGetText( IDC_CMS_SRC );
PutRegStr( pPhotoCDOpenSrcName, ( LPTSTR )LPCTSTR( string ) );
ListBoxGetSelectedString( IDC_CMS_SRCLIST );
PutRegStr( pPhotoCDOpenSrcProfile, ( LPTSTR )LPCTSTR( string ) );

ControlGetText( IDC_CMS_DST );
PutRegStr( pPhotoCDOpenDstName, ( LPTSTR )LPCTSTR( string ) );
ListBoxGetSelectedString( IDC_CMS_DSTLIST );
PutRegStr( pPhotoCDOpenDstProfile, ( LPTSTR )LPCTSTR( string ) );
}

/************************************************************************/
CCmsFileOpenDlg::CCmsFileOpenDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRCDST, pParentWnd )
/************************************************************************/
{
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsFileOpenDlg::OnInitDialog()
/************************************************************************/
{

CenterPopup( m_hWnd );

// Build SRC's list
ComboBoxAddString( IDC_CMS_SRC, ( LPCTSTR )szDefaultSrc );

for( pos = ColorManager.srcMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.srcMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_SRC );
	}

GetRegStr( pFileOpenSrcName, ( LPTSTR )LPCTSTR( string ), "" );

ComboBoxSelectString( IDC_CMS_SRC, lstrlen( LPCTSTR( string ) )?
	string : szDefaultSrc );

ControlGetText( IDC_CMS_SRC );

if( ColorManager.srcMap->Lookup( string, ( void * &)pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_SRCLIST );
		}
	ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
	}

// Build DST's list
ComboBoxAddString( IDC_CMS_DST, ( LPCTSTR )szDefaultDst );

for( pos = ColorManager.dstMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_DST );
	}

GetRegStr( pFileOpenDstName,  ( LPTSTR )LPCTSTR( string ), "" );

ComboBoxSelectString( IDC_CMS_DST, lstrlen( ( LPCTSTR )string ) ?
	string : szDefaultDst );

ControlGetText( IDC_CMS_DST );

if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_DSTLIST );
		}
	ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsFileOpenDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_SRC:
		if ( nCodeNotify != CBN_SELCHANGE )
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( IDC_CMS_SRC );

		ListBoxResetContent( IDC_CMS_SRCLIST );

		if( ColorManager.srcMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_SRCLIST );
				}
			ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
			}
	break;

	case IDC_CMS_DST:
		if (nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_DSTLIST );

		if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_DSTLIST );
				}
			ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
			}
	break;

	default:

	break;
	}

return( CPPModalDlg::OnCommand( wParam, lParam ) );
}

/************************************************************************/
void CCmsFileOpenDlg::OnOK()
/************************************************************************/
{

cmsInfoInit( pCmsInfo );
cmsGetSrcPT( m_hWnd, pCmsInfo, ColorManager.srcMap );
cmsGetDstPT( m_hWnd, pCmsInfo );

if( !pCmsInfo->src.ptInfo.toRCS )
	{
	if( pCmsInfo->dst.ptInfo.frRCS )
		{
		Message( IDS_CMS_NOSRCDEST );
		}
	else
		{
		ControlGetText( IDC_CMS_SRC );
		PutRegStr( pFileOpenSrcName, string );
		ControlGetText( IDC_CMS_DST );
		PutRegStr( pFileOpenDstName, string );
		bReturn = TRUE;
		CPPModalDlg::OnOK();
		}
	}
else
	{
	if( pCmsInfo->dst.ptInfo.frRCS )
		{
		ControlGetText( IDC_CMS_SRC );
		PutRegStr( pFileOpenSrcName, string );
		ControlGetText( IDC_CMS_DST );
		PutRegStr( pFileOpenDstName, string );
		bReturn = TRUE;
		CPPModalDlg::OnOK();
		}
	else
		{
		if( IsDstPTSelected( &ColorManager.Monitor.dst.ptInfo ) )
			{
			pCmsInfo->dst = ColorManager.Monitor.dst;
			ControlGetText( IDC_CMS_SRC );
			PutRegStr( pFileOpenSrcName, string );
			ControlGetText( IDC_CMS_DST );
			PutRegStr( pFileOpenDstName, string );
			bReturn = TRUE;
			CPPModalDlg::OnOK();
			}
		else
			{
			Message( IDS_CMS_NODESTNOMON );
			}
		}
	}
}

/************************************************************************/
void CCmsFileOpenDlg::OnCancel()
/************************************************************************/
{
cmsInfoInit( pCmsInfo );
CPPModalDlg::OnCancel();
bReturn = FALSE;
}

/************************************************************************/
CCmsConvertToDlg::CCmsConvertToDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRCDST, pParentWnd )
/************************************************************************/
{
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsConvertToDlg::OnInitDialog()
/************************************************************************/
{

CenterPopup( m_hWnd );

for( pos = ColorManager.srcMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.srcMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	pos1 = pPTs->GetStartPosition(); 
	pPTs->GetNextAssoc( pos1, string1, ( void *& )dwId );
	ColorManager.spcMap->Lookup( ( LPVOID )dwId, ( LPVOID & )nCrc );
	if( nCrc == pCmsInfo->src.DataType )
		ComboBoxAddString( IDC_CMS_SRC );
	}

GetRegStr( pConvertToSrcName,  ( LPTSTR )LPCTSTR( string ), "" );

iSel = ComboBoxFindStringExact( IDC_CMS_SRC );

ComboBoxSetCurSel( IDC_CMS_SRC, iSel == CB_ERR ? 0 : iSel );

ControlGetText( IDC_CMS_SRC );

if( ColorManager.srcMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_SRCLIST );
		}
	ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
	}

// Build DST's list
ComboBoxAddString( IDC_CMS_DST, szDefaultDst );

for( pos = ColorManager.dstMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_DST );
	}

GetRegStr( pConvertToDstName,  ( LPTSTR )LPCTSTR( string ), "" );

ComboBoxSelectString( IDC_CMS_DST, lstrlen( LPCTSTR( string ) ) ?
	string : szDefaultDst );

ControlGetText( IDC_CMS_DST );

if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_DSTLIST );
		}
	ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsConvertToDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_SRC:
		if( nCodeNotify != CBN_SELCHANGE )
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_SRCLIST );

		if( ColorManager.srcMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_SRCLIST );
				}
			ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
			}
	break;

	case IDC_CMS_DST:
		if (nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_DSTLIST );

		if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_DSTLIST );
				}
			ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
			}
	break;

   default:
	break;
   }
return( CPPModalDlg::OnCommand( wParam, lParam ) );
}

/************************************************************************/
void CCmsConvertToDlg::OnOK()
/************************************************************************/
{

cmsInfoInit( pCmsInfo );
cmsGetSrcPT( m_hWnd, pCmsInfo, ColorManager.srcMap );
cmsGetDstPT( m_hWnd, pCmsInfo );

if( !pCmsInfo->src.ptInfo.toRCS )
	{
	if( pCmsInfo->dst.ptInfo.frRCS )
		{
		Message( IDS_CMS_NOSRCDEST );
		}
	else
		{
		ControlGetText( IDC_CMS_SRC );
		PutRegStr( pConvertToSrcName, LPCTSTR( string ) );
		ControlGetText( IDC_CMS_DST );
		PutRegStr( pConvertToDstName, LPCTSTR( string ) );
		bReturn = TRUE;
		CPPModalDlg::OnOK();
		}
	}
else
	{
	if( pCmsInfo->dst.ptInfo.frRCS )
		{
		ControlGetText( IDC_CMS_SRC );
		PutRegStr( pConvertToSrcName, LPCTSTR( string ) );
		ControlGetText( IDC_CMS_DST );
		PutRegStr( pConvertToDstName, LPCTSTR( string ) );
		bReturn = TRUE;
		CPPModalDlg::OnOK();
		}
	else
		{
		if( IsDstPTSelected( &ColorManager.Monitor.dst.ptInfo ) )
			{
			pCmsInfo->dst = ColorManager.Monitor.dst;
			ControlGetText( IDC_CMS_SRC );
			PutRegStr( pConvertToSrcName, LPCTSTR( string ) );
			ControlGetText( IDC_CMS_DST );
			PutRegStr( pConvertToDstName, LPCTSTR( string ) );
			bReturn = TRUE;
			CPPModalDlg::OnOK();
			}
		else
			{
			Message( IDS_CMS_NODESTNOMON );
			}
		}
	}
}

/************************************************************************/
void CCmsConvertToDlg::OnCancel()
/************************************************************************/
{
cmsInfoInit( pCmsInfo );
bReturn = FALSE;
CPPModalDlg::OnCancel();
}

/************************************************************************/
CCmsInfoDlg::CCmsInfoDlg( BOOL bSrc, BOOL bDst, CWnd *pParentWnd ) :
	CCmsDlg( bSrc ? bDst ? IDD_CMS_SRCDSTINFO : IDD_CMS_SRCINFO : IDD_CMS_DSTINFO,
	pParentWnd )
/************************************************************************/
{

bDoSrc = bSrc;
bDoDst = bDst;
}

/************************************************************************/
BOOL CCmsInfoDlg::OnInitDialog()
/************************************************************************/
{

CenterPopup( m_hWnd );

if( bDoSrc )
	{
	ControlGetParentText( IDC_CMS_SRC );

	ListBoxResetContent( IDC_CMS_SRCINFO );

	if( !( ColorManager.srcMap->Lookup( string, ( void *& )pPTs ) ) )
		return( FALSE );

	ListBoxGetSelectedString( IDC_CMS_SRCLIST );

	pPTs->Lookup( string, ( LPVOID & )dwId );

	if( !ColorManager.uidMap->Lookup( ( LPVOID )dwId, ( LPVOID & )nCrc ) )
		return( FALSE );

	StaticSetText( IDC_CMS_SRCNAME );

	iSel = 0;

	while( ( *KCMSGetAttribute )( ( LPTSTR )LPCTSTR( string ), iSel++, nCrc, FIL ) )
		ListBoxAddString( IDC_CMS_SRCINFO );
	}

if( bDoDst )
	{
	ControlGetParentText( IDC_CMS_DST );

	if( !( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) ) )
		return( FALSE );

	ListBoxGetSelectedString( IDC_CMS_DSTLIST );

	pPTs->Lookup( string, ( LPVOID & )dwId );

	if( !ColorManager.uidMap->Lookup( ( LPVOID )dwId, ( LPVOID & )nCrc ) )
		return( FALSE );

	StaticSetText( IDC_CMS_DSTNAME );

	iSel = 0;

	while( ( *KCMSGetAttribute )( ( LPTSTR )LPCTSTR( string ), iSel++, nCrc, DST ) )
		ListBoxAddString( IDC_CMS_DSTINFO );
	}

return(TRUE);
}

/************************************************************************/
CCmsPasteDlg::CCmsPasteDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRC, pParentWnd )
/************************************************************************/
{

pInfo = new CCmsInfoDlg( YES, NO, this );
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsPasteDlg::OnInitDialog()
/************************************************************************/
{	  

CenterPopup( m_hWnd );

for( pos = ColorManager.srcMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.srcMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	pos1 = pPTs->GetStartPosition(); 
	pPTs->GetNextAssoc( pos1, string1, ( void *& )dwId );
	ColorManager.spcMap->Lookup( ( LPVOID )dwId, ( LPVOID & )nCrc );
	if( nCrc == pCmsInfo->src.DataType )
		ComboBoxAddString( IDC_CMS_SRC );
	}

GetRegStr( pPasteSrcName,  ( LPTSTR )LPCTSTR( string ), "" );

iSel = ComboBoxFindStringExact( IDC_CMS_SRC );

ComboBoxSetCurSel( IDC_CMS_SRC, iSel == CB_ERR ? 0 : iSel );

ControlGetText( IDC_CMS_SRC );

if( ColorManager.srcMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_SRCLIST );
		}
	ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsPasteDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{ 
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_SRC:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_SRCLIST );

		if( ColorManager.srcMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_SRCLIST );
				}
			ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
			}
	break;

   default:

	break;
   }

return( CPPModalDlg::OnCommand( wParam, lParam ) );
}

/************************************************************************/
void CCmsPasteDlg::OnOK()
/************************************************************************/
{
 
cmsInfoInit( pCmsInfo );
cmsGetSrcPT( m_hWnd, pCmsInfo, ColorManager.srcMap );
ControlGetText( IDC_CMS_SRC );
PutRegStr( pPasteSrcName, ( LPTSTR )LPCTSTR( string ) );
bReturn = TRUE;
CPPModalDlg::OnOK();
}

/************************************************************************/
void CCmsPasteDlg::OnCancel()
/************************************************************************/
{ 

cmsInfoInit( pCmsInfo );
bReturn = FALSE;
CPPModalDlg::OnCancel();
}

/************************************************************************/
CCmsPCDPasteDlg::CCmsPCDPasteDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRC, pParentWnd )
/************************************************************************/
{

pInfo = new CCmsInfoDlg( YES, NO, this );
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsPCDPasteDlg::OnInitDialog()
/************************************************************************/
{	  

CenterPopup( m_hWnd );

for( pos = ColorManager.pcdMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.pcdMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_SRC );
	}

GetRegStr( pPCDPasteSrcName, ( LPTSTR )LPCTSTR( string ), "" );

iSel = ComboBoxFindStringExact( IDC_CMS_SRC );

ComboBoxSetCurSel( IDC_CMS_SRC, iSel == CB_ERR ? 0 : iSel );

ControlGetText( IDC_CMS_SRC );

if( ColorManager.pcdMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_SRCLIST );
		}
	ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsPCDPasteDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{ 
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_SRC:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_SRCLIST );

		if( ColorManager.pcdMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_SRCLIST );
				}
			ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
			}
	break;

   default:

	break;
   }
return CPPModalDlg::OnCommand( wParam, lParam );
}

/************************************************************************/
void CCmsPCDPasteDlg::OnOK()
/************************************************************************/
{

cmsInfoInit( pCmsInfo );
cmsGetSrcPT( m_hWnd, pCmsInfo, ColorManager.pcdMap );
ControlGetText( IDC_CMS_SRC );
PutRegStr( pPCDPasteSrcName, ( LPTSTR )LPCTSTR( string ) );
bReturn = TRUE;
CPPModalDlg::OnOK();
}

/************************************************************************/
void CCmsPCDPasteDlg::OnCancel()
/************************************************************************/
{
 
cmsInfoInit( pCmsInfo );
bReturn = FALSE;
CPPModalDlg::OnCancel();
}

/************************************************************************/
CCmsFileNewDlg::CCmsFileNewDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRC, pParentWnd )
/************************************************************************/
{

pInfo = new CCmsInfoDlg( NO, YES, this );
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsFileNewDlg::OnInitDialog()
/************************************************************************/
{	  

CenterPopup( m_hWnd );

for( pos = ColorManager.dstMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	pos1 = pPTs->GetStartPosition(); 
	pPTs->GetNextAssoc( pos1, string1, ( void *& )dwId );
	ColorManager.spcMap->Lookup( ( LPVOID )dwId, ( LPVOID & )nCrc );
	if( nCrc == pCmsInfo->src.DataType )
		ComboBoxAddString( IDC_CMS_DST );
	}

GetRegStr( pFileNewSetupDstName,  ( LPTSTR )LPCTSTR ( string ), "" );

iSel = ComboBoxFindStringExact( IDC_CMS_DST );

ComboBoxSetCurSel( IDC_CMS_DST, iSel == CB_ERR ? 0 : iSel );

ControlGetText( IDC_CMS_DST );

if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_DSTLIST );
		}
	ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsFileNewDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{ 
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_DST:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_DSTLIST );

		if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_DSTLIST );
				}
			ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
			}
	break;

   default:

	break;
   }
return CPPModalDlg::OnCommand( wParam, lParam );
}

/************************************************************************/
void CCmsFileNewDlg::OnOK()
/************************************************************************/
{

cmsInfoInit( pCmsInfo );
cmsGetDstPT( m_hWnd, pCmsInfo );

ControlGetText( IDC_CMS_DST );
PutRegStr( pFileNewSetupDstName, ( LPTSTR )LPCTSTR( string ) );
ColorManager.FileNew.dst = pCmsInfo->dst;
bReturn =  TRUE;
CPPModalDlg::OnOK();
}

/************************************************************************/
void CCmsFileNewDlg::OnCancel()
/************************************************************************/
{

cmsInfoInit( pCmsInfo );
bReturn = FALSE;
CPPModalDlg::OnCancel();
}

/************************************************************************/
CCmsConvertDlg::CCmsConvertDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRC, pParentWnd )
/************************************************************************/
{

pInfo = new CCmsInfoDlg( NO, YES, this );
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsConvertDlg::OnInitDialog()
/************************************************************************/
{	  

CenterPopup( m_hWnd );

// Build DST's list
for( pos = ColorManager.dstMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	pos1 = pPTs->GetStartPosition(); 
	pPTs->GetNextAssoc( pos1, string1, ( void *& )dwId );
	ColorManager.spcMap->Lookup( ( LPVOID )dwId, ( LPVOID & )nCrc );
	if( nCrc != pCmsInfo->src.DataType )
		ComboBoxAddString( IDC_CMS_DST );
	}

if( pCmsInfo->src.DataType == FDT_RGBCOLOR )
	GetRegStr( pConvertCMYKDstName, ( LPTSTR )LPCTSTR( string ), "" );
else
	GetRegStr( pConvertRGBDstName, ( LPTSTR )LPCTSTR( string ), "" );

iSel = ComboBoxFindStringExact( IDC_CMS_DST );

ComboBoxSetCurSel( IDC_CMS_DST, iSel == CB_ERR ? 0 : iSel );

ControlGetText( IDC_CMS_DST );

if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_DSTLIST );
		}
	ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsConvertDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{ 
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_DST:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_DSTLIST );

		if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_DSTLIST );
				}
			ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
			}
	break;

   default:

	break;
   }
return CPPModalDlg::OnCommand( wParam, lParam );
}

/************************************************************************/
void CCmsConvertDlg::OnOK()
/************************************************************************/
{ 

ControlGetText( IDC_CMS_DST );

if( pCmsInfo->src.DataType == FDT_RGBCOLOR )
	PutRegStr( pConvertCMYKDstName, ( LPTSTR )LPCTSTR( string ) );
else
	PutRegStr( pConvertRGBDstName, ( LPTSTR )LPCTSTR( string ) );

cmsInfoInit( pCmsInfo );
cmsGetDstPT( m_hWnd, pCmsInfo );

bReturn = TRUE;

CPPModalDlg::OnOK();
}

/************************************************************************/
void CCmsConvertDlg::OnCancel()
/************************************************************************/
{ 

cmsInfoInit( pCmsInfo );

bReturn = FALSE;

CPPModalDlg::OnOK();
}

/************************************************************************/
CCmsPhotoCDOpenDlg::CCmsPhotoCDOpenDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRC, pParentWnd )
/************************************************************************/
{

pInfo = new CCmsInfoDlg( YES, YES, this );
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsPhotoCDOpenDlg::OnInitDialog()
/************************************************************************/
{	  

CenterPopup( m_hWnd );

ComboBoxAddString( IDC_CMS_SRC, szDefaultSrc );

for( pos = ColorManager.pcdMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.pcdMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_SRC );
	}

GetRegStr( pPhotoCDOpenSrcName,  ( LPTSTR )LPCTSTR( string ), "" );

ComboBoxSetCurSel( IDC_CMS_SRC, ComboBoxFindStringExact( IDC_CMS_SRC ) );

ControlGetText( IDC_CMS_SRC );

if( ColorManager.pcdMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_SRCLIST );
		}
	GetRegStr( pPhotoCDOpenSrcProfile,  ( LPTSTR )LPCTSTR( string ), "" );

	iSel = ListBoxFindStringExact( IDC_CMS_SRCLIST );
 	ListBoxSetCurSel( IDC_CMS_SRCLIST, iSel == LB_ERR ? 0 : iSel );
	}

// Build DST's list
ComboBoxAddString( IDC_CMS_DST, szDefaultDst );

for( pos = ColorManager.dstMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_DST );
	}

GetRegStr( pPhotoCDOpenDstName,  ( LPTSTR )LPCTSTR( string ), "" );

ComboBoxSetCurSel( IDC_CMS_SRC, ComboBoxFindStringExact( IDC_CMS_DST ) );

ControlGetText( IDC_CMS_DST );

if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_DSTLIST );
		}
	GetRegStr( pPhotoCDOpenDstProfile,  ( LPTSTR )LPCTSTR( string ), "" );
	iSel = ListBoxFindStringExact( IDC_CMS_DSTLIST );
 	ListBoxSetCurSel( IDC_CMS_DSTLIST, iSel == LB_ERR ? 0 : iSel );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsPhotoCDOpenDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{ 
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_SRC:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_SRCLIST );

		if( ColorManager.pcdMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_SRCLIST );
				}
			ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
			}
	break;

	case IDC_CMS_DST:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_DSTLIST );

		if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_DSTLIST );
				}
			ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
			}
	break;


   default:

	break;
   }
return CPPModalDlg::OnCommand( wParam, lParam );
}

/************************************************************************/
void CCmsPhotoCDOpenDlg::OnOK()
/************************************************************************/
{ 

cmsInfoInit( pCmsInfo );
cmsGetSrcPT( m_hWnd, pCmsInfo, ColorManager.pcdMap );
cmsGetDstPT( m_hWnd, pCmsInfo );

if( !pCmsInfo->src.ptInfo.toRCS )
	{
	if( pCmsInfo->dst.ptInfo.frRCS )
		{
		Message( IDS_CMS_NOSRCDEST );
		}
	else
		{
		PCDSaveSettings();
		bReturn = TRUE;
		CPPModalDlg::OnOK();
		}
	}
else
	{
	if( pCmsInfo->dst.ptInfo.frRCS )
		{
		PCDSaveSettings();
		bReturn = TRUE;
		CPPModalDlg::OnOK();
		}
	else
		{
		if( ColorManager.Monitor.dst.ptInfo.frRCS )
			{
			pCmsInfo->dst = ColorManager.Monitor.dst;
			PCDSaveSettings();
			bReturn = TRUE;
			CPPModalDlg::OnOK();
			}
		else
			{
			Message( IDS_CMS_NODESTNOMON );
			}
		}
	}
}

/************************************************************************/
void CCmsPhotoCDOpenDlg::OnCancel()
/************************************************************************/
{ 

bReturn = TRUE;
CPPModalDlg::OnCancel();
}

/************************************************************************/
CCmsPrintPreviewDlg::CCmsPrintPreviewDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRC, pParentWnd )
/************************************************************************/
{

pInfo = new CCmsInfoDlg( NO, YES, this );
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsPrintPreviewDlg::OnInitDialog()
/************************************************************************/
{	  

CenterPopup( m_hWnd );

// Build DST's list
ComboBoxAddString( IDC_CMS_DST, szDefaultDst );

for( pos = ColorManager.dstMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_DST );
	}

GetRegStr( pPrintPreviewDstName,  ( LPTSTR )LPCTSTR( string ), "" );

ComboBoxSelectString( IDC_CMS_DST, lstrlen( ( LPTSTR )LPCTSTR( string ) ) ?
	string : szDefaultDst );

ControlGetText( IDC_CMS_DST );

if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_DSTLIST );
		}
	ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsPrintPreviewDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{ 
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_DST:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		PutRegStr( pPrintPreviewDstName, ( LPTSTR )LPCTSTR( string ) );

		ListBoxResetContent( IDC_CMS_DSTLIST );

		if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_DSTLIST );
				}
			ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
			}
	break;

   default:

	break;
   }
return CPPModalDlg::OnCommand( wParam, lParam );
}

/************************************************************************/
void CCmsPrintPreviewDlg::OnOK()
/************************************************************************/
{ 

cmsInfoInit( pCmsInfo );
cmsGetDstPT( m_hWnd, pCmsInfo );
bReturn = TRUE;
CPPModalDlg::OnOK();
}

/************************************************************************/
void CCmsPrintPreviewDlg::OnCancel()
/************************************************************************/
{ 

cmsInfoInit( pCmsInfo );
bReturn = TRUE;
CPPModalDlg::OnOK();
}

/************************************************************************/
CCmsScanSetupDlg::CCmsScanSetupDlg( LPCMSINFO pCmsInfoParam, CWnd *pParentWnd ) :
	CCmsDlg( IDD_CMS_SRC, pParentWnd )
/************************************************************************/
{

pInfo = new CCmsInfoDlg( YES, YES, this );
pCmsInfo = pCmsInfoParam;
}

/************************************************************************/
BOOL CCmsScanSetupDlg::OnInitDialog()
/************************************************************************/
{	  

CenterPopup( m_hWnd );

// Build SRC's list
ComboBoxAddString( IDC_CMS_SRC, szDefaultSrc );

for( pos = ColorManager.scnMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.scnMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_SRC );
	}

GetRegStr( pScanSetupSrcName,  ( LPTSTR )LPCTSTR( string ), "" );

ComboBoxSelectString( IDC_CMS_SRC, lstrlen( ( LPTSTR )LPCTSTR( string ) ) ?
	string : szDefaultSrc );

ControlGetText( IDC_CMS_SRC );

if( ColorManager.scnMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_SRCLIST );
		}
	ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
	}

// Build DST's list
ComboBoxAddString( IDC_CMS_DST, szDefaultDst );

for( pos = ColorManager.dstMap->GetStartPosition(); pos != NULL; )
	{
	ColorManager.dstMap->GetNextAssoc( pos, string, ( void* &)pPTs );
	ComboBoxAddString( IDC_CMS_DST );
	}

GetRegStr( pScanSetupDstName,  ( LPTSTR )LPCTSTR( string ), "" );

ComboBoxSelectString( IDC_CMS_DST, lstrlen( ( LPTSTR )LPCTSTR( string ) ) ?
	string : szDefaultDst );

ControlGetText( IDC_CMS_DST );

if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
	{
	for( pos = pPTs->GetStartPosition(); pos != NULL; )
		{
		pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
		ListBoxAddString( IDC_CMS_DSTLIST );
		}
	ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
	}

return(TRUE);
}

/************************************************************************/
BOOL CCmsScanSetupDlg::OnCommand( WPARAM wParam, LPARAM lParam )
/************************************************************************/
{ 
UINT nId 			= LOWORD( wParam );
int nCodeNotify	= HIWORD( wParam );
HWND hControl 		= ( HWND )lParam;

switch ( nId )
	{
	case IDC_STATUSINFO:
		pInfo->DoModal();
	break;

	case IDC_CMS_SRC:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_SRCLIST );

		if( ColorManager.scnMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_SRCLIST );
				}
			ListBoxSetCurSel( IDC_CMS_SRCLIST, 0 );
			}
	break;

	case IDC_CMS_DST:
		if( nCodeNotify != CBN_SELCHANGE)
			break;
		if( GetFocus() != GetDlgItem( nId ) )
			break;
		
		ControlGetText( hControl );

		ListBoxResetContent( IDC_CMS_DSTLIST );

		if( ColorManager.dstMap->Lookup( string, ( void *& )pPTs ) )
			{
			for( pos = pPTs->GetStartPosition(); pos != NULL; )
				{
				pPTs->GetNextAssoc( pos, string, ( void *& )dwId );
				ListBoxAddString( IDC_CMS_DSTLIST );
				}
			ListBoxSetCurSel( IDC_CMS_DSTLIST, 0 );
			}
	break;

   default:

	break;
   }
return CPPModalDlg::OnCommand( wParam, lParam );
}

/************************************************************************/
void CCmsScanSetupDlg::OnOK()
/************************************************************************/
{ 
cmsInfoInit( pCmsInfo );
cmsGetSrcPT( m_hWnd, pCmsInfo, ColorManager.scnMap );
cmsGetDstPT( m_hWnd, pCmsInfo );

ColorManager.Scan.src = pCmsInfo->src;

if( !pCmsInfo->src.ptInfo.toRCS )
	{
	if( pCmsInfo->dst.ptInfo.frRCS )
		{
		Message( IDS_CMS_NOSRCDEST );
		}
	else
		{
		ControlGetText( IDC_CMS_SRC );
		PutRegStr( pScanSetupSrcName, ( LPTSTR )LPCTSTR( string ) );
		ControlGetText( IDC_CMS_DST );
		PutRegStr( pScanSetupDstName, ( LPTSTR )LPCTSTR ( string ) );
		ColorManager.Scan.dst = pCmsInfo->dst;
		SaveScanSetup();
		bReturn = TRUE;
		CPPModalDlg::OnOK;
		}
	}
else
	{
	if( pCmsInfo->dst.ptInfo.frRCS )
		{
		ControlGetText( IDC_CMS_SRC );
		PutRegStr( pScanSetupSrcName, ( LPTSTR )LPCTSTR( string ) );
		ControlGetText( IDC_CMS_DST );
		PutRegStr( pScanSetupDstName, ( LPTSTR )LPCTSTR ( string ) );
		ColorManager.Scan.dst = pCmsInfo->dst;
		SaveScanSetup();
		bReturn = TRUE;
		CPPModalDlg::OnOK;
		}
	else
		{
		if( ColorManager.Monitor.dst.ptInfo.frRCS )
			{
			pCmsInfo->dst = ColorManager.Monitor.dst;
			ControlGetText( IDC_CMS_SRC );
			PutRegStr( pScanSetupSrcName, ( LPTSTR )LPCTSTR( string ) );
			ControlGetText( IDC_CMS_DST );
			PutRegStr( pScanSetupDstName, ( LPTSTR )LPCTSTR( string ) );
			ColorManager.Scan.dst = pCmsInfo->dst;
			SaveScanSetup();
			bReturn = TRUE;
			CPPModalDlg::OnOK;
			}
		else
			{
			Message( IDS_CMS_NODESTNOMON );
			}
		}
	}
}

/************************************************************************/
void CCmsScanSetupDlg::OnCancel()
/************************************************************************/
{ 

cmsInfoInit( pCmsInfo );
bReturn = TRUE;
CPPModalDlg::OnCancel();
}


