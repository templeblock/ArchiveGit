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

/* $Id: p_tiff.c,v 1.17.2.7 2002/01/26 18:32:18 tm Exp $
 *
 * TIFF processing for PDFlib
 *
 */

#include <string.h>

#include "p_intern.h"
#include "p_image.h"

#ifndef HAVE_LIBTIFF

int
pdf_open_TIFF_data(
    PDF *p,
    int imageslot,
    const char *filename,
    const char *stringparam,
    int intparam)
{
    pdf_error(p, PDF_NonfatalError,
	"TIFF images not supported in this configuration");

    return -1;
}

#else

#ifdef TIFF_PDFLIB_MEM_SUPPORTED
#include "tiffiop.h"
static void *
pdf_libtiff_malloc(TIFF *t, tsize_t size)
{
    PDF *p = (PDF*) t->pdflib_opaque;
    return(p->malloc(p, (size_t)size, "libtiff"));
}

static void *
pdf_libtiff_realloc(TIFF *t, tdata_t mem, tsize_t size)
{
    PDF *p = (PDF*) t->pdflib_opaque;
    return(p->realloc(p, (void*)mem, (size_t)size, "libtiff"));
}

static void
pdf_libtiff_free(TIFF *t, tdata_t mem)
{
    PDF *p = (PDF*) t->pdflib_opaque;
    p->free(p, (void*)mem);
}
#endif /* TIFF_PDFLIB_MEM_SUPPORTED */

static void
pdf_data_source_TIFF_init(PDF *p, PDF_data_source *src)
{
    pdf_image	*image;

    image = (pdf_image *) src->private_data;

    if (image->strips == 1)
	image->info.tiff.cur_line = 0;

    if (image->use_raw) {
	/* malloc is done in the fill function */
	src->buffer_length = (size_t) 0;
	src->buffer_start = (pdf_byte *) NULL;
    } else {
	src->buffer_length = (size_t) (image->components * image->width);
	src->buffer_start = (pdf_byte *)
	    p->malloc(p, src->buffer_length, "pdf_data_source_TIFF_init");
    }
}

static pdf_bool
pdf_data_source_TIFF_fill(PDF *p, PDF_data_source *src)
{
    pdf_image	*image;
    int		col;
    pdf_byte	*dest;
    uint16	fillorder;
    uint32	*s, *bc;

    image = (pdf_image *) src->private_data;

    if (image->use_raw) {
	if (image->info.tiff.cur_line == image->strips)
	    return pdf_false;

	TIFFGetField(image->info.tiff.tif, TIFFTAG_STRIPBYTECOUNTS, &bc);

	if (bc[image->info.tiff.cur_line] > src->buffer_length) {
	    src->buffer_length = bc[image->info.tiff.cur_line];
	    src->buffer_start = (pdf_byte *) p->realloc(p, src->buffer_start,
		src->buffer_length, "pdf_data_source_TIFF_fill");
	}

	if (TIFFReadRawStrip(image->info.tiff.tif,
			(tstrip_t) image->info.tiff.cur_line,
			(tdata_t) src->buffer_start,
			(tsize_t) bc[image->info.tiff.cur_line]) == -1) {

	    p->free(p, (void *) src->buffer_start);
	    TIFFClose(image->info.tiff.tif);

	    pdf_error(p, PDF_IOError, "TIFF file damaged");
	}

	src->next_byte = src->buffer_start;
	src->bytes_available = bc[image->info.tiff.cur_line];

	if (TIFFGetField(image->info.tiff.tif, TIFFTAG_FILLORDER, &fillorder)
	    && (fillorder == FILLORDER_LSB2MSB)) {
	    TIFFReverseBits((unsigned char *) src->buffer_start,
		(unsigned long) src->bytes_available);
	}

	if (image->strips > 1) {
	    /* only a single strip of a multi-strip image */
	    image->info.tiff.cur_line = image->strips;
	} else
	    image->info.tiff.cur_line++;

    } else {
	if (image->info.tiff.cur_line++ == image->height)
	    return pdf_false;

	src->next_byte = src->buffer_start;
	src->bytes_available = src->buffer_length;

	dest = src->buffer_start;
	s = image->info.tiff.raster + 
	    ((int)image->height - image->info.tiff.cur_line) *
	    (int) image->width;
      
	switch (image->components) {
	  case 1:
	    for (col = 0; col < image->width; col++, s++) {
		*dest++ = (pdf_byte) TIFFGetR(*s);
	    }
	    break;

	  case 3:
	    for (col = 0; col < image->width; col++, s++) {
		*dest++ = (pdf_byte) TIFFGetR(*s);
		*dest++ = (pdf_byte) TIFFGetG(*s);
		*dest++ = (pdf_byte) TIFFGetB(*s);
	    }
	    break;

          case 4:
	    for (col = 0; col < image->width; col++, s++) {
		unsigned char* t = (unsigned char*)&(*s);
		*dest++ = (pdf_byte) t[0];
		*dest++ = (pdf_byte) t[1];
		*dest++ = (pdf_byte) t[2];
		*dest++ = (pdf_byte) t[3];
	    }
	    break;

	  default:
	    pdf_error(p, PDF_SystemError,
		"Unknown color space in TIFF image %s (%d components)", 
		image->filename, image->components);
	}
    }

    return pdf_true;
}

