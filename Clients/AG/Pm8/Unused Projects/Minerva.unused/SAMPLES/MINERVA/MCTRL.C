/******************************************************************************
 *
 * MCTRL.C - Minerva dialog box control utility functions
 *
 * Copyright (C) 1996, 1997 Pegasus Imaging Corporation
 *
 * This source code is provided to you as sample source code.  Pegasus Imaging
 * Corporation grants you the right to use this source code in any way you
 * wish. Pegasus Imaging Corporation assumes no responsibility and explicitly
 * disavows any responsibility for any and every use you make of this source
 * code.
 *
 * Global functions:
 *   CtrlDataDup           - allocate a duplicate instance of the control init data
 *   CtrlDataWriteDefaults - write control values to application ini file
 *   CtrlDataReadDefaults  - create a duplicate control value instance and initialize
 *                           from the application ini file
 *   CtrlDataFree          - free dup'ed ctrl data
 *   CtrlDataIsPushed      - return TRUE if the control is pushed
 *   CtrlDataSetInactive   - set the control as inactive
 *   CtrlDataIsInactive    - return TRUE if the control is inactive
 *   CtrlIsEnabled         - return TRUE if a control's window is enabled
 *   CtrlEnable            - Enable/Disable a control if it's not inactive
 *   CtrlDataGetText       - retrieve the text for the control
 *   CtrlDataSetText       - set the text for the edit or static text control
 *   CtrlDataSetLong       - set the text for the edit or static text control to a
 *                           numeric value formatted with commas
 *   CtrlDataGetLong       - get the numeric value for an edit or static text control
 *                           possibly formatted with commas
 *   CtrlDataSetPushed     - set checkbox or radio button pushed state
 *   CtrlDataWhichRadio    - given a radio button id, return the id of the pressed radio
 *                           button in the same group
 *   CtrlData              - look up init data for a control
 *   CtrlDataSetDecFixed   - set control data to fixed point decimal
 *
 * Local functions:
 *   ScramblePassword      - scramble password before writing to ini file
 *   UnScramblePassword    - unscramble password after reading from ini file
 * 
 * Revision History:
 *   12-18-96  1.00.03  jrb  once-over and additional commenting
 *   12-05-96  1.00.01  jrb  dialog settings were being read out of the ini file
 *                           but not set in the dup'ed ctrlinitdata, rather in
 *                           the static ctrlinitdata.
 *                           added password scrambling to/from INI file
 *                           dup defaults gp'ed on null defaults (like a bmp open
 *                           from mobmp.c)
 *   12-01-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#pragma warning(disable:4001)
/* PIC includes */
#include "pic.h"

/* minerva includes */
#include "resource.h"
#include "mctrl.h"
#include "mdlg.h"
#include "mopcodes.h"
#include "minerva.h" 
#include "mmisc.h"



/* used by ScramblePassword and UnscramblePassword */
#define DWORD_SCRAMBLER "qvxz"



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void ScramblePassword(LPSTR pszScrambled, LPCSTR pszPlainText, int wLenPlain);
static void UnScramblePassword(LPSTR pszPlainText, LPCSTR pszScrambled, int wLenPlain);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL CtrlDataDup(LPCTRLINITDATA arCtrlDefaults, LPCTRLINITDATA PICFAR* ppCtrlData)
 * 
 *  allocate a duplicate instance of the control init data
 *
 *  parameters:
 *      arCtrlDefaults - instance of array of control data to be dup'ed
 *      ppCtrlData     - returned pointer to duplicate
 *
 *  returns:  
 *      TRUE unless duplicate instance data can't be allocated
 *
 *  notes:
 *      This is used internally to mdlgctrl.c to duplicate the static instance passed into
 *      CtrlDataReadDefaults.  It is used externally in New Window to duplicate the current
 *      options values for the current window into the new window.
 *
 *      if arCtrlDefaults is NULL, then *ppCtrlData is returned NULL, although TRUE is returned
 *      This occurs for a bitmap opened by mobmp.c
 *
 *      wLimitText must have been specified for each edit control and for each static text
 *      control whose text changes (e.g. properties values) or read/write space won't be
 *      allocated
 *
 *      The space allocated for the edit text and static read/write text immediately follows
 *      the control array as part of the same memory block so it can all be freed by
 *      freeing the returned pointer.
 ******************************************************************************/
