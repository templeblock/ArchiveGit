#ifndef _control_h
#define _control_h

// Generic custom control definitions
#define WS_NOTENABLED	(WS_NOTACTIVE|WS_DISABLED)
#define WS_NOTACTIVE	1L

// All controls should have the first 3 words reserved
// either private data or a data pointer should follow
#define GWW_STATE		0
#define GWL_FONT		(GWW_STATE+sizeof(WORD))
#define GWW_STATUS		(GWL_FONT+sizeof(long))
#define GWW_ICONID		(GWW_STATUS+sizeof(WORD))
#define GWL_DATAPTR		(GWW_ICONID+sizeof(WORD)) 
#define GWW_COMMAND		(GWL_DATAPTR+sizeof(long))

#define BITMAP_EXTRA	(GWW_COMMAND+sizeof(WORD))

// GWW_STATUS possible values
#define ST_TRACK		0x0001
#define ST_INRECT		0x0002

#define BS_MASK			0x01L
#define BS_TRANSPARENT	0x02L
#define BS_HIGHLIGHT	0x04L
#define BS_DUAL			0x08L
#define BS_TRI			0x10L

#define WS_HINTCAPTION	0x100L
#define WS_HINTRESOURCE 0x200L

#define MC_VERT			0x2L

// Messages for the meter control
#define METER_MAX		100000L
#define SM_GETPOSITION	(WM_USER + 700)
#define SM_SETPOSITION	(WM_USER + 701)

#define BM_SETCOLOR		(WM_USER + 702)
#define BM_GETCOLOR		(WM_USER + 703)

// Messages for the DIB control
#define SM_SETSTATE		(WM_USER + 100)
#define SM_GETSTATE		(WM_USER + 101)
#define SM_SETCOMMAND	(WM_USER + 102)
#define SM_FORCEPAINT	(WM_USER + 103)
#define SM_SETICONID	(WM_USER + 104)

// Message for the Video's parent
#define WM_VIDEO_MSG	(WM_USER + 704)

// User Message for video control
#define WM_VIDEOMSG_BASE	(WM_USER+1000)
#define WM_VIDEOSOUND_ONOFF (WM_VIDEOMSG_BASE)
#define WM_VIDEO_EVENT		(WM_VIDEOMSG_BASE + 1)
#define WM_VIDEO_GOTOSHOT   (WM_VIDEOMSG_BASE + 2)

#endif
