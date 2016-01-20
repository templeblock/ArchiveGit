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

/* $Id: p_png.c,v 1.20.2.8 2002/01/26 18:32:18 tm Exp $
 *
 * PNG processing for PDFlib
 *
 */

#include <string.h>

#include "p_intern.h"
#include "p_image.h"

/* SPNG - Simple PNG
**
** The items below, prefixed with spng_, or SPNG_, establish a replacement
** for LIBPNG that works very fast, but processes simple PNG images only:
**	- bit_depth <= 8 (no 16-bit)
**	- interlace_type 0 (no interlacing)
**	- color_type 0, 2, or 3 (no alpha-channel); color type 3 requires
**        libpng for palette processing
*/
#define SPNG_SIGNATURE	"\211\120\116\107\015\012\032\012"

#define SPNG_CHUNK_IHDR	0x49484452
#define SPNG_CHUNK_PLTE	0x504C5445
#define SPNG_CHUNK_tRNS	0x74524E53
#define SPNG_CHUNK_IDAT	0x49444154
#define SPNG_CHUNK_IEND	0x49454E44

/* spng_init() return codes
*/
#define SPNG_ERR_OK	0	/* no error */
#define SPNG_ERR_NOPNG	1	/* bad PNG signature */
#define SPNG_ERR_FMT	2	/* bad PNG file format */

typedef struct
{
    /* from IHDR:
    */
    int		width;
    int		height;
    pdf_byte	bit_depth;
    pdf_byte	color_type;
    pdf_byte	compr_type;
    pdf_byte	filter_type;
    pdf_byte	interlace_type;
} spng_info;

