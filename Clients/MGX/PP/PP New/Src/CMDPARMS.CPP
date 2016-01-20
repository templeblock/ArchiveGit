//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include <process.h>
#include "cmdparms.h"
#include "ppafx.h"
#include "id.h"

extern LPTR PS_NewHandle(DWORD dwSize);
extern void PS_DisposHandle(LPTR lp);
extern DWORD PS_GetHandleSize(LPTR lp);

LOCAL WRITE_FUNC MapWriteInfo(LPMAPINFO lpInfo, LPSTR lpString, MACRO_FILE_HANDLE fh);
LOCAL BOOL RetouchWriteInfo(HPSTROKEINFO hpStrokeInfo, int nPoints,
						  	 BOOL fUsePressure, BOOL fWriteSource,
							 LPSTR lpString, MACRO_FILE_HANDLE fh);

LOCAL BOOL MapReadInfo(LPMAPINFO lpInfo, MACRO_FILE_HANDLE fh);
LOCAL LPRETOUCH_PARMS RetouchGetCommonParms(LPSTR lpCommand, MACRO_FILE_HANDLE fh, BOOL fReadSource, LPINT lpNextIndex);
LOCAL LPRETOUCH_PARMS RetouchReadParms(LPCMDLIST lpCmdList, LPSTR lpCommand, MACRO_FILE_HANDLE fh, BOOL fReadSource);
LOCAL HPSTROKEINFO ReadStrokeInfo(MACRO_FILE_HANDLE fh, int nPoints, BOOL fReadPressure, BOOL fReadSource);
LOCAL BOOL IsEOL(char c);
LOCAL LPDRAWFREE_PARMS DrawFreeReadParms(LPSTR lpCommand, MACRO_FILE_HANDLE fh);
LOCAL void GetDrawBrushParms(LPSTR lpCommand, LPDRAWBRUSH lpBrush, int index);
LOCAL LPFPOINT ReadFixedPointList(MACRO_FILE_HANDLE fh, int nPoints);
LOCAL BOOL WriteFixedPointList(MACRO_FILE_HANDLE fh, LPFPOINT lpPoints, int nPoints);
LOCAL LPPOINT ReadIntPointList(MACRO_FILE_HANDLE fh, int nPoints);
LOCAL BOOL WriteIntPointList(MACRO_FILE_HANDLE fh, LPPOINT lpPoints, int nPoints);
LOCAL BOOL ReadColorMap(MACRO_FILE_HANDLE fh, LPCOLORMAP lpColorMap);
LOCAL BOOL WriteColorMap(MACRO_FILE_HANDLE fh, LPCOLORMAP lpColorMap);
LOCAL LPSTR MergeModeToString(MERGE_MODE MergeMode, LPSTR lpString);
LOCAL MERGE_MODE StringToMergeMode(LPSTR lpString);
LOCAL BOOL GetOpenOptions(LPSTR lpCommand, LPPVOID lpOptions, ITEMID idFileType, int index);
LOCAL void GetOpenOptionsString(LPVOID lpOptions, ITEMID idFileType, LPSTR lpString);
LOCAL int GetCMSInfo(LPSTR lpCommand, LPCMSINFO lpCmsInfo, int index);

#define MM_STR_LEN 20
#define LFIXED_STR_LEN	20
/***********************************************************************/
LPSTR ReadLine(MACRO_FILE_HANDLE fh, LPSTR lpBuf, DWORD dwMaxSize)
/***********************************************************************/
{
char c;
DWORD dwBytes;
LPSTR lp;
BOOL fEOF;

fEOF = NO;
c = '\0';
lp = lpBuf;
dwBytes = 0;
--dwMaxSize; // leave room for null termination
while (dwBytes < dwMaxSize)
	{
#ifdef BUFFERED_IO
	if (fread(&c, 1, 1, fh) != 1)
#else
	if (FileRead(fh, &c, 1) != 1)
#endif
		{
		fEOF = YES;
		break;
		}
	// get rid of leading white space
	if (!dwBytes && (c == ' ' || c == '\t'))
		continue;
	if (c == '\r') // ignore carriage returns
		continue;
	if (c == '\n') // stop when new line hit
		break;
	*lp++ = c;
	++dwBytes;
	}

*lp = '\0';
if (dwBytes || !fEOF)
	return(lpBuf);
else
	return(NULL);
}

/***********************************************************************/
BOOL WriteString(MACRO_FILE_HANDLE fh, LPSTR lpString)
/***********************************************************************/
{
DWORD dwBytes;
char eol[10];

dwBytes = lstrlen(lpString);
#ifdef BUFFERED_IO
if (fwrite(lpString, 1, dwBytes, fh) != dwBytes)
#else
if (FileWrite(fh, lpString, dwBytes) != dwBytes)
#endif
	return(FALSE);
lstrcpy(eol, END_OF_LINE);
dwBytes = lstrlen(eol);
#ifdef BUFFERED_IO
if (fwrite(eol, 1, dwBytes, fh) != dwBytes)
#else
if (FileWrite(fh, eol, dwBytes) != dwBytes)
#endif
	return(FALSE);

return(TRUE);
}

/***********************************************************************/
/*																		*/
/*	GetParms functions:													*/
/*		Use to read macro commands from a macro file.  Also, used to 	*/
/*		allow functions to set parameters.  See lpCmdParms.				*/
/*																		*/
/* 	lpCmdList -	base command list pointer.  This is used primarily  */
/*					by load functions to copy their image name to the   */
/*					ThreadData structure.  It is there to provide       */
/*					additional information to the GetParms functions    */
/*					could of course be used for other purposes.			*/
/*																		*/
/*		lpCommand - buffer containing the command string.  If this 		*/
/*					parameter is NULL, lpCmdParms should be used.		*/
/*																		*/
/*		lpCmdParms - pointer to a parameter structure.  If this			*/
/*					parameter is NULL, lpCommand should be used.		*/
/*					This is the normal way commands 					*/
/*					are processed internally in the program.  The 		*/
/*					purpose of this is to allow parameters that don't	*/
/*					change to be set.  For example, Rotate90GetParms	*/
/*					sets the angle to 90 so that one function can be	*/
/*					called to handle all the rotate commands.			*/
/*																		*/
/*		fh -		This is the file handle for reading from a macro	*/
/*					file.  If a macro has multiple lines of data,		*/
/*					lpCommand only contains the first line and it is	*/
/*					the responsiblity of the GetParms function to read	*/
/*					the other lines by calling ReadLine().				*/
/*																		*/
/***********************************************************************/

/***********************************************************************/
LPVOID FastBitsReadParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPFASTBITS_PARMS lpParms;
int index;

if (lpCmdParms)
	{
	lpParms = (LPFASTBITS_PARMS)lpCmdParms;
	}
else
	{
	if (!(lpParms = (LPFASTBITS_PARMS)Alloc((long)sizeof(FASTBITS_PARMS))))
		return(NULL);
	index = 1;
	GetStringParm(lpCommand, index++, lpParms->szFileName);
	GetStringParm(lpCommand, index++, lpParms->szVolumeID);

	lpParms->Rect.left   = GetIntParm(lpCommand, index++);
	lpParms->Rect.top    = GetIntParm(lpCommand, index++);
	lpParms->Rect.right  = GetIntParm(lpCommand, index++);
	lpParms->Rect.bottom = GetIntParm(lpCommand, index++);

	index = GetCMSInfo(lpCommand, &lpParms->cmsInfo, index);

	}
lpParms->lpCmdList = lpCmdList;
#ifdef WIN32
	if (Control.UseThreading && lpCmdList)
		lstrcpy(lpCmdList->ThreadData.ThreadInfo.szThreadName,
				filename(lpParms->szFileName));
#endif
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID LoadFileGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPLOADFILE_PARMS lpParms;
int index;

if (lpCmdParms)
	{
	lpParms = (LPLOADFILE_PARMS)lpCmdParms;
	}
else
	{
	if (!(lpParms = (LPLOADFILE_PARMS)Alloc((long)sizeof(LOADFILE_PARMS))))
		return(NULL);
	index = 1;
	GetStringParm(lpCommand, index++, lpParms->szFileName);
	GetStringParm(lpCommand, index++, lpParms->szVolumeID);
	lpParms->idFileType = GetIntParm(lpCommand, index++);
	if (lpParms->idFileType)
		lpParms->idFileType += IDN_FIRSTFILETYPE - 1;

	index = GetCMSInfo(lpCommand, &lpParms->cmsInfo, index);

	if (!GetOpenOptions(lpCommand, &lpParms->lpOptions, lpParms->idFileType, index))
		{
		FreeUp(lpParms);
		return(NULL);
		}
	}
lpParms->lpCmdList = lpCmdList;
#ifdef WIN32
	if (Control.UseThreading && lpCmdList)
		lstrcpy(lpCmdList->ThreadData.ThreadInfo.szThreadName,
				filename(lpParms->szFileName));
#endif
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID LowResLoadGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPLOWRESLOAD_PARMS lpParms;
int index;

if (lpCmdParms)
	lpParms = (LPLOWRESLOAD_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPLOWRESLOAD_PARMS)Alloc((long)sizeof(LOWRESLOAD_PARMS))))
		return(NULL);
	index = 1;
	GetStringParm(lpCommand, index++, lpParms->szFileName);
	GetStringParm(lpCommand, index++, lpParms->szVolumeID);
	lpParms->idFileType = GetIntParm(lpCommand, index++);
	if (lpParms->idFileType)
		lpParms->idFileType += IDN_FIRSTFILETYPE - 1;
	lpParms->Resolution = GetIntParm(lpCommand, index++);

	index = GetCMSInfo(lpCommand, &lpParms->cmsInfo, index);

	if (!GetOpenOptions(lpCommand, &lpParms->lpOptions, lpParms->idFileType, index))
		{
		FreeUp(lpParms);
		return(NULL);
		}
	}
lpParms->lpCmdList = lpCmdList;
#ifdef WIN32
	if (Control.UseThreading && lpCmdList)
		lstrcpy(lpCmdList->ThreadData.ThreadInfo.szThreadName,
				filename(lpParms->szFileName));
#endif
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SaveFileGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSAVEFILE_PARMS lpParms;
int i, index;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPSAVEFILE_PARMS)Alloc((long)sizeof(SAVEFILE_PARMS))))
	return(NULL);

index = 1;

GetStringParm(lpCommand, index++, lpParms->szFileName);
lpParms->idFileType = GetIntParm(lpCommand, index++);
if (lpParms->idFileType)
	lpParms->idFileType += IDN_FIRSTFILETYPE - 1;
lpParms->idDataType = GetIntParm(lpCommand, index++);
if (lpParms->idDataType)
	lpParms->idDataType += IDC_FIRSTDATATYPE - 1;

index = GetCMSInfo(lpCommand, &lpParms->CmsInfo, index);

switch (lpParms->idFileType)
	{
	case IDN_PP4:
    case IDN_PPCOMPOUND:
	case IDN_PP:
		lpParms->PPFFOptions.PPFFCompress = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bSaveMask = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bSaveProxy = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bSaveCmdList = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bWritePT = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bSaveLink = GetIntParm(lpCommand, index++);
		break;
	case IDN_TIFF:
		lpParms->TIFFOptions.TIFFCompress = GetIntParm(lpCommand, index++);
		lpParms->TIFFOptions.TIFFDiff = GetIntParm(lpCommand, index++);
		lpParms->TIFFOptions.bWritePT = GetIntParm(lpCommand, index++);
		lpParms->TIFFOptions.bSaveMask = GetIntParm(lpCommand, index++);
		break;
	case IDN_TARGA:
		lpParms->TGAOptions.bSaveMask = GetIntParm(lpCommand, index++);
		break;
	case IDN_JPEG:
		lpParms->JPEGOptions.nCompressionFactor = GetIntParm(lpCommand, index++);
		lpParms->JPEGOptions.nSubsampling =	GetIntParm(lpCommand, index++);
		break;
	case IDN_DCS:
	case IDN_EPS:
		lpParms->EPSOptions.PreviewBPP = GetIntParm(lpCommand, index++);
		lpParms->EPSOptions.CompositeBPP = GetIntParm(lpCommand, index++);
		GetStringParm(lpCommand, index++, lpParms->EPSOptions.ClipPath);
		break;
	case IDN_AVI:
		lpParms->AVIOptions.iAVIPosType = GetIntParm(lpCommand, index++);
		lpParms->AVIOptions.iAVISizingType = GetIntParm(lpCommand, index++);
		lpParms->AVIOptions.AVIPosition = GetIntParm(lpCommand, index++);
		lpParms->AVIOptions.AVILastPosition = GetIntParm(lpCommand, index++);
		break;
	default:
		break;
	}

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID CmdGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPVOID lpParms;

if (lpCmdParms)
	return(lpCmdParms);

if (!(lpParms = (LPVOID)Alloc((long)sizeof(CMD_PARMS))))
	return(NULL);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID Rotate90GetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPROTATE_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPROTATE_PARMS)lpCmdParms;
else
if (!(lpParms = (LPROTATE_PARMS)Alloc((long)sizeof(ROTATE_PARMS))))
	return(NULL);
lpParms->Angle = TOFIXED(90);
lpParms->fUseWeightedAvg = NO;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID Rotate180GetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPROTATE_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPROTATE_PARMS)lpCmdParms;
else
if (!(lpParms = (LPROTATE_PARMS)Alloc((long)sizeof(ROTATE_PARMS))))
	return(NULL);
lpParms->Angle = TOFIXED(180);
lpParms->fUseWeightedAvg = NO;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID Rotate270GetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPROTATE_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPROTATE_PARMS)lpCmdParms;
else
if (!(lpParms = (LPROTATE_PARMS)Alloc((long)sizeof(ROTATE_PARMS))))
	return(NULL);
lpParms->Angle = TOFIXED(270);
lpParms->fUseWeightedAvg = NO;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID RotateAnyGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPROTATE_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPROTATE_PARMS)Alloc((long)sizeof(ROTATE_PARMS))))
	return(NULL);
lpParms->Angle = GetFixedParm(lpCommand, 1);
lpParms->fUseWeightedAvg = GetIntParm(lpCommand, 2);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID MirrorXGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPMIRROR_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPMIRROR_PARMS)lpCmdParms;
else
if (!(lpParms = (LPMIRROR_PARMS)Alloc((long)sizeof(MIRROR_PARMS))))
	return(NULL);
lpParms->xmirror = YES;
lpParms->ymirror = NO;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID MirrorYGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPMIRROR_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPMIRROR_PARMS)lpCmdParms;
else
if (!(lpParms = (LPMIRROR_PARMS)Alloc((long)sizeof(MIRROR_PARMS))))
	return(NULL);
lpParms->xmirror = NO;
lpParms->ymirror = YES;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID MirrorXYGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPMIRROR_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPMIRROR_PARMS)lpCmdParms;
else
if (!(lpParms = (LPMIRROR_PARMS)Alloc((long)sizeof(MIRROR_PARMS))))
	return(NULL);
lpParms->xmirror = YES;
lpParms->ymirror = YES;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SizeGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSIZE_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPSIZE_PARMS)Alloc((long)sizeof(SIZE_PARMS))))
	return(NULL);
lpParms->Width = GetFixedParm(lpCommand, 1);
lpParms->Height = GetFixedParm(lpCommand, 2);
lpParms->Resolution = GetIntParm(lpCommand, 3);
lpParms->SmartSize = GetIntParm(lpCommand, 4);
lpParms->MaintainSize = GetIntParm(lpCommand, 5);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID ExpandGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPEXPAND_PARMS lpParms;
COLORSPACE Space;
char szTemp[20];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPEXPAND_PARMS)Alloc((long)sizeof(EXPAND_PARMS))))
	return(NULL);
lpParms->fUseRightBottom = GetIntParm(lpCommand, 7);
if (lpParms->fUseRightBottom)
	{
	lpParms->Left = GetFixedParm(lpCommand, 1);
	lpParms->Top = GetFixedParm(lpCommand, 2);
	lpParms->Right = GetFixedParm(lpCommand, 3);
	lpParms->Bottom = GetFixedParm(lpCommand, 4);
	}
