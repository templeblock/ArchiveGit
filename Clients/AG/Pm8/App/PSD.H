/*
// PSD.H
//
// Photoshop (PSD) definitions.
//
// $Header: /PM8/App/PSD.H 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/PSD.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:40   Fred
// Initial revision.
// 
//    Rev 1.4   05 Jun 1997 16:44:48   Jay
// HasTransparency
// 
//    Rev 1.3   30 Apr 1997 10:40:28   Jay
// Now reads resolution from PSD file.
// 
//    Rev 1.2   11 Apr 1997 11:23:50   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.1   05 Dec 1996 12:30:40   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.0   01 Nov 1996 10:26:06   Jay
// Initial revision.
*/

#ifndef __PSD_H__
#define __PSD_H__

#include "imagedrv.h"
#include "file.h"

/*
//	For PSD file loading, the header definition.
*/

struct PSDHeader
{
	DWORD m_dwSignature;
	WORD m_wVersion;
	BYTE m_bReserved[6];
	WORD m_wChannels;
	DWORD m_dwRows;
	DWORD m_dwColumns;
	WORD m_wDepth;
	WORD m_wMode;
};

struct PSDResolutionInfo
{
	DWORD m_dwHRes;      // Fixed
	WORD m_wHResUnit;
	WORD m_wWidthUnit;

	DWORD m_dwVRes;      // Fixed
	WORD m_wVResUnit;
	WORD m_wHeightUnit;
};

class PSDHelper : public GraphicHelper
{
public:	

	// Constants
	enum EConstants
	{
		MAX_CHANNELS = 8
	};

/*
// The pertinent data.
*/

	struct
	{
		WORD m_wChannels;
		WORD m_wDepth;
		WORD m_wMode;
		WORD m_wCompression;
		ST_DEV_POSITION m_lColorDataSection;
		ST_DEV_POSITION m_lImageResourcesSection;
		ST_DEV_POSITION m_lLayerMaskSection;
		ST_DEV_POSITION m_lImageDataSection;
		ST_DEV_POSITION m_lChannelSizes[MAX_CHANNELS];	// Size of each channel.
	} m_Record;

/*
// The constructor.
*/

	PSDHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

/*
// Overridden methods (for storage purposes).
*/

	virtual ERRORCODE read(StorageDevicePtr device);
	virtual ERRORCODE write(StorageDevicePtr device);
	virtual ST_DEV_IO_SIZE size(StorageDevicePtr device);

/*
// The init function.
*/

	virtual ERRORCODE init(GRAPHIC_CREATE_STRUCT_PTR gcs);
	virtual BOOL HasTransparency(void);

	// Return the number of channels.
	int GetChannels(void) const
		{ return (int)m_Record.m_wChannels; }

	// Read the compression counts.
	// Will not read anything if not compressed.
	ERRORCODE ReadCompressionCounts(StorageDevice* pFile, LPWORD& pSizes);

	// Return the start of actual image data (past compression info).
	ST_DEV_POSITION ImageStart(void);
protected:

	// Read the resolution. This assumes m_lImageResourcesSection is set.
	void ReadResolution(StorageDevice* pDevice);

	// Seek a particular image resource.
	ERRORCODE SeekImageResource(StorageDevice* pFile, WORD wSearchID, DWORD& dwDataSize);

	// Read a p-string from a PSD file.
	ERRORCODE ReadPString(StorageDevice* pFile, CString& csString);
};

/*
// The PSD file driver.
*/

class PSDDriver : public ImageDriver
{
public:
	PSDDriver(GraphicPtr g, StorageDevice* pSource = NULL);
	virtual ~PSDDriver();

protected:
	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);

	ERRORCODE Rewind(void);
	ERRORCODE SetupChannelFiles(void);
	ERRORCODE ReadAChannel(LPBYTE buffer, int nChannel, int nChannels);

	ReadOnlyFile file;

	CPtrArray m_ChannelFiles;
	CDWordArray m_ChannelPositions;
	CDWordArray m_ChannelLowPositions;

	GraphicPtr graphic;

	LPWORD m_pSizes;			// The array of compressed sizes.
	int m_nCurrentLine;
	int m_nChannels;			// Faster access.
	int m_nRows;
	int m_nColumns;
	int m_nRowBytes;
	WORD m_wCompression;
};

#endif							// #ifndef __PSD_H__
