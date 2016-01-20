// GrapReq.cpp : implementation file
//

#include "stdafx.h"
#include "pmwinet.h"
#include "pmw.h"
#include "file.h"
#include "ConnMgr.h"


#include "progstat.h"
#include "BasReqIn.h"
#include "GrphReq.h"
#include "util.h"
#ifndef	ARTSTORE
#include "PGSSess.h"
#endif
#include "cdeflate.h"
#include "collfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetGraphicRequest

IMPLEMENT_DYNCREATE(CGetGraphicRequest, CBasicRequestInfo)

CGetGraphicRequest::CGetGraphicRequest()
{
	m_strFile = "";
	m_strLoginID = "";
	m_strPasswordID = "";
	m_strOutBuffer = "";
	m_pStatusDialog = NULL;
	m_fLastAttempt = FALSE;
	m_fGraphicIsAdvertisement = FALSE;
}

CGetGraphicRequest::CGetGraphicRequest(LPCSTR strFile, LPCSTR strLoginID, LPCSTR strPasswordID, LPCSTR strOutBuffer)
{
	m_strFile = strFile;
	m_strLoginID = strLoginID;
	m_strPasswordID = strPasswordID;
	m_strOutBuffer = strOutBuffer;
	m_pStatusDialog = NULL;
	m_fLastAttempt = FALSE;
	m_fGraphicIsAdvertisement = FALSE;

}

CGetGraphicRequest::~CGetGraphicRequest()
{
	DestroyStatusDialog();		// Just in case!
}

void CGetGraphicRequest::ProcessRequest(CString& pString)
{
	CString strServer = "http://";
	strServer += m_strFile;

	SetServerUrlAddress(strServer);

	CBasicRequestInfo::ProcessRequest(pString);

	CString strHeaders = "";
	UUEncodeHeaders(m_strLoginID, m_strPasswordID, strHeaders);
	strHeaders += "\r\n";
	SetHeaders(strHeaders);
}

UINT CGetGraphicRequest::DoRequest()
{
#ifdef ARTSTORE
	return 1;
#else
	DWORD   dwAccessType = PRE_CONFIG_INTERNET_ACCESS;

	UINT nRetCode = 6;  //default code for "try another server"

	CHttpConnection* pConnection = NULL;
	CHttpFile* pFile = NULL;

	CString SourceUrlFile;
	ProcessRequest(SourceUrlFile);

	// Create the download status dialog. Not shown yet.
	CreateStatusDialog();
	// Show it. This first connect can take a while.
	ShowStatusDialog();

   Util::YieldToWindows(); // Allow windows message processing

	// Create an internet session.
	CPGSSession InternetSession(_T("PGS"), dwAccessType);
	InternetSession.EnableStatusCallback(TRUE);

	// Set the status dialog.
	InternetSession.SetStatusDialog(m_pStatusDialog);

	TRY
	{
		// check to see if this is a reasonable URL

		CString strServerName;
		CString strObject;
		INTERNET_PORT nPort;
		DWORD dwServiceType;

		TRACE("Download item: %s\n", (LPCSTR)GetServerUrlAddress());
		if (!AfxParseURL(GetServerUrlAddress(), dwServiceType, strServerName, strObject, nPort) ||
			dwServiceType != INTERNET_SERVICE_HTTP)
		{
			// This is an internal server error!
			TRACE("Server Error: can only use URLs beginning with http:.");
			return nRetCode;			// Try the other server.
		}


		if(!m_pStatusDialog->GetAbortStatus())
		{
			// Get an HTTP connection.
			pConnection = InternetSession.GetHttpConnection(strServerName,  
															nPort,
															m_strLoginID,
															m_strPasswordID);
		}
		nRetCode = 0; 
		if (pConnection != NULL)
		{
			if(!m_pStatusDialog->GetAbortStatus())
			{
				// Open the request.
				pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject, NULL,
					NULL, NULL, NULL, INTERNET_FLAG_DONT_CACHE);
			}
			if (pFile != NULL)
			{

				if(!m_pStatusDialog->GetAbortStatus())
				{
					// Do the download.
					nRetCode = DoDownload(&InternetSession, pFile);
				}

				pFile->Close();
				delete pFile;
			}
			pConnection->Close();
			delete pConnection;
		}
	}
	CATCH (CInternetException, pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);

		if(pFile)
		{
			pFile->Close();
			delete pFile;
		}

		if(pConnection)
		{
			pConnection->Close();
			delete pConnection;
		}

		// Get rid of the download status dialog.
		DestroyStatusDialog();

		nRetCode = 2;
	}
	AND_CATCH_ALL(e)
	{
	}
	END_CATCH_ALL

	// Get rid of the download status dialog.
	DestroyStatusDialog();

	return nRetCode;
