/* globals */

typedef HANDLE HDIB;
typedef HANDLE HPAL;


#define PALFXMENU	1

/* file menu items */

#define     IDM_OPEN     01
#define     IDM_EXIT     02
#define     IDM_ABOUT    03

/* effects menu items */

#define     IDM_TOBLACK  200
#define     IDM_TODIB    201
#define     IDM_REVERT   202

/* Control IDs */

#define     IDC_FILENAME  400
#define     IDC_EDIT      401
#define     IDC_FILES     402
#define     IDC_PATH      403
#define     IDC_LISTBOX   404

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long    FAR PASCAL MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
HANDLE FAR PASCAL OpenDlg(HWND, unsigned, WORD, LONG);
BOOL CheckFileName(HWND, PSTR, PSTR);
void SeparateFile(HWND, LPSTR, LPSTR, LPSTR);
void UpdateListBox(HWND);
void AddExt(PSTR, PSTR);
void ChangeDefExt(PSTR, PSTR);

