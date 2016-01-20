/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                dlgscan.h

*****************************************************************************
*/

#ifndef _DLGSCAN_H
#define _DLGSCAN_H

#include "ppmodal.h"

/* ****************************** Public Data ***************************** */
/* ****************************** Constants ******************************* */
/* ****************************** Typedefs ******************************** */
/* ****************************** Classes ********************************* */
class CScanSetupDlg : public CPPModalDlg
{
public:
        
    CScanSetupDlg( CWnd* pParentWnd = NULL );

	virtual int DoModal();
  	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual BOOL PreDoModal(LPVOID lpVoid);
    virtual LPVOID PostDoModal();

protected:
    void ScanSetup_Update();
    void SetTwainStrings();

private:
    UINT m_idListToManage;
    BOOL m_fScanUseCMS;
    BOOL m_fCMSEnabled;
    FNAME m_ScanMap;
    BOOL m_bTwainOK;

    DLGSECTION m_scansec;

};

#endif // _DLGSCAN_H_

