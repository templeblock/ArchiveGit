//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "pcdlib.h"
#include "pcdui.h"
#include <ctype.h>

// Static prototypes
static BOOL FastbitsToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg );
static LPFRAME AttachPreviewFrame( HWND hDlg, ITEMID id, LPFRAME lpFrame );
static void UpdateMemoryReadout( HWND hDlg, ITEMID id, LPRECT lpGridRect );
static void GetPCDImageSize(LPINT lpWidth, LPINT lpHeight, LPINT lpDepth);
static void LoadPCDPreferences(LPPCDOPTIONS lpOptions);
static void StorePCDPreferences(LPPCDOPTIONS lpOptions);
LOCAL void FreeUpPCDOptions(LPPCDOPTIONS lpOptions);
LOCAL void GetPCDResolution(int PhotoCDRes, PCDresolution *pcdRes, LPINT lpRes);
LOCAL void FillPCDInfoList(HWND hDlg, ITEMID idList, PCDpacInfoPtr pInfo);
LOCAL void AddPCDInfo(HWND hDlg, ITEMID idList, ITEMID idDesc, int length, LPSTR lpInfo);

VOID WINPROC EXPORT PhotoCDProgress(short nNum, short nDenom, long lData);
BOOL WINPROC EXPORT PhotoCDAbort(long lData);
BOOL WINPROC EXPORT DlgPCDResolutionProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#define PCD_IMAGEPACK 	".pcd"

// Static data
static LPFRAME lpPreviewFrame;
static RECT rGridRect;
static BOOL bGridLock;
static BOOL bHaveGrid;
static int iImageWidth, iImageHeight, iImageDepth;
static RECT rControl;
static LPSTR lpPCDFile;
static PCDtransform PCDTransform;
static int nCurrentLine, nLinesRead;
static BOOL fAbort;

/* 
 * Photo CD related paths
 * These paths are hard-coded because they
 * involve the file system structure that is
 * defined by Photo CD technology.
 */
#define IMAGE_PAC_PREFIX "IMG"
#define OVERVIEW_FILE	"OVERVIEW.PCD"
#define OVERVIEW_PATH    "\\PHOTO_CD\\OVERVIEW.PCD" 
#define IMAGES_DIR       "\\PHOTO_CD\\IMAGES\\"    
#define DISC_RIGHTS_DIR	 "RIGHTS\\"
#define DISC_RIGHTS_FILE "RIGHTS.USE"    

#define MAIN_MEM_MINK	100L

// local defines copied from defaults.c
#define GetStr(N,STR) GetDefaultString(#N,STR,N,sizeof(N))
#define GetInt(N,INT) N=GetDefaultInt(#N,INT)
#define PutStr(N,STR) PutDefaultString(#N,STR)
#define PutInt(N,INT) { char t[40]; PutStr(N,itoa(INT,t,10)); }

/************************************************************************/
BOOL DoPCDResOverride(int OldPhotoCDRes, int PhotoCDResOverride)
/************************************************************************/
{
 	int PhotoCDRes;

	PhotoCDRes = OldPhotoCDRes + PhotoCDResOverride;
	if (PhotoCDRes < PCD_BASE_OVER_16)
		PhotoCDRes = PCD_BASE_OVER_16;
	if (PhotoCDRes > PCD_64BASE)
		PhotoCDRes = PCD_64BASE;
	return(PhotoCDRes != OldPhotoCDRes);
}

/************************************************************************/
LPPCDOPTIONS CReadImage::GetPCDOptions()
/************************************************************************/
{
	PCDphotoHdl	hPcd;	// handle to selected image pac
//	PCDpacInfoRec info;
	LPPCDOPTIONS lpOptions;

	lpOptions = (LPPCDOPTIONS)Alloc(sizeof(PCDOPTIONS));
	if (!lpOptions)
	{
		Message(IDS_EMEMALLOC);
		return(NULL);
	}
	clr((LPTR)lpOptions, sizeof(PCDOPTIONS));
	cmsInfoInit( &lpOptions->cmsInfo );

	// Call PCDopen to obtain an image pac handle
	if(PCDopen(m_lpFileName, &hPcd))
	{
		FreeUpPCDOptions(lpOptions);
		Message (IDS_EOPEN, m_lpFileName);
		return(NULL);
	}

	// Read image information which is documented nowhere!!!
	if (/*PCDreadImageInfo(hPcd, &info) ||*/
		PCDgetRotation(hPcd, &PCDTransform))
	{
		FreeUpPCDOptions(lpOptions);
		PCDclose(hPcd);
		Message (IDS_EREAD, m_lpFileName);
		return(NULL);
	}
	// close it for now
	PCDclose(hPcd);

	LoadPCDPreferences(lpOptions);

//	if (info.mediaId == PCD_BW_NEGATIVE ||
//    	info.mediaId == PCD_BW_REVERSAL ||
//    	info.mediaId == PCD_BW_HARDCOPY)
//		lpOptions->PhotoCDFormat = PCD_SINGLE;
//	else
		lpOptions->PhotoCDFormat = PCD_RGB;

	bGridLock = NO;
	AstralSetRectEmpty(&rGridRect);
	lstrcpy( Names.Saved, m_lpFileName );
	if (!AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_PCDOPTIONS,
		DlgPCDOptionsProc, (LPARAM)lpOptions))
	{
		FreeUpPCDOptions(lpOptions);
		return(NULL);
	}
	if (!Control.fPCDUseCMS)
		cmsInfoInit( &lpOptions->cmsInfo );
	if (lpOptions->fHaveArea = bHaveGrid)
	{
		int res;
		PCDresolution pcdRes;

		GetPCDResolution(lpOptions->PhotoCDRes, &pcdRes, &res);

		lpOptions->rArea.left = FGET(rGridRect.left, res);
		lpOptions->rArea.top = FGET(rGridRect.top, res);
		lpOptions->rArea.right = FGET(rGridRect.right, res);
		lpOptions->rArea.bottom = FGET(rGridRect.bottom, res);
	}
	return(lpOptions);
}

