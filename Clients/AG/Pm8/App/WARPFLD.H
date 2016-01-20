/*
// $Header: /PM8/App/WARPFLD.H 1     3/03/99 6:13p Gbeddow $
//
// $Workfile: WARPFLD.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPFLD.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     2/26/99 5:43p Gbeddow
// added some asserts in an effort to track down bug #9 but they weren't
// triggered, leaving the bug unresolved - it seems that some of the
// readymade projects have bad data
// 
//    Rev 1.0   14 Aug 1997 15:27:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:30   Fred
// Initial revision.
// 
//    Rev 1.4   17 Apr 1997 15:35:24   Fred
// Rotated fill patterns
// 
//    Rev 1.3   12 Mar 1997 14:53:58   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.2   07 Feb 1997 09:02:52   Fred
// Made WarpPoint public. Added CWarpField constructor.
// 
//    Rev 1.1   16 Jan 1997 09:03:04   Fred
// Switch to warp field data record
// 
//    Rev 1.0   14 Mar 1996 13:48:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:16   FRED
// Initial revision.
// 
//    Rev 1.7   21 Sep 1995 13:46:08   JAY
// Fixed 32/16 difference in warp field record.
// 
//    Rev 1.6   11 May 1995 09:37:00   JAY
// Changed CreateStruct to external class.
// 
//    Rev 1.5   09 May 1995 13:12:06   FRED
// Non linear warp fields.
// 
// 
//    Rev 1.4   28 Apr 1995 17:38:20   FRED
//  
// 
//    Rev 1.3   28 Apr 1995 13:49:14   FRED
//  
// 
//    Rev 1.2   27 Apr 1995 13:20:18   FRED
// 
//    Rev 1.1   25 Apr 1995 16:02:26   FRED
// 
//    Rev 1.0   25 Apr 1995 10:33:40   FRED
// Initial revision.
*/

class CWarpFieldEdge;
class CWarpFieldBounds;
class CWarpField;
class WarpFieldRecord;

#ifndef __WARPFLD_H__
#define __WARPFLD_H__

#include "outpath.h"
#include "cfixed.h"

/////////////////////////////////////////////////////////////////////////////
// CWarpFieldEdge

// This data matches what is written to the document as part of the
// CWarpFieldRecord (PMG 2 and 3). It must match the structure used
// in the CWarpFieldEdge object.
struct CWarpFieldEdgeData
{
	SHORT m_nType;					// EdgeType
	union
	{
		struct { long AX, AY, BX, BY; }						m_Line;
		struct { long AX, AY, BX, BY, CX, CY; }			m_Spline;
		struct { long AX, AY, BX, BY, CX, CY, DX, DY; }	m_Bezier;
	};
};

class CWarpFieldEdge
{
public:
	enum EdgeType
	{
		Line,
		Spline,
		Bezier
	};
public:
	CWarpFieldEdge(void);
	~CWarpFieldEdge(void);
	void FreeMaps(void);
	void Init(const CWarpFieldEdge* pEdge)
		{ *this = *pEdge; }
	void InitLine(long AX, long AY, long BX, long BY);
	void InitSpline(long AX, long AY, long BX, long BY, long CX, long CY);
	void InitBezier(long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY);
	void FromData(const CWarpFieldEdgeData& Data);
	void ToData(CWarpFieldEdgeData& Data);
	void Transform(COutlinePathMatrix* pMatrix);
	BOOL Linear(void)
		{ return Type() == Line; }
	void Path(COutlinePath* pPath, BOOL fMoveTo = FALSE);
	long X(long Y);
	long Y(long X);
	long SplineX(long Y);
	long SplineY(long X);
	long BezierX(long Y);
	long BezierY(long X);
public:
	EdgeType Type(void)
		{ return (EdgeType)m_nType; }
	void Type(EdgeType nType)
		{
			ASSERT(nType==Line || nType==Spline || nType==Bezier);
			m_nType = nType;
		}
protected:

	// Edge data -- must match CWarpFieldEdgeData
	SHORT m_nType;
	union
	{
		struct { long AX, AY, BX, BY; }						m_Line;
		struct { long AX, AY, BX, BY, CX, CY; }			m_Spline;
		struct { long AX, AY, BX, BY, CX, CY, DX, DY; }	m_Bezier;
	};
};

/////////////////////////////////////////////////////////////////////////////
// CWarpFieldBounds

