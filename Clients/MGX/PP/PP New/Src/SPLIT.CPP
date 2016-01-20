//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "id.h"

/***********************************************************************/
void SplitRGB(LPIMAGE lpImage)
/***********************************************************************/
{
LPFRAME lpRFrame, lpGFrame, lpBFrame, lpRGBFrame;
LPSTR lpChannel;
LPTR lpSrc;
LPTR lpR, lpG, lpB;
int Xsize, Ysize, FileType, y, iCount;
FNAME ImageName, ChannelName, CurFile;
BOOL fChanged, fDoZoom;
HWND hWnd;
LPIMAGE lpNewImage;
PTINFO  SaveInfo;

if (!lpImage)
	return;

if ((lpImage->DocumentType & 0xFF00) != IMG_NOSPLIT)
	return;

lpRGBFrame = ImgGetBaseEditFrame(lpImage);
if ( FrameType( lpRGBFrame ) != FDT_RGBCOLOR )
	return;

SaveInfo = lpRGBFrame->ptInfo;

fDoZoom = IsZoomed(CWnd::FromHandle(GetActiveDoc())->GetParentFrame()->GetSafeHwnd());
Xsize = FrameXSize(lpRGBFrame);
Ysize = FrameYSize(lpRGBFrame);
ProgressBegin(1, PROGRESS_ID(IDS_UNDOSPLITCHANNEL));
if (!(lpRFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpRGBFrame))))
	{
	ProgressEnd();
	return;
	}
if (!(lpGFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpRGBFrame))))
	{
	FrameClose(lpRFrame);
	ProgressEnd();
	return;
	}
if (!(lpBFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpRGBFrame))))
	{
	FrameClose(lpRFrame);
	FrameClose(lpGFrame);
	ProgressEnd();
	return;
	}
for (y = 0; y < Ysize; ++y)
	{
	if (AstralClockCursor(y, Ysize, YES))
		{
		FrameClose(lpRFrame);
		FrameClose(lpGFrame);
		FrameClose(lpBFrame);
		ProgressEnd();
		return;
		}
	if (!(lpSrc = FramePointer(lpRGBFrame, 0, y, NO)))
		continue;

	if (!(lpR = FramePointer(lpRFrame, 0, y, YES)))
		continue;

	if (!(lpG = FramePointer(lpGFrame, 0, y, YES)))
		continue;

	if (!(lpB = FramePointer(lpBFrame, 0, y, YES)))
		continue;

	iCount = Xsize;
	while( iCount-- > 0)
		{
		*lpR++ = *lpSrc++;
		*lpG++ = *lpSrc++;
		*lpB++ = *lpSrc++;
		}
	}
lstrcpy(CurFile, lpImage->CurFile);
lstrcpy(ImageName, CurFile);
stripext(ImageName);
FileType = lpImage->FileType;
fChanged = ImgChanged(lpImage);
CloseImage(NO, lpImage, NULL);

