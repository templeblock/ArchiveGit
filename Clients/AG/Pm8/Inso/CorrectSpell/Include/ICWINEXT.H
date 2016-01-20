/****************************************************************************
 *                   Copyright  1995  INSO Corporation                      *
 *                International CorrectSpell[TM] Software                   *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************
 
*-----------------------------------------------------------------------*
*                                                                       *
*    ICwinext.h - return codes and extern declarations for the          *
*             Windows version of ICS                                    *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description:
    This header contains all return codes and extern declarations
    for the window demonstration delivery spell module.  This 
    header must be included to call any functions in this module 
    or get access to its global data.

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICWINEXT_H
#define ICWINEXT_H

#ifndef __BORLANDC__
VOID FAR PASCAL WEP(int);
#endif
RETCODE CALLBACK ICDlgHandler(HWND, LPSTR, WPARAM);
BOOL FAR PASCAL ICwinact  ( HWND, LPSTR, WPARAM );
VOID FAR PASCAL ICwinter ( HWND, PFICBUFF );
VOID FAR PASCAL SendInfo ( HWND, LPSTR );
INT2B   FAR PASCAL GetWord  ( HWND );
void FAR PASCAL IntError ( INT2B, LPSTR );
VOID FAR PASCAL ResetOptionsVars ( HWND );
VOID FAR PASCAL CheckReturn( HWND, INT2B, INT2B );
VOID FAR PASCAL UpdateCurrWord( HWND, LPSTR );
BOOL CALLBACK OptionsDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK CWADlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK HyphDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK VerDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AnErrDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK NotFoundDlgProc(HWND, UINT, WPARAM, LPARAM);
VOID FAR PASCAL ReplaceWord(VOID);
int FAR PASCAL HypCount(BYTE, PFUCHAR);
VOID FAR PASCAL ProcessHyph(HWND, LONG);
LPSTR FAR PASCAL GetResString(HANDLE, WPARAM);
VOID FAR PASCAL AddXtraPunct(PFUCHAR);
VOID FAR PASCAL SplitChange(HWND, WPARAM);
VOID FAR PASCAL BuildInsideWord(LPSTR, BOOL);
VOID FAR PASCAL StrippedInput(LPSTR, BOOL, BOOL);
int FAR PASCAL GetCrudeToken(PFUCHAR, PFUCHAR, LPINT, LPINT);
VOID NEAR PASCAL RunChk(HWND, WPARAM);
int CALLBACK GetOptionsDlg(VOID);

/* Window demonstration functions only                     */

/* ICwinmsc.c */
VOID FAR PASCAL feedListbox( HWND );
RETCODE FAR PASCAL ReadConfigFile( BOOL );
VOID FAR PASCAL ICDisplayOptions( HWND );
INT2B FAR PASCAL InitLang( INT2B, BOOL );
VOID FAR PASCAL ICBuildMenu( HMENU );
RETCODE FAR PASCAL DoMenuAction( HWND, HMENU, WPARAM );
VOID FAR PASCAL SetLangIndex(VOID);

/* ICwintls.c */
INT2B FAR PASCAL strtoi( LPSTR );
INT2B FAR PASCAL itostr( LPSTR, INT2B, INT2B );
INT2B FAR PASCAL ltostr( LPSTR, LONG, INT2B );
LONG FAR PASCAL Wstrtol( LPSTR );
LPSTR FAR PASCAL lstrrchr( LPSTR, int);
VOID FAR PASCAL CenterDbox( HWND );
INT4B FAR PASCAL FileLength( HFILE );
BOOL NEAR PASCAL RdFile(HANDLE, HANDLE, HANDLE, POFSTRUCT, LPSTR, BOOL);
BOOL NEAR PASCAL WrFile(HANDLE, HANDLE, HANDLE, POFSTRUCT, LPSTR, BOOL);
VOID FAR PASCAL FillList(HWND);
VOID FAR PASCAL FillMsg(HWND);
INT2B FAR PASCAL InsertHyp(PFUCHAR, PFUINT4B, UCHAR);

VOID PASCAL  ParsePathFile(LPSTR, LPSTR, LPSTR);

#ifdef ICS_PDICTIONARY
VOID FAR FirstPDinit(HWND);

void PopFileInitialize (HWND) ;
BOOL PopFileOpenDlg    (HWND, LPSTR, LPSTR) ;
BOOL PopFileSaveDlg    (HWND, LPSTR, LPSTR) ;   /* Standard Save File DlgBox */

INT2B NEAR PASCAL DictLimit(VOID);

BOOL NEAR PASCAL DoPDinit(void);   /* do malloc for PDPArray */
void PDdisperr(SL_STATUS,HWND);

/*VOID NEAR PASCAL IPDisplayOptions(HWND, PFIPBUFF);  */

INT2B NEAR PASCAL DictLimit(VOID);
VOID FAR PASCAL OpenPDFile(char*, HWND);
VOID FAR PASCAL NewPDFile(char*, HWND);
VOID FAR PASCAL ClosePDFile(HWND);
BOOL FAR PASCAL SaveasPDFile(char* ,HWND);
BOOL FAR PASCAL SavePDFile(HWND);

BOOL CALLBACK PDCtypeDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK PDClsnosavDlgProc(HWND , UINT, WPARAM ,LPARAM);
BOOL CALLBACK PDAddwdDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK PDDelwdDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK PDOptionDlgProc(HWND, UINT , WPARAM, LPARAM);

VOID NEAR PASCAL PDlanglist(HWND);

BOOL CALLBACK PDSelectDlgProc(HWND, UINT , WPARAM, LPARAM);
VOID FAR PASCAL PDslist(HWND, int); /* It's also used in icwindlg.c */

BOOL CALLBACK PDWdlistDlgProc(HWND, UINT , WPARAM, LPARAM);
VOID NEAR PASCAL PDWordls(HWND);

BOOL FAR PASCAL DiscardPDFile(HWND);

VOID PDAfree(void);

VOID FirstPDinit(HWND); /* It's also used in icwindlg.c */
BOOL FAR PASCAL CommDlgProc(HWND hDlg, int id_Dlgbox, FARPROC PDinDlgProc );
VOID FAR PASCAL PDfaradd(HWND hDlg, LPSTR word, UCHAR langcode, BOOL ask);

#endif

#endif /* ICWINEXT_H */

/*    end of file ICwinext.h        */
