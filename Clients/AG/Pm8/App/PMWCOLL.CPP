/*
// $Workfile: PMWCOLL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWCOLL.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 64    2/24/99 1:31p Gbeddow
// fixed bug in regular (non-online) collection building that didn't set
// the thumbnail extents properly
// 
// 63    2/17/99 10:05a Gbeddow
// change "Online Thumbnail Extents=" entry in collection .ini file to
// "Thumbnail Extents=" to reflect the effect this setting has on BOTH the
// online thumbnail files and the thumbnails written to the collection
// 
// 62    2/09/99 1:48p Gbeddow
// Support in collection building for creating online collection GIF and
// JPEG thumbnail files, and related changes, per spec from Alexei
// 
// 61    2/08/99 1:47p Johno
// Got rid of ASSERT in ImportItem()
// 
// 60    2/05/99 2:49p Johno
// Projects of type PROJECT_TYPE_BlankPage are changed to
// PROJECT_TYPE_Poster in COrientationDialog::get_exit_info()
// 
// 59    2/04/99 3:06p Johno
// Changes for new "Blank Page" (poster) project type
// 
// 58    1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 57    1/05/99 5:31p Gbeddow
// art gallery (graphic browser) support for Print Shop-style graphic type
// filtering
// 
// 56    10/21/98 4:13p Jayn
// Product code mappings.
// 
// 55    10/19/98 10:50a Jayn
// More COM restructuring changes.
// 
// 54    9/23/98 11:57a Jayn
// Fixed assertions when file didn't exist.
// 
// 53    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 52    9/14/98 12:10p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 51    8/25/98 2:24p Jayn
// Fixed yet another bug with the preview code (this had to do with the
// mask).
// 
// 50    8/25/98 10:50a Jayn
// Fixed the round-off problems for good.
// 
// 49    8/24/98 3:00p Jayn
// Fixed a round-off problem in GetPreviewInfo
// 
// 48    8/20/98 6:23p Hforman
// Handling Craft subtype categories in ImportItem()
// 
// 47    8/12/98 10:29a Dennis
// Added support for Hidden Art collections
// 
// 46    8/10/98 3:05p Hforman
// moved some code into #ifdef NEW_THUMBNAILS
// 
// 45    7/24/98 9:07p Hforman
// fixed nasty bug in FindItem() I introduced last version
// 
// 44    7/23/98 7:30p Hforman
// putting dynamic name into DB instead of composite
// 
// 43    7/21/98 8:04p Hforman
// Changed key sent to CContentDirEntry::SetKey() from composite filename
// to dynamic filename, since composite could be greater than max key
// length
// 
// 42    7/18/98 4:10p Johno
// Use new GetPanelBoundForPreview() in GetPreviewInfo() (overridden in
// CCalendarDoc)
// 
// 41    7/15/98 6:05p Hforman
// added functions to convert 4.0 user projects to new category structure
// 
// 40    7/15/98 11:48a Johno
// Use CPmwDoc::GoToFirstPage() in place of switch in GetProjectDocument()
// 
// 39    7/14/98 4:40p Jayn
// Fix for groups in PMOs
// 
// 38    7/12/98 5:36p Jayn
// 
// 37    7/08/98 8:20p Hforman
// add "Copyright Title" handling
// 
// 36    7/06/98 5:45p Rlovejoy
// Added 'ProductCode' and 'ProjectTypes' variables and access.
// 
// 35    6/25/98 4:13p Jayn
// Got rid of an exception during builds.
// 
// 34    6/17/98 11:09a Jayn
// 
// 33    6/15/98 6:55p Hforman
// add "Match All" functionality for keyword searching
// 
// 32    6/08/98 4:08p Rlovejoy
// Accomodate various "copyright" placement types.
// 
// 31    6/01/98 5:45p Rlovejoy
// Added HiddenProjectsType; removed IsHidden().
// 
// 30    5/25/98 4:43p Fredf
// Support for building online collections.
// 
// 29    5/21/98 11:28a Hforman
// put back a line in Init() that got deleted accidentally
// 
// 28    5/21/98 10:13a Rlovejoy
// Implemented 'Hidden' attribute.
// 
// 27    5/20/98 6:17p Hforman
// added super collection functionality
// 
// 26    5/19/98 3:07p Hforman
// 
// 25    5/12/98 4:17p Hforman
// added ReloadCategoryStream()
// 
// 24    4/30/98 7:01p Hforman
// Modified Search() and Delete(), added RenameCategory()
// 
// 23    4/29/98 7:25p Jayn
// Fixed transparency problems.
// 
// 22    4/28/98 10:34a Dennis
// Added Order Form Filename entry
// 
// 21    4/24/98 7:41a Jayn
// Builder changes to support "inclusion column" and "external file".
// 
// 20    4/23/98 11:37a Rlovejoy
// Added support for copyright objects.
// 
// 19    4/21/98 9:11a Dennis
// Changed static Product Flag initialization to more generic
// initialization.
// 
// 18    4/20/98 9:09a Jayn
// Cached thumbnail support.
// 
// 17    4/16/98 7:11p Hforman
// 
// 16    4/15/98 4:33p Dennis
// Changes so code can be shared between PrintMaster and Collection
// Utility
// 
// 15    4/14/98 10:15a Dennis
// a) Moved NEW_THUMBNAILS and NEWCOMPRESSED_CONTENT defines to project
// file.
// b) Added content version support via. INI file
// c) Added external thumbnail support (enhanced UpdateThumbnail()).
// d) Removed Media and Style.
// e) Added Hide Item support.
// f) Added GetCategoryDataItem().
// 
// 14    4/13/98 3:27p Hforman
// user category mod
// 
// 13    4/13/98 2:35p Hforman
// add category param to ImportItem()
// 
// 12    4/08/98 6:44p Hforman
// interim checkin - more later
// 
// 11    4/07/98 8:41a Jayn
// Scales Pegasus palette.
// 
// 10    4/02/98 2:04p Hforman
// put JPEG compression back in for non-transparent bitmaps with > 255
// colors
// 
// 9     3/27/98 2:02p Hforman
// fixed bug in BuildDocumentPreview() related to transparency
// 
// 8     3/26/98 10:09a Jayn
// Support for DPP (Serif) importing, previewing, etc.
// 
// 7     3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 6     3/19/98 1:07p Hforman
// changes for transparent thumbnails (including adding Pegasus code)
// 
// 5     2/04/98 4:50p Hforman
// 
// 4     2/03/98 12:43p Hforman
// calling type_of_art_extension() to determine if ".pmo" file
// 
// 3     1/29/98 8:07p Hforman
// support for ".pmo" files
// 
// 2     1/19/98 1:21p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.2   21 Oct 1997 15:40:10   hal
// 
// modified BuildDocumentPreview() to allow interrupts
// 
//    Rev 1.1   16 Sep 1997 16:14:52   Jay
// Minor fix
// 
//    Rev 1.0   14 Aug 1997 15:24:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:50   Fred
// Initial revision.
// 
//    Rev 1.64   07 Aug 1997 09:13:24   Jay
// In OpenFile(), attempts to access a StorageFile-type to ensure it's available.
// 
//    Rev 1.63   06 Aug 1997 10:36:08   Jay
// Went back a rev since it destabilized the build (bad compiler!).
// 
//    Rev 1.61   04 Aug 1997 15:26:24   Fred
// Uses local buffer with make_full_path()
// 
//    Rev 1.60   30 Jul 1997 10:54:40   Fred
// Formats Paths with build ID
// 
//    Rev 1.59   21 Jul 1997 11:22:08   dennisp
// Keyword string now gets trimmed
// 
//    Rev 1.58   18 Jul 1997 12:59:36   dennisp
// Named "Your Own" category to fix delete problem
// 
//    Rev 1.57   17 Jul 1997 16:59:56   dennisp
// Removed logic to remove file extension from base name
// 
//    Rev 1.56   15 Jul 1997 11:02:58   dennisp
// Added set_panel call for Notecards
// 
//    Rev 1.55   11 Jul 1997 16:33:48   Jay
// Picture thumbnail generation won't fail for bad picture.
// 
//    Rev 1.54   10 Jul 1997 13:56:08   dennisp
// Added some #ifdef's NEW_THUMBNAILS for builder utility
// 
*/

#include "stdafx.h"
#include "pmwcoll.h"
#include "util.h"
#include "glicense.h"
#include "compfn.h"
#include "bmpfile.h"

#ifndef NO_PMWDOCS
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwtempl.h"
#endif

#ifdef NEW_COMPRESSED_CONTENT
#include "cdeflate.h"
#endif

// STATIC DATA
// INI File Entries
const char *   s_pszCollectionIcon  = "Icon";
const char *   s_pszFriendlyName    = "Friendly Name";
const char *   s_pszCDDir           = "CD Directory";
const char *   s_pszType            = "Type";
const char *   s_pszSubType         = "SubType";
const char *   s_pszSuperCollection = "SuperCollection";
const char *   s_pszBaseName        = "Base Name";
const char *   s_pszUser            = "User";
const char *   s_pszAddSubdirectory = "AddSubdirectory";
const char *   s_pszHasOrderForm		= "HasOrderForm";
const char *   s_pszSortIndex			= "SortIndex";
const char *   s_pszVersion         = "Version";
const char *   s_pszThumbnailDir    = "Thumbnail Directory";
const char *   s_pszThumbnailExtents = "Thumbnail Extents";
const char *   s_pszInclusionField  = "Inclusion Field";
const char *	s_pszCopyright			= "Copyright";
const char *   s_pszCopyrightTitle  = "Copyright Title";
const char *	s_pszOrderFileName   = "Order Filename";
const char *	s_pszProductCode		= "Product Code";
const char *	s_pszProjectTypes		= "Project Types";

const char *   s_pszDynamicName			= "[[S]]\\";
const char *   s_pszTypeProjects			= "Projects";
const char *   s_pszTypeArt				= "Art";
const char *   s_pszTypeSentiments		= "Sentiments";
const char *   s_pszTypeHiddenProjects	= "Hidden Projects";
const char *   s_pszTypeHiddenArt = "Hidden Art";

const char *	s_pszSubTypeArt			= "Art";
const char *	s_pszSubTypeWebArt		= "WebArt";

#define  CONTENTDIR_EXT       ".CDR"   // Content directory
#define  CONTENTDATA_EXT      ".CDT"   // Content data
#define  CATEGORYDIR_EXT      ".BDR"   // browser directory
#define  CATEGORYDATA_EXT     ".BDT"   // category data (item data and MST flags)
#define  THUMBNAILDIR_EXT     ".TDR"   // thumbnail directory
#define  THUMBNAILDATA_EXT    ".TDT"   // thumbnail data file
#define  ITEMSDIR_EXT         ".IDR"   // items directory
#define  ITEMSDATA_EXT        ".IDT"   // items data file
#define  KEYWORDSDIR_EXT      ".KDR"   // keywords directory
#define  KEYWORDSDATA_EXT     ".KDT"   // keywords data file

#define  FREELIST_EXT         ".FRL"   // Free list database

#define  STOPLIST_FILENAME  "stoplist.txt"

// Static class member initialization
BOOL                 CPMWCollection::sm_bReadStopList = FALSE;
CMlsStringArray      CPMWCollection::sm_csaStopList;
BYTE                 CPMWCollection::sm_byID=1;
// Default to supporting all products
USHORT               CPMWCollection::sm_shProductFlags = 0xffff;

/////////////////////////////////////////////////////////////////////////////
//
CPMWLicensedInfo::CPMWLicensedInfo()
{
}

CPMWLicensedInfo::~CPMWLicensedInfo()
{
}

