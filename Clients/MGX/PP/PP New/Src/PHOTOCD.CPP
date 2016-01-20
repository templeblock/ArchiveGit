 //®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "pcdlib.h"
#include <ctype.h>

// Static prototypes
static BOOL FastbitsToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg );
static LPFRAME AttachPreviewFrame( HWND hDlg, ITEMID id, LPFRAME lpFrame );
static void UpdateMemoryReadout( HWND hDlg, ITEMID id, LPRECT lpGridRect );
static void GetPCDImageSize(LPINT lpWidth, LPINT lpHeight, LPINT lpDepth);
static void LoadPCDPreferences(LPPCDOPTIONS lpOptions);
static void StorePCDPreferences(LPPCDOPTIONS lpOptions);
LOCAL HANDLE LoadPCDLut(LPSTR lpLutName, LPINT lpEntries);
LOCAL LPSTR FindNumber(LPSTR lp, LPSTR lpEnd, LPBYTE lpValue);
LOCAL void FreeUpPCDOptions(LPPCDOPTIONS lpOptions);
LOCAL void GetPCDResolution(int PhotoCDRes, PCDresolution *pcdRes, LPINT lpRes);
LOCAL void FillPCDInfoList(HWND hDlg, ITEMID idList, PCDpacInfoPtr pInfo);
LOCAL void AddPCDInfo(HWND hDlg, ITEMID idList, ITEMID idDesc, int length, LPSTR lpInfo);

VOID WINPROC EXPORT PhotoCDProgress(short nNum, short nDenom, long lData);
BOOL WINPROC EXPORT PhotoCDAbort(long lData);
BOOL WINPROC EXPORT DlgPCDResolutionProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

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
	if (PhotoCDRes < IDC_PCD_BASE_OVER_16)
		PhotoCDRes = IDC_PCD_BASE_OVER_16;
	if (PhotoCDRes > IDC_PCD_64BASE)
		PhotoCDRes = IDC_PCD_64BASE;
	return(PhotoCDRes != OldPhotoCDRes);
}

