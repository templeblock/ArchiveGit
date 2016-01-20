/*
    mdlg.h
*/



typedef BOOL (*PFNDLGONCOMMAND)(
    HWND           hwndDlg,
    int            nCtrlID,
    WORD           wNotifyCode,
    LPCTRLINITDATA arInitData,
    LPCSTR         pszDlgTemplate,
    void PICFAR*   pUserData);



void DlgCenterInOwner(HWND hWnd);
int  DlgDoModal(
    HWND            hwnd,
    LPCSTR          pszDlgName,
    PFNDLGONCOMMAND pfnOnCommand,
    LPCTRLINITDATA  arInitData,
    void PICFAR*    pUserData);
void DlgSetModified(HWND hwndDlg);
void DlgClearModified(HWND hwndDlg);
void DlgOnOK(HWND hwndDlg);
