// ****************************************************************************
//
//  File Name:			Printer.h
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
//  $Logfile:: /PM8/Framework/Include/Printer.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PRINTER_H_
#define		_PRINTER_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RPrinterDrawingSurface;
class RArchive;
class RDrawingSurface;
class RCancelPrintingSignal;

//	Symbolic argument to RPrinter ctor telling it to use canned values.
const BOOLEAN			kTemplatePrinter						= TRUE;

//	Force a paper update from printer driver when specified.
const BOOLEAN			kUpdatePaperAreas						= TRUE;

//	Flags describing printer changes. This flag describes an action to take with an accompanying printer.
const uLONG kChangeNoReason = 0;						//	Default reason requiring no action
const uLONG kChangePrinterDeleted = 1;				//	Specified printer was deleted
const uLONG kChangePrinterAdded = 2;				//	Specified printer was added
const uLONG kChangePrinterChanged = 4;				//	Specified printer changed ( usually in it's properties )
const uLONG kChangeDefaultChanged = 8;				//	Default printer was changed to the specified printer
const uLONG kChangeNoPrintersAttached = 16;		//	No printers are attached and the specified printer is a template printer
const uLONG kChangeUseThisPrinter = 32;			//	Specified printer should be used as 

//	Flags describing printer properties updating
const uLONG kPrinterPropertiesNoFlags = 0;		//	Default reason requiring no action
const uLONG kPrinterPropertiesPromptUser = 1;	//	Bring up the printing dialog to ask the user for prpoerties
const uLONG kPrinterPropertiesForceUpdate = 2;	//	Always get new properties even if we already have some

//	Result type of printer comparisons. Use values between 0 (no confidence) and 100 (complete confidence).
typedef uBYTE YConfidence;
const YConfidence kNoConfidence = 0;
const YConfidence kCompleteConfidence = 100;

//	Flags describing printer comparisons.

//	For the standard test
//	we have a match if the attached printer has the same name, is on the same port 
//	and has the same printable area as the given printer. We specifically require/allow
//	different orientations to have different adjustments.

// NOTE: If new field comparison flags are added which refer to actual fields of RPrinter then
// the constant kCompareAllFields MUST be updated to include them. If new comparison modifier flags 
//	are added then kComparisonModifierFlags must be modified to include. If this is not done then
// the confidence calculation mechanism will break. 
const uLONG kCompareNothing = 0;
const uLONG kComparePrinterName = 1;								//	Examine the printer name
const uLONG kComparePortName = 2;									//	Examine the port name
const uLONG kCompareDriverName = 4;									//	Examine the driver name
const uLONG kComparePrintableArea = 8;								//	Examine the printable area
const uLONG kCompareDriverPrintableArea = 16;					//	Examine the printable area returned from the driver for portrait us letter
const uLONG kCompareRequireTotalConfidence = 32;				//	Make sure that confidence estimates returned by RPrinter::IsSimilar require all specified fields to compare exactly before returning a non-zero confidence.
const uLONG kCompareRequirePrintableAreasMatch = 64;			//	Make sure that confidence estimates returned by RPrinter::IsSimilar require all specified fields to compare exactly before returning a non-zero confidence.
const uLONG kCompareRequireDriverPrintableAreasMatch = 128;	//	Make sure that confidence estimates returned by RPrinter::IsSimilar require all specified fields to compare exactly before returning a non-zero confidence.
const uLONG kComparePrinterOrientation = 512;					// for database lookups the printer orientation must also be the same
const uLONG kComparePrinterPageType = 1024;						// Examine the printer page type ( us letter , a4 letter, etc. )
const uLONG kComparePortOrDriverName = kComparePortName | kCompareDriverName;
const uLONG kCompareStandard = kComparePortName | kCompareDriverName | kComparePrintableArea | kComparePrinterPageType;
const uLONG kCompareInPrinterDB = kComparePortName | kCompareDriverName | kCompareDriverPrintableArea | kComparePrinterOrientation | kComparePrinterPageType | kCompareRequireTotalConfidence;
const uLONG kCompareAllNames = kComparePrinterName | kComparePortName | kCompareDriverName;
const uLONG kCompareAllFields = kCompareAllNames | kComparePrintableArea | kComparePrinterOrientation | kComparePrinterPageType;
const uLONG kComparisonModifierFlags = kCompareRequireTotalConfidence | kCompareRequirePrintableAreasMatch | kCompareRequireDriverPrintableAreasMatch;

//	Symbolic argument flags
const BOOLEAN kReverseXForm = TRUE;


