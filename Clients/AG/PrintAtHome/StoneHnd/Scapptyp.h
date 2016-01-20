#ifndef _H_SCAPPTYP
#define _H_SCAPPTYP
 
class CAGText;

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
typedef short APPLanguage;

// @type APPColor | An abstract type/magic cookie that the Composition Toolbox
// may use to specify color.
typedef COLORREF APPColor;

// @type APPFont | An abstract type/magic cookie that the Composition Toolbox
// may use to retrieve and specify font information.
typedef const scChar* APPFont;

// @type APPRender | An abstract type/magic cookie that the Composition Toolbox
// may use to specify font information plus additional drawing attributes
// that the client may wish to use (e.g. drop shadow ). Typically used when
// the traditional values returned by the font sub-system in Quickdraw or
// GDI would not suffice.
// @xref <t APPFont>
typedef struct RenderDef* APPRender;

// @type TypeSpec | An abstract type/magic cookie that the Composition Toolbox
// may use to retrieve typographic state information.
#include "refcnt.h"

class APPSpec : public RefCount
{
};
typedef class RefCountPtr<APPSpec> TypeSpec;

// @type APPColumn | An abstract type/magic cookie to be filled in
// appropriately by the client.
typedef CAGText* APPColumn;

// @type APPCtxPtr | An abstract type/magic cookie for use in file i/o.
// @xref <t IOFuncPtr>
typedef CAGText* APPCtxPtr;

// @type APPDrwCtx | An abstract type/magic cookie that the Composition Toolbox
// may use to pass thru drawing contexts.
typedef CAGText* APPDrwCtx;

#endif _H_SCAPPTYP
