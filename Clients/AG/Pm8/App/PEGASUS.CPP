/*
// $Workfile: PEGASUS.CPP $
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
// $Log: /PM8/App/PEGASUS.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 13    2/23/99 5:29p Guidom
// Fixed registration messages.
// 
// 12    2/19/99 12:29p Mwilson
// modified color reduction call
// 
// 11    2/11/99 11:37a Mwilson
// fixed scale routine
// 
// 10    2/09/99 5:24p Mwilson
// Fixed 8 bit scale
// 
// 9     2/09/99 4:48p Mwilson
// updated to handle zooming and alpha channel
// 
// 8     2/09/99 2:26p Guidom
// Updated some of the registration op-codes.  Still need to track down
// one more.
// 
// 7     2/08/99 5:51p Guidom
// Added zoom function to scale to a specific CSize
// 
// 6     2/08/99 2:47p Guidom
// Added color reduction and fixed up scale.
// 
// 5     2/03/99 10:39p Guidom
// Added support for zooming DIBs and updated to the 2.0 version of
// Pegasus.
// 
// 4     6/17/98 11:06a Jayn
// 
// 3     12/23/97 10:27a Jayn
// 
// 2     12/05/97 4:16p Johno
// Added WriteJPEGLinesReversed for "bottom up" dibs.
// 
//    Rev 1.0   14 Aug 1997 15:23:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:40   Fred
// Initial revision.
// 
//    Rev 1.3   02 Jun 1997 13:12:40   Jay
// Fixed 16-bit problems
// 
//    Rev 1.2   23 May 1997 16:14:24   Jay
// More optimized defaults for JPEG compression.
// 
//    Rev 1.1   22 May 1997 16:13:18   Jay
// More new JPEG stuff.
// 
//    Rev 1.0   22 May 1997 10:29:14   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pegasus.h"
#include "util.h"
#include "bitmapimage.h"

// in bmp.cpp
extern void InitBitmapInfoHeader(LPBITMAPINFOHEADER lpBmInfoHdr,
                                 DWORD dwWidth,
                                 DWORD dwHeight,
                                 int nBPP);

/////////////////////////////////////////////////////////////////////////////
// Utility functions.

long FindFreeSpace(QUEUE* pQueue)
{
   long lFreeSpace = 0;
   // is the rear after the front?
   if (pQueue->Rear >= pQueue->Front)
   {
      // if rear not at end of buffer, use space between rear and end
      if (pQueue->Rear < pQueue->End)
      {
         lFreeSpace = pQueue->End - pQueue->Rear;
      }
      else
      {
         // rear at end
         // is start also at end?
         if (pQueue->Front == pQueue->End)
         {
            // both rear and front at end, --> empty buffer
            // move front and rear to start, fill whole buffer
            pQueue->Front = pQueue->Start;
            pQueue->Rear = pQueue->Start;
            lFreeSpace = pQueue->End - pQueue->Start;
         }
         else
         {
            if (pQueue->Front > pQueue->Start)
            {
               // if front after start, use space between front and start
               // leave a space empty since front=rear means queue empty
               // omitting -1 leads to serious problems!!!!
               lFreeSpace = pQueue->Front - pQueue->Start - 1;
               // move rear to start since data written at front (present rear)
               pQueue->Rear = pQueue->Start;
            }
            else
            {
               // if rear at end and front before or at start, queue is full
               lFreeSpace = 0;
            }
         }
      }
   }
   else
   {
      // rear = front means empty, so leave space
      // omitting -1 leads to serious problems!!!!
      lFreeSpace = pQueue->Front - pQueue->Rear - 1;
   }
   return lFreeSpace;
}

/////////////////////////////////////////////////////////////////////////////
// CPegasusInputDevice

// Constructor.
CPegasusInputDevice::CPegasusInputDevice(StorageDevice* pDevice /*=NULL*/)
{
   m_pDevice = pDevice;
}

// Destructor.
CPegasusInputDevice::~CPegasusInputDevice()
{
}

// Fill the queue.
long CPegasusInputDevice::FillQueue(QUEUE* pQueue)
{
   // Make sure we have a queue.
   if (pQueue->Start == NULL)
   {
      // Queue is not allocated.
      ASSERT(FALSE);
      return ERR_GET_BUFF_ERR;
   }

   // Make sure we have a device.
   if (m_pDevice == NULL)
   {
      // Device is not set
      ASSERT(FALSE);
      return ERR_GET_BUFF_ERR;
   }

   // Add data to the queue until full or we exhaust the source.
   long lFreeSpace;
   while ((lFreeSpace = FindFreeSpace(pQueue)) != 0)
   {
      DWORD dwSizeRead;
      if (m_pDevice->huge_read(pQueue->Rear, lFreeSpace, &dwSizeRead) != ERRORCODE_None)
      {
         // Some sort of non-EOF error.
         return ERR_BAD_READ;
      }
      // We read something (maybe zero bytes).
      pQueue->Rear += dwSizeRead;
      if ((long)dwSizeRead < lFreeSpace)
      {
         // Out of source data.
         pQueue->QFlags |= Q_EOF;
         break;
      }
   }
   return ERR_NONE;
}

