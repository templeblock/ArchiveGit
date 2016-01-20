/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2002 PDFlib GmbH and Thomas Merz. All rights reserved. |
 +---------------------------------------------------------------------------+
 |    This software is NOT in the public domain.  It can be used under two   |
 |    substantially different licensing terms:                               |
 |                                                                           |
 |    The commercial license is available for a fee, and allows you to       |
 |    - ship a commercial product based on PDFlib                            |
 |    - implement commercial Web services with PDFlib                        |
 |    - distribute (free or commercial) software when the source code is     |
 |      not made available                                                   |
 |    Details can be found in the file PDFlib-license.pdf.                   |
 |                                                                           |
 |    The "Aladdin Free Public License" doesn't require any license fee,     |
 |    and allows you to                                                      |
 |    - develop and distribute PDFlib-based software for which the complete  |
 |      source code is made available                                        |
 |    - redistribute PDFlib non-commercially under certain conditions        |
 |    - redistribute PDFlib on digital media for a fee if the complete       |
 |      contents of the media are freely redistributable                     |
 |    Details can be found in the file aladdin-license.pdf.                  |
 |                                                                           |
 |    These conditions extend to ports to other programming languages.       |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* $Id: p_params.c,v 1.33.2.10 2002/01/26 18:32:18 tm Exp $
 *
 * PDFlib parameter handling
 *
 */

#include <string.h>

#include "p_intern.h"
#include "p_font.h"
#include "p_image.h"

/*
 * PDF_get_parameter() and PDF_set_parameter() deal with strings,
 * PDF_get_value() and PDF_set_value() deal with numerical values.
 */

#define pdf_state_all							\
    (pdf_state_object | pdf_state_document | pdf_state_page |		\
    pdf_state_pattern | pdf_state_template | pdf_state_path)

typedef struct
{
    char *	name;		/* parameter name */
    pdf_bool	mod_zero;	/* PDF_get_() modifier argument must be 0 */
    int		get_scope;	/* bit mask of legal states for PDF_get_() */
    int		set_scope;	/* bit mask of legal states for PDF_set_() */
} pdf_parm_descr;

static pdf_parm_descr parms[] =
{
#define pdf_gen_parm_descr	1
#include "p_params.h"
#undef	pdf_gen_parm_descr

    { "", 0, 0, 0 }
};

enum
{
#define pdf_gen_parm_enum	1
#include "p_params.h"
#undef	pdf_gen_parm_enum

    PDF_PARAMETER_LIMIT
};

static void
pdf_pscope_error(PDF *p, const char *parmname, const char *action)
{
    if (PDF_GET_STATE(p) == pdf_state_error)
	return;

    pdf_error(p, PDF_RuntimeError, "Can't %s parameter '%s' in '%s' scope",
	action, parmname, pdf_current_scope(p));
}

#define PDF_CHECK_SET_SCOPE(p, pn, s)					\
	if (((p)->state_stack[(p)->state_sp] & (s)) == 0)		\
	    pdf_pscope_error(p, pn, "set")

#define PDF_CHECK_GET_SCOPE(p, pn, s)					\
	if (((p)->state_stack[(p)->state_sp] & (s)) == 0)		\
	    pdf_pscope_error(p, pn, "get")

static int
get_index(const char *key)
{
    int i;

    for (i = 0; i < PDF_PARAMETER_LIMIT; ++i)
	if (strcmp(key, parms[i].name) == 0)
	    return i;

    return -1;
}

static pdf_bool
pdf_bool_value(PDF *p, const char *key, const char *value)
{
    if (!strcmp(value, "true"))
	return pdf_true;

    if (!strcmp(value, "false"))
	return pdf_false;
    
    pdf_error(p, PDF_ValueError,
	"Illegal value '%s' for parameter '%s'", value, key);

    return pdf_false;		/* compilers love it */
}

