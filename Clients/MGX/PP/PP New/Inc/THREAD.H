//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef _THREAD_H_
#define _THREAD_H_

#include "types.h"

typedef BOOL (FAR *THREADPROC)(LPVOID lpParms, LPRECT lpArea);
typedef LPVOID (FAR *THREADGETPARMSPROC)(LPVOID lpParms, LPRECT lpArea);
typedef void (FAR *THREADFREEPARMSPROC)(LPVOID lpParms);

BOOL SplitThreads(int nThreads, THREADPROC ThreadProc, THREADGETPARMSPROC ThreadGetParms, THREADFREEPARMSPROC ThreadFreeParms, LPVOID lpParms, LPRECT lpArea);
int SuggestThreads(int nThreads);

#endif // _THREAD_H_
