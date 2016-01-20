/*
// $Workfile: CWMF.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CWMF.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:46   FRED
// Initial revision.
// 
//    Rev 2.2   15 Sep 1995 14:04:52   JAY
// More 32-bit changes.
// 
//    Rev 2.1   08 Feb 1995 13:39:24   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:56:24   FRED
//  
// 
//    Rev 1.2   16 Sep 1994 10:45:44   FRED
// Got rid of Enum() method. Play() uses enumerate
// by default.
// 
// Added support for mask generation and color mapping.
// 
// 
//    Rev 1.1   15 Sep 1994 14:51:44   FRED
// Enhanced CWindowsMetaFile object.
// 
// 
//    Rev 1.0   15 Sep 1994 11:46:06   FRED
// Initial revision.
*/

class CWindowsMetaFile;

#ifndef __CWMF_H__
#define __CWMF_H__

/*
// WMF file support.
*/

#define  ALDUS_WMF_KEY        0x9AC6CDD7

/* placeable metafile header */

typedef struct {
	DWORD   key;
#ifdef WIN32
	WORD hmf;
	RECTS bbox;
#else
	HMETAFILE hmf;
	RECT    bbox;
#endif
	WORD    inch;
	DWORD   reserved;
	WORD    checksum;
} ALDUS_WMF_HEADER;

/*
// This class encapsulates the functionality needed for loading
// meta file information from a resource or file. Only meta files
// with the Aldus placeable meta file header are supported.
*/

class CWindowsMetaFile
{
public:

	// This structure is used to pass information to the virtual enumerate function.	
	
	typedef struct
	{
		CWindowsMetaFile* pObject;
		CDC* pDC;
		BOOL fMask;
		LPARAM lParam;
	} EnumData;

public:

	CWindowsMetaFile();
	virtual ~CWindowsMetaFile();
	
	virtual BOOL Init(LPCSTR pszFileName);
	virtual BOOL Init(HINSTANCE hInstance, LPCSTR pszResource, LPCSTR pszType);
	
	virtual const ALDUS_WMF_HEADER* AldusHeader(void);
	virtual const METAHEADER* Header(void);
	
	virtual BOOL Play(CDC* pDC, BOOL fMask = FALSE, LPARAM lParam = 0);
	
	virtual COLORREF MapColor(COLORREF clColor);
   
   // These static functions can be used by simply calling CWindowsMetaFile::GetMetaFile()
   // to access metafile information without creating a CWindowsMetaFile object.
   
	static BOOL GetMetaFile(LPCSTR pszFileName, ALDUS_WMF_HEADER* pAldusHeader, METAHEADER* pHeader, HMETAFILE* phMetaFile);
	static BOOL GetMetaFile(HINSTANCE hInstance, LPCSTR pszResource, LPCSTR pszType, ALDUS_WMF_HEADER* pAldusHeader, METAHEADER* pHeader, HMETAFILE* phMetaFile);
	
protected:

	virtual void Reset(void);
	
	static BOOL CALLBACK EnumMetaFileDispatch(HDC hDC, HANDLETABLE FAR* pObject, METARECORD FAR* pMetaRecord, int nObject, LPARAM lParam);
	virtual BOOL EnumMetaFileProc(HANDLETABLE FAR* pObject, METARECORD FAR* pMetaRecord, int nObject, EnumData* pData);
	
	virtual BOOL GetHeader(void);
	virtual BOOL GetMetaFile(void);

	// member variables
		
	BOOL m_fHeaderValid;
	BOOL m_fMetaFileValid;
	
	LPCSTR m_pszFileName;
	
	HINSTANCE m_hInstance;
	LPCSTR m_pszResource;
	LPCSTR m_pszType;
	
	ALDUS_WMF_HEADER m_AldusHeader;
	METAHEADER m_Header;
	HMETAFILE m_hMetaFile;
};

#endif			// __CWMF_H__