BOOL CtrlDataDup(LPCTRLINITDATA arCtrlDefaults, LPCTRLINITDATA PICFAR* ppCtrlData)
{
    LPSTR pszText;
    int nNum;
    int i;
    int nTextSize;
    
    if ( arCtrlDefaults == 0 )
        {
        *ppCtrlData = 0;
        return ( TRUE );
        }
        
    /* accumulate the total size of all read/write text fields
        edit and changeable static text */
    nTextSize = 0;
    for ( i = 0; arCtrlDefaults[i].eType != eEnd; i++ )
        {
        if ( arCtrlDefaults[i].eType == eEdit ||
             ( arCtrlDefaults[i].eType == eText && arCtrlDefaults[i].wLimitText != 0 ) )
            nTextSize += arCtrlDefaults[i].wLimitText + 1;
        }
    nNum = i + 1; /* have to allocate the eEnd entry also */

    *ppCtrlData = (LPCTRLINITDATA)GlobalAllocPtr(
        GMEM_MOVEABLE,
        nNum * sizeof(CTRLINITDATA) + nTextSize);
    if ( *ppCtrlData == 0 )
        return ( FALSE );

    /* copy control init data into dup instance */
    pszText = (LPSTR)&(*ppCtrlData)[nNum];
    /* pszText points past the control init data array to the start of the space allocated for
        edit text and read/write static text */
    for ( i = 0; i < nNum; i++ )
        {
        (*ppCtrlData)[i] = arCtrlDefaults[i];
        if ( arCtrlDefaults[i].eType == eEdit ||
             ( arCtrlDefaults[i].eType == eText && arCtrlDefaults[i].wLimitText != 0 ) )
            {
            (*ppCtrlData)[i].pszText = pszText;
            if ( arCtrlDefaults[i].pszText != 0 )
                lstrcpyn(pszText, arCtrlDefaults[i].pszText, arCtrlDefaults[i].wLimitText + 1);
            else
                pszText[0] = '\0';
            pszText += arCtrlDefaults[i].wLimitText + 1;
            }
        else if ( arCtrlDefaults[i].eType == eDialog )
            {
            if ( !CtrlDataDup(
                    (LPCTRLINITDATA)arCtrlDefaults[i].pszText,
                    (LPCTRLINITDATA PICFAR*)&(*ppCtrlData)[i].pszText) )
                {
                MiscGlobalFreePtr(ppCtrlData);
                return ( FALSE );
                }
            }            
        }
    return ( TRUE );
}



/******************************************************************************
 *  void CtrlDataWriteDefaults(
 *      const char *pszIniFilename,
 *      const char *pszSection,
 *      LPCTRLINITDATA ar)
 * 
 *  write control values to application ini file
 *
 *  parameters:
 *      pszIniFilename
 *      pszSection
 *      ar             - array of control data to be written
 *
 *  notes:
 *      Only written if key string is defined (ar[i].pszName) and only if the control
 *      is an edit, checkbox or radio button
 *
 *      eEdit && ar[i].u.bPassword will be written scrambled (simply) to the ini file
 * 
 *      checkboxes are written TRUE or FALSE
 *
 *      the pushed radio button only is written. The key is pszName and refers to
 *      all radio buttons in the group.  The value is ar[i].pszText (overloaded meaning
 *      with edit/static text) for the pushed radio button -- typically something
 *      like "Make Palette" or "25%"
 ******************************************************************************/