PDFLIB_API void PDFLIB_CALL
PDF_set_parameter(PDF *p, const char *key, const char *value)
{
    int i;

    PDF_TRACE(("PDF_set_parameter\t(pdf[%p], \"%s\", \"%s\");\n",
	(void *) p, key, value));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (key == NULL || !*key)
	pdf_error(p, PDF_ValueError, "Tried to set NULL parameter");

    if ((i = get_index(key)) == -1)
	pdf_error(p, PDF_ValueError,
	    "Tried to set unknown parameter '%s'", key);

    if (value == NULL || !*value)
	pdf_error(p, PDF_ValueError,
	    "Tried to set parameter '%s' to NULL value", key);

    PDF_CHECK_SET_SCOPE(p, key, parms[i].set_scope);

    switch (i)
    {
	case PDF_PARAMETER_RESOURCEFILE:
	    if (p->resourcefilename)
		pdf_error(p, PDF_RuntimeError,
		    "parameter 'resourcefile' can be set only once");

	    p->resourcefilename = pdf_strdup(p, value);
	    break;

	case PDF_PARAMETER_FONTAFM:
	case PDF_PARAMETER_FONTPFM:
	case PDF_PARAMETER_FONTOUTLINE:
	case PDF_PARAMETER_ENCODING:
	{
	    /* don't manipulate the caller's buffer */
	    char *resource = pdf_strdup(p, value);
	    char *filename;

	    if ((filename = strchr(resource, '=')) == NULL) {
		p->free(p, resource);
		pdf_error(p, PDF_ValueError, "Bogus resource line");
	    }
	    *filename++ = '\0';

	    /* special syntax: "==" means absolute path name */
	    if (*filename == '=') {
		filename++;		/* skip '=' character */
		pdf_add_resource(p, key, resource, filename, NULL);
	    } else {
		pdf_add_resource(p, key, resource, filename, p->prefix);
	    }

	    p->free(p, resource);
	    break;
	}

	case PDF_PARAMETER_FLUSH:
	    if (p->binding != NULL && strcmp(p->binding, "C++"))
		pdf_error(p, PDF_ValueError,
		"Flush points can only be set from C or C++ language bindings");

	    if (!strcmp(value, "none"))
		p->flush = PDF_FLUSH_NONE;
	    else if (!strcmp(value, "page"))
		p->flush |= PDF_FLUSH_PAGE;
	    else if (!strcmp(value, "content"))
		p->flush |= PDF_FLUSH_CONTENT;
	    else if (!strcmp(value, "heavy"))
		p->flush |= PDF_FLUSH_HEAVY;
	    else
		pdf_error(p, PDF_ValueError, "Bogus flush point %s", value);

	    break;

	case PDF_PARAMETER_DEBUG:
	{
	    const unsigned char *c;

	    for (c = (const unsigned char *) value; *c; c++) {
		p->debug[(int) *c] = 1;
		if (*c == 'u') {
		    PDF_CHECK_SET_SCOPE(p, key, pdf_state_document);
		    fflush(p->fp);
#ifndef WINCE
		    setbuf(p->fp, (char *) 0);
#endif
		}
	    }
	    break;
	}

	case PDF_PARAMETER_NODEBUG:
	{
	    const unsigned char *c;

	    for (c = (const unsigned char *) value; *c; c++)
		p->debug[(int) *c] = 0;

	    break;
	}

	case PDF_PARAMETER_BINDING:
	    if (!p->binding)
		p->binding = pdf_strdup(p, value);
	    break;

	case PDF_PARAMETER_INERROR:
	    PDF_SET_STATE(p, fn, pdf_state_error);
	    break;

	case PDF_PARAMETER_UNDERLINE:
	    p->underline = pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_OVERLINE:
	    p->overline = pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_STRIKEOUT:
	    p->strikeout = pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_COMPATIBILITY:
	    if (!strcmp(value, "1.2"))
		p->compatibility = PDF_1_2;
	    else if (!strcmp(value, "1.3"))
		p->compatibility = PDF_1_3;
	    else if (!strcmp(value, "1.4"))
		p->compatibility = PDF_1_4;
	    else
		pdf_error(p, PDF_ValueError,
		    "Unknown compatibility level '%s'", value);
	    break;

	case PDF_PARAMETER_PREFIX:
	    if (p->prefix)
		pdf_error(p, PDF_RuntimeError,
		    "parameter 'prefix' can be set only once");

	    p->prefix = pdf_strdup(p, value);
	    break;

	case PDF_PARAMETER_WARNING:
	    p->debug['w'] = (char) pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_FONTWARNING:
	    p->debug['F'] = (char) pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_IMAGEWARNING:
	    p->debug['i'] = (char) pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_PDIWARNING:
	    p->debug['p'] = (char) pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_INHERITGSTATE:
	    p->inheritgs = pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_PDISTRICT:
	    p->pdi_strict = pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_PASSTHROUGH:
	    p->debug['P'] = (char) !pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_OPENACTION:
	    if (!strcmp(value, "retain")) {
		p->open_action = retain;
	    } else if (!strcmp(value, "fitpage")) {
		p->open_action = fitpage;
	    } else if (!strcmp(value, "fitwidth")) {
		p->open_action = fitwidth;
	    } else if (!strcmp(value, "fitheight")) {
		p->open_action = fitheight;
	    } else if (!strcmp(value, "fitbbox")) {
		p->open_action = fitbbox;
	    } else {
		pdf_error(p, PDF_ValueError, "Unknown open action '%s'", value);
	    }
	    break;

	case PDF_PARAMETER_OPENMODE:
	    if (!strcmp(value, "none")) {
		p->open_mode = open_none;
	    } else if (!strcmp(value, "bookmarks")) {
		p->open_mode = open_bookmarks;
	    } else if (!strcmp(value, "thumbnails")) {
		p->open_mode = open_thumbnails;
	    } else if (!strcmp(value, "fullscreen")) {
		p->open_mode = open_fullscreen;
	    } else {
		pdf_error(p, PDF_ValueError, "Unknown open mode '%s'", value);
	    }
	    break;

	case PDF_PARAMETER_BOOKMARKDEST:
	    if (!strcmp(value, "retain")) {
		p->bookmark_dest = retain;
	    } else if (!strcmp(value, "fitpage")) {
		p->bookmark_dest = fitpage;
	    } else if (!strcmp(value, "fitwidth")) {
		p->bookmark_dest = fitwidth;
	    } else if (!strcmp(value, "fitheight")) {
		p->bookmark_dest = fitheight;
	    } else if (!strcmp(value, "fitbbox")) {
		p->bookmark_dest = fitbbox;
	    } else {
		pdf_error(p, PDF_ValueError,
		    "Unknown bookmark destination '%s'", value);
	    }
	    break;

	case PDF_PARAMETER_FILLRULE:
	    if (!strcmp(value, "winding")) {
		p->fillrule = pdf_fill_winding;
	    } else if (!strcmp(value, "evenodd")) {
		p->fillrule = pdf_fill_evenodd;
	    } else {
		pdf_error(p, PDF_ValueError, "Unknown fill rule '%s'", value);
	    }
	    break;

	/*
	 * check native unicode mode: this will be handled in the wrapper code
	 * of those language bindings which natively support Unicode. We simply
	 * catch typos in the boolean value here, or complain for all
	 * non-Unicode aware language bindings.
	 */
	case PDF_PARAMETER_NATIVEUNICODE:
	    if (!strcmp(value, "true") || !strcmp(value, "false")) {
		pdf_error(p, PDF_NonfatalError,
		    "Unicode mode setting not required in this configuration");
	    } else {
		pdf_error(p, PDF_ValueError,
		    "Unknown Unicode mode setting '%s'", value);
	    }
	    break;

	case PDF_PARAMETER_TRANSITION:
	    pdf_set_transition(p, value);
	    break;

	case PDF_PARAMETER_BASE:
	    if (p->base)
		p->free(p, p->base);

	    p->base = pdf_strdup(p, value);
	    break;

	case PDF_PARAMETER_SERIAL:
	    break;

	default:
	    pdf_error(p, PDF_ValueError, "Can't set parameter '%s'", key);
	    break;
    } /* switch */
}

