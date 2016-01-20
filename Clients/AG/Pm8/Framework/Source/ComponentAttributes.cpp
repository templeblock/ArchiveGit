// ****************************************************************************
//
//  File Name:			ComponentAttributes.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RComponentAttributes class
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
//  $Logfile:: /PM8/Framework/Source/ComponentAttributes.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ComponentAttributes.h"
#include "ChunkyStorage.h"
#include "ComponentTypes.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::RComponentAttributes( )
//
//  Description:		Constructor
//
//							The constructor initializes the struct to the defaults for
//							normal components.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentAttributes::RComponentAttributes( )
	: m_fSelectable( TRUE ),
	  m_fMovable( TRUE ),
	  m_fResizable( TRUE ),
	  m_fRotatable( TRUE ),
	  m_fFlippable( TRUE ),
	  m_fDeletable( TRUE ),
	  m_fShadowable( TRUE ),
	  m_fCanHaveGradient( TRUE ),
	  m_fLocked( FALSE ),
	  m_fCanChangeLockState( TRUE ),
	  m_fClipToParent( FALSE ),
	  m_ZLayer( kNormalZLayer ),
	  m_UniqueId( kNotUnique ),
	  m_fGlowable( TRUE ),
	  m_fAlphaMaskable( TRUE ),
	  m_fVignetteable( TRUE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::GetDefaultComponentAttributes()
//
//  Description:		Return the default attributes for the given component type
//
//							Note: This function breaks the encapsulaton of the
//							framework somewhat but it's necessary in order to both
//							isolate the knowledge of default component attributes and
//							to use the proper default component attributes when
//							reading project files created prior to the creation of new
//							component attributes. GCB 5/7/98
//
//  Exceptions:		none
//
// ****************************************************************************
RComponentAttributes RComponentAttributes::GetDefaultComponentAttributes( const YComponentType& hType )
{
	// Z Layers for special components
	const YZLayer kBackdropZLayer = -100;
	const YZLayer kBorderZLayer = 100;

	RComponentAttributes rAttributes;
	if (hType == kBackdropGraphicComponent)
	{
		rAttributes.SetMovable(FALSE);
		rAttributes.SetResizable(FALSE);
		rAttributes.SetRotatable(TRUE);
		rAttributes.SetSelectable(FALSE);
		rAttributes.SetClipToParent(TRUE);		
		rAttributes.SetZLayer(kBackdropZLayer);
		rAttributes.SetUniqueId(kBackdropId);
	}
	else if (hType == kBorderComponent)
	{
		rAttributes.SetMovable(FALSE);
		rAttributes.SetResizable(FALSE);
		rAttributes.SetRotatable(FALSE);
		rAttributes.SetFlippable(FALSE);
		rAttributes.SetShadowable(FALSE);
		rAttributes.SetGlowable(FALSE);
		rAttributes.SetAlphaMaskable(FALSE);
		rAttributes.SetVignetteable(FALSE);
		rAttributes.SetCanChangeLockState(FALSE);
		rAttributes.SetZLayer(kBorderZLayer);
		rAttributes.SetUniqueId(kBorderId);
	}	
	else if ( hType == kBackdropGraphicComponent )
	{
		rAttributes.SetShadowable( FALSE );
	}
	else if (
		hType == kMiniBorderComponent ||
		hType == kHorizontalLineComponent ||
		hType == kVerticalLineComponent ||
		hType == kLogoComponent ||
		hType == kInitialCapComponent ||
		hType == kSmartNumberComponent ||
		hType == kSealComponent ||
		hType == kTimepieceComponent ||
		hType == kSignatureComponent ||
		hType == kHeadlineComponent ||
		hType == kSpecialHeadlineComponent ||
		hType == kVerticalHeadlineComponent ||
		hType == kVerticalSpecialHeadlineComponent)
	{
		rAttributes.SetVignetteable( FALSE );
	}

	if (hType == kGroupComponent ||
		hType == kLogoComponent ||
		hType == kInitialCapComponent ||
		hType == kSmartNumberComponent ||
		hType == kSealComponent ||
		hType == kTimepieceComponent ||
		hType == kSignatureComponent)
	{
		// Disable shadow, glow and vignette on group and custom components
		// REVIEW GCB 5/4/98 - This is a workaround for not having time to implement
		// proper soft effects rendering on a group as well as shadow & glow on
		// individual components within the same group; this should be reviewed for
		// the next version.
		rAttributes.SetShadowable( FALSE );
		rAttributes.SetGlowable( FALSE );
		rAttributes.SetVignetteable( FALSE );
	}

	return rAttributes;
}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::operator=( )
//
//  Description:		assignment operator
//
//  Returns:			reference to this object
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentAttributes& RComponentAttributes::operator=( const RComponentAttributes& rhs )
	{
	m_fSelectable = rhs.m_fSelectable;
	m_fMovable = rhs.m_fMovable;
	m_fResizable = rhs.m_fResizable;
	m_fRotatable = rhs.m_fRotatable;
	m_fFlippable = rhs.m_fFlippable;
	m_fDeletable = rhs.m_fDeletable;
	m_fShadowable = rhs.m_fShadowable;
	m_fCanHaveGradient = rhs.m_fCanHaveGradient;
	m_fLocked = rhs.m_fLocked;
	m_ZLayer = rhs.m_ZLayer;
	m_fCanChangeLockState = rhs.m_fCanChangeLockState;
	m_fClipToParent = rhs.m_fClipToParent;
	m_UniqueId = rhs.m_UniqueId;
	m_fGlowable = rhs.m_fGlowable;
	m_fAlphaMaskable = rhs.m_fAlphaMaskable;
	m_fVignetteable = rhs.m_fVignetteable;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsSelectable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsSelectable( ) const
	{
	return m_fSelectable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetSelectable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetSelectable( BOOLEAN fSelectable )
	{
	m_fSelectable = fSelectable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsMovable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsMovable( BOOLEAN fCheckLockState ) const
	{
	return static_cast<BOOLEAN>( m_fMovable && ( !fCheckLockState || !m_fLocked ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetMovable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetMovable( BOOLEAN fMovable )
	{
	m_fMovable = fMovable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsResizable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsResizable( BOOLEAN fCheckLockState ) const
	{
	return static_cast<BOOLEAN>( m_fResizable && ( !fCheckLockState || !m_fLocked ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetResizable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetResizable( BOOLEAN fResizable )
	{
	m_fResizable = fResizable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsRotatable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsRotatable( BOOLEAN fCheckLockState ) const
	{
	return static_cast<BOOLEAN>( m_fRotatable && ( !fCheckLockState || !m_fLocked ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetFlippable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetFlippable( BOOLEAN fFlippable )
	{
	m_fFlippable = fFlippable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsFlippable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsFlippable( BOOLEAN fCheckLockState ) const
	{
	return static_cast<BOOLEAN>( m_fFlippable && ( !fCheckLockState || !m_fLocked ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetShadowable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetShadowable( BOOLEAN fShadowable )
	{
	m_fShadowable = fShadowable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsShadowable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsShadowable( BOOLEAN /* fCheckLockState */ ) const
	{
// REVIEW GCB 3/30/98 - spec allows banner headlines (which are locked) to be shadowed
// and there doesn't seem to be any reason to disallow for other locked objects
#if 0
	return static_cast<BOOLEAN>( m_fShadowable && ( !fCheckLockState || !m_fLocked ) );
#else
	return static_cast<BOOLEAN>( m_fShadowable );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetGlowable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetGlowable( BOOLEAN fGlowable )
	{
	m_fGlowable = fGlowable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsGlowable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsGlowable( BOOLEAN /* fCheckLockState */ ) const
	{
// REVIEW GCB 3/30/98 - spec allows banner headlines (which are locked) to be glowed
// and there doesn't seem to be any reason to disallow for other locked objects
#if 0
	return static_cast<BOOLEAN>( m_fGlowable && ( !fCheckLockState || !m_fLocked ) );
#else
	return static_cast<BOOLEAN>( m_fGlowable );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetAlphaMaskable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetAlphaMaskable( BOOLEAN fAlphaMaskable )
	{
	m_fAlphaMaskable = fAlphaMaskable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsAlphaMaskable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsAlphaMaskable( BOOLEAN fCheckLockState ) const
	{
	return static_cast<BOOLEAN>( m_fAlphaMaskable && ( !fCheckLockState || !m_fLocked ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetVignetteable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetVignetteable( BOOLEAN fVignetteable )
	{
	m_fVignetteable = fVignetteable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsVignetteable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsVignetteable( BOOLEAN fCheckLockState ) const
	{
	return static_cast<BOOLEAN>( m_fVignetteable && ( !fCheckLockState || !m_fLocked ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetDeletable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetDeletable( BOOLEAN fDeletable )
	{
	m_fDeletable = fDeletable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsDeletable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsDeletable( BOOLEAN fCheckLockState ) const
	{
	return static_cast<BOOLEAN>( m_fDeletable && ( !fCheckLockState || !m_fLocked ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetCanHaveGradient( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetCanHaveGradient( BOOLEAN fCanHaveGradient )
	{
	m_fCanHaveGradient = fCanHaveGradient;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::CanHaveGradient( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::CanHaveGradient( BOOLEAN fCheckLockState ) const
	{
	return static_cast<BOOLEAN>( m_fCanHaveGradient && ( !fCheckLockState || !m_fLocked ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetRotatable( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetRotatable( BOOLEAN fRotatable )
	{
	m_fRotatable = fRotatable;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsLocked( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsLocked( ) const
	{
	return m_fLocked;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetLocked( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetLocked( BOOLEAN fLocked )
	{
	if( m_fCanChangeLockState )
		m_fLocked = fLocked;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::CanChangeLockState( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::CanChangeLockState( ) const
	{
	return m_fCanChangeLockState;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::CanChangeLockState( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetCanChangeLockState( BOOLEAN fCanChangeLockState )
	{
	m_fCanChangeLockState = fCanChangeLockState;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::CanChangeLockState( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::ClipToParent( ) const
	{
	return m_fClipToParent;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::CanChangeLockState( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetClipToParent( BOOLEAN fClipToParent )
	{
	m_fClipToParent = fClipToParent;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::IsEditable( )
//
//  Description:		Accessor
//
//  Returns:			Attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentAttributes::IsEditable( ) const
	{
	// Currently we assume that if you can not select a component you can not 
	// edit it. If this changes I will just add a new attribute.
	return static_cast<BOOLEAN>( m_fSelectable );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::GetZLayer( )
//
//  Description:		Accessor
//
//  Returns:			Reference to the attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
YZLayer RComponentAttributes::GetZLayer( ) const
	{
	return m_ZLayer;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetZLayer( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetZLayer( YZLayer layer )
	{
	m_ZLayer = layer;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::GetUniqueId( )
//
//  Description:		Accessor
//
//  Returns:			Reference to the attribute
//
//  Exceptions:		None
//
// ****************************************************************************
//
YUniqueId RComponentAttributes::GetUniqueId( ) const
	{
	return m_UniqueId;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetUniqueId( )
//
//  Description:		Accessor
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentAttributes::SetUniqueId( YUniqueId uniqueId )
	{
	m_UniqueId = uniqueId;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::Write( )
//
//  Description:		Writes to the specified archive
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentAttributes::Write( RArchive& archive ) const
	{							
	archive << static_cast<BYTE>( m_fSelectable );
	archive << static_cast<BYTE>( m_fMovable );
	archive << static_cast<BYTE>( m_fResizable );
	archive << static_cast<BYTE>( m_fRotatable );
	archive << static_cast<BYTE>( m_fFlippable );
	archive << static_cast<BYTE>( m_fDeletable );
	archive << static_cast<BYTE>( m_fShadowable );
	archive << static_cast<BYTE>( m_fCanHaveGradient );
	archive << static_cast<BYTE>( m_fLocked );											
	archive << static_cast<BYTE>( m_fCanChangeLockState );
	archive << static_cast<BYTE>( m_fClipToParent );
	archive << m_ZLayer;
	archive << m_UniqueId;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::Read( )
//
//  Description:		Reads from the specified archive
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentAttributes::Read( RArchive& archive )
	{
	BYTE bTemp;

	archive >> bTemp;
	m_fSelectable = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fMovable = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fResizable = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fRotatable = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fFlippable = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fDeletable = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fShadowable = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fCanHaveGradient = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fLocked = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fCanChangeLockState = static_cast<BOOLEAN>( bTemp );

	archive >> bTemp;
	m_fClipToParent = static_cast<BOOLEAN>( bTemp );

	archive >> m_ZLayer;
	archive >> m_UniqueId;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::WriteExtendedAttributes( )
//
//  Description:		Writes the "extended" attributes to the specified archive
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentAttributes::WriteExtendedAttributes( RArchive& archive ) const
	{							
	archive << static_cast<BYTE>( m_fGlowable );
	archive << static_cast<BYTE>( m_fAlphaMaskable );
	archive << static_cast<BYTE>( m_fVignetteable );
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::ReadExtendedAttributes( )
//
//  Description:		Reads the "extended" attributes from the specified archive
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentAttributes::ReadExtendedAttributes( RArchive& archive )
	{
	RChunkStorage* chunkStorage = dynamic_cast<RChunkStorage*>(&archive);
	TpsAssert( chunkStorage, "archive is not an RChunkStorage!" );
	if (!chunkStorage)
		return;
	YStreamLength yChunkSize = chunkStorage->GetCurrentChunkSize();

	BYTE bTemp;

	if (yChunkSize > 0)
		{
		archive >> bTemp;
		m_fGlowable = static_cast<BOOLEAN>( bTemp );
		}
	if (yChunkSize > 1)
		{
		archive >> bTemp;
		m_fAlphaMaskable = static_cast<BOOLEAN>( bTemp );
		}
	if (yChunkSize > 2)
		{
		archive >> bTemp;
		m_fVignetteable = static_cast<BOOLEAN>( bTemp );
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::SetDefaultExtendedComponentAttributes()
//
//  Description:		Set the default extended attributes into this object for
//							the given component type.
//
//  Returns:			Nothing
//
//  Exceptions:		none
//
// ****************************************************************************
void RComponentAttributes::SetDefaultExtendedComponentAttributes( const YComponentType& hType )
{
	RComponentAttributes defaultComponentAttributes( GetDefaultComponentAttributes( hType ) );

	m_fGlowable  = defaultComponentAttributes.m_fGlowable;
	m_fAlphaMaskable = defaultComponentAttributes.m_fAlphaMaskable;
	m_fVignetteable = defaultComponentAttributes.m_fVignetteable;
}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::operator&( )
//
//  Description:		Ands this attributes structure with the specified one
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
RComponentAttributes RComponentAttributes::operator&( const RComponentAttributes& rhs ) const
	{
	RComponentAttributes attributes( *this );

	attributes.m_fSelectable = static_cast<BOOLEAN>( m_fSelectable && rhs.m_fSelectable );
	attributes.m_fMovable = static_cast<BOOLEAN>( m_fMovable && rhs.m_fMovable );
	attributes.m_fResizable = static_cast<BOOLEAN>( m_fResizable && rhs.m_fResizable );
	attributes.m_fRotatable = static_cast<BOOLEAN>( m_fRotatable && rhs.m_fRotatable );
	attributes.m_fFlippable = static_cast<BOOLEAN>( m_fFlippable && rhs.m_fFlippable );
	attributes.m_fDeletable = static_cast<BOOLEAN>( m_fDeletable && rhs.m_fDeletable );
	attributes.m_fShadowable = static_cast<BOOLEAN>( m_fShadowable && rhs.m_fShadowable );
	attributes.m_fCanHaveGradient = static_cast<BOOLEAN>( m_fCanHaveGradient && rhs.m_fCanHaveGradient );
	attributes.m_fGlowable = static_cast<BOOLEAN>( m_fGlowable && rhs.m_fGlowable );
	attributes.m_fAlphaMaskable = static_cast<BOOLEAN>( m_fAlphaMaskable && rhs.m_fAlphaMaskable );
	attributes.m_fVignetteable = static_cast<BOOLEAN>( m_fVignetteable && rhs.m_fVignetteable );

	return attributes;
	}

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::operator&=( )
//
//  Description:		Ands this attributes structure with the specified one
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentAttributes::operator&=( const RComponentAttributes& rhs )
	{
	*this = operator&( rhs );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RComponentAttributes::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
///
// ****************************************************************************
//
void RComponentAttributes::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RComponentAttributes, this );
	}
#endif	//	TPSDEBUG
