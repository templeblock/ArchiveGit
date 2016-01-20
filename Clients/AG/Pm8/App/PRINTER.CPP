/*
// Paper info.
//
// $Header: /PM8/App/PRINTER.CPP 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/PRINTER.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     7/16/98 1:45p Psasse
// Added Advanced Printing Functionality "Print As Bitmap"
// 
//    Rev 1.0   14 Aug 1997 15:24:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:06   Fred
// Initial revision.
// 
//    Rev 1.8   05 Jun 1997 17:04:30   Jay
// 
//    Rev 1.7   05 Jun 1997 16:47:44   Jay
// New PostScript type check and handling
// 
//    Rev 1.6   08 Apr 1997 16:11:18   Jay
// Print Instructions control
// 
//    Rev 1.5   22 Aug 1996 17:56:34   Jay
// DefaultFastBitmapPrinting is TRUE if colors > 8.
// 
//    Rev 1.4   07 Aug 1996 09:21:22   Jay
// HP banner printer bug fix.
// 
//    Rev 1.3   06 Aug 1996 11:17:40   Jay
//  
// 
//    Rev 1.2   24 May 1996 16:12:50   Fred
// TRACEx
// 
//    Rev 1.1   11 Apr 1996 12:34:04   FRED
// Copied from PMW2
// 
//    Rev 1.16   03 Apr 1996 09:06:14   FRED
// Fixes for DeskJet envelope printing
// 
//    Rev 1.15   04 Mar 1996 10:55:50   JAY
// Fixed uninitialized variable
// 
//    Rev 1.14   25 Jan 1996 09:34:52   JAY
// Ugh! 16-bit bug!
// 
//    Rev 1.13   24 Jan 1996 12:56:36   JAY
// New DeviceCapabilities handling for WIN32. Added m_csPrinterName.
// 
//    Rev 1.12   15 Jan 1996 09:16:44   JAY
// Changes to fix changes made for 32-bit version.
// 
//    Rev 1.11   08 Jan 1996 15:46:28   JAY
// Now gets correct printer/device names under WIN32.
// 
//    Rev 1.10   20 Nov 1995 11:18:08   JAY
// Nre reverse landscape check
// 
//    Rev 1.9   17 Nov 1995 18:05:46   FRED
// Updated to versions from L:\PMW2.16
// 
//    Rev 1.8   09 Oct 1995 15:15:54   JAY
// New direct (fast) bitmap printing
// 
//    Rev 1.7   06 Oct 1995 11:59:52   JAY
// New print code, etc.
// 
//    Rev 1.5   04 Sep 1995 09:00:32   JAY
// Margins now default to physical margins.
// 
//    Rev 1.4   28 Aug 1995 20:33:12   FRED
// Print alignment
// 
//    Rev 1.3   11 Aug 1995 12:53:28   JAY
// Better defaults.
// 
//    Rev 1.2   08 Aug 1995 11:01:40   JAY
// Envelope stuff.
// 
//    Rev 1.1   28 Jul 1995 18:15:28   FRED
// Removed "inline" from some functions
// so PMW would link.
// 
// 
//    Rev 1.0   28 Jul 1995 12:29:06   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "printer.h"
#include "pmwini.h"
#include "redisp.h"

#ifdef WIN32
#include <winspool.h>
#endif

extern RedisplayContext far printer_rc;

/*
// Constructor.
*/

CPrinterInfo::CPrinterInfo()
{
	Defaults();
}

/*
// Printer variable access.
// These are inline for this file only.
*/

int CPrinterInfo::GetOutlineGamma(void) const
{
	return printer_rc.outline_gamma_value;
}

void CPrinterInfo::SetOutlineGamma(int nGamma)
{
	printer_rc.outline_gamma_value = nGamma;
}

int CPrinterInfo::GetBitmapGamma(void) const
{
	return FastBitmaps() ? m_Record.m_nFastBitmapGamma : m_Record.m_nBitmapGamma;
}

void CPrinterInfo::SetBitmapGamma(int nGamma)
{
	if (FastBitmaps())
	{
		m_Record.m_nFastBitmapGamma = nGamma;
	}
	else
	{
		m_Record.m_nBitmapGamma = nGamma;
	}
	UpdateBitmapGamma();
}

void CPrinterInfo::UpdateBitmapGamma(void)
{
	printer_rc.bitmap_gamma_value = GetBitmapGamma();
}

int CPrinterInfo::GetPostScriptType(void) const
{
	return RedisplayContext::m_nCurrentPostScriptType;
}

void CPrinterInfo::SetPostScriptType(int nType)
{
	RedisplayContext::m_nCurrentPostScriptType = nType;
}

CFixed CPrinterInfo::GetLeftMargin(void) const
{
	return m_Record.m_Margins.Left;
}

void CPrinterInfo::SetLeftMargin(CFixed cfLeftMargin)
{
	m_Record.m_Margins.Left = cfLeftMargin;
}

CFixed CPrinterInfo::GetTopMargin(void) const
{
	return m_Record.m_Margins.Top;
}

void CPrinterInfo::SetTopMargin(CFixed cfTopMargin)
{
	m_Record.m_Margins.Top = cfTopMargin;
}

CFixed CPrinterInfo::GetRightMargin(void) const
{
	return m_Record.m_Margins.Right;
}

