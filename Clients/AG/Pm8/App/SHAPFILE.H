/*
// $Workfile: SHAPFILE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SHAPFILE.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:52   Fred
// Initial revision.
// 
//    Rev 1.2   14 May 1997 10:16:50   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.1   28 Jan 1997 14:10:42   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.0   14 Mar 1996 13:30:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:16   FRED
// Initial revision.
// 
//    Rev 1.6   13 Sep 1995 11:47:00   JAY
// New 32-bit changes
// 
//    Rev 1.6   13 Sep 1995 11:46:26   JAY
// 
//    Rev 1.5   30 Aug 1995 14:13:58   JAY
// New FindShape method.
// 
//    Rev 1.4   06 Jul 1995 17:26:42   JAY
// New binary caching technology
// 
//    Rev 1.3   23 Jun 1995 09:07:36   FRED
// Fixed "no shape selection" crash.
// 
// 
//    Rev 1.2   18 May 1995 13:20:56   FRED
// Shape file/dialog stuff
// 
//    Rev 1.1   17 May 1995 15:42:28   JAY
//  
// 
//    Rev 1.0   16 May 1995 12:00:24   JAY
// Initial revision.
*/

class CShapeFile;
class CShapeFileParser;

#ifndef __SHAPFILE_H__
#define __SHAPFILE_H__

#include "parse.h"
#include "outpath.h"

class StorageFile;

class CShapeEntry
{
public:
	enum
	{
		RectangleObject,
		EllipseObject,
		LineObject,
		DrawingObject
	};
	CString m_csName;
	int m_nType;
	COutlinePath m_Path;
	CFixedRect m_Bounds;
};

class CShapeFile
{
public:
	CShapeFile(void);
	~CShapeFile(void);
public:
	BOOL Init(LPCSTR lpszFileName, LPCSTR lpszBinaryFileName);
	int ShapeCount(void)
		{ return m_Shapes.GetSize(); }
	CShapeEntry* Shape(int nIndex)
		{ return (nIndex >= 0) && (nIndex < ShapeCount()) ? (CShapeEntry*)m_Shapes[nIndex] : NULL; }
	CShapeEntry* FindShape(LPCSTR pName);
protected:
	CPtrArray m_Shapes;
protected:
	void FreeShapes(void);

   enum
   {
      Version = 1       // <= 3.0 was version 0; 4.0 is now version 1
   };

	ERRORCODE ReadBinaryFile(LPCSTR lpszFileName);
	ERRORCODE WriteBinaryFile(LPCSTR lpszFileName);
	struct CShapeFileHeader
	{
		short int nVersion;
		short int nShapes;
	};
	struct CShapeEntryHeader
	{
		short int m_nType;
		CFixedRect m_Bounds;
	};
};

class CShapeFileParser : public CParser
{
private:
	virtual int NextChar(void);
	virtual void UngetChar(int nChar);
public:
	CShapeFileParser(StorageFile* pFile, CPtrArray* pArray);
	virtual ~CShapeFileParser();

	StorageFile* m_pFile;
	CPtrArray* m_pArray;

	BOOL m_fEmpty;

/* State information. */

	CFixedPoint m_cp;
	CShapeEntry* m_pEntry;

	static PARSE_STATE ParseStates[];
	static TOKEN_LOOKUP TokenLookup[];

	char m_cbString[MAX_TOKEN_SIZE];

	enum
	{
		TOKEN_BeginString,
		TOKEN_BeginPath,
		TOKEN_EndPath,
		TOKEN_MoveTo,
		TOKEN_LineTo,
		TOKEN_SplineTo,
		TOKEN_CurveTo,
		TOKEN_CurveTo01,
		TOKEN_CurveTo23,
		TOKEN_Close
	};

/* Our exceptions. */

	enum
	{
		string_too_long = CParseException::USER_DEFINED,
		stack_underflow
	};

/* State handler helpers. */

	void HandleString(void);
	CFixed PopNumber(void);
	void PopPoint(CFixedPoint& p);
	void PushNumber(CFixed l);

/* State handlers. */  

	void HandleName(void);
	void HandlePathOp(void);
	void HandleNumber(void);
	void HandleEndPath(void);

	CDWordArray m_dwaStack;			// We push CFixed number here.
};

#endif
