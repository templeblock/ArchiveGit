/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                  initthrd.h

*****************************************************************************
*/

#ifndef _INITTHRD_H
#define _INITTHRD_H

/* ******************************* Public Data ***************************** */
/* ********************************* Constants ****************************** */
/* *************************** Class Declarations ************************** */
/* ******************************* Typedefs ******************************* */
/* ******************************* Classes ******************************** */

class CInitializationThread : public CWinThread
{
public:
	DECLARE_DYNAMIC(CInitializationThread)
	CInitializationThread(CPictPubApp *pPPApp);

// Attributes
public:

// Operations
public:

// Overrides

// Implementation
    ~CInitializationThread();
    BOOL StartThread();

    BOOL SetupAppMainMenuComplete(BOOL bWaitInfinite) ;
    BOOL SetupAppSharedMenuComplete(BOOL bWaitInfinite) ;
    BOOL HintLoadComplete(BOOL bWaitInfinite) ;
    BOOL CMSInitComplete(BOOL bWaitInfinite) ;
    BOOL TWAINInitComplete(BOOL bWaitInfinite) ;
    BOOL InitThreadComplete(BOOL bWaitInfinite) ;

protected:
	virtual BOOL InitInstance();

private:
    CPictPubApp *m_pPictPubApp ;
    HANDLE m_hSetupAppMainMenuEvent;
    HANDLE m_hSetupAppSharedMenuEvent;
    HANDLE m_hHintsLoadEvent;
    HANDLE m_hCMSEvent;
    HANDLE m_hTWAINEvent;
    HANDLE m_hInitThreadDoneEvent;

    BOOL m_bSetupAppMainMenuEvent;
    BOOL m_bSetupAppSharedMenuEvent;
    BOOL m_bHintsLoadEvent;
    BOOL m_bCMSEvent;
    BOOL m_bTWAINEvent;
    BOOL m_bInitThreadDoneEvent;
};

/* *************************** Inline Methods ****************************** */
/* *************************** Global Data ********************************* */
/* *************************** Global Function ***************************** */
/* ************************************************************************* */

#endif // _INITTHRD_H
