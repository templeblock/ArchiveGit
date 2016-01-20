/*=========================================================================*\

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

	®PL1¯®FD1¯®TP0¯®BT0¯

\*=========================================================================*/

/*=========================================================================*\
	tgardr.c
\*=========================================================================*/

#include "stdafx.h"
#include "mgxbm.h"
#include "targa.h"
#include "types.h"
#include "mask.h"
#include "object.h"

/*=========================================================================*/

LOCAL void tgaReadHeader( CFile *pFile, TGAHDR *hdr );
LOCAL BITMAP_ERROR_CODE  CheckPic( TGAHDR *head );
LOCAL void tgaGetRawData( TGASTATE *tgaState, LPTR lpDst );
LOCAL void tgaGetScanline( TGASTATE *tgaState, int indepth, BOOL graymap, LPTR lpBuffer, LPTR lpFrameDst, LPTR lpAlphaDst, BOOL fTGACMYK );

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

LOCAL BITMAP_ERROR_CODE CheckPic ( TGAHDR *head )
{
	unsigned char imType;

	/*
		A proper image file should indicate the presence
		or absence of a color map
	*/

	if ((head->mapType != 0) && (head->mapType != 1))
		return (BEC_errFileRead);

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
		return(BEC_errBadColorMapSize);
	}

	/*
		If it is a color mapped image--- verify that color map is
		present and that data is at 256 levels, also varify CMapBits
	*/

	if ((head->dataType == 1) || (head->dataType == 9))
	{
		if (head->mapType != 1)
			return (BEC_errFileRead);
		if (head->dataBits != 8)
			return (BEC_errFileRead);
		if ((head->CMapBits != 16) && (head->CMapBits != 24) &&
			(head->CMapBits !=32))
			return (BEC_errFileRead);
	}

	/*
		Verify that the number of bits per pixel is 8, 16, 24 or 32
	*/

	if ((head->dataBits != 8) && (head->dataBits != 16) &&
		(head->dataBits !=24) && (head->dataBits != 32))
	{
		return (BEC_errFileRead);
	}

	/*
		Verify that the image type (descriptor byte) is one we support
		(non-interleaved and either screen origin)
	*/

	imType = head->imType & 0xE0;

	if ((imType != 0) && (imType != 0x20))
	{
		return(BEC_errBadInterleave);
	}

	return (BEC_errNone);
}

/*=========================================================================*/

