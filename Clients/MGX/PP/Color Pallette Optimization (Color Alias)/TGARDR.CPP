#include "stdafx.h"
#include "phtypes.h"
#include "targa.h"
#include "dib.h"

/*=========================================================================*/

LOCAL void tgaReadHeader( CFile *pFile, TGAHDR *hdr );
LOCAL BOOL  CheckPic( TGAHDR *head );
LOCAL void tgaGetRawData( TGASTATE *tgaState, LPTR lpDst );
LOCAL void tgaGetScanline( TGASTATE *tgaState, int indepth, BOOL graymap, LPTR lpBuffer, LPTR lpFrameDst );

#define MAXBUFSIZE INT_MAX /* must be at least 2048 */

/*=========================================================================*/

LOCAL void tgaReadHeader ( CFile *pFile, TGAHDR *hdr )
{
	TRY
	{
		pFile->Read(&hdr->textSize, 1);
		pFile->Read(&hdr->mapType,  1);
		pFile->Read(&hdr->dataType, 1);

		pFile->Read(&hdr->mapOrig, 2);
		pFile->Read(&hdr->mapLength, 2);

		pFile->Read(&hdr->CMapBits, 1);

		pFile->Read(&hdr->XOffset, 2);
		pFile->Read(&hdr->YOffset, 2);
		pFile->Read(&hdr->x, 2);
		pFile->Read(&hdr->y, 2);

		pFile->Read(&hdr->dataBits, 1);
		pFile->Read(&hdr->imType,   1);
	}
	END_TRY
}

/*=========================================================================*\

	CheckPic - Check that a file is an uncompressed VDA, ICB, or TARGA image

	Usage:
     	CheckPic (head)
             	struct hdStruct *head;

	Return value:
      	0 - File is an uncompressed VDA, ICB, or TARGA image
     	-1 - File is not an acceptable image

\*=========================================================================*/

LOCAL BOOL CheckPic ( TGAHDR *head )
{
	unsigned char imType;

	/*
		A proper image file should indicate the presence
		or absence of a color map
	*/

	if ((head->mapType != 0) && (head->mapType != 1))
		return (FALSE);

	/*
		Acceptable formats for ICB/TARGA (RGB) Images

    		1 -- Color Mapped
    		2 -- RGB
    		3 -- Black and White
    		9 -- Compresssed Color Mapped
   	   10 -- Compresssed RGB
   	   11 -- Compresssed B&W Mapped
	*/

	if ((head->dataType != 1) && (head->dataType != 2) &&
		(head->dataType != 3) && (head->dataType != 9) &&
		(head->dataType !=10) && (head->dataType != 11)) {
		return(FALSE);
	}

	/*
		If it is a color mapped image--- verify that color map is
		present and that data is at 256 levels, also varify CMapBits
	*/

	if ((head->dataType == 1) || (head->dataType == 9))
	{
		if (head->mapType != 1)
			return (FALSE);
		if (head->dataBits != 8)
			return (FALSE);
		if ((head->CMapBits != 16) && (head->CMapBits != 24) &&
			(head->CMapBits !=32))
			return (FALSE);
	}

	/*
		Verify that the number of bits per pixel is 8, 16, 24 or 32
	*/

	if ((head->dataBits != 8) && (head->dataBits != 16) &&
		(head->dataBits !=24) && (head->dataBits != 32))
	{
		return (FALSE);
	}

	/*
		Verify that the image type (descriptor byte) is one we support
		(non-interleaved and either screen origin)
	*/

	imType = head->imType & 0xE0;

	if ((imType != 0) && (imType != 0x20))
	{
		return(FALSE);
	}

	return (TRUE);
}

/*=========================================================================*/

