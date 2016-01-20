// ****************************************************************************
//
//  File Name:			DragDropDataTransfer.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the drag&drop data transfer classes
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
//  $Logfile:: /PM8/Framework/Include/DragDropDataTransfer.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DRAGDROPDATATRANSFER_H_
#define		_DRAGDROPDATATRANSFER_H_

#ifdef _WINDOWS
	#include "MfcDataTransfer.h"
	#define SourceBaseClass RMFCDataTransferSource
	#define TargetBaseClass RMFCDataTransferTarget
#else

#endif

const YModifierKey kCopyModifierKey = kModifierControl;

#ifndef	_COMPONENTCOLLECTION_H_
#include "ComponentCollection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;
class RDrawingSurface;

// ****************************************************************************
//
// Class Name:			RDragDropDataSource
//
// Description:		Encapsulates drag&drop as a data transfer source
//
// ****************************************************************************
//
class FrameworkLinkage RDragDropDataSource : public SourceBaseClass
	{
	// Construction & Destruction
	public :
												RDragDropDataSource( COleDataObject* pOleDataObject );

	// Operations
	public :
		void									Render( RDrawingSurface* pDrawingSurface, const R2dTransform& transform ) const;

	// Member data
	private :
		RComponentDocument*				m_pTransferDocument;
		RView*								m_pDropTargetView;
		RComponentCollection				m_ComponentCollection;
		RRealPoint							m_LastPoint;
		RRealPoint							m_MouseDownPoint;
	};

// ****************************************************************************
//
// Class Name:			RDragDropDataTarget
//
// Description:		Encapsulates drag&drop as a data transfer Target
//
// ****************************************************************************
//
class FrameworkLinkage RDragDropDataTarget : public TargetBaseClass
	{
	// Construction & Destruction
	public :
												RDragDropDataTarget( RDataRenderer* pDataRenderer );
												~RDragDropDataTarget( );

	// Operations
	public :
		YDropEffect							DoDragDrop( YDropEffect allowableEffects = kDropEffectMove | kDropEffectCopy );
	};

// ****************************************************************************
//
// Class Name:			RDragDropInfo
//
// Description:		Drag&Drop info block
//
// ****************************************************************************
//
class FrameworkLinkage RDragDropInfo
	{
	public :
	// Construction
												RDragDropInfo( );
												RDragDropInfo( RView* pSourceView,
																   const RRealPoint& dragStartPoint,
																	const RRealSize& dragOffset,
																	BOOLEAN fUseTargetDefaultSize );

	// Operations
	public :
		void									Copy( RDataTransferTarget& dataTarget ) const;
		void									Paste( const RDataTransferSource& dataSource );

	// Local structure which encapulates the actual data
	struct RInternalDataBlock
		{
		RView*								m_pSourceView;
		RRealPoint							m_DragStartPoint;
		RRealSize							m_DragOffset;
		BOOLEAN								m_fUseTargetDefaultSize;
		};

		RInternalDataBlock				m_Data;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _DRAGDROPDATATRANSFER_H_
