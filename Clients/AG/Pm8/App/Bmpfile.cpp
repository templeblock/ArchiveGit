#include "stdafx.h"

#include "bmpfile.h"

CBitmapFile::CBitmapFile()
   {
      m_pRGBQuad = NULL;
      m_nNumPaletteEntries = 0;
      m_pData = NULL;
      m_dwDataSize = 0;
   }

CBitmapFile::~CBitmapFile()
   {
      Close();

      delete [] m_pRGBQuad;
      delete [] m_pData;
   }

CBitmapFile::ErrorCode CBitmapFile::Open(LPCSTR szFileName)
   {
      m_csFileName = szFileName;
      if(!IsOpen())
         {
            if(!m_cfBitmap.Open(szFileName, CFile::modeRead | CFile::shareDenyWrite))
               return ecOpenFailed;
         }

      return ecNone;
   }

void CBitmapFile::Close()
   {
      if(!IsOpen())
         return;

      m_cfBitmap.Close();
   }

BOOL CBitmapFile::IsOpen() const
   {
      return (m_cfBitmap.m_hFile != CFile::hFileNull);
   }

CBitmapFile::ErrorCode CBitmapFile::ReadHeaders(LPCSTR szFileName)
   {
      UINT     uiBytesRead, uiBytesToRead;
      CBitmapFile::ErrorCode  errorcode = ecNone;

      if(szFileName != NULL)
         errorcode = Open(szFileName);

      if(!IsOpen() || errorcode != ecNone)
         return ecOpenFailed;

      uiBytesRead = m_cfBitmap.Read(&m_BitmapFileHeader, sizeof(m_BitmapFileHeader));
      if(uiBytesRead != sizeof(m_BitmapFileHeader))
         return ecReadFailed;

      // Make sure it's a bitmap file
      if(m_BitmapFileHeader.bfType != 0x4D42)      // "BM"
         return ecInvalidFileType;

      uiBytesRead = m_cfBitmap.Read(&m_BitmapInfoHeader, sizeof(m_BitmapInfoHeader));
      if(uiBytesRead != sizeof(m_BitmapInfoHeader))
         return ecReadFailed;

      // If old bitmap file format such as BITMAPCOREHEADER, fail
      if(m_BitmapInfoHeader.biSize != sizeof(m_BitmapInfoHeader))
         return ecReadFailed;

      if(m_BitmapInfoHeader.biClrUsed)
         m_nNumPaletteEntries = m_BitmapInfoHeader.biClrUsed;
      else if(m_BitmapInfoHeader.biBitCount == 8)
         m_nNumPaletteEntries = 256;
      else
         m_nNumPaletteEntries = 0;

      if(m_nNumPaletteEntries <= 256)
         {
            delete [] m_pRGBQuad;
            m_pRGBQuad = new RGBQUAD [m_nNumPaletteEntries];
            uiBytesToRead = m_nNumPaletteEntries * sizeof(RGBQUAD);
            uiBytesRead = m_cfBitmap.Read(m_pRGBQuad, uiBytesToRead);
            if(uiBytesRead != uiBytesToRead)
               return ecReadFailed;
         }
      else
         return ecInvalidFileType;

      return ecNone;
   }

CBitmapFile::ErrorCode CBitmapFile::Read(LPCSTR szFileName)
   {
      DWORD    dwBytesRead, dwBytesToRead;
      CBitmapFile::ErrorCode  errorcode = ecNone;

      if(szFileName != NULL)
         errorcode = Open(szFileName);

      if(!IsOpen() || errorcode != ecNone)
         return ecOpenFailed;

      if((errorcode = ReadHeaders()) != ecNone)
         return errorcode;

      delete [] m_pData;
      dwBytesToRead = m_cfBitmap.GetLength() - sizeof(m_BitmapFileHeader) -
         sizeof(m_BitmapInfoHeader) - (m_nNumPaletteEntries * sizeof(RGBQUAD));
      m_pData = new char[dwBytesToRead];
      dwBytesRead = m_cfBitmap.ReadHuge(m_pData, dwBytesToRead);
      if(dwBytesRead != dwBytesToRead)
         return ecReadFailed;

      m_dwDataSize = dwBytesRead;

      return ecNone;
   }

// Static Members
CBitmapFile::ErrorCode CBitmapFile::ComparePalettes(const CBitmapFile &bmpFile1, const CBitmapFile &bmpFile2)
   {
      CString  csError;
      BOOL     bPalettesMatch = TRUE;

      if(bmpFile1.GetPaletteEntryCount() != bmpFile2.GetPaletteEntryCount())
         {
            return ecPaletteEntryCountMismatch;
         }
      const RGBQUAD *pRGBQuadFile1 = bmpFile1.GetPaletteEntries();
      const RGBQUAD *pRGBQuadFile2 = bmpFile2.GetPaletteEntries();

      for(int i=0;i < bmpFile1.GetPaletteEntryCount();i++)
         {
            if(pRGBQuadFile1[i].rgbRed != pRGBQuadFile2[i].rgbRed || 
               pRGBQuadFile1[i].rgbGreen != pRGBQuadFile2[i].rgbGreen || 
               pRGBQuadFile1[i].rgbBlue != pRGBQuadFile2[i].rgbBlue)
               {
                  bPalettesMatch = FALSE;
               }
         }
      if(!bPalettesMatch)
         return ecPaletteEntryMismatch;

      return ecNone;
   }


