#ifndef __control
#define __control

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

#define BITMAP_EXTRA	(GWL_DATAPTR+sizeof(long))

// GWW_STATUS possible values
#define ST_TRACK		0x0001
#define ST_INRECT		0x0002

#define BS_MASK			0x01L
#define BS_TRANSPARENT	0x02L
#define BS_HIGHLIGHT	0x04L
#define BS_DUAL			0x08L

#define WS_HINTCAPTION	0x100L
#define WS_HINTRESOURCE 0x200L

// Messages for the meter control
#define METER_MAX		100000L
#define SM_GETPOSITION	(WM_USER + 700)
#define SM_SETPOSITION	(WM_USER + 701)

// Messages for the bmulti control
#define MB_SETSTATE (WM_USER + 702)
#define MB_GETSTATE	(WM_USER + 703)

// Messages for the DIB control
#define SM_SETSTATE		(WM_USER + 100)

#endif
