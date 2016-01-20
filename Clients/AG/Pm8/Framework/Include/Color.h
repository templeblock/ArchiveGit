// ****************************************************************************
// @Doc
// @Module FillColor.h |
//
//  File Name:			Color.h
//
//  Project:			Renaissance Framework
//
//  Author:				Michael G. Dunlap
//
//  Description:		Declaration of the Rolor class
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
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/Color.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _COLOR_H_
#define _COLOR_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// Forward references
class RDrawingSurface;
class RGradientFill;
class RImageColor;
class RBitmapImage;
class RArchive;

//
// @Class Fill color container
class FrameworkLinkage RColor
{
//
// @Access Support types
public:
	//
	// @ENum How to fill an area
	enum EFillMethod
	{
		kTransparent,	// @EMem Don't fill
		kSolid,			// @EMem Fill with solid color
		kGradient,		// @EMem Fill with gradient
//		kBitmap,       // Removed as ordinal value 3 was never 
//		               // completely supported, and the serialzation
//                   // for kBitmap has since changed.
		kOldBitmap = 4,// format change
		kBitmap        // @EMem Fill with bitmap 
	};
	//
	// @ENum How the fill is applied
	enum EFillEffect
	{
		kOpaque,			// @EMem completely opaque (default)
		kTranslucent	// @EMem Translucent fill (implementation to be determined)
	};

//
// @Access Internal Data
private:
	//
	// @CMember How to fill an area
	EFillMethod		m_eFillMethod;
	//
	// @CMember How to apply the color to the fill
	EFillEffect		m_eFillEffect;
	//
	// @CMember Color for solid fills
	RSolidColor		m_rgbFill;
	//
	// @CMember Gradient for gradient fills
	RGradientFill*	m_pGradient;
	//
	// @CMember Bitmap for bitmap fills
	RImageColor*	m_pImageColor;

//
// @Access Construction/Destruction
public:
	//
	// @CMember default ctor (kTransparent)
	RColor();
	//
	// @CMember solid color fill ctor
	RColor( EColors eColor );
	//
	// @CMember solid color fill ctor
	RColor( YPlatformColor yColor );
	//
	// @CMember solid color fill ctor
	RColor( const RSolidColor& rgb );
	//
	// @CMember gradient fill ctor
	RColor( const RGradientFill& gradient );
	//
	// @CMember blend ctor
	RColor( const RSolidColor* pColors, int nColors, YAngle angle );
	//
	// @CMember bitmap fill ctor
	RColor( const RImageColor& bitmap );
	//
	// @CMember copy ctor
	RColor( const RColor& fill );
	//
	// @CMember dtor
	~RColor();

//
// @Access Operators
public:
	//
	// @CMember assignment
	const RColor& operator=( const RColor& rhs );
	//
	// @CMember create a new RColor and transform it
	RColor operator*( const R2dTransform& transform ) const;
	//
	// @CMember transform this RColor
	const RColor& operator*=( const R2dTransform& transform );
	//
	// @CMember equality operator
	BOOLEAN operator==( const RColor& rhs ) const;
	//
	// @CMember equality operator
	BOOLEAN operator!=( const RColor& rhs ) const;
//
// @Access Operations
public:
	//
	// @CMember Get the fill method
	EFillMethod	GetFillMethod( ) const;
	//
	// @CMember Get the solid color 
	const RSolidColor GetSolidColor( YPercentage pct = 0.0 ) const;
	//
	// @CMember Set the bounding rect for the color (nul operation unless it's a gradient)
	RColor&	SetBoundingRect( const RIntRect& rcBounds );

	operator RImageColor() const;

//
// @Access Accessors
private:
	//
	// @CMember Get the fill effect
	EFillEffect GetFillEffect() const;
	//
	// @CMember Set the fill effect
	RColor&	SetFillEffect( EFillEffect eEffect );
	//
	// @CMember Get the gradient color (asserts kGradient)
	const RGradientFill* GetGradientFill( ) const;
	//
	// @CMember Get the bitmap (asserts kBitmap)
	const RImageColor* GetImageColor( ) const;
	//
	// @CMember Read this fillcolor from the given storage object
	void	Read( RArchive& ar );
	//
	// @CMember Write this fillcolor to the given storage object
	void	Write( RArchive& ar ) const;

//
// Friends
public :
	friend FrameworkLinkage RArchive& operator<<( RArchive& archive, const RColor& color );
	friend FrameworkLinkage RArchive& operator>>( RArchive& archive, RColor& color );
	friend class stRTFWriterImp;
	friend class RDrawingSurface;
	friend class RDcDrawingSurface;
	friend class RGpDrawingSurface;
	friend class RPostScriptDrawingSurface;
	friend class RView;
	friend class RPaneView;
	friend class RTextView;
	friend class RWinColorDlg;
	friend class RWinColorPaletteDlgEx;
	friend class RWinColorBtn;
	friend class RTextureGridCtrl;
	friend class RHeadlineColor;
	friend class RTrackingFeedbackRenderer;
	friend class RHeadlineDlg;
	friend class RHeadlineFacePage;
	friend class RHeadlineInterfaceImp;
};

FrameworkLinkage RArchive& operator<<( RArchive& archive, const RColor& color );
FrameworkLinkage RArchive& operator>>( RArchive& archive, RColor& color );

#ifndef TPSDEBUG
#include	"Color.inl"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // !defined(_COLOR_H_)