// ****************************************************************************
//
//  Class Name:	RPaperOrientations
//
//  Description:	Collect the two orientation sizes for a paper selection in a class.
//
// ****************************************************************************
//
class FrameworkLinkage RPaperOrientations
	{
	// Construction
	public:
												RPaperOrientations( );

	// Member data
	public :
		RIntSize								rLandscapeSize;
		RIntSize								rPortraitSize;
		RIntRect								rLandscapePrintableArea;
		RIntRect								rPortraitPrintableArea;
		BOOLEAN	m_fPortraitSizeValid;
		BOOLEAN	m_fLandscapeSizeValid;
		BOOLEAN	m_fPortraitPrintableAreaValid;
		BOOLEAN	m_fLandscapePrintableAreaValid;
		sWORD		m_swPlatformPaperId;
	};

class RPaperAreaCollection: public RArray<RPaperOrientations> {};

// ****************************************************************************
//
//  Class Name:	RPrinter
//
//  Description:	encapsulate info for a printer.
//
// ****************************************************************************
//
class FrameworkLinkage RPrinter : public RObject
	{
	// Enums
	public :
		enum					EOrientation		{ kPortrait=0					//	the printer is in portrait orientation
														, kLandscape					//	the printer is in landscape orientation
														, kTall = kPortrait			//	alternate name for portrait which is more meaningful in some portions of the code
														, kWide = kLandscape			//	alternate name for landscape which is more meaningful in some portions of the code
														, kNumberOfOrientations };	//	Since there are only two orientations this count is still valid when calculated this way.
		enum					EPaperSize			{ kUSLetter=0					//	the print is using us letter paper
														, kUSLegal						//	the print is using us legal paper
														, kA4Letter						//	the print is using a4 letter paper
														, kB5Letter						//	the print is using b5 letter paper
														, kTabloid						//	the print is using us tabloid paper
														, kA3Tabloid					//	the print is using a3 tabloid paper
														, kSpecialPaperSize			//	the print is using a user specified paper size not necessarily supported by the os
														, kNumberOfPaperSizes };
		enum					EPaperSide			{ kTopSide = 0					//	the print is printing on the top side of the paper
														, kBottomSide					//	the print is printing on the bottom side of the paper
														, kNumSides };
		enum					ETemplateReason	{ kPrinterDeleted=0			//	the printer was deleted
														, kPrinterDriverFailure		//	the attached printer's driver could not be opened
														, kPrinterNotAttached		//	this printer is not attached 
														, kNumTemplateReasons };

	// Construction, destruction & Initialization
	public :
								RPrinter( const RMBCString& rPrinterName, const RMBCString& rDriverName, const RMBCString& rPortName );
								RPrinter( const BOOLEAN fTemplate );
								RPrinter( );
								RPrinter( const RPrinter& rPrinter );
								RPrinter( const RMBCString& rDriverName );
		virtual				~RPrinter( );

	// public operations
	public :
		RIntRect 			CalcPaperRect( const EPaperSize paper, const EOrientation orientation ) const;
		RIntRect&			CalcPrintableAreaRect( const EPaperSize paper, const EOrientation orientation, const BOOLEAN fUpdatePaperAreas = TRUE );
		RIntRect 			CalcPrintableArea( const EOrientation orientation ) const;
		void		 			CalcOrientationTransform( const EOrientation orientation, R2dTransform& rBaseTransform, const BOOLEAN fReferseXForm = FALSE ) const;
		RPrinterDrawingSurface*	CreateDrawingSurface( RCancelPrintingSignal* pCancelPrintingSignal );
		void					DestroyDrawingSurface( RPrinterDrawingSurface*& pPrinterDrawingSurface );
		virtual uLONG		OpenPrinter( );
		virtual void		ClosePrinter( const uLONG ulHandle );
		virtual void		DisableOrientationTransform( );
		virtual BOOLEAN	OrientationTransformDisabled( ) const; 
		virtual void		EnableOrientationTransform( );
		virtual BOOLEAN	Update( BOOLEAN fUpdatePaperAreas = TRUE, BOOLEAN fUsePrinterDatabase = TRUE );
		virtual BOOLEAN	Enable( );
		void					GetBaseTransform( R2dTransform& transform, RDrawingSurface* pDrawingSurface ) const;
		RPrinter&			operator = ( const RPrinter &rPrinterSource );
		virtual void		Read( RArchive* pArchive );
		virtual void		Write( RArchive* pArchive );
		void					SpecifyTemplateValues( );
		YConfidence			IsSimilar( const RPrinter* pPrinter, const uLONG ulCompareFlags = kCompareStandard ) const;
		void					PreferPaper( const EPaperSize paper, const EOrientation orientation, EPaperSize& oldPaper, EOrientation& oldOrientation );
		virtual BOOLEAN	DoPropertiesDialog( );
		BOOLEAN				IsPaperSupported( EPaperSize paper ) const;

	// accessors
	public :
		EOrientation		GetOrientation(  ) const;
		RIntRect				GetPrintableArea(  ) const;
		RIntRect				GetPrintableArea( const EPaperSize paper, const EOrientation orientation ) const;
		RIntRect				GetAdjustedPrintableArea( const EPaperSize paper, const EOrientation orientation ) const;
		RIntRect				GetPrintableAreaFromDriver(  ) const;
		void					SetPrintableArea( const RIntRect& rPrintableArea );
		void					SetPrintableArea( const RIntRect& rPrintableArea, const EPaperSize paper, const EOrientation orientation );
		RIntRect				GetPaperRect(  ) const;
		void					SetPaperArea( const RIntSize rPaperSize );
		RIntSize				GetPaperArea(  ) const;
		RIntSize&			GetPaperArea( const EPaperSize paper, const EOrientation orientation, const BOOLEAN fUpdatePaperAreas = TRUE );
		EPaperSize			GetPaperSize(  ) const;
		BOOLEAN				SetPaperSize( const sWORD swPaperID );
		static sWORD		GetPlatformPaperId( const EPaperSize eSize );
		const RMBCString&	GetPrinterName( ) const;
		const RMBCString&	GetDriverName( ) const;
		const RMBCString&	GetPortName( ) const;
		BOOLEAN				IsTemplate( ) const;
		BOOLEAN				IsAttached( ) const;
		BOOLEAN				IsDeleted( ) const;
		void					MakeTemplate( const ETemplateReason eReason );
		BOOLEAN				Complete( ) const;
		void					Delete( );
		ETemplateReason	GetTemplateReason( ) const;
		RMBCString			GetPaperName( ) const;
		RMBCString			GetPaperNameAndOrientation( ) const;
		static RMBCString	GetPaperName( EPaperSize eSize );
		static RMBCString	GetPaperNameAndOrientation( EPaperSize eSize, EOrientation orientation );
		
	//	private operations
	private:
		BOOLEAN				GetPaperAreasFromDriver( const uLONG ulPrinterInfo, const EPaperSize paper, const EOrientation orientation, RIntSize &rPaperSize, RIntRect &rPrintableArea );
		BOOLEAN				GetProperties( const uLONG ulFlags );
		BOOLEAN				GetProperties( const uLONG ulPrinterHandle, const uLONG ulFlags );

		void					UpdatePaperInfo( const EPaperSize paper, const EOrientation orientation );
		void					InvalidatePaperInfo( );

	// Member data
	protected :
		EOrientation			m_Orientation;					// orientation of paper in printer
		EPaperSize				m_PaperSize;					//	one of above sizes
		RPaperAreaCollection	m_PaperAreas;					//	paper areas; one for each paper size and orientation
		RIntRect					m_PrintableAreaFromDriver;	//	size of useable area
		RMBCString				m_rPrinterName;				//	name of printer
		RMBCString				m_rDriverName;					//	name of driver
		RMBCString				m_rPortName;					//	name of port printer is attached to
		YIntDimension			m_yDriverInfoSize;			//	driver specific info size
		PUBYTE					m_pDriverInfo;					//	driver specific info
		BOOLEAN					m_fTemplatePrinter;			//	TRUE: no printer is attached so this info is artificial for temporary use only
		ETemplateReason		m_eTemplateReason;			//	see ETemplateReason
		BOOLEAN					m_fOrientationTransformDisabled;	//	TRUE: do not rotate to account for difference between page and project orientations
		BOOLEAN					m_fSpecificationComplete;	//	TRUE: the info in the has all been acquired from the named driver
																		//	FALSE: only the printer and port names are guaranteed to be valid
																		//		the rest must be acquired thru the Update call
#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void		Validate( ) const;
#endif
	};


