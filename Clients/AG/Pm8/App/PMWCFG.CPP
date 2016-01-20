/*
// $Workfile: PMWCFG.CPP $
// $Revision: 2 $
// $Date: 3/08/99 1:15p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWCFG.CPP $
// 
// 2     3/08/99 1:15p Mtaber
// Added Silver & Gold configurations
// Removed PublishingSuite & Deluxe (renamed to Silver & Gold)
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 60    2/20/99 9:20p Psasse
// Support for Vertical Banners
// 
// 59    2/04/99 3:05p Johno
// Changes for new "Blank Page" (poster) project type
// Disabled "Craft" for Printmaster
// 
// 58    1/20/99 1:30p Mwilson
// added creatacard config object
// 
// 57    12/24/98 5:07p Psasse
// added PhotoProjects type and re-enabled Crafts for all sku's
// 
// 56    11/30/98 3:43p Mwilson
// added supportsOnlineGallery func for CParty
// 
// 55    11/24/98 6:25p Johno
// SupportsProjectType() skips crafts for printmaster
// 
// 54    11/23/98 1:27p Mwilson
// Added creataparty configuration
// 
// 53    11/17/98 5:22p Mwilson
// removed online art gallery button and menu for crafts deluxe
// 
// 52    10/19/98 5:07p Mwilson
// changed supported projects for craft deluxe
// 
// 51    10/13/98 4:45p Mwilson
// added filters for craft deluxe
// 
// 50    10/12/98 4:14p Johno
// Added CArtAndMoreOnly1::DoUnsuport()
// 
// 49    10/12/98 1:26p Mwilson
// added craft deluxe configuration
// 
// 48    10/01/98 2:59p Jayn
// Changes for the standalone Art & More CD.
// 
// 47    9/29/98 5:36p Johno
// Changes for Art and More Store
// 
// 46    9/29/98 12:31p Johno
// Changes for Art and More Store configuration
// 
// 45    9/28/98 10:53a Johno
// New ArtAndMoreOnly1 configuration
// 
// 44    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 43    8/20/98 3:26p Dennis
// PNG resolution is now 100 for AG.
// 
// 42    8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 41    8/18/98 12:35p Dennis
// Photo Editor now supported in AG Print Standard
// 
// 40    8/06/98 4:33p Mwilson
// added supported projects for AG sku's for new craft project type
// 
// 39    7/30/98 3:11p Cboggio
// Get name of "Program Files" directory from Windows registry (currently
// for localized versions only)
// 
// 38    7/22/98 5:02p Dennis
// American Greetings Filter additions
// 
// 37    7/22/98 1:07p Jayn
// Uses shorter names for @@N
// 
// 36    7/22/98 12:05p Mwilson
// Added SupportsPersonalDelivery Function
// 
// 35    7/09/98 5:36p Cboggio
// Added CPrintmasterConfiguration::RemoveISP to remove AT&T for localized
// installs
// 
// 34    7/09/98 10:47a Mwilson
// added supported projects function to PrintStandard config.  
// 
// 33    7/08/98 3:53p Dennis
// Added GetFilter override for Spiritual Expressions
// 
// 32    7/08/98 3:20p Mwilson
// added unsupported dialog features for print standart
// 
// 31    7/08/98 1:49p Mwilson
// removed CanInstantUpgrade from print standard
// 
// 30    7/08/98 1:24p Dennis
// 
// 29    7/08/98 1:22p Mwilson
// Changed supports upgrade for print standard to TRUE
// 
// 28    7/08/98 1:04p Jayn
// Simplified filtering. Added Array variant to FillxxxFromList.
// 
// 27    7/06/98 5:50p Rlovejoy
// Put it right.
// 
// 26    7/06/98 5:44p Rlovejoy
// Implemented 'added on' project types.
// 
// 25    7/02/98 12:43p Mwilson
// added replacement text for reminder
// 
// 24    7/01/98 6:09p Psasse
// oops - deactivate sample code
// 
// 23    7/01/98 5:59p Psasse
// SetFilter method implemented
// 
// 22    7/01/98 5:22p Psasse
// more robust filtering routines
// 
// 21    6/29/98 12:29p Jayn
// Group name is always PrintMaster (no Platinum, Premier, etc).
// 
// 20    6/25/98 7:43p Psasse
// better
// 
// 19    6/25/98 7:35p Psasse
// added filtering of projects/collections/packs capability
// 
// 18    6/24/98 4:39p Dennis
// Added Destination directory for AG Configs
// 
// 17    6/21/98 11:56a Cboggio
// Allowed for Serif Draw and YYWB to be removed for localization
// 
// 16    6/19/98 6:47p Cboggio
// Configuration classes for localized versions
// 
// 15    6/17/98 10:37a Dennis
// Added ifdef for install program for GetProductURLFileName()
// 
// 14    6/12/98 4:52p Fredf
// Got rid of supreme (no orders!)
// 
// 13    6/10/98 4:14p Dennis
// Added URL Methods.
// Setup base class for American Greetings Configs.
// 
// 12    6/06/98 2:16p Fredf
// Added Office Suite and Supreme.
// 
// 11    5/26/98 2:04p Dennis
// Added ProductLine enumerated type and Product() method
// 
// 10    5/06/98 10:31a Jayn
// Premier and Platinum configurations.
// 
// 9     5/04/98 3:53p Jayn
// Changes for Deluxe.
// 
// 8     4/23/98 3:27p Dennis
// Added 'G" case for American Greetings replacement text.
// 
// 7     4/21/98 9:57a Dennis
// Added IsNetworkVersion() stub for install version.
// 
// 6     4/21/98 9:10a Dennis
// Added American Greetings Configuration Objects
// Added #ifdef INSTALL_BUILD so source file can be shared with install
// program.
// 
// 5     4/20/98 7:38p Fredf
// Removed "Gold"
// 
// 4     4/17/98 4:47p Fredf
// T-Shirts
// 
// 3     2/27/98 2:46p Jayn
// Changed for 5.0
// 
// 2     12/01/97 1:27p Jstoner
// created sticker project type
// 
//    Rev 1.3   19 Sep 1997 17:34:14   johno
// 
//    Rev 1.2   17 Sep 1997 16:29:12   johno
// Support for registration bonus support check
// Added GetOEMURL(). Will return NULL
// or the entry in MSREG.INI:
// 
//  [OEM]
//   URL=http://152.163.199.34/johniso/
// 
// 
//    Rev 1.1   19 Aug 1997 15:37:46   johno
// Added IsHelpFile () so ADDON.INI can control the existence
// of help files for add ons ([EXTRACONTENT], HELP=1)
// 
// 
//    Rev 1.0   14 Aug 1997 15:24:12   Fred
// Initial revision.
// 
//    Rev 1.1   14 Aug 1997 11:06:34   Fred
// changed 40 to 50
// 
//    Rev 1.0   14 Aug 1997 09:39:48   Fred
// Initial revision.
// 
//    Rev 1.38   30 Jul 1997 12:41:42   johno
// GetHubDLLName is now static
// 
//    Rev 1.37   18 Jul 1997 16:01:28   Fred
// Sentiment browser disable for CL and BP
// 
//    Rev 1.36   02 Jul 1997 14:25:52   Jay
// LZW is disabled in Classic version
// 
//    Rev 1.35   27 Jun 1997 17:42:16   johno
// Now GetAddOnININame works in 16 bit too!
// 
//    Rev 1.34   27 Jun 1997 16:24:22   johno
// IsAddon is now fully static
// 
// 
//    Rev 1.33   24 Jun 1997 11:34:16   Fred
// Added newsletters to bonus pack
// 
//    Rev 1.32   18 Jun 1997 16:42:36   johno
// Addon stuff
// 
//    Rev 1.31   11 Jun 1997 15:50:16   johno
// Add on support
// 
// 
//    Rev 1.30   10 Jun 1997 16:21:02   johno
// Add on control moved to configuration class
// 
// 
//    Rev 1.29   02 Jun 1997 09:42:14   Jay
// ProductFlags()
// 
//    Rev 1.28   19 May 1997 13:55:40   Fred
// Event Reminder, etc.
// 
//    Rev 1.27   04 Apr 1997 15:35:36   Jay
// Half-fold card and web projects
// 
//    Rev 1.26   28 Mar 1997 17:10:52   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.25   04 Mar 1997 17:03:44   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.24   11 Dec 1996 14:39:04   Jay
// 30 -> 40.
// 
//    Rev 1.23   18 Nov 1996 17:05:54   Jay
// AfxEnableMemoryTracking only in DEBUG build
// 
//    Rev 1.22   08 Nov 1996 15:46:08   johno
// 
//    Rev 1.21   08 Nov 1996 15:22:36   Jay
// Got rid of memory leaks.
// 
//    Rev 1.20   08 Nov 1996 11:31:12   johno
// Moved strings to resource DLL
// 
//    Rev 1.19   06 Nov 1996 17:00:38   johno
// Moved strings to resource DLL
// 
//    Rev 1.18   05 Nov 1996 09:54:38   Jay
// Include change
// 
//    Rev 1.17   01 Nov 1996 09:22:48   Jay
// Moved changes over from PMW3.01.17
// 
//    Rev 1.18   30 Oct 1996 12:06:02   Fred
// Fixed Bonus Pack project number to include only supported projects
// 
//    Rev 1.17   29 Oct 1996 19:45:16   Jay
// Made static buffer bigger in SupportedProjectList().
// 
//    Rev 1.16   09 Oct 1996 15:16:14   Jay
// Moved the BIN files back to the CD.
// 
//    Rev 1.15   09 Oct 1996 15:08:02   Jay
// Moved the BIN files to the working directory.
// 
//    Rev 1.14   09 Oct 1996 14:02:52   Jay
// Renamed DAT files to BIN files.
// 
//    Rev 1.13   16 Sep 1996 14:11:18   Jay
// Totem support.
// 
//    Rev 1.12   09 Sep 1996 16:49:24   Jay
// Spell-checking not available in BP and CL.
// 
//    Rev 1.11   08 Sep 1996 12:51:12   Fred
//  
// 
//    Rev 1.10   08 Sep 1996 09:22:48   Jay
// Non-instant upgrade dialog.
// 
//    Rev 1.9   07 Sep 1996 16:41:48   Jay
// BP and CL use the same PS files.
// 
//    Rev 1.8   01 Sep 1996 16:46:22   Jay
// New item inclusion scheme.
// 
//    Rev 1.7   14 Aug 1996 12:37:52   Jay
// New project build stuff.
// 
//    Rev 1.6   31 Jul 1996 11:57:50   Fred
// 16-bit compatibility
// 
//    Rev 1.5   23 Jul 1996 10:41:34   Jay
// Added newsletters and brochures; removed backgrounds
// 
//    Rev 1.4   18 Jul 1996 08:25:18   Jay
// 3.0!
// 
//    Rev 1.3   15 May 1996 08:46:32   Jay
// From PMW2
// 
//    Rev 1.39   13 May 1996 15:30:24   FRED
// No comman before last project name
// 
//    Rev 1.38   13 May 1996 13:15:50   FRED
// Trial Version
// 
//    Rev 1.37   10 May 1996 17:19:26   FRED
//  
// 
//    Rev 1.36   09 May 1996 16:20:18   FRED
// Trial Version
// 
//    Rev 1.35   06 May 1996 16:16:42   JAY
// Fixed typos
// 
//    Rev 1.34   06 May 1996 14:31:36   JAY
// New project build code.
// 
//    Rev 1.33   06 May 1996 12:01:44   FRED
// Dynamically configurable project support
// 
//    Rev 1.32   26 Apr 1996 18:09:32   FRED
// More demo stuff
// 
//    Rev 1.31   26 Apr 1996 13:08:26   JAY
// More demo code
// 
//    Rev 1.30   24 Apr 1996 16:45:14   JAY
// New demo support
// 
//    Rev 1.29   21 Mar 1996 17:25:50   JAY
// Fixed a bug with upgrading by clicking on album tab.
// 
//    Rev 1.28   21 Mar 1996 09:39:26   FRED
// Removed reliance on build.id, uses MSREG.INI instead
// 
//    Rev 1.27   23 Feb 1996 08:56:12   JAY
// Moved disabling of empty tabs to BP.
// 
//    Rev 1.26   14 Feb 1996 16:20:40   JAY
// New empty category support.
// 
//    Rev 1.25   14 Feb 1996 12:15:54   JAY
// New mechanism for Free Fonts
// 
//    Rev 1.24   12 Feb 1996 11:12:18   JAY
// Demo group name change
// 
//    Rev 1.23   12 Feb 1996 09:44:20   FRED
// Added 'the' to descriptive names
// 
//    Rev 1.22   12 Feb 1996 09:27:18   FRED
// Instant upgrade tweaks
// 
//    Rev 1.21   08 Feb 1996 09:23:22   JAY
// Demo changes
// 
//    Rev 1.20   07 Feb 1996 09:29:30   JAY
// New sound manager scheme: sound manager is open, but buttons bring up dialog.
// 
//    Rev 1.19   06 Feb 1996 10:48:18   JAY
// Corrected project type support in BP and CL.
// 
//    Rev 1.18   06 Feb 1996 07:53:48   JAY
// Ugh! Changed ">=" to "<=" in SupportsVirtualFile().
// 
//    Rev 1.17   05 Feb 1996 17:40:14   JAY
// More Classic and Bonus Pack code
// 
//    Rev 1.16   05 Feb 1996 14:09:08   FRED
// Dynamically displays program title is about box bitmap
// 
//    Rev 1.15   05 Feb 1996 08:06:06   JAY
//  
// 
//    Rev 1.14   31 Jan 1996 08:21:26   JAY
// More configuration stuff
// 
//    Rev 1.13   30 Jan 1996 08:35:52   JAY
// Added some code in MessageBox that MFC was doing.
// 
//    Rev 1.12   29 Jan 1996 16:53:10   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 1.11   26 Jan 1996 12:47:44   JAY
// Group name is now always just "PrintMaster Gold".
// 
//    Rev 1.10   26 Jan 1996 08:35:06   JAY
// 
//    Rev 1.9   25 Jan 1996 14:02:18   JAY
// Classic and Bonus Pack configurations.
// 
//    Rev 1.8   25 Jan 1996 09:20:14   JAY
//  
// 
//    Rev 1.7   24 Jan 1996 11:40:50   JAY
// More string customizations
// 
//    Rev 1.6   24 Jan 1996 10:04:02   JAY
// New string management in Configuration
// 
//    Rev 1.5   24 Jan 1996 08:43:44   JAY
//  
// 
//    Rev 1.4   23 Jan 1996 12:31:20   JAY
// Configuration in INSTALL. Strings, etc.
// 
//    Rev 1.3   22 Jan 1996 08:49:54   JAY
// More demo changes
// 
//    Rev 1.2   19 Jan 1996 09:35:56   JAY
// Support for empty categories.
// 
//    Rev 1.1   18 Jan 1996 17:10:20   JAY
// New project configuration class
// 
//    Rev 1.0   17 Jan 1996 17:39:32   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "pmw.h"
#include "pmwcfg.h"
#include "util.h"
#include "collctg.h"
#ifdef INSTALL
   #include "install.h"
   #undef GET_PMWAPP
   #define GET_PMWAPP()		(GetInstallApp())
#else
   #include "inifile.h"
#endif

#include <math.h>

#ifdef WIN32
#include <..\src\afximpl.h>         // for AfxGetSafeOwner()
#else
#include <auxdata.h>       // for _AfxGetSafeOwner()
#endif

#define CFG_3FLAG_UNKNOWN  2
#define CFG_3FLAG_YES      1
#define CFG_3FLAG_NO       0

// THE global configuration object.

CPrintMasterConfiguration* CPrintMasterConfiguration::m_pConfiguration = NULL;
static char BASED_CODE SECTION_Configuration[] = "Configuration";

// Array of known project types.
struct
{
   int m_nType;
   DWORD m_dwMask;
   UINT  m_uIDName;
} FAR Projects[] =
{
   { IDR_CARDTYPE,				0x800000L, IDS_GREETING_CARDS },
   { IDR_BANNERTYPE,				0x400000L, IDS_BANNERS        },
   { IDR_POSTERTYPE,				0x200000L, IDS_POSTERS        },
   { IDR_CALENDARTYPE,			0x100000L, IDS_CALENDARS      },
   { IDR_CERTIFICATETYPE,		0x080000L, IDS_CERTIFICATES   },
   { IDR_LABELTYPE,				0x040000L, IDS_LABELS         },
   { IDR_BIZCARDTYPE,			0x020000L, IDS_BUSINESS_CARDS },
   { IDR_STATIONERYTYPE,		0x010000L, IDS_LETTERHEAD     },
   { IDR_ENVELOPETYPE,			0x008000L, IDS_ENVELOPES      },
   { IDR_FAXCOVERTYPE,			0x004000L, IDS_FAX_SHEETS     },
   { IDR_NOTECARDTYPE,			0x002000L, IDS_NOTE_CARDS     },
   { IDR_NEWSLETTERTYPE,		0x001000L, IDS_NEWSLETTERS    },
   { IDR_BROCHURETYPE,			0x000800L, IDS_BROCHURES      },
   { IDR_HALFCARDTYPE,			0x000400L, IDS_HALFCARDS      },
   { IDR_WEBPUBTYPE,				0x000200L, IDS_WEBPUBS        },
   { IDR_POSTCARDTYPE,			0x000100L, IDS_POSTCARDS      },
	{ IDR_STICKERTYPE,			0x000080L, IDS_STICKERS       },
	{ IDR_TSHIRTTYPE,				0x000040L, IDS_TSHIRTS        },
	{ IDR_CRAFTTYPE,				0x000020L, IDS_CRAFTS	      },
	{ IDR_PHOTOPROJECTSTYPE,	0x000010L, IDS_PHOTOPROJECTS	},
	{ IDR_BLANKTYPE,				0x200000L, IDS_POSTERS			}
};


/////////////////////////////////////////////////////////////////////////////
// CPrintMasterConfiguration class Create function
BOOL CPrintMasterConfiguration::Create(int nConfiguration /*=Silver*/, IPathManager* pPathManager /*=NULL*/)
{
	// Assign the path manager.
	
   // Get rid of any old configuration.
   Destroy();

   // For now...
   TRY
   {
      switch (nConfiguration)
      {
         case Demo:
         {
            m_pConfiguration = new CDemoConfiguration;
            break;
         }
         case Classic:
         {
            m_pConfiguration = new CClassicConfiguration;
            break;
         }
         case BonusPack:
         {
            m_pConfiguration = new CBonusPackConfiguration;
            break;
         }
         case Silver:
         {
            m_pConfiguration = new CSilverConfiguration;
            break;
         }
         case Gold:
         {
            m_pConfiguration = new CGoldConfiguration;
            break;
         }
         case AddOn:
         {
            m_pConfiguration = new CConfigurationAddOn;
            break;
         }
         case Platinum:
         {
            m_pConfiguration = new CPlatinumConfiguration;
            break;
         }
         case NGCards:
         {
            m_pConfiguration = new CNGCardsConfiguration;
            break;
         }
         case Premier:
         {
            m_pConfiguration = new CPremierConfiguration;
            break;
         }
         case OfficeSuite:
         {
            m_pConfiguration = new COfficeSuiteConfiguration;
            break;
         }
// Localized version configurations

// UK English
         case ClassicENG:
         {
            m_pConfiguration = new CClassicConfigurationENG;
            break;
         }
         case SilverENG:
         {
            m_pConfiguration = new CSilverConfigurationENG;
            break;
         }
         case GoldENG:
         {
            m_pConfiguration = new CGoldConfigurationENG;
            break;
         }
         case PlatinumENG:
         {
            m_pConfiguration = new CPlatinumConfigurationENG;
            break;
         }
// German
         case ClassicDEU:
         {
            m_pConfiguration = new CClassicConfigurationDEU;
            break;
         }
         case SilverDEU:
         {
            m_pConfiguration = new CSilverConfigurationDEU;
            break;
         }
         case GoldDEU:
         {
            m_pConfiguration = new CGoldConfigurationDEU;
            break;
         }
         case PlatinumDEU:
         {
            m_pConfiguration = new CPlatinumConfigurationDEU;
            break;
         }

// French
         case ClassicFRA:
         {
            m_pConfiguration = new CClassicConfigurationFRA;
            break;
         }
         case SilverFRA:
         {
            m_pConfiguration = new CSilverConfigurationFRA;
            break;
         }
         case GoldFRA:
         {
            m_pConfiguration = new CGoldConfigurationFRA;
            break;
         }
         case PlatinumFRA:
         {
            m_pConfiguration = new CPlatinumConfigurationFRA;
            break;
         }

         case PrintStandard:
         {
            m_pConfiguration = new CPrintStandardConfiguration;
            break;
         }
         case PrintPremium:
         {
            m_pConfiguration = new CPrintPremiumConfiguration;
            break;
         }
         case CraftStandard:
         {
            m_pConfiguration = new CCraftStandardConfiguration;
            break;
         }
         case CraftDeluxe:
         {
            m_pConfiguration = new CCraftDeluxeConfiguration;
            break;
         }
         case CreataParty:
         {
            m_pConfiguration = new CCreataPartyConfiguration;
            break;
         }
         case CreataCard:
         {
            m_pConfiguration = new CCreataCardConfiguration;
            break;
         }
         case SpiritStandard:
         {
            m_pConfiguration = new CSpiritStandardConfiguration;
            break;
         }

			case ArtAndMoreOnly1:
			{
				m_pConfiguration = new CArtAndMoreOnly1;
				break;
			}

         default:
         {
            m_pConfiguration = NULL;
            break;
         }
      }
   }
   END_TRY

   if (m_pConfiguration != NULL)
   {
      if (m_pConfiguration->Init(pPathManager))
      {
      /*
      // Fixup the app name.
      */

         CWinApp* pApp = AfxGetApp();
         CString csAppName;

         m_pConfiguration->LoadString(AFX_IDS_APP_TITLE, csAppName);

#ifdef _DEBUG
         BOOL fEnabled = AfxEnableMemoryTracking(FALSE);
#endif
         free((void*)pApp->m_pszAppName);
         pApp->m_pszAppName = _strdup(csAppName);
#ifdef _DEBUG
         AfxEnableMemoryTracking(fEnabled);
#endif
         return TRUE;
      }
      else
      {
         Destroy();
      }
   }
   return (m_pConfiguration != NULL);
}
 
