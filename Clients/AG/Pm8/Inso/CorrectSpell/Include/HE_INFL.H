/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_infl.h
Version:	5.04
Purpose:	header file for	he_infl.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* structore types */

struct infl_obj_tag
	{
	memhandle_t hspell;
	memhandle_t hcp;
	memhandle_t han;
	CDICT infldict;
	};

typedef	struct infl_obj_tag INFL_OBJ;

/* prototypes */

int infl_init(memhandle_t FAR_PTR *phinfl, memhandle_t hspell, char FAR_PTR *pn);
void infl_finish(memhandle_t hinfl);
int infl_check(memhandle_t hinfl, char FAR_PTR *word, int options);

/* end */
