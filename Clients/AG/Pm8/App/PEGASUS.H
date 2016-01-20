/*
// $Workfile: PEGASUS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PEGASUS.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 8     2/09/99 5:24p Mwilson
// Fixed 8 bit scale
// 
// 7     2/09/99 4:48p Mwilson
// updated to handle zooming and alpha channel
// 
// 6     2/08/99 5:51p Guidom
// Added zoom function to scale to a specific CSize
// 
// 5     2/08/99 2:47p Guidom
// Added color reduction and fixed up scale.
// 
// 4     2/03/99 10:39p Guidom
// Added support for zooming DIBs and updated to the 2.0 version of
// Pegasus.
// 
// 3     6/17/98 11:06a Jayn
// 
// 2     12/05/97 4:17p Johno
// Added WriteJPEGLinesReversed for "bottom up" dibs.
// 
//    Rev 1.0   14 Aug 1997 15:23:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:06   Fred
// Initial revision.
// 
//    Rev 1.2   02 Jun 1997 13:12:42   Jay
// Fixed 16-bit problems
// 
//    Rev 1.1   22 May 1997 16:13:20   Jay
// More new JPEG stuff.
// 
//    Rev 1.0   22 May 1997 10:29:16   Jay
// Initial revision.
*/

#ifndef __PEGASUS_H__
#define __PEGASUS_H__

#include "pegasus\pic.h"
#include "pegasus\errors.h"
#include "CDib.h"

class StorageDevice;

//
// Abstract Pegasus input class.
//

class CPegasusInput
{
public:
	CPegasusInput() {}
	virtual ~CPegasusInput() {}

	// Fill the queue.
	virtual long FillQueue(QUEUE* pQueue) = 0;
};

//
// Standard CPegasusInput from a StorageDevice.
//

class CPegasusInputDevice : public CPegasusInput
{
public:
	CPegasusInputDevice(StorageDevice* pDevice = NULL);
	virtual ~CPegasusInputDevice();

	// Initialization
	void SetDevice(StorageDevice* pDevice)
		{ m_pDevice = pDevice; }
	// Fill the queue.
	virtual long FillQueue(QUEUE* pQueue);
protected:
	// The device.
	StorageDevice* m_pDevice;
};

//
// Abstract Pegasus output class.
//

class CPegasusOutput
{
public:
	CPegasusOutput() {}
	virtual ~CPegasusOutput() {}

	// Empty the queue.
	virtual long EmptyQueue(QUEUE* pQueue) = 0;
};

//
// Standard Pegasus output to a StorageDevice.
//

class CPegasusOutputDevice : public CPegasusOutput
{
public:
	CPegasusOutputDevice(StorageDevice* pDevice = NULL);
	virtual ~CPegasusOutputDevice();

	// Initialization.
	void SetDevice(StorageDevice* pDevice)
		{ m_pDevice = pDevice; }

	// Empty the queue.
	virtual long EmptyQueue(QUEUE* pQueue);

	// Compressed bytes written.
	DWORD BytesWritten(void) const
		{ return m_dwBytesWritten; }

protected:
	// The device.
	StorageDevice* m_pDevice;
	DWORD m_dwBytesWritten;
};

//
// Wrapper for the Pegasus library.
//

class CPegasus
{
public:
	CPegasus(int nDefaultGetQueueSize = 50000,int nDefaultPutQueueSize = 16384);
	virtual ~CPegasus();

	// Queue management.
	void FreeQueue(QUEUE* pQueue);
	long AllocateQueue(QUEUE* pQueue, long lQueueSize);
   long AllocateGetQueue(long lGetQueueSize = -1);
   long AllocatePutQueue(long lPutQueueSize = -1);	// Normally internally called.

	// Blank a pic parm.
	static void BlankPicParm(PIC_PARM* pPicParm);

	// Get image info (width, height, etc.)
	long QueryImageInfo(PIC_PARM* pPicParm = NULL);

	PIC_PARM* GetPicParm(void)
		{ return &m_PicParm; }

	// Set the input provider.
	void SetInput(CPegasusInput* pInput)
		{ m_pInput = pInput; }

	// Set the output provider.
	void SetOutput(CPegasusOutput* pOutput)
		{ m_pOutput = pOutput; }

	// Fill the input queue.
	long FillInputQueue(void);

	// Empty the output queue.
	long EmptyOutputQueue(void);

   // Call the Pegasus routine.
   RESPONSE CallPegasus(REQUEST lRequest, PIC_PARM* pPicParm = NULL);

   // Do a Pegasus loop.
	long DoIt(REQUEST lRequest, RESPONSE lUntilResponse = RES_DONE);

	// Data parameters.
	int ImageWidth(void) const;
	int ImageHeight(void) const;
	int ColorComponents(void) const;

	//
	// JPEG Decompression.
	//

	long StartJPEGDecompression(void);
	long ReadJPEGLines(int nLine, LPBYTE pLines, int nLineCount);
	long FinishJPEGDecompression(void);
	long AbortJPEGDecompression(void);

	//
	// JPEG Compression.
	//

	long StartJPEGCompression(int nWidth, int nHeight, int nColorComponents);
	long WriteJPEGLines(LPBYTE pLines, int nLineCount);
	long WriteJPEGLinesReversed(LPBYTE pLines, int nLineCount);
	long FinishJPEGCompression(void);
	long AbortJPEGCompression(void);

	//
	// DIB Scaling
	//
	long ZoomDib(const RBitmapImage &InputDib, RBitmapImage &OutputDib, CSize new_size, BOOL bMakeGrayScale = FALSE);
	long ZoomDib(const RBitmapImage &InputDib, RBitmapImage &OutputDib, long zoom_percent, BOOL bMakeGrayScale = FALSE);

	// Image color-reduction
	long ChangeColorDepth(const RBitmapImage &InputDib, RBitmapImage &OutputDib, BYTE *ColorMap);

private:
	long CallPegasusWithExternalBuffer();

protected:
	PIC_PARM	m_PicParm;

	CPegasusInput* m_pInput;
	CPegasusOutput* m_pOutput;

   int m_nDefaultGetQueueSize;
   int m_nDefaultPutQueueSize;

	// Next line during compress or decompress.
	long m_lBytesRead;
	REQUEST m_lNextRequest;
	int m_nNextLine;

	// Little helper function to enclose compression call.
	long CompressGetBuffer(RESPONSE lUntilResponse);

	// used for color reduction
	static BYTE m_arColorMap[0x8000];
	static BOOL	m_bColorMapLoaded;
};

#endif
