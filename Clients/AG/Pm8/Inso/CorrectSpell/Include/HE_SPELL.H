/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_spell.h
Version:	5.05
Purpose:	header file for	he_spell.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* constants: spelling results */

#define	SR_NOERR		0	/* spelling OK */
#define	SR_NOTHING_TO_DO	0x01	/* alphabetic character	is missing */
#define	SR_UNKNOWN_FORM		0x02	/* cannot locate the word */
#define	SR_UNKNOWN_MACRO	0x04	/* macro cannot	be expanded */
#define	SR_UNKNOWN_WORD		0x08	/* word	cannot be analyzed */
#define	SR_FOREIGN_LETTER	0x10	/* foreign letter is in	the word */
#define	SR_ANTI_COMPOUND	0x20	/* try to insert a space */
#define	SR_ANTI_WORD		0x40	/* do not try to split anyway */
#define	SR_VIOLATION_6_3	0x80	/* try to hyphenate */

/* constants: spelling options (powers of two) */

#ifdef HE_POLISH	/* { */
#define	SO_PART		AO_PART		/* Polish: particle requires nominal form */
#define	SO_VERB_LI	AO_VERB_LI	/* Polish: "li"	has been cut */
/*#define	SO_VERB_ZE	AO_VERB_ZE*/	/* Polish: "z.[e]" has been cut	*/
#endif			/* } */

#define	SO_FREQUENT	0x0001		/* if check frequent words first */
#define	SO_ROMAN	0x0002		/* if check roman numbers */
#define	SO_SEGMENT	0x0004		/* if segmentation required */
#define	SO_SINGLEONLY	AO_SINGLEONLY	/* only	single and prefixed words are allowed */
#define	HO_KEEPVOWELS	0x0010		/* do not hyphenate between two	vowels inside a	word */
#define	HO_KEEPVOWEL1	0x0020		/* same	except the right vowel is followed by two or more consonants */
#define	HO_KEEPONE	0x0040		/* do not hyphenate a single letter */

/* numeral macro constants */

#define	NM_NORMAL		0
#define	NM_ORDER		1
#define	NM_DECIMAL		2
#define	NM_FRACTION		3

/* constants: segmentation marks */

#define	SEG_NULL		((char)'\x20')
#define	SEG_COMPOUND		((char)'|')
#define	SEG_SUFFIX		((char)'+')

/* analyzer results */

#define	N_AN_RESULTS	16
/* AN_RESULT_SIZE is defined in	he_anal.H */

/* structure types */

struct spell_env_s
	{
	memhandle_t hcp;	/* code	page */
	memhandle_t han;	/* analyzer */
	memhandle_t hfw;	/* frequent words */
#ifdef HE_EXC_DIC /* { */
	memhandle_t hxc;	/* optional exclusion dictionary */
#endif	/* } */
#ifdef HE_INFLDICT /* {	*/
	memhandle_t hinfl;	/* inflectional	dictionary module */
#endif	/* } */
#ifdef HE_ANALYZE /* { */
	int FAR_PTR *p_n_morf_results;
	char FAR_PTR *FAR_PTR *p_morf_results;
#endif	/* } */
	};

typedef	struct spell_env_s SPELL_ENV;

struct spell_arg_s
	{
	/* input parameters */
	char FAR_PTR *word;	/* ptr to the word to be spell-checked */
	char FAR_PTR *segm;	/* ptr to the buffer for segmentation */
	char FAR_PTR *hyph;	/* ptr to the buffer for hyphenation */
	int wordlen;		/* input word length */
	int options;		/* spelling and	segmentation options */
	/* output parameters */
	int chkfirst;		/* number of skipped characters	*/
	int nonchkfirst;	/* number of skipped+checked characters	*/
	int n_foreign;		/* number of foreign letters */
	int first_upper;	/* if the first	letter is upper	*/
	int n_upper;		/* number of uppercase letters */
	int n_lower;		/* number of lowercase letters */
	int result;		/* detailed result */
#ifdef HE_ANALYZE /* { */
	int FAR_PTR *p_n_morf_results;
	char FAR_PTR *FAR_PTR *p_morf_results;
#endif	/* } */
	};

typedef	struct spell_arg_s SPELL_ARG;

struct spell_cache_tag
	{
	int sc,nc,tc;			/* cache sizes in pages	for stem, nont and term	*/
	int sp,np,tp;			/* cache preload flags for stem, nont and term */
	};

typedef	struct spell_cache_tag SPELL_CACHE;

/* prototypes */

int spell_init
	(memhandle_t FAR_PTR *phspellenv
	,memhandle_t hcp
#ifdef HE_EXC_DIC		/* { */
	,memhandle_t hxc
#endif	/* } */
	,char FAR_PTR *lexpath
	,SPELL_CACHE FAR_PTR *pcache);

#ifdef HE_EXC_DIC
memhandle_t spell_get_hxc(memhandle_t hspellenv);
#endif

int spell_finish(memhandle_t hspellenv);
int spell_check(memhandle_t hspellenv, SPELL_ARG FAR_PTR *argptr);

/* end */
