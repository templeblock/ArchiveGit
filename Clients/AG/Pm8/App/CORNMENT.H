/*
// $Workfile: CORNMENT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CORNMENT.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:34   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:14   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:56:10   FRED
//  
// 
//    Rev 1.5   28 Oct 1994 16:30:20   FRED
// ResetSelf() and working CInterfaceObjects
// 
//    Rev 1.4   13 Oct 1994 14:18:32   JAY
// Renamed some update/commit stuff
// 
//    Rev 1.3   12 Oct 1994 13:01:28   JAY
// Integration of Commit and Update at CPropertyObject level
// 
//    Rev 1.2   12 Oct 1994 08:44:00   JAY
// Now uses PID_Last chains in private PID enums
// 
//    Rev 1.1   19 Sep 1994 11:53:50   FRED
// Some enhancements.
// 
// 
//    Rev 1.0   16 Sep 1994 10:48:40   FRED
// Initial revision.
*/

class COrnament;
class COrnamentState;

#ifndef	__CORNMENT_H__
#define	__CORNMENT_H__

class CDrawContext;

class COrnamentState : public CPropertyState
{

public:

	// management

	COrnamentState();
	virtual ~COrnamentState();
	
	// implementation

	virtual void Init(void);

	enum
	{
		// private properties

		PID_Stretch						=		CPropertyState::PID_Last,
		PID_Last
	};
	
	DECLARE_OBJECT_PROPERTIES(COrnamentState)
	DEFINE_OBJECT_PROPERTY_COPY()

public:

	DEFINE_OBJECT_PROPERTY_DWORD(Style, PID_Style, "Style")
	DEFINE_OBJECT_PROPERTY_CPOINT(Position, PID_Position, "Position")
	DEFINE_OBJECT_PROPERTY_READ_ONLY(Origin, PID_Origin, "Origin", PTYPE_CPoint, CPoint, m_cpOrigin)
	DEFINE_OBJECT_PROPERTY_READ_ONLY(Offset, PID_Offset, "Offset", PTYPE_CSize, CSize, m_czOffset)
	DEFINE_OBJECT_PROPERTY_CSIZE(Extent, PID_Extent, "Extent")
	DEFINE_OBJECT_PROPERTY_READ_ONLY(Aspect, PID_Aspect, "Aspect", PTYPE_CSize, CSize, m_czAspect)
	DEFINE_OBJECT_PROPERTY_READ_ONLY(Bounds, PID_Bounds, "Bounds", PTYPE_CRect, CRect, m_crBounds)
	DEFINE_OBJECT_PROPERTY_BOOL(Visible, PID_Visible, "Visible")
};

class COrnament : public CPropertyObject
{
public:

	// management
	
	COrnament();
	virtual ~COrnament();
	
	// implementation
	
	DEFINE_PROPERTY_STATE(COrnamentState)

	virtual void Init(CDrawContext* pContext, BOOL fReset = TRUE);
	
	virtual void Reset(void);
	void ResetSelf(void);
	
	virtual void Draw(void);
	virtual BOOL Contains(const CPoint& cpLocation);
	
	virtual BOOL UpdateNeeded(void);
	virtual BOOL UpdateSelf(void);

	virtual BOOL CommitNeeded(void);
	virtual BOOL CommitSelf(void);

	virtual BOOL MaskNeeded(void) const;
	virtual BOOL MaskNeeded(CPropertyState* pState) const;
	virtual void ClearMaskNeeded(void);
	virtual BOOL ImageNeeded(void) const;
	virtual BOOL ImageNeeded(CPropertyState* pState) const;
	virtual void ClearImageNeeded(void);
	
	virtual BOOL SizeChanged(CPropertyState* pState) const;
	virtual BOOL PositionChanged(CPropertyState* pState) const;
	virtual BOOL AppearanceChanged(CPropertyState* pState) const;
	virtual BOOL StateChanged(CPropertyState* pState) const;

protected:

	// helper functions, virtual functions are supplied by derived class

	virtual void SizeContent(void) = 0;
	virtual CBitmap* GetMask(void);
	virtual CBitmap* GetImage(void);
	virtual void DrawMask(CDC* pDC) = 0;
	virtual void DrawImage(CDC* pDC) = 0;
	virtual CPoint GetOrigin(void) { return CPoint(0,0); }
	virtual CSize GetOffset(void) { return CSize(0,0); }
	
   void ResetBitmaps(void);

	// members

	CDrawContext* m_pContext;

	CPropertyState* m_pInvalidateState;
	CBitmap m_bmMask;
	CPropertyState* m_pMaskState;
	CBitmap m_bmImage;
	CPropertyState* m_pImageState;
};

#endif		// #ifndef __CORNMENT_H__

