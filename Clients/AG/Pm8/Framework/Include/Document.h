// ****************************************************************************
//
//  File Name:			Document.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RDocument class
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
//  $Logfile:: /PM8/Framework/Include/Document.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DOCUMENT_H_
#define		_DOCUMENT_H_

#include		"ComponentCollection.h"
#include		"ComponentInfoInterface.h"

#include		"URLList.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Forward References
class RAction;
class RApplication;
class RView;
class RComponentView;
class RComponentDocument;
class RStandaloneDocument;
class RChunkStorage;
class RPrinter;

//	Typedefs
typedef		RList<RView*>							YViewCollection;
typedef		YViewCollection::YIterator			YViewIterator;
typedef		RComponentCollection					YComponentCollection;
typedef		YComponentCollection::YIterator	YComponentIterator;
typedef		YCollectionIndex						YViewIndex;
typedef		YCollectionIndex						YComponentIndex;

// ****************************************************************************
//
//  Class Name:	RDocument
//
//  Description:	The base Renaissance document class. Component and
//						standalone document classes are derived off of this.
//						User document classes should never inherit directly
//						from this class.
//
//						The document class is responsible for providing an interface
//						to its data.
//
// ****************************************************************************
//
class FrameworkLinkage RDocument : public RCommandTarget, public IComponentInfo
	{
	// Construction & Destruction & Intialization
	public :
													RDocument( );
		virtual									~RDocument( );
		virtual void							Initialize( RApplication* pApp );

	// Operations
	public :
		virtual void 							Render( RDrawingSurface& drawingSurface,
															  const R2dTransform& transform,
															  const RIntRect& rcRender,
															  const RRealSize& size ) const;

		virtual void 							RenderBehindSoftEffect( RDrawingSurface& drawingSurface,
															  const R2dTransform& transform,
															  const RIntRect& rcRender,
															  const RRealSize& size ) const;
		virtual void 							RenderShadow( RDrawingSurface& drawingSurface,
															  const R2dTransform& transform,
															  const RIntRect& rcRender,
															  const RRealSize& size,
															  const RSolidColor& shadowColor ) const;

		BOOLEAN									IsActive( ) const;
		virtual RAction*						CreateScriptAction( const YActionId& actionId, RScript& script );
		virtual void							FreeDocumentContents( );
		virtual BOOLEAN						CanIncorporateData( const RDataTransferSource& dataSource ) const;
		virtual BOOLEAN						SendAction( RAction* pAction ) = 0;
		virtual void							BeginMultiUndo( ) = 0;
		virtual void							CancelMultiUndo( ) = 0;
		virtual void							EndMultiUndo( uWORD uwUndoStringId, uWORD uwRedoStringId ) = 0;
		virtual const YActionId&			GetNextActionId( ) const = 0;
		virtual void							AddComponent( RComponentDocument* pComponentDocument );
		void										RemoveComponent( RComponentDocument* pComponentDocument );
		virtual void							InvalidateAllViews( BOOLEAN fErase = TRUE );
		void										AddRView( RView* pView );
		virtual void							DeleteRView( RView* pView );
		const RFontList&						GetFontList( ) const;
		RFontList&								GetFontList( );
		const RURLList&							GetURLList( ) const;
		RURLList&								GetURLList( );
		virtual YViewIndex					GetViewIndex( RView* pView ) const;
		virtual RView*							GetViewByIndex( YViewIndex index ) const;
		YComponentIndex						GetComponentIndex( RComponentDocument* pView ) const;
		RComponentDocument*					GetComponentByIndex( YComponentIndex index ) const;
		virtual void							Read( RChunkStorage& storage, EReadReason reason );
		virtual void							Write( RChunkStorage& storage, EWriteReason reason ) const;
		void										XUpdateAllViews( EUpdateTypes updateType, uLONG lParam );
		virtual void							OnXActivate( BOOLEAN fActivating );
		void										TransferComponents( RDocument* pSourceDocument,
																			  const RComponentCollection& componentCollection,
																			  const RRealRect& sourceRect,
																			  const RRealRect& destRect );
		virtual BOOLEAN						CanInsertComponent( const YComponentType& componentType ) const = 0;

		virtual RComponentAttributes		GetDefaultObjectAttributes( const YComponentType& componentType ) const ;

	// Accessor functions
	public :
		RApplication* 							GetApplication( ) const;
		YViewIterator							GetViewCollectionStart( ) const;
		YViewIterator							GetViewCollectionEnd( ) const;
		virtual RView*							GetActiveView( ) const;
		YComponentIterator					GetComponentCollectionStart( ) const;
		YComponentIterator					GetComponentCollectionEnd( ) const;
		void										DeleteComponentCollection( );
		virtual RComponentDocument*		GetComponentWithUniqueId( YUniqueId uniqueId ) const;
		int										GetNumComponents( ) const;

	// Member data
	protected :
		RApplication*							m_pApplication;
		YViewCollection							m_ViewCollection;
		RFontList								m_FontList;
		RURLList								m_URLList;
		BOOLEAN									m_fActive;
		static BOOLEAN							m_fWarnedAboutMissingComponent;

	//	Member Data
	private :
		YComponentCollection					m_ComponentCollection;

	// Command map
	public :
		virtual RCommandMapBase*			GetRCommandMap( ) const;

	private :
		static RCommandMap<RDocument, RCommandTarget> m_CommandMap;
		friend class RCommandMap<RDocument, RCommandTarget>;

#ifdef	_WINDOWS
	public :
		virtual CDocument*					GetCDocument( ) const = 0;
#endif

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void							Validate( ) const = 0;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RDocument::GetRCommandMap( )
//
//  Description:		Gets a pointer to this classes command map.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCommandMapBase* RDocument::GetRCommandMap( ) const
	{
	return &m_CommandMap;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetDefaultObjectAttributes( )
//
//  Description:		Retrieves the default attributes of the specified
//                   component type.  At this level, it is just the
//                   default RComponentAttributes regardless of component
//							type.
//
//  Returns:			RCompnentAttributes
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RComponentAttributes RDocument::GetDefaultObjectAttributes( const YComponentType& /* componentType */ ) const 
{
	return RComponentAttributes() ;
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _DOCUMENT_H_
