/*
// PCD.CPP
//
// Routines for reading a Kodak Photo CD file.
//
// $Header: /PM8/App/PCD.CPP 1     3/03/99 6:08p Gbeddow $
//
// $Log: /PM8/App/PCD.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:23:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:40   Fred
// Initial revision.
// 
//    Rev 1.3   11 Apr 1997 11:23:34   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.2   05 Dec 1996 12:30:22   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   25 Aug 1996 12:59:22   Jay
// No longer sets default name at low level.
// 
//    Rev 1.0   14 Mar 1996 13:45:02   Jay
// Initial revision.
// 
//    Rev 1.1   07 Mar 1996 12:47:12   VSI
// PhotoCDLibrary disabled for _MAC build
// 
//    Rev 1.0   16 Feb 1996 12:10:08   FRED
// Initial revision.
// 
//    Rev 1.2   22 Jan 1996 13:02:32   JAY
// Fixed calling-convention problem with 32-bit version.
// 
//    Rev 1.1   21 Sep 1995 15:12:06   JAY
// 32-bit changes
// 
//    Rev 1.0   18 Aug 1995 15:04:56   JAY
// Initial revision.
*/

#include	"stdafx.h"

#include <memory.h>

#include "pmgdb.h"
#include	"grafrec.h"
#include	"grafobj.h"
#include	"pcd.h"
//#include	"pcdlib.h"

/////////////////////////////////////////////////////////////////////////////
// PCDLibrary
//
// This is the interface to the PCDLIB.DLL library. This allows a dynamic
// link to the library instead of the normal static one (which can fail if
// the library is unavailable).
//

/*
// The Photo CD library.
*/

#ifdef WIN32
#define PCDENTRY FAR
#else
#define PCDENTRY FAR PASCAL
#endif

class CPCDLibrary
{
	typedef PCDstatus (PCDENTRY *FN_PCDopen)(LPSTR lpPath, PCDphotoHdl FAR *lphPCD);
	typedef PCDstatus (PCDENTRY *FN_PCDclose)(PCDphotoHdl hPCD);
	typedef PCDstatus (PCDENTRY *FN_PCDreadImageInfo)(PCDphotoHdl hPCD, PCDpacInfoPtr pInfo);
	typedef PCDstatus (PCDENTRY *FN_PCDgetCount)(PCDphotoHdl hPCD, unsigned long FAR *lpRes, short FAR *lpResCount);
	typedef PCDstatus (PCDENTRY *FN_PCDsetResolution)(PCDphotoHdl hPCD, PCDresolution nRes);
#ifdef WIN32
	typedef PCDstatus (PCDENTRY *FN_PCDgetSize)(PCDphotoHdl hPCD, PCDrect* lpBoundRect);
	typedef PCDstatus (PCDENTRY *FN_PCDgetBlock)(PCDphotoHdl hPCD, PCDrect* lpBoundRect, PCDRAWDATA hpBuffer, long lStride);
#else
	typedef PCDstatus (PCDENTRY *FN_PCDgetSize)(PCDphotoHdl hPCD, LPRECT lpBoundRect);
	typedef PCDstatus (PCDENTRY *FN_PCDgetBlock)(PCDphotoHdl hPCD, LPRECT lpBoundRect, PCDRAWDATA hpBuffer, long lStride);
#endif

public:
	CPCDLibrary();
	~CPCDLibrary();

	PCDstatus Open(LPCSTR lpPath, PCDphotoHdl FAR *lphPCD);
	PCDstatus Close(PCDphotoHdl hPCD);
	PCDstatus ReadImageInfo(PCDphotoHdl hPCD, PCDpacInfoPtr pInfo);
	PCDstatus GetCount(PCDphotoHdl hPCD, unsigned long FAR *lpRes, short FAR *lpResCount);
	PCDstatus SetResolution(PCDphotoHdl hPCD, PCDresolution nRes);
	PCDstatus GetSize(PCDphotoHdl hPCD, LPRECT lpBoundRect);
	PCDstatus GetBlock(PCDphotoHdl hPCD, LPRECT lpBoundRect, PCDRAWDATA hpBuffer, long lStride);

protected:

/* Simple startup. Fail if PCDLIB.DLL not available. */
	BOOL Startup(void);
	void Shutdown(void);
	BOOL IsValid(void);
	void GetEntryPoints(void);

	BOOL m_fValid;
	BOOL m_fAvailable;
	HINSTANCE m_hLibrary;

