


#ifndef _STKR_DSKTP_H_
#define _STKR_DSKTP_H_

#include "desktop.h"

class CStickerEditDesktop : public CDocEditDesktop
{
public:
	CStickerEditDesktop(CPmwView* pView);
protected:
	virtual void DrawPanel(int nPanel, RedisplayContext* rc, BOOL fShadowed = FALSE);
	virtual BOOL GetPageBox(PBOX& pbox);
	virtual void DrawShadowBox(PBOX pbox, RedisplayContext* rc);
	virtual BOOL GetDesktopRegion(RedisplayContext* rc, CRgn& Region, LPCRECT pClip = NULL);
	virtual void DrawBackground(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawForeground(LPCRECT pClip, RedisplayContext* rc);
	virtual void DrawGuides(LPCRECT pClip, RedisplayContext* rc);

	
};

#endif