////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File name:		Configuration.cpp
//	Project:			Renaissance framework
//	Author:			G. Brown
//	Description:	Definition of configuration classes
//	Portability:	Platform-independent
//	Developer:		Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//	Client:			Broderbund Software, Inc.
//
//	Copyright (c) 1997 Turning Point Software.  All Rights Reserved.
//

#include "FrameworkIncludes.h"

ASSERTNAME

#include <process.h>
#include "Configuration.h"
#include "proident.h"
#include "sharecfg.h"
#include "StandaloneApplication.h"
#include "FrameworkResourceIDs.h"


static const char* kSTRING_ERROR_GENERAL_COMPONENT_MISSING = "The product identification file is missing or damaged. Please reinstall the product.";


// GetKPrintShopFamilyRegistryPath()
//
// Returns a string of the form "<company name>\<product family name>"
// for use with Windows registry configuration setup.
// For "normal" registry operations the classes below should be used
// instead of this function.
//
const FrameworkLinkage RMBCString& GetKPrintShopFamilyRegistryPath()
{
#if 0
	static RMBCString printShopFamilyRegistryPath("Broderbund Software\\The Print Shop Family");
#else
	static char* configFileName = "proident.pi";
	static RMBCString printShopFamilyRegistryPath;

	if (printShopFamilyRegistryPath.IsEmpty())
	{
		// default directory might be somewhere other than application directory
		// so get application path
		RMBCString path;
		char szAppPath [_MAX_PATH];
		char szDrive   [_MAX_DRIVE];
		char szDir     [_MAX_DIR];
		GetModuleFileName( NULL, szAppPath, _MAX_PATH );
		_splitpath( szAppPath, szDrive, szDir, NULL, NULL );
		path = szDrive;
		path += szDir;
		path += configFileName;

		productIdentifier proident = {0};
		if (proidLoad( path, proident ) > 0)
			printShopFamilyRegistryPath = RMBCString(proident.company) + "\\" + RMBCString(proident.family);
		if (printShopFamilyRegistryPath.IsEmpty())
		{
			// can't use normal Renaissance framework or MFC error handling
			// since we can be called by static initializers on startup
			MessageBox( ::GetActiveWindow(), kSTRING_ERROR_GENERAL_COMPONENT_MISSING, "", MB_ICONSTOP | MB_OK | MB_TASKMODAL );
			if (RMBCString(configFileName) == RMBCString("proident.pi"))
				exit( EXIT_FAILURE );
		}
	}
#endif

	return printShopFamilyRegistryPath;
}


// GetKApplicationRegistryPath()
//
// Returns a string of the form "<application name>\<version number>"
// for use with Windows registry configuration setup
// For "normal" registry operations the classes below should be used
// instead of this function.
//
const FrameworkLinkage RMBCString& GetKApplicationRegistryPath()
{
#if 0
	static RMBCString applicationRegistryPath("The Print Shop\\6.0");
#else
	static RMBCString applicationRegistryPath;

	if (applicationRegistryPath.IsEmpty())
		applicationRegistryPath = GetNamedApplicationRegistryPath();
#endif

	return applicationRegistryPath;
}