/************************************************************************/
BOOL CReadImage::PromptForResolution(LPSTR lpFileName, LPPCDOPTIONS lpOptions)
/************************************************************************/
{
	lstrcpy( Names.Saved, lpFileName );
	return((BOOL)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_PCDRESOLUTION,
		DlgPCDResolutionProc, (LPARAM)lpOptions));
}

/************************************************************************/
BOOL IsPCDImage( LPSTR lpFileName )
/************************************************************************/
{
FNAME	szFileName, szPrefix;

if (!StringsEqual(PCD_IMAGEPACK, extension(lpFileName)))
	return(FALSE);
lstrcpy(szFileName, filename(lpFileName));
lstrcpy(szPrefix, IMAGE_PAC_PREFIX);
if (!StringsEqualN(szFileName, szPrefix, lstrlen(szPrefix)))
	return(FALSE);
return(TRUE);
}

/************************************************************************/
BOOL GetPCDOverview( LPSTR lpFileName, LPSTR lpOverview )
/************************************************************************/
{
int	len;

/* get name of Image - .\PHOTO_CD\IMAGES\IMGNNNN.PCD */
lstrcpy(lpOverview, lpFileName);

/* strip off filename - .\PHOTOCD\IMAGES\ */
stripfile(lpOverview);

/* strip off IMAGES directory name - .\PHOTO_CD\ */
len = lstrlen(lpOverview);
if (lpOverview[len-1] = '\\')
	lpOverview[len-1] = '\0';
stripfile(lpOverview);

/* add name of Overview PAC - .\PHOTO_CD\OVERVIEW.PCD */
lstrcat(lpOverview, OVERVIEW_FILE);

/* see if this is a valid Photo CD Disk */
return(FileExists(lpOverview));
}

/***********************************************************************/
BOOL GetPCDLocation( LPSTR lpSpec )
/***********************************************************************/
{
FNAME	szOverview, szDir;

if (lpSpec[1] == ':')
	{
	lstrcpy(szDir, lpSpec);
	szDir[2] = '\0';
	}
else
	szDir[0] = '\0';
lstrcpy(szOverview, szDir);
lstrcat(szOverview, OVERVIEW_PATH);
if (!FileExists(szOverview))
	return(FALSE);
lstrcpy(szOverview, szDir);
lstrcat(szOverview, IMAGES_DIR);
lstrcat(szOverview, filename(lpSpec));
lstrcpy(lpSpec, szOverview);
return(TRUE);
}

/***********************************************************************/
void DisplayPCDInfo( HWND hDlg, LPSTR lpFileName )
/***********************************************************************/
{
lpPCDFile = lpFileName;
AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PCDFILEINFO, DlgPCDFileInfoProc);
}

// LOCAL prototypes
LOCAL BOOL PCDFileInfo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PCDFileInfo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPCDFileInfoProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PCDFileInfo_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PCDFileInfo_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PCDFILEINFO);
	}
}

/************************************************************************/
LOCAL BOOL PCDFileInfo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPSTR lpString;
int length, len;
PCDphotoHdl	hPcd;	// handle to selected image pac
PCDstatus status;
PCDpacInfoRec rec;
STRING	szString;

CenterPopup( hDlg );
SetDlgItemText(hDlg, IDC_INFOFILENAME, stripdir(lpPCDFile) );
if (AstralStr(IDI_PHOTOCD, &lpString))
	SetDlgItemText(hDlg, IDC_INFOFILETYPE, lpString);
if (PCDopen(lpPCDFile, &hPcd))
	return(TRUE);
status = PCDreadImageInfo(hPcd, &rec);
PCDclose(hPcd);
if (status)
	return(TRUE);
length = sizeof(rec.mediaType);
copy((LPTR)rec.mediaType, (LPTR)szString, length);
szString[length] = '\0';
SetDlgItemText(hDlg, IDC_INFOMEDIATYPE, szString);
length = sizeof(rec.scannerVendor);
copy((LPTR)rec.scannerVendor, (LPTR)szString, length);
szString[length] = '\0';
SetDlgItemText(hDlg, IDC_INFOSCANNERVENDOR, szString);
length = sizeof(rec.scannerProdID);
copy((LPTR)rec.scannerProdID, (LPTR)szString, length);
szString[length] = '\0';
SetDlgItemText(hDlg, IDC_INFOSCANNERTYPE, szString);
lstrcpy(szString, lpPCDFile);
stripfile(szString); // strip off image file name IMGNNNN.PCD
if (szString[lstrlen(szString)-1] = '\\')
	szString[lstrlen(szString)-1] = '\0';
stripfile(szString); // strip off image directory IMAGE
lstrcat(szString, DISC_RIGHTS_DIR); // add RIGHTS directory
if (rec.copyright)
	{
	len = lstrlen(szString);
	length = sizeof(rec.copyrightFile);
	copy((LPTR)rec.copyrightFile, (LPTR)szString+len, length);
	len += length;
	szString[len] = '\0';
	}
else
	lstrcat(szString, DISC_RIGHTS_FILE);
