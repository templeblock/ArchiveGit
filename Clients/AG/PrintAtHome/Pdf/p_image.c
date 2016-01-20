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

/* $Id: p_image.c,v 1.45.2.11 2002/01/26 18:32:18 tm Exp $
 *
 * PDFlib image routines
 *
 */

#include <string.h>
#include <math.h>

#include "p_intern.h"
#include "p_image.h"

static const char *pdf_filter_names[] = {
    "", "LZWDecode", "RunLengthDecode", "CCITTFaxDecode",
    "DCTDecode", "FlateDecode"
};

static void
pdf_init_image_struct(PDF *p, pdf_image *image)
{
    image->transparent	= pdf_false;
    image->reference	= pdf_ref_direct;
    image->compression	= none;
    image->invert	= pdf_false;
    image->predictor	= pred_default;
    image->in_use	= pdf_false;
    image->filename	= (char *) NULL;
    image->mask		= -1;
    image->params	= (char *) NULL;
    image->dpi_x	= (float) 0;
    image->dpi_y	= (float) 0;
    image->strips	= 1;
    image->rowsperstrip	= 1;
    image->colormap	= (pdf_colormap *) NULL;
    image->colormap_id	= BAD_ID;
    image->dochandle	= -1;		/* this means "not a PDI page" */
    image->use_raw	= pdf_false;
    image->transval[0]	= 0;
    image->transval[1]	= 0;
    image->transval[2]	= 0;
    image->transval[3]	= 0;

    (void) p;
}

void
pdf_init_images(PDF *p)
{
    int im;

    p->images_capacity = IMAGES_CHUNKSIZE;

    p->images = (pdf_image *) 
    	p->malloc(p, sizeof(pdf_image) * p->images_capacity, "pdf_init_images");

    for (im = 0; im < p->images_capacity; im++)
	pdf_init_image_struct(p, &(p->images[im]));
}

void
pdf_grow_images(PDF *p)
{
    int im;

    p->images = (pdf_image *) p->realloc(p, p->images,
	sizeof(pdf_image) * 2 * p->images_capacity, "pdf_grow_images");

    for (im = p->images_capacity; im < 2 * p->images_capacity; im++)
	pdf_init_image_struct(p, &(p->images[im]));

    p->images_capacity *= 2;
}

void
pdf_cleanup_images(PDF *p)
{
    int im;

    if (!p->images)
	return;

    /* Free images which the caller left open */

    /* When we think of inter-document survival of images,
    ** we MUST NOT FORGET that the current TIFF algorithm
    ** depends on contiguous image slots for the image strips!
    */
    for (im = 0; im < p->images_capacity; im++)
    {
	pdf_image *img = &p->images[im];

	if (img->in_use)		/* found used slot */
	    pdf_cleanup_image(p, im);	/* free image descriptor */
    }

    p->free(p, p->images);
    p->images = NULL;
}

void
pdf_init_xobjects(PDF *p)
{
    int index;

    p->xobjects_number = 0;

    if (p->xobjects == (pdf_xobject *) 0)
    {
	p->xobjects_capacity = XOBJECTS_CHUNKSIZE;

	p->xobjects = (pdf_xobject *) 
	    p->malloc(p, sizeof(pdf_xobject) * p->xobjects_capacity,
	    "pdf_init_xobjects");
    }

    for (index = 0; index < p->xobjects_capacity; index++)
	p->xobjects[index].flags = 0;
}

int
pdf_new_xobject(PDF *p, pdf_xobj_type type, id obj_id)
{
    static const char fn[] = "pdf_new_xobject";
    int i, slot = p->xobjects_number++;

    if (slot == p->xobjects_capacity)
    {
	p->xobjects = (pdf_xobject *) p->realloc(p, p->xobjects,
	    sizeof(pdf_xobject) * 2 * p->xobjects_capacity, fn);

	for (i = p->xobjects_capacity; i < 2 * p->xobjects_capacity; i++)
	    p->xobjects[i].flags = 0;

	p->xobjects_capacity *= 2;
    }

    if (obj_id == NEW_ID)
	obj_id = pdf_begin_obj(p, NEW_ID);

    p->xobjects[slot].obj_id = obj_id;
    p->xobjects[slot].type = type;
    p->xobjects[slot].flags = xobj_flag_used;

    return slot;
}

