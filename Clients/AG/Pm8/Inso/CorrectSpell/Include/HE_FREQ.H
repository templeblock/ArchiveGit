/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_freq.h
Version:	5.04
Purpose:	header file for	he_freq.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* structure types */

struct freq_s
	{
	int nwords;
	char buffer[1];
	};

typedef	struct freq_s FREQ_WORDS;

/* prototypes */

int freq_init(memhandle_t FAR_PTR *ph, memhandle_t hcp);
void freq_finish(memhandle_t h);
int is_frequent_word(memhandle_t h, char FAR_PTR *word);

/* end */
