// ****************************************************************************
//
//  File Name:			ComponentCollection.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RComponentCollection class
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
//  $Logfile:: /PM8/Framework/Include/ComponentCollection.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTCOLLECTION_H_
#define		_COMPONENTCOLLECTION_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDocument;
class RComponentDocument;
class RDrawingSurface;
class RDataTransferSource;
class RDataTransferTarget;
class RBitmapImage;
class RVectorImage;
class RCompositeSelection;
class RView;
class RComponentAttributes;

// ****************************************************************************
//
//  Class Name:	RComponentCollection
//
//  Description:	Class that maintains a collection to components
//
// ****************************************************************************
//
class FrameworkLinkage RComponentCollection : public RList<RComponentDocument*>
	{
	// Construction
	public :
												RComponentCollection( );
												RComponentCollection( const RCompositeSelection& selection );
												RComponentCollection( const RComponentCollection& rhs, RDocument* pParentDocument, BOOLEAN& fObjectConverted );

	// Operations
	public :
		RRealRect							GetBoundingRect( ) const;
		void									DrawDragTrackingRect( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RView& targetView ) const;
		void									DeleteAllComponents( );
		BOOLEAN								Paste( const RDataTransferSource& dataSource, RDocument* pParentDocument );
		void									Copy( RDataTransferTarget& dataTarget, YDataFormat dataFormat ) const;
		void									PromiseFormats( RDataTransferTarget& dataTarget ) const;
		RRealSize							ScaleToFitInView( RView* pTargetView );
		void									CombineAttributes( RComponentAttributes& attributes ) const;
		RRealRect							GetTrackingFeedbackBoundingRect( const R2dTransform& transform );
		void									Write( RChunkStorage& storage, EWriteReason reason ) const;
		BOOLEAN								Read( RChunkStorage& storage, RDocument* pParentDocument );
		void									SortByZOrder( );
		void									GetSizeLimits( RRealSize& minScaleFactor, RRealSize& maxScaleFactor ) const;
		BOOLEAN								PasteDataIntoComponent( const RDataTransferSource& dataSource, RDocument* pParentDocument, YDataFormat format );

	// Implementation
	private :
		RBitmapImage*						GetBitmapRepresentation( ) const;
		RVectorImage*						GetPictureRepresentation( ) const;
		void									Render( RDrawingSurface& drawingSurface ) const;
		BOOLEAN								CreateComponents( const RDataTransferSource& dataSource, RDocument* pParentDocument );
		void									CopyComponents( RDataTransferTarget& dataTarget ) const;
		void									CopyText( RDataTransferTarget& dataTarget ) const;
		void									CopyRTF( RDataTransferTarget& dataTarget ) const;
		void									CopyBitmap( RDataTransferTarget& dataTarget, YDataFormat dataFormat ) const;
		void									CopyPicture( RDataTransferTarget& dataTarget, YDataFormat dataFormat ) const;
	};

#ifdef _WINDOWS

// ****************************************************************************
//
//  Class Name:	RRTFConcatinator
//
//  Description:	Class to do all the grunt work of concatinating RTF streams.
//						On Windows, this is just a wrapper around the RTF control
//
// ****************************************************************************
//
class RRTFConcatinator
	{
	// Construction
	public :
												RRTFConcatinator( );

	// Operations
	public :
		void									Paste( const RDataTransferSource& dataSource );
		void									Copy( RDataTransferTarget& dataTarget );

	// Implementation
	private :
		static DWORD CALLBACK			StreamInCallback( DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb );
		static DWORD CALLBACK			StreamOutCallback( DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb );

	// Data
	private :
		CRichEditCtrl						m_RichEditControl;
		uBYTE*								m_pData;
		YDataLength							m_DataLength;
		DWORD									m_CurrentPosition;
		CString								m_String;
	};

#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _COMPONENTCOLLECTION_H_