else
	{
	lpParms->Width = GetFixedParm(lpCommand, 1);
	lpParms->Height = GetFixedParm(lpCommand, 2);
	lpParms->Top = GetFixedParm(lpCommand, 3);
	lpParms->Left = GetFixedParm(lpCommand, 4);
	}
Space = (COLORSPACE)GetIntParm(lpCommand, 5);
GetStringParm(lpCommand, 6, szTemp);
SetColorInfoString(szTemp, &lpParms->Color, Space);
SetColorInfo( &lpParms->Color, &lpParms->Color, Space );
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID ConvertGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPCONVERT_PARMS lpParms;
int index = 1;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPCONVERT_PARMS)Alloc((long)sizeof(CONVERT_PARMS))))
	return(NULL);
index = GetCMSInfo(lpCommand, &lpParms->cmsInfo, index);

lpParms->DitherType = (DITHER_TYPE)GetIntParm(lpCommand, index++);
lpParms->ConvertType = (CONVERT_TYPE)GetIntParm(lpCommand, index++);
lpParms->iLevels = GetIntParm(lpCommand, index++);
GetStringParm(lpCommand, index++, lpParms->szPalette);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID GradientGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPGRADIENT_PARMS lpParms;
STRING szTemp;
char szMM[MM_STR_LEN];
COLORSPACE Space;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPGRADIENT_PARMS)Alloc((long)sizeof(GRADIENT_PARMS))))
	return(NULL);
lpParms->VigOpacity = TOOPACITY(GetIntParm(lpCommand, 1));
lpParms->VigMergeMode = StringToMergeMode(GetStringParm(lpCommand, 2, szMM));
lpParms->iBaseRes = GetIntParm(lpCommand, 3);
lpParms->x1 = GetIntParm(lpCommand, 4);
lpParms->y1 = GetIntParm(lpCommand, 5);
lpParms->x2 = GetIntParm(lpCommand, 6);
lpParms->y2 = GetIntParm(lpCommand, 7);
Space = (COLORSPACE)GetIntParm(lpCommand, 8);
GetStringParm(lpCommand, 9, szTemp);
SetColorInfoString(szTemp, &lpParms->StartColor, Space);
SetColorInfo( &lpParms->StartColor, &lpParms->StartColor, Space );
Space = (COLORSPACE)GetIntParm(lpCommand, 10);
GetStringParm(lpCommand, 11, szTemp);
SetColorInfoString(szTemp, &lpParms->EndColor, Space);
SetColorInfo( &lpParms->EndColor, &lpParms->EndColor, Space );
lpParms->Gradient = GetIntParm(lpCommand, 12);
lpParms->Gradient += IDC_VIGFIRST;
lpParms->RepeatCount = GetIntParm(lpCommand, 13);
lpParms->SoftTransition = GetIntParm(lpCommand, 14);
lpParms->Midpoint = GetIntParm(lpCommand, 15);
lpParms->VigColorModel = GetIntParm(lpCommand, 16);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID TintFillGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPTINTFILL_PARMS lpParms;
STRING szTemp;
COLORSPACE Space;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPTINTFILL_PARMS)Alloc((long)sizeof(TINTFILL_PARMS))))
	return(NULL);
lpParms->FillOpacity = TOOPACITY(GetIntParm(lpCommand, 1));
lpParms->FillMergeMode = StringToMergeMode(GetStringParm(lpCommand, 2, szMM));
Space = (COLORSPACE)GetIntParm(lpCommand, 3);
GetStringParm(lpCommand, 4, szTemp);
SetColorInfoString(szTemp, &lpParms->Color, Space);
SetColorInfo( &lpParms->Color, &lpParms->Color, Space );
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID DeleteGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPTINTFILL_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPTINTFILL_PARMS)lpCmdParms;
else
if (!(lpParms = (LPTINTFILL_PARMS)Alloc((long)sizeof(TINTFILL_PARMS))))
	return(NULL);
lpParms->FillOpacity = 255;
lpParms->FillMergeMode = (MERGE_MODE)MM_NORMAL;
lpParms->Color.gray = 255;
SetColorInfo( &lpParms->Color, &lpParms->Color, CS_GRAY );
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID CutGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPTINTFILL_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPTINTFILL_PARMS)lpCmdParms;
else
if (!(lpParms = (LPTINTFILL_PARMS)Alloc((long)sizeof(TINTFILL_PARMS))))
	return(NULL);
lpParms->FillOpacity = 255;
lpParms->FillMergeMode = (MERGE_MODE)MM_NORMAL;
lpParms->Color.gray = 255;
SetColorInfo( &lpParms->Color, &lpParms->Color, CS_GRAY );
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID TextureFillGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPTEXTUREFILL_PARMS lpParms;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPTEXTUREFILL_PARMS)Alloc((long)sizeof(TEXTUREFILL_PARMS))))
	return(NULL);
lpParms->TextureOpacity = TOOPACITY(GetIntParm(lpCommand, 1));
lpParms->TextureMergeMode = StringToMergeMode(GetStringParm(lpCommand, 2, szMM));
GetStringParm(lpCommand, 3, lpParms->TextureName);
lpParms->fHorzFlip = GetIntParm(lpCommand, 4);
lpParms->fVertFlip = GetIntParm(lpCommand, 5);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID FloodGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPFLOOD_PARMS lpParms;
STRING szTemp;
COLORSPACE Space;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPFLOOD_PARMS)Alloc((long)sizeof(FLOOD_PARMS))))
	return(NULL);
lpParms->FloodOpacity = TOOPACITY(GetIntParm(lpCommand, 1));
lpParms->FloodMergeMode = StringToMergeMode(GetStringParm(lpCommand, 2, szMM));
Space = (COLORSPACE)GetIntParm(lpCommand, 3);
GetStringParm(lpCommand, 4, szTemp);
SetColorInfoString(szTemp, &lpParms->Color, Space);
SetColorInfo( &lpParms->Color, &lpParms->Color, Space );

lpParms->iBaseRes = GetIntParm(lpCommand, 5);
lpParms->x = GetIntParm(lpCommand, 6);
lpParms->y = GetIntParm(lpCommand, 7);

Space = (COLORSPACE)GetIntParm(lpCommand, 8);
GetStringParm(lpCommand, 9, szTemp);
SetColorInfoString(szTemp, &lpParms->InColor, Space);
SetColorInfo( &lpParms->InColor, &lpParms->InColor, Space );
lpParms->FloodRange = GetIntParm(lpCommand, 10);
lpParms->FloodFade = GetIntParm(lpCommand, 11);
lpParms->idFloodColorModel = GetIntParm(lpCommand, 12);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID ContBritGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPCONTBRIT_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPCONTBRIT_PARMS)Alloc((long)sizeof(CONTBRIT_PARMS))))
	return(NULL);
lpParms->Contrast = GetIntParm(lpCommand, 1);
lpParms->Brightness[0] = GetIntParm(lpCommand, 2);
lpParms->Brightness[1] = GetIntParm(lpCommand, 3);
lpParms->Brightness[2] = GetIntParm(lpCommand, 4);
lpParms->Brightness[3] = GetIntParm(lpCommand, 5);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID BalanceGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPBALANCE_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPBALANCE_PARMS)Alloc((long)sizeof(BALANCE_PARMS))))
	return(NULL);
lpParms->MasterContrast = GetIntParm(lpCommand, 1);
lpParms->MasterBrightness[0] = GetIntParm(lpCommand, 2);
lpParms->MasterBrightness[1] = GetIntParm(lpCommand, 3);
lpParms->MasterBrightness[2] = GetIntParm(lpCommand, 4);
lpParms->MasterBrightness[3] = GetIntParm(lpCommand, 5);
lpParms->fMasterReverse = GetIntParm(lpCommand, 6);
lpParms->RedContrast = GetIntParm(lpCommand, 7);
lpParms->RedBrightness[0] = GetIntParm(lpCommand, 8);
lpParms->RedBrightness[1] = GetIntParm(lpCommand, 9);
lpParms->RedBrightness[2] = GetIntParm(lpCommand, 10);
lpParms->RedBrightness[3] = GetIntParm(lpCommand, 11);
lpParms->fRedReverse = GetIntParm(lpCommand, 12);
lpParms->GreenContrast = GetIntParm(lpCommand, 13);
lpParms->GreenBrightness[0] = GetIntParm(lpCommand, 14);
lpParms->GreenBrightness[1] = GetIntParm(lpCommand, 15);
lpParms->GreenBrightness[2] = GetIntParm(lpCommand, 16);
lpParms->GreenBrightness[3] = GetIntParm(lpCommand, 17);
lpParms->fGreenReverse = GetIntParm(lpCommand, 18);
lpParms->BlueContrast = GetIntParm(lpCommand, 19);
lpParms->BlueBrightness[0] = GetIntParm(lpCommand, 20);
lpParms->BlueBrightness[1] = GetIntParm(lpCommand, 21);
lpParms->BlueBrightness[2] = GetIntParm(lpCommand, 22);
lpParms->BlueBrightness[3] = GetIntParm(lpCommand, 23);
lpParms->fBlueReverse = GetIntParm(lpCommand, 24);
lpParms->BlackContrast = GetIntParm(lpCommand, 25);
lpParms->BlackBrightness[0] = GetIntParm(lpCommand, 26);
lpParms->BlackBrightness[1] = GetIntParm(lpCommand, 27);
lpParms->BlackBrightness[2] = GetIntParm(lpCommand, 28);
lpParms->BlackBrightness[3] = GetIntParm(lpCommand, 29);
lpParms->fBlackReverse = GetIntParm(lpCommand, 30);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID MapGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPMODIFYMAP_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPMODIFYMAP_PARMS)Alloc((long)sizeof(MODIFYMAP_PARMS))))
	return(NULL);
lpParms->MasterInfo.Points = GetIntParm(lpCommand, 1);
lpParms->RedInfo.Points = GetIntParm(lpCommand, 2);
lpParms->GreenInfo.Points = GetIntParm(lpCommand, 3);
lpParms->BlueInfo.Points = GetIntParm(lpCommand, 4);
lpParms->BlackInfo.Points = GetIntParm(lpCommand, 5);
if (!MapReadInfo(&lpParms->MasterInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
if (!MapReadInfo(&lpParms->RedInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
if (!MapReadInfo(&lpParms->GreenInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
if (!MapReadInfo(&lpParms->BlueInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
if (!MapReadInfo(&lpParms->BlackInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}


return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID ToneGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPTONE_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPTONE_PARMS)Alloc((long)sizeof(TONE_PARMS))))
	return(NULL);
lpParms->MasterInfo.Points = GetIntParm(lpCommand, 1);
lpParms->RedInfo.Points = GetIntParm(lpCommand, 2);
lpParms->GreenInfo.Points = GetIntParm(lpCommand, 3);
lpParms->BlueInfo.Points = GetIntParm(lpCommand, 4);
lpParms->BlackInfo.Points = GetIntParm(lpCommand, 5);
if (!MapReadInfo(&lpParms->MasterInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
if (!MapReadInfo(&lpParms->RedInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
if (!MapReadInfo(&lpParms->GreenInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
if (!MapReadInfo(&lpParms->BlueInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
	
if (!MapReadInfo(&lpParms->BlackInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
	

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID ShapeMaskGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSHAPEMASK_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPSHAPEMASK_PARMS)Alloc((long)sizeof(SHAPEMASK_PARMS))))
	return(NULL);
lpParms->Shape = GetIntParm(lpCommand, 1);
lpParms->Mode = GetIntParm(lpCommand, 2);
lpParms->iBaseRes = GetIntParm(lpCommand, 3);
lpParms->x1 = GetIntParm(lpCommand, 4);
lpParms->y1 = GetIntParm(lpCommand, 5);
lpParms->x2 = GetIntParm(lpCommand, 6);
lpParms->y2 = GetIntParm(lpCommand, 7);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID FreeMaskGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPFREEMASK_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPFREEMASK_PARMS)Alloc((long)sizeof(FREEMASK_PARMS))))
	return(NULL);
lpParms->Mode = GetIntParm(lpCommand, 1);
lpParms->Antialias = GetIntParm(lpCommand, 2);
lpParms->iBaseRes = GetIntParm(lpCommand, 3);
lpParms->nPoints = GetIntParm(lpCommand, 4);
lpParms->lpPoints = ReadIntPointList(fh, lpParms->nPoints);
if (!lpParms->lpPoints)
	{
	FreeUp((LPVOID)lpParms);
	return(NULL);
	}
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID PointEditGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPPOINTEDIT_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPPOINTEDIT_PARMS)Alloc((long)sizeof(POINTEDIT_PARMS))))
	return(NULL);
lpParms->Mode = GetIntParm(lpCommand, 1);
lpParms->Antialias 		= GetIntParm(lpCommand, 2);

lpParms->iBaseRes 		= GetIntParm(lpCommand, 3);
lpParms->rErase.left 	= GetIntParm(lpCommand, 4);
lpParms->rErase.top 	= GetIntParm(lpCommand, 5);
lpParms->rErase.right 	= GetIntParm(lpCommand, 6);
lpParms->rErase.bottom 	= GetIntParm(lpCommand, 7);
lpParms->iEraseVal 		= GetIntParm(lpCommand, 8);
lpParms->nPoints = GetIntParm(lpCommand, 9);
lpParms->lpPoints = ReadIntPointList(fh, lpParms->nPoints);
if (!lpParms->lpPoints)
	{
	FreeUp((LPVOID)lpParms);
	return(NULL);
	}
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID EffectGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPEFFECT_PARMS lpParms;
LPSTR lpData;
int size;
char szData[256];

if (lpCmdParms)
	return(lpCmdParms);
	
if (!(lpParms = (LPEFFECT_PARMS)Alloc((long)sizeof(EFFECT_PARMS))))
	return(NULL);
// find the effect name
GetStringParm(lpCommand, 1, lpParms->szEffect);
// get the effect string
lpParms->lpString = NULL;
GetStringParm(lpCommand, 2, szData);
size = lstrlen(szData) + 1;
if (!(lpData = (LPSTR)Alloc((long)size+1)))
	{
	FreeUp(lpParms);
	return(NULL);
	}
lstrcpy(lpData, szData);
lpParms->lpString = lpData;
// get in out flag
lpParms->bInOut = GetIntParm(lpCommand, 3);
return(lpParms);
}


/***********************************************************************/
LPVOID MagicMaskGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPMAGICMASK_PARMS lpParms;
COLORSPACE Space;
char szTemp[20];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPMAGICMASK_PARMS)Alloc((long)sizeof(MAGICMASK_PARMS))))
	return(NULL);
	
lpParms->iBaseRes = GetIntParm(lpCommand, 1);
lpParms->x = GetIntParm(lpCommand, 2);
lpParms->y = GetIntParm(lpCommand, 3);

// get color info
Space = (COLORSPACE)GetIntParm(lpCommand, 4);
GetStringParm(lpCommand, 5, szTemp);
SetColorInfoString(szTemp, &lpParms->Color, Space);
SetColorInfo( &lpParms->Color, &lpParms->Color, Space );

lpParms->Mode = GetIntParm(lpCommand, 6);
lpParms->WandRange = GetIntParm(lpCommand, 7);
lpParms->WandFade = GetIntParm(lpCommand, 8);
lpParms->idMagicColorModel = GetIntParm(lpCommand, 9);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SimilarGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSIMILAR_PARMS lpParms;
COLORSPACE Space;
char szTemp[20];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPSIMILAR_PARMS)Alloc((long)sizeof(SIMILAR_PARMS))))
	return(NULL);
lpParms->iBaseRes = GetIntParm(lpCommand, 1);
lpParms->x = GetIntParm(lpCommand, 2);
lpParms->y = GetIntParm(lpCommand, 3);

// get color info
Space = (COLORSPACE)GetIntParm(lpCommand, 4);
GetStringParm(lpCommand, 5, szTemp);
SetColorInfoString(szTemp, &lpParms->Color, Space);
SetColorInfo( &lpParms->Color, &lpParms->Color, Space );

lpParms->Mode = GetIntParm(lpCommand, 6);
lpParms->WandRange = GetIntParm(lpCommand, 7);
lpParms->WandFade = GetIntParm(lpCommand, 8);
lpParms->idMagicColorModel = GetIntParm(lpCommand, 9);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID TransformerGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPTRANSFORMER_PARMS lpParms;
int i;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPTRANSFORMER_PARMS)Alloc((long)sizeof(TRANSFORMER_PARMS))))
	return(NULL);
	
