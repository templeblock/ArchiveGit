/*
// $Workfile: WARPFILE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPFILE.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:28   Fred
// Initial revision.
// 
//    Rev 1.1   14 May 1997 10:16:50   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.0   14 Mar 1996 13:31:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:54   FRED
// Initial revision.
// 
//    Rev 1.1   28 Apr 1995 17:36:50   FRED
//  
// 
//    Rev 1.0   28 Apr 1995 13:51:26   FRED
// Initial revision.
*/

class CWarpFile;
class CWarpFileParser;

#ifndef __WARPFILE_H__
#define __WARPFILE_H__

#include "parse.h"
#include "warpfld.h"

class StorageFile;

class CWarpFile
{
public:
	CWarpFile(void);
	~CWarpFile(void);
public:
	BOOL Init(LPCSTR lpszFileName);
	int WarpFieldCount(void)
		{ return m_WarpFields.GetSize(); }
	CWarpField* WarpField(int nIndex)
		{ return (CWarpField*)m_WarpFields[nIndex]; }
protected:
	CPtrArray m_WarpFields;
protected:
	void FreeWarpFields(void);
};

class CWarpFileParser : public CParser
{
private:
	virtual int NextChar(void);
	virtual void UngetChar(int nChar);
public:
	CWarpFileParser(StorageFile* pWarpFile, CPtrArray* pArray);
	virtual ~CWarpFileParser();

	StorageFile* m_pFile;
	CPtrArray* m_pArray;

/* State information. */

	CString m_csName;
	CWarpFieldBounds::BoundsType m_nType;
	
	long m_LastX;
	long m_LastY;
	
	struct
	{
		CWarpFieldEdge::EdgeType m_nType;
		long m_lCoordinate[8];
	} m_Edge[4];
	
	int m_nEdgesParsed;
	int m_nCoordinatesNeeded;
	int m_nCoordinatesParsed;

	static PARSE_STATE ParseStates[];
	static TOKEN_LOOKUP TokenLookup[];

	char m_cbString[MAX_TOKEN_SIZE];

	enum
	{
		TOKEN_BeginString,
		TOKEN_X,
		TOKEN_Y,
		TOKEN_XY,
		TOKEN_BeginEdge,
		TOKEN_Line,
		TOKEN_Spline,
		TOKEN_Bezier,
		TOKEN_EndEdge,
		TOKEN_EndEntry
	};

/* Our exceptions. */

	enum
	{
		string_too_long = CParseException::USER_DEFINED,
		too_many_coordinates,
		too_few_coordinates,
		too_many_edges,
		too_few_edges
	};

/* State handler helpers. */

	void HandleString(void);

/* State handlers. */  

	void HandleName(void);
	void HandleType(void);
	void HandleBeginEdge(void);
	void HandleEdgeType(void);
	void HandleEdgeNumber(void);
	void HandleEndEdge(void);
	void HandleEndEntry(void);
};

#endif
