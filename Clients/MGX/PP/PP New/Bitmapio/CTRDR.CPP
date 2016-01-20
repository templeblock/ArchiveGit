//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "bitmapio.h"
#include "getline.h"

#define CT_TYPE (0x5443)
typedef struct _cthdr
   {
   char     FileName[82];
   WORD     Units;
   WORD     NumColSep;
   WORD     ColSepMask;
   double   Height;
   double   Width;
   LONG     Lines;
   LONG     Pixels;
   WORD     Orientation;
   WORD     Resolution;
   UINT     LineSize;
   LONG     ChannelSize;
	LPTR     lpCTLine;
   } CTHEADER, FAR * LPCTHEADER;

static LPCTHEADER ReadCTHeader(CFile *pFile, LPSTR lpFileName, BITMAP_ERROR_CODE *pErr);
static void FreeCTHeader( LPCTHEADER lpTH);

BOOL CReadBitmap::CTRead()
   {
   LPCTHEADER  lpTH;
   LPFRAME     lpFrame;
   BOOL        fConvert = FALSE, fRet = FALSE;
   CFile 	   theFile;
   CFile*	   pTheFile;
   FRMTYPEINFO inType, outType;
   LPTR        lpIn, lpIn2,lpOut, lpFrmLine;
   int         x,y,j;
   UINT        bytes_read;
   BOOL        GrayScale;
   BITMAP_ERROR_CODE err;
    CFrameTypeConvert	Convert;

	ProgressBegin(1);
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

   lpFrame = NULL;
   if ( !(lpTH = ReadCTHeader(pTheFile, m_lpFileName, &err)))
   {
   		SetError(err);
	   goto Exit;
	}

   GrayScale = lpTH->ColSepMask == 8 && lpTH->NumColSep == 1;
	if (GrayScale)
		FrameSetTypeInfo(&inType, FDT_GRAYSCALE, NULL);
    else 
		FrameSetTypeInfo(&inType, FDT_CMYKCOLOR, NULL);

	if (!SetupTypes(&inType, &outType))
		goto Exit;

	fConvert = !FrameTypeInfoEqual( inType, outType );

	if( fConvert )
		{
		if( !Convert.Init( inType, outType, lpTH->Pixels ) )
			{
			SetError(BEC_errMemory);
			goto Exit;
			}
		}
   /* Create the image frame store */
   if ( !(lpFrame = FrameOpen(outType, lpTH->Pixels, lpTH->Lines, lpTH->Resolution)))
	   {
	   SetError(BEC_errFrameOpen);
      goto Exit;
      }

  	pTheFile->Seek (2048, CFile::begin);
	for (y=0; y < lpTH->Lines; y++)
		{
		if ( Progress(y, lpTH->Lines, YES))
			goto Exit;

      TRY
	      {
		   bytes_read = pTheFile->Read(lpTH->lpCTLine, lpTH->LineSize);
         }
      CATCH_ALL(e)
	      {
			goto BadRead;
	      }
      END_CATCH_ALL
		if (bytes_read != lpTH->LineSize)
			goto BadRead;

		if ( !(lpFrmLine = FramePointerRaw( lpFrame, 0, y, YES)))
			goto BadWrite;
		lpIn  = lpTH->lpCTLine;
      lpOut = lpFrmLine;

      lpIn2 = lpIn;
      for (x=0; x < lpTH->Pixels; x++)
			{
         j = 0;
         if (!GrayScale)
            {
            if (lpTH->ColSepMask & 0x1)  // cyan
               {
               j  += lpTH->ChannelSize;
               *lpOut++ = 255 - *lpIn2;
               }
            else
               *lpOut++ = 0;
            if (lpTH->ColSepMask & 0x2)  // magenta
               {
               *lpOut++ = 255 - *(lpIn2 + j);
               j  += lpTH->ChannelSize;
               }
            else
               *lpOut++ = 0;
            if (lpTH->ColSepMask & 0x4)  // yellow
               {
               *lpOut++ = 255 - *(lpIn2 + j);
               j  += lpTH->ChannelSize;
               }
            else
               *lpOut++ = 0;
            }
         if (lpTH->ColSepMask & 0x8)  // black
            *lpOut++ = 255 - *(lpIn2 + j);
         else
            *lpOut++ = 0;
         lpIn2++;
			}
		if( fConvert )
			Convert.ConvertData( lpFrmLine, lpFrmLine, y, lpTH->Pixels );
		}
   m_iWidth       = lpTH->Pixels;
   m_iHeight      = lpTH->Lines;
   m_iRes         = (int)lpTH->Resolution;
   m_lpFrame      = lpFrame;

   fRet = TRUE;
   goto Exit;

BadRead:
	SetError(BEC_errFileRead);
goto Exit;

BadWrite:
	SetError(BEC_errFrameRead);

Exit:
   FreeCTHeader(lpTH);
   CloseFile(pTheFile);
   if (!fRet && lpFrame)
	   FrameClose(lpFrame);

   ProgressEnd();
   return( fRet );
   }

