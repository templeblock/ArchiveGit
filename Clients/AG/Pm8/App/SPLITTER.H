/*
// $Workfile: SPLITTER.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPLITTER.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:00   Fred
// Initial revision.
// 
//    Rev 1.0   24 Jun 1996 00:31:18   Fred
// Initial revision.
*/

class CSplitterCursor;

#ifndef __SPLITTER_H__
#define __SPLITTER_H__

//////////////////////////////////////////////////////////////////////////
// CSplitterCursor

class CSplitterCursor
{
public:
	enum
	{
		Padding	=	2		// padding for grab area to either size of splitter bar
	};

public:
	CSplitterCursor();
	~CSplitterCursor();

public:
	void SetWindow(CWnd* pWnd)
		{ m_pWnd = pWnd; }
	void StartShow(void);
	void EndShow(void);
	void StartDrag(int nX, int nOffset, int nClipLeft, int nClipRight);
	void Drag(int nX);
	void EndDrag(int nX);

protected:
	void SetX(int nX);
	void DrawDrag(void);

public:
	BOOL IsShowing(void)
		{ return m_fShowing; }
	BOOL IsDragging(void)
		{ return m_fDragging; }
	int GetX(void)
		{ return m_nX; }

protected:
	CWnd* m_pWnd;
	BOOL m_fShowing;
	BOOL m_fDragging;
	int m_nX;
	int m_nOffset;
	int m_nClipLeft;
	int m_nClipRight;
	HCURSOR m_hcSplitter;
	HCURSOR m_hcPrevious;
};

#endif


