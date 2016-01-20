/*
// $Workfile: PMWCFG.H $
// $Revision: 2 $
// $Date: 3/08/99 1:14p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWCFG.H $
// 
// 2     3/08/99 1:14p Mtaber
// Added Silver & Gold configurations
// Removed PublishingSuite & Deluxe (renamed to Silver & Gold)
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 44    2/25/99 3:34p Johno
// Added SupportsPhotoOrganizer() and GetPrintShopFamilyRegistryPath()
// Preliminary, for launching PhotoOrganizer
// 
// 43    2/23/99 2:12p Johno
// Support for 3DPlanet
// 
// 42    2/20/99 9:20p Psasse
// Support for Vertical Banners
// 
// 41    1/20/99 1:30p Mwilson
// added creatacard config object
// 
// 40    11/30/98 3:43p Mwilson
// added supportsOnlineGallery func for CParty
// 
// 39    11/23/98 1:27p Mwilson
// Added creataparty configuration
// 
// 38    11/17/98 5:22p Mwilson
// removed online art gallery button and menu for crafts deluxe
// 
// 37    10/19/98 5:07p Mwilson
// changed supported projects for craft deluxe
// 
// 36    10/13/98 4:45p Mwilson
// added filters for craft deluxe
// 
// 35    10/12/98 4:14p Johno
// Added CArtAndMoreOnly1::DoUnsuport()
// 
// 34    10/12/98 1:26p Mwilson
// added craft deluxe configuration
// 
// 33    10/01/98 2:59p Jayn
// Changes for the standalone Art & More CD.
// 
// 32    9/29/98 5:36p Johno
// Changes for Art and More Store
// 
// 31    9/29/98 12:32p Johno
// Changes for Art and More Store configuration
// 
// 30    9/28/98 10:53a Johno
// New ArtAndMoreOnly1 configuration
// 
// 29    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 28    8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 27    8/18/98 12:35p Dennis
// Photo Editor now supported in AG Print Standard
// 
// 26    8/06/98 4:33p Mwilson
// added supported projects for AG sku's for new craft project type
// 
// 25    7/22/98 5:02p Dennis
// American Greetings Filter additions
// 
// 24    7/22/98 12:05p Mwilson
// Added SupportsPersonalDelivery Function
// 
// 23    7/10/98 9:51a Johno
// Added #ifdef INSTALL around RemoveISP(void) - seems required to link.
// 
// 22    7/09/98 5:37p Cboggio
// 
// 21    7/09/98 10:47a Mwilson
// added supported projects function to PrintStandard config.  
// 
// 20    7/08/98 3:53p Dennis
// Added GetFilter override for Spiritual Expressions
// 
// 19    7/08/98 3:20p Mwilson
// added unsupported dialog features for print standart
// 
// 18    7/08/98 1:22p Dennis
// Minor PrintStandard Config changes
// 
// 17    7/08/98 1:04p Jayn
// Simplified filtering. Added Array variant to FillxxxFromList.
// 
// 16    7/06/98 5:44p Rlovejoy
// Implemented 'added on' project types.
// 
// 15    7/02/98 12:43p Mwilson
// added replacement text for reminder
// 
// 14    7/01/98 5:59p Psasse
// SetFilter method implemented
// 
// 13    7/01/98 5:22p Psasse
// more robust filtering routines
// 
// 12    6/25/98 7:35p Psasse
// added filtering of projects/collections/packs capability
// 
// 11    6/21/98 11:56a Cboggio
// Allowed for Serif Draw and YYWB to be removed for localization
// 
// 10    6/19/98 6:48p Cboggio
// Configuration classes for localized versions
// 
// 9     6/12/98 4:52p Fredf
// Got rid of supreme (no orders!)
// 
// 8     6/10/98 4:14p Dennis
// Added URL Methods.
// Setup base class for American Greetings Configs.
// 
// 7     6/06/98 2:16p Fredf
// Added Office Suite and Supreme.
// 
// 6     5/26/98 2:04p Dennis
// Added ProductLine enumerated type and Product() method
// 
// 5     5/06/98 10:31a Jayn
// Premier and Platinum configurations.
// 
// 4     5/04/98 3:53p Jayn
// Changes for Deluxe.
// 
// 3     4/21/98 9:04a Dennis
// Added American Greetings Configuration Objects
// 
// 2     4/20/98 2:18p Fredf
// Migrated 4.0 About Box and Startup dialog changes.
// 
//    Rev 1.2   17 Sep 1997 16:28:02   johno
// Support for registration bonus support check
// Added GetOEMURL(). Will return NULL
// or the entry in MSREG.INI:
// 
//  [OEM]
//  URL=http://152.163.199.34/johniso/
// 
// 
//    Rev 1.1   19 Aug 1997 15:37:14   johno
// Added IsHelpFile () so ADDON.INI can control the existence
// of help files for add ons ([EXTRACONTENT], HELP=1)
// 
// 
//    Rev 1.0   14 Aug 1997 15:24:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:12   Fred
// Initial revision.
// 
//    Rev 1.21   30 Jul 1997 12:42:30   johno
// GetHubDLLName is now static
// 
//    Rev 1.20   18 Jul 1997 16:01:34   Fred
// Sentiment browser disable for CL and BP
// 
//    Rev 1.19   02 Jul 1997 14:25:54   Jay
// LZW is disabled in Classic version
// 
//    Rev 1.18   27 Jun 1997 16:23:54   johno
// IsAddon is now fully static
// 
//    Rev 1.17   24 Jun 1997 11:34:18   Fred
// Added newsletters to bonus pack
// 
//    Rev 1.16   18 Jun 1997 17:20:18   johno
// 
//    Rev 1.15   11 Jun 1997 15:50:34   johno
// Add on support
// 
// 
//    Rev 1.14   10 Jun 1997 16:20:44   johno
// Add on control moved to configuration class
// 
// 
//    Rev 1.13   02 Jun 1997 09:42:06   Jay
// ProductFlags()
// 
//    Rev 1.12   19 May 1997 13:56:18   Fred
// Event Reminder, etc.
// 
//    Rev 1.11   28 Mar 1997 17:11:20   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.10   04 Mar 1997 17:04:08   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.9   08 Nov 1996 15:22:34   Jay
// Got rid of memory leaks.
// 
//    Rev 1.8   01 Nov 1996 09:22:50   Jay
// Moved changes over from PMW3.01.17
// 
//    Rev 1.8   30 Oct 1996 12:06:02   Fred
// Fixed Bonus Pack project number to include only supported projects
// 
//    Rev 1.7   16 Sep 1996 14:11:18   Jay
// Totem support.
// 
//    Rev 1.6   09 Sep 1996 16:49:10   Jay
// Spell-checking not available in BP and CL.
// 
//    Rev 1.5   08 Sep 1996 09:22:50   Jay
// Non-instant upgrade dialog.
// 
//    Rev 1.4   01 Sep 1996 16:46:22   Jay
// New item inclusion scheme.
// 
//    Rev 1.3   15 May 1996 08:46:34   Jay
// From PMW2
// 
//    Rev 1.20   13 May 1996 13:16:08   FRED
// Trial Version
// 
//    Rev 1.19   06 May 1996 14:31:32   JAY
// New project build code.
// 
//    Rev 1.18   06 May 1996 12:01:42   FRED
// Dynamically configurable project support
// 
//    Rev 1.17   26 Apr 1996 13:08:22   JAY
// More demo code
// 
//    Rev 1.16   24 Apr 1996 16:45:16   JAY
// New demo support
// 
//    Rev 1.15   21 Mar 1996 09:40:48   FRED
// Removed reliance on build.id, uses MSREG.INI instead
// 
//    Rev 1.14   23 Feb 1996 08:56:10   JAY
// Moved disabling of empty tabs to BP.
// 
//    Rev 1.13   14 Feb 1996 16:20:06   JAY
// New empty category support.
// 
//    Rev 1.12   14 Feb 1996 12:15:54   JAY
// New mechanism for Free Fonts
// 
//    Rev 1.11   06 Feb 1996 10:48:18   JAY
// Corrected project type support in BP and CL.
// 
//    Rev 1.10   05 Feb 1996 17:39:54   JAY
// More Classic and Bonus Pack code
// 
//    Rev 1.9   05 Feb 1996 08:06:04   JAY
//  
// 
//    Rev 1.8   31 Jan 1996 08:21:40   JAY
// More configuration stuff
// 
//    Rev 1.7   29 Jan 1996 16:53:26   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 1.6   25 Jan 1996 14:02:32   JAY
// Classic and Bonus Pack configurations.
// 
//    Rev 1.5   24 Jan 1996 10:04:04   JAY
// New string management in Configuration
// 
//    Rev 1.4   24 Jan 1996 08:44:04   JAY
//  
// 
//    Rev 1.3   23 Jan 1996 12:31:18   JAY
// Configuration in INSTALL. Strings, etc.
// 
//    Rev 1.2   19 Jan 1996 09:35:58   JAY
// Support for empty categories.
// 
//    Rev 1.1   18 Jan 1996 17:10:14   JAY
// New project configuration class
// 
//    Rev 1.0   17 Jan 1996 17:39:32   FRED
// Initial revision.
*/ 

#define DemoIniFileName	"PMWDEMO.INF"
#define NO_DEMO_INTEGER	(-1234)			// A semi-random value... not!
#define NO_DEMO_DWORD	(0)				// A semi-random value... not!

#define AddOnIniFileName	"ADDON.INI"
#ifdef  WIN32
   #define HUBDllNameNormal   "hubres32.dll"
#else
   #define HUBDllNameNormal   "hubres.dll"
#endif   

#define	Planet3DDefaultHTMLName	"default.htm"

class CPrintMasterConfiguration;

#ifndef __PMWCFG_H__
#define __PMWCFG_H__

#include "inifile.h"
#include "ipathmgr.h"

class CPmwDoc;

/////////////////////////////////////////////////////////////////////////////
// CPrintMasterConfiguration class

class CPrintMasterConfiguration
{
public:
	CPrintMasterConfiguration();
	virtual ~CPrintMasterConfiguration();

public:
   enum ProductLine
   {
      plPrintMaster,
      plAmericanGreetings
   };

	enum
	{
		FirstConfiguration = 1,

		Demo = FirstConfiguration,		// Demo
		Classic,								// Upgradable Classic version
		BonusPack,							// Upgradable Bonus Pack version
		Silver,					// Full Silver (was Publishing Suite) version
		Gold,								// Gold (was Deluxe) version
      AddOn,                        // Add on, such as NG
      Platinum,							// Platinum version
      NGCards,								// National Geographic cards edition
      Premier,								// Premier version
      OfficeSuite,						// Office Suite

	// Printmaster 7.0 Localized configurations
	
	  // UK English
	  ClassicENG,
	  SilverENG,
	  GoldENG,
	  PlatinumENG,

	  // German
	  ClassicDEU,
	  SilverDEU,
	  GoldDEU,
	  PlatinumDEU,

	  // French
	  ClassicFRA,
	  SilverFRA,
	  GoldFRA,
	  PlatinumFRA,


      // American Greetings Configurations
      PrintStandard = 150,          // Print - Standard Version
      PrintPremium,                 // Print - Premium Version
      CraftStandard,                // Craft - Standard Version
      SpiritStandard,               // Spirit - Standard Version
		ArtAndMoreOnly1,
		CraftDeluxe,
		CreataParty,
		CreataCard,
		LastConfiguration					// use with < (not <=)
	};

public:
	// Product Line info.
	virtual ProductLine Product(void) const;

	// Level access.

	virtual int ProductLevel(void) const = 0;
	virtual WORD ProductFlags(void) const = 0;		// Used by browser.

	// Feature support.

	virtual BOOL SupportsPersonalDelivery(BOOL fComplain = FALSE);
	virtual BOOL SupportsProjectType(int nProjectType, BOOL fComplain = TRUE);
	virtual BOOL SupportsSaveProject(BOOL fComplain = TRUE);
	virtual BOOL SupportsCardBack(BOOL fComplain = TRUE, CPmwDoc* pDoc = NULL);
	virtual BOOL SupportsAddressBook(BOOL fComplain = TRUE);
	virtual BOOL SupportsEventReminder(BOOL fComplain = TRUE);
	virtual BOOL SupportsPhotoEditor(BOOL fComplain = TRUE);
	virtual BOOL SupportsCartoonMaker(BOOL fComplain = TRUE);
	virtual BOOL SupportsUnknownProjects(BOOL fComplain = TRUE);
	virtual BOOL SupportsNetworkInterface(BOOL fComplain = TRUE);
	virtual BOOL SupportsPictureCatalog(BOOL fComplain = TRUE);
	virtual BOOL SupportsVirtualFile(int nVersion, BOOL fComplain = TRUE);
	virtual BOOL SupportsUpgrading(void);
	virtual BOOL SupportsSpellCheck(BOOL fComplain = TRUE);
	virtual BOOL SupportsLZW(BOOL fComplain = TRUE);
	virtual BOOL SupportsCardServer(BOOL fComplain = TRUE);
	virtual BOOL SupportsSentimentGallery(BOOL fComplain = TRUE);
	virtual BOOL SupportsSerifDrawPlus(BOOL fComplain = TRUE);
	virtual BOOL SupportsBorderPlus(BOOL fComplain = TRUE);
	virtual BOOL SupportsInstructionPage();
	virtual BOOL SupportsOnlineGallery();
	virtual BOOL IsRegistrationFace(int nFlags);
	virtual BOOL AddProjectType(DWORD dwProjectTypeMask);
	virtual BOOL SupportsVerticalBanners();

   virtual BOOL	SupportsPhotoOrganizer(void)
	{
		return (FALSE);
	}

	BOOL	SupportsExtraContent(void)
	{
		return GetExtraContentName() != NULL;
	}

	virtual LPCSTR GetExtraContentName(void)
   {
      return NULL;
   }

	virtual LPCSTR
	GetPrintShopFamilyRegistryPath(void)
	{
		return "Broderbund Software\\The Print Shop Family";
	}
// added for localized versions
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemovePhotoEditor(void);
	virtual BOOL RemoveCartoonMaker(void);
	virtual BOOL RemoveRegistration(void);
	virtual BOOL RemoveSerifDraw(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
// end of localization additions
	virtual LPCSTR ReplacementText(char cID);
	virtual LPCSTR SupportedProjectsList(void);
	virtual DWORD SupportedProjects(void);
	virtual DWORD ProjectSupportedMask(int nProjectType);
	virtual CString& LoadString(UINT nID, CString& csString);
	virtual BOOL ReplaceText(CString& csText);
	virtual void ReplaceWindowText(CWnd* pWnd);
	virtual void ReplaceDialogText(CWnd* pWnd);
	virtual int MessageBox(UINT uText, UINT uCaption = 0, UINT nType = MB_OK);
	virtual void DocumentFilter(CString& csFilter);
	virtual void PictureFilter(CString& csFilter);

	virtual BOOL ExcludeFilters(LPCSTR pszFilters) const;
	virtual BOOL ExcludeFilters(const CStringArray& csaFilters) const;
	virtual BOOL GetFilterList(CStringArray& csaFilters) const;
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.
	// Can we instantly upgrade?
	virtual BOOL CanInstantUpgrade(void);
   // Is this a network version?
   virtual BOOL IsNetworkVersion(void);
 	// Home CD management.
	/*
	// Return a char string that is the root path for home CD files.
	// This string is assumed to be modifiable.
	// This replaces "A:\\" in the path manager.
	*/
	virtual LPSTR GetHomeCDRoot(void);
	// User-interface.
	virtual BOOL UnsupportedDialog(LPCSTR pOperation = NULL, LPCSTR pTitle = NULL);

	static BOOL Create(int nConfiguration = Silver, IPathManager* pPathManager = NULL);

	static void Destroy(void);		// for end of program
	static CString& SafeLoadString(UINT nID, CString& csString);
	static CPrintMasterConfiguration* m_pConfiguration;
	/*
	// Get the signature file for id'ing the CD.
	// This is used for the "run" CD in those cases where the "install" and
	// "run" CDs are different.
	*/
	static LPCSTR GetSignatureFile(int nConfiguration);
	// Get the signature used for "install-only" CDs (where the "run" CD
	// is something else).
	static LPCSTR GetInstallSignatureFile(int nConfiguration);
	/*
	// Get the volume label for the CD (not effected by instant upgrades).
	*/
	static LPCSTR GetVolumeLabel(int nConfiguration, int nBuildId);
   
   // Add on stuff
   
   static int                 m_nIsAddon;

   static BOOL
   IsAddOn (void);

   static LPCSTR GetAddOnININame (void);

	virtual LPCSTR GetHubDLLName (void);
   virtual LPCSTR GetHTMLName (void);
   virtual BOOL   IsHelpFile (void);
   virtual BOOL   SupportsRegistrationBonus (void);
   virtual LPCSTR GetOEMURL (void);
   virtual CString GetProductURLFileName(void);
   virtual LPCSTR GetProductURL();
   virtual LPCSTR GetOnlineGreetingsFTPSite(void);

	// Various default resolutions.
	virtual CPoint GetDefaultBMPResolution(void);
	virtual CPoint GetDefaultGIFResolution(void);
	virtual CPoint GetDefaultJPGResolution(void);
	virtual CPoint GetDefaultPCXResolution(void);
	virtual CPoint GetDefaultPNGResolution(void);
protected:
   static CString s_HubDLLName;
   static CString s_HTMLName; 
   static CString s_AddOnININame;

   BOOL				m_fCheckedOEMURL;
   CString			m_csOEMURL;
	DWORD				m_dwAddedProjectTypeMask;
	IPathManager*	m_pPathManager;

   virtual BOOL Init(IPathManager* pPathManager);
};

/////////////////////////////////////////////////////////////////////////////
// CDemoConfiguration class

class CDemoConfiguration : public CPrintMasterConfiguration
{
	INHERIT(CDemoConfiguration, CPrintMasterConfiguration)

public:
	CDemoConfiguration();
	virtual ~CDemoConfiguration();

public:
	// Level access.

	virtual int ProductLevel(void) const;
	virtual WORD ProductFlags(void) const;		// Used by browser.

	// Feature support.

	virtual BOOL SupportsSaveProject(BOOL fComplain = TRUE);
	virtual BOOL SupportsCardBack(BOOL fComplain = TRUE, CPmwDoc* pDoc = NULL);
	virtual BOOL SupportsAddressBook(BOOL fComplain = TRUE);
	virtual BOOL SupportsEventReminder(BOOL fComplain = TRUE);
	virtual BOOL SupportsPhotoEditor(BOOL fComplain = TRUE);
	virtual BOOL SupportsCartoonMaker(BOOL fComplain = TRUE);
	virtual BOOL SupportsUnknownProjects(BOOL fComplain = TRUE);
	virtual BOOL SupportsNetworkInterface(BOOL fComplain = TRUE);
	virtual BOOL SupportsPictureCatalog(BOOL fComplain = TRUE);
	virtual BOOL SupportsUpgrading(void);

	virtual BOOL SupportsVirtualFile(int nVersion, BOOL fComplain = TRUE);
	virtual BOOL IsRegistrationFace(int nFlags);

	virtual LPCSTR ReplacementText(char cID);
	virtual DWORD SupportedProjects(void);

	// Can we instantly upgrade?
	virtual BOOL CanInstantUpgrade(void);

	/*
	// Return a char string that is the root path for home CD files.
	// This string is assumed to be modifiable.
	// This replaces "A:\\" in the path manager.
	*/

	virtual LPSTR GetHomeCDRoot(void);

	// User-interface.

	virtual BOOL UnsupportedDialog(LPCSTR pOperation = NULL, LPCSTR pTitle = NULL);

	static LPCSTR GetSignatureFile(int nConfiguration);
	static LPCSTR GetLabelFormat(int nConfiguration);

protected:
	virtual BOOL Init(IPathManager* pPathManager);

	// The Demo ini file. Do NOT use this directly.
	static CIniFile m_DemoIniFile;
	// Get the Demo ini file. This will set its name as needed.
	static CIniFile& GetDemoIniFile(void);
	// This is provided by the application environment.
	static void GetDemoIniFileName(CString& csFileName);

	// Convenient function for loading an INI string.
	static LPCSTR LoadDemoIniString(CString& csString, LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault = NULL);
	static int LoadDemoIniInteger(int& nValue, LPCSTR pszSection, LPCSTR pszEntry, int nDefault);
	static DWORD LoadDemoIniDword(DWORD& dwValue, LPCSTR pszSection, LPCSTR pszEntry, DWORD dwDefault);

	// Configuration variables. These are read from the demo ini file.
	// Static variables are used by static functions (often before the
	// configuration has been created - bootstrap).
	static CString m_csSignatureFile;
	static CString m_csLabelFormat;

	CString m_csInstallDirectory;
	CString m_csHomeCDRoot;

	int m_nVirtualLevel;
	int m_nRegistrationFace;
	int m_nInstantUpgrade;
};

/////////////////////////////////////////////////////////////////////////////
// CGoldConfiguration class

class CGoldConfiguration : public CPrintMasterConfiguration
{
	INHERIT(CGoldConfiguration, CPrintMasterConfiguration)

public:
	CGoldConfiguration();
	virtual ~CGoldConfiguration();