// This data matches what is written to the document as part of the
// CWarpFieldRecord (PMG 2 and 3). It must match the structure used
// in the CWarpFieldBounds object.
struct CWarpFieldBoundsData
{
	SHORT m_nType;			// BoundsType
	CWarpFieldEdgeData m_Edge[4];
};

class CWarpFieldBounds
{
public:
	enum BoundsType
	{
		X,
		Y,
		XY
	};
public:
	void Init(const CWarpFieldBounds* pBounds)
		{ *this = *pBounds; }
	void FromData(const CWarpFieldBoundsData& Data);
	void ToData(CWarpFieldBoundsData& Data);
public:
	BoundsType Type(void)
		{ return (BoundsType)m_nType; }
	void Type(BoundsType nType)
		{
			ASSERT(nType==X || nType==Y || nType==XY);
			m_nType = nType;
		}
	CWarpFieldEdge* Edge(int nIndex)
		{ return &(m_Edge[nIndex]); }
	void Edge(int nIndex, const CWarpFieldEdge* pEdge)
		{ m_Edge[nIndex].Init(pEdge); }
	void Transform(COutlinePathMatrix* pMatrix);
	void Path(COutlinePath* pPath);
	BOOL Linear(void);
protected:

	// Bounds data -- must match CWarpFieldBoundsData
	SHORT m_nType;			// BoundsType
	CWarpFieldEdge m_Edge[4];
};

/////////////////////////////////////////////////////////////////////////////
// CWarpField

class CWarpField
{
public:
	CWarpField(void)
		{ m_crBounds.SetRectEmpty(); }
	const CString& Name(void)
		{ return m_csName; }
	void Name(LPCSTR lpszName)
		{ m_csName = lpszName; }
	CWarpFieldBounds* OriginalBounds(void)
		{ return &m_Original; }
	CWarpFieldBounds* TransformedBounds(void)
		{ return &m_Transformed; }
	void Transform(const CRect& crBounds);
	BOOL Linear(void)
		{ return m_Transformed.Linear(); }
public:
	void MoveTo(COutlinePath* pPath, long AX, long AY);
	void LineTo(COutlinePath* pPath, long AX, long AY);
	void SplineTo(COutlinePath* pPath, long AX, long AY, long BX, long BY);
	void BezierTo(COutlinePath* pPath, long AX, long AY, long BX, long BY, long CX, long CY);
public:
	void WarpPoint(long* pAX, long* pAY);
	void UnwarpPoint(long* pAX, long* pAY);
protected:
	void WarpLine(COutlinePath* pPath, long AX, long AY, long BX, long BY);
	void WarpSpline(COutlinePath* pPath, long AX, long AY, long BX, long BY, long CX, long CY);
	void WarpBezier(COutlinePath* pPath, long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY, int nDepth = 0);
protected:
	CString m_csName;
	CWarpFieldBounds m_Original;
	CWarpFieldBounds m_Transformed;
	CRect m_crBounds;
	COutlinePathMatrix m_Matrix;
	CFixedPoint m_CurrentPoint;
};

/////////////////////////////////////////////////////////////////////////////
// WarpField_CreateStruct

struct WarpField_CreateStruct
{
	LPCSTR m_pszName;
	CWarpFieldBoundsData m_BoundsData;
};

/////////////////////////////////////////////////////////////////////////////
// WarpFieldRecord

class WarpFieldRecord : public DatabaseRecord
{
public:
private:

/* Data */

	CString m_csName;			/* the name of this warp field. */
	
	struct
	{
		USHORT reference_count;
		CWarpFieldBoundsData m_BoundsData;
	} record;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	WarpFieldRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

public:

	virtual ~WarpFieldRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Set the reference count.
*/

	VOID set_reference_count(USHORT count)
		{ record.reference_count = count; }

/*
// Get the reference count.
*/

	USHORT get_reference_count(VOID)
		{ return record.reference_count; }

/*
// Increment the reference count.
*/

	USHORT inc_reference_count(VOID)
		{ return ++record.reference_count; }

/*
// Decrement the reference count.
*/

	USHORT dec_reference_count(VOID)
		{ return --record.reference_count; }

/*
// Set the name for this warp field.
*/

	void Name(LPCSTR pszName)
		{ m_csName = pszName; }

/*
// Get the name.
*/

	const CString& Name(void) const
		{ return m_csName; }
		
/*
// Get a pointer to the warp bounds.
*/

	CWarpFieldBoundsData* BoundsData()
		{ return &(record.m_BoundsData); }

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};

#endif
