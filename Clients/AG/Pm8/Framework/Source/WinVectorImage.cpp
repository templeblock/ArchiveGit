// *****************************************************************************
//
// File name:			WinVectorImage.cpp
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Windows-specific definition of the RVectorImage class members
//
// Portability:		Windows only
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
// Client:				Broderbund Software, Inc.
//
// Copyright (c) 1995-1996 Turning Point Software.  All rights reserved.
//
//  $Logfile:: /PM8/Framework/Source/WinVectorImage.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef _WINDOWS

#include "VectorImage.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "DcDrawingSurface.h"
#include "DataTransfer.h"
#include "ChunkyStorage.h"
#include "BitmapImage.h"
#include "OffscreenDrawingSurface.h"
#include "FileStream.h"
#include "PrintManager.h"
#include "Printer.h"
#include	"Document.h"
#include "PrinterDrawingSurface.h"
#include "DefaultDrawingSurface.h"

// Chunk ID
const uLONG kVectorImageChunkID = 0x02731276;

// ****************************************************************************
//
//  Function Name:	RVectorImage::RVectorImage()
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RVectorImage::RVectorImage()
{
	m_hEnhMetaFile = NULL;
	m_ySuggestedWidthInInches = kDefaultVectorImageWidth;
	m_ySuggestedHeightInInches = kDefaultVectorImageHeight;
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::RVectorImage()
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RVectorImage::RVectorImage(const RVectorImage& rhs)
{	
	Initialize((YImageHandle)::CopyEnhMetaFile((HENHMETAFILE)rhs.m_hEnhMetaFile, NULL));
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::~RVectorImage()
//
//  Description:		Destructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RVectorImage::~RVectorImage()
{
	if (m_hEnhMetaFile) ::DeleteEnhMetaFile(m_hEnhMetaFile);
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::Render()
//
//  Description:		Displays the image on the drawing surface
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::Render(RDrawingSurface& rDS, const RIntRect& rDestRect)
{
	TpsAssert(m_hEnhMetaFile, "m_hEnhMetaFile = NULL.");
	TpsAssert(rDS.GetSurface(), "rDS.GetSurface() returned NULL.");
		
	HDC hDC = (HDC)rDS.GetSurface();
	
	RECT tRect;
	tRect.left = rDestRect.m_Left;
	tRect.top = rDestRect.m_Top;
	tRect.right = rDestRect.m_Right;
	tRect.bottom = rDestRect.m_Bottom;

	// Call EnumEnhMetaFile() to play the metafile the way we want it	
	::EnumEnhMetaFile(hDC, m_hEnhMetaFile, reinterpret_cast<ENHMFENUMPROC>(EnhMetaFileProc), NULL, &tRect);	
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::Read()
//
//  Description:		(Re)creates the Vector from the information in the chunk
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::Read(RArchive& rArchive)
{	
	HENHMETAFILE hEnhMetaFile = NULL;
	uBYTE* pBuffer = NULL;
	try
	{		
		uLONG uBufferSize;
		rArchive >> uBufferSize;

		pBuffer = new uBYTE[uBufferSize];			
		rArchive.Read(uBufferSize, pBuffer);

		hEnhMetaFile = ::SetEnhMetaFileBits((DWORD)uBufferSize, (BYTE*)pBuffer);
		if (!hEnhMetaFile) ::ThrowExceptionOnOSError();
		Initialize((YImageHandle)hEnhMetaFile);

		// Clean up
		delete [] pBuffer;
	}
	catch(YException)
	{
		if (hEnhMetaFile) ::DeleteEnhMetaFile(hEnhMetaFile);
		delete [] pBuffer;
		throw;
	}		
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::Write()
//
//  Description:		Writes a Vector out to the given chunk
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::Write(RArchive& rArchive) const
{	
	// Do we have anything to write?
	if (!m_hEnhMetaFile) return;

	// Determine the size of the enhanced metafile, in bytes
	ENHMETAHEADER tEnhMetaFileHeader;
	if (!::GetEnhMetaFileHeader(m_hEnhMetaFile, sizeof(ENHMETAHEADER), &tEnhMetaFileHeader))
		::ThrowExceptionOnOSError();

	// Copy the enhanced metafile data into a buffer
	DWORD uBufferSize = tEnhMetaFileHeader.nBytes;	
	BYTE* pBuffer = NULL;
	try
	{
		pBuffer = new BYTE[uBufferSize];
		if (!::GetEnhMetaFileBits(m_hEnhMetaFile, uBufferSize, pBuffer))
			::ThrowExceptionOnOSError();

		rArchive << (uLONG)uBufferSize;			
		rArchive.Write((uLONG)uBufferSize, (uBYTE*)pBuffer);
		
		// Clean up
		delete [] pBuffer;
	}
	catch(YException)
	{
		delete [] pBuffer;
		throw;
	}
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::Copy()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::Copy(RDataTransferTarget& rDataTarget, YDataFormat yDataFormat) const
{	
	TpsAssert(yDataFormat == kPictureFormat || yDataFormat == kEnhancedMetafileFormat, "Unsupported format.");

	// Put up an enhanced metafile
	if(yDataFormat == kEnhancedMetafileFormat)
	{
		// Copy the metafile
		HENHMETAFILE hEnhMetaFile = ::CopyEnhMetaFile(m_hEnhMetaFile, NULL);
		if(!hEnhMetaFile) throw ::kMemory;

		// Set the metafile into the data transfer target.
		rDataTarget.SetData(kEnhancedMetafileFormat, hEnhMetaFile);
	}

	// Get the metafile bits in 16-bit Windows format
	else
	{
		// Global alloc enough memory for a METAFILEPICT structure
		HGLOBAL hGlobal = ::GlobalAlloc(GHND, sizeof(METAFILEPICT));
		if(!hGlobal) throw ::kMemory;

		// Get a pointer to the METAFILEPICT structure
		METAFILEPICT* pMetafilePict = reinterpret_cast<METAFILEPICT*>(::GlobalLock(hGlobal));

		// Get the enhanced metafile header for our metafile
		ENHMETAHEADER tEnhMetaHeader;
		::GetEnhMetaFileHeader(m_hEnhMetaFile, sizeof(ENHMETAHEADER), &tEnhMetaHeader);			

		// Fill in the fields in the METAFILEPICT structure
		pMetafilePict->mm = MM_ANISOTROPIC;
		pMetafilePict->xExt = tEnhMetaHeader.rclFrame.right - tEnhMetaHeader.rclFrame.left;
		pMetafilePict->yExt = tEnhMetaHeader.rclFrame.bottom - tEnhMetaHeader.rclFrame.top;
		pMetafilePict->hMF = GetWindowsMetaFile( );

		// Unlock the METAFILEPICT structure
		::GlobalUnlock( hGlobal );

		// Give it to the data transfer target
		rDataTarget.SetData(kPictureFormat, hGlobal);
	}
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::Paste()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::Paste(const RDataTransferSource& rDataSource, YDataFormat yDataFormat)
{
	HENHMETAFILE hEnhMetaFile = NULL;
	if(yDataFormat == kEnhancedMetafileFormat)
	{
		// Get the global handle from the data source
		hEnhMetaFile = static_cast<HENHMETAFILE>(rDataSource.GetData(kEnhancedMetafileFormat));
		if(!hEnhMetaFile) throw kMemory;
	}
	else if(yDataFormat == kPictureFormat)
	{
		// Get down data in the old Windows metafile format
		HGLOBAL hGlobal = rDataSource.GetData(kPictureFormat);
		METAFILEPICT* pMetafilePict = reinterpret_cast<METAFILEPICT*>(::GlobalLock(hGlobal));

		// Figure out how big the metafile data is
		UINT uBufferSize = ::GetMetaFileBitsEx(pMetafilePict->hMF, 0, NULL);

		// Allocate enough memory for the bits
		BYTE* pMetafileBits = new BYTE[uBufferSize];

		// Get the bits this time
		::GetMetaFileBitsEx(pMetafilePict->hMF, uBufferSize, pMetafileBits);

		// Get a reference DC
		RDrawingSurface* pDS = NULL;
		BOOLEAN fPrinterDS = FALSE;
		RPrinter* pDefaultPrinter = RPrintManager::GetPrintManager().GetDefaultPrinter();
		if (pDefaultPrinter && !pDefaultPrinter->IsTemplate())
		{
			pDS = pDefaultPrinter->CreateDrawingSurface( NULL );
			fPrinterDS = TRUE;
		}
		else
		{
			pDS = new RDefaultDrawingSurface;
		}
		TpsAssert(pDS, "Could not create reference DC.");
		
		// Convert the bits to an enhanced metafile
		hEnhMetaFile = ::SetWinMetaFileBits(uBufferSize, pMetafileBits, reinterpret_cast<HDC>(pDS->GetSurface()), pMetafilePict);
		
		// Cleanup		
		::GlobalUnlock(hGlobal);
		::GlobalFree(hGlobal);
		delete [] pMetafileBits;
		if (fPrinterDS)
		{
			RPrinterDrawingSurface* pPDS = static_cast<RPrinterDrawingSurface*>(pDS);
			pDefaultPrinter->DestroyDrawingSurface(pPDS);	
		}
		else
		{
			delete pDS;
		}
		pDS = NULL;
	}
	else
	{
		TpsAssertAlways("Unexpected format in Paste.");
	}
	
	// Set the metafile handle
	Initialize((YImageHandle)hEnhMetaFile);
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::operator=()
//
//  Description:		operator=
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::operator=(const RImage& rhs)
{
	// Make a copy of this metafile
	const RVectorImage* pVector = dynamic_cast<const RVectorImage*>(&(const_cast<RImage&>(rhs)));
	TpsAssert(pVector, "Bad operand in RVectorImage::operator=().");	

	// Check for assignment to self
	if (this == pVector) return;

	// Call the base member
	RImage::operator=(rhs);	
	
	// Call the RVectorImage member
	operator=(*pVector);
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::operator=()
//
//  Description:		operator=
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::operator=(const RVectorImage& rhs)
{
	// Check for assignment to self
	if (this == &rhs) return;

	// Copy the metafile
	Initialize((YImageHandle)::CopyEnhMetaFile((HENHMETAFILE)rhs.m_hEnhMetaFile, NULL));
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::GetWidthInInches()
//
//  Description:		Returns the width of the vector, in inches
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YRealDimension RVectorImage::GetWidthInInches( ) const
{
	// Set the size members here	
	// Implies that we must set default values when we load a non-enhanced metafile
	ENHMETAHEADER tEnhMetaFileHeader;
	if (!::GetEnhMetaFileHeader(m_hEnhMetaFile, sizeof(ENHMETAHEADER), &tEnhMetaFileHeader))
		::ThrowExceptionOnOSError();

	LONG sWidth = tEnhMetaFileHeader.rclFrame.right
		- tEnhMetaFileHeader.rclFrame.left;
	
	// Convert to logical units
	return ::MillimetersToInches((YFloatType)sWidth / 100.0);	
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::GetHeightInInches()
//
//  Description:		Returns the height of the vector, in inches
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YRealDimension RVectorImage::GetHeightInInches( ) const
{
	// Set the size members here	
	// Implies that we must set default values when we load a non-enhanced metafile
	ENHMETAHEADER tEnhMetaFileHeader;
	if (!::GetEnhMetaFileHeader(m_hEnhMetaFile, sizeof(ENHMETAHEADER), &tEnhMetaFileHeader))
		::ThrowExceptionOnOSError();

	LONG sHeight = tEnhMetaFileHeader.rclFrame.bottom
		- tEnhMetaFileHeader.rclFrame.top;
	
	// Convert to logical units
	return ::MillimetersToInches((YFloatType)sHeight / 100.0);
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::GetSuggestedWidthInInches()
//
//  Description:		Returns the suggested width of the vector, in inches
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YRealDimension RVectorImage::GetSuggestedWidthInInches( ) const
{
	return m_ySuggestedWidthInInches;
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::GetSuggestedHeightInInches()
//
//  Description:		Returns the suggested height of the vector, in inches
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YRealDimension RVectorImage::GetSuggestedHeightInInches( ) const
{
	return m_ySuggestedHeightInInches;
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::SetSuggestedWidthInInches()
//
//  Description:		Returns the suggested width of the vector, in inches
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::SetSuggestedWidthInInches(YRealDimension ySuggestedWidthInInches)
{
	m_ySuggestedWidthInInches = ySuggestedWidthInInches;
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::SetSuggestedHeightInInches()
//
//  Description:		Returns the suggested height of the vector, in inches
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::SetSuggestedHeightInInches(YRealDimension ySuggestedHeightInInches)
{
	m_ySuggestedHeightInInches = ySuggestedHeightInInches;
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::GetSystemHandle()
//
//  Description:		Returns the system handle of the vector image, an HENHMETAFILE
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YImageHandle RVectorImage::GetSystemHandle() const
{
	return (YImageHandle)m_hEnhMetaFile;
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::GetSizeInLogicalUnits( )
//
//  Description:		Returns the size in RLUs
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RVectorImage::GetSizeInLogicalUnits( ) const
{
	return RRealSize(::InchesToLogicalUnits(GetWidthInInches()), ::InchesToLogicalUnits(GetHeightInInches()));
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::SetEnhancedMetaFile()
//
//  Description:		Sets the ENHMETAFILE handle
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RVectorImage::Initialize(YImageHandle yHandle)
{
	HENHMETAFILE hEnhMetaFile = (HENHMETAFILE)yHandle;
	if (m_hEnhMetaFile) ::DeleteEnhMetaFile(m_hEnhMetaFile);
	m_hEnhMetaFile = hEnhMetaFile;
}

// ****************************************************************************
//
//  Function Name:	RVectorImage::GetWindowsMetaFile()
//
//  Description:		Converts this vector image to a windows format metafile
//
//  Returns:			Handle to the converted metafile
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
HMETAFILE RVectorImage::GetWindowsMetaFile( ) const
{
	// Get a reference DC for the conversion
	RDrawingSurface* pDS = NULL;
	BYTE* pMetafileBits = NULL;
	HMETAFILE hMetafile = NULL;
	try
	{
		RDrawingSurface* pDS = NULL;
		BOOLEAN fPrinterDS = FALSE;
		RPrinter* pDefaultPrinter = RPrintManager::GetPrintManager().GetDefaultPrinter();
		if (pDefaultPrinter && !pDefaultPrinter->IsTemplate())
		{
			pDS = pDefaultPrinter->CreateDrawingSurface( NULL );
			fPrinterDS = TRUE;
		}
		else
		{
			pDS = new RDefaultDrawingSurface;
		}

		// Figure out how big the new metafile data is
		UINT uBufferSize = ::GetWinMetaFileBits(m_hEnhMetaFile, 0, NULL, MM_ANISOTROPIC, reinterpret_cast<HDC>(pDS->GetSurface()));		

		// Allocate enough memory for the bits
		pMetafileBits = new BYTE[uBufferSize];

		// Get the bits this time
		::GetWinMetaFileBits(m_hEnhMetaFile, uBufferSize, pMetafileBits, MM_ANISOTROPIC, reinterpret_cast<HDC>(pDS->GetSurface()));

		// Convert the bits to a Windows metafile
		hMetafile = ::SetMetaFileBitsEx(uBufferSize, pMetafileBits);

		// Cleanup
		delete [] pMetafileBits;
		if (fPrinterDS)
		{
			RPrinterDrawingSurface* pPDS = static_cast<RPrinterDrawingSurface*>(pDS);
			pDefaultPrinter->DestroyDrawingSurface(pPDS);	
		}
		else
		{
			delete pDS;
		}
		pDS = NULL;
	}
	catch(YException)
	{
		if (hMetafile) ::DeleteMetaFile(hMetafile);
		delete [] pMetafileBits;
		delete pDS;
		throw;
	}

	return hMetafile;
}

int CALLBACK RVectorImage::EnhMetaFileProc(HDC hDC, HANDLETABLE* pHTable, ENHMETARECORD* pEMR, int nObj, LPARAM)
{
	// We ignore all palette functions and force our own palette into the DC during processing
	// of the first record, EMR_HEADER.  We clean up the palette during processing of the last
	// record, EMR_EOF.
	switch(pEMR->iType)
	{
	case EMR_HEADER:
		{
			// Make a copy of the Renaissance palette
			UINT nPaletteEntryCount = ::GetPaletteEntries((HPALETTE)tbitGetScreenPalette(), 0, 0, NULL);
			LOGPALETTE* pLogicalPalette = reinterpret_cast<LOGPALETTE*>(new BYTE[sizeof(LOGPALETTE) + (nPaletteEntryCount - 1) * sizeof(PALETTEENTRY)]);
			pLogicalPalette->palVersion = 0x300;
			pLogicalPalette->palNumEntries = nPaletteEntryCount;
			::GetPaletteEntries((HPALETTE)tbitGetScreenPalette(), 0, nPaletteEntryCount, pLogicalPalette->palPalEntry);

			// Create the palette and select it into the DC - RIP Ensure the palette returned is destroyed.
			HPALETTE hOldPal;
			HPALETTE hNewPal = ::CreatePalette(pLogicalPalette);
			VERIFY(hOldPal = ::SelectPalette(hDC, hNewPal, FALSE));
			VERIFY(::RealizePalette(hDC) != GDI_ERROR);

			// Once the palette has been realized, the old palette isn't needed,
			// so return the original palette and destroy the one we just built. -RIP-
			::SelectPalette( hDC, hOldPal, FALSE );
			::DeleteObject( hNewPal );

			// Clean up	the logical palette memory.
			delete [] reinterpret_cast<BYTE*>(pLogicalPalette);
		}
		break;

	case EMR_EOF:
		{
			// Select the old (stock) palette back into the DC - RIP No longer necessary due to changes above.
//			VERIFY(::DeleteObject(::SelectPalette(hDC, reinterpret_cast<HPALETTE>(::GetStockObject(DEFAULT_PALETTE)), FALSE)));
		}
		break;

	case EMR_CREATEPALETTE:
	case EMR_SELECTPALETTE:
	case EMR_REALIZEPALETTE:
	case EMR_SETPALETTEENTRIES:		
	case EMR_RESIZEPALETTE:		
		return TRUE;

	case EMR_STRETCHBLT:
	case EMR_STRETCHDIBITS:
		// Set the StretchBlt mode to COLORONCOLOR so that pixels are deleted, not combined	
		::SetStretchBltMode(hDC, COLORONCOLOR);
		break;
	}

	// Let Windows handle the record
	return ::PlayEnhMetaFileRecord(hDC, pHTable, pEMR, nObj);
}

#endif _WINDOWS