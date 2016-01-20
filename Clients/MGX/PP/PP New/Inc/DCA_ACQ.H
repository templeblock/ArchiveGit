/***********************************************************************
 TWAIN source code:
 Copyright (C) '91-'92 TWAIN Working Group: 
 Aldus, Caere, Eastman-Kodak, Logitech,
 Hewlett-Packard Corporations.
 All rights reserved.
*************************************************************************/

// Source file: dca_acq.c
VOID DCAcquire ( void );
long getEnumerationValue(pTW_ENUMERATION pEnumeration, int j);
int twainGetTransferType( void );
int twainGetFileFormat( void );
int twainNegotiatePixelTypes( void );
BOOL ProcessTWMessage(LPMSG lpMsg, HWND hWnd);
VOID DCTransferImage(HWND hWnd);
int GetCompleteNativeImage(HWND hWnd);
int GetCompleteFileImage(HWND hWnd, TW_SETUPFILEXFER dcFileSetup);
int GetCompleteMemoryImage (HWND hWnd, TW_SETUPMEMXFER dcMemSetup);
void StoreImageRect( LPFRAME lpFrame, TW_IMAGEINFO imageInfo, TW_IMAGEMEMXFER imageData, FRMDATATYPE Type, int expand);
void OpenImageWindow(HINSTANCE hInstance, HWND hWindow, LPVOID lpVoid, int DataType);
void twainError(int id);
int twainImageLoad( ITEMID idFileType, LPTSTR szName );


