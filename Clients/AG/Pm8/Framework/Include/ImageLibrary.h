#ifndef _IMAGELIBRARY_H_
#define _IMAGELIBRARY_H_

typedef sWORD YBrightness;
typedef sWORD YContrast;
typedef sWORD YSharpness;
typedef sLONG YBitmapHandle;
typedef uLONG YImageIndex;
typedef sLONG YJpegCompressionQuality;
typedef uLONG YArtisticEffect;
typedef uLONG YArtisticIntensity;
typedef uBYTE YLUTComponent;

enum EImageFormat
{
	kImageFormatUnknown,
	kImageFormatBMP,
	kImageFormatPNG,
	kImageFormatRLE,
	kImageFormatJPG,
	kImageFormatTIF,
	kImageFormatFPX,
	kImageFormatGIF,
	kImageFormatWMF,
	kImageFormatEMF,
	kImageFormatXRX,
	kImageFormatPCD,
	kImageFormatEPS,	// GCB 12/17/97 supported for opening pre-5.0 projects (which can contain EPS)
	kImageFormatPSD,	// RIP 12/17/98 Supported for opening PM projects and content.
	kImageFormatPCX,
	kImageFormatWPG
};

enum EImageExtFlag
{
	kImageExtFPX = 0x0001,
	kImageExtGIF = 0x0002
};

const YJpegCompressionQuality	kDefaultJpegCompressionQuality =	90;
const YImageIndex					kDefaultImageIndex =					1;

const YBrightness					kMinBrightness =						-255;
const YBrightness					kMaxBrightness =						255;
const YBrightness					kDefaultBrightness =					0;
const YBrightness					kBrightnessIncrement =				5;

const YContrast					kMinContrast =							0;
const YContrast					kMaxContrast =							200;
const YContrast					kDefaultContrast =					100;
const YContrast					kContrastIncrement =					5;

const YSharpness					kMinSharpness =						-4;
const YSharpness					kMaxSharpness =						4;
const YSharpness					kDefaultSharpness =					0;
const YSharpness					kSharpnessIncrement =				1;

const RRealRect					kDefaultCropArea =					RRealRect(0.0, 0.0, 1.0, 1.0);

const YArtisticEffect			kDefaultArtisticEffect =			0;
const YArtisticEffect			kLastArtisticEffect =			   10;

// Blur routine stuff
enum {
	blurIn,
	blurOut,
	blurInAndOut
};
typedef unsigned char typeOfBlur;

enum {
	boxFilter,
	gaussianFilter
};
typedef unsigned char typeOfFilter;
// End of blur routine stuff


class RImage;
class RBitmapImage;
class RVectorImage;
class RBuffer;
class RStream;
class RPolyPolygon;
class RGifReader;
class CPGIFAnimator;


