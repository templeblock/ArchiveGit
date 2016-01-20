//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include <process.h>

#ifdef WIN32
typedef struct _thread_parms
	{
	RECT			rArea;
	HANDLE			hEvent;
	CWinThread	   	*pThread;
	HANDLE			hThread;
	BOOL			fRet;
	THREADPROC		ThreadProc;	
	LPVOID			lpParms;
	} THREAD_PARMS, FAR *LPTHREAD_PARMS;

LOCAL UINT ThreadHandler(LPVOID lpParam);
#endif

/***********************************************************************/
//
// FUNCTION:	SplitThreads
//
// PURPOSE:	To split the processing of an area into multiple threads
//
// PARAMETERS:	
//				nThreads	-	number of threads to split the processing into
//								if nThreads is 0, the function will split
//								the processing based on the number of
//								processors present
//
//				ThreadProc	-	the proc to be called
//								by each thread for processing
//
//				ThreadGetParms- the proc to be called, before any threads
//								get created, to modify or copy the lpParms
//								parameter.  The value returned by this
//								function is passed to ThreadProc() instead
//								of lpParms.  This is where any per thread
//								allocations should get done.  Because, if
//								the allocations fail, the processing will
//								still happen with the calling thread.
//			
//				ThreadFreeParms-the proc called to freeup any memory
//								allocated by ThreadGetParms()
//
//				lpParms			- the parameter to be passed to ThreadProc()
//
//				lpArea			- the area to be divided among the threads
//
// RETURNS:	returns values returned from ThreadProc().  If any thread
//				returns FALSE SplitThread() returns FALSE.
//
// COMMENTS:
//				ThreadGetParms and ThreadFreeParms only get called for
// 			new threads that get created.  Only (nThreads-1) new threads
//				are created.  The calling thread which is used for processing
//				the last area always gets passed the lpParms that was
//				passed into SplitThreads().  You need to be carefull about
//				per thread memory allocations.  Processing should always
//				be able to continue at this level so that enough allocation
//				for one thread should be done before calling SplitThreads().
//
/***********************************************************************/
BOOL SplitThreads(	int nThreads,
					THREADPROC ThreadProc,
					THREADGETPARMSPROC ThreadGetParms,
					THREADFREEPARMSPROC ThreadFreeParms,
					LPVOID lpParms,
					LPRECT lpArea)
{
BOOL fRet;
// save local copy of area for use to mess with
RECT rArea = *lpArea;

#ifdef WIN32
int dy, i, n, j;
DWORD dwExit;
BOOL fDone, fUseThreads;
LPTHREAD_PARMS lpThreadParms;

nThreads = SuggestThreads(nThreads);
fUseThreads = (nThreads > 1);

if (fUseThreads && (RectHeight(&rArea) / nThreads) == 0)
	fUseThreads = FALSE;

// allocate thread structure for n-1 threads, cause this thread
// will be the nth thread
if (fUseThreads)
	{
	lpThreadParms = (LPTHREAD_PARMS)Alloc(sizeof(THREAD_PARMS)*(nThreads-1));
	if (!lpThreadParms)
		fUseThreads = FALSE;
	}

// if still cool for threading, setup up areas for threads
// and parameters
if (fUseThreads)
	{
	dy = RectHeight(&rArea);
	i = 0;
	n = nThreads;
	for (i = 0; i < (nThreads-1); ++i)
		{
		dy = RectHeight(&rArea) / n;
		lpThreadParms[i].rArea = rArea;
		lpThreadParms[i].rArea.bottom = lpThreadParms[i].rArea.top + dy - 1;
		rArea.top = lpThreadParms[i].rArea.bottom + 1;
		lpThreadParms[i].ThreadProc = ThreadProc;
		if (ThreadGetParms)
			{
			lpThreadParms[i].lpParms = (*ThreadGetParms)(lpParms, 
										&lpThreadParms[i].rArea);
			if (!lpThreadParms[i].lpParms)
				{
				if (ThreadFreeParms)
					{
					for (j = 0; j < i; ++j)
						(*ThreadFreeParms)(lpThreadParms[j].lpParms);
					}
				rArea = *lpArea;
				fUseThreads = FALSE;
				break;
				}
			}
		else
			lpThreadParms[i].lpParms = lpParms;
		--n;
		}
	}
// if still cool for threading, creates the threads and kick 'em off
if (fUseThreads)
	{
	for (i = 0; i < (nThreads-1); ++i)
		{
		lpThreadParms[i].hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		// someday need to handle thread creation error!!!
		lpThreadParms[i].pThread = AfxBeginThread(ThreadHandler, (LPVOID)&lpThreadParms[i], THREAD_PRIORITY_BELOW_NORMAL);
	 	lpThreadParms[i].hThread = lpThreadParms[i].pThread->m_hThread;
		}
	// kick 'em off after we lower their priority
	for (i = 0; i < (nThreads-1); ++i)
		SetEvent(lpThreadParms[i].hEvent);
	}

// let this thread do it's job with whatever rectangle is left
fRet = (*ThreadProc)(lpParms, &rArea);

// if using threading, wait until all threads are done
if (fUseThreads)
	{
	fDone = NO;
	while (!fDone)
		{
		fDone = YES;
		for (i = 0; i < (nThreads-1); ++i)
			{
			// if this call fails or the thread isn't STILL_ACTIVE
			// the thread has finished it's job with the result
			// in lpThreadParms[i].fRet
			if (GetExitCodeThread(lpThreadParms[i].hThread, &dwExit))
				{
				if (dwExit == STILL_ACTIVE)
					{
					fDone = NO;
					break;
					}
				}
			}
		// if other threads are not done, let them have the processor
		if (!fDone) 
			Sleep(10);
		}
	// clean up thread stuff and see if any thread failed
	for (i = 0; i < (nThreads-1); ++i)
		{
		if (!lpThreadParms[i].fRet)
			fRet = FALSE;
		CloseHandle(lpThreadParms[i].hEvent);
		if (ThreadFreeParms)
			(*ThreadFreeParms)(lpThreadParms[i].lpParms);
		}
	// get rid of our memory
	FreeUp(lpThreadParms);
	}
#else
// let this thread do it's job with whatever rectangle is left
fRet = (*ThreadProc)(lpParms, &rArea);
#endif
// return status
return(fRet);
}

