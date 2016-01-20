//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
#ifndef _MACRO_H_
#define _MACRO_H_

typedef struct _macrosetup
	{
	int	iTotal;
	ITEMID idDialog;
	LPTSTR lpFileName;
	} MACROSETUP, FAR *LPMACROSETUP;

// Source file: macro.c
BOOL WINPROC DlgMacroPlayProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgMacroRecordProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgBatchMacroProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgBatchOptionsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgMacroStatusProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPackageProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Recorder( int iCode, WPARAM wParam, LPARAM lParam );
BOOL InstallRecorder( LPTSTR pRecordFile );
BOOL RemoveRecorder( void );
LRESULT CALLBACK Player( int iCode, WPARAM wParam, LPARAM lParam );
BOOL InstallPlayer( LPTSTR pPlayFile );
BOOL RemovePlayer( void );
BOOL DemoRunning( void );
BOOL IsJournalPlaying( void );
BOOL IsJournalRecording( void );
BOOL PlayJournalMacro(LPTSTR lpFileName);
BOOL RecordJournalMacro(LPTSTR lpFileName);
#endif // _MACRO_H_

