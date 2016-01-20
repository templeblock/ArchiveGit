/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_hdr.h
Version:	5.04
Purpose:	header file for	he_hdr.c
Created	by:	Miklos Pal
Creation date:	09-31-1994
Changes:	<name>	<date>	<purpose>
*/

/* structure types */

struct hdrentry_tag
	{
	long id;
	long start;
	long length;
	};

typedef	struct hdrentry_tag HDRENTRY;

struct hdrobj_tag
	{
	long id;
	long ver;
	long n_entries;
	memhandle_t h_array;
	};

typedef	struct hdrobj_tag HDROBJ;

/* function prototypes */

int hdr_init(memhandle_t FAR_PTR *phhdr, char FAR_PTR *fn);
void hdr_finish(memhandle_t hhdr);
long hdr_find(memhandle_t hhdr,	long id);
int fgetl_lsb1st(FILE_ID fptr, long FAR_PTR *iip);
int fgetw_lsb1st(FILE_ID fptr, short FAR_PTR *ip);

/* end */
