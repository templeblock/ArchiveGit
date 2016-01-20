/*
// $Workfile: COLLMGR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
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
// $Log: /PM8/App/COLLMGR.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 22    3/01/99 2:26p Gbeddow
// fix for bug 155 (your own art is missing) which also fixed problems
// with "your own projects"
// 
// 21    1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 20    11/02/98 5:14p Jayn
// 
// 19    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 18    10/26/98 6:47p Jayn
// Download manager.
// 
// 17    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 16    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 15    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 14    7/14/98 3:45p Hforman
// speed up Search() w/unlicensed items
// 
// 13    7/10/98 3:42p Hforman
// fixed typo
// 
// 12    7/08/98 1:04p Jayn
// Simplified filtering. Added Array variant to FillxxxFromList.
// 
// 11    6/25/98 7:35p Psasse
// added filtering of projects/collections/packs capability
// 
// 10    6/15/98 6:55p Hforman
// add "Match All" functionality for keyword searching
// 
// 9     6/01/98 5:45p Rlovejoy
// Don't use IsHidden flag in search.
// 
// 8     5/22/98 5:34p Hforman
// CPMWCollectionSearchContext now has an array of collections
// 
// 7     5/21/98 1:54p Dennis
// Added Searchable Art Collections
// 
// 6     5/21/98 10:08a Rlovejoy
// Look for 'Hidden' attribute.
// 
// 5     5/20/98 2:27p Dennis
// Added Close() functionality
// 
// 4     4/14/98 10:08a Dennis
// a) Removed Media and Style
// b) Added UpdateSharedPaths() method for shared collection support.
// c) Enhanced OpenReadOnlyCollections() to look for INI files in shared
// path(s)
// 
// 3     4/08/98 6:44p Hforman
// interim checkin - more later
// 
// 2     4/06/98 11:18a Hforman
// 
//    Rev 1.0   14 Aug 1997 15:19:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:10   Fred
// Initial revision.
// 
//    Rev 1.10   11 Jul 1997 09:38:00   Jay
// Free collection stuff
// 
//    Rev 1.9   24 Jun 1997 15:17:30   Jay
// Support for multiple file paths and CD friendly names
// 
//    Rev 1.8   17 Jun 1997 12:42:42   Jay
// Import; OpenStreams()
// 
//    Rev 1.7   13 Jun 1997 13:35:52   dennisp
// Added FindCollection by Collection *
// 
//    Rev 1.6   10 Jun 1997 14:20:46   Jay
// Moves unlicensed content to end
// 
//    Rev 1.5   10 Jun 1997 09:12:46   Jay
// Sorted collections
// 
//    Rev 1.4   03 Jun 1997 11:32:24   dennisp
// Added Find() for locating an item
// 
//    Rev 1.3   06 May 1997 17:21:54   Jay
// Multi-collection stream management
// 
//    Rev 1.2   06 May 1997 15:32:02   Jay
// Initial multi-collection searches
// 
//    Rev 1.1   06 May 1997 10:30:48   Jay
// User collections
// 
//    Rev 1.0   21 Apr 1997 14:50:28   Jay
// Collection manager
*/

class CContentCollectionManager;

#ifndef __COLLMGR_H__
#define __COLLMGR_H__

#include "icollmgr.h"
#include "pmwcoll.h"

class CPMWCollectionImportContext;

/*
// An array to hold CPMWCollections of a particular type.
*/

class CContentCollectionArray : public CPtrArray
{
public:
// Construction
	CContentCollectionArray();
	virtual ~CContentCollectionArray();

// Operations
   void Empty();
	void SetDeleteElementsOnDestruction( BOOL fDeleteElementsOnDestruction );

	CPMWCollection* GetAt(int nCollection)
		{ return (CPMWCollection*)CPtrArray::GetAt(nCollection); }

private:
	BOOL m_fDeleteElementsOnDestruction;
};

/*
// The collection manager class.
*/

class CPMCollectionManager : public CCmdTarget
{
public:
	CPMCollectionManager();
	virtual ~CPMCollectionManager();

	BOOL Open(IPathManager* pPathManager, BOOL fRegistered, BOOL fAllowUserCollections = TRUE);
	void Close(void);

	void Reopen(void);

   // GetSharedPaths() returns a CString Array containing one or more paths
   // to shared collections
   const CStringArray * GetSharedPaths()
      { return &m_csaSharedPaths; }

	void UpdateFreeCollection(BOOL fRegistered);
	void UpdateCollections(void);

	// How many collections are there of a particular type or subtype?
	int NumberOfCollections(int nType, int nSubType);

	// Get a particular collection of a particular type or subtype.
	CPMWCollection* GetCollection(int nCollection, int nType, int nSubType);

