/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_sugg.h
Version:	5.05
Purpose:	header file for	he_sugg.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* definitions */

#define	MAX_SUGG_FIND	16
#define	MAX_SUGG_REPL	16

#define	SUGG_DEL	0x00000001L
#define	SUGG_SWAP	0x00000002L
#define	SUGG_DUP	0x00000004L
#define	SUGG_SPLIT	0x00000008L
#define	SUGG_HYP	0x00000010L
#define	SUGG_CAP	0x00000020L
#define	SUGG_Y_Z	0x00000040L
#define	SUGG_J_LY	0x00000080L
#define	SUGG_ACCENT_1	0x00000100L
#define	SUGG_ACCENT_2	0x00000200L
#define	SUGG_ASSIM	0x00000400L
#define	SUGG_VOICED	0x00000800L
#define	SUGG_PROP_NAM	0x00001000L
#define	SUGG_KEYB	0x00002000L
#define	SUGG_TYPEW	0x00004000L
#define	SUGG_2_CORR	0x00008000L
#define	SUGG_1_STOP	0x00010000L
#define	SUGG_CUST_DIC	0x00020000L
#define	SUGG_ABBREV	0x00040000L

/* structures */

struct suggest_env_s
	{
	memhandle_t hcp;
	long mask;
	int r;
	CDICT suggestdict;
	char suggest_path[PN_MAX];
	void FAR_PTR *objp;
	void FAR_PTR *usrp;
	int (*spell_f)(void FAR_PTR *, char FAR_PTR *, int);
	int (*result_f)(void FAR_PTR *,	char FAR_PTR *);
	int spellresult;	/* a copy of spellobjp->result made after */
				/* checking the	word to	be corrected */
	long options;		/* SUGG_ constants */
	int spellopt;		/* SO_ constants */
	int found;		/* suggestion(s) found */
	};

typedef	struct suggest_env_s SUGGEST_ENV;

/* prototypes */

#ifdef HE_NO_CALLBACK

#ifndef HE_SPELL_F
#define HE_SPELL_F spell_f
#endif

#ifndef HE_RESULT_F
#define HE_RESULT_F result_f
#endif

int HE_SPELL_F(void FAR_PTR *, char FAR_PTR *, int);
int HE_RESULT_F(void FAR_PTR *, char FAR_PTR *);

#else /* HE_NO_CALLBACK */

#define HE_SPELL_F spell_f
#define HE_RESULT_F result_f

#endif /* !HE_NO_CALLBACK */

int suggest_init(memhandle_t FAR_PTR *phsg, memhandle_t	hcp, char FAR_PTR *suggpath, long options);
int suggest_finish(memhandle_t hsg);
int suggest_words(memhandle_t hsg, void	FAR_PTR	*user_ptr, void	FAR_PTR	*objp, int (*spell_f)(void FAR_PTR *, char FAR_PTR *, int), int	(*result_f)(void FAR_PTR *, char FAR_PTR *), char FAR_PTR *word, int spellresult, long mask);

/* end */