static void
pdf_data_source_TIFF_terminate(PDF *p, PDF_data_source *src)
{
    p->free(p, (void *) src->buffer_start);
}

static int
pdf_check_colormap(int n, uint16* r, uint16* g, uint16* b)
{
    while (n-- > 0)
	if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
		return(16);
    return(8);
}

int
pdf_open_TIFF_data(
    PDF *p,
    int imageslot,
    const char *filename,
    const char *stringparam,
    int intparam)
{
    uint32 w, h;
    uint16 unit, bpc, compression, photometric, inkset, extra, *sinfo;
    uint16 planarconfig, predictor;
    uint16 *rmap, *gmap, *bmap;
    uint32 group3opts;
    tsample_t components;
    size_t npixels;
    pdf_image *image;
    int strip, mask = -1;
    float res_x, res_y;
    pdf_bool is_mask = pdf_false;
    pdf_bool colorize = pdf_false;

    image = &p->images[imageslot];

    /* Suppress all warnings and error messages */
    (void) TIFFSetErrorHandler(NULL);
    (void) TIFFSetWarningHandler(NULL);

    image->info.tiff.tif = TIFFOpen(filename, "r"
#ifdef TIFF_PDFLIB_MEM_SUPPORTED
	, (void *)p, pdf_libtiff_malloc, pdf_libtiff_realloc, pdf_libtiff_free,
	NULL, NULL
#endif
    );

    if (!image->info.tiff.tif) {
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
		"Unknown tag structure in TIFF file '%s'", filename);
	}
	return -1;
    }

    if (stringparam && *stringparam && !strcmp(stringparam, "page")) {
	if (TIFFSetDirectory(image->info.tiff.tif, (tdir_t) (intparam-1)) != 1){
	    TIFFClose(image->info.tiff.tif);
	    /* do not throw an exception even if imagewarning is true */
	    return -1;
	}
    }

    photometric = 255;	/* dummy value */

    if (TIFFGetField(image->info.tiff.tif, TIFFTAG_PHOTOMETRIC, &photometric) &&
	(photometric == PHOTOMETRIC_YCBCR ||
	photometric == PHOTOMETRIC_CIELAB ||
	photometric == PHOTOMETRIC_MASK)) {
	/* can't handle these yet */
	    TIFFClose(image->info.tiff.tif);
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
	    "Can't handle TIFF file '%s' due to unsupported color space (%d)",
		filename, photometric);
	    }
	    return -1;
    }

    TIFFGetField(image->info.tiff.tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(image->info.tiff.tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFGetFieldDefaulted(image->info.tiff.tif, TIFFTAG_BITSPERSAMPLE, &bpc);
    TIFFGetFieldDefaulted(image->info.tiff.tif, TIFFTAG_SAMPLESPERPIXEL,
    	&components);
    TIFFGetField(image->info.tiff.tif, TIFFTAG_COMPRESSION, &compression);
    TIFFGetFieldDefaulted(image->info.tiff.tif, TIFFTAG_EXTRASAMPLES,
    	&extra, &sinfo);
    TIFFGetFieldDefaulted(image->info.tiff.tif, TIFFTAG_PLANARCONFIG,
   	 &planarconfig);

    if (bpc != 1 && bpc != 2 && bpc != 4 && bpc != 8) {
	/* PDF doesn't support other values of the color depth */
	TIFFClose(image->info.tiff.tif);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
	"Can't handle TIFF file '%s' (PDF doesn't support %d bit color depth)",
	    filename, bpc);
	}
	return -1;
    }

    image->width	= (float) w;
    image->height	= (float) h;
    image->components	= components;
    image->strips	= 1;
    image->bpc		= bpc;

    /* fetch the resolution values if found in the file */
    if (TIFFGetField(image->info.tiff.tif, TIFFTAG_XRESOLUTION, &res_x) &&
	TIFFGetField(image->info.tiff.tif, TIFFTAG_YRESOLUTION, &res_y) &&
	TIFFGetFieldDefaulted(image->info.tiff.tif, TIFFTAG_RESOLUTIONUNIT,
		&unit) &&
	res_x > 0 && res_y > 0) {

	if (unit == RESUNIT_INCH) {
	    image->dpi_x = res_x;
	    image->dpi_y = res_y;

	} else if (unit == RESUNIT_CENTIMETER) {
	    image->dpi_x = res_x * (float) 2.54;
	    image->dpi_y = res_y * (float) 2.54;

	} else if (unit == RESUNIT_NONE) {
	    image->dpi_x = -res_x;
	    image->dpi_y = -res_y;
	}
    }

    if (compression != COMPRESSION_LZW && p->debug['P'])
	compression = (uint16) 0;	/* dummy: disable pass-through mode */

    /* find out whether we can use the compressed raw data directly */
    switch ((int) compression) {
	case COMPRESSION_CCITTRLE:
	case COMPRESSION_CCITTRLEW:
	    if (TIFFIsTiled(image->info.tiff.tif)) {
		image->use_raw = pdf_false;
		image->bpc = 8;
		break;
	    }

	    image->params = (char *) p->malloc(p, PDF_MAX_PARAMSTRING,
		"pdf_open_TIFF_data");

	    strcpy(image->params, "/EndOfBlock false");
	    strcat(image->params, "/EncodedByteAlign true");

	    if (photometric == PHOTOMETRIC_MINISBLACK)
		strcat(image->params, "/BlackIs1 true");

	    image->strips = (int) TIFFNumberOfStrips(image->info.tiff.tif);
	    image->compression = ccitt;
	    image->bpc = 1;
	    image->use_raw = pdf_true;
	    break;

	case COMPRESSION_CCITTFAX3:
	    if (TIFFIsTiled(image->info.tiff.tif)) {
		image->use_raw = pdf_false;
		image->bpc = 8;
		break;
	    }

	    image->params = (char *) p->malloc(p, PDF_MAX_PARAMSTRING,
		"pdf_open_TIFF_data");
	    strcpy(image->params, "");

	    /* The following contains three code segments which are
	     * disabled.
	     * Apparently, and contrary to my reading of the specs,
	     * the following can not be deduced from the respective
	     * TIFF entry or option:
	     * - I expected /EndOfBlock and /EndOfLine to be always
	     *   true for CCITTFAX3 images.
	     * 
	     * - /EncodedByteAlign can not reliably be deduced from
	     *   GROUP3OPT_FILLBITS;
	     * 
	     * - /BlackIs1 can not reliably be deduced from
	     *   PHOTOMETRIC_MINISBLACK;
	     *
	     * From practical experience, the respective lines are
	     * disabled, but I don't have any clear explanation for this.
	     * A few TIFF images still don't work with this setting,
	     * unfortunately.
	     */

	    /* SEE ABOVE!
	    strcpy(image->params, "/EndOfBlock false");
	    strcat(image->params, "/EndOfLine true");
	    */
	    /*
	    strcat(image->params, "/DamagedRowsBeforeError 1");
	    */

	    if (TIFFGetField(image->info.tiff.tif, TIFFTAG_GROUP3OPTIONS,
	    	&group3opts)) {
		/* /K = 0 (= G3,1D) is default */
		if (group3opts & GROUP3OPT_2DENCODING)
		    strcat(image->params, "/K 1");

		/* SEE ABOVE!
		if (group3opts & GROUP3OPT_FILLBITS)
		    strcat(image->params, "/EncodedByteAlign true");
		*/
	    }

	    /* SEE ABOVE!
	    if ((photometric == PHOTOMETRIC_MINISBLACK))
		strcat(image->params, "/BlackIs1 true");
	    */

	    image->strips = (int) TIFFNumberOfStrips(image->info.tiff.tif);
	    image->compression = ccitt;
	    image->bpc = 1;
	    image->use_raw = pdf_true;
	    break;

	case COMPRESSION_CCITTFAX4:
	    if (TIFFIsTiled(image->info.tiff.tif)) {
		image->use_raw = pdf_false;
		image->bpc = 8;
		break;
	    }

	    image->params = (char *) p->malloc(p, PDF_MAX_PARAMSTRING,
		"pdf_open_TIFF_data");

	    strcpy(image->params, "/K -1");

	    if (photometric == PHOTOMETRIC_MINISBLACK)
		strcat(image->params, "/BlackIs1 true");

	    image->strips = (int) TIFFNumberOfStrips(image->info.tiff.tif);
	    image->compression = ccitt;
	    image->bpc = 1;
	    image->use_raw = pdf_true;
	    break;

	case COMPRESSION_NONE:
	    /*
	     * can't use multiple data sources in raw mode, or deal with tiles
	     */
	    if ((planarconfig == PLANARCONFIG_SEPARATE && components > 1) ||
		TIFFIsTiled(image->info.tiff.tif)) {
		image->use_raw = pdf_false;
		image->bpc = 8;
		break;
	    }

	    if (extra != 0) {
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
			"Can't handle TIFF file '%s' with alpha channel",
			filename);
		}
		return -1;
	    }

	    if (photometric == PHOTOMETRIC_MINISWHITE)
		image->invert = !image->invert;

	    image->use_raw = pdf_false;
	    image->bpc = 8;
	    break;

	case COMPRESSION_LZW:
	    if (TIFFGetField(image->info.tiff.tif, TIFFTAG_PREDICTOR,
	    	&predictor)) {
		if (predictor != pred_default && predictor != pred_tiff) {
		    TIFFClose(image->info.tiff.tif);
		    if (p->debug['i']) {
			pdf_error(p, PDF_NonfatalError,
			    "Bad predictor tag (%d) in TIFF file '%s'",
			    (int) predictor, filename);
		    }
		    return -1;
		} else
		    image->predictor = (pdf_predictor) predictor;
	    }

	    if (TIFFIsTiled(image->info.tiff.tif)) {
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
			"Can't handle LZW-compressed tiled TIFF file '%s'",
			filename);
		}
		return -1;
	    }

	    if (planarconfig == PLANARCONFIG_SEPARATE && components > 1) {
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
	    "Can't handle LZW-compressed TIFF file '%s' with separate planes",
		    filename);
		}
		return -1;
	    }

	    if (extra != 0) {
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
		"Can't handle LZW-compressed TIFF file '%s' with alpha channel",
		    filename);
		}
		return -1;
	    }

	    if (photometric == PHOTOMETRIC_MINISWHITE)
		image->invert = !image->invert;

	    image->strips = (int) TIFFNumberOfStrips(image->info.tiff.tif);
	    image->compression = lzw;
	    image->use_raw = pdf_true;
	    break;

	case COMPRESSION_PACKBITS:
	    /*
	     * can't pass through extra bits or use multiple data sources
	     * in raw mode
	     */
	    if (extra != 0 ||
	       (planarconfig == PLANARCONFIG_SEPARATE && components > 1) ||
		TIFFIsTiled(image->info.tiff.tif)) {
		image->use_raw = pdf_false;
		image->bpc = 8;
		break;
	    }

	    if (photometric == PHOTOMETRIC_MINISWHITE)
		image->invert = !image->invert;

	    image->strips = (int) TIFFNumberOfStrips(image->info.tiff.tif);
	    image->compression = runlength;
	    image->use_raw = pdf_true;
	    break;

	case COMPRESSION_DEFLATE:
	case COMPRESSION_ADOBE_DEFLATE:
	    if (extra != 0 ||
	       (planarconfig == PLANARCONFIG_SEPARATE && components > 1) ||
		TIFFIsTiled(image->info.tiff.tif)) {
#ifdef TIFF_PDFLIB_MEM_SUPPORTED
		/* Our TIFFlib can't uncompress flate-compressed TIFFs... */
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
	    "Can't handle flate-compressed TIFF file '%s' with extra channel",
		    filename);
		}
		return -1;
