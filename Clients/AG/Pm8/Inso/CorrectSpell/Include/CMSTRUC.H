/****************************************************************************
 *                   Copyright  1995  INSO Corporation                      *
 *                International CorrectSpell[TM] Software                   *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************
*/

#ifndef _CMSTRUC_DEF
#define _CMSTRUC_DEF


#include "cmdefin.h"

#include "cmpstru.h"

#include "cmmostru.h"

#include "cmststru.h"

#include "cmmustru.h"

#include "cmjastru.h"

#include "cmimstru.h"

#include "cmlestru.h"

#include "cmccstru.h"

#include "cmsestru.h"

#include "cmhyph.h"

#ifdef CM_PLEX
#include "cmplsch.h"
#endif

typedef struct _cmCSTRUCT {
  
	jaotinproducttable   jttable;
	_jaotinproductptr      jaoproducts;
	mproducttable     mttable;
	_mproductptr        __mproducts;
	__lexproducttable    lexttable;
	_lexproductptr       _cmlexproducts;
	productind        jaonumof, mnumof, lexnumof;
	
	productptr         product;
	productind        numofproducts;
	
	modes          mode;
	boolean         ready;
	boolean    morfoerror;
	
	UINT2B charflags;
	cap_struct dict_cc;
	PF_SLPATH	mstr_path;
	long error_flags;

	UINT4B hymap[2];
	CHAR hyp_flag;

	CHAR morfo_init;
	UCHAR chkflags;
	
	stringtype      inputqueue;
	
#ifdef BASICFORM_MODE

	boolean  Affixjoint, jsti_joint;
	
#endif
	
	nodeptr     ruletree;
	nodetabletype nodes;
	datatabletype data;	
	
	_producttype      product_;
	
	nameset       FAR *nsets;    /* datatable: nimijoukot */
	__conditionset       FAR *esets;    /* datatable: condsets */
	_ruleclasset  FAR *ssets;    /* datatable: ruleclassets */
	_ruleclasset  FAR *lpsets;   /* nodetable: ruleclassets */
	
	boolean         ilexhomonym;
	
	mhttabletype           charm;
	_referencetabletype      charv;
	paramtabletype     charp;
	mtabletype             mtable;
	
	nameset              FAR *grades;      /* nimijoukot */
	char                    FAR *affixes;    /* affixes  */
	
	INT2B lexlastproduct;
	lexptrtable       lextempptr;
	boolean _holewordfound, partsfound;	

#ifndef MINIM_MEM	
	strtype schpool[POOLSIZE], FAR *schpoolptr;
	searchtype searches[MAX_SEARCHES], FAR *curr_search;
#endif
	
	foundwordtype matched_words[MAX_MATCHES], FAR *curr_word;
	foundwordtype FAR *_foundwordptrs[MAX_MATCHES_PER_SEARCH];	

#ifdef CACHE_LEX
	
	UINT2B *cchashtab;
	UINT2B cchsizeofhtab;
	UCHAR *cache;
	UCHAR cchfull;
	cache_item *cchp;

#endif
	 
	foundwordtype fword;

#ifdef FRQ_LEX

	PFCHAR     freqsk[FRQTABLE_SIZE];
	UINT2B    fcount[FRQTABLE_SIZE];

#endif
	
	pooltype     pool;
	
#ifdef MINIM_MEM	
	INT4B pstoffs;
	UINT2B pcoffs;  
#endif
	
	opttable        firstword, letterbetween, lastletter;
	space          hashspace;
	ilextabletype   __intermlext;
	namebset      FAR  *sets;   /* namebsets */
	
	SL_FHANDLE infile;
	
	node_buff nodeb;
	treepool_struct FAR *_treepools;
	tree_data FAR *trees;
	PFUCHAR FAR *suffixes;
	size_data sizetab;
	grammar_info FAR *grinfo;
	
#ifdef BASICFORM_MODE
	
	_basicform_data FAR *basicformitems;
	
#endif
	
	PFUCHAR altptr;
	UCHAR num_alt, altchars,cap_code;
	
	setword SetSpace[SETSPACE];
	setptr  SetTop;
	
	setptr  Tmpset;

   PFINT4B sl_num_read;

   PFUINT4B att_tab;

   productptr c__mproducts;
  
   productind cmnumofproducts;
   
   unsigned char list[ICLISTSIZE];

	UINT4B inmap[2];
	UCHAR incap;

	UINT4B outmap[2][3];
	UCHAR outcap[3];

	UCHAR ncapalt;
	UCHAR in_apostr, out_apostr;
	
#ifdef CM_HYPHENATION	 
	hyphen_buff hyp_buff;
#endif

#if defined(WINDOWS) || defined(WIN32) || defined(OS2)
    HMODULE hInstFinnDll;
#endif

#ifdef CM_PLEX

    pfplexCtrl FAR *plex;
    
    pfplexCtrl FAR *stoplex;
    
#endif    
	
      INT4B memsize;

      CHAR prolex;
		
} cmCStruct,
FAR * pcmCS;
	
	  
#endif
	
