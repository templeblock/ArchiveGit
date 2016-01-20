#ifndef _MAPI_H_
#define _MAPI_H_

#include "smapi.h"

/* Message Box styles */
#define MBS_ERROR           (MB_ICONSTOP | MB_OK)

class CMapiMail
{
public:

	CMapiMail(HWND hParent,HINSTANCE hInstance);
	~CMapiMail();

	BOOL InitMail (BOOL bAutoLogon=TRUE);
	BOOL IsLoggedOn () {return (m_lhSession!=0);};
	BOOL Logon();
	BOOL Logoff();
	BOOL SendMail();

private:
	void MakeMessageBox (HWND hWnd, ULONG ulResult, UINT idString, UINT fStyle);
	BOOL InitSimpleMAPI ();

	// MAPI Functions
	LPFNMAPILOGON lpfnMAPILogon;
	LPFNMAPILOGOFF lpfnMAPILogoff;
	LPFNMAPISENDMAIL lpfnMAPISendMail;
	LPFNMAPISENDDOCUMENTS lpfnMAPISendDocuments;
	LPFNMAPIFINDNEXT lpfnMAPIFindNext;
	LPFNMAPIREADMAIL lpfnMAPIReadMail;
	LPFNMAPISAVEMAIL lpfnMAPISaveMail;
	LPFNMAPIDELETEMAIL lpfnMAPIDeleteMail;
	LPFNMAPIFREEBUFFER lpfnMAPIFreeBuffer;
	LPFNMAPIADDRESS lpfnMAPIAddress;
	LPFNMAPIDETAILS lpfnMAPIDetails;
	LPFNMAPIRESOLVENAME lpfnMAPIResolveName;

	LHANDLE m_lhSession;
	HINSTANCE m_hLibMail;
	HINSTANCE m_hInstance;
	HWND m_hParentWnd;
};

#endif //_MAPI_H_
