//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "cpo.h"

typedef struct _internal
{
	LPVOID lpCPO;
} INTERNAL_DATA, FAR *LPINTERNAL_DATA;

LOCAL LPFRAME ConvertFrameOld(LPFRAME lpSrcFrame, int iFunction, LPINT lpType);
LOCAL LPFRAME ConvertFrame(LPFRAME lpSrcFrame, FRMTYPEINFO OutTypeInfo, CFrameTypeConvert *pTypeConvert);

/***********************************************************************/
BOOL ConvertTo(LPIMAGE lpImage, int iFunction)
/***********************************************************************/
{
CONVERT_PARMS parms;
DITHER_TYPE DitherType = DT_NONE;

if (ImgInMaskEditMode(lpImage))
	return(FALSE);
ImgGetTypeInfo(lpImage, &parms.cmsInfo.src);
switch (iFunction)
	{
	case IDM_CONVLA:
		FrameSetTypeInfo(&parms.cmsInfo.dst, FDT_LINEART);
		break;
	case IDM_CONVSP:
		FrameSetTypeInfo(&parms.cmsInfo.dst, FDT_LINEART);
		DitherType = DT_SCATTER;
		break;
	case IDM_CONVCT:
		FrameSetTypeInfo(&parms.cmsInfo.dst, FDT_GRAYSCALE);
		break;
	case IDM_CONVPALETTE:
		FrameSetTypeInfo(&parms.cmsInfo.dst, FDT_PALETTECOLOR);
		if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_CONVERTTO, DlgConvertProc ))
			return( NO );
		DitherType = Convert.ConvertDitherType;
		break;
	case IDM_CONVRGBCOLOR:
		if (Control.CMSEnabled && IsSrcPTSelected(&parms.cmsInfo.src.ptInfo))
		{
			CMSINFO	CmsInfo;

			CmsInfo.src.DataType = FDT_CMYKCOLOR;

			if (!CmsGetDstPT(&CmsInfo))
				return(FALSE);
			else
				parms.cmsInfo.dst = CmsInfo.dst;		
		}
		else
		{
			FrameSetTypeInfo(&parms.cmsInfo.dst, FDT_RGBCOLOR);
		}
		break;
	case IDM_CONVCMYKCOLOR:
		if (Control.CMSEnabled && IsSrcPTSelected(&parms.cmsInfo.src.ptInfo))
		{
			CMSINFO	CmsInfo;

			CmsInfo.src.DataType = FDT_RGBCOLOR;

			if (!CmsGetDstPT(&CmsInfo))
				return(FALSE);
			else
				parms.cmsInfo.dst = CmsInfo.dst;		
		}
		else
		{
			FrameSetTypeInfo(&parms.cmsInfo.dst, FDT_CMYKCOLOR);
		}
		break;
	case IDM_COLORMANAGED:
		if (!CmsGetSrcDstPT(&parms.cmsInfo))
			return(TRUE);
		break;
	}
parms.DitherType = DitherType;
parms.ConvertType = Convert.ConvertType;
parms.iLevels = Convert.ConvertColors;
lstrcpy(parms.szPalette, Convert.ConvertPalette);
ProcessCommand(lpImage->lpCmdList, IDS_CMD_CONVERT, &parms);
return(TRUE);
}

