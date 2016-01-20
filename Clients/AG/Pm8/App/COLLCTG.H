//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/COLLCTG.H 2     3/08/99 1:13p Mtaber $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1996-1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/COLLCTG.H $
// 
// 2     3/08/99 1:13p Mtaber
// Added product flags for Silver & Gold configurations
// Renamed PubSuite -> Silver
// Renamed Deluxe -> Gold
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 14    1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 13    1/25/99 1:20p Gbeddow
// support for the latest spec'd list of graphic types and initial support
// for a new web art browser dialog
// 
// 12    1/05/99 5:31p Gbeddow
// art gallery (graphic browser) support for Print Shop-style graphic type
// filtering
// 
// 11    12/18/98 4:52p Gbeddow
// support for writing grapic types into art collection files
// 
// 10    8/10/98 3:06p Hforman
// add CropCopyright code
// 
// 9     6/06/98 2:16p Fredf
// Added platinum and premier.
// NOTE THAT THE DEFINITION FOR PLATINUM IS DIFFERENT THAN IN PRINTMASTER
// 4.0
// 
// 8     5/19/98 3:09p Hforman
// add a param to CCategoryDatabase::Add()
// 
// 7     5/12/98 4:18p Hforman
// lots more add/delete functionality and fixes
// 
// 6     5/06/98 6:10p Hforman
// added Delete() functionality; merged Dennis' changes for FindNext()
// 
// 5     4/30/98 7:02p Hforman
// Major modifications for User Categories
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef _COLLCTG_INC
#define _COLLCTG_INC

#include "carray.h"
#include "charray.h"
#include "sortarry.h"
#include "collfile.h"
#include "file.h"

#pragma pack(1)   // Pack all structures used in this file

#define MAX_CATEGORY_LEVELS	3
#define MAX_CATEGORY_NAME_LEN 20	// not including NULL terminator

// delimiter between category levels (i.e. "banners/business/retail")
#define DELIMITER_CHAR			'/'

// forward declarations
class CCategoryData;

// Class that represents data object for a Catagory Tree Entry
class CCategoryEntry
   {
      enum FieldSizes { nameSize = MAX_CATEGORY_NAME_LEN+1,  productCodeSize=7 };
   protected:
      BYTE     m_byProductCode[productCodeSize];  // License info
      DWORD    m_dwFlags;           //
      char     m_szName[nameSize];  // Category name (includes NULL terminator)
      short    m_shNextSibling;     // Points to next sibling (next item at this level)
      short    m_shFirstChild;      // Points to this categories first child
      short    m_shParent;          // Parent
      DWORD    m_dwItemsOffset;     // Offset to items in item file (.BDT file)
      DWORD    m_dwItemCount;       // Number of items for this category
      BYTE     m_byLevel;           // Level of this item
   protected:
      // Internal helpers
   public:
      enum FamilyTypes
         {
            none = -1
         };
   // Initialization
      void Empty();

   // Get Operations
      short GetParent()                      { return m_shParent; }
      short GetFirstChild()                  { return m_shFirstChild; }
      short GetNextSibling()                 { return m_shNextSibling; }
      BYTE  GetLevel()                       { return m_byLevel; }
      DWORD GetFlags()                       { return m_dwFlags; }

      CString GetProductCode() const         { return (LPCSTR) m_byProductCode; }
      void GetName(CString *pUsersName)		{ ASSERT(pUsersName); *pUsersName = m_szName; }
      const LPCSTR GetName() const           { return (LPCSTR) m_szName; }
      DWORD GetOffsetToData()                { return m_dwItemsOffset; }
      DWORD GetItemCount()                   { return m_dwItemCount;}

   // Static Get Operations
      static short GetNameSize()             { return nameSize; }
      static short GetProductCodeSize()      { return productCodeSize; }

   // Set Operations
      void  SetParent(short shParent)        { m_shParent = shParent; }
      void  SetFirstChild(short shChild)     { m_shFirstChild = shChild; }
      void  SetNextSibling(short shSibling)  { m_shNextSibling = shSibling; }
      void  SetLevel(BYTE byLevel)           { m_byLevel = byLevel; }
      void  SetFlags(DWORD dwFlags)          { m_dwFlags = dwFlags; }
      void  SetLicenseInfo(CString &strProductCode)
			{
				Util::SafeStringCopy((char *)m_byProductCode,
											strProductCode.GetBuffer(1),
											sizeof(m_byProductCode));
				strProductCode.ReleaseBuffer();
			}
		void  SetName(CString &strCategoryName)
			{
				Util::SafeStringCopy(m_szName, strCategoryName.GetBuffer(1),
											sizeof(m_szName));
				strCategoryName.ReleaseBuffer();
			}
      void  SetOffsetToData(DWORD dwOffset)   { m_dwItemsOffset = dwOffset; }
      void  SetItemCount(DWORD dwItemCount)   { m_dwItemCount = dwItemCount; }
   };