lpParms->iBaseRes = GetIntParm(lpCommand, 1);
lpParms->SelectRect.left 	= GetIntParm(lpCommand, 2);
lpParms->SelectRect.top 	= GetIntParm(lpCommand, 3);
lpParms->SelectRect.right 	= GetIntParm(lpCommand, 4);
lpParms->SelectRect.bottom 	= GetIntParm(lpCommand, 5);
lpParms->Function = GetIntParm(lpCommand, 6);
lpParms->Function += IDC_TRANSFUNCTIONFIRST;
lpParms->Mode = GetIntParm(lpCommand, 7);
lpParms->TransformOpacity = TOOPACITY(GetIntParm(lpCommand, 8));
lpParms->TransformMergeMode = StringToMergeMode(GetStringParm(lpCommand, 9, szMM));
lpParms->fHiQuality = GetIntParm(lpCommand, 10);
lpParms->fPasteInto = GetIntParm(lpCommand, 11);
lpParms->fDelete 	= GetIntParm(lpCommand, 12);
for (i = 0; i < 4; ++i)
	{
	lpParms->DistortPoints[i].x = GetIntParm(lpCommand, 13+(i*2));
	lpParms->DistortPoints[i].y = GetIntParm(lpCommand, 14+(i*2));
	}
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID TransformObjectsGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPTRANSFORMOBJECTS_PARMS lpParms;
int i;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPTRANSFORMOBJECTS_PARMS)Alloc((long)sizeof(TRANSFORMOBJECTS_PARMS))))
	return(NULL);
lpParms->fHiQuality = GetIntParm(lpCommand, 1);
lpParms->iBaseRes = GetIntParm(lpCommand, 2);
for (i = 0; i < 4; ++i)
	{
	lpParms->DistortPoints[i].x = GetIntParm(lpCommand, 3+(i*2));
	lpParms->DistortPoints[i].y = GetIntParm(lpCommand, 4+(i*2));
	}
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID PasteFromGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPPASTEFROM_PARMS lpParms;
char szMM[MM_STR_LEN];
int i, index = 1;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPPASTEFROM_PARMS)Alloc((long)sizeof(PASTEFROM_PARMS))))
	return(NULL);

lpParms->TransformOpacity = TOOPACITY(GetIntParm(lpCommand, index++));
lpParms->TransformMergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));
lpParms->fHiQuality = GetIntParm(lpCommand, index++);
lpParms->fPasteInto = GetIntParm(lpCommand, index++);
lpParms->iBaseRes = GetIntParm(lpCommand, index++);
for (i = 0; i < 4; ++i)
	{
	lpParms->DistortPoints[i].x = GetIntParm(lpCommand, index++);
	lpParms->DistortPoints[i].y = GetIntParm(lpCommand, index++);
	}
GetStringParm(lpCommand, index++, lpParms->szClipboard);
GetStringParm(lpCommand, index++, lpParms->szClipFileName);
index = GetCMSInfo(lpCommand, &lpParms->cmsInfo, index);

lpParms->fTemporary = FALSE;

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID PasteGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPPASTE_PARMS lpParms;
char szMM[MM_STR_LEN];
int i, index = 1;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPPASTE_PARMS)Alloc((long)sizeof(PASTE_PARMS))))
	return(NULL);
lpParms->TransformOpacity = TOOPACITY(GetIntParm(lpCommand, index++));
lpParms->TransformMergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));
lpParms->fHiQuality = GetIntParm(lpCommand, index++);
lpParms->fPasteInto = GetIntParm(lpCommand, index++);
lpParms->iBaseRes = GetIntParm(lpCommand, index++);
for (i = 0; i < 4; ++i)
	{
	lpParms->DistortPoints[i].x = GetIntParm(lpCommand, index++);
	lpParms->DistortPoints[i].y = GetIntParm(lpCommand, index++);
	}
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID LoadMaskGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPLOADMASK_PARMS lpParms;
int i;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPLOADMASK_PARMS)Alloc((long)sizeof(LOADMASK_PARMS))))
	return(NULL);
lpParms->Mode = GetIntParm(lpCommand, 1);
lpParms->fHiQuality = GetIntParm(lpCommand, 2);
lpParms->iBaseRes = GetIntParm(lpCommand, 3);
for (i = 0; i < 4; ++i)
	{
	lpParms->DistortPoints[i].x = GetIntParm(lpCommand, 4+(i*2));
	lpParms->DistortPoints[i].y = GetIntParm(lpCommand, 5+(i*2));
	}
GetStringParm(lpCommand, 12, lpParms->szMask);
lpParms->fDeleteMask = GetIntParm(lpCommand, 13);
GetStringParm(lpCommand, 14, lpParms->szMaskFileName);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID ReadSelectObjectRelativeParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSELECTOBJRELATIVE_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPSELECTOBJRELATIVE_PARMS)Alloc((long)sizeof(SELECTOBJRELATIVE_PARMS))))
	return(NULL);
lpParms->Pos = GetIntParm(lpCommand, 1);
lpParms->Shift = GetIntParm(lpCommand, 2);
return((LPVOID)lpParms);
}



/***********************************************************************/
WRITE_FUNC WriteSelectObjectRelativeParms(LPSELECTOBJRELATIVE_PARMS lpParms,
                   MACRO_FILE_HANDLE fh, LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat, lpParms->Pos, lpParms->Shift);
return(WF_WRITE);
}

/***********************************************************************/
LPVOID ReadObjectLayeringGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPOBJECTLAYERING_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPOBJECTLAYERING_PARMS)Alloc((long)sizeof(OBJECTLAYERING_PARMS))))
	return(NULL);

lpParms->Offset = GetIntParm(lpCommand, 1);
return((LPVOID)lpParms);
}

/***********************************************************************/
WRITE_FUNC WriteObjectLayeringParms(LPOBJECTLAYERING_PARMS lpParms,
                  MACRO_FILE_HANDLE fh, LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat, lpParms->Offset);
return(WF_WRITE);
}




/***********************************************************************/
LPVOID SelectObjectsGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSELECTOBJECTS_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPSELECTOBJECTS_PARMS)Alloc((long)sizeof(SELECTOBJECTS_PARMS))))
	return(NULL);

lpParms->iBaseRes 			= GetIntParm(lpCommand, 1);
lpParms->SelectRect.left 	= GetIntParm(lpCommand, 2);
lpParms->SelectRect.top 	= GetIntParm(lpCommand, 3);
lpParms->SelectRect.right 	= GetIntParm(lpCommand, 4);
lpParms->SelectRect.bottom 	= GetIntParm(lpCommand, 5);
lpParms->fExtendedSel 		= GetIntParm(lpCommand, 6);
lpParms->fSelectAll 		= GetIntParm(lpCommand, 7);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID MoveObjectsGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPMOVEOBJECTS_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPMOVEOBJECTS_PARMS)Alloc((long)sizeof(MOVEOBJECTS_PARMS))))
	return(NULL);
lpParms->iBaseRes = GetIntParm(lpCommand, 1);
lpParms->x = GetIntParm(lpCommand, 2);
lpParms->y = GetIntParm(lpCommand, 3);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SetOpacityGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSETATTROBJECTS_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPSETATTROBJECTS_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPSETATTROBJECTS_PARMS)Alloc((long)sizeof(SETATTROBJECTS_PARMS))))
		return(NULL);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, 1));
	}
lpParms->AttributeType = AT_OPACITY;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID OrderUpGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPLAYEROBJECTS_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPLAYEROBJECTS_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPLAYEROBJECTS_PARMS)Alloc((long)sizeof(LAYEROBJECTS_PARMS))))
		return(NULL);
	}
lpParms->fToTop = YES;
lpParms->fOneLevel = YES;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID FeatherAlphaGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPFEATHERALPHA_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPFEATHERALPHA_PARMS)Alloc((long)sizeof(FEATHERALPHA_PARMS))))
	return(NULL);
lpParms->inside 	= GetIntParm(lpCommand, 1);
lpParms->iBaseRes 	= GetIntParm(lpCommand, 2);
lpParms->size 		= GetIntParm(lpCommand, 3);
lpParms->edge 		= GetIntParm(lpCommand, 4);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID MaskSmoothGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
 LPMASK_SMOOTH_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPMASK_SMOOTH_PARMS)Alloc((long)sizeof(MASK_SMOOTH_PARMS))))
	return(NULL);
lpParms->iBaseRes = GetIntParm(lpCommand, 1);
lpParms->gauss = GetIntParm(lpCommand, 2);
lpParms->thresh = GetIntParm(lpCommand, 3);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID RemoveHolesGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
 LPREMOVE_HOLES_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPREMOVE_HOLES_PARMS)Alloc((long)sizeof(REMOVE_HOLES_PARMS))))
	return(NULL);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID TextGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPTEXT_PARMS lpParms;
char szTemp[MAX_CMD_LEN];
COLORSPACE Space;
int lines, len;
LPSTR lpTextBuf;
long lBufSize;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPTEXT_PARMS)Alloc((long)sizeof(TEXT_PARMS))))
	return(NULL);
GetStringParm(lpCommand, 1, lpParms->szFaceName);
lpParms->Size = GetIntParm(lpCommand, 2);
lpParms->Italic = (BOOL)GetIntParm(lpCommand, 3);
lpParms->Underline = (BOOL)GetIntParm(lpCommand, 4);
lpParms->Strikeout = (BOOL)GetIntParm(lpCommand, 5);
lpParms->Weight = GetIntParm(lpCommand, 6);
lpParms->Angle = GetIntParm(lpCommand, 7);
lpParms->AntiAlias = GetIntParm(lpCommand, 8);
lpParms->Justification = GetIntParm(lpCommand, 9);
lpParms->iBaseRes = GetIntParm(lpCommand, 10);
lpParms->StartPos.x = GetIntParm(lpCommand, 11);
lpParms->StartPos.y = GetIntParm(lpCommand, 12);
Space = (COLORSPACE)GetIntParm(lpCommand, 13);
GetStringParm(lpCommand, 14, szTemp);
SetColorInfoString(szTemp, &lpParms->Color, Space);
SetColorInfo( &lpParms->Color, &lpParms->Color, Space );
lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, 15));
lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, 16, szMM));
lines = GetIntParm(lpCommand, 17);
lpParms->DestPos.x = GetIntParm(lpCommand, 18);
lpParms->DestPos.y = GetIntParm(lpCommand, 19);

lpTextBuf = NULL;
lBufSize = 0;
while (--lines >= 0)
	{
	if (!ReadLine(fh, szTemp, sizeof(szTemp)))
		{
		FreeUp((LPTR)lpParms);
		if (lpTextBuf)
			FreeUp((LPTR)lpTextBuf);
		return(NULL);
		}
	len = lstrlen(szTemp);
	szTemp[len] = VK_RETURN;
	++len;
	szTemp[len] = '\0';
	lBufSize += len;
	if (lpTextBuf)
		lpTextBuf = (LPSTR)AllocExtend(( LPTR )lpTextBuf, lBufSize+1);
	else
		{
		lpTextBuf = (LPSTR)Alloc(lBufSize+1);
		*lpTextBuf = '\0';
		}
	if (!lpTextBuf)
		{
		FreeUp((LPTR)lpParms);
		return(NULL);
		}
	lstrcat(lpTextBuf, szTemp);
	}
lpParms->lpTextBuf = lpTextBuf;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID ActivateWindowGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPACTIVATEWINDOW_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPACTIVATEWINDOW_PARMS)Alloc((long)sizeof(ACTIVATEWINDOW_PARMS))))
	return(NULL);
GetStringParm(lpCommand, 1, lpParms->szFileName);
lpParms->iOffset = GetIntParm(lpCommand, 2);
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID NewImageGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPNEW_PARMS lpParms;
COLORSPACE Space;
char szTemp[20];

if (lpCmdParms)
	lpParms = (LPNEW_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPNEW_PARMS)Alloc((long)sizeof(NEW_PARMS))))
		return(NULL);
	lpParms->Type = (FRMDATATYPE)GetIntParm(lpCommand, 1);
	lpParms->Width = GetFixedParm(lpCommand, 2);
	lpParms->Height = GetFixedParm(lpCommand, 3);
	lpParms->Resolution = GetIntParm(lpCommand, 4);
	Space = (COLORSPACE)GetIntParm(lpCommand, 5);
	GetStringParm(lpCommand, 6, szTemp);
	SetColorInfoString(szTemp, &lpParms->Color, Space);
	SetColorInfo( &lpParms->Color, &lpParms->Color, Space );
	GetCMSInfo( lpCommand, &lpParms->CmsInfo, 7 );
	}
lpParms->lpCmdList = lpCmdList;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID PosterizeGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPPOSTERIZE_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPPOSTERIZE_PARMS)Alloc((long)sizeof(POSTERIZE_PARMS))))
	return(NULL);

lpParms->MasterInfo.Levels = GetIntParm(lpCommand, 1);
lpParms->MasterInfo.Threshold = GetIntParm(lpCommand, 2);
lpParms->RedInfo.Levels = GetIntParm(lpCommand, 3);
lpParms->RedInfo.Threshold = GetIntParm(lpCommand, 4);
lpParms->GreenInfo.Levels = GetIntParm(lpCommand, 5);
lpParms->GreenInfo.Threshold = GetIntParm(lpCommand, 6);
lpParms->BlueInfo.Levels = GetIntParm(lpCommand, 7);
lpParms->BlueInfo.Threshold = GetIntParm(lpCommand, 8);
lpParms->BlackInfo.Levels = GetIntParm(lpCommand, 9);
lpParms->BlackInfo.Threshold = GetIntParm(lpCommand, 10);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID HueGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPHUE_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPHUE_PARMS)Alloc((long)sizeof(HUE_PARMS))))
	return(NULL);

lpParms->HueInfo.Points = GetIntParm(lpCommand, 1);
lpParms->Saturation = GetIntParm(lpCommand, 2);
lpParms->Brightness = GetIntParm(lpCommand, 3);

if (!MapReadInfo(&lpParms->HueInfo, fh))
	{
	FreeUp(lpParms);
	return(NULL);
	}
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID ApplyCalGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPAPPLYCAL_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPAPPLYCAL_PARMS)Alloc((long)sizeof(APPLYCAL_PARMS))))
	return(NULL);

GetStringParm(lpCommand, 1, lpParms->szScanMap);
GetStringParm(lpCommand, 2, lpParms->szPrintMap);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID CopyToFileGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPCOPYTOFILE_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPCOPYTOFILE_PARMS)Alloc((long)sizeof(COPYTOFILE_PARMS))))
	return(NULL);

GetStringParm(lpCommand, 1, lpParms->szName);
lpParms->Type = GetIntParm(lpCommand, 2);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SaveMaskGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSAVEMASK_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPSAVEMASK_PARMS)Alloc((long)sizeof(SAVEMASK_PARMS))))
	return(NULL);

GetStringParm(lpCommand, 1, lpParms->szName);

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID PaintGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
COLORSPACE Space;
STRING szTemp;
int index;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
	lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));
	Space = (COLORSPACE)GetIntParm(lpCommand, index++);
	GetStringParm(lpCommand, index++, szTemp);
	SetColorInfoString(szTemp, &lpParms->Color, Space);
	SetColorInfo( &lpParms->Color, &lpParms->Color, Space );
	}
lpParms->BrushType = BT_PAINT;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SprayGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
COLORSPACE Space;
STRING szTemp;
int index;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
	lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));
	Space = (COLORSPACE)GetIntParm(lpCommand, index++);
	GetStringParm(lpCommand, index++, szTemp);
	SetColorInfoString(szTemp, &lpParms->Color, Space);
	SetColorInfo( &lpParms->Color, &lpParms->Color, Space );
	}
lpParms->BrushType = BT_SPRAY;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID CloneGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, YES, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
	lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));
	GetStringParm(lpCommand, index++, lpParms->szSourceImage);
	lpParms->ptSource.x = GetIntParm(lpCommand, index++);
	lpParms->ptSource.y = GetIntParm(lpCommand, index++);
	}
