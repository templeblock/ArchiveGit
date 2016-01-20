#ifndef _BMPFILE_INC_
#define _BMPFILE_INC_

class CBitmapFile
   {
   public:
      CBitmapFile();
      ~CBitmapFile();

      enum ErrorCode
         {
            ecNone,
            ecOpenFailed,
            ecReadFailed, 
            ecPaletteEntryMismatch, 
            ecPaletteEntryCountMismatch,
            ecInvalidFileType
         };
      ErrorCode         Open(LPCSTR szFileName);
      void              Close();
      BOOL              IsOpen() const;
      ErrorCode         Read(LPCSTR szFileName = NULL);
      ErrorCode         ReadHeaders(LPCSTR szFileName = NULL);
      const CString &   GetFileName() const;
      void *            GetData() const;
      DWORD             GetDataSize() const;
      const BITMAPINFOHEADER * GetInfoHeader() const;
      const BITMAPINFO  * GetInfo() const;


   // Static Methods
      static CBitmapFile::ErrorCode ComparePalettes(const CBitmapFile &bmpFile1, const CBitmapFile &bmpFile2);
   protected:
   // Helpers
//      int   ReadFileHeader();
//      int   ReadInfoHeader();
//      int   ReadPalette();

      int   GetPaletteEntryCount() const;
      const RGBQUAD * GetPaletteEntries() const;

   protected:
      BITMAPFILEHEADER  m_BitmapFileHeader;
      BITMAPINFOHEADER  m_BitmapInfoHeader;
      RGBQUAD           *m_pRGBQuad;
      int               m_nNumPaletteEntries;
      CString           m_csFileName;
      void *            m_pData;
      DWORD             m_dwDataSize;
      CFile             m_cfBitmap;
   };

// Inline Section
inline const CString & CBitmapFile::GetFileName() const
   {
      return m_csFileName;
   }

inline const RGBQUAD * CBitmapFile::GetPaletteEntries() const
   {
      return m_pRGBQuad;
   }

inline int   CBitmapFile::GetPaletteEntryCount() const
   {
      return m_nNumPaletteEntries;
   }

inline void * CBitmapFile::GetData() const
   {
      return m_pData;
   }

inline DWORD CBitmapFile::GetDataSize() const
   {
      return m_dwDataSize;
   }

inline const BITMAPINFOHEADER * CBitmapFile::GetInfoHeader() const
   {
      return &m_BitmapInfoHeader;
   }

inline const BITMAPINFO * CBitmapFile::GetInfo() const
   {
      return (BITMAPINFO *)&m_BitmapInfoHeader;
   }
#endif