void CtrlDataWriteDefaults(LPCSTR pszIniFilename, LPCSTR pszSection, LPCTRLINITDATA ar)
{
    int i;
    char sz[_MAX_PATH + 1];
        
    for ( i = 0; ar[i].eType != eEnd; i++ )
        {
        if ( ar[i].pszName != 0 )
            {
            switch ( ar[i].eType )
                {
                case eRadioButton:
                    assert(ar[i].pszText != 0);            
                    if ( ar[i].u.bPushed )
                        WritePrivateProfileString(
                            pszSection,
                            ar[i].pszName,
                            ar[i].pszText,
                            pszIniFilename);
                    break;                        

                case eEdit:
                    if ( ar[i].u.bPassword )
                        ScramblePassword(sz, ar[i].pszText, ar[i].wLimitText);
                    else
                        lstrcpy(sz, ar[i].pszText);
                    WritePrivateProfileString(pszSection, ar[i].pszName, sz, pszIniFilename);
                    break;

                case eCheckBox:
                    if ( ar[i].u.bPushed )
                        WritePrivateProfileString(
                            pszSection,
                            ar[i].pszName,
                            "TRUE",
                            pszIniFilename);
                    else                            
                        WritePrivateProfileString(
                            pszSection,
                            ar[i].pszName,
                            "FALSE",
                            pszIniFilename);
                    break;

                case eDialog:
                    CtrlDataWriteDefaults(
                        pszIniFilename,
                        ar[i].pszName,
                        (LPCTRLINITDATA)ar[i].pszText);
                    break;

                default:
                    assert(FALSE);
                    break;
                }
            }                 
        
        }
}



/******************************************************************************
 *  void CtrlDataReadDefaults(
 *      const char *pszIniFilename,
 *      const char *pszSection,
 *      LPCTRLINITDATA ar,
 *      LPCTRLINITDATA PICFAR *ppCtrlData)
 * 
 *  create a duplicate control value instance and initialize from the application ini file
 *
 *  parameters:
 *      pszIniFilename
 *      pszSection
 *      ar             - array of control data to be initialized from ini file
 *
 *  notes:
 *      arCtrlDefaults identifies entries to be read from the ini file, but also specifies
 *      defaults in case the ini file doesn't exist or some value is missing
 *
 *      ( eEdit && arCtrlDefaults[i].u.bPassword ) means the password is scrambled in the
 *      ini file, so is unscrambled here
 * 
 *      checkboxes are read TRUE if "TRUE", "ON" or not 0, otherwise they are FALSE
 *
 *      the pushed radio button only is read. The key is pszName and refers to
 *      all radio buttons in the group.  The value is ar[i].pszText (overloaded meaning
 *      with edit/static text) for the pushed radio button -- typically something
 *      like "Make Palette" or "25%"
 ******************************************************************************/
void CtrlDataReadDefaults(LPCSTR pszIniFilename, LPCSTR pszSection, LPCTRLINITDATA arCtrlDefaults)
{
    char sz[_MAX_PATH + 1];
    int len;
    int i;
    
    /* read defaults from the application ini file */
    for ( i = 0; arCtrlDefaults[i].eType != eEnd; i++ )
        {
        len = 0;
        if ( arCtrlDefaults[i].pszName != 0 &&
             ( arCtrlDefaults[i].eType == eDialog ||
               ( len = GetPrivateProfileString(
                  pszSection,
                  arCtrlDefaults[i].pszName,
                  "",
                  sz,
                  sizeof(sz),
                  pszIniFilename) ) != 0 ) )
            {
            assert(len < sizeof(sz) - 1);
            switch ( arCtrlDefaults[i].eType )
                {
                case eRadioButton:
                    if ( lstrcmpi(sz, arCtrlDefaults[i].pszText) == 0 )
                        arCtrlDefaults[i].u.bPushed = TRUE;
                    else
                        arCtrlDefaults[i].u.bPushed = FALSE;
                    break;                        

                case eEdit:
                    if ( arCtrlDefaults[i].u.bPassword )
                        {
                        arCtrlDefaults[i].pszText[arCtrlDefaults[i].wLimitText] = '\0';
                        UnScramblePassword(
                            arCtrlDefaults[i].pszText,
                            sz,
                            arCtrlDefaults[i].wLimitText);
                        }
                    else
                        lstrcpyn(arCtrlDefaults[i].pszText, sz, arCtrlDefaults[i].wLimitText + 1);
                    break;

                case eCheckBox:
                    if ( lstrcmpi(sz, "TRUE") == 0 ||
                         lstrcmpi(sz, "ON") == 0 || 
                         atoi(sz) != 0 )
                        arCtrlDefaults[i].u.bPushed = TRUE;
                    else
                        arCtrlDefaults[i].u.bPushed = FALSE;                        
                    break;

                case eDialog:
                    CtrlDataReadDefaults(
                        pszIniFilename,
                        arCtrlDefaults[i].pszName,
                        (LPCTRLINITDATA)arCtrlDefaults[i].pszText);
                    break;

                default:
                    assert(FALSE);
                    break;
                }
            }                 
        
        }
}