	// Flags
	enum
	{
		FLAG_ImsiInstall = 1
	};
public:
	// Level access.

	virtual int ProductLevel(void) const;
	virtual WORD ProductFlags(void) const;		// Used by browser.

	// Feature support.

	virtual LPCSTR ReplacementText(char cID);

	// User-interface.
protected:
};

/////////////////////////////////////////////////////////////////////////////
// CSilverConfiguration class

class CSilverConfiguration : public CGoldConfiguration
{
	INHERIT(CSilverConfiguration, CGoldConfiguration)

public:
	CSilverConfiguration();
	virtual ~CSilverConfiguration();

public:
	// Level access.

	virtual int ProductLevel(void) const;
	virtual WORD ProductFlags(void) const;		// Used by browser.

	// Feature support.

	virtual LPCSTR ReplacementText(char cID);

	// User-interface.
protected:
};

/////////////////////////////////////////////////////////////////////////////
// CBonusPackConfiguration class

class CBonusPackConfiguration : public CSilverConfiguration
{
	INHERIT(CBonusPackConfiguration, CSilverConfiguration)

public:
	CBonusPackConfiguration();
	virtual ~CBonusPackConfiguration();

public:
	// Level access.

	virtual int ProductLevel(void) const;
	virtual WORD ProductFlags(void) const;		// Used by browser.

	// Feature support.

	virtual BOOL SupportsAddressBook(BOOL fComplain = TRUE);
	virtual BOOL SupportsEventReminder(BOOL fComplain = TRUE);
	virtual BOOL SupportsPhotoEditor(BOOL fComplain = TRUE);
	virtual BOOL SupportsCartoonMaker(BOOL fComplain = TRUE);
	virtual BOOL SupportsUnknownProjects(BOOL fComplain = TRUE);
	virtual BOOL SupportsUpgrading(void);
	virtual BOOL SupportsSentimentGallery(BOOL fComplain = TRUE);

	virtual LPCSTR ReplacementText(char cID);
	virtual DWORD SupportedProjects(void);

	// User-interface.

	virtual BOOL UnsupportedDialog(LPCSTR pOperation = NULL, LPCSTR pTitle = NULL);
protected:
};

/////////////////////////////////////////////////////////////////////////////
// CClassicConfiguration class

class CClassicConfiguration : public CBonusPackConfiguration
{
	INHERIT(CClassicConfiguration, CBonusPackConfiguration)

public:
	CClassicConfiguration();
	virtual ~CClassicConfiguration();

public:
	// Level access.

	virtual int ProductLevel(void) const;
	virtual WORD ProductFlags(void) const;		// Used by browser.
	virtual BOOL SupportsUpgrading(void);
	virtual BOOL SupportsLZW(BOOL fComplain = TRUE);
	virtual BOOL SupportsCardServer(BOOL fComplain = TRUE);
	virtual BOOL SupportsSerifDrawPlus(BOOL fComplain = TRUE);
	virtual BOOL SupportsBorderPlus(BOOL fComplain = TRUE);

	// Feature support.

	virtual LPCSTR ReplacementText(char cID);
	virtual DWORD SupportedProjects(void);
	virtual void DocumentFilter(CString& csFilter);
	virtual void PictureFilter(CString& csFilter);

	// User-interface.
protected:
};

/////////////////////////////////////////////////////////////////////////////
// CConfigurationAddOn class
class CConfigurationAddOn : public CPrintMasterConfiguration
{
	INHERIT(CConfigurationAddOn, CPrintMasterConfiguration)
public:
	CConfigurationAddOn();
	virtual ~CConfigurationAddOn();

public:
	// Level access.

	virtual int ProductLevel(void) const;
	virtual WORD ProductFlags(void) const;		// Used by browser.

	// Feature support.

	virtual BOOL SupportsProjectType(int nProjectType, BOOL fComplain = TRUE);
	virtual BOOL SupportsSaveProject(BOOL fComplain = TRUE);
	virtual BOOL SupportsCardBack(BOOL fComplain = TRUE, CPmwDoc* pDoc = NULL);
	virtual BOOL SupportsAddressBook(BOOL fComplain = TRUE);
	virtual BOOL SupportsEventReminder(BOOL fComplain = TRUE);
	virtual BOOL SupportsPhotoEditor(BOOL fComplain = TRUE);
	virtual BOOL SupportsCartoonMaker(BOOL fComplain = TRUE);
	virtual BOOL SupportsUnknownProjects(BOOL fComplain = TRUE);
	virtual BOOL SupportsNetworkInterface(BOOL fComplain = TRUE);
	virtual BOOL SupportsPictureCatalog(BOOL fComplain = TRUE);
	virtual BOOL SupportsVirtualFile(int nVersion, BOOL fComplain = TRUE);
	virtual BOOL SupportsUpgrading(void);
	virtual BOOL SupportsSpellCheck(BOOL fComplain = TRUE);
	virtual BOOL IsRegistrationFace(int nFlags);
	virtual LPCSTR ReplacementText(char cID);

	// Can we instantly upgrade?
	virtual BOOL CanInstantUpgrade(void);

	static void Destroy(void);		// for end of program

   virtual LPCSTR GetExtraContentName(void);
   
