// ****************************************************************************
//
//  File Name:			ComponentAttributes.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RComponentAttributes class
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
//  $Logfile:: /PM8/Framework/Include/ComponentAttributes.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTATTRIBUTES_H_
#define		_COMPONENTATTRIBUTES_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

const YZLayer kNormalZLayer = 0;

const YUniqueId kNotUnique = 0;

// Unique Ids for components which must be unique in a panel
//
// Note: Putting this info here breaks the encapsulaton of the
// framework somewhat but it's necessary in order to both
// isolate the knowledge of default component attributes (see
//	the RComponentAttributes::GetDefaultAttribute() function) and
// to use the proper default component attributes when reading
// project files created prior to the creation of new component
// attributes. GCB 5/7/98
//
const YUniqueId kBorderId = 1000;
const YUniqueId kBackdropId = 2000;
const YUniqueId kBannerObjectId = 3000;
const YUniqueId kCalendarObjectId = 4000;


// ****************************************************************************
//
//  Class Name:		RComponentAttributes
//
//  Description:		Class which encapsulates various component attributes
//
// ****************************************************************************
//
class FrameworkLinkage RComponentAttributes : public RObject
	{
	// Construction
	public :
												RComponentAttributes( );

	// Accessors
	public :
		static RComponentAttributes	GetDefaultComponentAttributes( const YComponentType& hType );

		BOOLEAN								IsSelectable( ) const;
		void									SetSelectable( BOOLEAN fSelectable );

		BOOLEAN								IsMovable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetMovable( BOOLEAN fMovable );

		BOOLEAN								IsResizable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetResizable( BOOLEAN fResizable );

		BOOLEAN								IsRotatable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetRotatable( BOOLEAN fRotatable );

		BOOLEAN								IsFlippable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetFlippable( BOOLEAN fFlippable );

		BOOLEAN								IsShadowable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetShadowable( BOOLEAN fShadowable );

		BOOLEAN								IsGlowable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetGlowable( BOOLEAN fGlowable );

		BOOLEAN								IsAlphaMaskable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetAlphaMaskable( BOOLEAN fAlphaMaskable );

		BOOLEAN								IsVignetteable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetVignetteable( BOOLEAN fVignetteable );

		BOOLEAN								IsDeletable( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetDeletable( BOOLEAN fDeletable );

		BOOLEAN								CanHaveGradient( BOOLEAN fCheckLockState = TRUE ) const;
		void									SetCanHaveGradient( BOOLEAN fCanHaveGradient );

		BOOLEAN								IsLocked( ) const;
		void									SetLocked( BOOLEAN fLocked );

		BOOLEAN								CanChangeLockState( ) const;
		void									SetCanChangeLockState( BOOLEAN fCanChangeLockState );

		BOOLEAN								ClipToParent( ) const;
		void									SetClipToParent( BOOLEAN fClipToParent );

		BOOLEAN								IsEditable( ) const;

		YZLayer								GetZLayer( ) const;
		void									SetZLayer( YZLayer layer );

		YUniqueId							GetUniqueId( ) const;
		void									SetUniqueId( YUniqueId UniqueId );

	// Operations
	public :
		RComponentAttributes&			operator=( const RComponentAttributes& rhs );
		RComponentAttributes				operator&( const RComponentAttributes& rhs ) const;
		void									operator&=( const RComponentAttributes& rhs );

		void									Write( RArchive& archive ) const;
		void									Read( RArchive& archive );
								 
	// PS 6.0 and later:
	// write & read extended component attributes separately for
	// backward and forward compatibility
		void									WriteExtendedAttributes( RArchive& archive ) const;
		void									ReadExtendedAttributes( RArchive& archive );
		void									SetDefaultExtendedComponentAttributes( const YComponentType& hType );


	// Member data
	private :
		BOOLEAN								m_fSelectable;
		BOOLEAN								m_fMovable;
		BOOLEAN								m_fResizable;
		BOOLEAN								m_fRotatable;
		BOOLEAN								m_fFlippable;
		BOOLEAN								m_fColorable;
		BOOLEAN								m_fShadowable;
		BOOLEAN								m_fCanHaveGradient;
		BOOLEAN								m_fLocked;
		BOOLEAN								m_fCanChangeLockState;
		BOOLEAN								m_fClipToParent;
		BOOLEAN								m_fDeletable;
		YZLayer								m_ZLayer;
		YUniqueId							m_UniqueId;

	// PS 6.0 and later:
	// extended component attributes written separately via WriteExtendedAttributes
	// for backward and forward compatibility
	private :
		BOOLEAN								m_fGlowable;
		BOOLEAN								m_fAlphaMaskable;
		BOOLEAN								m_fVignetteable;

#ifdef	TPSDEBUG
	//	Validate code
	public :
		virtual void						Validate( ) const;
#endif	//	TPSDEBUG
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif
