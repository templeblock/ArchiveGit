/*
// $Workfile: CWMF.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/CWMF.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   04 Oct 1996 16:09:54   FRED
// Fixed some 16-bit code
// 
//    Rev 1.1   01 Oct 1996 10:38:14   FRED
//  
// 
//    Rev 1.0   11 Mar 1996 15:55:22   FRED
// Initial revision.
// 
//    Rev 1.1   02 Feb 1996 13:04:22   JAY
//  
// 
//    Rev 1.0   02 Feb 1996 08:58:26   JAY
// Initial revision.
// 
//    Rev 2.4   09 Nov 1995 12:28:16   JAY
// Removed an ASSERT which seemed to be wrong...
// 
//    Rev 2.3   15 Sep 1995 14:05:00   JAY
// More 32-bit changes.
// 
//    Rev 2.2   13 Sep 1995 11:48:12   JAY
// New 32-bit changes
// 
//    Rev 2.1   08 Feb 1995 13:36:06   JAY
// Reverted. New series.
// 
//    Rev 1.1   09 Nov 1994 14:42:30   FRED
//  
// 
//    Rev 1.0   03 Nov 1994 12:53:50   FRED
//  
// 
//    Rev 1.2   16 Sep 1994 10:46:38   FRED
// Got rid of Enum() method. Play() uses enumerate
// by default.
// 
// Added support for mask generation and color mapping.
// 
// 
//    Rev 1.1   15 Sep 1994 14:51:32   FRED
// Enhanced CWindowsMetaFile object.
// 
// 
//    Rev 1.0   15 Sep 1994 11:46:08   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "cwmf.h"

// These type definitions are used to create replacement records
// for metafile records that are changed. The data should not be
// changed directly because it may be read-only memory.

#if 0
// META_CREATEBRUSHINDIRECT

typedef struct
{
	DWORD rdSize;
	WORD rdFunction;
	LOGBRUSH lBrush;
} MBRUSHIND_RECORD;

// META_CREATEPENINDIRECT

typedef struct
{
	DWORD rdSize;
	WORD rdFunction;
	LOGPEN lPen;
} MPENIND_RECORD;

// META_SETBKCOLOR
// META_SETTEXTCOLOR

typedef struct
{
	DWORD rdSize;
	WORD rdFunction;
	COLORREF clColor;
} MCOLOR_RECORD;

// A record large enough to hold all the possible replacement records.

typedef union
{
	MBRUSHIND_RECORD mBrush;
	MPENIND_RECORD mPen;
	MCOLOR_RECORD mColor;
} REPLACE_RECORD;
#else
#ifdef WIN32
typedef struct
{
	WORD			lbStyle;
	COLORREF		lbColor;
	short int	lbHatch;
} LOGBRUSH16;

typedef struct
{
	WORD		lopnStyle;
	POINTS	lopnWidth;
	COLORREF	lopnColor;
} LOGPEN16;

#endif

/*
// Local structure and union definitions.
*/

typedef struct
{
	DWORD rdSize;
	WORD rdFunction;
#ifdef WIN32
	LOGBRUSH16 lBrush;
#else
	LOGBRUSH lBrush;
#endif
} MBRUSHIND_RECORD;

typedef struct
{
	DWORD rdSize;
	WORD rdFunction;
#ifdef WIN32
	LOGPEN16 lPen;
#else
	LOGPEN lPen;
#endif
} MPENIND_RECORD;

typedef struct
{
	DWORD rdSize;
	WORD rdFunction;
	COLORREF clColor;
} MCOLOR_RECORD;

typedef union
{
	MBRUSHIND_RECORD mBrush;
	MPENIND_RECORD mPen;
	MCOLOR_RECORD mColor;
} REPLACE_RECORD;

#endif

CWindowsMetaFile::CWindowsMetaFile()
{
	m_pszFileName = NULL;
	
	m_hInstance = NULL;
	m_pszResource = NULL;
	m_pszType = NULL;
		
	m_fHeaderValid = FALSE;
	
	m_hMetaFile = NULL;
	m_fMetaFileValid = FALSE;
}

CWindowsMetaFile::~CWindowsMetaFile()
{
	Reset();
}

void CWindowsMetaFile::Reset(void)
{
	m_pszFileName = NULL;
	
	m_hInstance = NULL;
	m_pszResource = NULL;
	m_pszType = NULL;
		
	m_fHeaderValid = FALSE;
	
	if (m_hMetaFile != NULL)
	{
		DeleteMetaFile(m_hMetaFile);
		m_hMetaFile = NULL;
	}
	
	m_fMetaFileValid = FALSE;
}


BOOL CWindowsMetaFile::Init(LPCSTR pszFileName)
{
	Reset();
	
	m_pszFileName = pszFileName;
	
	return GetHeader();
}

BOOL CWindowsMetaFile::Init(HINSTANCE hInstance, LPCSTR pszResource, LPCSTR pszType)
{
	Reset();
	
	m_hInstance = hInstance;
	m_pszResource = pszResource;
	m_pszType = pszType;
	
	return GetHeader();
}

const ALDUS_WMF_HEADER* CWindowsMetaFile::AldusHeader(void)
{
	return GetHeader() ? &m_AldusHeader : NULL;
}

const METAHEADER* CWindowsMetaFile::Header(void)
{
	return GetHeader() ? &m_Header : NULL;
}

BOOL CWindowsMetaFile::Play(CDC* pDC, BOOL fMask /*=FALSE*/, LPARAM lParam /*=0*/)
{
	BOOL fSuccess = FALSE;
	
	ASSERT(pDC != NULL);
	
	if (GetMetaFile())
	{
		EnumData FAR* pData = new EnumData;
		pData->pObject = this;
		pData->pDC = pDC;
		pData->fMask = fMask;
		pData->lParam = lParam;
		
		fSuccess = EnumMetaFile(pDC->GetSafeHdc(), m_hMetaFile, EnumMetaFileDispatch, (LPARAM)pData);
		
		delete pData;
	}
	
	return fSuccess;
}

#ifdef WIN32
BOOL CALLBACK CWindowsMetaFile::EnumMetaFileDispatch(HDC hDC, HANDLETABLE FAR* pObject, METARECORD FAR* pMetaRecord, int nObject, LPARAM lParam)
#else
BOOL _export CALLBACK CWindowsMetaFile::EnumMetaFileDispatch(HDC hDC, HANDLETABLE FAR* pObject, METARECORD FAR* pMetaRecord, int nObject, LPARAM lParam)
#endif
{
	return (((EnumData*)lParam)->pObject)->EnumMetaFileProc(pObject, pMetaRecord, nObject, (EnumData*)lParam);
}

BOOL CWindowsMetaFile::EnumMetaFileProc(HANDLETABLE FAR* pObject, METARECORD FAR* pMetaRecord, int nObject, EnumData* pData)
{
	ASSERT(pData->pDC != NULL);
	
	HDC hDC = pData->pDC->GetSafeHdc();
		
	REPLACE_RECORD Record;
	
	// Do color mapping for certain records.
	
	switch (pMetaRecord->rdFunction)
	{
		case META_CREATEBRUSHINDIRECT:
		{
			Record.mBrush = *(MBRUSHIND_RECORD*)pMetaRecord;
			Record.mBrush.lBrush.lbColor = MapColor(Record.mBrush.lBrush.lbColor);
			pMetaRecord = (METARECORD*)&Record;
			break;
		}
		case META_CREATEPENINDIRECT:
		{
			Record.mPen = *(MPENIND_RECORD*)pMetaRecord;
			Record.mPen.lPen.lopnColor = MapColor(Record.mPen.lPen.lopnColor);
			pMetaRecord = (METARECORD*)&Record;
			break;
		}
		case META_SETBKCOLOR:
		case META_SETTEXTCOLOR:
		{
			Record.mColor = *(MCOLOR_RECORD*)pMetaRecord;
			Record.mColor.clColor = MapColor(Record.mColor.clColor);
			pMetaRecord = (METARECORD*)&Record;
			break;
		}
	}
	
	// If the mask flag is set, set things up to force all drawing in black.
	
	if (pData->fMask)
	{
		SetROP2(hDC, R2_BLACK);
		SetTextColor(hDC, RGB(0,0,0));
		SetBkColor(hDC, RGB(255,255,255));
	}
	
	PlayMetaFileRecord(hDC, pObject, pMetaRecord, nObject);
	
	return TRUE;
}

COLORREF CWindowsMetaFile::MapColor(COLORREF clColor)
{
	return clColor;
}

BOOL CWindowsMetaFile::GetHeader(void)
{
	if (!m_fHeaderValid)
	{
		if (m_pszFileName != NULL)
		{
			m_fHeaderValid = GetMetaFile(m_pszFileName, &m_AldusHeader, &m_Header, NULL);
		}
		
		else if (m_hInstance != NULL)
		{
			m_fHeaderValid = GetMetaFile(m_hInstance, m_pszResource, m_pszType, &m_AldusHeader, &m_Header, NULL);
		}
	}
	
	return m_fHeaderValid;
}

BOOL CWindowsMetaFile::GetMetaFile(void)
{
	if (!m_fMetaFileValid)
	{
		if (m_pszFileName != NULL)
		{
			m_fMetaFileValid = GetMetaFile(m_pszFileName, NULL, NULL, &m_hMetaFile);
		}
		
		else if (m_hInstance != NULL)
		{
			m_fMetaFileValid = GetMetaFile(m_hInstance, m_pszResource, m_pszType, NULL, NULL, &m_hMetaFile);
		}
	}
	
	return m_fMetaFileValid;
}

BOOL CWindowsMetaFile::GetMetaFile(LPCSTR pszFileName, ALDUS_WMF_HEADER* pAldusHeader, METAHEADER* pHeader, HMETAFILE* phMetaFile)
{
	BOOL fSuccess = FALSE;
	
	ASSERT(pszFileName != NULL);
	
	// Create the file we will be using.

	CFile File;

	// Attempt to open the file.

	if (File.Open(pszFileName, CFile::modeRead | CFile::shareDenyWrite))
	{
		ALDUS_WMF_HEADER AldusHeader;

		// Read the Aldus header.

		if (File.Read(&AldusHeader, sizeof(AldusHeader)) == sizeof(AldusHeader))
		{
			// Save the Aldus header if the user has requested it.
					
			if (pAldusHeader != NULL)
			{
				*pAldusHeader = AldusHeader;
			}
						
			// Validate the header.

			if (AldusHeader.key == ALDUS_WMF_KEY)
			{
				// If the user has only requested the Aldus header, stop and return TRUE.
				
				fSuccess = (pHeader == NULL) && (phMetaFile == NULL);
				
				if (!fSuccess)
				{
					// Read the metafile header.
	
					METAHEADER Header;
	
					DWORD dwPosition = File.GetPosition();
	
					if (File.Read(&Header, sizeof(Header)) == sizeof(Header))
					{
						// Save the metafile header if the user has requested it.
							
						if (pHeader != NULL)
						{
							*pHeader = Header;
						}
						
						// If the user does not want a metafile handle, stop and return TRUE.
						
						fSuccess = phMetaFile == NULL;
								
						if (!fSuccess)
						{
							*phMetaFile = NULL;
								
							// Get the size from the metafile header.
							
							DWORD dwSize = Header.mtSize*2;
							
							if (dwSize != 0)
							{
								// Allocate the memory to hold the metafile data.
									
								HGLOBAL hMetaFileData;
									
								hMetaFileData = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, dwSize);
									
								if (hMetaFileData != NULL)
								{
									LPVOID pMetaFileData;
										
									pMetaFileData = GlobalLock(hMetaFileData);
										
									if (pMetaFileData != NULL)
									{
										// Seek back to the start of the metafile header.
										
										File.Seek(dwPosition, CFile::begin);

										// Read the metafile data.
										
										if (File.ReadHuge(pMetaFileData, dwSize) == dwSize)
										{
											// We have now read the data, make a metafile from it.
	
#ifdef WIN32
											*phMetaFile = ::SetMetaFileBitsEx(dwSize, (LPBYTE)pMetaFileData);
#else
											*phMetaFile = ::SetMetaFileBits(hMetaFileData);
#endif
											fSuccess = *phMetaFile != NULL;
										}

#ifdef WIN32
										GlobalUnlock(hMetaFileData);
										pMetaFileData = NULL;
#else
										if (*phMetaFile == NULL)
										{
											GlobalUnlock(hMetaFileData);
											pMetaFileData = NULL;
										}
#endif
									}
#ifdef WIN32
									GlobalFree(hMetaFileData);
									hMetaFileData = NULL;
#else
									if (*phMetaFile == NULL)
									{
										GlobalFree(hMetaFileData);
										hMetaFileData = NULL;
									}
#endif
								}
							}
						}
					}
				}
			}
		}
	}

	return fSuccess;
}

BOOL CWindowsMetaFile::GetMetaFile(HINSTANCE hInstance, LPCSTR pszResource, LPCSTR pszType, ALDUS_WMF_HEADER* pAldusHeader, METAHEADER* pHeader, HMETAFILE* phMetaFile)
{
	BOOL fSuccess = FALSE;
	
	ASSERT(hInstance != NULL);
	ASSERT(pszResource != NULL);
	ASSERT(pszType != NULL);
								
	HRSRC hrResource;
	
	hrResource = FindResource(hInstance, pszResource, pszType);
	
	if (hrResource != NULL)
	{
		HGLOBAL hResource;
		
		hResource = LoadResource(hInstance, hrResource);
		
		if (hResource != NULL)
		{
			LPBYTE pResource;

#ifdef WIN32
			DWORD dwGlobalSize = 0;
#else
			DWORD dwGlobalSize = GlobalSize(hResource);
#endif

			pResource = (LPBYTE)LockResource(hResource);

			if (pResource != NULL)
			{
				ASSERT((dwGlobalSize == 0) || (dwGlobalSize >= sizeof(ALDUS_WMF_HEADER)+sizeof(METAHEADER)));
				if ((dwGlobalSize == 0) || (dwGlobalSize >= sizeof(ALDUS_WMF_HEADER)+sizeof(METAHEADER)))
				{
					// Save the Aldus header if the user has requested it.
					
					if (pAldusHeader != NULL)
					{
						*pAldusHeader = *((ALDUS_WMF_HEADER*)pResource);
					}
						
					// Validate the Aldus header.
					
					if (((ALDUS_WMF_HEADER*)pResource)->key == ALDUS_WMF_KEY)
					{
						// Save the metafile header if the user has requested it.
						
						if (pHeader != NULL)
						{
							*pHeader = *((METAHEADER*)(pResource+sizeof(ALDUS_WMF_HEADER)));
						}
						
						// Get the size from the metafile header.
						
						DWORD dwSize = ((METAHEADER*)(pResource+sizeof(ALDUS_WMF_HEADER)))->mtSize*2;
						
						if (dwGlobalSize != 0 && dwGlobalSize < sizeof(ALDUS_WMF_HEADER)+dwSize)
						{
							// This can be cause by the WMF files where mtSize includes the
							// size of the Aldus header. Attempt to adjust for this by
							// decreasing mtSize.
													
							dwSize -= sizeof(ALDUS_WMF_HEADER);
						}
							
						if (dwSize != 0)
						{
							ASSERT((dwGlobalSize == 0) || (dwGlobalSize >= sizeof(ALDUS_WMF_HEADER)+dwSize));
							if ((dwGlobalSize == 0) || (dwGlobalSize >= sizeof(ALDUS_WMF_HEADER)+dwSize))
							{
								// If the user wants a metafile handle, continue.
								// Otherwise, return TRUE.
								
								fSuccess = phMetaFile == NULL;
								
								if (!fSuccess)
								{
									*phMetaFile = NULL;
									
									// Allocate the memory to hold the metafile data.
									
									HGLOBAL hMetaFileData;
									
									hMetaFileData = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, dwSize);
									
									if (hMetaFileData != NULL)
									{
										LPVOID pMetaFileData;
										
										pMetaFileData = GlobalLock(hMetaFileData);
										
										if (pMetaFileData != NULL)
										{
											// Copy the meta file data from the resource into the new memory block.
											// We have now read the data, make a metafile from it.
		
#ifdef WIN32
											memcpy(pMetaFileData, (LPVOID)(pResource+sizeof(ALDUS_WMF_HEADER)), dwSize);
											*phMetaFile = ::SetMetaFileBitsEx(dwSize, (LPBYTE)pMetaFileData);
											GlobalUnlock(hMetaFileData);
											pMetaFileData = NULL;
#else
											hmemcpy(pMetaFileData, (LPVOID)(pResource+sizeof(ALDUS_WMF_HEADER)), dwSize);
											*phMetaFile = ::SetMetaFileBits(hMetaFileData);
										
											if (*phMetaFile == NULL)
											{
												GlobalUnlock(hMetaFileData);
												pMetaFileData = NULL;
											}
#endif
	
											fSuccess = *phMetaFile != NULL;
										}
									
#ifdef WIN32
										GlobalFree(hMetaFileData);
										hMetaFileData = NULL;
#else
										if (*phMetaFile == NULL)
										{
											GlobalFree(hMetaFileData);
											hMetaFileData = NULL;
										}
#endif
									}
								}
							}
						}
					}
					
					GlobalUnlock(hResource);
					pResource = NULL;
				}
			}
			
			FreeResource(hResource);
			hResource = NULL;
		}
	}
	
	return fSuccess;
}
