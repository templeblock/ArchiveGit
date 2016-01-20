// ****************************************************************************
//
//  File Name:			VectorImage.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RVectorImage class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/VectorImage.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _VECTORIMAGE_H_
#define _VECTORIMAGE_H_

#include "Image.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// Aldus Placeable Header
// Since we are a 32bit app, we have to be sure this structure compiles to
// be identical to a 16 bit app's version. To do this, we use the #pragma
// to adjust packing, we use a WORD for the hmf handle, and a SMALL_RECT
// for the bbox rectangle.
#ifdef _WINDOWS

//	NOTE:	The following is a clone of the SMALL_RECT struct defined in 
//			WINCON.H.  This kludge is necessary because Pretzel's STDAFX.H
//			defines _WINCON_ in order to keep the compiler from choking
//			on the Pretzel declaration of PCOORD in PAGEDEFS.H (which conflicts
//			with the Windows declaration in WINCON.H (yuchh!).  With _WINCON_
//			defined, the WINCON.H declarations do not get included in any sources
//			which also include STDAFX.H (yuccccchhhhhhh!)
typedef struct _pretzelSMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
} pretzelSMALL_RECT, *PpretzelSMALL_RECT;

#pragma pack( push )
#pragma pack( 2 )
struct RAPMFileHeader
{
  DWORD			uKey;
  WORD			hMF;
  pretzelSMALL_RECT	rBoundingBox;
  WORD			sInch;
  DWORD			uReserved;
  WORD			sChecksum;
};
#pragma pack( pop )
const DWORD kAldusPlaceableKey = 0x9ac6cdd7l;
#endif
#ifdef MAC
struct RAPMFileHeader
{
  uLONG			uKey;
  sWORD			hMF;
  Rect			rBoundingBox;
  sWORD			sInch;
  uLONG			uReserved;
  sWORD			sChecksum;
};
const uLONG kAldusPlaceableKey = 0x9ac6cdd7l;
#endif

const YRealDimension kDefaultVectorImageWidth = 2.0; // inches
const YRealDimension kDefaultVectorImageHeight = 2.0; // inches

class FrameworkLinkage RVectorImage : public RImage
{
public:
											RVectorImage();
											RVectorImage(const RVectorImage& rhs);
	virtual 								~RVectorImage();

	// Operations
	virtual void						Initialize(YImageHandle yHandle);
	virtual void						Render(RDrawingSurface& rDS, const RIntRect& rDestRect);
	virtual void						Read( RArchive& rArchive );
	virtual void						Write( RArchive& rArchive ) const;	
	virtual void						Copy(RDataTransferTarget& rDataTarget, YDataFormat yDataFormat) const;
	virtual void						Paste(const RDataTransferSource& rDataSource, YDataFormat yDataFormat);

	virtual void						operator=(const RImage& rhs);
	void									operator=(const RVectorImage& rhs);

	virtual YFloatType				GetAspectRatio() const {return GetWidthInInches() / GetHeightInInches();}
	virtual YImageHandle				GetSystemHandle() const;
	virtual RRealSize					GetSizeInLogicalUnits() const;
	
	// Attributes
	virtual YRealDimension			GetWidthInInches() const;
	virtual YRealDimension			GetHeightInInches() const;
	virtual YRealDimension			GetSuggestedWidthInInches() const;
	virtual YRealDimension			GetSuggestedHeightInInches() const;
	virtual void						SetSuggestedWidthInInches(YRealDimension ySuggestedWidthInInches);
	virtual void						SetSuggestedHeightInInches(YRealDimension ySuggestedHeightInInches);

protected:
#ifdef _WINDOWS
	HENHMETAFILE						m_hEnhMetaFile;	
	HMETAFILE							GetWindowsMetaFile() const;
	static int CALLBACK				EnhMetaFileProc(HDC hDC, HANDLETABLE* pHTable, ENHMETARECORD* pEnhMetaRecord, int nObj, LPARAM lpData);	

#endif

private:
	YRealDimension						m_ySuggestedWidthInInches;
	YRealDimension						m_ySuggestedHeightInInches;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _VECTORIMAGE_H_