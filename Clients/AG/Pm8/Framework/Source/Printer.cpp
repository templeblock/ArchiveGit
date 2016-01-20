// ****************************************************************************
//
//  File Name:			Printer.cpp
//
//  Project:			Renaissance Application
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPrinter class
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
//  $Logfile:: /PM8/Framework/Source/Printer.cpp                              $
//     $Date:: 3/03/99 6:17p                                                  $
//   $Author:: Gbeddow                                                        $
//	$Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Printer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"PrintManager.h"
#include "ResourceManager.h"
#include "FrameworkResourceIDs.h"
#include "ApplicationGlobals.h"
#include "StandaloneView.h"
#include	"PrinterDrawingSurface.h"
#include	"PrinterCompensation.h"
#include "Document.h"
#include "ChunkyStorage.h"

//	Define chunk versioning info.
//	Because we are putting our info into a chunk created in project code
//	we need to use the value defined there for version comparison. Because
//	we install the version in the source project code we do not need to worry
//	about future compatibility issues.
const YVersion			kPrinterAreaFromDriverRequiredVersion( kDefaultDataVersion + 6 );
const YVersion			kCurrentPrinterRequiredVersion( kPrinterAreaFromDriverRequiredVersion );

//	Local fcns.
static uBYTE CountFlags( uLONG ulFlags );

// ****************************************************************************
//
//  Function Name:	CountFlags( uLONG ulFlags )
//
//  Description:		Return the number of flags set in the given flag word.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
uBYTE CountFlags( uLONG ulFlags )
	{
		uBYTE ubNumFlags = 0;
		for( int i = 0; i < (sizeof( ulFlags ) * 8); ++i, ulFlags >>= 1 )
			if ( ulFlags & 01 )
				++ubNumFlags;

		return ubNumFlags;
	}