/******************************************************************************
 *  void CtrlDataFree(LPCTRLINITDATA *parCtrlData)
 * 
 *  free dup'ed ctrl data
 *
 *  parameters:
 *      parCtrlData - pointer to array of control data
 ******************************************************************************/
void CtrlDataFree(LPCTRLINITDATA PICFAR* parCtrlData)
{
    int i;
    
    if ( *parCtrlData == 0 )
        return;

    for ( i = 0; (*parCtrlData)[i].eType != eEnd; i++ )
        {
        if ( (*parCtrlData)[i].eType == eDialog )
            CtrlDataFree((LPCTRLINITDATA PICFAR*)&(*parCtrlData)[i].pszText);
        }
    MiscGlobalFreePtr(parCtrlData);
}



/******************************************************************************
 *  BOOL CtrlDataIsPushed(LPCTRLINITDATA arCtrlData, int nID)
 * 
 *  return TRUE if the control is pushed
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *
 *  returns:
 *      TRUE if pushed
 ******************************************************************************/
BOOL CtrlDataIsPushed(LPCTRLINITDATA arCtrlData, int nID)
{
    LPCTRLINITDATA pCtrl = CtrlData(arCtrlData, nID);
    assert(pCtrl != NULL && ( pCtrl->eType == eCheckBox || pCtrl->eType == eRadioButton ));
    return ( pCtrl->u.bPushed );
}



/******************************************************************************
 *  BOOL CtrlDataSetInactive(LPCTRLINITDATA arCtrlData, int nID)
 * 
 *  set the control as inactive
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *
 *  notes:
 *      An inactive control is greyed during WM_INITDIALOG processing and
 *      won't be enabled by CtrlEnable.  This is used, for example, by
 *      sequential jpeg expand when opening an image.  If the image has no
 *      palette, then the "Embedded Palette" radio button is set inactive.
 *      If the image is gray scale, then all the Color Palette controls are
 *      set inactive.
 ******************************************************************************/
void CtrlDataSetInactive(LPCTRLINITDATA arCtrlData, int nID)
{
    LPCTRLINITDATA pCtrl = CtrlData(arCtrlData, nID);
    assert(pCtrl != NULL);
    pCtrl->bInactive = TRUE;
}



/******************************************************************************
 *  BOOL CtrlDataIsInactive(LPCTRLINITDATA arCtrlData, int nID)
 * 
 *  return TRUE if the control is inactive
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *
 *  returns:
 *      TRUE if inactive
 *
 *  notes:
 *      Currently this is only used locally to mdlgctrl.c, but it just seems like
 *      some opcode may be able to use this status to hold some image state so
 *      it's global
 *
 *      If the control isn't in the control array, then it's definitely not
 *      inactive, so that's an acceptable condition
 ******************************************************************************/
BOOL CtrlDataIsInactive(LPCTRLINITDATA arCtrlData, int nID)
{
    LPCTRLINITDATA pCtrl = CtrlData(arCtrlData, nID);
    return ( pCtrl != 0 && pCtrl->bInactive );
}



/******************************************************************************
 *  void CtrlEnable(HWND hwndDlg, LPCTRLINITDATA arCtrlData, int nID, BOOL bEnable)
 * 
 *  Enable/Disable a control if it's not inactive
 *
 *  parameters:
 *      hwndDlg    - HWND for dialog window
 *      nID        - control id
 *      arCtrlData - array of control data
 *      bEnable    - TRUE to enable, else FALSE to gray
 ******************************************************************************/
void CtrlEnable(HWND hwndDlg, LPCTRLINITDATA arCtrlData, int nID, BOOL bEnabled)
{
    EnableWindow(
        GetDlgItem(hwndDlg, nID),
        bEnabled && !CtrlDataIsInactive(arCtrlData, nID));
}



/******************************************************************************
 *  BOOL CtrlIsEnabled(HWND hwndDlg, int nID)
 * 
 *  return TRUE if a control's window is enabled
 *
 *  parameters:
 *      hwndDlg    - HWND for dialog window
 *      nID        - control id
 ******************************************************************************/