lpParms->BrushType = BT_CLONE;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID TextureGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO/*YES*/, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
	lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));
	}
lpParms->BrushType = BT_TEXTURE;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SmearGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
	lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));
	}
lpParms->BrushType = BT_SMEAR;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID EraserGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
COLORSPACE Space;
STRING szTemp;
int index;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
	lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));
	Space = (COLORSPACE)GetIntParm(lpCommand, index++);
	GetStringParm(lpCommand, index++, szTemp);
	SetColorInfoString(szTemp, &lpParms->Color, Space);
	SetColorInfo( &lpParms->Color, &lpParms->Color, Space );
	}
lpParms->BrushType = BT_ERASER;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID AltEraserGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPRETOUCH_PARMS)Alloc((long)sizeof(RETOUCH_PARMS))))
		return(NULL);
	index = 1;
	lpParms->BrushShape = GetIntParm(lpCommand, index++);
	lpParms->BrushShape += IDC_FIRST_BRUSHSHAPE;
	GetStringParm(lpCommand, index++, lpParms->ShapeName);
	lpParms->nPoints = GetIntParm(lpCommand, index++);
	lpParms->fUsePressure = GetIntParm(lpCommand, index++);
	lpParms->Size = GetIntParm(lpCommand, index++);
	lpParms->Feather = GetIntParm(lpCommand, index++);
	lpParms->iDstRes = GetIntParm(lpCommand, index++);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));

	lpParms->hpStrokeInfo = ReadStrokeInfo(fh, lpParms->nPoints,
								lpParms->fUsePressure, NO);
	if (!lpParms->hpStrokeInfo)
		{
		FreeUp(lpParms);
		return(NULL);
		}
	}
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SharpGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Pressure = GetIntParm(lpCommand, index++);
	}
lpParms->BrushType = BT_SHARP;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID SmoothGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Pressure = GetIntParm(lpCommand, index++);
	}
lpParms->BrushType = BT_SMOOTH;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID LightenGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Pressure = GetIntParm(lpCommand, index++);
	}
lpParms->BrushType = BT_LIGHTEN;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID DarkenGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	lpParms = RetouchGetCommonParms(lpCommand, fh, NO, &index);
	if (!lpParms)
		return(NULL);
	lpParms->Pressure = GetIntParm(lpCommand, index++);
	}
lpParms->BrushType = BT_DARKEN;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID DrawShapeGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPDRAWSHAPE_PARMS lpParms;
COLORSPACE Space;
STRING szTemp;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPDRAWSHAPE_PARMS)Alloc((long)sizeof(DRAWSHAPE_PARMS))))
	return(NULL);
lpParms->ShapeType = (SHAPE_TYPE)GetIntParm(lpCommand, 1);
lpParms->FillStyle = (FILL_STYLE)GetIntParm(lpCommand, 2);

lpParms->iBaseRes = GetIntParm(lpCommand, 3);
lpParms->x1 = GetIntParm(lpCommand, 4);
lpParms->y1 = GetIntParm(lpCommand, 5);
lpParms->x2 = GetIntParm(lpCommand, 6);
lpParms->y2 = GetIntParm(lpCommand, 7);

GetDrawBrushParms(lpCommand, &lpParms->Brush, 8);

Space = (COLORSPACE)GetIntParm(lpCommand, 15);
GetStringParm(lpCommand, 16, szTemp);
SetColorInfoString(szTemp, &lpParms->BorderColor, Space);
SetColorInfo( &lpParms->BorderColor, &lpParms->BorderColor, Space );

Space = (COLORSPACE)GetIntParm(lpCommand, 17);
GetStringParm(lpCommand, 18, szTemp);
SetColorInfoString(szTemp, &lpParms->FillColor, Space);
SetColorInfo( &lpParms->FillColor, &lpParms->FillColor, Space );

return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID PaintOnGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;

if (lpCmdParms)
	lpParms = (LPRETOUCH_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPRETOUCH_PARMS)Alloc((long)sizeof(RETOUCH_PARMS))))
		return(NULL);
	index = 1;
	lpParms->BrushShape = GetIntParm(lpCommand, index++);
	lpParms->BrushShape += IDC_FIRST_BRUSHSHAPE;
	GetStringParm(lpCommand, index++, lpParms->ShapeName);
	lpParms->nPoints = GetIntParm(lpCommand, index++);
	lpParms->fUsePressure = GetIntParm(lpCommand, index++);
	lpParms->Size = GetIntParm(lpCommand, index++);
	lpParms->Feather = GetIntParm(lpCommand, index++);
	lpParms->iDstRes = GetIntParm(lpCommand, index++);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
	lpParms->Mode = GetIntParm(lpCommand, index++);
	lpParms->fPaintOnObject = GetIntParm(lpCommand, index++);

	lpParms->hpStrokeInfo = ReadStrokeInfo(fh, lpParms->nPoints,
								lpParms->fUsePressure, NO);
	if (!lpParms->hpStrokeInfo)
		{
		FreeUp(lpParms);
		return(NULL);
		}
	}
lpParms->BrushType = BT_PAINTON;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID StrokeMaskGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSTROKEMASK_PARMS lpParms;
COLORSPACE Space;
STRING szTemp;
int index;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	lpParms = (LPSTROKEMASK_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPSTROKEMASK_PARMS)Alloc((long)sizeof(STROKEMASK_PARMS))))
		return(NULL);
	index = 1;
	lpParms->idBrush = GetIntParm(lpCommand, index++);
	lpParms->idBrush += IDC_STROKEFIRST;
	GetStringParm(lpCommand, index++, lpParms->StyleName);
	lpParms->BrushShape = GetIntParm(lpCommand, index++);
	lpParms->BrushShape += IDC_FIRST_BRUSHSHAPE;
	GetStringParm(lpCommand, index++, lpParms->ShapeName);
	lpParms->iBaseRes = GetIntParm(lpCommand, index++);
	lpParms->Size = GetIntParm(lpCommand, index++);
	lpParms->Feather = GetIntParm(lpCommand, index++);
	lpParms->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
	lpParms->Pressure = GetIntParm(lpCommand, index++);
	lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, index++, szMM));

	Space = (COLORSPACE)GetIntParm(lpCommand, index++);
	GetStringParm(lpCommand, index++, szTemp);
	SetColorInfoString(szTemp, &lpParms->Color, Space);
	SetColorInfo( &lpParms->Color, &lpParms->Color, Space );
	}
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID DrawFreeGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPDRAWFREE_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPDRAWFREE_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = DrawFreeReadParms(lpCommand, fh)))
		return(NULL);
	}
lpParms->fClosed = YES;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID PencilGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPDRAWFREE_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPDRAWFREE_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = DrawFreeReadParms(lpCommand, fh)))
		return(NULL);
	}
lpParms->fClosed = NO;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID PaletteEditGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPPALETTEEDIT_PARMS lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPPALETTEEDIT_PARMS)Alloc((long)sizeof(PALETTEEDIT_PARMS))))
	return(NULL);
lpParms->fReMap = (BOOL)GetIntParm(lpCommand, 1);
lpParms->DitherType = (DITHER_TYPE)GetIntParm(lpCommand, 2);
lpParms->lpColorMap = FrameCreateColorMap();
if (!lpParms->lpColorMap)
	{
	FreeUp((LPTR)lpParms);
	return(NULL);
	}
lpParms->lpColorMap->NumEntries = GetIntParm(lpCommand, 3);
if (!ReadColorMap(fh, lpParms->lpColorMap))
	{
	FrameDestroyColorMap(lpParms->lpColorMap);
	FreeUp((LPTR)lpParms);
	return(NULL);
	}
return((LPVOID)lpParms);
}

/************************************************************************/
LPVOID AlignObjGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/************************************************************************/
{
LPALIGNOBJECTS_PARMS	lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPALIGNOBJECTS_PARMS)Alloc((long)sizeof(ALIGNOBJECTS_PARMS))))
	return(NULL);
lpParms->HorzAlign = ( HORZ_ALIGN )GetIntParm(lpCommand, 1);
lpParms->VertAlign = ( VERT_ALIGN )GetIntParm(lpCommand, 2);
lpParms->RectAlign = ( RECT_ALIGN )GetIntParm(lpCommand, 3);
lpParms->bPreview  = GetIntParm(lpCommand, 4);
return((LPVOID)lpParms);
}
                                                                        
/***********************************************************************/
LPVOID SetMergeModeGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSETATTROBJECTS_PARMS lpParms;
char szMM[MM_STR_LEN];

if (lpCmdParms)
	lpParms = (LPSETATTROBJECTS_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPSETATTROBJECTS_PARMS)Alloc((long)sizeof(SETATTROBJECTS_PARMS))))
		return(NULL);
	lpParms->MergeMode = StringToMergeMode(GetStringParm(lpCommand, 1, szMM));
	}
lpParms->AttributeType = AT_MERGEMODE;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID OrderDownGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPLAYEROBJECTS_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPLAYEROBJECTS_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPLAYEROBJECTS_PARMS)Alloc((long)sizeof(LAYEROBJECTS_PARMS))))
		return(NULL);
	}
lpParms->fToTop = NO;
lpParms->fOneLevel = YES;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID OrderToTopGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPLAYEROBJECTS_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPLAYEROBJECTS_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPLAYEROBJECTS_PARMS)Alloc((long)sizeof(LAYEROBJECTS_PARMS))))
		return(NULL);
	}
lpParms->fToTop = YES;
lpParms->fOneLevel = NO;
return((LPVOID)lpParms);
}

/***********************************************************************/
LPVOID OrderToBottomGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPLAYEROBJECTS_PARMS lpParms;

if (lpCmdParms)
	lpParms = (LPLAYEROBJECTS_PARMS)lpCmdParms;
else
	{
	if (!(lpParms = (LPLAYEROBJECTS_PARMS)Alloc((long)sizeof(LAYEROBJECTS_PARMS))))
		return(NULL);
	}
lpParms->fToTop = NO;
lpParms->fOneLevel = NO;
return((LPVOID)lpParms);
}

/************************************************************************/
LPVOID PosObjGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/************************************************************************/
{
LPPOSOBJECTS_PARMS	lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPPOSOBJECTS_PARMS)Alloc((long)sizeof(POSOBJECTS_PARMS))))
	return(NULL);
lpParms->iBaseRes = GetIntParm(lpCommand, 1);
lpParms->XOffset = GetIntParm(lpCommand, 2);
lpParms->YOffset = GetIntParm(lpCommand, 3);
return((LPVOID)lpParms);
}
                                                                        
/************************************************************************/
LPVOID PSFilterGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/************************************************************************/
{
LPPSFILTER_PARMS	lpParms;
DWORD				dwSize;
char				szTemp[MAX_CMD_LEN];

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPPSFILTER_PARMS)Alloc((long)sizeof(PSFILTER_PARMS))))
	return(NULL);
GetStringParm(lpCommand, 1, lpParms->szFilter);
lpParms->fHandle = GetIntParm(lpCommand, 2);
dwSize = GetLongParm(lpCommand, 3);
lpParms->lpParameters = NULL;
if (dwSize)
	{
	LPTR lpParameters, lp;
	DWORD dwBytes;

	if (lpParms->fHandle == 2) // PhotoShop handle
		{
		lpParameters = (LPTR)PS_NewHandle(dwSize);
		if (!lpParameters)
			{
			FreeUp(lpParms);
			return(NULL);
			}
		lp = *((LPPTR)lpParameters);
		}
	else // windows handle or windows pointer
		{
		lpParameters = (LPTR)GlobalAllocPtr(GMEM_MOVEABLE, dwSize);
		if (!lpParameters)
			{
			FreeUp(lpParms);
			return(NULL);
			}
		lp = lpParameters;
		}
		
	while (dwSize)
		{
		if (!ReadLine(fh, szTemp, sizeof(szTemp)))
			{
			FreeUp(lpParms);
			if (lpParms->fHandle == 2)
				PS_DisposHandle(lpParameters);
			else
				GlobalFreePtr(lpParameters);
			}
	   	dwBytes = HexToBytes(szTemp, lp);
		lp += dwBytes;
		dwSize -= dwBytes;
		}
	if (lpParms->fHandle == 1) // windows handle
		{
		HGLOBAL hMem = GlobalPtrHandle(lpParameters);
		GlobalUnlockPtr(lpParameters);
		long lTemp = LOWORD((long)(LPTR)hMem);
		lpParms->lpParameters = (LPTR)lTemp;
		}
	else
		lpParms->lpParameters = lpParameters;
	}
return((LPVOID)lpParms);
}
                                                                        
/************************************************************************/
LPVOID CropGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/************************************************************************/
{
LPCROP_PARMS	lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPCROP_PARMS)Alloc((long)sizeof(CROP_PARMS))))
	return(NULL);
lpParms->iBaseRes 		= GetIntParm(lpCommand, 1);
lpParms->CropRect.left 	= GetIntParm(lpCommand, 2);
lpParms->CropRect.top 	= GetIntParm(lpCommand, 3);
lpParms->CropRect.right = GetIntParm(lpCommand, 4);
lpParms->CropRect.bottom= GetIntParm(lpCommand, 5);
return((LPVOID)lpParms);
}
                                                                        
/***********************************************************************/
LPVOID SaveToAlbumGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPSAVETOALBUM_PARMS lpParms;
int i, index = 1;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPSAVETOALBUM_PARMS)Alloc((long)sizeof(SAVETOALBUM_PARMS))))
	return(NULL);

GetStringParm(lpCommand, index++, lpParms->szFileName);
GetStringParm(lpCommand, index++, lpParms->szAlbum);
lpParms->idFileType = GetIntParm(lpCommand, index++);
if (lpParms->idFileType)
	lpParms->idFileType += IDN_FIRSTFILETYPE - 1;
lpParms->idDataType = GetIntParm(lpCommand, index++);
if (lpParms->idDataType)
	lpParms->idDataType += IDC_FIRSTDATATYPE - 1;
switch (lpParms->idFileType)
	{
	case IDN_PP4:
	case IDN_PP:
    case IDN_PPCOMPOUND:
		lpParms->PPFFOptions.PPFFCompress = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bSaveMask = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bSaveProxy = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bSaveCmdList = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bWritePT = GetIntParm(lpCommand, index++);
		lpParms->PPFFOptions.bSaveLink = GetIntParm(lpCommand, index++);
		break;
	case IDN_TIFF:
		lpParms->TIFFOptions.TIFFCompress = GetIntParm(lpCommand, index++);
		lpParms->TIFFOptions.TIFFDiff = GetIntParm(lpCommand, index++);
		lpParms->TIFFOptions.bWritePT = GetIntParm(lpCommand, index++);
		lpParms->TIFFOptions.bSaveMask = GetIntParm(lpCommand, index++);
		break;
	case IDN_TARGA:
		lpParms->TGAOptions.bSaveMask = GetIntParm(lpCommand, index++);
		break;
	case IDN_JPEG:
		lpParms->JPEGOptions.nCompressionFactor = GetIntParm(lpCommand, index++);
		lpParms->JPEGOptions.nSubsampling =	GetIntParm(lpCommand, index++);
		break;
	case IDN_DCS:
	case IDN_EPS:
		lpParms->EPSOptions.PreviewBPP = GetIntParm(lpCommand, index++);
		lpParms->EPSOptions.CompositeBPP = GetIntParm(lpCommand, index++);
		GetStringParm(lpCommand, index++, lpParms->EPSOptions.ClipPath);
		break;
	case IDN_AVI:
		lpParms->AVIOptions.iAVIPosType = GetIntParm(lpCommand, index++);
		lpParms->AVIOptions.iAVISizingType = GetIntParm(lpCommand, index++);
		lpParms->AVIOptions.AVIPosition = GetIntParm(lpCommand, index++);
		lpParms->AVIOptions.AVILastPosition = GetIntParm(lpCommand, index++);
		break;
	default:
		break;
	}

return((LPVOID)lpParms);
}

/************************************************************************/
LPVOID DiscardUndoGetParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
/************************************************************************/
{
LPDISCARDUNDO_PARMS	lpParms;

if (lpCmdParms)
	return(lpCmdParms);
if (!(lpParms = (LPDISCARDUNDO_PARMS)Alloc((long)sizeof(DISCARDUNDO_PARMS))))
	return(NULL);
lpParms->fDiscardUndo = GetIntParm(lpCommand, 1);

return((LPVOID)lpParms);
}
                                                                        