void
pdf_write_xobjects(PDF *p)
{
    if (p->xobjects_number > 0)
    {
	pdf_bool hit = pdf_false;
	int i;

	for (i = 0; i < p->xobjects_capacity; ++i)
	{
	    if (p->xobjects[i].flags & xobj_flag_write)
	    {
		if (!hit)
		{
		    pdf_puts(p, "/XObject");
		    pdf_begin_dict(p);
		    hit = pdf_true;
		}

		pdf_printf(p, "/I%d %ld 0 R\n", i, p->xobjects[i].obj_id);
		p->xobjects[i].flags &= ~xobj_flag_write;
	    }
	}

	if (hit)
	    pdf_end_dict(p);
    }
}

void
pdf_cleanup_xobjects(PDF *p)
{
    if (p->xobjects) {
	p->free(p, p->xobjects);
	p->xobjects = NULL;
    }
}

/* structure for 32-bit aligned memory data */
typedef struct {
    size_t cur_scanline;
    size_t num_scanlines;
    size_t scanline_widthbytes;
    size_t scanline_widthbytes_aligned;
} FILL_ALIGN, *PFILL_ALIGN;

static int
pdf_data_source_buf_fill_aligned(PDF *p, PDF_data_source *src)
{
    PFILL_ALIGN pfa = (PFILL_ALIGN) src->private_data;

    (void) p;	/* avoid compiler warning "unreferenced parameter" */

    if (pfa->cur_scanline < pfa->num_scanlines) {
	src->next_byte =
	    src->buffer_start +
		pfa->scanline_widthbytes_aligned * pfa->cur_scanline;
	src->bytes_available = pfa->scanline_widthbytes;
	pfa->cur_scanline++;
	return pdf_true;
    }

    return pdf_false;
}