	// Look up a collection by name.
	CPMWCollection* FindCollection(LPCSTR pszCollection);

	// Look up a collection by ID (across all types).
	CPMWCollection* FindCollection(WORD wID);

	// Look up a collection by ID of a particular type or subtype.
	CPMWCollection* FindCollection(WORD wID, int nType, int nSubType);

	// Look up a collection by Collection * to get it's index.
	int FindCollection(CPMWCollection * pCollectionToFind);

	// Get a user collection of a particular type.
	CPMWCollection* GetUserCollection(int nType)
		{ return m_pUserCollections[nType]; }

	// Get the friendly name for a volume.
	BOOL VolumeToFriendlyName(LPCSTR pszVolume, CString& csFriendlyName) const;

	//
	// Search section
	//
	int Search(CPMWCollectionSearchContext& Context);

   // UpdateSharedPaths() retrieves Shared Path information from Registry
   void UpdateSharedPaths();

protected:
	void DoOpen(BOOL fRegistered);
	void DoClose(void);

	BOOL OpenReadOnlyCollections(BOOL fNewOnly = FALSE);
	int OpenUserCollection(CPMWCollection::CollectionTypes nType);
	int SetupUserIni(CString csIniFile, CPMWCollection::CollectionTypes nType, BOOL fForceCopy = FALSE, BOOL fForceCDDef = FALSE);

	BOOL AddIniCollections(CIniFile& IniFile, LPCSTR pszSection, BOOL fNewOnly = FALSE);

	void AutoImport30Content(CPMWCollection::CollectionTypes nType);
	void AutoImport40Content(CPMWCollection::CollectionTypes nType);

	CPMWCollectionImportContext* NewImportContext(CPMWCollection::CollectionTypes nType);

	// The main collection arrays. Both read-only (shared) and user collections
	// live in these arrays.
	// There is one for each type.
	// 0 = projects
	// 1 = arts
	// 2 = sentiments
	CContentCollectionArray m_Collections[CPMWCollection::typeListEnd];

	// Keep a pointer to the user collections.
	// These pointers are also stored in the Collections array, so
	// these particular instances are not deleted.
	CPMWCollection* m_pUserCollections[CPMWCollection::typeListEnd];

	// The subtype collection array - a logical subset of the m_Collections array
	// There is one for each subtype.
	// 0 = Art
	// 1 = WebArt
	CContentCollectionArray m_SubTypeCollections[CPMWCollection::subTypeListEnd];

	// Volume label to friendly name lookup.
	CMapStringToString m_mapFriendlyNames;

	// Was the user registered last time we read the collections?
	BOOL m_fRegistered;

	// Do we allow user collections?
	BOOL m_fAllowUserCollections;

   // Shared Collection Paths
   CStringArray   m_csaSharedPaths;

	// The path manager we use.
	IPathManager* m_pPathManager;

	void SortCollections(void);

private:
	// Sort a CContentCollectionArray
	void SortContentCollectionArray(CContentCollectionArray* pArray);

//
// Interfaces.
//

protected:
	DECLARE_INTERFACE_MAP()

	// This defines the interface that is passed to others.
	// It will have name m_XCollectionManager.
	// To see comments on the functions, see the ICollectionManager definition
	// in ICOLLMGR.H.
	BEGIN_INTERFACE_PART(CollectionManager, ICollectionManager)
		INIT_INTERFACE_PART(CPMCollectionManager, CollectionManager)
		STDMETHOD_(int, NumberOfCollections)(THIS_ int nType, int nSubType);
		STDMETHOD_(CPMWCollection*, GetCollection)(THIS_ int nCollection, int nType, int nSubType);
		STDMETHOD_(CPMWCollection*, FindCollection)(THIS_ LPCSTR pszCollection);
		STDMETHOD_(CPMWCollection*, FindCollection)(THIS_ WORD wID);
		STDMETHOD_(CPMWCollection*, FindCollection)(THIS_ WORD wID, int nType, int nSubType);
		STDMETHOD_(int, FindCollection)(THIS_ CPMWCollection* pCollectionToFind);
		STDMETHOD_(CPMWCollection*, GetUserCollection)(THIS_ int nType);
		STDMETHOD_(BOOL, VolumeToFriendlyName)(THIS_ LPCSTR pszVolume, CString& csFriendlyName);
		STDMETHOD_(int, Search)(THIS_ CPMWCollectionSearchContext& Context);
		STDMETHOD_(const CStringArray*, GetSharedPaths)(THIS);
	END_INTERFACE_PART(CollectionManager)
};

//
// Get the string that says "All" (after localization).
//

extern LPCSTR BrowserAllString(void);

#endif