LOCAL void tgaGetRawData(TGASTATE *tgaState, LPTR lpDst)
{
	int iCount = tgaState->PixelsPerScanline;

	TRY
	{
		if (tgaState->bCompressed)
		{
			while(iCount-- > 0)
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

				if (tgaState->rawPacket)
				{
					tgaState->cnt--;
					tgaState->pFile->Read(lpDst, tgaState->inputBytes);
				}
				else
				{
					tgaState->cnt--;
					copy(tgaState->DataBuf, lpDst, tgaState->inputBytes);
				}
				lpDst += tgaState->inputBytes;
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
	LPTR      lpBuffer,
	LPTR      lpFrameDst,
    LPTR      lpAlphaDst,
	BOOL	  fTGACMYK)
{
	unsigned int uiValue;
    LPTR lpSrc = lpBuffer;
	int iCount = tgaState->PixelsPerScanline;

	switch( indepth )
	{
		case 1 :
            if( graymap && tgaState->lpRGBmap )
            {
				tgaGetRawData( tgaState, lpSrc );

				// Map MUST be gray 
				while( iCount-- > 0 )
					*lpFrameDst++ = tgaState->lpRGBmap[ *lpSrc++ ].red;
            }
            else
				tgaGetRawData( tgaState, lpFrameDst );
		break;

		case 2 :
			tgaGetRawData( tgaState, lpSrc );

			while(iCount-- > 0)
			{
				uiValue = ( lpSrc[1]<<8 ) | ( lpSrc[0] );

				*lpFrameDst++ = ( uiValue >> 7 ) & 0xF8;
				*lpFrameDst++ = ( uiValue >> 2 ) & 0xF8;
				*lpFrameDst++ = ( uiValue << 3 ) & 0xF8;
				lpSrc += 2;
			}
		break;

		case 3 :
			tgaGetRawData( tgaState, lpFrameDst );
			swapBGR( lpFrameDst, lpFrameDst, iCount );
		break;

		case 4 :
			tgaGetRawData( tgaState, lpSrc );

			if( fTGACMYK )       // 32-bit CMYK targas
			{
				while( iCount-- > 0 )
				{
					*lpFrameDst++ = lpSrc[2] ^ 0xFF;
					*lpFrameDst++ = lpSrc[1] ^ 0xFF;
					*lpFrameDst++ = lpSrc[0] ^ 0xFF;
					*lpFrameDst++ = lpSrc[3] ^ 0xFF;
					lpSrc += 4;
				}
			}
			else                        // 24-bit w/alpha channel
			{
			    while( iCount-- > 0 )
			    {
				    *lpFrameDst++ = lpSrc[2];
				    *lpFrameDst++ = lpSrc[1];
				    *lpFrameDst++ = lpSrc[0];
				    *lpAlphaDst++ = lpSrc[3];
				    lpSrc += 4;
			    }
            }
		break;
	}
}

/*=========================================================================*/

BOOL CReadBitmap::TGARead()
{
	LPFRAME  lpFrame;
	LPFRAME  lpAlpha;
    LPMASK   lpMask;
    LPOBJECT lpObject;
	LPTR	 lpBuffer;
	LPTR     lpFrameLine;
	LPTR     lpFrameOut, lpAlphaOut;
	LPRGB    lpRGBmap;
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
	FRMTYPEINFO inType, outType;
	LPCOLORMAP lpColorMap;
	CFrameTypeConvert TypeConvert;
	CFile*	pTheFile;
	BITMAP_ERROR_CODE err;
	BOOL	fRet = FALSE;
    RECT    rMask;

	ProgressBegin( 1 );
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

	TRY
	{
	
		lpBuffer    = NULL;
		lpFrame     = NULL;
		lpAlpha     = NULL;
		lpMask      = NULL;
		lpFrameLine = NULL;
		lpColorMap  = NULL;
		lpRGBmap    = NULL;

		/* Open the file header and see if it is a valid TARGA image */
		tgaReadHeader( pTheFile, &PicHdr );

		/* Check for an acceptable image type */
		if( (err = CheckPic( &PicHdr )) != BEC_errNone )
		{
			SetError(err);
			goto BadRead;
		}

		/* skip past image description */
		if( PicHdr.textSize )
			pTheFile->Seek(( long )PicHdr.textSize, CFile::current );

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
				lpColorMap = FrameCreateColorMap();
				if( !lpColorMap )
				{
					SetError(BEC_errMemory);
					//Message( IDS_EMEMALLOC );
					goto Exit;
				}

				lpColorMap->NumEntries = PicHdr.mapLength;
				lpRGBmap = lpColorMap->RGBData;

				/* initialize color map entries */
				for( i = 0; i < 256; i++ )
				{
					lpRGBmap[i].red   =
					lpRGBmap[i].green =
					lpRGBmap[i].blue  = 0;
				}

				j = PicHdr.mapOrig;
				graymap = TRUE;

				for( i = PicHdr.mapLength; i; i--, j++ )
				{
					switch( mapBytes )
					{
						case 2:
							pTheFile->Read(( LPTR )&wTemp, 2 );
							lpRGBmap[j].red = ( wTemp >> 7 ) & 0xF8;
							lpRGBmap[j].green = ( wTemp >> 2 ) & 0xF8;
							lpRGBmap[j].blue = ( wTemp << 3 ) & 0xF8;
						break;

						case 3:
							pTheFile->Read(( LPTR )&lpRGBmap[j].blue,  1 );
							pTheFile->Read(( LPTR )&lpRGBmap[j].green, 1 );
							pTheFile->Read(( LPTR )&lpRGBmap[j].red,   1 );
						break;

						case 4:
							pTheFile->Read(( LPTR )&lpRGBmap[j].blue,  1 );
							pTheFile->Read(( LPTR )&lpRGBmap[j].green,  1 );
							pTheFile->Read(( LPTR )&lpRGBmap[j].red,    1 );
							pTheFile->Read(( LPTR )&wTemp, 1 );
						break;

						default:
							SetError(BEC_errBadColorMapSize);
							//Message( IDS_BADCOLORMAPSIZE, mapBytes );
							goto Exit;
						break;
					}

					if( graymap )
					{
						graymap =
							( lpRGBmap[j].red == lpRGBmap[j].green ) &&
							( lpRGBmap[j].red == lpRGBmap[j].blue );
					}
				}
			}
			else
			{
				mapLength = mapBytes * PicHdr.mapLength;

				if( mapLength )
					pTheFile->Seek(( long ) mapLength, CFile::current );
			}
		}
		else
		{
			lpRGBmap    = NULL;
			colorMapped = FALSE;

			if( indepth == 1 )
				graymap = TRUE;
		}

		npix = PicHdr.x;
		nlin = PicHdr.y;

		if( indepth == 1 )
		{
			if( colorMapped )
				FrameSetTypeInfo( &inType, FDT_PALETTECOLOR, lpColorMap );
			else
				FrameSetTypeInfo( &inType, FDT_GRAYSCALE );
		}
        else
		if( indepth == 4 && m_fTGACMYK )
			FrameSetTypeInfo( &inType, FDT_CMYKCOLOR);
		else
			FrameSetTypeInfo( &inType, FDT_RGBCOLOR);

		if (!SetupTypes(&inType, &outType, graymap))
			goto Exit;

		if( !TypeConvert.Init( inType, outType, npix, m_DitherType ))
		{
			SetError(BEC_errMemory);
			//Message( IDS_EMEMALLOC );
			goto Exit;	
		}

		/* allocate space for one line of the image */
		if( !AllocLines( &lpBuffer, 1, npix, indepth ))
		{
			SetError(BEC_errMemory);
			//Message( IDS_EMEMALLOC );
			goto BadWrite;
		}

        if( indepth == 2 )
            outdepth = 3;
        else
            outdepth = indepth;

		/* allocate space for one line of the image */
		if( !AllocLines( &lpFrameLine, 1, npix, outdepth ))
		{
			SetError(BEC_errMemory);
			//Message( IDS_EMEMALLOC );
			goto BadWrite;
		}

		/* Create the image frame store */
		lpFrame = FrameOpen( outType, npix, nlin, 75 );

		if( !lpFrame )
		{
			SetError(BEC_errFrameOpen);
			//FrameError( IDS_EIMAGEOPEN );
			goto Exit;
		}

        if( indepth == 4 && !m_fTGACMYK )  // 24-bit w/alpha channel  
        {
            lpAlpha =  FrameOpen( FDT_GRAYSCALE, npix, nlin, 75 );

		    if( !lpAlpha )
		    {
	            FrameClose( lpFrame );
				SetError(BEC_errFrameOpen);
			    //FrameError( IDS_EIMAGEOPEN );
			    goto Exit;
		    }
        }

		/* See if compressed file or not */
		tgaState.bCompressed       = ( PicHdr.dataType > 8 ) ? 1 : 0;
		tgaState.lpRGBmap          = lpRGBmap;
		tgaState.pFile             = pTheFile;
		tgaState.inputBytes        = indepth;
		tgaState.PixelsPerScanline = npix;
		tgaState.BytesPerScanline  = npix * indepth;
		tgaState.cnt               = 0;

		for( row = 0; row < PicHdr.y; row++ )
		{
			if( Progress( row, nlin, YES ))
				goto Exit;

			sy = ( PicHdr.imType & 0x20 ) ? row : ( nlin - row - 1 );

			lpFrameOut = FramePointerRaw( lpFrame, 0, sy, YES );
			if( !lpFrameOut )
				goto BadWrite;

            if( lpAlpha )
            {
			    lpAlphaOut = FramePointerRaw( lpAlpha, 0, sy, YES );
			    if( !lpAlphaOut )
				    goto BadWrite;
            }

	    	tgaGetScanline( &tgaState, indepth, graymap, lpBuffer, lpFrameLine, lpAlphaOut, m_fTGACMYK );

			TypeConvert.ConvertData( lpFrameLine, lpFrameOut, sy, npix );
		}

        if( lpAlpha )
        {
	        if( !( lpMask = MaskCreate( lpAlpha, FrameXSize( lpAlpha ),
						   		        FrameYSize( lpAlpha ),
						   		        OFF, m_fNoUndo, FDT_GRAYSCALE )))
	        {
                FrameClose( lpAlpha );
                lpAlpha = NULL;
	        }
            else
                if( !MaskRectUpdate( lpMask, &rMask ))
                {
                    MaskClose( lpMask );
                    lpMask = NULL;
                }
        }

        lpObject = ObjCreateFromFrame( ST_PERMANENT, lpFrame, lpMask,
                                       NULL, m_fNoUndo );

        if( !lpObject )
        {
            FrameClose( lpFrame );

            if( lpMask )
            {
                MaskClose( lpMask );
                lpAlpha = NULL;
            }
    		goto BadWrite;
        }

		ObjAddTail( &m_ObjList, lpObject );

		m_iWidth = npix;
		m_iHeight = nlin;
		m_iRes = FrameResolution( lpFrame );

		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadRead;
	}
	END_CATCH_ALL

BadRead:
	SetError(BEC_errFileRead);
	//Message( IDS_EREAD, m_lpFileName );
	goto Exit;

BadWrite:
	SetError(BEC_errFrameRead);
	//FrameError( IDS_EFRAMEREAD );

Exit:
	CloseFile(pTheFile);
	if( lpBuffer )
		FreeUp( lpBuffer );
	if( lpFrameLine )
		FreeUp( lpFrameLine );
	if( lpColorMap )
		FrameDestroyColorMap( lpColorMap );

	ProgressEnd();

	return( fRet );
}

/*=========================================================================*/

