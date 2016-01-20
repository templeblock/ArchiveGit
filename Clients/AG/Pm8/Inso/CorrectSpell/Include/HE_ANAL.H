/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_anal.h
Version:	5.04
Purpose:	header file for	he_anal.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* macro functions */

/* language-independent	macros */

#define	IS_NOMINAL(code)		(((code)&(0x8000U))==0x8000U)
#define	IS_VERBAL(code)			(((code)&(0x8000U))==0x0000U)

/* language-dependent macros */

#ifdef HE_HUNGARIAN			/* { */

#define	IS_HUN_ANTIWORD(code)		(((code)&(0x800eU))==0x800eU)
#define	IS_HUN_SUP_PREF(code)		(((code)&(0xffffU))==0x8010U)
#define	IS_HUN_WORD_TO_SPLIT(code)	(((code)&(0x0001U))==0x0001U)
#define	IS_HUN_COMPOUND(code)		(((code)&(0x8003U))==0x8003U)
#define	IS_HUN_IK_VERB(code)		(((code)&(0x9000U))==0x1000U)
#define	IS_HUN_NON_IK_VERB(code)	(((code)&(0x9000U))==0x0000U)
#define	IS_HUN_UNSUFF_WORD(code)	(((code)&(0x800fU))==0x8000U)
#define	IS_HUN_PREFIX(code)		(((code)&(0x800fU))==0x8001U)
#define	IS_HUN_ADJECTIVE(code)		(((code)&(0x800eU))==0x800aU)
#define	IS_HUN_NUMERIC(code)		(((code)&(0x800eU))==0x8006U)
#define	IS_HUN_NUMERIC1(code)		(((code)&(0x800fU))==0x8006U)
#define	IS_HUN_NUM1(code)		(((code)&(0x801eU))==0x8006U)
#define	IS_HUN_NUM2(code)		(((code)&(0x801eU))==0x8016U)
#define	IS_HUN_NUM(code)		(((code)&(0x800eU))==0x8006U)
#define	IS_HUN_NOUN_OR_ADJ(code)	(((code)&(0x8006U))==0x8002U)
#define	IS_HUN_NOUN(code)		(((code)&(0x800eU))==0x8002U)
#define	IS_HUN_COMPONENT_OK(code)	((((code)&(0x8010U))==0x8010U)\
				       &&(((code)&(0x000eU))!=0x0006U))
#define	IS_HUN_SPECIAL(code)		((code)==0x8000U)

#endif					/* } */

#ifdef HE_POLISH			/* { */

#define	IS_POL_ANTIWORD(code)		(((code)&(0x80ffU))==0x80f5U)
#define	IS_POL_UNSUFF_WORD(code)	(((code)&(0x800fU))==0x8005U)
#define	IS_POL_ADJECTIVE(code)		(((code)&(0x8003U))==0x8000U)
#define	IS_POL_COMP_ADJV(code)		((((code)&(0x8003U))==0x8000U)\
				       &&(((code)&(0x1300U))!=0x0000U))
#define	IS_POL_NEG_TERM(code)		(((code)&(0x9303U))==0x8000U)
#define	IS_POL_NUM_TERM(code)		(((code)&(0x9303U))==0x8000U)
#define	IS_POL_PREFIX(code)		(((code)&(0x803fU))==0x8025U)
#define	IS_POL_NUM_PREF(code)		(((code)&(0x80ffU))==0x80a5U)
#define	IS_POL_SUP_PREF(code)		(((code)&(0x80ffU))==0x8025U)
#define	IS_POL_NEG_PREF(code)		(((code)&(0x80ffU))==0x8065U)
#define	IS_POL_GEO_PREF(code)		(((code)&(0x80ffU))==0x80e5U)
/*
#define	IS_POL_NOUN(code)		((((code)&(0x8002U))==0x8002U)\
				       ||(((code)&(0x8007U))==0x8001U)\
				       ||(((code)&(0x800fU))==0x800dU))
*/

#endif					/* } */

#ifdef HE_ENGLISH			/* { */

