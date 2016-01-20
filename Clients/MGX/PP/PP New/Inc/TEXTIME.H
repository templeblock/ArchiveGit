/****************************************************************************

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.  
	This material is confidential and a trade secret.  Permission to use this 
	work for any purpose must be obtained in writing from 
	MICROGRAFX, 1303 E Arapaho Rd., Richardson, Tx.  75081.

*****************************************************************************
*****************************************************************************

                                    Text_IME
				 
*****************************************************************************
****************************************************************************/

#define IME_MODE_UNINITIALIZED 256

#define IS_MODE_JIS(mode) (!((mode) & IME_MODE_ALPHANUMERIC) || ((mode) & IME_MODE_DBCSCHAR))
#define DEFAULT_IME_MODE ( IME_MODE_NOCODEINPUT | IME_MODE_DBCSCHAR | IME_MODE_KATAKANA )
#define IME_MODE_KATA_OR_HIRA ( IME_MODE_KATAKANA | IME_MODE_HIRAGANA )

typedef BOOL( FAR PASCAL *WINNLSPROC )( HWND, BOOL );
typedef LRESULT( FAR PASCAL *MESSAGEPROC )( HWND, LPARAM );

typedef struct
{
    WORD   wCharSet;      	// Character set of the last font selected.
    WORD   wPrevIMEMode;  	// Previous IME input mode
    HFONT  hIMEFont;      	// handle to current IME font
    HFONT  hLastIMEFont;  	// Previous IME font handle
    HWND   hIMEWnd;       	// Handle to the current IME edit window
    HANDLE hIME;          	// Handle to Input method editor's data block
    LPARAM lIMEParam;     	// In 3.1 only the LOWORD is used, it contains the
                          	// memory handle of the IME memory block. In NT this
                          	// will be a pointer.
} IMESTATE, FAR *LPIMESTATE;

typedef struct
{
	HGLOBAL hIMEState;		// Handle to a global memory block		
    WORD   	wPrevIMEMode;  	// Previous IME input mode
	WORD 	wLastJISMode;
} TEXTIME, FAR *LPTEXTIME;

void  FAR PASCAL ImeInit( LPTEXTIME, HWND );
void  FAR PASCAL ImeTerminate( LPTEXTIME );
void  FAR PASCAL ImeMoveConvertWin( LPTEXTIME, int, int );
WORD  FAR PASCAL ImeSendVKey( LPTEXTIME, WORD );
WORD  FAR PASCAL ImeSetMode( LPTEXTIME, int );
WORD  FAR PASCAL ImeGetMode( LPTEXTIME lpTextIME );
void  FAR PASCAL ImeCreateFont( LPTEXTIME, LPLOGFONT );
void  FAR PASCAL ImeSetFont( LPTEXTIME );
DWORD FAR PASCAL ImeMessage( unsigned, UINT, long );
BOOL  FAR PASCAL SetupIME( HWND hWnd, LPLOGFONT lpLogFont );
BOOL  FAR PASCAL TerminateIME( void );