void CPrintMasterConfiguration::Destroy(void)
{
   delete m_pConfiguration;
   m_pConfiguration = NULL;
}

/*
// Get the signature file for id'ing the CD.
// This is used for the "run" CD in those cases where the "install" and
// "run" CDs are different.
*/

LPCSTR CPrintMasterConfiguration::GetSignatureFile(int nConfiguration)
{
   switch (nConfiguration)
   {
      case Demo:
      {
         return CDemoConfiguration::GetSignatureFile(nConfiguration);
      }
      case Classic:
      {
         return "PMCL80.ID";
      }
      case BonusPack:
      {
         return "PMBP70.ID";
      }
      case Silver:
      {
		return "PMSL80.ID";
      }
      case Gold:
      {
		return "PMGD80.ID";

      }
      case AddOn:
      {
         return CConfigurationAddOn::GetSignatureFile ();
      }
      case Platinum:
      {
         return "PMPL80.ID";
      }
      case NGCards:
      {
         return "PMNG70.ID";
      }
      case Premier:
      {
         return "PMPR80.ID";
      }
      case OfficeSuite:
      {
         return "PMOS70.ID";
      }
// localized versions
// Localized version configurations

// UK English
       case ClassicENG:
       {
         return "PMCL80E.ID";
       }
       case SilverENG:
       {
         return "PMSL80E.ID";
       }
       case GoldENG:
       {
         return "PMGD80E.ID";
       }
       case PlatinumENG:
       {
         return "PMPL80E.ID";
       }
// German
       case ClassicDEU:
       {
         return "PMCL80D.ID";
       }
       case SilverDEU:
       {
         return "PMPSL80D.ID";
       }
       case GoldDEU:
       {
         return "PMGD80D.ID";
       }
       case PlatinumDEU:
       {
         return "PMPL80D.ID";
       }

// French
       case ClassicFRA:
       {
         return "PMCL80F.ID";
       }
       case SilverFRA:
       {
         return "PMSL80F.ID";
       }
       case GoldFRA:
       {
         return "PMGD80F.ID";
       }
       case PlatinumFRA:
       {
         return "PMPL80F.ID";
       }

      case CraftStandard:
      {
         return "AGCRST10.ID";   // Craft Standard
      }
      case CraftDeluxe:
      {
         return "AGCRDX10.ID";   // Craft Deluxe
      }
      case CreataParty:
      {
         return "AGCPST10.ID";   // Creata Party Standard
      }
      case CreataCard:
      {
         return "AGCCST10.ID";   // Creata Party Standard
      }
      case PrintStandard:
      {
         return "AGPRST10.ID";   // Print Standard 
      }
      case PrintPremium:
      {
         return "AGPRPR10.ID";   // Print Premium 
      }                                           
      case SpiritStandard:
      {
         return "AGSPST10.ID";   // Spiritual Standard
      }

      case ArtAndMoreOnly1:
      {
         return "AGSTORE1.ID";
      }

      default:
      {
         return NULL;
      }
   }
}