#define	IS_ENG_ANTIWORD(code)		(((code)&(0x800eU))==0x800eU)
/*
#define	IS_ENG_COMPOUND(code)		(((code)&(0x8003U))==0x8003U)
*/
#define	IS_ENG_UNSUFF_WORD(code)	(((code)&(0x800fU))==0x8000U)
#define	IS_ENG_PREFIX(code)		(((code)&(0x800fU))==0x8001U)
/*
#define	IS_ENG_ADJECTIVE(code)		(((code)&(0x800eU))==0x800aU)
#define	IS_ENG_NUMERIC(code)		(((code)&(0x800eU))==0x8006U)
#define	IS_ENG_NUM1(code)		(((code)&(0x801eU))==0x8006U)
#define	IS_ENG_NUM2(code)		(((code)&(0x801eU))==0x8016U)
#define	IS_ENG_NUM(code)		(((code)&(0x800eU))==0x8006U)
#define	IS_ENG_NOUN_OR_ADJ(code)	(((code)&(0x8006U))==0x8002U)
#define	IS_ENG_NOUN(code)		(((code)&(0x800eU))==0x8002U)
#define	IS_ENG_COMPONENT_OK(code)	((((code)&(0x8010U))==0x8010U)\
				       &&(((code)&(0x000eU))!=0x0006U))
*/

#endif					/* } */

/* analyzer status values */

#define	AN_NOERR	0
#define	AN_SPLIT_IT	1
#define	AN_ANTIWORD	2
#define	AN_6_3_RULE	4

/* analyzer option values */

#define	AO_HYPHENATED		0x0100U		/* allows compound components without hyphen sign */
#define	AO_SEGMENTEDONLY	0x0200U		/* allows compound and prefixed	words */
#define	AO_SINGLEONLY		0x0400U		/* allows single and prefixed words */
#define	AO_NUMONLY		0x0800U		/* numeral only	*/

#define	AO_PART			0x1000U		/* Polish: particle has been cut */
#define	AO_HYPH1		0x2000U		/* Polish: first part of a hyphenated compound */
#define	AO_VERB_LI		0x4000U		/* Polish: "li"	has been cut */

/* analyzer result */

#define	AN_RESULT_SIZE	320

/* structure types */

struct stem_data_s
	{
	unsigned char len;	/* len(gth) must be the	first byte in the structure */
	unsigned char stemclass0;
	unsigned char stemclass1;
	unsigned char morf0;
	unsigned char morf1;
	};

typedef	struct stem_data_s STEM_DATA;

#ifdef HE_NONT_DICT /* { */
struct nont_data_s
	{
	unsigned char len;	/* len(gth) must be the	first byte in the structure */
	unsigned char stemclass0;
	unsigned char stemclass1;
	unsigned char termclass0;
	unsigned char termclass1;
	unsigned char morf0;
	unsigned char morf1;
	};

typedef	struct nont_data_s NONT_DATA;
#endif	/* } */

struct term_data_s
	{
	unsigned char len;	/* len(gth) must be the	first byte in the structure */
	unsigned char termclass0;
	unsigned char termclass1;
	unsigned char morf0;
	unsigned char morf1;
	};

typedef	struct term_data_s TERM_DATA;

struct result_data_s
	{
	STEM_DATA stem1;
	STEM_DATA stem2;
	STEM_DATA stem3;
#ifdef HE_NONT_DICT /* { */
	NONT_DATA nont;
#endif	/* } */
	TERM_DATA term;
#ifdef HE_ANALYZE /* { */
	char morf_lst[AN_RESULT_SIZE];
#endif	/* } */
	unsigned short stem1_ra;
	unsigned short stem2_ra;
	unsigned short stem3_ra;
	unsigned short result_ra;
	unsigned short laststem_ra;
#ifdef HE_POLISH /* { */
	char term_lb;
#ifdef HE_NONT_DICT /* { */
	char nont_lb;
#endif	/* } */
#endif	/* } */
	};

typedef	struct result_data_s RESULT_DATA;

