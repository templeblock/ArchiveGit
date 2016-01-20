/* he_lang.h */

/* Code	Page numbers */

#if 0 /* { not implemented yet */

#ifdef HE_CP_437	/* { */
#define	HE_CPN_437	 437
#endif			/* } */
#ifdef HE_CP_852	/* { */
#define	HE_CPN_852	 852
#endif			/* } */
#ifdef HE_CP_CWI1	/* { */
#define	HE_CPN_CWI1	 797
#endif			/* } */
#ifdef HE_CP_CWI2	/* { */
#define	HE_CPN_CWI2	 897
#endif			/* } */

#endif	/* } */

#ifdef HE_CP_CWIL2	/* { */
#define	HE_CPN_CWIL2	 997
#endif			/* } */
#ifdef HE_CP_DOSL2	/* { */
#define	HE_CPN_DOSL2	1852
#endif			/* } */
#ifdef HE_CP_WINL2	/* { */
#define	HE_CPN_WINL2	1250
#endif			/* } */
#ifdef HE_CP_ISOL2	/* { */
#define	HE_CPN_ISOL2	2250
#endif			/* } */
#ifdef HE_CP_MACL2	/* { */
#define	HE_CPN_MACL2	3250
#endif			/* } */
#ifdef HE_CP_ICSL2	/* { */
#define	HE_CPN_ICSL2	4250
#endif			/* } */
#ifdef HE_CP_DOSL1	/* { */
#define	HE_CPN_DOSL1	1850
#endif			/* } */
#ifdef HE_CP_WINL1	/* { */
#define	HE_CPN_WINL1	1252
#endif			/* } */
#ifdef HE_CP_WINH1	/* { */
#define	HE_CPN_WINH1	9252
#ifndef	HE_CPN_WINL1  /* { */
#define	HE_CPN_WINL1 1252
#endif		     /*	} */
#endif			/* } */
#ifdef HE_CP_ISOL1	/* { */
#define	HE_CPN_ISOL1	2252
#endif			/* } */
#ifdef HE_CP_MACL1	/* { */
#define	HE_CPN_MACL1	3252
#endif			/* } */
#ifdef HE_CP_ICSL1	/* { */
#define	HE_CPN_ICSL1	4252
#endif			/* } */

/* Language Codes */

#ifdef HE_HUNGARIAN	/* { */
#define	HE_LANG_HUN	0x000e	/* Hungarian */
#endif			/* } */
#ifdef HE_POLISH	/* { */
#define	HE_LANG_POL	0x0015	/* Polish */
#endif			/* } */
#ifdef HE_ENGLISH	/* { */
#define	HE_LANG_ENG	0x0009	/* English */
#endif			/* } */
#ifdef HE_GERMAN	/* { */
#define	HE_LANG_GER	0x0007	/* German */
#endif			/* } */
#ifdef HE_FRENCH	/* { */
#define	HE_LANG_FRA	0x000c	/* French */
#endif			/* } */
#ifdef HE_CZEH		/* { */
#define	HE_LANG_CZE	0x0005	/* Czeh	*/
#endif			/* } */
#ifdef HE_SLOVAK	/* { */
#define	HE_LANG_SLK	0x001b	/* Slovak */
#endif			/* } */
#ifdef HE_TURKISH	/* { */
#define	HE_LANG_TUR	0x001f	/* Turkish */
#endif			/* } */
#define	HE_LANG_MASK	0x01ff	/* Language Code bit-mask */

/* special internal codes */

#define	CWI_MINUS	'\x10' /*16*/	   /* minus sign if it is different from hyphen	*/
#define	CWI_SHY		'\x11' /*17*/	   /* short hyphen sign	if used	*/
#define	CWI_NBRHYP	'\x12' /*18*/	   /* non-breaking hyphen if used */
#define	CWI_COMMA	'\x2c' /*44*/	   /* ',' */
#define	CWI_HYPHEN	'\x2d' /*45*/	   /* '-' hyphen sign even if the minus sign is different */
#define	CWI_PERIOD	'\x2e' /*46*/	   /* '.' */
#define	CWI_SLASH	'\x2f' /*47*/	   /* division sign */
#define	CWI_NBRSPACE	'\x5f' /*95*/	   /* non-breaking space: underline character */
#define	CWI_ENDASH	'\xc4' /*196*/	   /* en-dash: horizontal single line drawing character	*/

/* character flags */

