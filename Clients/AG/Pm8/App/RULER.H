/*
// $Header: /PM8/App/RULER.H 1     3/03/99 6:11p Gbeddow $
//
// ruler.h : Ruler class
//
// $Log: /PM8/App/RULER.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     6/02/98 12:09p Cboggio
// Code changes for Metric rulers (centimetres instead of inches) for
// localization
// 
//    Rev 1.0   14 Aug 1997 15:25:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:48   Fred
// Initial revision.
// 
//    Rev 1.5   12 Mar 1997 15:52:24   Jay
// Rotation, etc.
// 
//    Rev 1.4   19 Feb 1997 16:53:30   Jay
// Snapping
// 
//    Rev 1.3   10 Feb 1997 13:40:10   Jay
// Fixes, extensions
// 
//    Rev 1.2   06 Feb 1997 17:07:04   Jay
// Cursor lines
// 
//    Rev 1.1   06 Feb 1997 12:57:26   Jay
// Ruler now displays marks and numbers.
// 
//    Rev 1.0   06 Feb 1997 09:13:34   Jay
// Rulers!
*/

#ifndef __RULER_H__
#define __RULER_H__

class CPmwView;

class CRulerLineArray : public CWordArray
{
public:
#ifndef WIN32
	int Append(const CRulerLineArray& src);
	void Copy(const CRulerLineArray& src);
#endif
};

class CRuler : public CWnd
{
	INHERIT(CRuler, CWnd);

public:
	CRuler(CPmwView* pView, int nType);
	virtual ~CRuler();

	void Update(void);

	BOOL Create(UINT uID);

	enum
	{
		Horizontal,
		Vertical,
		Corner
	};

	PCOORD GetLastUnit(void) const
		{ return m_LastUnit; }

#ifdef LOCALIZE
	PCOORD GetLastBaseX0(void) const
	{return m_BaseX0;}

	PCOORD GetLastBaseY0(void) const
	{return m_BaseY0;}

	double GetLastdUnit(void) const
	{ return m_dLastUnit;}
#endif

protected:
	// Generated message map functions
	//{{AFX_MSG(CRuler)
	afx_msg void OnPaint();
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CPmwView* m_pView;
	int m_nType;
	CRulerLineArray m_cwaLines;			// Shadow lines shown on the ruler.
	PCOORD m_LastUnit;						// What units were we drawn with?

#ifdef LOCALIZE
	PCOORD m_BaseY0;
	PCOORD m_BaseX0;
	double m_dLastUnit;
#endif

	void UpdateCursorShadow(void);
	void ToggleLines(CDC* pDC, CRulerLineArray& cwaToggle);

	static CFont& GetXFont(void);
	static CFont& GetYFont(void);
	static void GetFontMetrics(TEXTMETRIC& tm);

	static CFont m_cfXRuler;			// Font for X ruler text.
	static CFont m_cfYRuler;			// Font for Y ruler text.
};

#endif
