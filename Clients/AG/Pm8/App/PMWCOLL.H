/*
// $Workfile: PMWCOLL.H $
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
// $Log: /PM8/App/PMWCOLL.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 35    2/24/99 1:31p Gbeddow
// fixed bug in regular (non-online) collection building that didn't set
// the thumbnail extents properly
// 
// 34    2/09/99 1:48p Gbeddow
// Support in collection building for creating online collection GIF and
// JPEG thumbnail files, and related changes, per spec from Alexei
// 
// 33    1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 32    12/18/98 4:52p Gbeddow
// support for writing grapic types into art collection files
// 
// 31    10/21/98 4:13p Jayn
// Product code mappings.
// 
// 30    10/19/98 10:50a Jayn
// More COM restructuring changes.
// 
// 29    9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 28    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 27    8/12/98 10:29a Dennis
// Added support for Hidden Art collections
// 
// 26    7/15/98 6:05p Hforman
// added functions to convert 4.0 user projects to new category structure
// 
// 25    7/08/98 8:20p Hforman
// add "Copyright Title" handling
// 
// 24    7/06/98 5:45p Rlovejoy
// Added 'ProductCode' and 'ProjectTypes' variables and access.
// 
// 23    6/15/98 6:55p Hforman
// add "Match All" functionality for keyword searching
// 
// 22    6/08/98 4:08p Rlovejoy
// Accomodate various "copyright" placement types.
// 
// 21    6/01/98 5:45p Rlovejoy
// Added HiddenProjectsType; removed IsHidden().
// 
// 20    5/25/98 4:43p Fredf
// Support for building online collections.
// 
// 19    5/21/98 10:13a Rlovejoy
// Added 'Hidden' attribute.
// 
// 18    5/20/98 6:17p Hforman
// added super collection functionality
// 
// 17    5/19/98 3:04p Hforman
// add GetCategoryDB(); remove default param "All" from Search()
// 
// 16    5/12/98 4:17p Hforman
// added ReloadCategoryStream()
// 
// 15    4/30/98 7:01p Hforman
// 
// 14    4/28/98 10:34a Dennis
// Added Order Form Filename entry
// 
// 13    4/24/98 7:41a Jayn
// Builder changes to support "inclusion column" and "external file".
// 
// 12    4/23/98 11:37a Rlovejoy
// Added support for copyright objects.
// 
// 11    4/20/98 9:09a Jayn
// Cached thumbnail support.
// 
// 10    4/15/98 4:37p Dennis
// Discovered PMGOBJ.H is required for PrintMaster build (by refresh
// extents).
// 
// 9     4/15/98 4:33p Dennis
// Changes so code can be shared between PrintMaster and Collection
// Utility
// 
// 8     4/14/98 10:15a Dennis
// a) Moved NEW_THUMBNAILS and NEWCOMPRESSED_CONTENT defines to project
// file.
// b) Added content version support via. INI file
// c) Added external thumbnail support (enhanced UpdateThumbnail()).
// d) Removed Media and Style.
// e) Added Hide Item support.
// f) Added GetCategoryDataItem().
// 
// 7     4/13/98 4:38p Jayn
// 
// 6     4/13/98 2:35p Hforman
// add category param to ImportItem()
// 
// 5     4/08/98 6:44p Hforman
// interim checkin - more later
// 
// 4     3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 3     3/19/98 1:00p Hforman
// changed UNSPECIFIED_COLOR  to ((COLORREF)-1)
// 
// 2     1/19/98 1:21p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.1   21 Oct 1997 15:35:58   hal
// add REFRESH_EXTENT to CPMWDocumentPreviewData to allow interruptable drawin
// 
//    Rev 1.0   14 Aug 1997 15:24:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:12   Fred
// Initial revision.
// 
//    Rev 1.34   10 Jul 1997 13:56:42   dennisp
// Added some #ifdef's NEW_THUMBNAILS for builder utility
// 
*/