BOOL CPMWLicensedInfo::IsLicensed(DWORD dwItemNumber)
{
	int nCount = m_cdaUnlicensed.GetSize();
	ASSERT((nCount & 1) == 0);
	for (int n = 0; n < nCount; )
	{
		DWORD dwStart = m_cdaUnlicensed.GetAt(n++);
		DWORD dwEnd = m_cdaUnlicensed.GetAt(n++);
		if (dwItemNumber >= dwStart && dwItemNumber <= dwEnd)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CPMWLicensedInfo::AddUnlicensedRange(DWORD dwStartItem, DWORD dwEndItem)
{
	int nCount = m_cdaUnlicensed.GetSize();
	if (nCount > 0)
	{
		// See if this range is contiguous with the last range.
		if (m_cdaUnlicensed.GetAt(nCount-1) == dwStartItem-1)
		{
			// It is. Extent the last range.
			m_cdaUnlicensed.SetAt(nCount-1, dwEndItem);
			return;
		}
	}

	// Add a new range.
	m_cdaUnlicensed.Add(dwStartItem);
	m_cdaUnlicensed.Add(dwEndItem);
}

/////////////////////////////////////////////////////////////////////////////
// CPMWCollectionData

CPMWCollectionData::CPMWCollectionData()
   {
      Empty();
   }

void CPMWCollectionData::Empty()
   {
      m_dwUnused = 0;
      m_strContentFileName.Empty();
      m_strFriendlyName.Empty();
      m_strKeywords.Empty();
      m_strThumbnailFileName.Empty();
      m_CategoryData.Empty();
      m_ContentData.Empty();
   }

/////////////////////////////////////////////////////////////////////////////
// CPMWCollection

CPMWCollection::CPMWCollection()
   {
      m_bInitialized = FALSE;
      m_enVersion = verOriginal;
   }

CPMWCollection::CPMWCollection(const CString &strINIFileName,
   const CString &strSection, CollectionOrigin origin, UINT OpenFlag,
   const CString &strCollectionPath)
   {
      InitMembers(strINIFileName, strSection, origin, OpenFlag, strCollectionPath, FALSE);
   }

CPMWCollection::~CPMWCollection()
   {
      int         nIndex;

      CloseAllStreams(TRUE);

      // Go through all databases and make sure they are closed
      // This will also release memory for file objects
      for(nIndex=0;nIndex < dbListEnd;nIndex++)
         CloseDatabase((DatabaseTypes) nIndex);

      // Make sure everything is cleaned up
      for(nIndex=0;nIndex < sfListEnd;nIndex++)
         CloseFile(ftStorageFile, nIndex);
      for(nIndex=0;nIndex < ciListEnd;nIndex++)
         CloseFile(ftCIndex, nIndex);
      for(nIndex=0;nIndex < cfListEnd;nIndex++)
         CloseFile(ftCFile, nIndex);

		if (m_pCollectionDraw != NULL)
		{
			m_pCollectionDraw->Release();
			m_pCollectionDraw = NULL;
		}
   }

int CPMWCollection::Init(IPathManager* pPathManager,
								 ICollectionDraw* pCollectionDraw,
								 const CString &strINIFileName,
								 const CString &strSection, 
								 CollectionOrigin origin, 
								 UINT OpenFlag,
								 const CString &strCollectionPath,
								 BOOL bOnlineBuild)
   {
      InitMembers(strINIFileName, strSection, origin, OpenFlag,
         strCollectionPath, bOnlineBuild);
      return Init(pPathManager, pCollectionDraw);
   }

void CPMWCollection::InitMembers(const CString &strINIFileName,
											const CString &strSection,
											CollectionOrigin origin,
											UINT OpenFlag,
											const CString &strCollectionPath,
											BOOL bOnlineBuild)
   {
      m_strINIFileName     = strINIFileName;
      m_strSection         = strSection;
      m_nOpenFlag          = OpenFlag;
      m_strCollectionPath  = strCollectionPath;
		m_bOnlineBuild			= bOnlineBuild;

      // The collection path defaults to the content directory if empty.
      if(m_strCollectionPath.IsEmpty())
         {
            m_strCollectionPath = s_pszDynamicName;
         }

      m_IniFile.Name(strINIFileName);

      m_bInitialized = FALSE;
      m_origin = origin;
   }

int CPMWCollection::Init(IPathManager* pPathManager, ICollectionDraw* pCollectionDraw)
   {
      CString        strEntry;
      int            nIndex, nRetVal;

		// Initialize the path manager.
		m_pPathManager			= pPathManager;

		m_pCollectionDraw		= pCollectionDraw;
		if (m_pCollectionDraw != NULL)
		{
			m_pCollectionDraw->AddRef();
		}

      // If you get this assertion, Init() has already occurred-duplicate call
      ASSERT(!m_bInitialized);

      // If already initialized, nothing to do
      if(m_bInitialized)
         return TRUE;

      m_byID = 0;

      for(nIndex=0;nIndex < dbListEnd;nIndex++)
         m_naDBOpenCount[nIndex] = 0;
      for(nIndex=0;nIndex < sfListEnd;nIndex++)
         m_paStorageFiles[nIndex] = NULL;
      for(nIndex=0;nIndex < ciListEnd;nIndex++)
         m_paCIndexFiles[nIndex] = NULL;
      for(nIndex=0;nIndex < cfListEnd;nIndex++)
         m_paCFiles[nIndex] = NULL;

      m_bUserCollection = FALSE;
      m_nStreamsOpenCount = 0;
      m_nStreamsOpen = 0;

      m_pDBContent      = NULL;
      m_pDBCategory     = NULL;
      m_pDBThumbnails   = NULL;
      m_pDBItemInfo     = NULL;
      m_pDBKeywords     = NULL;

      if(!Util::FileExists(m_IniFile.Name()))
         return ERRORCODE_NoINIFile;

      if(!sm_bReadStopList)
      {
         ReadStopList();
      }

      m_strIcon = m_IniFile.GetString(m_strSection, s_pszCollectionIcon);
      // Check for Icon existance

      m_strFriendlyName = m_IniFile.GetString(m_strSection, s_pszFriendlyName);
      m_strCDDir = m_IniFile.GetString(m_strSection, s_pszCDDir);

		// collection type
      m_strType = m_IniFile.GetString(m_strSection, s_pszType);
      if(m_strType.CompareNoCase(s_pszTypeProjects) == 0)
         m_Type = typeProjects;
      else if(m_strType.CompareNoCase(s_pszTypeArt) == 0)
         m_Type = typeArt;
      else if(m_strType.CompareNoCase(s_pszTypeSentiments) == 0)
         m_Type = typeSentiments;
      else if(m_strType.CompareNoCase(s_pszTypeHiddenProjects) == 0)
         m_Type = typeHiddenProjects;
      else if(m_strType.CompareNoCase(s_pszTypeHiddenArt) == 0)
         m_Type = typeHiddenArt;
      else
         return ERRORCODE_InvalidINIEntry;

		// collection subtype
		m_SubType = subTypeNone;
      m_strSubType = m_IniFile.GetString(m_strSection, s_pszSubType);
      if (m_Type == typeArt && (m_strSubType.CompareNoCase(s_pszSubTypeArt) == 0 || m_strSubType.IsEmpty()))
         m_SubType = subTypeArt;
      else if (m_Type == typeArt && m_strSubType.CompareNoCase(s_pszSubTypeWebArt) == 0)
			m_SubType = subTypeWebArt;
      else if (!m_strSubType.IsEmpty())
			return ERRORCODE_InvalidINIEntry;

		m_fAddSubdirectory	= m_IniFile.GetInteger(m_strSection, s_pszAddSubdirectory, FALSE);
		m_fHasOrderForm		= m_IniFile.GetInteger(m_strSection, s_pszHasOrderForm, FALSE);
		m_nSortIndex			= m_IniFile.GetInteger(m_strSection, s_pszSortIndex, 0);
		m_enVersion				= (Version)m_IniFile.GetInteger(m_strSection, s_pszVersion, verOriginal);

		// Super Collection name is optional
		m_strSuperCollection	= m_IniFile.GetString(m_strSection, s_pszSuperCollection);

		// Copyright Title name is optional
		m_strCopyrightTitle	= m_IniFile.GetString(m_strSection, s_pszCopyrightTitle);

      // Base Name is optional
      m_strBaseName  = m_IniFile.GetString(m_strSection, s_pszBaseName);
      if(m_strBaseName.IsEmpty())
         m_strBaseName = m_strSection;
      if(m_origin == originUser)
         m_bUserCollection = TRUE;

		// Product code is optional
		m_strProductCode = m_IniFile.GetString(m_strSection, s_pszProductCode);

		// Project Types
		m_dwProjectTypes = m_IniFile.GetInteger(m_strSection, s_pszProjectTypes, 0);

		// Thumbnail Dir is optional
      m_strThumbnailDir = m_IniFile.GetString(m_strSection, s_pszThumbnailDir);

		// if m_bOnlineBuild, set m_strOnlineThumbnailDir to build online thumbnail files
		if (m_bOnlineBuild)
	      m_strOnlineThumbnailDir = "FTP\\Thumbnails";

		// Thumbnail Extents is optional, default 100x100 for thumbnails
      m_dwThumbnailExtents = m_IniFile.GetInteger(m_strSection, s_pszThumbnailExtents, 100);

		// Inclusion field is optional
      m_strInclusionField = m_IniFile.GetString(m_strSection, s_pszInclusionField);

		// Retrieve copyright info for cards
		CString csCopyright;
		CString CopyrightSearch[] = {
			"", "HFT", "QFT", "HFW", "QFW"
		};
		for (int i = COPYRIGHT_Default; i < COPYRIGHT_NVariants; i++)
		{
			CString csSearch = s_pszCopyright + CopyrightSearch[i];
			csCopyright = m_IniFile.GetString(m_strSection, (const char*)csSearch);
			if (!csCopyright.IsEmpty())
			{
				// break it up...
				m_Copyright[i].csCopyrightName = csCopyright.SpanExcluding(",");
				CString csBound = csCopyright.Right(csCopyright.GetLength() - (csCopyright.Find(',')+1));
				sscanf((const char*)csBound, "%f,%f,%f,%f",
					&m_Copyright[i].rctAbstractBound.left,
					&m_Copyright[i].rctAbstractBound.top,
					&m_Copyright[i].rctAbstractBound.right,
					&m_Copyright[i].rctAbstractBound.bottom);
			}
		}

      // Construct the database file name prefix.
      Util::ConstructPath(m_strDBFileNamePrefix, m_strCollectionPath, m_strBaseName);

      if(m_strIcon.IsEmpty() || m_strFriendlyName.IsEmpty() ||
         m_strType.IsEmpty())
            return ERRORCODE_InvalidINIEntry;

      // If not a user collection, we need CD directory and name entries
      if(!IsUserCollection())
         {
            if(m_strCDDir.IsEmpty())
               return ERRORCODE_InvalidINIEntry;

            // Non user collections may have an order filename entry
            m_strOrderFileName = m_IniFile.GetString(m_strSection, s_pszOrderFileName);
            // If no entry, default to using collection filename prefix 
            if(m_strOrderFileName.IsEmpty())
               {
                  m_strOrderFileName = GetDBFileNamePrefix();
                  m_strOrderFileName += ".ORD";
               }
            m_strOrderFileName = m_pPathManager->LocatePath(m_strOrderFileName);
         }

		// Read the override paths for the individual files.

		// Get the list of paths entries.
		// The section is the base name + " Paths" (like "PMG4ART Paths").
		CString csPathsSection = m_strSection;
		csPathsSection += " Paths";

		LPSTR pEntries = m_IniFile.GetSectionKeys(csPathsSection);
		if (pEntries != NULL)
		{
			for (LPCSTR pEntry = pEntries; *pEntry != '\0'; pEntry += strlen(pEntry)+1)
			{
				// Read the next path.
				// The entry is the extension (with the '.'), and the value is
				// the path.
				CString csFormat = m_IniFile.GetString(csPathsSection, pEntry);
				CString csPath;
#ifdef NEW_THUMBNAILS
				csPath.Format(csFormat, GET_PMWAPP()->GetBuildId());
#endif
				if (!csPath.IsEmpty())
				{
					m_mapPaths.SetAt(pEntry, csPath);
				}
			}
         delete [] pEntries;
		}

		// Read the product code mappings.
		// Key is code in collection data. Value is code in LicenseData.
		// This came into being because the Online server uses a JDE product
		// code (> 6 characters), but the collection format only supports 6
		// characters (old MLS format). So, we map.
		// Typically, the product codes burned into the online collections will
		// all be the same, with the differentiation happening via this map to
		// the real server product code.

		// Get the list of mappings.
		// The section is the base name + " Product Code Map" (like "PMG4ART Product Code Map").
		CString csMapSection = m_strSection;
		csMapSection += " Product Code Map";

		pEntries = m_IniFile.GetSectionKeys(csMapSection);
		if (pEntries != NULL)
		{
			for (LPCSTR pEntry = pEntries; *pEntry != '\0'; pEntry += strlen(pEntry)+1)
			{
				// Read the next mapping.
				// The entry is the collection-based code, while the value is the
				// server-based code.
				CString csValue = m_IniFile.GetString(csMapSection, pEntry);
				if (!csValue.IsEmpty())
				{
					m_mapProductCode.SetAt(pEntry, csValue);
				}
			}
         delete [] pEntries;
		}

      // If user collection, Open/Create free list database and
      // leave open until collection object is destroyed.
      // If more than one file ends up using the free list
      // then this might need to be done on Collection init
      if(m_origin == originUser)
         {
            nRetVal = OpenFreeList();
            if(nRetVal != ERRORCODE_None)
               return nRetVal;
            m_bUserCollection = TRUE;
         }

      m_bInitialized = TRUE;
      m_byID = GetNextID();
      return ERRORCODE_None;
   }

int CPMWCollection::Search(CPMWCollectionSearchContext& Context)
{
	// Unpack the search context.
	CString strCategory = Context.m_strAryCategories.GetAt(0);
	CString strKeywords = Context.m_strKeywords;
	BOOL fKeywordsMatchAll = Context.m_fKeywordsMatchAll;
	CCategoryDataItem::Tones  tone = (CCategoryDataItem::Tones)Context.m_nTone;
	CCategoryDataItem::CustomFlags  graphictype = (CCategoryDataItem::CustomFlags)Context.m_nGraphicType;
	CPMWCollectionResultArray& ResultArray = Context.GetSearchResultArray();
	CPMWRankingInfoArray& RankingInfoArray = Context.GetRankingInfoArray();

   BYTE                 byDefaultItemValue;
   BOOL                 bMatchGraphicType, bMatchTone, bMatchProductFlags;
   DWORD                dwNumCategoryItems, dwItemNum;
   DWORD                dwNumItems, dwCurItem;
   int                  nRetVal, i, nCurKeyword;
   USHORT               usCurItem, usNumCategoryItems;
   USHORT               usNumKeywordItems;
   CMlsStringArray      csaKeywords;
   CCategoryData        categoryData;
   CKeywordDirEntry     keywordEntry;
	CByteArray				byaScores;

	if(!m_bInitialized)
	{
		ASSERT(m_bInitialized);
		return ERRORCODE_NotInitialized;
	}

	ResultArray.RemoveAll();
	DWORD dwNumResults = 0;
	int nKeywordCount = 0;

	// Select the streams to open.
	int nStreams = streamCategory | streamItem | streamKeyword;
	if (!IsOpen(nStreams))
	{
		if ((nRetVal = OpenStreams(nStreams)) != ERRORCODE_None)
			return nRetVal;
	}

   nRetVal = m_pDBCategory->FindFirst(strCategory, &categoryData);
   if(nRetVal != ERRORCODE_None || categoryData.m_carrayItems.GetSize() <= 0)
      return ERRORCODE_Fail;

   // Search result set is currently limited to 64K items
//    dwNumCategoryItems = m_pDBCategory->GetItemCount();
	m_pDBItemInfo->GetItemRange(&dwNumCategoryItems);
   if(dwNumCategoryItems == 0 || dwNumCategoryItems > 0xffff)
   {
      return ERRORCODE_Fail;
   }
   // Limit result set to 64K items
   // usNumCategoryItems represents the number of items in the category
   // database
   usNumCategoryItems = (USHORT) dwNumCategoryItems;

   // Do not use dwNumCategoryItems after this point!

	byaScores.SetSize(usNumCategoryItems);

	Util::Trim(strKeywords);
   if(strKeywords.IsEmpty())
      byDefaultItemValue = 0;
   else
   {
      byDefaultItemValue = 0x80;   // Set high bit
      // Parse passed Keyword string into words where the delimiter is a space
      csaKeywords.SetNames(strKeywords, ' ');
      nKeywordCount = csaKeywords.GetSize();
      if(csaKeywords.GetSize() > MaxKeywords)
      {
         return ERRORCODE_Fail;   // Too many keywords
      }
   }

   // Init score array
   for (usCurItem = 0; usCurItem < usNumCategoryItems; usCurItem++)
      byaScores[usCurItem] = byDefaultItemValue;

   for (nCurKeyword = 0; nCurKeyword < csaKeywords.GetSize(); nCurKeyword++)
   {
      nRetVal = m_pDBKeywords->FindFirst(csaKeywords[nCurKeyword], &keywordEntry);
      // DGP-Do wildcard Keyword string search here
      while(nRetVal == ERRORCODE_None)
      {
         usNumKeywordItems = keywordEntry.m_carrayItemNumbers.GetSize();

         // Loop through all items associated with this keyword
         for (i = 0; i < usNumKeywordItems; i++)
         {
            // Get item number from keyword item number array
            dwItemNum = keywordEntry.GetItemNumber(i);

            ASSERT(dwItemNum <= 0xffff);
				byaScores[dwItemNum-1] |= (1 << nCurKeyword);
         }
			nRetVal = m_pDBKeywords->FindNext(csaKeywords[nCurKeyword], &keywordEntry);
      }
   }

   do
   {
      CCategoryDataItem    *pDataItem;

      // Get number of items in found category
      dwNumItems = categoryData.m_carrayItems.GetSize();

      // Loop through them 
      for (dwCurItem = 0; dwCurItem < dwNumItems; dwCurItem++)
      {
         BOOL  bHideItem = FALSE;

         // Default to matching Tone
         bMatchTone = TRUE;
         // Get Data item (dwCurItem) from category search result set
         pDataItem = (CCategoryDataItem *)categoryData.m_carrayItems[dwCurItem];
         ASSERT(pDataItem);

         // Check if it matches Tone
         // Tone will default to matching if caller specifys none
         if(tone != CCategoryDataItem::toneNone)
            bMatchTone  = (tone == pDataItem->GetTone());

         // Default to matching Graphic Type
         // Check if it matches graphictype
         // Graphic Type will default to matching if caller specifies none
         bMatchGraphicType = TRUE;
         if (graphictype != CCategoryDataItem::cfNone)
            bMatchGraphicType  = (graphictype == pDataItem->GetGraphicType());

         // If version is new collection format, check if item is hidden
         if(GetVersion() > verOriginal)
         {
            if(pDataItem->GetCustomFlags() & CCategoryDataItem::cfHideItem)
               bHideItem = TRUE;
         }

         // Compare flags for this data item to flags for collection class
         // If one of the data item flags matches the flags for the 
         // collection, this is a valid content item for this product
         USHORT uProductFlags = pDataItem->GetProductFlags();
         if(uProductFlags == 0 || (uProductFlags & GetProductFlags()))
            bMatchProductFlags = TRUE;
         else
            bMatchProductFlags = FALSE;
			if(bMatchTone && bMatchGraphicType && bMatchProductFlags && !bHideItem)
			{
				// If keywords, clear the high bit only, otherwise set score to 1
				if (pDataItem->GetItemNumber()-1 >= usNumCategoryItems)
				{
					ASSERT(0);	// something's messed up!
				}
				else
				{
					if(csaKeywords.GetSize())
						byaScores[pDataItem->GetItemNumber()-1] &= 0x7f;
					else
						byaScores[pDataItem->GetItemNumber()-1] = 1;
				}
			}
		}
   } while((nRetVal = m_pDBCategory->FindNext(&categoryData)) == ERRORCODE_None);

	// Allocate the search result array.
	ResultArray.SetSize(usNumCategoryItems);

   // Copy over results
   if(csaKeywords.GetSize())
   {
      // If keywords, then result array is sorted by score

      // Add Results with multiple keyword hits first
      for (int nHitCount = csaKeywords.GetSize(); nHitCount > 0; nHitCount--)
      {
			// Compute a ranking info as we go.
			CPMWRankingInfo RankingInfo;
         for (dwCurItem = 0; dwCurItem < usNumCategoryItems; dwCurItem++)
         {
            // If high bit is cleared and score matches, it's good
            BYTE bScore = byaScores[dwCurItem];
            if (GetBitCount(bScore) == nHitCount)
            {
               CPMWCollectionResult& Result = ResultArray.ElementAt(dwNumResults);
               Result.SetCollectionID(GetID());
					Result.SetItemNumber(dwCurItem+1);

               // if 1st time we got a result with this keyword count, it's the starting index
               if(RankingInfo.GetCount() == 0)
                  RankingInfo.SetStartIndex((USHORT)dwNumResults);
               RankingInfo.IncrementCount();
               dwNumResults++;
            }
				RankingInfoArray.Add(RankingInfo);
         }
			if (fKeywordsMatchAll)
			{
				// if we're anding, we're done
				nHitCount = 0;
			}
      }
   }
   else // no keywords
   {
      for (dwCurItem = 0; dwCurItem < usNumCategoryItems; dwCurItem++)
      {
         if( byaScores[dwCurItem] == 1)
         {
            CPMWCollectionResult& Result = ResultArray.ElementAt(dwNumResults);
            Result.SetCollectionID(GetID());
				Result.SetItemNumber(dwCurItem+1);
            dwNumResults++;
         }
      }
   }
	// Shrink the array to the size we want.
	ASSERT(dwNumResults <= usNumCategoryItems);
	ResultArray.SetSize(dwNumResults);

   return ERRORCODE_None;
}

// Get the friendly name for a volume.
BOOL CPMWCollection::ExtensionToPath(LPCSTR pszExtension, CString& csPath) const
	{
		return m_mapPaths.Lookup(pszExtension, csPath);
	}

BYTE CPMWCollection::GetBitCount(BYTE byValueToCount)
   {
      // if high bit set, skip it-return score of 0
      if(byValueToCount & 0x80)
         return 0;

		static BYTE byaBitCounts[16] =
		{
			0,			// 0 = 0000
			1,			// 1 = 0001
			1,			// 2 = 0010
			2,			// 3 = 0011
			1,			// 4 = 0100
			2,			// 5 = 0101
			2,			// 6 = 0110
			3,			// 7 = 0111
			1,			// 8 = 1000
			2,			// 9 = 1001
			2,			// A = 1010
			3,			// B = 1011
			2,			// C = 1100
			3,			// D = 1101
			3,			// E = 1110
			4			// F = 1111
		};

		return byaBitCounts[byValueToCount & 0x0f] + byaBitCounts[byValueToCount>>4];
   }

void CPMWCollection::GetLicensedInfo(CPMWLicensedInfo& Info)
{
	// Run through the categories in this collection and add unlicensed ranges.

   CCategoryDir* pDir = m_pDBCategory->GetDir();
   int nCategories = pDir->GetCategoryCount();
	int nLastLevel = pDir->GetNumLevels()-1;

	DWORD dwStartItem = 1;
   DWORD dwEndItem;

	for (int nCategory = 0; nCategory < nCategories; nCategory++)
	{
		CCategoryEntry* pCategory = pDir->GetCategoryEntry(nCategory);
		ASSERT(pCategory != NULL);
		if (pCategory == NULL)
		{
			break;
		}

		// Skip top level categories, look at sub-categories only
		if (pCategory->GetLevel() != nLastLevel)
		{
			continue;
		}
		dwEndItem = dwStartItem + (pCategory->GetItemCount() - 1);

		// See if this category is licensed or not.
		if (!LicenseData.ProductIsLicensed(MapProductCode(pCategory->GetProductCode())))
		{
			Info.AddUnlicensedRange(dwStartItem, dwEndItem);
		}

		dwStartItem = dwEndItem+1;
	}
}

const CString & CPMWCollection::GetOrderFileName() const
   {
      return m_strOrderFileName;
   }

#ifndef NO_PMWDOCS
// Import an item. This probably only works on a user collection.
int CPMWCollection::ImportItem(LPCSTR szFileName,
										 LPCSTR szFriendlyName /*=NULL*/,
										 LPCSTR szKeywords /*=NULL*/,
										 LPCSTR szUserCategory /*=NULL*/,
										 BOOL fThumbnailOnly /*=TRUE*/)
{
	int nResult = ERRORCODE_Memory;
	CString csFullName = m_pPathManager->ExpandPath(szFileName);
	CString csName = m_pPathManager->MakeCompositePath(csFullName);
	CompositeFileName cfn(csName);
	CString strDynamicName = cfn.get_dynamic_name(TRUE);
	if (!strDynamicName.IsEmpty())
	{
		CPMWCollectionData Data;
		Data.m_strContentFileName = strDynamicName;
// FF: DWORD m_dwContentTimeStamp was phased out starting in PM 5.0
//		Data.m_dwContentTimeStamp = Util::ComputeTimeStamp(csFullName);
		Data.m_dwUnused = 0;
		Data.m_strFriendlyName = szFriendlyName;
		Data.m_strKeywords = szKeywords;
		Data.m_ContentData.Empty();

		if (Data.m_strFriendlyName.IsEmpty())
		{
			CompositeFileName cfn(strDynamicName);
			Util::SplitPath(cfn.get_file_name(), NULL, NULL, &Data.m_strFriendlyName, NULL);
		}

		CString strCategory = "";
		if (szUserCategory != NULL)
		{
			strCategory = szUserCategory;

			if (m_Type == typeProjects)
			{
				// Get the project type (super category) and prepend to category
				CDocTemplate* pTemplate = GET_PMWAPP()->GetMatchingTemplate(strDynamicName);
				if (pTemplate)
				{
					int nType = ((CPmwDocTemplate*)pTemplate)->ProjectType();
					// Blank pages are saved as, and remain, posters						
					if (nType == PROJECT_TYPE_BlankPage)
					{
						// Blank pages don't really exist
						// We will change it to a poster
						nType = PROJECT_TYPE_Poster;	
					}

					// NOTE: Craft projects are currently split into subproject types.
					if (nType == PROJECT_TYPE_Craft)
					{
						CString strFilename = m_pPathManager->LocatePath(strDynamicName);
						CPmwDoc* pDoc = (CPmwDoc*)GET_PMWAPP()->OpenHiddenDocument(strFilename);
						if (pDoc)
						{
							CRAFT_TYPE craftType = (CRAFT_TYPE)pDoc->DocumentRecord()->GetProjectSubtype();
							CMlsStringArray mlsCrafts(IDS_CRAFT_TYPES);
							if (craftType > 0 && craftType < mlsCrafts.GetSize())
							{
								strCategory = mlsCrafts[craftType] + "|";
							}
							else
							{
								// Didn't find a subproject type. Just use "Crafts"
								strCategory.LoadString(IDS_CRAFTS);
								strCategory += "|";
							}

							strCategory += szUserCategory;
							pDoc->OnCloseDocument();
						}
					}
					else
					{
						CMlsStringArray mlsProjects(IDS_PROJECT_TYPES);
						if (nType >= 0 && nType < mlsProjects.GetSize())
						{
							strCategory = mlsProjects[nType] + "|";
						}
						else
						{
							// Something's messed up! Just use "generic"
							ASSERT(0);
							strCategory = mlsProjects[0] + "|";
						}

						strCategory += szUserCategory;
					}
				}
			}
			else if (m_Type == typeArt)
			{
				strCategory = szUserCategory;
			}
		}

		CCategoryDataItem cdItem;
		cdItem.Empty();
		Data.m_CategoryData.Set(strCategory, CString(""), &cdItem);

		DWORD dwItemNumber;
		if (FindItem(strDynamicName, &dwItemNumber) == ERRORCODE_None)
		{
			// Update existing item thumbnail.
			if (fThumbnailOnly)
			{
				nResult = UpdateThumbnail(dwItemNumber, &Data);
			}
			else
			{
				nResult = Update(dwItemNumber, &Data);
			}
		}
		else
		{
			// Add new item.
			nResult = Add(&Data);
		}
	}
   return nResult;
}

BOOL CPMWCollection::Is40UserProjects()
{
	if (!m_bUserCollection || !(m_Type == typeProjects))
		return FALSE;

	if (!IsOpen(streamCategory))
	{
		if (OpenStreams(streamCategory) != ERRORCODE_None)
			return FALSE;
	}

	if (m_pDBCategory->GetDir()->GetNumLevels() == 1)
		return TRUE;
	else
		return FALSE;
}

int CPMWCollection::Convert40UserProjectCategories()
{
	// Convert 4.0 Project categories to new category structure

	int nRetVal = ERRORCODE_None;

	// Select the streams to open.
	int nStreams = streamCategory | streamItem;
	if (!IsOpen(nStreams))
	{
		if ((nRetVal = OpenStreams(nStreams)) != ERRORCODE_None)
			return nRetVal;
	}
	
	CCategoryDir* pCatDir = m_pDBCategory->GetDir();
	if (pCatDir == NULL)
		return ERRORCODE_Corrupt;

	CCategoryEntry oldEntry;
	if (pCatDir->GetItem(0, &oldEntry) != ERRORCODE_None)
		return ERRORCODE_Corrupt;

	int nItem;
	int nItems = pCatDir->GetItemCount();

	// Get the data item numbers for all items in the DB
	CDWordArray dataItemNums;
	for (DWORD dwItem = 1; dwItem <= (DWORD)nItems; dwItem++)
	{
		CCategoryDataItem dataItem;
		if (m_pDBCategory->GetCategoryDataItem(dwItem, &dataItem) != ERRORCODE_None)
			return ERRORCODE_Corrupt;
		
		dataItemNums.Add(dataItem.GetItemNumber());
	}

	// Delete the current category DB
	for (nItem = 0; nItem < nItems; nItem++)
	{
		DWORD dwItem = dataItemNums[nItem];
		if (m_pDBCategory->Delete(dwItem) != ERRORCODE_None)
			return ERRORCODE_Corrupt;
	}

	// Re-add each item to category DB
	for (nItem = 0; nItem < nItems; nItem++)
	{
		DWORD dwItem = dataItemNums[nItem];

		// Get filename from item info data object
		CItemsDataEntry dataEntry;
		if (m_pDBItemInfo->Find(dwItem, &dataEntry) != ERRORCODE_None)
			return ERRORCODE_Corrupt;
		if (m_pDBItemInfo->Read() != ERRORCODE_None)
			return ERRORCODE_Corrupt;

		CString strFilename = dataEntry.GetFileName();

		// load default project category
		CString strDefCat;
		strDefCat.LoadString(IDS_DEF_PROJECT_CATEGORY);

		// Get the project type (super category) and prepend to category.
		CDocTemplate* pTemplate = GET_PMWAPP()->GetMatchingTemplate(strFilename);
		if (pTemplate)
		{
			int nType = ((CPmwDocTemplate*)pTemplate)->ProjectType();
			CMlsStringArray mlsProjects(IDS_PROJECT_TYPES);
			CString strCategory;
			if (nType >= 0 && nType < mlsProjects.GetSize())
			{
				CString strCategory = mlsProjects[nType] + "|";
				strCategory += strDefCat;
			}
			else
			{
				// Something's messed up! Just use "generic"
				ASSERT(0);
				strCategory = mlsProjects[0] + "|";
			}

			strCategory += strDefCat;
			CCategoryDataItem dataItem;
			dataItem.Empty();
			dataItem.SetItemNumber(dwItem);

			CCategoryData catData;
			catData.Set(strCategory, oldEntry.GetProductCode(), &dataItem, oldEntry.GetFlags());

			if (m_pDBCategory->Add(&catData, TRUE) != ERRORCODE_None)
				return ERRORCODE_Corrupt;
		}
		else // failed to get template
		{
			return ERRORCODE_Corrupt;
		}
	}

	return nRetVal;
}

#endif // NO_PMWDOCS

int CPMWCollection::Add(CPMWCollectionData *pCollectionData)
   {
      int      nRetVal;

      ASSERT(pCollectionData);

      m_pClientCD = pCollectionData;

      m_itemData.m_carrayKeywordIDs.RemoveAll();
		if (!IsOpen())
		{
			if ((nRetVal = OpenStreams()) != ERRORCODE_None)
				return nRetVal;
		}

      // Get next item number
      nRetVal = m_pDBItemInfo->GetNewItemNumber(&m_dwCurItemNumber);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;
		CCategoryDataItem* pCatData = (CCategoryDataItem *)pCollectionData->m_CategoryData.m_carrayItems[0];
      pCatData->SetItemNumber(m_dwCurItemNumber);

      // Add Category
      nRetVal = AddCategoryItem();
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      // Add keywords
      nRetVal = AddKeywords();
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      // Add Item info
      nRetVal = AddItemInfo();
//      if(nRetVal != ERRORCODE_None)
//         return nRetVal;

      // Add content
      nRetVal = AddContent();
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      // Add Thumbnail - No argument causes append 
      nRetVal = UpdateThumbnail();
//      if(nRetVal != ERRORCODE_None)
//         return nRetVal;

      return nRetVal;
   }

int CPMWCollection::FindItem(const CString &strFileName, DWORD *pdwItemNumber)
{
   ASSERT(pdwItemNumber);

	if (strFileName.IsEmpty())
		return ERRORCODE_Fail;	// nothing to find

   int                  nRetVal;
   CFileContent         *pContentFile;
   CContentDirEntry     contentDir;
   CContentDataEntry    contentData;

   nRetVal = NewContentStream(&pContentFile);
   if(nRetVal != ERRORCODE_None)
      return nRetVal;

	contentDir.SetKey(strFileName);
	nRetVal = pContentFile->Find(&contentDir, &contentData);
	if (nRetVal == ERRORCODE_None)
	{
		// Fill in callers item number
		*pdwItemNumber = contentDir.GetItemNum();
	}
	else
	{
	   // We may have been passed a composite filename, but the
		// DB has just the dynamic name as the key, so try that.
		CompositeFileName cfn(strFileName);
		// see if we have a dynamic name
		if (cfn.has_dynamic_name())
		{
			CString strDynamic = cfn.get_dynamic_name(TRUE);
			contentDir.SetKey(strDynamic);
			nRetVal = pContentFile->Find(&contentDir, &contentData);
			if (nRetVal == ERRORCODE_None)
			{
				*pdwItemNumber = contentDir.GetItemNum();
			}
		}
	}

   // Cleanup
   ReleaseContentStream(pContentFile);
   return nRetVal;
}

int CPMWCollection::AddKeywords()
   {
      int               i, nNumWords;
      DWORD             dwKeywordID;
      CString           strNewKeywords, strFriendlyName;
      CMlsStringArray   csaNewKeywords, csaFriendlyNameWords;


      // Replace all hypens and ampersands with spaces which should make 
      // better keywords for friendly name portion.

      // Keywords should be left unmodified since it's usually desirable
      // to leave these special characters in.  There is a better chance
      // for a search hit since both forms of a word end up in the index.

      strFriendlyName = m_pClientCD->m_strFriendlyName;

      // Add Category and Sub-Category as keywords for normal collection types
      if(!IsUserCollection())
         {
            CMlsStringArray   csaCategories;
            int               nCategoryCount, nCurCategory;

            csaCategories.SetNames(m_pClientCD->m_CategoryData.GetCategory());
            nCategoryCount = csaCategories.GetSize();
            // Skip adding project type/super category
            if(GetType() != CPMWCollection::typeProjects)
               nCurCategory = 0;
            else
               nCurCategory = 1;
            // We should have 2 categories for collections other than projects
            // Projects have 3 categories
            ASSERT(nCategoryCount >= 2 && nCategoryCount <= 3);
            for(;nCurCategory < nCategoryCount; nCurCategory++)
               {
                  strFriendlyName += ' ';
                  strFriendlyName += csaCategories[nCurCategory];
               }
         }

      Util::ReplaceChar(strFriendlyName, '-', ' ');
      Util::ReplaceChar(strFriendlyName, '&', ' ');

      // Make keywords out of keyword and friendly name fields
      strNewKeywords = m_pClientCD->m_strKeywords;

      // Parse keywords field into array of keywords
      // Keywords are delimited by a space
      csaNewKeywords.SetNames(strNewKeywords, ' ');

      // Parse friendly name into words
      csaFriendlyNameWords.SetNames(strFriendlyName, ' ');

      // Add friendly name words as keywords
      // Loop thru all friendly name words and add those that that are 
      // not already in keyword list
      for(i=0, nNumWords = csaFriendlyNameWords.GetSize(); i < nNumWords; i++)
         {
            // if failed to find friendly name word in keyword list
            // then add it to keyword list
            CString           strFriendlyName=csaFriendlyNameWords[i];

            if(csaFriendlyNameWords[i].IsEmpty())
               continue;

            // Remove anything that isn't alpha-numeric
            Util::RemoveChar(csaFriendlyNameWords[i], Util::NotAlphaNumeric);

            // Check if word is already in keyword list
            if(csaNewKeywords.FindName(csaFriendlyNameWords[i], CMlsStringArray::IgnoreCase) >= 0)
               continue;

            csaNewKeywords.Add(csaFriendlyNameWords[i]);
         }
      // Add keywords to keyword database
      for(i=0, nNumWords = csaNewKeywords.GetSize(); i<nNumWords; i++)
         {
            Util::Capitalize(csaNewKeywords[i]);
            // Search for word in stoplist, if found don't add it
            if(sm_csaStopList.FindName(csaNewKeywords[i], CMlsStringArray::IgnoreCase) >= 0)
               continue;

            dwKeywordID = m_pDBKeywords->Add(csaNewKeywords[i], m_dwCurItemNumber);
            m_itemData.m_carrayKeywordIDs.Add(&dwKeywordID);
         }
      return ERRORCODE_None;
   }

// Reads stop list into CString array
// The stop list is a list of words which should be not be used as keywords
int CPMWCollection::ReadStopList()
   {
      BOOL           bOpened;
      CHAR           szModuleFileName[_MAX_PATH+1];
      CHAR           chLine[128];            // Shouldn't have a word larger than this 
      int            /*nLineCount,*/ nCharIndex;
      CFile          cfileStopList;
      CString        strStopListFileName;
      TextReadFile   trStopListFile(STOPLIST_FILENAME);

      ::GetModuleFileName(AfxGetInstanceHandle(), szModuleFileName,
         sizeof(szModuleFileName)-1);
      strStopListFileName = szModuleFileName;
      nCharIndex = strStopListFileName.ReverseFind('\\');
      if(nCharIndex < 0)
         {
            ASSERT(0);
            return ERRORCODE_IntError;
         }
      // Remove filename prefix
      strStopListFileName.GetBufferSetLength(nCharIndex);
      strStopListFileName += '\\';
      strStopListFileName +=  STOPLIST_FILENAME;

      // Check for existance
      bOpened = cfileStopList.Open(strStopListFileName, CFile::modeRead);
      if(!bOpened)
		{
			sm_bReadStopList = TRUE;		// Don't do this again.
         return ERRORCODE_Open;
		}
      cfileStopList.Close();

      // Start with empty stoplist
      sm_csaStopList.SetSize(0);

      // Read each line which should be a stopword
      while(trStopListFile.read_line(chLine, sizeof(chLine)) == ERRORCODE_None)
         {
            CPMWCollection::sm_csaStopList.Add(chLine);
//            nLineCount++;
         }
      sm_bReadStopList = TRUE;
      return ERRORCODE_None;
   }

int CPMWCollection::AddItemInfo()
   {
      m_itemData.SetFriendlyName(m_pClientCD->m_strFriendlyName);
      m_itemData.SetFileName(m_pClientCD->m_strContentFileName);
      return m_pDBItemInfo->Add(&m_itemData);
   }

int CPMWCollection::AddCategoryItem()
   {
      int      nRetVal;
      nRetVal = m_pDBCategory->Add(&m_pClientCD->m_CategoryData, m_bUserCollection);
      return nRetVal;
   }

int CPMWCollection::AddContent()
   {
      int               nRetVal;
      CContentDirEntry  contentDir;
      CContentDataEntry *pContentEntry;

      contentDir.SetKey(m_pClientCD->m_strContentFileName);

// FF: DWORD m_dwContentTimeStamp was phased out starting in PM 5.0
//		contentDir.SetTimeStamp(m_pClientCD->m_dwContentTimeStamp);
      contentDir.SetItemNum(m_dwCurItemNumber);
      pContentEntry = &m_pClientCD->m_ContentData;

#ifdef NEW_COMPRESSED_CONTENT
      // Fixup the compression value in the info in case we do not compress.
      DWORD dwUncompressedSize = pContentEntry->m_DataHeader.GetUnCompressedSize();
      ASSERT(dwUncompressedSize == pContentEntry->GetDataSize());
      pContentEntry->m_DataHeader.Set(dwUncompressedSize, dwUncompressedSize, 0);

      CDeflate Deflator;
      CContentDataEntry CompressedContentEntry;

      LPBYTE pData = NULL;
      DWORD dwDestWritten = 0;

      // Compress the data. If it compresses, store it that way.
      if (pContentEntry->GetData() != NULL)
         {
				if (DataIsCompressable(m_pClientCD->m_strContentFileName))
					{
						// Try compressing the data.
						if (Deflator.Compress((LPBYTE)pContentEntry->GetData(),
													 dwUncompressedSize,
													 &pData,
													 &dwDestWritten))
							{
								// We have data. If the size got smaller, use it.
								if (dwDestWritten < pContentEntry->GetDataSize())
									{
										CompressedContentEntry.m_DataHeader.Set(dwUncompressedSize,
																							 dwDestWritten,
																							 2);
										CompressedContentEntry.Attach(pData, dwDestWritten);
										pContentEntry = &CompressedContentEntry;
									}
							}
					}
         }
		else
		// No content data in the stream. This is external storage.
         contentDir.SetStorageMethod(CContentDirEntry::storageExternal);
#endif

      nRetVal = m_pDBContent->Add(&contentDir, pContentEntry);

#ifdef NEW_COMPRESSED_CONTENT
      CDeflate::FreeData(pData);
#endif
      return nRetVal;
   }

#ifdef NEW_COMPRESSED_CONTENT
BOOL CPMWCollection::DataIsCompressable(LPCSTR pszFile)
   {
      if (m_Type == typeSentiments)
         return FALSE;
      CString csExtension;
      Util::SplitPath(pszFile, NULL, NULL, NULL, &csExtension);
      return csExtension.CompareNoCase(".TIF") != 0
            && csExtension.CompareNoCase(".JPG") != 0;
   }
#endif

// UpdateThumbnail will append the thumbnail to the thumbnail database
// if dwItemNum == 0, Otherwise an update occurs
int CPMWCollection::UpdateThumbnail(DWORD dwItemNum)
{
   int                        nRetVal = ERRORCODE_Fail;

#ifdef NEW_THUMBNAILS
	CThumbnailDataEntry ThumbnailData;

	int nStreams = streamThumbnail | streamContent;
   if(!IsOpen(nStreams))
   {
		nRetVal = OpenStreams(nStreams);
		if(nRetVal != ERRORCODE_None)
			return nRetVal;
	}
   m_dwCurItemNumber = dwItemNum;

   if(m_pClientCD->m_strThumbnailFileName.IsEmpty())
   {
		if (UseCachedThumbnail(m_pClientCD, &ThumbnailData))
			nRetVal = ERRORCODE_None;
		else
		{
			if (m_pCollectionDraw != NULL)
			{
				// GCB 2/24/99 set thumbnail extents
				m_pClientCD->m_dwThumbnailExtents = m_dwThumbnailExtents;

				// GCB 2/3/99 if online thumbnail file is needed, set m_strOnlineThumbnailDir,
				// m_fAddSubdirectory in m_pClientCD so that
				// m_pCollectionDraw will create the thumbnail file with the specified filename,
				// and in the proper directory in CPMWCollectionDraw::BuildThumbnail()
				if (!m_strOnlineThumbnailDir.IsEmpty())
				{
					m_pClientCD->m_strOnlineThumbnailDir = m_strCollectionPath;
					CString csTmp( m_strOnlineThumbnailDir );
					if (csTmp[0] == '\\')
						Util::RemoveBackslashFromPath(m_pClientCD->m_strOnlineThumbnailDir);
					else
						Util::AppendBackslashToPath(m_pClientCD->m_strOnlineThumbnailDir);
					m_pClientCD->m_strOnlineThumbnailDir += csTmp;
					m_pClientCD->m_fAddSubdirectory = m_fAddSubdirectory;
				}

				nRetVal = m_pCollectionDraw->BuildThumbnail(GetType(), m_pClientCD, &ThumbnailData);
			}
			else
			{
				// Attempt to draw without a valid object to draw with.
				ASSERT(FALSE);
				nRetVal = ERRORCODE_IntError;
			}
			CacheThumbnail(m_pClientCD, &ThumbnailData);
		}
      // If we built it, add it
      if(nRetVal == ERRORCODE_None)
      {
         if(dwItemNum == 0)
			{
				TRY
				{
					m_pDBThumbnails->Add(&ThumbnailData);
				}
				CATCH_ALL(e)
				{
					nRetVal = ERRORCODE_Fail;
				}
				END_CATCH_ALL
			}
         else
            nRetVal = m_pDBThumbnails->Update(dwItemNum, &ThumbnailData);
      }
   }
   else
   {
      CBitmapFile    cbfThumbnail;
      CBitmapFile::ErrorCode  ecBitmap;

      ecBitmap = cbfThumbnail.Read(m_pClientCD->m_strThumbnailFileName);
      if(ecBitmap == CBitmapFile::ecNone)
      {
         const BITMAPINFOHEADER *pbmpHeader;

         pbmpHeader = cbfThumbnail.GetInfoHeader();

			CString csEmptyStr;

         ThumbnailData.SetThumbnailData(
            (BITMAPINFO *)cbfThumbnail.GetInfo(),
            (LPBYTE)cbfThumbnail.GetData(),
            NULL, 
            pbmpHeader->biWidth,
            pbmpHeader->biHeight,
            pbmpHeader->biBitCount,
            cbfThumbnail.GetDataSize(),
				csEmptyStr);
			TRY
			{
				m_pDBThumbnails->Add(&ThumbnailData);
            nRetVal = ERRORCODE_None;
			}
			CATCH_ALL(e)
			{
				nRetVal = ERRORCODE_Fail;
			}
			END_CATCH_ALL
      }
   }
#else
   return ERRORCODE_Fail;
#endif
   return nRetVal;
}

int CPMWCollection::Delete(DWORD dwItemToDelete)
   {
      BOOL                 bDeleteError = FALSE;
      int                  nRetVal;
      CItemsDataEntry      itemInfoEntry;
      CString              strFileName;

      if(!IsOpen())
      {
         nRetVal = OpenStreams();
         if(nRetVal != ERRORCODE_None)
            return nRetVal;
      }

      m_dwCurItemNumber = dwItemToDelete;

      // Find item info for filename and Keyword ID data
      nRetVal = m_pDBItemInfo->Find(dwItemToDelete, &itemInfoEntry);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;

      // Fill in Item Info data object
      nRetVal = m_pDBItemInfo->Read();
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Read;

      // After this point, try to delete as much as we can
      nRetVal = DeleteKeywords(dwItemToDelete, itemInfoEntry);
      if(nRetVal != ERRORCODE_None)
         bDeleteError = TRUE;

      // Delete Content item
      CContentDirEntry     contentDirEntry;
      CContentDataEntry    contentDataEntry;

		contentDirEntry.SetKey(itemInfoEntry.GetFileName());
		nRetVal = m_pDBContent->Find(&contentDirEntry, &contentDataEntry);
		if (nRetVal != ERRORCODE_None)
		{
			// We may have been passed a composite filename, but the
			// DB has just the dynamic name as the key, so try to find that.
			CompositeFileName cfn(itemInfoEntry.GetFileName());
			// see if we have a dynamic name
			if (cfn.has_dynamic_name())
			{
				CString strDynamic = cfn.get_dynamic_name(TRUE);
				contentDirEntry.SetKey(strDynamic);
				nRetVal = m_pDBContent->Find(&contentDirEntry, &contentDataEntry);
				bDeleteError = (nRetVal != ERRORCODE_None);
			}
			else
				bDeleteError = TRUE;
		}

		if (nRetVal == ERRORCODE_None)
		{
	      nRetVal = m_pDBContent->Delete();
	      if(nRetVal != ERRORCODE_None)
		      bDeleteError = TRUE;
		}
      
      // Done with item info data, delete it
      nRetVal = m_pDBItemInfo->Delete();
      if(nRetVal != ERRORCODE_None)
         bDeleteError = TRUE;
      
      // Delete Category Item
      nRetVal = m_pDBCategory->Delete(dwItemToDelete);
      if(nRetVal != ERRORCODE_None)
         bDeleteError = TRUE;

      // Delete Thumbnail
      CThumbnailDataEntry thumbnailData;
      nRetVal = m_pDBThumbnails->Find(dwItemToDelete, &thumbnailData);
      if(nRetVal != ERRORCODE_None)
         bDeleteError = TRUE;
      nRetVal = m_pDBThumbnails->Delete();
      if(nRetVal != ERRORCODE_None)
         bDeleteError = TRUE;

      if(bDeleteError)
         return ERRORCODE_Fail;

      return ERRORCODE_None;
   }

// DeleteKeywords removes the Keywords associated with an item
// This method doesn't actually remove the Keywords from the Keywords database
// but removes an items association with a keyword
int CPMWCollection::DeleteKeywords(DWORD dwItemToDelete, CItemsDataEntry &itemInfoEntry)
   {
      int                  nRetVal, nNumKeywordIds, i;
      DWORD                dwKeywordID;
      CString              strKeyword;
      CKeywordDirEntry     keywordEntry;

		int nStreams = streamKeyword;
      if(!IsOpen(nStreams))
         {
            nRetVal = OpenStreams(nStreams);
            if(nRetVal != ERRORCODE_None)
               return nRetVal;
         }

      // Delete item associated with various Keywords
      nNumKeywordIds = itemInfoEntry.m_carrayKeywordIDs.GetSize();
      nRetVal = ERRORCODE_None;
      for(i=0; i < nNumKeywordIds && nRetVal == ERRORCODE_None;i++)
         {
            dwKeywordID = itemInfoEntry.GetKeywordID(i);
//            dwKeywordID = (*(DWORD *)itemInfoEntry.m_carrayKeywordIDs[i]);
            // Find keyword by ID and fill in Keyword string
            nRetVal = m_pDBKeywords->Find(dwKeywordID, strKeyword);
            if(nRetVal == ERRORCODE_None)
               {
                  // Locate by Keyword now to get items
                  nRetVal = m_pDBKeywords->Find(strKeyword, &keywordEntry);
                  if(nRetVal == ERRORCODE_None)
                     nRetVal = m_pDBKeywords->Delete(dwItemToDelete);
                  else
                     continue;
               }
            else
               continue;
         }
      return nRetVal;
   }

int CPMWCollection::Update(DWORD dwItemToUpdate, CPMWCollectionData *pCollectionData)
   {
      int               nRetVal;

      ASSERT(pCollectionData);
      m_pClientCD = pCollectionData;
      m_dwCurItemNumber = dwItemToUpdate;

      nRetVal = UpdateThumbnail(dwItemToUpdate);
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      nRetVal = UpdateItemInfo(dwItemToUpdate, pCollectionData);
      return nRetVal;
   }

int CPMWCollection::UpdateItemInfo(DWORD dwItemToUpdate, CPMWCollectionData *pCollectionData)
   {
      CItemsDataEntry   itemInfoEntry;
      int               nRetVal;
      CString           strCurKeywords;

		int nStreams = streamKeyword | streamItem;
      if(!IsOpen(nStreams))
         {
            nRetVal = OpenStreams(nStreams);
            if(nRetVal != ERRORCODE_None)
               return nRetVal;
         }
      ASSERT(pCollectionData);
      m_pClientCD = pCollectionData;
      m_dwCurItemNumber = dwItemToUpdate;

      m_itemData.Empty();
      m_itemData.SetFriendlyName(m_pClientCD->m_strFriendlyName);
      m_itemData.SetFileName(m_pClientCD->m_strContentFileName);

      // Find item info for filename and Keyword ID data
      nRetVal = m_pDBItemInfo->Find(dwItemToUpdate, &itemInfoEntry);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Fail;

      // Fill in Item Info data object
      nRetVal = m_pDBItemInfo->Read();
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Read;

      // DGP - Make Update smart by getting existing keywords and friendly name
      // and update only whats needed
#if 0
      nRetVal = GetKeywords(dwItemToUpdate, strCurKeywords);
      if(nRetVal == ERRORCODE_None)
         {
            Util::Trim()
         }
#endif
      // Remove all Keywords associated with this item
      nRetVal = DeleteKeywords(dwItemToUpdate, itemInfoEntry);
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      // Add new keywords and build Keyword ID list
      nRetVal = AddKeywords();
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      nRetVal = m_pDBItemInfo->Update(dwItemToUpdate, &m_itemData);

      return nRetVal;
   }

int CPMWCollection::UpdateThumbnail(DWORD dwItemNum,
                           CPMWCollectionData *pCollectionData)
	{
		ASSERT(dwItemNum != 0);
      ASSERT(pCollectionData);
		if (dwItemNum == 0 || pCollectionData == NULL)
			{
				return ERRORCODE_IntError;
			}

      int               nRetVal;

      m_pClientCD = pCollectionData;

      nRetVal = UpdateThumbnail(dwItemNum);
      return nRetVal;
	}

int CPMWCollection::GetItemInfo(LPCSTR pszName,
										  DWORD& dwItemNumber,
										  CString& csDescription,
										  CString& csKeywords)
{
	int nError;
	if (dwItemNumber == 0)
	{
		if ((nError = FindItem(pszName, &dwItemNumber)) != ERRORCODE_None)
		{
			return nError;
		}
	}
	ASSERT(dwItemNumber != 0);

	CFileItems* pFileItems = NULL;
	// Open up an item information stream so we can get the file name and
	// friendly name of the given item.
	if ((nError = NewItemInfoStream(&pFileItems)) == ERRORCODE_None)
	{
		// Search for the item.
		CItemsDataEntry Information;
		if ((nError = pFileItems->Find(dwItemNumber, &Information)) == ERRORCODE_None)
		{
			// Read the item information.
			if ((nError = pFileItems->Read()) == ERRORCODE_None)
			{
				// Save the file name and friendly name.
				csDescription = Information.GetFriendlyName();
				// Now, handle the keywords.
				CFileKeywordDir* pKeywords = NULL;
				if (Information.GetKeywordCount() > 0
						&& (nError = NewKeywordStream(&pKeywords)) == ERRORCODE_None)
				{
					// Read the keywords.
					for (int nKeyword = 0;
							nKeyword < Information.GetKeywordCount();
							nKeyword++)
					{
						CString csKeyword;
						if ((nError = pKeywords->Find(Information.GetKeywordID(nKeyword), csKeyword)) == ERRORCODE_None)
						{
							if (!csKeywords.IsEmpty())
							{
								csKeywords += ' ';
							}
							csKeywords += csKeyword;
						}
						else
						{
							break;
						}
					}
					ReleaseKeywordStream(pKeywords);
				}
			}
		}
		ReleaseItemInfoStream(pFileItems);
	}
	return nError;
}

int CPMWCollection::GetKeywords(DWORD dwItem, CString &strKeywords)
   {
      int               i, nRetVal;
      CItemsDataEntry   itemInfoEntry;
      CString           strKeyword;
      DWORD             dwKeywordID;

		int nStreams = streamItem | streamKeyword;
      if(!IsOpen(nStreams))
         {
            nRetVal = OpenStreams(nStreams);
            if(nRetVal != ERRORCODE_None)
               return nRetVal;
         }
      nRetVal = m_pDBItemInfo->Find(dwItem, &itemInfoEntry);
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      nRetVal = m_pDBItemInfo->Read();
      if(nRetVal != ERRORCODE_None)
         return nRetVal;

      strKeywords.Empty();

      // Loop through keywords IDs and build string 
      for(i=0; i < itemInfoEntry.GetKeywordCount(); i++)
         {
            dwKeywordID = itemInfoEntry.GetKeywordID(i);
            nRetVal = m_pDBKeywords->Find(dwKeywordID, strKeyword);
            if(nRetVal != ERRORCODE_None)
               return nRetVal;
            // Separate keywords by a space
            if(!strKeywords.IsEmpty())
               strKeywords += ' ';
            strKeywords += strKeyword;
         }
      return ERRORCODE_None;
   }

CString CPMWCollection::GetProductCode(DWORD dwItem)
   {
      int         nRetVal;

		int nStreams = streamCategory;
      if(!IsOpen(nStreams))
         {
            nRetVal = OpenStreams(nStreams);
            if(nRetVal != ERRORCODE_None)
//               return nRetVal;		// <-- Say what?
					return "";
         }
      return MapProductCode(m_pDBCategory->GetProductCode(dwItem));
   }

//
// Map a product code.
// The incoming code is the product code as stored in the collection file.
// The outgoing code is the server-based product code (the one stored in
// the license data).
//

CString CPMWCollection::MapProductCode(const CString& csProductCode)
{
	CString csValue;
	// Lookup the product code. We short-circuit the lookup in obvious cases.
	if (!csProductCode.IsEmpty()					// Code is not empty
		 && m_mapProductCode.GetCount() != 0	// We have some mappings
		 && m_mapProductCode.Lookup(csProductCode, csValue))
	{
		// A non-empty product code should not map to an empty one!
		// Since this is being stored in an INI file, somebody could potentially
		// edit the file to map the code to an empty one. We do not allow that.
		// Of course, they could map all their collections to one product code,
		// which they then purchase, thereby releasing everything.
		if (!csValue.IsEmpty())
		{
			return csValue;
		}
	}
	return csProductCode;
}

CString CPMWCollection::GetCategory(DWORD dwItem)
{
	return m_pDBCategory->GetCategory(dwItem, m_bUserCollection);
}

ERRORCODE CPMWCollection::GetCategoryDataItem(DWORD dwItem, CCategoryDataItem *pDataItem)
   {
      ERRORCODE errorcode;

		int nStreams = streamCategory;
      if(!IsOpen(nStreams))
         {
            errorcode = (ERRORCODE) OpenStreams(nStreams);
            if(errorcode != ERRORCODE_None)
               return errorcode;
         }
      return m_pDBCategory->GetCategoryDataItem(dwItem, pDataItem);
}

ERRORCODE CPMWCollection::RenameCategory(CString& strOld, CString& strNew)
{
	if (m_bUserCollection)
		return m_pDBCategory->RenameCategory(strOld, strNew);
	else
		return ERRORCODE_Access;
}

int CPMWCollection::OpenStreams(int nStreams /*=streamAll*/)
{
   int      nRetVal = ERRORCODE_None;

   // Check open reference count for database stream
	int nStreamsToOpen = (nStreams & ~m_nStreamsOpen);
   if(m_nStreamsOpenCount == 0 || nStreamsToOpen != 0)
   {
		// DGP-Need to throw exception if creating new stream fails
		if (nStreamsToOpen & streamContent)
			nRetVal = NewContentStream(&m_pDBContent);
		if (nRetVal == ERRORCODE_None && (nStreamsToOpen & streamCategory))
			nRetVal = NewCategoryStream(&m_pDBCategory);
		if (nRetVal == ERRORCODE_None && (nStreamsToOpen & streamThumbnail))
			nRetVal = NewThumbnailStream(&m_pDBThumbnails);
		if (nRetVal == ERRORCODE_None && (nStreamsToOpen & streamItem))
			nRetVal = NewItemInfoStream(&m_pDBItemInfo);
		if (nRetVal == ERRORCODE_None && (nStreamsToOpen & streamKeyword))
			nRetVal = NewKeywordStream(&m_pDBKeywords);

		if (nRetVal == ERRORCODE_None)
			m_nStreamsOpen |= nStreams;	// Do not use nStreamsToOpen! IsOpen() will get confused.
		else
			CloseAllStreams(TRUE);
	}

	if (nRetVal == ERRORCODE_None)
		m_nStreamsOpenCount++;

   return nRetVal;
}

int CPMWCollection::CloseAllStreams(BOOL bForceClose)
   {
      if(m_nStreamsOpenCount == 0)
         return ERRORCODE_None;

      // Check open reference count for database stream
      // If going to 0 then actually close files
      m_nStreamsOpenCount--;
      if(m_nStreamsOpenCount > 0 && !bForceClose)
         return ERRORCODE_None;

      ASSERT(m_nStreamsOpenCount >= 0);
      if(m_nStreamsOpenCount < 0)
         {
            // Caller is attempting to close when there is no need to
            // We should never get here
            m_nStreamsOpenCount = 0;
            return ERRORCODE_IntError;
         }

      if(m_pDBContent)
         ReleaseContentStream(m_pDBContent);
      if(m_pDBCategory)
         ReleaseCategoryStream(m_pDBCategory);
      if(m_pDBThumbnails)
         ReleaseThumbnailStream(m_pDBThumbnails);
      if(m_pDBItemInfo)
         ReleaseItemInfoStream(m_pDBItemInfo);
      if(m_pDBKeywords)
         ReleaseKeywordStream(m_pDBKeywords);
      m_pDBContent      = NULL;
      m_pDBCategory     = NULL;
      m_pDBThumbnails   = NULL;
      m_pDBItemInfo     = NULL;
      m_pDBKeywords     = NULL;

      m_nStreamsOpenCount = 0;
		m_nStreamsOpen = 0;

      return ERRORCODE_None;
   }

#if 0
int CPMWCollection::OpenAllDatabases()
   {
      int      nRetVal, nIndex;
      
      for(nIndex=0;nIndex < dbListEnd && nRetVal == ERRORCODE_None;nIndex++)
         nRetVal = OpenDatabase((DatabaseTypes) nIndex);
      return nRetVal;
   }

int CPMWCollection::CloseAllDatabases()
   {
      int      nRetVal, nIndex;
      
      for(nIndex=0;nIndex < dbListEnd && nRetVal == ERRORCODE_None;nIndex++)
         nRetVal = CloseDatabase((DatabaseTypes) nIndex);
      return nRetVal;
   }
#endif

int CPMWCollection::NewContentStream(CFileContent **pNewStream)
   {
      return NewStream(dbContent, (LPVOID *) pNewStream);
   }

int CPMWCollection::ReleaseContentStream(CFileContent *pDatabase)
   {
      ASSERT(pDatabase);
      ASSERT(m_bInitialized);
      CloseDatabase(dbContent);
      delete pDatabase;
      return ERRORCODE_None;
   }

int CPMWCollection::NewCategoryStream(CCategoryDatabase **pNewStream)
   {
      return NewStream(dbCategory, (LPVOID *) pNewStream);
   }

int CPMWCollection::ReleaseCategoryStream(CCategoryDatabase *pDatabase)
   {
      ASSERT(pDatabase);
      ASSERT(m_bInitialized);
      CloseDatabase(dbCategory);
      delete pDatabase;
      return ERRORCODE_None;
   }

int CPMWCollection::NewThumbnailStream(CFileThumbnails **pNewStream)
   {
      return NewStream(dbThumbnail, (LPVOID *) pNewStream);
   }

int CPMWCollection::ReleaseThumbnailStream(CFileThumbnails *pDatabase)
   {
      ASSERT(pDatabase);
      ASSERT(m_bInitialized);
      CloseDatabase(dbThumbnail);
//      CloseDatabase(dbContent);
//      CloseDatabase(dbFreeList);
      delete pDatabase;
      return ERRORCODE_None;
   }

int CPMWCollection::NewItemInfoStream(CFileItems **pNewStream)
   {
      return NewStream(dbItemData, (LPVOID *) pNewStream);
   }

int CPMWCollection::ReleaseItemInfoStream(CFileItems *pDatabase)
   {
      ASSERT(pDatabase);
      ASSERT(m_bInitialized);
      CloseDatabase(dbItemData);
      delete pDatabase;
      return ERRORCODE_None;
   }

int CPMWCollection::NewKeywordStream(CFileKeywordDir **pNewStream)
   {
      return NewStream(dbKeyword, (LPVOID *) pNewStream);
   }

int CPMWCollection::ReleaseKeywordStream(CFileKeywordDir *pDatabase)
   {
      ASSERT(pDatabase);
      ASSERT(m_bInitialized);
      CloseDatabase(dbKeyword);
      delete pDatabase;
      return ERRORCODE_None;
   }

//************************************************************************
// Internal helpers

int CPMWCollection::NewStream(DatabaseTypes thisDatabase, LPVOID *pNewStream)
   {
      int nRetVal, nDirIndex, nDataIndex;
      
      *pNewStream = NULL;

      ASSERT(m_bInitialized);
      if(!m_bInitialized)
         return ERRORCODE_NotInitialized;

      nRetVal = OpenDatabase(thisDatabase);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Open;
      GetDBInterfaceInfo(thisDatabase, &nDirIndex, &nDataIndex);

      // Create stream after opening databases
      switch(thisDatabase)
         {
            case dbContent:
               *pNewStream = new CFileContent(
                  m_paCIndexFiles[nDirIndex],
                  m_paStorageFiles[nDataIndex]);
               if(*pNewStream)
                  {
                     // Only user collections have free lists
                     if(IsUserCollection())
                        (*(CFileContent **)pNewStream)->
                           SetFreeList(&m_FreeList);
                  }
               break;
            case dbCategory:
               *pNewStream = new CCategoryDatabase(
                  m_paStorageFiles[nDataIndex]);
               ((CCategoryDatabase *) *pNewStream)->Init(m_paCFiles[nDirIndex]);
               if(*pNewStream)
                  {
                     // Only user collections have free lists
                     if(IsUserCollection())
                        (*(CCategoryDatabase **)pNewStream)->SetFreeList(&m_FreeList);
                  }
               break;
            case dbThumbnail:
               *pNewStream = new CFileThumbnails(
                  m_paStorageFiles[nDirIndex],
                  m_paStorageFiles[nDataIndex]);
               if(*pNewStream)
                  {
                     // Only user collections have free lists
                     if(IsUserCollection())
                     (*(CFileThumbnails **)pNewStream)->SetFreeList(&m_FreeList);
                  }
               break;
            case dbItemData:
               *pNewStream = new CFileItems(
                  m_paStorageFiles[nDirIndex],
                  m_paStorageFiles[nDataIndex]);
               if(*pNewStream)
                  {
                     // Only user collections have free lists
                     if(IsUserCollection())
                        (*(CFileItems **)pNewStream)->SetFreeList(&m_FreeList);
                  }
               break;
            case dbKeyword:
               *pNewStream = new CFileKeywordDir(
                  m_paCIndexFiles[nDirIndex]);
               break;
         }
      return ERRORCODE_None;
   }

int CPMWCollection::OpenFreeList()
   {
      int         nRetVal;

      nRetVal = OpenDatabase(dbFreeList);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Open;
      m_FreeList.Init(m_paCIndexFiles[ciFreeList]);
      return nRetVal;
   }

int CPMWCollection::CloseFreeList()
   {
      return CloseDatabase(dbFreeList);
   }

// This method opens directory and data files
int CPMWCollection::OpenDatabase(DatabaseTypes databaseType)
   {
      CString           strDirFileName, strDataFileName;
      CString           strDirExt, strDataExt;
      FileTypes         ftDirFile, ftDataFile;
      int               nDirIndex, nDataIndex, nRetVal;
      CCIndexIndexList  indexList;

      strDirFileName = strDataFileName = m_strDBFileNamePrefix;

      // Check open reference count for database stream
      if(m_naDBOpenCount[databaseType])
         {
            m_naDBOpenCount[databaseType]++;
            return ERRORCODE_None;
         }

      GetDBInterfaceInfo(databaseType, &nDirIndex, &nDataIndex,
         &ftDirFile, &ftDataFile, &indexList);
      GetDBExtension(databaseType, strDirExt, strDataExt);

      strDirFileName  += strDirExt;
      strDataFileName += strDataExt;
		if (m_nOpenFlag & modeCreate)
		{
			// Create case. File does not exist yet.
			strDirFileName = m_pPathManager->ExpandPath(strDirFileName);
			if (nDataIndex>=0)
				strDataFileName = m_pPathManager->ExpandPath(strDataFileName);
		}
		else
		{
			// Non-create case. File must exist.
			strDirFileName  = m_pPathManager->LocatePath(strDirFileName);
			if (strDirFileName.IsEmpty())
				return ERRORCODE_Open;
			if (nDataIndex>=0)
			{
				strDataFileName = m_pPathManager->LocatePath(strDataFileName);
				if (strDataFileName.IsEmpty())
					return ERRORCODE_Open;
			}
		}


      // Open directory, Only directories have index lists for CIndex files
      nRetVal = OpenFile(strDirFileName,  ftDirFile, m_nOpenFlag, nDirIndex,
         TRUE, &indexList);
      if(nRetVal != ERRORCODE_None)
         return ERRORCODE_Open;

      // Keyword data file is not used
      if(nDataIndex>=0)
      {
         nRetVal = OpenFile(strDataFileName, ftDataFile, m_nOpenFlag, nDataIndex, FALSE);
         if(nRetVal != ERRORCODE_None)
         {
            CloseFile(ftDataFile, nDataIndex);
            return ERRORCODE_Open;
         }
      }
      m_naDBOpenCount[databaseType]++;
      return ERRORCODE_None;
   }

// This method closes directory and data files
int CPMWCollection::CloseDatabase(DatabaseTypes databaseType)
   {
      FileTypes      ftDirFile, ftDataFile;
      int            nDirIndex=-1, nDataIndex=-1, nRetVal;

      if(m_naDBOpenCount[databaseType] == 0)
         return ERRORCODE_None;

      // Check open reference count for database stream
      // If going to 0 then actually close files
      if(m_naDBOpenCount[databaseType] > 1)
         {
            m_naDBOpenCount[databaseType]--;
            return ERRORCODE_None;
         }
      m_naDBOpenCount[databaseType]--;

      GetDBInterfaceInfo(databaseType, &nDirIndex, &nDataIndex,
         &ftDirFile, &ftDataFile);

      if(nDirIndex >= 0)
         nRetVal = CloseFile(ftDirFile, nDirIndex);
      if(nDataIndex >= 0)
         nRetVal = CloseFile(ftDataFile, nDataIndex);

      return ERRORCODE_None;
   }

int CPMWCollection::GetDBInterfaceInfo(DatabaseTypes databaseType,
   int *pnDirIndex, int *pnDataIndex,
   FileTypes *pftDirFile, FileTypes *pftDataFile,
   CCIndexIndexList *pIndexList)
   {
      FileTypes                  ftDirFile, ftDataFile;
      static CCIndexIndexList    s_indexList;

      ASSERT(pnDirIndex);
      ASSERT(pnDataIndex);

      // Default to StorageFile unless specified otherwise
      ftDirFile = ftDataFile = ftStorageFile;
      *pnDirIndex = *pnDataIndex   = -1;
      switch(databaseType)
         {
            case dbContent:
               ftDirFile    = ftCIndex;
               *pnDirIndex      = ciContentDir;
               // User collections store content external to database
               if(!IsUserCollection())
                  *pnDataIndex     = sfContentData;
               break;
            case dbCategory:
               ftDirFile      = ftCFile;
               *pnDirIndex      = cfCategoryDir;
               *pnDataIndex     = sfCategoryData;
               break;
            case dbThumbnail:
               *pnDirIndex      = sfThumbnailDir;
               *pnDataIndex     = sfThumbnailData;
               break;
            case dbItemData:
               *pnDirIndex      = sfItemInfoDir;
               *pnDataIndex     = sfItemInfoData;
               break;
            case dbKeyword:
               ftDirFile      = ftCIndex;
               *pnDirIndex    = ciKeywordDir;
               if(pIndexList)
                  CFileKeywordDir::GetIndexList(pIndexList);

               // Data is in directory file
               break;
            case dbFreeList:
               ftDirFile      = ftCIndex;
               *pnDirIndex    = ciFreeList;
               if(pIndexList)
                  CFreeLists::GetIndexList(pIndexList);
               // No Data file for this database 
               break;
            default:
               ASSERT(0);  // Called with invalid type
               return ERRORCODE_BadParameter;
               break;
         }
      if(pftDirFile)
         *pftDirFile  = ftDirFile;
      if(pftDataFile)
         *pftDataFile = ftDataFile;
      return ERRORCODE_None;
   }

int CPMWCollection::GetDBExtension(DatabaseTypes databaseType,
   CString &strDirExt, CString &strDataExt)
   {
      const char     *pDirExt, *pDataExt;

      pDirExt = pDataExt = "";
      strDirExt.Empty();
      strDataExt.Empty();
      switch(databaseType)
         {
            case dbContent:
               pDirExt  = CONTENTDIR_EXT;
               pDataExt = CONTENTDATA_EXT;
               break;
            case dbCategory:
               pDirExt  = CATEGORYDIR_EXT;
               pDataExt = CATEGORYDATA_EXT;
               break;
            case dbThumbnail:
               pDirExt  = THUMBNAILDIR_EXT;
               pDataExt = THUMBNAILDATA_EXT;
               break;
            case dbItemData:
               pDirExt  = ITEMSDIR_EXT;
               pDataExt = ITEMSDATA_EXT;
               break;
            case dbKeyword:
               pDirExt  = KEYWORDSDIR_EXT;
               break;
            case dbFreeList:
               pDirExt  = FREELIST_EXT;
               break;
            default:
               ASSERT(1);  // Called with invalid type
               return ERRORCODE_BadParameter;
               break;
         }
      strDirExt = pDirExt;
      strDataExt = pDataExt;
      return ERRORCODE_None;
   }

int CPMWCollection::OpenFile(CString strFileName, FileTypes thisFileType,
   UINT openFlags, int nIndex, BOOL bCacheFile, CCIndexIndexList *pIndexList)
   {
      int                     nRetVal;
      ST_DEV_POSITION         stdevPosLength;
      ST_DEV_IO_SIZE          stdevBufferSize;
      UINT                    openFlag;

      ASSERT(!strFileName.IsEmpty());
      if(thisFileType < 0 || strFileName.IsEmpty() || nIndex < 0)
         return ERRORCODE_BadParameter;

      // Delete existing file
      if( m_nOpenFlag & modeCreate )
         {
            remove(strFileName);
            // Make sure read/write access flag is set for CFile
            m_nOpenFlag |= m_nOpenFlag & modeReadWrite;
         }

      nRetVal = CloseFile(thisFileType, nIndex);
      switch(thisFileType)
         {
            case ftCIndex:
               m_paCIndexFiles[nIndex] = new CCIndexFile;
               openFlag = 0;
               if( m_nOpenFlag & modeCreate )
                  openFlag |= CCIndexFile::modeCreate;
               if( m_nOpenFlag & modeReadWrite )
                  openFlag |= (CCIndexFile::modeReadWrite | CCIndexFile::shareExclusive);
               else
                  openFlag |= CCIndexFile::modeRead;

               nRetVal = m_paCIndexFiles[nIndex]->Open(strFileName,
                  pIndexList, openFlag);
               if(nRetVal != CCIndexFile::success)
               {
#if 0
                  CString csMessage;
                  csMessage.Format("Error opening %s: %d",
                                   (LPCSTR)strFileName,
                                   nRetVal);
                  AfxMessageBox(csMessage);
#endif
                  return ERRORCODE_Open;
               }
               break;
            case ftCFile:
            {
               CFileException e;
               m_paCFiles[nIndex] = new CFile;
               openFlag = m_nOpenFlag;
               if (!(m_nOpenFlag & (CFile::modeWrite|CFile::modeReadWrite|CFile::modeCreate)))
               {
                  // Read-only files only deny write.
                  openFlag |= CFile::shareDenyWrite;
               }

               nRetVal = m_paCFiles[nIndex]->Open(strFileName, openFlag, &e);
               if(nRetVal == 0)
               {
#if 0
                  CString csMessage;
                  csMessage.Format("Error opening %s: %d (%ld)",
                                   (LPCSTR)strFileName,
                                   e.m_cause,
                                   e.m_lOsError);
                  AfxMessageBox(csMessage);
#endif
                  return ERRORCODE_Open;
               }
               break;
            }
            case ftStorageFile:
               if( m_nOpenFlag & modeCreate || m_nOpenFlag & modeReadWrite )
                  {
                     m_paStorageFiles[nIndex] = new StorageFile;
                     m_paStorageFiles[nIndex]->set_name(strFileName);
                     if(m_nOpenFlag & modeCreate)
								{
									nRetVal = m_paStorageFiles[nIndex]->initialize();
									if (nRetVal != ERRORCODE_None)
										return nRetVal;
								}
                  }
               else
                  {
                     m_paStorageFiles[nIndex] = new ReadOnlyFile;
                     m_paStorageFiles[nIndex]->set_name(strFileName);
                     if(bCacheFile)
                        {
                           m_paStorageFiles[nIndex]->length(&stdevPosLength);
                           // Buffer entire file
                           if(stdevPosLength > 64000)
                              stdevBufferSize = 64000;
                           else
                              stdevBufferSize = (ST_DEV_IO_SIZE) stdevPosLength;
                        }
                     else
                        stdevBufferSize = 4096; // Do some caching
                     ((ReadOnlyFile *)(m_paStorageFiles[nIndex]))->set_read_buffer(stdevBufferSize);

							// Make sure we can access the file.
							nRetVal = m_paStorageFiles[nIndex]->seek(0, ST_DEV_SEEK_SET);
							if (nRetVal != ERRORCODE_None)
								 return nRetVal;
                  }
               // Turn on exception handling
               if(m_paStorageFiles[nIndex])
                  m_paStorageFiles[nIndex]->UseExceptions(TRUE);
               break;
            default:
               ASSERT(0);
               return ERRORCODE_BadParameter;
         }
      return ERRORCODE_None;
   }

int CPMWCollection::CloseFile(FileTypes thisFileType, int nThisFile)
   {
      int nRetVal;

      switch (thisFileType)
         {
            case ftCIndex:
               if(m_paCIndexFiles[nThisFile] == NULL)
                  return ERRORCODE_None;
               nRetVal = m_paCIndexFiles[nThisFile]->Close();
               delete m_paCIndexFiles[nThisFile];
               m_paCIndexFiles[nThisFile] = NULL;
               break;
            case ftCFile:
               if(m_paCFiles[nThisFile] == NULL)
                  return ERRORCODE_None;
               if(m_paCFiles[nThisFile]->m_hFile != (UINT)CFile::hFileNull)
                  m_paCFiles[nThisFile]->Close();
               delete m_paCFiles[nThisFile];
               m_paCFiles[nThisFile] = NULL;
               break;
            case ftStorageFile:
               if(m_paStorageFiles[nThisFile] == NULL)
                  return ERRORCODE_None;
               nRetVal = m_paStorageFiles[nThisFile]->flush();
               delete m_paStorageFiles[nThisFile];
               m_paStorageFiles[nThisFile] = NULL;
               break;
         }
      return ERRORCODE_None;
   }

//
// Try to use a cached thumbnail.
//

BOOL CPMWCollection::UseCachedThumbnail(const CPMWCollectionData* pClientData, CThumbnailDataEntry* pThumbnailData)
{
	// Bail out early if no thumbnail directory.
	if (m_strThumbnailDir.IsEmpty() || pClientData->m_strRelativeFileName.IsEmpty())
		return FALSE;

	// Create a thumbnail file name.
	CString csThumbnailFile = GetThumbnailFile(pClientData);
	TRACE("Use ThumbnailFile: %s\n", (LPCSTR)csThumbnailFile);

	// Get the times for the thumbnail and source files.
	// If it doesn't exist, we'll get an error (and that is meaningful).
	CTimeStamp ctsThumbnail;
	CTimeStamp ctsSource;

	BOOL fSuccess = FALSE;
	if (ctsThumbnail.SetFileModify(csThumbnailFile)
			&& ctsSource.SetFileModify(pClientData->m_strFullFileName)
			&& ctsThumbnail >= ctsSource)
	{
		// Open the file so we can read the data.
		CFile cfThumbnail;
		if (cfThumbnail.Open(csThumbnailFile, CFile::modeRead | CFile::shareDenyWrite))
		{
			// Read the header.
			// First, read the thumbnail header.
			cfThumbnail.Read(pThumbnailData->GetHeader(), pThumbnailData->GetHeaderSize());

			// Allocate the data portion.
			long lSize = pThumbnailData->GetItemSize();
			LPVOID pData = pThumbnailData->AllocData(lSize);
			if (pData != NULL)
			{
				// Then write out the data.
				cfThumbnail.Read(pData, lSize);
				fSuccess = TRUE;
			}
		}
	}
	return fSuccess;
}

//
// Cache a thumbnail.
//

void CPMWCollection::CacheThumbnail(const CPMWCollectionData* pClientData, CThumbnailDataEntry* pThumbnailData)
{
	// Bail out early if no thumbnail directory.
	if (m_strThumbnailDir.IsEmpty() || pClientData->m_strRelativeFileName.IsEmpty())
		return;

	// Create a thumbnail file name.
	CString csThumbnailFile = GetThumbnailFile(pClientData);
	TRACE("Cache Thumbnail File: %s\n", (LPCSTR)csThumbnailFile);

	// Get the directory for this file.
	CString csDir;
	Util::SplitPath(csThumbnailFile, &csDir, NULL);
	Util::MakeDirectory(csDir);

	// Create the thumbnail file with the generated file name.
	CFile cfThumbnail;
	if (cfThumbnail.Open(csThumbnailFile, CFile::modeCreate | CFile::modeWrite))
	{
		TRY
		{
			// First, write out the thumbnail header.
			cfThumbnail.Write(pThumbnailData->GetHeader(), pThumbnailData->GetHeaderSize());
			// Then write out the data.
			cfThumbnail.Write(pThumbnailData->GetData(), pThumbnailData->GetItemSize());
		}
		CATCH_ALL(e)
		{
			// We got an error. Try to delete the bad file.
			TRY
			{
				cfThumbnail.Close();
				CFile::Remove(csThumbnailFile);
			}
			END_TRY
		}
		END_CATCH_ALL
	}
}

CString CPMWCollection::GetThumbnailFile(const CPMWCollectionData* pClientData)
{
	// The thumbnail file is the thumbnail directory followed by the
	// relative build path for the item (the spreadsheet directory and
	// file name columns). The extension is ".Txy", where the original extension
	// is ".xyz" (e.g. a ".CGM" file would have thumbnail extension ".TCG").

	CString csThumbnailFile;
	// Combine the thumbnail directory with the relative path. By convention,
	// the relative path currently has a leading slash. The code handles this
	// case but does not assume it.

	ASSERT(!m_strThumbnailDir.IsEmpty());
	ASSERT(!pClientData->m_strRelativeFileName.IsEmpty());
	csThumbnailFile = m_strThumbnailDir;
	if (pClientData->m_strRelativeFileName[0] == '\\')
	{
		Util::RemoveBackslashFromPath(csThumbnailFile);
	}
	else
	{
		Util::AppendBackslashToPath(csThumbnailFile);
	}
	csThumbnailFile += pClientData->m_strRelativeFileName;

	// Change the extension. ".xyz" becomes ".Txy".
	// If there isn't a three-character extension, this doesn't do anything.

	CString csDrive;
	CString csDirectory;
	CString csFilename;
	CString csExtension;
	Util::SplitPath(csThumbnailFile, &csDrive, &csDirectory, &csFilename, &csExtension);
	if (csExtension.GetLength() == 4)
	{
		csExtension = ".T" + csExtension.Mid(1, 2);
		Util::ConstructPath(csThumbnailFile, csDrive, csDirectory, csFilename, csExtension);
	}

	return csThumbnailFile;
}

