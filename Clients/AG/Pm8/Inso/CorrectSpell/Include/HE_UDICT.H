/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_udict.h
Version:	5.04
Purpose:	header file for	he_udict.c
Created	by:	Miklos Pal
Creation date:	12-2-1993
Changes:	<name>	<date>	<purpose>
*/

/* constants */

#define	UD_LOAD		0x0001		/* load	from file */
#define	UD_SAVE		0x0002		/* save	at exit	if modified */
#define	UD_CREATE	0x0004		/* create file if does not exist */

#define	UD_WORD_DICT	0x1000		/* word	dictionary identifier */
#define	UD_REPL_DICT	0x2000		/* replace dictionary identifier */

/* structures */

struct userdict_env_s
	{
	int options;			/* UD_ options */
	CDICT dict;			/* dictionary */
	char pathname[PN_MAX];		/* pathname */
	};

typedef	struct userdict_env_s USERDICT;

/* prototypes */

int userdict_init(memhandle_t FAR_PTR *phenv, memhandle_t hcp, char FAR_PTR *pathname, int options);
int userdict_finish(memhandle_t	henv);
int userdict_clear(memhandle_t henv);
int userdict_add(memhandle_t henv, char	FAR_PTR	*word, char FAR_PTR *repl);
int userdict_get(memhandle_t henv, int index, char FAR_PTR *to,	char FAR_PTR *by);
int userdict_find(memhandle_t henv, char FAR_PTR *word,	char FAR_PTR *repl);
int userdict_delete(memhandle_t	henv, char FAR_PTR *word);
int userdict_type(memhandle_t henv);
int userdict_size(memhandle_t henv);

/* end */