/***********************************************************************/
BOOL ConvertImage(LPIMAGE lpImage, LPCONVERT_PARMS lpParms)
/***********************************************************************/
{
BOOL fRet = FALSE;
LPFRAME lpDstFrame, lpBaseFrame;
LPOBJECT lpBase, lpObject;
BOOL fError, fOptimize, fProgressSet;
INTERNAL_DATA data;
int	inDepth, iMaxWidth, nColors, i;
LPCOLORMAP lpColorMap;
LPRGB lpRGB;
FRMTYPEINFO InTypeInfo, OutTypeInfo;
CFrameTypeConvert TypeConvert;
DITHER_TYPE DitherType = DT_DEFAULT;

if (ImgInMaskEditMode(lpImage))
	return(TRUE);

lpColorMap = NULL;

lpBase = ImgGetBase(lpImage);
lpBaseFrame = ObjGetEditFrame(lpBase);
inDepth = FrameDepth(lpBaseFrame);

OutTypeInfo = lpParms->cmsInfo.dst;
FrameGetTypeInfo(lpBaseFrame, &InTypeInfo);

// are we try to convert to a color managed image?
if (Control.CMSEnabled && IsDstPTSelected(&OutTypeInfo.ptInfo))
	{
	// see if our image already has a color managed source
	if (!IsSrcPTSelected(&InTypeInfo.ptInfo))
		{
		// nope, see if the source passed in is valid for this image
		if (IsSrcPTSelected(&lpParms->cmsInfo.src.ptInfo) &&
			(lpParms->cmsInfo.src.DataType == InTypeInfo.DataType))
			{	
			InTypeInfo = lpParms->cmsInfo.src;
			}
		// go get a source from the user
		else
			{
			CMSINFO CmsInfo2;
			if( CmsGetSrcPTType( &InTypeInfo, &CmsInfo2) )
				InTypeInfo = CmsInfo2.src;
			}
		}
	}
else
 	FrameSetTypeInfo(&InTypeInfo, InTypeInfo.DataType, InTypeInfo.ColorMap);

// no conversion necessary
if (FrameTypeInfoEqual(InTypeInfo, OutTypeInfo))
	return(TRUE);

DitherType = lpParms->DitherType;
fOptimize = lpParms->ConvertType == CT_OPTIMIZED;
fProgressSet = FALSE;


if ( OutTypeInfo.DataType == FDT_PALETTECOLOR/* && inDepth >= 3 */)
	{
	if (lpParms->ConvertType == CT_CUSTOM)
		{
		FNAME szFileName;

		if ( !LookupExtFileN( lpParms->szPalette, szFileName, IDN_PALETTE, NO ) )
			return(FALSE);
		if (!(lpColorMap = Palette_ReadColorMap(szFileName)))
			return(FALSE);
		}
	else
		{
		// allocate a map to carry around with image
		lpColorMap = FrameCreateColorMap();
		if (!lpColorMap)
			{
			Message(IDS_EMEMALLOC);
			goto MemError;
			}

		lpColorMap->NumEntries = lpParms->iLevels;

		if (InTypeInfo.DataType == FDT_GRAYSCALE && lpColorMap->NumEntries == 256 &&
			fOptimize)
			{
			DitherType = DT_NONE;
			lpRGB = lpColorMap->RGBData;
			for (i = 0; i < lpColorMap->NumEntries; ++i)
				{
				lpRGB->red = lpRGB->green = lpRGB->blue = i;
				++lpRGB;
				}
			}
		else
			{
			// 1. CreateOptimizedPalette phase - only if fOptimize
			// 2. Mapping phase
			ProgressBegin(fOptimize ? 2 : 1,
					lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
			fProgressSet = TRUE;
			if (fOptimize)
				ProgressBegin(1, 0);
			if (!CreateOptimizedPalette(lpBase, ObjGetEditFrame(lpBase),
									lpColorMap->RGBData, &lpColorMap->NumEntries,
									fOptimize,
									fOptimize ? AstralClockCursor : NULL))
				{
				if (fOptimize)
					ProgressEnd();
				Message(IDS_EMEMALLOC);
				goto ExitFalse;
				}
			if (fOptimize)
				ProgressEnd();
			}
		}
	if (lpColorMap->NumEntries <= 16)
		lpColorMap->NumEntries = 16;
	else
		lpColorMap->NumEntries = 256;
	FrameSetTypeInfo(&OutTypeInfo, FDT_PALETTECOLOR, lpColorMap);
	}

if (!fProgressSet)
	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
fProgressSet = TRUE;
	
iMaxWidth = 0;
lpObject = NULL;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (RectWidth(&lpObject->rObject) > iMaxWidth)
		iMaxWidth = RectWidth(&lpObject->rObject);
	}
if (!TypeConvert.Init(InTypeInfo, OutTypeInfo, iMaxWidth, DitherType))
	{
	Message(IDS_EMEMALLOC);
	goto ExitFalse;
	}

if (ImgMultipleObjects(lpImage))
	{
	ProgressBegin(ImgCountObjects(lpImage), 0);
	
	if ( !ImgEditInit(lpImage, ET_ALLOBJECTS, UT_NEWDATA|UT_COLORMAP, lpBase) )
		goto ExitFalse;

	fError = NO;
	lpObject = NULL;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
		{
		lpDstFrame = ConvertFrame(ObjGetEditFrame(lpObject), OutTypeInfo, &TypeConvert);
		if (!lpDstFrame)
			{
			fError = YES;
			break;
			}
 		ImgEditedObjectFrame( lpImage, lpObject, lpParms->Common.idDirty, NULL,
						lpDstFrame, NULL);
		}

	if (fError)
		{
		lpObject = NULL;
		while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
			{
			if (lpObject->Pixmap.UndoFrame)
				{
		 		lpDstFrame = lpObject->Pixmap.EditFrame;
		 		lpObject->Pixmap.EditFrame = lpObject->Pixmap.UndoFrame;
		 		lpObject->Pixmap.UndoFrame = NULL;
		 		FrameClose(lpDstFrame);
		 		}
			}
		}
	ProgressEnd();
	}
else
	{
	if ( !ImgEditInit(lpImage, ET_OBJECT, UT_NEWDATA|UT_COLORMAP, lpBase) )
		goto ExitFalse;

	ProgressBegin(1, 0);
	lpDstFrame = ConvertFrame(lpBaseFrame, OutTypeInfo, &TypeConvert);
	ProgressEnd();

	if (!lpDstFrame)
		goto ExitFalse;
	/* Setup the new image and bring up the new image window */
	ImgEditedObjectFrame( lpImage, lpBase, lpParms->Common.idDirty, NULL,
						lpDstFrame, NULL);
	}

switch (lpParms->cmsInfo.dst.DataType)
	{
	case FDT_LINEART:
		lpImage->DataType = IDC_SAVELA;
		break;
	case FDT_GRAYSCALE:
		lpImage->DataType = IDC_SAVECT;
		break;
	case FDT_PALETTECOLOR:
		lpImage->DataType = IDC_SAVE8BITCOLOR;
		break;
	case FDT_RGBCOLOR:
		lpImage->DataType = IDC_SAVE24BITCOLOR;
		break;
	case FDT_CMYKCOLOR:
		lpImage->DataType = IDC_SAVE32BITCOLOR;
		break;
	}
ImgColorMapChanged(lpImage);
lpParms->Common.UpdateType = UT_DATATYPE;

fRet = TRUE;

ExitFalse:
if (fProgressSet)
	ProgressEnd();
MemError:
if (lpColorMap)
	FrameDestroyColorMap(lpColorMap);
return(fRet);
}