#else
		/* ...but an external TIFFlib may be configured to do so */
		image->use_raw = pdf_false;
		image->bpc = 8;
		break;
#endif
	    }

	    if (TIFFGetField(image->info.tiff.tif, TIFFTAG_PREDICTOR,
	    	&predictor)) {
		if (predictor != pred_default && predictor != pred_tiff) {
		    TIFFClose(image->info.tiff.tif);
		    if (p->debug['i']) {
			pdf_error(p, PDF_NonfatalError,
			"Unknown predictor %d in TIFF file '%s'",
			predictor, filename);
		    }
		    return -1;
		} else
		    image->predictor = (pdf_predictor) predictor;
	    }

	    image->strips = (int) TIFFNumberOfStrips(image->info.tiff.tif);
	    image->compression = flate;
	    image->use_raw = pdf_true;
	    break;

#ifdef PDF_NYI
	/* dead code, doesn't work yet */
	case COMPRESSION_JPEG:
	    if (TIFFIsTiled(image->info.tiff.tif)) {
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
		    "Can't handle JPEG-compressed tiled TIFF file '%s'",
			filename);
		}
		return -1;
	    }

	    if (extra != 0 ||
	       (planarconfig == PLANARCONFIG_SEPARATE && components > 1) ||
		TIFFIsTiled(image->info.tiff.tif)) {
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
		    "Can't handle JPEG-compressed TIFF file '%s'", filename);
		}
		return -1;
	    }

	    image->strips = (int) TIFFNumberOfStrips(image->info.tiff.tif);
	    image->compression = dct;
	    image->use_raw = pdf_true;
	    break;