/////////////////////////////////////////////////////////////////////////////
// CPegasusOutputDevice

// Constructor.
CPegasusOutputDevice::CPegasusOutputDevice(StorageDevice* pDevice /*=NULL*/)
{
   m_pDevice = pDevice;
   m_dwBytesWritten = 0;
}

// Destructor.
CPegasusOutputDevice::~CPegasusOutputDevice()
{
}

// Empty a queue.
long CPegasusOutputDevice::EmptyQueue(QUEUE* pQueue)
{
   // Make sure we have a queue.
   if (pQueue->Start == NULL)
   {
      // Queue is not allocated.
      ASSERT(FALSE);
      return ERR_PUT_BUFF_ERR;
   }

   long lBuffSize = pQueue->Rear - pQueue->Front;
   ASSERT(lBuffSize >= 0);
   if (lBuffSize > 0)
   {
      // Note: m_pDevice can be NULL if we are writing to the ether (counting bytes).
      if (m_pDevice != NULL)
      {
         if (m_pDevice->huge_write(pQueue->Front, lBuffSize) != ERRORCODE_None)
         {
            return ERR_BAD_WRITE;
         }
      }
      // Reset the queue.
      pQueue->Rear = pQueue->Front;
      m_dwBytesWritten += lBuffSize;
   }

   return ERR_NONE;
}

/////////////////////////////////////////////////////////////////////////////
// CPegasus

// CPegasus static members

BOOL	CPegasus::m_bColorMapLoaded = FALSE;

BYTE CPegasus::m_arColorMap[0x8000];

// Constructor.
CPegasus::CPegasus(int nDefaultGetQueueSize /*=50000*/, int nDefaultPutQueueSize /*=16384*/)
{
   BlankPicParm(&m_PicParm);
   m_nDefaultGetQueueSize = nDefaultGetQueueSize;
   m_nDefaultPutQueueSize = nDefaultPutQueueSize;
   m_nNextLine = -1;
}

// Destructor.
CPegasus::~CPegasus()
{
   FreeQueue(&m_PicParm.Get);
   FreeQueue(&m_PicParm.Put);
}

// Allocate the Get queue.
long CPegasus::AllocateGetQueue(long lGetQueueSize /*=-1*/)
{
   long lStatus = ERR_NONE;

   // Set default values if necessary.
   if (lGetQueueSize < m_nDefaultGetQueueSize)
   {
      lGetQueueSize = m_nDefaultGetQueueSize;
   }

   // Allocate the Get queue if desired.
   return AllocateQueue(&m_PicParm.Get, lGetQueueSize);
}

// Allocate the Put queue.
long CPegasus::AllocatePutQueue(long lPutQueueSize /*=-1*/)
{
   long lStatus = ERR_NONE;

   // Set default values if necessary.
   if (lPutQueueSize < m_nDefaultPutQueueSize)
   {
      lPutQueueSize = m_nDefaultPutQueueSize;
   }

   // Allocate the Put queue if desired.
   return AllocateQueue(&m_PicParm.Put, lPutQueueSize);
}

// Allocate a queue.
long CPegasus::AllocateQueue(QUEUE* pQueue, long lQueueSize)
{
   long lStatus = ERR_NONE;

   // Do we have a queue start?
   if (pQueue->Start == NULL)
   {
      // We need to allocate the queue.
#ifdef WIN32
      TRY
      {
         pQueue->Start = new BYTE[lQueueSize];
      }
      END_TRY
#else
      pQueue->Start = (BYTE PICHUGE*)halloc(lQueueSize, 1);
#endif

      // If we allocated the queue, set the pointers.
      if (pQueue->Start == NULL)
      {
         lStatus = ERR_MEMORY_ALLOCATION_ERROR;
      }
      else
      {
         // Initialize the queue.
         pQueue->End = pQueue->Start + lQueueSize;
         pQueue->Front =
            pQueue->Rear = pQueue->Start;
         pQueue->QFlags = 0;
      }
   }

   return lStatus;
}

// Free any allocated queue.
void CPegasus::FreeQueue(QUEUE* pQueue)
{
#ifdef WIN32
   delete [] (LPBYTE)pQueue->Start;
#else
   if (pQueue->Start != NULL)
   {
      hfree(pQueue->Start);
   }
#endif
   pQueue->Start = NULL;
}

void CPegasus::BlankPicParm(PIC_PARM* pPicParm)
{
   // Blank the whole thing.
   memset(pPicParm, 0, sizeof(PIC_PARM));

   // Set the standard fields.
   pPicParm->ParmSize = sizeof(PIC_PARM);
   pPicParm->ParmVer = CURRENT_PARMVER;
   pPicParm->ParmVerMinor = 1;
}

/*
// Query image info given a PIC_PARM structure (Get queue must exist and
// contain the start of the image data).
*/

long CPegasus::QueryImageInfo(PIC_PARM* pPicParm /*=NULL*/)
{
   long lType = 0;

   if (pPicParm == NULL)
   {
      pPicParm = &m_PicParm;
   }

   pPicParm->u.QRY.BitFlagsReq = QBIT_BIWIDTH | QBIT_BIHEIGHT | QBIT_BIBITCOUNT | QBIT_BICOMPRESSION;

   // if PegasusQuery is sucessful, the type is loaded into the bitmap header.
   PegasusQuery(pPicParm);
   if (pPicParm->u.QRY.BitFlagsAck & QBIT_BICOMPRESSION)
   {
      lType = pPicParm->Head.biCompression;
   }
   return lType;
}

// Fill the input queue.
long CPegasus::FillInputQueue(void)
{
   if (m_pInput == NULL)
   {
      return ERR_INVALID_OPERATION;
   }
   return m_pInput->FillQueue(&m_PicParm.Get);
}

// Empty the output queue.
long CPegasus::EmptyOutputQueue(void)
{
   if (m_pOutput == NULL)
   {
      return ERR_INVALID_OPERATION;
   }

   if (m_PicParm.Put.Front == m_PicParm.Put.Rear)
   {
      // Nothing in the queue. We succeed by default.
      return ERR_NONE;
   }

   // Something in the Put queue. Flush it.
   return m_pOutput->EmptyQueue(&m_PicParm.Put);
}

// Call the Pegasus routine.
RESPONSE CPegasus::CallPegasus(REQUEST lRequest, PIC_PARM* pPicParm /*=NULL*/)
{
   if (pPicParm == NULL)
   {
      pPicParm = &m_PicParm;
   }
   return ::Pegasus(pPicParm, lRequest);
}

// The main call to the Pegasus library.
long CPegasus::DoIt(REQUEST lRequest, RESPONSE lUntilResponse /*=RES_DONE*/)
{
   long lStatus = ERR_NONE;
   RESPONSE lResponse = CallPegasus(lRequest);
   while (lResponse != RES_DONE && lStatus == ERR_NONE)
   {
      // If we hit the "until" response, break out now.
      if (lResponse == lUntilResponse)
      {
         break;
      }
      switch (lResponse)
      {
         // get more data if needed
         case RES_GET_NEED_DATA:
         {
            lStatus = FillInputQueue();
            break;
         }

         // output buffer full
         case RES_PUT_NEED_SPACE:
         {
            lStatus = EmptyOutputQueue();
            break;
         }

         // return the error code if an error is detected
         case RES_ERR:
         {
            if (m_PicParm.Status != ERR_NONE)
            {
               lStatus = m_PicParm.Status;
            }
            else
            {
               lStatus = ERR_BAD_DATA;
            }
            break;
         }
         default:
         {
            break;
         }
      }
      if (lStatus == ERR_NONE)
      {
         lResponse = CallPegasus(REQ_CONT);
      }
   }
   return lStatus;
}

int CPegasus::ImageWidth(void) const
{
   return (int)m_PicParm.Head.biWidth;
}

int CPegasus::ImageHeight(void) const
{
   long lHeight = m_PicParm.Head.biHeight;
   if (lHeight < 0) lHeight = -lHeight;
   return (int)lHeight;
}

int CPegasus::ColorComponents(void) const
{
   return m_PicParm.Head.biBitCount == 24 ? 3 : 1;
}

/////////////////////////////////////////////////////////////////////////////
// JPEG Decompression

//
// Start a JPEG decompression.
//

long CPegasus::StartJPEGDecompression(void)
{
   long lStatus;

   if ((lStatus = AllocateGetQueue()) == ERR_NONE
       && (lStatus = FillInputQueue()) == ERR_NONE)
   {
      // Get the image info so we can determine what opcode to use.
      PIC_PARM QueryParm = m_PicParm;
      long lType;
      if ((lType = QueryImageInfo(&QueryParm)) == 0)
      {
         TRACE("Pegasus: Error getting image info\n");
         lStatus = ERR_BAD_IMAGE_TYPE;
      }
      else
      {
         lStatus = ERR_NONE;

//         TRACE("Pegasus: Got image info\n");

         // Set the operation we want to perform.
         switch (lType)
         {
            case BI_PICJ:
            case BI_JPEG:
            {
               m_PicParm.Op = OP_EXPJ;
					m_PicParm.ReservedPtr6 = (BYTE PICHUGE *)"Mindscape";
					m_PicParm.ReservedPtr7 = (BYTE PICHUGE *)0x5D9860E6;
               break;
            }
            case BI_PJPG:
            {
               m_PicParm.Op = OP_P2D;
					m_PicParm.ReservedPtr6 = (BYTE PICHUGE *)"Mindscape";
					m_PicParm.ReservedPtr7 = (BYTE PICHUGE *)0x68306EC1;
               m_PicParm.u.P2D.PicFlags |= PF_DecodeToDataEnd;
               break;
            }
            default:
            {
               lStatus = ERR_BAD_IMAGE_TYPE;
               break;
            }
         }

         // Invoke Pegasus to set up for decompression.
         if (lStatus == ERR_NONE)
         {
            RESPONSE lResponse = CallPegasus(REQ_INIT);
            while ((lStatus == ERR_NONE) && (lResponse != RES_DONE))
            {
               switch (lResponse)
               {
                  // get more data if needed
                  case RES_GET_NEED_DATA:
                  {
                     lStatus = FillInputQueue();
                     break;
                  }

                  // return the error code if an error is detected
                  case RES_ERR:
                  {
                     if (m_PicParm.Status != ERR_NONE)
                     {
                        lStatus = m_PicParm.Status;
                     }
                     else
                     {
                        lStatus = ERR_BAD_DATA;
                     }
                     break;
                  }

                  // image contains APP2 data, allocate buffer to hold it
                  case RES_ALLOCATE_APP2_BUF:
                  {
                     ASSERT(FALSE);
                     break;
                  }

                  // image contains comment, allocate buffer to hold it
                  case RES_ALLOCATE_COMMENT_BUF:
                  {
                     ASSERT(FALSE);
                     break;
                  }

                  // should not get any other messages
                  default:
                  {
                     ASSERT(FALSE);
                     break;
                  }
               }

               // continue unless error
               if (lStatus == ERR_NONE)
               {
                  lResponse = CallPegasus(REQ_CONT);
               }
            }
         }
      }

      if (lStatus == ERR_NONE)
      {
         lStatus = AllocatePutQueue(m_PicParm.u.S2D.StripSize);
      }
   }

   m_nNextLine = 0;
   m_lBytesRead = 0;

   return lStatus;
}

//
// Read some JPEG lines.
//

long CPegasus::ReadJPEGLines(int nLine, LPBYTE pLines, int nLineCount)
{
   BYTE PICHUGE* p = (BYTE PICHUGE*)pLines;
   long lStatus = ERR_NONE;

   // Compute the size of a row in bytes.
   int nRowSize = (ImageWidth() * ColorComponents() + 3) & ~3;

   // Compute where in the stream to read and how much to read.
   long lOffset = (long)nRowSize*(long)nLine;
   long lBytesToRead = (long)nRowSize*(long)nLineCount;

   // Figure out how far we have to skip.
   long lBytesToSeek = lOffset - m_lBytesRead;

   if (lBytesToSeek < 0)
   {
      // Tried to back up.
      return ERR_BAD_SEEK;
   }

   // Seek to the byte we need.
   while (lBytesToSeek > 0)
   {
      long lBytesInBuffer = m_PicParm.Put.Rear - m_PicParm.Put.Front;
      ASSERT(lBytesInBuffer >= 0);
      if (lBytesInBuffer == 0)
      {
         // Decompress until the put buffer fills up.
         if ((lStatus = DoIt(m_lBytesRead == 0 ? REQ_EXEC : REQ_CONT, RES_PUT_NEED_SPACE)) != ERR_NONE)
         {
            return lStatus;
         }
         if ((lBytesInBuffer = m_PicParm.Put.Rear - m_PicParm.Put.Front) <= 0)
         {
            return ERR_UNEXPECTED_EOF;          // Or pick a better ERR...
         }
      }
      // We have some data in the buffer.
      long lBytesToTake = (lBytesInBuffer < lBytesToSeek) ? lBytesInBuffer : lBytesToSeek;

      // Remove the data from the buffer.
      m_lBytesRead += lBytesToTake;
      lBytesToSeek -= lBytesToTake;
      m_PicParm.Put.Front += lBytesToTake;
   }

   // Read the bytes we want.
   while (lBytesToRead > 0)
   {
      long lBytesInBuffer = m_PicParm.Put.Rear - m_PicParm.Put.Front;
      ASSERT(lBytesInBuffer >= 0);
      if (lBytesInBuffer == 0)
      {
         // Decompress until the put buffer fills up.
         if ((lStatus = DoIt(m_lBytesRead == 0 ? REQ_EXEC : REQ_CONT, RES_PUT_NEED_SPACE)) != ERR_NONE)
         {
            return lStatus;
         }
         if ((lBytesInBuffer = m_PicParm.Put.Rear - m_PicParm.Put.Front) <= 0)
         {
            return ERR_UNEXPECTED_EOF;          // Or pick a better ERR...
         }
      }
      // We have some data in the buffer.
      long lBytesToTake = (lBytesInBuffer < lBytesToRead) ? lBytesInBuffer : lBytesToRead;

      // Remove the data from the buffer.
      m_lBytesRead += lBytesToTake;
      lBytesToRead -= lBytesToTake;

      // Copy the data.
      Util::HMemCpy(p, m_PicParm.Put.Front, lBytesToTake);
      p += lBytesToTake;
      m_PicParm.Put.Front += lBytesToTake;
   }

   return ERR_NONE;
}

//
// Finish.
//

long CPegasus::FinishJPEGDecompression(void)
{
   CallPegasus(REQ_TERM);
   return ERR_NONE;
}

//
// Abort.
//

long CPegasus::AbortJPEGDecompression(void)
{
   CallPegasus(REQ_TERM);
   return ERR_NONE;
}

/////////////////////////////////////////////////////////////////////////////
// JPEG Compression

//
// Start a JPEG compression
//

long CPegasus::StartJPEGCompression(int nWidth, int nHeight, int nColorComponents)
{
   long lStatus;

   if ((lStatus = AllocatePutQueue()) == ERR_NONE)
   {
      // Fill out the opcode and image header in m_PicParm.
      m_PicParm.Op = OP_D2J;

		m_PicParm.ReservedPtr6 = (BYTE PICHUGE *)"Mindscape";
		m_PicParm.ReservedPtr7 = (BYTE PICHUGE *)0x8F0332A5;
      // Fill out the compression parameters we want.

      m_PicParm.u.D2S.PicFlags = PF_OptimizeHuff;
      m_PicParm.u.D2S.LumFactor = 24;
      m_PicParm.u.D2S.ChromFactor = 30;
      m_PicParm.u.D2S.SubSampling = 2;
      m_PicParm.u.D2S.JpegType = JT_RAW;

      // Set the bitmap info header.
      InitBitmapInfoHeader(&m_PicParm.Head, nWidth, nHeight, nColorComponents*8);

      // Invoke Pegasus to set up for compression.
      RESPONSE lResponse = CallPegasus(REQ_INIT);
      while ((lStatus == ERR_NONE) && (lResponse != RES_DONE))
      {
         switch (lResponse)
         {
            // get more data if needed
            case RES_GET_NEED_DATA:
            {
               lStatus = FillInputQueue();
               break;
            }

            // return the error code if an error is detected
            case RES_ERR:
            {
               lStatus = m_PicParm.Status;
               break;
            }

            // should not get any other messages
            default:
            {
               ASSERT(FALSE);
               break;
            }
         }

         // continue unless error
         if (lStatus == ERR_NONE)
         {
            lResponse = CallPegasus(REQ_CONT);
         }
      }

      if (lStatus == ERR_NONE)
      {
         // Allocate exactly the StripSize.
         m_nDefaultGetQueueSize = 0;      // No default!
         lStatus = AllocateGetQueue(m_PicParm.u.D2S.StripSize);
      }
   }
   
   m_lNextRequest = REQ_EXEC;

   return lStatus;
}

//
// Write lines during JPEG compression.
//

long CPegasus::WriteJPEGLines(LPBYTE pLines, int nLineCount)
{
   long lStatus = ERR_NONE;
#ifdef WIN32
   LPBYTE p = pLines;
#else
   BYTE __huge* p = (BYTE __huge*)pLines;
#endif

   // Compute the size of a row in bytes.
   int nRowSize = Util::RoundToDWORD(ImageWidth() * ColorComponents());

   // Compute how many bytes to write.
   long lBytesToWrite = (long)nLineCount*(long)nRowSize;

   while (lBytesToWrite > 0)
   {
      long lFreeSpace;
      if ((lFreeSpace = FindFreeSpace(&m_PicParm.Get)) == 0)
      {
         // We need to flush the Get buffer.
         if ((lStatus = CompressGetBuffer(RES_GET_NEED_DATA)) != ERR_NONE)
         {
            return lStatus;
         }
         if ((lFreeSpace = FindFreeSpace(&m_PicParm.Get)) == 0)
         {
            return ERR_OUT_OF_SPACE;
         }
      }
      // We now have room in the Get buffer.
      // Compute how many bytes to put.
      long lBytesToPut = (lFreeSpace < lBytesToWrite) ? lFreeSpace : lBytesToWrite;
      Util::HMemCpy(m_PicParm.Get.Rear, p, lBytesToPut);

      // Update the variables.
      m_PicParm.Get.Rear += lBytesToPut;
      p += lBytesToPut;
      lBytesToWrite -= lBytesToPut;
   }
   return lStatus;
}

long CPegasus::WriteJPEGLinesReversed(LPBYTE pLines, int nLineCount)
{
   long lStatus = ERR_NONE;
#ifdef WIN32
   LPBYTE p = pLines;
#else
   BYTE __huge* p = (BYTE __huge*)pLines;
#endif

   // Compute the size of a row in bytes.
   int nRowSize = Util::RoundToDWORD(ImageWidth() * ColorComponents());

   p += Util::RoundToDWORD(nRowSize * (nLineCount - 1));
   // Compute how many bytes to write.
   long lBytesToWrite = (long)nLineCount*(long)nRowSize;

   while (lBytesToWrite > 0)
   {
      long lFreeSpace;
      if ((lFreeSpace = FindFreeSpace(&m_PicParm.Get)) == 0)
      {
         // We need to flush the Get buffer.
         if ((lStatus = CompressGetBuffer(RES_GET_NEED_DATA)) != ERR_NONE)
         {
            return lStatus;
         }
         if ((lFreeSpace = FindFreeSpace(&m_PicParm.Get)) == 0)
         {
            return ERR_OUT_OF_SPACE;
         }
      }
      // We now have room in the Get buffer.
      // Compute how many bytes to put.
      Util::HMemCpy(m_PicParm.Get.Rear, p, nRowSize);
      // Update the variables.
      m_PicParm.Get.Rear += nRowSize;
      p -= nRowSize;
      lBytesToWrite -= nRowSize;
   }
   return lStatus;
}
//
// Finish.
//