#include "Gear.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class FrameworkLinkage RImageLibrary : public RObject
{
// Member functions
public:
	// Construction / destruction
										RImageLibrary();
										~RImageLibrary();

	// Image import/export		
	RImage*							ImportImage(const RMBCString& rImagePath, YImageIndex nIndex = kDefaultImageIndex);
	RImage*							ImportImage(const RBuffer& rBuffer, YImageIndex nIndex = kDefaultImageIndex);
	RImage*							ImportPreviewImage(const RMBCString& rImagePath, const RIntSize& rMaxSize, YImageIndex nIndex = kDefaultImageIndex);	
	RImage*							ImportPreviewImage(const RBuffer& rBuffer, const RIntSize& rMaxSize, YImageIndex nIndex = kDefaultImageIndex);
		
	BOOLEAN							ExportImage(RImage& rImage, const RMBCString& rImagePath, EImageFormat eFormat);	
	BOOLEAN							ExportImage(RImage& rImage, RBuffer& rBuffer, EImageFormat eFormat);	
	BOOLEAN							ExportImage(const RBuffer& rBuffer, const RMBCString& rImagePath, EImageFormat eFormat);

	RRealSize						GetImageDimensions( const RBuffer& rBuffer );
	YImageIndex						GetMaxIndex(const RMBCString& rImagePath);	
	YImageIndex						GetMaxIndex(const RBuffer& rBuffer);	
	BOOLEAN							IsJpegFile(const RMBCString& rImagePath);
	BOOLEAN							IsGifFile(const RMBCString& rImagePath);
	BOOLEAN							IsAnimatedGIF(const RBuffer& rBuffer);
	EImageFormat					GetImageFormat(const RMBCString& rImagePath);
	EImageFormat					GetImageFormat(const RBuffer& rBuffer);
	BOOLEAN							IsFileSupported(const RMBCString& rImagePath);
	BOOLEAN							IsFormatSupported(EImageFormat eFormat);
	void								WritePlttData( const RBitmapImage& rBitmap, const RMBCString& rOutputFileName );	

	BOOLEAN							LoadExtension( EImageExtFlag eExtFlag, LPCSTR szExtension, LPCSTR szLicense, LPCSTR szAuth ) ;

	RGifReader *					CreateGIFReader(const RBuffer& rBuffer);
	CPGIFAnimator *				CreateGIFAnimator(const RBuffer& rBuffer);

	// Bitmap image editing	
	void								Brighten(RBitmapImage& rBitmap, YBrightness nBrightness);	
	void								Contrast(RBitmapImage& rBitmap, YContrast nContrast);
	void								Sharpen(RBitmapImage& rBitmap, YSharpness nSharpness);				
	void								Crop(RBitmapImage& rBitmap, RRealRect& rCropArea);	
	void								GetOffscreenOutline(RBitmapImage& rBitmap, const RSolidColor& transparentColor, RPolyPolygon& outline );	
	void								Tint(RBitmapImage& rBitmap, YTint nTint);	
	void								Tint(RBitmapImage& rBitmap, RSolidColor& rColor);	
	void								ChangeBitDepth(RBitmapImage& rBitmap, uLONG uNewBitDepth);
	void								Rotate(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, RBitmapImage& rMaskBitmap, YAngle nRotation);	
	void								ArtisticEffects(RBitmapImage& rBitmap, YArtisticEffect nEffect, YArtisticIntensity nIntensity);
	void								ApplyLUTsRGB(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, YLUTComponent *pRedLUT, YLUTComponent *pGreenLUT, YLUTComponent *pBlueLUT );
	void								ConvertToGrayscale( RBitmapImage& rBitmap );
	BOOLEAN								RemoveEncircledChromaRed( RBitmapImage& rBitmap, const RIntRect& rRect );
	BOOLEAN								RemoveEncircledChromaAny( RBitmapImage& rBitmap, const RIntRect& rRect );
	BOOLEAN								RemoveScratch( RBitmapImage& rBitmap, const RIntRect& rRect );
	BOOLEAN								RemoveShine( RBitmapImage& rBitmap, const RIntRect& rRect );
	void								AntiAliasWith50PercentReduction( RBitmapImage &rBitmap );

	// Bitmap rotation and transformation
	void								Transform(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, RBitmapImage& rMaskBitmap, const R2dTransform& rTransform);
	void								RCTransform(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, RBitmapImage& rMaskBitmap, YAngle rotationAngle, YScaleFactor xScale, YScaleFactor yScale);
	void								RCTransform(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, RBitmapImage& rMaskBitmap, YAngle rotationAngle, YScaleFactor xScale, YScaleFactor yScale,
											RIntSize* rightTileOffset, RIntSize* belowTileOffset);

	// Bitmap blend (transparency) and blur operations
	void								AlphaBlend(const RBitmapImage& rSrcBitmap, const RIntRect& srcRect, RBitmapImage& rDstBitmap, const RIntRect& dstRect, const RBitmapImage& rAlphaBitmap, const RIntRect& alphaRect) const;
	void								AlphaBlend(const RSolidColor& solidColor, RBitmapImage& rDstBitmap, const RIntRect& dstRect, const RBitmapImage& rAlphaBitmap, const RIntRect& alphaRect) const;
	void								AlphaBlend(const RBitmapImage& rSrcBitmap, const RIntRect& srcRect, RBitmapImage& rDstBitmap, const RIntRect& dstRect) const;
	void								Blur(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, uLONG blurRadius, typeOfBlur blurType, typeOfFilter filterType);

	// Exception handling
	YException						GetLastException();

private:	
	
	RBitmapImage*					LoadPNGImage(const RBuffer& rBuffer, YImageIndex nIndex = kDefaultImageIndex);

	RBitmapImage*					LoadBitmapImage(const RMBCString& rImagePath, YImageIndex nIndex = kDefaultImageIndex);
	RBitmapImage*					LoadBitmapImage(const RBuffer& rBuffer, YImageIndex nIndex = kDefaultImageIndex);
	RVectorImage*					LoadVectorImage(const RMBCString& rImagePath);
	RVectorImage*					LoadVectorImage(const RBuffer& rBuffer);

	BOOLEAN							SaveBitmapToFlashPix(RBitmapImage& rBitmap, const RMBCString& rImagePath);
	BOOLEAN							SaveBitmapImage(RBitmapImage& rBitmap, const RMBCString& rImagePath, EImageFormat eFormat);
	BOOLEAN							SaveBitmapImage(RBitmapImage& rBitmap, RBuffer& rBuffer, EImageFormat eFormat);
	BOOLEAN							SaveVectorImage(RVectorImage& rVector, const RMBCString& rImagePath, EImageFormat eFormat);
	BOOLEAN							SaveVectorImage(RVectorImage& rVector, RBuffer& rBuffer, EImageFormat eFormat);

	// blur implementation
	void								Erode(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, uLONG erosionRadius);
	void								Dilate(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, uLONG dilationRadius);
	void								BoxBlur(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, uLONG blurRadius);
	void								GaussianBlur(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, uLONG blurRadius);

	// Exception handling
	void								ThrowExceptionOnLibraryError();
	void								ClearLastException();

	// Format verification
	BOOLEAN							IsSupportedFormat(int nAccusoftFormat);

	// Image alpha data
	BOOLEAN							SaveImageTransparancyColor(HIGEAR hIGear, RBitmapImage* pNewBitmap, const RBuffer& rBuffer);

	// Artistic Effect helper
	void								ApplyEffectToExpandedDIB(HIGEAR hIGear, YArtisticEffect nEffect, YArtisticIntensity nIntensity, UINT nExtraWidth, UINT nExtraHeight, UINT nBlockSize );

// Data members
private:
	YException						m_nLastException;
	static uWORD					m_uwExtFlags ;

	// Temporary buffers and associated variables for blur routines.
	uBYTE								*divisionLut;
	uBYTE								*scanlineBuffer;

#ifdef TPSDEBUG
public:
	virtual void					Validate() const {}
#endif // TPSDEBUG
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_BITMAPIMPORTLIBRARY_H_