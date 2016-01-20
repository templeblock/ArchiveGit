/*
// $Header: /PM8/App/DBMAP.H 1     3/03/99 6:04p Gbeddow $
//
// Definitions for standard database mapping.
//
// $Log: /PM8/App/DBMAP.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     11/21/97 11:01a Jayn
// Moved StorageFileOnCFile from dbmap.* to file.*
// 
//    Rev 1.0   14 Aug 1997 15:20:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:30   Fred
// Initial revision.
// 
//    Rev 1.1   18 Jul 1996 17:59:22   Fred
// Moved class from file.cpp
// 
//    Rev 1.0   14 Mar 1996 13:26:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:50   FRED
// Initial revision.
// 
//    Rev 2.7   07 Jul 1995 16:13:14   JAY
// Include rearranging; document saving with album.
// 
//    Rev 2.6   05 Jul 1995 11:16:58   JAY
// Rearrange includes to reduce symbol count.
// 
//    Rev 2.5   07 Jun 1995 15:39:52   JAY
//  
// 
//    Rev 2.4   01 May 1995 15:28:04   JAY
// Restructured the macro stuff
// 
//    Rev 2.3   25 Apr 1995 09:00:32   JAY
// DatabaseMapping now can create mandatory records.
// 
//    Rev 2.2   17 Feb 1995 16:47:52   JAY
// Text editing undo and redo.
// 
//    Rev 2.1   08 Feb 1995 13:39:24   JAY
// Reverted. New series.
// 
//    Rev 1.2   30 Nov 1994 17:02:22   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 17:15:42   JAY
// Cut and paste!
// 
//    Rev 1.0   05 Jan 1994 08:12:04   JAY
// Restructuring, cut/paste, images, etc.
*/

#ifndef __DBMAP_H__
#define __DBMAP_H__

class CStdMacroServer;
class FontServer;
class StorageDevice;

class DatabaseMapping
{
public:
	CString				m_csName;
	StorageDevice*		device;
	FontServer*			font_server;
	PMGDatabase*		database;
	CStdMacroServer*	m_pMacroServer;

	// Additional objects for accessing a storage-based document.
	LPSTORAGE			m_pStorage;			// In case we need it.
	COleStreamFile		m_Stream;			// OLE Stream we are based on.

	DatabaseMapping();
	virtual ~DatabaseMapping();
	virtual ERRORCODE map_to_file(LPCSTR name, BOOL read_only);
	virtual ERRORCODE map_to_document(LPCSTR name, BOOL read_only);

	ERRORCODE initialize(char *preamble,
							SHORT signature,
							SHORT file_type,
							SHORT file_flags);

	ERRORCODE open(SHORT *signature, SHORT *file_type, SHORT *file_flags, BOOL fConvertIfOld = FALSE);

	ERRORCODE flush_database(VOID);
	ERRORCODE CreateMandatoryRecords(DOC_CREATE_STRUCT* pDCS = NULL);
	void FreeAll(void);
};

/*
// Parse the beginning of the file and return the signature.
*/

extern ERRORCODE parse_document_preamble(StorageDevice* device,
								SHORT *signature,
								SHORT *file_type,
								SHORT *file_flags);

#endif				// #ifndef __DBMAP_H__