/***********************************************************************/
LOCAL LPFRAME ConvertFrame(LPFRAME lpSrcFrame, FRMTYPEINFO OutTypeInfo, CFrameTypeConvert *pTypeConvert)
/***********************************************************************/
{
int	y, Xsize, Ysize;
LPFRAME lpDstFrame;
FRMTYPEINFO InTypeInfo;
LPTR lpSrc, lpDst;
CFrameTypeConvert TypeConvert;

Xsize = FrameXSize(lpSrcFrame);
Ysize = FrameYSize(lpSrcFrame);
FrameGetTypeInfo(lpSrcFrame, &InTypeInfo);
if (!pTypeConvert)
	{
	if (!TypeConvert.Init(InTypeInfo, OutTypeInfo, Xsize))
		{
		Message(IDS_EMEMALLOC);
		return(NULL);
		}
	pTypeConvert = &TypeConvert;
	}

lpDstFrame = FrameOpen(OutTypeInfo, Xsize, Ysize, FrameResolution(lpSrcFrame));
if (!lpDstFrame)
	{
	FrameError(IDS_EPROCESSOPEN);
	return(NULL);
	}

ProgressBegin(1, 0);
for (y = 0; y < Ysize; ++y)
	{
	if (AstralClockCursor(y, Ysize, YES))
		{
		FrameClose(lpDstFrame);
		goto ExitError;
		}
	if (!(lpSrc = FramePointerRaw(lpSrcFrame, 0, y, NO)))
		continue;
	if (!(lpDst = FramePointerRaw(lpDstFrame, 0, y, YES)))
		continue;
	pTypeConvert->ConvertData(lpSrc, lpDst, y, Xsize);
	}

ProgressEnd();
return(lpDstFrame);

ExitError:
ProgressEnd();
return(NULL);
}

// LOCAL prototypes
LOCAL BOOL Convert_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Convert_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Convert_Enable(HWND hDlg);

/***********************************************************************/
BOOL WINPROC EXPORT DlgConvertProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Convert_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Convert_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CONVERTTO);
	}
}

/************************************************************************/
LOCAL BOOL Convert_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
ITEMID id;

CenterPopup( hDlg );
CheckComboItem( hDlg, IDC_MINITYPES, IDC_MINI256, IDC_MINICUSTOM,
	Convert.MiniType );
if (Convert.MiniType == IDC_MINI256)
	Convert.ConvertColors = 256;