if ( !AstralStr( IDS_BLUE, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpBFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITRGB|IMG_SPLITB,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}

if ( !AstralStr( IDS_GREEN, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpGFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITRGB|IMG_SPLITG,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}

if ( !AstralStr( IDS_RED, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpRFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITRGB|IMG_SPLITR,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
		  lpNewImage->SaveInfo = SaveInfo;	
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}

ProgressEnd();
}

/***********************************************************************/
void CombineRGB(LPIMAGE lpImage)
/***********************************************************************/
{
LPFRAME lpRFrame, lpGFrame, lpBFrame, lpRGBFrame, lpGSFrame;
LPTR lpDst;
LPTR lpR, lpG, lpB;
int Xsize, Ysize, FileType, y, i, iCount;
BOOL fChanged, fDoZoom;
FNAME ImageName;
LPIMAGE lpRImage, lpGImage, lpBImage;
HWND hWnd;
LPIMAGE lpNewImage, lpSearch;
PTINFO  SaveInfo;
FRMTYPEINFO TypeInfo;

if (!lpImage)
	return;

if ((lpImage->DocumentType & 0xFF00) != IMG_SPLITRGB)
	return;
lpGSFrame = ImgGetBaseEditFrame(lpImage);
if ( FrameType( lpGSFrame ) != FDT_GRAYSCALE )
	return;

ProgressBegin(1, PROGRESS_ID(IDS_UNDOCOMBINECHANNEL));
fDoZoom = IsZoomed(CWnd::FromHandle(GetActiveDoc())->GetParentFrame()->GetSafeHwnd());
lstrcpy(ImageName, lpImage->ImageName);
lpRImage = lpGImage = lpBImage = NULL;

POSITION TemplatePos = PictPubApp.GetFirstDocTemplate ();
while (TemplatePos)
   {
   CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);    
   POSITION DocPos = pTemplate->GetFirstDocPosition();
   while (DocPos)
      {
      CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
      if (pDoc && pDoc->m_lpImage)
         if (StringsEqual(pDoc->m_lpImage->ImageName, ImageName))
            {
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITRGB|IMG_SPLITR))
				   lpRImage = pDoc->m_lpImage;
			   else 
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITRGB|IMG_SPLITG))
				   lpGImage = pDoc->m_lpImage;
		   	else
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITRGB|IMG_SPLITB))
				   lpBImage = pDoc->m_lpImage;
            }
      }
   }

if (!lpRImage || !lpGImage || !lpBImage)
	{
	Message(IDS_CHANNELS);
	ProgressEnd();
	return;
	}

if (ImgInMaskEditMode(lpRImage) ||
	ImgInMaskEditMode(lpGImage) ||
	ImgInMaskEditMode(lpBImage))
	{
	Message(IDS_CHANNELS);
	ProgressEnd();
	return;
	}

if (ImgMultipleObjects(lpRImage) ||
	ImgMultipleObjects(lpGImage) ||
	ImgMultipleObjects(lpBImage))
	{
	Message(IDS_OBJECTSNEEDCOMBINE);
	ProgressEnd();
	return;
	}

lpRFrame = ImgGetBaseEditFrame(lpRImage);
lpGFrame = ImgGetBaseEditFrame(lpGImage);
lpBFrame = ImgGetBaseEditFrame(lpBImage);
Xsize = FrameXSize(lpRFrame);
Ysize = FrameYSize(lpRFrame);
if (Xsize != FrameXSize(lpGFrame) || Ysize != FrameYSize(lpGFrame) ||
    Xsize != FrameXSize(lpBFrame) || Ysize != FrameYSize(lpBFrame))
	{
	Message(IDS_CHANNELSSIZE);
	ProgressEnd();
	return;
	}

SaveInfo = lpRImage->SaveInfo;
FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL, &SaveInfo);
if (!(lpRGBFrame = FrameOpen(TypeInfo, Xsize, Ysize, FrameResolution(lpRFrame))))
	{
	ProgressEnd();
	return;
	}
lpRGBFrame->ptInfo = lpRImage->SaveInfo;
for (y = 0; y < Ysize; ++y)
	{
	if (AstralClockCursor(y, Ysize, YES))
		{
		FrameClose(lpRGBFrame);
		ProgressEnd();
		return;
		}

	if (!(lpDst = FramePointer(lpRGBFrame, 0, y, NO)))
		continue;

	if (!(lpR = FramePointer(lpRFrame, 0, y, YES)))
		continue;

	if (!(lpG = FramePointer(lpGFrame, 0, y, YES)))
		continue;

	if (!(lpB = FramePointer(lpBFrame, 0, y, YES)))
		continue;

	iCount = Xsize;
	while( iCount -- > 0)
		{
		*lpDst++ = *lpR++;
		*lpDst++ = *lpG++;
		*lpDst++ = *lpB++;
		}
	}
lstrcpy(ImageName, lpRImage->ImageName);
FileType = lpRImage->FileType;
fChanged = ImgChanged(lpRImage) || ImgChanged(lpGImage) ||
	ImgChanged(lpBImage);
