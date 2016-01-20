/*
// $Workfile: DESKTOP.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DESKTOP.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 11    2/19/99 2:55p Johno
// Moved warning rect stuff from CWebEditDesktop into CDocEditDesktop
// (base class)
// 
// 10    1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 9     6/02/98 11:59a Johno
// Tweeks
// 
// 8     5/31/98 5:27p Johno
// Functional HTML warning rectangle
// 
// 7     3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 6     2/24/98 9:18a Dennis
// Added CWebEditDesktop class to prevent Web Backgrounds from being
// erased with white.
// 
// 5     12/09/97 8:31a Jstoner
// modified desktop so that label outline is dotted and shows thru objects
// drawn on top of desktop
// affects label and sticker projects
// 
// 4     12/02/97 11:41a Jstoner
// moved CStickerEditDesktop into its own file, mimimize rebuild time
// while working.
// 
// 3     12/02/97 9:02a Jstoner
// 
// 2     11/26/97 10:45a Jstoner
// minor fixes to desktop
// 
//    Rev 1.1   26 Oct 1997 18:57:40   JOE
// modified calendar desktop to handle picture/calendar
// 
//    Rev 1.0   14 Aug 1997 15:20:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:32   Fred
// Initial revision.
// 
//    Rev 1.4   10 Feb 1997 13:40:08   Jay
// Fixes, extensions
// 
//    Rev 1.3   07 Feb 1997 16:59:40   Jay
// Start of using a region for the gray; no flicker.
// 
//    Rev 1.2   31 Jan 1997 15:50:54   Jay
// New desktop background color
// 
//    Rev 1.1   28 Jul 1996 11:28:40   Jay
// Brochure stuff
// 
//    Rev 1.0   14 Mar 1996 13:26:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:54   FRED
// Initial revision.
// 
//    Rev 1.4   06 Aug 1995 09:41:52   JAY
// Banner desktop.
// 
//    Rev 1.3   20 Jul 1995 12:51:24   JAY
// Label project. New paper info stuff. etc
// 
//    Rev 1.2   12 Jul 1995 17:45:24   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 1.1   12 Jul 1995 08:24:04   JAY
// New card style.
// 
//    Rev 1.0   30 Jun 1995 16:36:52   JAY
// Initial revision.
*/ 

#ifndef __DESKTOP_H__
#define __DESKTOP_H__

#include "redisp.h"

class CPmwView;

typedef CArray<CPoint, CPoint&> CDesktopShape;

#define  WARNING_RECT_WIDTH   2

class CDocEditDesktop : public CEditDesktop
{
public:
	CDocEditDesktop(CPmwView* pView);

	virtual void DrawBackground(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawForeground(LPCRECT pClip, RedisplayContext* rc);

	virtual void SetView(CPmwView* pView);

	static COLORREF m_clDesktop;

   virtual void
   SetWarningRect(PBOX pb);
    
   virtual void
   GetWarningRect(PBOX &rpb)
   {
      ASSERT(&rpb != NULL);
      rpb = mWarningPBOX;   
   }
 
protected:
	virtual void DrawPanel(int nPanel, RedisplayContext* rc, BOOL fShadowed = FALSE);

	virtual void EraseBackground(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawPage(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawGuides(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawPanels(RedisplayContext* rc, BOOL fShadowed = FALSE);
	virtual void DrawShadowBox(PBOX pbox, RedisplayContext* rc);

	virtual BOOL GetDesktopRegion(RedisplayContext* rc, CRgn& Region, LPCRECT pClip = NULL);

	virtual BOOL GetPageBox(PBOX& pbox);

	virtual void GetDesktopShapes(PBOX pbox, RedisplayContext* rc, CDesktopShape& PageShape, CDesktopShape& ShadowShape);

	void DrawHorizontalGuides(Array* pGuideArray, RedisplayContext* rc, COLORREF Color = (COLORREF)-1);
	void DrawVerticalGuides(Array* pGuideArray, RedisplayContext* rc, COLORREF Color = (COLORREF)-1);

	CPmwView* m_pView;
   PBOX  mWarningPBOX;
};

class CCardEditDesktop : public CDocEditDesktop
{
public:
	CCardEditDesktop(CPmwView* pView);
protected:
	virtual void DrawPanel(int nPanel, RedisplayContext* rc, BOOL fShadowed = FALSE);
	virtual void DrawPage(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawPanels(RedisplayContext* rc, BOOL fShadowed = FALSE);

	void DrawPanelTitles(RedisplayContext* rc);

	virtual BOOL GetPageBox(PBOX& pbox);
};

class CLabelEditDesktop : public CDocEditDesktop
{
public:
	CLabelEditDesktop(CPmwView* pView);
protected:
	virtual void DrawShadowBox(PBOX pbox, RedisplayContext* rc);
	virtual BOOL GetDesktopRegion(RedisplayContext* rc, CRgn& Region, LPCRECT pClip = NULL);
	virtual void DrawBackground(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawForeground(LPCRECT pClip, RedisplayContext* rc);
};

class CPhotoProjectsEditDesktop : public CLabelEditDesktop
{
public:
	CPhotoProjectsEditDesktop(CPmwView* pView);
protected:
};

class CBannerEditDesktop : public CDocEditDesktop
{
public:
	CBannerEditDesktop(CPmwView* pView);
protected:
	virtual void DrawGuides(LPCRECT pClip, RedisplayContext* rc);

	virtual BOOL GetPageBox(PBOX& pbox);
};

class CBrochureEditDesktop : public CDocEditDesktop
{
public:
	CBrochureEditDesktop(CPmwView* pView);
protected:
//	virtual void DrawPage(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawGuides(LPCRECT pClip, RedisplayContext* rc);
};

class CCalendarEditDesktop : public CDocEditDesktop
{
public:
	CCalendarEditDesktop(CPmwView* pView);
protected:
	virtual void DrawGuides(LPCRECT pClip, RedisplayContext* rc);
	virtual BOOL GetPageBox(PBOX& pbox);
	void DrawPBOX(PBOX * pBox, RedisplayContext* rc);
};

class CWebEditDesktop : public CDocEditDesktop
{
public:
	CWebEditDesktop(CPmwView* pView);

protected:

   virtual void DrawBackground(LPCRECT pClip, RedisplayContext* rc);
};

class CNewsletterEditDesktop : public CDocEditDesktop
{
public:
	CNewsletterEditDesktop(CPmwView* pView);
protected:
	virtual void DrawGuides(LPCRECT pClip, RedisplayContext* rc);
	virtual void GetDesktopShapes(PBOX pbox, RedisplayContext* rc, CDesktopShape& PageShape, CDesktopShape& ShadowShape);
	virtual void DrawPage(LPCRECT pClip, RedisplayContext* rc);

	int GetLRStatus(void);
	CPoint GetFoldSize(RedisplayContext* rc);
};

#endif				// #ifndef __DESKTOP_H__
