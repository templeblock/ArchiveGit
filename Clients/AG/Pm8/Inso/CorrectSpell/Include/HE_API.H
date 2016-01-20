/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_api.h
Version:	5.04
Purpose:	API base level include file to be included in OEM sources
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

#include "he_defs.h"

#include "he_lim.h"
#include "he_env.h"
#include "he_llio.h"
#include "he_hlio.h"
#include "he_error.h"
#include "he_lang.h"
#include "he_freq.h"
#include "he_mat.h"
#include "he_lex.h"
#include "he_anal.h"
#include "he_spell.h"

#if defined(HE_USERDICT)  \
 || defined(HE_HYPHENATE) \
 || defined(HE_SUGGEST)	  \
 || defined(HE_INFLDICT)
#include "he_dlk.h"
#include "he_cdict.h"
#endif

#if defined(HE_HYPHENATE)
#include "he_hyph.h"
#endif

#if defined(HE_USERDICT)
#include "he_udict.h"
#endif

#if defined(HE_SUGGEST)
#include "he_sugg.h"
#endif

#if defined(HE_INFLDICT)
#include "he_infl.h"
#endif

#if defined(HE_LEMMATIZE)
#include "he_stem.h"
#endif

/* end */
