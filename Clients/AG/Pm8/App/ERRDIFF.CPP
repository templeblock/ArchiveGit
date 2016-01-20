/*
// ERRDIFF.CPP
//
// Copyright (C) 1993 MicroLogic Software, Inc.
//
// Error diffusion routines.
// June 9, 1992 - Jay N. : Initial creation (split from image.c)
//
// $Header: /PM8/App/ERRDIFF.CPP 1     3/03/99 6:05p Gbeddow $
//
// $Log: /PM8/App/ERRDIFF.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:20   Fred
// Initial revision.
// 
//    Rev 1.2   11 Apr 1997 11:23:28   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.1   01 Nov 1996 17:11:38   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:47:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:24   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:42   JAY
// Initial revision.
// 
//    Rev 1.5   30 Dec 1994 13:59:58   JAY
// New dither method with expanded color set (based on WinG palette).
// 
//    Rev 1.4   30 Nov 1994 16:35:38   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Aug 1994 07:33:44   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   10 Feb 1994 15:36:02   JAY
// Fixed bug in setup_translate_tables (and elsewhere) relating to rc gamma
// curve and also BMP red/blue flip.
// 
//    Rev 1.1   05 Jan 1994 08:06:12   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:08   JAY
// 
// 
*/

#include	"stdafx.h"

#include	<string.h>

#include	"pmgobj.h"
#include	"grafrec.h"
#include	"grafobj.h"
#include	"image.h"
#include	"dither.h"
#include "utils.h"

/*
// Process the next row with the error diffusion method (smoothed).
// Note that since we are smoothing, the data is pre-translated.
*/

