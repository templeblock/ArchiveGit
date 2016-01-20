// ****************************************************************************
//
//  File Name:			MfcDataTransfer.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		MFC implementation of the classes declared in DataTransfer.h
//
//  Portability:		MFC dependent
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
//  $Logfile:: /PM8/Framework/Include/MfcDataTransfer.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MFCDATATRANSFER_H_
#define		_MFCDATATRANSFER_H_

#ifndef		_DATATRANSFER_H_
#include		"DataTransfer.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RRenaissanceOleDataSource;
class RComponentCollection;
class RCompositeSelection;
class RView;

// ****************************************************************************
//
// Class Name:			RMFCDataTransferSource
//
// Description:		MFC implementation of the data transfer source interface
//
// ****************************************************************************
//
class FrameworkLinkage RMFCDataTransferSource : public RDataTransferSource
	{
	// Construction & Destruction
	public :
												RMFCDataTransferSource( COleDataObject* pOleDataObject );

	// Operations
	public :
		virtual void						GetData( YDataFormat dataFormat, uBYTE*& pData, YDataLength& dataLength ) const;
		virtual HNATIVE					GetData( YDataFormat dataFormat ) const;
		virtual BOOLEAN					IsFormatAvailable( YDataFormat dataFormat ) const;
		virtual void						BeginEnumFormats( ) const;
		virtual BOOLEAN					GetNextFormat( YDataFormat& dataFormat ) const;

	// Member data
	protected :
		COleDataObject*					m_pOleDataObject;
		BOOLEAN								m_fEnumerating;

#ifdef	TPSDEBUG
	public :
		virtual void						Validate( ) const;
#endif	//	TPSDEBUG
	};

// ****************************************************************************
//
// Class Name:			RMFCDataTransferTarget
//
// Description:		MFC implementation of the data transfer target interface
//
// ****************************************************************************
//
class FrameworkLinkage RMFCDataTransferTarget : public RDataTransferTarget
	{
	// Construction & Destruction
	public :
												RMFCDataTransferTarget( RDataRenderer* pDataRenderer );

	// Operations
	public :
		virtual void						SetData( YDataFormat dataFormat, const uBYTE* pData, YDataLength dataLength );
		virtual void						SetData( YDataFormat dataFormat, HNATIVE handle );
		virtual void						PromiseData( YDataFormat dataFormat );

	// Member data
	protected :
		COleDataSource*					m_pOleDataSource;
		BOOLEAN								m_fDelayedRenderingAvailable;

#ifdef	TPSDEBUG
	public :
		virtual void						Validate( ) const;
#endif	//	TPSDEBUG
	};

// ****************************************************************************
//
// Class Name:			RRenaissanceOleDropSource
//
// Description:		Class derived from MFC's COleDropSource to give feedback
//							durning a drag&drop operation.
//
// ****************************************************************************
//
class RRenaissanceOleDropSource : public COleDropSource
	{
	// Operations
	public :
		virtual SCODE						GiveFeedback( DROPEFFECT dropEffect );
	};

// ****************************************************************************
//
// Class Name:			RRenaissanceOleDropTarget
//
// Description:		Class derived from MFC's COleDropTarget to vector
//							drag&drop operations
//
// ****************************************************************************
//
class RRenaissanceOleDropTarget : public COleDropTarget
	{
	// Implementation
	private :
		virtual DROPEFFECT				OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
		virtual void						OnDragLeave( CWnd* pWnd );
		virtual DROPEFFECT				OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
		virtual BOOL						OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );

	// Members
	private :
		RView*								m_pDragDropView;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _MFCDATATRANSFER_H_
