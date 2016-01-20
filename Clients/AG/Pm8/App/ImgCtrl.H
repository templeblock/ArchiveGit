//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ImgCtrl.H 1     3/03/99 6:06p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ImgCtrl.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 12    2/25/99 3:43p Lwu
// added selected glow state to buttons
// 
// 11    2/24/99 4:20p Lwu
// overloaded SetControlSize call to take in background image for blending
// 
// 10    2/24/99 4:07p Mwilson
// added include
// 
// 9     2/22/99 4:30p Lwu
// added src rectangle to draw method
// 
// 8     2/19/99 12:29p Mwilson
// added color reduction
// 
// 7     2/17/99 4:27p Lwu
// added default and disable states to the button
// 
// 6     2/17/99 10:12a Mwilson
// modified tiling
// 
// 5     2/16/99 5:29p Mwilson
// added tiling capability
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
//////////////////////////////////////////////////////////////////////////////

#ifndef __IMGCTRL_H__
#define __IMGCTRL_H__
#include "dbbld.h"
#include "InCompdb.h"
#include "cdib.h"
#include "drawingsurface.h"

class CPImageControl;

class CPImageControl : public CObject
{
public:
   CPImageControl(CString csUpImage, 
						CString csDownImage, 
						CString csGlowImage, 
						CString csSelGlowImage,
						CString csDisabledImage,
						CString csDefaultImage,
						BOOL bTiled,
						CPImageControl* pBkgndCtrl, 
						CPUIImageComponentManager& refComponentManager);
						
   // Construction
   virtual ~CPImageControl();
 
	enum States
	{
		UP = 0,
		DOWN,
		GLOW,
		SELECTEDGLOW,
		DISABLED,
		DEFAULT
	};

	enum BorderPieces		// pieces of the border
	{
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT
	};
	
	enum CornerPieces		// pieces of the border
	{
		TOPLEFT = 0,
		BOTTOMLEFT,
		TOPRIGHT,
		BOTTOMRIGHT
	};


   // Operations

	int SetControlSize(CRect& rcBkgnd);
	int SetControlSize(CRect& rcBkgnd, CPImageControl* pBkgdImage);

	void SetOutputResolution(CSize czNewRes);

   // Check if user is clicking in Opaque area of control
   virtual BOOL InClickableRegion(CPoint& ptLoc);

   // Draws Image onto user supplied DC
   virtual int Draw( int nState, CDC& dcDest,const CRect& rcSrc, const CRect& rcDest);

   // Draws Image onto user supplied DC
   virtual int Draw( int nState, RDrawingSurface& rDSDest, const CRect& rcSrc, const CRect& rcDest);

//   virtual ERRORCODE SetBackgroundImage(CDib &refBackgroundImage, CRect &refScreenPosition);

protected:
	int		CreateImage(CPUIImageComponentDataEntry& dataEntry, RBitmapImage* pImage);
	int		LoadImages();
	int		ScaleImages();
	int		BlendImages(CRect& rcBkgnd);
	void		FreeImages();
	int		ScaleImage(RBitmapImage** ppImage, CSize& czNewSize, BOOL bMakeGrayScale = FALSE);
	void		Tile(RBitmapImage* pImage, RDrawingSurface& rDSDest, const CRect& rcSrc);
	int		ReduceImagesColorDepth();
	int		ReduceImageColorDepth(RBitmapImage** ppBitmapImage, PALETTEENTRY*  pPalEntries);

	CString		m_csUpImage;
	CString		m_csDownImage;
	CString		m_csGlowImage;
	CString		m_csSelGlowImage;
	CString		m_csDisabledImage;
	CString		m_csDefaultImage;

	CPUIImageComponentManager* m_pComponentManager;
	
	RBitmapImage*	m_pImageUp;
	RBitmapImage*	m_pImageDown;
	RBitmapImage*	m_pImageGlow;
	RBitmapImage*	m_pImageSelGlow;
	RBitmapImage*	m_pImageDisabled;
	RBitmapImage*	m_pImageDefault;

	RBitmapImage*	m_pImageAlpha;

	CPImageControl* m_pBkgndCtrl;
	CSize				m_czImageSize;
	BOOL				m_bTiled;
	CSize				m_czResolution;
	CPUIImageInfo::ImageType  m_eImageType;

private:
};

#endif
