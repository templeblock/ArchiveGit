// ****************************************************************************
//
//  File Name:			DataTransfer.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Data transfer base classes
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
//  $Logfile:: /PM8/Framework/Include/DataTransfer.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DATATRANSFER_H_
#define		_DATATRANSFER_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#ifdef _WINDOWS
	const YDataFormat kComponentFormat = ::RegisterClipboardFormat( "PSD 5.0 Component" );
	const YDataFormat kDragDropFormat = ::RegisterClipboardFormat( "PSD 5.0 Drag&Drop" );
	const YDataFormat kPictureFormat = CF_METAFILEPICT;
	const YDataFormat kEnhancedMetafileFormat = CF_ENHMETAFILE;
	const YDataFormat kBitmapFormat = CF_BITMAP;
	const YDataFormat kDibFormat = CF_DIB;
	const YDataFormat kTextFormat = CF_TEXT;
	const YDataFormat kRTFFormat = ::RegisterClipboardFormat( CF_RTF );

	#define DataRendererBaseClass COleDataSource
#else
	const YDataFormat kComponentFormat = 'PSD5';
	const YDataFormat kDragDropFormat = 'PSDD';
	const YDataFormat kPictureFormat = 'PICT';
	const YDataFormat kBitmapFormat = 'BMP ';
	const YDataFormat kTextFormat = 'TEXT';
	const YDataFormat kRTFFormat	= 'RTF '

	#define DataRendererBaseClass RObject
#endif

typedef YDataFormatCollection::YIterator YDataFormatIterator;

class RDataRenderer;

// ****************************************************************************
//
// Class Name:			RDataTransferSource
//
// Description:		Abstract base class for data transfer sources
//
// ****************************************************************************
//
class FrameworkLinkage RDataTransferSource : public RObject
	{
	// Operations
	public :
		virtual void						GetData( YDataFormat dataFormat, uBYTE*& pData, YDataLength& dataLength ) const = 0;
		virtual HNATIVE					GetData( YDataFormat dataFormat ) const = 0;
		virtual BOOLEAN					IsFormatAvailable( YDataFormat dataFormat ) const = 0;
		virtual void						BeginEnumFormats( ) const = 0;
		virtual BOOLEAN					GetNextFormat( YDataFormat& dataFormat ) const = 0;
	};

// ****************************************************************************
//
// Class Name:			RDataTransferTarget
//
// Description:		Abstract base class for data transfer targets
//
// ****************************************************************************
//
class FrameworkLinkage RDataTransferTarget : public RObject
	{
	// Operations
	public :
		virtual void						SetData( YDataFormat dataFormat, const uBYTE* pData, YDataLength dataLength ) = 0;
		virtual void						SetData( YDataFormat dataFormat, HNATIVE handle ) = 0;
		virtual void						PromiseData( YDataFormat dataFormat ) = 0;
	};

// ****************************************************************************
//
// Class Name:			RDataRenderer
//
// Description:		Abstract base class for data renderers
//
// ****************************************************************************
//
class FrameworkLinkage RDataRenderer : public DataRendererBaseClass
	{
	// Operations
	public :
		virtual BOOLEAN					RenderData( RDataTransferSource* pDataSource, RDataTransferTarget* pDataTarget, YDataFormat dataFormat ) = 0;

#ifdef _WINDOWS
		virtual BOOL						OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium );
#endif
	};

// ****************************************************************************
//
// Class Name:			RMemoryDataTransfer
//
// Description:		Memory data transfer target.
//
// ****************************************************************************
//
class FrameworkLinkage RMemoryDataTransfer : public RDataTransferTarget, public RDataTransferSource
	{
	// Construction & Destruction
	public :
												RMemoryDataTransfer( );
												RMemoryDataTransfer( const RMemoryDataTransfer& rhs );
		virtual								~RMemoryDataTransfer( );

	// Target operations
	public :
		virtual void						SetData( YDataFormat dataFormat, const uBYTE* pData, YDataLength dataLength );
		virtual void						SetData( YDataFormat dataFormat, HNATIVE handle );
		virtual void						PromiseData( YDataFormat dataFormat );

	// Source operations
	public :
		virtual void						GetData( YDataFormat dataFormat, uBYTE*& pData, YDataLength& dataLength ) const;
		virtual HNATIVE					GetData( YDataFormat dataFormat ) const;
		virtual BOOLEAN					IsFormatAvailable( YDataFormat dataFormat ) const;
		virtual void						BeginEnumFormats( ) const;
		virtual BOOLEAN					GetNextFormat( YDataFormat& dataFormat ) const;

	// Member data
	private :
		HNATIVE								m_hData;
		uBYTE*								m_pData;
		YDataLength							m_DataLength;
		YDataFormat							m_DataFormat;
		BOOLEAN								m_fEnumerating;

#ifdef	TPSDEBUG
	public :
		virtual void						Validate( ) const { };
#endif	//	TPSDEBUG
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _DATATRANSFER_H_