LOCAL void tgaGetRawData(TGASTATE *tgaState, LPTR lpDst)
{
	int nPixels, nBytes;
	int iCount = tgaState->PixelsPerScanline;

	TRY
	{
		if (tgaState->bCompressed)
		{
			if (tgaState->lpBuffer)
			{
				while(iCount)
				{	
					/* if count is zero, we must start a new packet */
					if (tgaState->cnt == 0)
					{
						unsigned char byte;

						/* get packet header */
						byte = *tgaState->lpBuffer++;

						/* set count for packet */
						tgaState->cnt = (0x7F & byte) + 1;

						/* set type of packet */
						tgaState->rawPacket = (0x80 & byte) == 0;

						if (!tgaState->rawPacket)
						{
							CopyMemory(tgaState->DataBuf, tgaState->lpBuffer, tgaState->inputBytes);
							tgaState->lpBuffer += tgaState->inputBytes;
						}
					}

					nPixels = tgaState->cnt;
					if (nPixels > iCount)
						nPixels = iCount;
					iCount -= nPixels;
					tgaState->cnt -= nPixels;
					if (tgaState->rawPacket)
					{
						nBytes = nPixels * tgaState->inputBytes;
						CopyMemory(lpDst, tgaState->lpBuffer, nBytes);
						tgaState->lpBuffer += nBytes;
						lpDst += nBytes;
					}
					else
					{
						while (--nPixels >= 0)
						{
							CopyMemory(lpDst, tgaState->DataBuf, tgaState->inputBytes);
							lpDst += tgaState->inputBytes;
						}
					}
				}
			}
			else
			{
				while(iCount)
				{	
					/* if count is zero, we must start a new packet */
					if (tgaState->cnt == 0)
					{
						unsigned char byte;

						/* get packet header */
						tgaState->pFile->Read( &byte, 1 );

						/* set count for packet */
						tgaState->cnt = (0x7F & byte) + 1;

						/* set type of packet */
						tgaState->rawPacket = (0x80 & byte) == 0;

						if (!tgaState->rawPacket)
							tgaState->pFile->Read(tgaState->DataBuf, tgaState->inputBytes );
					}

					nPixels = tgaState->cnt;
					if (nPixels > iCount)
						nPixels = iCount;
					iCount -= nPixels;
					tgaState->cnt -= nPixels;
					if (tgaState->rawPacket)
					{
						nBytes = nPixels * tgaState->inputBytes;
						tgaState->pFile->Read(lpDst, nBytes);
						lpDst += nBytes;
					}
					else
					{
						while (--nPixels >= 0)
						{
							CopyMemory(lpDst, tgaState->DataBuf, tgaState->inputBytes);
							lpDst += tgaState->inputBytes;
						}
					}
				}
			}
		}
		else
		{
			tgaState->pFile->Read( lpDst, tgaState->BytesPerScanline );
		}
	}
	END_TRY
}

/*=========================================================================*/

LOCAL void tgaGetRawData24(TGASTATE *tgaState, LPTR lpDst)
{
	int nPixels, nBytes;
	int iCount = tgaState->PixelsPerScanline;

	TRY
	{
		if (tgaState->bCompressed)
		{
			if (tgaState->lpBuffer)
			{
				while(iCount)
				{	
					/* if count is zero, we must start a new packet */
					if (tgaState->cnt == 0)
					{
						unsigned char byte;

						/* get packet header */
						byte = *tgaState->lpBuffer++;

						/* set count for packet */
						tgaState->cnt = (0x7F & byte) + 1;

						/* set type of packet */
						tgaState->rawPacket = (0x80 & byte) == 0;

						if (!tgaState->rawPacket)
						{
							CopyMemory(tgaState->DataBuf, tgaState->lpBuffer, 3);
							tgaState->lpBuffer += 3;
						}
					}

					nPixels = tgaState->cnt;
					if (nPixels > iCount)
						nPixels = iCount;
					iCount -= nPixels;
					tgaState->cnt -= nPixels;
					if (tgaState->rawPacket)
					{
						nBytes = nPixels * 3;
						CopyMemory(lpDst, tgaState->lpBuffer, nBytes);
						tgaState->lpBuffer += nBytes;
						lpDst += nBytes;
					}
					else
					{
						while (--nPixels >= 0)
						{
							*lpDst++ = tgaState->DataBuf[0];
							*lpDst++ = tgaState->DataBuf[1];
							*lpDst++ = tgaState->DataBuf[2];
						}
					}
				}
			}
			else
			{
				while(iCount)
				{	
					/* if count is zero, we must start a new packet */
					if (tgaState->cnt == 0)
					{
						unsigned char byte;

						/* get packet header */
						tgaState->pFile->Read( &byte, 1 );

						/* set count for packet */
						tgaState->cnt = (0x7F & byte) + 1;

						/* set type of packet */
						tgaState->rawPacket = (0x80 & byte) == 0;

						if (!tgaState->rawPacket)
							tgaState->pFile->Read(tgaState->DataBuf, 3 );
					}

					nPixels = tgaState->cnt;
					if (nPixels > iCount)
						nPixels = iCount;
					iCount -= nPixels;
					tgaState->cnt -= nPixels;
					if (tgaState->rawPacket)
					{
						nBytes = nPixels * 3;
						tgaState->pFile->Read(lpDst, nBytes);
						lpDst += nBytes;
					}
					else
					{
						while (--nPixels >= 0)
						{
							*lpDst++ = tgaState->DataBuf[0];
							*lpDst++ = tgaState->DataBuf[1];
							*lpDst++ = tgaState->DataBuf[2];
						}
					}
				}
			}
		}
		else
		{
			tgaState->pFile->Read( lpDst, tgaState->BytesPerScanline );
		}
	}
	END_TRY
}

