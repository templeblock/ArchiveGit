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

/* $Id: p_stream.c,v 1.38.10.8 2002/01/28 12:54:48 tm Exp $
 *
 * Stream handling routines for PDFlib 
 *
 */

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "p_intern.h"

#ifdef HAVE_LIBZ
#include "zlib.h"
#endif

struct pdf_stream_s {
    pdf_byte	*basepos;		/* start of this chunk */
    pdf_byte	*curpos;		/* next write position */
    pdf_byte	*maxpos;		/* maximum position of chunk */
    size_t	base_offset;		/* base offset of this chunk */
    pdf_bool	compressing;		/* in compression state */
#ifdef HAVE_LIBZ
    z_stream	z;			/* zlib compression stream */
#endif
};

#ifdef HAVE_LIBZ
/* wrapper for p->malloc for use in zlib */
static voidpf
pdf_zlib_alloc(voidpf p, uInt items, uInt size)
{
    return (voidpf) ((PDF *)p)->malloc((PDF *)p, items * size, "zlib");
}

#endif	/* HAVE_LIBZ */

/*
 * The caller must use the contents of the returned buffer before
 * calling the next PDFlib function.
 */

PDFLIB_API const char * PDFLIB_CALL
PDF_get_buffer(PDF *p, long *size)
{
    static const char fn[] = "PDF_get_buffer";

    PDF_TRACE(("%s\t(pdf[%p], [%p]);", fn, (void *) p, (void *) size));

    if (PDF_SANITY_CHECK_FAILED(p)) {
	*size = (long) 0;
	PDF_TRACE((" [NULL, size=0]\n"));
	return ((const char *) NULL);
    }

    if (p->writeproc)
	pdf_error(p, PDF_RuntimeError,
	    "Don't use PDF_get_buffer() when writing to file");

    PDF_CHECK_SCOPE(p, fn, pdf_state_object | pdf_state_document);

    *size = (long) (p->stream->curpos - p->stream->basepos);

    p->stream->base_offset +=
	(size_t) (p->stream->curpos - p->stream->basepos);
    p->stream->curpos = p->stream->basepos;

    PDF_TRACE((" [%p, size=%ld]\n", (void *) (p->stream->basepos), *size));

    return (const char *) p->stream->basepos;
}

pdf_bool
pdf_buffer_not_empty(PDF *p)
{
    return(!p->writeproc && p->stream->curpos != p->stream->basepos);
}

void
pdf_boot_stream(PDF *p)
{
    p->stream = (pdf_stream *) p->malloc(p, sizeof(pdf_stream),
		"pdf_boot_stream");

    /* curpos must be initialized here so that the check for empty
     * buffer in PDF_delete() also works in the degenerate case of
     * no output produced.
     */
    p->stream->basepos = p->stream->curpos = NULL;
}

void
pdf_init_stream(PDF *p)
{
    /*
     * This may be left over from the previous run. We deliberately
     * don't reuse the previous buffer in order to avoid potentially
     * unwanted growth of the allocated buffer due to a single large
     * document in a longer series of documents.
     */
    if (p->stream->basepos)
	p->free(p, (void *) p->stream->basepos);

    p->stream->basepos		= (pdf_byte *)p->malloc(p, STREAM_CHUNKSIZE,
				    "pdf_open_stream");
    p->stream->curpos		= p->stream->basepos;
    p->stream->maxpos		= p->stream->basepos + STREAM_CHUNKSIZE;
    p->stream->base_offset	= 0L;
    p->stream->compressing	= pdf_false;

#ifdef HAVE_LIBZ
    p->stream->z.zalloc	= (alloc_func) pdf_zlib_alloc;
    p->stream->z.zfree	= (free_func) p->free;
    p->stream->z.opaque	= (voidpf) p;
#endif
}

static void
pdf_check_stream(PDF *p, size_t len)
{
    size_t max;
    int cur;
    
    if (p->stream->curpos + len <= p->stream->maxpos)
	return;

    if (p->flush & PDF_FLUSH_HEAVY) {
	pdf_flush_stream(p);

	if (p->stream->curpos + len <= p->stream->maxpos)
	    return;
    }

    max = (size_t) (2 * (p->stream->maxpos - p->stream->basepos));
    cur = p->stream->curpos - p->stream->basepos;

    p->stream->basepos = (pdf_byte *)
	p->realloc(p, (void *) p->stream->basepos, max, "pdf_check_stream");
    p->stream->maxpos = p->stream->basepos + max;
    p->stream->curpos = p->stream->basepos + cur;

    pdf_check_stream(p, len);
}

