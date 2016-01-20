/*
// $Workfile: FFLDFILE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FFLDFILE.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:48   Fred
// Initial revision.
// 
//    Rev 1.1   14 May 1997 10:16:48   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.0   14 Mar 1996 13:37:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:14   FRED
// Initial revision.
// 
//    Rev 1.0   07 Aug 1995 07:48:40   FRED
// Standard fill-in fields.
*/

class CFillinFieldFile;
class CFillinFieldFileParser;

#ifndef __FFLDFILE_H__
#define __FFLDFILE_H__

class StorageFile;

#include "parse.h"

class CFillinFieldFile
{
public:
	CFillinFieldFile(void);
	~CFillinFieldFile(void);
public:
	BOOL Init(LPCSTR lpszFileName, CStringArray* pArray);
protected:
	CStringArray* m_pFields;
};

class CFillinFieldFileParser : public CParser
{
private:
	virtual int NextChar(void);
	virtual void UngetChar(int nChar);
public:
	CFillinFieldFileParser(StorageFile* pFillinFieldFile, CStringArray* pArray);
	virtual ~CFillinFieldFileParser();

	StorageFile* m_pFile;
	CStringArray* m_pArray;

/* State information. */

	CString m_csName;
	
	static PARSE_STATE ParseStates[];
	static TOKEN_LOOKUP TokenLookup[];

	char m_cbString[MAX_TOKEN_SIZE];

	enum
	{
		TOKEN_BeginString,
		TOKEN_EndEntry
	};

/* Our exceptions. */

	enum
	{
		string_too_long = CParseException::USER_DEFINED
	};

/* State handler helpers. */

	void HandleString(void);

/* State handlers. */  

	void HandleName(void);
	void HandleEndEntry(void);
};

#endif
