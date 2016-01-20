// filcmprs.h : interface of the CFileCompress class & dependants
//
/////////////////////////////////////////////////////////////////////////////
#ifndef FILECMPRS_H
#define FILECMPRS_H
//#include "..\cdeflate.h"

#include <afxtempl.h>

// Files compression state info
enum OperateType 
{
	COMPRESS, DECOMPRESS, WRITE_HEADER
};
enum ProcessType 
{
	CLEAN, PROCESSING, PROCESSED, PROCESS_ERROR
};
struct SFileState
{
	CString csFilename;
	enum OperateType operation;
	enum ProcessType state;
};

class CFileCompress
{
	// Keep an array of files to process
	int m_nFiles;
	CString m_csPath;
	CArray<SFileState, SFileState&> m_aFileList;

private:
	BOOL m_fCompressionHeader;

public:
	CFileCompress();
	void InitFileList(enum OperateType operation, CString& csPath, CStringArray& aFilenames, int nFiles);

	const BOOL GetCompressionHeader ( void )
	{	return m_fCompressionHeader;	}
	void SetCompressionHeader (const BOOL fHeader )
	{	m_fCompressionHeader = fHeader;	}

// Operations
public:
	BOOL StartOperation(int nIndex);
	void PerformOperation(int nIndex);
	void GetProcessString(int nIndex, CString& csProcessString);
	
	int Compress(CString& csFilename);
	int Compress(CString& csFilename, LPBYTE* ppDestination, DWORD* pdwDestination);
	int Compress(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType, LPBYTE* ppDestination, DWORD* pdwDestination);
	int Decompress(CString& csFilename);
	int Decompress(CString& csFilename, LPBYTE* ppDestination, DWORD* pdwDestination);
	int Decompress(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType, LPBYTE* ppDestination, DWORD* pdwDestination);
	int WriteHeader(CString& csFilename);
	int WriteHeader(CString& csFilename, LPBYTE* ppDestination, DWORD* pdwDestination);
	void FreeData(LPBYTE pData);

protected:
	void BuildDestinationFilename(CString& csSrc, CString& csDest);
	LPVOID LoadCompressionResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType, DWORD& dwSize);
	int SourceCompress(CStdioFile* pSrcFile, CStdioFile* pDstFile, LPBYTE* ppDestination, DWORD* pdwDestination, BOOL bUserBuf);
	int SourceDecompress(CStdioFile* pSrcFile, CStdioFile* pDstFile, LPBYTE* ppDestination, DWORD* pdwDestination, BOOL bUserBuf);
	int SourceWriteHeader(CStdioFile* pSrcFile, CStdioFile* pDstFile, LPBYTE* ppDestination, DWORD* pdwDestination, BOOL bUserBuf);


};

// CBitmapFileCompress
// Class for compression/decompression of bitmap resources
class CBitmapFileCompress : public CFileCompress
{
public:
	CBitmapFileCompress() : CFileCompress() {}

// Operations
public:
	// So far, this is the only one we need
	int Decompress(HMODULE hModule, LPCTSTR lpName, LPBYTE* ppDestination, DWORD* pdwDestination);
};

#pragma pack(1)
class CFileCompressInfo
  {
  public:
    CFileCompressInfo()                  { Empty(); }
    void       Set(long lCompressedSize, long lUnCompressedSize, 
      unsigned short shCompressionType)
      {
        ASSERT( lCompressedSize <= lUnCompressedSize );
        m_lUnCompressedSize = lUnCompressedSize;
        m_lCompressedSize   = lCompressedSize;
        m_shCompressionType = shCompressionType;
      }
    long       GetCompressedSize()     { return m_lCompressedSize; }
    long       GetUnCompressedSize()   { return m_lUnCompressedSize; }
    unsigned short GetType()           { return m_shCompressionType; }
    void Empty()
    {
       m_lUnCompressedSize = m_lCompressedSize = 0;
       m_shCompressionType = 0;
    }
  protected:
    long            m_lUnCompressedSize;
    long            m_lCompressedSize;
    unsigned short  m_shCompressionType;
  };
#pragma pack()

#endif //FILECMPRS_H
