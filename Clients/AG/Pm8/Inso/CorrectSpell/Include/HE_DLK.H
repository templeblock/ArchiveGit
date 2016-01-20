/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_dlk.h
Version:	5.04
Purpose:	header file for	he_dlk.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* structure types */

struct dlkitem_s
	{
	memhandle_t h_prev_item;
	memhandle_t h_next_item;
	};

typedef	struct dlkitem_s DLKITEM;

struct dlklist_s
	{
	memhandle_t h_first_item;
	memhandle_t h_last_item;
	};

typedef	struct dlklist_s DLKLIST;

/* function prototypes */

memhandle_t dlk_prev(memhandle_t h_item);
memhandle_t dlk_next(memhandle_t h_item);
void dlk_link(DLKLIST FAR_PTR *listp, memhandle_t h_item, memhandle_t h_curr);
void dlk_unlink(DLKLIST	FAR_PTR	*listp,	memhandle_t h_item);

/* macros */

#define	dlk_append(lp,h)	dlk_link(lp,h,DM_FULL)

/* end */