void CPrinterInfo::SetRightMargin(CFixed cfRightMargin)
{
	m_Record.m_Margins.Right = cfRightMargin;
}

CFixed CPrinterInfo::GetBottomMargin(void) const
{
	return m_Record.m_Margins.Bottom;
}

void CPrinterInfo::SetBottomMargin(CFixed cfBottomMargin)
{
	m_Record.m_Margins.Bottom = cfBottomMargin;
}

CFixed CPrinterInfo::GetLeftOffset(void) const
{
	return m_Record.m_OriginOffset.x;
}

void CPrinterInfo::SetLeftOffset(CFixed cfLeftOffset)
{
	m_Record.m_OriginOffset.x = cfLeftOffset;
}

CFixed CPrinterInfo::GetTopOffset(void) const
{
	return m_Record.m_OriginOffset.y;
}

void CPrinterInfo::SetTopOffset(CFixed cfTopOffset)
{
	m_Record.m_OriginOffset.y = cfTopOffset;
}

BOOL CPrinterInfo::GetPrintAsBitmap(void) const
{
	return m_Record.m_fPrintAsBitmap;
}

void CPrinterInfo::SetPrintAsBitmap(BOOL fSet)
{
	m_Record.m_fPrintAsBitmap = fSet;
	printer_rc.is_printing_as_bitmap = fSet;

}

/*
// Set defaults for the printer info.
*/

void CPrinterInfo::Defaults(void)
{
	m_nOrientationType = PORTRAIT;

	memset(&m_Record, 0, sizeof(m_Record));

	m_Record.m_nCopies = -1;				// Unknown
	m_Record.m_nOrientation = -1;			// Unknown
	m_Record.m_nTractor = -1;				// Unknown
	m_PhysicalMargins.Left = MakeFixed(0.25);
	m_PhysicalMargins.Top = MakeFixed(0.25);
	m_PhysicalMargins.Right = MakeFixed(0.25);
	m_PhysicalMargins.Bottom = MakeFixed(0.25);
	m_Record.m_OriginOffset.x = MakeFixed(0.0);
	m_Record.m_OriginOffset.y = MakeFixed(0.0);
	m_Record.m_Margins = m_PhysicalMargins;
	m_Record.m_wDoubleSidedType = 0;
	m_Record.m_fPrintInstructions = DEFAULT_PRINT_INSTRUCTIONS;

	m_PaperSize.x = MakeFixed(8.5);
	m_PaperSize.y = MakeFixed(11.0);

	SetOutlineGamma(DEFAULT_OUTLINE_GAMMA);
	SetBitmapGamma(DEFAULT_BITMAP_GAMMA);
	m_nDefaultPostScriptType = 0;	// Assume no.
	SetPostScriptType(m_nDefaultPostScriptType);

	m_Record.m_nNumColors = 0;		// Unknown
	m_Record.m_nRasterCaps = 0;	// Unknown
	m_Record.m_nBitsPixel = 0;		// Unknown
	m_Record.m_nPlanes = 0;			// Unknown

	m_Record.m_fPrintAsBitmap = FALSE;			// Unknown
}

#ifdef WIN32
static LPBYTE GetPrinterInfo(HANDLE hPrinter, int nLevel)
{
	LPBYTE pData = NULL;
	DWORD cbNeeded;

/*
// Make an initial call to see how much data we need.
*/
	if (!GetPrinter(hPrinter, nLevel, NULL, 0, &cbNeeded))
	{
	/*
	// Allocate memory for the data.
	*/
		DWORD cbBuf = cbNeeded;
		pData = new BYTE[cbBuf];
	/*
	// Get the data.
	*/
		if (!GetPrinter(hPrinter, nLevel, pData, cbBuf, &cbNeeded))
		{
			delete pData;
			pData = NULL;
		}
	}
	return pData;
}

static LPBYTE GetDriverInfo(HANDLE hPrinter, int nLevel)
{
	LPBYTE pData = NULL;
	DWORD cbNeeded;

/*
// Make an initial call to see how much data we need.
*/
	if (!GetPrinterDriver(hPrinter, NULL, nLevel, NULL, 0, &cbNeeded))
	{
	/*
	// Allocate memory for the data.
	*/
		DWORD cbBuf = cbNeeded;
		pData = new BYTE[cbBuf];
	/*
	// Get the data.
	*/
		if (!GetPrinterDriver(hPrinter, NULL, nLevel, pData, cbBuf, &cbNeeded))
		{
			delete pData;
			pData = NULL;
		}
	}
	return pData;
}
#endif

/*
// Get the names for the current printer.
*/