CloseImage(NO, lpRImage, NULL);
CloseImage(NO, lpGImage, NULL);
CloseImage(NO, lpBImage, NULL);

lpNewImage = CreateImage(ImageName, lpRGBFrame, NULL, 
   NULL, FileType, IDC_SAVE24BITCOLOR, IMG_DOCUMENT,NULL);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))
	   {
        lpNewImage->fChanged = TRUE;
		PictPubApp.DocumentChanged(lpNewImage);
		}
    else
        DestroyImage(lpNewImage);
}

ProgressEnd();
}

/***********************************************************************/
void SplitHSL(LPIMAGE lpImage)
/***********************************************************************/
{
LPFRAME lpHFrame, lpSFrame, lpLFrame, lpRGBFrame;
LPSTR lpChannel;
LPRGB lpSrc;
LPTR lpH, lpS, lpL;
int Xsize, Ysize, FileType, y, iCount;
FNAME ImageName, ChannelName, CurFile;
BOOL fChanged, fDoZoom;
HSLS hsl;
HWND hWnd;
LPIMAGE lpNewImage;
PTINFO  SaveInfo;

if (!lpImage)
	return;

if ((lpImage->DocumentType & 0xFF00) != IMG_NOSPLIT)
	return;

lpRGBFrame = ImgGetBaseEditFrame(lpImage);
if ( FrameType( lpRGBFrame ) != FDT_RGBCOLOR )
	return;

SaveInfo = lpRGBFrame->ptInfo;

fDoZoom = IsZoomed(CWnd::FromHandle(GetActiveDoc())->GetParentFrame()->GetSafeHwnd());
Xsize = FrameXSize(lpRGBFrame);
Ysize = FrameYSize(lpRGBFrame);
ProgressBegin(1, PROGRESS_ID(IDS_UNDOSPLITCHANNEL));
if (!(lpHFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpRGBFrame))))
	{
	ProgressEnd();
	return;
	}
if (!(lpSFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpRGBFrame))))
	{
	FrameClose(lpHFrame);
	ProgressEnd();
	return;
	}
if (!(lpLFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpRGBFrame))))
	{
	FrameClose(lpHFrame);
	FrameClose(lpSFrame);
	ProgressEnd();
	return;
	}
for (y = 0; y < Ysize; ++y)
	{
	if (AstralClockCursor(y, Ysize, YES))
		{
		FrameClose(lpHFrame);
		FrameClose(lpSFrame);
		FrameClose(lpLFrame);
		ProgressEnd();
		return;
		}
	if (!(lpSrc = (LPRGB)FramePointer(lpRGBFrame, 0, y, NO)))
		continue;

	if (!(lpH = FramePointer(lpHFrame, 0, y, YES)))
		continue;

	if (!(lpS = FramePointer(lpSFrame, 0, y, YES)))
		continue;

	if (!(lpL = FramePointer(lpLFrame, 0, y, YES)))
		continue;

	iCount = Xsize;
	while( iCount-- > 0)
		{
		RGBtoHSL(lpSrc->red, lpSrc->green, lpSrc->blue, &hsl);
		*lpH++ = hsl.hue;
		*lpS++ = hsl.sat;
		*lpL++ = hsl.lum;
		++lpSrc;
		}
	}
lstrcpy(CurFile, lpImage->CurFile);
lstrcpy(ImageName, CurFile);
stripext(ImageName);
FileType = lpImage->FileType;
fChanged = ImgChanged(lpImage);
CloseImage(NO, lpImage, NULL);

