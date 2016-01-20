/** waveconv.h                                                          **/

#ifndef _WAVECONV_H_
#define _WAVECONV_H_


//
//  bilingual...
//
#ifdef RC_INVOKED
    #define RCID(id)    id
#else
    #define RCID(id)    MAKEINTRESOURCE(id)
#endif


//
//  misc. defines
//
#define MAX_FILE_PATH_LEN       128
#define MAX_FILE_TITLE_LEN      16
#define MAX_FORMAT_LEN          32

#define MAX_ERR_LEN             512


//
//  resource defines...
//
#define ICON_APP                RCID(10)

#define MENU_APP                RCID(20)
#define IDM_FILE_OPEN           100
#define IDM_FILE_SAVEAS         101
#define IDM_FILE_ABOUT          109
#define IDM_FILE_EXIT           110
#define	IDM_INFO		201
#define IDM_DISP		202
#define IDM_PLAY		203
#define IDM_FORMAT		301
#define IDM_STEREO		302
#define IDM_MONO		303
#define IDM_SAMPLE		304


//
//
//
#define ABOUTBOX		RCID(26)


//
//
//
#define DLG_CONVERT             RCID(30)
#define IDD_TXT_INFILEPATH      100
#define IDD_TXT_OUTFILEPATH     101
#define IDD_TXT_STATUS          102

//
//
//
#define APP_WINDOW_WIDTH        600
#define APP_WINDOW_HEIGHT       350


//
//  Debugging support stuff
//
#ifdef DEBUG
    extern void FAR cdecl dprintf(LPSTR szFormat, ...);

    #define DPF     dprintf
#else
    #define DPF     ; / ## /
#endif

/////
//
//
/////

BOOL FAR PASCAL AppDialogBox(HWND hwnd, LPCSTR lpszDlg, DLGPROC lpfn, LPARAM lParam);



#endif


/** EOF: waveconv.h **/