void CPrinterInfo::GetNames(HGLOBAL hDevNames, BOOL fUseWinIni /*=TRUE*/)
{
/*
// In case we fail.
*/

	m_csDriverName.Empty();
	m_csDeviceName.Empty();
	m_csOutputName.Empty();
	m_csPrinterName.Empty();

/*
// Now try and get the printer info.
*/

	LPDEVNAMES pDevNames = NULL;

	if (hDevNames != NULL)
	{
		pDevNames = (LPDEVNAMES)::GlobalLock(hDevNames);
	}

	CString csWinDevice;
	CString csWinDriver;
	CString csWinOutput;

	if (fUseWinIni)
	{
		char szBuffer[256];
		::GetProfileString("windows", "device", "?", szBuffer, sizeof(szBuffer));
		if (strcmp(szBuffer, "?") != 0)
		{
			csWinDevice = strtok(szBuffer, ",");
			csWinDriver = strtok(NULL, ",");
			csWinOutput = strtok(NULL, ",");
		}
	}

	if (pDevNames == NULL)
	{
	/*
	// We don't have a printer name. Use defaults.
	*/
		TRACE0("*** Using WIN.INI ***\n");
		m_csDeviceName = csWinDevice;
		m_csDriverName = csWinDriver;
		m_csOutputName = csWinOutput;
	}
	else
	{
	/* Extract the driver name. */

		m_csDeviceName = (LPCSTR)pDevNames + pDevNames->wDeviceOffset;
		m_csDriverName = (LPCSTR)pDevNames + pDevNames->wDriverOffset;
		m_csOutputName = (LPCSTR)pDevNames + pDevNames->wOutputOffset;

		::GlobalUnlock(hDevNames);

	}

#ifdef WIN32

/*
// We don't want this transient WINSPOOL name.
// If it shows up, go back to the WIN.INI device= name if we have one.
*/
	if (m_csDriverName.CompareNoCase("WINSPOOL") == 0)
	{
		if (!csWinDriver.IsEmpty())
		{
			m_csDriverName = csWinDriver;
		}
	}

	m_csPrinterName = m_csDeviceName;

/*
// Get the actual driver name (not the name the user gave it).
*/
	HANDLE hPrinter;
	if (OpenPrinter((LPSTR)(LPCSTR)m_csDeviceName, &hPrinter, NULL))
	{
		PRINTER_INFO_2* pInfo2;
		if ((pInfo2 = (PRINTER_INFO_2*)GetPrinterInfo(hPrinter, 2)) != NULL)
		{
			if (pInfo2->pDriverName != NULL)
			{
				m_csDeviceName = pInfo2->pDriverName;
			}
			delete [] pInfo2;
		}
		DRIVER_INFO_2* pDrvInfo2;
		if ((pDrvInfo2 = (DRIVER_INFO_2*)GetDriverInfo(hPrinter, 2)) != NULL)
		{
			if (pDrvInfo2->pDriverPath != NULL)
			{
				m_csDriverName = pDrvInfo2->pDriverPath;
			}
			delete [] pDrvInfo2;
		}
		ClosePrinter(hPrinter);
	}
#else
	m_csPrinterName = m_csDeviceName;
#endif

   CString csOutput;
   csOutput.Format("Drv: '%s'; Dev: '%s'; Out: '%s'\n(Name: '%s')",
			         (LPCSTR)m_csDriverName,
			         (LPCSTR)m_csDeviceName,
			         (LPCSTR)m_csOutputName,
						(LPCSTR)m_csPrinterName);
//	AfxMessageBox(csOutput);
   TRACE("%s\n", csOutput);
}

BOOL CPrinterInfo::GetSectionName(CString& csName)
{
	csName.Empty();

	if (!m_csDriverName.IsEmpty() && !m_csDeviceName.IsEmpty())
	{
		csName = m_csDriverName;
		csName += ',';
		csName += m_csDeviceName;

		return TRUE;
	}
	return FALSE;
}

void CPrinterInfo::UpdateFromDevNames(HGLOBAL hDevNames, HGLOBAL hDevMode, HDC hDC, int nOrientation, BOOL fUseWinIni /*=TRUE*/)
{
	Defaults();
	m_nOrientationType = nOrientation;
	GetNames(hDevNames, fUseWinIni);
	GetDeviceInfo(hDevMode, hDC);
	ReadSettings();
}

/*
// Get the device caps from the printer driver.
*/