/*
// Get the signature used for "install-only" CDs (where the "run" CD
// is something else).
*/

LPCSTR CPrintMasterConfiguration::GetInstallSignatureFile(int nConfiguration)
{
	static CString csReturnFile;
	LPCSTR pszFile = GetSignatureFile(nConfiguration);
	if (pszFile != NULL)
	{
		CString csDrive;
		CString csPath;
		CString csFile;
		CString csExtension;

		Util::SplitPath(pszFile, &csDrive, &csPath, &csFile, &csExtension);
		csFile += "I";

		Util::ConstructPath(csReturnFile,
								  csDrive, csPath, csFile, csExtension);
		pszFile = csReturnFile;
	}
	
	return pszFile;
}

/*
// Get the volume label for the CD (not effected by instant upgrades).
*/

LPCSTR CPrintMasterConfiguration::GetVolumeLabel(int nConfiguration, int nBuildId)
{
   LPCSTR pszFormat = "";

   switch (nConfiguration)
   {
      case Demo:
      {
         pszFormat = CDemoConfiguration::GetLabelFormat(nConfiguration);
         break;
      }
      case Classic:
      {
         pszFormat = "PM80CL%d";
         break;
      }
      case BonusPack:
      {
         pszFormat = "PM70BP%d";
         break;
      }
      case Silver:
      {
         pszFormat = "PM80SL%d";
         break;
      }
      case Gold:
      {
         pszFormat = "PM80GD%d";
         break;
      }
      case Premier:
      {
         pszFormat = "PM80PR%d";
         break;
      }
      case OfficeSuite:
      {
         pszFormat = "PM70OS%d";
         break;
      }
      case Platinum:
      {
         pszFormat = "PM70PL%d";
         break;
      }
      case NGCards:
      {
         pszFormat = "PM70NG%d";
         break;
      }
      case AddOn:
      {
         pszFormat = CConfigurationAddOn::GetVolumeLabel ();
         break;
      }
// localized versions
// Localized version configurations

// UK English
      case ClassicENG:
      {
         pszFormat = "PM80CLE%d";
         break;
      }
      case SilverENG:
      {
         pszFormat = "PM80SLE%d";
         break;
      }
      case GoldENG:
      {
         pszFormat = "PM80GDE%d";
         break;
      }
      case PlatinumENG:
      {
         pszFormat = "PM80PLE%d";
         break;
      }
// German
      case ClassicDEU:
      {
         pszFormat = "PM80CLD%d";
         break;
      }
      case SilverDEU:
      {
         pszFormat = "PM80SLD%d";
         break;
      }
      case GoldDEU:
      {
         pszFormat = "PM80GDD%d";
         break;
      }
      case PlatinumDEU:
      {
         pszFormat = "PM80PLD%d";
         break;
      }

// French
      case ClassicFRA:
      {
         pszFormat = "PM80CLF%d";
         break;
      }
      case SilverFRA:
      {
         pszFormat = "PM80SLF%d";
         break;
      }
      case GoldFRA:
      {
         pszFormat = "PM80GDF%d";
         break;
      }
      case PlatinumFRA:
      {
         pszFormat = "PM80PLF%d";
         break;
      }

      case PrintStandard:
         pszFormat = "AGPRST01%d";
         break;
      case PrintPremium:
         pszFormat = "AGPRPR01%d";
         break;
      case CraftStandard:
         pszFormat = "AGCRST01%d";
         break;
      case CraftDeluxe:
         pszFormat = "AGCRDX01%d";
         break;
      case CreataParty:
         pszFormat = "AGCPST01%d";
         break;
      case CreataCard:
         pszFormat = "AGCCST01%d";
         break;
      case SpiritStandard:
         pszFormat = "AGSPST01%d";
         break;
      case ArtAndMoreOnly1:
         pszFormat = "AGAMS01%d";
         break;

      default:
      {
         pszFormat = "";
         break;
      }
   }

   static char szVolumeLabel[32];
   sprintf(szVolumeLabel, pszFormat, nBuildId);

   return szVolumeLabel;
}


/////////////////////////////////////////////////////////////////////////////
// CPrintMasterConfiguration class

CPrintMasterConfiguration::CPrintMasterConfiguration()
{
   m_fCheckedOEMURL = FALSE;
	m_dwAddedProjectTypeMask = 0;
	m_pPathManager = NULL;
}

CPrintMasterConfiguration::~CPrintMasterConfiguration()
{
	if (m_pPathManager != NULL)
	{
		m_pPathManager->Release();
	}
}

BOOL CPrintMasterConfiguration::Init(IPathManager* pPathManager)
{
	if ((m_pPathManager = pPathManager) != NULL)
	{
		m_pPathManager->AddRef();
	}

   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsPersonalDelivery(BOOL fComplain /*=FALSE*/)
{
   return FALSE;
}

BOOL CPrintMasterConfiguration::SupportsProjectType(int nProjectType, BOOL fComplain /*=TRUE*/)
{
   // The allowed projects are defined by the value returned
   // by the SupportedProjects method.
   //
   // The secret formula is:
   //
   //    Cards            = 0x00800000
   //    Banners          = 0x00400000
   //    Signs            = 0x00200000
   //    Calendars        = 0x00100000
   //    Certificates     = 0x00080000
   //    Labels           = 0x00040000
   //    Business Cards   = 0x00020000
   //    Letterhead       = 0x00010000
   //    Envelopes        = 0x00008000
   //    Fax Sheets       = 0x00004000
   //    Note Cards       = 0x00002000
   //    Newsletters      = 0x00001000
   //    Brochures        = 0x00000800
   //    Half-fold Cards  = 0x00000400
   //    Web Publications = 0x00000200
	//		Post Cards       = 0x00000100
	//		Stickers         = 0x00000080
	//		T-Shirts         = 0x00000040
	//		Crafts	        = 0x00000020
   //
   // Leaving room for 6 other projects.
   //
   //    P = sum of the numbers for the enabled projects
   //    C = checksum = (floor(P/17)*3+10) mod 256
   //    N = P*256+C
   //
   // Some known values:
   //
   //    All Possible Projects
   //       FFFFFF37 (4294967095)
   //    Cards Only
   //       8000009F (2147483807)
   //    Cards, Banners, Signs, Calendars, Certificates
   //       F80000CC (4160749772)
   //    Letterhead, Envelopes, and Business Cards
   //       03800026 (58720294)
   //    Cards, Banners, Signed, Calendars, Certificates, Labels,
   //    Business Cards, Letterhead, Envelopes, Fax Sheets, Note Cards
   //       FFE00091 (4292870289)

   DWORD dwSupportedProjects = SupportedProjects();

#ifdef _DEBUG
	DWORD n = 0x00FFFFDF;
	DWORD c = (((n/17)*3)+10) & 0x000000FF;
	DWORD m = (n << 8) | c;
#endif

   // Validate the value.
   DWORD dwP = dwSupportedProjects >> 8;
   DWORD dwC = dwSupportedProjects & 0x000000FFL;

   if ((((dwP/17)*3+10) & 0x000000FFL) == dwC)
   {
      // Check if the project is included.
		DWORD dwMask = ProjectSupportedMask(nProjectType);
      if ((dwP & dwMask) != 0 || (m_dwAddedProjectTypeMask & dwMask) != 0)
      {
         // Project is supported!
         TRACE("Project %d is supported\n", nProjectType);
         return TRUE;
      }
   }

   // The project type is not supported, give the user a
   // chance to upgrade.
   TRACE("Project %d is NOT supported\n", nProjectType);
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_CREATE)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsProjectType(nProjectType, FALSE);
      }
   }
   return FALSE;
}

CPrintMasterConfiguration::ProductLine CPrintMasterConfiguration::Product(void) const
{
   if(ProductLevel() < PrintStandard)
      return plPrintMaster;
   else
      return plAmericanGreetings;
}

BOOL CPrintMasterConfiguration::AddProjectType(DWORD dwProjectTypeMask)
{
   // Validate the value.
   DWORD dwP = dwProjectTypeMask >> 8;
   DWORD dwC = dwProjectTypeMask & 0x000000FFL;
	if ((((dwP/17)*3+10) & 0x000000FFL) != dwC)
		return FALSE;

	// OR the supplied bits into the running mask now that the checksum is removed
	m_dwAddedProjectTypeMask |= dwP;
	return TRUE;
}
	
