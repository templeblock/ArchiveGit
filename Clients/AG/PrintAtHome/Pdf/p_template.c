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

/* $Id: p_template.c,v 1.24.2.6 2002/01/26 18:32:18 tm Exp $
 *
 * PDFlib template routines
 *
 */

#include "p_intern.h"
#include "p_image.h"

/* Start a new template definition. */
PDFLIB_API int PDFLIB_CALL
PDF_begin_template(PDF *p, float width, float height)
{
    static const char fn[] = "PDF_begin_template";
    pdf_image *image;
    int im;

    PDF_TRACE(("%s\t(pdf[%p], %f, %f);\n", fn, (void *) p, width, height));

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    PDF_CHECK_SCOPE(p, fn, pdf_state_document);
    
    if (width <= 0 || height <= 0)
	pdf_error(p, PDF_ValueError, "Template size must be positive");
    
    for (im = 0; im < p->images_capacity; im++)
	if (!p->images[im].in_use)		/* found free slot */
	    break;

    if (im == p->images_capacity) 
	pdf_grow_images(p);

    image		= &p->images[im];
    image->in_use	= pdf_true;		/* mark slot as used */
    image->no		= pdf_new_xobject(p, form_xobject, NEW_ID);
    image->width	= width;
    image->height	= height;

    p->height		= height;
    p->width		= width;
    p->thumb_id		= BAD_ID;
    PDF_PUSH_STATE(p, fn, pdf_state_template);
    p->templ		= im;		/* remember the current template id */
    p->next_content	= 0;
    p->contents 	= c_page;
    p->procset		= 0;
    p->sl		= 0;

    pdf_init_tstate(p);
    pdf_init_gstate(p);
    pdf_init_cstate(p);

    pdf_begin_dict(p);				/* form xobject dict*/
    pdf_puts(p, "/Type/XObject\n");
    pdf_puts(p, "/Subtype/Form\n");

    /* contrary to the PDF reference /FormType and /Matrix are required! */
    pdf_printf(p, "/FormType 1\n");
    pdf_printf(p, "/Matrix[1 0 0 1 0 0]\n");

    p->res_id = pdf_alloc_id(p);
    pdf_printf(p, "/Resources %ld 0 R\n", p->res_id);

    pdf_printf(p, "/BBox[0 0 %f %f]\n", p->width, p->height);
	
    p->contents_length_id = pdf_alloc_id(p);
    pdf_printf(p, "/Length %ld 0 R\n", p->contents_length_id);

    if (p->compresslevel)
	pdf_puts(p, "/Filter/FlateDecode\n");

    pdf_end_dict(p);				/* form xobject dict*/
    pdf_begin_stream(p);

    pdf_compress_init(p);

    p->start_contents_pos = pdf_tell(p);
    p->next_content++;

    return im;
}

/* Finish the template definition. */
PDFLIB_API void PDFLIB_CALL
PDF_end_template(PDF *p)
{
    static const char fn[] = "PDF_end_template";
    long length;

    PDF_TRACE(("%s\t(pdf[%p]);\n", fn, (void *) p));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_template);
    
    /* check whether PDF_save() and PDF_restore() calls are balanced */
    if (p->sl > 0)
	pdf_error(p, PDF_RuntimeError,
	    "Unmatched save level at end of template");

    pdf_end_text(p);
    p->contents = c_none;

    if (p->compresslevel)
	pdf_compress_end(p);
					
    length = pdf_tell(p) - p->start_contents_pos;
    pdf_end_stream(p);
    pdf_end_obj(p);				/* form xobject */

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

#ifndef HAVE_PDI

PDFLIB_API int PDFLIB_CALL
PDF_open_pdi(
    PDF *p,
    const char *filename,
    const char *stringparam,
    int intparam)
{
    (void) filename;
    (void) stringparam;
    (void) intparam;

    PDF_TRACE(("PDF_open_pdi\t(pdf[%p], \"%s,\" \"%s\", %d);\n",
	(void *) p, filename, stringparam, intparam));

    pdf_error(p, PDF_NonfatalError,
    "Manipulating existing PDFs requires the PDI feature (see www.pdflib.com)");

    PDF_TRACE((" [%d]\n", -1));
    return -1;
}

PDFLIB_API void PDFLIB_CALL
PDF_close_pdi(PDF *p, int doc)
{
    (void) doc;
    (void) p;

    PDF_TRACE(("PDF_close_pdi\t(pdf[%p], %d);\n", (void *) p, doc));

    return;
}

PDFLIB_API int PDFLIB_CALL
PDF_open_pdi_page( PDF *p, int doc, int pagenum, const char *label)
{
    (void) doc;
    (void) pagenum;
    (void) label;

    PDF_TRACE(("PDF_open_pdi_page\t(pdf[%p], %d, %d, \"%s\");\n",
	(void *) p, doc, pagenum, label));

    pdf_error(p, PDF_NonfatalError,
    "Manipulating existing PDFs requires the PDI feature (see www.pdflib.com)");

    PDF_TRACE((" [%d]\n", -1));
    return -1;
}

PDFLIB_API void PDFLIB_CALL
PDF_place_pdi_page(PDF *p, int page, float x, float y, float sx, float sy)
{
    (void) p;
    (void) page;
    (void) x;
    (void) y;
    (void) sx;
    (void) sy;

    PDF_TRACE(("PDF_place_pdi_page\t(pdf[%p], %d, %f, %f, %f, %f);\n",
	(void *) p, page, x, y, sx, sy));

    return;
}

PDFLIB_API void PDFLIB_CALL
PDF_close_pdi_page(PDF *p, int page)
{
    (void) p;
    (void) page;

    PDF_TRACE(("PDF_close_pdi_page\t(pdf[%p], %d);\n", (void *) p, page));

    return;
}

PDFLIB_API const char *PDFLIB_CALL
PDF_get_pdi_parameter(
    PDF *p,
    const char *key,
    int doc,
    int page,
    int index,
    int *len)
{
    PDF_TRACE(("PDF_get_pdi_parameter\t(pdf[%p], \"%s\", %d, %d, %d, %d);\n",
	(void *) p, key, doc, page, index, len));

    (void) p;
    (void) key;
    (void) doc;
    (void) page;
    (void) index;
    (void) len;

    return (const char *) NULL;
}

PDFLIB_API float PDFLIB_CALL
PDF_get_pdi_value(PDF *p, const char *key, int doc, int page, int index)
{
    PDF_TRACE(("PDF_get_pdi_value\t(pdf[%p], \"%s\", %d, %d, %d);\n",
	(void *) p, key, doc, page, index));

    (void) p;
    (void) key;
    (void) doc;
    (void) page;
    (void) index;

    return (float) 0;
}

#endif	/* !HAVE_PDI */