#endif
}

HRESULT
//CGetGraphicRequest::DoDownload(HINTERNET hSession)
CGetGraphicRequest::DoDownload(CPGSSession* pSession, CHttpFile* pFile)
{
#ifdef ARTSTORE
	return 1;
#else
	BOOL fAbort = FALSE;
	
	HRESULT hRet = 1;

	// Send the request.
	pFile->SendRequest();

	// Query for the length of the content.
	DWORD dwSize;
	DWORD dwBufferSize = sizeof(dwSize);
	if (pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH
							| HTTP_QUERY_FLAG_NUMBER,
							&dwSize,
							&dwBufferSize))
	{
		TRACE("Download data size: %ld\n", dwSize);
	}
	else
	{
		DWORD dwError = ::GetLastError();
		dwSize = 50000;		// Total guess.
	}

	DWORD dwRetCode = 400;
	if (pFile->QueryInfoStatusCode(dwRetCode))
	{
		if(dwRetCode >= 400)
		{
			TRACE("Http Error Status Code: %ld\n", dwRetCode);
			hRet = 2;
		}
	}
	else
	{
		DWORD dwError = ::GetLastError();
		dwSize = 50000;		// Total guess.
	}

	if(hRet != 2)
	{
		CCompressInfo CompressInfo;
		CompressInfo.Empty();

		LPVOID szBuff2;
		LPVOID szBufOut;

		//create the subdirectories if necessary
		CString strPath, strFile;
		Util::SplitPath(m_strOutBuffer, &strPath, &strFile);
		Util::MakeDirectory(strPath);

		StorageFile File(m_strOutBuffer);
		File.initialize();

		char Buffer[1024];

		DWORD dwTotalRead = 0;
		m_pStatusDialog->SetStartTime(timeGetTime());

		for (;;)
		{
			Util::YieldToWindows(); // Allow windows message processing

			if(m_pStatusDialog->GetAbortStatus())
			{
				fAbort = TRUE;
				break;
			}

			int nRead = 0;
			TRY
			{
				SetStatusProgress(dwTotalRead, dwSize);
				m_pStatusDialog->OnProgress(dwTotalRead, dwSize);

				nRead = pFile->Read(Buffer, sizeof(Buffer));
			}
			CATCH_ALL(e)
			{
				// PMGTODO: Find a better error to return.
				File.flush();	// Close it so we can...
				File.zap();		// ...get rid of the corrupt file
				return E_OUTOFMEMORY;
			}
			END_CATCH_ALL

			if (nRead <= 0)
			{
				// All done.
				break;
			}

			if (dwTotalRead == 0)
			{
				// First time. Save the compress info.
				CompressInfo = *(CCompressInfo*)Buffer;
			}

			// Write to the destination file.
			File.huge_write(Buffer, nRead);

			// Update the total.
			dwTotalRead += nRead;
		}

		if(fAbort == FALSE)
		{
			// Make sure the progress goes to 100%
			SetStatusProgress(dwSize, dwSize);

			if(CheckForBadHeader(&CompressInfo) || (dwTotalRead != CompressInfo.GetCompressedSize()+sizeof(CompressInfo)))
			{
				File.zap();
				if(m_fLastAttempt && !m_fGraphicIsAdvertisement)
				  AfxMessageBox(IDS_CORRUPT_DOWNLOAD);
			}
			else
			{
				szBuff2 = (LPVOID)(new char[CompressInfo.GetCompressedSize()]);
				szBufOut = (LPVOID)(new char[CompressInfo.GetUnCompressedSize()]);

				File.seek(sizeof(CCompressInfo), ST_DEV_SEEK_SET);
				File.huge_read(szBuff2, dwTotalRead);

				DecompressFile(&CompressInfo, szBuff2, szBufOut);

				File.seek(0, ST_DEV_SEEK_SET);
				File.initialize();
				File.huge_write(szBufOut, CompressInfo.GetUnCompressedSize());

				delete [] (char*)szBuff2;
				delete [] (char*)szBufOut;
				hRet = 0;
			}
		}
		else
		{
			File.zap();
			  return 110;
		}
	}

	return hRet;