	FN_PCDopen m_PCDopen;
	FN_PCDclose m_PCDclose;
	FN_PCDreadImageInfo m_PCDreadImageInfo;
	FN_PCDgetCount m_PCDgetCount;
	FN_PCDsetResolution m_PCDsetResolution;
	FN_PCDgetSize m_PCDgetSize;
	FN_PCDgetBlock m_PCDgetBlock;
};

CPCDLibrary::CPCDLibrary()
{
	m_hLibrary = 0;
	m_fAvailable = TRUE;					// Assume it is until we know otherwise.
}

CPCDLibrary::~CPCDLibrary()
{
	Shutdown();
}

void CPCDLibrary::Shutdown(void)
{
	if (m_hLibrary >= (HINSTANCE)HINSTANCE_ERROR)
	{
		::FreeLibrary(m_hLibrary);
		m_hLibrary = NULL;
	}
}

BOOL CPCDLibrary::IsValid(void)
{
#ifdef _MAC
   //TODO: MAC - figure out how to handle photo CD
   AfxMessageBox("CPCDLibrary::IsValid PhotoCD not supported");
   return FALSE;
#endif

	od("IsValid(): Enter...\r\n");
/*
// If the library has been loaded, we are definitely OK.
// Return TRUE immediately.
*/
	if (m_hLibrary >= (HINSTANCE)HINSTANCE_ERROR)
	{
	/* Valid! */
		return TRUE;
	}
/*
// The library is not loaded. If we have already tried to load it (and
// obviously failed), we don't try anymore.
*/

	if (!m_fAvailable)
	{
	/* We don't keep trying. */
		return FALSE;
	}

/*
// The library has not been loaded and is believed to be available.
// Try to load it now.
// The library is assumed to be in the main parent directory.
*/

	// Make a local file so we can get its full name.

#ifdef WIN32
	LPCSTR pName = StorageFile::TranslateFileName("PCDLIB32.DLL", FALSE);
#else
	LPCSTR pName = StorageFile::TranslateFileName("PCDLIB.DLL", FALSE);
#endif
	if (pName != NULL)
	{
	/* We now have the full name. Do the load. */
		UINT wOldSem = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
		od("IsValid(): LoadLibrary(\"%s\")...\r\n", pName);
		m_hLibrary = ::LoadLibrary(pName);
		::SetErrorMode(wOldSem);

		if (m_hLibrary >= (HINSTANCE)HINSTANCE_ERROR)
		{
		/* We got the library. Get the entry points. */
			od("IsValid(): GetEntryPoints()...\r\n");
			GetEntryPoints();
		}
	}

	if (m_hLibrary >= (HINSTANCE)HINSTANCE_ERROR)
	{
	/* We were successful! */
		od("IsValid(): Success()...\r\n");
		return TRUE;
	}
	else
	{
	/* We failed to load. Don't bother again in the future. */
		od("IsValid(): Failed()...\r\n");
		m_fAvailable = FALSE;
		return FALSE;
	}
}

/*
// Get all the entry points.
*/

void CPCDLibrary::GetEntryPoints(void)
{
#ifdef WIN32
	m_PCDopen = (FN_PCDopen)GetProcAddress(m_hLibrary, "PCDopen");
	m_PCDclose = (FN_PCDclose)GetProcAddress(m_hLibrary, "PCDclose");
	m_PCDreadImageInfo = (FN_PCDreadImageInfo)GetProcAddress(m_hLibrary, "PCDreadImageInfo");
	m_PCDgetCount = (FN_PCDgetCount)GetProcAddress(m_hLibrary, "PCDgetCount");
	m_PCDsetResolution = (FN_PCDsetResolution)GetProcAddress(m_hLibrary, "PCDsetResolution");
	m_PCDgetSize = (FN_PCDgetSize)GetProcAddress(m_hLibrary, "PCDgetSize");
	m_PCDgetBlock = (FN_PCDgetBlock)GetProcAddress(m_hLibrary, "PCDgetBlock");
#else
	m_PCDopen = (FN_PCDopen)GetProcAddress(m_hLibrary, "PCDOPEN");
	m_PCDclose = (FN_PCDclose)GetProcAddress(m_hLibrary, "PCDCLOSE");
	m_PCDreadImageInfo = (FN_PCDreadImageInfo)GetProcAddress(m_hLibrary, "PCDREADIMAGEINFO");
	m_PCDgetCount = (FN_PCDgetCount)GetProcAddress(m_hLibrary, "PCDGETCOUNT");
	m_PCDsetResolution = (FN_PCDsetResolution)GetProcAddress(m_hLibrary, "PCDSETRESOLUTION");
	m_PCDgetSize = (FN_PCDgetSize)GetProcAddress(m_hLibrary, "PCDGETSIZE");
	m_PCDgetBlock = (FN_PCDgetBlock)GetProcAddress(m_hLibrary, "PCDGETBLOCK");
#endif

	if ((m_PCDopen == NULL)
	 || (m_PCDclose == NULL)
	 || (m_PCDreadImageInfo == NULL)
	 || (m_PCDgetCount == NULL)
	 || (m_PCDsetResolution == NULL)
	 || (m_PCDgetSize == NULL)
	 || (m_PCDgetBlock == NULL))
	{
		Shutdown();
	}
}

PCDstatus CPCDLibrary::Open(LPCSTR lpPath, PCDphotoHdl FAR *lphPCD)
{
	od("Open(): IsValid...\r\n");
	if (IsValid())
	{
		ASSERT(m_PCDopen != NULL);
		od("Open(): m_PCDopen()...\r\n");
		return m_PCDopen((LPSTR)lpPath, lphPCD);
	}
	return ERRORCODE_NotInitialized;
}

PCDstatus CPCDLibrary::Close(PCDphotoHdl hPCD)
{
	if (IsValid())
	{
		ASSERT(m_PCDclose != NULL);
		return m_PCDclose(hPCD);
	}
	return ERRORCODE_NotInitialized;
}

PCDstatus CPCDLibrary::ReadImageInfo(PCDphotoHdl hPCD, PCDpacInfoPtr pInfo)
{
	if (IsValid())
	{
		ASSERT(m_PCDreadImageInfo != NULL);
		return m_PCDreadImageInfo(hPCD, pInfo);
	}
	return ERRORCODE_NotInitialized;
}

PCDstatus CPCDLibrary::GetCount(PCDphotoHdl hPCD, unsigned long FAR *lpRes, short FAR *lpResCount)
{
	if (IsValid())
	{
		ASSERT(m_PCDgetCount != NULL);
		return m_PCDgetCount(hPCD, lpRes, lpResCount);
	}
	return ERRORCODE_NotInitialized;
}

PCDstatus CPCDLibrary::SetResolution(PCDphotoHdl hPCD, PCDresolution nRes)
{
	if (IsValid())
	{
		ASSERT(m_PCDsetResolution != NULL);
		return m_PCDsetResolution(hPCD, nRes);
	}
	return ERRORCODE_NotInitialized;
}

PCDstatus CPCDLibrary::GetSize(PCDphotoHdl hPCD, LPRECT lpBoundRect)
{
	if (IsValid())
	{
		ASSERT(m_PCDgetSize != NULL);
#ifdef WIN32
		PCDrect BoundRect;
		PCDstatus Status = m_PCDgetSize(hPCD, &BoundRect);
		makeRECTfromPCDrect(BoundRect, lpBoundRect);
		return Status;
#else
		return m_PCDgetSize(hPCD, lpBoundRect);
#endif
	}
	return ERRORCODE_NotInitialized;
}

PCDstatus CPCDLibrary::GetBlock(PCDphotoHdl hPCD, LPRECT lpBoundRect, PCDRAWDATA hpBuffer, long lStride)
{
	if (IsValid())
	{
		ASSERT(m_PCDgetBlock != NULL);
#ifdef WIN32
		PCDrect BoundRect;
		makePCDrect((*lpBoundRect), (&BoundRect));
		return m_PCDgetBlock(hPCD, &BoundRect, hpBuffer, lStride);
#else
		return m_PCDgetBlock(hPCD, lpBoundRect, hpBuffer, lStride);
#endif
	}
	return ERRORCODE_NotInitialized;
}

static CPCDLibrary PCD;

/////////////////////////////////////////////////////////////////////////////
// PCDHelper - Photo CD image helper.

WORD PCDHelper::m_wDraftResolution = PCD_BASE_OVER_16;
WORD PCDHelper::m_wProofResolution = PCD_BASE_OVER_4;

/*
// Read the PCD helper info.
*/

ERRORCODE PCDHelper::read(StorageDevicePtr device)
{
	return device->read_record(&m_Record, sizeof(m_Record));
}

/*
// Write the PCD helper info.
*/

ERRORCODE PCDHelper::write(StorageDevicePtr device)
{
	return device->write_record(&m_Record, sizeof(m_Record));
}

/*
// Size the PCD helper info.
*/

ST_DEV_IO_SIZE PCDHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(m_Record));
}

static BOOL near SupportsResolution(LONG lRes, int nStep)
{
	if (nStep < 0 || nStep > 31)
	{
		return FALSE;
	}
	return (lRes & (1L << nStep)) != 0;
}

/*
// Initialize a PCD image.
*/

ERRORCODE PCDHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	m_Record.m_wResolution = (gcs == NULL || gcs->proof)
					? m_wProofResolution			// Bigger size.
					: m_wDraftResolution;		// Smaller size.

	PCDphotoHdl hPCD;

	int nError;

	od("OpenPCD...\r\n");
	if ((nError = OpenPCD(&hPCD)) == 0)
	{
		PCDpacInfoRec Info;
		od("ReadImageInfo...\r\n");
		if ((nError = PCD.ReadImageInfo(hPCD, &Info)) == 0)
		{
			od("Got info for PCD file!\r\n");
		}
	/*
	// Query the library for the image info.
	*/

		unsigned long lRes;
		short nResCount;
		if ((nError = PCD.GetCount(hPCD, &lRes, &nResCount)) == 0)
		{
			od("Got count %d with lRes %lx.\r\n", nResCount, lRes);

		/*
		// Make sure our default resolution is supported.
		// If not, search for one that is supported.
		*/
			int nResolution = (int)m_Record.m_wResolution;
			if (!SupportsResolution(lRes, nResolution))
			{
				for (int nStep = 0; nStep < 16; nStep++)
				{
					if (SupportsResolution(lRes, nResolution-nStep))
					{
						m_Record.m_wResolution = nResolution-nStep;
						break;
					}
					if (SupportsResolution(lRes, nResolution+nStep))
					{
						m_Record.m_wResolution = nResolution+nStep;
						break;
					}
				}
			}

		/*
		// Verify the resolution we ended up with.
		*/

			if (!SupportsResolution(lRes, m_Record.m_wResolution))
			{
				od("Resolution not supported.\r\n");
				nError = pcdNotAvailable;
			}
			else if ((nError = PCD.SetResolution(hPCD, (PCDresolution)m_Record.m_wResolution)) == 0)
			{
				RECT r;

				if ((nError = PCD.GetSize(hPCD, &r)) == 0)
				{
					od("Got bound (%d, %d, %d, %d)\r\n", r);

					int xsize = r.right - r.left;
					int ysize = r.bottom - r.top;

					graphic->record.x_size = xsize;
					graphic->record.y_size = ysize;

					if (xsize < ysize)
					{
						xsize = ysize;
					}

				/* Fit the file into a 2"x2" square. */

					graphic->record.x_resolution = \
					graphic->record.y_resolution = xsize/2;

					graphic->record.storage = GRAPHIC_STORAGE_FILE;
				}
			}
		}

		PCD.Close(hPCD);
	}

	if (nError == 0 && gcs->proof)
	{
	/* Create what will be our drawing image. */
		nError = graphic->CreateImage();
	}

	if (nError != 0)
	{
		od("*** Got error %d ***\r\n", nError);
	}

	return (nError <= 0) ? (ERRORCODE)nError : ERRORCODE_Open;
}

PCDstatus PCDHelper::OpenPCD(PCDphotoHdl* lphPCD)
{
	PCDstatus nError;
	LPCSTR pName = StorageFile::TranslateFileName(graphic->m_csFileName, TRUE);

	if (pName == NULL)
	{
		return ERRORCODE_Open;			// This will be handled properly.
	}

	CString csName = pName;				// Save this in case path_manager is called again.

	*lphPCD = 0;					// Maybe this is a good idea...

	od("PCD.Open(\"%s\")...\r\n", (LPCSTR)csName);
	for (int i = 0; i < 3; i++)
	{
		if ((nError = PCD.Open(csName, lphPCD)) == 0)
		{
			od("Opened '%s'...\r\n", (LPCSTR)csName);
			break;
		}
		if (nError != pcdBadFmt)
		{
			break;
		}
	}
	if (nError == 0)
	{
		if ((nError = PCD.SetResolution(*lphPCD, (PCDresolution)m_Record.m_wResolution)) != 0)
		{
			PCD.Close(*lphPCD);
		}
	}

	return nError;
}

/////////////////////////////////////////////////////////////////////////////
// PCDDriver - Photo CD image driver.

WORD PCDDriver::m_wMaxCacheSize = 64000;

/*
// The constructor for the PCD driver.
*/

PCDDriver::PCDDriver(GraphicPtr g) : graphic(g)
{
	m_fOpen = FALSE;

	m_hLineCache = NULL;
	m_pLineCache = NULL;
}

/*
// The destructor for the PCD driver.
*/

PCDDriver::~PCDDriver()
{
	close_image();
}

/*
// The image_info() method for PCD files.
*/