// Class for management of Category Tree file
class CCategoryDir
   {
   public:
      CCategoryDir();
      ~CCategoryDir();
      int               Init(CFile *pFile);
      // These return 0 based indexes into category entry array
      int               GetParent(int nItemNum);
      int               GetFirstChild(int nItemNum);
      int               GetNextSibling(int nItemNum);
      int               GetPrevSibling(int nItemNum);
      int               FindPrevLevel(int nItemNum, int nCurLevel=-1);
      int               GetItem(int nItemNum, CCategoryEntry *pCategoryItem);
      int               IncrementItemCount(int nItemNum);
   // Category and SubCategory separated by "|"
      virtual int       FindFirst(CString &strCategoryToFind, int nStartIndex=0);
      virtual int       FindNext();
      int               GetCurrentCategory()      {  return m_nCurCategory; }
   // Finds a specific Category (doesn't support delimiters)
      int               FindCategory(int nStartIndex, CString &strCategoryToFind);

   // Get full category name associated with an item ie. "Animals/Dogs" (not for User collections)
      CString           GetCategoryStrInReadOnly(DWORD dwItem);

		ERRORCODE			RenameCategory(CString& strOld, CString& strNew);

   // GetProductCode returns product code associated with specified item
      CString           GetProductCode(DWORD dwItem);

	// Get the total item count for all categories.
		DWORD					GetItemCount(void);

   // Add will add new categories as needed but must be supplied in Sorted order
      virtual int       Add(CString &strCategory, CString &strProductCode, DWORD dwFlags,
									 DWORD dwDataOffset);

	// Insert/DeleteEntry will insert/delete a category at index specified
		void              InsertEntry(int nInsertPos, const CCategoryEntry& newEntry);
		int					DeleteEntry(int nEntryToDelete);
	// Update entry
      int               Update(int nIndex, CCategoryEntry &refNewCategory);
	// Modify entry NextSib/1stChild/Parent relationships
		int					UpdatePositions(int nStartIndex, int nChange);
		int					UpdateChildOffsets(int nParentEntry, DWORD dwNewOffset);
      int               GetCategoryCount();
      CCategoryEntry *  GetCategoryEntry(int nCategory);
		// Get depth of category tree
      int               GetNumLevels()						{ return m_nNumLevels; }
		void					SetDataItemSize(DWORD size)	{ m_dwDataItemSize = size; }

   // Diagnostics
      int               Dump(CStdioFile &refStdioDumpFile);

   // Internal Helpers
   protected:
      int               AddEntryToEnd(const CCategoryEntry& newEntry);
      int               LoadCategories();
      void              UpdateCategoryDepth();
      int               FindCategory(int nLevelToFind, int nStartIndex=0);

      // Find a category entry based on item number (read-only collections)
      CCategoryEntry *  FindInReadOnly(DWORD dwItem);

   protected:
      BOOL              m_bWildcardSearch;
      CFile *           m_pFile;
      CVarArray         m_carrayCategories;	// array of CCategoryEntry
      CMlsStringArray   m_csaCategoryFields;
      int               m_naCurCategory[MAX_CATEGORY_LEVELS];
      int               m_nCurLevel;
      int               m_nStartLevel;
      int               m_nNumCurLevels;  // Number of levels current category has
      int               m_nNumLevels;     // Deepest level in category tree
      int               m_nCurCategory;   // Current category index from Find

		// we need to know the size of items in the category data file, so we can
		// find items by using the offsets stored in the CCategoryEntry
		DWORD					m_dwDataItemSize;
   };

