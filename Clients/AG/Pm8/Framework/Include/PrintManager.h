// ****************************************************************************
//
//  File Name:			PrinterManager.h
//
//  Project:			Renaissance Application
//
//  Author:				R. Hood
//
//  Description:		Declaration of the RPrinter class
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
//  $Logfile:: /PM8/Framework/Include/PrintManager.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PRINTMANAGER_H_
#define		_PRINTMANAGER_H_

#include "Printer.h"
#include "PrinterStatus.h"
#include "ThreadSupport.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Forward declarations.
class RPrinterDatabase;
class RDocument;
class RDocumentCollection;

// temporary list of attached documents
class RDocumentCollection: public RArray<RDocument*> {};

//	Show a printer calibration status dialog.
const BOOLEAN kShowStatus = TRUE;

//	Number of printer test page elements per page.
const YCounter kNumPrinterTesPagePrintElements = 6;

// ****************************************************************************
//
//  Class Name:	RPrintManager
//
//  Description:	Encapsulates info for all currently connected printers.
//
// ****************************************************************************
//
class FrameworkLinkage RPrintManager : public RObject
	{
	// Construction, destruction & Initialization
	public :
										RPrintManager( );
		virtual						~RPrintManager( );

	// public operations
	public :
		static void					Startup( );
		void							Shutdown( );
		void							PrinterChanged( const RPrinter* pPrinter, const uLONG ulReason );
		RPrinterCollection*		GetPrinterCollection( );
		RPrinter*					GetDefaultPrinter( );
		YCounter						GetDefaultPrinterIndex( );
		YCounter						GetPrinterIndex( const RPrinter* pPrinter );
		void							SetDefaultPrinter( const YCounter yNewDefault );
		uLONG							AttachedPrintersChanged( const RMBCString& rInfo );
		static RPrintManager&	GetPrintManager( );
		virtual void				TestPrinter( RPrinter* pPrinter );
		virtual void				TestPrinter( );
		virtual BOOLEAN			CalibratePrinter( RPrinter* pPrinter, BOOLEAN fShowStatus = !kShowStatus, YCounter numCalibrationPages = 1 );
		virtual BOOLEAN			ConfirmPrinter( RPrinter* pPrinter, const RIntRect& rNewPrintableArea, BOOLEAN fShowStatus = !kShowStatus, YCounter numConfirmationPages = 1 );
		BOOLEAN						CanTestDefaultPrinter( );
		void							UpdatePrinterFromDatabase( RPrinter* pPrinter );
		void							UpdatePrinterFromDatabase( RPrinter* pPrinter, const RIntRect& rNewPrintableArea );
		RPrinterStatus*			GetStatus( );
		BOOLEAN						GetSystemDefaultPrinterName( RMBCString& rDriverName );
		BOOLEAN						SetDefaultPrinter( const RMBCString& rDriverName );
		RPrinter*					FindPrinter( const RPrinter* pPrinter, const uLONG ulCompareFlags = kCompareStandard );
		RPrinter*					FindPrinter( const RMBCString& rDriverName, const uLONG ulCompareFlags = kCompareStandard );
		RPrinter*					AddPrinter( const RPrinter* pPrinter );
		RPrinter*					DeletePrinter( const RPrinter* pPrinter );
		BOOLEAN						GetAttachedPrinters( );
		virtual void				PrintTestPage( RDrawingSurface& drawingSurface
														, const R2dTransform& transform
														, const RPrinter& printer
														, const RIntRect& rcRender
														, RPrinterStatus* pPrinterStatus = NULL ) const;
		virtual void				PrintConfirmationPage( RDrawingSurface& drawingSurface
														, const R2dTransform& transform
														, const RPrinter& printer
														, const RIntRect& rcRender
														, const RIntRect& printableArea
														, RPrinterStatus* pPrinterStatus = NULL ) const;
		RIntRect						CompensatePrintableArea( const RIntRect rSideCalibration
														, const RIntSize rCenterOffset
														, RPrinter* pPrinter );

		static YThreadResult		InitializePrintManagerThreadFunction( void* pThreadParam );
		static BOOLEAN				IsPrintManagerInitialized( );

	// Member data
	protected :
		RPrinterCollection		m_AttachedPrinters;	//	attached printers
		YCounter						m_CurrentPrinter;		//	index of default printer
		static REvent				m_InitializedEvent;
		static BOOLEAN				m_fPrintManagerInitialized;	//	TRUE: the print manager has been prepared for use
		static RPrintManager		m_PrintManager;		//	global list of all printers
		RPrinterDatabase*			m_pUserDatabase;		//	list of actual printable areas for printers determined by user
		RPrinterDatabase*			m_pVendorDatabase;	//	list of actual printable areas for printers determined by vendor
		RPrinterStatus				m_Status;				//	current printing status
		RDocumentCollection*		m_pOpenDocuments;		//	temporary home for list of open documents until it is available in the framework

	friend class RPrinter;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void				Validate( ) const;
#endif
	};

