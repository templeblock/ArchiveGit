#ifndef _PIXUTIL_H_
#define _PIXUTIL_H_

#include "pixmap.h"

BOOL PixmapInitUndo( LPPIXMAP lpPixmap );
BOOL PixmapGetInfo(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, LPINT lpXSize, LPINT lpYSize, LPINT lpDepth, LPFRMDATATYPE lpType);
BOOL PixmapGetInfoEx(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, LPINT lpXSize, LPINT lpYSize, LPINT lpDepth, LPFRMDATATYPE lpType, LPINT lpResolution);
LPTR PixmapPtr(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, int x, int y, BOOL bModify);
LPTR PixmapPtr(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, int x, int y, BOOL bModify, int dx );
LPTR PixmapPtrRaw(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, int x, int y, BOOL bModify, int dx = 0);
void PixmapCheck(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, int x, int y, BOOL bModify );
BOOL PixmapWrite(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, int sx, int sy, int dx, LPTR pixels, int count );
VOID PixmapSyncLines( LPPIXMAP lpPixmap );
LPFRAME PixmapFrame(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType);
void PixmapFree( LPPIXMAP lpPixmap );
void PixmapFreeUndo( LPPIXMAP lpPixmap );
void PixmapAddUndoArea(LPPIXMAP lpPixmap, LPRECT lpUndo);
BOOL PixmapEditInit( LPPIXMAP lpPixmap, BOOL UseApply);
void PixmapEditUndo( LPPIXMAP lpPixmap );
void PixmapEditApply( LPPIXMAP lpPixmap );
void PixmapNewFrame( LPPIXMAP lpPixmap, LPFRAME lpNewFrame, LPRECT lpEditRect,
					BOOL fUseApply);
#endif // _PIXMAP_H_
