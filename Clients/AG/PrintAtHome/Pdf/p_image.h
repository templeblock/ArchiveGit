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

/* $Id: p_image.h,v 1.13.2.2 2002/01/07 18:26:29 tm Exp $
 *
 * Header file for the PDFlib image subsystem
 *
 */

#ifndef P_IMAGE_H
#define P_IMAGE_H

#ifdef HAVE_LIBTIFF
#include "tiffio.h"
#endif

#ifdef HAVE_LIBPNG
#include "png.h"
#endif

/* JPEG specific image information */
typedef struct pdf_jpeg_info_t {
    long		startpos;	/* start of image data in file */
} pdf_jpeg_info;

/* GIF specific image information */
typedef struct pdf_gif_info_t {
    int			c_size;		/* current code size (9..12)	*/
    int			t_size;		/* current table size (257...)	*/
    int			i_buff;		/* input buffer			*/
    int			i_bits;		/* # of bits in i_buff		*/
    int			o_buff;		/* output buffer		*/
    int			o_bits;		/* # of bits in o_buff		*/
} pdf_gif_info;


/* PNG specific image information */
typedef struct pdf_png_info_t {
    size_t		nbytes;		/* number of bytes left		*/
    					/* in current IDAT chunk	*/
#ifdef HAVE_LIBPNG
    png_structp		png_ptr;
    png_infop		info_ptr;
    png_uint_32		rowbytes;
    pdf_byte		*raster;
    int			cur_line;
#endif	/* HAVE_LIBPNG */
} pdf_png_info;


/* TIFF specific image information */
typedef struct pdf_tiff_info_t {
#ifdef HAVE_LIBTIFF
    TIFF		*tif;		/* pointer to TIFF data structure */
    uint32		*raster;	/* frame buffer */
#endif	/* HAVE_LIBTIFF */

    int			cur_line;	/* current image row or strip */
} pdf_tiff_info;

/* CCITT specific image information */
typedef struct pdf_ccitt_info_t {
    int			BitReverse;	/* reverse all bits prior to use */
} pdf_ccitt_info;

/* Type of image reference */
typedef enum {pdf_ref_direct, pdf_ref_file, pdf_ref_url} pdf_ref_type;

typedef pdf_byte pdf_colormap[256][3];	/* colormap */
typedef enum { none, lzw, runlength, ccitt, dct, flate } pdf_compression;
typedef enum { pred_default = 1, pred_tiff = 2, pred_png = 15 } pdf_predictor;

/* The image descriptor */
struct pdf_image_s {
    FILE		*fp;		/* image file pointer */
    char		*filename;	/* image file name or url */
    pdf_ref_type	reference;	/* kind of image data reference */
    /* width and height in pixels, or in points for PDF pages and templates */
    float		width;		/* image width */
    float		height;		/* image height */
    int			bpc;		/* bits per color component */
    int			components;	/* number of color components */
    pdf_compression	compression;	/* image compression type */
    pdf_colorspace	colorspace;	/* image color space */

    pdf_bool		invert;		/* invert: reverse black and white */
    pdf_bool		transparent;	/* image is transparent */
    pdf_byte		transval[4];	/* transparent color values */
    int			mask;		/* image number of image mask, or -1 */
    pdf_predictor	predictor;	/* predictor for lzw and flate */

    int			palette_size;	/* # of palette entries (not bytes!) */
    pdf_colormap 	*colormap;	/* pointer to color palette */

    float		dpi_x;		/* horiz. resolution in dots per inch */
    float		dpi_y;		/* vert. resolution in dots per inch */
    					/* dpi is 0 if unknown */

    pdf_bool		in_use;		/* image slot currently in use */

    char		*params;	/* K and BlackIs1 for TIFF and CCITT */
    int			strips;		/* number of strips in image */
    int			rowsperstrip;	/* number of rows per strip */
    id			colormap_id;	/* reuse colormap for multiple strips */
    int			pagehandle;	/* PDI page handle */
    int			dochandle;	/* PDI document handle */
    pdf_bool		use_raw;	/* use raw (compressed) image data */

    /* image format specific information */
    union {
	pdf_jpeg_info	jpeg;
	pdf_gif_info	gif;
	pdf_png_info	png;
	pdf_tiff_info	tiff;
	pdf_ccitt_info	ccitt;
    } info;

    int			no;		/* PDF image number */
    PDF_data_source	src;
};

/* xobject types */
typedef enum {
    image_xobject = 1 << 0,
    form_xobject = 1 << 1,
    pdi_xobject = 1 << 2
} pdf_xobj_type;

typedef enum {
    xobj_flag_used = 1 << 0,		/* in use */
    xobj_flag_write = 1 << 1		/* write at end of page */
} pdf_xobj_flags;

/* A PDF xobject */
struct pdf_xobject_s {
    id			obj_id;		/* object id of this xobject */
    int			flags;		/* bit mask of pdf_xobj_flags */
    pdf_xobj_type	type;		/* type of this xobject */
};

/* p_tiff.c */
int 	pdf_open_TIFF_data(PDF *p, int imageslot, const char *filename,
		const char *stringparam, int intparam);

/* p_gif.c */
int	 pdf_open_GIF_data(PDF *p, int imageslot, const char *filename,
		const char *stringparam, int intparam);

/* p_jpeg.c */
int	 pdf_open_JPEG_data(PDF *p, int imageslot, const char *filename,
		const char *stringparam, int intparam);

/* p_png.c */
int	 pdf_open_PNG_data(PDF *p, int imageslot, const char *filename,
		const char *stringparam, int intparam);

/* p_image.c */
int	pdf_new_xobject(PDF *p, pdf_xobj_type type, id obj_id);
void	pdf_grow_images(PDF *p);
void	pdf_put_image(PDF *p, int im, pdf_bool firststrip);

#endif /* P_IMAGE_H */