#endif
}  // CDownload::DoDownload

BOOL CGetGraphicRequest::DecompressFile(CCompressInfo* pCompressInfo, void *pInBuffer, void* pOutBuffer)
{
   if (pCompressInfo != NULL)
   {
      switch (pCompressInfo->GetType())
      {
		 case CThumbnailDataHeader::COMPRESSION_ReducedDeflate:
		 {
			//fReduced24To8Bit = TRUE;
			// Fall through to...
		 }
		 case CThumbnailDataHeader::COMPRESSION_Deflate:
		 {
			// Decompress the data.
			CDeflate Deflator;
			LPBYTE pNewData = NULL;
			DWORD dwNewData = 0;
			void* pCompressedData = NULL;
			void* pDecompressedData = NULL;

			pCompressedData = pInBuffer;
			Deflator.Decompress((LPBYTE)pCompressedData, (DWORD)pCompressInfo->GetCompressedSize(), &pNewData, &dwNewData);
			ASSERT(dwNewData == (DWORD)pCompressInfo->GetUnCompressedSize());

			// Allocate memory for the destination.
			pDecompressedData = Util::HugeAlloc(dwNewData);

			// Use decompressed data.
			pInBuffer = pDecompressedData;

			Util::HMemCpy(pInBuffer, pNewData, dwNewData);
			Util::HMemCpy(pOutBuffer, pDecompressedData, dwNewData);
			Deflator.FreeData(pNewData);

			// If we allocated a decompression buffer, free it now.
			Util::HugeFree(pDecompressedData);
			pDecompressedData = NULL;
			break;
		 }
		 default:
		 {
			// Just copy the data.
			ASSERT(pCompressInfo->GetType() == CThumbnailDataHeader::COMPRESSION_None);
			ASSERT(pCompressInfo->GetUnCompressedSize() == pCompressInfo->GetCompressedSize());
			memcpy(pOutBuffer, pInBuffer, pCompressInfo->GetUnCompressedSize());
			break;
		 }
	  }
   }

   return TRUE;
}



BOOL CGetGraphicRequest::CheckForBadHeader(CCompressInfo* pCompressInfo)
{

	BOOL fRet = FALSE;

	if((pCompressInfo->GetType() != CThumbnailDataHeader::COMPRESSION_ReducedDeflate) &&
	   (pCompressInfo->GetType() != CThumbnailDataHeader::COMPRESSION_Deflate) &&
	   (pCompressInfo->GetType() != CThumbnailDataHeader::COMPRESSION_None) &&			
	   (pCompressInfo->GetType() != CThumbnailDataHeader::COMPRESSION_JPEG))
	{
		 fRet = TRUE;
	}

	if(pCompressInfo->GetCompressedSize() <= 0)
		fRet = TRUE;

	if(pCompressInfo->GetUnCompressedSize() <= 0)
		fRet = TRUE;

	return fRet;
}

//
// Create the download status dialog.
//

void CGetGraphicRequest::CreateStatusDialog(void)
{
	if (m_pStatusDialog == NULL)
	{
		m_pStatusDialog = new CDownloadStatusDialog(CWnd::GetSafeOwner(NULL, NULL));
	}
}

//
// Destroy the download status dialog, if we have one.
//

void CGetGraphicRequest::DestroyStatusDialog(void)
{
	if (m_pStatusDialog != NULL)
	{
		// Make me go away, and re-enable the parent.
		m_pStatusDialog->Hide();
		// Destroy the window.
		m_pStatusDialog->DestroyWindow();
		// Delete the pointer.
		delete m_pStatusDialog;
		// We no longer have a pointer.
		m_pStatusDialog = NULL;
	}
}

//
// Show the download status dialog, if we have one.
//

void CGetGraphicRequest::ShowStatusDialog(void)
{
	if (m_pStatusDialog != NULL)
	{
		m_pStatusDialog->Show();
	}
}

void CGetGraphicRequest::SetStatusProgress(int nCurrent, int nTotal)
{
	if (m_pStatusDialog != NULL)
	{
		m_pStatusDialog->SetProgress(nCurrent, nTotal);
	}
}