BOOL CtrlIsEnabled(HWND hwndDlg, int nID)
{
    return ( IsWindowEnabled(GetDlgItem(hwndDlg, nID)) );
}



/******************************************************************************
 *  BOOL CtrlDataSetPushed(LPCTRLINITDATA arCtrlData, int nID, BOOL bPushed)
 * 
 *  set the control as pushed or not
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *      bPushed    - TRUE to push it, else FALSE
 *
 *  notes:
 *      if a radio button is specified, this code turns off other radio buttons
 *      in the group using ar[i].pszName to find the other radio buttons
 ******************************************************************************/
void CtrlDataSetPushed(LPCTRLINITDATA arCtrlData, int nID, BOOL bPushed)
{
    int i;
    LPCTRLINITDATA pCtrl = CtrlData(arCtrlData, nID);

    assert(pCtrl != NULL && ( pCtrl->eType == eCheckBox || pCtrl->eType == eRadioButton ));
 
    if ( pCtrl->eType == eRadioButton && !pCtrl->u.bPushed && bPushed )
        {   /* have to clear any other radio buttons in the group */
        for ( i = 0; arCtrlData[i].eType != eEnd; i++ )
            {
            if ( arCtrlData[i].eType == eRadioButton &&
                 arCtrlData[i].nID != nID &&
                 lstrcmp(arCtrlData[i].pszName, pCtrl->pszName) == 0 )
                arCtrlData[i].u.bPushed = FALSE;
            }
        }
    pCtrl->u.bPushed = bPushed;
}



/******************************************************************************
 *  int CtrlDataGetText(LPCTRLINITDATA arCtrlData, int nID, LPSTR pszText, int nSize)
 * 
 *  retrieve the text for the control
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *      pszText    - buffer for text
 *      nSize      - size of buffer
 *
 *  returns:
 *      length of string copied to the buffer (at most nSize - 1)
 *
 *      ...GetText can be called for a static text control if wLimitText != 0
 *      although minerva isn't doing that yet
 ******************************************************************************/
int  CtrlDataGetText(LPCTRLINITDATA arCtrlData, int nID, LPSTR pszText, int nSize)
{
    LPCTRLINITDATA pCtrl = CtrlData(arCtrlData, nID);

    assert(pCtrl != NULL &&
           ( pCtrl->eType == eEdit || ( pCtrl->eType == eText && pCtrl->wLimitText != 0 )));
    assert(pszText != NULL && nSize > 1);
    lstrcpyn(pszText, pCtrl->pszText, nSize);
    return ( lstrlen(pszText) );
}



/******************************************************************************
 *  void CtrlDataSetText(LPCTRLINITDATA arCtrlData, int nID, LPSTR pszText)
 * 
 *  set the text for the edit or static text control
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *      pszText    - text to set or NULL (equivalent to "")
 *
 *  returns:
 *      ...SetText can be called for a static text control if wLimitText != 0
 *      allowing Properties dialog values to be set
 ******************************************************************************/
void CtrlDataSetText(LPCTRLINITDATA arCtrlData, int nID, LPCSTR pszText)
{
    LPCTRLINITDATA pCtrl = CtrlData(arCtrlData, nID);

    assert(pCtrl != NULL &&
        ( pCtrl->eType == eEdit || ( pCtrl->eType == eText && pCtrl->wLimitText != 0 )));
    if ( pszText == NULL )
        *pCtrl->pszText = '\0';
    else
        {        
        assert((WORD)lstrlen(pszText) <= pCtrl->wLimitText);
        lstrcpyn(pCtrl->pszText, pszText, min((WORD)lstrlen(pszText), pCtrl->wLimitText) + 1);
        }
}



/******************************************************************************
 *  void CtrlDataSetLong(LPCTRLINITDATA arCtrlData, int nID, long lVal, BOOL bSigned)
 * 
 *  set the text for the edit or static text control to a numeric value formatted
 *  with commas
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *      lVal       - numeric value
 *      bSigned    - TRUE if lVal is signed
 ******************************************************************************/