static int
spng_getint(FILE *fp)
{
    unsigned char buf[4];

    if (fread(buf, 1, 4, fp) != 4)
	return -1;

    return ((int) buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
} /* spng_getint */

static int
spng_init(PDF *p, pdf_image *image, spng_info *spi)
{
    FILE *fp = image->fp;
    char buf[8];

    (void) p;

    /* check signature
    */
    if (fread(buf, 1, 8, fp) != 8 || strncmp(buf, SPNG_SIGNATURE, 8) != 0)
	return SPNG_ERR_NOPNG;

    /* read IHDR
    */
    if (spng_getint(fp) != 13 || spng_getint(fp) != SPNG_CHUNK_IHDR)
	return SPNG_ERR_FMT;

    spi->width = spng_getint(fp);
    spi->height = spng_getint(fp);

    if (fread(buf, 1, 5, fp) != 5)
	return SPNG_ERR_FMT;

    spi->bit_depth	= (pdf_byte) buf[0];
    spi->color_type	= (pdf_byte) buf[1];
    spi->compr_type	= (pdf_byte) buf[2];
    spi->filter_type	= (pdf_byte) buf[3];
    spi->interlace_type	= (pdf_byte) buf[4];

    (void) spng_getint(fp);	/* CRC */

    /* decide whether this image is "simple".
    */
#ifdef HAVE_LIBPNG
    if (spi->bit_depth > 8 || spi->color_type > 3 || spi->interlace_type != 0)
#else
    if (spi->bit_depth > 8 || spi->color_type > 2 || spi->interlace_type != 0)
#endif	/* !HAVE_LIBPNG */
    {
	image->use_raw = pdf_false;
	return SPNG_ERR_OK;
    }
    else
	image->use_raw = pdf_true;

    /* read (or skip) all chunks up to the first IDAT.
    */
    for (/* */ ; /* */ ; /* */)
    {
	int len = spng_getint(fp);
	int type = spng_getint(fp);

	switch (type)
	{
	    case SPNG_CHUNK_IDAT:		/* prepare data xfer	*/
		image->info.png.nbytes = (size_t) len;
		return SPNG_ERR_OK;

	    case -1:
		return SPNG_ERR_FMT;

	    /* if we decide to live without LIBPNG,
	    ** we should handle these cases, too.
	    */
	    case SPNG_CHUNK_tRNS:		/* transparency chunk	*/
	    case SPNG_CHUNK_PLTE:		/* read in palette	*/

	    default:
		fseek(fp, len + 4, SEEK_CUR);	/* skip data & CRC	*/
		break;
	} /* switch */
    }

    return SPNG_ERR_OK;
} /* spng_init */

#define PDF_PNG_BUFFERSIZE	8192

static void
pdf_data_source_PNG_init(PDF *p, PDF_data_source *src)
{
    static const char fn[] = "pdf_data_source_PNG_init";
    pdf_image	*image = (pdf_image *) src->private_data;

#ifdef HAVE_LIBPNG
    if (image->use_raw)
    {
#endif
	src->buffer_length = PDF_PNG_BUFFERSIZE;
	src->buffer_start = (pdf_byte *) p->malloc(p, src->buffer_length, fn);
	src->bytes_available = 0;
	src->next_byte = src->buffer_start;

#ifdef HAVE_LIBPNG
    }
    else
    {
	image->info.png.cur_line = 0;
	src->buffer_length = image->info.png.rowbytes;
    }
#endif
}

#undef min
#define min(a, b) (((a) < (b)) ? (a) : (b))

static void
spng_error(PDF *p, PDF_data_source *src)
{
    pdf_image	*image = (pdf_image *) src->private_data;
    char	filename[PDF_FILENAMELEN];

    strncpy(filename, image->filename, PDF_FILENAMELEN-1);
    filename[PDF_FILENAMELEN-1] = '\0';

    PDF_close_image(p, image - p->images);
    pdf_error(p, PDF_IOError, "Unexpected end of PNG file '%s'", filename);
} /* spng_error */

static pdf_bool
pdf_data_source_PNG_fill(PDF *p, PDF_data_source *src)
{
    pdf_image	*image = (pdf_image *) src->private_data;

#ifdef HAVE_LIBPNG
    if (image->use_raw)
    {
#endif
	FILE *	fp = image->fp;
	size_t	buf_avail = src->buffer_length;
	pdf_byte *scan = src->buffer_start;

	src->bytes_available = 0;

	if (image->info.png.nbytes == 0)
	    return pdf_false;

	do
	{
	    size_t nbytes = min(image->info.png.nbytes, buf_avail);

	    if (fread(scan, 1, nbytes, fp) != nbytes)
		spng_error(p, src);

	    src->bytes_available += nbytes;
	    scan += nbytes;
	    buf_avail -= nbytes;

	    if ((image->info.png.nbytes -= nbytes) == 0)
	    {
		/* proceed to next IDAT chunk
		*/
		(void) spng_getint(fp);				/* CRC */
		image->info.png.nbytes = (size_t) spng_getint(fp); /* length */

		if (spng_getint(fp) != SPNG_CHUNK_IDAT)
		{
		    image->info.png.nbytes = 0;
		    return pdf_true;
		}
	    }
	} while (buf_avail);

#ifdef HAVE_LIBPNG
    }
    else
    {
	if (image->info.png.cur_line == image->height)
	    return pdf_false;

	src->next_byte = image->info.png.raster +
	    image->info.png.cur_line * image->info.png.rowbytes;

	src->bytes_available = src->buffer_length;

	image->info.png.cur_line++;
    }
#endif	/* HAVE_LIBPNG */

    return pdf_true;
}

static void
pdf_data_source_PNG_terminate(PDF *p, PDF_data_source *src)
{
    pdf_image	*image = (pdf_image *) src->private_data;

#ifdef HAVE_LIBPNG
    if (image->use_raw)
    {
#endif
	p->free(p, (void *) src->buffer_start);

#ifdef HAVE_LIBPNG
    }
#endif
}

#ifdef HAVE_LIBPNG
/* 
 * We suppress libpng's warning message by supplying 
 * our own error and warning handlers
*/
static void
pdf_libpng_warning_handler(png_structp png_ptr, png_const_charp message)
{
    (void) png_ptr;	/* avoid compiler warning "unreferenced parameter" */
    (void) message;	/* avoid compiler warning "unreferenced parameter" */

    /* do nothing */
    return;
}

/*
 * use the libpng portability aid in an attempt to overcome version differences
 */
#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

struct errordata_s { jmp_buf jmpbuf; };

static void
pdf_libpng_error_handler(png_structp png_ptr, png_const_charp message)
{
    (void) message;	/* avoid compiler warning "unreferenced parameter" */

    longjmp(((struct errordata_s *) png_get_error_ptr(png_ptr))->jmpbuf, 1);
}

#ifdef PNG_PDFLIB_MEM_SUPPORTED
static void *
pdf_libpng_malloc(PDF *p, size_t size)
{
    return(p->malloc(p, size, "libpng"));
}

static void
pdf_libpng_free(PDF *p, void *mem)
{
    p->free(p, mem);
}
#endif	/* PNG_PDFLIB_MEM_SUPPORTED */

int
pdf_open_PNG_data(
    PDF *p,
    int imageslot,
    const char *filename,
    const char *stringparam,
    int intparam)
{
    FILE *save_fp;
    spng_info s_info;

    pdf_byte sig[8];
    png_uint_32 width, height, ui;
    png_bytep *row_pointers, trans;
    png_color_8p sig_bit;
    png_color_16p trans_values;
    struct errordata_s errordata;
    int bit_depth, color_type, i, num_trans;
    int PDF_VOLATILE mask = -1;
    float dpi_x, dpi_y;
    pdf_image *image;

    image = &p->images[imageslot];

#ifdef PNG_PDFLIB_MEM_SUPPORTED
    /* 
     * We can install our own memory handlers in libpng since
     * our PNG library is specially extended to support this.
     * But it is not safe to use this feature if an external libpng is used.
     */

    image->info.png.png_ptr = 
	png_create_read_struct_2(PNG_LIBPNG_VER_STRING, (png_voidp) &errordata,
	pdf_libpng_error_handler, pdf_libpng_warning_handler,
	p, (png_malloc_ptr) pdf_libpng_malloc,
	(png_free_ptr) pdf_libpng_free);
#else
    image->info.png.png_ptr =
	png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) &errordata,
	pdf_libpng_error_handler, pdf_libpng_warning_handler);