#define	F_ALPHABETIC	0x00000001UL	/* all letters,	including a-umlaut */
#define	F_UPPERCASE	0x00000002UL	/* uppercase letters */
#define	F_LOWERCASE	0x00000004UL	/* lowercase letters, including scharfes-s */
#define	F_VOWEL		0x00000008UL	/* vowels */
#define	F_CONSONANT	0x00000010UL	/* consonants */
#define	F_NUMERIC	0x00000020UL	/* digits */
#define	F_PUNCTUATION	0x00000040UL	/* .,;:!? */
#define	F_ENDMARK	0x00000080UL	/* .!? */
#define	F_PERIOD	0x00000100UL	/* . */
#define	F_DECPOINT	0x00000200UL	/* , */
#define	F_SEP_1000	0x00000400UL	/* thousand separator */
#define	F_DIVSIGN	0x00000800UL	/* / slash */
#define	F_HYPHEN	0x00001000UL	/* - hyphen */
#define	F_MINUS		0x00002000UL	/* - minus sign	*/
#define	F_PLUS		0x00004000UL	/* + plus or +/- sign */
#define	F_ENDASH	0x00008000UL	/* -- en-dash */
#define	F_EMDASH	0x00010000UL	/* --- em-dash */
#define	F_NBRSPACE	0x00020000UL	/* _ non-breaking space	*/
#define	F_LEFT		0x00040000UL	/* left	side signs */
#define	F_RIGHT		0x00080000UL	/* right side signs */
#define	F_BRACKET	0x00100000UL	/* bracket signs: [<{()}>] */
#define	F_QUOTATION	0x00200000UL	/* quotation sign: " */
#define	F_APOSTROPHE	0x00400000UL	/* apostrophe sign: ' */
#define	F_MACRO		0x00800000UL	/* named characters */
#define	F_NMACRO	0x01000000UL	/* named characters following digits */
#define	F_ELLIPSES	0x02000000UL	/* ... ellipses	*/
#define	F_BEGINMARK	0x04000000UL	/* inverse !? */

/* character name array	parameters */

#define	MAX_CHARNAME	16
#define	MAX_NCHARNAMES	16

/* Nonterminal suffix dictionary */

#if defined(HE_HUNGARIAN) /* { */
#define	HE_NONT_DICT
#endif	/* } */

/* international settings */

struct intl_s
	{
	char sthousand;		/* thousand separator: '.', ','	or '\0'	*/
	char sdecimal;		/* decimal point: '.', ',' or '\0' */
	char sminus;		/* minus sign: '-' or different	*/
	char shyphen;		/* hyphen sign:	'-' or different */
	char snbrhyp;		/* non-breaking	hyphen:	'-' or different */
	int language;		/* language code */
	int codepage;		/* code	page number */
#ifdef HE_NONSTD_CP /* { */
	int sec_codepage;	/* secondary code page number */
#endif	/* } */
	};

typedef	struct intl_s INTL;

/* character name structure */

struct charname_s
	{
	char character;
	int namelen;
	char name[MAX_CHARNAME+1];
	};

typedef	struct charname_s CHARNAME;

/* character set structure */

struct charset_s
	{
	char oem2upper[256];		/* oem upper case conversion */
	char oem2lower[256];		/* oem lower case conversion */
	char oem2cwi[256];		/* oem to lex conversion */
	char cwi2oem[256];		/* lex to oem conversion */
	unsigned long oeminfo[256];	/* oem character flags */
	int ncharnames;			/* number of symbol names actually stored */
	CHARNAME charnames[MAX_NCHARNAMES];	/* symbol name array */
	int language;			/* language code */
	int codepage;			/* code	page number */
#ifdef HE_NONSTD_CP /* { */
	int sec_codepage;		/* secondary code page number */
#endif	/* } */
	};

typedef	struct charset_s CHARSET_INFO;

/* prototypes */

#if defined(WINDOWS) || defined(WIN32) || defined(HE_WIN_INTL) /* { */
#include <windows.h>

void charset_win_intl
	(INTL FAR_PTR *pintl
	,int language
#ifdef HE_NONSTD_CP	/* { */
	,char FAR_PTR *inifile
	,char FAR_PTR *inisection
#endif	/* } */
	);

#endif	/* } */

int charset_init(memhandle_t FAR_PTR *phcp, INTL FAR_PTR *pintl);
int charset_alloc(memhandle_t FAR_PTR *phcp);
int charset_reinit(memhandle_t hcp, INTL FAR_PTR *pintl);
void charset_finish(memhandle_t	hcp);

int get_he_language(memhandle_t	hcp);
int get_he_codepage(memhandle_t	hcp);
int has_lang_nont(int language);

#ifdef HE_NONSTD_CP /* { */
int get_he_sec_codepage(memhandle_t hcp);
#endif	/* } HE_NONSTD_CP */
/* end */