BOOL CPrintMasterConfiguration::SupportsSaveProject(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsCardBack(BOOL fComplain /*=TRUE*/, CPmwDoc* pDoc /*=NULL*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsAddressBook(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsEventReminder(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsPhotoEditor(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsCartoonMaker(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsUnknownProjects(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsNetworkInterface(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsPictureCatalog(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsVirtualFile(int nVersion, BOOL fComplain /*=TRUE*/)
{
   return nVersion <= ProductLevel();
}

BOOL CPrintMasterConfiguration::SupportsUpgrading(void)
{
   return FALSE;
}

BOOL CPrintMasterConfiguration::SupportsSpellCheck(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsSerifDrawPlus(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsBorderPlus(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsInstructionPage()
{
   return FALSE;
}

BOOL CPrintMasterConfiguration::SupportsOnlineGallery()
{
   return TRUE;
}

BOOL CPrintMasterConfiguration::SupportsVerticalBanners()
{
   return TRUE;
}


BOOL CPrintMasterConfiguration::SupportsLZW(BOOL fComplain /*=TRUE*/)
{
#ifdef INSTALL
   return TRUE;
#else
	ASSERT(m_pPathManager != NULL);
	static int nSupportsLZW = -1;		// Unknown until read.
	if (nSupportsLZW == -1 && m_pPathManager != NULL)
	{
		// Query the MSREG.INI file on the home CD to determine whether this
		// is supported or not.
		// Get it from the MSREG.INI file.
		CString csMsregIni = m_pPathManager->ExpandPath("[[H]]\\MSREG\\MSREG.INI");
		CIniFile MsregIniFile(csMsregIni);
		// Default is not for Classic or Demo (or level 0 which is nothing).
		nSupportsLZW = MsregIniFile.GetInteger(SECTION_Configuration, "SupportsLZW", 0x7FFFFFF8);
	}
	return nSupportsLZW & (1<<ProductLevel());
#endif
}

BOOL CPrintMasterConfiguration::SupportsCardServer(BOOL fComplain /*=TRUE*/)
{
	// We need LZW support to run this.
	return SupportsLZW(fComplain);
}

BOOL CPrintMasterConfiguration::SupportsSentimentGallery(BOOL fComplain /*=TRUE*/)
{
	return TRUE;
}

BOOL CPrintMasterConfiguration::ExcludeFilters(LPCSTR pszFilters) const
{
	CStringArray csaFilters;
	Util::FillArrayFromDelimitedString(pszFilters, ",", csaFilters);
	return ExcludeFilters(csaFilters);
}

BOOL CPrintMasterConfiguration::ExcludeFilters(const CStringArray& csaFilters) const
{
	BOOL fRet = FALSE;

	CStringArray csaMyFilters;
	GetFilterList(csaMyFilters);

	for(int x = 0; x < csaMyFilters.GetSize(); x++)
	{
		for(int n = 0; n < csaFilters.GetSize(); n++)
		{
			if(csaFilters.GetAt(n).CompareNoCase(csaMyFilters.GetAt(x)) == 0)
			{
				fRet = TRUE;
				break;
			}
		}
	}

	return fRet;
}

BOOL CPrintMasterConfiguration::GetFilterList(CStringArray& csaFilters) const
{
	LPCSTR pszFilters = GetFilters();

	if (pszFilters == NULL)
	{
		return FALSE;
	}

	Util::FillArrayFromDelimitedString(pszFilters, ", ", csaFilters);
	return TRUE;
}

LPCSTR CPrintMasterConfiguration::GetFilters(void) const
{
	return NULL;
}

BOOL CPrintMasterConfiguration::RemoveYearYouWereBorn(void)
{ // should we remove the "Year You Were Born" feature completely (menu item, button etc.)
	return FALSE;
}

BOOL CPrintMasterConfiguration::RemovePhotoEditor(void)
{  // should we remove the "Photo Editor" feature completely (menu item, button etc.)
   return FALSE;
}

BOOL CPrintMasterConfiguration::RemoveCartoonMaker(void)
{   // should we remove the "Cartoon maker" feature completely (menu item, button etc.)
   return FALSE;
}

BOOL CPrintMasterConfiguration::RemoveRegistration(void)
{
	return FALSE;
}

BOOL CPrintMasterConfiguration::RemoveSerifDraw(void)
{
	return FALSE;
}

BOOL CPrintMasterConfiguration::RemoveDividers(void)
{
	return FALSE;
}

#ifdef INSTALL
BOOL CPrintMasterConfiguration::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return FALSE;
}
#endif



#ifdef INSTALL
BOOL CPrintMasterConfiguration::IsNetworkVersion(void)
{
   return FALSE;
}
#endif

BOOL CPrintMasterConfiguration::IsRegistrationFace(int nFlags)
{
   // 1 == normal (DX, PS, BP, CL, and small demo)
   // 4 == full demo
   return (nFlags & 1) != 0;
}

LPCSTR CPrintMasterConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'G':                  // Group
      case 'N':                  // Name (short name)
      case 'T':                  // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'R':					//EventReminder             
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_EVENT_REMINDER);
      }
      case 'd':
      {
#ifdef LOCALIZE
		 static char cbBuffer[_MAX_PATH];
		 long lSize = sizeof(cbBuffer);

		 if (::RegQueryValue(HKEY_LOCAL_MACHINE,
						  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir",
						  cbBuffer,
						  &lSize) == ERROR_SUCCESS)
		 {
		 	strcat(cbBuffer, "\\Mindscape\\PrintMaster");
		 	return cbBuffer;
		 }
		 else
#endif
            return "c:\\Program Files\\Mindscape\\PrintMaster";
      }
      case 'P':                  // List of supported projects
      {
         return SupportedProjectsList();
      }
		case 'C':
		{
			// CD name.
			static char Buffer[_MAX_PATH] = "";
			if (Buffer[0] == '\0' && m_pPathManager != NULL)
			{
				// Get it from the MSREG.INI file.
				CString csMsregIni = m_pPathManager->ExpandPath("MSREG.INI");
				CIniFile MsregIniFile(csMsregIni);
				CString csName = MsregIniFile.GetString(SECTION_Configuration, "CDName");
				strcpy(Buffer, csName);
			}
			return Buffer;
		}
      default:
      {
         return "";
      }
   }
}

LPCSTR CPrintMasterConfiguration::SupportedProjectsList(void)
{
   static char FAR szSupportedProjects[512] = "";

   *szSupportedProjects = '\0';
   int nLastProject = -1;
   BOOL fFirstProject = TRUE;
   for (int i = 0; i < sizeof(Projects)/sizeof(Projects[0]); i++)
   {
      if (SupportsProjectType(Projects[i].m_nType, FALSE))
      {
         if (nLastProject != -1)
         {
            if (!fFirstProject)
            {
               strcat(szSupportedProjects, ", ");
            }
            else
            {
               fFirstProject = FALSE;
            }
            strcat(szSupportedProjects,
                   GET_PMWAPP()->GetResourceStringPointer(Projects[nLastProject].m_uIDName));
         }
         nLastProject = i;
      }
   }
   if (nLastProject != -1)
   {
      if (!fFirstProject)
      {
         strcat(szSupportedProjects, GET_PMWAPP()->GetResourceStringPointer(IDS_S_AND_S));
      }
      strcat(szSupportedProjects, GET_PMWAPP()->GetResourceStringPointer(Projects[nLastProject].m_uIDName));
   }
      
   return szSupportedProjects;
}

DWORD CPrintMasterConfiguration::SupportedProjects(void)
{
   // Magic number for all projects -- even undefined ones!
	//return 0xFFFFFF37L;
	// Magic number for all projects but crafts
	return   0xFFFFDF31L;
}

DWORD CPrintMasterConfiguration::ProjectSupportedMask(int nProjectType)
{
   // Lookup project type.
   for (int i = 0; i < sizeof(Projects)/sizeof(Projects[0]); i++)
   {
      if (nProjectType == Projects[i].m_nType)
      {
         return Projects[i].m_dwMask;
      }
   }

   // Unknown project type!
   return 0;
}

/*
// Replace any replaceable text.
*/

BOOL CPrintMasterConfiguration::ReplaceText(CString& csText)
{
   CString csNew;
   BOOL fReplaced = FALSE;

   TRY
   {
      int nIndex;
      while ((nIndex = csText.Find("@@")) != -1)
      {
         csNew = csText.Left(nIndex);
         csNew += ReplacementText(csText[nIndex+2]);
         csNew += csText.Mid(nIndex+3);
         csText = csNew;
         fReplaced = TRUE;
      }
   }
   END_TRY
   return fReplaced;
}

/*
// Replace text in a window.
*/

void CPrintMasterConfiguration::ReplaceWindowText(CWnd* pWnd)
{
   CString csText;

   TRY
   {
      pWnd->GetWindowText(csText);
      if (ReplaceText(csText))
      {
         pWnd->SetWindowText(csText);
      }

   }
   END_TRY
}

/*
// Replace necessary text in the dialog strings.
*/

void CPrintMasterConfiguration::ReplaceDialogText(CWnd* pDlg)
{
/*
// Replace text in the dialog title.
*/

   ReplaceWindowText(pDlg);

/*
// Replace text in the dialog children.
*/

   CWnd* pChild = pDlg->GetWindow(GW_CHILD);
   while (pChild != NULL)
   {
      ReplaceWindowText(pChild);
      pChild = pChild->GetWindow(GW_HWNDNEXT);
   }
}

int CPrintMasterConfiguration::MessageBox(UINT uText, UINT uCaption, UINT nType /*=MB_OK*/)
{
   int nResult = IDABORT;
   CString csText;
   CString csCaption;
   TRY
   {
      LoadString(uText, csText);

      if (uCaption == 0)
      {
         csCaption = ReplacementText('T');
      }
      else
      {
         LoadString(uCaption, csCaption);
      }
      AfxGetApp()->EnableModeless(FALSE);
#ifdef WIN32
      nResult = ::MessageBox(CWnd::GetSafeOwner(NULL)->GetSafeHwnd(), csText, csCaption, nType);
#else
      nResult = ::MessageBox(_AfxGetSafeOwner(NULL), csText, csCaption, nType);
#endif
      AfxGetApp()->EnableModeless(TRUE);
   }
   END_TRY

   return nResult;
}

void CPrintMasterConfiguration::DocumentFilter(CString& csFilters)
{
   CString csFilters2;

   TRY
   {
      LoadConfigurationString(IDS_DOCUMENT_FILTERS_PART_1, csFilters);
      LoadConfigurationString(IDS_DOCUMENT_FILTERS_PART_2, csFilters2);
      csFilters += csFilters2;
      LoadConfigurationString(IDS_DOCUMENT_FILTERS_PART_3, csFilters2);
      csFilters += csFilters2;
      LoadConfigurationString(IDS_DOCUMENT_FILTERS_PART_4, csFilters2);
      csFilters += csFilters2;
   }
   END_TRY
}

void CPrintMasterConfiguration::PictureFilter(CString& csFilters)
{
	CString csFilters2;

	// Get our filter strings.

	TRY
	{
		LoadConfigurationString(IDS_PICTURE_FILTERS_PART_1, csFilters);
		LoadConfigurationString(IDS_PICTURE_FILTERS_PART_2, csFilters2);
		csFilters += csFilters2;
	}
   END_TRY
}

static void ResourceUnavailable(int nID, CString& csString)
{
   csString.Empty();

   TRY
   {
      char cbBuffer[40];

      wsprintf(cbBuffer, "(Can't load resource string: %u)", nID);
      csString = cbBuffer;
   }
   END_TRY
}

