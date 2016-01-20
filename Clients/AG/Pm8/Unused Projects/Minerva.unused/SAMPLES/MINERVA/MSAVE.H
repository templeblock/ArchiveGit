/*
    msave.h
*/



void SaveOnFileSave(HWND hwnd);
void SaveOnFileSaveAs(HWND hwnd);
BOOL SaveGetSaveAs(LPSTR pszFilename, SAVEOP **ppSaveOp);
BOOL SaveImage(
    HWND                  hwnd,
    LPCSTR                pszFilename,
    SAVEOP               *pSaveOp,
    BYTE PICHUGE* PICFAR* ppSavedImage,
    DWORD PICFAR*         pdwSavedLen);