void CPrinterInfo::GetDeviceInfo(HGLOBAL hDevMode, HDC in_hDC)
{
	// Ignore the incoming DC since we may not be able to change it via ResetDC.
	in_hDC = NULL;
/*
// I don't trust these printer drivers.
// I know for sure that some drivers are bad; there may be more than what
// I know (from another program).
// We will use an INI file mechanism to try and detect and skip over the
// bad ones. Unfortunately, this assumes that a bad one will cause a GP
// fault...
*/
	if (!GetProfileInt(ENTRY_BadDC, FALSE))
	{
	
		/*
		// We set the INI file setting now. If this fails for any reason
		// (particularly for a GP fault), then it will be set forever as "bad".
		*/
	
		WriteProfileInt(ENTRY_BadDC, TRUE);
	
#ifndef WIN32
		LPFNDEVCAPS DeviceCapabilities = NULL;
		CString csDriverName = m_csDriverName + ".DRV";
	
		UINT uSEM = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
		HINSTANCE hDriver = ::LoadLibrary((LPSTR)(LPCSTR)csDriverName);
		::SetErrorMode(uSEM);
	
		if ((UINT)hDriver >= 32)
		{
			DeviceCapabilities = (LPFNDEVCAPS)GetProcAddress(hDriver, "DeviceCapabilities");
		}
#endif

		if (DeviceCapabilities != NULL)
		{
		/*
		// Read the number of copies setting.
		*/
			m_Record.m_nCopies = (int)DeviceCapabilities((LPSTR)(LPCSTR)m_csPrinterName,
															 (LPSTR)(LPCSTR)m_csOutputName,
															 DC_COPIES,
															 NULL,
															 NULL);
			TRACE1("Supports %d copies\n", m_Record.m_nCopies);
	
		/*
		// Read the landscape orientation setting.
		*/
			m_Record.m_nOrientation = (int)DeviceCapabilities((LPSTR)(LPCSTR)m_csPrinterName,
																	(LPSTR)(LPCSTR)m_csOutputName,
																	(WORD)DC_ORIENTATION,
																	NULL,
																	NULL);
			TRACE1("Orientation is %d degrees\n", m_Record.m_nOrientation);
	
		/*
		// Read the number of bins.
		*/
			int nBins = (int)DeviceCapabilities((LPSTR)(LPCSTR)m_csPrinterName,
																	(LPSTR)(LPCSTR)m_csOutputName,
																	(WORD)DC_BINS,
																	NULL,
																	NULL);
			if (nBins > 0 && nBins < 32768/sizeof(WORD))
			{
			/* Read the bins. */
				WORD* pBins = new WORD[nBins];
				if (pBins != NULL)
				{
					DeviceCapabilities((LPSTR)(LPCSTR)m_csPrinterName,
								  (LPSTR)(LPCSTR)m_csOutputName,
								  (WORD)DC_BINS,
								  (LPSTR)pBins,
								  NULL);
	
					// Assume FALSE until found.
					m_Record.m_nTractor = 0;
	
					for (int i = 0; i < nBins; i++)
					{
						if (pBins[i] == DMBIN_TRACTOR)
						{
						// We found it.
							m_Record.m_nTractor = 1;
							break;
						}
					}
					delete [] pBins;
				}
			}
		}
#ifndef WIN32
		if ((UINT)hDriver >= 32)
		{
			::FreeLibrary(hDriver);
		}
#endif
	
		WriteProfileInt(ENTRY_BadDC, FALSE, FALSE);
	}

/*
// Compute the desired orientation (as it is stored in the DevMode structure
*/

	SHORT desired_orientation = m_nOrientationType == PORTRAIT
					? DMORIENT_PORTRAIT
					: DMORIENT_LANDSCAPE;

	HDC hDC = NULL;

/* If we have no DC, create one now. */

	CPrintDialog pd(TRUE);

	AfxGetApp()->GetPrinterDeviceDefaults(&pd.m_pd);

	hDevMode = pd.m_pd.hDevMode;
	LPDEVMODE dm = NULL;
	DWORD dwOrgFields;
	int nOrgOrientation; 
	if (hDevMode != NULL)
	{
		dm = (LPDEVMODE)::GlobalLock(pd.m_pd.hDevMode);
		if (dm != NULL)
		{
			dwOrgFields = dm->dmFields;
			nOrgOrientation = dm->dmOrientation;
			dm->dmFields |= DM_ORIENTATION;
			dm->dmOrientation = desired_orientation;
		}
	}

	if (pd.m_pd.hDevNames != NULL)
	{
		HDC dc_save = pd.m_pd.hDC;
		hDC = pd.CreatePrinterDC();
		pd.m_pd.hDC = dc_save;
	}


/*
// Now update the paper info.
*/

/* Lock the dev mode structure if we have one. */

	SHORT orientation = DMORIENT_PORTRAIT;		/* In case we fail. */
	int paper_source = DMBIN_AUTO;				/* In case we fail. */

	if (dm != NULL)
	{
		if (dm->dmFields & DM_ORIENTATION)
		{
			orientation = dm->dmOrientation;
			ASSERT(orientation == desired_orientation);
		}
		if (dm->dmFields & DM_DEFAULTSOURCE)
		{
			paper_source = dm->dmDefaultSource;
		}
	}

	// Save the paper size from the DEVMODE structure: DMPAPER_LETTER, ...

	if (dm != NULL)
	{
		m_nDevModePaperSize = dm->dmPaperSize;
	}
	else
	{
		m_nDevModePaperSize = DMPAPER_LETTER;
	}

/* Set the paper type. */

	m_nPaperType = paper_source == DMBIN_TRACTOR
							? PAPER_TYPE_CONTINUOUS
							: PAPER_TYPE_CUT_SHEET;

	int nPaperXUnit, nPaperYUnit;
	int nPaperWidth, nPaperHeight;
	int nPrintableWidth, nPrintableHeight;
	int nLeftOffset, nTopOffset;

	if (hDC == NULL)
	{
	/* Some stupid defaults. (8 1/2" by 11"). */
		nPaperXUnit = 1000;
		nPaperYUnit = 1000;

		nPaperWidth = 8500;
		nPaperHeight = 11000;

		nPrintableWidth = 8000;
		nPrintableHeight = 10500;

		nLeftOffset = 250;
		nTopOffset = 250;

		m_nDefaultPostScriptType = 0;		// Assume no.
	}
	else
	{
		/* Lock the DevMode structure! */

#if 0
		LPDEVMODE dm;
		if (hDevMode != NULL && (dm = (LPDEVMODE)GlobalLock(hDevMode)) != NULL)
		{
		/* Well, we have it now... */

//			if (!(dm->dmFields & DM_ORIENTATION)
//						|| dm->dmOrientation != desired_orientation)
			{
			/*
			// Orientation is not set or not what we want.
			// Force it to be what we want.
			*/
				dm->dmFields |= DM_ORIENTATION;
				dm->dmOrientation = desired_orientation;
				orientation = desired_orientation;
				ResetDC(hDC, dm);
			}
			GlobalUnlock(hDevMode);
		}
#endif

		m_Record.m_nNumColors = GetDeviceCaps(hDC, NUMCOLORS);
		m_Record.m_nRasterCaps = GetDeviceCaps(hDC, RASTERCAPS);
		m_Record.m_nBitsPixel = GetDeviceCaps(hDC, BITSPIXEL);
		m_Record.m_nPlanes = GetDeviceCaps(hDC, PLANES);

	/* Get the device specifications. */
		nPaperXUnit = GetDeviceCaps(hDC, LOGPIXELSX);
		nPaperYUnit = GetDeviceCaps(hDC, LOGPIXELSY);

		nPrintableWidth = GetDeviceCaps(hDC, HORZRES);
		nPrintableHeight = GetDeviceCaps(hDC, VERTRES);

		POINT p;

		if (::Escape(hDC, GETPHYSPAGESIZE, NULL, NULL, &p) > 0)
		{
			nPaperWidth = p.x;
			nPaperHeight = p.y;
		}
		else
		{
			nPaperWidth = nPrintableWidth;
			nPaperHeight = nPrintableHeight;
		}

		if (::Escape(hDC, GETPRINTINGOFFSET, NULL, NULL, &p) > 0)
		{
			nLeftOffset = p.x;
			nTopOffset = p.y;
			od("Printing offset: %d, %d\r\n", p);
		}
		else
		{
			nLeftOffset = (nPaperWidth - nPrintableWidth)/2;
			nTopOffset = (nPaperHeight - nPrintableHeight)/2;
		}

		od("PhysPageSize: %d, %d; ", nPaperWidth, nPaperHeight);
		od("w:%d, h:%d, xres:%d, yres:%d\r\n",
					nPrintableWidth, nPrintableHeight,
 					nPaperXUnit, nPaperYUnit);

		// Determine the PostScript type at this time since we have a DC.
		m_nDefaultPostScriptType = DeterminePostScriptType(hDC);

		// Delete the DC if we created it.
		if (hDC != in_hDC)
		{
			DeleteDC(hDC);
		}

		if (orientation == DMORIENT_LANDSCAPE)
		{
		/* Device orientation is landscape. Reverse everything. */
			int tmp;

			tmp = nPrintableWidth;
			nPrintableWidth = nPrintableHeight;
			nPrintableHeight = tmp;

			tmp = nPaperWidth;
			nPaperWidth = nPaperHeight;
			nPaperHeight = tmp;

			tmp = nPaperXUnit;
			nPaperXUnit = nPaperYUnit;
			nPaperYUnit = tmp;

			// Pull in our orientation setting NOW
			m_Record.m_nOrientation = GetProfileInt(ENTRY_Orientation, m_Record.m_nOrientation);
			int nOrientation = m_Record.m_nOrientation;
			if (nOrientation == -1)
			{
				nOrientation = 90;			// Assume
				m_Record.m_nTractor = GetProfileInt(ENTRY_Tractor, m_Record.m_nTractor);
				if (m_Record.m_nTractor == 1 || paper_source == DMBIN_TRACTOR)
				{
					m_Record.m_nTractor = 1;
					nOrientation = 270;
				}
			}

			if (nOrientation == 90)
			{
				int nNewTop = nPaperHeight - (nLeftOffset + nPrintableHeight);
				nLeftOffset = nTopOffset;
				nTopOffset = nNewTop;
			}
			else
			{
				ASSERT(nOrientation == 270);
				int nNewLeft = nPaperWidth - (nTopOffset + nPrintableWidth);
				nTopOffset = nLeftOffset;
				nLeftOffset = nNewLeft;
			}
		}

	/*
	// Tractor paper is fine, but if we can't print on the full page, it
	// will have to be cut sheet.
	*/

#if 0
		if (m_nPaperType == PAPER_TYPE_CONTINUOUS
				&& nPrintableHeight != nPaperHeight)
		{
			m_nPaperType = PAPER_TYPE_CUT_SHEET;
		}
#else
		if (nPrintableHeight == nPaperHeight)
		{
			m_nPaperType = PAPER_TYPE_CONTINUOUS;
		}
		else
		{
			m_nPaperType = PAPER_TYPE_CUT_SHEET;
		}
#endif
	}


/*
// Store the values we computed in the member variables.
*/

	CFixed lXUnits = MakeFixed(nPaperXUnit);
	CFixed lYUnits = MakeFixed(nPaperYUnit);

	m_PaperSize.x = DivFixed(MakeFixed(nPaperWidth), lXUnits);
	m_PaperSize.y = DivFixed(MakeFixed(nPaperHeight), lYUnits);
	m_PrintableSize.x = DivFixed(MakeFixed(nPrintableWidth), lXUnits);
	m_PrintableSize.y = DivFixed(MakeFixed(nPrintableHeight), lYUnits);
	m_PhysicalMargins.Left = DivFixed(MakeFixed(nLeftOffset), lXUnits);
	m_PhysicalMargins.Top = DivFixed(MakeFixed(nTopOffset), lYUnits);
	m_PhysicalMargins.Right = m_PaperSize.x - (m_PrintableSize.x + m_PhysicalMargins.Left);
	m_PhysicalMargins.Bottom = m_PaperSize.y - (m_PrintableSize.y + m_PhysicalMargins.Top);

	if (dm != NULL)
	{
		ASSERT(hDevMode != NULL);
		
		dm->dmFields = dwOrgFields;
		dm->dmOrientation = nOrgOrientation;
		::GlobalUnlock(hDevMode);
	}
}

/*
// Read the settings from the INI file(s).
*/

void CPrinterInfo::ReadSettings(void)
{
	// Read our overrides.
	m_Record.m_nCopies = GetProfileInt(ENTRY_Copies, m_Record.m_nCopies);
	m_Record.m_nOrientation = GetProfileInt(ENTRY_Orientation, m_Record.m_nOrientation);
	m_Record.m_nTractor = GetProfileInt(ENTRY_Tractor, m_Record.m_nTractor);
	
	if (m_nOrientationType == PORTRAIT)
	{
		SetLeftMargin(GetProfileFixed(ENTRY_LeftMargin, m_PhysicalMargins.Left));
		SetTopMargin(GetProfileFixed(ENTRY_TopMargin, m_PhysicalMargins.Top));
		SetRightMargin(GetProfileFixed(ENTRY_RightMargin, m_PhysicalMargins.Right));
		SetBottomMargin(GetProfileFixed(ENTRY_BottomMargin, m_PhysicalMargins.Bottom));
		SetLeftOffset(GetProfileFixed(ENTRY_LeftOffset, m_Record.m_OriginOffset.x));
		SetTopOffset(GetProfileFixed(ENTRY_TopOffset, m_Record.m_OriginOffset.y));
	}
	else
	{
		SetLeftMargin(GetProfileFixed(ENTRY_LeftMarginLandscape, m_PhysicalMargins.Left));
		SetTopMargin(GetProfileFixed(ENTRY_TopMarginLandscape, m_PhysicalMargins.Top));
		SetRightMargin(GetProfileFixed(ENTRY_RightMarginLandscape, m_PhysicalMargins.Right));
		SetBottomMargin(GetProfileFixed(ENTRY_BottomMarginLandscape, m_PhysicalMargins.Bottom));
		SetLeftOffset(GetProfileFixed(ENTRY_LeftOffsetLandscape, m_Record.m_OriginOffset.x));
		SetTopOffset(GetProfileFixed(ENTRY_TopOffsetLandscape, m_Record.m_OriginOffset.y));
	}

	// Read our standard values.
	FastBitmaps(GetProfileInt(ENTRY_FastBitmaps, GetDefaultFastBitmaps()));
	SetOutlineGamma(GetProfileInt(ENTRY_OutlineGammaValue, DEFAULT_OUTLINE_GAMMA));
	m_Record.m_nBitmapGamma = GetProfileInt(ENTRY_GammaValue, DEFAULT_BITMAP_GAMMA);
	m_Record.m_nFastBitmapGamma = GetProfileInt(ENTRY_FastBitmapGammaValue, DEFAULT_FAST_BITMAP_GAMMA);
	UpdateBitmapGamma();
	SetPostScriptType(GetProfileInt(ENTRY_PostscriptType, m_nDefaultPostScriptType));
	int nDefaultEnvelopePosition =
					(m_Record.m_nTractor == 1)
							? (ENVELOPE_Wide | ENVELOPE_FlapUp | ENVELOPE_Center | ENVELOPE_RRotate)
							: (IsDeskJet()
									? (ENVELOPE_FlapUp | ENVELOPE_Right)
									: (ENVELOPE_FlapDown | ENVELOPE_Center));
	SetDefaultEnvelopePosition(GetProfileInt(ENTRY_DefaultEnvelopePosition, nDefaultEnvelopePosition));
	SetEnvelopePosition(GetProfileInt(ENTRY_EnvelopePosition, GetDefaultEnvelopePosition()));
	ReverseLandscape(GetProfileInt(ENTRY_UseReverseLandscape, GetDefaultReverseLandscape()));
	SetDoubleSidedType(GetProfileInt(ENTRY_DoubleSidedType, 0));
	SetPrintInstructions(GetProfileInt(ENTRY_PrintInstructions, DEFAULT_PRINT_INSTRUCTIONS));
	SetPrintAsBitmap((BOOL)(GetProfileInt(ENTRY_FlagPrintAsBitmap, FALSE)));
}

/*
// Write the settings to the INI file.
*/

void CPrinterInfo::WriteSettings(void)
{
	WriteProfileInt(ENTRY_FastBitmaps, FastBitmaps(), GetDefaultFastBitmaps());
	WriteProfileInt(ENTRY_OutlineGammaValue, GetOutlineGamma(), DEFAULT_OUTLINE_GAMMA);
	WriteProfileInt(ENTRY_GammaValue, m_Record.m_nBitmapGamma, DEFAULT_BITMAP_GAMMA);
	WriteProfileInt(ENTRY_FastBitmapGammaValue, m_Record.m_nFastBitmapGamma, DEFAULT_FAST_BITMAP_GAMMA);
	WriteProfileInt(ENTRY_PostscriptType, GetPostScriptType(), m_nDefaultPostScriptType);
	WriteProfileInt(ENTRY_EnvelopePosition, GetEnvelopePosition(), GetDefaultEnvelopePosition());

	if (m_nOrientationType == PORTRAIT)
	{
		WriteProfileFixed(ENTRY_LeftMargin, GetLeftMargin(), m_PhysicalMargins.Left);
		WriteProfileFixed(ENTRY_TopMargin, GetTopMargin(), m_PhysicalMargins.Top);
		WriteProfileFixed(ENTRY_RightMargin, GetRightMargin(), m_PhysicalMargins.Right);
		WriteProfileFixed(ENTRY_BottomMargin, GetBottomMargin(), m_PhysicalMargins.Bottom);
		WriteProfileFixed(ENTRY_LeftOffset, GetLeftOffset(), MakeFixed(0));
		WriteProfileFixed(ENTRY_TopOffset, GetTopOffset(), MakeFixed(0));
	}
	else
	{
		WriteProfileFixed(ENTRY_LeftMarginLandscape, GetLeftMargin(), m_PhysicalMargins.Left);
		WriteProfileFixed(ENTRY_TopMarginLandscape, GetTopMargin(), m_PhysicalMargins.Top);
		WriteProfileFixed(ENTRY_RightMarginLandscape, GetRightMargin(), m_PhysicalMargins.Right);
		WriteProfileFixed(ENTRY_BottomMarginLandscape, GetBottomMargin(), m_PhysicalMargins.Bottom);
		WriteProfileFixed(ENTRY_LeftOffsetLandscape, GetLeftOffset(), MakeFixed(0));
		WriteProfileFixed(ENTRY_TopOffsetLandscape, GetTopOffset(), MakeFixed(0));
	}
	
	WriteProfileInt(ENTRY_UseReverseLandscape, ReverseLandscape(), GetDefaultReverseLandscape());
	WriteProfileInt(ENTRY_DoubleSidedType, GetDoubleSidedType(), 0);
	WriteProfileInt(ENTRY_PrintInstructions, GetPrintInstructions(), DEFAULT_PRINT_INSTRUCTIONS);
	WriteProfileInt(ENTRY_FlagPrintAsBitmap, GetPrintAsBitmap(), FALSE);
}

/*
// Setup the INI name.
*/

void CPrinterInfo::SetIniName(LPCSTR pName)
{
	m_csIniName = pName;
}

/*
// Write a string setting.
*/

BOOL CPrinterInfo::WriteProfileString(LPCSTR pEntry, LPCSTR pString)
{
	CString csSection;

	if (m_csIniName.IsEmpty() || !GetSectionName(csSection))
	{
		return FALSE;
	}
	else
	{
		return ::WritePrivateProfileString(csSection, pEntry, pString, m_csIniName);
	}
}

/*
// Get an int setting.
*/

UINT CPrinterInfo::GetProfileInt(LPCSTR pEntry, UINT wDefault)
{
	CString csSection;

	if (m_csIniName.IsEmpty() || !GetSectionName(csSection))
	{
		return wDefault;
	}
	else
	{
		return ::GetPrivateProfileInt(csSection, pEntry, wDefault, m_csIniName);
	}
}

/*
// Write an int setting.
*/

BOOL CPrinterInfo::WriteProfileInt(LPCSTR pEntry, UINT wValue)
{
	char Buffer[40];
	itoa(wValue, Buffer, 10);

	return WriteProfileString(pEntry, Buffer);
}

/*
// Write an int setting if it is not default (remove it otherwise).
*/

BOOL CPrinterInfo::WriteProfileInt(LPCSTR pEntry, UINT wValue, UINT wDefault)
{
	if (wValue == wDefault)
	{
		return RemoveEntry(pEntry);
	}
	else
	{
	/* Unconditional write. */
		return WriteProfileInt(pEntry, wValue);
	}
}

/*
// Get a fixed setting.
*/

CFixed CPrinterInfo::GetProfileFixed(LPCSTR pEntry, CFixed cfDefault)
{
	CString csSection;

	if (!m_csIniName.IsEmpty() && GetSectionName(csSection))
	{
		char szBuffer[40];
		if (::GetPrivateProfileString(csSection, pEntry, "", szBuffer, sizeof(szBuffer)-1, m_csIniName) > 0)
		{
			cfDefault = atol(szBuffer);
		}
	}
	return cfDefault;
}

/*
// Write a fixed setting.
*/

BOOL CPrinterInfo::WriteProfileFixed(LPCSTR pEntry, CFixed cfValue)
{
	char Buffer[40];
	ltoa((long)cfValue, Buffer, 10);

	return WriteProfileString(pEntry, Buffer);
}

/*
// Write a fixed setting if it is not default (remove it otherwise).
*/

BOOL CPrinterInfo::WriteProfileFixed(LPCSTR pEntry, CFixed cfValue, CFixed cfDefault)
{
	if (cfValue == cfDefault)
	{
		return RemoveEntry(pEntry);
	}
	else
	{
	/* Unconditional write. */
		return WriteProfileFixed(pEntry, cfValue);
	}
}

/*
// Remove a setting.
*/

BOOL CPrinterInfo::RemoveEntry(LPCSTR pEntry)
{
	return WriteProfileString(pEntry, NULL);
}


