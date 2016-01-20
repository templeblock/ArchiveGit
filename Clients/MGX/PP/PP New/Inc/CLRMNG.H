/************************************************************************

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

************************************************************************/

#ifndef __clrmng_h__
#define __clrmng_h__

/************************************************************************/
#define MAX_SEQ				4
#define CMS_LINES_PER_BLT	50

#define GetRegStr(K,V,STR) GetPrivateProfileString(_T("-"),K,STR,V,256,szCms)
#define GetRegInt(K,V,INT) (V=GetPrivateProfileInt(_T("-"),K,INT,szCms))
#define GetRegFix(K,V,INT) { TCHAR t[40]; GetPrivateProfileString(_T("-"),K,itoa(INT,t,10),t,sizeof(t),szFileName); V=AsciiFixed(t); }
#define PutRegStr(K,V) WritePrivateProfileString(_T("-"),K,V,szCms)
#define PutRegInt(K,V) { TCHAR t[40]; WritePrivateProfileString(_T("-"),K,itoa(V,t,10),szCms); }
#define PutRegFix(K,V) { TCHAR t[40]; WritePrivateProfileString(_T("-"),K,FixedAscii(V,t,-1),szFileName); }

typedef struct COLORMANAGER_tag
{
	CMapStringToPtr	*srcMap;
	CMapStringToPtr	*dstMap;
	CMapStringToPtr	*monMap;
	CMapStringToPtr	*pcdMap;
	CMapStringToPtr	*scnMap;
	CMapPtrToPtr	*crcMap;
	CMapPtrToPtr	*uidMap;
	CMapPtrToPtr	*spcMap;
	CMSINFO			Monitor;
	CMSINFO			Scan;
	CMSINFO			FileNew;
	CMSINFO			PrintPreview;
	LPVOID			CmsXform;
	CMSINFO			PhotoCD;
}
COLORMANAGER, far *LPCOLORMANAGER;

typedef enum
{
	FIL,
	SCN,
	PCD,
	MON,
	DST,
	PRT
} ATTRIB_TYPE; 

/************************************************************************/

void	CMSInit( void );
BOOL	CMSOpen( BOOL EnableDisable );

typedef BOOL	( FAR *LPKODAKOPEN )( BOOL, FARPROC, LPTSTR );
typedef LPTSTR ( FAR *LPKODAKATTRIB )( LPTSTR pkey, int nIndex, long lUniqueID,
	ATTRIB_TYPE Type );

typedef double ( FAR *LPKCMSGAMMPROC )( LONG lCrc );
typedef LONG ( FAR *LPKCMSSIZEPROC )( LONG lCrc, LPTSTR pMandM );
typedef BOOL ( FAR *LPKCMSDATAPROC )( LONG lCrc, LPTR pData );
typedef LPVOID ( FAR *LPKCMSCNCTTHREEPROC )( LPFRMTYPEINFO, LPFRMTYPEINFO, LPFRMTYPEINFO );
typedef LPVOID ( FAR *LPKCMSCOPYGAMMAXFORM )( LPVOID );

LONG GetXformDataSize( LONG lCrc, LPTSTR pMandM );

BOOL GetXformData( LONG lCrc, LPTR pData );

void cmsInfoInit( LPCMSINFO pcmsInfo );

void cmsGetSrcPT( HWND hDlg, LPCMSINFO pcmsInfo, CMapStringToPtr *pMap );

void cmsGetDstPT( HWND hDlg, LPCMSINFO pcmsInfo );

void CmsGetSrcPT( LPTSTR lpImageFile, LPCMSINFO pCmsInfo);

BOOL CmsGetDstPT(LPCMSINFO pCmsInfo);

BOOL CmsGetSrcDstPT(LPCMSINFO pCmsInfo);

void cmsSavePTName( HWND hDlg, LPTSTR pt );

BOOL IsSrcPTSelected( LPPTINFO );

BOOL IsDstPTSelected( LPPTINFO );

BOOL ConvertCRCtoUID( LPFRMTYPEINFO, LPFRMTYPEINFO );

LONG ConvertCRC( LONG lCrc );

LPTSTR CmsGetPTNameFromFile( LPTSTR lpFile, LPTSTR szBlank );

LPTSTR CmsGetPTName( LPFRAME lpFrame, LPTSTR lpStr, LPFRMTYPEINFO lpTypeInfo = NULL );

void LoadScanSetup( void );

void LoadPhotoCDSetup( void );

void LoadMonitorSetup( void );

void SaveScanSetup( void );

void SavePhotoCDSetup( void );

void SaveMonitorSetup( void );

BOOL DBOpen( LPCOLORMANAGER pColorManager );

CMapPtrToPtr *DBBuildList( LPTSTR pSection );

CMapStringToPtr *DBOpenSection( LPTSTR pSection );

void CmsPhotoCD_OnInitDialog( HWND hDlg );

void CmsPhotoCDInit( HWND hDlg );

void CmsSavePhotoCDSelection( HWND hDlg );

void CmsScanSetupInit( HWND hDlg );

void CmsInitBlackLuts( HWND hDlg );

void CmsPrintSetupInit( HWND hDlg );

void CmsPrintSetup_OnInitDialog( HWND hDlg );

void CmsPrintSetupProcessMsg( HWND hDlg, ITEMID id );

void CmsMonitorSetup_OnInitDialog( HWND hDlg );

void CmsMonitorSetupInit( HWND hDlg );

void CmsSaveMonitorSelection( );

LPVOID CmsCopyGammaXform( LPVOID xform );

#ifndef DLL

BOOL CmsGetMenuState( LPIMAGE lpImage );

#endif

BOOL CmsGetSrcPTType( LPFRMTYPEINFO lpSrcType, LPCMSINFO pCmsInfo);

#ifndef DLL

void CmsPrintPreview( LPIMAGE lpImage );

#endif

void CmsSetSuperBltGamma( BOOL bIniting = FALSE );

void CmsGetMonitorGamma( LONG Crc );

void CmsMonitorSetupProcessMsg( HWND hDlg, ITEMID id );

void CmsThreadInit( void );

void CmsThreadTerm( void );

BOOL WINPROC EXPORT DlgCmsPrintSetupInfoProc( HWND hDlg, UINT msg, WPARAM wParam,
                                          LPARAM lParam);

BOOL WINPROC EXPORT DlgCmsMonitorSetupInfoProc( HWND hDlg, UINT msg, WPARAM wParam,
                                          LPARAM lParam);

#endif // __clrmng_h__


