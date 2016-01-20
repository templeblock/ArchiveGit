// filcmprs.cpp : implementation of the CFileCompress class & dependants
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "filcmprs.h"

#ifndef __CDEFLATE_H__
#include "cdeflate.h"
#endif

const N_INITIAL_FILES = 24;

CFileCompress::CFileCompress()
{
	m_aFileList.SetSize(N_INITIAL_FILES);
	m_nFiles = 0;
	m_fCompressionHeader = FALSE;
}

void CFileCompress::InitFileList(enum OperateType operation, CString& csPath, CStringArray& aFilenames, int nFiles)
{
	int i;
	m_nFiles = nFiles;
	m_csPath = csPath;
	for (i = 0; i < nFiles; i++)
	{
		SFileState stFileState;
		stFileState.csFilename = aFilenames[i];
		stFileState.operation = operation;
		stFileState.state = CLEAN;

		m_aFileList.SetAtGrow(i, stFileState);
	}
}

// Compression/decompression
// Use CDeflate to handle dirty work
BOOL CFileCompress::StartOperation(int nIndex)
{
	// Indicate processing is beginning
	if (nIndex >= m_nFiles)
		return FALSE;

	m_aFileList[nIndex].state = PROCESSING;
	return TRUE;
}

void CFileCompress::PerformOperation(int nIndex)
{
	// Actually do the operation
	int nStatus;
	switch (m_aFileList[nIndex].operation)
	{
	case COMPRESS:
		nStatus = Compress(m_aFileList[nIndex].csFilename);
		break;
	case DECOMPRESS:
		nStatus = Decompress(m_aFileList[nIndex].csFilename);
		break;
	case WRITE_HEADER:
		nStatus = WriteHeader(m_aFileList[nIndex].csFilename);
		break;
	default:
		break;
	}

	// Change the state
	m_aFileList[nIndex].state = (nStatus) ? PROCESSED : PROCESS_ERROR;
}

void CFileCompress::GetProcessString(int nIndex, CString& csProcessString)
{
	// Check bound
	csProcessString.Empty();
	if (nIndex > m_nFiles)
		return;
	
	// Operation string
	static char* szPrewords[] = {
		"Compress",
		"Decompress",
		"Head"
	};
	CString csOperation = szPrewords[m_aFileList[nIndex].operation];

	// Use the state to generate a description
	switch (m_aFileList[nIndex].state)
	{
	case PROCESSING:
		csProcessString = csOperation + "ing '" + m_aFileList[nIndex].csFilename + "'...";
		break;
	case PROCESSED:
		csProcessString = csOperation + "ed '" + m_aFileList[nIndex].csFilename + "'.";
		break;
	case PROCESS_ERROR:
		csProcessString = "ERROR " + csOperation + "ing '" + m_aFileList[nIndex].csFilename + "'.";
		break;
	}
}