void	DrawCalibrationText( RDrawingSurface& drawingSurface
			, const R2dTransform& transform
			, const RIntPoint& rTopLeft
			, const RIntSize& rMarkerSize
			, const char* pMarkerText
			, const YCounter yOutStringLength
			, const BOOLEAN fFrame
			, const int nDrawTextMode );

void	DrawTestStringOne( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
		   , const RIntRect& rTopTitleRect, const RIntRect& rBottomTitleRect, RIntRect& rTitleRect );

void	DrawPaperNameString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RPrinter& printer, const RIntRect& pageArea, const RIntRect& rTitleRect );

void	DrawTitleString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& pageArea, const RIntRect& rTitleRect );

void	DrawDriverNameString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RPrinter& printer, const RIntRect& pageArea, const RIntRect& rTitleRect );

void	DrawSideMarkers( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
		   , const RPrinter& printer );

void	DrawCenterMarkers( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& pageArea );


// ****************************************************************************
//
//  Function Name:	RPrintManager::IsPrintManagerInitialized( )
//
//  Description:		Returns m_fPrintManagerInitialized
//
//  Returns:			see above
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RPrintManager::IsPrintManagerInitialized( )
	{
	return m_fPrintManagerInitialized;
	}

//	Test page sizes
const YCounter kNumSideMarkers								= 32;
const YCounter kNumCenterMarkers								= 16;
const YRealDimension kMarkWidth								= 1./4.;
const YRealDimension kMarkIncrement							= 1./32.;
const YRealDimension kSideMarkOffset						= 2.;
const YRealDimension kCalibrationTextOffset				= 2.;
const YRealDimension kCenterMarkWidth						= 1.;
const	YIntDimension	yMarkWidth								= ::InchesToLogicalUnits( kMarkWidth );
const RIntSize			kMarkSize( yMarkWidth, yMarkWidth );
const int				kMarkerDrawTextMode					= MacWinDos(UNUSED, DT_SINGLELINE | DT_BOTTOM | DT_CENTER | DT_NOCLIP,	UNUSED );

//	calibration title font info
const YRealDimension	kDrawingTestFontHeight				= 1.;			//	size of characters in inches
const YRealDimension	kDrawingTestFontWidth				= .75;
const YRealDimension	kCalibrationFontHeight				= .25;		//	size of characters in inches
const YRealDimension	kCalibrationFontWidth				= .14;
const int				kCalibrationFontEscapement			= 0;
const int				kCalibrationFontTopOrientation 	= 0;			//	specified in tenths of a degree counter clockwise from x axis
const int				kCalibrationFontLeftOrientation 	= 900;		//	specified in tenths of a degree counter clockwise from x axis
const int				kCalibrationFontBottomOrientation = 1800;		//	specified in tenths of a degree counter clockwise from x axis
const int				kCalibrationFontRightOrientation = 2700;		//	specified in tenths of a degree counter clockwise from x axis
const int				kCalibrationFontWeight				= MacWinDos( UNDEFINED, FW_DONTCARE, UNDEFINED );
const BOOLEAN			kCalibrationFontItalic				= TRUE;
const BOOLEAN			kCalibrationFontUnderline			= TRUE;
const BOOLEAN			kCalibrationFontStrikeOut			= TRUE;
const PSBYTE			kCalibrationFontFaceName			= "Times Roman";
const uBYTE				kCalibrationFontClipPrecision		= MacWinDos( UNDEFINED, CLIP_DEFAULT_PRECIS, UNDEFINED );
const uBYTE				kCalibrationFontQuality				= MacWinDos( UNDEFINED, DEFAULT_QUALITY, UNDEFINED );
const uBYTE				kCalibrationFontCharSet				= MacWinDos( UNDEFINED, ANSI_CHARSET, UNDEFINED );
const uBYTE				kCalibrationFontNamePrecision		= MacWinDos( UNDEFINED, OUT_TT_PRECIS, UNDEFINED );
const uBYTE				kCalibrationFontPitchAndFamily	= MacWinDos( UNDEFINED, DEFAULT_PITCH | FF_ROMAN, UNDEFINED );
const PSBYTE			kCalibrationTestFontFaceName		= "Courier";

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_PRINTMANAGER_H_