#ifndef _PMWCOLL_INC
#define _PMWCOLL_INC

#include "ipathmgr.h"		// IPathManager
#include "icolldrw.h"		// ICollectionDraw

#include "collsrch.h"
#include "collctg.h"
#include "collfile.h"
#include "cindex.h"
#include "file.h"
#include "freelist.h"
#include "inifile.h"
#include "sortarry.h"

#define UNSPECIFIED_COLOR  ((COLORREF)-1)

class CPMWCollectionData
   {
   public:
   // Construction
      CPMWCollectionData();

   // Operations
      void                 Empty();

   public:
// FF: DWORD m_dwContentTimeStamp was phased out starting in PM 5.0
// This data will be a time_t in collections built with PMG 4.0
// and zero in collections built with 5.0
//		DWORD                m_dwContentTimeStamp;
		DWORD                m_dwUnused;
      CString              m_strContentFileName;
      CString              m_strFriendlyName;
      CString              m_strKeywords;
      CString              m_strThumbnailFileName;
      CString              m_strRelativeFileName;
      CString              m_strFullFileName;
   // Declared in collfile.h
      CContentDataEntry    m_ContentData;
   // Declared in collfile.h
      CCategoryData        m_CategoryData;
		CString					m_strOnlineThumbnailDir; 
		BOOL						m_fAddSubdirectory;
		DWORD						m_dwThumbnailExtents;
	};

class CPMWLicensedInfo
	{
	public:
		CPMWLicensedInfo();
		virtual ~CPMWLicensedInfo();

		BOOL IsLicensed(DWORD dwItemNumber);
		void AddUnlicensedRange(DWORD dwRangeStart, DWORD dwRangeEnd);
		BOOL HasUnlicensedItems(void) const
			{ return m_cdaUnlicensed.GetSize() > 0; }

	protected:
		CDWordArray m_cdaUnlicensed;
	};

// Copyright placement
enum CopyrightVariants
{
	COPYRIGHT_Default,
	COPYRIGHT_HalfFoldT,
	COPYRIGHT_QuarterFoldT,
	COPYRIGHT_HalfFoldW,
	COPYRIGHT_QuarterFoldW,

	COPYRIGHT_NVariants
};
struct SFracBound
{
	float left, top, right, bottom;
};	
struct SCopyright
{
	// Contains info on the copyright placement object
	CString csCopyrightName;
	SFracBound rctAbstractBound;
};

