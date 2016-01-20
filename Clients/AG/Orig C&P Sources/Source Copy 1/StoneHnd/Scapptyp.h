/***************************************************************************

	File:		SCAPPTYP.H

	$Header: /Projects/Toolbox/ct/SCAPPTYP.H 2     5/30/97 8:45a Wmanis $

	Contains:	Defintion by client of data types.

	Written by:	Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	

    Use this to define application types for proper type checking,
    these are types that are by and large passede thru the Compostion 
    Toolboxt and thus type information is superfulous
	
@doc

 ***************************************************************************/

#ifndef _H_SCAPPTYP
#define _H_SCAPPTYP
 
#ifdef _WINDOWS   
		#include <windows.h>
#endif

/* ==================================================================== */
/* ==================================================================== */

class DemoView;
class CAGText;
class ApplIOContext;

enum {
	Japanese	= 0,
	English,
	Spanish,
	Italian,
	Portuguese,
	French,
	German,
	Dutch,
	Bokmal,
	Nynorsk,
	Swedish,
	Danish,
	Icelandic,
	Greek,
	Turkish,
	Russian,
	Croatian,
	Finnish,
	Miscellaneous,
	MAX_LANGUAGES
};

// @type APPLanguage | An abstract type/magic cookie that the Composition Toolbox
// may use to specify hyphenation language.
typedef short							APPLanguage;


// @type APPColor | An abstract type/magic cookie that the Composition Toolbox
// may use to specify color.
//
typedef COLORREF						APPColor;		/* color reference */

// @type APPDrwCtx | An abstract type/magic cookie that the Composition Toolbox
// may use to pass thru drawing contexts.
//
typedef CAGText*						APPDrwCtx;		// drawing context

// @type APPFont | An abstract type/magic cookie that the Composition Toolbox
// may use to retrieve and specify font information.
//
typedef const scChar*					APPFont;

// @type APPRender | An abstract type/magic cookie that the Composition Toolbox
// may use to specify font information plus additional drawing attributes
// that the client may wish to use (e.g. drop shadow ). Typically used when
// the traditional values returned by the font sub-system in Quickdraw or
// GDI would not suffice.
// @xref <t APPFont>
typedef struct RenderDef*				APPRender;

// @type TypeSpec | An abstract type/magic cookie that the Composition Toolbox
// may use to retrieve typographic state information.
//
#include "refcnt.h"
class stSpec : public RefCount
{
};
typedef class RefCountPtr<stSpec>		TypeSpec;
typedef class RefCountPtr<stSpec>		scFontRender;

// @type APPColumn | An abstract type/magic cookie to be filled in
// appropriately by the client.
//
typedef	CAGText*						APPColumn;

// @type APPCtxPtr | An abstract type/magic cookie for use in file i/o.
// @xref <t IOFuncPtr>
typedef CAGText*						APPCtxPtr;		

/* ==================================================================== */
		
#endif