static float
pdf_pos_value(PDF *p, const char *key, float value, int minver)
{
    if (p->compatibility < minver)
	pdf_error(p, PDF_RuntimeError,
	    "Parameter '%s' requires PDF%d.%d or higher",
		key, minver / 10, minver % 10);

    if (value <= (float) 0)
	pdf_error(p, PDF_ValueError,
	    "Illegal value %f for parameter '%s'", value, key);

    return value;
}

PDFLIB_API void PDFLIB_CALL
PDF_set_value(PDF *p, const char *key, float value)
{
    int i;
    int ivalue = (int) value;

    PDF_TRACE(("PDF_set_value\t(pdf[%p], \"%s\", \"%f\");\n",
	(void *) p, key, value));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (key == NULL || !*key)
	pdf_error(p, PDF_ValueError, "Tried to set NULL parameter");

    if ((i = get_index(key)) == -1)
	pdf_error(p, PDF_ValueError,
	    "Tried to set unknown parameter '%s'", key);

    PDF_CHECK_SET_SCOPE(p, key, parms[i].set_scope);

    switch (i)
    {
	case PDF_PARAMETER_COMPRESS:
	    if (ivalue < 0 || ivalue > 9)
		pdf_error(p, PDF_ValueError,
		    "Bogus compression level %f", value);

#ifdef	HAVE_LIBZ
	    /*
	     * We must restart the compression engine and start a new
	     * contents section if we're in the middle of a page.
	     */
	    if (PDF_GET_STATE(p) == pdf_state_page) {
		pdf_end_contents_section(p);
		p->compresslevel = ivalue;
		pdf_begin_contents_section(p);
	    } else 
		p->compresslevel = ivalue;
#endif	/* HAVE_LIBZ */

	    break;

	case PDF_PARAMETER_FLOATDIGITS:
	    if (3 <= ivalue && ivalue <= 6)
		p->floatdigits = ivalue;
	    else
		pdf_error(p, PDF_ValueError,
		    "Invalid floating point output precision %f", value);
	    break;

	case PDF_PARAMETER_PAGEWIDTH:
	    if (p->compatibility >= PDF_1_3 &&
		(value < PDF_ACRO4_MINPAGE || value > PDF_ACRO4_MAXPAGE))
		    pdf_error(p, PDF_NonfatalError,
			"Page width incompatible with Acrobat 4");

	    if (p->compatibility == PDF_1_2 &&
		(value < PDF_ACRO3_MINPAGE || value > PDF_ACRO3_MAXPAGE))
		    pdf_error(p, PDF_RuntimeError,
			"Page width incompatible with Acrobat 3");

	    p->width = pdf_pos_value(p, key, value, PDF_1_2);
	    break;

	case PDF_PARAMETER_PAGEHEIGHT:
	    if (p->compatibility >= PDF_1_3 &&
		(value < PDF_ACRO4_MINPAGE || value > PDF_ACRO4_MAXPAGE))
		    pdf_error(p, PDF_NonfatalError,
			"Page height incompatible with Acrobat 4");

	    if (p->compatibility == PDF_1_2 &&
		(value < PDF_ACRO3_MINPAGE || value > PDF_ACRO3_MAXPAGE))
		    pdf_error(p, PDF_RuntimeError,
			"Page height incompatible with Acrobat 3");

	    p->height = pdf_pos_value(p, key, value, PDF_1_2);
	    break;

	case PDF_PARAMETER_CROPBOX_LLX:
	    p->CropBox.llx = pdf_pos_value(p, key, value, PDF_1_2);
	    break;

	case PDF_PARAMETER_CROPBOX_LLY:
	    p->CropBox.lly = pdf_pos_value(p, key, value, PDF_1_2);
	    break;

	case PDF_PARAMETER_CROPBOX_URX:
	    p->CropBox.urx = pdf_pos_value(p, key, value, PDF_1_2);
	    break;

	case PDF_PARAMETER_CROPBOX_URY:
	    p->CropBox.ury = pdf_pos_value(p, key, value, PDF_1_2);
	    break;

	case PDF_PARAMETER_BLEEDBOX_LLX:
	    p->BleedBox.llx = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_BLEEDBOX_LLY:
	    p->BleedBox.lly = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_BLEEDBOX_URX:
	    p->BleedBox.urx = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_BLEEDBOX_URY:
	    p->BleedBox.ury = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_TRIMBOX_LLX:
	    p->TrimBox.llx = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_TRIMBOX_LLY:
	    p->TrimBox.lly = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_TRIMBOX_URX:
	    p->TrimBox.urx = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_TRIMBOX_URY:
	    p->TrimBox.ury = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_ARTBOX_LLX:
	    p->ArtBox.llx = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_ARTBOX_LLY:
	    p->ArtBox.lly = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_ARTBOX_URX:
	    p->ArtBox.urx = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_ARTBOX_URY:
	    p->ArtBox.ury = pdf_pos_value(p, key, value, PDF_1_3);
	    break;

	case PDF_PARAMETER_LEADING:
	    pdf_set_leading(p, value);
	    break;

	case PDF_PARAMETER_TEXTRISE:
	    pdf_set_text_rise(p, value);
	    break;

	case PDF_PARAMETER_HORIZSCALING:
	    pdf_set_horiz_scaling(p, value);
	    break;

	case PDF_PARAMETER_TEXTRENDERING:
	    pdf_set_text_rendering(p, (int) value);
	    break;

	case PDF_PARAMETER_CHARSPACING:
	    pdf_set_char_spacing(p, value);
	    break;

	case PDF_PARAMETER_WORDSPACING:
	    pdf_set_word_spacing(p, value);
	    break;

	case PDF_PARAMETER_DURATION:
	    pdf_set_duration(p, value);
	    break;

	default:
	    pdf_error(p, PDF_ValueError, "Can't set parameter '%s'", key);
	    break;
    } /* switch */
}

