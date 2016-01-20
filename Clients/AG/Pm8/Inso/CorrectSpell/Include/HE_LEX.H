/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_lex.h
Version:	5.04
Purpose:	header file for	he_lex.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* structure types */

struct struct_index_desc
	{
	memhandle_t hentry;
	long dboffset;
	short stored_emb;
	short all_emb;
	int back_track;
	int back_emb;
	};

typedef	struct struct_index_desc INDEX_DESC;

struct struct_cache_entry
	{
	memhandle_t data;	/* char* */
	int tag;
	};

typedef	struct struct_cache_entry CACHE_ENTRY;

struct struct_db_info
	{
#ifdef HE_BUFFERED_IO /* { */
	FILE_ID	fptr;
#else	/* }{ */
	LL_HANDLE fhandle;
#endif	/* } */
	long dic_pos;
#ifdef HE_ANALYZE /* { */
	memhandle_t morf_bcd;
	memhandle_t morf_lst;
	memhandle_t morf_ind;
	long bcd_size;
	long lst_nwords;
	long lst_size;
#endif	/* } */
	memhandle_t cache;	/* CACHE_ENTRY*	*/
	memhandle_t inxlist;	/* INDEX_DESC* */
	long recctr;
	long pagesize;
	long infosize;
	long embmax;
	int npages;		/* number of cache pages */
	int preload;		/* preload pages to cache */
	};

typedef	struct struct_db_info DB_INFO;

/* function prototypes */

void db_set_file
	(memhandle_t h
#ifdef HE_BUFFERED_IO /* { */
	,FILE_ID fptr);
#else	/* }{ */
	,LL_HANDLE fhandle);
#endif	/* } */

int db_init
	(memhandle_t FAR_PTR *ph
	,char FAR_PTR *lexpath
	,long dic_pos
	,long ind_pos
#ifdef HE_ANALYZE /* { */
	,long mor_pos
#endif	/* } */
	,int npages, int preload);

void db_finish(memhandle_t h);
int db_search(memhandle_t h, char FAR_PTR *word, char FAR_PTR *resultinfo);
int db_get_embmax(memhandle_t h);
int db_get_infosize(memhandle_t	h);

#ifdef HE_ANALYZE
int db_getmorf(memhandle_t h, int offset, char FAR_PTR *to);
#endif

/* end of he_lex.h */
