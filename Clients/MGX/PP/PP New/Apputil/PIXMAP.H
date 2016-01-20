/*®FD1¯®PL1¯®TP0¯®BT0¯®RM240¯*/
#ifndef _PIXMAP_H_
#define _PIXMAP_H_
#include "mgxframe.h"
#include "appdata.h"
#define LINE_EDITED     1
#define UNDO_EMPTY      2

typedef enum
	{
	PMT_EDIT,
	PMT_UNDO
	} PIXMAP_TYPE;

void PixmapSetup( LPPIXMAP lpPixmap, LPFRAME lpFrame, BOOL NoUndo);
void PixmapFree( LPPIXMAP lpPixmap );
void PixmapFreeUndo( LPPIXMAP lpPixmap );

#endif // _PIXMAP_H_