// GetNamedApplicationRegistryPath( char* configFileName = "proident.pi" )
//
// Returns a string of the form "<application name>\<version number>"
// for use with Windows registry configuration setup
// If configFileName is supplied, specifies the name
// of the "product identification file" which defaults to proident.pi.
// For "normal" registry operations the classes below should be used;
// this function should only be used when trying to launch other applications in
// the print shop family.
//
const FrameworkLinkage RMBCString& GetNamedApplicationRegistryPath( char* configFileName /* = "proident.pi" */ )
{
#if 0
	static RMBCString namedApplicationRegistryPath("The Print Shop\\6.0");
#else
	static RMBCString namedApplicationRegistryPath;
	namedApplicationRegistryPath.Empty();

	// default directory might be somewhere other than application directory
	// so get application path
	RMBCString path;
	char szAppPath [_MAX_PATH];
	char szDrive   [_MAX_DRIVE];
	char szDir     [_MAX_DIR];
	GetModuleFileName( NULL, szAppPath, _MAX_PATH );
	_splitpath( szAppPath, szDrive, szDir, NULL, NULL );
	path = szDrive;
	path += szDir;
	path += configFileName;

	productIdentifier proident = {0};
	if (proidLoad( path, proident ) > 0)
		namedApplicationRegistryPath = RMBCString(proident.product) + "\\" + RMBCString(proident.version);
	if (namedApplicationRegistryPath.IsEmpty())
	{
		// can't use normal Renaissance framework or MFC error handling
		// since we can be called by static initializers on startup
		MessageBox( ::GetActiveWindow(), kSTRING_ERROR_GENERAL_COMPONENT_MISSING, "", MB_ICONSTOP | MB_OK | MB_TASKMODAL );
		if (RMBCString(configFileName) == RMBCString("proident.pi"))
			exit( EXIT_FAILURE );
	}
#endif

	return namedApplicationRegistryPath;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RPrintShopFamilyConfiguration::RPrintShopFamilyConfiguration()
//	Description:	Constructor
//
const RMBCString RPrintShopFamilyConfiguration::kSharedCollectionsPath("Shared Collections Path");

RPrintShopFamilyConfiguration::RPrintShopFamilyConfiguration()
:	TConfiguration<RSystemConfiguration>("Software\\" + GetKPrintShopFamilyRegistryPath() + "\\")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RRenaissanceConfiguration::RRenaissanceConfiguration()
//	Description:	Constructor
//
const RMBCString RRenaissanceConfiguration::kCollectionsLocation("Collections Location");
const RMBCString RRenaissanceConfiguration::kInstallFromRoot("$Install_FromRoot$");
const RMBCString RRenaissanceConfiguration::kDictionariesLocation("Dictionaries Location");

RRenaissanceConfiguration::RRenaissanceConfiguration()
:	TConfiguration<RPrintShopFamilyConfiguration>(GetKApplicationRegistryPath())
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RRenaissanceUserConfiguration::RRenaissanceUserConfiguration()
//	Description:	Constructor
//
RRenaissanceUserConfiguration::RRenaissanceUserConfiguration()
: TConfiguration<RUserConfiguration>("Software\\" + GetKPrintShopFamilyRegistryPath() + "\\" + GetKApplicationRegistryPath() + "\\")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RRenaissanceUserPreferences::RRenaissanceUserPreferences()
//	Description:	Constructor
//
const RMBCString RRenaissanceUserPreferences::kDefaultFont("DefaultFont");
const RMBCString RRenaissanceUserPreferences::kHiResPhoto("HiResPhoto");
const RMBCString RRenaissanceUserPreferences::kBackgroundPrinting("BkgndPrint");
const RMBCString RRenaissanceUserPreferences::kFullPanelBitmapPrinting("FullPanelBitmapPrinting");
const RMBCString RRenaissanceUserPreferences::kWorkMethod("WorkMethod");
const RMBCString RRenaissanceUserPreferences::kFirstName("FirstName");
const RMBCString RRenaissanceUserPreferences::kLastName("LastName");
const RMBCString RRenaissanceUserPreferences::kAddress("Address");
const RMBCString RRenaissanceUserPreferences::kCity("City");
const RMBCString RRenaissanceUserPreferences::kState("State");
const RMBCString RRenaissanceUserPreferences::kZipCode("ZipCode");
const RMBCString RRenaissanceUserPreferences::kCountry("Country");
const RMBCString RRenaissanceUserPreferences::kTypeFaceFontList("TypeFace FontList");
const RMBCString RRenaissanceUserPreferences::kDesktopImageTile("Desktop Image Tile");

RRenaissanceUserPreferences::RRenaissanceUserPreferences()
:	TConfiguration<RRenaissanceUserConfiguration>("Preferences")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RRenaissanceLanguagePreferences::RRenaissanceLanguagePreferences()
//	Description:	Constructor
//
RRenaissanceLanguagePreferences::RRenaissanceLanguagePreferences()
:	TConfiguration<RRenaissanceUserConfiguration>("Language")
{
}
const RMBCString RRenaissanceLanguagePreferences::kPaperType("PaperType");


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RScrapbookConfiguration::RScrapbookConfiguration()
//	Description:	Constructor
//
const RMBCString RScrapbookConfiguration::kVisible("Visible");
const RMBCString RScrapbookConfiguration::kXPosition("X Position");
const RMBCString RScrapbookConfiguration::kYPosition("Y Position");
const RMBCString RScrapbookConfiguration::kFile("File");
const RMBCString RScrapbookConfiguration::kCurrentItem("Current Item");
const RMBCString RScrapbookConfiguration::kLoadTimeWarnThreshold("Load Time Warn Threshold");

RScrapbookConfiguration::RScrapbookConfiguration()
:	TConfiguration<RRenaissanceConfiguration>("Scrapbook")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RMainframeConfiguration::RMainframeConfiguration()
//	Description:	Constructor
//
const RMBCString RMainframeConfiguration::kPosition("Main Window Position");
const RMBCString RMainframeConfiguration::kToolbarData("Toolbar Data");
const RMBCString RMainframeConfiguration::kDebugMenu("Debug Menu");
const RMBCString RMainframeConfiguration::kPath2Project("Path2Project");

RMainframeConfiguration::RMainframeConfiguration()
:	TConfiguration<RRenaissanceUserConfiguration>("Mainframe")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RListConfiguration::RListConfiguration()
//	Description:	Constructor
//
const RMBCString RListConfiguration::kLastAddressList("LastAddressFile");
const RMBCString RListConfiguration::kLastCustomList("LastCustomFile");

RListConfiguration::RListConfiguration()
:	TConfiguration<RRenaissanceUserConfiguration>("Lists")
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RPowerUserConfiguration::RPowerUserConfiguration()
//	Description:	Constructor
//
const RMBCString RPowerUserConfiguration::kUndoCount("Undo");
const RMBCString RPowerUserConfiguration::kUnlockCD("UnlockCD");

RPowerUserConfiguration::RPowerUserConfiguration()
:	TConfiguration<RRenaissanceUserConfiguration>("PUser")	
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RToolConfiguration::RToolConfiguration()
//	Description:	Constructor
//
const RMBCString RToolConfiguration::kSpawnitPath("Spawnit Path");
const RMBCString RToolConfiguration::kDictionariesLocation("Dictionaries");
const RMBCString RToolConfiguration::kOldLibsVolume("OldLibs"); // DWORD 0=Product CD, 1=Install CD
const RMBCString RToolConfiguration::kDeluxeAdBitmaps("DeluxeAdBitmaps"); // DWORD 0="reinstall", 1="purchase deluxe version"

RToolConfiguration::RToolConfiguration()
:	TConfiguration<RRenaissanceConfiguration>("Tools")
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RGradientConfiguration::RGradientConfiguration()
//	Description:	Constructor
//
const RMBCString RGradientConfiguration::kMaxDpi("Maximum DPI");
const RMBCString RGradientConfiguration::kPrintPrecision("Print Precision");
const RMBCString RGradientConfiguration::kDisplayPrecision("Display Precision");

RGradientConfiguration::RGradientConfiguration()
:	TConfiguration<RRenaissanceConfiguration>("Gradients")
{
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RDocIDConfiguration::RDocIDConfiguration()
//	Description:	Constructor
//
const RMBCString RDocIDConfiguration::kLastDocumentID("LastDocID");

RDocIDConfiguration::RDocIDConfiguration()
:	TConfiguration<RRenaissanceUserConfiguration>("Documents")
{
}	



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RDocExpHTMLPermDirConfiguration::RDocExpHTMLPermDirConfiguration()
//	Description:	Constructor
//
const RMBCString RDocExpHTMLPermDirConfiguration::kExportHTMLPermanentDirectory("Last Used Export HTML Perm Dir");

RDocExpHTMLPermDirConfiguration::RDocExpHTMLPermDirConfiguration()
:	TConfiguration<RRenaissanceUserConfiguration>("Documents")
{
}
