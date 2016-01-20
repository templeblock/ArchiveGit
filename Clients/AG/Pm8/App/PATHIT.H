/*
// $Header: /PM8/App/PATHIT.H 1     3/03/99 6:08p Gbeddow $
//
// $Workfile: PATHIT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PATHIT.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:06   FRED
// Initial revision.
// 
//    Rev 1.1   18 May 1995 07:45:08   JAY
// More inflate nonsense.
// 
//    Rev 1.0   09 May 1995 08:07:12   JAY
// Initial revision.
*/

#ifndef __PATHIT_H__
#define __PATHIT_H__

/////////////////////////////////////////////////////////////////////////////
// COutlinePathIterator

class COutlinePathIterator
{
public:
	COutlinePathIterator(COutlinePathArray* pArray);

/* Special fixup. */

	void Fixup(void);

/* Path iteration. */

	void FirstPath(void);
	BOOL NextPath(void);

/* Point iteration. */

	CFixedPoint* FirstPoint(void);
	CFixedPoint* CurrentPoint(void);
	CFixedPoint* NextPoint(void);

/* Component iteration. */

	long* CurrentComponent(void)
		{ return m_pCurrentComponent; }
	long* GetNextComponent(void);
	long* NextComponent(void);
	void ToBOP(void);
	void ToEOP(void);

/* Status. */

	BOOL AtEnd(void) const
		{ return (*m_pCurrentComponent == COutlinePathArray::OPE_End); }
	BOOL AtClose(void) const
		{ return (*m_pCurrentComponent == COutlinePathArray::OPE_Close); }
	BOOL AtEOP(void) const;

	BOOL AtTerminalPoint(void) const
		{ return m_nCurrentPoint == m_nCurrentPoints-1; }

	void SetNewComponent(long* pComponent);
	DWORD ComponentOffset(long* pComponent)
		{ return (DWORD)(pComponent - m_pArrayData); }

protected:

	CFixedPoint* ComponentPoint(long* pComponent, int nPoint)
		{ return (CFixedPoint*)(pComponent + 1 + nPoint*2); }

	BOOL IsValidComponent(long* pComponent = NULL);
	int ComponentPoints(long* pComponent = NULL);

	COutlinePathArray* m_pArray;
	long* m_pArrayData;			// For Fixup().
	long* m_pCurrentPath;
	long* m_pCurrentComponent;
	long* m_pDiscoveredEOP;
	int m_nCurrentPoints;
	int m_nCurrentPoint;
};

#endif
