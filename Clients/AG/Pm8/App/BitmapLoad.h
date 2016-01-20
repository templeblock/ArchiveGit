#ifndef LOAD256BITMAP_H
#define LOAD256BITMAP_H

HBITMAP Load256Bitmap( LPCTSTR lpBitmapName );
HBITMAP Load256Bitmap( UINT nID);

void SetButtonBitmap( CWnd* pParent, UINT nCtlID, UINT nBitmapID );

#endif // LOAD256BITMAP_H