VOID ImageContext::process_errdiff_smoothed(int plane, LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width)
{
 	SHORT far *e, far *ep;
	SHORT delta, compare, fraction;
	UCHAR dest_mask;
	SHORT error, count, cur_error;
	SHORT Alpha, Beta, Gamma, Delta;
	LPBYTE s, d;
	SHORT smooth_inc, smooth_final, smooth_value;
	SHORT smooth_fraction, smooth_compare, smooth_delta;

#if 0
	printf("sf:%d, df:%d, xoff:%d, w:%d, sw:%d\n",
						src_factor, dst_factor, x_offset, width, src_width);
#endif

/* Initialize the source and destination pointers. */

	memset(dp, 0, (width+7)/8);

/* Initialize the DDA variables. */

	delta = dst_factor;
	compare = src_factor;
	fraction = 0;

	dest_mask = 0x80;

	ep = (*error_info)->errors + (plane*(width+2));
	e = ep+1;

	s = sp;
	d = dp;

/* Initialize the pointers for the error buffers. */

	if ((*error_info)->lineno & 1)
	{
		SHORT soff;

		x_offset += width-1;
		e += width-1;

		soff = scale_number(x_offset+1, src_factor, dst_factor);
//		if ((fraction = delta - (SHORT)(((LONG)(x_offset+1)*(LONG)src_factor)%(LONG)dst_factor)) == dst_factor)
		if ((fraction = delta - mulrem(x_offset+1, src_factor, dst_factor)) == dst_factor)
		{
			fraction = 0;
			soff--;
		}

		s = sp+soff;

		if (soff >= src_width)
		{
#if 0
			printf("Off the end! src:%d, dst:%d, width:%d, src_width:%d\n",
									src_factor, dst_factor, width, src_width);
#endif
		}

	/* Reset and recalc the destination variables. */

		x_offset = width-1;
		dest_mask = 0x80 >> (x_offset & 7);
		d = dp + (x_offset >> 3);

		cur_error = *e;
		*e = 0;
		smooth_value = (SHORT)*s;
		if (soff-- > 0)
		{
			s--;
		}
		smooth_compare = smooth_pixel_width;

	/* Grab source byte and advance to the previous source byte. */

		smooth_final = (SHORT)*s;
		if (soff-- > 0)
		{
			s--;
		}

		smooth_inc = 1;

		smooth_fraction = 0;

		if ((smooth_delta = smooth_final - smooth_value) < 0)
		{
			smooth_delta = -smooth_delta;
			smooth_inc = -1;
		}

		for (count = width; count > 0; )
		{
		/* This byte is a keeper. */

		/* Compute the error corrected value. */

			error = smooth_value + cur_error;

//			printf("[b:%d] *e:%d {%Fp} ", byte, *e, e);

		/* Compute the value we will set. */
		/* Compute the error in this case. */

			if (error > 127)
			{
			/* Above threshold. Approximation is 255. Error is value-255. */
				error -= 255;
				*d |= dest_mask;
			}
			else
			{
			/* Below threshold. Approximation is 0. Error is value-0. */
			}

		/* Distribute the error in the correct directions. */

			Delta = error;
			Delta -= (Alpha = (7*error) >> 4);
			Delta -= (Beta  = (3*error) >> 4);
			Delta -= (Gamma = (5*error) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			cur_error = e[-1] + Alpha;				/* for next time */

			e[1] += Beta;
			e[0] += Gamma;
			e[-1] = Delta;

		/* Handle the destination bit. */

			if ((dest_mask <<= 1) == 0)
			{
				dest_mask = 0x01;
				d--;
			}
			e--;

			if (--count == 0)
			{
				break;
			}

			fraction += compare;
			if (fraction >= delta)
			{
				while (fraction >= delta)
				{
					fraction -= delta;

				/* Fix this for now in case it's not right. */

					smooth_value = smooth_final;

				/* Grab source byte and advance to the previous source byte. */

					smooth_final = (SHORT)*s;
					if (soff-- > 0)
					{
						s--;
					}

					smooth_inc = 1;

					smooth_fraction = 0;
					if ((smooth_delta = smooth_final - smooth_value) < 0)
					{
						smooth_delta = -smooth_delta;
						smooth_inc = -1;
					}
				}
			}
			else
			{
			/* Update the smoothing if smoothing is happening. */

				if (smooth_delta != 0)
				{
					smooth_fraction += smooth_delta;
					while (smooth_fraction >= smooth_compare)
					{
						smooth_fraction -= smooth_compare;
						smooth_value += smooth_inc;
					}
				/* Stop smoothing if we hit the final value. */
					if (smooth_value == smooth_final)
					{
						smooth_delta = 0;
					}
				}
			}
		}
		if (d < dp && dest_mask != 1)
		{
//			printf("R:DD %Fp < %Fp (%02x) DD\n", d, dp, dest_mask);
		}
		if (s < sp-1)
		{
//			printf("R:SS %Fp < %Fp SS\n", s, sp);
		}
		if (e < ep-1)
		{
//			printf("R:EE %Fp < %Fp EE\n", e, ep);
		}
		if (((delta >= compare) && fraction != 0)
				|| ((delta < compare) && fraction != delta))
		{
//			printf("R:FF f:%d d:%d c:%d FF\n", fraction, delta, compare);
		}
	}
	else
	{
		SHORT soff = 0;

		if (x_offset != 0)
		{
			soff = scale_number(x_offset+1, compare, delta);
//			if ((fraction = delta - (SHORT)(((LONG)(x_offset+1)*(LONG)compare)%(LONG)delta)) == delta)
			if ((fraction = delta - mulrem(x_offset+1, compare, delta)) == delta)
			{
				fraction = 0;
				soff--;
			}
			s += soff;
			fraction += compare - delta;
		}

		cur_error = *e;
		*e = 0;
		smooth_value = soff == 0 ? (SHORT)*s : (SHORT)*(s-1);
		smooth_compare = smooth_pixel_width;

		for (count = width; count > 0; )
		{
		/* Grab source byte and advance to the next source byte. */

			smooth_final = (SHORT)*s++;

			smooth_fraction = 0;
			smooth_inc = 1;

			if ((smooth_delta = smooth_final - smooth_value) < 0)
			{
				smooth_delta = -smooth_delta;
				smooth_inc = -1;
			}

			if ((fraction += delta) >= compare)
			{
			/* This byte is a keeper. */

				while (fraction >= compare)
				{
					fraction -= compare;

				/* Compute the error corrected value. */

					error = smooth_value + cur_error;

//					printf("[b:%d] *e:%d {%Fp} ", byte, *e, e);

				/* Compute the value we will set. */
				/* Compute the error in this case. */

					if (error > 127)
					{
					/* Above threshold. Approximation is 255. Error is value-255. */
						error -= 255;
						*d |= dest_mask;
					}
					else
					{
					/* Below threshold. Approximation is 0. Error is value-0. */
					}
					
				/* Distribute the error in the correct directions. */

					Delta = error;
					Delta -= (Alpha = (7*error) >> 4);
					Delta -= (Beta  = (3*error) >> 4);
					Delta -= (Gamma = (5*error) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					cur_error = e[1] + Alpha;				/* for next time */

					e[-1] += Beta;
					e[0] += Gamma;
					e[1] = Delta;

#if 0
					_asm
					{
						ror	dest_mask, 1
						jnc	foo
						inc	word ptr d
					foo:
					}
#else
					if ((dest_mask >>= 1) == 0)
					{
						dest_mask = 0x80;
						d++;
					}
#endif
					e++;

					if (--count == 0)
					{
						break;
					}

				/* Update the smoothing if smoothing is happening. */

					if (smooth_delta != 0)
					{
						smooth_fraction += smooth_delta;
						while (smooth_fraction >= smooth_compare)
						{
							smooth_fraction -= smooth_compare;
							smooth_value += smooth_inc;
						}
					/* Stop smoothing if we hit the final value. */
						if (smooth_value == smooth_final)
						{
							smooth_delta = 0;
						}
					}
				}
			}
		/* Fix this for now in case it's not right. */
			smooth_value = smooth_final;
		}

		if (fraction != 0)
		{
//			printf("L:FF f:%d d:%d c:%d FF\n", fraction, delta, compare);
		}
	}
}

/*
// Process the next row with the error diffusion method (unsmoothed).
*/

void ImageContext::process_errdiff_unsmoothed(int plane, LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width)
{
 	SHORT far *e, far *ep;
	SHORT delta, compare, fraction;
	UCHAR dest_mask;
	SHORT error, byte, count, cur_error;
	SHORT Alpha, Beta, Gamma, Delta;
	LPBYTE s, d;

#if 0
	printf("sf:%d, df:%d, xoff:%d, w:%d, sw:%d\n",
						src_factor, dst_factor, x_offset, width, src_width);
#endif

/* Initialize the source and destination pointers. */

	memset(dp, 0, (width+7)/8);

/* Initialize the DDA variables. */

	delta = dst_factor;
	compare = src_factor;
	fraction = 0;

	dest_mask = 0x80;

	ep = (*error_info)->errors + (plane*(width+2));
	e = ep+1;

	s = sp;
	d = dp;

/* Initialize the pointers for the error buffers. */

	if ((*error_info)->lineno & 1)
	{
		SHORT soff;

		x_offset += width-1;
		e += width-1;

		soff = scale_number(x_offset+1, src_factor, dst_factor);
//		if ((fraction = delta - (SHORT)(((LONG)(x_offset+1)*(LONG)src_factor)%(LONG)dst_factor)) == dst_factor)
		if ((fraction = delta - mulrem(x_offset+1, src_factor, dst_factor)) == dst_factor)
		{
			fraction = 0;
			soff--;
		}

		s = sp+soff;
		if (s >= sp+src_width)
		{
#if 0
			printf("Off the end! src:%d, dst:%d, width:%d, src_width:%d\n",
									src_factor, dst_factor, width, src_width);
#endif
		}

	/* Reset and recalc the destination variables. */

		x_offset = width-1;
		dest_mask = 0x80 >> (x_offset & 7);
		d = dp + (x_offset >> 3);

		cur_error = *e;
		*e = 0;
		for (count = width; count > 0; )
		{
			/* This byte is a keeper. */
			/* Get the current byte and translate it to its correct value. */

			byte = (SHORT)gray_translate_table[(SHORT)*s];

		/* Compute the error corrected value. */

			error = byte + cur_error;

//			printf("[b:%d] *e:%d {%Fp} ", byte, *e, e);

		/* Compute the value we will set. */
		/* Compute the error in this case. */

			if (error > 127)
			{
			/* Above threshold. Approximation is 255. Error is value-255. */
				error -= 255;
				*d |= dest_mask;
			}
			else
			{
			/* Below threshold. Approximation is 0. Error is value-0. */
			}
					
		/* Distribute the error in the correct directions. */

			Delta = error;
			Delta -= (Alpha = (7*error) >> 4);
			Delta -= (Beta  = (3*error) >> 4);
			Delta -= (Gamma = (5*error) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			cur_error = e[-1] + Alpha;				/* for next time */

			e[1] += Beta;
			e[0] += Gamma;
			e[-1] = Delta;

			if ((dest_mask <<= 1) == 0)
			{
				dest_mask = 0x01;
				d--;
			}
			e--;

			fraction += compare;
			while (fraction >= delta)
			{
				fraction -= delta;
				s--;
			}

			if (--count == 0)
			{
				break;
			}
		}

		if (d < dp && dest_mask != 1)
		{
//			printf("R:DD %Fp < %Fp (%02x) DD\n", d, dp, dest_mask);
		}
		if (s < sp-1)
		{
//			printf("R:SS %Fp < %Fp SS\n", s, sp);
		}
		if (e < ep-1)
		{
//			printf("R:EE %Fp < %Fp EE\n", e, ep);
		}
		if (((delta >= compare) && fraction != 0)
				|| ((delta < compare) && fraction != delta))
		{
//			printf("R:FF f:%d d:%d c:%d FF\n", fraction, delta, compare);
		}
	}
	else
	{
		if (x_offset != 0)
		{
			s += scale_number(x_offset+1, compare, delta);
//			if ((fraction = delta - (SHORT)(((LONG)(x_offset+1)*(LONG)compare)%(LONG)delta)) == delta)
			if ((fraction = delta - mulrem(x_offset+1, compare, delta)) == delta)
			{
				fraction = 0;
				s--;
			}
			fraction += compare - delta;
		}

		cur_error = *e;
		*e = 0;

		for (count = width; count > 0; )
		{
			if ((fraction += delta) >= compare)
			{
			/* This byte is a keeper. */
			/* Get the current byte and translate it to its correct value. */

				byte = (SHORT)gray_translate_table[(SHORT)*s];

				while (fraction >= compare)
				{
					fraction -= compare;

				/* Compute the error corrected value. */

					error = byte + cur_error;

//					printf("[b:%d] *e:%d {%Fp} ", byte, *e, e);

				/* Compute the value we will set. */
				/* Compute the error in this case. */

					if (error > 127)
					{
					/* Above threshold. Approximation is 255. Error is value-255. */
						error -= 255;
						*d |= dest_mask;
					}
					else
					{
					/* Below threshold. Approximation is 0. Error is value-0. */
					}

				/* Distribute the error in the correct directions. */

					Delta = error;
					Delta -= (Alpha = (7*error) >> 4);
					Delta -= (Beta  = (3*error) >> 4);
					Delta -= (Gamma = (5*error) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					cur_error = e[1] + Alpha;				/* for next time */

					e[-1] += Beta;
					e[0] += Gamma;
					e[1] = Delta;

#if 0
					_asm
					{
						ror	dest_mask, 1
						jnc	foo
						inc	word ptr d
					foo:
					}
#else
					if ((dest_mask >>= 1) == 0)
					{
						dest_mask = 0x80;
						d++;
					}
#endif
					e++;
					if (--count == 0)
					{
						break;
					}
				}
			}
			s++;
		}

		if (fraction != 0)
		{
//			printf("L:FF f:%d d:%d c:%d FF\n", fraction, delta, compare);
		}
	}
}

/*
// Process the next row with the error diffusion method (unsmoothed).
*/

VOID PalPaletteContext::process_errdiff_unsmoothed(LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width)
{
	SHORT red_error, green_error, blue_error, count;
	SHORT Alpha, Beta, Gamma, Delta;

	BOOL src24 = smoothing || (image_info.image_type == IMAGE_FULL_COLOR);
	BOOL pal8 = raster_info.type == RASTER_TYPE_8;
	SHORT step = src24 ? 3 : 1;
//	SHORT red_index = image_info.red_index;
//	SHORT blue_index = red_index ^ 2;

#if 0
	od("<sf:%d, df:%d, xoff:%d, w:%d, sw:%d",
						src_factor, dst_factor, x_offset, width, src_width);
#endif

/* Initialize the source and destination pointers. */

	memset(dp, 0, (width+7)/8);

/* Initialize the DDA variables. */

	SHORT delta = dst_factor;
	SHORT compare = src_factor;
	SHORT fraction = 0;

	SHORT far *ep = (*error_info)->errors;
	SHORT far *e = ep+3;

	LPBYTE s = sp;
	LPBYTE d = dp;

/* Initialize the pointers for the error buffers. */

	if ((*error_info)->lineno & 1)
	{
		SHORT soff;

		x_offset += width-1;
		e += (width-1)*3;
		d += width-1;

		soff = scale_number(x_offset+1, src_factor, dst_factor)*step;
		if ((fraction = delta - mulrem(x_offset+1, src_factor, dst_factor)) == dst_factor)
		{
			fraction = 0;
			soff -= step;
		}

		s = sp+soff;

		if (s >= sp+src_width)
		{
#if 0
			printf("Off the end! src:%d, dst:%d, width:%d, src_width:%d\n",
									src_factor, dst_factor, width, src_width);
#endif
		}

	/* Reset and recalc the destination variables. */

		red_error = e[0];
		green_error = e[1];
		blue_error = e[2];

		e[0] = e[1] = e[2] = 0;

		for (count = width; count > 0; )
		{
			/* This byte is a keeper. */
			/* Get the current byte and translate it to its correct value. */

			RGBTRIPLE rgb;
			SHORT red, blue, green, new_red, new_blue, new_green;

			if (src24)
			{
				rgb = *(RGBTRIPLE far *)s;
			}
			else
			{
				rgb = palette.m_pData[*s];
			}

		/* Compute the translated, error corrected value. */

#if 0
			red = ((UCHAR *)&rgb)[red_index] + red_error;
			green = ((UCHAR *)&rgb)[1] + green_error;
			blue = ((UCHAR *)&rgb)[blue_index] + blue_error;
#else
			red = ((UCHAR *)&rgb)[0] + red_error;
			green = ((UCHAR *)&rgb)[1] + green_error;
			blue = ((UCHAR *)&rgb)[2] + blue_error;
#endif

//			printf("[b:%d] *e:%d {%Fp} ", byte, *e, e);

		/* Compute the value we will set. */

			if (pal8)
			{
			/* Bits are two-state. */
				new_red = red > 127 ? 255 : 0;
				new_green = green > 127 ? 255 : 0;
				new_blue = blue > 127 ? 255 : 0;
			}
			else
			{
#if 0
				new_red = red + 32;
				if (new_red <= 0) new_red = 0;
				else if (new_red > 255)	new_red = 255;
				else new_red &= 0x00c0;

				new_green = green + 32;
				if (new_green <= 0) new_green = 0;
				else if (new_green > 255) new_green = 255;
				else new_green &= 0x00c0;

				new_blue = blue + 32;
				if (new_blue <= 0) new_blue = 0;
				else if (new_blue > 255) new_blue = 255;
				else new_blue &= 0x00c0;
#else
				new_red = red /*+ 25*/;
				if (new_red <= 0) new_red = 0;
				else if (new_red > 255)	new_red = 255;
				else new_red -= aModulo51[new_red];

				new_green = green /*+ 25*/;
				if (new_green <= 0) new_green = 0;
				else if (new_green > 255) new_green = 255;
				else new_green -= aModulo51[new_green];

				new_blue = blue /*+ 25*/;
				if (new_blue <= 0) new_blue = 0;
				else if (new_blue > 255) new_blue = 255;
				else new_blue -= aModulo51[new_blue];
#endif
			}

		/* Compute the new error. */

			red_error = red - new_red;
			green_error = green - new_green;
			blue_error = blue - new_blue;

		/* Store the destination value. */

			if (pal8)
			{
				BYTE byte = 0;

				if (new_red) byte |= 4;
				if (new_green) byte |= 2;
				if (new_blue) byte |= 1;

				*d-- = byte;
			}
			else
			{
				BYTE bIndex = aTimes36[aDividedBy51Rounded[new_blue]]
								+ aTimes6[aDividedBy51Rounded[new_green]]
								+ aDividedBy51Rounded[new_red];
				*d-- = aWinGHalftoneTranslation[bIndex];
//				*d-- = ((new_red+1)>>6)*25 + ((new_green+1)>>6)*5 + ((new_blue+1)>>6);
			}

		/* Distribute the error in the correct directions. */

			Delta = red_error;
			Delta -= (Alpha = (7*red_error) >> 4);
			Delta -= (Beta  = (3*red_error) >> 4);
			Delta -= (Gamma = (5*red_error) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			red_error = e[0-1*3] + Alpha;				/* for next time */
			e[0+1*3] += Beta;
			e[0+0*3] += Gamma;
			e[0-1*3] = Delta;

			Delta = green_error;
			Delta -= (Alpha = (7*green_error) >> 4);
			Delta -= (Beta  = (3*green_error) >> 4);
			Delta -= (Gamma = (5*green_error) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			green_error = e[1-1*3] + Alpha;				/* for next time */
			e[1+1*3] += Beta;
			e[1+0*3] += Gamma;
			e[1-1*3] = Delta;

			Delta = blue_error;
			Delta -= (Alpha = (7*blue_error) >> 4);
			Delta -= (Beta  = (3*blue_error) >> 4);
			Delta -= (Gamma = (5*blue_error) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			blue_error = e[2-1*3] + Alpha;				/* for next time */
			e[2+1*3] += Beta;
			e[2+0*3] += Gamma;
			e[2-1*3] = Delta;

			e -= 3;

			fraction += compare;
			while (fraction >= delta)
			{
				fraction -= delta;
				s -= step;
			}

			if (--count == 0)
			{
				break;
			}
		}
	}
	else
	{
		if (x_offset != 0)
		{
			s += scale_number(x_offset+1, compare, delta)*step;
			if ((fraction = delta - mulrem(x_offset+1, compare, delta)) == delta)
			{
				fraction = 0;
				s -= step;
			}
			fraction += compare - delta;
		}

		red_error = e[0];
		green_error = e[1];
		blue_error = e[2];

		e[0] = e[1] = e[2] = 0;

		for (count = width; count > 0; )
		{
			if ((fraction += delta) >= compare)
			{
			/* This byte is a keeper. */
			/* Get the current byte and translate it to its correct value. */

				RGBTRIPLE rgb;
				SHORT red, blue, green, new_red, new_blue, new_green;

				if (src24)
				{
					rgb = *(RGBTRIPLE far *)s;
				}
				else
				{
					rgb = palette.m_pData[*s];
				}

			/* Compute the translated, error corrected value. */

				while (fraction >= compare)
				{
					fraction -= compare;

#if 0
					red = ((UCHAR *)&rgb)[red_index] + red_error;
					green = ((UCHAR *)&rgb)[1] + green_error;
					blue = ((UCHAR *)&rgb)[blue_index] + blue_error;
#else
					red = ((UCHAR *)&rgb)[0] + red_error;
					green = ((UCHAR *)&rgb)[1] + green_error;
					blue = ((UCHAR *)&rgb)[2] + blue_error;
#endif

//					printf("[b:%d] *e:%d {%Fp} ", byte, *e, e);

				/* Compute the value we will set. */

					if (pal8)
					{
					/* Bits are two-state. */
						new_red = red > 127 ? 255 : 0;
						new_green = green > 127 ? 255 : 0;
						new_blue = blue > 127 ? 255 : 0;
					}
					else
					{
#if 0
						new_red = red + 32;
						if (new_red <= 0) new_red = 0;
						else if (new_red > 255)	new_red = 255;
						else new_red &= 0x00c0;

						new_green = green + 32;
						if (new_green <= 0) new_green = 0;
						else if (new_green > 255) new_green = 255;
						else new_green &= 0x00c0;

						new_blue = blue + 32;
						if (new_blue <= 0) new_blue = 0;
						else if (new_blue > 255) new_blue = 255;
						else new_blue &= 0x00c0;
#else
						new_red = red /*+ 25*/;
						if (new_red <= 0) new_red = 0;
						else if (new_red > 255)	new_red = 255;
						else new_red -= aModulo51[new_red];

						new_green = green /*+ 25*/;
						if (new_green <= 0) new_green = 0;
						else if (new_green > 255) new_green = 255;
						else new_green -= aModulo51[new_green];

						new_blue = blue /*+ 25*/;
						if (new_blue <= 0) new_blue = 0;
						else if (new_blue > 255) new_blue = 255;
						else new_blue -= aModulo51[new_blue];
#endif
					}

				/* Compute the new error. */

					red_error = red - new_red;
					green_error = green - new_green;
					blue_error = blue - new_blue;

				/* Store the destination value. */

					if (pal8)
					{
						BYTE byte = 0;

						if (new_red) byte |= 4;
						if (new_green) byte |= 2;
						if (new_blue) byte |= 1;

						*d++ = byte;
					}
					else
					{
						BYTE bIndex = aTimes36[aDividedBy51Rounded[new_blue]]
										+ aTimes6[aDividedBy51Rounded[new_green]]
										+ aDividedBy51Rounded[new_red];
						*d++ = aWinGHalftoneTranslation[bIndex];
//						*d++ = ((new_red+1)>>6)*25 + ((new_green+1)>>6)*5 + ((new_blue+1)>>6);
					}

				/* Distribute the error in the correct directions. */

					Delta = red_error;
					Delta -= (Alpha = (7*red_error) >> 4);
					Delta -= (Beta  = (3*red_error) >> 4);
					Delta -= (Gamma = (5*red_error) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					red_error = e[0+1*3] + Alpha;		/* for next time */
					e[0-1*3] += Beta;
					e[0+0*3] += Gamma;
					e[0+1*3] = Delta;

					Delta = green_error;
					Delta -= (Alpha = (7*green_error) >> 4);
					Delta -= (Beta  = (3*green_error) >> 4);
					Delta -= (Gamma = (5*green_error) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					green_error = e[1+1*3] + Alpha;				/* for next time */
					e[1-1*3] += Beta;
					e[1+0*3] += Gamma;
					e[1+1*3] = Delta;

					Delta = blue_error;
					Delta -= (Alpha = (7*blue_error) >> 4);
					Delta -= (Beta  = (3*blue_error) >> 4);
					Delta -= (Gamma = (5*blue_error) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					blue_error = e[2+1*3] + Alpha;				/* for next time */
					e[2-1*3] += Beta;
					e[2+0*3] += Gamma;
					e[2+1*3] = Delta;

					e += 3;

					if (--count == 0)
					{
						break;
					}
				}
			}
			s += step;
		}
	}
}

/*
// Process the next row with the error diffusion method (smoothed).
// Note that the source coming in here is always 24-bit.
*/

VOID PalPaletteContext::process_errdiff_smoothed(LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
							int x_offset, int width,
							int src_width)
{
	SHORT red_error, green_error, blue_error, count;
	SHORT Alpha, Beta, Gamma, Delta;

	BOOL pal8 = raster_info.type == RASTER_TYPE_8;
//	SHORT red_index = image_info.red_index;
//	SHORT blue_index = red_index ^ 2;

	SHORT smooth_inc[3], smooth_final[3], smooth_value[3];
	SHORT smooth_fraction[3], smooth_compare, smooth_delta[3];

#if 0
	od("<sf:%d, df:%d, xoff:%d, w:%d, sw:%d",
						src_factor, dst_factor, x_offset, width, src_width);
#endif

/* Initialize the source and destination pointers. */

	memset(dp, 0, (width+7)/8);

/* Initialize the DDA variables. */

	SHORT delta = dst_factor;
	SHORT compare = src_factor;
	SHORT fraction = 0;

	SHORT far *ep = (*error_info)->errors;
	SHORT far *e = ep+3;

	LPBYTE s = sp;
	LPBYTE d = dp;

/* Initialize the pointers for the error buffers. */

	if ((*error_info)->lineno & 1)
	{
		SHORT soff;

		x_offset += width-1;
		e += (width-1)*3;
		d += width-1;

		soff = scale_number(x_offset+1, src_factor, dst_factor)*3;
		if ((fraction = delta - mulrem(x_offset+1, src_factor, dst_factor)) == dst_factor)
		{
			fraction = 0;
			soff -= 3;
		}

		s = sp+soff;

		if (soff >= src_width)
		{
#if 0
			printf("Off the end! src:%d, dst:%d, width:%d, src_width:%d\n",
									src_factor, dst_factor, width, src_width);
#endif
		}

	/* Reset and recalc the destination variables. */

		red_error = e[0];
		green_error = e[1];
		blue_error = e[2];

		e[0] = e[1] = e[2] = 0;

#if 0
		smooth_value[0] = s[red_index];
		smooth_value[1] = s[1];
		smooth_value[2] = s[blue_index];
#else
		smooth_value[0] = s[0];
		smooth_value[1] = s[1];
		smooth_value[2] = s[2];
#endif
		if ((soff -= 3) >= 0)
		{
			s -= 3;
		}
		smooth_compare = smooth_pixel_width;

	/* Grab source byte and advance to the previous source byte. */

#if 0
		smooth_final[0] = s[red_index];
		smooth_final[1] = s[1];
		smooth_final[2] = s[blue_index];
#else
		smooth_final[0] = s[0];
		smooth_final[1] = s[1];
		smooth_final[2] = s[2];
#endif
		if ((soff -= 3) >= 0)
		{
			s -= 3;
		}

		smooth_inc[0] =
			smooth_inc[1] =
			smooth_inc[2] = 1;

		smooth_fraction[0] =
			smooth_fraction[1] =
			smooth_fraction[2] = 0;

		if ((smooth_delta[0] = smooth_final[0] - smooth_value[0]) < 0)
		{
			smooth_delta[0] = -smooth_delta[0];
			smooth_inc[0] = -1;
		}
		if ((smooth_delta[1] = smooth_final[1] - smooth_value[1]) < 0)
		{
			smooth_delta[1] = -smooth_delta[1];
			smooth_inc[1] = -1;
		}
		if ((smooth_delta[2] = smooth_final[2] - smooth_value[2]) < 0)
		{
			smooth_delta[2] = -smooth_delta[2];
			smooth_inc[2] = -1;
		}

		for (count = width; count > 0; )
		{
			/* This byte is a keeper. */
			/* Get the current byte and translate it to its correct value. */

			SHORT red, blue, green, new_red, new_blue, new_green;

		/* Compute the translated, error corrected value. */

			red = smooth_value[0] + red_error;
			green = smooth_value[1] + green_error;
			blue = smooth_value[2] + blue_error;

//			printf("[b:%d] *e:%d {%Fp} ", byte, *e, e);

		/* Compute the value we will set. */

			if (pal8)
			{
			/* Bits are two-state. */
				new_red = red > 127 ? 255 : 0;
				new_green = green > 127 ? 255 : 0;
				new_blue = blue > 127 ? 255 : 0;
			}
			else
			{
				new_red = red /*+ 25*/;
				if (new_red <= 0) new_red = 0;
				else if (new_red > 255)	new_red = 255;
				else new_red -= aModulo51[new_red];

				new_green = green /*+ 25*/;
				if (new_green <= 0) new_green = 0;
				else if (new_green > 255) new_green = 255;
				else new_green -= aModulo51[new_green];

				new_blue = blue /*+ 25*/;
				if (new_blue <= 0) new_blue = 0;
				else if (new_blue > 255) new_blue = 255;
				else new_blue -= aModulo51[new_blue];
			}

		/* Compute the new error. */

			red_error = red - new_red;
			green_error = green - new_green;
			blue_error = blue - new_blue;

		/* Store the destination value. */

			if (pal8)
			{
				BYTE byte = 0;

				if (new_red) byte |= 4;
				if (new_green) byte |= 2;
				if (new_blue) byte |= 1;

				*d-- = byte;
			}
			else
			{
				BYTE bIndex = aTimes36[aDividedBy51Rounded[new_blue]]
								+ aTimes6[aDividedBy51Rounded[new_green]]
								+ aDividedBy51Rounded[new_red];
				*d-- = aWinGHalftoneTranslation[bIndex];

//				*d-- = ((new_red+1)>>6)*25 + ((new_green+1)>>6)*5 + ((new_blue+1)>>6);
			}

		/* Distribute the error in the correct directions. */

			Delta = red_error;
			Delta -= (Alpha = (7*red_error) >> 4);
			Delta -= (Beta  = (3*red_error) >> 4);
			Delta -= (Gamma = (5*red_error) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			red_error = e[0-1*3] + Alpha;				/* for next time */
			e[0+1*3] += Beta;
			e[0+0*3] += Gamma;
			e[0-1*3] = Delta;

			Delta = green_error;
			Delta -= (Alpha = (7*green_error) >> 4);
			Delta -= (Beta  = (3*green_error) >> 4);
			Delta -= (Gamma = (5*green_error) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			green_error = e[1-1*3] + Alpha;				/* for next time */
			e[1+1*3] += Beta;
			e[1+0*3] += Gamma;
			e[1-1*3] = Delta;

			Delta = blue_error;
			Delta -= (Alpha = (7*blue_error) >> 4);
			Delta -= (Beta  = (3*blue_error) >> 4);
			Delta -= (Gamma = (5*blue_error) >> 4);

		/*
		// e[1] += alpha
		// ne[-1] += beta;
		// ne[0] += gamma;
		// ne[1] += delta;
		*/

			blue_error = e[2-1*3] + Alpha;				/* for next time */
			e[2+1*3] += Beta;
			e[2+0*3] += Gamma;
			e[2-1*3] = Delta;

			e -= 3;

			if (--count == 0)
			{
				break;
			}

			fraction += compare;

			if (fraction >= delta)
			{
				while (fraction >= delta)
				{
					fraction -= delta;
				/* Fix this for now in case it's not right. */

					smooth_value[0] = smooth_final[0];
					smooth_value[1] = smooth_final[1];
					smooth_value[2] = smooth_final[2];

				/* Grab source byte and advance to the previous source byte. */

#if 0
					smooth_final[0] = (SHORT)s[red_index];
					smooth_final[1] = (SHORT)s[1];
					smooth_final[2] = (SHORT)s[blue_index];
#else
					smooth_final[0] = (SHORT)s[0];
					smooth_final[1] = (SHORT)s[1];
					smooth_final[2] = (SHORT)s[2];
#endif
					if ((soff -= 3) >= 0)
					{
						s -= 3;
					}

					smooth_inc[0] =
						smooth_inc[1] =
						smooth_inc[2] = 1;

					smooth_fraction[0] =
						smooth_fraction[1] =
						smooth_fraction[2] = 0;

					if ((smooth_delta[0] = smooth_final[0] - smooth_value[0]) < 0)
					{
						smooth_delta[0] = -smooth_delta[0];
						smooth_inc[0] = -1;
					}
					if ((smooth_delta[1] = smooth_final[1] - smooth_value[1]) < 0)
					{
						smooth_delta[1] = -smooth_delta[1];
						smooth_inc[1] = -1;
					}
					if ((smooth_delta[2] = smooth_final[2] - smooth_value[2]) < 0)
					{
						smooth_delta[2] = -smooth_delta[2];
						smooth_inc[2] = -1;
					}
				}
			}
			else
			{
			/* Update the smoothing if smoothing is happening. */

				/* Red */
				if (smooth_delta[0] != 0)
				{
					smooth_fraction[0] += smooth_delta[0];
					while (smooth_fraction[0] >= smooth_compare)
					{
						smooth_fraction[0] -= smooth_compare;
						smooth_value[0] += smooth_inc[0];
					}
				/* Stop smoothing if we hit the final value. */
					if (smooth_value[0] == smooth_final[0])
					{
						smooth_delta[0] = 0;
					}
				}
				/* Green */
				if (smooth_delta[1] != 0)
				{
					smooth_fraction[1] += smooth_delta[1];
					while (smooth_fraction[1] >= smooth_compare)
					{
						smooth_fraction[1] -= smooth_compare;
						smooth_value[1] += smooth_inc[1];
					}
				/* Stop smoothing if we hit the final value. */
					if (smooth_value[1] == smooth_final[1])
					{
						smooth_delta[1] = 0;
					}
				}
				/* Blue */
				if (smooth_delta[2] != 0)
				{
					smooth_fraction[2] += smooth_delta[2];
					while (smooth_fraction[2] >= smooth_compare)
					{
						smooth_fraction[2] -= smooth_compare;
						smooth_value[2] += smooth_inc[2];
					}
				/* Stop smoothing if we hit the final value. */
					if (smooth_value[2] == smooth_final[2])
					{
						smooth_delta[2] = 0;
					}
				}
			}
		}
	}
	else
	{
		SHORT soff = 0;
		if (x_offset != 0)
		{
			soff = scale_number(x_offset+1, compare, delta)*3;
			if ((fraction = delta - mulrem(x_offset+1, compare, delta)) == delta)
			{
				fraction = 0;
				soff -= 3;
			}
			s += soff;
			fraction += compare - delta;
		}

		red_error = e[0];
		green_error = e[1];
		blue_error = e[2];

		e[0] = e[1] = e[2] = 0;

		if (soff != 0)
		{
			s -= 3;
		}
#if 0
		smooth_value[0] = s[red_index];
		smooth_value[1] = s[1];
		smooth_value[2] = s[blue_index];
#else
		smooth_value[0] = s[0];
		smooth_value[1] = s[1];
		smooth_value[2] = s[2];
#endif
		if (soff != 0)
		{
			s += 3;
		}

		smooth_compare = smooth_pixel_width;		/* Same for all channels. */

		for (count = width; count > 0; )
		{
#if 0
			smooth_final[0] = s[red_index];
			smooth_final[1] = s[1];
			smooth_final[2] = s[blue_index];
#else
			smooth_final[0] = s[0];
			smooth_final[1] = s[1];
			smooth_final[2] = s[2];
#endif
			s += 3;
			smooth_fraction[0] =
				smooth_fraction[1] =
				smooth_fraction[2] = 0;
			smooth_inc[0] =
				smooth_inc[1] =
				smooth_inc[2] = 1;

			if ((smooth_delta[0] = smooth_final[0] - smooth_value[0]) < 0)
			{
				smooth_delta[0] = -smooth_delta[0];
				smooth_inc[0] = -1;
			}
			if ((smooth_delta[1] = smooth_final[1] - smooth_value[1]) < 0)
			{
				smooth_delta[1] = -smooth_delta[1];
				smooth_inc[1] = -1;
			}
			if ((smooth_delta[2] = smooth_final[2] - smooth_value[2]) < 0)
			{
				smooth_delta[2] = -smooth_delta[2];
				smooth_inc[2] = -1;
			}

			if ((fraction += delta) >= compare)
			{
			/* This byte is a keeper. */

				while (fraction >= compare)
				{
					fraction -= compare;

					SHORT new_red, new_green, new_blue;

					SHORT red = smooth_value[0] + red_error;
					SHORT green = smooth_value[1] + green_error;
					SHORT blue = smooth_value[2] + blue_error;

//					printf("[b:%d] *e:%d {%Fp} ", byte, *e, e);

				/* Compute the value we will set. */

					if (pal8)
					{
					/* Bits are two-state. */
						new_red = red > 127 ? 255 : 0;
						new_green = green > 127 ? 255 : 0;
						new_blue = blue > 127 ? 255 : 0;
					}
					else
					{
						new_red = red /*+ 25*/;
						if (new_red <= 0) new_red = 0;
						else if (new_red > 255)	new_red = 255;
						else new_red -= aModulo51[new_red];

						new_green = green /*+ 25*/;
						if (new_green <= 0) new_green = 0;
						else if (new_green > 255) new_green = 255;
						else new_green -= aModulo51[new_green];

						new_blue = blue /*+ 25*/;
						if (new_blue <= 0) new_blue = 0;
						else if (new_blue > 255) new_blue = 255;
						else new_blue -= aModulo51[new_blue];
					}

				/* Compute the new error. */

					red_error = red - new_red;
					green_error = green - new_green;
					blue_error = blue - new_blue;

				/* Store the destination value. */

					if (pal8)
					{
						BYTE byte = 0;

						if (new_red) byte |= 4;
						if (new_green) byte |= 2;
						if (new_blue) byte |= 1;

						*d++ = byte;
					}
					else
					{
						BYTE bIndex = aTimes36[aDividedBy51Rounded[new_blue]]
										+ aTimes6[aDividedBy51Rounded[new_green]]
										+ aDividedBy51Rounded[new_red];
						*d++ = aWinGHalftoneTranslation[bIndex];
//						*d++ = ((new_red+1)>>6)*25 + ((new_green+1)>>6)*5 + ((new_blue+1)>>6);
					}

				/* Distribute the error in the correct directions. */

					Delta = red_error;
					Delta -= (Alpha = (7*red_error) >> 4);
					Delta -= (Beta  = (3*red_error) >> 4);
					Delta -= (Gamma = (5*red_error) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					red_error = e[0+1*3] + Alpha;		/* for next time */
					e[0-1*3] += Beta;
					e[0+0*3] += Gamma;
					e[0+1*3] = Delta;

					Delta = green_error;
					Delta -= (Alpha = (7*green_error) >> 4);
					Delta -= (Beta  = (3*green_error) >> 4);
					Delta -= (Gamma = (5*green_error) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					green_error = e[1+1*3] + Alpha;				/* for next time */
					e[1-1*3] += Beta;
					e[1+0*3] += Gamma;
					e[1+1*3] = Delta;

					Delta = blue_error;
					Delta -= (Alpha = (7*blue_error) >> 4);
					Delta -= (Beta  = (3*blue_error) >> 4);
					Delta -= (Gamma = (5*blue_error) >> 4);

				/*
				// e[1] += alpha
				// ne[-1] += beta;
				// ne[0] += gamma;
				// ne[1] += delta;
				*/

					blue_error = e[2+1*3] + Alpha;				/* for next time */
					e[2-1*3] += Beta;
					e[2+0*3] += Gamma;
					e[2+1*3] = Delta;

					e += 3;

				/* Update the smoothing if smoothing is happening. */

					/* Red */
					if (smooth_delta[0] != 0)
					{
						smooth_fraction[0] += smooth_delta[0];
						while (smooth_fraction[0] >= smooth_compare)
						{
							smooth_fraction[0] -= smooth_compare;
							smooth_value[0] += smooth_inc[0];
						}
					/* Stop smoothing if we hit the final value. */
						if (smooth_value[0] == smooth_final[0])
						{
							smooth_delta[0] = 0;
						}
					}

					/* Green */
					if (smooth_delta[1] != 0)
					{
						smooth_fraction[1] += smooth_delta[1];
						while (smooth_fraction[1] >= smooth_compare)
						{
							smooth_fraction[1] -= smooth_compare;
							smooth_value[1] += smooth_inc[1];
						}
					/* Stop smoothing if we hit the final value. */
						if (smooth_value[1] == smooth_final[1])
						{
							smooth_delta[1] = 0;
						}
					}
					/* Blue */
					if (smooth_delta[2] != 0)
					{
						smooth_fraction[2] += smooth_delta[2];
						while (smooth_fraction[2] >= smooth_compare)
						{
							smooth_fraction[2] -= smooth_compare;
							smooth_value[2] += smooth_inc[2];
						}
					/* Stop smoothing if we hit the final value. */
						if (smooth_value[2] == smooth_final[2])
						{
							smooth_delta[2] = 0;
						}
					}

					if (--count == 0)
					{
						break;
					}
				}
			}

		/* Fix this for now in case it's not right. */

			smooth_value[0] = smooth_final[0];
			smooth_value[1] = smooth_final[1];
			smooth_value[2] = smooth_final[2];
		}
	}
}