void
pdf_flush_stream(PDF *p)
{
    size_t size;

    if (!p->writeproc || p->stream->compressing)
	return;

    size = (size_t) (p->stream->curpos - p->stream->basepos);

    if (p->writeproc(p, (void *) p->stream->basepos, size) != size) {
	p->free(p, p->stream->basepos);
	p->stream->basepos = NULL;
	pdf_error(p, PDF_IOError, "Couldn't write output (disk full?)");
    }

    p->stream->base_offset += (size_t) (p->stream->curpos - p->stream->basepos);
    p->stream->curpos = p->stream->basepos;
}

void
pdf_close_stream(PDF *p)
{
    /* this may happen in rare cases */
    if (!p->stream->basepos)
	return;

    if (p->stream->basepos) {
	p->free(p, (void *) p->stream->basepos);
	p->stream->basepos = NULL;
    }

    p->free(p, (void *) p->stream);
}

long
pdf_tell(PDF *p)
{
    return(p->stream->base_offset + p->stream->curpos - p->stream->basepos);
}

int
pdf_compresslevel(PDF *p)
{
    return p->compresslevel;
}

void
pdf_compress_init(PDF *p)
{
    if (!p->compresslevel) {
	p->stream->compressing = pdf_false;
	return;
    }

#ifdef HAVE_LIBZ
    if (deflateInit(&p->stream->z, p->compresslevel) != Z_OK)
	pdf_error(p, PDF_SystemError, "Compression error (deflateInit)");

    p->stream->z.avail_in = 0;
#endif /* HAVE_LIBZ */

    p->stream->compressing = pdf_true;
}

void
pdf_compress_end(PDF *p)
{
    int status;

    /* this may happen during cleanup triggered by an exception handler */
    if (!p->stream->compressing)
	return;

    if (!p->compresslevel) {
	p->stream->compressing = pdf_false;
	return;
    }

#ifdef HAVE_LIBZ
    /* Finish the stream */
    do {
	pdf_check_stream(p, 128);
	p->stream->z.next_out = (Bytef *) p->stream->curpos;
	p->stream->z.avail_out =
	    (uInt) (p->stream->maxpos - p->stream->curpos);

	status = deflate(&(p->stream->z), Z_FINISH);
	p->stream->curpos = p->stream->z.next_out;

	if (status != Z_STREAM_END && status != Z_OK)
	    pdf_error(p, PDF_SystemError, "Compression error (Z_FINISH)");

    } while (status != Z_STREAM_END);

    if (deflateEnd(&p->stream->z) != Z_OK)
	pdf_error(p, PDF_SystemError, "Compression error (deflateEnd)");

    p->stream->compressing = pdf_false;
#endif /* HAVE_LIBZ */
}

/* **************************** output functions ************************ */

/*
 * Write binary data to the output without any modification,
 * and apply compression if we are currently in compression mode.
 */
void
pdf_write(PDF *p, const void *data, size_t size)
{
#ifdef HAVE_LIBZ
    if (p->stream->compressing) {
	p->stream->z.avail_in	= (uInt) size;
	p->stream->z.next_in	= (Bytef *) data;
	p->stream->z.avail_out	= 0;

	while (p->stream->z.avail_in > 0) {
	    if (p->stream->z.avail_out == 0) {
		/* estimate output buffer size */
		pdf_check_stream(p, (size_t) (p->stream->z.avail_in/4 + 16));
		p->stream->z.next_out	= (Bytef *) p->stream->curpos;
		p->stream->z.avail_out	= 
			(uInt) (p->stream->maxpos - p->stream->curpos);
	    }

	    if (deflate(&(p->stream->z), Z_NO_FLUSH) != Z_OK)
		pdf_error(p, PDF_SystemError, "Compression error (Z_NO_FLUSH)");

	   p->stream->curpos = p->stream->z.next_out;
	}
    }else {
#endif /* HAVE_LIBZ */

	pdf_check_stream(p, size);
	memcpy(p->stream->curpos, data, size);
	p->stream->curpos += size;

#ifdef HAVE_LIBZ
    }
#endif /* HAVE_LIBZ */
}


/*
 * Write a string to the output.
 */

void
pdf_puts(PDF *p, const char *s)
{
    pdf_write(p, (void *) s, strlen(s));
}


/* Write a character to the output without any modification. */

void
pdf_putc(PDF *p, char c)
{
    pdf_write(p, (void *) &c, (size_t) 1);
}

/*
 * Write a formatted string to the output, converting from native
 * encoding to ASCII if necessary.
 */

static const char digits[] = "0123456789ABCDEF";