   LPCSTR GetHTMLName (void);
   static LPCSTR GetSignatureFile (void);
   static LPCSTR GetVolumeLabel (void);
   BOOL   IsHelpFile (void);

protected:

	static   CString  m_csSignatureFile;
	static   CString  m_csVolumeLabel;
	static   CString  m_csName;
   static   CString  m_csExtraContentName;
	static   BOOL     m_fCheckedExtraContentName;
            BOOL     m_fIsHelpFile;
};

/////////////////////////////////////////////////////////////////////////////
// CPlatinumConfiguration class

class CPlatinumConfiguration : public CPrintMasterConfiguration
{
   INHERIT(CPlatinumConfiguration, CPrintMasterConfiguration)

public:
   CPlatinumConfiguration();
   virtual ~CPlatinumConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.

   // Feature support.

   virtual LPCSTR ReplacementText(char cID);

   // User-interface.
   virtual LPCSTR GetExtraContentName(void)
   {
      return Planet3DDefaultHTMLName;
   }

   BOOL	SupportsPhotoOrganizer(void)
	{
		return (TRUE);
	}

protected:
};

/////////////////////////////////////////////////////////////////////////////
// CNGCardsConfiguration class

class CNGCardsConfiguration : public CSilverConfiguration
{
   INHERIT(CNGCardsConfiguration, CSilverConfiguration)
public:
   virtual int ProductLevel(void) const;
   virtual LPCSTR ReplacementText(char cID);
};

/////////////////////////////////////////////////////////////////////////////
// CPremierConfiguration class

class CPremierConfiguration : public CPrintMasterConfiguration
{
   INHERIT(CPremierConfiguration, CPrintMasterConfiguration)

public:
   CPremierConfiguration();
   virtual ~CPremierConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.

   // Feature support.

   virtual LPCSTR ReplacementText(char cID);

   // User-interface.
   virtual LPCSTR GetExtraContentName(void)
   {
      return Planet3DDefaultHTMLName;
   }

   BOOL	SupportsPhotoOrganizer(void)
	{
		return (TRUE);
	}

protected:
};

/////////////////////////////////////////////////////////////////////////////
// COfficeSuiteConfiguration class

class COfficeSuiteConfiguration : public CPremierConfiguration
{
   INHERIT(COfficeSuiteConfiguration, CPremierConfiguration)

public:
   COfficeSuiteConfiguration();
   virtual ~COfficeSuiteConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.

   // Feature support.

   virtual LPCSTR ReplacementText(char cID);

   // User-interface.
protected:
};

///////////////////////////////////////////
// American Greetings Configuration Objects

class CAGConfiguration : public CPrintMasterConfiguration
{
   INHERIT(CAGConfiguration, CPrintMasterConfiguration)

public:
   // Construction
   CAGConfiguration();
   virtual ~CAGConfiguration();

   // Level access.

   // Feature support.
   virtual BOOL SupportsPersonalDelivery(BOOL fComplain = FALSE);
	virtual DWORD SupportedProjects(void);
   virtual BOOL SupportsCartoonMaker(BOOL fComplain = TRUE);
   virtual CString GetProductURLFileName(void);
   virtual LPCSTR GetOnlineGreetingsFTPSite(void);
   virtual LPCSTR ReplacementText(char cID);
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.

	// Various default resolutions.
	virtual CPoint GetDefaultBMPResolution(void);
	virtual CPoint GetDefaultJPGResolution(void);
	virtual CPoint GetDefaultPNGResolution(void);
};

/////////////////////////////////////////////////////////////////////////////
// CPrintStandardConfiguration class

class CPrintStandardConfiguration : public CAGConfiguration
{
   INHERIT(CPrintStandardConfiguration, CAGConfiguration)

public:
   CPrintStandardConfiguration();
   virtual ~CPrintStandardConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.

   // Feature support.

	virtual DWORD SupportedProjects(void);
	virtual BOOL UnsupportedDialog(LPCSTR pOperation = NULL, LPCSTR pTitle = NULL);
   virtual BOOL SupportsUpgrading(void);
	virtual BOOL SupportsCardServer(BOOL fComplain = TRUE);
	virtual BOOL SupportsAddressBook(BOOL fComplain = TRUE);
	virtual BOOL SupportsEventReminder(BOOL fComplain = TRUE);
   virtual LPCSTR ReplacementText(char cID);

   // User-interface.
protected:
};

class CArtAndMoreOnly1 : public CPrintStandardConfiguration
{
   INHERIT(CArtAndMoreOnly1, CPrintStandardConfiguration)

   virtual LPCSTR 
	ReplacementText(char cID);
	
	virtual BOOL 
	SupportsBorderPlus(BOOL fComplain = TRUE);

	virtual BOOL 
	SupportsSentimentGallery(BOOL fComplain = TRUE);

   virtual BOOL 
	SupportsPhotoEditor(BOOL fComplain =TRUE);

	virtual BOOL 
	SupportsLZW(BOOL fComplain = TRUE);

	virtual BOOL 
	SupportsEventReminder(BOOL fComplain = TRUE);

   virtual BOOL 
	SupportsPersonalDelivery(BOOL fComplain = FALSE);

	virtual BOOL 
	CanInstantUpgrade(void);

	virtual BOOL 
	SupportsCardServer(BOOL fComplain = TRUE);

	virtual BOOL 
	SupportsAddressBook(BOOL fComplain = TRUE);