/************************************************************************/
BOOL CReadImage::PCDRead()
/************************************************************************/
{
#ifndef WIN32
PCDphotoHdl	hPcd;	// handle to selected image pac
PCDtransform Transform;
PCDresolution pcdRes;
PCDformat pcdFormat;
PCDbuttonRec pcdButtons;
RECT rImage;
LPTR lpOut, lpConvertLine;
HPTR lpIn, lpBuffer, lpLine, lpCMYKBuffer;
long lStride, dxBytes;
long nLines, nLinesLeft;
LPFRAME lpFrame;
int npix, nlin, y, xres;
PCDpacInfoRec info;
FARPROC lpProgressProc, lpAbortProc;
long lW, lH, lMaxMem, lMemAlloc;
int iAllocTries;
short stat;
LPPCDOPTIONS lpOptions;
int PhotoCDRes;
FRMTYPEINFO inType, outType;
CFrameTypeConvert TypeConvert;
BOOL fConvert;
int indepth;
HANDLE hLut;
int nEntries;
ITEMID id;
short wResCount;
DWORD dwRes, dwMyRes;
BOOL fRet = FALSE, fResSupported;
CMSINFO CmsInfo;
BOOL fUsingCMS;

if (m_lpOptions)
	lpOptions = (LPPCDOPTIONS)m_lpOptions;
else
	{
	lpOptions = GetPCDOptions();
	if (lpOptions)
		m_cmsInfo = lpOptions->cmsInfo;
	}

if (!lpOptions)
	return(FALSE);

ProgressBegin(1, PROGRESS_ID(IDS_UNDOLOADIMAGE));

lpFrame = NULL;
lpBuffer = NULL;
lpConvertLine = NULL;
hPcd = NULL;
lpProgressProc = NULL;
lpAbortProc = NULL;
outType = m_outType;

// Call PCDopen to obtain an image pac handle
if(PCDopen(m_lpFileName, &hPcd))
	{
	if (!m_lpOptions)
		FreeUpPCDOptions(lpOptions);
	Message (IDS_EOPEN, m_lpFileName);
	ProgressEnd();
	return(FALSE);
	}

if (PCDgetCount(hPcd, &dwRes, &wResCount))
	{
	if (!m_lpOptions)
		FreeUpPCDOptions(lpOptions);
	PCDclose(hPcd);
	Message (IDS_EREAD, m_lpFileName);
	ProgressEnd();
	return(FALSE);
	}

// Read image information which is documented nowhere!!!
if (PCDreadImageInfo(hPcd, &info))
	{
	if (!m_lpOptions)
		FreeUpPCDOptions(lpOptions);
	PCDclose(hPcd);
	Message (IDS_EREAD, m_lpFileName);
	ProgressEnd();
	return(FALSE);
	}

//if (info.mediaId == PCD_BW_NEGATIVE ||
//    info.mediaId == PCD_BW_REVERSAL ||
//    info.mediaId == PCD_BW_HARDCOPY)
CmsInfo = m_cmsInfo;

if (lpOptions->PhotoCDFormat == IDC_PCDSINGLE)
	{
	FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
	m_inDataType = IDC_SAVECT;
	}
else
	{
	FrameSetTypeInfo(&inType, FDT_RGBCOLOR, NULL, &CmsInfo.src.ptInfo);
	m_inDataType = IDC_SAVE24BITCOLOR;
	}
if (outType.DataType == FDT_NONE)
	outType = inType;
else
	{
	if (outType.DataType == FDT_GRAYSCALE || outType.DataType == FDT_LINEART)
		FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
	else
		FrameSetTypeInfo(&inType, FDT_RGBCOLOR, NULL, &CmsInfo.src.ptInfo);
	}
if (inType.DataType == FDT_GRAYSCALE)
	{
	indepth = 1;
	pcdFormat = PCD_SINGLE;
	}
else
	{
	indepth = 3;
	pcdFormat = ( Control.CMSEnabled && IsDstPTSelected( &outType.ptInfo ) &&
		IsSrcPTSelected( &inType.ptInfo ) ) ? PCD_YCC : PCD_RGB;
	}

// use override to the resolution and then bound it
// and then check if it is supported
PhotoCDRes = lpOptions->PhotoCDRes + m_PhotoCDResOverride;

if (PhotoCDRes < IDC_PCD_BASE_OVER_16)
	PhotoCDRes = IDC_PCD_BASE_OVER_16;
if (PhotoCDRes > IDC_PCD_64BASE)
	PhotoCDRes = IDC_PCD_64BASE;

for (id = IDC_PCD_64BASE; id >= IDC_PCD_BASE_OVER_16; --id)
	{
	dwMyRes = 1 << (id-IDC_PCD_BASE_OVER_16+1);
	fResSupported = (dwMyRes & dwRes) != 0;
	if (PhotoCDRes == id && !fResSupported)
		--PhotoCDRes;
	}
// make sure new resolution is setup
lpOptions->PhotoCDRes = PhotoCDRes;

GetPCDResolution(PhotoCDRes, &pcdRes, &xres);

// Call PCDgetRotation() to ascertain the affine transformation
// necessary to specify before retrieving the image in order
// to display the image rightside-up.
//
// In preparation for PCDloadImage(), specify the transformation
// to be used in retrieving data from the image pac.

// try to set the color buttons
pcdButtons.BrightButton = lpOptions->BrightButton;
pcdButtons.RedButton = lpOptions->RedButton;
pcdButtons.GreenButton = lpOptions->GreenButton;
pcdButtons.BlueButton = lpOptions->BlueButton;
pcdButtons.SatButton = lpOptions->SatButton;
pcdButtons.OutOfGamut = lpOptions->OutOfGamut;
pcdButtons.SubtractSBA = lpOptions->SubtractSBA;
PCDsetColorButtons(hPcd, &pcdButtons);

hLut = LoadPCDLut(lpOptions->LutName, &nEntries);
if (hLut)
	{
	PCDsetLut(hPcd, hLut, nEntries);
	GlobalFree(hLut);
	}

if(PCDgetRotation(hPcd, &Transform))
	goto BadRead;
if (PCDsetTransform(hPcd, Transform))
	goto BadRead;
if (PCDsetFormat(hPcd, pcdFormat))
	goto BadRead;
if (PCDsetPlaneColumn(hPcd, 1L, (short)indepth))
	goto BadRead;
if (PCDsetResolution(hPcd, pcdRes))
	goto BadRead;
if (PCDgetSize(hPcd, &rImage))
	goto BadRead;
npix = rImage.right;
nlin = rImage.bottom;

if ( lpOptions->fHaveArea )
	{
	rImage.left = FMUL(xres, lpOptions->rArea.left);
	rImage.top = FMUL(xres, lpOptions->rArea.top);
	rImage.right = FMUL(xres, lpOptions->rArea.right);
	rImage.bottom = FMUL(xres, lpOptions->rArea.bottom);

	rImage.left = bound( rImage.left, 0, npix-1 );
	rImage.right = bound( rImage.right, 0, npix-1 );
	rImage.top = bound( rImage.top, 0, nlin-1 );
	rImage.bottom = bound( rImage.bottom, 0, nlin-1 );

	npix = RectWidth(&rImage);
	nlin = RectHeight(&rImage);
	}
else
	SetRect(&rImage, 0, 0, npix-1, nlin-1);

lpProgressProc = MakeProcInstance((FARPROC)PhotoCDProgress, PictPubApp.Get_hInstAstral());
if (!lpProgressProc)
	goto NoMemory;
if (PCDsetProgress(hPcd, lpProgressProc, (long)nlin))
	goto BadRead;
lpAbortProc = MakeProcInstance((FARPROC)PhotoCDAbort, PictPubApp.Get_hInstAstral());
if (!lpAbortProc)
	goto NoMemory;
if (PCDsetAbort(hPcd, lpAbortProc, 0L))
	goto BadRead;

fConvert = !FrameTypeInfoEqual( inType, outType);

fUsingCMS = Control.CMSEnabled && 
			IsSrcPTSelected(&inType.ptInfo) &&
			IsDstPTSelected(&outType.ptInfo);

if (fConvert)
	{
	if (!TypeConvert.Init(inType, outType, npix, m_DefaultDitherType))
		{
		Message(IDS_EMEMALLOC);
		goto BadPCD;	
		}
	lpConvertLine = Alloc((long)npix*indepth);
	if (!lpConvertLine)
		{
		Message(IDS_EMEMALLOC);
		goto BadPCD;	
		}
	}

if (! (lpFrame = FrameOpen(outType, npix, nlin, xres)))
	{
	FrameError(IDS_EIMAGEOPEN);
	goto BadPCD;
	}
lStride = (long)npix * (long)indepth;

#ifdef UNUSED
/*******************************************************/
// Old way of memory allocation PP 3.1
nLines = 65536L / lStride;
if (!(lpBuffer = Alloc(lStride*nLines)))
	{
	nLines = 7;
	if (!(lpBuffer = Alloc(lStride*nLines)))
		{
		Message (IDS_EMEMALLOC);
		goto BadRead;
		}
	}
/*******************************************************/
#endif

// try a little more than a MB to better optimize for medium resolution
// tries 512x768x3, 256x384x3, 128x192x3, and then 64K
// these size correspond to the PhotoCD resolutions
// leave at least a 100K around
lMaxMem = (long)AvailableMemory() - (MAIN_MEM_MINK*1024L);
lW = 512L; // start with PCD_BASE resolution
lH = 768L;
iAllocTries = 4;
while (--iAllocTries >= 0)
	{
	if (iAllocTries)
		nLines = (lW * lH * 3L ) / lStride;
	else // last time try 64K buffer
		{
		dxBytes = lStride;
		nLines = 65536L / lStride;
		}
	if (nLines > nlin)
		nLines = nlin;
	lMemAlloc = lStride * (long)nLines;
	if (lMemAlloc <= lMaxMem)
		if (lpBuffer = (HPTR)Alloc(lMemAlloc))
			{
			dxBytes = lStride;
			break;
			}
	lW /= 2;
	lH /= 2;
	}

if( fUsingCMS && outType.DataType == FDT_CMYKCOLOR )
	{	
	dxBytes = (long)npix * (long)4;
	iAllocTries = 4;
	while (--iAllocTries >= 0)
		{
		if (iAllocTries)
			nLines = (lW * lH * 4L ) / dxBytes;
		else // last time try 64K buffer
			nLines = 65536L / dxBytes;
		if (nLines > nlin)
			nLines = nlin;
		lMemAlloc = dxBytes * (long)nLines;
		if (lMemAlloc <= lMaxMem)
			if (lpCMYKBuffer = (HPTR)Alloc(lMemAlloc))
				break;
		lW /= 2;
		lH /= 2;
		}
	}
else
	lpCMYKBuffer = lpBuffer;

if (!lpBuffer || !lpCMYKBuffer)
	goto NoMemory;
//Print("npix = %d nlin = %d lStride = %ld nLines = %d buffer size = %ld maxmem //= %ld memory left = %ld", npix, nlin, lStride, nLines, lMemAlloc, lMaxMem, //AvailableMemory());
nLinesLeft = nlin;
++rImage.right; // PhotoCD toolkit uses Windows style rectangles
rImage.bottom = rImage.top;
y = 0;
fAbort = FALSE; // for progress and abort procs
while (nLinesLeft > 0)
	{
	nCurrentLine = y; // for PhotoCDProgress proc
	nLinesRead = nLines;
	if (nLinesRead > nLinesLeft)
		nLinesRead = nLinesLeft;
	nLinesLeft -= nLinesRead;
	rImage.top = rImage.bottom;
	rImage.bottom = rImage.top + nLinesRead;
	if (stat = PCDgetBlock(hPcd, &rImage, lpBuffer, lStride))
		{
		if (fAbort)
			goto Cancelled;
		else
			{
			Message(IDS_PCDERROR, stat);
			goto BadRead;
			}
		}

	if( fConvert && fUsingCMS)
	{
		TypeConvert.ConvertData(lpBuffer, lpCMYKBuffer, y, npix, nLinesRead);
	}

	lpLine = lpCMYKBuffer + ((long)(nLinesRead-1) * dxBytes);

	while (--nLinesRead >= 0)
		{
		lpOut = FramePointerRaw(lpFrame, 0, y, YES);
		if (!lpOut)
			goto BadWrite;
		lpIn = lpLine;
		if (fConvert && !fUsingCMS)
			{
			CopyFromHuge(lpIn, lpConvertLine, lStride);
			swapBGR(lpConvertLine, lpConvertLine, npix);
			TypeConvert.ConvertData(lpConvertLine, lpOut, y, npix, 1);
			}
		else
		if (fConvert)
			{
			CopyFromHuge(lpIn, lpOut, dxBytes);
			}
		else
			{
			CopyFromHuge(lpIn, lpOut, lStride);
			if (outType.DataType == FDT_RGBCOLOR)
				swapBGR(lpOut, lpOut, npix);
			}
	  	++y;
	  	lpLine -= dxBytes;
		}
	}

m_iWidth = npix;
m_iHeight = nlin;
m_iRes = xres;
m_lpFrame = lpFrame;
fRet = TRUE;
goto Exit;

NoMemory:
Message (IDS_EMEMALLOC);
goto BadPCD;

BadRead:
Message (IDS_EREAD, m_lpFileName);
goto BadPCD;

BadWrite:
FrameError(IDS_EFRAMEREAD);

Cancelled:
BadPCD:
Exit:
ProgressEnd();
if ( !fRet )
	{
	if ( lpFrame )
		FrameClose( lpFrame );
	}
if (!m_lpOptions)
	FreeUpPCDOptions(lpOptions);
if (hPcd)
	PCDclose(hPcd);
if (lpBuffer)
	FreeUp (lpBuffer);
if (lpCMYKBuffer)
	FreeUp (lpCMYKBuffer);
if (lpConvertLine)
	FreeUp(lpConvertLine);
if (lpProgressProc)
	FreeProcInstance(lpProgressProc);
if (lpAbortProc)
	FreeProcInstance(lpAbortProc);
return(fRet);
#endif
return(FALSE);
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

#ifndef WIN32

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
//		lpOptions->PhotoCDFormat = IDC_PCDSINGLE;
//	else
		lpOptions->PhotoCDFormat = IDC_PCDRGB;

	bGridLock = NO;
	AstralSetRectEmpty(&rGridRect);
	lstrcpy( Names.Saved, m_lpFileName );
	if (!AstralDlgParam(NO, PictPubApp.Get_hInstAstral(), PictPubApp.Get_hWndAstral(), IDD_PCDOPTIONS,
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
#endif
	return(lpOptions);
}

/************************************************************************/
BOOL CReadImage::PromptForResolution(LPSTR lpFileName, LPPCDOPTIONS lpOptions)
/************************************************************************/
{
	lstrcpy( Names.Saved, lpFileName );
	return((BOOL)AstralDlgParam(NO, PictPubApp.Get_hInstAstral(), PictPubApp.Get_hWndAstral(), IDD_PCDRESOLUTION,
		DlgPCDResolutionProc, (LPARAM)lpOptions));
}

/************************************************************************/
LPFRAME ReadPCDThumbnail( LPSTR ifile, short nImgNo, int outdepth,
                           LPINT lpDataType )
/************************************************************************/
{
#ifndef WIN32
PCDoviewHdl	hPcd;	// handle to selected image pac
PCDtransform Transform;
PCDformat pcdFormat;
RECT rImage;
int indepth;
LPTR lpOut;
LPTR lpIn, lpBuffer;
long lStride;
int nLines;
LPFRAME lpOldFrame, lpFrame;
int npix;
int nlin;
int y, i, iCount;
FNAME szOverview;
char szNum[5];

lpOldFrame = NULL;
lpFrame = NULL;
lpBuffer = NULL;
hPcd = NULL;

if (!nImgNo && !IsPCDImage(ifile))
	goto BadRead;

if (!nImgNo)
	{
	lstrcpy(szOverview, filename(ifile));
	for (i = 0; i < 4; ++i)
		{
		szNum[i] = szOverview[i+3];
		if (szNum[i] < '0' || szNum[i] > '9')
			goto BadRead;
		}
	szNum[i] = '\0';
	nImgNo = atoi(szNum);
	}
if (!GetPCDOverview(ifile, szOverview))
	{
	Message (IDS_EOPEN, ifile);
	goto	BadPCD;
	}

// Call PCDopen to obtain an image pac handle
if(PCDOopen(szOverview, &hPcd))
	{
	Message (IDS_EOPEN, ifile);
	goto	BadPCD;
	}

// Call PCDgetRotation() to ascertain the affine transformation
// necessary to specify before retrieving the image in order
// to display the image rightside-up.
//
// In preparation for PCDloadImage(), specify the transformation
// to be used in retrieving data from the image pac.
if (PCDOsetSelect(hPcd, nImgNo))
	goto BadRead;
if (PCDOgetRotation(hPcd, &Transform))
	goto BadRead;
if (PCDOsetTransform(hPcd, Transform))
	goto BadRead;
indepth = 3;
if (outdepth < 0)
	outdepth = indepth;
if (outdepth < 1)
	outdepth = 1;
else if (outdepth > 1)
	outdepth = 3;
if (outdepth == 1)
	pcdFormat = PCD_SINGLE;
else
	pcdFormat = PCD_RGB;
if (PCDOsetFormat(hPcd, pcdFormat))
	goto BadRead;
if (PCDOsetPlaneColumn(hPcd, 1L, 3))
	goto BadRead;
if (PCDOsetResolution(hPcd, PCD_BASE_OVER_64))
	goto BadRead;
if (PCDOgetSize(hPcd, &rImage))
	goto BadRead;
npix = rImage.right;
nlin = rImage.bottom;

if (! (lpFrame = FrameOpen((FRMDATATYPE)outdepth, npix, nlin, 100)))
	{
	FrameError(IDS_EIMAGEOPEN);
	goto BadPCD;
	}

lStride = (long)npix * (long)outdepth;
nLines = nlin;
if (!(lpBuffer = Alloc(lStride*nLines)))
	goto NoMemory;

y = 0;
if (PCDOgetBlock(hPcd, lpBuffer, lStride))
	goto BadRead;
lpIn = lpBuffer + ((long)(nLines-1) * lStride);
while (--nlin >= 0)
	{
	lpOut = FramePointer(lpFrame, 0, y, YES);
	if (!lpOut)
		goto BadWrite;
	iCount = npix;
	if (outdepth == 1)
		copy(lpIn, lpOut, npix);
	else
		Convert24to24((LPRGB)lpIn, npix, NULL, lpOut, 0, 0, NULL);
	++y;
	lpIn -= lStride;
	}

if (lpDataType)
	{
	if (outdepth == 1)
		*lpDataType = IDC_SAVECT;
	else
		*lpDataType = IDC_SAVE24BITCOLOR;
	}
if (hPcd)
	PCDOclose(hPcd);
if (lpBuffer)
	FreeUp (lpBuffer);
return (lpFrame);

NoMemory:
Message (IDS_EMEMALLOC);
goto BadPCD;

BadRead:
Message (IDS_EREAD, ifile);
goto BadPCD;

BadWrite:
FrameError(IDS_EFRAMEREAD);

BadPCD:
if (hPcd)
	PCDOclose(hPcd);
if (lpBuffer)
	FreeUp (lpBuffer);
if ( lpFrame )
	FrameClose( lpFrame );
#endif
return (NULL);
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
AstralDlg( NO, PictPubApp.Get_hInstAstral(), hDlg, IDD_PCDFILEINFO, DlgPCDFileInfoProc);
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
#ifndef WIN32
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
#endif
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

CenterPopup( hDlg );

#ifndef WIN32

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

for (id = IDC_PCD_64BASE; id >= IDC_PCD_BASE_OVER_16; --id)
	{
	dwMyRes = 1 << (id-IDC_PCD_BASE_OVER_16+1);
	fResSupported = (dwMyRes & dwRes) != 0;
	ControlEnable(hDlg, id, fResSupported);
	if (lpEditOptions->PhotoCDRes == id && !fResSupported)
		--lpEditOptions->PhotoCDRes;
	}
if (lpEditOptions->PhotoCDRes < IDC_PCD_BASE_OVER_16)
	return(TRUE);
CheckComboItem(hDlg, IDC_PCDRESOLUTION, IDC_PCD_BASE_OVER_16,
		IDC_PCD_64BASE, lpEditOptions->PhotoCDRes);

CheckComboItem(hDlg, IDC_PCDFORMAT, IDC_PCDSINGLE, IDC_PCDRGB,
		Control.fPCDUseCMS ? IDC_PCDRGB : lpEditOptions->PhotoCDFormat);
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
							PictPubApp.Get_hInstAstral(),
							MAKEINTRESOURCE(PCDOptions_iTabTable[i]),
							PCDOptions_iTabTable[i],
					    	DSCF_DISCARDABLE,
							NULL);
}

PCDOptions_Update(hDlg);

#endif

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
		InitDlgItemSpin( hDlg, IDC_PCD_BRIGHTBUTTON, lpEditOptions->BrightButton, YES, INT_MIN, INT_MAX );
		InitDlgItemSpin( hDlg, IDC_PCD_REDBUTTON, lpEditOptions->RedButton, YES, INT_MIN, INT_MAX );
		InitDlgItemSpin( hDlg, IDC_PCD_GREENBUTTON, lpEditOptions->GreenButton, YES, INT_MIN, INT_MAX );
		InitDlgItemSpin( hDlg, IDC_PCD_BLUEBUTTON, lpEditOptions->BlueButton, YES, INT_MIN, INT_MAX );
		InitDlgItemSpin( hDlg, IDC_PCD_SATBUTTON, lpEditOptions->SatButton, YES, INT_MIN, INT_MAX );
		CheckDlgButton(hDlg, IDC_PCD_SUBTRACTSBA, lpEditOptions->SubtractSBA); 
		CheckDlgButton(hDlg, IDC_PCD_OUTOFGAMUT, lpEditOptions->OutOfGamut); 
		InitExtNameN( hDlg, IDC_EXTNAMES, IDC_FILENAME,
				lpEditOptions->LutName, IDN_PCDLUT, IDS_NONE, NO);

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
		AstralDlgParam(NO, PictPubApp.Get_hInstAstral(), hDlg, IDD_CMS_SRCDST,
			DlgCmsPhotoCDOpenProc, (LPARAM)&pcdCmsInfo);
		lpEditOptions->cmsInfo = pcdCmsInfo;
		CmsPhotoCD_OnInitDialog(hDlg);
		CmsPhotoCDInit( hDlg );
	break;

	case IDC_EXTNAMES:
	if ( codeNotify != CBN_SELCHANGE )
		break;
	GetExtNameN( hDlg, id, IDC_FILENAME, lpEditOptions->LutName,
		IDN_PCDLUT, codeNotify, IDS_NONE, NO );
	break;

	case IDC_EXTMANAGE:
		PopupMenuEx( hDlg, id, IDC_EXTMANAGE, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			lpEditOptions->LutName, IDN_PCDLUT, id, IDS_NONE );
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
	lpEditOptions->PhotoCDFormat = id;
	GetPCDImageSize(&iImageWidth, &iImageHeight, &iImageDepth);
	ImageMemory( hDlg, IDC_PCD_IMAGESIZE, FGET(iImageWidth, 1 ),
		FGET( iImageHeight, 1 ), 1/*Resolution*/, iImageDepth );
	UpdateMemoryReadout( hDlg, IDC_PCD_CROPSIZE, &rGridRect );
	break;
	
	case IDC_PCDRESOLUTION:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	lpEditOptions->PhotoCDRes = id;
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

		CReadImage read(Names.Saved, IDN_PHOTOCD);

		PhotoCDRes = lpEditOptions->PhotoCDRes;
		lpEditOptions->PhotoCDRes = IDC_PCD_BASE_OVER_16;
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
int id, res;
PCDresolution pcdRes;
BOOL fResSupported;

CenterPopup( hDlg );

lpEditOptions = (LPPCDOPTIONS)lParam;

#ifndef WIN32

if (PCDopen(Names.Saved, &hPcd))
	return(TRUE);
status = PCDgetCount(hPcd, &dwRes, &wResCount);
PCDclose(hPcd);
if (status)
	return(TRUE);

for (id = IDC_PCD_64BASE; id >= IDC_PCD_BASE_OVER_16; --id)
	{
	dwMyRes = 1 << (id-IDC_PCD_BASE_OVER_16+1);
	fResSupported = (dwMyRes & dwRes) != 0;
	ControlEnable(hDlg, id, fResSupported);
	}
CheckComboItem(hDlg, IDC_PCDRESOLUTION, IDC_PCD_BASE_OVER_16,
		IDC_PCD_64BASE, lpEditOptions->PhotoCDRes);

GetPCDImageSize(&iImageWidth, &iImageHeight, &iImageDepth);
GetPCDResolution(lpEditOptions->PhotoCDRes, &pcdRes, &res);

rGridRect.left = FMUL(res, lpEditOptions->rArea.left);
rGridRect.top = FMUL(res, lpEditOptions->rArea.top);
rGridRect.right = FMUL(res, lpEditOptions->rArea.right);
rGridRect.bottom = FMUL(res, lpEditOptions->rArea.bottom);
UpdateMemoryReadout( hDlg, IDC_PCD_CROPSIZE, &rGridRect );

#endif

return(TRUE);
}