/*=========================================================================*/

LOCAL void tgaGetScanline(
	TGASTATE *tgaState,
	int       indepth,
    BOOL      graymap,
	LPTR      lpIn,
	LPTR      lpOut)
{
	unsigned int uiValue;
    LPTR lpSrc = lpIn;
	int iCount = tgaState->PixelsPerScanline;

	switch( indepth )
	{
		case 1 :
            if( graymap && tgaState->lpRGBmap )
            {
				tgaGetRawData( tgaState, lpSrc );

				// Map MUST be gray 
				while( iCount-- > 0 )
					*lpOut++ = tgaState->lpRGBmap[ *lpSrc++ ].red;
            }
            else
				tgaGetRawData( tgaState, lpOut );
		break;

		case 2 :
			tgaGetRawData( tgaState, lpSrc );

			while(iCount-- > 0)
			{
				uiValue = ( lpSrc[1]<<8 ) | ( lpSrc[0] );

				*lpOut++ = ( uiValue >> 7 ) & 0xF8;
				*lpOut++ = ( uiValue >> 2 ) & 0xF8;
				*lpOut++ = ( uiValue << 3 ) & 0xF8;
				lpSrc += 2;
			}
		break;

		case 3 :
			tgaGetRawData24( tgaState, lpOut );
		break;

		case 4 :
			tgaGetRawData( tgaState, lpSrc );

		    while( iCount-- > 0 )
		    {
			    *lpOut++ = lpSrc[2];
			    *lpOut++ = lpSrc[1];
			    *lpOut++ = lpSrc[0];
			    lpSrc += 4;
		    }
		break;
	}
}

/*=========================================================================*/

