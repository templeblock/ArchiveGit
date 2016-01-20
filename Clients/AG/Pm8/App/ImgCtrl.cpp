//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ImgCtrl.cpp 1     3/03/99 6:06p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ImgCtrl.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 12    2/25/99 3:43p Lwu
// added selected glow state to buttons
// 
// 11    2/24/99 4:20p Lwu
// overloaded SetControlSize call to take in background image for blending
// 
// 10    2/22/99 4:30p Lwu
// added src rectangle to draw method
// 
// 9     2/19/99 12:29p Mwilson
// added color reduction
// 
// 8     2/17/99 4:27p Lwu
// added default and disable states to the button
// 
// 7     2/17/99 10:12a Mwilson
// modified tiling
// 
// 6     2/16/99 5:29p Mwilson
// added tiling capability
// 
// 5     2/11/99 12:48p Mwilson
// fixed InClickableRegion
// 
// 4     2/09/99 5:24p Mwilson
// Fixed 8 bit scale
// 
// 3     2/09/99 4:48p Mwilson
// updated to handle zooming and alpha channel
// 
// 1     2/05/99 8:51a Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImgCtrl.h"
#include "InCompdb.h"
#include "dcdrawingsurface.h"
#include "offscreendrawingsurface.h"
#include "bitmapimage.h"
#include "imageutils.h"
#include "memdev.h"
#include "ImageLibrary.h"
#include "bmpTiler.h"

const CSize czDefaultRes(800, 600);
extern CPalette* pOurPal;

CPImageControl::CPImageControl(CString csUpImage, 
										CString csDownImage, 
										CString csGlowImage,
										CString csSelGlowImage,
										CString csDisabledImage,
										CString csDefaultImage,
										BOOL bTiled,
										CPImageControl* pBkgndCtrl, 
										CPUIImageComponentManager& refComponentManager):									  
	m_csUpImage				(csUpImage),
	m_csDownImage			(csDownImage),
	m_csGlowImage			(csGlowImage),
	m_csSelGlowImage		(csSelGlowImage),
	m_csDisabledImage		(csDisabledImage),
	m_csDefaultImage		(csDefaultImage),
	m_pBkgndCtrl			(pBkgndCtrl),
	m_pComponentManager	(&refComponentManager),
	m_pImageUp				(NULL),
	m_pImageDown			(NULL),
	m_pImageGlow			(NULL),
	m_pImageSelGlow		(NULL),
	m_pImageDisabled		(NULL),
	m_pImageDefault		(NULL),
	m_pImageAlpha			(NULL),
	m_eImageType			(CPUIImageInfo::typeDIB),
	m_bTiled					(bTiled),
	m_czResolution			(0,0)
{

}

CPImageControl::~CPImageControl()
{
	FreeImages();	
}

void CPImageControl::FreeImages()
{
	if(m_pImageUp)
	{
		delete m_pImageUp;
		m_pImageUp = NULL;
	}
	if(m_pImageDown)
	{
		delete m_pImageDown;
		m_pImageDown = NULL;
	}
	if(m_pImageGlow)
	{
		delete m_pImageGlow;
		m_pImageGlow = NULL;
	}
	if(m_pImageSelGlow)
	{
		delete m_pImageSelGlow;
		m_pImageSelGlow = NULL;
	}
	
	if (m_pImageDisabled)
	{
		delete m_pImageDisabled;
		m_pImageDisabled = NULL;
	}

	if (m_pImageDefault)
	{
		delete m_pImageDefault;
		m_pImageDefault = NULL;
	}

	if(m_pImageAlpha)
	{
		delete m_pImageAlpha;
		m_pImageAlpha = NULL;
	}

}

