/*
// $Header: /PM8/Download/icollmgr.h 1     3/03/99 6:15p Gbeddow $
//
// The collection manager interface.
//
// $Log: /PM8/Download/icollmgr.h $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 2     1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 1     9/23/98 10:43a Jayn
*/

#ifndef __ICOLLMGR_H__
#define __ICOLLMGR_H__

class CPMWCollection;
class CPMWCollectionSearchContext;

//// {BCF2DEC3-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_ICollectionManager, 0xbcf2dec3, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

/*
// The collection manager interface.
*/

#undef INTERFACE
#define INTERFACE   ICollectionManager

DECLARE_INTERFACE_(ICollectionManager, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS) PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** ICollectionManager methods ***

	// How many collections are there of a particular type or subtype?
	STDMETHOD_(int, NumberOfCollections)(THIS_ int nType, int nSubType) PURE;

	// Get a particular collection of a particular type or subtype.
	STDMETHOD_(CPMWCollection*, GetCollection)(THIS_ int nCollection, int nType, int nSubType) PURE;

	// Look up a collection by name.
	STDMETHOD_(CPMWCollection*, FindCollection)(THIS_ LPCSTR pszCollection) PURE;

	// Look up a collection by ID (across all types).
	STDMETHOD_(CPMWCollection*, FindCollection)(THIS_ WORD wID) PURE;

	// Look up a collection by ID of a particular type or subtype.
	STDMETHOD_(CPMWCollection*, FindCollection)(THIS_ WORD wID, int nType, int nSubType) PURE;

	// Look up a collection by Collection * to get it's index.
	STDMETHOD_(int, FindCollection)(THIS_ CPMWCollection* pCollectionToFind) PURE;

	// Get a user collection of a particular type.
	STDMETHOD_(CPMWCollection*, GetUserCollection)(THIS_ int nType) PURE;

	// Get the friendly name for a volume.
	STDMETHOD_(BOOL, VolumeToFriendlyName)(THIS_ LPCSTR pszVolume, CString& csFriendlyName) PURE;

	// Perform a search.
	STDMETHOD_(int, Search)(THIS_ CPMWCollectionSearchContext& Context) PURE;

	// Get the shared paths.
	STDMETHOD_(const CStringArray*, GetSharedPaths)(THIS) PURE;
};

#endif					// #ifndef __ICOLLMGR_H__