void
pdf_put_image(PDF *p, int im, pdf_bool firststrip)
{
    id		length_id;
    long	length;
    pdf_image	*image;
    int		i;

    image = &p->images[im];

    /* york: how to check? see also F-imagecolor!
    switch (image->colorspace) {
        case ImageMask:
        case DeviceGray:
        case Indexed:
        case DeviceRGB:
        case DeviceCMYK:
	    break;

	default:
	    pdf_error(p, PDF_SystemError, "Unknown color space");
	    break;
    }
    */

    /* Images may also be written to the output before the first page */
    if (PDF_GET_STATE(p) == pdf_state_page)
	pdf_end_contents_section(p);

    /* Image object */

    image->no = pdf_new_xobject(p, image_xobject, NEW_ID);

    pdf_begin_dict(p); 		/* XObject */

    pdf_puts(p, "/Type/XObject\n");
    pdf_puts(p, "/Subtype/Image\n");

    pdf_printf(p,"/Width %d\n", (int) image->width);
    pdf_printf(p,"/Height %d\n", (int) image->height);
    pdf_printf(p,"/BitsPerComponent %d\n", image->bpc);

    /* 
     * Transparency handling
     */

    /* Masking by color: single transparent color value */
    if (image->transparent) {
	if (image->colorspace == Indexed || image->colorspace == DeviceGray)
	    pdf_printf(p,"/Mask[%d %d]\n",
		(int) image->transval[0], (int) image->transval[0]);

	else if (image->colorspace == DeviceRGB)
	    pdf_printf(p,"/Mask[%d %d %d %d %d %d]\n",
		(int) image->transval[0], (int) image->transval[0],
		(int) image->transval[1], (int) image->transval[1],
		(int) image->transval[2], (int) image->transval[2]);

	else if (image->colorspace == DeviceCMYK)
	    pdf_printf(p,"/Mask[%d %d %d %d %d %d %d %d]\n",
		(int) image->transval[0], (int) image->transval[0],
		(int) image->transval[1], (int) image->transval[1],
		(int) image->transval[2], (int) image->transval[2],
		(int) image->transval[3], (int) image->transval[3]);
	else
	    pdf_error(p, PDF_SystemError,
	    	"Unknown color space with transparency");

    /* Masking by position: separate bitmap mask */
    } else if (image->mask != -1) {
	pdf_printf(p, "/Mask %ld 0 R\n",
	    p->xobjects[p->images[image->mask].no].obj_id);
    }

    switch (image->colorspace) {
	case ImageMask:
	    pdf_puts(p, "/ImageMask true\n");
	    break;

	case Indexed:
	    if (firststrip)
		image->colormap_id = pdf_alloc_id(p);

	    pdf_puts(p, "/ColorSpace[/Indexed/DeviceRGB ");
	    pdf_printf(p, "%d %ld 0 R]\n", image->palette_size - 1,
	    	image->colormap_id);
	    break;
	
	default:
            /* colorize the image with a spot color */
            if (image->colorspace >= LastCS) {
                /* TODO: reconsider spot color numbering scheme */
                int spot = image->colorspace - LastCS;
                p->colorspaces[spot].used_on_current_page = pdf_true;
                pdf_printf(p, "/ColorSpace %ld 0 R\n",
                    p->colorspaces[spot].obj_id);

                /* the following is tricky: /Separation color space
                 * always works as a subtractive color space. The
                 * image, however, is Grayscale, and therefore
                 * additive.
                 */
                if (firststrip) {
                    image->invert = !image->invert;
                }
            } else {
                pdf_printf(p, "/ColorSpace/%s\n",
                    pdf_colorspace_names[image->colorspace]);
            }
	    break;
    }

    /* special case: referenced image data instead of direct data */
    if (image->reference != pdf_ref_direct) {

	if (image->compression != none) {
	    pdf_printf(p, "/FFilter[/%s]\n",
		    pdf_filter_names[image->compression]);
	}

	if (image->compression == ccitt) {
	    pdf_puts(p, "/FDecodeParms[<<");

	    if ((int) image->width != 1728)	/* CCITT default width */
		pdf_printf(p, "/Columns %d", (int) image->width);

	    pdf_printf(p, "/Rows %d", (int) image->height);

	    /* write CCITT parameters if required */
	    if (image->params)
		pdf_puts(p, image->params);

	    pdf_puts(p, ">>]\n");

	}

	if (image->reference == pdf_ref_file) {

	    /* LATER: make image file name platform-neutral:
	     * Change : to / on the Mac
	     * Change \ to / on Windows
	     */
	    pdf_printf(p, "/F(%s)/Length 0", image->filename);

	} else if (image->reference == pdf_ref_url) {

	    pdf_printf(p, "/F<</FS/URL/F(%s)>>/Length 0", image->filename);
	}

	pdf_end_dict(p);		/* XObject */

	pdf_begin_stream(p);		/* dummy image data */
	pdf_end_stream(p);		/* dummy image data */

	if (PDF_GET_STATE(p) == pdf_state_page)
	    pdf_begin_contents_section(p);

	pdf_end_obj(p);			/* XObject */

	return;
    }

    /* 
     * Now the (more common) handling of actual image
     * data to be included in the PDF output.
     */
    /* do we need a filter (either ASCII or decompression)? */

    if (p->debug['a']) {
	pdf_puts(p, "/Filter[/ASCIIHexDecode");
	if (image->compression != none)
	    pdf_printf(p, "/%s", pdf_filter_names[image->compression]);
	pdf_puts(p, "]\n");

    } else {
	/* force compression if not a recognized precompressed image format */
	if (!image->use_raw && p->compresslevel)
	    image->compression = flate;

	if (image->compression != none)
	    pdf_printf(p, "/Filter[/%s]\n",
		    pdf_filter_names[image->compression]);
    }

    /* prepare precompressed (raw) image data */
    if (image->use_raw) {
	pdf_printf(p, "/DecodeParms[%s<<", (p->debug['a'] ? "null" : ""));

	/* write EarlyChange or CCITT parameters if required */
	if (image->params)
	    pdf_puts(p, image->params);

	if (image->compression == flate || image->compression == lzw) {
	    if (image->predictor != pred_default) {
		pdf_printf(p, "/Predictor %d", (int) image->predictor);
		pdf_printf(p, "/Columns %d", (int) image->width);
		if (image->bpc != 8)
		    pdf_printf(p, "/BitsPerComponent %d", image->bpc);

		if (image->colorspace < LastCS) {
		    switch (image->colorspace) {
			case Indexed:
			case ImageMask:
			case DeviceGray:
			    /* 1 is default */
			    break;
			case DeviceRGB:
			    pdf_puts(p, "/Colors 3");
			    break;
			case DeviceCMYK:
			    pdf_puts(p, "/Colors 4");
			    break;
			default:
			    pdf_error(p, PDF_SystemError,
			"Unknown colorspace (%d)", (int )image->colorspace);
			    break;
		    }
		}
	    }
	}

	if (image->compression == ccitt) {
	    if ((int) image->width != 1728)	/* CCITT default width */
		pdf_printf(p, "/Columns %d", (int) image->width);

	    pdf_printf(p, "/Rows %d", (int) image->height);
	}
	pdf_puts(p, ">>]\n");		/* DecodeParms dict and array */
    }

    if (image->invert) {
	pdf_puts(p, "/Decode[1 0");
	for (i = 1; i < image->components; i++)
	    pdf_puts(p, " 1 0");
	pdf_puts(p, "]\n");
    }

    /* Write the actual image data */
    length_id = pdf_alloc_id(p);

    pdf_printf(p,"/Length %ld 0 R\n", length_id);
    pdf_end_dict(p);		/* XObject */

    pdf_begin_stream(p);	/* image data */
    p->start_contents_pos = pdf_tell(p);

    /* image data */

    if (p->debug['a'])
	pdf_ASCIIHexEncode(p, &image->src);
    else {
	if (image->use_raw)
	    pdf_copy(p, &image->src);
	else
	    pdf_compress(p, &image->src);
    }

    length = pdf_tell(p) - p->start_contents_pos;

    pdf_end_stream(p);	/* image data */
    pdf_end_obj(p);	/* XObject */

    pdf_begin_obj(p, length_id);		/* Length object */
    pdf_printf(p,"%ld\n", length);
    pdf_end_obj(p);

    if (p->flush & PDF_FLUSH_CONTENT)
	pdf_flush_stream(p);

    /* Image data done */

    /*
     * Write colormap information for indexed color spaces
     */
    if (firststrip && image->colorspace == Indexed) {
	pdf_begin_obj(p, image->colormap_id);		/* colormap object */
	pdf_begin_dict(p); 		

	if (p->debug['a'])
	    pdf_puts(p, "/Filter[/ASCIIHexDecode]\n");
	else if (p->compresslevel)
	    pdf_puts(p, "/Filter[/FlateDecode]\n");

	/* Length of colormap object */
	length_id = pdf_alloc_id(p);
	pdf_printf(p,"/Length %ld 0 R\n", length_id);
	pdf_end_dict(p);

	pdf_begin_stream(p);			/* colormap data */
	p->start_contents_pos = pdf_tell(p);

	if (image->components != 1) {
	    pdf_error(p, PDF_SystemError,
	    	"Bogus indexed colorspace (%d color components)",
		image->components);
	}

	image->src.init		= pdf_noop;
	image->src.fill		= pdf_data_source_buf_fill;
	image->src.terminate	= pdf_noop;

	image->src.buffer_start	= (unsigned char *) image->colormap;
	image->src.buffer_length= (size_t) (image->palette_size * 3);

	image->src.bytes_available = 0;
	image->src.next_byte	= NULL;

	/* Write colormap data */
	if (p->debug['a'])
	    pdf_ASCIIHexEncode(p, &image->src);
	else {
	    pdf_compress(p, &image->src);
	}

	length = pdf_tell(p) - p->start_contents_pos;

	pdf_end_stream(p);			/* Colormap data */
	pdf_end_obj(p);				/* Colormap object */

	pdf_begin_obj(p, length_id);		/* Length object for colormap */
	pdf_printf(p, "%ld\n", length);
	pdf_end_obj(p);				/* Length object for colormap */
    }

    if (PDF_GET_STATE(p) == pdf_state_page)
	pdf_begin_contents_section(p);

    if (p->flush & PDF_FLUSH_CONTENT)
	pdf_flush_stream(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_place_image(PDF *p, int im, float x, float y, float scale)
{
    static const char fn[] = "PDF_place_image";
    pdf_matrix m;
    pdf_image *image;
    int row;
    int imageno;

    PDF_TRACE(("%s\t(pdf[%p], %d, %f, %f, %f);\n",
	fn, (void *) p, im, x, y, scale));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use
	|| p->xobjects[p->images[im].no].type == pdi_xobject)
    {
    	pdf_error(p, PDF_ValueError,
		"Bad image or template handle %d in PDF_place_image", im);
    }

    PDF_CHECK_SCOPE(p, fn, pdf_state_ppt);

    if (PDF_GET_STATE(p) == pdf_state_template && im == p->templ)
	pdf_error(p, PDF_ValueError,
		"Can't use template within its own definition");

    image = &p->images[im];

    if (fabs(scale) < (float) PDF_SMALLREAL)
	pdf_error(p, PDF_ValueError,
		"Scaling factor 0 for image %s", image->filename);

    if (p->xobjects[image->no].type == form_xobject) {
	pdf_end_text(p);
	pdf_begin_contents_section(p);

	imageno = image->no;
	
	m.a = scale;
	m.d = scale;
	m.b = m.c = (float) 0.0;
	m.e = x;
	m.f = y;

	PDF_save(p);

	    pdf_concat_raw(p, &m);
	    if (!p->inheritgs)
		pdf_reset_gstate(p);

	    pdf_printf(p, "/I%d Do\n", imageno);
	    p->xobjects[imageno].flags |= xobj_flag_write;

	PDF_restore(p);

	return;
    }

    switch (image->colorspace) {
        case ImageMask:
        case DeviceGray:
	    p->procset	|= ImageB;
	    break;

	/*
	 * It appears that indexed images require both, although this is
	 * not documented.
	 */
        case Indexed:
	    p->procset	|= ImageI;
	    p->procset	|= ImageC;
	    break;

        case DeviceRGB:
        case DeviceCMYK:
	    p->procset	|= ImageC;
	    break;

	case CalGray:
        case CalRGB:
        case Lab:
        case PatternCS:
        case Separation:
            pdf_error(p, PDF_SystemError,
                "Bogus colorspace (%d) in PDF_place_image",
                    (int) image->colorspace);

        /* image has been colorized with a spot color */
        default:
            break;
    
	/* york: check the above carefully!
	** original version:
	default:
	    pdf_error(p, PDF_SystemError,
	    	"Bogus colorspace (%d) in PDF_place_image",
		    (int) image->colorspace);
	*/
    }

    pdf_end_text(p);
    pdf_begin_contents_section(p);

    imageno = image->no;	/* number of first strip */

    if (image->strips == 1)
	image->rowsperstrip = (int) image->height;

    for (row = 0; row < image->height; row += image->rowsperstrip, imageno++) {
	int height;	/* height of the current strip */

	height = (row + image->rowsperstrip > image->height ? 
		    (int) image->height - row : image->rowsperstrip);

	PDF_save(p);

	m.a = image->width * scale;
	m.d = height * scale;
	m.b = m.c = (float) 0.0;
	m.e = x;
	m.f = y + scale * (image->height - row - height);
	pdf_concat_raw(p, &m);

	pdf_printf(p, "/I%d Do\n", imageno);
	p->xobjects[imageno].flags |= xobj_flag_write;

	if (image->mask != -1)
	    p->xobjects[p->images[image->mask].no].flags |= xobj_flag_write;

	PDF_restore(p);
    }
}

#define MAX_THUMBNAIL_SIZE	106

PDFLIB_API void PDFLIB_CALL
PDF_add_thumbnail(PDF *p, int im)
{
    static const char fn[] = "PDF_add_thumbnail";
    pdf_image *image;

    PDF_TRACE(("%s\t(pdf[%p], %d);\n", fn, (void *) p, im));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_page);
    
    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_add_thumbnail", im);

    if (p->thumb_id != BAD_ID)
	pdf_error(p, PDF_RuntimeError,
		"More than one thumbnail for this page (PDF_add_thumbnail)");

    image = &p->images[im];

    if (image->strips > 1)
	pdf_error(p, PDF_RuntimeError,
		"Can't use multi-strip images as thumbnails");

    if (image->width > MAX_THUMBNAIL_SIZE || image->height > MAX_THUMBNAIL_SIZE)
    	pdf_error(p, PDF_ValueError,
	    "Thumbnail image larger than %d pixels", MAX_THUMBNAIL_SIZE);

    if (image->colorspace != DeviceGray &&
	image->colorspace != DeviceRGB &&
	image->colorspace != Indexed)
    	pdf_error(p, PDF_RuntimeError,
	    "Unsupported color space in thumbnail image");

    /* Add the image to the thumbnail key of the current page.  */
    p->thumb_id = p->xobjects[image->no].obj_id;
}