if ( !AstralStr( IDS_HUE, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpHFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITHSL|IMG_SPLITH,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
		  lpNewImage->SaveInfo = SaveInfo;	
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}


if ( !AstralStr( IDS_SAT, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpSFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITHSL|IMG_SPLITS,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}
if ( !AstralStr( IDS_LUM, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpLFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITHSL|IMG_SPLITL,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}

ProgressEnd();
}

/***********************************************************************/
void CombineHSL(LPIMAGE lpImage)
/***********************************************************************/
{
LPFRAME lpHFrame, lpSFrame, lpLFrame, lpRGBFrame, lpGSFrame;
LPRGB lpDst;
LPTR lpH, lpS, lpL;
int Xsize, Ysize, FileType, y, i, iCount;
BOOL fChanged, fDoZoom;
FNAME ImageName;
LPIMAGE lpHImage, lpSImage, lpLImage;
HWND hWnd;
LPIMAGE lpNewImage, lpSearch;
PTINFO SaveInfo;
FRMTYPEINFO TypeInfo;

if (!lpImage)
	return;

if ((lpImage->DocumentType & 0xFF00) != IMG_SPLITHSL)
	return;
lpGSFrame = ImgGetBaseEditFrame(lpImage);
if ( FrameType( lpGSFrame ) != FDT_GRAYSCALE )
	return;

ProgressBegin(1, PROGRESS_ID(IDS_UNDOCOMBINECHANNEL));
fDoZoom = IsZoomed(CWnd::FromHandle(GetActiveDoc())->GetParentFrame()->GetSafeHwnd());
lstrcpy(ImageName, lpImage->ImageName);
lpHImage = lpSImage = lpLImage = NULL;

POSITION TemplatePos = PictPubApp.GetFirstDocTemplate ();
while (TemplatePos)
   {
   CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);    
   POSITION DocPos = pTemplate->GetFirstDocPosition();
   while (DocPos)
      {
      CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
      if (pDoc && pDoc->m_lpImage)
         if (StringsEqual(pDoc->m_lpImage->ImageName, ImageName))
            {
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITHSL|IMG_SPLITH))
				   lpHImage = pDoc->m_lpImage;
			   else 
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITHSL|IMG_SPLITS))
				   lpSImage = pDoc->m_lpImage;
		   	else
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITHSL|IMG_SPLITL))
				   lpLImage = pDoc->m_lpImage;
            }
      }
   }

if (!lpHImage || !lpSImage || !lpLImage)
	{
	Message(IDS_CHANNELS);
	ProgressEnd();
	return;
	}
if (ImgInMaskEditMode(lpHImage) ||
	ImgInMaskEditMode(lpSImage) ||
	ImgInMaskEditMode(lpLImage))
	{
	Message(IDS_CHANNELS);
	ProgressEnd();
	return;
	}
if (ImgMultipleObjects(lpHImage) ||
	ImgMultipleObjects(lpSImage) ||
	ImgMultipleObjects(lpLImage))
	{
	Message(IDS_OBJECTSNEEDCOMBINE);
	ProgressEnd();
	return;
	}

lpHFrame = ImgGetBaseEditFrame(lpHImage);
lpSFrame = ImgGetBaseEditFrame(lpSImage);
lpLFrame = ImgGetBaseEditFrame(lpLImage);
Xsize = FrameXSize(lpHFrame);
Ysize = FrameYSize(lpHFrame);
if (Xsize != FrameXSize(lpSFrame) || Ysize != FrameYSize(lpSFrame) ||
    Xsize != FrameXSize(lpLFrame) || Ysize != FrameYSize(lpLFrame))
	{
	Message(IDS_CHANNELSSIZE);
	ProgressEnd();
	return;
	}

SaveInfo = lpHImage->SaveInfo;
FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL, &SaveInfo);
if (!(lpRGBFrame = FrameOpen(TypeInfo, Xsize, Ysize, FrameResolution(lpHFrame))))
	{
	ProgressEnd();
	return;
	}
lpRGBFrame->ptInfo = lpHImage->SaveInfo;
for (y = 0; y < Ysize; ++y)
	{
	if (AstralClockCursor(y, Ysize, YES))
		{
		FrameClose(lpRGBFrame);
		ProgressEnd();
		return;
		}

	if (!(lpDst = (LPRGB)FramePointer(lpRGBFrame, 0, y, NO)))
		continue;

	if (!(lpH = FramePointer(lpHFrame, 0, y, YES)))
		continue;

	if (!(lpS = FramePointer(lpSFrame, 0, y, YES)))
		continue;

	if (!(lpL = FramePointer(lpLFrame, 0, y, YES)))
		continue;

	iCount = Xsize;
	while( iCount -- > 0)
		{
		HSLtoRGB(*lpH, *lpS, *lpL, lpDst);
		++lpH;
		++lpS;
		++lpL;
		++lpDst;
		}
	}
