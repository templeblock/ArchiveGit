/* Simple MAPI functions */
#pragma once

#ifndef MAPI_H
#include <mapi.h>
#endif

typedef ULONG (FAR PASCAL *LPFNMAPILOGON)(ULONG ulUIParam, LPTSTR lpszProfileName,
    LPTSTR lpszPassword, FLAGS flFlags, ULONG ulReserved, LPLHANDLE lplhSession);

typedef ULONG (FAR PASCAL *LPFNMAPILOGOFF)(LHANDLE lhSession, ULONG ulUIParam, 
    FLAGS flFlags, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPISENDMAIL)(LHANDLE lhSession, ULONG ulUIParam,
    lpMapiMessage lpMessage, FLAGS flFlags, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPISENDDOCUMENTS)(ULONG ulUIParam, LPTSTR lpszDelimChar,
    LPTSTR lpszFilePaths, LPTSTR lpszFileNames, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPIFINDNEXT)(LHANDLE lhSession, ULONG ulUIParam,
    LPTSTR lpszMessageType, LPTSTR lpszSeedMessageID, FLAGS flFlags,
    ULONG ulReserved, LPTSTR lpszMessageID);

typedef ULONG (FAR PASCAL *LPFNMAPIREADMAIL)(LHANDLE lhSession, ULONG ulUIParam,
    LPTSTR lpszMessageID, FLAGS flFlags, ULONG ulReserved,
    lpMapiMessage FAR *lppMessage);

typedef ULONG (FAR PASCAL *LPFNMAPISAVEMAIL)(LHANDLE lhSession, ULONG ulUIParam,
    lpMapiMessage lpMessage, FLAGS flFlags, ULONG ulReserved,
    LPTSTR lpszMessageID);

typedef ULONG (FAR PASCAL *LPFNMAPIDELETEMAIL)(LHANDLE lhSession, ULONG ulUIParam,
    LPTSTR lpszMessageID, FLAGS flFlags, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPIFREEBUFFER)(LPVOID pv);

typedef ULONG (FAR PASCAL *LPFNMAPIADDRESS)(LHANDLE lhSession, ULONG ulUIParam,
    LPTSTR lpszCaption, ULONG nEditFields, LPTSTR lpszLabels, ULONG nRecips,
    lpMapiRecipDesc lpRecips, FLAGS flFlags, ULONG ulReserved,
    LPULONG lpnNewRecips, lpMapiRecipDesc FAR *lppNewRecips);

typedef ULONG (FAR PASCAL *LPFNMAPIDETAILS)(LHANDLE lhSession, ULONG ulUIParam,
    lpMapiRecipDesc lpRecip, FLAGS flFlags, ULONG ulReserved);

typedef ULONG (FAR PASCAL *LPFNMAPIRESOLVENAME)(LHANDLE lhSession, ULONG ulUIParam,
    LPTSTR lpszName, FLAGS flFlags, ULONG ulReserved,
    lpMapiRecipDesc FAR *lppRecip);

extern LPFNMAPILOGON lpfnMAPILogon;
extern LPFNMAPILOGOFF lpfnMAPILogoff;
extern LPFNMAPISENDMAIL lpfnMAPISendMail;
extern LPFNMAPISENDDOCUMENTS lpfnMAPISendDocuments;
extern LPFNMAPIFINDNEXT lpfnMAPIFindNext;
extern LPFNMAPIREADMAIL lpfnMAPIReadMail;
extern LPFNMAPISAVEMAIL lpfnMAPISaveMail;
extern LPFNMAPIDELETEMAIL lpfnMAPIDeleteMail;
extern LPFNMAPIFREEBUFFER lpfnMAPIFreeBuffer;
extern LPFNMAPIADDRESS lpfnMAPIAddress;
extern LPFNMAPIDETAILS lpfnMAPIDetails;
extern LPFNMAPIRESOLVENAME lpfnMAPIResolveName;

#undef MAPILogon
#undef MAPILogoff
#undef MAPISendMail
#undef MAPISendDocuments
#undef MAPIFindNext
#undef MAPIReadMail
#undef MAPISaveMail
#undef MAPIDeleteMail
#undef MAPIFreeBuffer
#undef MAPIAddress
#undef MAPIDetails
#undef MAPIResolveName
#define MAPILogon            (*lpfnMAPILogon)
#define MAPILogoff            (*lpfnMAPILogoff)
#define MAPISendMail        (*lpfnMAPISendMail)
#define MAPISendDocuments    (*lpfnMAPISendDocuments)
#define MAPIFindNext        (*lpfnMAPIFindNext)
#define MAPIReadMail        (*lpfnMAPIReadMail)
#define MAPISaveMail        (*lpfnMAPISaveMail)
#define MAPIDeleteMail        (*lpfnMAPIDeleteMail)
#define MAPIFreeBuffer        (*lpfnMAPIFreeBuffer)
#define MAPIAddress            (*lpfnMAPIAddress)
#define MAPIDetails            (*lpfnMAPIDetails)
#define MAPIResolveName        (*lpfnMAPIResolveName)