// Compression
int CFileCompress::Compress(CString& csFilename)
{
	CString csDestination;
	BuildDestinationFilename(csFilename, csDestination);

	CStdioFile srcFile, dstFile;
	srcFile.SetFilePath(m_csPath);
	dstFile.SetFilePath(m_csPath);
	if (!srcFile.Open(csFilename, CFile::modeRead | CFile::typeBinary) ||
		!dstFile.Open(csDestination, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{    
#ifdef _DEBUG
        afxDump << "File could not be opened " << "\n";
		return FALSE;
#endif
	}
	LPBYTE ppDestination;
	DWORD pdwDestination;

	return (SourceCompress(&srcFile, &dstFile, &ppDestination, &pdwDestination, FALSE));
}

int CFileCompress::Compress(CString& csFilename, LPBYTE* ppDestination, DWORD* pdwDestination)
{
	CStdioFile srcFile;
	srcFile.SetFilePath(m_csPath);
	if (!srcFile.Open(csFilename, CFile::modeRead | CFile::typeBinary))
	{    
#ifdef _DEBUG
        afxDump << "File could not be opened " << "\n";
#endif
		return FALSE;
	}

	return (SourceCompress(&srcFile, NULL, ppDestination, pdwDestination, TRUE));
}

int CFileCompress::Compress(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType, LPBYTE* ppDestination, DWORD* pdwDestination)
{
	// Open the resource
	DWORD dwSize;
	LPVOID pSource = LoadCompressionResource(hModule, lpName, lpType, dwSize);
	if (pSource != NULL)
	{
		CDeflate deflate;

		// Do the compression
		return (deflate.Compress((LPBYTE)pSource, dwSize, ppDestination, pdwDestination));
	}
	return 0;
}

// Decompression
int CFileCompress::Decompress(CString& csFilename)
{
	CString csDestination;
	BuildDestinationFilename(csFilename, csDestination);
	
	CStdioFile srcFile, dstFile;
	srcFile.SetFilePath(m_csPath);
	dstFile.SetFilePath(m_csPath);
	if (!srcFile.Open(csFilename, CFile::modeRead | CFile::typeBinary) ||
		!dstFile.Open(csDestination, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{    
#ifdef _DEBUG
        afxDump << "File could not be opened " << "\n";
#endif
		return FALSE;
	}
	LPBYTE ppDestination;
	DWORD pdwDestination;

	return (SourceDecompress(&srcFile, &dstFile, &ppDestination, &pdwDestination, FALSE));
}

int CFileCompress::Decompress(CString& csFilename, LPBYTE* ppDestination, DWORD* pdwDestination)
{
	CStdioFile srcFile;
	srcFile.SetFilePath(m_csPath);
	if (!srcFile.Open(csFilename, CFile::modeRead | CFile::typeBinary))
	{    
#ifdef _DEBUG
        afxDump << "File could not be opened " << "\n";
#endif
		return FALSE;
	}

	return (SourceDecompress(&srcFile, NULL, ppDestination, pdwDestination, TRUE));
}

int CFileCompress::Decompress(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType, LPBYTE* ppDestination, DWORD* pdwDestination)
{
	// Open the resource
	DWORD dwSize;
	LPVOID pSource = LoadCompressionResource(hModule, lpName, lpType, dwSize);
	if (pSource != NULL)
	{
		CDeflate deflate;

		// Do the decompression
		return (deflate.Decompress((LPBYTE)pSource, dwSize, ppDestination, pdwDestination));
	}
	return 0;
}

int CFileCompress::WriteHeader(CString& csFilename)
{
	CString csDestination;
	BuildDestinationFilename(csFilename, csDestination);

	CStdioFile srcFile, dstFile;
	srcFile.SetFilePath(m_csPath);
	dstFile.SetFilePath(m_csPath);
	if (!srcFile.Open(csFilename, CFile::modeRead | CFile::typeBinary) ||
		!dstFile.Open(csDestination, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{    
#ifdef _DEBUG
        afxDump << "File could not be opened " << "\n";
		return FALSE;
#endif
	}
	LPBYTE ppDestination;
	DWORD pdwDestination;

	return (SourceWriteHeader(&srcFile, &dstFile, &ppDestination, &pdwDestination, FALSE));
}

int CFileCompress::WriteHeader(CString& csFilename, LPBYTE* ppDestination, DWORD* pdwDestination)
{
	CStdioFile srcFile;
	srcFile.SetFilePath(m_csPath);
	if (!srcFile.Open(csFilename, CFile::modeRead | CFile::typeBinary))
	{    
#ifdef _DEBUG
        afxDump << "File could not be opened " << "\n";
#endif
		return FALSE;
	}

	return (SourceWriteHeader(&srcFile, NULL, ppDestination, pdwDestination, TRUE));
}


void CFileCompress::FreeData(LPBYTE pData)
{
	// Free buffer
	CDeflate deflate;
	deflate.FreeData(pData);
}

// Hidden
void CFileCompress::BuildDestinationFilename(CString& csSrc, CString& csDest)
{
	csDest = csSrc.SpanExcluding(".");
	
	// Get an extension string: ".ext" --> "..ex" --> "._ex"
	CString csRawExt = "." + csSrc.Right(csSrc.GetLength() - csDest.GetLength());
	CString csCompressExt = csRawExt.Left(csRawExt.GetLength()-1);
	csCompressExt.SetAt(1, '_');
	csDest += csCompressExt;
}

LPVOID CFileCompress::LoadCompressionResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType, DWORD& dwSize)
{
	// Load the resource to be used for compression/decompression
	HMODULE hRMod = (hModule != NULL) ? hModule : AfxGetResourceHandle();
	HRSRC hResInfo = ::FindResource(hRMod, lpName, lpType);
	dwSize = ::SizeofResource(hRMod, hResInfo);
	HGLOBAL hGlobal = ::LoadResource(hRMod, hResInfo);
	return (::LockResource(hGlobal));
}

int CFileCompress::SourceCompress(CStdioFile* pSrcFile, CStdioFile* pDstFile, LPBYTE* ppDestination, DWORD* pdwDestination, BOOL bUserBuf)
{	
	CDeflate deflate;
	int status = 0;

	// Put the file in memory
	DWORD nLength = pSrcFile->GetLength();
	LPBYTE pSource = (LPBYTE)new char[nLength];
	pSrcFile->Read((void *)pSource, nLength);

	// Do the compression
	if (deflate.Compress((LPBYTE)pSource, nLength, ppDestination, pdwDestination))
	{
		if (pDstFile != NULL)
		{
			if(m_fCompressionHeader)
			{
				CFileCompressInfo ci;
				ci.Empty();
				/*		
						COMPRESSION_None			= 0,
						COMPRESSION_JPEG			= 1,
						COMPRESSION_Deflate			= 2,
				*/
				ci.Set( (long)*pdwDestination, (long)nLength, 2);
				pDstFile->Write((const void*)&ci, sizeof(CFileCompressInfo));
			}
			
			pDstFile->Write((const void*)*ppDestination, (UINT)*pdwDestination);
		}
		status = 1;
	}
	delete [] pSource;
	if (!bUserBuf)
	{
		deflate.FreeData(*ppDestination);
	}
	return status;
}

int CFileCompress::SourceDecompress(CStdioFile* pSrcFile, CStdioFile* pDstFile, LPBYTE* ppDestination, DWORD* pdwDestination, BOOL bUserBuf)
{	
	CDeflate deflate;
	int status = 0;

	// Put the file in memory
	DWORD nLength = pSrcFile->GetLength();
	LPBYTE pSource = (LPBYTE)new char[nLength];
	int nSeekLength = 0;

	if(m_fCompressionHeader)
	{
		pSrcFile->Seek(sizeof(CFileCompressInfo), CFile::begin);
		nSeekLength = sizeof(CFileCompressInfo);
	}
	
	pSrcFile->Read((void *)pSource, nLength - nSeekLength);

	// Do the decompression
	if (deflate.Decompress((LPBYTE)pSource, nLength, ppDestination, pdwDestination))
	{
		if (pDstFile != NULL )
		{
			if ((UINT)*pdwDestination != 0)
			{
				pDstFile->Write((const void*)*ppDestination, (UINT)*pdwDestination);
			}
			else
			{
				pDstFile->Write((const void*)pSource, nLength - nSeekLength);
			}
		}
		status = 1;
	}
	delete [] pSource;
	if (!bUserBuf)
	{
		deflate.FreeData(*ppDestination);
	}
	return status;
}

int CFileCompress::SourceWriteHeader(CStdioFile* pSrcFile, CStdioFile* pDstFile, LPBYTE* ppDestination, DWORD* pdwDestination, BOOL bUserBuf)
{	
	int status = 0;

	// Put the file in memory
	DWORD nLength = pSrcFile->GetLength();
	LPBYTE pSource = (LPBYTE)new char[nLength];
	pSrcFile->Read((void *)pSource, nLength);

	if (pDstFile != NULL)
	{
		if(m_fCompressionHeader)
		{
			CFileCompressInfo ci;
			ci.Empty();
			ci.Set( (long)nLength, (long)nLength, 0);
			pDstFile->Write((const void*)&ci, sizeof(CFileCompressInfo));
		}
		
		pDstFile->Write((const void*)pSource, (UINT)nLength);
	}
	status = 1;

	delete [] pSource;
	return status;
}

// CBitmapFileCompress
// Class for compression/decompression of bitmap resources
int CBitmapFileCompress::Decompress(HMODULE hModule, LPCTSTR lpName, LPBYTE* ppDestination, DWORD* pdwDestination)
{
	// Open the resource
	DWORD dwSize;
	LPBYTE pSource = (LPBYTE)LoadCompressionResource(hModule, lpName, "COMPRESSEDBITMAP", dwSize);
	if (pSource != NULL)
	{
		CDeflate deflate;

		// Do the decompression
		if (deflate.Decompress(pSource, dwSize, ppDestination, pdwDestination))
		{
			// Strip off the header
			memmove((void*)*ppDestination, (const void*)(*ppDestination + sizeof(BITMAPFILEHEADER)), *pdwDestination - sizeof(BITMAPFILEHEADER));
			return 1;
		}
		return 0;
	}
	return 0;
}