	virtual BOOL 
	RemoveRegistration(void);

   virtual BOOL   
	SupportsRegistrationBonus (void)
	{
		return FALSE;
	}

#ifdef INSTALL
	virtual BOOL RemoveISP(void)
		{ return TRUE; }
#endif

public:
	virtual LPCSTR 
	GetHubDLLName (void)
	{
		return "artmhub1.dll";
	}
	// This does not need to be internationalized
	virtual LPCSTR 
	GetUnsupportedString(void)
	{
		return "\n\nTo obtain this feature, visit your local software retailer, or call Mindscape® Direct at 1-800-299-2082 to purchase a copy of American Greetings® Print!";
	}
	
	void
	DoUnsuport(LPCSTR fnc);

protected:
};

class CPrintPremiumConfiguration : public CAGConfiguration
{
   INHERIT(CPrintPremiumConfiguration, CAGConfiguration)

public:
   CPrintPremiumConfiguration();
   virtual ~CPrintPremiumConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.

   // Feature support.

   virtual BOOL SupportsUpgrading(void);
   virtual LPCSTR ReplacementText(char cID);

   // User-interface.
protected:
};

class CCraftStandardConfiguration : public CAGConfiguration
{
   INHERIT(CCraftStandardConfiguration, CAGConfiguration)

public:
   CCraftStandardConfiguration();
   virtual ~CCraftStandardConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.

   // Feature support.

	virtual BOOL SupportsUpgrading(void);
	virtual DWORD SupportedProjects(void);
	virtual BOOL SupportsPersonalDelivery(BOOL fComplain = FALSE);
	virtual BOOL SupportsInstructionPage();
	virtual LPCSTR ReplacementText(char cID);

   // User-interface.
protected:
};

class CCraftDeluxeConfiguration : public CCraftStandardConfiguration
{
   INHERIT(CCraftStandardConfiguration, CCraftStandardConfiguration)

public:
   CCraftDeluxeConfiguration();
   virtual ~CCraftDeluxeConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.

   // Feature support.
   virtual BOOL SupportsUpgrading(void);
   virtual LPCSTR ReplacementText(char cID);
	virtual BOOL SupportsOnlineGallery();

   // User-interface.
   virtual LPCSTR GetExtraContentName(void)
   {
      return Planet3DDefaultHTMLName;
   }

   BOOL	SupportsPhotoOrganizer(void)
	{
		return (TRUE);
	}

protected:
};

class CCreataCardConfiguration : public CAGConfiguration
{
   INHERIT(CCreataCardConfiguration, CAGConfiguration)

public:
   CCreataCardConfiguration();
   virtual ~CCreataCardConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.

   // Feature support.
	virtual BOOL SupportsOnlineGallery();
	virtual BOOL SupportsUpgrading(void);
	virtual DWORD SupportedProjects(void);
	virtual BOOL SupportsPersonalDelivery(BOOL fComplain = FALSE);
	virtual BOOL SupportsInstructionPage();
	virtual LPCSTR ReplacementText(char cID);

   // User-interface.
protected:
};

class CCreataPartyConfiguration : public CCraftStandardConfiguration
{
   INHERIT(CCraftStandardConfiguration, CCraftStandardConfiguration)

public:
   CCreataPartyConfiguration();
   virtual ~CCreataPartyConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.

   // Feature support.
	virtual BOOL SupportsOnlineGallery();
   virtual BOOL SupportsUpgrading(void);
   virtual LPCSTR ReplacementText(char cID);

   // User-interface.
protected:
};

class CSpiritStandardConfiguration : public CAGConfiguration
{
   INHERIT(CSpiritStandardConfiguration, CAGConfiguration)

public:
   CSpiritStandardConfiguration();
   virtual ~CSpiritStandardConfiguration();

public:
   // Level access.

   virtual int ProductLevel(void) const;
   virtual WORD ProductFlags(void) const;    // Used by browser.
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.

   // Feature support.

   virtual BOOL SupportsUpgrading(void);
   virtual LPCSTR ReplacementText(char cID);