long CPegasus::FinishJPEGCompression(void)
{
   // Finish compressing the data in the Get queue.
   if (m_PicParm.Get.Front != m_PicParm.Get.Rear)
   {
      long lStatus;
      if ((lStatus = CompressGetBuffer(RES_DONE)) != ERR_NONE)
      {
         TRACE("Pegasus: Compression error on finish!\n");
         return lStatus;
      }
   }

   // Flush the data in the Put queue.
   EmptyOutputQueue();

   // Terminate the compression.
   CallPegasus(REQ_TERM);
   return ERR_NONE;
}

//
// Abort.
//

long CPegasus::AbortJPEGCompression(void)
{
   CallPegasus(REQ_TERM);
   return ERR_NONE;
}

//
// Little helper function to enclose compression call.
//

long CPegasus::CompressGetBuffer(RESPONSE lUntilResponse)
{
   long lThisRequest = m_lNextRequest;
   m_lNextRequest = REQ_CONT;

   return DoIt(lThisRequest, lUntilResponse);
}

//
// Helper function used by ZoomDib and ChangeColorDepth
//
long CPegasus::CallPegasusWithExternalBuffer()
{
	long lStatus = 0;

	// Invoke Pegasus to set up for compression.
	RESPONSE lResponse = CallPegasus(REQ_EXEC);
	lStatus = m_PicParm.Status;
	while ((lStatus == ERR_NONE) && (lResponse != RES_DONE))
	{
		switch (lResponse)
		{
		// get more data if needed
			case RES_GET_NEED_DATA:
			{
				lStatus = FillInputQueue();
				break;
			}

		// return the error code if an error is detected
			case RES_ERR:
			{
				lStatus = m_PicParm.Status;
				break;
			}

		// should not get any other messages
			default:
			{
				ASSERT(FALSE);
				break;
			}
		}

		// continue unless error
		if (lStatus == ERR_NONE)
		{
			lResponse = CallPegasus(REQ_CONT);
		}
	}

	if (lStatus == ERR_NONE)
	{
		CallPegasus(REQ_TERM);
	}

	return(lStatus);
}

// Function to scale a DIB to a fixed size

long CPegasus::ZoomDib(const RBitmapImage &InputDib, RBitmapImage &OutputDib, CSize new_size, BOOL bMakeGrayScale /*= FALSE*/)
{
	long								lStatus = 0;
	DWORD								dwColorTableSize;
	BITMAPINFO						*ptrSrcBMI;

	m_PicParm.ParmSize = sizeof(PIC_PARM);
	m_PicParm.ParmVer = CURRENT_PARMVER;   
	m_PicParm.ParmVerMinor = 1;
	m_PicParm.Op = OP_ZOOM;
	m_PicParm.u.ZOOM.PicFlags = 0;
	m_PicParm.ReservedPtr6= (BYTE PICHUGE*)"Mindscape";
	m_PicParm.ReservedPtr7 = (BYTE PICHUGE*)0x3D156CC1;

	//ptr to dib data
	ptrSrcBMI = (BITMAPINFO*)InputDib.GetRawData();
	//copy the bitmapinfoheader structure
	memcpy(&(m_PicParm.Head), &(ptrSrcBMI->bmiHeader), sizeof(BITMAPINFOHEADER));
	dwColorTableSize = InputDib.GetColorDataSize((LPVOID)ptrSrcBMI);

	//if the src dib is 8 bit copy the color table
	if(ptrSrcBMI->bmiHeader.biBitCount == 8)
	{
		memcpy(&m_PicParm.ColorTable, &ptrSrcBMI->bmiColors, dwColorTableSize);
		//for some reason we have to set the number of colors used to 256 
		if(m_PicParm.Head.biClrUsed == 0)
			m_PicParm.Head.biClrUsed = 256;

		if(bMakeGrayScale)
			m_PicParm.u.ZOOM.PicFlags = PF_ConvertGray;
	}
	DWORD dwbiImageSize = InputDib.GetImageDataSize((LPVOID)ptrSrcBMI);

	BYTE* pImageData = (BYTE*)InputDib.GetImageData((LPVOID)ptrSrcBMI);
	// Fill out the opcode and image header in m_PicParm.

	//ptr to beginning of src image data
	m_PicParm.Get.Start = pImageData;
	//ptr to first byte following image data
	m_PicParm.Get.End = pImageData + dwbiImageSize;
	m_PicParm.Get.Front = (BYTE*)m_PicParm.Get.Start;
	m_PicParm.Get.Rear = (BYTE*)m_PicParm.Get.End;
	m_PicParm.Get.QFlags = Q_EOF;

	//mode 0 means use NewWidth and NewHeight members to determing new size
	m_PicParm.u.ZOOM.Mode = 0;
	m_PicParm.u.ZOOM.NewWidth = new_size.cx;
	m_PicParm.u.ZOOM.NewHeight = new_size.cy;

	//new bit depth must be 8 if GrayScale flag is set otherwise it must be 24
	if(m_PicParm.Head.biBitCount == 1)
		m_PicParm.u.ZOOM.NewBitCount = 1;
	else if (bMakeGrayScale)
		m_PicParm.u.ZOOM.NewBitCount = 8;
	else
		m_PicParm.u.ZOOM.NewBitCount = 24;

	//initialize
	RESPONSE err_val = CallPegasus(REQ_INIT, &m_PicParm);


	//init output dib
	OutputDib.Initialize(new_size.cx, new_size.cy, m_PicParm.u.ZOOM.NewBitCount);
	BYTE* pDibData = (BYTE*)OutputDib.GetRawData();
	BYTE* pOutputImageData = (BYTE*)OutputDib.GetImageData(pDibData);

	//init buffers
	m_PicParm.Put.Start = pOutputImageData;
	m_PicParm.Put.End = pOutputImageData + OutputDib.GetImageDataSize(pDibData);
	m_PicParm.Put.Front = m_PicParm.Put.Start;
	m_PicParm.Put.Rear = m_PicParm.Put.Front;
	
	//scale the dib
	lStatus = CallPegasusWithExternalBuffer();
	//need to copy the bitmapinfoheader and the color table to the dst dib.
	//It is not filled in by Pegasus
	memcpy(pDibData, &m_PicParm.u.ZOOM.BiOut, m_PicParm.u.ZOOM.BiOut.biSize);
	if(m_PicParm.u.ZOOM.BiOut.biClrUsed != 0)
	{
		memcpy(pDibData + m_PicParm.u.ZOOM.BiOut.biSize,
			m_PicParm.ColorTable, m_PicParm.u.ZOOM.BiOut.biClrUsed * sizeof(RGBQUAD));
	}

	m_PicParm.Get.Start = NULL;
   m_PicParm.Put.Start = NULL; 


	return(lStatus);
}