struct an_obj_s
	{
	int language;			/* language code */
#ifdef HE_NONT_DICT /* { */
	int has_nont;			/* nonterminal suffix dictionary is used */
#endif	/* } */
#if defined(HE_HUNGARIAN) || defined(HE_POLISH)	|| defined(HE_ENGLISH) /* { */
	int (*p_stem_suffix_check)(struct an_obj_s FAR_PTR *anobjp);
#endif	/* } */
#if defined(HE_HUNGARIAN) || defined(HE_POLISH)	/* { */
	int (*p_prefix_stem_suffix_check)(struct an_obj_s FAR_PTR *anobjp);
#endif	/* } */
	int (*p_is_prefix)(unsigned short code);
	int (*p_is_antiword)(unsigned short code);
	int (*p_is_unsuff_word)(unsigned short code);
	memhandle_t hlexhdr;		/* lexicon header object */
	memhandle_t hstemclt;		/* stem	class table */
	memhandle_t htermclt;		/* term	class table */
	long nstemclt;
	long ntermclt;
	long sstemclt;
	long stermclt;
#ifdef HE_BUFFERED_IO /* { */
	FILE_ID	fptr;
#else	/* }{ */
	LL_HANDLE fhandle;
#endif	/* } */
	int an_status;			/* a combination of AN_SPLIT_IT, AN_ANTIWORD and AN_6_3_RULE */
	memhandle_t hstem;		/* stem	dictionary */
#ifdef HE_NONT_DICT /* { */
	memhandle_t hnont;		/* non-terminal	suffix dictionary */
#endif	/* } */
	memhandle_t hterm;		/* terminal suffix dictionary */
	memhandle_t hvmx;		/* verbal matrix */
	memhandle_t hnmx;		/* nominal matrix */
	memhandle_t stem1_result;	/* handles for the results of search functions */
	memhandle_t stem2_result;
	memhandle_t stem3_result;
#ifdef HE_NONT_DICT /* { */
	memhandle_t nont_result;
#endif	/* } */
	memhandle_t term_result;
	char FAR_PTR *word;		/* input word */
	int wordlen;			/* length of the input word */
	char revword[MAX_WORD+1];	/* reversed copy of the	input word, null-terminated */
	int an_flags;			/* options */
	int stem1_alt_max;		/* search result counters */
	int stem2_alt_max;
	int stem3_alt_max;
#ifdef HE_NONT_DICT /* { */
	int nont_alt_max;
#endif	/* } */
	int term_alt_max;
	STEM_DATA FAR_PTR *laststem_p;	/* pointer to the last valid stem (stem<1 or 2 or 3>_p)	*/
	STEM_DATA FAR_PTR *stem1_p;	/* pointers to the currently tested items */
	STEM_DATA FAR_PTR *stem2_p;
	STEM_DATA FAR_PTR *stem3_p;
#ifdef HE_NONT_DICT /* { */
	NONT_DATA FAR_PTR *nont_p;
#endif	/* } */
	TERM_DATA FAR_PTR *term_p;
	char firststem_rb;
	unsigned short firststem_ra;
	char laststem_rb;
	unsigned short laststem_ra;
	unsigned short stem1_ra;
	unsigned short stem2_ra;
	unsigned short stem3_ra;
#ifdef HE_NONT_DICT /* { */
	char nont_rb;
	unsigned short nont_ra;
	char nont_lb;
	unsigned short nont_la;
	unsigned short nont_lc;
#endif	/* } */
	char term_lb;
	unsigned short term_la;
	unsigned short term_lc;
	int stem2_pos;			/* 2nd stem offset */
	int stem3_pos;			/* 3rd stem offset */
#ifdef HE_NONT_DICT /* { */
	int nont_pos;			/* nont	suffix offset if the list is loaded */
#endif	/* } */
	int term_loaded;		/* terminal sufix list is loaded */
	char anti[MAX_WORD];		/* antiword flags */
	};

typedef	struct an_obj_s	AN_OBJ;

struct dict_cache_tag
	{
	int sc,nc,tc;			/* cache sizes in pages	for stem, nont and term	*/
	int sp,np,tp;			/* cache preload flags for stem, nont and term */
	};

typedef	struct dict_cache_tag DICT_CACHE;

/* function prototypes */

int an_init(memhandle_t	FAR_PTR	*ph,char FAR_PTR *lexpath,DICT_CACHE FAR_PTR *pcache,int language);
void an_finish(memhandle_t h);

int an_analyze(memhandle_t h, char FAR_PTR *word, int wordlen, int options, RESULT_DATA	FAR_PTR	*resultp, int maxresults);
int an_get_status(memhandle_t h);
int is_nominal_result(RESULT_DATA FAR_PTR *resultp, int	language);

#ifdef HE_INFLDICT /* {	*/
int an_stem_codes(memhandle_t h, char FAR_PTR *stem, char FAR_PTR *b_list, short FAR_PTR *a_list, int list_max);
#endif	/* } */

#ifdef HE_HUNGARIAN /* { */
int is_special_result_hun(RESULT_DATA FAR_PTR *resultp);
#endif	/* } */

/* end of he_anal.h */