CString& CPrintMasterConfiguration::SafeLoadString(UINT nID, CString& csString)
{
   if (GetConfiguration() == NULL)
   {
      TRY
      {
         Util::LoadString(nID, csString);
      }
      CATCH_ALL(e)
      {
         ResourceUnavailable(nID, csString);
      }
      END_CATCH_ALL

      return csString;
   }
   else
   {
      return GetConfiguration()->LoadString(nID, csString);
   }
}

CString& CPrintMasterConfiguration::LoadString(UINT nID, CString& csString)
{
   TRY
   {
      Util::LoadString(nID, csString);

      ReplaceText(csString);
   }
   CATCH_ALL(e)
   {
      ResourceUnavailable(nID, csString);
   }
   END_CATCH_ALL

   return csString;
}

/*
// Return a char string that is the root path for home CD files.
// This string is assumed to be modifiable.
// This replaces "A:\\" in the path manager.
*/

LPSTR CPrintMasterConfiguration::GetHomeCDRoot(void)
{
#ifdef INSTALL
	static char pRoot[] = "@:\\";			// '@' gets drive letter
	return pRoot;
#else
	static char cbRoot[40] = "";

	ASSERT(m_pPathManager != NULL);
	if (cbRoot[0] == '\0' && m_pPathManager != NULL)
	{
		CString csMsregIni = m_pPathManager->ExpandPath("MSREG.INI");
		CIniFile MsregIniFile(csMsregIni);
		// '@' gets drive letter
		CString csName = MsregIniFile.GetString(SECTION_Configuration, "CDRoot", "@:\\");
		strcpy(cbRoot, csName);
	}
   return cbRoot;
#endif
}

BOOL CPrintMasterConfiguration::UnsupportedDialog(LPCSTR pOperation /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
   ASSERT(FALSE);
   return FALSE;
}

LPCSTR CPrintMasterConfiguration::GetHubDLLName (void)
{
   if (s_HubDLLName.IsEmpty ())
   {
#ifndef INSTALL
      if (IsAddOn () == TRUE)
      {
         CString  INIFileName = GetGlobalPathManager()->ExpandPath (AddOnIniFileName);
         CIniFile INIFile (INIFileName);
// JRO this allows the same ADDON.INI to be used for 16 and 32 bit
#ifdef WIN32
         s_HubDLLName = INIFile.GetString ("HUB", "HubResource32");
#else
         s_HubDLLName = INIFile.GetString ("HUB", "HubResource16");
#endif
      }
      else
#endif
         s_HubDLLName = HUBDllNameNormal;
   }

   if (s_HubDLLName.IsEmpty ())
      return NULL;

   return s_HubDLLName;
}

LPCSTR CPrintMasterConfiguration::GetHTMLName (void)
{
   return NULL;
}

BOOL
CPrintMasterConfiguration::IsHelpFile (void)
{
   return TRUE;
}
BOOL 
CPrintMasterConfiguration::SupportsRegistrationBonus (void)
{
   return TRUE;
}

LPCSTR 
CPrintMasterConfiguration::GetOEMURL (void)
{
#ifdef INSTALL
   return NULL;
#else
   if (m_fCheckedOEMURL == FALSE)  // Have we checked this yet?
   {
      m_fCheckedOEMURL = TRUE;
      
      TRY
      {
         CIniFile INIFile;
         GET_PMWAPP()->InitPrintMasterIniFile (&INIFile, "MSREG.INI");
         m_csOEMURL = INIFile.GetString ("OEM", "URL");
      }
		END_TRY
   }

   if (m_csOEMURL.IsEmpty ())
      return NULL;

   return (m_csOEMURL);
#endif
}

CString CPrintMasterConfiguration::GetProductURLFileName()
   {
		return (m_pPathManager == NULL)
				? "PMW.URL"
				: m_pPathManager->ExpandPath("PMW.URL");
   }

LPCSTR CPrintMasterConfiguration::GetProductURL(void) 
   {
      static CString csUrl;
      GetPrivateProfileString(
         "InternetShortcut",
         "URL",
         "http://www.printmaster.com/",
         csUrl.GetBuffer(256),
         256-1,
         GetProductURLFileName());
      csUrl.ReleaseBuffer();
      return csUrl;
   }

LPCSTR CPrintMasterConfiguration::GetOnlineGreetingsFTPSite(void)
{
	return "greetings.printmaster.com";
}

CPoint CPrintMasterConfiguration::GetDefaultBMPResolution(void)
{
	return CPoint(100, 100);
}

CPoint CPrintMasterConfiguration::GetDefaultGIFResolution(void)
{
	return CPoint(200, 200);
}

CPoint CPrintMasterConfiguration::GetDefaultJPGResolution(void)
{
	return CPoint(300, 300);
}

CPoint CPrintMasterConfiguration::GetDefaultPCXResolution(void)
{
	return CPoint(200, 200);
}

CPoint CPrintMasterConfiguration::GetDefaultPNGResolution(void)
{
	return CPoint(200, 200);
}

// ADDON STUFF
CString CPrintMasterConfiguration::s_HubDLLName;
CString CPrintMasterConfiguration::s_HTMLName; 
CString CPrintMasterConfiguration::s_AddOnININame;

int     CPrintMasterConfiguration::m_nIsAddon = CFG_3FLAG_UNKNOWN;

LPCSTR CPrintMasterConfiguration::GetAddOnININame (void)
{
   if (s_AddOnININame.IsEmpty ())
   {
      CString Temp;
      GetModuleFileName (AfxGetInstanceHandle(), Temp.GetBuffer (_MAX_PATH + 1), _MAX_PATH);
      Temp.ReleaseBuffer ();

      Util::SplitPath (Temp, &s_AddOnININame, NULL);
      Util::AppendBackslashToPath (s_AddOnININame);

      s_AddOnININame += "ADDON.INI";
   }
   
   return s_AddOnININame;
}

BOOL
CPrintMasterConfiguration::IsAddOn (void)
{
   if (m_nIsAddon == CFG_3FLAG_UNKNOWN)
   {
      CString  AddOnINIFileName = CPrintMasterConfiguration::GetAddOnININame ();
      if (Util::FileExists (AddOnINIFileName) == TRUE)
         m_nIsAddon = CFG_3FLAG_YES;
      else
         m_nIsAddon = CFG_3FLAG_NO;   
   }

   return (m_nIsAddon == CFG_3FLAG_YES ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CDemoConfiguration class

CIniFile CDemoConfiguration::m_DemoIniFile;
CString CDemoConfiguration::m_csSignatureFile;
CString CDemoConfiguration::m_csLabelFormat;

CDemoConfiguration::CDemoConfiguration()
{
   m_nVirtualLevel = NO_DEMO_INTEGER;
   m_nRegistrationFace = NO_DEMO_INTEGER;
   m_nInstantUpgrade = NO_DEMO_INTEGER;
}

CDemoConfiguration::~CDemoConfiguration()
{
}

LPCSTR CDemoConfiguration::GetSignatureFile(int /*nConfiguration*/)
{
   return LoadDemoIniString(
               m_csSignatureFile,
               SECTION_Configuration,
               "SignatureFile",
               "PMDE70.ID");
}

LPCSTR CDemoConfiguration::GetLabelFormat(int /*nConfiguration*/)
{
   return LoadDemoIniString(
               m_csLabelFormat,
               SECTION_Configuration,
               "LabelFormat",
               "PM70DE%d");
}

CIniFile& CDemoConfiguration::GetDemoIniFile(void)
{
   if (*(m_DemoIniFile.Name()) == '\0')      // Is empty?
   {
      CString csName;
      GetDemoIniFileName(csName);   // Get the file name from the app.
      m_DemoIniFile.Name(csName);
   }
   return m_DemoIniFile;
}

LPCSTR CDemoConfiguration::LoadDemoIniString(CString& csString, LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault /*=NULL*/)
{
   if (csString.IsEmpty())
   {
      csString = GetDemoIniFile().GetString(pszSection, pszEntry, pszDefault);
   }
   return csString;
}

int CDemoConfiguration::LoadDemoIniInteger(int& nValue, LPCSTR pszSection, LPCSTR pszEntry, int nDefault)
{
   if (nValue == NO_DEMO_INTEGER)
   {
      nValue = GetDemoIniFile().GetInteger(pszSection, pszEntry, nDefault);
   }
   return nValue;
}

DWORD CDemoConfiguration::LoadDemoIniDword(DWORD& dwValue, LPCSTR pszSection, LPCSTR pszEntry, DWORD dwDefault)
{
   // dwValue MUST be initialize to something. Its value is ONLY changed
   // if its current value if NO_DEMO_DWORD (defined in PMWCFG.H)
   if (dwValue == NO_DEMO_DWORD)
   {
      dwValue = GetDemoIniFile().GetDword(pszSection, pszEntry, dwDefault);
   }
   
   return dwValue;
}

int CDemoConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::Demo;
}

WORD CDemoConfiguration::ProductFlags(void) const
{
   ASSERT(FALSE);    // What flags?
   return 0;
}

BOOL CDemoConfiguration::Init(IPathManager* pPathManager)
{
   return CPrintMasterConfiguration::Init(pPathManager);
}

BOOL CDemoConfiguration::SupportsSaveProject(BOOL fComplain /*=TRUE*/)
{
   return TRUE;
#if 0
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOSAVE)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsSaveProject(FALSE);
      }
   }
   return FALSE;
#endif
}

#if 0
BOOL CDemoConfiguration::SupportsCardBack(BOOL fComplain /*=TRUE*/, CPmwDoc* pDoc /*=NULL*/)
{
   /*
   // This function had PMW-specific code which INSTALL could not handle.
   // There is a version of this function in PMW.CPP and one in INSTALL.CPP
   */
}
#endif

BOOL CDemoConfiguration::SupportsAddressBook(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOADDR)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsAddressBook(FALSE);
      }
   }
   return FALSE;
}

BOOL CDemoConfiguration::SupportsEventReminder(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOREMINDER)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsEventReminder(FALSE);
      }
   }
   return FALSE;
}

BOOL CDemoConfiguration::SupportsPhotoEditor(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOPHOTOEDIT)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsPhotoEditor(FALSE);
      }
   }
   return FALSE;
}

BOOL CDemoConfiguration::SupportsCartoonMaker(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOCARTOON)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsCartoonMaker(FALSE);
      }
   }
   return FALSE;
}

BOOL CDemoConfiguration::SupportsUnknownProjects(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CDemoConfiguration::SupportsNetworkInterface(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NONET)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsNetworkInterface(FALSE);
      }
   }
   return FALSE;
}

BOOL CDemoConfiguration::SupportsPictureCatalog(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOPICT)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsPictureCatalog(FALSE);
      }
   }
   return FALSE;
}

BOOL CDemoConfiguration::SupportsUpgrading(void)
{
   return TRUE;
}

LPCSTR CDemoConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'G':                     // Group
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_GROUP_PMG_TRIAL);
      }
      case 'N':                     // Name (short name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_NAME_PMG_TRIAL1);
      }
      case 'T':                     // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_NAME_PMG_TRIAL2);
      }
      case 'D':                     // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_NAME_PMG_TRIAL3);
      }
      case 'd':
      {
         return LoadDemoIniString(m_csLabelFormat, SECTION_Configuration, "InstallDirectory", "c:\\pmwtrial");
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}

DWORD CDemoConfiguration::SupportedProjects(void)
{
   DWORD dwProjects = NO_DEMO_DWORD;
   return LoadDemoIniDword(dwProjects, SECTION_Configuration, "Projects", 0);
}

