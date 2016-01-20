//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef _LIST_H_
#define _LIST_H_

#include "types.h"
#include "appdata.h"

#define ListGetHead(lpList) ((LPVOID)((lpList)->Link.lpNext))
#define ListGetTail(lpList) ((LPVOID)((lpList)->Link.lpPrev))
#define ListIsEmpty(lpList) ((lpList)->Link.lpNext == NULL)
#define ListGetNext(lpNode) ((LPVOID)((lpNode)->Link.lpNext))
#define ListGetPrev(lpNode) ((LPVOID)((lpNode)->Link.lpPrev))

void ListAddTail(LPLIST lpList, LPVOID lpNewNode);
void ListAddHead(LPLIST lpList, LPVOID lpNewNode);
void ListInsertAfter(LPLIST lpList, LPVOID lpAfter, LPVOID lpNewNode);
void ListInsertBefore(LPLIST lpList, LPVOID lpBefore, LPVOID lpNewNode);
void ListUnlink(LPLIST lpList, LPVOID lpNode);
int ListGetCount(LPLIST lpList);
void ListInit(LPLIST lpList);

#endif // _LIST_H_