#endif

    if (!image->info.png.png_ptr) {
	pdf_error(p, PDF_MemoryError,
	    "Couldn't allocate memory for PNG handler");
    }

    image->info.png.info_ptr = png_create_info_struct(image->info.png.png_ptr);

    if (image->info.png.info_ptr == NULL) {
	png_destroy_read_struct(&image->info.png.png_ptr,
	    (png_infopp) NULL, (png_infopp) NULL);
	pdf_error(p, PDF_MemoryError,
	    "Couldn't allocate memory for PNG handler");
    }

    if (setjmp(errordata.jmpbuf)) {
	fclose(image->fp);
	png_destroy_read_struct(&image->info.png.png_ptr,
	    &image->info.png.info_ptr, NULL);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
		"Error reading PNG file '%s'", filename);
	}
	return -1;
    }

    /* now open the image file */
    if ((image->fp = fopen(filename, READMODE)) == NULL) {
	png_destroy_read_struct(&image->info.png.png_ptr,
		&image->info.png.info_ptr, NULL);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
		"Couldn't open PNG file '%s'", filename);
	}
	return -1;	/* Couldn't open PNG file */
    }

    if (fread(sig, 1, 8, image->fp) == 0 || !png_check_sig(sig, 8)) {
	fclose(image->fp);
	png_destroy_read_struct(&image->info.png.png_ptr,
		&image->info.png.info_ptr, NULL);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
		"File '%s' doesn't appear to be a PNG file",
	    filename);
	}
	return -1;
    }

    png_init_io(image->info.png.png_ptr, image->fp);
    png_set_sig_bytes(image->info.png.png_ptr, 8);
    png_read_info(image->info.png.png_ptr, image->info.png.info_ptr);
    png_get_IHDR(image->info.png.png_ptr, image->info.png.info_ptr,
	    &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    image->width	= (float) width;
    image->height	= (float) height;

    /* reduce 16-bit images to 8 bit since PDF stops at 8 bit */
    if (bit_depth == 16) {
	png_set_strip_16(image->info.png.png_ptr);
	bit_depth = 8;
    }

    image->bpc		= bit_depth;

    /* 
     * Since PDF doesn't support a real alpha channel but only binary
     * tranparency ("poor man's alpha"), we do our best and treat
     * alpha values of up to 50% as transparent, and values above 50%
     * as opaque.
     *
     * Since this behaviour is not exactly what the image author had in mind, 
     * it should probably be made user-configurable.
     *
     */
#define ALPHA_THRESHOLD 128

    switch (color_type) {
	case PNG_COLOR_TYPE_GRAY_ALPHA:
	    /* LATER: construct mask from alpha channel */
	    /*
	    png_set_IHDR(image->info.png.png_ptr, image->info.png.info_ptr,
	    	width, height, bit_depth, PNG_COLOR_MASK_ALPHA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	    */
	    png_set_strip_alpha(image->info.png.png_ptr);
	    /* fall through */

	case PNG_COLOR_TYPE_GRAY:
	    if (png_get_sBIT(image->info.png.png_ptr,
	    		image->info.png.info_ptr, &sig_bit)) {
		png_set_shift(image->info.png.png_ptr, sig_bit);
	    }

	    image->colorspace	= DeviceGray;
	    image->components	= 1;
	    break;

	case PNG_COLOR_TYPE_RGB_ALPHA:
	    /* LATER: construct mask from alpha channel */
	    png_set_strip_alpha(image->info.png.png_ptr);
	    /* fall through */

	case PNG_COLOR_TYPE_RGB:
	    image->colorspace	= DeviceRGB;
	    image->components	= 3;

	    break;

	case PNG_COLOR_TYPE_PALETTE:
	    png_get_PLTE(image->info.png.png_ptr, image->info.png.info_ptr,
			(png_colorp*) &image->colormap, &image->palette_size);

	    image->colorspace	= Indexed;
	    image->components	= 1;

	    break;
    }

    if (stringparam && *stringparam) {
        if (!strcmp(stringparam, "colorize")) {
            if (image->components != 1) {
                fclose(image->fp);
                pdf_error(p, PDF_ValueError,
		    "Can't colorize PNG image with more than 1 component");
            }
	    if (intparam >= p->colorspaces_number) {
		fclose(image->fp);
                pdf_error(p, PDF_ValueError,
		    "Invalid color number %d for image %s", intparam, filename);
	    }
            image->colorspace = (pdf_colorspace) (intparam + LastCS);

	} else if (!strcmp(stringparam, "mask")) {
	    if (image->components != 1 || image->bpc != 1) {
		fclose(image->fp);
		pdf_error(p, PDF_ValueError,
		    "Can't handle PNG image mask with more than 1 bit");
	    }
	    image->colorspace = ImageMask;

	} else if (!strcmp(stringparam, "masked")) {
	    mask = intparam;
	    if (mask >= 0 && (mask >= p->images_capacity ||
		!p->images[mask].in_use || p->images[mask].strips != 1 ||
		p->images[mask].colorspace != ImageMask)) {
		    fclose(image->fp);
		    pdf_error(p, PDF_ValueError,
		    "Bad image mask (no %d) for image '%s'", mask, filename);
	    }
	} else if (!strcmp(stringparam, "ignoremask")) {
	    /* we invert this flag later */
	    image->transparent = pdf_true;
	} else {
	    fclose(image->fp);
	    pdf_error(p, PDF_ValueError,
	    	"Unknown parameter %s in pdf_open_PNG", stringparam);
	}
    }

    image->mask = mask;

    /* let libpng expand interlaced images */
    (void) png_set_interlace_handling(image->info.png.png_ptr);

    /* read the physical dimensions chunk to find the resolution values */
    dpi_x = (float) png_get_x_pixels_per_meter(image->info.png.png_ptr,
    		image->info.png.info_ptr);
    dpi_y = (float) png_get_y_pixels_per_meter(image->info.png.png_ptr,
    		image->info.png.info_ptr);

    if (dpi_x != 0 && dpi_y != 0) {	/* absolute values */
	image->dpi_x = dpi_x * (float) 0.0254;
	image->dpi_y = dpi_y * (float) 0.0254;

    } else {				/* aspect ratio */
	image->dpi_y = -png_get_pixel_aspect_ratio(image->info.png.png_ptr,
			    image->info.png.info_ptr);

	if (image->dpi_y == (float) 0)	/* unknown */
	    image->dpi_x = (float) 0;
	else
	    image->dpi_x = (float) -1.0;
    }

    /* read the transparency chunk */
    if (png_get_valid(image->info.png.png_ptr, image->info.png.info_ptr,
    	PNG_INFO_tRNS)) {
	png_get_tRNS(image->info.png.png_ptr, image->info.png.info_ptr,
	    &trans, &num_trans, &trans_values);
	if (num_trans > 0) {
	    if (color_type == PNG_COLOR_TYPE_GRAY) {
		image->transparent = !image->transparent;
		/* LATER: scale down 16-bit transparency values ? */
		image->transval[0] = (pdf_byte) trans_values[0].gray;

	    } else if (color_type == PNG_COLOR_TYPE_RGB) {
		image->transparent = !image->transparent;
		/* LATER: scale down 16-bit transparency values ? */
		image->transval[0] = (pdf_byte) trans_values[0].red;
		image->transval[1] = (pdf_byte) trans_values[0].green;
		image->transval[2] = (pdf_byte) trans_values[0].blue;

	    } else if (color_type == PNG_COLOR_TYPE_PALETTE) {
		/* we use the first transparent entry in the tRNS palette */
		for (i = 0; i < num_trans; i++) {
		    if ((pdf_byte) trans[i] < ALPHA_THRESHOLD) {
			image->transparent = !image->transparent;
			image->transval[0] = (pdf_byte) i;
			break;
		    }
		}
	    }
	}
    }

    png_read_update_info(image->info.png.png_ptr, image->info.png.info_ptr);

    image->info.png.rowbytes =
	png_get_rowbytes(image->info.png.png_ptr, image->info.png.info_ptr);

    image->info.png.raster = (pdf_byte *)
	p->malloc(p,image->info.png.rowbytes * height, "pdf_open_PNG/raster");

    row_pointers = (png_bytep *)
	p->malloc(p, height * sizeof(png_bytep), "pdf_open_PNG/pointers");

    for (ui = 0; ui < height; ui++) {
	row_pointers[ui] =
	    image->info.png.raster + ui * image->info.png.rowbytes;
    }

    /* try the simple way:
    */
    save_fp = image->fp;

    image->src.init		= pdf_data_source_PNG_init;
    image->src.fill		= pdf_data_source_PNG_fill;
    image->src.terminate	= pdf_data_source_PNG_terminate;
    image->src.private_data	= (void *) image;


    if ((image->fp = fopen(filename, READMODE)) != NULL
	&& spng_init(p, image, &s_info) == SPNG_ERR_OK && image->use_raw)
    {
	fclose(save_fp);
	image->predictor	= pred_png;
	image->compression	= flate;
    }
    else
    {
	if (image->fp != (FILE *) 0)
	    fclose(image->fp);

	image->fp = save_fp;

	/* fetch the actual image data */
	png_read_image(image->info.png.png_ptr, row_pointers);
    }

    image->in_use		= pdf_true;		/* mark slot as used */
    image->filename		= pdf_strdup(p, filename);

    pdf_put_image(p, imageslot, pdf_true);

    fclose(image->fp);

    p->free(p, image->info.png.raster);
    p->free(p, row_pointers);

    png_destroy_read_struct(&image->info.png.png_ptr,
	    &image->info.png.info_ptr, NULL);

    return imageslot;
}