class RPrinterCollection: public RArray<RPrinter> {};

// ****************************************************************************
//
//  Class Name:	RPreferPaperHelper
//
//  Description:	Class to save, set and restore the paper type for this printer
//
// ****************************************************************************
//
class RPreferPaperHelper
	{
	public :
													RPreferPaperHelper( RPrinter& printer, const RPrinter::EPaperSize newPaperSize, const RPrinter::EOrientation newPaperOrientation );
													//	Not a baseclass, does not have to be virtual
													~RPreferPaperHelper( );	

	private:
		RPrinter::EPaperSize					m_OldPaperSize;
		RPrinter::EOrientation				m_OldPaperOrientation;
		RPrinter&								m_Printer;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RPreferPaperHelper::RPreferPaperHelper( )
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RPreferPaperHelper::RPreferPaperHelper( RPrinter& printer, const RPrinter::EPaperSize newPaperSize, const RPrinter::EOrientation newPaperOrientation )
	: m_Printer( printer )
{
	m_Printer.PreferPaper( newPaperSize, newPaperOrientation, m_OldPaperSize, m_OldPaperOrientation );
}

// ****************************************************************************
//
//  Function Name:	RPreferPaperHelper::RPreferPaperHelper( )
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RPreferPaperHelper::~RPreferPaperHelper( )
{
	m_Printer.PreferPaper( m_OldPaperSize, m_OldPaperOrientation, m_OldPaperSize, m_OldPaperOrientation );
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_PRINTER_H_
