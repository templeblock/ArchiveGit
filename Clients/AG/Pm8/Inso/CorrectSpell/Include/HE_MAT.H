/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_mat.h
Version:	5.04
Purpose:	header file for	he_mat.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* structure types */

struct struct_mt_info
	{
	char h_start;
	char h_end;
	char v_start;
	char v_end;
	char data[1];
	};

typedef	struct struct_mt_info MT_INFO;

/* function prototypes */

int mt_init(memhandle_t	FAR_PTR	*phvmx,	memhandle_t FAR_PTR *phnmx, char FAR_PTR *mxpath, long seek_to);
void mt_finish(memhandle_t hvmx, memhandle_t hnmx);
int mt_match(memhandle_t hmx, char horiz, char vert);

/* end of he_mat.h */
