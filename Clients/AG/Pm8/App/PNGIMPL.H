/*
// PNG.H
//
// PNG helper and driver definitions.
//
// $Header: /PM8/App/PNGIMPL.H 2     3/08/99 7:15p Lwilson $
//
// $Log: /PM8/App/PNGIMPL.H $
// 
// 2     3/08/99 7:15p Lwilson
// Changed Png.h dependency to new directory.
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 5     2/08/99 3:33p Mwilson
// added routine to spilt png file int 2 dibs.  One for Alpha channel and
// one for color data.
// 
// 4     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 3     11/25/97 10:35a Johno
// Added CPNGWriter::WriteDIB - does "bottom up" dib write
// 
// 2     11/19/97 5:32p Johno
// Added Jay's PNG writer code, modified for CStorageDevice
// 
//    Rev 1.0   17 Sep 1997 09:11:18   Jay
// Initial revision.
*/

#ifndef __PNG_H__
#define __PNG_H__

class GraphicObject;
class StorageDevice;

#include "imagedrv.h"
#include "file.h"
#include "png.h"
#include	"grafrec.h"
#include	"grafobj.h"

/*
// PNG library wrapper.
*/

class CPNGReader
{
public:
	CPNGReader();
	virtual ~CPNGReader();

	ERRORCODE Initialize(StorageDevice* pDevice, BOOL fReadInfo = TRUE);
	ERRORCODE ReadInfo(void);
	ERRORCODE ReadImage(void);
   ERRORCODE AllocateRows(void);
	LPBYTE GetRow(int nRow);

	png_structp GetStruct(void)
		{ return m_pPNG; }

	png_infop GetInfo(void)
		{ return m_pInfo; }

protected:
	// Callback functions.
	static void OnError(png_structp pStruct, png_const_charp pszError);
	static void OnWarning(png_structp pStruct, png_const_charp pszWarning);
	static void OnRead(png_structp pStruct, png_bytep pData, png_size_t nSize);

	virtual void ErrorFunction(png_structp pStruct, png_const_charp pszError);
	virtual void WarningFunction(png_structp pStruct, png_const_charp pszWarning);
	virtual void ReadFunction(png_structp pStruct, png_bytep pData, png_size_t nSize);

	// 
	// Data.
	//
	png_structp m_pPNG;
   png_infop m_pInfo;

	StorageDevice* m_pDevice;

	LPBYTE* m_pRows;
	LPBYTE m_pRowMem;
};

/*
//	For PNG file loading, the helper definition.
*/

class PNGHelper : public GraphicHelper
{
public:	

/*
// The pertinent data.
*/

	struct
	{
		WORD m_wColorType;					// PNG color type
		BYTE m_bBitDepth;						// 1, 2, 4, 8, or 16 bits/channel
	} m_Record;

/*
// The constructor.
*/

	PNGHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

/*
// Overridden methods (for storage purposes).
*/

	ERRORCODE read(StorageDevicePtr device);
	ERRORCODE write(StorageDevicePtr device);
	ST_DEV_IO_SIZE size(StorageDevicePtr device);

/*
// The init function.
*/

	ERRORCODE init(GRAPHIC_CREATE_STRUCT_PTR gcs);

	// Global default resolutions.
	static POINT m_pntResolution;
};

/*
// The PNG file driver.
*/

class PNGDriver : public ImageDriver
{
protected:
	ReadOnlyFile file;
	GraphicPtr graphic;
	BOOL m_fRead;
public:
	PNGDriver(GraphicPtr g, StorageDevice* pSource = NULL);
	virtual ~PNGDriver();

	CPNGReader m_Reader;

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);
	virtual void close_image(VOID);
};

//
// The PNG writer.
//

class CPNGWriter
{
public:
	CPNGWriter();
	virtual ~CPNGWriter();

   virtual int Initialize (StorageDevice *pdev);

   virtual int SetInfo (int nChannels, int nRowBytes, int nWidth, int nHeight);
	virtual int WriteData(LPBYTE pData, BOOL UseBGR = TRUE);
	virtual int WriteDIB (LPBYTE pData, BOOL UseBGR = TRUE);

protected:
   StorageDevice  *m_pdev;
	png_structp    m_pPNG;
	png_infop      m_pInfo;
	int            m_nRowBytes;

	// Callback functions.
	static void OnError(png_structp pStruct, png_const_charp pszError);
	static void OnWarning(png_structp pStruct, png_const_charp pszWarning);
	static void OnWrite(png_structp pStruct, png_bytep pData, png_size_t nSize);
	static void OnFlush(png_structp pStruct);

	virtual void ErrorFunction(png_structp pStruct, png_const_charp pszError);
	virtual void WarningFunction(png_structp pStruct, png_const_charp pszWarning);
	virtual void WriteFunction(png_structp pStruct, png_bytep pData, png_size_t nSize);
	virtual void FlushFunction(png_structp pStruct);
};

#endif							// #ifndef __PNG_H__
