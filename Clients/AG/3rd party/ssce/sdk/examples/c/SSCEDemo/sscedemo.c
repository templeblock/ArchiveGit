// Sentry Spelling Checker Engine
// SSCEDemo: SSCE Demo Program for Windows

// Copyright (c) 1995 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed by
// a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: sscedemo.c,v 5.14 2000/06/28 17:34:45 wsi Exp wsi $

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <ctype.h>
#include <malloc.h>                           
#include <ssce.h> 
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>  
#include <time.h>
#include "resrc1.h"

#define APP_NAME "SSCEDemo"
#define MAX_ALT_WORDS 50
#define MAX_FILE_NAME_LEN 256
#define MAX_FILE_NAME_SZ (MAX_FILE_NAME_LEN + 1)
#if !defined(MAX_PATH)
  #define MAX_PATH 260
#endif

// SSCE API function ids
enum {
	AddToLex,
	CheckBlock,
	CheckBlockDlg,
	CheckCtrlDlg,
	CheckWord,
	ClearLex,
	CloseBlock,
	CloseLex,
	CloseSession,
	CreateLex,
	DelBlockText,
	DelBlockWord,
	DelFromLex,
	EditLexDlg,
	FindLexWord,
	GetAutoCorrect,
	GetBlock,
	GetBlockInfo,
	GetBlockWord,
	GetHelpFile,
	GetLex,
	GetLexId,
	GetLexInfo,
	GetMainLexFiles,
	GetMainLexPath,
	GetMinSuggestDepth,
	GetOption,
	GetRegTreeName,
	GetSid,
	GetStatistics,
	GetStringTableName,
	GetUserLexFiles,
	GetUserLexPath,
	InsertBlockText,
	NextBlockWord,
	OpenBlock,
	OpenLex,
	OpenSession,
	OptionsDlg,
	ReplaceBlockWord,
	ResetLex,
	SetAutoCorrect,
	SetBlockCursor,
	SetDebugFile,
	SetDialogOrigin,
	SetHelpFile,
	SetIniFile,
	SetKey,
	SetMainLexFiles,
	SetMainLexPath,
	SetMinSuggestDepth,
	SetOption,
	SetRegTreeName,
	SetStringTableName,
	SetUserLexFiles,
	SetUserLexPath,
	Suggest,
	Version
};

// Error code names:
static const char *errorNames[] = {
	"Ok",
	"SSCE_TOO_MANY_SESSIONS_ERR",
	"SSCE_BAD_SESSION_ID_ERR",
	"SSCE_WORD_NOT_FOUND_ERR",
	"SSCE_FILE_NOT_FOUND_ERR",
	"SSCE_TOO_MANY_LEXICONS_ERR",
	"SSCE_UNKNOWN_ACTION_ERR",
	"SSCE_BAD_LEXICON_ID_ERR",
	"SSCE_BUFFER_TOO_SMALL_ERR",
	"SSCE_READ_ONLY_LEXICON_ERR",
	"Unknown error (-10)",
	"Unknown error (-11)",
	"SSCE_OUT_OF_MEMORY_ERR",
	"SSCE_UNSUPPORTED_ACTION_ERR",
	"SSCE_LEXICON_EXISTS_ERR",
	"SSCE_TOO_MANY_BLOCKS_ERR",
	"SSCE_BAD_BLOCK_ID_ERR",
	"SSCE_CANCEL_ERR",
	"Unknown error (-18)",
	"SSCE_INVALID_WORD_ERR",
	"SSCE_WORD_OUT_OF_SEQUENCE_ERR",
	"Unknown error (-21)",
	"SSCE_FILE_READ_ERR",
	"SSCE_FILE_WRITE_ERR",
	"SSCE_FILE_OPEN_ERR",
	"SSCE_BUSY_ERR",
	"SSCE_UNKNOWN_LEX_FORMAT_ERR"
};                     