#ifdef WIN32
/***********************************************************************/
LOCAL UINT ThreadHandler(LPVOID lpParam)
/***********************************************************************/
{
LPTHREAD_PARMS lpThreadParms;

lpThreadParms = (LPTHREAD_PARMS)lpParam;
WaitForSingleObject(lpThreadParms->hEvent, INFINITE);

lpThreadParms->fRet = (*lpThreadParms->ThreadProc)(
								lpThreadParms->lpParms,
								&lpThreadParms->rArea);
return(0);
}
#endif

/***********************************************************************/
//
// FUNCTION:	SuggestThreads
//
// PURPOSE:	To suggest the number of threads for processing a single operation
//
// PARAMETERS:	
//				nThreads	-	number of desired threads to split the
//								processing into	if nThreads is 0, the 
//								function will split	the processing based 
//								on the number of processors present.
//
// RETURNS:		If threading is allowed then it returns the suggested 
//					number of threads.
//				If per-process threading is not allowed then it returns 0.
//
/***********************************************************************/
int SuggestThreads(int nThreads)
//***********************************************************************
{
#ifdef WIN32
BOOL fUseThreads;

if (nThreads <= 0)
	nThreads = SystemInfo.dwNumberOfProcessors;
if (nThreads <= 0)
	nThreads = 1;
fUseThreads = Control.UseFunctionThreading && Control.UseThreading;
if (!fUseThreads)
	return(0);
else
	return(nThreads);
#else
return(0);
#endif
}