lstrcpy(ImageName, lpHImage->ImageName);
FileType = lpHImage->FileType;
fChanged = ImgChanged(lpHImage) || ImgChanged(lpSImage) ||
	ImgChanged(lpLImage);
CloseImage(NO, lpHImage, NULL);
CloseImage(NO, lpSImage, NULL);
CloseImage(NO, lpLImage, NULL);

lpNewImage = CreateImage(ImageName, lpRGBFrame, NULL, 
   NULL, FileType, IDC_SAVE24BITCOLOR, IMG_DOCUMENT,NULL);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))
	   {
        lpNewImage->fChanged = TRUE;
		PictPubApp.DocumentChanged(lpNewImage);
		}
    else
        DestroyImage(lpNewImage);
}

ProgressEnd();
}

/***********************************************************************/
void SplitCMYK(LPIMAGE lpImage)
/***********************************************************************/
{
LPFRAME lpCFrame, lpMFrame, lpYFrame, lpKFrame, lpCMYKFrame;
LPTR lpSrc;
LPTR lpC, lpM, lpY, lpK;
LPSTR lpChannel;
int Xsize, Ysize, FileType, y, iCount;
FNAME ImageName, ChannelName, CurFile;
BOOL fChanged, fDoZoom;
HWND hWnd;
LPIMAGE lpNewImage;
PTINFO  SaveInfo;

if (!lpImage)
	return;

if ((lpImage->DocumentType & 0xFF00) != IMG_NOSPLIT)
	return;

lpCMYKFrame = ImgGetBaseEditFrame(lpImage);
if ( FrameType(lpCMYKFrame) != FDT_CMYKCOLOR )
	return;

SaveInfo = lpCMYKFrame->ptInfo;

ProgressBegin(1, PROGRESS_ID(IDS_UNDOSPLITCHANNEL));
fDoZoom = IsZoomed(CWnd::FromHandle(GetActiveDoc())->GetParentFrame()->GetSafeHwnd());
Xsize = FrameXSize(lpCMYKFrame);
Ysize = FrameYSize(lpCMYKFrame);
if (!(lpCFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpCMYKFrame))))
	{
	ProgressEnd();
	return;
	}
if (!(lpMFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpCMYKFrame))))
	{
	FrameClose(lpCFrame);
	ProgressEnd();
	return;
	}
if (!(lpYFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpCMYKFrame))))
	{
	FrameClose(lpCFrame);
	FrameClose(lpMFrame);
	ProgressEnd();
	return;
	}
if (!(lpKFrame = FrameOpen(FDT_GRAYSCALE, Xsize, Ysize, FrameResolution(lpCMYKFrame))))
	{
	FrameClose(lpCFrame);
	FrameClose(lpMFrame);
	FrameClose(lpYFrame);
	ProgressEnd();
	return;
	}

for (y = 0; y < Ysize; ++y)
	{
	if (AstralClockCursor(y, Ysize, YES))
		{
		FrameClose(lpCFrame);
		FrameClose(lpMFrame);
		FrameClose(lpYFrame);
		FrameClose(lpKFrame);
		ProgressEnd();
		return;
		}

	if (!(lpSrc = FramePointerRaw(lpCMYKFrame, 0, y, NO)))
		continue;

	if (!(lpC = FramePointerRaw(lpCFrame, 0, y, YES)))
		continue;

	if (!(lpM = FramePointerRaw(lpMFrame, 0, y, YES)))
		continue;

	if (!(lpY = FramePointerRaw(lpYFrame, 0, y, YES)))
		continue;

	if (!(lpK = FramePointerRaw(lpKFrame, 0, y, YES)))
		continue;

	iCount = Xsize;
	while(iCount-- > 0)
	{
		*lpC++ = (*lpSrc++)^0xFF;
		*lpM++ = (*lpSrc++)^0xFF;
		*lpY++ = (*lpSrc++)^0xFF;
		*lpK++ = (*lpSrc++)^0xFF;
	}
	}

