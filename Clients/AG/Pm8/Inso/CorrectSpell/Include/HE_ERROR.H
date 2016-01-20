/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_error.h
Version:	5.04
Purpose:	basic error codes
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* list	of error codes */

#define	EC_OUT_OF_MEMORY	-1
#define	EC_FILE_NOT_FOUND	-2
#define	EC_FILE_NOT_CREATED	-3
#define	EC_READ_ERROR		-4
#define	EC_WRITE_ERROR		-5
#define	EC_SEEK_ERROR		-6
#define	EC_TOO_LONG_LINE	-7
#define	EC_MISSING_ENTRY	-8
#define	EC_SYNTAX_ERROR		-9
#define	EC_TOO_LONG_WORD	-10
#define	EC_DICT_OVERFLOW	-11
#define	EC_FILE_NOT_DELETED	-12
#define	EC_FILE_NOT_RENAMED	-13
#define	EC_UNKNOWN_LANGUAGE	-14
#define	EC_UNKNOWN_CODEPAGE	-15

#define	EC_RESULT_OVERFLOW	-20	/* internal error or damaged lexicon file */

#define	EL_INI_FILE		-100	/* ini-file processing */
#define	EL_LEX_FILE		-200	/* main	lexicon	file */
#define	EL_MAT_FILE		-300	/* morpheme matching matrix processing */
#define	EL_ANALYZER		-400	/* word-form analyzer */
#define	EL_SEARCHER		-500	/* morpheme database access */
#define	EL_FREQUWDS		-600	/* frequent word processing */
#define	EL_STEMMING		-700	/* stemming; ignore in speller/hyphenation applications	*/
#define	EL_SPELLING		-800	/* spelling */
#define	EL_HYPHENAT		-900	/* hyphenation */
#define	EL_SUGGEST		-1000	/* suggestion */
#define	EL_SPEC_DIC		-1100	/* pronounced character	dictionary */
#define	EL_SUGG_DIC		-1200	/* suggest dictionary */
#define	EL_USER_DIC		-1300	/* custom or temporary dictionary, not file-related problem */
#define	EL_CUST_DIC		-1400	/* custom dictionary */
#define	EL_CHYP_DIC		-1500	/* custom hyphenation dictionary */
#define	EL_SHYP_DIC		-1600	/* supplemental	hyphenation dictionary */
#define	EL_CP_FILE		-1700	/* code	page descriptor	file */
#define	EL_MATRIX		-1800	/* morpheme matching matrix processing */
#define	EL_CHARSET		-1900	/* character set initialization	*/
#define	EL_MRF_FILE		-2000	/* morphem info	file */
#define	EL_LEX_HDR		-2100	/* lexicon header file */
#define	EL_INFL_DIC		-2200	/* inflectional	user dictionary	*/

/* end */