class CPMWCollection
   {
   public:
   // Flag values
	   enum OpenFlags
         {
		      modeRead =          0x0000,
		      modeReadWrite =     0x0002,
		      modeCreate =        0x1000
		   };
      enum CollectionTypes
         {
				typeListStart,
            typeProjects = typeListStart,		// First type
            typeArt,
            typeSentiments,
				typeHiddenProjects,
				typeHiddenArt,
				typeListEnd								// Last type.
         };
      enum CollectionSubTypes
         {
				subTypeNone = -1,
				subTypeListStart = 0,
				subTypeArt = subTypeListStart,
				subTypeWebArt,
				subTypeListEnd
         };
      enum CollectionOrigin
         {
            originGeneric,
            originUser
         };
		enum StreamTypes
			{
				streamAll = -1,
				streamContent = 1,
				streamCategory = 2,
				streamThumbnail = 4,
				streamItem = 8,
				streamKeyword = 16,
			};
      enum Version
         {
            verOriginal,
            verNoMediaStyle,   // No Media Style with Custom Flags
				verGraphicTypes    // verNoMediaStyle + graphic types
         };

//protected:
      CPMWCollection();
      CPMWCollection(const CString &strINIFileName, const CString &strSection,
         CollectionOrigin origin, UINT openFlag = modeRead, const CString &strCollectionPath=CString(""));
      virtual ~CPMWCollection();

      int               Init(IPathManager* pPathManager, ICollectionDraw* pCollectionDraw);
      int               Init(IPathManager* pPathManager, ICollectionDraw* pCollectionDraw, const CString &strINIFileName, const CString &strSection,
         CollectionOrigin origin, UINT openFlag = modeRead, const CString &strCollectionPath=CString(""), BOOL bOnlineBuild = FALSE);

   // GetType() returns what type of collection this is
      const CollectionTypes   GetType() const;

   // GetSubType() returns what subtype of collection this is
      const CollectionSubTypes   GetSubType() const;

   // GetVersion() returns the version number of the collection
      Version GetVersion() const
         { return m_enVersion; }

   // GetID() returns unique ID associated with this collection object
      const BYTE        GetID()
         {  return m_byID; }

   // Content database operations
      int               NewContentStream(CFileContent **pDatabase);
      int               ReleaseContentStream(CFileContent *pDatabase);
   // Category database operations
      int               NewCategoryStream(CCategoryDatabase **pDatabase);
      int               ReleaseCategoryStream(CCategoryDatabase *pDatabase);
   // Thumbnail database operations
      int               NewThumbnailStream(CFileThumbnails **pDatabase);
      int               ReleaseThumbnailStream(CFileThumbnails *pDatabase);
   // Items info database operations
      int               NewItemInfoStream(CFileItems **pDatabase);
      int               ReleaseItemInfoStream(CFileItems *pDatabase);
   // Keyword database operations
      int               NewKeywordStream(CFileKeywordDir **pDatabase);
      int               ReleaseKeywordStream(CFileKeywordDir *pDatabase);

      int               Add(CPMWCollectionData *pCollectionData);

    // Delete removes item from this collection
      int               Delete(DWORD dwItemToDelete);

   // Opens free list database if it isn't already open
      int               OpenFreeList();
      int               CloseFreeList();

      int               OpenStreams(int nStreams = streamAll);
      int               CloseAllStreams(BOOL bForceClosed=FALSE);
      BOOL              IsOpen(int nStreams = streamAll)
								{
									return (m_nStreamsOpenCount > 0
												&& (m_nStreamsOpen & nStreams) == nStreams);
								}

		CCategoryDatabase*	GetCategoryDB()	{ return m_pDBCategory; }

   // Get operations for .INI file strings
		const CString &   GetFriendlyName() const;
		const CString &   GetIconName() const;
		const CString &   GetCDDir(void) const;
      const CString &   GetDBFileNamePrefix() const;
            LPCSTR      GetIniFile() const;
      const CString &   GetSection() const;
		const CString &   GetCollectionPath(void) const;
		const CString &   GetBaseName(void) const;
		const CString &   GetThumbnailDir(void) const;
		const CString &   GetOnlineThumbnailDir(void) const;
		const CString &   GetSuperCollection(void) const;
		const CString &   GetCopyrightTitle(void) const;
		const CString &   GetInclusionField(void) const;
		BOOL					GetAddSubdirectory(void) const;
		BOOL					HasOrderForm(void) const;
		int					GetSortIndex(void) const;
		BOOL					ExtensionToPath(LPCSTR pszExtension, CString& csPath) const;
		const CString &	GetProductCode(void) const;
		DWORD					GetProjectTypes(void) const;
		const SCopyright& GetCopyright(enum CopyrightVariants index) const;
      const CString &   GetOrderFileName() const;

   // SetProductFlags() is used to specify what type of content should
   // be made available from the Search process to all collections.
   // Note that this is a static member.
   //
   // Product Flags can be one or more of
   // the ProductFlags enumerated type defined in CCategoryDataItem
      static void       SetProductFlags(USHORT usProductFlags)
         {
            sm_shProductFlags = usProductFlags;
         }
      static USHORT     GetProductFlags()
         {
            return sm_shProductFlags;
         }

   // Search() collection
   // The default parameters for Tone indicate
   // that search will not be constrained to an attribute
      int               Search(CPMWCollectionSearchContext& Context);

		void GetLicensedInfo(CPMWLicensedInfo& Info);

	// Import an item. This only works on a user collection.
#ifdef NEW_THUMBNAILS
		int ImportItem(LPCSTR szFileName,
							LPCSTR szFriendlyName = NULL,
							LPCSTR szKeywords = NULL,
							LPCSTR szUserCategory = NULL,
							BOOL fThumbnailOnly = TRUE);
#endif

   // Returns item number in pdwItemNumber associated with content file
   // Return ERRORCODE_Fail if item wasn't found (doesn't exist)
      int               FindItem(const CString &strContentFileName,
                           DWORD *pdwItemNumber);

      CString           GetProductCode(DWORD dwItem);
      CString           GetCategory(DWORD dwItem);
      ERRORCODE         GetCategoryDataItem(DWORD dwItem, CCategoryDataItem *pCDataItem);

	// rename a category -- User collections only	
		ERRORCODE			RenameCategory(CString& strOld, CString& strNew);

#ifndef NO_PMWDOCS
		BOOL					Is40UserProjects();
		int					Convert40UserProjectCategories();
#endif

   // Update currently rebuilds thumbnail and updates keywords and friendly name
      int               Update(DWORD dwItemToUpdate, CPMWCollectionData *pCollectionData);

   // UpdateItemInfo updates keywords and friendly name
   // You only need to fill in Friendly Name and Keyword members of
   // PMWCollectionData
      int               UpdateItemInfo(DWORD dwItemToUpdate,
                           CPMWCollectionData *pCollectionData);

		int					GetItemInfo(LPCSTR pszName,
												DWORD& dwItemNumber,
												CString& csDescription,
												CString& csKeywords);

   // UpdateItemInfo updates thumbnail.
   // You only need to fill in file name.
      int               UpdateThumbnail(DWORD dwItemNum,
                           CPMWCollectionData *pCollectionData);

   // GetKeywords retrieves all keywords associated with an item
      int               GetKeywords(DWORD dwItem, CString &strKeywords);

      BOOL              IsUserCollection()  {  return m_bUserCollection; }


// Protected data members
   protected:

		// Increasing this requires architecture changes (we use 7 bits within
		// a score byte, plus the high bit).
		enum
			{
				MaxKeywords = 7
			};

      enum DatabaseTypes
         {
            dbContent,
            dbCategory,
            dbThumbnail,
            dbItemData,
            dbKeyword,
            dbFreeList, 
            dbListEnd
         };
      enum StorageFiles
         {
            sfContentData,
            sfCategoryData,
            sfThumbnailDir,
            sfThumbnailData,
            sfItemInfoDir,
            sfItemInfoData, 
            sfListEnd
         };
      enum CIndexFiles
         {
            ciContentDir,
            ciKeywordDir,
            ciFreeList, 
            ciListEnd
         };
      enum CFileFiles
         {
            cfCategoryDir,
            cfListEnd
         };
      enum FileTypes
         {
            ftCFile, 
            ftStorageFile,
            ftCIndex
         };
      BOOL              m_bInitialized;
      BOOL              m_bUserCollection;
      BYTE              m_byID;
      int               m_naDBOpenCount[dbListEnd], m_nStreamsOpenCount;
		int					m_nStreamsOpen;		// Bit-field
      int               m_nOpenFlag;
      StorageFile *     m_paStorageFiles[sfListEnd];
      CCIndexFile *     m_paCIndexFiles[ciListEnd];
      CFile *           m_paCFiles[cfListEnd];
      CString           m_strINIFileName, m_strSection;
      CString           m_strFriendlyName, m_strIcon, m_strCDDir;
      CString           m_strType, m_strSubType, m_strBaseName;
		CString				m_strSuperCollection;
		CString				m_strCopyrightTitle;
      CString           m_strDBFileNamePrefix;
		CString				m_strCollectionPath;
		BOOL					m_bOnlineBuild;
		CString				m_strThumbnailDir;
		CString				m_strOnlineThumbnailDir;
		DWORD					m_dwThumbnailExtents;
		CString				m_strInclusionField;		// For builds.
      CString           m_strOrderFileName;
		CString				m_strProductCode;
		DWORD					m_dwProjectTypes;
		SCopyright			m_Copyright[COPYRIGHT_NVariants];
      CIniFile          m_IniFile;
      CollectionTypes   m_Type;
      CollectionSubTypes m_SubType;
      CFreeLists        m_FreeList;
      CollectionOrigin  m_origin;
		BOOL					m_fAddSubdirectory;
		BOOL					m_fHasOrderForm;
		int					m_nSortIndex;
      Version           m_enVersion;          // Version of database
		IPathManager*		m_pPathManager;
		ICollectionDraw*	m_pCollectionDraw;

   // Databases used internally for high level operations such as Add()
      CFileContent               *m_pDBContent;
      CCategoryDatabase          *m_pDBCategory;
      CFileThumbnails            *m_pDBThumbnails;
      CFileItems                 *m_pDBItemInfo;
      CFileKeywordDir            *m_pDBKeywords;

   // Data used for high level operations
      CItemsDataEntry            m_itemData;
      DWORD                      m_dwCurItemNumber;
      CPMWCollectionData         *m_pClientCD;

   // Static members
      static BOOL                sm_bReadStopList;
   	static CMlsStringArray     sm_csaStopList;
      static BYTE                sm_byID;
      static USHORT              sm_shProductFlags;
                                 
	// Path lookup by extension.
		CMapStringToString m_mapPaths;

	// Product code mapping.
	// Key is code in collection data. Value is code in LicenseData.
	// This came into being because the Online server uses a JDE product
	// code (> 6 characters), but the collection format only supports 6
	// characters (old MLS format). So, we map.
	// Typically, the product codes burned into the online collections will
	// all be the same, with the differentiation happening via this map to
	// the real server product code.
		CMapStringToString m_mapProductCode;

// Internal helpers
   protected:
      void              InitMembers(const CString &strINIFileName,
												const CString &strSection,
												CollectionOrigin origin,
												UINT OpenFlag,
												const CString &strCollectionPath,
												BOOL bOnlineBuild);
      int               NewStream(DatabaseTypes thisDatabase, LPVOID *pNewStream);
      int               OpenDatabase(DatabaseTypes databaseType);
      int               CloseDatabase(DatabaseTypes databaseType);
      int               OpenFile(CString strFileName, FileTypes filetype,
											UINT openFlags, int nIndex, BOOL bCacheFile=FALSE,
											CCIndexIndexList *pIndexList=NULL);
      int               CloseFile(FileTypes thisFileType, int nThisFile);
      int               GetDBInterfaceInfo(DatabaseTypes databaseType,
                           int *pnDirIndex, int *pnDataIndex,
                           FileTypes *pftDirFile=NULL, FileTypes *pftDataFile=NULL,
                           CCIndexIndexList *pIndexList=NULL);
      int               GetDBExtension(DatabaseTypes databaseType,
                           CString &strDirExt, CString &strDataExt);
      int               ReadStopList();
      BYTE              GetBitCount(BYTE byValueToCount);
   // DeleteKeywords removes item association with keyword IDs in 
   // CItemsDataEntry parameter.  A call to retrieve the CItemsDataEntry
   // is required prior to calling DeleteKeywords.
      int               DeleteKeywords(DWORD dwItemToDelete, CItemsDataEntry &itemInfoEntry);
	// Thumbnail operations
	// Where'd they all go???

   // High Level operations
      int               AddKeywords();
      int               AddItemInfo();
      int               AddCategoryItem();
      int               AddContent();
   // UpdateThumbnail will add the generated thumbnail if dwItemNum == 0
      int               UpdateThumbnail(DWORD dwItemNum=0);
		int					CreateOnlineThumbnailFile( CThumbnailDataEntry* ThumbnailData );
      void              AllocateResultArray(USHORT usNumItems);
   // Static operations
      static BYTE       GetNextID()
         { return sm_byID++; }

		BOOL UseCachedThumbnail(const CPMWCollectionData* pClientData, CThumbnailDataEntry* pThumbnailData);
		void CacheThumbnail(const CPMWCollectionData* pClientData, CThumbnailDataEntry* pThumbnailData);
		CString GetThumbnailFile(const CPMWCollectionData* pClientData);

		CString MapProductCode(const CString& csProductCode);

#ifdef NEW_COMPRESSED_CONTENT
	public:
	// Content compression operations
		BOOL					DataIsCompressable(LPCSTR pszFile);
#endif
   };

