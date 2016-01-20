#include "stdafx.h"
#include "mgxbm.h"
#include "dllmain.h"
#include "l_bitmap.h"
#include "l_error.h"

typedef struct _jpgprocinfo
{
	int LineNo;
	int	LinesLeft;
	int npix;
	int nlin;
	int in_depth;
	CFrameTypeConvert TypeConvert;
	LPFRAME lpFrame;
	CReadBitmap *pReader;
} JPGPROCINFO;

BOOL ReadCB(BITMAPHANDLE* pBitmap, L_UCHAR L_FAR* lpBuffer, L_INT nLines, L_VOID L_FAR* lpParms);


BOOL CReadBitmap::JPGRead( )
{
	int xres, i;
	FILEINFO FileInfo;												//Lead FILEINFO structure
	BITMAPHANDLE BitmapHandle;										//Lead BITMAPHANDLE structure
	FILEREADCALLBACK pfnReadCB;										//called by L_DecompressFile every eight lines
	FRMTYPEINFO inType, outType;
	JPGPROCINFO procInfo;

	ASSERT(m_pFile == NULL);

	if (m_pFile != NULL)
		return(FALSE);

	ProgressBegin(1);

	BitmapDllApp.BlockJPGRead();

	if(!L_FileInfo(m_lpFileName, &FileInfo))						//get info required for L_InitBitmap
	{
		ProgressEnd();
		BitmapDllApp.ReleaseJPGRead();
		return(FALSE);
	}

	procInfo.pReader = this;
	procInfo.npix = m_iWidth = FileInfo.Width;											//retrieve relevant info from FileInfo structure
	procInfo.nlin = m_iHeight = FileInfo.Height;									
	if(!FileInfo.ResolutionUnit)
		xres = m_iRes = 75;
	else
		xres = m_iRes = FileInfo.ResolutionUnit;

	procInfo.in_depth  = FileInfo.BitsPerPixel/8;

	if(procInfo.in_depth == 1)											    //set the frame type
		FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
	else
		FrameSetTypeInfo(&inType, FDT_RGBCOLOR);

	if (!SetupTypes(&inType, &outType))
		goto BadJPG;

	L_InitBitmap(&BitmapHandle, m_iWidth, m_iHeight, FileInfo.BitsPerPixel); 	//initialize the BitmapHandle structure
	BitmapHandle.Order = ORDER_RGB;		

	if ( !(procInfo.lpFrame = FrameOpen(outType, procInfo.npix, procInfo.nlin, xres)) )
	{
		SetError(BEC_errFrameOpen);
		goto BadJPG;
	}

	procInfo.LineNo = 0;
	procInfo.LinesLeft = m_iHeight;

	if (!procInfo.TypeConvert.Init(inType, outType, procInfo.npix, m_DitherType))
	{
		SetError(BEC_errMemory);
		goto BadJPG;	
	}

	pfnReadCB = ReadCB;

	if(!L_DecompressFile(m_lpFileName, &BitmapHandle, pfnReadCB, FileInfo.BitsPerPixel, (L_VOID L_FAR *)&procInfo, 0))
		goto BadJPG;

	m_iWidth = procInfo.npix;
	m_iHeight = procInfo.nlin;
	m_iRes = xres;
	m_lpFrame = procInfo.lpFrame;
	ProgressEnd();
	BitmapDllApp.ReleaseJPGRead();
   	return (TRUE);

	BadJPG:
	if (procInfo.lpFrame)    
       	FrameClose(procInfo.lpFrame);
	ProgressEnd();
	BitmapDllApp.ReleaseJPGRead();
	return( FALSE );
}



BOOL ReadCB(BITMAPHANDLE* pBitmap, L_UCHAR L_FAR* lpBuffer, L_INT nLines, L_VOID L_FAR* lpParms)
{
	JPGPROCINFO *pInfo = (JPGPROCINFO *)lpParms;
	CReadBitmap *pReader = pInfo->pReader;
	int Lines, i;
	LPTR lpIn = (LPTR) lpBuffer;
	LPTR lpOut;
	long lWidth = (long)pInfo->npix * pInfo->in_depth;

	Lines = nLines;
	if (Lines > pInfo->LinesLeft)
		Lines = pInfo->LinesLeft;
	pInfo->LinesLeft -= Lines;

	for ( i=0; i<Lines; i++ )
	{
		if (pReader->Progress(pInfo->LineNo, pInfo->nlin, TRUE))
			return(FALSE);

		if ( !(lpOut = FramePointerRaw(pInfo->lpFrame, 0, pInfo->LineNo, TRUE)) )
			return(FALSE);

		lpIn = lpBuffer + (lWidth * i);
	
		pInfo->TypeConvert.ConvertData(lpIn, lpOut, pInfo->LineNo, pInfo->npix);
		++pInfo->LineNo;
	}

	return(TRUE);   //notifies decompression engine that lpBuffer (lpIn) was processed
}