LPCTHEADER ReadCTHeader(CFile *pTheFile, LPSTR lpFileName, BITMAP_ERROR_CODE *pErr)
   { 
   BOOL        fRet = FALSE;
   LPCTHEADER  lpTH;
   BYTE        Buff[15];
   BYTE        tmp;
   WORD        wtmp;

   *pErr = BEC_errNone;
   if (!(lpTH = (LPCTHEADER)Alloc((long)sizeof(CTHEADER))))
       {
	   *pErr = BEC_errMemory;
	   return(NULL);
	   }

   // Clear out all the header data
   clr( (LPTR)lpTH, sizeof(CTHEADER));

TRY
	{
	if (pTheFile->Read ((LPTR)&lpTH->FileName, 80) != 80) /* read FileName */
	   goto BadRead;
   lpTH->FileName[80] = 0;
	if (pTheFile->Read ((LPTR)&wtmp, 2) != 2) /* read File Type */
	   goto BadRead;
   if (wtmp != CT_TYPE)
	   goto BadRead;

  	pTheFile->Seek (1024, CFile::begin);
	if (pTheFile->Read ((LPTR)&tmp, 1) != 1) /* read Unit of measurement */
	   goto BadRead;
   lpTH->Units = tmp;
	if (pTheFile->Read ((LPTR)&tmp, 1) != 1) /* read Unit of measurement */
	   goto BadRead;
   lpTH->NumColSep = tmp;
	if (pTheFile->Read ((LPTR)&lpTH->ColSepMask, 2) != 2) /* read Unit of measurement */
	   goto BadRead;
	swapw (&lpTH->ColSepMask);
	if (pTheFile->Read ((LPTR)Buff, 14) != 14) /* read Unit of measurement */
	   goto BadRead;
   Buff[14] = 0;
   lpTH->Height = atof((const char *)Buff);
	if (pTheFile->Read ((LPTR)Buff, 14) != 14) /* read Unit of measurement */
	   goto BadRead;
   Buff[14] = 0;
   lpTH->Width = atof((const char *)Buff);
	if (pTheFile->Read ((LPTR)Buff, 12) != 12) /* read Unit of measurement */
	   goto BadRead;
   Buff[12] = 0;
   lpTH->Lines = atol((const char *)Buff);
	if (pTheFile->Read ((LPTR)Buff, 12) != 12) /* read Unit of measurement */
	   goto BadRead;
   Buff[12] = 0;
   lpTH->Pixels = atol((const char *)Buff);
	if (pTheFile->Read ((LPTR)&tmp, 1) != 1) /* read Unit of measurement */
	   goto BadRead;
   lpTH->Orientation = tmp;

   if (!(lpTH->ColSepMask & 0x000F))   // make sure at lease on of the CMYK
	   goto BadRead;                       // channel do exist
      
   if (lpTH->Units)   // inches
      lpTH->Resolution = (int)(lpTH->Pixels / lpTH->Width);
   else                 // mm
      lpTH->Resolution = (int)(lpTH->Pixels * 25.4 / lpTH->Width);
   if (lpTH->Pixels & 1) // odd pixels
      lpTH->ChannelSize = lpTH->Pixels + 1;
   else
      lpTH->ChannelSize = lpTH->Pixels;

   lpTH->LineSize = lpTH->ChannelSize * lpTH->NumColSep;

   if (lpTH->NumColSep > 4)
      lpTH->NumColSep = 4;

	if (!AllocLines( &lpTH->lpCTLine, 1, lpTH->LineSize, 1))
		{
		*pErr = BEC_errMemory;
		goto Exit;
		}

   fRet = TRUE;
	goto Exit;
	}
CATCH_ALL(e)
	{
	goto BadRead;
	}
END_CATCH_ALL
BadRead:
	*pErr = BEC_errFileRead;
Exit:
   if (!fRet)
	   {
	   FreeCTHeader(lpTH);
      lpTH = NULL;
	   }
	return(lpTH);
   }

void FreeCTHeader( LPCTHEADER lpTH)
   {
   if (!lpTH)
	   return;
   if (lpTH->lpCTLine)
	   FreeUp(lpTH->lpCTLine);
   FreeUp((LPTR)lpTH);
   }