/*
// Return a char string that is the root path for home CD files.
// This string is assumed to be modifiable.
// This replaces "A:\\" in the path manager.
*/

LPSTR CDemoConfiguration::GetHomeCDRoot(void)
{
   // '@' gets drive letter
   return (LPSTR)(LPCSTR)LoadDemoIniString(m_csHomeCDRoot, SECTION_Configuration, "HomeCDRoot", "@:\\pmwtrial\\");
}

#if 0
BOOL CDemoConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
   /*
   // This function had PMW-specific code which INSTALL could not handle.
   // There is a version of this function in PMW.CPP and one in INSTALL.CPP
   */
}
#endif

BOOL CDemoConfiguration::SupportsVirtualFile(int nVersion, BOOL fComplain /*=TRUE*/)
{
   return nVersion == LoadDemoIniInteger(m_nVirtualLevel, SECTION_Configuration, "VirtualLevel", ProductLevel());
}

BOOL CDemoConfiguration::IsRegistrationFace(int nFlags)
{
   // 1 == normal (DX, PS, BP, CL, and small demo)
   // 4 == full demo

   return (nFlags & LoadDemoIniInteger(m_nRegistrationFace, SECTION_Configuration, "RegistrationFace", 1)) != 0;
}

// Can we instantly upgrade?

BOOL CDemoConfiguration::CanInstantUpgrade(void)
{
#if 1
   return CPrintMasterConfiguration::CanInstantUpgrade();
#else
   return LoadDemoIniInteger(m_nInstantUpgrade, SECTION_Configuration, "InstantUpgrade", FALSE);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CGoldConfiguration class

CGoldConfiguration::CGoldConfiguration()
{
}

CGoldConfiguration::~CGoldConfiguration()
{
}

int CGoldConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::Gold;
}

WORD CGoldConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagGold
				| CCategoryDataItem::pflagSilver
				| CCategoryDataItem::pflagBonusPack
				| CCategoryDataItem::pflagClassic;
}

LPCSTR CGoldConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'T':               // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMGOLD);
      }
      case 'D':               // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMGOLDCD);
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}


/////////////////////////////////////////////////////////////////////////////
// CSilverConfiguration class

CSilverConfiguration::CSilverConfiguration()
{
	//sample code
	//a comma de-limited list of filters

	//SetFilter("AG Toons");
}

CSilverConfiguration::~CSilverConfiguration()
{
}

int CSilverConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::Silver;
}

WORD CSilverConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagSilver
				| CCategoryDataItem::pflagBonusPack
				| CCategoryDataItem::pflagClassic;
}

LPCSTR CSilverConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'T':               // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMSILVER);
      }
      case 'D':               // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMSILVERCD);
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}


/////////////////////////////////////////////////////////////////////////////
// CBonusPackConfiguration class

CBonusPackConfiguration::CBonusPackConfiguration()
{
}

CBonusPackConfiguration::~CBonusPackConfiguration()
{
}

int CBonusPackConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::BonusPack;
}

WORD CBonusPackConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagBonusPack
				| CCategoryDataItem::pflagClassic;
}

BOOL CBonusPackConfiguration::SupportsAddressBook(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOADDR)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsAddressBook(FALSE);
      }
   }
   return FALSE;
}

BOOL CBonusPackConfiguration::SupportsEventReminder(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOREMINDER)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsEventReminder(FALSE);
      }
   }
   return FALSE;
}

BOOL CBonusPackConfiguration::SupportsPhotoEditor(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOPHOTOEDIT)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsPhotoEditor(FALSE);
      }
   }
   return FALSE;
}

BOOL CBonusPackConfiguration::SupportsCartoonMaker(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOCARTOON)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsCartoonMaker(FALSE);
      }
   }
   return FALSE;
}

BOOL CBonusPackConfiguration::SupportsUnknownProjects(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CBonusPackConfiguration::SupportsUpgrading(void)
{
   return TRUE;
}

BOOL CBonusPackConfiguration::SupportsSentimentGallery(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_SENTIMENTS)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsSentimentGallery(FALSE);
      }
   }
   return FALSE;
}

LPCSTR CBonusPackConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'T':               // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMGBP);
      }
      case 'D':               // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMGBPCD);
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}

DWORD CBonusPackConfiguration::SupportedProjects(void)
{
   // Magic number projects supported in Bonus Pack.
   return 0xFFF40018L;
}

#if 0
BOOL CBonusPackConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
   /*
   // This function had PMW-specific code which INSTALL could not handle.
   // There is a version of this function in PMW.CPP and one in INSTALL.CPP
   */
}
#endif


/////////////////////////////////////////////////////////////////////////////
// CClassicConfiguration class

CClassicConfiguration::CClassicConfiguration()
{
}

CClassicConfiguration::~CClassicConfiguration()
{
}

int CClassicConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::Classic;
}

WORD CClassicConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagClassic;
}

BOOL CClassicConfiguration::SupportsUpgrading(void)
{
   return TRUE;
}

BOOL CClassicConfiguration::SupportsLZW(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_LZW)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsLZW(FALSE);
      }
   }
   return FALSE;
}

BOOL CClassicConfiguration::SupportsCardServer(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_CARDSERVER)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsCardServer(FALSE);
      }
   }
   return FALSE;
}

BOOL CClassicConfiguration::SupportsSerifDrawPlus(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_DRAWPLUS)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsSerifDrawPlus(FALSE);
      }
   }
   return FALSE;
}

BOOL CClassicConfiguration::SupportsBorderPlus(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_BORDERPLUS)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsBorderPlus(FALSE);
      }
   }
   return FALSE;
}

LPCSTR CClassicConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'T':               // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMGC);
      }
      case 'D':               // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMGCCD);
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}

DWORD CClassicConfiguration::SupportedProjects(void)
{
   // Magic number projects supported in Classic.
   return 0xFFE40045L;
}

void CClassicConfiguration::DocumentFilter(CString& csFilters)
{
   LoadConfigurationString(IDS_DOCUMENT_FILTERS_CLASSIC, csFilters);
}

void CClassicConfiguration::PictureFilter(CString& csFilters)
{
	CString csFilters2;

	// Get our filter strings.

	TRY
	{
		LoadConfigurationString(IDS_CLASSIC_PICTURE_FILTERS_PART_1, csFilters);
		LoadConfigurationString(IDS_CLASSIC_PICTURE_FILTERS_PART_2, csFilters2);
		csFilters += csFilters2;
	}
   END_TRY
}

/////////////////////////////////////////////////////////////////////////////
// CConfigurationAddOn class

CString CConfigurationAddOn::m_csSignatureFile;
CString CConfigurationAddOn::m_csVolumeLabel;
CString CConfigurationAddOn::m_csName;
CString CConfigurationAddOn::m_csExtraContentName;
BOOL    CConfigurationAddOn::m_fCheckedExtraContentName = FALSE;

CConfigurationAddOn::CConfigurationAddOn()
{
   m_fIsHelpFile = CFG_3FLAG_UNKNOWN;
}

CConfigurationAddOn::~CConfigurationAddOn()
{
   s_HubDLLName.Empty ();
   m_csSignatureFile.Empty ();
   m_csVolumeLabel.Empty ();
}

int CConfigurationAddOn::ProductLevel(void) const
{
   return CPrintMasterConfiguration::AddOn;
}

WORD CConfigurationAddOn::ProductFlags(void) const
{
   return 0;
}

BOOL CConfigurationAddOn::SupportsProjectType(int nProjectType, BOOL fComplain /*=TRUE*/)
{
   return INHERITED::SupportsProjectType(nProjectType, fComplain);
}

BOOL CConfigurationAddOn::SupportsSaveProject(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsCardBack(BOOL fComplain /*=TRUE*/, CPmwDoc* pDoc /*=NULL*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsAddressBook(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsEventReminder(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsPhotoEditor(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsCartoonMaker(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsUnknownProjects(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsNetworkInterface(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsPictureCatalog(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsVirtualFile(int nVersion, BOOL fComplain /*=TRUE*/)
{
   //return nVersion <= ProductLevel();
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsUpgrading(void)
{
   return FALSE;
}

BOOL CConfigurationAddOn::SupportsSpellCheck(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CConfigurationAddOn::IsRegistrationFace(int nFlags)
{
   return FALSE;
}

BOOL CConfigurationAddOn::CanInstantUpgrade(void)
{
   return FALSE;
}

LPCSTR CConfigurationAddOn::GetHTMLName (void) 
{
   if (s_HTMLName.IsEmpty ())
   {
      CString  INIFileName = GetAddOnININame ();
      CIniFile INIFile (INIFileName);
   
      s_HTMLName = INIFile.GetString ("EXTRACONTENT", "HTMLNAME");
   }
   
   if (s_HTMLName.IsEmpty ())
      return NULL;

   return s_HTMLName;   
}

LPCSTR CConfigurationAddOn::GetSignatureFile (void)
{
   if (m_csSignatureFile.IsEmpty ())
   {
      CString  INIFileName = GetAddOnININame ();
      CIniFile INIFile (INIFileName);
   
      m_csSignatureFile = INIFile.GetString ("IDENTIFICATION", "SignatureFile");
   }
   
   if (m_csSignatureFile.IsEmpty ())
      return NULL;

   return m_csSignatureFile;   
}

LPCSTR CConfigurationAddOn::GetVolumeLabel ()
{
   if (m_csVolumeLabel.IsEmpty ())
   {
      CString  INIFileName = GetAddOnININame ();
      CIniFile INIFile (INIFileName);
      m_csVolumeLabel = INIFile.GetString ("IDENTIFICATION", "VolumeLabel");
   }

   if (m_csVolumeLabel.IsEmpty ())
      //return NULL;
      return "Error";
   
   return m_csVolumeLabel;   
}

LPCSTR CConfigurationAddOn::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':                  // Name (short name)
      case 'T':                  // Title (long name)
      if (m_csName.IsEmpty ())
      {
         CString  INIFileName = GetAddOnININame ();
         CIniFile INIFile (INIFileName);
         m_csName = INIFile.GetString ("IDENTIFICATION", "Name");
      }
      
      if (m_csName.IsEmpty ())
         //return NULL;
         return "Error";

      return m_csName;

      default:
      return INHERITED::ReplacementText (cID);
   }
}

LPCSTR CConfigurationAddOn::GetExtraContentName (void)
{
   if ((m_csExtraContentName.IsEmpty ()) && (m_fCheckedExtraContentName == FALSE))
   {
      CString  INIFileName = GetAddOnININame ();
      CIniFile INIFile (INIFileName);
      m_csExtraContentName = INIFile.GetString ("EXTRACONTENT", "MENUNAME");
      m_fCheckedExtraContentName = TRUE;
   }
   
   if (m_csExtraContentName.IsEmpty ())
      return NULL;

   return m_csExtraContentName;
}

BOOL CConfigurationAddOn::IsHelpFile (void)
{
   if (m_fIsHelpFile == CFG_3FLAG_UNKNOWN)
   {
      m_fIsHelpFile = FALSE;
      TRY
      {
         CString  INIFileName = GetAddOnININame ();
         CIniFile INIFile (INIFileName);
         int i = INIFile.GetInteger ("EXTRACONTENT", "HELP", 0);
         m_fIsHelpFile = (i == 0 ? CFG_3FLAG_NO : CFG_3FLAG_YES);
      }
      CATCH_ALL(e)
      {
      }
      END_CATCH_ALL
   }

   return (m_fIsHelpFile == CFG_3FLAG_YES ? TRUE : FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CPlatinumConfiguration class

CPlatinumConfiguration::CPlatinumConfiguration()
{
}

CPlatinumConfiguration::~CPlatinumConfiguration()
{
}

int CPlatinumConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::Platinum;
}

WORD CPlatinumConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagPlatinum
				| CCategoryDataItem::pflagPremier
				| CCategoryDataItem::pflagGold
				| CCategoryDataItem::pflagSilver
				| CCategoryDataItem::pflagBonusPack
				| CCategoryDataItem::pflagClassic;
}

LPCSTR CPlatinumConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'G':               // Group
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'N':               // Name (short name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'T':               // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMP);
      }
      case 'D':               // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMPCD);
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CNGCardsConfiguration class

int CNGCardsConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::Silver;
}

LPCSTR CNGCardsConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'T':               // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'D':               // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMGNGCD);
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CPremierConfiguration class

CPremierConfiguration::CPremierConfiguration()
{
}

CPremierConfiguration::~CPremierConfiguration()
{
}

int CPremierConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::Premier;
}

WORD CPremierConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagPremier
				| CCategoryDataItem::pflagSilver
				| CCategoryDataItem::pflagBonusPack
				| CCategoryDataItem::pflagClassic;
}

LPCSTR CPremierConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'G':               // Group
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'N':               // Name (short name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'T':               // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMPR);
      }
      case 'D':               // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMPRCD);
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// COfficeSuiteConfiguration class

COfficeSuiteConfiguration::COfficeSuiteConfiguration()
{
}

COfficeSuiteConfiguration::~COfficeSuiteConfiguration()
{
}

int COfficeSuiteConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::OfficeSuite;
}

WORD COfficeSuiteConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagPremier
				| CCategoryDataItem::pflagSilver
				| CCategoryDataItem::pflagBonusPack
				| CCategoryDataItem::pflagClassic;
}

LPCSTR COfficeSuiteConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'G':               // Group
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'N':               // Name (short name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PM);
      }
      case 'T':               // Title (long name)
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMOS);
      }
      case 'D':               // Descriptive name
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_PMOSCD);
      }
      default:
      {
         return INHERITED::ReplacementText(cID);
      }
   }
}

///////////////////////////////////////////
// American Greetings Configuration Objects

// CAGConfiguration represents configuration info. that is the same for
// all American Greetings Products
CAGConfiguration::CAGConfiguration()
   {
   }

CAGConfiguration::~CAGConfiguration()
   {
   }

BOOL CAGConfiguration::SupportsCartoonMaker(BOOL fComplain)
{
   return FALSE;
}

BOOL CAGConfiguration::SupportsPersonalDelivery(BOOL fComplain)
{
   return TRUE;
}

LPCSTR CAGConfiguration::GetFilters(void) const
   {
      return "MS";
   }

DWORD CAGConfiguration::SupportedProjects(void)
{
   // Magic number for all projects -- even undefined ones!
	return 0xFFFDDFD7L;
}


// American Greetings Product Specific Configurations
CString CAGConfiguration::GetProductURLFileName()
   {
		return (m_pPathManager == NULL)
				? "AG.URL"
				: m_pPathManager->ExpandPath("AG.URL");
   }

LPCSTR CAGConfiguration::GetOnlineGreetingsFTPSite(void)
{
	return "onlinegreetings.mindscape.com";
}

CPoint CAGConfiguration::GetDefaultBMPResolution(void)
{
	return CPoint(100, 100);
}

CPoint CAGConfiguration::GetDefaultJPGResolution(void)
{
	return CPoint(100, 100);
}

CPoint CAGConfiguration::GetDefaultPNGResolution(void)
{
	return CPoint(100, 100);
}

LPCSTR CAGConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'R':					//EventReminder             
      {
         return GET_PMWAPP()->GetResourceStringPointer(IDS_FORGET_ME_NOT);
      }
      default:
         return INHERITED::ReplacementText(cID);
   }
}

CPrintStandardConfiguration::CPrintStandardConfiguration()
{
}

CPrintStandardConfiguration::~CPrintStandardConfiguration()
{
}

BOOL CPrintStandardConfiguration::SupportsUpgrading(void)
{
   return TRUE;
}

DWORD CPrintStandardConfiguration::SupportedProjects(void)
{
	return 0xEDE40018;
}

int CPrintStandardConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::PrintStandard;
}

WORD CPrintStandardConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagAGStandard;
}

BOOL CPrintStandardConfiguration::SupportsAddressBook(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOADDR)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsEventReminder(FALSE);
      }
   }
   return FALSE;
}

BOOL CPrintStandardConfiguration::SupportsEventReminder(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_NOREMINDER)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsEventReminder(FALSE);
      }
   }
   return FALSE;
}

BOOL CPrintStandardConfiguration::SupportsCardServer(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
      if (UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer(IDS_UNSUP_CARDSERVER)))
      {
      /*
      // They upgraded. Configuration has changed.
      // See if it supports it now (quietly).
      */
         return GetConfiguration()->SupportsEventReminder(FALSE);
      }
   }
   return FALSE;
}

LPCSTR CPrintStandardConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      case 'T':               // Title (long name)
      case 'G':               // Group
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGFPST);
      case 'D':               // Descriptive name
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGFPST_CD);
      case 'd':
         return "c:\\Program Files\\Mindscape\\AGPrint";
      default:
         return INHERITED::ReplacementText(cID);
   }
}
////
LPCSTR 
CArtAndMoreOnly1::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGFPST);
      case 'T':               // Title (long name)
      case 'G':               // Group
         return GET_PMWAPP()->GetResourceStringPointer(IDS_ASTORE);
      case 'D':               // Descriptive name
         return GET_PMWAPP()->GetResourceStringPointer(IDS_ASTORE_CD);
      case 'd':
         return "c:\\Program Files\\Mindscape\\American Greetings Print! Art & More Edition";
      default:
         return INHERITED::ReplacementText(cID);
   }
}

void
CArtAndMoreOnly1::DoUnsuport(LPCSTR fnc)
{
	CString str = fnc;
	str += " not available in\n";
	str += ReplacementText('G');
	str += ".";
	str += GetUnsupportedString();
	AfxMessageBox(str, MB_OK);   
}

BOOL 
CArtAndMoreOnly1::SupportsBorderPlus(BOOL fComplain /*=TRUE*/)
{
   if (fComplain)
   {
		DoUnsuport("BorderPlus is");
	}
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::SupportsSentimentGallery(BOOL fComplain /* TRUE */)
{
   if (fComplain)
   {
		DoUnsuport("The Sentiment Gallery is");
	}
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::SupportsPhotoEditor(BOOL fComplain /* TRUE */)
{
   if (fComplain)
   {
		DoUnsuport("Photo Editing is");
	}
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::SupportsLZW(BOOL fComplain /* TRUE */)
{
   if (fComplain)
   {
		DoUnsuport("LZW-compressed graphics are");
	}
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::SupportsEventReminder(BOOL fComplain /* TRUE */)
{
   if (fComplain)
   {
		DoUnsuport("The Event Reminder is");
	}
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::SupportsPersonalDelivery(BOOL fComplain /* FALSE */)
{
   if (fComplain)
   {
		DoUnsuport("Personal delivery is");
	}
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::CanInstantUpgrade(void)
{
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::SupportsCardServer(BOOL fComplain /* TRUE */)
{
   if (fComplain)
   {
		DoUnsuport("Online Greetings are");
	}
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::SupportsAddressBook(BOOL fComplain /* TRUE */)
{
   if (fComplain)
   {
		DoUnsuport("The Address Book is");
	}
   return FALSE;
}

BOOL 
CArtAndMoreOnly1::RemoveRegistration(void)
{
	return TRUE;
}
////
CPrintPremiumConfiguration::CPrintPremiumConfiguration()
{
}

CPrintPremiumConfiguration::~CPrintPremiumConfiguration()
{
}

BOOL CPrintPremiumConfiguration::SupportsUpgrading(void)
{
   return FALSE;
}

int CPrintPremiumConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::PrintPremium;
}

WORD CPrintPremiumConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagAGPremium;
}

LPCSTR CPrintPremiumConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      case 'T':               // Title (long name)
      case 'G':               // Group
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGFPPR);
      case 'D':               // Descriptive name
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGFPPR_CD);
      case 'd':
         return "c:\\Program Files\\Mindscape\\AGPrint";
      default:
         return INHERITED::ReplacementText(cID);
   }
}

CCraftStandardConfiguration::CCraftStandardConfiguration()
{
}

CCraftStandardConfiguration::~CCraftStandardConfiguration()
{
}

int CCraftStandardConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::CraftStandard;
}

WORD CCraftStandardConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagAGStandard;
}

BOOL CCraftStandardConfiguration::SupportsUpgrading(void)
{
   return TRUE;
}

DWORD CCraftStandardConfiguration::SupportedProjects(void)
{
	return 0xFFFDFFDAL;
}

BOOL CCraftStandardConfiguration::SupportsPersonalDelivery(BOOL fComplain)
{
   return FALSE;
}

LPCSTR CCraftStandardConfiguration::GetFilters(void) const
{
	return "MS AGCRPR";
}

BOOL CCraftStandardConfiguration::SupportsInstructionPage()
{
   return TRUE;
}

LPCSTR CCraftStandardConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      case 'T':               // Title (long name)
      case 'G':               // Group
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGCRST);
      case 'D':               // Descriptive name
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGCRST_CD);
      case 'd':
         return "c:\\Program Files\\Mindscape\\AGCraft";
      default:
         return INHERITED::ReplacementText(cID);
   }
}

CCraftDeluxeConfiguration::CCraftDeluxeConfiguration()
{
}

CCraftDeluxeConfiguration::~CCraftDeluxeConfiguration()
{
}

int CCraftDeluxeConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::CraftDeluxe;
}

WORD CCraftDeluxeConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagAGPremium;
}

LPCSTR CCraftDeluxeConfiguration::GetFilters(void) const
{
	return CAGConfiguration::GetFilters();
}

BOOL CCraftDeluxeConfiguration::SupportsUpgrading(void)
{
   return FALSE;
}

BOOL CCraftDeluxeConfiguration::SupportsOnlineGallery()
{
   return FALSE;
}

LPCSTR CCraftDeluxeConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      case 'T':               // Title (long name)
      case 'G':               // Group
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGCRDX);
      case 'D':               // Descriptive name
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGCRDX_CD);
      case 'd':
         return "c:\\Program Files\\Mindscape\\AGCraft";
      default:
         return INHERITED::ReplacementText(cID);
   }
}

CCreataPartyConfiguration::CCreataPartyConfiguration()
{
}

CCreataPartyConfiguration::~CCreataPartyConfiguration()
{
}

int CCreataPartyConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::CreataParty;
}

WORD CCreataPartyConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagAGStandard;
}

LPCSTR CCreataPartyConfiguration::GetFilters(void) const
{
	return "MS AGCPPR";
}

BOOL CCreataPartyConfiguration::SupportsUpgrading(void)
{
   return FALSE;
}

BOOL CCreataPartyConfiguration::SupportsOnlineGallery()
{
   return FALSE;
}

