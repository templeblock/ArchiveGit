//****************************************************************************
//
// File Name:			FixFlaw.h
//
// Project:				Framework
//
// Author:				Bob Gotsch / John Fleischhauer
//
// Description:			FixFlaw declaration - class to encapsulate fix flaw features
//
// Portability:			Windows
//
// Copyright (C) 1998 Broderbund Software, Inc.  All Rights Reserved.
//
//
// *****************************************************************************

#ifndef	_FIXFLAW_H_
#define	_FIXFLAW_H_

#include "ImageLibrary.h"

// for RemoveEncircledChroma()
enum EDiscreteHue
{ 	
	kDiscreteHueAny, 
	kDiscreteHueRed, 
	kDiscreteHueGreen, 
	kDiscreteHueBlue, 
	kDiscreteHueMagenta, 
	kDiscreteHueYellow, 
	kDiscreteHueCyan
};

class RFixFlaw : public RObject
{
	public:
				RFixFlaw( RBitmapImage& rBitmap, const RIntRect& rRect, RImageLibrary& rLibrary );
				~RFixFlaw();

		BOOLEAN	RemoveEncircledChroma( const EDiscreteHue eHue );
		BOOLEAN	RemoveScratch();
		BOOLEAN	RemoveShine();


	private:
		RBitmapImage&			m_rBitmap;
		const RIntRect& 		m_rRect;
		RImageLibrary&			m_rLibrary;

		BOOLEAN			DoRemoveEncircledChroma( uBYTE *pPixelData, uLONG uSquareSize, uLONG uBitmapWidthInBytes, EDiscreteHue hueSelector );
		BOOLEAN			DoMaskedBlur();
		BOOLEAN			DoRemoveShine( uBYTE *pPixelData, const RIntRect& rBmpRect, uLONG uBmpWidthInBytes, uBYTE *pMaskData, const RIntRect& rMaskRect, uLONG uMaskBmpWidthInBytes );

		uBYTE*			GetBitmapPixels( RBitmapImage& rBitmap, const RIntRect& rRect );
		RBitmapImage*	CreateMask( BOOLEAN bSoftEdge = FALSE );
		uBYTE*			GetMaskPixels( RBitmapImage* pMask );
		RBitmapImage*	CopyRectAreaOfBitmap();
		BOOLEAN			ReplaceMaskedPixels( uBYTE *pOldPixels, const RIntRect& rOldRect, uLONG uOldBmpWidthInBytes, uBYTE *pNewPixels, const RIntRect& rNewRect, uLONG uNewBmpWidthInBytes, uBYTE *pMaskData, const RIntRect& rMaskRect, uLONG uMaskBmpWidthInBytes );

		YFloatType		LookupProfile( sLONG percentage, sLONG *profile );
		void 			NeutralizeBy( uBYTE *pR, uBYTE *pG, uBYTE *pB, YFloatType neutralization);
		void 			MixDownWith( uBYTE *pR, uBYTE *pG, uBYTE *pB, uBYTE avgR, uBYTE avgG, uBYTE avgB, sLONG avgBrightness, YFloatType pixelWeightFactor );
		YFloatType		Redness( uBYTE r, uBYTE g, uBYTE b );

#ifdef	TPSDEBUG
	BOOLEAN				TestRectPosition();

	public:
		virtual void	Validate() const;
#endif

};

#endif		// _CASTLUTS_H_