// Function names and ids
static const struct {
	const char *name;
	int id;
	const char *param1Lbl;
	const char *param2Lbl;
	const char *param3Lbl;
	const char *param4Lbl;
	BOOL sidEnabled;
	BOOL lexIdEnabled;
	BOOL blkIdEnabled;
	BOOL textEnabled;
} functions[] = {
	{"SSCE_AddToLex", AddToLex, "word:", "action:", "otherWord:", NULL, TRUE, TRUE, FALSE, FALSE},
	{"SSCE_CheckBlock", CheckBlock, NULL, NULL, NULL, NULL, TRUE, FALSE, TRUE, TRUE},
	{"SSCE_CheckBlockDlg", CheckBlockDlg, "blkSz:", "showContext:", NULL, NULL, FALSE, FALSE, FALSE, TRUE},
	{"SSCE_CheckCtrlDlg", CheckCtrlDlg, "selectedOnly:", NULL, NULL, NULL, FALSE, FALSE, FALSE, TRUE},
	{"SSCE_CheckWord", CheckWord, "word:", NULL, NULL, NULL, TRUE, FALSE, FALSE, FALSE},
	{"SSCE_ClearLex", ClearLex, NULL, NULL, NULL, NULL, TRUE, TRUE, FALSE, FALSE},
	{"SSCE_CloseBlock", CloseBlock, NULL, NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_CloseLex", CloseLex, NULL, NULL, NULL, NULL, TRUE, TRUE, FALSE, FALSE},
	{"SSCE_CloseSession", CloseSession, NULL, NULL, NULL, NULL, TRUE, FALSE, FALSE, FALSE},
	{"SSCE_CreateLex", CreateLex, "fileName:", "language:", NULL, NULL, TRUE, FALSE, FALSE, FALSE},
	{"SSCE_DelBlockText", DelBlockText, "numChars:", NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_DelBlockWord", DelBlockWord, NULL, NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_DelFromLex", DelFromLex, "word:", NULL, NULL, NULL, TRUE, TRUE, FALSE, FALSE},
	{"SSCE_EditLexDlg", EditLexDlg, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_FindLexWord", FindLexWord, "word:", NULL, NULL, NULL, TRUE, TRUE, FALSE, FALSE},
	{"SSCE_GetAutoCorrect", GetAutoCorrect, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetBlock", GetBlock, NULL, NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_GetBlockInfo", GetBlockInfo, NULL, NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_GetBlockWord", GetBlockWord, NULL, NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_GetHelpFile", GetHelpFile, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetLex", GetLex, NULL, NULL, NULL, NULL, TRUE, TRUE, FALSE, FALSE},
	{"SSCE_GetLexId", GetLexId, "lexFileName:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetLexInfo", GetLexInfo, NULL, NULL, NULL, NULL, TRUE, TRUE, FALSE, FALSE},
	{"SSCE_GetMainLexFiles", GetMainLexFiles, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetMainLexPath", GetMainLexPath, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetMinSuggestDepth", GetMinSuggestDepth, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetOption", GetOption, "option:", NULL, NULL, NULL, TRUE, FALSE, FALSE, FALSE},
	{"SSCE_GetRegTreeName", GetRegTreeName, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetSid", GetSid, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetStatistics", GetStatistics, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetStringTableName", GetStringTableName, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetUserLexFiles", GetUserLexFiles, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_GetUserLexPath", GetUserLexPath, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_InsertBlockText", InsertBlockText, "text:", NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_NextBlockWord", NextBlockWord, NULL, NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_OpenBlock", OpenBlock, "blkSz:", NULL, NULL, NULL, TRUE, FALSE, FALSE, TRUE},
	{"SSCE_OpenLex", OpenLex, "fileName:", "memBudget:", NULL, NULL, TRUE, FALSE, FALSE, FALSE},
	{"SSCE_OpenSession", OpenSession, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_OptionsDlg", OptionsDlg, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_ReplaceBlockWord", ReplaceBlockWord, "word:", NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_ResetLex", ResetLex, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetAutoCorrect", SetAutoCorrect, "autoCorrect:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetBlockCursor", SetBlockCursor, "cursor:", NULL, NULL, NULL, TRUE, FALSE, TRUE, FALSE},
	{"SSCE_SetDebugFile", SetDebugFile, "fileName:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetDialogOrigin", SetDialogOrigin, "x:", "y:", NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetHelpFile", SetHelpFile, "fileName:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetIniFile", SetIniFile, "fileName:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetKey", SetKey, "licenseKey:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetMainLexFiles", SetMainLexFiles, "fileList:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetMainLexPath", SetMainLexPath, "path:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetMinSuggestDepth", SetMinSuggestDepth, "depth:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetOption", SetOption, "option:", "val:", NULL, NULL, TRUE, FALSE, FALSE, FALSE},
	{"SSCE_SetRegTreeName", SetRegTreeName, "treeName:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetStringTableName", SetStringTableName, "tableName:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetUserLexFiles", SetUserLexFiles, "fileList:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_SetUserLexPath", SetUserLexPath, "path:", NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE},
	{"SSCE_Suggest", Suggest, "word:", "depth:", NULL, NULL, TRUE, FALSE, FALSE, FALSE},
	{"SSCE_Version", Version, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE, FALSE}
};

// SSCE_CheckBlock and SSCE_CheckWord result names
static struct {
	SSCE_S16 mask;
	const char *name;
} checkResults[] = {
	{SSCE_MISSPELLED_WORD_RSLT, "SSCE_MISSPELLED_WORD_RSLT"},
	{SSCE_AUTO_CHANGE_WORD_RSLT, "SSCE_AUTO_CHANGE_WORD_RSLT"},
	{SSCE_CONDITIONALLY_CHANGE_WORD_RSLT, "SSCE_CONDITIONALLY_CHANGE_WORD_RSLT"},
	{SSCE_UNCAPPED_WORD_RSLT, "SSCE_UNCAPPED_WORD_RSLT"},
	{SSCE_MIXED_CASE_WORD_RSLT, "SSCE_MIXED_CASE_WORD_RSLT"},
	{SSCE_MIXED_DIGITS_WORD_RSLT, "SSCE_MIXED_DIGITS_WORD_RSLT"},
	{SSCE_END_OF_BLOCK_RSLT, "SSCE_END_OF_BLOCK_RSLT"},
	{SSCE_DOUBLED_WORD_RSLT, "SSCE_DOUBLED_WORD_RSLT"}
};

// Action names
static struct {
	SSCE_S16 action;
	const char *name;
} actions[] = {
	{SSCE_AUTO_CHANGE_ACTION, "SSCE_AUTO_CHANGE_ACTION"},
	{SSCE_AUTO_CHANGE_PRESERVE_CASE_ACTION, "SSCE_AUTO_CHANGE_PRESERVE_CASE_ACTION"},
	{SSCE_CONDITIONAL_CHANGE_ACTION, "SSCE_CONDITIONAL_CHANGE_ACTION"},
	{SSCE_CONDITIONAL_CHANGE_PRESERVE_CASE_ACTION, "SSCE_CONDITIONAL_CHANGE_PRESERVE_CASE_ACTION"},
	{SSCE_EXCLUDE_ACTION, "SSCE_EXCLUDE_ACTION"},
	{SSCE_IGNORE_ACTION, "SSCE_IGNORE_ACTION"}
};

// Lexicon format names
static struct {
	SSCE_S16 format;
	const char *name;
} lexFormats[] = {
	{SSCE_COMPRESSED_LEX_FMT, "SSCE_COMPRESSED_LEX_FMT"},
	{SSCE_TEXT_LEX_FMT, "SSCE_TEXT_LEX_FMT"}
};

// Language names
static struct {
	SSCE_S16 lang;
	const char *name;
} languages[] = {
	{SSCE_ANY_LANG, "SSCE_ANY_LANG"},
	{SSCE_AMER_ENGLISH_LANG, "SSCE_AMER_ENGLISH_LANG"},
	{SSCE_BRIT_ENGLISH_LANG, "SSCE_BRIT_ENGLISH_LANG"},
	{SSCE_CATALAN_LANG, "SSCE_CATALAN_LANG"},
	{SSCE_CZECH_LANG, "SSCE_CZECH_LANG"},
	{SSCE_DANISH_LANG, "SSCE_DANISH_LANG"},
	{SSCE_DUTCH_LANG, "SSCE_DUTCH_LANG"},
	{SSCE_FINNISH_LANG, "SSCE_FINNISH_LANG"},
	{SSCE_FRENCH_LANG, "SSCE_FRENCH_LANG"},
	{SSCE_GERMAN_LANG, "SSCE_GERMAN_LANG"},
	{SSCE_HUNGARIAN_LANG, "SSCE_HUNGARIAN_LANG"},
	{SSCE_ITALIAN_LANG, "SSCE_ITALIAN_LANG"},
	{SSCE_NORWEGIAN_BOKMAL_LANG, "SSCE_NORWEGIAN_BOKMAL_LANG"},
	{SSCE_NORWEGIAN_NYNORSK_LANG, "SSCE_NORWEGIAN_NYNORSK_LANG"},
	{SSCE_POLISH_LANG, "SSCE_POLISH_LANG"},
	{SSCE_PORTUGUESE_BRAZIL_LANG, "SSCE_PORTUGUESE_BRAZIL_LANG"},
	{SSCE_PORTUGUESE_IBERIAN_LANG, "SSCE_PORTUGUESE_IBERIAN_LANG"},
	{SSCE_RUSSIAN_LANG, "SSCE_RUSSIAN_LANG"},
	{SSCE_SPANISH_LANG, "SSCE_SPANISH_LANG"},
	{SSCE_SWEDISH_LANG, "SSCE_SWEDISH_LANG"},
};

static const char appName[] = APP_NAME;

LRESULT CALLBACK WinProc(HWND win, UINT msgType, WPARAM wParam, LPARAM lParam);

static void appendMsg(HWND win, const char *msg);
static void busy(BOOL on);
static void displayCheckResult(HWND win, const char *funcName,
  SSCE_S16 result);
static void displayResult(HWND win, const char *funcName, SSCE_S16 rv);
static SSCE_S16 getS16(HWND win, int id);
static SSCE_S32 getS32(HWND win, int id);
static SSCE_U32 getU32(HWND win, int id);
static void onCommand(HWND win, int id, HWND hwndCtl, UINT codeNotify);
static BOOL onCreate(HWND win, CREATESTRUCT *reateStruct);
static void onDestroy(HWND win);
static void onFunctionListSelChange(HWND win);
static void onInvokeBtn(HWND win);
static void onShowWindow(HWND win, BOOL fShow, UINT status);

int PASCAL WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine,
  int cmdShow) {
	MSG msg;
	HWND win;

	if (prevInstance == 0) {
		WNDCLASS winClass;

		winClass.style = CS_HREDRAW | CS_VREDRAW;
		winClass.lpfnWndProc = (WNDPROC)WinProc;
		winClass.cbClsExtra = 0;
		winClass.cbWndExtra = DLGWINDOWEXTRA;
		winClass.hInstance = instance;
		winClass.hIcon = LoadIcon(instance, APP_NAME);
		winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		winClass.lpszMenuName = NULL;
		winClass.lpszClassName = appName;

		RegisterClass(&winClass);
	}

	win = CreateDialog(instance, appName, 0, NULL);
	ShowWindow(win, cmdShow);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (msg.wParam);
}

// Process Windows messages.				 
LRESULT CALLBACK WinProc(HWND win, UINT msgType, WPARAM wParam, LPARAM lParam) {
	switch (msgType) {
	case WM_CREATE:
		return(HANDLE_WM_CREATE(win, wParam, lParam, onCreate));
	case WM_COMMAND:
		HANDLE_WM_COMMAND(win, wParam, lParam, onCommand);
		return (0);
	case WM_DESTROY:
		HANDLE_WM_DESTROY(win, wParam, lParam, onDestroy);
		return (0);
	case WM_SHOWWINDOW:
		HANDLE_WM_SHOWWINDOW(win, wParam, lParam, onShowWindow);
		return (0);
	}         
	return (DefWindowProc(win, msgType, wParam, lParam));
}

// Append a message to the result display.
static void appendMsg(HWND win, const char *msg) {
	char *newMsg;
	long len;

	// There's no easy way to append text to a text box. We'll insert an
	// extra character (a newline) each time text is added. When text is
	// added later, we'll select the extra character and replace the selection
	// with the new text.

	// Add a newline to the message.
	newMsg = (char *)malloc(strlen(msg) + 2 + 1);
	if (NULL == newMsg) {
		return;
	}
	strcpy(newMsg, msg);
	strcat(newMsg, "\r\n");

	len = Edit_GetTextLength(GetDlgItem(win, IDC_RESULT_EDT));
	if (len > 0) {
		// Select the last character.
		Edit_SetSel(GetDlgItem(win, IDC_RESULT_EDT), len, len);

		// Replace the selection with the new message.
		Edit_ReplaceSel(GetDlgItem(win, IDC_RESULT_EDT), newMsg);
	}
	else {
		// Just add the message.
		Edit_SetText(GetDlgItem(win, IDC_RESULT_EDT), newMsg);
	}
	free(newMsg);
}

// Indicate that we're busy doing something (or not)
static void busy(BOOL on) {
	static HCURSOR origCursor;
	
	if (on) {
		origCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);
	}
	else {
    	ShowCursor(FALSE);
		SetCursor(origCursor);
	}
}

// Display the result from SSCE_CheckBlock or SSCE_CheckWord.
static void displayCheckResult(HWND win, const char *funcName,
  SSCE_S16 result) {
	if (result <= 0) {
		displayResult(win, funcName, result);
	}
	else {
		int i;
		char msg[256];

		wsprintf(msg, "%s returned:", funcName);
		appendMsg(win, msg);
		for (i = 0; i < sizeof(checkResults) / sizeof(checkResults[0]); ++i) {
			if ((checkResults[i].mask & result) != 0) {
				appendMsg(win, checkResults[i].name);
			}
		}
	}
}

// Display the result from an SSCE function.
static void displayResult(HWND win, const char *funcName, SSCE_S16 rv) {
	char msg[256];

	if (rv < 0) {
		wsprintf(msg, "%s returned %s", funcName, errorNames[-rv]);
	}
	else {
		wsprintf(msg, "%s returned %hd", funcName, rv);
	}
	appendMsg(win, msg);
}

// Obtain an SSCE_S16 value from an edit control.
static SSCE_S16 getS16(HWND win, int id) {
	char str[64];

	GetDlgItemText(win, id, str, sizeof(str));
	return ((SSCE_S16)atoi(str));
}

// Obtain an SSCE_S32 value from an edit control.
static SSCE_S32 getS32(HWND win, int id) {
	char str[64];

	GetDlgItemText(win, id, str, sizeof(str));
	return ((SSCE_S32)strtol(str, NULL, 0));
}

// Obtain an SSCE_U32 value from an edit control.
static SSCE_U32 getU32(HWND win, int id) {
	char str[64];

	GetDlgItemText(win, id, str, sizeof(str));
	return ((SSCE_U32)strtoul(str, NULL, 0));
}

// Respond to a WM_COMMAND message.
static void onCommand(HWND win, int id, HWND hwndCtl, UINT codeNotify) {
	switch (id) {
	case IDC_FUNCTION_LST:
		if (codeNotify == LBN_SELCHANGE || LBN_DBLCLK) {
			onFunctionListSelChange(win);
		}
		break;
	case IDC_INVOKE_BTN:
		onInvokeBtn(win);
		break;
	}
}

// Respond to a WM_CREATE message.
static BOOL onCreate(HWND win, CREATESTRUCT *createStruct) {
	return (TRUE);
}
 
// Respond to a WM_DESTROY message.
static void onDestroy(HWND win) {
	PostQuitMessage(0);
}

// Respond to a change in the function list selection.
static void onFunctionListSelChange(HWND win) {
	int idx = ListBox_GetCurSel(GetDlgItem(win, IDC_FUNCTION_LST));
	int id;
	int i;

	if (LB_ERR == idx) {
		return;
	}

	// Locate the function table entry corresponding to the selection.
	id = (int)ListBox_GetItemData(GetDlgItem(win, IDC_FUNCTION_LST), idx);
	for (i = 0; i < sizeof(functions) / sizeof(functions[0]); ++i) {
		if (functions[i].id == id) {
			break;
		}
	}
	if (i >= sizeof(functions) / sizeof(functions[0])) {
		// Shouldn't happen, but just in case...
		return;
	}

	// Clear the parameters.
	SetDlgItemText(win, IDC_PARAM1_EDT, "");
	SetDlgItemText(win, IDC_PARAM2_EDT, "");
	SetDlgItemText(win, IDC_PARAM3_EDT, "");
	SetDlgItemText(win, IDC_PARAM4_EDT, "");

	// Set the parameter labels as specified in the table.
	SetDlgItemText(win, IDC_PARAM1_LBL, functions[i].param1Lbl != NULL ?
	  functions[i].param1Lbl : "");
	SetDlgItemText(win, IDC_PARAM2_LBL, functions[i].param2Lbl != NULL ?
	  functions[i].param2Lbl : "");
	SetDlgItemText(win, IDC_PARAM3_LBL, functions[i].param3Lbl != NULL ?
	  functions[i].param3Lbl : "");
	SetDlgItemText(win, IDC_PARAM4_LBL, functions[i].param4Lbl != NULL ?
	  functions[i].param4Lbl : "");
	EnableWindow(GetDlgItem(win, IDC_PARAM1_EDT),
	  functions[i].param1Lbl != NULL);
	EnableWindow(GetDlgItem(win, IDC_PARAM2_EDT),
	  functions[i].param2Lbl != NULL);
	EnableWindow(GetDlgItem(win, IDC_PARAM3_EDT),
	  functions[i].param3Lbl != NULL);
	EnableWindow(GetDlgItem(win, IDC_PARAM4_EDT),
	  functions[i].param4Lbl != NULL);

	// Enable the session, lexicon, block id and text fields as required.
	EnableWindow(GetDlgItem(win, IDC_SESSION_ID_EDT), functions[i].sidEnabled);
	EnableWindow(GetDlgItem(win, IDC_LEXICON_ID_EDT),
	  functions[i].lexIdEnabled);
	EnableWindow(GetDlgItem(win, IDC_BLOCK_ID_EDT), functions[i].blkIdEnabled);
	EnableWindow(GetDlgItem(win, IDC_TEXT_EDT), functions[i].textEnabled);
}

// Respond to an Invoke button press
static void onInvokeBtn(HWND win) {
	int idx = ListBox_GetCurSel(GetDlgItem(win, IDC_FUNCTION_LST));
	int id;
	int i;
	char str[256];
	char path[MAX_PATH];
	SSCE_S16 s16, s16_1, s16_2, s16_3;
	SSCE_S32 s32, s32_1, s32_2, s32_3, s32_4;
	SSCE_CHAR word[SSCE_MAX_WORD_SZ];
	SSCE_CHAR otherWord[SSCE_MAX_WORD_SZ];
	SSCE_CHAR *bfr;
	SSCE_CHAR *p;
	const char *fmt, *lang;
	SSCE_S16 scores[16];

	if (LB_ERR == idx) {
		return;
	}

	id = (int)ListBox_GetItemData(GetDlgItem(win, IDC_FUNCTION_LST), idx);

	// Invoke the selected function.
	switch (id) {
	case AddToLex:
		GetDlgItemText(win, IDC_PARAM1_EDT, word, sizeof(word));
		GetDlgItemText(win, IDC_PARAM3_EDT, otherWord, sizeof(otherWord));
		s16 = SSCE_AddToLex(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_LEXICON_ID_EDT), word,
		  getS16(win, IDC_PARAM2_EDT), otherWord);
		displayResult(win, "SSCE_AddToLex", s16);
		break;
	case CheckBlock:
		s16 = SSCE_CheckBlock(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), word, sizeof(word),
		  otherWord, sizeof(otherWord));
		displayCheckResult(win, "SSCE_CheckBlock", s16);
		if (s16 >= 0) {
			wsprintf(str, "errWord: %s; otherWord: %s", word, otherWord);
		}
		appendMsg(win, str);
		break;
	case CheckBlockDlg:
		s32_1 = getS32(win, IDC_PARAM1_EDT);
		bfr = (SSCE_CHAR *)malloc((size_t)s32_1);
		GetDlgItemText(win, IDC_TEXT_EDT, bfr, (int)s32_1);
		s32 = SSCE_CheckBlockDlg(win, bfr, strlen(bfr), s32_1,
		  getS16(win, IDC_PARAM2_EDT));
		wsprintf(str, "SSCE_CheckBlockDlg returned %ld", s32);
		appendMsg(win, str);
		if (s32 >= 0) {
			SetDlgItemText(win, IDC_TEXT_EDT, bfr);
		}
		free(bfr);
		break;
	case CheckCtrlDlg:
		s16 = SSCE_CheckCtrlDlg(win, GetDlgItem(win, IDC_TEXT_EDT),
		  getS16(win, IDC_PARAM1_EDT));
		displayResult(win, "SSCE_CheckCtrlDlg", s16);
		break;
	case CheckWord:
		GetDlgItemText(win, IDC_PARAM1_EDT, word, sizeof(word));
		s16 = SSCE_CheckWord(getS16(win, IDC_SESSION_ID_EDT),
		  word, otherWord, sizeof(otherWord));
		displayCheckResult(win, "SSCE_CheckWord", s16);
		if (s16 >= 0) {
			wsprintf(str, "otherWord: %s", otherWord);
		}
		appendMsg(win, str);
		break;
	case ClearLex:
		s16 = SSCE_ClearLex(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_LEXICON_ID_EDT));
		displayResult(win, "SSCE_ClearLex", s16);
		break;
	case CloseBlock:
		s16 = SSCE_CloseBlock(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT));
		displayResult(win, "SSCE_CloseBlock", s16);
		break;
	case CloseLex:
		s16 = SSCE_CloseLex(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_LEXICON_ID_EDT));
		displayResult(win, "SSCE_CloseLex", s16);
		break;
	case CloseSession:
		s16 = SSCE_CloseSession(getS16(win, IDC_SESSION_ID_EDT));
		displayResult(win, "SSCE_CloseSession", s16);
		break;
	case CreateLex:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_CreateLex(getS16(win, IDC_SESSION_ID_EDT),
		  path, getS16(win, IDC_PARAM2_EDT));
		if (s16 >= 0) {
			wsprintf(str, "%hd", s16);
			SetDlgItemText(win, IDC_LEXICON_ID_EDT, str);
		}
		wsprintf(str, "SSCE_CreateLex(%s) returned %hd", path, s16);
		appendMsg(win, str);
		break;
	case DelBlockText:
		s16 = SSCE_DelBlockText(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), getS32(win, IDC_PARAM1_EDT));
		displayResult(win, "SSCE_DelBlockText", s16);
		break;
	case DelBlockWord:
		s32 = SSCE_DelBlockWord(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), word, sizeof(word));
		wsprintf(str, "SSCE_DelBlockWord returned %ld; deleted text = %s",
		  s32, word);
		appendMsg(win, str);
		break;
	case DelFromLex:
		GetDlgItemText(win, IDC_PARAM1_EDT, word, sizeof(word));
		s16 = SSCE_DelFromLex(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_LEXICON_ID_EDT), word);
		displayResult(win, "SSCE_DelFromLex", s16);
		break;
	case EditLexDlg:
		s16 = SSCE_EditLexDlg(win);
		displayResult(win, "SSCE_EditLexDlg", s16);
		break;
	case FindLexWord:
		GetDlgItemText(win, IDC_PARAM1_EDT, word, sizeof(word));
		s16 = SSCE_FindLexWord(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_LEXICON_ID_EDT), word, otherWord, sizeof(otherWord));
		if (s16 < 0) {
			displayResult(win, "SSCE_FindLexWord", s16);
		}
		else {
			int i;
			const char *actionName = "(unknown)";

			for (i = 0; i < sizeof(actions) / sizeof(actions[0]); ++i) {
				if (actions[i].action == s16) {
					actionName = actions[i].name;
					break;
				}
			}
			wsprintf(str, "SSCE_FindLexWord returned %s", actionName);
			appendMsg(win, str);
			if (otherWord[0] != '\0') {
				wsprintf(str, "otherWord: %s", otherWord);
				appendMsg(win, str);
			}
		}
		break;
	case GetAutoCorrect:
		s16 = SSCE_GetAutoCorrect();
		displayResult(win, "SSCE_GetAutoCorrect", s16);
		break;
	case GetBlock:
		s16 = SSCE_GetBlockInfo(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), &s32_1, NULL, NULL, NULL);
		if (s16 < 0) {
			displayResult(win, "SSCE_GetBlockInfo", s16);
			break;
		}
		bfr = (SSCE_CHAR *)malloc((size_t)s32_1 + 1);
		if (NULL == bfr) {
			appendMsg(win, "Out of memory");
			break;
		}
		s16 = (SSCE_S16)SSCE_GetBlock(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), bfr, s32_1 + 1);
		bfr[s32_1] = '\0';
		displayResult(win, "SSCE_GetBlock", s16);
		appendMsg(win, "Block contents:");
		appendMsg(win, bfr);
		free(bfr);
		break;
	case GetBlockInfo:
		s16 = SSCE_GetBlockInfo(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), &s32_1, &s32_2, &s32_3, &s32_4);
		displayResult(win, "SSCE_GetBlockInfo", s16);
		wsprintf(str,
		  "textLen = %ld; blkSz = %ld; curPos = %ld; wordCount = %ld",
		  s32_1, s32_2, s32_3, s32_4);
		appendMsg(win, str);
		break;
	case GetBlockWord:
		s16 = SSCE_GetBlockWord(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), word, sizeof(word));
		displayResult(win, "SSCE_GetBlockWord", s16);
		wsprintf(str, "word: %s", word);
		appendMsg(win, str);
		break;
	case GetHelpFile:
		SSCE_GetHelpFile(path, sizeof(path));
		wsprintf(str, "help file: %s", path);
		appendMsg(win, str);
		break;
	case GetLex:
		s16 = SSCE_GetLexInfo(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_LEXICON_ID_EDT), &s32_1, NULL, NULL);
		if (s16 < 0) {
			displayResult(win, "SSCE_GetLexInfo", s16);
			break;
		}
		bfr = (SSCE_CHAR *)malloc((size_t)s32_1);
		if (NULL == bfr) {
			appendMsg(win, "Out of memory");
			break;
		}
		s32 = SSCE_GetLex(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_LEXICON_ID_EDT), bfr, s32_1);
		wsprintf(str, "SSCE_GetLex returned %ld", s32);
		appendMsg(win, str);
		for (p = bfr; *p != '\0'; p += strlen(p) + 1) {
			appendMsg(win, p);
		}
		free(bfr);
		break;
	case GetLexId:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_GetLexId(path);
		displayResult(win, "SSCE_GetLexId", s16);
		break;
	case GetLexInfo:
		s16 = SSCE_GetLexInfo(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_LEXICON_ID_EDT), &s32_1, &s16_2, &s16_3);
		displayResult(win, "SSCE_GetLexInfo", s16);
		fmt = "(unknown)";
		for (i = 0; i < sizeof(lexFormats) / sizeof(lexFormats[0]); ++i) {
			if (lexFormats[i].format == s16_2) {
				fmt = lexFormats[i].name;
				break;
			}
		}
		lang = "(unknown)";
		for (i = 0; i < sizeof(languages) / sizeof(languages[0]); ++i) {
			if (languages[i].lang == s16_3) {
				lang = languages[i].name;
				break;
			}
		}
		wsprintf(str, "size = %ld; format = %s; language = %s", s32_1,
		  fmt, lang);
		appendMsg(win, str);
		break;
	case GetMainLexFiles:
		SSCE_GetMainLexFiles(path, sizeof(path));
		wsprintf(str, "MainLexFiles: %s", path);
		appendMsg(win, str);
		break;
	case GetMainLexPath:
		SSCE_GetMainLexPath(path, sizeof(path));
		wsprintf(str, "MainLexPath: %s", path);
		appendMsg(win, str);
		break;
	case GetMinSuggestDepth:
		s16 = SSCE_GetMinSuggestDepth();
		displayResult(win, "SSCE_GetMinSuggestDepth", s16);
		break;
	case GetOption:
		s32 = (SSCE_S32)SSCE_GetOption(getS16(win, IDC_SESSION_ID_EDT),
		  (SSCE_U32)getS32(win, IDC_PARAM1_EDT));
		if (s32 < 0) {
			displayResult(win, "SSCE_GetOption", (SSCE_S16)s32);
		}
		else {
			wsprintf(str, "SSCE_GetOption returned %ld", s32);
			appendMsg(win, str);
		}
		break;
	case GetRegTreeName:
		SSCE_GetRegTreeName(path, sizeof(path));
		wsprintf(str, "RegTreeName: %s", path);
		appendMsg(win, str);
		break;
	case GetSid:
		s16 = SSCE_GetSid();
		displayResult(win, "SSCE_GetSid", s16);
		if (s16 >= 0) {
			wsprintf(str, "%hd", s16);
			SetDlgItemText(win, IDC_SESSION_ID_EDT, str);
		}
		break;
	case GetStatistics:
		SSCE_GetStatistics((SSCE_U32 *)&s32_1, (SSCE_U32 *)&s32_2,
		  (SSCE_U32 *)&s32_3);
		wsprintf(str,
		  "wordsChecked = %lu; wordsChanged = %lu; errorsDetected = %lu",
		  s32_1, s32_2, s32_3);
		appendMsg(win, str);
		break;
	case GetStringTableName:
		SSCE_GetStringTableName(path, sizeof(path));
		wsprintf(str, "StringTableName: %s", path);
		appendMsg(win, str);
		break;
	case GetUserLexFiles:
		SSCE_GetUserLexFiles(path, sizeof(path));
		wsprintf(str, "UserLexFiles: %s", path);
		appendMsg(win, str);
		break;
	case GetUserLexPath:
		SSCE_GetUserLexPath(path, sizeof(path));
		wsprintf(str, "UserLexPath: %s", path);
		appendMsg(win, str);
		break;
	case InsertBlockText:
		GetDlgItemText(win, IDC_PARAM1_EDT, word, sizeof(word));
		s16 = SSCE_InsertBlockText(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), word);
		displayResult(win, "SSCE_InsertBlockText", s16);
		break;
	case NextBlockWord:
		s16 = SSCE_NextBlockWord(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT));
		displayResult(win, "SSCE_NextBlockWord", s16);
		break;
	case OpenBlock:
		s32_1 = getS32(win, IDC_PARAM1_EDT);
		bfr = (SSCE_CHAR *)malloc((size_t)s32_1);
		if (NULL == bfr) {
			appendMsg(win, "Out of memory");
			break;
		}
		GetDlgItemText(win, IDC_TEXT_EDT, bfr, (int)s32_1);
		s16 = SSCE_OpenBlock(getS16(win, IDC_SESSION_ID_EDT),
		  bfr, strlen(bfr), s32_1, TRUE);
		if (s16 >= 0) {
			wsprintf(str, "%hd", s16);
			SetDlgItemText(win, IDC_BLOCK_ID_EDT, str);
		}
		displayResult(win, "SSCE_OpenBlock", s16);
		free(bfr);
		break;
	case OpenLex:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_OpenLex(getS16(win, IDC_SESSION_ID_EDT),
		  path, getS32(win, IDC_PARAM2_EDT));
		if (s16 >= 0) {
			wsprintf(str, "%hd", s16);
			SetDlgItemText(win, IDC_LEXICON_ID_EDT, str);
		}
		wsprintf(str, "SSCE_OpenLex(%s) returned %hd", path, s16);
		appendMsg(win, str);
		break;
	case OpenSession:
		s16 = SSCE_OpenSession();
		if (s16 >= 0) {
			wsprintf(str, "%hd", s16);
			SetDlgItemText(win, IDC_SESSION_ID_EDT, str);
		}
		displayResult(win, "SSCE_OpenSession", s16);
		break;
	case OptionsDlg:
		s16 = SSCE_OptionsDlg(win);
		displayResult(win, "SSCE_OptionsDlg", s16);
		break;
	case ReplaceBlockWord:
		GetDlgItemText(win, IDC_PARAM1_EDT, word, sizeof(word));
		s16 = SSCE_ReplaceBlockWord(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), word);
		displayResult(win, "SSCE_ReplaceBlockWord", s16);
		break;
	case ResetLex:
		s16 = SSCE_ResetLex();
		displayResult(win, "SSCE_ResetLex", s16);
		break;
	case SetAutoCorrect:
		s16 = SSCE_SetAutoCorrect(getS16(win, IDC_PARAM1_EDT));
		displayResult(win, "SSCE_SetAutoCorrect", s16);
		break;
	case SetBlockCursor:
		s16 = SSCE_SetBlockCursor(getS16(win, IDC_SESSION_ID_EDT),
		  getS16(win, IDC_BLOCK_ID_EDT), getS32(win, IDC_PARAM1_EDT));
		displayResult(win, "SSCE_SetBlockCursor", s16);
		break;
	case SetDebugFile:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		SSCE_SetDebugFile(path);
		wsprintf(str, "Diagnostic file set to %s", path);
		break;
	case SetDialogOrigin:
		SSCE_SetDialogOrigin(getS16(win, IDC_PARAM1_EDT),
		  getS16(win, IDC_PARAM2_EDT));
		appendMsg(win, "Dialog origin set");
		break;
	case SetHelpFile:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_SetHelpFile(path);
		displayResult(win, "SSCE_SetHelpFile", s16);
		break;
	case SetIniFile:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_SetIniFile(path);
		displayResult(win, "SSCE_SetIniFile", s16);
		break;
	case SetKey:
		s32 = SSCE_SetKey(getU32(win, IDC_PARAM1_EDT));
		wsprintf(str, "SSCE_SetKey returned %ld", s32);
		appendMsg(win, str);
		break;
	case SetMainLexFiles:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_SetMainLexFiles(path);
		displayResult(win, "SSCE_SetMainLexFiles", s16);
		break;
	case SetMainLexPath:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_SetMainLexPath(path);
		displayResult(win, "SSCE_SetMainLexPath", s16);
		break;
	case SetMinSuggestDepth:
		s16 = SSCE_SetMinSuggestDepth(getS16(win, IDC_PARAM1_EDT));
		displayResult(win, "SSCE_SetMinSuggestDepth", s16);
		break;
	case SetOption:
		s32 = (SSCE_S32)SSCE_SetOption(getS16(win, IDC_SESSION_ID_EDT),
		  (SSCE_U32)getS32(win, IDC_PARAM1_EDT),
		  (SSCE_U32)getS32(win, IDC_PARAM2_EDT));
		wsprintf(str, "SSCE_SetOption returned %ld", s32);
		appendMsg(win, str);
		break;
	case SetRegTreeName:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_SetRegTreeName(path);
		displayResult(win, "SSCE_SetRegTreeName", s16);
		break;
	case SetStringTableName:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_SetStringTableName(path);
		displayResult(win, "SSCE_SetStringTableName", s16);
		break;
	case SetUserLexFiles:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_SetUserLexFiles(path);
		displayResult(win, "SSCE_SetUserLexFiles", s16);
		break;
	case SetUserLexPath:
		GetDlgItemText(win, IDC_PARAM1_EDT, path, sizeof(path));
		s16 = SSCE_SetUserLexPath(path);
		displayResult(win, "SSCE_SetUserLexPath", s16);
		break;
	case Suggest:
		s16_1 = SSCE_MAX_WORD_SZ * 16;
		bfr = (SSCE_CHAR *)malloc((size_t)s16_1);
		if (NULL == bfr) {
			appendMsg(win, "Out of memory");
			break;
		}
		GetDlgItemText(win, IDC_PARAM1_EDT, word, sizeof(word));
		s16 = SSCE_Suggest(getS16(win, IDC_SESSION_ID_EDT),
		  word, getS16(win, IDC_PARAM2_EDT), bfr, (SSCE_S32)s16_1,
		  scores, sizeof(scores) / sizeof(scores[0]));
		displayResult(win, "SSCE_Suggest", s16);
		if (s16 >= 0) {
			for (p = bfr, i = 0; *p != '\0'; p += strlen(p) + 1, ++i) {
				wsprintf(str, "%s [%hd]", p, scores[i]);
				appendMsg(win, str);
			}
		}
		free(bfr);
		break;
	case Version:
		SSCE_Version(word, sizeof(word));
		wsprintf(str, "Version: %s", word);
		appendMsg(win, str);
		break;
	}
}

// Respond to a WM_SHOWWINDOW message.
static void onShowWindow(HWND win, BOOL fShow, UINT status) {
	static BOOL firstTime = TRUE;

	if (firstTime) {
		int i;

		// Fill the function list with names.
		for (i = 0; i < sizeof(functions) / sizeof(functions[0]); ++i) {
			int idx;

			idx = ListBox_AddString(GetDlgItem(win, IDC_FUNCTION_LST),
			  functions[i].name);
			ListBox_SetItemData(GetDlgItem(win, IDC_FUNCTION_LST), idx,
			  functions[i].id);
		}
		ListBox_SetCurSel(GetDlgItem(win, IDC_FUNCTION_LST), 0);
		onFunctionListSelChange(win);

		appendMsg(win, "IMPORTANT! Be sure to invoke SSCE_SetKey with your Sentry license");
		appendMsg(win, "key, provided by Wintertree Software. Enter the key in hexadecimal");
		appendMsg(win, "like this: 0x1234ABCD");
		appendMsg(win, "The demo will not run without a valid key!");
	}
}

