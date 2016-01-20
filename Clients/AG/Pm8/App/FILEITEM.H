/*
// $Workfile: FILEITEM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FILEITEM.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:50   Fred
// Initial revision.
// 
//    Rev 1.7   07 Sep 1996 10:50:12   Jay
// New Search Stuff
// 
//    Rev 1.6   01 Sep 1996 09:22:46   Jay
//  
// 
//    Rev 1.5   30 Aug 1996 10:45:24   Jay
// Keyword/item stuff
// 
//    Rev 1.4   25 Aug 1996 12:57:22   Jay
// New merging schemes.
// 
//    Rev 1.3   14 Aug 1996 15:28:02   Jay
// More project album stuff.
// 
//    Rev 1.2   08 Aug 1996 18:10:24   Jay
// JPEG, etc!
// 
//    Rev 1.1   07 Aug 1996 17:50:08   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:37:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:16   FRED
// Initial revision.
// 
//    Rev 1.7   15 Sep 1995 14:04:48   JAY
// More 32-bit changes.
// 
//    Rev 1.6   04 Aug 1995 15:19:16   JAY
// Preview updating, etc.
// 
//    Rev 1.5   26 Jun 1995 16:58:48   JAY
//  
// 
//    Rev 1.4   19 Jun 1995 17:19:12   JAY
// Item extensions
// 
//    Rev 1.3   16 Jun 1995 13:00:40   JAY
//  
// 
//    Rev 1.2   07 Jun 1995 15:39:52   JAY
//  
// 
//    Rev 1.1   06 Jun 1995 16:54:42   JAY
// Project previews in the album.
// 
//    Rev 1.0   06 Jun 1995 10:13:20   JAY
// Initial revision.
*/ 

class CFileItemData;

#ifndef __FILEITEM_H__
#define __FILEITEM_H__

#include "itemdata.h"
#include "file.h"
#include "recarray.h"

/*
// The breathe function prototype.
// This should return FALSE to continue or TRUE to signal an interrupt
// of some kind.
*/

typedef BOOL (*BREATHE_FUNCTION)(LPVOID pBreatheData);

class CDrawContext;
class CEmbeddedFileData;
class CItemDataDescriptor;
class RedisplayContext;
class CThumbNailCreationData;

class CFileItemCreationData
{
public:
	CFileItemCreationData();

	LPCSTR	m_pFileName;
	LPCSTR	m_pFriendlyName;
	BOOL		m_fEmbed;
	BOOL		m_fCreateThumbNail;
	DWORD		m_dwFlags;
	DATAID	m_idParentCategory;
};

#define FILEITEM_FLAG_Free					1			// Is "free" item.
#define FILEITEM_FLAG_HasKeywords		2			// Wrote keyword array.

/*
// This is a file item data.
// (This is meant to be derived from.)
*/

class CFileItemData : public CItemData
{
	INHERIT(CFileItemData, CItemData)
public:
	// management

	CFileItemData();
	virtual ~CFileItemData();

	virtual BOOL Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where);

	virtual void Draw(CDrawContext* pContext, const CRect& crContent, int nSubItem);
	void FileName(const char* pName);
	const CString& FileName(void) const;

	void FriendlyName(const char* pName);
	const CString& FriendlyName(void) const;

	virtual void Name(CString& csName);

	virtual int SubItems(void) const;
	virtual CRect Bounds(int nSubItem) const;
	virtual CSize Resolution(int nSubItem) const;

	DATAID SourceID(void) const
		{ return m_Record.m_idSource; }
	DATAID ThumbNailID(void) const
		{ return m_Record.m_idThumbNail; }

	DWORD Flags(void) const
		{ return m_Record.m_dwFlags; }
	void Flags(DWORD dwFlags)
		{ m_Record.m_dwFlags = dwFlags; }

	// Is this item licensed?
	BOOL IsLicensed(void);

	// Return the id of the category containing license information.
	DATAID ParentCategory(void) const
		{ return m_Record.m_idParentCategory; }

	void Update(const CItemDataDescriptor& Descriptor);

	enum
	{
		FILE_TYPE_Unknown = 0,
		FILE_TYPE_Generic,
		FILE_TYPE_Banner,
	};

	WORD FileType(void) const
		{ return m_Record.m_wFileType; }

	ERRORCODE ExtractFile(LPCSTR pDestName);

	virtual BOOL DoDraw(CDC* pDC, CRect& crBounds, BOOL fThumbNail, BREATHE_FUNCTION pBreathe = NULL, LPVOID pBreatheData = NULL);

/*
// Duplication section.
*/
public:

	virtual void Assign(const CDataRecord& Record);

/*
// Sorting support.
*/

	BOOL SortKey(int nKey, CString& csKey);

/*
// Keyword support.
*/

	void AddKeyword(DATAID idKeyword);
	int Keywords(void);
	DATAID GetKeyword(int nKeyword);
	void RemoveKeyword(DATAID idKeyword);
	void RemoveAllKeywords();

protected:
	virtual BOOL SetupDevice(StorageFile* pFile);
	virtual BOOL InitFile(void);

	virtual BOOL ThumbNailDraw(CDC* pDC, CRect& crBounds);
	void EmbedSource(LPCSTR pFileName = NULL);
	void CreateThumbNail(void);

	virtual void ThumbNailDims(int& nWidth, int& nHeight, BOOL fMonochrome = FALSE);
	virtual void ThumbNailPreCreate(CThumbNailCreationData* pData);
	virtual BOOL MonochromeThumbNail(void);

	virtual void SetRCInfo(RedisplayContext* pRC, HDC hDC, BOOL fThumbNail);

// members
	CString m_csFileName;
	CString m_csFriendlyName;

	struct
	{
		RECTS m_crBounds;
		SIZES m_czResolution;
		DATAID m_idSource;
		DATAID m_idThumbNail;
		WORD m_wFlags;
		WORD m_wFileType;				// File type.
		DWORD m_dwFlags;
		DATAID m_idParentCategory;
	} m_Record;

	CDrawContext* m_pDrawingContext;
	BOOL m_fUpdating;

	// This is calculated once. Any changes in this status require an
	// album rebuild anyway (except "free" status).
	BOOL m_fLicensed;

	LPBITMAPINFO m_pThumbNailDIB;

	RecordArray m_Keywords;

	// This is called when the data manager is about to delete us.
	virtual void OnDelete(void);

	// This is called when the data manager is about to delete us.
	virtual void OnFlush(void);

/*
// Breathing support.
*/
	static BOOL DrawBreatheHook(LPVOID pData);
	BOOL Breathe(void);

/******** StorageElement support ********/

/*
// ReadData()
//
// This is a method defined by a class derived from this one.
// It handles the reading of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
*/

	virtual	ERRORCODE ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// This is a method defined by a class derived from this one.
// It handles the writing of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
*/

	virtual	ERRORCODE WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// This is a method defined by a class derived from this one.
// It returns the size of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
//
// A pointer to the device is passed to allow access to the device's
// sizing routines.
*/

	virtual	ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};

#endif			// __FILEITEM_H__
