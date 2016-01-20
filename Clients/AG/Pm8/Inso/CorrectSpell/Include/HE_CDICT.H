/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_cdict.h
Version:	5.05
Purpose:	header file for	he_cdict.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* defines */

#define	CDICT_REPLDUP	0x0001
#define	CDICT_IGNCASE	0x0002
#define	CDICT_USRFLAG	0x0004

/* structure types */

struct dict_data_page_s
	{
	DLKITEM	link;
	int data_used;
	char data_arr[1];
	};

typedef	struct dict_data_page_s	DICT_DATA_PAGE;

struct dict_data_ref_s
	{
	memhandle_t handle;
	int offset;
	};

typedef	struct dict_data_ref_s DICT_DATA_REF;

struct dict_data_x_ref_s
	{
	/* same	as struct dict_data_ref_s */
	memhandle_t handle;
	int offset;
	/* new elements	of the structure */
	memhandle_t x_handle;
	int x_pos;
	};

typedef	struct dict_data_x_ref_s DICT_DATA_X_REF;

struct dict_index_page_s
	{
	DLKITEM	link;
	int ref_first;
	int ref_used;
	DICT_DATA_REF ref_arr[1];
	};

typedef	struct dict_index_page_s DICT_INDEX_PAGE;

struct cdict_s
	{
	memhandle_t hcp;
	int modified;	/* -1 =	not loaded; 0 =	loaded;	1 = modified */
	int sorted;	/* 0 = not sorted, 1 = sorted (in input	file) */
	int options;
#ifdef HE_LID /* { */
	int lid;	/* language code */
#endif	/* } */
	int n_elements;
	int data_max;
	int index_max;
	DLKLIST	datalist;
	DLKLIST	indexlist;
	};

typedef	struct cdict_s CDICT;

/* function prototypes */

void cdict_init(CDICT FAR_PTR *cdp, memhandle_t	hcp, int data_max, int index_max, int options);
void cdict_finish(CDICT	FAR_PTR	*cdp);
int cdict_size(CDICT FAR_PTR *cdp);
int cdict_access(CDICT FAR_PTR *cdp, int index,	DICT_DATA_X_REF	FAR_PTR	*refp);
int cdict_find_first(CDICT FAR_PTR *cdp, char FAR_PTR *word, int wordlen);
int cdict_locate_first(CDICT FAR_PTR *cdp, char	FAR_PTR	*word, int wordlen);
int cdict_delete(CDICT FAR_PTR *cdp, int index);
int cdict_add(CDICT FAR_PTR *cdp, char FAR_PTR *word, int wordlenplus);
void letter_stat(CHARSET_INFO FAR_PTR *pcp, char FAR_PTR *word,	int FAR_PTR *first_upper_ptr, int FAR_PTR *n_upper_ptr,	int FAR_PTR *n_lower_ptr);

/* end */
