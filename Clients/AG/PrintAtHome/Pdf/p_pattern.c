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

/* $Id: p_pattern.c,v 1.19.2.3 2002/01/26 18:32:18 tm Exp $
 *
 * PDFlib pattern routines
 *
 */

#include <string.h>

#include "p_intern.h"

struct pdf_pattern_s {
    id		obj_id;			/* object id of this pattern */
    int		painttype;		/* colored or uncolored */
    int		csindex;		/* index in color space list */
    pdf_bool	used_on_current_page;	/* this pattern used on current page */
};

void
pdf_init_pattern(PDF *p)
{
    int i;

    p->pattern_number = 0;
    p->pattern_capacity = PATTERN_CHUNKSIZE;

    p->pattern = (pdf_pattern *) p->malloc(p,
	sizeof(pdf_pattern) * p->pattern_capacity, "pdf_init_pattern");

    for (i = 0; i < p->pattern_capacity; i++) {
	p->pattern[i].used_on_current_page = pdf_false;
	p->pattern[i].obj_id = BAD_ID;
    }
}

static void
pdf_grow_pattern(PDF *p)
{
    int i;

    p->pattern = (pdf_pattern *) p->realloc(p, p->pattern,
	sizeof(pdf_pattern) * 2 * p->pattern_capacity, "pdf_grow_pattern");

    for (i = p->pattern_capacity; i < 2 * p->pattern_capacity; i++) {
	p->pattern[i].used_on_current_page = pdf_false;
	p->pattern[i].obj_id = BAD_ID;
    }

    p->pattern_capacity *= 2;
}

void
pdf_write_page_pattern(PDF *p)
{
    int i, total = 0;

    for (i = 0; i < p->pattern_number; i++)
	if (p->pattern[i].used_on_current_page)
	    total++;

    if (total > 0) {
	pdf_puts(p, "/Pattern");

	pdf_begin_dict(p);			/* pattern */

	for (i = 0; i < p->pattern_number; i++) {
	    if (p->pattern[i].used_on_current_page) {
		p->pattern[i].used_on_current_page = pdf_false; /* reset */
		pdf_printf(p, "/P%d %ld 0 R\n", i, p->pattern[i].obj_id);
	    }
	}

	pdf_end_dict(p);			/* pattern */
    }
}

void
pdf_cleanup_pattern(PDF *p)
{
    if (p->pattern) {
	p->free(p, p->pattern);
	p->pattern = NULL;
    }
}

/* Start a new pattern definition. */
PDFLIB_API int PDFLIB_CALL
PDF_begin_pattern(
    PDF *p,
    float width,
    float height,
    float xstep,
    float ystep,
    int painttype)
{
    static const char fn[] = "PDF_begin_pattern";

    PDF_TRACE(("%s\t(pdf[%p], %f, %f, %f, %f, %d);\n",
	fn, (void *) p, width, height, xstep, ystep, painttype));

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    PDF_CHECK_SCOPE(p, fn, pdf_state_document);
    
    if (width <= 0 || height <= 0)
	pdf_error(p, PDF_ValueError, "Pattern size must be positive");
    
    /* Acrobat 3 doesn't display patterns on screen so we disable it */
    if (p->compatibility == PDF_1_2)
	pdf_error(p, PDF_RuntimeError,
	    "Patterns not fully supported in Acrobat 3");

    if (painttype != 1 && painttype != 2)
	pdf_error(p, PDF_ValueError,
	    "Bad pattern paint type %d in PDF_begin_pattern", painttype);

    if (p->pattern_number == p->pattern_capacity)
	pdf_grow_pattern(p);

    p->pattern[p->pattern_number].obj_id = pdf_begin_obj(p, NEW_ID);
    p->pattern[p->pattern_number].painttype = painttype;

    p->height		= height;
    p->width		= width;
    p->thumb_id		= BAD_ID;
    PDF_PUSH_STATE(p, fn, pdf_state_pattern);
    p->next_content	= 0;
    p->contents 	= c_page;
    p->procset		= 0;
    p->sl		= 0;

    pdf_init_tstate(p);
    pdf_init_gstate(p);
    pdf_init_cstate(p);

    pdf_begin_dict(p);				/* pattern dict*/

    p->res_id = pdf_alloc_id(p);
	
    pdf_puts(p, "/PatternType 1\n");

    /* colored or uncolored pattern */
    pdf_printf(p, "/PaintType %d\n", painttype);
    pdf_puts(p, "/TilingType 1\n");		/* constant spacing */

    pdf_printf(p, "/BBox[0 0 %f %f]\n", p->width, p->height);

    pdf_printf(p, "/XStep %f\n", xstep);
    pdf_printf(p, "/YStep %f\n", ystep);

    pdf_printf(p, "/Resources %ld 0 R\n", p->res_id);

    p->contents_length_id = pdf_alloc_id(p);
    pdf_printf(p, "/Length %ld 0 R\n", p->contents_length_id);

    if (p->compresslevel)
	pdf_puts(p, "/Filter/FlateDecode\n");

    pdf_end_dict(p);				/* pattern dict*/
    pdf_begin_stream(p);

    pdf_compress_init(p);

    p->start_contents_pos = pdf_tell(p);
    p->next_content++;

    return p->pattern_number++;
}

