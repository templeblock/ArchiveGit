#include "stdafx.h"
#include "resource.h"
#include "MapiMail.h"
#include <mapix.h>

/////////////////////////////////////////////////////////////////////////////
CMapiMail::CMapiMail(HWND hParent,HINSTANCE hInstance)
{
	_ASSERT(hInstance!=NULL);
	_ASSERT(hParent!=NULL);

	m_hInstance = hInstance;
	m_hParentWnd = hParent;

	// MAPI Functions
	lpfnMAPILogon = NULL;
	lpfnMAPILogoff = NULL;
	lpfnMAPISendMail = NULL;
	lpfnMAPISendDocuments = NULL;
	lpfnMAPIFindNext = NULL;
	lpfnMAPIReadMail = NULL;
	lpfnMAPISaveMail = NULL;
	lpfnMAPIDeleteMail = NULL;
	lpfnMAPIFreeBuffer = NULL;
	lpfnMAPIAddress = NULL;
	lpfnMAPIDetails = NULL;
	lpfnMAPIResolveName = NULL;

	m_lhSession = 0L;
	m_hLibMail = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CMapiMail::~CMapiMail()
{
	Logoff();

    if (m_hLibMail)
	{
        FreeLibrary (m_hLibMail);
        m_hLibMail = 0;
    }
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMapiMail::InitMail(BOOL bAutoLogon/*TRUE*/)
{
    BOOL fInit;

    if (fInit = InitSimpleMAPI ())
	{
		if (bAutoLogon)
        	fInit = Logon();
    }
    return (fInit);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMapiMail::InitSimpleMAPI()
{
    UINT fuError;

    fuError = SetErrorMode(SEM_NOOPENFILEERRORBOX);
#ifdef WIN32
   m_hLibMail = LoadLibrary("MAPI32.DLL");
#else
   m_hLibMail = LoadLibrary("MAPI.DLL");
#endif
    SetErrorMode(fuError);

#ifdef WIN32
    if (!m_hLibMail)
#else
    if (m_hLibMail < 32)
#endif
        return (FALSE);

	// Get the Proc Addresses for the MAPI ENTRIES

    if (!(lpfnMAPILogon = (LPFNMAPILOGON) GetProcAddress (m_hLibMail, "MAPILogon")))
        return (FALSE);
    if (!(lpfnMAPILogoff = (LPFNMAPILOGOFF) GetProcAddress (m_hLibMail, "MAPILogoff")))
        return (FALSE);
    if (!(lpfnMAPISendMail = (LPFNMAPISENDMAIL) GetProcAddress (m_hLibMail, "MAPISendMail")))
        return (FALSE);
    if (!(lpfnMAPISendDocuments = (LPFNMAPISENDDOCUMENTS) GetProcAddress (m_hLibMail, "MAPISendDocuments")))
        return (FALSE);
    if (!(lpfnMAPIFindNext = (LPFNMAPIFINDNEXT) GetProcAddress (m_hLibMail, "MAPIFindNext")))
        return (FALSE);
    if (!(lpfnMAPIReadMail = (LPFNMAPIREADMAIL) GetProcAddress (m_hLibMail, "MAPIReadMail")))
        return (FALSE);
    if (!(lpfnMAPISaveMail = (LPFNMAPISAVEMAIL) GetProcAddress (m_hLibMail, "MAPISaveMail")))
        return (FALSE);
    if (!(lpfnMAPIDeleteMail = (LPFNMAPIDELETEMAIL) GetProcAddress (m_hLibMail, "MAPIDeleteMail")))
        return (FALSE);
    if (!(lpfnMAPIFreeBuffer = (LPFNMAPIFREEBUFFER) GetProcAddress (m_hLibMail, "MAPIFreeBuffer")))
        return (FALSE);
    if (!(lpfnMAPIAddress = (LPFNMAPIADDRESS) GetProcAddress (m_hLibMail, "MAPIAddress")))
        return (FALSE);
    if (!(lpfnMAPIDetails = (LPFNMAPIDETAILS) GetProcAddress (m_hLibMail, "MAPIDetails")))
        return (FALSE);
    if (!(lpfnMAPIResolveName = (LPFNMAPIRESOLVENAME) GetProcAddress (m_hLibMail, "MAPIResolveName")))
        return (FALSE);

	// Get the Proc Addresses for the CMC ENTRIES

    return (TRUE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMapiMail::Logon()
{
	_ASSERT(m_hLibMail != NULL);

	if (!m_lhSession)
	{
	    ULONG ulResult;

        if ((ulResult = MAPILogon ((ULONG) m_hParentWnd, NULL, NULL,
                    MAPI_LOGON_UI | MAPI_NEW_SESSION | MAPI_EXTENDED /*| MAPI_ALLOW_OTHERS*/,
                    0, &m_lhSession)) != SUCCESS_SUCCESS)
		{
            m_lhSession = 0;
            MakeMessageBox (m_hParentWnd, ulResult, IDS_PROJNAME, MBS_ERROR);
        }
    }
	return (m_lhSession != 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMapiMail::Logoff()
{
	_ASSERT(m_hLibMail != NULL);

	if (m_lhSession)
	{
    	MAPILogoff (m_lhSession, (ULONG) m_hParentWnd, 0, 0);
        m_lhSession = 0;
    }
	return (m_lhSession == 0);
}

/////////////////////////////////////////////////////////////////////////////
void CMapiMail::MakeMessageBox (HWND m_hParentWnd, ULONG ulResult, UINT idString, UINT fStyle)
{
    char szMessage[256];
    char szMapiReturn[64];

    LoadString (m_hInstance, idString, szMessage, 255);

    if (ulResult)
	{
        LoadString (m_hInstance, (UINT)ulResult, szMapiReturn, 64);
        lstrcat (szMessage, "\nReturn Code: ");
        lstrcat (szMessage, szMapiReturn);
    }

    MessageBox (m_hParentWnd, szMessage, "Problem", fStyle);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMapiMail::SendMail()
{
	_ASSERT(m_hLibMail != NULL);

	BOOL bRet = FALSE;

	if(m_lhSession)
	{
	    MapiMessage msgSend;
	    memset(&msgSend, 0, sizeof(MapiMessage));
		msgSend.lpszSubject = "My Subject";		// LPSTR Message Subject
		msgSend.lpszNoteText = "<html><body><font size=12>Test</font></body></html>";		// LPSTR Message Text
//		msgSend.lpszMessageType;	// LPSTR Message Class
//		msgSend.lpszDateReceived;	// LPSTR in YYYY/MM/DD HH:MM format
//		msgSend.lpszConversationID;	// LPSTR conversation thread ID
//		msgSend.flFlags;			// FLAGS unread,return receipt
//		msgSend.lpOriginator;		// lpMapiRecipDesc Originator descriptor
//		msgSend.nRecipCount;		// ULONG Number of recipients
//		msgSend.lpRecips;			// lpMapiRecipDesc Recipient descriptors
//		msgSend.nFileCount;			// ULONG # of file attachments
//		msgSend.lpFiles;			// lpMapiFileDesc Attachment descriptors
	
	    MAPISendMail(m_lhSession, (ULONG)m_hParentWnd, &msgSend, MAPI_DIALOG, 0L);
		bRet = TRUE;
	} // PR_RTF_COMPRESSED

	return bRet;
}