// Inline Section

// CPMWCollection inline members

inline const CPMWCollection::CollectionTypes CPMWCollection::GetType() const
   { return m_Type; }
inline const CPMWCollection::CollectionSubTypes CPMWCollection::GetSubType() const
   { return m_SubType; }
inline const CString &   CPMWCollection::GetFriendlyName() const
   { return m_strFriendlyName; }
inline const CString &   CPMWCollection::GetIconName() const
   { return m_strIcon; }
inline const CString&    CPMWCollection::GetCDDir(void) const
   { return m_strCDDir; }
inline const CString&    CPMWCollection::GetDBFileNamePrefix(void) const
   { return m_strDBFileNamePrefix; }
inline LPCSTR CPMWCollection::GetIniFile() const
   { return m_IniFile.Name(); }
inline const CString&    CPMWCollection::GetSection() const
   { return m_strSection; }
inline const CString&    CPMWCollection::GetBaseName(void) const
   { return m_strBaseName; }
inline const CString&    CPMWCollection::GetCollectionPath(void) const
   { return m_strCollectionPath; }
inline const CString&    CPMWCollection::GetThumbnailDir(void) const
   { return m_strThumbnailDir; }
inline const CString&    CPMWCollection::GetOnlineThumbnailDir(void) const
   { return m_strOnlineThumbnailDir; }