SetDlgItemText(hDlg, IDC_INFOCOPYRIGHT, Lowercase(szString));
ControlEnable(hDlg, IDC_INFOCOPYRIGHT, FileExists(szString));
return(TRUE);
}

/************************************************************************/
LOCAL void PCDFileInfo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
STRING szString, szCommand;

switch (id)
	{
	case IDC_INFOCOPYRIGHT:
	GetDlgItemText(hDlg, id, szString, sizeof(szString));
	lstrcpy(szCommand, "NOTEPAD ");
	lstrcat(szCommand, szString);
	WinExec(szCommand, SW_SHOW);
	break;

	case IDOK:
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL PCDOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PCDOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void PCDOptions_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange);
LOCAL void PCDOptions_Update(HWND hDlg);
LOCAL void PCDOptions_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);
LOCAL void PCDOptions_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);

static LPPCDOPTIONS lpEditOptions;

static ITEMID idListToManage;
static DLGSECTION photocdsec;
static CMSINFO pcdCmsInfo;

static int PCDOptions_iTabTable[] =
	{
	IDD_PHOTOCD,
	IDD_PHOTOCDCMS
	};

/***********************************************************************/
BOOL WINPROC EXPORT DlgPCDOptionsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PCDOptions_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PCDOptions_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, PCDOptions_OnMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, PCDOptions_OnDrawItem);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
	HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, PCDOptions_OnPaletteChanged);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PCDOPTIONS);
	}
}

/************************************************************************/
LOCAL void PCDOptions_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
	if ( lpMeasureItem->CtlID == IDC_PCD_INFO)
		InfoMeasureItem(hDlg, lpMeasureItem);
	else
		MeasureItem( hDlg, lpMeasureItem, NO );
}

/************************************************************************/
LOCAL void PCDOptions_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
	if ( lpDrawItem->CtlID == IDC_PCD_INFO)
		InfoDrawItem(hDlg, lpDrawItem);
	else
		DrawItem( hDlg, lpDrawItem, 0 );
}

/************************************************************************/
LOCAL BOOL PCDOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
PCDphotoHdl	hPcd;	// handle to selected image pac
PCDstatus status;
DWORD dwRes, dwMyRes;
short wResCount;
int id;
BOOL fResSupported;
PCDpacInfoRec info;
PCDresolution res;

CenterPopup( hDlg );

if (!Control.CMSEnabled)
	Control.fPCDUseCMS = FALSE;
pcdCmsInfo = ColorManager.PhotoCD;

CmsPhotoCD_OnInitDialog(hDlg);

lpEditOptions = (LPPCDOPTIONS)lParam;

if (PCDopen(Names.Saved, &hPcd))
	return(TRUE);

status = PCDreadImageInfo(hPcd, &info);
if (!status)
	status = PCDgetCount(hPcd, &dwRes, &wResCount);
PCDclose(hPcd);
if (status)
	return(TRUE);

if (Control.fPCDUseCMS)
	lpEditOptions->cmsInfo = pcdCmsInfo;
else
	cmsInfoInit(&lpEditOptions->cmsInfo);

FillPCDInfoList(hDlg, IDC_PCD_INFO, &info);

for (res = PCD_64BASE; res >= PCD_BASE_OVER_16;)
	{
	dwMyRes = 1 << (res-PCD_BASE_OVER_16+1);
	fResSupported = (dwMyRes & dwRes) != 0;
	ControlEnable(hDlg, id, fResSupported);
	if (lpEditOptions->PhotoCDRes == res && !fResSupported)
		--lpEditOptions->PhotoCDRes;
	res = (PCDresolution)((int)res - 1);
	}
if (lpEditOptions->PhotoCDRes < PCD_BASE_OVER_16)
	return(TRUE);
CheckComboItem(hDlg, IDC_PCDRESOLUTION, IDC_PCD_BASE_OVER_16,
		IDC_PCD_64BASE, lpEditOptions->PhotoCDRes - PCD_BASE_OVER_16 + IDC_PCD_BASE_OVER_16);

CheckComboItem(hDlg, IDC_PCDFORMAT, IDC_PCDSINGLE, IDC_PCDRGB,
		Control.fPCDUseCMS ? IDC_PCDRGB : lpEditOptions->PhotoCDFormat - PCD_SINGLE + IDC_PCDSINGLE);
ControlEnable(hDlg, IDC_CMS, Control.CMSEnabled); 
CheckDlgButton(hDlg, IDC_CMS, Control.fPCDUseCMS); 
ControlEnable(hDlg, IDC_PCD_DOPREVIEW, !Control.fPCDUseCMS);
ControlEnable(hDlg, IDC_PCDFORMAT, !Control.fPCDUseCMS);

lpPreviewFrame = NULL;
bHaveGrid = NO;
GetPCDImageSize(&iImageWidth, &iImageHeight, &iImageDepth);
ImageMemory( hDlg, IDC_PCD_IMAGESIZE, FGET(iImageWidth, 1 ),
	FGET( iImageHeight, 1 ), 1/*Resolution*/, iImageDepth );
SetDlgItemText(hDlg, IDC_PCD_CROPSIZE, "");
FORWARD_WM_COMMAND(hDlg, IDC_PCD_DOPREVIEW, GetDlgItem(hDlg, IDC_PCD_DOPREVIEW),
					0, PostMessage);

// Initialise the dialog section.
InitDlgSection(&photocdsec, ID_SECTION0, hDlg, IDC_PANE0, NULL);