/***********************************************************************/
/*																		*/
/*	WriteParms functions:												*/
/*		Used write commands to a macro file.							*/
/*																		*/
/*		lpParms -	The parameter structure to be written to the file	*/
/*																		*/
/*		fh -		This is the file handle for reading from a macro	*/
/*					file.  If a macro has multiple lines of data,		*/
/*					lpCommand only contains the first line and it is	*/
/*					the responsiblity of the GetParms function to read	*/
/*					the other lines by calling ReadLine().				*/
/*																		*/
/*		lpFormat - 	formatting buffer to be use by the function			*/
/*																		*/
/*		lpString - 	output buffer to be used by the functions			*/
/*																		*/
/***********************************************************************/

/***********************************************************************/
WRITE_FUNC RotateWriteParms(LPROTATE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szParm1[20];

wsprintf(lpString, lpFormat,
		FixedAscii(lpParms->Angle, szParm1, FIXED_PREC),
		lpParms->fUseWeightedAvg);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC FastBitsWriteParms(LPFASTBITS_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		(LPSTR)lpParms->szFileName,
		(LPSTR)lpParms->szVolumeID,
		lpParms->Rect.left,
		lpParms->Rect.top,
		lpParms->Rect.right,
		lpParms->Rect.bottom,
		lpParms->cmsInfo.src.DataType,
		lpParms->cmsInfo.src.ptInfo.toRCS,
		lpParms->cmsInfo.src.ptInfo.frRCS,
		lpParms->cmsInfo.dst.DataType,
		lpParms->cmsInfo.dst.ptInfo.toRCS,
		lpParms->cmsInfo.dst.ptInfo.frRCS);
return(WF_WRITE);
}


/***********************************************************************/
WRITE_FUNC LoadFileWriteParms(LPLOADFILE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
ITEMID id;
STRING szOptions;

id = lpParms->idFileType;
if (id)
	id = id - IDN_FIRSTFILETYPE + 1;
GetOpenOptionsString(lpParms->lpOptions, lpParms->idFileType, szOptions);
wsprintf(lpString, lpFormat,
		(LPSTR)lpParms->szFileName,
		(LPSTR)lpParms->szVolumeID,
		id,
		lpParms->cmsInfo.src.DataType,
		lpParms->cmsInfo.src.ptInfo.toRCS,
		lpParms->cmsInfo.src.ptInfo.frRCS,
		lpParms->cmsInfo.dst.DataType,
		lpParms->cmsInfo.dst.ptInfo.toRCS,
		lpParms->cmsInfo.dst.ptInfo.frRCS,
		(LPSTR)szOptions);
return(WF_WRITE);
}


/***********************************************************************/
WRITE_FUNC LowResLoadWriteParms(LPLOWRESLOAD_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
STRING szOptions;
ITEMID id;

id = lpParms->idFileType;
if (id)
	id = id - IDN_FIRSTFILETYPE + 1;
GetOpenOptionsString(lpParms->lpOptions, lpParms->idFileType, szOptions);
wsprintf(lpString, lpFormat,
		(LPSTR)lpParms->szFileName,
		(LPSTR)lpParms->szVolumeID,
		id,
		lpParms->Resolution,
		lpParms->cmsInfo.src.DataType,
		lpParms->cmsInfo.src.ptInfo.toRCS,
		lpParms->cmsInfo.src.ptInfo.frRCS,
		lpParms->cmsInfo.dst.DataType,
		lpParms->cmsInfo.dst.ptInfo.toRCS,
		lpParms->cmsInfo.dst.ptInfo.frRCS,
		(LPSTR)szOptions);
return(WF_WRITE);
}


