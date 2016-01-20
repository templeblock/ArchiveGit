/*®FD1¯®PL1¯®TP0¯®BT0¯®RM240¯*/
#ifndef _PIXMAP_H_
#define _PIXMAP_H_
#include "framelib.h"

#define LINE_EDITED     1
#define UNDO_EMPTY      2

typedef enum
	{
	PMT_EDIT,
	PMT_UNDO
	} PIXMAP_TYPE;

typedef struct _pixmap
	{
	struct _frame far *EditFrame;
	struct _frame far *UndoFrame;
	BOOL    		UndoFailed;		// cannot create undo for this image
	LPTR			EditFlags;		// Dirty Lines buffer
	RECT			UndoRect;		// Undo area in file coordinates
	BOOL			fNewFrame;		// image has new edit frame, undo different
	} PIXMAP, FAR* LPPIXMAP;

#endif // _PIXMAP_H_

