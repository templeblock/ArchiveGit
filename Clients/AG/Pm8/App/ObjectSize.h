#ifndef _OBJECTSIZE_H_
#define _OBJECTSIZE_H_

// ****************************************************************************
//
//  Class Name:	RObjectSize
//
//  Description:	class for object size rules for all project types.
//
// ****************************************************************************
//
class RObjectSize
{
public:
//	NOTE: the order of the RObjectSize enum is linked to the order of elements in the static arrays
//		RObjectSize::m_yMinSize and RObjectSize::m_yDefaultSize
	enum				EObjectSizeType		{ kAspectRatio = 0, kDefaultSizePortrait, kMinSizePortrait, kMaxObjectSize
													, kDefaultSizeLandscape, kMinSizeLandscape, kMaxObjectSizeBanner
													, kNumObjectSizeTypes };
													
	enum				EObjectType		{ kSquareObject = 0, kRowObject, kColumnObject
											, kBackdropObject, kHeadlineObject, kSpecialHeadlineObject, kVerticalHeadlineObject, kVerticalSpecialHeadlineObject
											, kTextBlockObject, kReturnAddressObject
											, kHorizontalRuledLineObject, kVerticalRuledLineObject, kSealObject
											, k1SignatureObject, k2SignatureObject, k3SignatureObject, k4SignatureObject
											, kBorderObject, kMiniBorderObject, kCertificateBorderObject
											, kImportedObjectObject, kImageObject, kPhotoObject, kMiniBackdropObject
											, kTileBackdropObject, kTimepieceObject, kDecorativeNumberObject
											, kInitialCapObject, kNonStandardObject, kLogoObject, kGroupObject
											, kCGMObject, kNumObjectTypes };
public:
	static YRealDimension			GetMinSizePortrait( const EObjectType eType );
	static YRealDimension			GetMinSizeLandscape( const EObjectType eType );
	static YRealDimension			GetMaxObjectSize( const EObjectType eType );
	static YRealDimension			GetMaxObjectSizeBanner( const EObjectType eType );
	static YRealDimension			GetDefaultSizePortrait( const EObjectType eType );
	static YRealDimension			GetDefaultSizeLandscape( const EObjectType eType );
	static YRealDimension			GetAspectRatio( const EObjectType eType );
	static EObjectType				GetObjectType( const YComponentType& componentType );

private:
	static YRealDimension			m_yObjectSizes[ kNumObjectTypes ][ kNumObjectSizeTypes ];
};

#endif // _OBJECTSIZE_H_