else
if (Convert.MiniType == IDC_MINI16)
	Convert.ConvertColors = 16;
else
if (Convert.MiniType == IDC_MINI8)
	Convert.ConvertColors = 8;

InitDlgItemSpin( hDlg, IDC_PALETTECOLORS, Convert.ConvertColors, FALSE,
                 1, 256 );

if ( Convert.ConvertDitherType == DT_PATTERN )
	id = IDC_DITHERPATTERN;
else
if ( Convert.ConvertDitherType == DT_SCATTER )
	id = IDC_DITHERSCATTERED;
else
	id = IDC_DITHERNONE;
CheckComboItem( hDlg, IDC_DITHERTYPES, IDC_DITHERNONE,
	IDC_DITHERSCATTERED, id );
if ( Convert.ConvertType == CT_OPTIMIZED)
	id = IDC_PALETTEOPTIMIZED;
else
if ( Convert.ConvertType == CT_STANDARD)
	id = IDC_PALETTESTANDARD;
else
	id = IDC_PALETTECUSTOM;
CheckComboItem( hDlg, IDC_PALETTETYPES, IDC_PALETTEOPTIMIZED,
	IDC_PALETTECUSTOM, id );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Convert.ConvertPalette, IDN_PALETTE );

Convert_Enable(hDlg);

return(TRUE);
}

/************************************************************************/
LOCAL void Convert_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;

switch (id)
	{
	case IDC_MINITYPES:
// case IDC_MINI256:
// case IDC_MINI16:
// case IDC_MINI8:
// case IDC_MINICUSTOM:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Convert.MiniType = id;

	if (Convert.MiniType == IDC_MINI256)
		Convert.ConvertColors = 256;
	else
	if (Convert.MiniType == IDC_MINI16)
		Convert.ConvertColors = 16;
	else
	if (Convert.MiniType == IDC_MINI8)
		Convert.ConvertColors = 8;
	Convert_Enable(hDlg);
	break;

	case IDC_DITHERTYPES:
//	case IDC_DITHERPATTERN:
//	case IDC_DITHERSCATTERED:
//	case IDC_DITHERNONE:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	if ( id == IDC_DITHERPATTERN )
		{
		Convert.fDither = YES;
		Convert.fScatter = NO;
		Convert.ConvertDitherType = DT_PATTERN;
		}
	else
	if ( id == IDC_DITHERSCATTERED )
		{
		Convert.fDither = NO;
		Convert.fScatter = YES;
		Convert.ConvertDitherType = DT_SCATTER;
		}
	else
		{
		Convert.fDither = NO;
		Convert.fScatter = NO;
		Convert.ConvertDitherType = DT_NONE;
		}
	break;

	case IDC_PALETTETYPES:
//	case IDC_PALETTEOPTIMIZED:
//	case IDC_PALETTESTANDARD:
//	case IDC_PALETTECUSTOM:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	if (id == IDC_PALETTEOPTIMIZED)
		Convert.ConvertType = CT_OPTIMIZED;
	else
	if (id == IDC_PALETTESTANDARD)
		Convert.ConvertType = CT_STANDARD;
	else
		Convert.ConvertType = CT_CUSTOM;
	Convert.fOptimize = ( id == IDC_PALETTEOPTIMIZED );
	Convert_Enable(hDlg);
	break;

	case IDC_PALETTECOLORS:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Convert.ConvertColors = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Convert.ConvertPalette, IDN_PALETTE, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Convert.ConvertPalette,
		IDN_PALETTE, id, NO );
	break;

    case IDOK:
	// make sure we can open a colormap
	if (Convert.ConvertType == CT_CUSTOM)
		{
		FNAME szFileName;
		LPCOLORMAP lpColorMap;

		if ( !LookupExtFile( Convert.ConvertPalette, szFileName, IDN_PALETTE ) )
			break;
		if (!(lpColorMap = Palette_ReadColorMap(szFileName)))
			break;
		FrameDestroyColorMap(lpColorMap);
		}

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
LOCAL void Convert_Enable(HWND hDlg)
/************************************************************************/
{
ControlEnable( hDlg, IDC_EXTNAMES, Convert.ConvertType == CT_CUSTOM);
ControlEnable( hDlg, IDC_PALETTECOLORS, Convert.ConvertType != CT_CUSTOM &&
										Convert.MiniType == IDC_MINICUSTOM);
ControlEnable( hDlg, IDC_MINITYPES, Convert.ConvertType != CT_CUSTOM);
}




