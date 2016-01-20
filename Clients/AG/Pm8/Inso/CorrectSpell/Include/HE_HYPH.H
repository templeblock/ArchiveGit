/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_hyph.h
Version:	5.04
Purpose:	header file for	he_hyph.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* constants */

#define	HYP_NULL	(char)'\x20'
#define	HYP_NORMAL	(char)'-'
#define	HYP_INSERT	(char)'='
#define	HYP_INSTWO	(char)'#'
#define	HYP_TRUNC	(char)'*'	/* HYP_TRUNC < '0' */

#ifndef	UD_LOAD
#define	UD_LOAD	0x0001			/* load	from file, see he_udict.h */
#endif

#ifndef	UD_SAVE
#define	UD_SAVE	0x0002			/* save	at exit	if modified, see he_udict.h */
#endif

#ifndef	UD_CREATE
#define	UD_CREATE 0x0004		/* create file if does not exist, see he_udict.h */
#endif

/* structure types */

struct hyphen_env_s
	{
	int state;			/* used	internally */
	int ixstart;			/* used	internally */
	int options;			/* UD_ user dictionary options */
	memhandle_t hcp;		/* code	page handle */
	char custom_hyp_path[PN_MAX];	/* custom hyphenation dictionary path */
	char suppl_hyp_path[PN_MAX];	/* supplementary hyphenation dictionary	path */
	CDICT segdict;
	CDICT hypdict;
	CDICT ambdict;
	CDICT findict;
	CDICT custom_hyp_dict;
	};

typedef	struct hyphen_env_s HYPHEN_ENV;

/* prototypes */

int hyphen_init(memhandle_t FAR_PTR *phhyphenv,	memhandle_t hspellenv, char FAR_PTR *supplpath,	char FAR_PTR *custompath, int options);
int hyphen_finish(memhandle_t hhyphenv);
int hyphen_usr(memhandle_t hhyphenv, SPELL_ARG FAR_PTR *argptr);
int hyphen_ate(memhandle_t hhyphenv, SPELL_ARG FAR_PTR *argptr);
int hyphen_delete(memhandle_t hhyph, char FAR_PTR *word);
int hyphen_find(memhandle_t henv, char FAR_PTR *word);
int hyphen_add(memhandle_t hhyphenv, char FAR_PTR *abuff);
int hyphen_get(memhandle_t hhyphenv, int index,	char FAR_PTR *abuff);
int hyphen_dsize(memhandle_t hhyphenv);

/* end */