PDFLIB_API float PDFLIB_CALL
PDF_get_value(PDF *p, const char *key, float mod)
{
    int i = -1;
    int imod = (int) mod;
    float result = (float) 0;

    PDF_TRACE(("PDF_get_value\t(pdf[%p], \"%s\", %f);", (void *) p, key, mod));

    /* some parameters can be retrieved with p == 0.
    */
    if (key != NULL && (i = get_index(key)) != -1)
    {
	switch (i)
	{
	    case PDF_PARAMETER_MAJOR:
		result = PDFLIB_MAJORVERSION;
		PDF_TRACE((" [%f]\n", result));
		return result;

	    case PDF_PARAMETER_MINOR:
		result = PDFLIB_MINORVERSION;
		PDF_TRACE((" [%f]\n", result));
		return result;

	    case PDF_PARAMETER_REVISION:
		result = PDFLIB_REVISION;
		PDF_TRACE((" [%f]\n", result));
		return result;
	} /* switch */
    }

    if (PDF_SANITY_CHECK_FAILED(p))
	return (float) 0;

    if (key == NULL || !*key)
	pdf_error(p, PDF_ValueError, "Tried to get NULL parameter");

    if (i == -1)
	pdf_error(p, PDF_ValueError,
	    "Tried to get unknown parameter '%s'", key);

    PDF_CHECK_GET_SCOPE(p, key, parms[i].get_scope);

    if (parms[i].mod_zero && mod != (float) 0)
	pdf_error(p, PDF_ValueError,
		"Bad argument %f in PDF_get_value/%s", mod, key);

    switch (i)
    {
	case PDF_PARAMETER_IMAGEWIDTH:
	    if (imod < 0 || imod >= p->images_capacity
		|| !p->images[imod].in_use) {
		    pdf_error(p, PDF_ValueError,
			"Bad image number %d in PDF_get_value/%s", imod, key);
	    }
	    result = (float) (p->images[imod].width);
	    break;

	case PDF_PARAMETER_IMAGEHEIGHT:
	    if (imod < 0 || imod >= p->images_capacity
		|| !p->images[imod].in_use) {
		    pdf_error(p, PDF_ValueError,
			"Bad image number %d in PDF_get_value/%s", imod, key);
	    }
	    result = (float) (p->images[imod].height);
	    break;

	case PDF_PARAMETER_RESX:
	    if (imod < 0 || imod >= p->images_capacity
		|| !p->images[imod].in_use) {
		    pdf_error(p, PDF_ValueError,
			"Bad image number %d in PDF_get_value/%s", imod, key);
	    }
	    result = (float) (p->images[imod].dpi_x);
	    break;

	case PDF_PARAMETER_RESY:
	    if (imod < 0 || imod >= p->images_capacity
		|| !p->images[imod].in_use) {
		    pdf_error(p, PDF_ValueError,
			"Bad image number %d in PDF_get_value/%s", imod, key);
	    }
	    result = (float) (p->images[imod].dpi_y);
	    break;

	case PDF_PARAMETER_CURRENTX:
	    result = (float) (p->gstate[p->sl].x);
	    break;

	case PDF_PARAMETER_CURRENTY:
	    result = (float) (p->gstate[p->sl].y);
	    break;

	case PDF_PARAMETER_TEXTX:
	    result = (float) (p->tstate[p->sl].m.e);
	    break;

	case PDF_PARAMETER_TEXTY:
	    result = (float) (p->tstate[p->sl].m.f);
	    break;

	case PDF_PARAMETER_WORDSPACING:
	    result = (float) (p->tstate[p->sl].w);
	    break;

	case PDF_PARAMETER_CHARSPACING:
	    result = (float) (p->tstate[p->sl].c);
	    break;

	case PDF_PARAMETER_HORIZSCALING:
	    result = (float) (p->tstate[p->sl].h * (float) 100);
	    break;

	case PDF_PARAMETER_TEXTRISE:
	    result = (float) (p->tstate[p->sl].rise);
	    break;

	case PDF_PARAMETER_LEADING:
	    result = (float) (p->tstate[p->sl].l);
	    break;

	case PDF_PARAMETER_TEXTRENDERING:
	    result = (float) (p->tstate[p->sl].mode);
	    break;

	case PDF_PARAMETER_FONT:
	    result = (float) (pdf_get_font(p));
	    break;

	case PDF_PARAMETER_FONTSIZE:
	    result = (float) (pdf_get_fontsize(p));
	    break;

	case PDF_PARAMETER_CAPHEIGHT:
	    if (imod < 0 || imod >= p->fonts_number) {
		pdf_error(p, PDF_ValueError,
		    "Bad font descriptor %d in PDF_get_value/%s", imod, key);
	    }
	    result = (float) (p->fonts[imod].capHeight / (float) 1000);
	    break;

	case PDF_PARAMETER_ASCENDER:
	    if (imod < 0 || imod >= p->fonts_number) {
		pdf_error(p, PDF_ValueError,
		    "Bad font descriptor %d in PDF_get_value/%s", imod, key);
	    }
	    result = (float) (p->fonts[imod].ascender / (float) 1000);
	    break;

	case PDF_PARAMETER_DESCENDER:
	    if (imod < 0 || imod >= p->fonts_number) {
		pdf_error(p, PDF_ValueError,
		    "Bad font descriptor %d in PDF_get_value/%s", imod, key);
	    }
	    result = (float) (p->fonts[imod].descender / (float) 1000);
	    break;

	default:
	    pdf_error(p, PDF_ValueError, "Can't get parameter '%s'", key);
	    break;
    } /* switch */

    PDF_TRACE((" [%f]\n", result));
    return result;
}