void CtrlDataSetLong(LPCTRLINITDATA arCtrlData, int nID, long lVal, BOOL bSigned)
{
    char sz[sizeof("-4,000,000,000")];
    
    if ( bSigned )
        MiscLtoaWithCommas(lVal, sz, sizeof(sz));
    else
        MiscUltoaWithCommas(lVal, sz, sizeof(sz));        
    CtrlDataSetText(arCtrlData, nID, sz);
}



/******************************************************************************
 *  long CtrlDataGetLong(LPCTRLINITDATA arCtrlData, int nID, BOOL bSigned)
 * 
 *  get the numeric value for an edit or static text control where the
 *  text is possibly formatted with commas
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *      bSigned    - TRUE if lVal is signed
 *
 *  returns:
 *      numeric value
 ******************************************************************************/
long CtrlDataGetLong(LPCTRLINITDATA arCtrlData, int nID, BOOL bSigned)
{
    char sz[sizeof("-4,000,000,000")];
    char *psz;
    long lVal;
    
    CtrlDataGetText(arCtrlData, nID, sz, sizeof(sz));
    if ( sz[0] == '\0' )
        return ( 0 );

    /* evaluate a leading '-' if signed */
    if ( bSigned )
        {
        if ( sz[0] == '-' )
            strcpy(&sz[0], &sz[1]); /* and bSigned is still TRUE */
        else
            bSigned = FALSE;
        }
    /* note now the sense of bSigned is changed to indicate the sign of the result */
    
    /* delete any commas */
    while ( ( psz = strchr(sz, ',') ) != NULL )
        strcpy(psz, psz + 1);

    lVal = strtoul(sz, &psz, 10);
    assert(psz != sz && *psz == '\0');
    if ( bSigned )
        lVal = -lVal;
    return ( lVal );
}



/******************************************************************************
 *  int CtrlDataWhichRadio(LPCTRLINITDATA arCtrlData, int nID)
 * 
 *  given a radio button id, return the id of the pressed radio button in the
 *  same group
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - radio button control id
 *
 *  returns:
 *      pressed radio button id
 ******************************************************************************/
int CtrlDataWhichRadio(LPCTRLINITDATA arCtrlData, int nID)
{
    int i;
    int j;
    LPCTRLINITDATA pCtrl = CtrlData(arCtrlData, nID);
    assert(pCtrl != NULL && pCtrl->eType == eRadioButton);
    
    for ( i = 0; arCtrlData[i].eType != eEnd; i++ )
        {
        if ( arCtrlData[i].eType == eRadioButton &&
             arCtrlData[i].u.bPushed &&
             strcmp(arCtrlData[i].pszName, pCtrl->pszName) == 0 )
            {
            /* look through the remainder of the group as a data consistency check
                to make sure no other button is pushed */
            for ( j = i + 1; arCtrlData[j].eType != eEnd; j++ )
                assert(arCtrlData[j].eType != eRadioButton ||
                    !arCtrlData[j].u.bPushed ||
                    strcmp(arCtrlData[j].pszName, pCtrl->pszName) != 0);
            return ( arCtrlData[i].nID );
            }
        }    
    /* so far some button must always be pushed -- although I can forsee
        that sometime it would be useful if no button pushed was valid */
    assert(FALSE);
    return ( -1 );
}



/******************************************************************************
 *  LPCTRLINITDATA CtrlData(LPCTRLINITDATA arCtrlData, int nID)
 * 
 *  look up the control and return the control's data
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 ******************************************************************************/
LPCTRLINITDATA CtrlData(LPCTRLINITDATA arCtrlData, int nID)
{
    int i;
    
    for ( i = 0; arCtrlData[i].eType != eEnd; i++ )
        if ( arCtrlData[i].nID == nID )
            return ( &arCtrlData[i] );
    return ( NULL );
}



/******************************************************************************
 *  void CtrlDataSetLong(LPCTRLINITDATA arCtrlData, int nID, long lVal, int nFixed
 * 
 *  set the text for the edit or static text control to a signed, numeric fixed-point
 *  decimal value formatted with commas
 *
 *  parameters:
 *      arCtrlData - array of control data
 *      nID        - control id
 *      lVal       - numeric value
 *      nFixed     - number of digits right of decimal
 ******************************************************************************/