void
pdf_cleanup_image(PDF *p, int im)
{
    /* clean up parameter string if necessary */
    if (p->images[im].params) {
	p->free(p, p->images[im].params);
	p->images[im].params = NULL;
    }

    if (p->images[im].filename)
	p->free(p, p->images[im].filename);

    /* free the image slot and prepare for next use */
    pdf_init_image_struct(p, &(p->images[im]));
}

PDFLIB_API void PDFLIB_CALL
PDF_close_image(PDF *p, int im)
{
    static const char fn[] = "PDF_close_image";

    PDF_TRACE(("%s\t(pdf[%p], %d);\n", fn, (void *) p, im));

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    PDF_CHECK_SCOPE(p, fn, pdf_state_document | pdf_state_page);

    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use
	|| p->xobjects[p->images[im].no].type == pdi_xobject)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_close_image", im);

    pdf_cleanup_image(p, im);
}

/* interface for using image data directly in memory */

PDFLIB_API int PDFLIB_CALL
PDF_open_image(
    PDF *p,
    const char *type,
    const char *source,
    const char *data,
    long length,
    int width,
    int height,
    int components,
    int bpc,
    const char *params)
{
    static const char fn[] = "PDF_open_image";
    pdf_image *image;
    int im;

    PDF_TRACE(("%s\t(pdf[%p], \"%s\", \"%s\", source[%p], ",
    	fn, (void *) p, type, source, (void *) data));

    PDF_TRACE(("%ld, %d, %d, %d, %d, \"%s\");",
    	length, width, height, components, bpc, params));

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    PDF_CHECK_SCOPE(p, fn, pdf_state_document | pdf_state_page);

    if (type == NULL || *type == '\0')
    	pdf_error(p, PDF_ValueError, "No image type in PDF_open_image");

    if (source == NULL || *source == '\0')
    	pdf_error(p, PDF_ValueError, "No image source in PDF_open_image");

    if (!strcmp(type, "raw") && data == NULL)
    	pdf_error(p, PDF_ValueError, "Bad raw image pointer in PDF_open_image");

    if (strcmp(type, "ccitt") && strcmp(type, "raw")
	    && params != NULL && *params != '\0')
    	pdf_error(p, PDF_NonfatalError,
	    "Unnecessary CCITT parameter in PDF_open_image");

    for (im = 0; im < p->images_capacity; im++)
	if (!p->images[im].in_use)		/* found free slot */
	    break;

    if (im == p->images_capacity) 
	pdf_grow_images(p);

    image = &p->images[im];

    if (!strcmp(type, "jpeg")) {
	image->compression = dct;
	image->use_raw	= pdf_true;

    } else if (!strcmp(type, "ccitt")) {
	image->compression = ccitt;
	image->use_raw	= pdf_true;

	if (length < 0L) {
	    image->info.ccitt.BitReverse = pdf_true;
	    length = -length;
	}

	if (params != NULL && *params != '\0')
	    image->params = pdf_strdup(p, params);
	else
	    image->params = NULL;

    } else if (!strcmp(type, "raw")) {
	image->compression = none;

    } else
	pdf_error(p, PDF_ValueError,
	    "Unknown image type '%s' in PDF_open_image", type);

    switch (components) {
	case 1:
	    if (params && !strcmp(params, "mask")) {
		if (strcmp(type, "raw") || bpc != 1)
		    pdf_error(p, PDF_ValueError,
			"Unsuitable image mask in PDF_open_image");
		image->colorspace = ImageMask;
	    } else
		image->colorspace = DeviceGray;
	    break;

	case 3:
	    image->colorspace = DeviceRGB;
	    break;

	case 4:
	    image->colorspace = DeviceCMYK;
	    break;

	default:
	    pdf_error(p, PDF_ValueError,
	    	"Bogus number of components (%d) in PDF_open_image",
		components);
    }

    image->width		= (float) width;
    image->height		= (float) height;
    image->bpc			= bpc;
    image->components		= components;
    image->in_use		= pdf_true;		/* mark slot as used */

    if (!strcmp(source, "memory")) {	/* in-memory image data */
	if (image->compression == none && length != (long)
	    	(height * ((width * components * bpc + 7) / 8)))
	    pdf_error(p, PDF_ValueError,
		"Bogus image data length '%ld' in PDF_open_image", length);

	image->src.init		= pdf_noop;
	image->src.fill		= pdf_data_source_buf_fill;
	image->src.terminate	= pdf_noop;

	image->src.buffer_start	= (unsigned char *) data;
	image->src.buffer_length= (size_t) length;

	image->src.bytes_available = 0;
	image->src.next_byte	= NULL;

    } else if (!strcmp(source, "memory32")) {
	/* 32 bit aligned in-memory image data */
	FILL_ALIGN fa;

	if (image->compression == none && length != (long)
	    	(height * ((width * components * bpc + 7) / 8)))
	    pdf_error(p, PDF_ValueError,
		"Bogus image data length '%ld' in PDF_open_image", length);

	image->src.buffer_start	= (unsigned char *) data;
	image->src.buffer_length= (size_t) length;

	image->src.init		= pdf_noop;
	image->src.fill		= pdf_data_source_buf_fill_aligned;
	image->src.terminate	= pdf_noop;

	fa.cur_scanline = 0;
	fa.num_scanlines = (size_t) height;
	fa.scanline_widthbytes = (size_t) ((width * components * bpc + 7) / 8);
	/* dword align */
	fa.scanline_widthbytes_aligned = (fa.scanline_widthbytes + 3) & ~0x3;

	image->src.private_data = (void *) &fa;
	image->src.bytes_available = 0;
	image->src.next_byte	= NULL;

    } else if (!strcmp(source, "fileref")) {	/* file reference */
	if (p->compatibility == PDF_1_2)
	    pdf_error(p, PDF_RuntimeError,
		"External image file references are not supported in PDF 1.2");

	image->reference	= pdf_ref_file;
	image->filename		= pdf_strdup(p, data);

    } else if (!strcmp(source, "url")) {	/* url reference */
	if (p->compatibility == PDF_1_2)
	    pdf_error(p, PDF_RuntimeError,
		"External image URLs are not supported in PDF 1.2");

	image->reference	= pdf_ref_url;
	image->filename		= pdf_strdup(p, data);

    } else			/* error */
	pdf_error(p, PDF_ValueError,
	    "Bogus image data source '%s' in PDF_open_image", source);

    pdf_put_image(p, im, pdf_true);

    PDF_TRACE((" [%d]\n", im));

    return im;
}