// Function to scale a DIB by a percentage

long CPegasus::ZoomDib(const RBitmapImage &InputDib, RBitmapImage &OutputDib, long zoom_percent, BOOL bMakeGrayScale /*= FALSE*/)
{
	long								lStatus = 0;
	DWORD								dwColorTableSize;
	BYTE								*pZoomOut = NULL;
	BITMAPINFO						*ptrSrcBMI;

	m_PicParm.ParmSize = sizeof(PIC_PARM);
	m_PicParm.ParmVer = CURRENT_PARMVER;   
	m_PicParm.ParmVerMinor = 1;
	m_PicParm.Op = OP_ZOOM;
	m_PicParm.u.ZOOM.PicFlags = 0;
	m_PicParm.ReservedPtr6= (BYTE PICHUGE*)"Mindscape";
	m_PicParm.ReservedPtr7 = (BYTE PICHUGE*)0x3D156CC1;

	ptrSrcBMI = (BITMAPINFO*)InputDib.GetRawData();

	memcpy(&(m_PicParm.Head), &(ptrSrcBMI->bmiHeader), sizeof(BITMAPINFOHEADER));
	dwColorTableSize = InputDib.GetColorDataSize((LPVOID)ptrSrcBMI);

	//if the src dib is 8 bit copy the color table
	if(ptrSrcBMI->bmiHeader.biBitCount == 8)
	{
		memcpy(&m_PicParm.ColorTable, &ptrSrcBMI->bmiColors, dwColorTableSize);
		//for some reason we have to set the number of colors used to 256 
		if(m_PicParm.Head.biClrUsed == 0)
			m_PicParm.Head.biClrUsed = 256;

		if(bMakeGrayScale)
			m_PicParm.u.ZOOM.PicFlags = PF_ConvertGray;
	}
	DWORD dwbiImageSize = InputDib.GetImageDataSize((LPVOID)ptrSrcBMI);

	BYTE* pImageData = (BYTE*)InputDib.GetImageData((LPVOID)ptrSrcBMI);
	// Fill out the opcode and image header in m_PicParm.

	//ptr to beginning of src image data
	m_PicParm.Get.Start = pImageData;
	//ptr to first byte following image data
	m_PicParm.Get.End = pImageData + dwbiImageSize;
	m_PicParm.Get.Front = (BYTE*)m_PicParm.Get.Start;
	m_PicParm.Get.Rear = (BYTE*)m_PicParm.Get.End;
	m_PicParm.Get.QFlags = Q_EOF;

	m_PicParm.u.ZOOM.Mode = 1;
	if(zoom_percent >= 100)
	{
		m_PicParm.u.ZOOM.NewWidth = (long)( sqrt(zoom_percent / 100.0) * 100 + 0.5 );
	}
	else
	{
		m_PicParm.u.ZOOM.NewWidth = (long)( 100 / sqrt(100.0 / zoom_percent) + 0.5 );
	}

	m_PicParm.u.ZOOM.NewHeight = 100;
	if(m_PicParm.Head.biBitCount == 0 && m_PicParm.Head.biBitCount == 1)
		m_PicParm.u.ZOOM.NewBitCount = m_PicParm.Head.biBitCount;
	else if (bMakeGrayScale)
		m_PicParm.u.ZOOM.NewBitCount = 8;
	else
		m_PicParm.u.ZOOM.NewBitCount = 24;

	RESPONSE err_val = CallPegasus(REQ_INIT, &m_PicParm);


	//init output dib
	OutputDib.Initialize(m_PicParm.u.ZOOM.NewWidth, m_PicParm.u.ZOOM.NewHeight, m_PicParm.u.ZOOM.NewBitCount);
	BYTE* pDibData = (BYTE*)OutputDib.GetRawData();
	BYTE* pOutputImageData = (BYTE*)OutputDib.GetImageData(pDibData);

	//init buffers
	m_PicParm.Put.Start = pOutputImageData;
	m_PicParm.Put.End = pOutputImageData + OutputDib.GetImageDataSize(pDibData);
	m_PicParm.Put.Front = m_PicParm.Put.Start;
	m_PicParm.Put.Rear = m_PicParm.Put.Front;

	lStatus = CallPegasusWithExternalBuffer();

	memcpy(pDibData, &m_PicParm.u.ZOOM.BiOut, m_PicParm.u.ZOOM.BiOut.biSize);
	if(m_PicParm.u.ZOOM.BiOut.biClrUsed != 0)
	{
		memcpy(pDibData + m_PicParm.u.ZOOM.BiOut.biSize,
			m_PicParm.ColorTable, m_PicParm.u.ZOOM.BiOut.biClrUsed * sizeof(RGBQUAD));
	}

	m_PicParm.Get.Start = NULL;
   m_PicParm.Put.Start = NULL; 
   
   return lStatus;
}