#else	/* !HAVE_LIBPNG */

/* simple built-in PNG reader without libpng */

int
pdf_open_PNG_data(
    PDF *p,
    int imageslot,
    const char *filename,
    const char *stringparam,
    int intparam)
{
    spng_info s_info;
    int PDF_VOLATILE mask = -1;
    pdf_image *image;

    image = &p->images[imageslot];

    if (stringparam && !strcmp(stringparam, "masked")) {
	mask = intparam;
	if (mask >= 0 &&
	    (mask >= p->images_capacity || !p->images[mask].in_use ||
	    p->images[mask].colorspace != ImageMask))
	    pdf_error(p, PDF_ValueError,
		"Bad image mask (no %d) for image '%s'", mask, filename);
    }

    image->src.init		= pdf_data_source_PNG_init;
    image->src.fill		= pdf_data_source_PNG_fill;
    image->src.terminate	= pdf_data_source_PNG_terminate;
    image->src.private_data	= (void *) image;

    if ((image->fp = fopen(filename, READMODE)) != NULL
	&& spng_init(p, image, &s_info) == SPNG_ERR_OK && image->use_raw)
    {
	image->in_use		= pdf_true;		/* mark slot as used */
	image->filename		= pdf_strdup(p, filename);

	image->predictor	= pred_png;
	image->compression	= flate;

	image->width		= (float) s_info.width;
	image->height		= (float) s_info.height;
	image->bpc		= s_info.bit_depth;

	image->components	= 3;
	image->mask		= mask;

	/* other types are rejected in spng_init() */
	image->colorspace	= 
			(s_info.color_type == 0 ? DeviceGray : DeviceRGB);
    }
    else
    {
	fclose(image->fp);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
	    "File '%s' doesn't appear to be a PNG file, or libpng not attached",
	    filename);
	}
	return -1;
    }

    pdf_put_image(p, imageslot, pdf_true);

    fclose(image->fp);

    return imageslot;
}

#endif	/* !HAVE_LIBPNG */