LPCSTR CCreataPartyConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      case 'T':               // Title (long name)
      case 'G':               // Group
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGCPST);
      case 'D':               // Descriptive name
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGCPST_CD);
      case 'd':
         return "c:\\Program Files\\Mindscape\\CreataParty";
      default:
         return INHERITED::ReplacementText(cID);
   }
}


CCreataCardConfiguration::CCreataCardConfiguration()
{
}

CCreataCardConfiguration::~CCreataCardConfiguration()
{
}

int CCreataCardConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::CreataCard;
}

WORD CCreataCardConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagAGStandard;
}

LPCSTR CCreataCardConfiguration::GetFilters(void) const
{
	return "MS AGCPPR";
}

BOOL CCreataCardConfiguration::SupportsPersonalDelivery(BOOL fComplain)
{
   return FALSE;
}

BOOL CCreataCardConfiguration::SupportsInstructionPage()
{
   return TRUE;
}

DWORD CCreataCardConfiguration::SupportedProjects(void)
{
	return 0xFFFDFFDAL;
}

BOOL CCreataCardConfiguration::SupportsUpgrading(void)
{
   return FALSE;
}

BOOL CCreataCardConfiguration::SupportsOnlineGallery()
{
   return FALSE;
}

LPCSTR CCreataCardConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      case 'T':               // Title (long name)
      case 'G':               // Group
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGCCST);
      case 'D':               // Descriptive name
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGCCST_CD);
      case 'd':
         return "c:\\Program Files\\Mindscape\\CreataCard";
      default:
         return INHERITED::ReplacementText(cID);
   }
}


CSpiritStandardConfiguration::CSpiritStandardConfiguration()
{
}

CSpiritStandardConfiguration::~CSpiritStandardConfiguration()
{
}

BOOL CSpiritStandardConfiguration::SupportsUpgrading(void)
{
   return FALSE;
}

int CSpiritStandardConfiguration::ProductLevel(void) const
{
   return CPrintMasterConfiguration::SpiritStandard;
}

WORD CSpiritStandardConfiguration::ProductFlags(void) const
{
   return CCategoryDataItem::pflagAGStandard;
}

LPCSTR CSpiritStandardConfiguration::GetFilters(void) const
   {
      return "AD SP MS";
   }

LPCSTR CSpiritStandardConfiguration::ReplacementText(char cID)
{
   switch (cID)
   {
      case 'N':               // Name (short name)
      case 'T':               // Title (long name)
      case 'G':               // Group
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGSPST);
      case 'D':               // Descriptive name
         return GET_PMWAPP()->GetResourceStringPointer(IDS_AGSPST_CD);
      case 'd':
         return "c:\\Program Files\\Mindscape\\AGSpirit";
      default:
         return INHERITED::ReplacementText(cID);
   }
}

/////////////////////////////////////////////////////////////////////////////
// UK English Platinum
// CPlatinumConfigurationENG class

CPlatinumConfigurationENG::CPlatinumConfigurationENG()
{
}

CPlatinumConfigurationENG::~CPlatinumConfigurationENG()
{
}

int CPlatinumConfigurationENG::ProductLevel(void) const
{
   return CPrintMasterConfiguration::PlatinumENG;
}

BOOL CPlatinumConfigurationENG::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CPlatinumConfigurationENG::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CPlatinumConfigurationENG::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// UK English Gold
// CGoldConfigurationENG class

CGoldConfigurationENG::CGoldConfigurationENG()
{
}

CGoldConfigurationENG::~CGoldConfigurationENG()
{
}

int CGoldConfigurationENG::ProductLevel(void) const
{
   return CPrintMasterConfiguration::GoldENG;
}

BOOL CGoldConfigurationENG::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CGoldConfigurationENG::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CGoldConfigurationENG::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// UK English Publishing Suite
// CSilverConfigurationENG class

CSilverConfigurationENG::CSilverConfigurationENG()
{
}

CSilverConfigurationENG::~CSilverConfigurationENG()
{
}

int CSilverConfigurationENG::ProductLevel(void) const
{
   return CPrintMasterConfiguration::SilverENG;
}

BOOL CSilverConfigurationENG::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CSilverConfigurationENG::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CSilverConfigurationENG::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// UK English Classic
// CClassicConfigurationENG class

CClassicConfigurationENG::CClassicConfigurationENG()
{
}

CClassicConfigurationENG::~CClassicConfigurationENG()
{
}

int CClassicConfigurationENG::ProductLevel(void) const
{
   return CPrintMasterConfiguration::ClassicENG;
}

BOOL CClassicConfigurationENG::SupportsUpgrading(void)
{
   return FALSE;
}

BOOL CClassicConfigurationENG::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CClassicConfigurationENG::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CClassicConfigurationENG::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


// GERMAN Configurations
/////////////////////////////////////////////////////////////////////////////
// German Platinum
// CPlatinumConfigurationDEU class

CPlatinumConfigurationDEU::CPlatinumConfigurationDEU()
{
}

CPlatinumConfigurationDEU::~CPlatinumConfigurationDEU()
{
}

int CPlatinumConfigurationDEU::ProductLevel(void) const
{
   return CPrintMasterConfiguration::PlatinumDEU;
}

BOOL CPlatinumConfigurationDEU::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CPlatinumConfigurationDEU::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CPlatinumConfigurationDEU::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// German Gold
// CGoldConfigurationDEU class

CGoldConfigurationDEU::CGoldConfigurationDEU()
{
}

CGoldConfigurationDEU::~CGoldConfigurationDEU()
{
}

int CGoldConfigurationDEU::ProductLevel(void) const
{
   return CPrintMasterConfiguration::GoldDEU;
}

BOOL CGoldConfigurationDEU::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CGoldConfigurationDEU::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CGoldConfigurationDEU::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// German Publishing Suite
// CSilverConfigurationDEU class

CSilverConfigurationDEU::CSilverConfigurationDEU()
{
}

CSilverConfigurationDEU::~CSilverConfigurationDEU()
{
}

int CSilverConfigurationDEU::ProductLevel(void) const
{
   return CPrintMasterConfiguration::SilverDEU;
}

BOOL CSilverConfigurationDEU::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CSilverConfigurationDEU::RemoveDividers(void)
{
	return TRUE;
}


#ifdef INSTALL
BOOL CSilverConfigurationDEU::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// German Classic
// CClassicConfigurationDEU class

CClassicConfigurationDEU::CClassicConfigurationDEU()
{
}

CClassicConfigurationDEU::~CClassicConfigurationDEU()
{
}

int CClassicConfigurationDEU::ProductLevel(void) const
{
   return CPrintMasterConfiguration::ClassicDEU;
}

BOOL CClassicConfigurationDEU::SupportsUpgrading(void)
{
   return FALSE;
}

BOOL CClassicConfigurationDEU::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CClassicConfigurationDEU::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CClassicConfigurationDEU::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


// FRENCH Configurations
/////////////////////////////////////////////////////////////////////////////
// French Platinum
// CPlatinumConfigurationFRA class

CPlatinumConfigurationFRA::CPlatinumConfigurationFRA()
{
}

CPlatinumConfigurationFRA::~CPlatinumConfigurationFRA()
{
}

int CPlatinumConfigurationFRA::ProductLevel(void) const
{
   return CPrintMasterConfiguration::PlatinumFRA;
}

BOOL CPlatinumConfigurationFRA::SupportsPhotoEditor(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CPlatinumConfigurationFRA::SupportsCartoonMaker(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CPlatinumConfigurationFRA::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CPlatinumConfigurationFRA::RemovePhotoEditor(void)
{
	return TRUE;
}
	
BOOL CPlatinumConfigurationFRA::RemoveCartoonMaker(void)
{
	return TRUE;
}

BOOL CPlatinumConfigurationFRA::RemoveRegistration(void)
{
	return TRUE;
}

BOOL CPlatinumConfigurationFRA::RemoveSerifDraw(void)
{
	return TRUE;
}

BOOL CPlatinumConfigurationFRA::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CPlatinumConfigurationFRA::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// French Gold
// CGoldConfigurationFRA class

CGoldConfigurationFRA::CGoldConfigurationFRA()
{
}

CGoldConfigurationFRA::~CGoldConfigurationFRA()
{
}

int CGoldConfigurationFRA::ProductLevel(void) const
{
   return CPrintMasterConfiguration::GoldFRA;
}

BOOL CGoldConfigurationFRA::SupportsPhotoEditor(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CGoldConfigurationFRA::SupportsCartoonMaker(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CGoldConfigurationFRA::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CGoldConfigurationFRA::RemovePhotoEditor(void)
{
	return TRUE;
}
	
BOOL CGoldConfigurationFRA::RemoveCartoonMaker(void)
{
	return TRUE;
}

BOOL CGoldConfigurationFRA::RemoveRegistration(void)
{
	return TRUE;
}

BOOL CGoldConfigurationFRA::RemoveSerifDraw(void)
{
	return TRUE;
}

BOOL CGoldConfigurationFRA::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CGoldConfigurationFRA::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// French Publishing Suite
// CSilverConfigurationFRA class

CSilverConfigurationFRA::CSilverConfigurationFRA()
{
}

CSilverConfigurationFRA::~CSilverConfigurationFRA()
{
}

int CSilverConfigurationFRA::ProductLevel(void) const
{
   return CPrintMasterConfiguration::SilverFRA;
}

BOOL CSilverConfigurationFRA::SupportsPhotoEditor(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CSilverConfigurationFRA::SupportsCartoonMaker(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CSilverConfigurationFRA::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CSilverConfigurationFRA::RemovePhotoEditor(void)
{
	return TRUE;
}
	
BOOL CSilverConfigurationFRA::RemoveCartoonMaker(void)
{
	return TRUE;
}

BOOL CSilverConfigurationFRA::RemoveRegistration(void)
{
	return TRUE;
}

BOOL CSilverConfigurationFRA::RemoveSerifDraw(void)
{
	return TRUE;
}

BOOL CSilverConfigurationFRA::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CSilverConfigurationFRA::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// French Classic
// CClassicConfigurationFRA class

CClassicConfigurationFRA::CClassicConfigurationFRA()
{
}

CClassicConfigurationFRA::~CClassicConfigurationFRA()
{
}

int CClassicConfigurationFRA::ProductLevel(void) const
{
   return CPrintMasterConfiguration::ClassicFRA;
}

BOOL CClassicConfigurationFRA::SupportsUpgrading(void)
{
   return FALSE;
}

BOOL CClassicConfigurationFRA::SupportsPhotoEditor(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CClassicConfigurationFRA::SupportsCartoonMaker(BOOL fComplain /*=TRUE*/)
{
   return FALSE;
}

BOOL CClassicConfigurationFRA::RemoveYearYouWereBorn(void)
{
	return TRUE;
}

BOOL CClassicConfigurationFRA::RemovePhotoEditor(void)
{
	return TRUE;
}
	
BOOL CClassicConfigurationFRA::RemoveCartoonMaker(void)
{
	return TRUE;
}

BOOL CClassicConfigurationFRA::RemoveRegistration(void)
{
	return TRUE;
}

BOOL CClassicConfigurationFRA::RemoveSerifDraw(void)
{
	return TRUE;
}

BOOL CClassicConfigurationFRA::RemoveDividers(void)
{
	return TRUE;
}

#ifdef INSTALL
BOOL CClassicConfigurationFRA::RemoveISP(void)
{ // should we remove the ISP from the installation?
	return TRUE;
}
#endif


