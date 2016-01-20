//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "list.h"

/***********************************************************************/
void ListAddTail(LPLIST lpList, LPVOID lpNewNode)
/***********************************************************************/
{
ListInsertAfter(lpList, lpList->Link.lpPrev, lpNewNode);
}

/***********************************************************************/
void ListAddHead(LPLIST lpList, LPVOID lpNewNode)
/***********************************************************************/
{
ListInsertBefore(lpList, lpList->Link.lpNext, lpNewNode);
}

/***********************************************************************/
void ListInsertAfter(LPLIST lpList, LPVOID lpAfterParam,
						LPVOID lpNewNodeParam)
/***********************************************************************/
{
LPLISTNODE lpAfter = (LPLISTNODE)lpAfterParam;
LPLISTNODE lpNewNode = (LPLISTNODE)lpNewNodeParam;
LPLISTNODE lpAfterNext;

// if no after passed in, link in at end of list
if (!lpAfter)
	lpAfter = lpList->Link.lpPrev;

// if we have a node to link after, do it to it
if (lpAfter)
	{
	// link into node that it's supposed to be after
	// save what after points to for linking in later
	lpNewNode->Link.lpPrev = lpAfter;
	lpAfterNext = lpAfter->Link.lpNext;
	lpAfter->Link.lpNext = lpNewNode;

	// assume node passed in may after other nodes linked after
	while (lpNewNode->Link.lpNext)
		lpNewNode = lpNewNode->Link.lpNext;

	// link in what after pointed to
	lpNewNode->Link.lpNext = lpAfterNext;

	// link what after pointed to back to new node list
	if (lpAfterNext)
		lpAfterNext->Link.lpPrev = lpNewNode;
	else
		lpList->Link.lpPrev = lpNewNode;
	}
else
	{
	// if still NULL, list is empty, add new element
	lpList->Link.lpNext = lpNewNode;
	// assume node passed in may after other nodes linked after
	// so find last node in list and setup tail pointer
	while (lpNewNode->Link.lpNext)
		lpNewNode = lpNewNode->Link.lpNext;
	lpList->Link.lpPrev = lpNewNode;
	}
}

/***********************************************************************/
void ListInsertBefore(LPLIST lpList, LPVOID lpBeforeParam,
						LPVOID lpNewNodeParam)
/***********************************************************************/
{
LPLISTNODE lpBefore = (LPLISTNODE)lpBeforeParam;
LPLISTNODE lpNewNode = (LPLISTNODE)lpNewNodeParam;
LPLISTNODE lpBeforePrev;

// if no before no passed in link in at head of list
if (!lpBefore)
	lpBefore = lpList->Link.lpNext;

// if we have a node to link before, do it to it
if (lpBefore)
	{
	// point new node to to what was before the before node
	lpBeforePrev = lpBefore->Link.lpPrev;
	lpNewNode->Link.lpPrev = lpBeforePrev;
	if (lpBeforePrev)
		lpBeforePrev->Link.lpNext = lpNewNode;
	else
		lpList->Link.lpNext = lpNewNode;

	// assume node passed in may after other nodes linked after
	while (lpNewNode->Link.lpNext)
		lpNewNode = lpNewNode->Link.lpNext;

	// link end of our list to before node
	lpNewNode->Link.lpNext = lpBefore;
	lpBefore->Link.lpPrev = lpNewNode;
	}
else
	{
	// if still NULL, list is empty, add new element
	lpList->Link.lpNext = lpNewNode;
	// assume node passed in may after other nodes linked after
	// so find last node in list and setup tail pointer
	while (lpNewNode->Link.lpNext)
		lpNewNode = lpNewNode->Link.lpNext;
	lpList->Link.lpPrev = lpNewNode;
	}
}

/***********************************************************************/
void ListUnlink(LPLIST lpList, LPVOID lpNodeParam)
/***********************************************************************/
{
LPLISTNODE lpNode = (LPLISTNODE)lpNodeParam;

// if a next object in list, link to this object's previous
if (lpNode->Link.lpNext)
	lpNode->Link.lpNext->Link.lpPrev = lpNode->Link.lpPrev;
// if a previous object in list, link to this object's next
if (lpNode->Link.lpPrev)
	lpNode->Link.lpPrev->Link.lpNext = lpNode->Link.lpNext;
// fix head and tail pointers if necessary
if (lpList->Link.lpNext == lpNode)
	lpList->Link.lpNext = lpNode->Link.lpNext;
if (lpList->Link.lpPrev == lpNode)
	lpList->Link.lpPrev = lpNode->Link.lpPrev;
lpNode->Link.lpPrev = NULL;
lpNode->Link.lpNext = NULL;
}

/***********************************************************************/
int ListGetCount(LPLIST lpList)
/***********************************************************************/
{
LPLISTNODE lpNode;
int iCount = 0;

lpNode = (LPLISTNODE)ListGetHead(lpList);
while (lpNode)
	{
	++iCount;
	lpNode = (LPLISTNODE)ListGetNext(lpNode);
	}
return(iCount);
}

/***********************************************************************/
void ListInit(LPLIST lpList)
/***********************************************************************/
{
lpList->Link.lpNext = lpList->Link.lpPrev = NULL;
}

