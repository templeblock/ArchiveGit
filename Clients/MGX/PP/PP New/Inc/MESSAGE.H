#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "forum.h"
// Source file: message.c
int PPMessageBox(LPCTSTR lpszText, UINT nType);

void FrameError(ITEMID idMsg);
BOOL MessageEnable(BOOL bEnable);
void dbg( LPTSTR lpFormat, ... );
int Print( LPTSTR lpFormat, ... );
int Message( ITEMID idStr, ... );
void PrintStatus( LPTSTR lpFormat, ... );
WORD MessageStatus( ITEMID idStr, ... );
WORD MessageHint( ITEMID idStr, ... );
WORD SendMessageStatus( ITEMID idStr, ... );
void StatusOfRectangle( LPRECT lpRect, LFIXED fScale, int Resolution );
void Status( LPTSTR lpString );
int AstralConfirm( ITEMID idStr, ... );
int AstralOKCancel( ITEMID idStr, ... );
BOOL AstralAffirm( ITEMID idStr, ... );
void ReadOutF( ITEMID idStr, int iNum, ... );
HWND MessageParent( void );
int GetDlgString(LPTSTR lpInitText, LPTSTR lpResult, ITEMID idStr, ...);
int WINPROC GetDlgStringProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
int WINPROC DlgYesToAllProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgYesNoRetryProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

int AstralYesNoYesToAll(HWND hParent, BOOL bMultiple, int idDialog, int idStr, ... );
int AstralYesToAllString(HWND hParent, int idDialog, LPTSTR lpString, BOOL bMultiple);
int AstralYesToAllStrings(HWND hParent, int idDialog, LPTSTR lpString, LPTSTR lpAltString, BOOL bMultiple);
int AstralYesNoRetry(HWND hParent, int idDialog, int idStr, ... );

void SetMessageLogging(CFile *pFile);
void LogMessage(CFile *pFile, LPTSTR lpMessage);

#define X_STATUS 6

class CStatusForum : public CForum
{
	public:
	CStatusForum();

	DEFINE_FORUM_TOPIC(Status, CString);
	protected:
};

extern CStatusForum	StatusForum;

#endif