// ****************************************************************************
//
//  Function Name:	RPaperOrientations::RPaperOrientations( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPaperOrientations::RPaperOrientations( )
	{
	rLandscapeSize							= RIntSize(0,0);
	rPortraitSize							= RIntSize(0,0);
	rLandscapePrintableArea				= RIntRect(0,0,0,0);
	rPortraitPrintableArea				= RIntRect(0,0,0,0);
	m_fPortraitSizeValid					= FALSE;
	m_fLandscapeSizeValid				= FALSE;
	m_fPortraitPrintableAreaValid		= FALSE;
	m_fLandscapePrintableAreaValid	=  FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::RPrinter( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter::RPrinter( const RMBCString& rDriverName )
	:	m_fSpecificationComplete( FALSE ), m_fTemplatePrinter( FALSE ), m_rPrinterName( rDriverName )
	, m_rPortName( rDriverName ), m_rDriverName( rDriverName ), m_pDriverInfo( NULL ), m_yDriverInfoSize( 0 )
	, m_PrintableAreaFromDriver( 0,0,0,0 )
	, m_fOrientationTransformDisabled( FALSE ), m_eTemplateReason( kPrinterNotAttached )
	{
	//	Construct a printer given nothing but a driver name by 
	//	allowing update to pick up the values from the driver.
	//	The bogus printer and port names supplied above will be replaced
	//	by valid values in this process.

	//	Populate the paper sizes collection.
	for( int i=0; i < kNumberOfPaperSizes; ++i )
		m_PaperAreas.InsertAtEnd( RPaperOrientations() );
	
	//	Install default paper sizes.
	SpecifyTemplateValues( );

	Update();
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::RPrinter( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter::RPrinter( const RMBCString& rPrinterName, const RMBCString& rDriverName, const RMBCString& rPortName )
	:	m_fSpecificationComplete( FALSE ), m_fTemplatePrinter( FALSE ), m_rPrinterName( rPrinterName )
	, m_rPortName( rPortName ), m_rDriverName( rDriverName ), m_pDriverInfo( NULL ), m_yDriverInfoSize( 0 )
	, m_PrintableAreaFromDriver( 0,0,0,0 )
	, m_fOrientationTransformDisabled( FALSE ), m_eTemplateReason( kPrinterNotAttached )
	{
	//	Populate the paper sizes collection.
	for( int i=0; i < kNumberOfPaperSizes; ++i )
		m_PaperAreas.InsertAtEnd( RPaperOrientations() );
	
	//	Install default paper sizes.
	SpecifyTemplateValues( );
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::RPrinter( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter::RPrinter( )
	: m_fSpecificationComplete( FALSE ), m_fTemplatePrinter( FALSE ), m_pDriverInfo( NULL )
	, m_yDriverInfoSize( 0 ), m_PrintableAreaFromDriver( 0,0,0,0 )
	, m_fOrientationTransformDisabled( FALSE ), m_eTemplateReason( kPrinterNotAttached )
	{
	//	Populate the paper sizes collection.
	for( int i=0; i < kNumberOfPaperSizes; ++i )
		m_PaperAreas.InsertAtEnd( RPaperOrientations() );
	
	//	Install default paper sizes.
	SpecifyTemplateValues( );
	}


// ****************************************************************************
//
//  Function Name:	RPrinter::RPrinter( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter::RPrinter( const BOOLEAN in_fTemplate )
	: m_fSpecificationComplete( FALSE ), m_fTemplatePrinter( in_fTemplate ), m_pDriverInfo( NULL )
	, m_yDriverInfoSize( 0 ), m_PrintableAreaFromDriver( 0,0,0,0 )
	, m_fOrientationTransformDisabled( FALSE ), m_eTemplateReason( kPrinterNotAttached )
{
	BOOLEAN fTemplate = in_fTemplate;	

	//	Populate the paper sizes collection.
	for( int i=0; i < kNumberOfPaperSizes; ++i )
		m_PaperAreas.InsertAtEnd( RPaperOrientations() );
	
	//	Install default paper sizes.
	SpecifyTemplateValues( );

	if ( !fTemplate )
		{
		//	If we're creating a default printer then copy in the right values.
		//	
		if ( m_rPrinterName.IsEmpty( ) )
			*this = *RPrintManager::GetPrintManager().GetDefaultPrinter();

		// Make sure the settings are current. If we still don't have a default printer
		//	then we should use the template printer's settings.
		if ( m_rPrinterName.IsEmpty( ) )
			fTemplate = TRUE;
		else
			Update();
		}

	if ( fTemplate )
		{
		//	If the printer is constructed as a template then name it
		//	appropriately so that the description strings shown to the user 
		//	reflects the actual situation when this printer is used.
		SpecifyTemplateValues( );
		}
}

// ****************************************************************************
//
//  Function Name:	RPrinter::RPrinter( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter::RPrinter( const RPrinter& rPrinter )
	: m_fSpecificationComplete( FALSE ), m_fTemplatePrinter( FALSE ), m_pDriverInfo( NULL )
	, m_yDriverInfoSize( 0 ), m_PrintableAreaFromDriver( 0,0,0,0 )
	, m_fOrientationTransformDisabled( FALSE ), m_eTemplateReason( kPrinterNotAttached )
	{
	//	Populate the paper sizes collection.
	for( int i = m_PaperAreas.Count(); i < kNumberOfPaperSizes; ++i )
		m_PaperAreas.InsertAtEnd( RPaperOrientations() );

	//	Call into the = operator where we duplicate the driverinfo handle.
	*this = rPrinter;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::~RPrinter( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RPrinter::~RPrinter( )
{
	delete [] m_pDriverInfo;
}


// ****************************************************************************
//
//  Function Name:	RPrinter::operator =	(const RPrinter &rPrinterSource )
//
//  Description:		operator =
//
//  Returns:			this
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RPrinter& RPrinter::operator =	( const RPrinter &rPrinterSource )
	{
	if ( &rPrinterSource != this)
		{
		m_Orientation				 = rPrinterSource.m_Orientation;				
		m_PaperSize					 = rPrinterSource.m_PaperSize;				
		m_rPrinterName				 = rPrinterSource.m_rPrinterName;			
		m_rDriverName				 = rPrinterSource.m_rDriverName;				
		m_rPortName					 = rPrinterSource.m_rPortName;				
		m_yDriverInfoSize			 = rPrinterSource.m_yDriverInfoSize;
		m_PrintableAreaFromDriver= rPrinterSource.m_PrintableAreaFromDriver;			
		m_fTemplatePrinter		 = rPrinterSource.m_fTemplatePrinter;
		m_eTemplateReason			 = rPrinterSource.m_eTemplateReason;
		m_fSpecificationComplete = rPrinterSource.m_fSpecificationComplete;
		
		//	Copy the paper sizes.
		RPaperAreaCollection::YIterator IterSrc = rPrinterSource.m_PaperAreas.Start();
		RPaperAreaCollection::YIterator IterDst = m_PaperAreas.Start();
//		if ( IterDst != m_PaperAreas.End() )
//			{
//			SetPaperArea( rPrinterSource.GetPaperArea() );
//			SetPrintableArea( rPrinterSource.GetPrintableArea() );
//			}
		for( ; IterDst != m_PaperAreas.End(); ++IterDst, ++IterSrc )
			*IterDst = *IterSrc;

		//	Free any existing driver info.
		delete [] m_pDriverInfo;
		m_pDriverInfo = NULL;
		
		//	Copy the driver info into a new buffer.
		if ( rPrinterSource.m_pDriverInfo )
			{
			try
				{
				//	Allocate and fill the devmode buffer.
				m_pDriverInfo = new uBYTE[ m_yDriverInfoSize ];
				::memcpy( m_pDriverInfo, rPrinterSource.m_pDriverInfo, m_yDriverInfoSize );
				}
			catch( YException& exception )
				{
				m_yDriverInfoSize = 0;
				::ReportException( exception );
				}
			catch( ... )
				{
				m_yDriverInfoSize = 0;
				// REVIEW: What else do we do here?
				::ReportException( kUnknownError );
				}
			}
		}
	
	return *this;
	}


// ****************************************************************************
//
//  Function Name:	RPrinter::SpecifyTemplateValues( ) 
//
//  Description:		Install useable default values for page dimensions.
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::SpecifyTemplateValues( )
	{
	// No printer. Fill in some defaults.
	m_PaperSize = kUSLetter;
	m_Orientation = kPortrait;

	//	Fill in standard paper sizes.
	//	Us Letter.
	RPaperAreaCollection::YIterator Iter = m_PaperAreas.Start();
	Iter[kUSLetter].rLandscapeSize = RIntSize( ::InchesToLogicalUnits( 11 ), ::InchesToLogicalUnits( 8.5 ) );
	Iter[kUSLetter].rPortraitSize = RIntSize( ::InchesToLogicalUnits( 8.5 ), ::InchesToLogicalUnits( 11 ) );
	Iter[kUSLetter].rLandscapePrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 10.75 ), ::InchesToLogicalUnits( 8.25 ) );
	Iter[kUSLetter].rPortraitPrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 8.25 ), ::InchesToLogicalUnits( 10.75 ) );
	Iter[kUSLetter].m_swPlatformPaperId = GetPlatformPaperId( kUSLetter );

	//	Us Legal.
	Iter[kUSLegal].rLandscapeSize = RIntSize( ::InchesToLogicalUnits( 14 ), ::InchesToLogicalUnits( 8.5 ) );
	Iter[kUSLegal].rPortraitSize = RIntSize( ::InchesToLogicalUnits( 8.5 ), ::InchesToLogicalUnits( 14 ) );
	Iter[kUSLegal].rLandscapePrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 13.75 ), ::InchesToLogicalUnits( 8.25 ) );
	Iter[kUSLegal].rPortraitPrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 8.25 ), ::InchesToLogicalUnits( 13.75 ) );
	Iter[kUSLegal].m_swPlatformPaperId = GetPlatformPaperId( kUSLegal );

	//	Envelope.
	Iter[kSpecialPaperSize].rLandscapeSize = RIntSize( ::InchesToLogicalUnits( 14 ), ::InchesToLogicalUnits( 8.5 ) );
	Iter[kSpecialPaperSize].rPortraitSize = RIntSize( ::InchesToLogicalUnits( 8.5 ), ::InchesToLogicalUnits( 14 ) );
	Iter[kSpecialPaperSize].rLandscapePrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 13.75 ), ::InchesToLogicalUnits( 8.25 ) );
	Iter[kSpecialPaperSize].rPortraitPrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 8.25 ), ::InchesToLogicalUnits( 13.75 ) );
	Iter[kSpecialPaperSize].m_swPlatformPaperId = GetPlatformPaperId( kSpecialPaperSize );

	//	Us Tabloid.
	Iter[kTabloid].rLandscapeSize = RIntSize( ::InchesToLogicalUnits( 17 ), ::InchesToLogicalUnits( 11 ) );
	Iter[kTabloid].rPortraitSize = RIntSize( ::InchesToLogicalUnits( 11 ), ::InchesToLogicalUnits( 17 ) );
	Iter[kTabloid].rLandscapePrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 16.75 ), ::InchesToLogicalUnits( 10.75 ) );
	Iter[kTabloid].rPortraitPrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 10.75 ), ::InchesToLogicalUnits( 16.75 ) );
	Iter[kTabloid].m_swPlatformPaperId = GetPlatformPaperId( kTabloid );

	//	A4 Letter.	millimeter measurements converted to inches below using 1mm == .039"
	Iter[kA4Letter].rLandscapeSize = RIntSize( ::InchesToLogicalUnits( 297.*.039 ), ::InchesToLogicalUnits( 210.*.039 ) );
	Iter[kA4Letter].rPortraitSize = RIntSize( ::InchesToLogicalUnits( 210.*.039 ), ::InchesToLogicalUnits( 297.*.039 ) );
	Iter[kA4Letter].rLandscapePrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 297.*.039 - .25 ), ::InchesToLogicalUnits( 210.*.039 - .25 ) );
	Iter[kA4Letter].rPortraitPrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 210.*.039 - .25 ), ::InchesToLogicalUnits( 297.*.039 - .25 ) );
	Iter[kA4Letter].m_swPlatformPaperId = GetPlatformPaperId( kA4Letter );

	//	A3 Tabloid.	millimeter measurements converted to inches below using 1mm == .039"
	Iter[kA3Tabloid].rLandscapeSize = RIntSize( ::InchesToLogicalUnits( 420.*.039 ), ::InchesToLogicalUnits( 297.*.039 ) );
	Iter[kA3Tabloid].rPortraitSize = RIntSize( ::InchesToLogicalUnits( 297.*.039 ), ::InchesToLogicalUnits( 420.*.039 ) );
	Iter[kA3Tabloid].rLandscapePrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 420.*.039 - .25 ), ::InchesToLogicalUnits( 297.*.039 - .25 ) );
	Iter[kA3Tabloid].rPortraitPrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 297.*.039 - .25 ), ::InchesToLogicalUnits( 420.*.039 - .25 ) );
	Iter[kA3Tabloid].m_swPlatformPaperId = GetPlatformPaperId( kA3Tabloid );

	//	B5 Letter.	millimeter measurements converted to inches below using 1mm == .039"
	Iter[kB5Letter].rLandscapeSize = RIntSize( ::InchesToLogicalUnits( 257.*.039 ), ::InchesToLogicalUnits( 182.*.039 ) );
	Iter[kB5Letter].rPortraitSize = RIntSize( ::InchesToLogicalUnits( 182.*.039 ), ::InchesToLogicalUnits( 257.*.039 ) );
	Iter[kB5Letter].rLandscapePrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 257.*.039 - .25 ), ::InchesToLogicalUnits( 182.*.039 - .25 ) );
	Iter[kB5Letter].rPortraitPrintableArea = RIntRect( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 182.*.039 - .25 ), ::InchesToLogicalUnits( 257.*.039 - .25 ) );
	Iter[kB5Letter].m_swPlatformPaperId = GetPlatformPaperId( kB5Letter );
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::Complete( ) const
//
//  Description:		Return TRUE iff the printer's info is all valid
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::Complete( ) const
	{
	return m_fSpecificationComplete;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::IsTemplate( ) const
//
//  Description:		Return TRUE if no printers are available.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::IsTemplate( ) const
	{
	return m_fTemplatePrinter;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::IsDeleted( ) const
//
//  Description:		Return TRUE if the printer was deleted.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::IsDeleted( ) const
	{
	return static_cast<BOOLEAN>( m_fTemplatePrinter && ( kPrinterDeleted == m_eTemplateReason ) );
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::IsAttached( ) const
//
//  Description:		Return TRUE if the printer is flagged as available in the print manager.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::IsAttached( ) const
	{
	return static_cast<BOOLEAN>( !( m_fTemplatePrinter && ( kPrinterNotAttached == m_eTemplateReason ) ) );
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::MakeTemplate( const ETemplateReason ulReason )
//
//  Description:		Prevent this printer from being used as an output device.
//							It can still be used in the formatting of documents.
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::MakeTemplate( const ETemplateReason eReason ) 
	{
	m_fTemplatePrinter = TRUE;
	m_eTemplateReason = eReason;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::IsSimilar( const RPrinter* pPrinter, const uLONG ulCompareFlags /*= kCompareStandard*/ ) const
//
//  Description:		Return confidence value for the degree that the given printer matches this one within the specified limits.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YConfidence RPrinter::IsSimilar( const RPrinter* pPrinter, const uLONG ulCompareFlags /*= kCompareStandard*/ ) const
	{
	TpsAssert( pPrinter, "Null input printer" );
	TpsAssert( kCompareNothing != ulCompareFlags, "no compare flags" );

	//	Get the flags which specify fields to compare.
	// Strip out comparison modifier flags like kCompareRequireTotalConfidence
	//	so we count only fields which refer to members of RPrinter.
	uLONG ulCompareFlagsOnly = ulCompareFlags & ~kComparisonModifierFlags;
	
	//	Count them.
	uBYTE ubNumFlags = ::CountFlags( ulCompareFlagsOnly );

	//	Collect comparison results. Distribute confidence among the attributes (25 each).
	YConfidence yComparePrinterName = kNoConfidence;
	YConfidence yComparePortName = kNoConfidence;
	YConfidence yCompareDriverName = kNoConfidence;
	YConfidence yCompareDriverOrientation = kNoConfidence;
	YConfidence yComparePageSize = kNoConfidence;
	YConfidence yComparePrintableArea = kNoConfidence;
	YConfidence yCompareDriverPrintableArea = kNoConfidence;
	const YConfidence yEachComparisonContribution = kCompleteConfidence/ubNumFlags;

	//	Test the printers based on the given comparison flags.
	if ( (ulCompareFlags & kCompareDriverName) && ( pPrinter->GetDriverName() == GetDriverName() ) ) 
		yCompareDriverName = yEachComparisonContribution;
	
	if ( (ulCompareFlags & kComparePrinterName) && ( pPrinter->GetPrinterName() == GetPrinterName() ) ) 
		yComparePrinterName = yEachComparisonContribution;
	
	if ( (ulCompareFlags & kComparePortName) && ( pPrinter->GetPortName() == GetPortName() ) ) 
		yComparePortName = yEachComparisonContribution;
	
	//	Compare printable area values from the drivers for these printers to see if they match.
	if ( (ulCompareFlags & kComparePrintableArea) && ( pPrinter->GetPrintableArea() == GetPrintableArea() ) ) 
		yComparePrintableArea = yEachComparisonContribution;
	
	//	Compare printable area from driver
	if ( (ulCompareFlags & kCompareDriverPrintableArea) && (pPrinter->GetPrintableAreaFromDriver() == GetPrintableAreaFromDriver() ))
			yCompareDriverPrintableArea = yEachComparisonContribution;

	//	Compare printer orientation
	if ( (ulCompareFlags & kComparePrinterOrientation) && (pPrinter->GetOrientation() == m_Orientation) )
		yCompareDriverOrientation = yEachComparisonContribution;

	//	Compare printer paper type.
	if ( (ulCompareFlags & kComparePrinterPageType) && (pPrinter->GetPaperSize() == m_PaperSize) )
		yComparePageSize = yEachComparisonContribution;
		
	//	Collect our confidence in this match.
	YConfidence yCollectedConfidence = yComparePrinterName + yComparePortName + yCompareDriverName + yCompareDriverOrientation
			+ yComparePrintableArea	+ yCompareDriverPrintableArea + yComparePageSize;

	//	If we're required to have all fields match then make sure they do.
	if ( kCompareRequireTotalConfidence & ulCompareFlags )
		{
		//	If we compared succesfully with all of the flags return total confidence
		//	else we have no confidence in the comparison.
		if ( (ubNumFlags * yEachComparisonContribution) == yCollectedConfidence )
			return kCompleteConfidence;
		else
			return kNoConfidence;
		}
	
	//	If we require printable areas match and they don't then we have no confidence in the comparison.
	else if ( (kCompareRequireDriverPrintableAreasMatch & ulCompareFlags) && !yCompareDriverPrintableArea )
		return kNoConfidence;
	else
		return yCollectedConfidence;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::Delete( )
//
//  Description:		Mark this printer as unusable.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::Delete( )
	{
	m_eTemplateReason = kPrinterDeleted;
	m_fTemplatePrinter = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetTemplateReason( ) const
//
//  Description:		return the reason this printer is a template printer.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter::ETemplateReason RPrinter::GetTemplateReason( ) const
	{
	return m_eTemplateReason;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::Enable( )
//
//  Description:		Try to make this printer available for use even if it was 
//							disabled or deleted previously.
//
//  Returns:			TRUE: the printable is attached and useable.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::Enable( )
	{
	BOOLEAN fEnabled = FALSE;

	//	Clear the printers template status and check to see if it's useable.
	m_fTemplatePrinter = FALSE;
	if ( Update( FALSE ) )
		fEnabled = TRUE;
	else
		//	If the update fails then disable the printer.
		Delete( );

	return fEnabled;
	}


// ****************************************************************************
//
//  Function Name:	RPrinter::GetOrientation( )
//
//  Description:	
//
//  Returns:			EOrientation
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter::EOrientation RPrinter::GetOrientation(  ) const
{
	return m_Orientation;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPrintableAreaFromDriver( )
//
//  Description:		Return the printable area from the driver.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPrinter::GetPrintableAreaFromDriver(  ) const
{
	return m_PrintableAreaFromDriver;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPrintableArea( )
//
//  Description:		Return the printable area.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPrinter::GetPrintableArea( const EPaperSize paper, const EOrientation orientation ) const
{
	//	We are creating a new object on the stack for the return value so the const cast is OK here.	
	return const_cast<RPrinter*>(this)->CalcPrintableAreaRect( paper, orientation );
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPrintableArea( )
//
//  Description:		Return the printable area.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPrinter::GetPrintableArea(  ) const
{
	//	We are creating a new object on the stack for the return value so the const cast is OK here.	
	return GetPrintableArea( m_PaperSize, m_Orientation );
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetAdjustedPrintableArea( )
//
//  Description:		Return the printable area adjusted to have equal margins on 
//							the top/bottom and left/right.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPrinter::GetAdjustedPrintableArea( const EPaperSize paper, const EOrientation orientation ) const
	{
	//	Won't modify rPaperArea so this cast is ok.
	const RIntSize rPaperArea( const_cast<RPrinter*>(this)->GetPaperArea( paper, orientation ) );
	const RIntRect rAdjustedPrintableArea( GetPrintableArea( paper, orientation ) );			

	//	Make the printable area equal on top/bottom and left/right.
	if (rAdjustedPrintableArea.m_Left > (rPaperArea.m_dx - rAdjustedPrintableArea.m_Right))
		rAdjustedPrintableArea.m_Right = rPaperArea.m_dx - rAdjustedPrintableArea.m_Left;
	else if (rAdjustedPrintableArea.m_Left < (rPaperArea.m_dx - rAdjustedPrintableArea.m_Right))
		rAdjustedPrintableArea.m_Left = rPaperArea.m_dx - rAdjustedPrintableArea.m_Right;
	
	if (rAdjustedPrintableArea.m_Top > (rPaperArea.m_dy - rAdjustedPrintableArea.m_Bottom))
		rAdjustedPrintableArea.m_Bottom = rPaperArea.m_dy - rAdjustedPrintableArea.m_Top;
	else if (rAdjustedPrintableArea.m_Top < (rPaperArea.m_dy - rAdjustedPrintableArea.m_Bottom))
		rAdjustedPrintableArea.m_Top = rPaperArea.m_dy - rAdjustedPrintableArea.m_Bottom;

	return rAdjustedPrintableArea;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::SetPrintableArea( const RIntRect& rPrintableArea )
//
//  Description:		Set the printable area to the given printable area.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::SetPrintableArea( const RIntRect& rPrintableArea, const EPaperSize paper, const EOrientation orientation )
{
#ifdef	TPSDEBUG
RIntSize rPaperSize = GetPaperArea( paper, orientation, !kUpdatePaperAreas );
TpsAssert( (rPaperSize.m_dx >= rPrintableArea.Width()) && (rPaperSize.m_dy >= rPrintableArea.Height())
	, "printable area does not fit on page" );
#endif	//	TPSDEBUG

	CalcPrintableAreaRect( paper, orientation, !kUpdatePaperAreas ) = rPrintableArea;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::SetPrintableArea( const RIntRect& rPrintableArea )
//
//  Description:		Set the printable area to the given printable area.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::SetPrintableArea( const RIntRect& rPrintableArea )
{
	SetPrintableArea( rPrintableArea, m_PaperSize, m_Orientation );
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperSize( )
//
//  Description:		return the paper size used 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter::EPaperSize RPrinter::GetPaperSize(  ) const
{
	return m_PaperSize;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperArea( )
//
//  Description:	
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RPrinter::GetPaperArea( ) const
{
	//	We are creating a new object on the stack for the return value so the const cast is OK here.	
	return const_cast<RPrinter*>(this)->GetPaperArea( m_PaperSize, m_Orientation );
}

// ****************************************************************************
//
//  Function Name:	RPrinter::SetPaperArea( )
//
//  Description:	
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::SetPaperArea( const RIntSize rSize )
{
	GetPaperArea( m_PaperSize, m_Orientation ) = rSize;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperArea( )
//
//  Description:		return the paper size associated with the current orientation and paper 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize& RPrinter::GetPaperArea( const EPaperSize paper, const EOrientation orientation, const BOOLEAN fUpdatePaperAreas /*= TRUE*/ )
{
	// Update the paper info
	if ( fUpdatePaperAreas )	
		UpdatePaperInfo( paper, orientation );

	//	Return paper area that the driver returns.
	RPaperAreaCollection::YIterator Iter = m_PaperAreas.Start();
	if ( kPortrait == orientation )
		return Iter[paper].rPortraitSize;
	else	
		return Iter[paper].rLandscapeSize;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperRect( )
//
//  Description:	
//
//  Returns:			Rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPrinter::GetPaperRect(  ) const
{
	return CalcPaperRect( m_PaperSize, m_Orientation );
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPrinterName( ) const
//
//  Description:		Returns the name of this printer.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RPrinter::GetPrinterName(  ) const
{
	return m_rPrinterName;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetDriverName( ) const
//
//  Description:		Returns the name of the driver used for this printer.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RPrinter::GetDriverName(  ) const
{
	return m_rDriverName;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPortName( ) const
//
//  Description:		Returns the port this printer is attached to.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RPrinter::GetPortName(  ) const
{
	return m_rPortName;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetBaseTransform( R2dTransform& transform ) const
//
//  Description:		Return the 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::GetBaseTransform( R2dTransform& transform, RDrawingSurface* pDrawingSurface ) const
	{
	TpsAssert( pDrawingSurface, "Null input pointer." );

	//	Start with a clean transform.
	transform.MakeIdentity();

	// Scale to the screen DPI or scale to the device DPI.
	RRealSize deviceDPI;

	if( pDrawingSurface->IsPrinting( ) )
		deviceDPI = pDrawingSurface->GetDPI( );
	else
		deviceDPI = GetScreenDPI( );

	transform.PreScale( (YFloatType)deviceDPI.m_dx / kSystemDPI, (YFloatType)deviceDPI.m_dy / kSystemDPI );
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::CalcPrintableArea( const EOrientation orientation ) const
//
//  Description:		Return printable area rotated into the given orientation.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPrinter::CalcPrintableArea( const EOrientation orientation ) const
	{
	R2dTransform rBaseTransform;	
	RIntRect rAdjustedPrintableArea( GetAdjustedPrintableArea( m_PaperSize, m_Orientation ) );
	RIntSize rPaperArea( GetPaperArea( ) );

	//	Rotate the printable area in a way consistent with the rotation performed in 
	//	CalcOrientationTransform( orientation, rBaseTransform ). This preserves the
	//	size and location of the printable area in the rotated coordinate system.
	if ( ((orientation == kPortrait) && (m_Orientation == kLandscape) )
		|| ( (orientation == kLandscape) && (m_Orientation == kPortrait) ) )
		{
		YIntDimension temp = rAdjustedPrintableArea.m_Bottom;
		rAdjustedPrintableArea.m_Bottom = rAdjustedPrintableArea.m_Right;
		rAdjustedPrintableArea.m_Right = rPaperArea.m_dy - rAdjustedPrintableArea.m_Top;
		rAdjustedPrintableArea.m_Top = rAdjustedPrintableArea.m_Left;
		rAdjustedPrintableArea.m_Left = rPaperArea.m_dy - temp;
		}

	return rAdjustedPrintableArea;
	}


// ****************************************************************************
//
//  Function Name:	RPrinter::DisableOrientationTransform( )
//
//  Description:		disable the orientation transform
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::DisableOrientationTransform( )
	{
	m_fOrientationTransformDisabled = TRUE;
	}


// ****************************************************************************
//
//  Function Name:	RPrinter::EnableOrientationTransform( )
//
//  Description:		enable the orientation transform
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::EnableOrientationTransform( )
	{
	m_fOrientationTransformDisabled = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::OrientationTransformDisabled( ) const
//
//  Description:		return m_fOrientationTransformDisabled.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::OrientationTransformDisabled( ) const
	{
	return m_fOrientationTransformDisabled;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::CalcOrientationTransform( const EOrientation orientation
//							, R2dTransform& rBaseTransform ) const
//
//  Description:		Get transform to take output from the given orientation to the actual orientation.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::CalcOrientationTransform( const EOrientation orientation, R2dTransform& rBaseTransform, const BOOLEAN fReferseXForm /*= FALSE*/ ) const
	{
	//	Apply no transform if the orientation transform is disabled.
	if ( m_fOrientationTransformDisabled )
		return;
	
	//	In either of these cases the page orientation doesn't match the 
	//	project's orientation. Modify the transform to make them match.
	//	If we are asked to reverse the orientation xform then do so.
	RIntRect rPrintableArea = GetPrintableArea(  );
	if ( orientation == kPortrait && m_Orientation == kLandscape )
		{
		//	Rotate the coordinate so that the tall panel is horizontal(-90). 
		//	The rotation was about the origin so the page is now offset to the left
		//	by the printable areas height. Move it back.
		if ( fReferseXForm )
			{
			rBaseTransform.PreTranslate( -rPrintableArea.Width(), 0 );
			rBaseTransform.PreRotateAboutOrigin( DegreesToRadians( 90 ) );
			}
		else
			{
			rBaseTransform.PreTranslate( rPrintableArea.Width(), 0 );
			rBaseTransform.PreRotateAboutOrigin( DegreesToRadians( -90 ) );
			}
		}
	else if ( orientation == kLandscape && m_Orientation == kPortrait )
		{
		//	Rotate the coordinate so that the wide panel is vertical(-90). 
		//	The rotation was about the origin so the page is now offset to the left
		//	by the printable areas width. Move it back.
		if ( fReferseXForm )
			{
			rBaseTransform.PreTranslate( -rPrintableArea.Width(), 0 );
			rBaseTransform.PreRotateAboutOrigin( DegreesToRadians( 90 ) );
			}
		else
			{
			rBaseTransform.PreTranslate( rPrintableArea.Width(), 0 );
			rBaseTransform.PreRotateAboutOrigin( DegreesToRadians( -90 ) );
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::CalcPrintableAreaRect( )
//
//  Description:		Get the printable area for the given orientation and paper size from our cached values.
//
//  Returns:			Rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect& RPrinter::CalcPrintableAreaRect( const EPaperSize paper, const EOrientation orientation, const BOOLEAN fUpdatePaperAreas /*= TRUE*/ )
{
	// Update the paper info
	if ( fUpdatePaperAreas )	
		UpdatePaperInfo( paper, orientation );

	//	Return printable area that the driver returns.
	RPaperAreaCollection::YIterator Iter = m_PaperAreas.Start();
	if ( kPortrait == orientation )
		return Iter[paper].rPortraitPrintableArea;
	else	
		return Iter[paper].rLandscapePrintableArea;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::CalcPaperRect( )
//
//  Description:		Get the paper area for the given orientation and paper size from our cached values.
//
//  Returns:			Rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPrinter::CalcPaperRect( const EPaperSize paper, const EOrientation orientation ) const
{
	//	We are creating a new object on the stack for the return value so the const cast is OK here.	
	return RIntRect( const_cast<RPrinter*>(this)->GetPaperArea( paper, orientation ) );
}


// ****************************************************************************
//
// Function Name:		RPrinter::Read( RArchive& archive )
//
// Description:		Read our printer info.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPrinter::Read( RArchive* pArchive )
	{
	TpsAssert( pArchive, "Null input archive" );

	uLONG ulTemp;
	*pArchive >> ulTemp;
	m_Orientation = static_cast<EOrientation>( ulTemp );
	*pArchive >> ulTemp;
	m_PaperSize = static_cast<EPaperSize>( ulTemp );
	*pArchive >> GetPaperArea( m_PaperSize, m_Orientation, !kUpdatePaperAreas );
	
	//	Install the saved printable area
	RIntRect rPrintableArea;
	*pArchive >> rPrintableArea;
	SetPrintableArea( rPrintableArea );
	
	*pArchive >> m_rPrinterName;
	*pArchive >> m_rDriverName;
	*pArchive >> m_rPortName;
	*pArchive >> ulTemp;
	m_fTemplatePrinter = static_cast<BOOLEAN>( ulTemp );
	*pArchive >> ulTemp;
	m_fSpecificationComplete = static_cast<BOOLEAN>( ulTemp );

	//	We need a dynamic cast here since project params might be stored
	//	in something other than a chunk storage.
	RChunkStorage* pStorage = dynamic_cast<RChunkStorage*>(pArchive);
	if (NULL != pStorage)
		{
		*pArchive >> m_PrintableAreaFromDriver;	
		}
	}

// ****************************************************************************
//
// Function Name:		RPrinter::Write( RArchive& archive )
//
// Description:		Write our printer info.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RPrinter::Write( RArchive* pArchive )
	{
	TpsAssert( pArchive, "Null input archive" );

	*pArchive << static_cast<uLONG>( m_Orientation );
	*pArchive << static_cast<uLONG>( m_PaperSize );
	*pArchive << GetPaperArea( );
	*pArchive << GetPrintableArea(  );
	*pArchive << m_rPrinterName;
	*pArchive << m_rDriverName;
	*pArchive << m_rPortName;
	*pArchive << (uLONG)m_fTemplatePrinter;
	*pArchive << (uLONG)m_fSpecificationComplete;
	*pArchive << m_PrintableAreaFromDriver;
	}


// ****************************************************************************
//
//  Function Name:	RPrinter::DoPropertiesDialog( )
//
//  Description:		Get the printers properties into m_pDriverInfo.
//
//  Returns:			TRUE if OK was chosen form the properties dialog
//
//  Exceptions:		memory
//
// ****************************************************************************
//
BOOLEAN RPrinter::DoPropertiesDialog(  ) 
	{
	return GetProperties( kPrinterPropertiesPromptUser | kPrinterPropertiesForceUpdate );
	}


// ****************************************************************************
//
//  Function Name:	RPrinter::GetProperties( )
//
//  Description:		Get the printers properties into m_pDriverInfo.
//
//  Returns:			TRUE if OK was chosen form the properties dialog
//
//  Exceptions:		memory
//
// ****************************************************************************
//
BOOLEAN RPrinter::GetProperties( const uLONG ulFlags ) 
	{
	//	If we're just making sure there is something in m_pDriverInfo then we're done.
	if ( m_pDriverInfo && !( (ulFlags & kPrinterPropertiesPromptUser) || (ulFlags & kPrinterPropertiesForceUpdate) ) )
		return FALSE;

	//	The devmode properties live either in the printer driver or in this printer object if the
	//	user has changed them with the properties dialog. If m_pDriverInfo is NULL then the
	//	default info will be retrieved from the driver else the user's preferences (stored
	//	in m_pDriverInfo) will be used.
	uLONG ulPrinter = OpenPrinter( );
	
	BOOLEAN fPropertiesModified = GetProperties( ulPrinter, ulFlags );

	if ( ulPrinter )
		ClosePrinter( ulPrinter );

	return fPropertiesModified;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::InvalidatePaperInfo
//
//  Description:		Marks all the cached paper info as dirty
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::InvalidatePaperInfo( )
	{
	RPaperAreaCollection::YIterator Iter = m_PaperAreas.Start( );
	for( ; Iter != m_PaperAreas.End( ); ++Iter )
		{
		( *Iter ).m_fPortraitSizeValid = FALSE;
		( *Iter ).m_fPortraitPrintableAreaValid = FALSE;
		( *Iter ).m_fLandscapeSizeValid = FALSE;
		( *Iter ).m_fLandscapePrintableAreaValid = FALSE;
		}
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperName
//
//  Description:		Return the string associated with the currently selected 
//							paper.
//
//  Returns:			^
//
//  Exceptions:		Resource
//
// ****************************************************************************
//
RMBCString RPrinter::GetPaperName( ) const
{
	return GetPaperName( m_PaperSize );
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperNameAndOrientation
//
//  Description:		Return the string associated with the currently selected 
//							paper and orientation.
//
//  Returns:			^
//
//  Exceptions:		Resource
//
// ****************************************************************************
//
RMBCString RPrinter::GetPaperNameAndOrientation( ) const
{
	return GetPaperNameAndOrientation( m_PaperSize, m_Orientation );
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperName
//
//  Description:		Return the string associated with the currently selected 
//							paper.
//
//  Returns:			^
//
//  Exceptions:		Resource
//
// ****************************************************************************
//
RMBCString RPrinter::GetPaperName( EPaperSize eSize )
{
	switch( eSize )
	{
	case kUSLetter:
		return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USLETTER );
	case kUSLegal:
		return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USLEGAL );
	case kTabloid:
		return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USTABLOID );
	case kA4Letter:
		return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_A4LETTER );
	case kB5Letter:
		return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_B5LETTER );
	case kA3Tabloid:
		return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_A3TABLOID );
	default:
		TpsAssertAlways( "Invalid paper size" );
	}
	return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USLETTER );
}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperNameAndOrientation
//
//  Description:		Return the string associated with the currently selected 
//							paper and orientation.
//
//  Returns:			^
//
//  Exceptions:		Resource
//
// ****************************************************************************
//
RMBCString RPrinter::GetPaperNameAndOrientation( EPaperSize eSize, EOrientation orientation )
{
	switch( eSize )
	{
	case kUSLetter:
		if ( kWide == orientation )
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USLETTER_WIDE );
		else
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USLETTER_TALL );
	case kUSLegal:
		if ( kWide == orientation )
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USLEGAL_WIDE );
		else
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USLEGAL_TALL );
	case kTabloid:
		if ( kWide == orientation )
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USTABLOID_WIDE );
		else
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USTABLOID_TALL );
	case kA4Letter:
		if ( kWide == orientation )
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_A4LETTER_WIDE );
		else
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_A4LETTER_TALL );
	case kB5Letter:
		if ( kWide == orientation )
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_B5LETTER_WIDE );
		else
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_B5LETTER_TALL );
	case kA3Tabloid:
		if ( kWide == orientation )
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_A3TABLOID_WIDE );
		else
			return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_A3TABLOID_TALL );
	default:
		TpsAssertAlways( "Invalid paper size" );
	}

	return GetResourceManager().GetResourceString( STRING_PRINT_PAPER_SIZE_USLETTER_TALL );
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPrinter::Validate( )
//
//  Description:		Validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::Validate( ) const
{
	RObject::Validate();
	TpsAssertIsObject( RPrinter, this );
}

#endif	//	TPSDEBUG