int CPImageControl::LoadImages()
{
	int nRetVal = ERRORCODE_Fail;
	CPUIImageComponentDataEntry dataEntry;
	FreeImages();

	if(!m_csUpImage.IsEmpty())
	{
		m_pImageUp = new RBitmapImage;
		if(m_pComponentManager->LoadResource(dataEntry, m_csUpImage) == ERRORCODE_None)
			nRetVal = CreateImage(dataEntry, m_pImageUp);
		else
			return ERRORCODE_Fail;
	}

	if(!m_csGlowImage.IsEmpty())
	{
		m_pImageGlow = new RBitmapImage;
		if(m_pComponentManager->LoadResource(dataEntry, m_csGlowImage) == ERRORCODE_None)
			nRetVal = CreateImage(dataEntry, m_pImageGlow);
		else
			return ERRORCODE_Fail;
	}

	if(!m_csSelGlowImage.IsEmpty())
	{
		m_pImageSelGlow = new RBitmapImage;
		if(m_pComponentManager->LoadResource(dataEntry, m_csSelGlowImage) == ERRORCODE_None)
			nRetVal = CreateImage(dataEntry, m_pImageSelGlow);
		else
			return ERRORCODE_Fail;
	}

	if(!m_csDownImage.IsEmpty())
	{
		m_pImageDown = new RBitmapImage;
		if(m_pComponentManager->LoadResource(dataEntry, m_csDownImage) == ERRORCODE_None)
			nRetVal = CreateImage(dataEntry, m_pImageDown);
		else
			return ERRORCODE_Fail;
	}

	if(!m_csDisabledImage.IsEmpty())
	{
		m_pImageDisabled = new RBitmapImage;
		if(m_pComponentManager->LoadResource(dataEntry, m_csDisabledImage) == ERRORCODE_None)
			nRetVal = CreateImage(dataEntry, m_pImageDisabled);
		else
			return ERRORCODE_Fail;
	}

	if(!m_csDefaultImage.IsEmpty())
	{
		m_pImageDefault = new RBitmapImage;
		if(m_pComponentManager->LoadResource(dataEntry, m_csDefaultImage) == ERRORCODE_None)
			nRetVal = CreateImage(dataEntry, m_pImageDefault);
		else
			return ERRORCODE_Fail;
	}

	return nRetVal;
}

void CPImageControl::SetOutputResolution(CSize czNewRes)
{
	if(m_bTiled)
	{
		LoadImages();

		float fXScaleFactor = (float)czNewRes.cx / czDefaultRes.cx;
		float fYScaleFactor = (float)czNewRes.cy / czDefaultRes.cy;

		m_czImageSize.cx = m_czImageSize.cx * fXScaleFactor + .5;
		m_czImageSize.cy = m_czImageSize.cy * fYScaleFactor + .5;
		ScaleImages();
		HDC hDC = GetDC(NULL);
		int nBitDepth = GetDeviceCaps(hDC, BITSPIXEL);
		if(nBitDepth == 8)
			ReduceImagesColorDepth();
	}
}

int CPImageControl::SetControlSize(CRect& rcBkgnd, CPImageControl* pBkgdImage)
{
	m_pBkgndCtrl = pBkgdImage;
	return SetControlSize(rcBkgnd);
}

int CPImageControl::SetControlSize(CRect& rcBkgnd)
{

	if(rcBkgnd.Width() == m_czImageSize.cx && rcBkgnd.Height() == m_czImageSize.cy)
		return ERRORCODE_None;

	int nRetVal = ERRORCODE_None;

	nRetVal = LoadImages();
	if(nRetVal != ERRORCODE_None)
	{
		return nRetVal;
	}

	RBitmapImage* pImage = m_pImageUp;
	if(!pImage)
		pImage = m_pImageGlow;

	int nImageWidth = pImage->GetWidthInPixels();
	int nImageHeight = pImage->GetHeightInPixels();

	if(m_bTiled)
	{
		m_czImageSize.cx = nImageWidth;
		m_czImageSize.cy = nImageHeight;
	}
	else
	{
		m_czImageSize.cx = rcBkgnd.Width();
		m_czImageSize.cy = rcBkgnd.Height();
	}

	nRetVal = ScaleImages();
	if(nRetVal != ERRORCODE_None)
	{
		return nRetVal;
	}

	nRetVal = BlendImages(rcBkgnd);

	if(!m_bTiled)
	{
		HDC hDC = GetDC(NULL);
		int nBitDepth = GetDeviceCaps(hDC, BITSPIXEL);
		if(nBitDepth == 8)
			nRetVal = ReduceImagesColorDepth();
	}

	return nRetVal;
}