PDFLIB_API const char * PDFLIB_CALL
PDF_get_parameter(PDF *p, const char *key, float mod)
{
    int i = -1;
    const char *result = NULL;

    PDF_TRACE(("PDF_get_parameter\t(pdf[%p], \"%s\", %f);",
	(void *) p, key, mod));

    /* some parameters can be retrieved with p == 0.
    */
    if (key != NULL && (i = get_index(key)) != -1)
    {
	switch (i)
	{
	    case PDF_PARAMETER_VERSION:
		result = PDFLIB_VERSIONSTRING;
		PDF_TRACE((" [%s]\n", result));
		return result;

	    case PDF_PARAMETER_PDI:
#ifdef HAVE_PDI
		result = "true";
#else
		result = "false";
#endif
		PDF_TRACE((" [%s]\n", result));
		return result;
	} /* switch */
    }

    if (PDF_SANITY_CHECK_FAILED(p))
	return (const char *) NULL;

    if (key == NULL || !*key)
	pdf_error(p, PDF_ValueError, "Tried to get NULL parameter");

    if (i == -1)
	pdf_error(p, PDF_ValueError,
	    "Tried to get unknown parameter '%s'", key);

    PDF_CHECK_GET_SCOPE(p, key, parms[i].get_scope);

    if (parms[i].mod_zero && mod != (float) 0)
	pdf_error(p, PDF_ValueError,
		"Bad argument %f in PDF_get_value/%s", mod, key);

    switch (i)
    {
	case PDF_PARAMETER_FONTNAME:
	    result = pdf_get_fontname(p);
	    break;

	case PDF_PARAMETER_FONTENCODING:
	    result = pdf_get_fontencoding(p);
	    break;
     
	case PDF_PARAMETER_UNDERLINE:
	    result = (p->underline ? "true" : "false");
	    break;

	case PDF_PARAMETER_OVERLINE:
	    result = (p->overline ? "true" : "false");
	    break;

	case PDF_PARAMETER_STRIKEOUT:
	    result = (p->strikeout ? "true" : "false");
	    break;

	case PDF_PARAMETER_INHERITGSTATE:
	    result = (p->inheritgs ? "true" : "false");
	    break;

	default:
	    pdf_error(p, PDF_ValueError, "Can't get parameter '%s'", key);
	    break;
    } /* switch */

    PDF_TRACE((" [%s]\n", result));
    return result;
}