/* Finish the pattern definition. */
PDFLIB_API void PDFLIB_CALL
PDF_end_pattern(PDF *p)
{
    static const char fn[] = "PDF_end_pattern";
    long length;

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_pattern);

    /* check whether PDF_save() and PDF_restore() calls are balanced */
    if (p->sl > 0)
	pdf_error(p, PDF_RuntimeError,
	    "Unmatched save level at end of pattern");

    pdf_end_text(p);
    p->contents = c_none;

    if (p->compresslevel)
	pdf_compress_end(p);
					
    length = pdf_tell(p) - p->start_contents_pos;
    pdf_end_stream(p);
    pdf_end_obj(p);				/* pattern */

    pdf_begin_obj(p, p->contents_length_id);	/* Length object */
    pdf_printf(p, "%ld\n", length);
    pdf_end_obj(p);
  
    pdf_begin_obj(p, p->res_id);		/* Resource object */
    pdf_begin_dict(p);				/* Resource dict */

    pdf_write_page_procsets(p);			/* ProcSet resources */

    pdf_write_page_fonts(p);			/* Font resources */

    pdf_write_page_colorspaces(p);		/* Color space resources */

    pdf_write_page_pattern(p);			/* Pattern resources */

    pdf_write_xobjects(p);			/* XObject resources */

    pdf_end_dict(p);				/* resource dict */
    pdf_end_obj(p);				/* resource object */

    PDF_POP_STATE(p, fn);

    if (p->flush & PDF_FLUSH_PAGE)
	pdf_flush_stream(p);
}

static void
pdf_write_pattern_color(PDF *p, pdf_bool fill)
{
    pdf_color color;

    /* fetch the current fill or stroke color and apply it to the pattern */
    if (fill)
	color = p->cstate[p->sl].fill;
    else
	color = p->cstate[p->sl].stroke;

    if (color.cs == DeviceGray || color.cs == Separation) {
	pdf_printf(p, "%f", color.val.gray);

    } else if (color.cs == DeviceRGB) {
	pdf_printf(p, "%f %f %f",
			color.val.rgb.r, color.val.rgb.g, color.val.rgb.b);

    } else if (color.cs == DeviceCMYK) {
	pdf_printf(p, "%f %f %f %f",
			color.val.cmyk.c, color.val.cmyk.m,
			color.val.cmyk.y, color.val.cmyk.k);

    } else {
	pdf_error(p, PDF_SystemError,
	    "Unknown color space in pdf_write_pattern_color");
    }
}

void
pdf_setpatterncolor(PDF *p, const char *type, int pattern)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (!type || !*type)
	pdf_error(p, PDF_ValueError, "Missing type in pdf_setspotcolor");

    if (strcmp(type, "fill") && strcmp(type, "stroke") && strcmp(type, "both"))
	pdf_error(p, PDF_ValueError, "Unknown type in pdf_spotcolor");

    if (pattern < 0 || pattern >= p->pattern_number)
	pdf_error(p, PDF_ValueError,
	    "Invalid pattern number %d in PDF_setcolor", pattern);

    if (PDF_GET_STATE(p) == pdf_state_pattern
	&& pattern == p->pattern_number-1) {
	pdf_error(p, PDF_ValueError,
	    "Can't use pattern within its own definition");
    }

    if (!strcmp(type, "fill") || !strcmp(type, "both")) {
	if (p->pattern[pattern].painttype == 1) {
	    pdf_puts(p, "/Pattern cs\n");

	} else if (p->pattern[pattern].painttype == 2) {
	    /* TODO: find spot color name */
	    if (p->cstate[p->sl].fill.cs == Separation)
		pdf_error(p, PDF_ValueError, "Separation NYI for patterns");
	    pdf_printf(p, "/CS%d cs\n",
		pdf_add_colorspace(p, PatternCS, (char *) NULL));
		/* TODO: add spot name if based on separation color space */
	    pdf_write_pattern_color(p, pdf_true);
	}

	pdf_printf(p, "/P%d scn\n", pattern);

	if (p->pattern[pattern].painttype == 1) {
	    p->cstate[p->sl].fill.cs = PatternCS;
	    p->cstate[p->sl].fill.val.pattern = pattern;

	} else if (p->pattern[pattern].painttype == 2) {
	    /* TODO: deal with color */
	}
    }

    if (!strcmp(type, "stroke") || !strcmp(type, "both")) {
	if (p->pattern[pattern].painttype == 1) {
	    pdf_puts(p, "/Pattern CS\n");

	} else if (p->pattern[pattern].painttype == 2) {
	    /* TODO */
	    if (p->cstate[p->sl].fill.cs == Separation)
		pdf_error(p, PDF_ValueError, "Separation NYI for patterns");
	    pdf_printf(p, "/CS%d CS\n",
		pdf_add_colorspace(p, PatternCS, (char *) NULL));
		/* TODO: add spot name if based on separation color space */
	    pdf_write_pattern_color(p, pdf_false);
	}

	pdf_printf(p, "/P%d SCN\n", pattern);

	if (p->pattern[pattern].painttype == 1) {
	    p->cstate[p->sl].stroke.cs = PatternCS;
	    p->cstate[p->sl].stroke.val.pattern = pattern;

	} else if (p->pattern[pattern].painttype == 2) {
	    /* TODO */
	}
    }

    p->pattern[pattern].used_on_current_page = pdf_true;
}
