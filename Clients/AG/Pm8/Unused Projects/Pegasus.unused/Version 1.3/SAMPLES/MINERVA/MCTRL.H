/*
    mctrl.h
*/



/* control initialization data

   This structure is pretty freely accessed by mdlg.c and mdlgctrl.c -- and of course
   implicitly by any opcode which defines a static instance of a CTRLINITDATA array.
   
   The intent was to allow ini settings read/write to be fairly automatic, and then
   it became fairly convenient for retrieving dialog control values from the options
   dialogs and allowing the settings to be retrieved some later time to supply to the opcode */
typedef struct _CtrlInitData
    {
    char *pszName;      /* name of ini key for this control (0 if none)
                            this should be the same for all radio buttons in a group
                            as that's how a push of one unpushes the others */
    enum eCtrlType
        {
        eEnd, eText, eGroupBox, ePushButton, eRadioButton, eCheckBox, eEdit,
        eListBox, eScrollBar, eComboSimple, eComboDropDown, eComboDropList,
        eDialog
        } eType;        /* no need yet for eGroupBox, eListBox, eScrollBar or eCombo...
                            so they aren't implemented */
    int   nID;          /* dialog box control identifier */
    /* I wouldn't bother with the union, but it's such a pain to
        initialize the CTRLINITDATA static arrays already */
    union                             
        {
        BOOL bPushed;       /* checkbox or radio button control value is pushed or not */
        BOOL bPassword;     /* edit is a password field (scramble to ini file) */
        } u;
    WORD  wLimitText;   /* maximum length for edit (0 is not acceptable, although it's
                            probably tolerated too much) or any static text which must
                            be dynamic (like property values) */
    LPSTR pszText;      /* overloaded -- edit or static text value or
                            radio button ini setting value for this radio button */
    BOOL  bInactive;    /* control is inactive -- grayed during WM_INITDIALOG and can't
                            be enabled within the MDLGCTRL.C API */
    } CTRLINITDATA, PICFAR* LPCTRLINITDATA;



BOOL   CtrlDataDup(LPCTRLINITDATA arCtrlDefaults, LPCTRLINITDATA PICFAR* ppCtrlData);
void   CtrlDataReadDefaults(
    LPCSTR pszIniFilename,
    LPCSTR pszSection,
    LPCTRLINITDATA pCtrlDefaultData);
void   CtrlDataWriteDefaults(
    LPCSTR pszIniFilename,
    LPCSTR pszSection,
    LPCTRLINITDATA ppCtrlData);
void   CtrlDataFree(LPCTRLINITDATA PICFAR* parCtrlData);
BOOL   CtrlDataIsPushed(LPCTRLINITDATA pCtrlData, int nID);
void   CtrlDataSetPushed(LPCTRLINITDATA pCtrlData, int nID, BOOL bPushed);
int    CtrlDataGetText(LPCTRLINITDATA arCtrlData, int nID, LPSTR pszText, int nSize);
void   CtrlDataSetText(LPCTRLINITDATA pCtrlData, int nID, LPCSTR pszText);
void   CtrlDataSetInactive(LPCTRLINITDATA arCtrlData, int nID);
BOOL   CtrlDataIsInactive(LPCTRLINITDATA arCtrlData, int nID);
void   CtrlEnable(HWND hwndDlg, LPCTRLINITDATA arCtrlData, int nID, BOOL bEnabled);
BOOL   CtrlIsEnabled(HWND hwndDlg, int nID);
void   CtrlDataSetLong(LPCTRLINITDATA arCtrlData, int nID, long lVal, BOOL bSigned);
LPCTRLINITDATA CtrlData(LPCTRLINITDATA arCtrlData, int nID);
long   CtrlDataGetLong(LPCTRLINITDATA arCtrlData, int nID, BOOL bSigned);
int    CtrlDataWhichRadio(LPCTRLINITDATA arCtrlData, int nID);
void   CtrlDataSetDecFixed(LPCTRLINITDATA arCtrlData, int nID, long lVal, int nFixed);