#else	/* !PDF_NYI */

	case COMPRESSION_JPEG:
#ifdef TIFF_PDFLIB_MEM_SUPPORTED
	    /* Our TIFFlib can't uncompress JPEG-compressed TIFFs... */
	    TIFFClose(image->info.tiff.tif);
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
		"Can't handle JPEG-compressed TIFF file '%s'", filename);
	    }
	    return -1;
#else
	    /* ...but an external TIFFlib may be configured to do so */
	    image->use_raw = pdf_false;
	    image->bpc = 8;
	    break;
#endif	/* TIFF_PDFLIB_MEM_SUPPORTED */

#endif	/* !PDF_NYI */

	default:
	    image->use_raw = pdf_false;
	    image->bpc = 8;
    }

    /* palette images are automatically converted to RGB by tifflib */
    if (image->components == 1 && !image->use_raw &&
	TIFFGetField(image->info.tiff.tif, TIFFTAG_COLORMAP,
		&rmap, &gmap, &bmap)) {
	image->components = 3;
    }

    if (stringparam && *stringparam && strcmp(stringparam, "page")) {
        if (!strcmp(stringparam, "colorize")) {
            if (image->components != 1) {
                TIFFClose(image->info.tiff.tif);
                pdf_error(p, PDF_ValueError,
		    "Can't colorize TIFF image with more than 1 component");
            }
	    if (intparam >= p->colorspaces_number) {
                TIFFClose(image->info.tiff.tif);
                pdf_error(p, PDF_ValueError,
		    "Invalid color number %d for image %s", intparam, filename);
	    }
            image->colorspace = (pdf_colorspace) (intparam + LastCS);
            colorize = pdf_true;

	} else if (!strcmp(stringparam, "mask")) {
	    if (image->components != 1 || image->bpc != 1) {
		TIFFClose(image->info.tiff.tif);
		pdf_error(p, PDF_ValueError,
		    "Can't handle TIFF image mask with more than 1 bit");
	    }
/*	    if (image->strips != 1) { */
/*		TIFFClose(image->info.tiff.tif); */
/*		pdf_error(p, PDF_ValueError, */
/*		    "Can't use multistrip TIFF image as mask"); */
/*	    } */
	    image->colorspace = ImageMask;
	    is_mask = pdf_true;

	} else if (!strcmp(stringparam, "masked")) {
	    mask = intparam;
	    if (mask >= 0 && (mask >= p->images_capacity ||
		!p->images[mask].in_use || p->images[mask].strips != 1 ||
		p->images[mask].colorspace != ImageMask)) {
		    TIFFClose(image->info.tiff.tif);
		    pdf_error(p, PDF_ValueError,
		    "Bad image mask (no %d) for image '%s'", mask, filename);
	    }
	    if (image->strips != 1) {
		TIFFClose(image->info.tiff.tif);
		pdf_error(p, PDF_ValueError,
		    "Can't mask multistrip TIFF image");
	    }
	} else if (!strcmp(stringparam, "invert")) {
	    image->invert = !image->invert;
	} else {
	    TIFFClose(image->info.tiff.tif);
	    pdf_error(p, PDF_ValueError,
	    	"Unknown parameter %s in pdf_open_TIFF", stringparam);
	}
    }

    image->mask = mask;

    switch (image->components) {
	case 1:
	    if (!colorize && !is_mask)
		image->colorspace = DeviceGray;
	    break;

	case 3:
	    image->colorspace = DeviceRGB;
	    break;

	case 4:
	    if (photometric == PHOTOMETRIC_SEPARATED) {
		TIFFGetFieldDefaulted(image->info.tiff.tif, TIFFTAG_INKSET,
			&inkset);
		if (inkset != INKSET_CMYK) {
		    TIFFClose(image->info.tiff.tif);
		    if (p->debug['i']) {
			pdf_error(p, PDF_NonfatalError,
	"Can't handle separated TIFF file '%s' with inkset different from CMYK",
			    filename);
		    }
		    return -1;
		}
		image->colorspace = DeviceCMYK;
	    } else {
		/* if it's not separated it must be RGB with alpha */
		image->components = 3;
		image->colorspace = DeviceRGB;
		image->compression = none;
	    }
	    break;

	default:
	    TIFFClose(image->info.tiff.tif);
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
		    "Bad number of color components (%d) in TIFF file '%s'",
		    image->components, filename);
	    }
	    return -1;
    }

    image->src.private_data	= (void *) image;
    image->src.init		= pdf_data_source_TIFF_init;
    image->src.fill		= pdf_data_source_TIFF_fill;
    image->src.terminate	= pdf_data_source_TIFF_terminate;

    if (image->use_raw) {
	uint32 row, rowsperstrip;

	/* must handle colormap ourselves */
	if (photometric == PHOTOMETRIC_PALETTE) {
	    int i;
	    if (!TIFFGetField(image->info.tiff.tif, TIFFTAG_COLORMAP,
	    		&rmap, &gmap, &bmap)) {
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
			"Couldn't read colormap from colormapped file '%s'",
			filename);
		}
		return -1;
	    }

	    image->colorspace = Indexed;
	    image->components = 1;
	    image->palette_size = 1 << bpc;
	    image->colormap = (pdf_colormap*) p->malloc(p, sizeof(pdf_colormap),
	    	"pdf_open_TIFF_data");