lstrcpy(CurFile, lpImage->CurFile);
lstrcpy(ImageName, CurFile);
FileType = lpImage->FileType;
fChanged = ImgChanged(lpImage);
CloseImage(NO, lpImage, NULL);
stripext(ImageName);

if ( !AstralStr( IDS_BLACK, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpKFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITCMYK|IMG_SPLITK,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}

if ( !AstralStr( IDS_YELLOW, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpYFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITCMYK|IMG_SPLITY,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}


if ( !AstralStr( IDS_MAGENTA, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpMFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITCMYK|IMG_SPLITM,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}


if ( !AstralStr( IDS_CYAN, &lpChannel ) )
	lpChannel = "";

lstrcpy(ChannelName, lpChannel);
lstrcat(ChannelName, stripdir(ImageName));
lpNewImage = CreateImage(ChannelName, lpCFrame, NULL, 
   NULL, FileType, IDC_SAVECT, IMG_SPLITCMYK|IMG_SPLITC,CurFile);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))         
    {
		  lpNewImage->SaveInfo = SaveInfo;
        lpNewImage->fChanged = TRUE;
        lpNewImage->fUntitled = YES;
		PictPubApp.DocumentChanged(lpNewImage);
    }
    else
        DestroyImage(lpNewImage);
}


ProgressEnd();
}

/***********************************************************************/
void CombineCMYK(LPIMAGE lpImage)
/***********************************************************************/
{
LPFRAME lpCFrame, lpMFrame, lpYFrame, lpKFrame, lpCMYKFrame, lpGSFrame;
LPTR lpC, lpM, lpY, lpK;
LPTR lpDst;
int Xsize, Ysize, FileType, y, i, iCount;
BOOL fChanged, fDoZoom;
FNAME ImageName;
LPIMAGE lpCImage, lpMImage, lpYImage, lpKImage;
HWND hWnd;
LPIMAGE lpNewImage, lpSearch;
PTINFO SaveInfo;
FRMTYPEINFO TypeInfo;

if (!lpImage)
	return;

if ((lpImage->DocumentType & 0xFF00) != IMG_SPLITCMYK)
	return;

lpGSFrame = ImgGetBaseEditFrame(lpImage);
if ( FrameType(lpGSFrame) != FDT_GRAYSCALE )
	return;

ProgressBegin(1, PROGRESS_ID(IDS_UNDOCOMBINECHANNEL));
fDoZoom = IsZoomed(CWnd::FromHandle(GetActiveDoc())->GetParentFrame()->GetSafeHwnd());
lstrcpy(ImageName, lpImage->ImageName);
lpCImage = lpMImage = lpYImage = lpKImage = NULL;

POSITION TemplatePos = PictPubApp.GetFirstDocTemplate ();
while (TemplatePos)
   {
   CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);    
   POSITION DocPos = pTemplate->GetFirstDocPosition();
   while (DocPos)
      {
      CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
      if (pDoc && pDoc->m_lpImage)
         if (StringsEqual(pDoc->m_lpImage->ImageName, ImageName))
            {
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITCMYK|IMG_SPLITC))
				   lpCImage = pDoc->m_lpImage;
			   else 
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITCMYK|IMG_SPLITM))
				   lpMImage = pDoc->m_lpImage;
		   	else
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITCMYK|IMG_SPLITY))
				   lpYImage = pDoc->m_lpImage;
		   	else
			   if (pDoc->m_lpImage->DocumentType==(IMG_SPLITCMYK|IMG_SPLITK))
				   lpKImage = pDoc->m_lpImage;
            }
      }
   }

