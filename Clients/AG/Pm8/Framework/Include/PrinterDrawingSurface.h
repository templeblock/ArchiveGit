// ****************************************************************************
//
//  File Name:			PrinterDrawingSurface.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas & R. Hood
//
//  Description:		Declaration of the RPrinterDrawingSurface class
//
//  Portability:		Win32 or Mac dependent
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
//  $Logfile:: /PM8/Framework/Include/PrinterDrawingSurface.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PRINTERDRAWINGSURFACE_H_
#define		_PRINTERDRAWINGSURFACE_H_

#ifdef _WINDOWS
	#include		"DcDrawingSurface.h"
	#define		PrinterDrawingSurfaceBaseClass RDcDrawingSurface
#endif

#ifdef MAC
	#include		"GpDrawingSurface.h"
	#define		PrinterDrawingSurfaceBaseClass RGpDrawingSurface
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RCancelPrintingSignal;

// ****************************************************************************
//
//  Class Name:	RDocInfo
//
//  Description:	Class to hold information about a document
//
// ****************************************************************************
//
class FrameworkLinkage RDocInfo
	{
	public:
	//	Constructors:
		RDocInfo( );
		void Initialize( const RMBCString& rDocName, const RMBCString& rFileName );
		
	//	Members:
#ifdef _WINDOWS
		DOCINFO m_DocInfo;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RPrinterDrawingSurface
//
//  Description:	Printer drawing surface class
//
// ****************************************************************************
//
class FrameworkLinkage RPrinterDrawingSurface : public PrinterDrawingSurfaceBaseClass
	{
	// Construction, destruction & Initialization
	public :
#ifdef MAC
												RPrinterDrawingSurface( const THPrint hPrintInfo );
#endif
#ifdef _WINDOWS
												RPrinterDrawingSurface( RCancelPrintingSignal* pCancelPrintingSignal );
#endif
		virtual								~RPrinterDrawingSurface( );
#ifdef MAC
		virtual BOOLEAN					Initialize( const GrafPtr pPort, 
																const RIntRect& boundingRect, 
																const BOOLEAN fUseMaxResolution );
#endif
#ifdef _WINDOWS
		virtual BOOLEAN					Initialize( const RIntRect& boundingRect );
		virtual BOOLEAN					Initialize( HDC hOutputDC, HDC hAttributeDC );
#endif

	// Accessors
	public :
		virtual RIntSize					GetDPI( ) const;
		RIntSize								GetMaxDPI( const BOOLEAN fDescrete ) const;
		RIntSize								GetMinDPI( const BOOLEAN fDescrete ) const;
		BOOLEAN								fIsPostScript( ) const;

	// Functions to set the state of the device
	public :
		virtual BOOLEAN					BeginPage( );
		virtual BOOLEAN					EndPage( );
		virtual BOOLEAN					BeginDoc( );
		virtual BOOLEAN					EndDoc( );
		virtual BOOLEAN					PageStarted( );
		virtual BOOLEAN					DocStarted( );
		virtual RDocInfo&					GetDocInfo( );

	// Operations
	public :
		virtual void						MoveTo( const RIntPoint& point );

	// Implementation
	private :
		virtual void						SolidLineTo( const RIntPoint& point );
		virtual void						SolidPolyline( const RIntPoint* pPoints, YPointCount numPoints );
		virtual void						SolidPolylineTo( const RIntPoint* pPoints, YPointCount numPoints );
		virtual void						SolidFillRectangle( const RIntRect& rect );
		virtual void						SolidPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						SolidFillPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						SolidPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						SolidFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						SolidFramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount ); 
		virtual void						DeviceInvertRectangle( const RIntRect& rect );
		virtual void						DeviceInvertPolygon( const RIntPoint* pVertices, YPointCount numVertices );
		virtual void						DeviceInvertPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount );
		virtual void						CreatePen( );
		virtual void						CreateBrush( );

		virtual BOOLEAN					IsPrintingCanceled( );
		void									CheckPrintingCanceled( );

#ifdef MAC
		BOOLEAN								SetDPI( const RIntSize& dpi );
		void									DefaultDPI( );