//
// This function currently only scales 24-bit DIB's to 8-bit.
//

long CPegasus::ChangeColorDepth(const RBitmapImage &InputDib, RBitmapImage &OutputDib, BYTE *ColorMap)
{
	long						lStatus = 0;
	int						ColorTableSize;
	int						OutputDataSize;
	int						OutputHeaderSize;
	BYTE						*pDataOut = NULL;
	BYTE						*pDataIn = NULL;
	BITMAPINFO				*ptrBMI;
//	BITMAPINFOHEADER		BMInfoHeader;
	BITMAPINFOHEADER		*ptrBMInfoHeader;

   memset(&m_PicParm, 0, sizeof(PIC_PARM));

   m_PicParm.ParmSize = sizeof(PIC_PARM);
   m_PicParm.ParmVer = CURRENT_PARMVER;
	m_PicParm.Op = OP_UTL;
	m_PicParm.ReservedPtr6= (BYTE PICHUGE*)"Mindscape";
	m_PicParm.ReservedPtr7 = (BYTE PICHUGE*)0xF5739941;

	ptrBMInfoHeader = (BITMAPINFOHEADER*)InputDib.GetRawData();

	memcpy(&m_PicParm.Head, ptrBMInfoHeader, sizeof(BITMAPINFOHEADER));

	m_PicParm.u.UTL.Subcode = 8;
	m_PicParm.u.UTL.OutBpp = 8;
	m_PicParm.u.UTL.PicFlags = PF_Dither;
	m_PicParm.u.UTL.ptr3 = ColorMap;
	m_PicParm.u.UTL.NumColors = 256;
	m_PicParm.u.UTL.AllocType = 1;


	ptrBMI = (BITMAPINFO *)ptrBMInfoHeader;
	pDataIn = (BYTE*)InputDib.GetImageData((LPVOID)ptrBMInfoHeader);

	ColorTableSize = InputDib.GetColorDataSize((LPVOID)ptrBMInfoHeader);

	int biImageSize = InputDib.GetImageDataSize((LPVOID)ptrBMInfoHeader);

	// Fill out the opcode and image header in m_PicParm.
	m_PicParm.Get.Start = (BYTE*)pDataIn;
	m_PicParm.Get.End = (BYTE*)pDataIn + biImageSize + 1;
	m_PicParm.Get.Front = (BYTE*)m_PicParm.Get.Start;
	m_PicParm.Get.Rear = (BYTE*)pDataIn + biImageSize;
	m_PicParm.Get.QFlags = Q_EOF;

	OutputDataSize = (ptrBMInfoHeader->biHeight) * ((ptrBMInfoHeader->biWidth * 8 + 31) & ~31) / 8;
	OutputHeaderSize = sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD));
	OutputDib.Initialize(ptrBMInfoHeader->biWidth, ptrBMInfoHeader->biHeight, 8);
	pDataOut = (BYTE*)OutputDib.GetRawData();
	BYTE* pOutImgData = (BYTE*)OutputDib.GetImageData(pDataOut);
	DWORD pOutImgDataSize = OutputDib.GetImageDataSize(pDataOut);

	m_PicParm.Put.Start = pOutImgData;
	m_PicParm.Put.End = pOutImgData + OutputDataSize + 1;
	m_PicParm.Put.Front = m_PicParm.Put.Start;
	m_PicParm.Put.Rear = m_PicParm.Put.Front;

	RESPONSE err_val = CallPegasus(REQ_INIT, &m_PicParm);

	lStatus = CallPegasusWithExternalBuffer();

	memcpy(pDataOut + sizeof(BITMAPINFOHEADER),
			m_PicParm.ColorTable, 256 * sizeof(RGBQUAD));

	m_PicParm.Get.Start = NULL;
   m_PicParm.Put.Start = NULL; 


	return (lStatus);
}