// Must NOT have virtual functions
class CCategoryDataItem
   {
   public:
      // Tone - lower byte
      enum Tones
         {
            toneNone                = 0x00000000,
            toneHumorous            = 0x00000001,
            toneSentimental         = 0x00000002,
            toneTraditional         = 0x00000004,
            toneContemporary        = 0x00000008, 
            toneSpiritual           = 0x00000010
         };

      // Custom Flags - middle two bytes
      enum CustomFlags
         {
            cfNone                  = 0x000000,
            cfFreeSample            = 0x000001,
            cfHideItem              = 0x000002,
				cfCropCopyright         = 0x000004,
//            miscPenInk             = 0x000004,
//            miscPencil             = 0x000008,
//            miscAirbrush           = 0x000010,
//            miscCalligraphy        = 0x000020,
//            miscPhotos             = 0x000040,
//            miscCutpaper           = 0x000080,
//            miscWoodstyle          = 0x000100,
//            miscOilAcrylic         = 0x000200

            cfBWGraphic             = 0x000800,		// graphic types
            cfColorGraphic          = 0x001000,
            cfFineArt               = 0x002000,
            cfPhoto                 = 0x004000,
            cfClipPhoto             = 0x008000
         };
#if 0
      // Styles   - Upper byte
      enum Styles
         {
            styleNone               = 0x00, 
            styleAbstract           = 0x01, 
            styleGraphic            = 0x02,
            styleRealistic          = 0x04, 
            stylePainterly          = 0x08, 
            styleCartoon            = 0x10
         };
#endif
      // Product Flags are one of these two enumerated types base upon 
      // current configuration
      enum ProductFlags
         {
            pflagSilver				= 0x01,
			// pflagPubSuite           = 0x01,	// Publishing suite and Silver are the same
            pflagBonusPack          = 0x02, 
            pflagClassic            = 0x04, 
            // pflagDeluxe             = 0x08, // Deluxe and Gold are the same
            pflagGold				= 0x08,
			pflagPremier			= 0x10,	// For now, Office Suite and Supreme are the same
            pflagPlatinum           = 0x20
         };
      enum AGProductFlags
         {
            pflagAGStandard           = 0x01,
            pflagAGPremium            = 0x02
         };

    // Get operations
      DWORD    GetItemNumber()                  { return m_dwItemNum; }
      Tones    GetTone()   { return (Tones)(m_dwMSTFlags & 0x000000FF); }

																				// bits 11-15 of middle two bytes
		CustomFlags GetGraphicType()  { return (CustomFlags)((m_dwMSTFlags & 0x00F80000) >> 8); }

																				// bits 0-2 of middle two bytes
      CustomFlags GetCustomFlags()  { return (CustomFlags)((m_dwMSTFlags & 0x00000700) >> 8); }

//      Styles   GetStyle()  { return (Styles)((m_dwMSTFlags & 0xFF000000) >> 24); }
      USHORT   GetProductFlags() { return m_shProductFlags; }

    // Set operations
      void     SetItemNumber(DWORD dwItemNum)   { m_dwItemNum = dwItemNum; }
      void     SetTone(Tones newTone)     { m_dwMSTFlags |= newTone; }
		void		SetGraphicType(CustomFlags newGraphicType) { m_dwMSTFlags |= newGraphicType << 8; }
      void     SetCustomFlag(CustomFlags newFlag);
//      void     SetMedia(Media newMedia)   { m_dwMSTFlags |= newMedia << 8; }
//      void     SetStyle(Styles newStyle)  { m_dwMSTFlags |= newStyle << 24; }
//      void     SetAttributes(Tones newTone, Media newMedia, Styles newStyle);
      void     SetAttributes(Tones newTone, CustomFlags newCustomFlag);
      void     SetProductFlags(USHORT shProductFlags)
         { m_shProductFlags = shProductFlags; }
   // Compare operations
      BOOL     CompareAttributes(CCategoryDataItem &refCDataItem);
   // Clear object
      void     Empty()
         {  m_dwItemNum = 0;  m_dwMSTFlags = 0; m_shProductFlags = 0; }
   // Low level operations
      DWORD    GetMSTFlags()                    { return m_dwMSTFlags; }
      void     SetMSTFlags(DWORD dwFlags)       { m_dwMSTFlags = dwFlags; }
   protected:
      DWORD          m_dwItemNum;
      DWORD          m_dwMSTFlags;
      USHORT         m_shProductFlags;
   };

// This class describes the data interface for a Category object
class CCategoryData : public CDataFileEntry
{
public:
	// This is a huge array of CCategoryDataItems
	CHugeArray  m_carrayItems;

public:
	CCategoryData()      { m_carrayItems.SetElementSize(sizeof(CCategoryDataItem)); }