ERRORCODE PCDDriver::image_info(IMAGE_INFO_PTR info)
{
//	PCDHelperPtr helper = (PCDHelperPtr)graphic->my_little_helper();

	info->planes = 3;
	info->bpp = 8;
	info->red_index = 2;			/* Damn reversed thing! */
	info->width = graphic->record.x_size;
	info->height = graphic->record.y_size;
	info->image_type = IMAGE_FULL_COLOR;

	return ERRORCODE_None;
}

/*
// The open_image() method for PCD files.
*/

ERRORCODE PCDDriver::open_image(PALETTE_INFO_PTR palette)
{
	PCDHelper* pHelper = (PCDHelper*)graphic->my_little_helper();

	int nError;

	if ((nError = pHelper->OpenPCD(&m_hPCD)) == 0)
	{
		m_fOpen = TRUE;
	}
	return (nError <= 0) ? (ERRORCODE)nError : ERRORCODE_Open;
}

/*
// The read_block() method for PCD files.
*/

ERRORCODE PCDDriver::read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask)
{
	ASSERT(pMask == NULL);

	int nHeight = graphic->record.y_size;

	if (line >= nHeight)
	{
		return ERRORCODE_IntError;
	}

	if (m_pLineCache == NULL)
	{
	/* Compute the number of lines to allocate for the cache. */

		m_nCacheMax = m_wMaxCacheSize/source_row_size;
		if (m_nCacheMax > nHeight)
		{
		/* No sense in allocating more than we'll be reading. */
			m_nCacheMax = nHeight;
		}

	/* Try to allocate it. */

		if (m_nCacheMax != 0)
		{
			DWORD dwSize = (WORD)m_nCacheMax*source_row_size;
			if ((m_hLineCache = ::GlobalAlloc(GMEM_MOVEABLE, dwSize)) != NULL)
			{
				if ((m_pLineCache = (LPBYTE)::GlobalLock(m_hLineCache)) == NULL)
				{
					::GlobalFree(m_hLineCache);
					m_hLineCache = NULL;
				}
				else
				{
					m_nCacheTop = -1;
				}
			}
		}
	}

	RECT r;

	r.left = 0;
	r.right = graphic->record.x_size;

	int nError = ERRORCODE_IntError;

	if (m_pLineCache == NULL)
	{
		r.top = line;
		r.bottom = line+count;
		if (r.bottom >= nHeight)
		{
			r.bottom = nHeight-1;
		}

		nError = PCD.GetBlock(m_hPCD, &r, (PCDRAWDATA)buffer, (long)source_row_size);
		if (nError != 0)
		{
			od("Got error %d on PCD GetBlock\r\n", nError);
		}
	}
	else
	{
	/* Do the caching thing. */
		while (count != 0)
		{
			if (line >= nHeight)
			{
				return ERRORCODE_IntError;
			}

			if (m_nCacheTop == -1 || line >= m_nCacheTop + m_nCacheLines)
			{
			/* We need to read the next chunk. */
				m_nCacheTop = line;
				m_nCacheLines = m_nCacheMax;
				if (m_nCacheTop + m_nCacheLines >= nHeight)
				{
					m_nCacheLines = nHeight - m_nCacheTop;
				}

				r.top = m_nCacheTop;
				r.bottom = r.top + m_nCacheLines;
				nError = PCD.GetBlock(m_hPCD, &r, (PCDRAWDATA)m_pLineCache, (long)source_row_size);
				if (nError != 0)
				{
					break;
				}
			}

		/* Now we copy lines out. */

			int nMaxLine = m_nCacheTop + m_nCacheLines;
#if 0
			LPBYTE pSource = m_pLineCache + (line - m_nCacheTop)*source_row_size;
			int nLines = count;
			int nMaxLine = m_nCacheTop + m_nCacheLines;
			if (line + nLines >= nMaxLine)
			{
				nLines = nMaxLine - line;
			}
#else
		// It reads in upside-down.
			LPBYTE pSource = m_pLineCache + (nMaxLine-1 - line)*source_row_size;
			int nLines = 1;
#endif

			WORD wSize = (WORD)nLines*source_row_size;
			memcpy(buffer, pSource, wSize);

			count -= nLines;
			buffer += wSize;
			line += nLines;
			nError = 0;
		}
	}
	return (nError <= 0) ? (ERRORCODE)nError : ERRORCODE_Open;
}

void PCDDriver::close_image(void)
{
	if (m_fOpen)
	{
		PCD.Close(m_hPCD);
		m_fOpen = FALSE;
	}

	if (m_hLineCache != NULL)
	{
		if (m_pLineCache != NULL)
		{
			::GlobalUnlock(m_hLineCache);
			m_pLineCache = NULL;
		}
		::GlobalFree(m_hLineCache);
		m_hLineCache = NULL;
	}
}
