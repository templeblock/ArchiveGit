/////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/downmgr.H 1     3/03/99 6:05p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/downmgr.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 6     11/18/98 10:35p Psasse
// handle delete collections response
// 
// 5     11/16/98 9:56p Psasse
// begin to incorporate delete functionality
// 
// 4     10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 3     10/26/98 6:47p Jayn
// Download manager.
// 
// 2     10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
// 1     10/24/98 2:18p Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __DOWNMGR_H__
#define __DOWNMGR_H__

#include "icontsrv.h"
#include "icontmgr.h"
#include "ipathmgr.h"

class CCollectionProgressDialog;

//
// Download concepts:
//
// Downloads are controlled via directory (*.DIR) files. These files are
// based on standard Windows INI files for ease of creation and maintenance.
// The DIR files list which files need to be downloaded. Hopefully, they will
// also contain validation information (e.g. time/date stamp, file size, or
// checksum) to allow the program to make sure the file has been truly
// downloaded.
//
// The following is a sample DIR file:
//
// [Files]
// gcpack1.bdr=coll/gcpack1.bdr
// gcpack1.bdt=coll/gcpack1.bdt
// gcpack1.cdr=coll/gcpack1.cdr
// gcpack1.cdt=coll/gcpack1.cdt
// gcpack1.idr=coll/gcpack1.idr
// gcpack1.idt=coll/gcpack1.idt
// gcpack1.kdr=coll/gcpack1.kdr
// gcpack1.kdt=coll/gcpack1.kdt
// .....
//
// [Sizes]
// gcpack1.bdr=34789
// gcpack1.bdt=12356
// .....
//
// Note that I have provided a file sizes section. However, it is undecided
// at this moment what the validation information will be (if any on this
// first phase). Since additional information appears in parallel, separate
// sections, additional sections can be added at will for later revs of the
// code (which can be made to silently deal with missing sections from older
// versions of the DIR files).
//
// The [Files] section is currently the truly mandatory section. The string
// to the left of each '=' is the destination file name. The string to the
// right of each '=' is the file name on the download server (minus the server
// base IP address).
//

//
// Collection file class.
// This wraps a file as expressed in the DIR file above.
//

class CDownloadFile
{
public:
	CString m_csFileName;
	CString m_csSourceName;
	// Any additional info can go here.

	// Check the download state of this file.
	BOOL CheckDownloadState(LPCSTR pszDirectory) const;
};

typedef CArray<CDownloadFile,CDownloadFile&> CDownloadFileArray;

//
// Collection directory class.
// This wraps the INI file described above.
//

class CDownloadDirectory
{
public:
	CDownloadDirectory();
	~CDownloadDirectory();

	// Read this info from a DIR file.
	BOOL Read(LPCSTR pszDirFile);

	// Get the array of files.
	const CDownloadFileArray& GetFiles(void)
		{ return m_Files; }

	// Are we valid?
	BOOL IsValid(void)
		{ return !m_csDirName.IsEmpty() && m_Files.GetSize() > 0; }

	CDownloadDirectory& operator=(const CDownloadDirectory& d)
		{ m_csDirName = d.m_csDirName; m_Files.Copy(d.m_Files); return *this; }

protected:
	CString m_csDirName;
	CDownloadFileArray m_Files;
};

//
// A candidate collection for downloading.
//

class CDownloadCollection
{
public:
	// What state is this collection in?
	enum DownloadState
	{
		STATE_Unknown,
		STATE_NotDownloaded,
		STATE_PartiallyDownloaded,
		STATE_Downloaded
	};

	// The collection information.
	CServerCollectionInfo m_Info;		// From the server.
	CDownloadDirectory m_Dir;			// The directory for this download.
	DownloadState m_nState;				// What state the collection is in.
	BOOL m_fDownload;						// Do we download it or not?

	// Update the state of this collection.
	void UpdateDownloadState(IPathManager* pPathManager);

	// Get the name of the DIR file.
	CString GetDirName(void);

	// Get the string representing the state.
	CString GetStateString(void);
};

typedef CArray<CDownloadCollection,CDownloadCollection&> CDownloadCollectionArray;

/////////////////////////////////////////////////////////////////////////////
// CCollectionConnectionCallback
// The callback during a request.

class CCollectionConnectionCallback : public IServerConnectionCallback
{
public:
	CCollectionConnectionCallback(CCollectionProgressDialog* pDialog);
	virtual DWORD OnStatusChange(DWORD dwInternetStatus);

public:
	CCollectionProgressDialog* GetDialog(void)
		{ return m_pDialog; }
protected:
	CCollectionProgressDialog* m_pDialog;
};

/////////////////////////////////////////////////////////////////////////////
// CContentManagerDownloadCallback
// The callback during a download. It writes the data to a file.

class CCollectionDownloadCallback : public IServerDownloadCallback
{
public:
	CCollectionDownloadCallback(CCollectionProgressDialog* pDialog, LPCSTR pszFilename);
	~CCollectionDownloadCallback();

	// Delete the file that was being downloaded.
	void ZapFile(void);

protected:
	virtual DWORD OnReceiveData(LPVOID pData, DWORD dwDataSize, DWORD dwDataTotal);

protected:
	BOOL CloseFile(void);

protected:
	CCollectionProgressDialog* m_pDialog;
	CString m_csFileName;
	DWORD m_dwTotalRead;
	CFile* m_pFile;
};

/////////////////////////////////////////////////////////////////////////////
// Collection download manager.
//

class CDownloadManager
{
public:
	CDownloadManager();
	virtual ~CDownloadManager();

	// Do it!
	void Run(IContentManager* pContentManager, IPathManager* pPathManager, LPCSTR pszChosenCollection = NULL);

protected:		// Though still useful!
	// The message broadcast when content changes.
	static UINT m_uContentChangedMessage;
	static UINT m_uSharedPathsChangedMessage;

	// Initialize the download manager.
	BOOL Init(IContentManager* pContentManager, IPathManager* pPathManager);

	// Get a list of collections from the server.
	BOOL GetCollections(CDownloadCollectionArray& Collections);

	// Get the state of the collections.
	void UpdateDownloadStates(CDownloadCollectionArray& Collections);

	// Ask the user which files to download.
	int AskUserAboutDownload(CDownloadCollectionArray& Collections, CString& csDownloadDirectory, LPCSTR pszChosenCollection = NULL);

	// Do the download.
	BOOL DoDownload(CDownloadCollectionArray& Collections, LPCSTR pszDirectory);

protected:
	void FreeAll(void);

	// Download a file.
	int DownloadFile(LPCSTR pszSourceName, LPCSTR pszFileName, CCollectionConnectionCallback& ConnectionCallback);

	// Download directory support functions.
	CString GetDownloadDirectory(const CMapStringToString& mapSharedPaths);
	void AddDownloadDirectory(CMapStringToString& mapSharedPaths, LPCSTR pszDirectory);

	// Shared path support functions.
	void GetSharedPaths(CMapStringToString& mapSharedPaths);
	void AddSharedPath(LPCSTR pszKey, LPCSTR pszValue);

protected:
	IContentManager* m_pContentManager;
	IPathManager* m_pPathManager;
};

#endif