/***********************************************************************/
WRITE_FUNC SizeWriteParms(LPSIZE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szParm1[20], szParm2[20];

wsprintf(lpString, lpFormat,
		FixedAscii(lpParms->Width, szParm1, FIXED_PREC),
		FixedAscii(lpParms->Height, szParm2, FIXED_PREC),
		lpParms->Resolution,
		lpParms->SmartSize,
		lpParms->MaintainSize);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC SaveFileWriteParms(LPSAVEFILE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
STRING szOptions;
int i;
ITEMID idFileType = 0, idDataType = 0;

if (lpParms->idFileType)
	idFileType = lpParms->idFileType-IDN_FIRSTFILETYPE+1;
if (lpParms->idDataType)
	idDataType = lpParms->idDataType-IDC_FIRSTDATATYPE+1;

switch (lpParms->idFileType)
	{
	case IDN_PP4:
	case IDN_PP:
    case IDN_PPCOMPOUND:
		wsprintf(szOptions, "%d %d %d %d %d %d",
				lpParms->PPFFOptions.PPFFCompress,
				lpParms->PPFFOptions.bSaveMask,
				lpParms->PPFFOptions.bSaveProxy,
				lpParms->PPFFOptions.bSaveCmdList,
				lpParms->PPFFOptions.bWritePT,
				lpParms->PPFFOptions.bSaveLink);
		break;
	case IDN_TIFF:
		wsprintf(szOptions, "%d %d %d %d",
				lpParms->TIFFOptions.TIFFCompress,
				lpParms->TIFFOptions.TIFFDiff,
				lpParms->TIFFOptions.bWritePT,
				lpParms->TIFFOptions.bSaveMask);
		break;
	case IDN_TARGA:
		wsprintf(szOptions, "%d",
				lpParms->TGAOptions.bSaveMask);
		break;
	case IDN_JPEG:
		wsprintf(szOptions, "%d %d",
				lpParms->JPEGOptions.nCompressionFactor,
				lpParms->JPEGOptions.nSubsampling);
		break;
	case IDN_DCS:
	case IDN_EPS:
		wsprintf(szOptions, "%d %d %s",
				lpParms->EPSOptions.PreviewBPP,
				lpParms->EPSOptions.CompositeBPP,
				lpParms->EPSOptions.ClipPath);
		break;
	case IDN_AVI:
		wsprintf(szOptions, "%d %d %d %d",
				lpParms->AVIOptions.iAVIPosType,
				lpParms->AVIOptions.iAVISizingType,
				lpParms->AVIOptions.AVIPosition,
				lpParms->AVIOptions.AVILastPosition);
		break;
	default:
		szOptions[0] = '\0';
		break;
	}
wsprintf(lpString, lpFormat,
		(LPSTR)lpParms->szFileName,
		idFileType,
		idDataType,
		lpParms->CmsInfo.src.DataType,
		lpParms->CmsInfo.src.ptInfo.toRCS,
		lpParms->CmsInfo.src.ptInfo.frRCS,
		lpParms->CmsInfo.dst.DataType,
		lpParms->CmsInfo.dst.ptInfo.toRCS,
		lpParms->CmsInfo.dst.ptInfo.frRCS,
		(LPSTR)szOptions);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC ExpandWriteParms(LPEXPAND_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szParm1[20], szParm2[20];
char szParm3[20], szParm4[20], szColor[20];
COLORSPACE Space;

Space = GetColorInfoString( szColor, &lpParms->Color );
if (lpParms->fUseRightBottom)
	{
	wsprintf(lpString, lpFormat,
			FixedAscii(lpParms->Left, szParm1, FIXED_PREC),
			FixedAscii(lpParms->Top, szParm2, FIXED_PREC),
			FixedAscii(lpParms->Right, szParm3, FIXED_PREC),
			FixedAscii(lpParms->Bottom, szParm4, FIXED_PREC),
			Space,
			(LPSTR)szColor,
			lpParms->fUseRightBottom);
	}
else
	{
	wsprintf(lpString, lpFormat,
			FixedAscii(lpParms->Width, szParm1, FIXED_PREC),
			FixedAscii(lpParms->Height, szParm2, FIXED_PREC),
			FixedAscii(lpParms->Top, szParm3, FIXED_PREC),
			FixedAscii(lpParms->Left, szParm4, FIXED_PREC),
			Space,
			(LPSTR)szColor,
			lpParms->fUseRightBottom);
	}
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC ConvertWriteParms(LPCONVERT_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
EXTNAME szPalette;

szPalette[0] = '\0';
if (lpParms->cmsInfo.dst.DataType == FDT_PALETTECOLOR && lpParms->ConvertType == CT_CUSTOM)
	lstrcpy(szPalette, lpParms->szPalette);
wsprintf(lpString, lpFormat,
		lpParms->cmsInfo.src.DataType,
		lpParms->cmsInfo.src.ptInfo.toRCS,
		lpParms->cmsInfo.src.ptInfo.frRCS,
		lpParms->cmsInfo.dst.DataType,
		lpParms->cmsInfo.dst.ptInfo.toRCS,
		lpParms->cmsInfo.dst.ptInfo.frRCS,
		lpParms->DitherType,
		lpParms->ConvertType,
		lpParms->iLevels,
		(LPSTR)szPalette);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC GradientWriteParms(LPGRADIENT_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szStart[20], szEnd[20];
char szMM[MM_STR_LEN];
COLORSPACE StartSpace, EndSpace;

StartSpace = GetColorInfoString( szStart, &lpParms->StartColor );
EndSpace = GetColorInfoString( szEnd, &lpParms->EndColor );
wsprintf(lpString, lpFormat,
		TOTRANSPARENCY(lpParms->VigOpacity),
		MergeModeToString(lpParms->VigMergeMode, szMM),
		lpParms->iBaseRes,
		lpParms->x1,
		lpParms->y1,
		lpParms->x2,
		lpParms->y2,
		StartSpace,
		(LPSTR)szStart,
		EndSpace,
		(LPSTR)szEnd,
		lpParms->Gradient-IDC_VIGFIRST,
		lpParms->RepeatCount,
		lpParms->SoftTransition,
		lpParms->Midpoint,
		lpParms->VigColorModel);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC TintFillWriteParms(LPTINTFILL_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
COLORSPACE Space;
char szColor[20];
char szMM[MM_STR_LEN];

Space = GetColorInfoString( szColor, &lpParms->Color );
wsprintf(lpString, lpFormat,
		TOTRANSPARENCY(lpParms->FillOpacity),
		MergeModeToString(lpParms->FillMergeMode, szMM),
		Space,
		(LPSTR)szColor);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC TextureFillWriteParms(LPTEXTUREFILL_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szMM[MM_STR_LEN];

wsprintf(lpString, lpFormat,
		TOTRANSPARENCY(lpParms->TextureOpacity),
		MergeModeToString(lpParms->TextureMergeMode, szMM),
		(LPSTR)lpParms->TextureName,
		lpParms->fHorzFlip,
		lpParms->fVertFlip);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC FloodWriteParms(LPFLOOD_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
COLORSPACE Space, InSpace;
char szMM[MM_STR_LEN];
char szColor[20], szInColor[20];

Space = GetColorInfoString( szColor, &lpParms->Color );
InSpace = GetColorInfoString( szInColor, &lpParms->InColor );
wsprintf(lpString, lpFormat,
		TOTRANSPARENCY(lpParms->FloodOpacity),
		MergeModeToString(lpParms->FloodMergeMode, szMM),
		Space,
		(LPSTR)szColor,
		lpParms->iBaseRes,
		lpParms->x,
		lpParms->y,
		InSpace,
		(LPSTR)szInColor,
		lpParms->FloodRange,
		lpParms->FloodFade,
		lpParms->idFloodColorModel);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC ContBritWriteParms(LPCONTBRIT_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->Contrast,
		lpParms->Brightness[0],
		lpParms->Brightness[1],
		lpParms->Brightness[2],
		lpParms->Brightness[3]);

return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC BalanceWriteParms(LPBALANCE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
 		lpParms->MasterContrast,
		lpParms->MasterBrightness[0],
		lpParms->MasterBrightness[1],
		lpParms->MasterBrightness[2],
		lpParms->MasterBrightness[3],
		lpParms->fMasterReverse,
 		lpParms->RedContrast,
		lpParms->RedBrightness[0],
		lpParms->RedBrightness[1],
		lpParms->RedBrightness[2],
		lpParms->RedBrightness[3],
		lpParms->fRedReverse,
 		lpParms->GreenContrast,
		lpParms->GreenBrightness[0],
		lpParms->GreenBrightness[1],
		lpParms->GreenBrightness[2],
		lpParms->GreenBrightness[3],
		lpParms->fGreenReverse,
 		lpParms->BlueContrast,
		lpParms->BlueBrightness[0],
		lpParms->BlueBrightness[1],
		lpParms->BlueBrightness[2],
		lpParms->BlueBrightness[3],
		lpParms->fBlueReverse,
 		lpParms->BlackContrast,
		lpParms->BlackBrightness[0],
		lpParms->BlackBrightness[1],
		lpParms->BlackBrightness[2],
		lpParms->BlackBrightness[3],
		lpParms->fBlackReverse);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC ShapeMaskWriteParms(LPSHAPEMASK_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->Shape,
		lpParms->Mode,
		lpParms->iBaseRes,
		lpParms->x1,
		lpParms->y1,
		lpParms->x2,
		lpParms->y2);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC FreeMaskWriteParms(LPFREEMASK_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->Mode,
		lpParms->Antialias,
 		lpParms->iBaseRes,
		lpParms->nPoints);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (!WriteIntPointList(fh, lpParms->lpPoints, lpParms->nPoints))
	return(WF_ERROR);
return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC PointEditWriteParms(LPPOINTEDIT_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->Mode,
		lpParms->Antialias,
 		lpParms->iBaseRes,
		lpParms->rErase.left,
		lpParms->rErase.top,
		lpParms->rErase.right,
		lpParms->rErase.bottom,
		lpParms->iEraseVal,
		lpParms->nPoints);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (!WriteIntPointList(fh, lpParms->lpPoints, lpParms->nPoints))
	return(WF_ERROR);
return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC MagicMaskWriteParms(LPMAGICMASK_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szColor[20];
COLORSPACE Space;

Space = GetColorInfoString( szColor, &lpParms->Color);

wsprintf(lpString, lpFormat,
		lpParms->iBaseRes,
		lpParms->x,
		lpParms->y,
		Space,
		(LPSTR)szColor,
		lpParms->Mode,
		lpParms->WandRange,
		lpParms->WandFade,
		lpParms->idMagicColorModel);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC SimilarWriteParms(LPSIMILAR_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szColor[20];
COLORSPACE Space;

Space = GetColorInfoString( szColor, &lpParms->Color);

wsprintf(lpString, lpFormat,
		lpParms->iBaseRes,
		lpParms->x,
		lpParms->y,
		Space,
		(LPSTR)szColor,
		lpParms->Mode,
		lpParms->WandRange,
		lpParms->WandFade,
		lpParms->idMagicColorModel);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC TransformerWriteParms(LPTRANSFORMER_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szMM[MM_STR_LEN];

wsprintf(lpString, lpFormat,
 		lpParms->iBaseRes,
		lpParms->SelectRect.left,
		lpParms->SelectRect.top,
		lpParms->SelectRect.right,
		lpParms->SelectRect.bottom,
		lpParms->Function - IDC_TRANSFUNCTIONFIRST,
		lpParms->Mode,
		TOTRANSPARENCY(lpParms->TransformOpacity),
		MergeModeToString(lpParms->TransformMergeMode, szMM),
		lpParms->fHiQuality,
		lpParms->fPasteInto,
		lpParms->fDelete,
		lpParms->DistortPoints[0].x,
		lpParms->DistortPoints[0].y,
		lpParms->DistortPoints[1].x,
		lpParms->DistortPoints[1].y,
		lpParms->DistortPoints[2].x,
		lpParms->DistortPoints[2].y,
		lpParms->DistortPoints[3].x,
		lpParms->DistortPoints[3].y);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC TransformObjectsWriteParms(LPTRANSFORMOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{

wsprintf(lpString, lpFormat,
		lpParms->fHiQuality,
 		lpParms->iBaseRes,
		lpParms->DistortPoints[0].x,
		lpParms->DistortPoints[0].y,
		lpParms->DistortPoints[1].x,
		lpParms->DistortPoints[1].y,
		lpParms->DistortPoints[2].x,
		lpParms->DistortPoints[2].y,
		lpParms->DistortPoints[3].x,
		lpParms->DistortPoints[3].y);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC SelectObjectsWriteParms(LPSELECTOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->iBaseRes,
		lpParms->SelectRect.left,
		lpParms->SelectRect.top,
		lpParms->SelectRect.right,
		lpParms->SelectRect.bottom,
		lpParms->fExtendedSel,
		lpParms->fSelectAll);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC MoveObjectsWriteParms(LPMOVEOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
 		lpParms->iBaseRes,
		lpParms->x,
		lpParms->y);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC SetOpacityWriteParms(LPSETATTROBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		TOTRANSPARENCY(lpParms->Opacity));
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC LayerObjectsWriteParms(LPLAYEROBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->fToTop,
		lpParms->fOneLevel);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC PasteFromWriteParms(LPPASTEFROM_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szMM[MM_STR_LEN];

wsprintf(lpString, lpFormat,
		TOTRANSPARENCY(lpParms->TransformOpacity),
		MergeModeToString(lpParms->TransformMergeMode, szMM),
		lpParms->fHiQuality,
		lpParms->fPasteInto,
 		lpParms->iBaseRes,
		lpParms->DistortPoints[0].x,
		lpParms->DistortPoints[0].y,
		lpParms->DistortPoints[1].x,
		lpParms->DistortPoints[1].y,
		lpParms->DistortPoints[2].x,
		lpParms->DistortPoints[2].y,
		lpParms->DistortPoints[3].x,
		lpParms->DistortPoints[3].y,
		(LPSTR)lpParms->szClipboard,
		(LPSTR)lpParms->szClipFileName,
		lpParms->cmsInfo.src.DataType,
		lpParms->cmsInfo.src.ptInfo.toRCS,
		lpParms->cmsInfo.src.ptInfo.frRCS,
		lpParms->cmsInfo.dst.DataType,
		lpParms->cmsInfo.dst.ptInfo.toRCS,
		lpParms->cmsInfo.dst.ptInfo.frRCS);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC PasteWriteParms(LPPASTE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szMM[MM_STR_LEN];

wsprintf(lpString, lpFormat,
		TOTRANSPARENCY(lpParms->TransformOpacity),
		MergeModeToString(lpParms->TransformMergeMode, szMM),
		lpParms->fHiQuality,
		lpParms->fPasteInto,
 		lpParms->iBaseRes,
		lpParms->DistortPoints[0].x,
		lpParms->DistortPoints[0].y,
		lpParms->DistortPoints[1].x,
		lpParms->DistortPoints[1].y,
		lpParms->DistortPoints[2].x,
		lpParms->DistortPoints[2].y,
		lpParms->DistortPoints[3].x,
		lpParms->DistortPoints[3].y);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC LoadMaskWriteParms(LPLOADMASK_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->Mode,
		lpParms->fHiQuality,
 		lpParms->iBaseRes,
		lpParms->DistortPoints[0].x,
		lpParms->DistortPoints[0].y,
		lpParms->DistortPoints[1].x,
		lpParms->DistortPoints[1].y,
		lpParms->DistortPoints[2].x,
		lpParms->DistortPoints[2].y,
		lpParms->DistortPoints[3].x,
		lpParms->DistortPoints[3].y,
		(LPSTR)lpParms->szMask,
		lpParms->fDeleteMask,
		(LPSTR)lpParms->szMaskFileName);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC EffectWriteParms(LPEFFECT_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
LPSTR lpData;

if (lpParms->lpString)
	lpData = lpParms->lpString;
else
	lpData = "";
	
wsprintf(lpString, lpFormat,
		lpParms->szEffect,
		lpData,
		lpParms->bInOut);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC FeatherAlphaWriteParms(LPFEATHERALPHA_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->inside,
 		lpParms->iBaseRes,
		lpParms->size,
		lpParms->edge);
		
return(WF_WRITE);
}


/***********************************************************************/
WRITE_FUNC MaskSmoothWriteParms(LPMASK_SMOOTH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
 		lpParms->iBaseRes,
		lpParms->gauss,
		lpParms->thresh);
		
return(WF_WRITE);
}



/***********************************************************************/
WRITE_FUNC TextWriteParms(LPTEXT_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
COLORSPACE Space;
char szColor[20];
char szMM[MM_STR_LEN];
LPSTR lpText, lpStart;
int	lines;

Space = GetColorInfoString( szColor, &lpParms->Color );

if (lpParms->lpTextBuf)
	{
	lpStart = lpParms->lpTextBuf;
	lpText = lpStart;
	lines = 0;
	while (*lpText)
		{
		if (*lpText == VK_RETURN)
			{
			++lines;
			lpText = AnsiNext(lpText);
			lpStart = lpText;
			}
		else
			lpText = AnsiNext(lpText);
		}
	if (lpText > lpStart)
		++lines;
	}
else
	lines = 0;
wsprintf(lpString, lpFormat,
		(LPSTR)lpParms->szFaceName,
		lpParms->Size,
		lpParms->Italic,
		lpParms->Underline,
		lpParms->Strikeout,
		lpParms->Weight,
		lpParms->Angle,
		lpParms->AntiAlias,
		lpParms->Justification,
 		lpParms->iBaseRes,
		lpParms->StartPos.x,
		lpParms->StartPos.y,
		Space,
		(LPSTR)szColor,
		TOTRANSPARENCY(lpParms->Opacity),
		MergeModeToString(lpParms->MergeMode, szMM),
		lines,
		lpParms->DestPos.x,
		lpParms->DestPos.y);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);
if (lpParms->lpTextBuf)
	{
	lpStart = lpParms->lpTextBuf;
	lpText = lpStart;
	while (*lpText)
		{
		if (*lpText == VK_RETURN)
			{
			*lpText = '\0';
			lstrcpy(lpString, lpStart);
			*lpText = VK_RETURN;
			if (!WriteString(fh, lpString))
				return(WF_ERROR);
			lpText = AnsiNext(lpText);
			lpStart = lpText;
			}
		else
			lpText = AnsiNext(lpText);
		}
	if (lpText > lpStart)
		{
		lstrcpy(lpString, lpStart);
		if (!WriteString(fh, lpString))
			return(WF_ERROR);
		}
	}
return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC ActivateWindowWriteParms(LPACTIVATEWINDOW_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		(LPSTR)lpParms->szFileName,
		lpParms->iOffset);
		
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC NewImageWriteParms(LPNEW_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szParm1[20], szParm2[20], szColor[20];
COLORSPACE Space;

Space = GetColorInfoString( szColor, &lpParms->Color );
wsprintf(lpString, lpFormat,
		(int)lpParms->Type,
		FixedAscii(lpParms->Width, szParm1, FIXED_PREC),
		FixedAscii(lpParms->Height, szParm2, FIXED_PREC),
		lpParms->Resolution,
		Space,
		(LPSTR)szColor,
		lpParms->CmsInfo.src.DataType,
		lpParms->CmsInfo.src.ptInfo.toRCS,
		lpParms->CmsInfo.src.ptInfo.frRCS,
		lpParms->CmsInfo.dst.DataType,
		lpParms->CmsInfo.dst.ptInfo.toRCS,
		lpParms->CmsInfo.dst.ptInfo.frRCS);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC MapWriteParms(LPMODIFYMAP_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->MasterInfo.Points,
		lpParms->RedInfo.Points,
		lpParms->GreenInfo.Points,
		lpParms->BlueInfo.Points,
		lpParms->BlackInfo.Points);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (MapWriteInfo(&lpParms->MasterInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);
if (MapWriteInfo(&lpParms->RedInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);
if (MapWriteInfo(&lpParms->GreenInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);
if (MapWriteInfo(&lpParms->BlueInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);
if (MapWriteInfo(&lpParms->BlackInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC ToneWriteParms(LPTONE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->MasterInfo.Points,
		lpParms->RedInfo.Points,
		lpParms->GreenInfo.Points,
		lpParms->BlueInfo.Points,
		lpParms->BlackInfo.Points);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (MapWriteInfo(&lpParms->MasterInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);
if (MapWriteInfo(&lpParms->RedInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);
if (MapWriteInfo(&lpParms->GreenInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);
if (MapWriteInfo(&lpParms->BlueInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);
if (MapWriteInfo(&lpParms->BlackInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC PosterizeWriteParms(LPPOSTERIZE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->MasterInfo.Levels,
		lpParms->MasterInfo.Threshold,
		lpParms->RedInfo.Levels,
		lpParms->RedInfo.Threshold,
		lpParms->GreenInfo.Levels,
		lpParms->GreenInfo.Threshold,
		lpParms->BlueInfo.Levels,
		lpParms->BlueInfo.Threshold,
		lpParms->BlackInfo.Levels,
		lpParms->BlackInfo.Threshold);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC HueWriteParms(LPHUE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->HueInfo.Points,
		lpParms->Saturation,
		lpParms->Brightness);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (MapWriteInfo(&lpParms->HueInfo, lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC ApplyCalWriteParms(LPAPPLYCAL_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		(LPSTR)lpParms->szScanMap,
		(LPSTR)lpParms->szPrintMap);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC CopyToFileWriteParms(LPCOPYTOFILE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->szName,
		lpParms->Type);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC SaveMaskWriteParms(LPSAVEMASK_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->szName);
return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC PaintWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szColor[20];
char szMM[MM_STR_LEN];
COLORSPACE Space;

Space = GetColorInfoString( szColor, &lpParms->Color );
wsprintf(lpString, lpFormat,
	(LPSTR)lpParms->StyleName,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
	TOTRANSPARENCY(lpParms->Opacity),
 	MergeModeToString(lpParms->MergeMode, szMM),
	Space,
	(LPSTR)szColor);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, NO,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC SprayWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szColor[20];
char szMM[MM_STR_LEN];
COLORSPACE Space;

Space = GetColorInfoString( szColor, &lpParms->Color );
wsprintf(lpString, lpFormat,
	(LPSTR)lpParms->StyleName,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
	TOTRANSPARENCY(lpParms->Opacity),
 	MergeModeToString(lpParms->MergeMode, szMM),
	Space,
	(LPSTR)szColor);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, NO,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC CloneWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szMM[MM_STR_LEN];

wsprintf(lpString, lpFormat,
	(LPSTR)lpParms->StyleName,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
	lpParms->iSrcRes,
	TOTRANSPARENCY(lpParms->Opacity),
 	MergeModeToString(lpParms->MergeMode, szMM),
	(LPSTR)lpParms->szSourceImage,
	lpParms->ptSource.x,
	lpParms->ptSource.y);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, YES,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC TextureWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szMM[MM_STR_LEN];

wsprintf(lpString, lpFormat,
	(LPSTR)lpParms->StyleName,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
//	lpParms->iSrcRes,
	TOTRANSPARENCY(lpParms->Opacity),
 	MergeModeToString(lpParms->MergeMode, szMM));

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, NO /* YES */,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC SmearWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szMM[MM_STR_LEN];

wsprintf(lpString, lpFormat,
	(LPSTR)lpParms->StyleName,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
	TOTRANSPARENCY(lpParms->Opacity),
 	MergeModeToString(lpParms->MergeMode, szMM));

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, NO,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC EraserWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szColor[20];
char szMM[MM_STR_LEN];
COLORSPACE Space;

Space = GetColorInfoString( szColor, &lpParms->Color );
wsprintf(lpString, lpFormat,
	(LPSTR)lpParms->StyleName,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
	TOTRANSPARENCY(lpParms->Opacity),
 	MergeModeToString(lpParms->MergeMode, szMM),
	Space,
	(LPSTR)szColor);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, NO,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC AltEraserWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
	TOTRANSPARENCY(lpParms->Opacity));

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, NO,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC FilterWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
	(LPSTR)lpParms->StyleName,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
	lpParms->Pressure);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, NO,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC PaintOnWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->nPoints,
	lpParms->fUsePressure,
	lpParms->Size,
	lpParms->Feather,
	lpParms->iDstRes,
	TOTRANSPARENCY(lpParms->Opacity),
	lpParms->Mode,
	lpParms->fPaintOnObject);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (RetouchWriteInfo(lpParms->hpStrokeInfo, lpParms->nPoints,
					lpParms->fUsePressure, NO,
					lpString, fh) == WF_ERROR)
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC StrokeMaskWriteParms(LPSTROKEMASK_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szColor[20];
char szMM[MM_STR_LEN];
COLORSPACE Space;

Space = GetColorInfoString( szColor, &lpParms->Color );
wsprintf(lpString, lpFormat,
	lpParms->idBrush-IDC_STROKEFIRST,
	(LPSTR)lpParms->StyleName,
	lpParms->BrushShape-IDC_FIRST_BRUSHSHAPE,
	(LPTR)lpParms->ShapeName,
	lpParms->iBaseRes,
	lpParms->Size,
	lpParms->Feather,
	TOTRANSPARENCY(lpParms->Opacity),
	lpParms->Pressure,
 	MergeModeToString(lpParms->MergeMode, szMM),
	Space,
	(LPSTR)szColor);

return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC DrawShapeWriteParms(LPDRAWSHAPE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szBorderColor[20], szFillColor[20];
char szMM[MM_STR_LEN];
COLORSPACE BorderSpace, FillSpace;

BorderSpace = GetColorInfoString( szBorderColor, &lpParms->BorderColor );
FillSpace = GetColorInfoString( szFillColor, &lpParms->FillColor );
wsprintf(lpString, lpFormat,
		lpParms->ShapeType,
		lpParms->FillStyle,
 		lpParms->iBaseRes,
		lpParms->x1,
		lpParms->y1,
		lpParms->x2,
		lpParms->y2,
		(LPSTR)lpParms->Brush.StyleName,
		lpParms->Brush.BrushShape-IDC_FIRST_BRUSHSHAPE,
		(LPSTR)lpParms->Brush.ShapeName,
		lpParms->Brush.Size,
		lpParms->Brush.Feather,
		TOTRANSPARENCY(lpParms->Brush.Opacity),
	 	MergeModeToString(lpParms->Brush.MergeMode, szMM),
		BorderSpace,
		(LPSTR)szBorderColor,
		FillSpace,
		(LPSTR)szFillColor);

return(WF_WRITE);
}

/***********************************************************************/
WRITE_FUNC DrawFreeWriteParms(LPDRAWFREE_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szBorderColor[20], szFillColor[20];
char szMM[MM_STR_LEN];
COLORSPACE BorderSpace, FillSpace;

BorderSpace = GetColorInfoString( szBorderColor, &lpParms->BorderColor );
FillSpace = GetColorInfoString( szFillColor, &lpParms->FillColor );
wsprintf(lpString, lpFormat,
		lpParms->FillStyle,
		(LPSTR)lpParms->Brush.StyleName,
		lpParms->Brush.BrushShape-IDC_FIRST_BRUSHSHAPE,
		(LPSTR)lpParms->Brush.ShapeName,
		lpParms->Brush.Size,
		lpParms->Brush.Feather,
		TOTRANSPARENCY(lpParms->Brush.Opacity),
	 	MergeModeToString(lpParms->Brush.MergeMode, szMM),
		BorderSpace,
		(LPSTR)szBorderColor,
		FillSpace,
		(LPSTR)szFillColor,
 		lpParms->iBaseRes,
		lpParms->nPoints);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (!WriteIntPointList(fh, lpParms->lpPoints, lpParms->nPoints))
	return(WF_ERROR);

return(WF_DONE);
}

/***********************************************************************/
WRITE_FUNC PaletteEditWriteParms(LPPALETTEEDIT_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->fReMap,
		lpParms->DitherType,
		lpParms->lpColorMap->NumEntries);

// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (!WriteColorMap(fh, lpParms->lpColorMap))
	return(WF_ERROR);

return(WF_DONE);
}

/************************************************************************/
WRITE_FUNC AlignObjWriteParms(LPALIGNOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPSTR lpFormat, LPSTR lpString)
/************************************************************************/
{
wsprintf( lpString, lpFormat, lpParms->HorzAlign, lpParms->VertAlign,
	lpParms->RectAlign, lpParms->bPreview );
return( WF_WRITE );
}

/***********************************************************************/
WRITE_FUNC SetMergeModeWriteParms(LPSETATTROBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
char szMM[MM_STR_LEN];
wsprintf(lpString, lpFormat,
	 	MergeModeToString(lpParms->MergeMode, szMM));
return(WF_WRITE);
}

/************************************************************************/
WRITE_FUNC PosObjWriteParms(LPPOSOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPSTR lpFormat, LPSTR lpString)
/************************************************************************/
{
wsprintf( lpString, lpFormat, 
 		lpParms->iBaseRes,
	     lpParms->XOffset,
          lpParms->YOffset);
return( WF_WRITE );
}

/************************************************************************/
WRITE_FUNC PSFilterWriteParms(LPPSFILTER_PARMS lpParms, MACRO_FILE_HANDLE fh, LPSTR lpFormat, LPSTR lpString)
/************************************************************************/
{
DWORD dwSize = 0;

if (lpParms->lpParameters)
	{
	if (lpParms->fHandle == 2)
		{
		dwSize = PS_GetHandleSize(lpParms->lpParameters);
		}
	else // windows handle or windows pointer
		{
		HGLOBAL hMem;

		if (lpParms->fHandle) // Windows handle
			hMem = (HGLOBAL)LOWORD((long)lpParms->lpParameters);
		else // Windows pointer
			hMem = GlobalPtrHandle(lpParms->lpParameters);
		dwSize = GlobalSize(hMem);
		}
	}
wsprintf( lpString, lpFormat,
		(LPSTR)lpParms->szFilter,
		lpParms->fHandle,
		dwSize);
// if no file handle passed in, just setup command string
if (fh == MACRO_FILE_HANDLE_INVALID)
	return(WF_WRITE);

if (!WriteString(fh, lpString))
	return(WF_ERROR);

if (dwSize)
	{
	LPTR lp;
	DWORD dwBytes;
	HGLOBAL hMem;

	if (lpParms->fHandle == 2) // PhotoShop handle
		lp = *((LPPTR)lpParms->lpParameters);
	else
	if (lpParms->fHandle) // Windows handle
		{
		hMem = (HGLOBAL)LOWORD(lpParms->lpParameters);
		lp = (LPTR)GlobalLock(hMem);
		}
	else // Windows pointer
		lp = lpParms->lpParameters;

	while (dwSize)
		{
		dwBytes = 40;
		if (dwSize < dwBytes)
			dwBytes = dwSize;
		BytesToHex(lp, lpString, dwBytes);
		if (!WriteString(fh, lpString))
			return(WF_ERROR);
		dwSize -= dwBytes;
		lp += dwBytes;
		}
	if (lpParms->fHandle == 1) // Windows handle
		GlobalUnlock(hMem);
	}

return( WF_DONE );
}

/************************************************************************/
WRITE_FUNC CropWriteParms(LPCROP_PARMS lpParms, MACRO_FILE_HANDLE fh, LPSTR lpFormat, LPSTR lpString)
/************************************************************************/
{
wsprintf(lpString, lpFormat,
 		lpParms->iBaseRes,
		lpParms->CropRect.left,
		lpParms->CropRect.top,
		lpParms->CropRect.right,
		lpParms->CropRect.bottom);

return(WF_WRITE);
}

/************************************************************************/
WRITE_FUNC DiscardUndoWriteParms(LPDISCARDUNDO_PARMS lpParms, MACRO_FILE_HANDLE fh, LPSTR lpFormat, LPSTR lpString)
/************************************************************************/
{
wsprintf(lpString, lpFormat,
		lpParms->fDiscardUndo);

return(WF_WRITE);
}

/***********************************************************************/
LOCAL BOOL IsEOL(char c)
/***********************************************************************/
{
return(c == '\r' || c == '\n');
}


/***********************************************************************/
LOCAL BOOL MapReadInfo(LPMAPINFO lpInfo, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
int i;
char szTemp[MAX_CMD_LEN];
LFIXED lf;

if (!ReadLine(fh, szTemp, sizeof(szTemp)))
	return(FALSE);
// read info 
lpInfo->iCurves = GetIntParm(szTemp, 0);
lpInfo->Reverse = GetIntParm(szTemp, 1);

lf = GetFixedParm(szTemp, 2);
lpInfo->gamma = DOUBLE(lf);
for (i = 0; i < lpInfo->Points; ++i)
	lpInfo->Pnt[i].x = GetIntParm(szTemp, i+3);
if (!ReadLine(fh, szTemp, sizeof(szTemp)))
	return(FALSE);
for (i = 0; i < lpInfo->Points; ++i)
	lpInfo->Pnt[i].y = GetIntParm(szTemp, i);
return(TRUE);
}

/***********************************************************************/
LOCAL LPRETOUCH_PARMS RetouchGetCommonParms(LPSTR lpCommand, MACRO_FILE_HANDLE fh, BOOL fReadSource, LPINT lpNextIndex)
/***********************************************************************/
{
LPRETOUCH_PARMS lpParms;
int index;

if (!(lpParms = (LPRETOUCH_PARMS)Alloc((long)sizeof(RETOUCH_PARMS))))
	return(NULL);
index = 1;
GetStringParm(lpCommand, index++, lpParms->StyleName);
lpParms->BrushShape = GetIntParm(lpCommand, index++);
lpParms->BrushShape += IDC_FIRST_BRUSHSHAPE;
GetStringParm(lpCommand, index++, lpParms->ShapeName);
lpParms->nPoints = GetIntParm(lpCommand, index++);
lpParms->fUsePressure = GetIntParm(lpCommand, index++);
lpParms->Size = GetIntParm(lpCommand, index++);
lpParms->Feather = GetIntParm(lpCommand, index++);
lpParms->iDstRes = GetIntParm(lpCommand, index++);
if (fReadSource)
	lpParms->iSrcRes = GetIntParm(lpCommand, index++);

lpParms->hpStrokeInfo = ReadStrokeInfo(fh, lpParms->nPoints,
							lpParms->fUsePressure, fReadSource);
if (!lpParms->hpStrokeInfo)
	{
	FreeUp(lpParms);
	return(NULL);
	}
*lpNextIndex = index;
return(lpParms);
}

/***********************************************************************/
LOCAL HPSTROKEINFO ReadStrokeInfo(MACRO_FILE_HANDLE fh, int nPoints, BOOL fReadPressure, BOOL fReadSource)
/***********************************************************************/
{
STRING szString;
HPSTROKEINFO hpStrokeInfo;
int i;
DWORD dwStrokeInfoSize;

if (fReadSource)
	{
	if (fReadPressure)
		dwStrokeInfoSize = sizeof(STROKEINFO4);
	else
		dwStrokeInfoSize = sizeof(STROKEINFO3);
	}
else
	{
	if (fReadPressure)
		dwStrokeInfoSize = sizeof(STROKEINFO2);
	else
		dwStrokeInfoSize = sizeof(STROKEINFO1);
	}

hpStrokeInfo = Alloc((long)nPoints*dwStrokeInfoSize);
if (!hpStrokeInfo)
	return(NULL);
for (i = 0; i < nPoints; ++i)
	{
	if (!ReadLine(fh, szString, sizeof(szString)))
		return(NULL);
	if (fReadSource)
		{
		if (fReadPressure)
			{
			HPSTROKEINFO4 hpStrokePtr = (HPSTROKEINFO4)hpStrokeInfo;

			hpStrokePtr[i].dx = GetIntParm(szString, 0);
			hpStrokePtr[i].dy = GetIntParm(szString, 1);
			hpStrokePtr[i].sx = GetIntParm(szString, 2);
			hpStrokePtr[i].sy = GetIntParm(szString, 3);
			hpStrokePtr[i].Pressure = GetIntParm(szString, 4);
			}
		else
			{
			HPSTROKEINFO3 hpStrokePtr = (HPSTROKEINFO3)hpStrokeInfo;

			hpStrokePtr[i].dx = GetIntParm(szString, 0);
			hpStrokePtr[i].dy = GetIntParm(szString, 1);
			hpStrokePtr[i].sx = GetIntParm(szString, 2);
			hpStrokePtr[i].sy = GetIntParm(szString, 3);
			}
		}
	else
		{
		if (fReadPressure)
			{
			HPSTROKEINFO2 hpStrokePtr = (HPSTROKEINFO2)hpStrokeInfo;

			hpStrokePtr[i].dx = GetIntParm(szString, 0);
			hpStrokePtr[i].dy = GetIntParm(szString, 1);
			hpStrokePtr[i].Pressure = GetIntParm(szString, 4);
			}
		else
			{
			HPSTROKEINFO1 hpStrokePtr = (HPSTROKEINFO1)hpStrokeInfo;

			hpStrokePtr[i].dx = GetIntParm(szString, 0);
			hpStrokePtr[i].dy = GetIntParm(szString, 1);
			}
		}
	}
return(hpStrokeInfo);
}

/***********************************************************************/
LOCAL LPDRAWFREE_PARMS DrawFreeReadParms(LPSTR lpCommand, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
LPDRAWFREE_PARMS lpParms;
COLORSPACE Space;
STRING szTemp;

if (!(lpParms = (LPDRAWFREE_PARMS)Alloc((long)sizeof(DRAWFREE_PARMS))))
	return(NULL);
lpParms->FillStyle = (FILL_STYLE)GetIntParm(lpCommand, 1);

GetDrawBrushParms(lpCommand, &lpParms->Brush, 2);

Space = (COLORSPACE)GetIntParm(lpCommand, 9);
GetStringParm(lpCommand, 10, szTemp);
SetColorInfoString(szTemp, &lpParms->BorderColor, Space);
SetColorInfo( &lpParms->BorderColor, &lpParms->BorderColor, Space );

Space = (COLORSPACE)GetIntParm(lpCommand, 11);
GetStringParm(lpCommand, 12, szTemp);
SetColorInfoString(szTemp, &lpParms->FillColor, Space);
SetColorInfo( &lpParms->FillColor, &lpParms->FillColor, Space );

lpParms->iBaseRes = GetIntParm(lpCommand, 13);
lpParms->nPoints = GetIntParm(lpCommand, 14);

lpParms->lpPoints = ReadIntPointList(fh, lpParms->nPoints);
if (!lpParms->lpPoints)
	{
	FreeUp((LPVOID)lpParms);
	return(NULL);
	}
return(lpParms);
}

/***********************************************************************/
LOCAL void GetDrawBrushParms(LPSTR lpCommand, LPDRAWBRUSH lpBrush, int index)
/***********************************************************************/
{
GetStringParm(lpCommand, index++, lpBrush->StyleName);
lpBrush->BrushShape = GetIntParm(lpCommand, index++);
lpBrush->BrushShape += IDC_FIRST_BRUSHSHAPE;
GetStringParm(lpCommand, index++, lpBrush->ShapeName);
lpBrush->Size = GetIntParm(lpCommand, index++);
lpBrush->Feather = GetIntParm(lpCommand, index++);
lpBrush->Opacity = TOOPACITY(GetIntParm(lpCommand, index++));
lpBrush->MergeMode = (MERGE_MODE)GetIntParm(lpCommand, index++);
}

/***********************************************************************/
LOCAL LPFPOINT ReadFixedPointList(MACRO_FILE_HANDLE fh, int nPoints)
/***********************************************************************/
{
LPFPOINT lpPoints;
STRING szString;
int size, i;

lpPoints = (LPFPOINT)Alloc(nPoints*sizeof(FPOINT));
if (!lpPoints)
	return(NULL);
size = sizeof(szString);
for (i = 0; i < nPoints; ++i)
	{
	if (!ReadLine(fh, szString, size))
		{
		FreeUp((LPTR)lpPoints);
		return(NULL);
		}
	lpPoints[i].fx = GetFixedParm(szString, 0);
	lpPoints[i].fy = GetFixedParm(szString, 1);
	}
return(lpPoints);
}

/***********************************************************************/
LOCAL BOOL WriteFixedPointList(MACRO_FILE_HANDLE fh, LPFPOINT lpPoints, int nPoints)
/***********************************************************************/
{
int i;
STRING szString;
char szParm1[20], szParm2[20];

for (i = 0; i < nPoints; ++i)
	{
	wsprintf(szString, "%s %s",
		FixedAscii(lpPoints[i].fx, szParm1, FIXED_PREC),
		FixedAscii(lpPoints[i].fy, szParm2, FIXED_PREC));
	if (!WriteString(fh, szString))
		return(FALSE);
	}
return(TRUE);
}

/***********************************************************************/
LOCAL LPPOINT ReadIntPointList(MACRO_FILE_HANDLE fh, int nPoints)
/***********************************************************************/
{
LPPOINT lpPoints;
STRING szString;
int size, i;

lpPoints = (LPPOINT)Alloc(nPoints*sizeof(POINT));
if (!lpPoints)
	return(NULL);
size = sizeof(szString);
for (i = 0; i < nPoints; ++i)
	{
	if (!ReadLine(fh, szString, size))
		{
		FreeUp((LPTR)lpPoints);
		return(NULL);
		}
	lpPoints[i].x = GetIntParm(szString, 0);
	lpPoints[i].y = GetIntParm(szString, 1);
	}
return(lpPoints);
}

/***********************************************************************/
LOCAL BOOL WriteIntPointList(MACRO_FILE_HANDLE fh, LPPOINT lpPoints, int nPoints)
/***********************************************************************/
{
int i;
STRING szString;

for (i = 0; i < nPoints; ++i)
	{
	wsprintf(szString, "%d %d",
		lpPoints[i].x,
		lpPoints[i].y);
	if (!WriteString(fh, szString))
		return(FALSE);
	}
return(TRUE);
}

/***********************************************************************/
LOCAL WRITE_FUNC MapWriteInfo(LPMAPINFO lpInfo, LPSTR lpString, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
char temp[10];
int i;
char szParm1[20];

*lpString = '\0';
// write info 
wsprintf(lpString, "%3d %3d %s ",
	lpInfo->iCurves,
	lpInfo->Reverse,
	FixedAscii(DBL2FIX(lpInfo->gamma), szParm1, FIXED_PREC));

// write points
for (i = 0; i < lpInfo->Points; ++i)
	{
	wsprintf(temp, "%3d ", (int)lpInfo->Pnt[i].x);
	lstrcat(lpString, temp);
	}
if (!WriteString(fh, lpString))
	return(WF_ERROR);
*lpString = '\0';
for (i = 0; i < lpInfo->Points; ++i)
	{
	wsprintf(temp, "%3d ", (int)lpInfo->Pnt[i].y);
	lstrcat(lpString, temp);
	}
if (!WriteString(fh, lpString))
	return(WF_ERROR);
return(WF_DONE);
}

/***********************************************************************/
LOCAL BOOL RetouchWriteInfo(HPSTROKEINFO hpStrokeInfo, int nPoints,
						  	 BOOL fUsePressure, BOOL fWriteSource,
							 LPSTR lpString, MACRO_FILE_HANDLE fh)
/***********************************************************************/
{
int i;

for (i = 0; i < nPoints; ++i)
	{
	if (fUsePressure && fWriteSource)
		{
		HPSTROKEINFO4 hpStrokePtr = (HPSTROKEINFO4)hpStrokeInfo;
		wsprintf(lpString, "%d %d %d %d %d",
				hpStrokePtr[i].dx,
				hpStrokePtr[i].dy,
				hpStrokePtr[i].sx,
				hpStrokePtr[i].sy,
				(int)hpStrokePtr[i].Pressure);
		}
	else
	if (fUsePressure)
		{
		HPSTROKEINFO2 hpStrokePtr = (HPSTROKEINFO2)hpStrokeInfo;
		wsprintf(lpString, "%d %d %d",
				hpStrokePtr[i].dx,
				hpStrokePtr[i].dy,
				(int)hpStrokePtr[i].Pressure);
		}
	else
	if (fWriteSource)
		{
		HPSTROKEINFO3 hpStrokePtr = (HPSTROKEINFO3)hpStrokeInfo;
		wsprintf(lpString, "%d %d %d %d",
				hpStrokePtr[i].dx,
				hpStrokePtr[i].dy,
				hpStrokePtr[i].sx,
				hpStrokePtr[i].sy);
		}
	else
		{
		HPSTROKEINFO1 hpStrokePtr = (HPSTROKEINFO1)hpStrokeInfo;
		wsprintf(lpString, "%d %d",
				hpStrokePtr[i].dx,
				hpStrokePtr[i].dy);
		}

	if (!WriteString(fh, lpString))
		return(FALSE);
	}
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL ReadColorMap(MACRO_FILE_HANDLE fh, LPCOLORMAP lpColorMap)
/***********************************************************************/
{
STRING szString;
int i, size;
COLORINFO Color;

size = sizeof(szString);
for (i = 0; i < lpColorMap->NumEntries; ++i)
	{
	if (!ReadLine(fh, szString, size))
		return(FALSE);
	SetColorInfoString(szString, &Color, CS_RGB);
	lpColorMap->RGBData[i] = Color.rgb;
	}
return(TRUE);
}

/***********************************************************************/
LOCAL BOOL WriteColorMap(MACRO_FILE_HANDLE fh, LPCOLORMAP lpColorMap)
/***********************************************************************/
{
STRING szString;
int i;
COLORINFO Color;

Color.ColorSpace = CS_RGB;
for (i = 0; i < lpColorMap->NumEntries; ++i)
	{
	Color.rgb = lpColorMap->RGBData[i];
	GetColorInfoString( szString, &Color );
	if (!WriteString(fh, szString))
		return(FALSE);
	}
return(TRUE);
}

/***********************************************************************/
WRITE_FUNC SaveToAlbumWriteParms(LPSAVETOALBUM_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
/***********************************************************************/
{
STRING szOptions;
int i;
ITEMID idFileType = 0, idDataType = 0;

if (lpParms->idFileType)
	idFileType = lpParms->idFileType-IDN_FIRSTFILETYPE+1;
if (lpParms->idDataType)
	idDataType = lpParms->idDataType-IDC_FIRSTDATATYPE+1;

switch (lpParms->idFileType)
	{
	case IDN_PP4:
	case IDN_PP:
    case IDN_PPCOMPOUND:
		wsprintf(szOptions, "%d %d %d %d %d %d",
				lpParms->PPFFOptions.PPFFCompress,
				lpParms->PPFFOptions.bSaveMask,
				lpParms->PPFFOptions.bSaveProxy,
				lpParms->PPFFOptions.bSaveCmdList,
				lpParms->PPFFOptions.bWritePT,
				lpParms->PPFFOptions.bSaveLink);
		break;
	case IDN_TIFF:
		wsprintf(szOptions, "%d %d %d %d",
				lpParms->TIFFOptions.TIFFCompress,
				lpParms->TIFFOptions.TIFFDiff,
				lpParms->TIFFOptions.bWritePT,
				lpParms->TIFFOptions.bSaveMask);
		break;
	case IDN_TARGA:
		wsprintf(szOptions, "%d",
				lpParms->TGAOptions.bSaveMask);
		break;
	case IDN_JPEG:
		wsprintf(szOptions, "%d %d",
				lpParms->JPEGOptions.nCompressionFactor,
				lpParms->JPEGOptions.nSubsampling);
		break;
	case IDN_DCS:
	case IDN_EPS:
		wsprintf(szOptions, "%d %d %s",
				lpParms->EPSOptions.PreviewBPP,
				lpParms->EPSOptions.CompositeBPP,
				lpParms->EPSOptions.ClipPath);
		break;
	case IDN_AVI:
		wsprintf(szOptions, "%d %d %d %d",
				lpParms->AVIOptions.iAVIPosType,
				lpParms->AVIOptions.iAVISizingType,
				lpParms->AVIOptions.AVIPosition,
				lpParms->AVIOptions.AVILastPosition);
		break;
	default:
		szOptions[0] = '\0';
		break;
	}
wsprintf(lpString, lpFormat,
		(LPSTR)lpParms->szFileName,
		(LPSTR)lpParms->szAlbum,
		idFileType,
		idDataType,
		(LPSTR)szOptions);
return(WF_WRITE);
}

/***********************************************************************/
/*																		*/
/*	FreeParms functions:												*/
/*		Use to free up parameters in parameter structure.				*/
/*																		*/
/*		lpParms -	The parameter structure containing pointers to 		*/
/*					something to be freed.								*/
/*																		*/
/***********************************************************************/

/***********************************************************************/
void LoadFileFreeParms(LPLOADFILE_PARMS lpParms)
/***********************************************************************/
{
if (lpParms && lpParms->lpOptions)
	FreeUp(lpParms->lpOptions);
}

/***********************************************************************/
void FreeMaskFreeParms(LPFREEMASK_PARMS lpParms)
/***********************************************************************/
{
if (lpParms && lpParms->lpPoints)
	FreeUp(lpParms->lpPoints);
}

/***********************************************************************/
void PointEditFreeParms(LPPOINTEDIT_PARMS lpParms)
/***********************************************************************/
{
if (lpParms && lpParms->lpPoints)
	FreeUp(lpParms->lpPoints);
}

/***********************************************************************/
void EffectFreeParms(LPEFFECT_PARMS lpParms)
/***********************************************************************/
{
	if (lpParms && lpParms->lpString)
	{
		FreeUp(lpParms->lpString);
		lpParms->lpString = NULL;
	}
}

/***********************************************************************/
void TextFreeParms(LPTEXT_PARMS lpParms)
/***********************************************************************/
{
if (lpParms && lpParms->lpTextBuf)
	FreeUp(lpParms->lpTextBuf);
}

/***********************************************************************/
void RetouchFreeParms(LPRETOUCH_PARMS lpParms)
/***********************************************************************/
{
if (lpParms && lpParms->hpStrokeInfo)
	FreeUp(lpParms->hpStrokeInfo);
}

/***********************************************************************/
void PSFilterFreeParms(LPPSFILTER_PARMS lpParms)
/***********************************************************************/
{
if (lpParms && lpParms->lpParameters)
	{
	if (lpParms->fHandle == 2) // PhotoShop handle
		PS_DisposHandle(lpParms->lpParameters);
	else
	if (lpParms->fHandle) // Windows handle
		GlobalFree((HGLOBAL)LOWORD((long)lpParms->lpParameters));
	else // Windows pointer
		GlobalFreePtr(lpParms->lpParameters);
	}
}

/***********************************************************************/
void PasteFromFreeParms(LPPASTEFROM_PARMS lpParms)
/***********************************************************************/
{
if (lpParms)
	{
	LPCMD_PARMS lpCmdParms = (LPCMD_PARMS)lpParms;

	// if this packet was ever stored and this file was
	// flagged as temporary, it really means it came from
	// the Windows clipboard and we should bump the
	// reference count for it
	if (lpCmdParms->Common.fParmsStored && lpParms->fTemporary)
		PictPubApp.AddClipboardFileReference(lpParms->szClipFileName);
	}
}

/***********************************************************************/
void DrawFreeFreeParms(LPDRAWFREE_PARMS lpParms)
/***********************************************************************/
{
if (lpParms && lpParms->lpPoints)
	FreeUp(lpParms->lpPoints);
}

/***********************************************************************/
void PaletteEditFreeParms(LPPALETTEEDIT_PARMS lpParms)
/***********************************************************************/
{
if (lpParms && lpParms->lpColorMap)
	FrameDestroyColorMap(lpParms->lpColorMap);
}

/***********************************************************************/
LOCAL LPSTR MergeModeToString(MERGE_MODE MergeMode, LPSTR lpString)
/***********************************************************************/
{
AstralStrEx(MergeMode+IDS_MODEFIRST, lpString, MM_STR_LEN);
return(lpString);
}

/***********************************************************************/
LOCAL MERGE_MODE StringToMergeMode(LPSTR lpString)
/***********************************************************************/
{
int id;
STRING szString;

for (id = IDS_MODEFIRST; id <= IDS_MODELAST; ++id)
	{
	AstralStrEx(id, szString, sizeof(szString));
	if (StringsEqual(szString, lpString))
		return((MERGE_MODE)(id-IDS_MODEFIRST));
	}
return(MM_NORMAL);
}

WRITE_FUNC ChromaMaskWriteParms(LPCHROMA_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
   {
   char        szColor[20];
   LPSTR       lpStr;
   COLORSPACE  Space;

   wsprintf(lpString, lpFormat,
		lpParms->ChromaCombineMode,
		lpParms->ChromaColorMode,lpParms->Fade,lpParms->Mask,lpParms->Delete,
		lpParms->MaskRange[0],lpParms->MaskRange[1],lpParms->MaskRange[2],
		lpParms->MaskRange[3],lpParms->MaskRange[4],lpParms->MaskRange[5],
		lpParms->MaskRange[6],lpParms->MaskRange[7]);

   for (int i=0; i < 8; i++)
      {
      lpStr = lpString + strlen(lpString);
      Space = GetColorInfoString( szColor, &lpParms->MaskColor[i]);
      wsprintf(lpStr, " %d %s",Space, szColor);
      }
   return(WF_WRITE);
   }

LPVOID ChromaMaskReadParms(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
   {
   LPCHROMA_PARMS lpParms;
   COLORSPACE     Space;
   STRING         szTemp;
   int            i;

   if (lpCmdParms)
	   return(lpCmdParms);
   if (!(lpParms = (LPCHROMA_PARMS)Alloc((long)sizeof(CHROMA_PARMS))))
	   return(NULL);

   lpParms->ChromaCombineMode = GetIntParm(lpCommand, 1);
   lpParms->ChromaColorMode   = GetIntParm(lpCommand, 2);
   lpParms->Fade              = GetIntParm(lpCommand, 3);
   lpParms->Mask              = GetIntParm(lpCommand, 4);
   lpParms->Delete            = GetIntParm(lpCommand, 5);

   for (i=0; i < 8; i++)
      lpParms->MaskRange[i]   = GetIntParm(lpCommand, i+6);

   for (i=0; i < 8; i++)
      {
      Space = (COLORSPACE)GetIntParm(lpCommand, i*2+14);
      GetStringParm(lpCommand, i*2+15, szTemp);
      SetColorInfoString(szTemp, &lpParms->MaskColor[i], Space);
      SetColorInfo(&lpParms->MaskColor[i], &lpParms->MaskColor[i], Space);
      }

   return((LPVOID)lpParms);
   }

LPVOID MaskFromObjectRead(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
   {
   LPMASKFROMOBJECT_PARMS lpParms;
   int i;

   if (lpCmdParms)
	   return(lpCmdParms);
   if (!(lpParms = (LPMASKFROMOBJECT_PARMS)Alloc((long)sizeof(MASKFROMOBJECT_PARMS))))
	   return(NULL);
   lpParms->Mode = GetIntParm(lpCommand, 1);
   lpParms->fHiQuality = GetIntParm(lpCommand, 2);
   lpParms->iBaseRes = GetIntParm(lpCommand, 3);
   for (i = 0; i < 4; ++i)
	   {
	   lpParms->DistortPoints[i].x = GetIntParm(lpCommand, 4+(i*2));
	   lpParms->DistortPoints[i].y = GetIntParm(lpCommand, 5+(i*2));
	   }
   return((LPVOID)lpParms);
   }

WRITE_FUNC MaskFromObjectWrite(LPMASKFROMOBJECT_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
   {
   wsprintf(lpString, lpFormat,
		   lpParms->Mode,
		   lpParms->fHiQuality,
	 	   lpParms->iBaseRes,
		   lpParms->DistortPoints[0].x,
		   lpParms->DistortPoints[0].y,
		   lpParms->DistortPoints[1].x,
		   lpParms->DistortPoints[1].y,
		   lpParms->DistortPoints[2].x,
		   lpParms->DistortPoints[2].y,
		   lpParms->DistortPoints[3].x,
		   lpParms->DistortPoints[3].y);
   return(WF_WRITE);
   }

LPVOID ObjectFromMaskRead(LPCMDLIST lpCmdList, LPSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh)
   {
   LPVOID lpParms;

   if (lpCmdParms)
	   return(lpCmdParms);

   if (!(lpParms = (LPVOID)Alloc((long)sizeof(OBJECTFROMMASK_PARMS))))
	   return(NULL);
   return(lpParms);
   }

WRITE_FUNC ObjectFromMaskWrite(LPOBJECTFROMMASK_PARMS lpParms, MACRO_FILE_HANDLE fh,
								LPSTR lpFormat, LPSTR lpString)
   {
   wsprintf(lpString, lpFormat);
   return(WF_WRITE);
   }

/***********************************************************************/
LOCAL int GetCMSInfo(LPSTR lpCommand, LPCMSINFO lpCmsInfo, int index)
/***********************************************************************/
{
	lpCmsInfo->src.DataType = ( FRMDATATYPE )GetIntParm( lpCommand, index++ );
	lpCmsInfo->src.ColorMap = NULL;
	lpCmsInfo->src.ptInfo.toRCS = GetULongParm( lpCommand, index++ );
	lpCmsInfo->src.ptInfo.frRCS = GetULongParm( lpCommand, index++ );
	lpCmsInfo->dst.DataType = ( FRMDATATYPE )GetIntParm( lpCommand, index++ );
	lpCmsInfo->dst.ColorMap = NULL;
	lpCmsInfo->dst.ptInfo.toRCS = GetULongParm( lpCommand, index++ );
	lpCmsInfo->dst.ptInfo.frRCS = GetULongParm( lpCommand, index++ );
	return(index);
}

/***********************************************************************/
LOCAL BOOL GetOpenOptions(LPSTR lpCommand, LPPVOID lpOptions, ITEMID idFileType, int index)
/***********************************************************************/
{
	LPPCDOPTIONS lpPCD;

	switch (idFileType)
		{
		case IDN_PHOTOCD:
			lpPCD = (LPPCDOPTIONS)Alloc(sizeof(PCDOPTIONS));
			if (!lpPCD)
				return(FALSE);
			lpPCD->PhotoCDFormat = GetIntParm(lpCommand, index++);
			lpPCD->PhotoCDRes = GetIntParm(lpCommand, index++);
			lpPCD->BrightButton = GetIntParm(lpCommand, index++);
			lpPCD->RedButton = GetIntParm(lpCommand, index++);
			lpPCD->GreenButton = GetIntParm(lpCommand, index++);
			lpPCD->BlueButton = GetIntParm(lpCommand, index++);
			lpPCD->SatButton = GetIntParm(lpCommand, index++);
			lpPCD->OutOfGamut = GetIntParm(lpCommand, index++);
			lpPCD->SubtractSBA = GetIntParm(lpCommand, index++);
			GetStringParm(lpCommand, index++, lpPCD->LutFileName);
			lpPCD->LutName[0] = '\0';
			lpPCD->fHaveArea = GetIntParm(lpCommand, index++);
			lpPCD->rArea.left = GetFixedParm(lpCommand, index++);
			lpPCD->rArea.top = GetFixedParm(lpCommand, index++);
			lpPCD->rArea.right = GetFixedParm(lpCommand, index++);
			lpPCD->rArea.bottom = GetFixedParm(lpCommand, index++);
			*lpOptions = (LPVOID)lpPCD;
			break;
		default:
			*lpOptions = NULL;
			break;
		}
	return(TRUE);
}

/***********************************************************************/
LOCAL void GetOpenOptionsString(LPVOID lpOptions, ITEMID idFileType, LPSTR lpString)
/***********************************************************************/
{
char szParm1[20];
char szParm2[20];
char szParm3[20];
char szParm4[20];
LPPCDOPTIONS lpPCD;

switch (idFileType)
	{
	case IDN_PHOTOCD:
		lpPCD = (LPPCDOPTIONS)lpOptions;
		wsprintf(lpString, "%d %d %d %d %d %d %d %d %d \042%s\042 %d %s %s %s %s",
			lpPCD->PhotoCDFormat,
			lpPCD->PhotoCDRes,
			lpPCD->BrightButton,
			lpPCD->RedButton,
			lpPCD->GreenButton,
			lpPCD->BlueButton,
			lpPCD->SatButton,
			lpPCD->OutOfGamut,
			lpPCD->SubtractSBA,
			(LPSTR)lpPCD->LutFileName,
			lpPCD->fHaveArea,
			FixedAscii(lpPCD->rArea.left, szParm1, FIXED_PREC),
			FixedAscii(lpPCD->rArea.top, szParm2, FIXED_PREC),
			FixedAscii(lpPCD->rArea.right, szParm3, FIXED_PREC),
			FixedAscii(lpPCD->rArea.bottom, szParm4, FIXED_PREC));
		break;

	default:
		lpString[0] = '\0';
		break;
	}
}