/************************************************************************/
LOCAL void PCDResolution_OnCommand(HWND hDlg, int id, HWND hCtlWnd, UINT codeNotify)
/************************************************************************/
{
PCDresolution pcdRes;
int res;
FRECT rArea;

switch (id)
	{
	case IDC_PCDRESOLUTION:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	lpEditOptions->PhotoCDRes = id;
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
lpOptions->PhotoCDRes = GetDefaultInt(PhotoCDRes_Str, IDC_PCD_BASE-IDC_PCD_BASE_OVER_16);
lpOptions->PhotoCDRes += IDC_PCD_BASE_OVER_16;

lpOptions->PhotoCDFormat	= GetDefaultInt(PhotoCDFormat_Str, IDC_PCDRGB-IDC_PCDSINGLE);lpOptions->PhotoCDFormat += IDC_PCDSINGLE;
lpOptions->BrightButton 	= GetDefaultInt(BrightButton_Str, 0);
lpOptions->RedButton 		= GetDefaultInt(RedButton_Str, 0);
lpOptions->GreenButton 		= GetDefaultInt(GreenButton_Str, 0);
lpOptions->BlueButton 	 	= GetDefaultInt(BlueButton_Str, 0);
lpOptions->SatButton  		= GetDefaultInt(SatButton_Str, 0);
lpOptions->OutOfGamut  		= GetDefaultInt(OutOfGamut_Str, FALSE);
lpOptions->SubtractSBA 		= GetDefaultInt(SubtractSBA_Str, FALSE);
GetDefaultString(PCDTableName_Str, "", lpOptions->LutName, sizeof(lpOptions->LutName));
lpOptions->fHaveArea = NO;
}

/***********************************************************************/
static void StorePCDPreferences(LPPCDOPTIONS lpOptions)
/***********************************************************************/
{
PutDefaultInt(PhotoCDRes_Str, lpOptions->PhotoCDRes - IDC_PCD_BASE_OVER_16);
PutDefaultInt(PhotoCDFormat_Str, lpOptions->PhotoCDFormat - IDC_PCDSINGLE);
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
LOCAL HANDLE LoadPCDLut(LPSTR lpLutName, LPINT lpEntries)
/***********************************************************************/
{
	HANDLE hLut;
	LPSTR lp, lpBuf, lpEnd;
	LPTR lpLut;
	FNAME szFileName;
	long lSize, lRead;
	FILE_HANDLE fh;
	int i, nEntries;

	if ( !LookupExtFileN( lpLutName, szFileName, IDN_PCDLUT, NO ) )
	{
		if ( AstralStr( IDS_NONE, &lp ) )
			lstrcpy( lpLutName, lp );
	}

	if ( StringsEqualID( lpLutName, IDS_NONE ) )
		return(NULL);

	fh = FileOpen(szFileName, FO_READ);
	if (fh == FILE_HANDLE_INVALID)
	{
		Message(IDS_EOPEN, (LPSTR)szFileName);
		return(NULL);
	}

	hLut = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, 512);
	if (!hLut)
	{
		Message(IDS_EMEMALLOC);
		FileClose(fh);
		return(NULL);
	}

	lpLut = (LPTR)GlobalLock(hLut);
	if (!lpLut)
	{
		Message(IDS_EMEMALLOC);
		GlobalFree(hLut);
		hLut = NULL;
		FileClose(fh);
		return(NULL);
	}

	lSize = FileSeek(fh, 0L, SEEK_END);
	lpBuf = (LPSTR)Alloc(lSize);
	if (!lpBuf)
	{
		Message(IDS_EMEMALLOC);
		GlobalUnlock(hLut);
		GlobalFree(hLut);
		hLut = NULL;
		FileClose(fh);
		return(NULL);
	}
	// back to beginning
	FileSeek(fh, 0L, SEEK_SET);
	lRead = FileRead(fh, lpBuf, lSize);
	FileClose(fh);
	if (lRead != lSize)
	{
		Message(IDS_EREAD, (LPSTR)szFileName);
		FreeUp(lpBuf);
		GlobalUnlock(hLut);
		GlobalFree(hLut);
		hLut = NULL;
		FileClose(fh);
		return(NULL);
	}
	if (StringsEqualN(extension(szFileName), ".bin", 4))
	{
		if (lSize > 512)
			lSize = 512;
		copy((LPTR)lpBuf, (LPTR)lpLut, (int)lSize);
		nEntries = (int)lSize;
	}
	else
	{
		lpEnd = lpBuf + lSize;
		for (i = 0; i < 512; ++i)
		{
			lpBuf = FindNumber(lpBuf, lpEnd, &lpLut[i]);
			if (!lpBuf)
				break;
		}
		nEntries = i;
	}
	FreeUp(lpBuf);
	GlobalUnlock(hLut);

	*lpEntries = nEntries;
	return(hLut);
}

/***********************************************************************/
LOCAL LPSTR FindNumber(LPSTR lp, LPSTR lpEnd, LPBYTE lpValue)
/***********************************************************************/
{
	char num[10];
	int i;

	while (lp < lpEnd)
	{
		if (isdigit(*lp))
		{	
			i = 0;
			while (isdigit(*lp) && (lp < lpEnd))
			{
				num[i] = *lp++;
				++i;
			}
			num[i] = '\0';
			*lpValue = Ascii2Int( num );
			if (lp < lpEnd)
				return(lp);
			else
				return(NULL);
		}
		++lp;
	}
	return(NULL);
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

if (lpEditOptions->PhotoCDRes == IDC_PCD_BASE_OVER_16)
	{
	*lpWidth = 192;
	*lpHeight = 128;
	}
else if (lpEditOptions->PhotoCDRes == IDC_PCD_BASE_OVER_4)
	{
	*lpWidth = 384;
	*lpHeight = 256;
	}
else if (lpEditOptions->PhotoCDRes == IDC_PCD_4BASE)
	{
	*lpWidth = 1536;
	*lpHeight = 1024;
	}
else if (lpEditOptions->PhotoCDRes == IDC_PCD_16BASE)
	{
	*lpWidth = 3072;
	*lpHeight = 2048;
	}
else if (lpEditOptions->PhotoCDRes == IDC_PCD_64BASE)
	{
	*lpWidth = 6144;
	*lpHeight = 4096;
	}
else //if (lpEditOptions->PhotoCDRes == IDC_PCD_BASE)
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
if (lpEditOptions->PhotoCDFormat == IDC_PCDSINGLE)
	*lpDepth = 1;
else
	*lpDepth = 3;
}

/************************************************************************/
LOCAL void GetPCDResolution(int PhotoCDRes, PCDresolution *pcdRes, LPINT lpRes)
/************************************************************************/
{
	if (PhotoCDRes == IDC_PCD_BASE_OVER_16)
 	{
		*pcdRes = PCD_BASE_OVER_16;
		*lpRes = 25;
 	}
	else if (PhotoCDRes == IDC_PCD_BASE_OVER_4)
 	{
		*pcdRes = PCD_BASE_OVER_4;
		*lpRes = 50;
 	}
	else if (PhotoCDRes == IDC_PCD_4BASE)
 	{
		*pcdRes = PCD_4BASE;
		*lpRes = 200;
 	}
	else if (PhotoCDRes == IDC_PCD_16BASE)
 	{
		*pcdRes = PCD_16BASE;
		*lpRes = 400;
 	}
	else if (PhotoCDRes == IDC_PCD_64BASE)
 	{
		*pcdRes = PCD_64BASE;
		*lpRes = 800;
 	}
	else //if (PhotoCDRes == IDC_PCD_BASE)
 	{
		*pcdRes = PCD_BASE;
		*lpRes = 100;
 	}
}