// Set threshold so that a category is always discarded when the user
// selects another.
SetDlgSectionThreshold(&photocdsec, 1, 1);

int nItems = sizeof(PCDOptions_iTabTable)/sizeof(int);
for (int i = 0; i < nItems; ++i)
{
	// add the halftone category
	if (PCDOptions_iTabTable[i] != DLGSEC_ERR)
		AddDlgSectionCategory(	&photocdsec,
							PictPubApp.GetResourceHandle(),
							MAKEINTRESOURCE(PCDOptions_iTabTable[i]),
							PCDOptions_iTabTable[i],
					    	DSCF_DISCARDABLE,
							NULL);
}

PCDOptions_Update(hDlg);

return(TRUE);
}

/************************************************************************/
LOCAL void FillPCDInfoList(HWND hDlg, ITEMID idList, PCDpacInfoPtr pInfo)
/************************************************************************/
{
	char szValue[256];

	GetDlgItemText(hDlg, (int)pInfo->mediaId+IDC_PCD_COLORNEGATIVE, szValue, sizeof(szValue));
	AddPCDInfo(hDlg, idList, IDC_PCD_MEDIAID,
				lstrlen(szValue), szValue);

	AddPCDInfo(hDlg, idList, IDC_PCD_MEDIATYPE,
				sizeof(pInfo->mediaType), pInfo->mediaType);

	AddPCDInfo(hDlg, idList, IDC_PCD_SCANNERVENDOR,
				sizeof(pInfo->scannerVendor), pInfo->scannerVendor);

	AddPCDInfo(hDlg, idList, IDC_PCD_SCANNERMODEL,
				sizeof(pInfo->scannerProdID), pInfo->scannerProdID);

	AddPCDInfo(hDlg, idList, IDC_PCD_EQUIPMENT,
				sizeof(pInfo->piwEquipment), pInfo->piwEquipment);

	AddPCDInfo(hDlg, idList, IDC_PCD_COPYRIGHT,
				sizeof(pInfo->copyrightFile), pInfo->copyrightFile);
}

/************************************************************************/
LOCAL void AddPCDInfo(HWND hDlg, ITEMID idList, ITEMID idDesc, int length, LPSTR lpInfo)
/************************************************************************/
{
	HWND	hControl;
	char 	szInfo[256];
	char	szValue[256];

	hControl = GetDlgItem(hDlg, idList);
	if (!hControl)
		return;

	GetDlgItemText(hDlg, idDesc, szInfo, sizeof(szInfo));
	lstrcat(szInfo, ":");
	copy((LPTR)lpInfo, (LPTR)szValue, length);
	szValue[length] = '\0';
	lstrcat(szInfo, szValue);
	ListBox_AddString(hControl, szInfo);
}

/************************************************************************/
LOCAL void PCDOptions_Update(HWND hDlg)
/************************************************************************/
{
	ITEMID idCategory;

	idCategory = PCDOptions_iTabTable[Control.fPCDUseCMS ? 1 : 0];
	SelectDlgSectionCategory(&photocdsec, idCategory);

	if (idCategory == IDD_PHOTOCD)
	{
		// these are buddied here because the auto-buddying does
		// not work with dynamic dialogs
		SetSpinBuddy(hDlg, IDC_PCD_BRIGHTBUTTON);
		SetSpinBuddy(hDlg, IDC_PCD_REDBUTTON);
		SetSpinBuddy(hDlg, IDC_PCD_GREENBUTTON);
		SetSpinBuddy(hDlg, IDC_PCD_BLUEBUTTON);
		SetSpinBuddy(hDlg, IDC_PCD_SATBUTTON);

		InitDlgItemSpin( hDlg, IDC_PCD_BRIGHTBUTTON, lpEditOptions->BrightButton, YES, SHRT_MIN, SHRT_MAX );
		InitDlgItemSpin( hDlg, IDC_PCD_REDBUTTON, lpEditOptions->RedButton, YES, SHRT_MIN, SHRT_MAX );
		InitDlgItemSpin( hDlg, IDC_PCD_GREENBUTTON, lpEditOptions->GreenButton, YES, SHRT_MIN, SHRT_MAX );
		InitDlgItemSpin( hDlg, IDC_PCD_BLUEBUTTON, lpEditOptions->BlueButton, YES, SHRT_MIN, SHRT_MAX );
		InitDlgItemSpin( hDlg, IDC_PCD_SATBUTTON, lpEditOptions->SatButton, YES, SHRT_MIN, SHRT_MAX );
		CheckDlgButton(hDlg, IDC_PCD_SUBTRACTSBA, lpEditOptions->SubtractSBA); 
		CheckDlgButton(hDlg, IDC_PCD_OUTOFGAMUT, lpEditOptions->OutOfGamut); 
		InitExtNameN( hDlg, IDC_EXTNAMES, IDC_FILENAME,
				lpEditOptions->LutName, IDN_PCDLUT, IDS_NONE, NO);
		LookupExtFileN(lpEditOptions->LutName, lpEditOptions->LutFileName, IDN_PCDLUT, FALSE);
	}
	else
	if (idCategory == IDD_PHOTOCDCMS)
	{
		CmsPhotoCDInit( hDlg );
	}

}

/************************************************************************/
LOCAL void PCDOptions_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange)
/************************************************************************/
{
if (IsOurWindow(hwndPaletteChange))
	AstralControlPaint(hDlg, IDC_PCD_PREVIEW);
}