if (!lpCImage || !lpMImage || !lpYImage || !lpKImage)
	{
	Message(IDS_CHANNELS);
	ProgressEnd();
	return;
	}
if (ImgInMaskEditMode(lpCImage) ||
	ImgInMaskEditMode(lpMImage) ||
	ImgInMaskEditMode(lpYImage) ||
	ImgInMaskEditMode(lpKImage))
	{
	Message(IDS_CHANNELS);
	ProgressEnd();
	return;
	}
if (ImgMultipleObjects(lpCImage) ||
	ImgMultipleObjects(lpMImage) ||
	ImgMultipleObjects(lpYImage) ||
	ImgMultipleObjects(lpKImage))
	{
	Message(IDS_OBJECTSNEEDCOMBINE);
	ProgressEnd();
	return;
	}

lpCFrame = ImgGetBaseEditFrame(lpCImage);
lpMFrame = ImgGetBaseEditFrame(lpMImage);
lpYFrame = ImgGetBaseEditFrame(lpYImage);
lpKFrame = ImgGetBaseEditFrame(lpKImage);

Xsize = FrameXSize(lpCFrame);
Ysize = FrameYSize(lpCFrame);

if (Xsize != FrameXSize(lpMFrame) || Ysize != FrameYSize(lpMFrame) ||
    Xsize != FrameXSize(lpYFrame) || Ysize != FrameYSize(lpYFrame) ||
    Xsize != FrameXSize(lpKFrame) || Ysize != FrameYSize(lpKFrame))
	{
	Message(IDS_CHANNELSSIZE);
	ProgressEnd();
	return;
	}
SaveInfo = lpCImage->SaveInfo;
FrameSetTypeInfo(&TypeInfo, FDT_CMYKCOLOR, NULL, &SaveInfo);
if (!(lpCMYKFrame = FrameOpen(TypeInfo, Xsize, Ysize, FrameResolution(lpCFrame))))
	{
	ProgressEnd();
	return;
	}

lpCMYKFrame->ptInfo = lpCImage->SaveInfo;

for (y = 0; y < Ysize; ++y)
	{
	if (AstralClockCursor(y, Ysize, YES))
		{
		FrameClose(lpCMYKFrame);
		ProgressEnd();
		return;
		}

	if (!(lpDst = FramePointerRaw( lpCMYKFrame, 0, y, YES)))
		continue;

	if (!(lpC = FramePointerRaw(lpCFrame, 0, y, NO)))
		continue;

	if (!(lpM = FramePointerRaw(lpMFrame, 0, y, NO)))
		continue;

	if (!(lpY = FramePointerRaw(lpYFrame, 0, y, NO)))
		continue;

	if (!(lpK = FramePointerRaw(lpKFrame, 0, y, NO)))
		continue;

	iCount = Xsize;
	while(iCount-- > 0)
	{
		*lpDst++ = (*lpC++)^0xFF;
		*lpDst++ = (*lpM++)^0xFF;
		*lpDst++ = (*lpY++)^0xFF;
		*lpDst++ = (*lpK++)^0xFF;
	}
	}

lstrcpy(ImageName, lpCImage->ImageName);
FileType = lpCImage->FileType;
fChanged = ImgChanged(lpCImage) || ImgChanged(lpMImage) ||
	ImgChanged(lpYImage) || ImgChanged(lpKImage);
CloseImage(NO, lpCImage, NULL);
CloseImage(NO, lpMImage, NULL);
CloseImage(NO, lpYImage, NULL);
CloseImage(NO, lpKImage, NULL);

lpNewImage = CreateImage(ImageName, lpCMYKFrame, NULL, 
   NULL, FileType, IDC_SAVE24BITCOLOR, IMG_DOCUMENT,NULL);
if (lpNewImage)
{
    if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
       lpNewImage))
	   {
        lpNewImage->fChanged = TRUE;
		PictPubApp.DocumentChanged(lpNewImage);
		}
    else
        DestroyImage(lpNewImage);
}
ProgressEnd();
}


