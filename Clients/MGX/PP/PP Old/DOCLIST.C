// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/************************************************************************/
/* Document List Definition						*/
/************************************************************************/
typedef struct _doclist
	{
	HANDLE doc;
	struct _doclist far *next;
} DOCLIST;
typedef DOCLIST far *LPDOCLIST;

static LPDOCLIST DocHead = NULL;

/************************************************************************/
BOOL AddDoc(doc)
/************************************************************************/
HANDLE doc;
{
LPDOCLIST node, new;

if ( !(new = (LPDOCLIST)Alloc((long)sizeof(DOCLIST))) )
	return(FALSE);
new->doc = doc;
new->next = NULL;
if (!DocHead)
	{
	DocHead = new;
	}
else	{
	node = DocHead;
	while (node->next)
		node = node->next;
	node->next = new;
	}
return(TRUE);
}


/************************************************************************/
BOOL RemDoc(doc)
/************************************************************************/
HANDLE doc;
{
LPDOCLIST node, prev, save;

node = DocHead;
prev = NULL;
while (node)
	{
	if (node->doc == doc)
		{
		save = node->next;
		if (prev)
			prev->next = save;
		else	DocHead = save;
		FreeUp((LPTR)node);
		return(TRUE);
		}
	prev = node;
	node = node->next;
	}
return(FALSE);
}


/************************************************************************/
BOOL IsDoc(doc)
/************************************************************************/
HANDLE doc;
{
LPDOCLIST node;

node = DocHead;
while (node)
	{
	if (node->doc == doc)
		return(TRUE);
	node = node->next;
	}
return(FALSE);
}


/************************************************************************/
int NumDocs()
/************************************************************************/
{
LPDOCLIST node;
int num_docs;

num_docs = 0;
node = DocHead;
while (node)
	{
	++num_docs;
	node = node->next;
	}
return(num_docs);
}


/************************************************************************/
HANDLE GetDoc(index)
/************************************************************************/
int index;
{
LPDOCLIST node;
int doc_index;

doc_index = 0;
node = DocHead;
while (node)
	{
	if (doc_index == index)
		return(node->doc);
	++doc_index;
	node = node->next;
	}
return(NULL);
}