int CPrinterInfo::GetSafeOrientation(void)
{
	int nOrientation = m_Record.m_nOrientation;
	if (nOrientation == -1)
	{
		nOrientation = 90;			// Assume
		if (m_Record.m_nTractor == 1)
		{
			nOrientation = 270;
		}
	}
	return nOrientation;
}

void CPrinterInfo::GetOrientedMargins(CFixedRect* pRect)
{
	*pRect = Margins();
	OrientRect(pRect);
}

void CPrinterInfo::OrientRect(CFixedRect* pRect, BOOL fUnOrient /*=FALSE*/, BOOL fJustDoIt /*=FALSE*/)
{
	if (m_nOrientationType == LANDSCAPE || fJustDoIt)
	{
		CFixed t;
		BOOL f90 = (GetSafeOrientation() == 90);
		if (fUnOrient)
		{
			f90 = !f90;
		}
		
		if (f90)
   	{
   		t = pRect->Top;
   		pRect->Top = pRect->Left;
   		pRect->Left = pRect->Bottom;
   		pRect->Bottom = pRect->Right;
   		pRect->Right = t;
   	}
		else
   	{
   		t = pRect->Left;
   		pRect->Left = pRect->Top;
   		pRect->Top = pRect->Right;
   		pRect->Right = pRect->Bottom;
   		pRect->Bottom = t;
   	}
	}
}

BOOL CPrinterInfo::GetDefaultReverseLandscape(void)
{
	BOOL f;
	if ((f = GetProfileInt(ENTRY_ReverseLandscape, (UINT)-1)) != -1)
	{
		return f;
	}

	// By default, all DJ's have reverse landscape set.
	return IsDeskJet();
}

BOOL CPrinterInfo::GetDefaultFastBitmaps(void)
{
	return ((m_Record.m_nPlanes*m_Record.m_nBitsPixel > 3) || (m_Record.m_nNumColors > 8));
}

// Try to determine (to a reasonable degree of certainty) if the
// printer is a DeskJet.
//
//		We assume the printer is a DeskJet if:
//
//			1) The driver name contains "DESKJET".
//			2) The driver name looks like "HP..DJ...".
//			3) The printer name contains "DESKJET".

BOOL CPrinterInfo::IsDeskJet(void)
{
	BOOL fResult = FALSE;
	CString csName;

	TRY
	{
#if 0
		CString csMessage;
		csMessage.Format("Driver |%s| Device |%s| Output |%s| Printer |%s|",
			(LPCSTR)m_csDriverName,
			(LPCSTR)m_csDeviceName,
			(LPCSTR)m_csOutputName,
			(LPCSTR)m_csPrinterName);
		AfxMessageBox(csMessage);
#endif

		csName = m_csDriverName;
		csName.MakeUpper();

		if (csName.Find("DESKJET") >= 0)
		{
			fResult = TRUE;
		}
		else if ((csName.Find("HP") == 0) && (csName.Find("DJ") == 3))
		{
			fResult = TRUE;
		}
		else
		{
			csName = m_csDeviceName;
			csName.MakeUpper();

			if (csName.Find("DESKJET") >= 0)
			{
				fResult = TRUE;
			}
		}
	}
	END_TRY

	return fResult;
}

BOOL CPrinterInfo::IsEnvelopePaperSize(void) const
{
	short s = DevModePaperSize();

	BOOL fResult =
		(s == DMPAPER_ENV_9)
	|| (s == DMPAPER_ENV_10)
	|| (s == DMPAPER_ENV_11)
	|| (s == DMPAPER_ENV_12)
	|| (s == DMPAPER_ENV_14)
	|| (s == DMPAPER_ENV_DL)
	|| (s == DMPAPER_ENV_C5)
	|| (s == DMPAPER_ENV_C3)
	|| (s == DMPAPER_ENV_C4)
	|| (s == DMPAPER_ENV_C6)
	|| (s == DMPAPER_ENV_C65)
	|| (s == DMPAPER_ENV_B4)
	|| (s == DMPAPER_ENV_B5)
	|| (s == DMPAPER_ENV_B6)
	|| (s == DMPAPER_ENV_ITALY)
	|| (s == DMPAPER_ENV_MONARCH)
	|| (s == DMPAPER_ENV_PERSONAL)
#if(WINVER >= 0x0400)
	|| (s == DMPAPER_ENV_INVITE)
#endif
	;

	return fResult;
}

// Try to determine (to a reasonable degree of certainty) if the
// printer is a PostScript
//
//		We assume the printer is a DeskJet if:
//
//			1) The DC supports EPSPRINTING
//			2) The driver name contains "PSCRIPT".

int CPrinterInfo::DeterminePostScriptType(HDC hDC)
{
	BOOL fResult = FALSE;
	CString csName;

	int nPostScriptType = 0;

	TRY
	{
		// Test 1:
		// If the printer supports the EPSPRINTING escape, then we
		// assume it is a PostScript printer.

		int eps = EPSPRINTING;
		if (::Escape(hDC, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&eps, NULL))
		{
			// This is a PostScript printer.
			nPostScriptType = 1;
		}
		else
		{
			csName = m_csDriverName;
			csName.MakeUpper();

			if (csName.Find("PSCRIPT") >= 0)
			{
				nPostScriptType = 1;
			}
		}
	}
	END_TRY

	return nPostScriptType;
}