PDIB ReadTarga(LPCSTR lpFileName)
{
	LPTR	 lpIn;
	LPTR 	lpOut;
	LPTR	lpBuffer;
	RGBS	RGBmap[256];
	RGBQUAD  rgbQuad[256];
	int      row, i, j;
	int      indepth, outdepth;
	int      npix, nlin;
	int      sy;
	WORD     wTemp;
	int      mapBytes;
	int      mapLength;
	BOOL     colorMapped;    /* color mapped image flag */
	TGAHDR   PicHdr;
	BOOL     graymap;
	TGASTATE tgaState;
	CFile	theFile;
	BOOL	fRet = FALSE;
	PDIB 	pDib;
	char 	szMessage[256];
	BITMAPINFOHEADER bmi;
	
	if (!theFile.Open(lpFileName, CFile::modeRead))
	{
		wsprintf(szMessage, "Cannot open file: '%s'", lpFileName);
		MessageBox(NULL, szMessage, "Error", MB_ICONSTOP|MB_OK);
		return(FALSE);
	}

	TRY
	{
		lpIn    = NULL;
		pDib = NULL;

		/* Open the file header and see if it is a valid TARGA image */
		tgaReadHeader( &theFile, &PicHdr );

		/* Check for an acceptable image type */
		if( !CheckPic( &PicHdr ) )
		{
			MessageBox(NULL, "Unsupported format", "Error", MB_ICONSTOP|MB_OK);
			goto BadRead;
		}

		/* skip past image description */
		if( PicHdr.textSize )
			theFile.Seek(( long )PicHdr.textSize, CFile::current );

		/* byte/pixel on the disk */
		indepth = ( PicHdr.dataBits + 7 ) / 8;

		/* Set up the color map */
		/* remember whether or not the image is color mapped. */
		if(( PicHdr.dataType == 1 ) || ( PicHdr.dataType == 9 ))
			colorMapped = TRUE;
		else
			colorMapped = FALSE;

		/* Read color map if provided */
		graymap = FALSE;

		if(( PicHdr.mapType == 1 ) && ( PicHdr.mapLength > 0 ))
		{
			mapBytes = ( PicHdr.CMapBits + 7 ) / 8;

			/* We either want to read the color map data, or skip over it. */
			if( colorMapped )
			{
				// NumEntries = PicHdr.mapLength;

				/* initialize color map entries */
				for( i = 0; i < 256; i++ )
				{
					RGBmap[i].red   =
					RGBmap[i].green =
					RGBmap[i].blue  = 0;
				}

				j = PicHdr.mapOrig;
				graymap = TRUE;

				for( i = PicHdr.mapLength; i; i--, j++ )
				{
					switch( mapBytes )
					{
						case 2:
							theFile.Read(( LPTR )&wTemp, 2 );
							RGBmap[j].red = ( wTemp >> 7 ) & 0xF8;
							RGBmap[j].green = ( wTemp >> 2 ) & 0xF8;
							RGBmap[j].blue = ( wTemp << 3 ) & 0xF8;
						break;

						case 3:
							theFile.Read(( LPTR )&RGBmap[j].blue,  1 );
							theFile.Read(( LPTR )&RGBmap[j].green, 1 );
							theFile.Read(( LPTR )&RGBmap[j].red,   1 );
						break;

						case 4:
							theFile.Read(( LPTR )&RGBmap[j].blue,  1 );
							theFile.Read(( LPTR )&RGBmap[j].green,  1 );
							theFile.Read(( LPTR )&RGBmap[j].red,    1 );
							theFile.Read(( LPTR )&wTemp, 1 );
						break;

						default:
							goto Exit;
						break;
					}

					if( graymap )
					{
						graymap =
							( RGBmap[j].red == RGBmap[j].green ) &&
							( RGBmap[j].red == RGBmap[j].blue );
					}
				}
			}
			else
			{
				mapLength = mapBytes * PicHdr.mapLength;

				if( mapLength )
					theFile.Seek(( long ) mapLength, CFile::current );
			}
		}
		else
		{
			colorMapped = FALSE;

			if( indepth == 1 )
				graymap = TRUE;
		}

		npix = PicHdr.x;
		nlin = PicHdr.y;

		/* allocate space for one line of the image */
		if (!(lpIn = (LPTR)malloc((long)npix*(long)indepth)))
			goto BadMem;

        if( indepth == 2 )
            outdepth = 3;
        else
            outdepth = indepth;

		bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmi.biWidth = npix;
		bmi.biHeight = nlin;
		bmi.biPlanes = 1;
		bmi.biBitCount = outdepth * 8;
		bmi.biCompression = BI_RGB;
		bmi.biSizeImage = 0;
		bmi.biXPelsPerMeter = 1000;
		bmi.biYPelsPerMeter = 1000;
		bmi.biClrUsed = PicHdr.mapLength;
		bmi.biClrImportant = 0;
		memset(rgbQuad, 0, sizeof(rgbQuad));
		for (i = 0; i < (int)bmi.biClrUsed; ++i)
		{
			rgbQuad[i].rgbRed = RGBmap[i].red;
			rgbQuad[i].rgbGreen = RGBmap[i].green;
			rgbQuad[i].rgbBlue = RGBmap[i].blue;
		}
		
		pDib = new CDib(&bmi, rgbQuad, NULL);
		if (!pDib)
			goto BadMem;
				
		LPTR lp = (LPTR)malloc(pDib->GetSizeImage());
		if (!lp)
			goto BadMem;
		pDib->SetPtr(lp);

        //if( indepth == 4 )  // 24-bit w/alpha channel  
        //{
        //    lpAlpha =  FrameOpen( FDT_GRAYSCALE, npix, nlin, 75 );

		//    if( !lpAlpha )
		//    {
		//	    goto Exit;
		//    }
        //}

		/* See if compressed file or not */
		tgaState.bCompressed       = ( PicHdr.dataType > 8 ) ? 1 : 0;
		tgaState.lpRGBmap          = RGBmap;
		tgaState.pFile             = &theFile;
		tgaState.inputBytes        = indepth;
		tgaState.PixelsPerScanline = npix;
		tgaState.BytesPerScanline  = npix * indepth;
		tgaState.cnt               = 0;

		if (tgaState.bCompressed)
		{
			DWORD dwPos = theFile.GetPosition();
			DWORD dwLength = theFile.GetLength();
			DWORD dwSize = dwLength - dwPos;
			lpBuffer = (LPTR)malloc(dwSize);
			if (lpBuffer)
				theFile.Read(lpBuffer, dwSize);
		}
		else
			lpBuffer = NULL;
		tgaState.lpBuffer = lpBuffer;

		for( row = 0; row < PicHdr.y; row++ )
		{
			sy = ( PicHdr.imType & 0x20 ) ? row : ( nlin - row - 1 );

			lpOut = pDib->GetXY(0, sy);
	    	tgaGetScanline( &tgaState, indepth, graymap, lpIn, lpOut );
		}

		if (lpBuffer)
			free(lpBuffer);
		
		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadRead;
	}
	END_CATCH_ALL

BadRead:
	wsprintf(szMessage, "Error reading from file: '%s'", lpFileName);
	MessageBox(NULL, szMessage, "Error", MB_ICONSTOP|MB_OK);
	goto Exit;

BadMem:
	MessageBox(NULL, "Out of memory", "Error", MB_ICONSTOP|MB_OK);

Exit:
	theFile.Close();
	if( lpIn )
		free( lpIn );

	return( pDib );
}

/*=========================================================================*/