void CtrlDataSetDecFixed(LPCTRLINITDATA arCtrlData, int nID, long lVal, int nFixed)
{
    DWORD dwDiv = 1;
    int i;
    char sz[sizeof("-4,000,000,000")];
    char szDec[sizeof("000000000")];
    
    assert(nFixed <= 9);
    for ( i = 0; i < nFixed; i++ )
        dwDiv *= 10;

    MiscLtoaWithCommas(lVal / dwDiv, sz, sizeof(sz));
    if ( nFixed != 0 )
        {
        strcat(sz, ".");
        lVal = labs(lVal);
        wsprintf(szDec, "%.9lu", lVal % dwDiv);
        strcat(sz, &szDec[strlen(szDec) - nFixed]);
        }
    CtrlDataSetText(arCtrlData, nID, sz);
}



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



/******************************************************************************
 *  void ScramblePassword(LPSTR pszScrambled, LPCSTR pszPlainText, int wLenPlain)
 * 
 *  scramble password before writing to ini file
 *
 *  parameters:
 *      pszScrambled
 *      pszPlainText
 *      wLenPlain
 *
 *  notes:
 *      _Please_ use a better method for your own purposes.  This is here solely
 *      because I have a philosophical objection to writing passwords to files
 *      in plain text.  Clearly any algorithm which has to have an unscramble,
 *      and for which you have the source code, is not going to be very secure.
 ******************************************************************************/
static void ScramblePassword(LPSTR pszScrambled, LPCSTR pszPlainText, int wLenPlain)
{
    char sz[_MAX_PATH + 1];
    DWORD dw = MiscTickCount();
    LPDWORD pdw = (LPDWORD)sz;
    UINT i;
    UINT n;
    char *psz = DWORD_SCRAMBLER;
    
    if ( *pszPlainText == '\0' )
        {
        *pszScrambled = '\0';
        return;
        }
    assert(wLenPlain < sizeof(sz));
    memset(sz, '\0', sizeof(sz));
    strcpy(sz, psz);
    lstrcpy(&sz[strlen(sz)], pszPlainText);
    for ( i = 0; i < ( wLenPlain + strlen(psz) + sizeof(DWORD) - 1 ) / sizeof(DWORD); i++ )
        pdw[i] ^= dw;
    n = i * sizeof(DWORD);
    for ( i = 0; i < n; i++ )
        {
        *pszScrambled++ = (char)( (unsigned char)sz[i] / 10 + 'a' );
        *pszScrambled++ = (char)( (unsigned char)sz[i] % 10 + '0' );
        }
    *pszScrambled++ = '\0';
}



/******************************************************************************
 *  void ScramblePassword(LPSTR pszScrambled, LPCSTR pszPlainText, int wLenPlain)
 * 
 *  unscramble password after writing to ini file
 *
 *  parameters:
 *      pszPlainText
 *      pszScrambled
 *      wLenPlain
 *
 *  notes:
 *      _Please_ use a better method for your own purposes.  This is here solely
 *      because I have a philosophical objection to writing passwords to files
 *      in plain text.  Clearly any algorithm which has to have an unscramble,
 *      and for which you have the source code, is not going to be very secure.
 ******************************************************************************/
static void UnScramblePassword(LPSTR pszPlainText, LPCSTR pszScrambled, int wLenPlain)
{
    char sz[_MAX_PATH + 1];
    UINT n;
    UINT i;
    LPDWORD pdw = (LPDWORD)sz;
    char *psz = DWORD_SCRAMBLER;

    if ( *pszScrambled == '\0' )
        {
        *pszPlainText = '\0';
        return;
        }
    n = ( ( wLenPlain + strlen(psz) + sizeof(DWORD) - 1 ) / sizeof(DWORD) ) * sizeof(DWORD);
    for ( i = 0; i < n; i++ )
        {
        sz[i] = (char)( *pszScrambled++ - 'a' );
        sz[i] = (char)( sz[i] * 10 + *pszScrambled++ - '0' );
        }
    for ( i = 0; i < sizeof(DWORD); i++ )
        sz[i] ^= psz[i];
    for ( i = 1; i < n / sizeof(DWORD); i++ )
        pdw[i] ^= pdw[0];
    hmemcpy(pszPlainText, &sz[strlen(psz)], wLenPlain);
}