#define CVT(x) (uint16) (((x) * 255) / ((1L<<16)-1))
	    if (pdf_check_colormap(image->palette_size, rmap, gmap, bmap) == 16)
	    {
                /* convert colormap to 8 bit values  */
		for (i = 0; i < image->palette_size; i++) {
		    rmap[i] = CVT(rmap[i]);
		    gmap[i] = CVT(gmap[i]);
		    bmap[i] = CVT(bmap[i]);
		}
	    }
#undef CVT

	    for (i = 0; i < image->palette_size; i++) {
		(*image->colormap)[i][0] = (pdf_byte) rmap[i];
		(*image->colormap)[i][1] = (pdf_byte) gmap[i];
		(*image->colormap)[i][2] = (pdf_byte) bmap[i];
	    }
	}

	if (image->strips > image->height)
	    image->strips = (int) image->height;

	if (TIFFGetFieldDefaulted(image->info.tiff.tif,
	    TIFFTAG_ROWSPERSTRIP, &rowsperstrip) == 1 && (int) rowsperstrip
	    	!= -1)
	    image->rowsperstrip = (int) rowsperstrip;
	else
	    image->rowsperstrip = (int) image->height;

	/*
	 * The first strip must be handled separately because it carries the
	 * colormap for indexed images. Other strips reuse this colormap.
	 */
	image->info.tiff.cur_line = 0;
	image->height = (float)
	    (image->rowsperstrip > (int) h ? (int) h : image->rowsperstrip);
	pdf_put_image(p, imageslot, pdf_true);

	if (photometric == PHOTOMETRIC_PALETTE) {
	    /*
	     * these may have been overwritten because of the colormap
	     * output sequence
	     */
	    image->src.init		= pdf_data_source_TIFF_init;
	    image->src.fill		= pdf_data_source_TIFF_fill;
	    image->src.terminate	= pdf_data_source_TIFF_terminate;
	}

	for (row = (uint32) image->rowsperstrip, strip = 1;
		row < h; row += (uint32) image->rowsperstrip, strip++) {

	    image->height = (float)
	    (int) (row+image->rowsperstrip > h ? h-row : image->rowsperstrip);

	    /*
	     * tell pdf_data_source_TIFF_fill() to read only data of the
	     * current strip
	     */
	    image->info.tiff.cur_line = strip;
	    pdf_put_image(p, imageslot, pdf_false);
	}

	if (image->colormap)
	    p->free(p, image->colormap);

	image->height = (float) h;
	image->no -= (image->strips - 1);	/* number of first strip */

    } else {
	image->info.tiff.raster = (uint32 *) NULL;

	if (image->colorspace != DeviceCMYK) {
	    npixels = (size_t) (w * h);
	    image->info.tiff.raster = (uint32 *) p->malloc(p,
		(size_t) (npixels * sizeof (uint32)), "pdf_open_TIFF");

	    if (!TIFFReadRGBAImage(image->info.tiff.tif,
		    w, h, image->info.tiff.raster, 1)) {
		p->free(p, (void *) image->info.tiff.raster);
		TIFFClose(image->info.tiff.tif);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
		    "Couldn't read image data from TIFF file '%s'", filename);
		}
		return -1;
	    }
	} else {
	    int linecounter = 0;

	    npixels = (size_t) (TIFFScanlineSize(image->info.tiff.tif) * h);
	    image->info.tiff.raster =
		(uint32 *) p->malloc(p, (size_t) npixels, "pdf_open_TIFF");

	    while (linecounter < image->height) {
		if (TIFFReadScanline(image->info.tiff.tif,
		    (tdata_t) (image->info.tiff.raster +
		    ((int)image->height - linecounter - 1) * (int)image->width),
		    (uint32) linecounter, (tsample_t) 0) == -1) {

		    p->free(p, (void *) image->info.tiff.raster);
		    TIFFClose(image->info.tiff.tif);
		    if (p->debug['i']) {
			pdf_error(p, PDF_NonfatalError,
			"Couldn't read image data from TIFF file '%s'",
			filename);
		    }
		    return -1;
		}
		linecounter++;
	    }
	}

	pdf_put_image(p, imageslot, pdf_true);

	if (image->info.tiff.raster)
	    p->free(p, (void *) image->info.tiff.raster);
    }

    image->in_use = pdf_true;			/* mark slot as used */
    image->filename = pdf_strdup(p, filename);

    TIFFClose(image->info.tiff.tif);

    return imageslot;
}

#endif	/* HAVE_LIBTIFF */
