#ifndef __editpoly_h__
#define __editpoly_h__
/*=======================================================================*\

	EDITPOLY.H - Bezier and line segments with selection flags

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

class CEditPoly : public CPoly
{
public:
	CEditPoly();
	virtual ~CEditPoly();
	virtual BOOL IsA(LPTR lpClass);
	virtual BOOL AddPoint(LPPOINT lpPoint);
	virtual BOOL DeletePoint();
	virtual BOOL AddBezierLine(LPPOINT lpHandle1, LPPOINT lpHandle2, LPPOINT lpEnd);
	virtual BOOL AddBezierLine2(LPPOINT lpStart, LPPOINT lpHandle1, LPPOINT lpHandle2);
	virtual BOOL InsertPoint(LPPOINT lpPoint);
	virtual int LineToBezier(LPPOINT lpHandle1, LPPOINT lpHandle2);
	virtual BOOL BezierToLine();
	void SetFlags(WORD wFlags);
	void SetFlags(WORD wFlags, int index);
	WORD GetFlags();
	WORD GetFlags(int index);
	
private:
	BOOL make_room(int size);
	int FlagsAllocated;
	int FlagsUsed;
	LPWORD lpFlags;
};

typedef CEditPoly FAR *LPCEditPoly;


#endif /* __editpoly_h__ */