int CPImageControl::ReduceImagesColorDepth()
{
	int nNumEntries = pOurPal->GetEntryCount();
	PALETTEENTRY*  pPalEntries = new PALETTEENTRY[nNumEntries];
	pOurPal->GetPaletteEntries(0, nNumEntries, pPalEntries);
	if(m_pImageUp)
	{
		if(ReduceImageColorDepth(&m_pImageUp, pPalEntries) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageDown)
	{
		if(ReduceImageColorDepth(&m_pImageDown, pPalEntries) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageGlow)
	{
		if(ReduceImageColorDepth(&m_pImageGlow, pPalEntries) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageSelGlow)
	{
		if(ReduceImageColorDepth(&m_pImageSelGlow, pPalEntries) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageDisabled)
	{
		if(ReduceImageColorDepth(&m_pImageDisabled, pPalEntries) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageDefault)
	{
		if(ReduceImageColorDepth(&m_pImageDefault, pPalEntries) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	delete pPalEntries;
	return ERRORCODE_None;
}

int CPImageControl::ReduceImageColorDepth(RBitmapImage** ppBitmapImage, PALETTEENTRY*  pPalEntries)
{
	RBitmapImage* pTempImage = new RBitmapImage;
	if(CPImageUtils::ChangeColorDepth(**ppBitmapImage, pTempImage, (BYTE*)pPalEntries) == ERRORCODE_None)
	{
		delete *ppBitmapImage;
		*ppBitmapImage = pTempImage;
		return ERRORCODE_None;
	}
	else
	{
		delete pTempImage;
		return ERRORCODE_Fail;
	}
}

int CPImageControl::BlendImages(CRect& rcBkgnd)
{
	if(m_pImageAlpha == NULL || m_pBkgndCtrl == NULL)
		return ERRORCODE_None;

	//Get the right background image.
	CRect rcImage(0,0, m_czImageSize.cx, m_czImageSize.cy);
	RBitmapImage rBkgndImage;
	rBkgndImage.Initialize(rcBkgnd.Width(), rcBkgnd.Height(), 24); 
	
	ROffscreenDrawingSurface offscreenDS;
	offscreenDS.SetImage( &rBkgndImage );
	offscreenDS.SetFillColor( RSolidColor( kWhite ) );
	offscreenDS.FillRectangle( rcBkgnd );
	m_pBkgndCtrl->Draw(UP, offscreenDS, rcBkgnd, rcImage);
	RBitmapImage* pBkgndImage = (RBitmapImage*)offscreenDS.ReleaseImage();
	

	RImageLibrary rLib;
	//blend the existing images
	if(m_pImageUp)
	{
		//new up a temporary image to blend into and set the background data into it.
		RBitmapImage* pTempImage = new RBitmapImage;
		pTempImage->Initialize(rBkgndImage.GetRawData());
		//blend the image
		rLib.AlphaBlend(*m_pImageUp, rcImage, *pTempImage, rcImage, *m_pImageAlpha, rcImage);
		//delete the old image
		delete m_pImageUp;
		//save the newly blended image
		m_pImageUp = pTempImage;	
	}
	
	if(m_pImageDown)
	{
		RBitmapImage* pTempImage = new RBitmapImage;
		pTempImage->Initialize(rBkgndImage.GetRawData());
		rLib.AlphaBlend(*m_pImageDown, rcImage, *pTempImage, rcImage, *m_pImageAlpha, rcImage);
		delete m_pImageDown;
		m_pImageDown = pTempImage;	
	}
	
	if(m_pImageGlow)
	{
		RBitmapImage* pTempImage = new RBitmapImage;
		pTempImage->Initialize(rBkgndImage.GetRawData());
		rLib.AlphaBlend(*m_pImageGlow, rcImage, *pTempImage, rcImage, *m_pImageAlpha, rcImage);
		delete m_pImageGlow;
		m_pImageGlow = pTempImage;	
	}
	
	if(m_pImageSelGlow)
	{
		RBitmapImage* pTempImage = new RBitmapImage;
		pTempImage->Initialize(rBkgndImage.GetRawData());
		rLib.AlphaBlend(*m_pImageSelGlow, rcImage, *pTempImage, rcImage, *m_pImageAlpha, rcImage);
		delete m_pImageSelGlow;
		m_pImageSelGlow = pTempImage;	
	}

	if(m_pImageDisabled)
	{
		RBitmapImage* pTempImage = new RBitmapImage;
		pTempImage->Initialize(rBkgndImage.GetRawData());
		rLib.AlphaBlend(*m_pImageDisabled, rcImage, *pTempImage, rcImage, *m_pImageAlpha, rcImage);
		delete m_pImageDisabled;
		m_pImageDisabled = pTempImage;	
	}
	
	if(m_pImageDefault)
	{
		RBitmapImage* pTempImage = new RBitmapImage;
		pTempImage->Initialize(rBkgndImage.GetRawData());
		rLib.AlphaBlend(*m_pImageDefault, rcImage, *pTempImage, rcImage, *m_pImageAlpha, rcImage);
		delete m_pImageDefault;
		m_pImageDefault = pTempImage;	
	}

	return ERRORCODE_None;
}

int CPImageControl::ScaleImage(RBitmapImage** ppImage, CSize& czNewSize, BOOL bMakeGrayScale /* = FALSE*/)
{
	RBitmapImage* pScaledImage = new RBitmapImage;
	if(CPImageUtils::Scale(**ppImage, pScaledImage, czNewSize, bMakeGrayScale) == ERRORCODE_None)
	{
		delete *ppImage;
		*ppImage = pScaledImage;
	}
	else
	{
		delete pScaledImage;
		return ERRORCODE_Fail;
	}
	return ERRORCODE_None;
}

int CPImageControl::ScaleImages()
{
	//scale existing images
	if(m_pImageUp)
	{
		if(ScaleImage(&m_pImageUp, m_czImageSize) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageDown)
	{
		if(ScaleImage(&m_pImageDown, m_czImageSize) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageGlow)
	{
		if(ScaleImage(&m_pImageGlow, m_czImageSize) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageSelGlow)
	{
		if(ScaleImage(&m_pImageSelGlow, m_czImageSize) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageDisabled)
	{
		if(ScaleImage(&m_pImageDisabled, m_czImageSize) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageDefault)
	{
		if(ScaleImage(&m_pImageDefault, m_czImageSize) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}
	if(m_pImageAlpha)
	{
		if(ScaleImage(&m_pImageAlpha, m_czImageSize, TRUE) != ERRORCODE_None)
			return ERRORCODE_Fail;
	}

	return ERRORCODE_None;
}

int CPImageControl::Draw( int nState, RDrawingSurface& rDSDest, const CRect& rcSrc, const CRect& rcDest)
{
	RBitmapImage* pImage = NULL;
	//set image to render
	switch(nState)
	{
		case UP:
			pImage = m_pImageUp;
			break;
		case DOWN:
			pImage = m_pImageDown;
			break;
		case GLOW:
			pImage = m_pImageGlow;
			break;
		case SELECTEDGLOW:
			pImage = m_pImageSelGlow;
			break;
		case DISABLED:
			pImage = m_pImageDisabled;
			break;
		case DEFAULT:
			pImage = m_pImageDefault;
			break;
	}
	if(!pImage)
	{
		return ERRORCODE_Fail;
	}
	if(m_bTiled)
	{
		Tile(pImage, rDSDest, rcSrc);
	}
	//render the image
	pImage->Render(rDSDest, rcSrc, rcDest); 

	return ERRORCODE_None;
}

int CPImageControl::Draw( int nState, CDC& dcDest,const CRect& rcSrc, const CRect& rcDest)
{
	//render to a dc
	RDcDrawingSurface destDS;
	destDS.Initialize(dcDest.m_hDC, dcDest.m_hAttribDC);
	
	int retVal = Draw(nState, destDS, rcSrc, rcDest);
	destDS.RestoreDefaults();
	destDS.DetachDCs( );

	return retVal;
}

void CPImageControl::Tile(RBitmapImage* pImage, RDrawingSurface& rDSDest, const CRect& rcSrc)
{
	CBmpTiler tiler;
	tiler.TileBmpInRect(rDSDest, rcSrc, pImage);
}

int CPImageControl::CreateImage(CPUIImageComponentDataEntry& dataEntry, RBitmapImage* pImage)
{
	int nRetVal = ERRORCODE_Fail;

	//store the image type
	m_eImageType = ((CPUIImageInfo*)dataEntry.GetHeader())->GetType();

	if( m_eImageType == CPUIImageInfo::typePNG)
	{
		//set up the storage device
		MemoryDevice memDevice;
		memDevice.AddPiece(dataEntry.GetData(), dataEntry.GetDataSize());

		//Convert the png to two seperate dibs.  we only need to extract the mask once.
		if(m_pImageAlpha)
			nRetVal = CPImageUtils::PngToDib(&memDevice, pImage, NULL);
		else
		{
			nRetVal = CPImageUtils::PngToDib(&memDevice, pImage, &m_pImageAlpha);
		}
	}
	else if (m_eImageType == CPUIImageInfo::typeDIB)
	{
		LPVOID pData = dataEntry.GetData();
		if(pData)
		{
			LPVOID pDibData = (BYTE*)pData + sizeof(BITMAPFILEHEADER);
			pImage->Initialize(pDibData);
			nRetVal = ERRORCODE_None;
		}
	}


	return nRetVal;
}	

BOOL CPImageControl::InClickableRegion(CPoint& ptLoc)
{
	if(m_eImageType == CPUIImageInfo::typeDIB || !m_pImageAlpha)
	{
		CRect rcImage(0,0, m_czImageSize.cx, m_czImageSize.cy);
		if(rcImage.PtInRect(ptLoc))
			return TRUE;
		else 
			return FALSE;
	}

	//get header for size and bitdepth info
	BITMAPINFOHEADER* pHeader = (BITMAPINFOHEADER*)m_pImageAlpha->GetRawData();

	int nMaskRowSize = (pHeader->biWidth*pHeader->biBitCount / 8 + 3) & ~3;
	//calculate the offset to the right pixel
	DWORD dwOffset = (pHeader->biHeight - 1 - ptLoc.y ) * nMaskRowSize + ptLoc.x;

	if(dwOffset > m_pImageAlpha->GetImageDataSize((LPVOID)pHeader) || dwOffset < 0)
		return FALSE;

	//get pointer to image data
	BYTE* pData = (BYTE*)m_pImageAlpha->GetImageData(m_pImageAlpha->GetRawData());
	//check data at offset.  Return true if it is more the 50% transparent
	return (pData[dwOffset] > 128);
}
