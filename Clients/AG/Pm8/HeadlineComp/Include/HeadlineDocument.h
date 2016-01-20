// ****************************************************************************
//
//  File Name:			HeadlineDocument.h
//
//  Project:			Headline Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RHeadlineDocument class
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlineDocument.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINEDOCUMENT_H_
#define		_HEADLINEDOCUMENT_H_

#include		"ComponentDocument.h"

// ****************************************************************************
//
//  Class Name:	RHeadlineDocument
//
//  Description:	The Headline document.
//
// ****************************************************************************
//
class RHeadlineDocument : public RComponentDocument
	{
	// Construction & Destruction
	public :
													RHeadlineDocument( RApplication* pApp,
																		 const RComponentAttributes& componentAttributes,
																		 const YComponentType& componentType,
																		 BOOLEAN fLoading );

		virtual									~RHeadlineDocument( );

	// Operations
	public :
		virtual void							Render( RDrawingSurface& drawingSurface,
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

		virtual void							Write( RChunkStorage& storage, EWriteReason reason ) const;
		virtual void							Read( RChunkStorage& storage, EReadReason reason );
		virtual void							ResetToDefaultSize( RDocument* pParentDocument );
		void										GetHeadlineData( HeadlineDataStruct* pData );
		void										SetHeadlineData( HeadlineDataStruct* pData );
		void										GetHeadlineText( RMBCString& text );
		void										SetHeadlineText( const RMBCString& text );
		void										FreeDocumentContents( );
		virtual RComponentView*				CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView );		
		RHeadlinePersistantObject&			HeadlineObject( );
		const RHeadlinePersistantObject&	HeadlineObject( ) const;
		virtual RRealSize						GetDataSize( ) const;

	//	Private Members
	private :
		RHeadlinePersistantObject			m_HeadlineObject;

// Debugging support
#ifdef	TPSDEBUG
	public :
		virtual void							Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::HeadlineObject( )
//
//  Description:		Return a reference to the Headline Object
//
//	 Returns:			m_HeadlineObject
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RHeadlinePersistantObject&	RHeadlineDocument::HeadlineObject( )
	{
	return m_HeadlineObject;
	}

#endif		// _HEADLINEDOCUMENT_H_