#endif

#ifdef MAC
#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
#endif

	// Member data
	private :
#ifdef MAC
		TPPrPort				 				m_pPrintPort;
#endif
		RIntRect 							m_rcBounds;
	protected :
#ifdef MAC
		THPrint 								m_hPrintInfo;
#endif
		RCancelPrintingSignal*			m_pCancelPrintingSignal;
		BOOLEAN								m_fResolutionChanged;
		BOOLEAN								m_fPageStarted;			//	TRUE: a page has been started on the output device
		BOOLEAN								m_fDocStarted;				//	TRUE: a document has been started on the output device
		RDocInfo								m_DocInfo;					//	name and destination file of doc for printing
	};

// ****************************************************************************
//
//  Class Name:	RCancelPrintingSignal
//
//  Description:	Object that can be used to signal when printing has been
//						canceled.
//
// ****************************************************************************
//
class RCancelPrintingSignal
	{
	// Construction
	public :
												RCancelPrintingSignal( );

	// Operations
	public :
		void									CancelPrinting( );
		BOOLEAN								IsPrintingCanceled( );

	// Member data
	private :
		BOOLEAN								m_fPrintingCanceled;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RDocInfo::RDocInfo( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RDocInfo::RDocInfo( ) 
		{
#ifdef _WINDOWS
		m_DocInfo.cbSize = sizeof( DOCINFO );
		m_DocInfo.lpszDocName = NULL;
		m_DocInfo.lpszOutput = NULL;
		m_DocInfo.lpszDatatype = NULL;
		m_DocInfo.fwType = 0;
#endif
		}

// ****************************************************************************
//
//  Function Name:	RDocInfo::Initialize( const RMBCString& rDocName, const RMBCString& rFileName )
//
//  Description:		Install the doc info's doc name and output file name.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RDocInfo::Initialize( const RMBCString& rDocName, const RMBCString& rFileName )
		{
#ifdef _WINDOWS
		m_DocInfo.lpszDocName = rDocName;
		m_DocInfo.lpszOutput = rFileName;
#endif
		}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::GetDocInfo( )
//
//  Description:		m_DocInfo.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RDocInfo& RPrinterDrawingSurface::GetDocInfo( )
	{
	return m_DocInfo;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::DocStarted( )
//
//  Description:		m_fDocStarted.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RPrinterDrawingSurface::DocStarted( )
	{
	return m_fDocStarted;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::PageStarted( )
//
//  Description:		m_fPageStarted.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RPrinterDrawingSurface::PageStarted( )
	{
	return m_fPageStarted;
	}

// ****************************************************************************
//
//  Function Name:	RCancelPrintingSignal::IsPrintingCanceled( )
//
//  Description:		Accessor.
//
//  Returns:			TRUE if printing has been canceled
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RCancelPrintingSignal::IsPrintingCanceled( )
	{
	return m_fPrintingCanceled;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::IsPrintingCanceled( )
//
//  Description:		Accessor.
//
//  Returns:			TRUE if printing has been canceled
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RPrinterDrawingSurface::IsPrintingCanceled( )
	{
	if( m_pCancelPrintingSignal && m_pCancelPrintingSignal->IsPrintingCanceled( ) )
		return TRUE;
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::CheckPrintingCanceled( )
//
//  Description:		Throws a print canceled exception if printing has been
//							canceled.
//
//  Returns:			Nothing
//
//  Exceptions:		kPrintingCanceled
//
// ****************************************************************************
//
inline void RPrinterDrawingSurface::CheckPrintingCanceled( )
	{
	if( IsPrintingCanceled( ) )
		throw kPrintingCanceled;
	}

// ****************************************************************************
//
//  Function Name:	RCancelPrintingSignal::RCancelPrintingSignal( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCancelPrintingSignal::RCancelPrintingSignal( )
	: m_fPrintingCanceled( FALSE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCancelPrintingSignal::CancelPrinting( )
//
//  Description:		Signal that printing has been canceled
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RCancelPrintingSignal::CancelPrinting( )
	{
	m_fPrintingCanceled = TRUE;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _PRINTERDRAWINGSURFACE_H_