/************************************************************************/
LOCAL void PCDOptions_OnCommand(HWND hDlg, int id, HWND hCtlWnd, UINT codeNotify)
/************************************************************************/
{
RECT 		rRect;
HWND 		hControl;
BOOL 		Bool;
int 		PhotoCDRes;

CCmsPhotoCDOpenDlg PcdDlg( &pcdCmsInfo,
	CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );

switch (id)
	{
	case IDC_CMS:
		if( Control.CMSEnabled )
			{
			Control.fPCDUseCMS = !Control.fPCDUseCMS;
			CheckDlgButton(hDlg, IDC_CMS, Control.fPCDUseCMS);
			if (Control.fPCDUseCMS)
				lpEditOptions->cmsInfo = pcdCmsInfo;
			else
				cmsInfoInit(&lpEditOptions->cmsInfo);
			ControlEnable(hDlg, IDC_PCD_DOPREVIEW, !Control.fPCDUseCMS);
			ControlEnable(hDlg, IDC_PCDFORMAT, !Control.fPCDUseCMS);
			}
		PCDOptions_Update(hDlg);
	break;

	case IDC_CMS_ENABLED:
		PcdDlg.DoModal();
		lpEditOptions->cmsInfo = pcdCmsInfo;
		CmsPhotoCD_OnInitDialog(hDlg);
		CmsPhotoCDInit( hDlg );
	break;

	case IDC_EXTNAMES:
	if ( codeNotify != CBN_SELCHANGE )
		break;
	GetExtNameN( hDlg, id, IDC_FILENAME, lpEditOptions->LutName,
		IDN_PCDLUT, codeNotify, IDS_NONE, NO );
	LookupExtFileN(lpEditOptions->LutName, lpEditOptions->LutFileName, IDN_PCDLUT, FALSE);
	break;

	case IDC_EXTMANAGE:
		PopupMenuEx( hDlg, id, IDC_EXTMANAGE, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			lpEditOptions->LutName, IDN_PCDLUT, id, IDS_NONE );
		LookupExtFileN(lpEditOptions->LutName, lpEditOptions->LutFileName, IDN_PCDLUT, FALSE);
	break;

	case IDC_PCD_BRIGHTBUTTON:
	lpEditOptions->BrightButton = GetDlgItemSpin( hDlg, id, &Bool, YES );
	break;
	case IDC_PCD_REDBUTTON:
	lpEditOptions->RedButton = GetDlgItemSpin( hDlg, id, &Bool, YES );
	break;
	case IDC_PCD_GREENBUTTON:
	lpEditOptions->GreenButton = GetDlgItemSpin( hDlg, id, &Bool, YES );
	break;
	case IDC_PCD_BLUEBUTTON:
	lpEditOptions->BlueButton = GetDlgItemSpin( hDlg, id, &Bool, YES );
	break;
	case IDC_PCD_SATBUTTON:
	lpEditOptions->SatButton = GetDlgItemSpin( hDlg, id, &Bool, YES );
	break;
	case IDC_PCD_OUTOFGAMUT:
	lpEditOptions->OutOfGamut = !lpEditOptions->OutOfGamut;
	CheckDlgButton(hDlg, id, lpEditOptions->OutOfGamut);
	break;
	case IDC_PCD_SUBTRACTSBA:
	lpEditOptions->SubtractSBA = !lpEditOptions->SubtractSBA;
	CheckDlgButton(hDlg, id, lpEditOptions->SubtractSBA);
	break;

	case IDC_PCDFORMAT:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	lpEditOptions->PhotoCDFormat = id-IDC_PCDSINGLE+PCD_SINGLE;
	GetPCDImageSize(&iImageWidth, &iImageHeight, &iImageDepth);
	ImageMemory( hDlg, IDC_PCD_IMAGESIZE, FGET(iImageWidth, 1 ),
		FGET( iImageHeight, 1 ), 1/*Resolution*/, iImageDepth );
	UpdateMemoryReadout( hDlg, IDC_PCD_CROPSIZE, &rGridRect );
	break;
	
	case IDC_PCDRESOLUTION:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	lpEditOptions->PhotoCDRes = id-IDC_PCD_BASE_OVER_16+PCD_BASE_OVER_16;
	Image2Control( (LPINT)&rGridRect.left, (LPINT)&rGridRect.top );
	Image2Control( (LPINT)&rGridRect.right, (LPINT)&rGridRect.bottom );
	GetPCDImageSize(&iImageWidth, &iImageHeight, &iImageDepth);
	SetImageControlMapping( iImageWidth, iImageHeight, &rControl );
	Control2Image( (LPINT)&rGridRect.left, (LPINT)&rGridRect.top );
	Control2Image( (LPINT)&rGridRect.right, (LPINT)&rGridRect.bottom );
	AstralControlRepaint(hDlg, IDC_PCD_PREVIEW);
	ImageMemory( hDlg, IDC_PCD_IMAGESIZE, FGET(iImageWidth, 1 ),
		FGET( iImageHeight, 1 ), 1/*Resolution*/, iImageDepth );
	UpdateMemoryReadout( hDlg, IDC_PCD_CROPSIZE, &rGridRect );
	break;
	
	case IDC_PCD_DOPREVIEW:
	if (lpPreviewFrame)
		FrameClose(lpPreviewFrame);
	lpPreviewFrame = NULL;
	GetPCDImageSize(&iImageWidth, &iImageHeight, &iImageDepth);
	if ( hControl = GetDlgItem( hDlg, IDC_PCD_PREVIEW ) )
		{
		GetClientRect( hControl, &rRect );

		// Open the passed in image at the right size
		AstralCursor(IDC_WAIT);

		CReadImage read(Names.Saved, BFT_PHOTOCD);

		PhotoCDRes = lpEditOptions->PhotoCDRes;
		lpEditOptions->PhotoCDRes = PCD_BASE_OVER_16;
		read.SetOptions(lpEditOptions);

		FRMTYPEINFO TypeInfo;

		if (iImageDepth == 1 && !Control.fPCDUseCMS)
			FrameSetTypeInfo(&TypeInfo, FDT_GRAYSCALE);
		else
			FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR);

		lpPreviewFrame = read.ReadFrame(TypeInfo);
		lpEditOptions->PhotoCDRes = PhotoCDRes;
		AstralCursor(NULL);
		}

	if (!lpPreviewFrame)
		break;

	// iImageWidth and iImageHeight are used for Image2Control mapping
	if ( !(iImageDepth  = FrameDepth(lpPreviewFrame)) )
		iImageDepth = 1;

	// Attach the image to the preview control
	AttachPreviewFrame( hDlg, IDC_PCD_PREVIEW, lpPreviewFrame );

	bHaveGrid = YES;
	// Initialize the grid rectangle, if necessary
	if ( AstralIsRectEmpty( &rGridRect ) )
		{ // Break it into thirds
		rGridRect.left = iImageWidth/3;
		rGridRect.top  = iImageHeight/3;
		rGridRect.right  = 2 * rGridRect.left;
		rGridRect.bottom = 2 * rGridRect.top;
		}
	UpdateMemoryReadout( hDlg, IDC_PCD_CROPSIZE, &rGridRect );
	break;

	case IDC_PCD_PREVIEW:
	UpdateMemoryReadout( hDlg, IDC_PCD_CROPSIZE, &rGridRect );
	break;

	case IDOK:
	// Unattach the preview frame
	StorePCDPreferences(lpEditOptions);

	ColorManager.PhotoCD = pcdCmsInfo;
	CmsSavePhotoCDSelection( hDlg );
	SavePhotoCDSetup();

	if (lpPreviewFrame)
		{
		lpPreviewFrame = AttachPreviewFrame( hDlg, IDC_PCD_PREVIEW, NULL );
		FrameClose(lpPreviewFrame);
		}
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	// Unattach any preview frame
	if (lpPreviewFrame)
		{
		lpPreviewFrame = AttachPreviewFrame( hDlg, IDC_PCD_PREVIEW, NULL );
		FrameClose(lpPreviewFrame);
		}
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL PCDResolution_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PCDResolution_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPCDResolutionProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PCDResolution_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PCDResolution_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PCDRESOLUTION);
	}
}