   // User-interface.
protected:
};


/////////////////////////////////////////////////////////////////////////////
// Configuration classes for Localized versions
/////////////////////////////////////////////////////////////////////////////
// Platinum UK English
//
// CPlatinumConfigurationENG class

class CPlatinumConfigurationENG : public CPlatinumConfiguration
{
   INHERIT(CPlatinumConfigurationENG, CPlatinumConfiguration)

public:
   CPlatinumConfigurationENG();
   virtual ~CPlatinumConfigurationENG();
   virtual int ProductLevel(void) const;
   virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Deluxe UK English
//
// CGoldConfigurationENG class

class CGoldConfigurationENG : public CGoldConfiguration
{
	INHERIT(CGoldConfigurationENG, CGoldConfiguration)

public:
	CGoldConfigurationENG();
	virtual ~CGoldConfigurationENG();
	virtual int ProductLevel(void) const;
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Publishing Suite UK English
//
// CSilverConfigurationENG class
 
class CSilverConfigurationENG : public CSilverConfiguration
{
	INHERIT(CSilverConfigurationENG, CSilverConfiguration)

public:
	CSilverConfigurationENG();
	virtual ~CSilverConfigurationENG();
	virtual int ProductLevel(void) const;
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Classic UK English
//
// CClassicConfigurationENG class

class CClassicConfigurationENG : public CClassicConfiguration
{
	INHERIT(CClassicConfigurationENG, CClassicConfiguration)

public:
	CClassicConfigurationENG();
	virtual ~CClassicConfigurationENG();
      virtual BOOL SupportsUpgrading(void);
	virtual int ProductLevel(void) const;
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};


// German Configuration classes
/////////////////////////////////////////////////////////////////////////////
// Platinum GERMAN
//
// CPlatinumConfigurationDEU class

class CPlatinumConfigurationDEU : public CPlatinumConfiguration
{
   INHERIT(CPlatinumConfigurationDEU, CPlatinumConfiguration)

public:
   CPlatinumConfigurationDEU();
   virtual ~CPlatinumConfigurationDEU();
   virtual int ProductLevel(void) const;
   virtual BOOL RemoveYearYouWereBorn(void);
   virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Gold German
//
// CGoldConfigurationDEU class

class CGoldConfigurationDEU : public CGoldConfiguration
{
	INHERIT(CGoldConfigurationDEU, CGoldConfiguration)

public:
	CGoldConfigurationDEU();
	virtual ~CGoldConfigurationDEU();
	virtual int ProductLevel(void) const;
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Publishing Suite German
//
// CSilverConfigurationDEU class

class CSilverConfigurationDEU : public CSilverConfiguration
{
	INHERIT(CSilverConfigurationDEU, CSilverConfiguration)

public:
	CSilverConfigurationDEU();
	virtual ~CSilverConfigurationDEU();
	virtual int ProductLevel(void) const;
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Classic German
//
// CClassicConfigurationDEU class

class CClassicConfigurationDEU : public CClassicConfiguration
{
	INHERIT(CClassicConfigurationDEU, CClassicConfiguration)

public:
	CClassicConfigurationDEU();
	virtual ~CClassicConfigurationDEU();
      virtual BOOL SupportsUpgrading(void);
	virtual int ProductLevel(void) const;
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

// French Configuration classes
/////////////////////////////////////////////////////////////////////////////
// Platinum French
//
// CPlatinumConfigurationFRA class

class CPlatinumConfigurationFRA : public CPlatinumConfiguration
{
   INHERIT(CPlatinumConfigurationFRA, CPlatinumConfiguration)

public:
   CPlatinumConfigurationFRA();
   virtual ~CPlatinumConfigurationFRA();
   virtual int ProductLevel(void) const;
   virtual BOOL SupportsPhotoEditor(BOOL fComplain =TRUE);
   virtual BOOL SupportsCartoonMaker(BOOL fComplain =TRUE);
   virtual BOOL RemoveYearYouWereBorn(void);
   virtual BOOL RemovePhotoEditor(void);
   virtual BOOL RemoveCartoonMaker(void);
	virtual BOOL RemoveRegistration(void);
	virtual BOOL RemoveSerifDraw(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Gold French
//
// CGoldConfigurationFRA class

class CGoldConfigurationFRA : public CGoldConfiguration
{
	INHERIT(CGoldConfigurationFRA, CGoldConfiguration)

public:
	CGoldConfigurationFRA();
	virtual ~CGoldConfigurationFRA();
	virtual int ProductLevel(void) const;
      virtual BOOL SupportsPhotoEditor(BOOL fComplain =TRUE);
      virtual BOOL SupportsCartoonMaker(BOOL fComplain =TRUE);
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemovePhotoEditor(void);
	virtual BOOL RemoveCartoonMaker(void);
	virtual BOOL RemoveRegistration(void);
	virtual BOOL RemoveSerifDraw(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Publishing Suite French
//
// CSilverConfigurationFRA class

class CSilverConfigurationFRA : public CSilverConfiguration
{
	INHERIT(CSilverConfigurationFRA, CSilverConfiguration)

public:
	CSilverConfigurationFRA();
	virtual ~CSilverConfigurationFRA();
	virtual int ProductLevel(void) const;
      virtual BOOL SupportsPhotoEditor(BOOL fComplain =TRUE);
      virtual BOOL SupportsCartoonMaker(BOOL fComplain =TRUE);
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemovePhotoEditor(void);
	virtual BOOL RemoveCartoonMaker(void);
	virtual BOOL RemoveRegistration(void);
	virtual BOOL RemoveSerifDraw(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Classic French
//
// CClassicConfigurationFRA class

class CClassicConfigurationFRA : public CClassicConfiguration
{
	INHERIT(CClassicConfigurationFRA, CClassicConfiguration)

public:
	CClassicConfigurationFRA();
	virtual ~CClassicConfigurationFRA();
	virtual int ProductLevel(void) const;
    virtual BOOL SupportsUpgrading(void);
    virtual BOOL SupportsPhotoEditor(BOOL fComplain =TRUE);
    virtual BOOL SupportsCartoonMaker(BOOL fComplain =TRUE);
	virtual BOOL RemoveYearYouWereBorn(void);
	virtual BOOL RemovePhotoEditor(void);
	virtual BOOL RemoveCartoonMaker(void);
	virtual BOOL RemoveRegistration(void);
	virtual BOOL RemoveSerifDraw(void);
	virtual BOOL RemoveDividers(void);
#ifdef INSTALL
	virtual BOOL RemoveISP(void);
#endif
};


// THE global configuration object.

#define GetConfiguration()		(CPrintMasterConfiguration::m_pConfiguration)
#define LoadConfigurationString(i, s) GetConfiguration()->SafeLoadString(i, s)
#endif