	// Get operations
	CString &   GetCategory()			{  return m_strCategory; }
	CString &   GetProductCode()		{  return m_strProductCode; }
	DWORD       GetFlags()				{ return m_dwFlags; }
	const CCategoryDataItem*   GetItem(DWORD dwIndex)
		{  return ((const CCategoryDataItem *) m_carrayItems.GetAt(dwIndex)); }
	DWORD       GetItemCount()			{ return m_carrayItems.GetSize(); }

	// Set operations
	void        SetFlags(DWORD dwFlags)	{ m_dwFlags = dwFlags; }
	void        SetCategory(CString &strCategory)	{  m_strCategory = strCategory; }
	void        SetProductCode(const CString &strProductCode)  {  m_strProductCode = strProductCode; }
	// Use Set() if you want to init. this object to contain 1 item
	int         Set(CString &strCategory, CString &strProductCode,
						 CCategoryDataItem *pCDItem, DWORD dwFlags=0);
	// Misc operations
	int         AddItem(CCategoryDataItem *pCDItem)    {  return m_carrayItems.Add(pCDItem); }
	int         DeleteItem(DWORD dwItemToDelete);
	void        Empty();

	virtual CDataFileEntry* CreateObject() { return new CCategoryData; }

protected:
	CString     m_strCategory;
	CString     m_strProductCode;
	DWORD       m_dwFlags;
};

class CCategoryDatabase 
{
public:
   CCategoryDatabase(StorageDevice *pCFileData);
   ~CCategoryDatabase();
// Init will cause entire Category Directory to be read into directory object
   int            Init(CFile *pFile)       { return m_Dir.Init(pFile); }

// Find a Category
//   Categories are separated by | char
//    Supplying "All" will get all items
   int            FindFirst(CString &strCategory, CCategoryData *pCategoryData);
   int            FindNext(CCategoryData *pCategoryData);
   int            Read(DWORD dwBytesToRead=0);
   virtual int    Add(CCategoryData *pCategoryData, BOOL fUserCollection);
   int            Delete(DWORD dwItemToDelete);

// Associates free list file with data file, must be called prior to Delete
   virtual int    SetFreeList(CFreeLists *pFreeList) { return m_Data.SetFreeList(pFreeList); }

   DWORD          GetItemSize()               { return m_Data.GetItemSize(); }
   DWORD          GetItemCount()
						{
							ST_DEV_POSITION   stdevFileLength;
							m_Data.GetDevice()->length(&stdevFileLength);
							return stdevFileLength / sizeof(CCategoryDataItem);
						}
   CString        GetProductCode(DWORD dwItem) { return m_Dir.GetProductCode(dwItem); }
   CCategoryDir * GetDir()                     { return &m_Dir; }
   CString        GetCategory(DWORD dwItem, BOOL fUserColl);
   ERRORCODE      GetCategoryDataItem(DWORD dwItem, CCategoryDataItem *pCDataItem);
	ERRORCODE		RenameCategory(CString& strOld, CString& strNew)
						{ return m_Dir.RenameCategory(strOld, strNew); }

protected:
	int				AddToExisting(int nItemEntry, CCategoryData *pCategoryData);
	int				InsertSubcategory(int nParentEntry, CCategoryData* pCategoryData);
	int				AddToEnd(CCategoryData *pCategoryData);
	int				GetCatEntryNumFromItemNum(DWORD dwItem);

   // m_bReadItems is TRUE if all items for the category supplied to Find 
   // were read into data object
   BOOL				m_bReadItems;
   BOOL				m_bOkToFindNext;
   int				m_nCategoryIndex;    // current category index
   CCategoryDir	m_Dir;
   CFileData		m_Data;
   CCategoryEntry	m_categoryItem;      // category entry 
   CCategoryData*	m_pCategoryData;
};

// Inline Section
inline void CCategoryDataItem::SetCustomFlag(CustomFlags newFlag)
   {
      if(newFlag != cfNone)
         m_dwMSTFlags |= newFlag << 8;
      else
         m_dwMSTFlags &= 0xFFFFF8FF;   // Clear custom flag value - bottom 3 bits of middle two bytes
   } 

inline void CCategoryDataItem::SetAttributes(Tones newTone, CustomFlags newCustomFlag)
   {
      SetTone(newTone);
      SetCustomFlag(newCustomFlag);
   }


inline BOOL CCategoryDataItem::CompareAttributes(CCategoryDataItem &refCDataItem)
   {
      return refCDataItem.GetMSTFlags() == GetMSTFlags();
   }


#pragma pack()   // back to default packing

#endif      // end if _COLLCTG_INC