static char *
pdf_ltoa(char *buf, long n, int width, char pad, int base)
{
    char	aux[20];
    int		k, i = sizeof aux;
    char *	dest = buf;
    long ul = n;
    ldiv_t qr;

    if (n == 0)
    {
	if (width == 0)
	    width = 1;

	for (k = 0; k < width; ++k)
	    *(dest++) = '0';

	return dest;
    }

    if (n < 0 && base == 10)
    {
	ul = -ul;	/* safe against overflow, while "n = -n" isn't! */
	*(dest++) = '-';
    }

    n = (long) (ul / base);
    aux[--i] = digits[ul % base];

    while (0 < n)
    {
	qr = ldiv(n, base);

	n = qr.quot;
	aux[--i] = digits[qr.rem];
    }

    width -= (int) (sizeof aux) - i;
    for (k = 0; k < width; ++k)
	*(dest++) = pad;

    memcpy(dest, &aux[i], sizeof aux - i);
    return dest + sizeof aux - i;
} /* pdf_ltoa */

static char *
pdf_ftoa(PDF *p, char *buf, double x)
{
    static const long pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };

    char *	dest = buf;
    double	integ, fract;
    long	f;

    if (fabs(x) < PDF_SMALLREAL)
    {
	*dest = '0';
	return dest + 1;
    }

    if (x < 0)
    {
	x = -x;
	*(dest++) = '-';
    }

    if (x >= PDF_BIGREAL)
    {
	if (x > PDF_BIGINT)
	    pdf_error(p, PDF_ValueError,
		"floating point value too large in pdf_ftoa");

	return pdf_ltoa(dest, (long) floor(x + 0.5), 0, ' ', 10);
    }

    fract = modf(x, &integ);
    f = (long) floor(fract * pow10[p->floatdigits] + 0.5);

    if (f == pow10[p->floatdigits])
    {
	integ += 1.0;
	f = 0;
    }

    if (integ == 0 && f == 0)	/* avoid "-0" */
	dest = buf;

    dest = pdf_ltoa(dest, (long) integ, 0, ' ', 10);

    if (f != 0)
    {
	ldiv_t	qr;
	char *	aux;
	int	i = p->floatdigits;

	*(dest++) = '.';

	do	/* avoid trailing zeros */
	{
	    qr = ldiv(f, 10);
	    f = qr.quot;
	    --i;
	} while (qr.rem == 0);

	aux = dest + i + 1;
	dest[i--] = digits[qr.rem];

	for (; 0 <= i; --i)
	{
	    qr = ldiv(f, 10);
	    f = qr.quot;
	    dest[i] = digits[qr.rem];
	}

	return aux;
    }

    return dest;
} /* pdf_ftoa */

int
pdf_vsprintf(PDF *p, char *buf, const char *format, va_list args)
{
    char *dest = buf;

    for (/* */ ; /* */ ; /* */)
    {
	int	width = 0;
	char	pad = ' ';

	/* as long as there is no '%', just print.
	*/
	while (*format != 0 && *format != '%')
	    *(dest++) = *(format++);

	if (*format == 0)
	{
	    *dest = 0;
	    return dest - buf;
	}

	if (*(++format) == '0')
	{
	    pad = '0';
	    ++format;
	}

	while (isdigit(*format))
	    width = 10*width + *(format++) - '0';

	switch (*format)
	{
	    case 'X':
		dest = pdf_ltoa(dest, va_arg(args, int), width, pad, 16);
		break;

	    case 'c':
		*(dest++) = (char) va_arg(args, int);
		break;

	    case 'd':
		dest = pdf_ltoa(dest, va_arg(args, int), width, pad, 10);
		break;

	    case 'f':
		dest = pdf_ftoa(p, dest, va_arg(args, double));
		break;

	    case 'l':
	    {
		long n = va_arg(args, long);

		switch (*(++format))
		{
		    case 'X':
			dest = pdf_ltoa(dest, n, width, pad, 16);
			break;

		    case 'd':
			dest = pdf_ltoa(dest, n, width, pad, 10);
			break;

		    default:
			pdf_error(p, PDF_SystemError,
			    "unknown format 'l%c' (%02X) in pdf_vsprintf",
				isprint(*format) ? *format : '?', *format);
		}

		break;
	    }

	    case 's':
	    {
		char *	str = va_arg(args, char *);
		size_t	len = strlen(str);

		memcpy(dest, str, len);
		dest += len;
		break;
	    }

	    case '%':
		*(dest++) = '%';
		break;

	    default:
		pdf_error(p, PDF_SystemError,
		    "unknown format specification '%c' (%02X) in pdf_vsprintf",
			    isprint(*format) ? *format : '?', *format);
	} /* switch */

	++format;
    } /* loop */
} /* pdf_vsprintf */


void
pdf_printf(PDF *p, const char *fmt, ...)
{
    char	buf[PDF_LINEBUFLEN];	/* formatting buffer */
    va_list ap;

    va_start(ap, fmt);

    pdf_vsprintf(p, buf, fmt, ap);
    pdf_puts(p, buf);

    va_end(ap);
}