BOOL CWriteBitmap::CTWrite()
   {
	RECT              rSave;
	FRMTYPEINFO       inType, outType;
	LPFRAME           lpFrame;
	CFrameTypeConvert TypeConvert;
	LPOBJECT          lpObject;
	CFile 	         theFile;
	CFile*	         pTheFile = NULL;
	BOOL              fRet = FALSE;
	LPTR              lpTH, lp, lp2,lpBuffer, lpImgScanline;
	int               i,x,j,y, LineSize, npix, nlin;
   double            dbl;
   int               NumOfChannels = 4;

	if (m_lpObjList)
	   {
		lpObject = m_lpObjList->lpHead;
		if (lpObject)
			lpFrame = ObjGetEditFrame(lpObject);
	   }
	else
	   {
		lpFrame = m_lpFrame;
		lpObject = NULL;
	   }

	ASSERT(lpFrame);

	if (m_fSrcArea)
		rSave = m_rSrcArea;
	else
	   {
		rSave.top    = rSave.left = 0;
		rSave.bottom = FrameYSize(lpFrame)-1;
		rSave.right  = FrameXSize(lpFrame)-1;
	   }

	npix = RectWidth(&rSave);
	nlin = RectHeight(&rSave);
	inType = FrameTypeInfo(lpFrame);
	// cause FramePointer never returns line art
	if (inType.DataType == FDT_LINEART)
		inType.DataType = FDT_GRAYSCALE;
   
	if (inType.DataType == FDT_GRAYSCALE)
      {
      NumOfChannels = 1;
	   FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
      }
   else
	   FrameSetTypeInfo(&outType, FDT_CMYKCOLOR);
		// initialize stuff to do the type conversion
	ProgressBegin(1);
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

   if (npix & 1) // odd pixels
      LineSize = npix + 1;
   else
      LineSize = npix;

	AllocLines(&lpBuffer, 1, max(LineSize*NumOfChannels, FrameByteWidth(lpFrame)), 1);
	AllocLines(&lpImgScanline, 1, max(LineSize*NumOfChannels, FrameByteWidth(lpFrame)), 1);
	if (!lpBuffer || !lpImgScanline)
	   {
	   SetError(BEC_errMemory);
		goto Exit;
	   }

   if (!(lpTH = Alloc(1024)))
	   {
	   SetError(BEC_errMemory);
	   return(NULL);
	   }

   clr( (LPTR)lpTH, 1024);
   
   set(lpTH, 80, ' ');
   lstrcpy((LPSTR)lpTH, m_lpFileName);
   lpTH[lstrlen(m_lpFileName)] = ' ';
   lpTH[80] = 'C';
   lpTH[81] = 'T';
	pTheFile->Write(lpTH, 1024);  // write first cluster

   clr( (LPTR)lpTH, 84);
   lpTH[0] = 1;               // inches
   lpTH[1] = NumOfChannels;   // 
   if (NumOfChannels == 1)
      *((LPWORD)&lpTH[2]) = 0x0800;
   else
      *((LPWORD)&lpTH[2]) = 0x0F00;
   dbl = nlin / (double)lpFrame->Resolution;
   sprintf((LPSTR)&lpTH[4], "%+.7E",dbl);
   lpTH[6] = lpTH[5];
   lpTH[5] = '.';
   if (lpTH[15] == '+')
      lpTH[17] = lpTH[18] + 1;
   else
      {
      lpTH[17] = lpTH[18] - 1;
      lpTH[15] = '+';
      }
   dbl = npix / (double)lpFrame->Resolution;
   sprintf((LPSTR)&lpTH[18], "%+.7E",dbl);
   lpTH[20] = lpTH[19];
   lpTH[19] = '.';
   if (lpTH[29] == '+')
      lpTH[31] = lpTH[32] + 1;
   else
      {
      lpTH[31] = lpTH[32] - 1;
      lpTH[29] = '+';
      }
   sprintf((LPSTR)&lpTH[32], "%+12d",nlin);
   lpTH[32] = '+';
   for (i=33; i < 44;i++)
      if (lpTH[i] == ' ' || lpTH[i] == '+')
         lpTH[i] = '0';
   sprintf((LPSTR)&lpTH[44], "%+12d",npix);
   lpTH[44] = '+';
   for (i=45; i < 56;i++)
      if (lpTH[i] == ' ' || lpTH[i] == '+')
         lpTH[i] = '0';
   lpTH[56] = 0;                 // scan direction
	pTheFile->Write(lpTH, 1024);  // write second cluster, Parameter Block
	TRY
	   {
	   if (!TypeConvert.Init(inType, outType, npix, DT_DEFAULT))
	      {
		   SetError(BEC_errMemory);
		   goto Exit;
		   }
		for( y=rSave.top; y <= rSave.bottom; y++)
		   {
			if (Progress( y-rSave.top, nlin, m_bEscapable ))
				goto Cancelled;
			if (lpObject)
			   {
				if (!ObjGetLine( lpObject, rSave.left, y,
					(rSave.right - rSave.left) + 1, lpImgScanline))
					goto BadRead;
				lp = lpImgScanline;
			   }
			else
			   {
				if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
					goto BadRead;
			   }
			// convert the data to the new type
			TypeConvert.ConvertData(lp, lpBuffer, y, npix);
         lp2 = lpBuffer;
         for (x=0; x < npix; x++)
            for (j=0; j < NumOfChannels; j++)
               *(lpImgScanline + j * LineSize + x) = 255 - *lp2++;
			pTheFile->Write( lpImgScanline, LineSize*NumOfChannels);
         }
		fRet = TRUE;
		goto Exit;
	   }
	CATCH_ALL(e)
	   {
		goto BadWrite;
	   }
	END_CATCH_ALL


BadWrite:
   SetError(BEC_errWriteFile);
	goto Exit;

BadRead:
	SetError(BEC_errFrameRead);

Cancelled:
Exit:
	CloseFile(pTheFile, fRet);

	if (lpTH)
		FreeUp (lpTH);
	if (lpBuffer)
		FreeUp(lpBuffer);
	if (lpImgScanline)
		FreeUp(lpImgScanline);
	ProgressEnd();

	return( fRet );
   }