PDFLIB_API int PDFLIB_CALL
PDF_open_image_file(
    PDF *p,
    const char *type,
    const char *filename,
    const char *stringparam,
    int intparam)
{
    static const char fn[] = "PDF_open_image_file";
    int imageslot;
    int ret;

    PDF_TRACE(("%s(pdf[%p], \"%s\", \"%s\", \"%s\", %d);",
	fn, (void *) p, type, filename, stringparam, intparam));

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    PDF_CHECK_SCOPE(p, fn, pdf_state_document | pdf_state_page);

    if (type == NULL || *type == '\0')
	pdf_error(p, PDF_ValueError,
	    "Bad image file type for image %s", filename);

    if (filename == NULL || *filename == '\0')
	pdf_error(p, PDF_ValueError, "Bad image file name for %s image", type);

    if (stringparam &&
    	p->compatibility == PDF_1_2 && !strcmp(stringparam, "masked"))
	pdf_error(p, PDF_RuntimeError,
		"Masked images are not supported in PDF 1.2");

    for (imageslot = 0; imageslot < p->images_capacity; imageslot++)
	if (!p->images[imageslot].in_use)		/* found free slot */
	    break;

    if (imageslot == p->images_capacity) 
	pdf_grow_images(p);

    if (!strcmp(type, "png") || !strcmp(type, "PNG")) {
	ret = pdf_open_PNG_data(p, imageslot, filename, stringparam, intparam);

	if (ret == -1)
	    pdf_init_image_struct(p, &p->images[imageslot]);
	PDF_TRACE((" [%d]\n", ret));
	return ret;
    }

    if (!strcmp(type, "gif") || !strcmp(type, "GIF")) {
	ret = pdf_open_GIF_data(p, imageslot, filename, stringparam, intparam);

	if (ret == -1)
	    pdf_init_image_struct(p, &p->images[imageslot]);
	PDF_TRACE((" [%d]\n", ret));
	return ret;
    }

    if (!strcmp(type, "tiff") || !strcmp(type, "TIFF")) {
	ret = pdf_open_TIFF_data(p, imageslot, filename, stringparam, intparam);

	if (ret == -1)
	    pdf_init_image_struct(p, &p->images[imageslot]);
	PDF_TRACE((" [%d]\n", ret));
	return ret;
    }

    if (!strcmp(type, "jpeg") || !strcmp(type, "JPEG")) {
	ret = pdf_open_JPEG_data(p, imageslot, filename, stringparam, intparam);

	if (ret == -1)
	    pdf_init_image_struct(p, &p->images[imageslot]);
	PDF_TRACE((" [%d]\n", ret));
	return ret;
    }

    pdf_error(p, PDF_ValueError,
	"Image type %s for image file %s not supported", type, filename);

    /* never reached */
    PDF_TRACE((" [%d]\n", -1));
    return -1;
}
