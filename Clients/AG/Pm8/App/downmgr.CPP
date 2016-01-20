//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/downmgr.CPP 1     3/03/99 6:05p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/downmgr.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 11    11/23/98 8:28p Psasse
// Update when deleting collections
// 
// 10    11/21/98 10:45p Psasse
// 
// 9     11/18/98 10:35p Psasse
// handle delete collections response
// 
// 8     11/17/98 9:55a Johno
// Added #include "pmwinet.h" for release build
// I'm not sure why this is needed...
// 
// 7     11/16/98 9:56p Psasse
// begin to incorporate delete functionality
// 
// 6     11/06/98 4:52p Jayn
// 
// 5     10/27/98 7:08p Jayn
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

#include "stdafx.h"
#include "resource.h"
#include "downmgr.h"
#include "downdlg.h"

#include "inifile.h"
#include "util.h"
#include <mmsystem.h>
#include "pmwinet.h"

#define CLOSE 0
#define DOWNLOAD 1
#define DELETECOLLECTION 2

static char BASED_CODE cbSharedKey[] = "SOFTWARE\\Mindscape\\Shared";

/////////////////////////////////////////////////////////////////////////////
// CDownloadFile

// Check the download state of this file.
BOOL CDownloadFile::CheckDownloadState(LPCSTR pszDirectory) const
{
	BOOL fValid = FALSE;
	// Build the name to check for.
	CString csFileName;
	Util::ConstructPath(csFileName, pszDirectory, m_csFileName);

	// For now, just check for existence.
	if (Util::FileExists(csFileName))
	{
		// Check for anything else?
		fValid = TRUE;
	}
	return fValid;
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadDirectory

// Constructor.
CDownloadDirectory::CDownloadDirectory()
{
}

// Destructor.
CDownloadDirectory::~CDownloadDirectory()
{
}

//
// Read the info from a DIR file.
//

BOOL CDownloadDirectory::Read(LPCSTR pszDirFile)
{
	// Remember what we tried to read.
	m_csDirName = pszDirFile;

	// Get rid of any files we currently have.
	m_Files.RemoveAll();

	// Setup the INI file to read from.
	CIniFile DirFile(pszDirFile);

	// Read the Files section.
	LPSTR pszKeys = DirFile.GetSectionKeys("Files");
	if (pszKeys != NULL)
	{
		// We got some keys.
		LPSTR p = pszKeys;
		while (*p != '\0')
		{
			// Fill out the file structure.
			CDownloadFile File;
			File.m_csFileName = p;
			// Read the source file.
			File.m_csSourceName = DirFile.GetString("Files", p);
			// Read anything else...

			// Validate the file.
			if (!File.m_csSourceName.IsEmpty())
			{
				// OK to add.
				m_Files.Add(File);
			}
			// Advance to the next key.
			p += strlen(p)+1;
		}

		// Get rid of the keys data.
		delete [] pszKeys;
	}

	// Only valid if we actually got some files.
	return (m_Files.GetSize() > 0);
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadCollection

// Update the state of this collection.
void CDownloadCollection::UpdateDownloadState(IPathManager* pPathManager)
{
	// Try to read the dir file.
	CString csDirName = pPathManager->LocatePath(GetDirName());
	if (!csDirName.IsEmpty() && m_Dir.Read(csDirName))
	{
		// We have the directory!
		// To prevent myself from going insane trying to figure out all
		// the possible permutations of things, I am restricting the downloaded
		// files to live where the DIR file does.

		CString csDirectory;
		Util::SplitPath(csDirName, &csDirectory, NULL);

		m_nState = STATE_Downloaded;		// Assume so!

		// Check the state of each file.
		const CDownloadFileArray& Files = m_Dir.GetFiles();
		for (int n = 0; n < Files.GetSize(); n++)
		{
			if (!Files[n].CheckDownloadState(csDirectory))
			{
				// Not all files are downloaded.
				m_nState = STATE_PartiallyDownloaded;
				break;
			}
		}
	}
	else
	{
		// No valid DIR file.
		m_nState = STATE_NotDownloaded;
	}
}

CString CDownloadCollection::GetDirName(void)
{
	// DIR files live in the content arena.
	CString csDirName;
	return Util::ConstructPath(csDirName, "[[S]]", m_Info.m_csName + ".DIR");
}

// Get the string representing the state.
CString CDownloadCollection::GetStateString(void)
{
	int nID;
	switch (m_nState)
	{
		case STATE_NotDownloaded:
		{
			nID = IDS_DownloadedNo;
			break;
		}
		case STATE_PartiallyDownloaded:
		{
			nID = IDS_DownloadedPartial;
			break;
		}
		case STATE_Downloaded:
		{
			nID = IDS_DownloadedYes;
			break;
		}
		default:
		{
			// Not supposed to happen!
			ASSERT(FALSE);
			return "Unknown";
		}
	}
	CString cs;
	cs.LoadString(nID);
	return cs;
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadManager

UINT CDownloadManager::m_uContentChangedMessage = ::RegisterWindowMessage("PMContentChanged");
UINT CDownloadManager::m_uSharedPathsChangedMessage = ::RegisterWindowMessage("MindscapeSharedPathsChanged");

CDownloadManager::CDownloadManager()
{
	m_pContentManager = NULL;
	m_pPathManager = NULL;
}

CDownloadManager::~CDownloadManager()
{
	FreeAll();
}

//
// Free all session data.
//

void CDownloadManager::FreeAll(void)
{
	// Release the content manager.
	if (m_pContentManager != NULL)
	{
		m_pContentManager->Release();
		m_pContentManager = NULL;
	}

	// Release the path manager.
	if (m_pPathManager != NULL)
	{
		m_pPathManager->Release();
		m_pPathManager = NULL;
	}
}

//
// Initialize the download manager.
//

BOOL CDownloadManager::Init(IContentManager* pContentManager, IPathManager* pPathManager)
{
	// Get rid of any old interfaces.
	FreeAll();

	// Attach the new content manager.
	if ((m_pContentManager = pContentManager) != NULL)
	{
		m_pContentManager->AddRef();
	}
	else
	{
		// NULL ContentManager passed to DownloadManager.
		ASSERT(FALSE);
		return FALSE;
	}

	// Attach the new path manager.
	if ((m_pPathManager = pPathManager) != NULL)
	{
		m_pPathManager->AddRef();
	}
	else
	{
		// NULL PathManager passed to DownloadManager.
		ASSERT(FALSE);
		return FALSE;
	}

	return TRUE;
}

//
// Allow the user to download files.
//

void CDownloadManager::Run(IContentManager* pContentManager, IPathManager* pPathManager, LPCSTR pszChosenCollection /*=NULL*/)
{
	if (Init(pContentManager, pPathManager))
	{
		CDownloadCollectionArray Collections;
		// Read the list of collections.
		if (GetCollections(Collections))
		{
			// Are there any collections?
			if (Collections.GetSize() == 0)
			{
				// We have no collections. :(
				AfxMessageBox(IDS_NoCollectionsToDownload);
			}
			else
			{
				// We have collections. :)

				// Get the shared directories.
				CMapStringToString mapSharedPaths;
				GetSharedPaths(mapSharedPaths);

				// Compute the default directory to use for downloading.
				CString csDownloadDirectory = GetDownloadDirectory(mapSharedPaths);

				// Get the collection download states.
				UpdateDownloadStates(Collections);

				// Ask the user what to do.
				int iResponse = 0;
				
				while (iResponse = AskUserAboutDownload(Collections, csDownloadDirectory, pszChosenCollection))
				{
					
					if(iResponse == DOWNLOAD)
					{
						
						// Add this download directory to the registry.
						AddDownloadDirectory(mapSharedPaths, csDownloadDirectory);

						// Download the collections.
						DoDownload(Collections, csDownloadDirectory);

						// Get the collection download states.
						UpdateDownloadStates(Collections);
					}
					else if (iResponse == DELETECOLLECTION)
					{
						// Get the collection download states.
						UpdateDownloadStates(Collections);
					}						
				}
			}
		}
	}
}

//
// Get the directory to use for downloading.
//

CString CDownloadManager::GetDownloadDirectory(const CMapStringToString& mapSharedPaths)
{
	CString csDirectory;
	if (!mapSharedPaths.Lookup("online", csDirectory))
	{
		CString csProgramFilesDir;
		Util::GetProgramFilesDir(csProgramFilesDir);
		Util::ConstructPath(csDirectory, csProgramFilesDir, "Mindscape\\Online");
	}
	return csDirectory;
}

//
// Add this download directory to the registry if not already there.
// We also add it to the map since that is what we locally refer to.
//

void CDownloadManager::AddDownloadDirectory(CMapStringToString& mapSharedPaths, LPCSTR pszDirectory)
{
	// See if this directory exists in the current shared paths.
	BOOL fExists = FALSE;
	POSITION pos = mapSharedPaths.GetStartPosition();
	while (pos != NULL)
	{
		// Get the next pair.
		CString csKey, csValue;
		mapSharedPaths.GetNextAssoc(pos, csKey, csValue);

		// See if this is the directory we just used.
		if (csValue.CompareNoCase(pszDirectory) == 0)
		{
			// The path exists!
			fExists = TRUE;
			break;
		}
	}

	// If we did not find it, add it now.
	if (!fExists)
	{
		for (int n = 0; n < 4096; n++)
		{
			CString csKey;
			if (n == 0)
			{
				csKey = "Online";
			}
			else
			{
				csKey.Format("Online%d", n);
			}
			CString csValue;
			if (!mapSharedPaths.Lookup(csKey, csValue))
			{
				// We have found a slot to use.
				AddSharedPath(csKey, pszDirectory);

				// Add it to our map, too.
				csKey.MakeLower();
				mapSharedPaths.SetAt(csKey, pszDirectory);
				break;
			}
		}
	}
}

//
// Borrowed from the collection manager.
//

void CDownloadManager::GetSharedPaths(CMapStringToString& mapSharedPaths)
{
   char     chKey[MAX_PATH], szSharedPath[MAX_PATH];
   HKEY     hKey = NULL, hSharedKey = NULL;
   DWORD    retCode;
   DWORD    dwLength = 0;
   CString  csSharedPathKey;

	mapSharedPaths.RemoveAll();

   TRY
   {
      if ( (retCode = RegOpenKeyEx(
               HKEY_LOCAL_MACHINE,
               cbSharedKey,
               0,
               KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
               &hKey)) == ERROR_SUCCESS)
      {
         for(int i=0;retCode == ERROR_SUCCESS;i++)
            {
               retCode = RegEnumKey(hKey, i, chKey, MAX_PATH);
               if(retCode == (DWORD) ERROR_SUCCESS)
                  {
                     hSharedKey = NULL;
                     csSharedPathKey = cbSharedKey;
                     csSharedPathKey += "\\";
                     csSharedPathKey += chKey;
                     if ( (retCode = RegOpenKeyEx(
                              HKEY_LOCAL_MACHINE,
                              (LPCSTR)csSharedPathKey,
                              0,
                              KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
                              &hSharedKey)) == ERROR_SUCCESS)
                     {
                        dwLength = sizeof(szSharedPath);
                        if ((retCode = RegQueryValueEx(
                                 hSharedKey,
                                 NULL,
                                 NULL,
                                 NULL,
                                 (LPBYTE)szSharedPath,
                                 &dwLength)) == ERROR_SUCCESS)
                           {
										CString csKey = chKey;
										csKey.MakeLower();
                              mapSharedPaths.SetAt(csKey, szSharedPath);
                           }
                        RegCloseKey(hSharedKey);
                     }
                  }

            }
         RegCloseKey(hKey);
      }
   }
   CATCH_ALL(e)
   {
      if (hKey != NULL)
      {
         RegCloseKey(hKey);
      }
      if (hSharedKey != NULL)
      {
         RegCloseKey(hSharedKey);
      }
      csSharedPathKey.Empty();
   }
   END_CATCH_ALL
}

//
// Add a shared path to the registry.
//

void CDownloadManager::AddSharedPath(LPCSTR pszKey, LPCSTR pszValue)
{
	HKEY hRootKey;
	// Open the root shared key. We want permission to create a subkey.
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						  cbSharedKey,
						  0,
						  KEY_CREATE_SUB_KEY,
						  &hRootKey) == ERROR_SUCCESS)
	{
		HKEY hNewKey = NULL;
		DWORD dwDisposition;
		
		if (RegCreateKeyEx(hRootKey,
								 pszKey,
								 0,
								 "",
								 REG_OPTION_NON_VOLATILE,
								 KEY_ALL_ACCESS,
								 NULL,
								 &hNewKey,
								 &dwDisposition) == ERROR_SUCCESS)
		{
			// We created the key.
			// Add the value.
         RegSetValueEx(hNewKey,
							  NULL,
							  0,
							  REG_SZ,
							  (const unsigned char *)pszValue,
							  strlen(pszValue)+1);
			RegCloseKey(hNewKey);
		}
		RegCloseKey(hRootKey);
	}

	// Inform all concerned about the change in the registry.
	::PostMessage(HWND_BROADCAST, m_uSharedPathsChangedMessage, 0, 0);
}

//
// Get a list of collections from the server.
//

BOOL CDownloadManager::GetCollections(CDownloadCollectionArray& Collections)
{
	ASSERT(m_pContentManager != NULL);

	CServerCollectionInfoArray ServerCollections;
	if (m_pContentManager != NULL
		 && m_pContentManager->GetCollectionList(ServerCollections) == CSSTATUS_Success)
	{
		// Add a download collection for each server collection.
		for (int n = 0; n < ServerCollections.GetSize(); n++)
		{
			// Construct the download collection for this server collection.
			CDownloadCollection Collection;
			Collection.m_Info = ServerCollections.GetAt(n);
			// We do not add collections without SKU numbers.
			if (!Collection.m_Info.m_csSKU.IsEmpty())
			{
				Collection.m_nState = CDownloadCollection::STATE_Unknown;
				Collection.m_fDownload = FALSE;

				// Add this collection.
				Collections.Add(Collection);
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//
// Get the collection status.
//

void CDownloadManager::UpdateDownloadStates(CDownloadCollectionArray& Collections)
{
	// Update the state for each collection.
	for (int n = 0; n < Collections.GetSize(); n++)
	{
		Collections[n].UpdateDownloadState(m_pPathManager);
	}
}

// Ask the user which files to download or delete.
int CDownloadManager::AskUserAboutDownload(CDownloadCollectionArray& Collections, CString& csDownloadDirectory, LPCSTR pszChosenCollection /*=NULL*/)
{
	int iRet = 0;
	
	CDownloadDialog Dialog(&Collections, &csDownloadDirectory, pszChosenCollection);
	int iDlgRet = Dialog.DoModal();
			
	if(iDlgRet == IDOK)
	{
		iRet = DOWNLOAD;
	}
	else if(iDlgRet == IDCANCEL)
	{
		iRet = CLOSE;
	}
	else
	{
		iRet = DELETECOLLECTION;
	}

	return iRet;
}

// Do the download.
BOOL CDownloadManager::DoDownload(CDownloadCollectionArray& Collections, LPCSTR pszDirectory)
{
	//
	// Build an array containing just the collections we want to download.
	//

	CPtrArray cpaDownloadCollections;

	for (int n = 0; n < Collections.GetSize(); n++)
	{
		CDownloadCollection& Collection = Collections.ElementAt(n);
		if (Collection.m_fDownload)
		{
			// Add this collection to the list.
			cpaDownloadCollections.Add(&Collection);
		}
	}

	//
	// Now, do the download.
	//

	int nCollections = cpaDownloadCollections.GetSize();
	if (nCollections == 0)
	{
		// Should not even be here.
		ASSERT(FALSE);
		return FALSE;
	}

	// Setup the progress dialog.
	CCollectionProgressDialog Dialog;
	Dialog.Create(CCollectionProgressDialog::IDD, CWnd::GetSafeOwner(NULL));
	Dialog.Show();

	// Create the connection callback.
	CCollectionConnectionCallback ConnectionCallback(&Dialog);

	int nStatus = CSSTATUS_Success;
	int nCollectionsDownloaded = 0;
	for (int nCollection = 0; nCollection < nCollections; nCollection++)
	{
		// Get the collection to download.
		CDownloadCollection* pCollection = (CDownloadCollection*)cpaDownloadCollections.GetAt(nCollection);

		// Update the dialog text.
		Dialog.SetCollectionsLeft(nCollections-nCollection);
		Dialog.SetCurrentCollection(pCollection->m_Info.m_csDescription);
		Dialog.SetFilesLeft(-1);		// Getting DIR file.

		// Download the DIR file.
		// Build the source name of the DIR file.
		CString csDirSourceName;
		csDirSourceName = pCollection->m_Info.m_csFileName;
		// Sometimes these files end in '.'. Get rid of it if so.
		int nLength = csDirSourceName.GetLength();
		if (nLength > 0 && csDirSourceName[nLength-1] == '.')
		{
			csDirSourceName = csDirSourceName.Left(nLength-1);
		}

		// Stick the DIR extension on.
		csDirSourceName += ".dir";

		// Build the destination name of the DIR file.
		CString csDirFileName;
		Util::ConstructPath(csDirFileName, pszDirectory, pCollection->m_Info.m_csName + ".dir");

		if ((nStatus = DownloadFile(csDirSourceName, csDirFileName, ConnectionCallback)) == CSSTATUS_Success)
		{
			// We have the DIR file.
			if (pCollection->m_Dir.Read(csDirFileName))
			{
				// We have the directory.
				const CDownloadFileArray& Files = pCollection->m_Dir.GetFiles();
				int nFiles = Files.GetSize();
				for (int nFile = 0; nFile < nFiles; nFile++)
				{
					// Set how many files are left.
					Dialog.SetFilesLeft(nFiles-nFile);

					// Get the file to download.
					const CDownloadFile& File = Files.GetAt(nFile);

					//if (!File.CheckDownloadState(pszDirectory))
					//{
						// We need to download it.
						// Construct the name of the file.
						CString csDestName;
						Util::ConstructPath(csDestName, pszDirectory, File.m_csFileName);
						if ((nStatus = DownloadFile(File.m_csSourceName, csDestName, ConnectionCallback)) != CSSTATUS_Success)
						{
							// The file was not downloaded.
							if (nStatus != CSSTATUS_UserAbort)
							{
								// See if the user wants to continue downloading files.
								if (AfxMessageBox(IDS_CantDownloadCollection, MB_YESNO) == IDNO)
								{
									nStatus = CSSTATUS_UserAbort;
								}
							}
						}
					//}

					// If the user says abort, abort!
					if (nStatus == CSSTATUS_UserAbort)
					{
						break;
					}
				}
				if (nStatus == CSSTATUS_Success)
				{
					nCollectionsDownloaded++;
				}
			}
			else
			{
				// We could not read the DIR file.
				// See if the user wants to continue downloading files.
				if (AfxMessageBox(IDS_CantReadDirFile, MB_YESNO) == IDNO)
				{
					nStatus = CSSTATUS_UserAbort;
				}
			}
		}
		else
		{
			// We did not download the DIR file.
			if (nStatus != CSSTATUS_UserAbort)
			{
				// See if the user wants to continue downloading files.
				if (AfxMessageBox(IDS_CantDownloadDirFile, MB_YESNO) == IDNO)
				{
					nStatus = CSSTATUS_UserAbort;
				}
			}
		}

		// If the user says abort, abort!
		if (nStatus == CSSTATUS_UserAbort)
		{
			break;
		}
	}

	// Get of the dialog.
	Dialog.Hide();

	// If we successfully downloaded any collections...
	if (nCollectionsDownloaded > 0)
	{
		// ...inform any interested party that the content has changed.
		::PostMessage(HWND_BROADCAST, m_uContentChangedMessage, 0, 0);
	}

	return (nStatus == CSSTATUS_Success);
}

//
// Download a file.
//

int CDownloadManager::DownloadFile(LPCSTR pszSourceName, LPCSTR pszFileName, CCollectionConnectionCallback& ConnectionCallback)
{
	// Create the download callback.
	CCollectionDownloadCallback DownloadCallback(ConnectionCallback.GetDialog(), pszFileName);

	// Fill out the download info.
	CServerDownloadInfo DownloadInfo;
	DownloadInfo.m_csFile = pszSourceName;

	// Do the download.
	int nStatus = m_pContentManager->DownloadFile(DownloadInfo, &ConnectionCallback, &DownloadCallback);
	if (nStatus != CSSTATUS_Success)
	{
		// Get rid of any partially downloaded file.
		DownloadCallback.ZapFile();
	}
	return nStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionConnectionCallback
// The connection callback during a request.

CCollectionConnectionCallback::CCollectionConnectionCallback(CCollectionProgressDialog* pDialog)
{
	m_pDialog = pDialog;	
}

DWORD CCollectionConnectionCallback::OnStatusChange(DWORD dwInternetStatus)
{
	DWORD dwStatus = CSSTATUS_Success;

	if (m_pDialog != NULL)
	{
		UINT uID = 0;
#ifndef	ARTSTORE
		switch (dwInternetStatus)
		{
			case INTERNET_STATUS_CONNECTING_TO_SERVER:
			{
				// "Connecting to server..."
				uID = IDS_InternetConnecting;
				break;
			}
			case INTERNET_STATUS_SENDING_REQUEST:
			{
				// "Sending request..."
				uID = IDS_InternetSending;
				break;
			}
			case INTERNET_STATUS_RECEIVING_RESPONSE:
			{
				// "Receiving data..."
//				uID = IDS_InternetReceiving;
				break;
			}
			case INTERNET_STATUS_CLOSING_CONNECTION:
			{
				// "Closing connection..."
				uID = IDS_InternetDisconnecting;
				break;
			}
//			case INTERNET_STATUS_RESOLVING_NAME:
//			case INTERNET_STATUS_NAME_RESOLVED:
//			case INTERNET_STATUS_CONNECTED_TO_SERVER:
//			case INTERNET_STATUS_REQUEST_SENT:
//			case INTERNET_STATUS_RESPONSE_RECEIVED:
//			case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
//			case INTERNET_STATUS_PREFETCH:
//			case INTERNET_STATUS_CONNECTION_CLOSED:
//			case INTERNET_STATUS_HANDLE_CREATED:
//			case INTERNET_STATUS_HANDLE_CLOSING:
//			case INTERNET_STATUS_REQUEST_COMPLETE:
//			case INTERNET_STATUS_REDIRECT:
			default:
			{
				break;
			}
		}
#endif
		if (uID != 0)
		{
			CString csStatus;
			csStatus.LoadString(uID);
			m_pDialog->SetStatusText(csStatus);
		}

		// Check if we need to abort.
		if (m_pDialog->GetAbortStatus())
		{
			dwStatus = CSSTATUS_UserAbort;
		}
	}
	return dwStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CCollectionDownloadCallback
// The callback during a collection file download.

CCollectionDownloadCallback::CCollectionDownloadCallback(CCollectionProgressDialog* pDialog, LPCSTR pszFileName)
{
	m_pDialog = pDialog;
	m_csFileName = pszFileName;
	m_dwTotalRead = 0;
	m_pFile = NULL;
}

CCollectionDownloadCallback::~CCollectionDownloadCallback()
{
	// Get rid of the file if we opened it.
	CloseFile();
}

BOOL CCollectionDownloadCallback::CloseFile(void)
{
	if (m_pFile != NULL)
	{
		m_pFile->Close();
		delete m_pFile;
		return TRUE;
	}
	return FALSE;
}

//
// Get rid of the file if we wrote anything to it.
//

void CCollectionDownloadCallback::ZapFile(void)
{
	if (CloseFile())
	{
		TRY
			CFile::Remove(m_csFileName);
		END_TRY
	}
}

DWORD CCollectionDownloadCallback::OnReceiveData(LPVOID pData, DWORD dwDataSize, DWORD dwDataTotal)
{
	DWORD dwStatus = CSSTATUS_Success;

	if (m_pDialog != NULL)
	{
		if (dwDataSize == 0)
		{
			// First time!
			m_pDialog->SetStartTime(timeGetTime());
			// Update the progress dialog.
			m_pDialog->OnProgress(m_dwTotalRead, dwDataTotal);

			// Make sure the directory exists.
			CString csDirectory;
			Util::SplitPath(m_csFileName, &csDirectory, NULL);
			Util::MakeDirectory(csDirectory);

			// Create the file to read.
			m_pFile = new CFile;
			if (!m_pFile->Open(m_csFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive))
			{
				delete m_pFile;
				m_pFile = NULL;
			}
		}

		if (m_pFile == NULL)
		{
			// We could not open the file.
			dwStatus = CSSTATUS_WriteError;
		}
		else
		{
			// Write the data to the file.
			TRY
			{
				m_pFile->Write(pData, dwDataSize);
			}
			CATCH_ALL(e)
			{
				dwStatus = CSSTATUS_WriteError;
			}
			END_CATCH_ALL

			// Update how much has been read.
			m_dwTotalRead += dwDataSize;

			// Update the progress dialog.
			m_pDialog->OnProgress(m_dwTotalRead, dwDataTotal);

		}

		// Check if we need to abort.
		if (m_pDialog->GetAbortStatus())
		{
			dwStatus = CSSTATUS_UserAbort;
		}
	}

	return dwStatus;
}

