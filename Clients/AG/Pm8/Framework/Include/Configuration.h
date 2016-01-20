////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File name:		Configuration.h
//	Project:			Renaissance framework
//	Author:			G. Brown
//	Description:	Declaration of configuration classes
//	Portability:	Platform-independent
//	Developer:		Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//	Client:			Broderbund Software, Inc.
//
//	Copyright (c) 1997 Turning Point Software.  All Rights Reserved.
//

#ifndef	_CONFIGURATION_H_
#define	_CONFIGURATION_H_

#include "ConfigurationBase.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// GetKPrintShopFamilyRegistryPath()
//
// Returns a string of the form "<company name>\<product family name>"
// for use with Windows registry configuration setup.
// For "normal" registry operations the classes below should be used
// instead of this function.
//
const FrameworkLinkage RMBCString& GetKPrintShopFamilyRegistryPath();

// GetKApplicationRegistryPath()
//
// Returns a string of the form "<application name>\<version number>"
// for use with Windows registry configuration setup.
// For "normal" registry operations the classes below should be used
// instead of this function.
//
const FrameworkLinkage RMBCString& GetKApplicationRegistryPath();

// GetNamedApplicationRegistryPath( char* configFileName = "proident.pi" )
//
// Returns a string of the form "<application name>\<version number>"
// for use with Windows registry configuration setup.
// If configFileName is supplied, specifies the name
// of the "product identification file" which defaults to proident.pi.
// For "normal" registry operations the classes below should be used;
// this function should only be used when trying to launch other applications in
// the print shop family.
//
const FrameworkLinkage RMBCString& GetNamedApplicationRegistryPath( char* configFileName = "proident.pi" );


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RPrintShopFamilyConfiguration
//	Description:	Enscapsulates Print Shop Family registry entries
//
class FrameworkLinkage RPrintShopFamilyConfiguration : public TConfiguration<RSystemConfiguration>
{
public:
								RPrintShopFamilyConfiguration();

public:
	static const RMBCString		kSharedCollectionsPath;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RRenaissanceConfiguration
//	Description:	Enscapsulates Renaissance-specific registry entries
//
class FrameworkLinkage RRenaissanceConfiguration : public TConfiguration<RPrintShopFamilyConfiguration>
{
public:
								RRenaissanceConfiguration();

public:
	static const RMBCString		kCollectionsLocation;
	static const RMBCString		kInstallFromRoot;
	static const RMBCString		kDictionariesLocation;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RRenaissanceUserConfiguration
//	Description:	Enscapsulates Renaissance user-specific registry entries
//
class FrameworkLinkage RRenaissanceUserConfiguration : public TConfiguration<RUserConfiguration>
{
public:
								RRenaissanceUserConfiguration();
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RRenaissanceUserPreferences
//	Description:	Enscapsulates Renaissance user preferences
//
class FrameworkLinkage RRenaissanceUserPreferences : public TConfiguration<RRenaissanceUserConfiguration>
{
public:
								RRenaissanceUserPreferences();

public:	
	static const RMBCString		kDefaultFont;
	static const RMBCString		kHiResPhoto;
	static const RMBCString		kBackgroundPrinting;
	static const RMBCString		kFullPanelBitmapPrinting;
	static const RMBCString		kWorkMethod;
	static const RMBCString		kFirstName;
	static const RMBCString		kLastName;
	static const RMBCString		kAddress;
	static const RMBCString		kCity;
	static const RMBCString		kState;
	static const RMBCString		kZipCode;
	static const RMBCString		kCountry;
	static const RMBCString		kLanguageVersionFlag;
	static const RMBCString		kTypeFaceFontList;
	static const RMBCString		kDesktopImageTile;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RRenaissanceLanguagePreferences
//	Description:	Enscapsulates Renaissance language preferences
//
class FrameworkLinkage RRenaissanceLanguagePreferences : public TConfiguration<RRenaissanceUserConfiguration>
{
public:
										RRenaissanceLanguagePreferences();

public:	
	static const RMBCString		kPaperType;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RScrapbookConfiguration
//	Description:	Enscapsulates Scrapbook configuration
//
class FrameworkLinkage RScrapbookConfiguration : public TConfiguration<RRenaissanceConfiguration>
{
public:
								RScrapbookConfiguration();

public:
	static const RMBCString		kVisible;
	static const RMBCString		kXPosition;
	static const RMBCString		kYPosition;
	static const RMBCString		kFile;
	static const RMBCString		kCurrentItem;
	static const RMBCString		kLoadTimeWarnThreshold;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RMainframeConfiguration
//	Description:	Enscapsulates main frame window configuration
//
class FrameworkLinkage RMainframeConfiguration : public TConfiguration<RRenaissanceUserConfiguration>
{
public:
								RMainframeConfiguration();

public:
	static const RMBCString		kPosition;
	static const RMBCString		kToolbarData;
	static const RMBCString		kDebugMenu;
	static const RMBCString		kPath2Project;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RListConfiguration
//	Description:	Enscapsulates list configuration
//
class FrameworkLinkage RListConfiguration : public TConfiguration<RRenaissanceUserConfiguration>
{
public:
								RListConfiguration();

public:
	static const RMBCString		kLastAddressList;
	static const RMBCString		kLastCustomList;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RPowerUserConfiguration
//	Description:	Enscapsulates list configuration
//
class FrameworkLinkage RPowerUserConfiguration : public TConfiguration<RRenaissanceUserConfiguration>
{
public:
								RPowerUserConfiguration();

public:
	static const RMBCString		kUndoCount;
	static const RMBCString		kUnlockCD;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RToolConfiguration
//	Description:	Enscapsulates tools and other misc configuration
//
class FrameworkLinkage RToolConfiguration : public TConfiguration<RRenaissanceConfiguration>
{
public:
								RToolConfiguration();

public:
	static const RMBCString		kSpawnitPath;
	static const RMBCString		kDictionariesLocation;
	static const RMBCString		kOldLibsVolume;
	static const RMBCString		kDeluxeAdBitmaps;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RGradientConfiguration
//	Description:	Enscapsulates list configuration
//
class FrameworkLinkage RGradientConfiguration : public TConfiguration<RRenaissanceConfiguration>
{
public:
								RGradientConfiguration();

public:
	static const RMBCString		kMaxDpi;
	static const RMBCString		kPrintPrecision;
	static const RMBCString		kDisplayPrecision;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RDocIDConfiguration
//	Description:	Enscapsulates Last Document ID information
//
class FrameworkLinkage RDocIDConfiguration : public TConfiguration<RRenaissanceUserConfiguration>
{
public:
										RDocIDConfiguration();

public:
	static const RMBCString		kLastDocumentID;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RDocExpHTMLPermDirConfiguration
//	Description:	Enscapsulates Last Document ID information
//
class FrameworkLinkage RDocExpHTMLPermDirConfiguration : public TConfiguration<RRenaissanceUserConfiguration>
{
public:
										RDocExpHTMLPermDirConfiguration();

public:
	static const RMBCString		kExportHTMLPermanentDirectory;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _CONFIGURATION_H_