/************************************************************************/
LOCAL BOOL PCDResolution_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
PCDphotoHdl	hPcd;	// handle to selected image pac
PCDstatus status;
DWORD dwRes, dwMyRes;
short wResCount;
PCDresolution pcdRes;
BOOL fResSupported;
int dpi;
PCDresolution res;

CenterPopup( hDlg );

lpEditOptions = (LPPCDOPTIONS)lParam;

if (PCDopen(Names.Saved, &hPcd))
	return(TRUE);
status = PCDgetCount(hPcd, &dwRes, &wResCount);
PCDclose(hPcd);
if (status)
	return(TRUE);

for (res = PCD_64BASE; res >= PCD_BASE_OVER_16;)
	{
	dwMyRes = 1 << (res-PCD_BASE_OVER_16+1);
	fResSupported = (dwMyRes & dwRes) != 0;
	ControlEnable(hDlg, (int)res-PCD_BASE_OVER_16+IDC_PCD_BASE_OVER_16, fResSupported);
	res = (PCDresolution)((int)res - 1);
	}
CheckComboItem(hDlg, IDC_PCDRESOLUTION, IDC_PCD_BASE_OVER_16,
		IDC_PCD_64BASE, lpEditOptions->PhotoCDRes-PCD_BASE_OVER_16+IDC_PCD_BASE_OVER_16);

GetPCDImageSize(&iImageWidth, &iImageHeight, &iImageDepth);
GetPCDResolution(lpEditOptions->PhotoCDRes, &pcdRes, &dpi);

rGridRect.left = FMUL(dpi, lpEditOptions->rArea.left);
rGridRect.top = FMUL(dpi, lpEditOptions->rArea.top);
rGridRect.right = FMUL(dpi, lpEditOptions->rArea.right);
rGridRect.bottom = FMUL(dpi, lpEditOptions->rArea.bottom);
UpdateMemoryReadout( hDlg, IDC_PCD_CROPSIZE, &rGridRect );

return(TRUE);
}