inline const CString&    CPMWCollection::GetSuperCollection(void) const
   { return m_strSuperCollection; }
inline const CString&    CPMWCollection::GetCopyrightTitle(void) const
   { return m_strCopyrightTitle; }
inline const CString&    CPMWCollection::GetInclusionField(void) const
   { return m_strInclusionField; }
inline BOOL					 CPMWCollection::GetAddSubdirectory(void) const
	{ return m_fAddSubdirectory; }
inline BOOL					 CPMWCollection::HasOrderForm(void) const
	{ return m_fHasOrderForm; }
inline int					 CPMWCollection::GetSortIndex(void) const
	{ return m_nSortIndex; }
// We assume this product code does not need mapping.
inline const CString&	 CPMWCollection::GetProductCode(void) const
	{ return m_strProductCode; }
inline DWORD				 CPMWCollection::GetProjectTypes(void) const
	{ return m_dwProjectTypes; }

inline const SCopyright& CPMWCollection::GetCopyright(enum CopyrightVariants index) const
	{ return m_Copyright[index]; }

inline CPMWCollection::CollectionTypes operator++(CPMWCollection::CollectionTypes& t, int)
   { return t = (CPMWCollection::CollectionTypes)(t+1); }
//inline CPMWCollection::StorageFiles operator++(CPMWCollection::StorageFiles &refType,int)
//   {
//      return refType = (CPMWCollection::StorageFiles)(refType+1);
//   }
//inline CPMWCollection::CIndexFiles operator++(CPMWCollection::CIndexFiles &refType,int)
//   {
//      return refType = (CPMWCollection::CIndexFiles)(refType+1);
//   }


// CPMWCollection::CollectionSubTypes inline members
inline CPMWCollection::CollectionSubTypes operator++(CPMWCollection::CollectionSubTypes& t, int)
   { return t = (CPMWCollection::CollectionSubTypes)(t+1); }

#endif // End of _PMWCOLL_INC


