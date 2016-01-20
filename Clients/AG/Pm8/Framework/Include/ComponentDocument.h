// ****************************************************************************
//
//  File Name:			ComponentDocument.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RComponentDocument class
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
//  $Logfile:: /PM8/Framework/Include/ComponentDocument.h                     $
//   $Author:: Rgrenfel                                                       $
//     $Date:: 3/08/99 1:12p                                                  $
// $Revision:: 2                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTDOCUMENT_H_
#define		_COMPONENTDOCUMENT_H_

#include		"Document.h"
#include		"ComponentAttributes.h"
#include		"UIContextData.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDrawingSurface;
class RStandaloneDocument;
class RUndoableAction;
class RComponentDocument;

const BOOLEAN kApplyToChildren = TRUE;

// ****************************************************************************
//
//  Class Name:		RComponentDocument
//
//  Description:		The base class for Component Renaissance documents.
//
//							The document class is responsible for providing an interface
//							to its data.
//
// ****************************************************************************
//
class FrameworkLinkage RComponentDocument : public RDocument
	{
	// Construction & Destruction & Intialization
	public :
													RComponentDocument( RApplication* pApp,
																			  const RComponentAttributes& componentAttributes,
																			  const YComponentType& componentType,
																			  BOOLEAN fLoading );
		
		virtual									~RComponentDocument( );

	// Operations
	public :
		virtual BOOLEAN						SendAction( RAction* pAction ) ;
		virtual void							BeginMultiUndo( );
		virtual void							CancelMultiUndo( );
		virtual void							EndMultiUndo( uWORD uwUndoStringId, uWORD uwRedoStringId );
		virtual const YActionId&			GetNextActionId( ) const;
		virtual RAction*						CreateScriptAction( const YActionId& actionId, RScript& script );		
		virtual void							DeleteRView( RView* pView );
		virtual RComponentView*				CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView ) = 0;
		virtual void							Read( RChunkStorage& storage, EReadReason reason );
		virtual void							Write( RChunkStorage& storage, EWriteReason reason ) const;
		const YComponentType&				GetComponentType( ) const;
		void								SetComponentType( const YComponentType& componentType );
		virtual void							GetDataFormatsSupportedForCopy( YDataFormatCollection& collection ) const;
		virtual void							Copy( YDataFormat dataFormat, RDataTransferTarget& dataTransferTarget );
		virtual void							Paste( YDataFormat dataFormat, const RDataTransferSource& dataTransferSource );		
		virtual void							ResetToDefaultSize( RDocument* pParentDocument );
		virtual void							InvalidateAllViews( BOOLEAN fErase = TRUE );
		virtual void							ReleaseResources( );
		virtual RComponentDocument *		Clone();

	// Accessors
	public :
		void										SetParentDocument( RDocument* pParentDocument );
		RDocument*								GetParentDocument( ) const;
		virtual RStandaloneDocument*		GetTopLevelDocument( ) const;
		RComponentView*						GetView( const RView* pParentView ) const;
		const RComponentAttributes&		GetComponentAttributes( ) const;
		void										SetComponentAttributes( const RComponentAttributes& componentAttributes );
		virtual RRealSize						GetMinObjectSize( const YComponentType& componentType, BOOLEAN fMaintainAspect = FALSE );
		virtual RRealSize						GetMaxObjectSize( const YComponentType& componentType );
		virtual RRealSize						GetDefaultObjectSize( const YComponentType& componentType );
		// This method is only defined for some objects, the return value MUST be checked.
		virtual BOOLEAN						GetDefaultSize( const RRealSize &sizePanel, RRealSize *psizeDefault )
			{ return FALSE; }
		virtual BOOLEAN						IsFrameable( ) const;
		void										SetTint(YTint tint, BOOLEAN fApplyToChildren = kApplyToChildren);
		YTint										GetTint() const;
		RUIContextData&						GetUIContextData( );
		virtual BOOLEAN						CanInsertComponent( const YComponentType& componentType ) const;
		virtual RRealSize						GetDataSize( ) const;

		void								SetFrameFillColor( RColor color )
											{ m_FrameFillColor = color; }
		RColor								GetFrameFillColor()
											{ return m_FrameFillColor; }

		BOOLEAN							HasURL() { return !m_URL.GetURL().IsEmpty(); }
		RURL								GetURL() { return m_URL; }
		void								SetURL( const RURL &url ) { m_URL = url; }


	//	Member Data
	protected:
		YComponentType							m_ComponentType;
		RColor									m_FrameFillColor;

	private :
		RDocument*								m_pParentDocument;
		RComponentAttributes					m_ComponentAttributes;
		YTint										m_Tint;
		RUIContextData							m_UIContextData;
		// Component level URL.  Do not use for text boxes except in applications
		// that don't support TextComponent embedded text URLs.  (PS and PressWriter)
		RURL										m_URL;

#ifdef	_WINDOWS
	public :
		virtual CDocument*					GetCDocument( ) const;
#endif

#ifdef	TPSDEBUG
	//	Validate code
	public :
		virtual void							Validate( ) const = 0;
#endif	//	TPSDEBUG
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef		_WINDOWS

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetCDocument( )
//
//  Description:		Retrieves the underlying MFC object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline CDocument* RComponentDocument::GetCDocument( ) const
	{
	TpsAssert( NULL, "Component documents are not derived from CDocument." );
	return NULL;
	}

#endif		// _WINDOWS

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _COMPONENTDOCUMENT_H_