/************************************************************************/
LOCAL void PCDResolution_OnCommand(HWND hDlg, int id, HWND hCtlWnd, UINT codeNotify)
/************************************************************************/
{
PCDresolution pcdRes;
int res;

switch (id)
	{
	case IDC_PCDRESOLUTION:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	lpEditOptions->PhotoCDRes = id-IDC_PCD_BASE_OVER_16+PCD_BASE_OVER_16;
	GetPCDResolution(lpEditOptions->PhotoCDRes, &pcdRes, &res);
	rGridRect.left = FMUL(res, lpEditOptions->rArea.left);
	rGridRect.top = FMUL(res, lpEditOptions->rArea.top);
	rGridRect.right = FMUL(res, lpEditOptions->rArea.right);
	rGridRect.bottom = FMUL(res, lpEditOptions->rArea.bottom);
	
	GetPCDImageSize(&iImageWidth, &iImageHeight, &iImageDepth);
	UpdateMemoryReadout( hDlg, IDC_PCD_CROPSIZE, &rGridRect );
	break;
	
	case IDOK:
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

/************************************************************************/
VOID WINPROC EXPORT PhotoCDProgress(short nNum, short nDenom, long lData)
/************************************************************************/
{
int	y;

y = nCurrentLine + (((long)nNum * (long)nLinesRead)/(long)nDenom);
if (AstralClockCursor(y, (int)lData, YES))
	fAbort = TRUE;
else
	fAbort = FALSE;

}

/************************************************************************/
BOOL WINPROC EXPORT PhotoCDAbort(long lData)
/************************************************************************/
{
return(fAbort);
}

LPSTR BASED_CODE PhotoCDFormat_Str	= "PhotoCDFormat";
LPSTR BASED_CODE PhotoCDRes_Str 	= "PhotoCDRes";
LPSTR BASED_CODE BrightButton_Str   = "BrightButton";
LPSTR BASED_CODE RedButton_Str   	= "RedButton";
LPSTR BASED_CODE GreenButton_Str   	= "GreenButton";
LPSTR BASED_CODE BlueButton_Str   	= "BlueButton";
LPSTR BASED_CODE SatButton_Str   	= "SatButton";
LPSTR BASED_CODE OutOfGamut_Str   	= "OutOfGamut";
LPSTR BASED_CODE SubtractSBA_Str   	= "SubtractSBA";
LPSTR BASED_CODE PCDTableName_Str  	= "PCDTableName";

/***********************************************************************/
static void LoadPCDPreferences(LPPCDOPTIONS lpOptions)
/***********************************************************************/
{
lpOptions->PhotoCDRes = GetDefaultInt(PhotoCDRes_Str, PCD_BASE);
lpOptions->PhotoCDFormat	= GetDefaultInt(PhotoCDFormat_Str, PCD_RGB);
lpOptions->BrightButton 	= GetDefaultInt(BrightButton_Str, 0);
lpOptions->RedButton 		= GetDefaultInt(RedButton_Str, 0);
lpOptions->GreenButton 		= GetDefaultInt(GreenButton_Str, 0);
lpOptions->BlueButton 	 	= GetDefaultInt(BlueButton_Str, 0);
lpOptions->SatButton  		= GetDefaultInt(SatButton_Str, 0);
lpOptions->OutOfGamut  		= GetDefaultInt(OutOfGamut_Str, FALSE);
lpOptions->SubtractSBA 		= GetDefaultInt(SubtractSBA_Str, FALSE);
GetDefaultString(PCDTableName_Str, "", lpOptions->LutName, sizeof(lpOptions->LutName));
LookupExtFileN(lpOptions->LutName, lpOptions->LutFileName, IDN_PCDLUT, FALSE);
lpOptions->fHaveArea = NO;
}

/***********************************************************************/
static void StorePCDPreferences(LPPCDOPTIONS lpOptions)
/***********************************************************************/
{
PutDefaultInt(PhotoCDRes_Str, lpOptions->PhotoCDRes);
PutDefaultInt(PhotoCDFormat_Str, lpOptions->PhotoCDFormat);
PutDefaultInt(BrightButton_Str, lpOptions->BrightButton);
PutDefaultInt(RedButton_Str, lpOptions->RedButton);
PutDefaultInt(GreenButton_Str, lpOptions->GreenButton);
PutDefaultInt(BlueButton_Str, lpOptions->BlueButton);
PutDefaultInt(SatButton_Str, lpOptions->SatButton);
PutDefaultInt(OutOfGamut_Str, lpOptions->OutOfGamut);
PutDefaultInt(SubtractSBA_Str, lpOptions->SubtractSBA);
PutDefaultString(PCDTableName_Str, lpOptions->LutName);
}

/***********************************************************************/
LOCAL void FreeUpPCDOptions(LPPCDOPTIONS lpOptions)
/***********************************************************************/
{
	if (!lpOptions)
		return;
//	if (lpOptions->hLut)
//		GlobalFree(lpOptions->hLut);
	FreeUp(lpOptions);
}

/************************************************************************/
static BOOL FastbitsToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg )
/************************************************************************/
{
POINT pt;
static HDC shDC;
static RECT SelectRect;
static BOOL bTrack;

switch (msg)
	{
	case WM_SIZE: // Values used for Image2Control mapping
	rControl.right  = rControl.left + x - 1;
	rControl.bottom = rControl.top  + y - 1;
	SetImageControlMapping( iImageWidth, iImageHeight, &rControl );
	break;

	case WM_MOVE: // Values used for Image2Control mapping
	rControl.left = x;
	rControl.top = y;
	SetImageControlMapping( iImageWidth, iImageHeight, &rControl );
	break;

	case WM_LBUTTONDOWN:
	if (bTrack)
		break;
	bTrack = YES;
	UpdateWindow( hWindow );
	shDC = GetDC( hWindow );
	SetClipDC( shDC, &rControl );
	if ( !bGridLock )
		{
		pt.x = x; pt.y = y;
		DrawGrid( hWindow, shDC, &rGridRect, SL_BOX ); // turn it off
		StartSelection( hWindow, shDC, &SelectRect, SL_BOX | SL_SPECIAL,
			pt, 0L );
		rGridRect = SelectRect;
		Control2Image( (LPINT)&rGridRect.left, (LPINT)&rGridRect.top );
		Control2Image( (LPINT)&rGridRect.right, (LPINT)&rGridRect.bottom );
		}
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0L );
	break;

	case WM_LBUTTONUP:
	if (!bTrack)
		break;
	bTrack = NO;
	if ( !bGridLock )
		{
		bHaveGrid = YES;
		EndSelection( hWindow, shDC, &SelectRect, SL_BOX, YES );
		rGridRect = SelectRect;
		Control2Image( (LPINT)&rGridRect.left, (LPINT)&rGridRect.top );
		Control2Image( (LPINT)&rGridRect.right, (LPINT)&rGridRect.bottom );
		DrawGrid( hWindow, shDC, &rGridRect, SL_BOX ); // turn it on
		}
	if ( shDC )
		ReleaseDC( hWindow, shDC );
	shDC = NULL;
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0L );
	break;

	case WM_MOUSEMOVE:
	if (!bTrack)
		break;
	if ( !bGridLock )
		{
		pt.x = x;
		pt.y = y;
		UpdateSelection( hWindow, shDC, &SelectRect, SL_BOX,
			pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||RBUTTON,FROMCENTER);
		rGridRect = SelectRect;
		Control2Image( (LPINT)&rGridRect.left, (LPINT)&rGridRect.top );
		Control2Image( (LPINT)&rGridRect.right, (LPINT)&rGridRect.bottom );
		}
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0L );
	break;

	case WM_LBUTTONDBLCLK:
	rGridRect.left = rGridRect.top = 0;
	rGridRect.right = iImageWidth - 1;
	rGridRect.bottom = iImageHeight - 1;
	bHaveGrid = NO;
	InvalidateRect(hWindow, NULL, NO);
	UpdateWindow(hWindow);
	break;

	case WM_PAINT:
	SetClipDC( hDC, &rControl );
	DrawGrid( hWindow, hDC, &rGridRect, SL_BOX );
	break;
	}

return(TRUE);
}


/***********************************************************************/
static void UpdateMemoryReadout( HWND hDlg, ITEMID id, LPRECT lpGridRect )
/***********************************************************************/
{
RECT rRect;

rRect = *lpGridRect;
BoundRect( &rRect, 0, 0, iImageWidth-1, iImageHeight-1 );
ImageMemory( hDlg, id, FGET( RectWidth(&rRect), 1 ),
	FGET( RectHeight(&rRect), 1 ), 1/*Resolution*/, iImageDepth );
}

/************************************************************************/
static LPFRAME AttachPreviewFrame( HWND hDlg, ITEMID id, LPFRAME lpFrame )
/************************************************************************/
{
HWND hControl;
LPFRAME lpOldFrame;

// Get the handle to the preview image control
if ( !(hControl = GetDlgItem( hDlg, id )) )
	return( FALSE );

// Link the passed in frame to the sample image control
lpOldFrame = (LPFRAME)GetWindowLong( hControl, GWL_IMAGE );
SetWindowLong( hControl, GWL_IMAGE, (long)lpFrame );
SetWindowLong( hControl, GWL_IMAGE_TOOLPROC, (long)FastbitsToolProc );
InvalidateRect( hControl, NULL, NO );
return( lpOldFrame );
}

/************************************************************************/
static void GetPCDImageSize(LPINT lpWidth, LPINT lpHeight, LPINT lpDepth)
/************************************************************************/
{
int temp;

if (lpEditOptions->PhotoCDRes == PCD_BASE_OVER_16)
	{
	*lpWidth = 192;
	*lpHeight = 128;
	}
else if (lpEditOptions->PhotoCDRes == PCD_BASE_OVER_4)
	{
	*lpWidth = 384;
	*lpHeight = 256;
	}
else if (lpEditOptions->PhotoCDRes == PCD_4BASE)
	{
	*lpWidth = 1536;
	*lpHeight = 1024;
	}
else if (lpEditOptions->PhotoCDRes == PCD_16BASE)
	{
	*lpWidth = 3072;
	*lpHeight = 2048;
	}
else if (lpEditOptions->PhotoCDRes == PCD_64BASE)
	{
	*lpWidth = 6144;
	*lpHeight = 4096;
	}
else //if (lpEditOptions->PhotoCDRes == PCD_BASE)
	{
	*lpWidth = 768;
	*lpHeight = 512;
	}
if (PCDTransform == PCD_ROTATE_90 || PCDTransform == PCD_ROTATE_270)
	{
	temp = *lpWidth;
	*lpWidth = *lpHeight;
	*lpHeight = temp;
	}
if (lpEditOptions->PhotoCDFormat == PCD_SINGLE)
	*lpDepth = 1;
else
	*lpDepth = 3;
}

/************************************************************************/
LOCAL void GetPCDResolution(int PhotoCDRes, PCDresolution *pcdRes, LPINT lpRes)
/************************************************************************/
{
	if (PhotoCDRes == PCD_BASE_OVER_16)
 	{
		*pcdRes = PCD_BASE_OVER_16;
		*lpRes = 25;
 	}
	else if (PhotoCDRes == PCD_BASE_OVER_4)
 	{
		*pcdRes = PCD_BASE_OVER_4;
		*lpRes = 50;
 	}
	else if (PhotoCDRes == PCD_4BASE)
 	{
		*pcdRes = PCD_4BASE;
		*lpRes = 200;
 	}
	else if (PhotoCDRes == PCD_16BASE)
 	{
		*pcdRes = PCD_16BASE;
		*lpRes = 400;
 	}
	else if (PhotoCDRes == PCD_64BASE)
 	{
		*pcdRes = PCD_64BASE;
		*lpRes = 800;
 	}
	else //if (PhotoCDRes == PCD_BASE)
 	{
		*pcdRes = PCD_BASE;
		*lpRes = 100;
 	}
}
