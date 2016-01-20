/*
// $Workfile: PMWCFG.H $
// $Revision: 2 $
// $Date: 3/08/99 3:50p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/PMWCFG.H $
// 
// 2     3/08/99 3:50p Mtaber
// Added Silver & Gold configurations
// This change removes the PublishingSuite & Deluxe configurations
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 2     11/03/98 9:55a Mwilson
// added craft deluxe config
// 
// 1     6/22/98 10:04a Mwilson
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

class CPrintMasterConfiguration;

#ifndef __PMWCFG_H__
#define __PMWCFG_H__

#include "inifile.h"

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
		Silver,					// Full Publishing Suite version
		Gold,								// Deluxe version
      AddOn,                        // Add on, such as NG
      Platinum,							// Platinum version
      NGCards,								// National Geographic cards edition
      Premier,								// Premier version
      OfficeSuite,						// Office Suite
      Supreme,								// Comes with a Coke and large fries

      // American Greetings Configurations
      PrintStandard = 150,          // Print - Standard Version
      PrintPremium,                 // Print - Premium Version
      CraftStandard,                // Craft - Standard Version
      SpiritStandard,               // Spirit - Standard Version
		ArtAndMoreOnly1,
		CraftDeluxe,
		LastConfiguration					// use with < (not <=)
	};

public:
	// Product Line info.
	virtual ProductLine Product(void) const;

	// Level access.

	virtual int ProductLevel(void) const = 0;
	virtual WORD ProductFlags(void) const = 0;		// Used by browser.

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
	virtual BOOL SupportsLZW(BOOL fComplain = TRUE);
	virtual BOOL SupportsCardServer(BOOL fComplain = TRUE);
	virtual BOOL SupportsSentimentGallery(BOOL fComplain = TRUE);
	virtual BOOL IsRegistrationFace(int nFlags);

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

	static BOOL Create(int nConfiguration = Silver);

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

   virtual LPCSTR GetExtraContentName (void)
   {
      return NULL;
   }

   static LPCSTR GetAddOnININame (void);

   static  LPCSTR GetHubDLLName (void);
   virtual LPCSTR GetHTMLName (void);
   virtual BOOL   IsHelpFile (void);
   virtual BOOL   SupportsRegistrationBonus (void);
   virtual LPCSTR GetOEMURL (void);

protected:
   static CString s_HubDLLName;
   static CString s_HTMLName; 
   static CString s_AddOnININame;

   BOOL     m_fCheckedOEMURL;
   CString  m_csOEMURL;

   virtual BOOL Init(void);
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
	virtual BOOL Init(void);

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

   virtual LPCSTR GetExtraContentName (void);
   
   LPCSTR GetHubDLLName (void);
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

/////////////////////////////////////////////////////////////////////////////
// CSuprememConfiguration class

class CSupremeConfiguration : public CPremierConfiguration
{
   INHERIT(CSupremeConfiguration, CPremierConfiguration)

public:
   CSupremeConfiguration();
   virtual ~CSupremeConfiguration();

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
   virtual LPCSTR GetOnlineGreetingsFTPSite(void);
   virtual LPCSTR ReplacementText(char cID);
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.
   virtual WORD ProductFlags(void) const;    // Used by browser.

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
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.

   // Feature support.
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
	virtual LPCSTR GetFilters(void) const;		// Override this one for simplicity.

   // Feature support.

   virtual BOOL SupportsUpgrading(void);
   virtual LPCSTR ReplacementText(char cID);

   // User-interface.
protected:
};


// THE global configuration object.

#define GetConfiguration()		(CPrintMasterConfiguration::m_pConfiguration)
#define LoadConfigurationString(i, s) GetConfiguration()->SafeLoadString(i, s)
#endif
