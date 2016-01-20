#include "stdafx.h"
#include "Gif.h"

#define GIF_COLORMAP 0x80

typedef unsigned char gif_cmap_t[256][4];
typedef short gif_table_t[4096];

static int gif_eof = 0;
static int gif_read_cmap(FILE* fp, int nColors, gif_cmap_t Colormap, int* bGray);
static int gif_get_block(FILE* fp, unsigned char* buffer);
static int gif_get_code (FILE* fp, int code_size, int first_time);

/////////////////////////////////////////////////////////////////////////////
COLORREF GetGifTransparentColor(const char* pFileName)
{
	COLORREF clrTransparent = CLR_NONE;

	// Read the GIF header
	FILE* fp = fopen(pFileName, "r");
	if (!fp)
		return clrTransparent;

	unsigned char buf[1024];
	fread(buf, 13, 1, fp);

	int xsize = (buf[7] << 8) | buf[6];
	int ysize = (buf[9] << 8) | buf[8];
	int nColors = 2 << (buf[10] & 0x07);

	gif_cmap_t Colormap;
	if (buf[10] & GIF_COLORMAP)
	{
		int bGray = false;
		if (gif_read_cmap(fp, nColors, Colormap, &bGray))
		{
			fclose(fp);
			return clrTransparent;
		}
	}

	int iTransparent = -1;

	while (1)
	{
		switch (getc(fp))
		{
			case ';' :	// End of image
			{
				fclose(fp);
				return clrTransparent; // Early end of file
			}

			case '!' :	// Extension record
			{
				buf[0] = getc(fp);
				if (buf[0] == 0xf9)	// Graphic Control Extension
				{
					gif_get_block(fp, buf);
					if (buf[0] & 1)	// Get transparent color index
						iTransparent = buf[3];
				}

				while (gif_get_block(fp, buf) != 0)
					;
				break;
			}

			case ',' :	// Image data
			{
				fread(buf, 9, 1, fp);

				if (buf[8] & GIF_COLORMAP)
				{
					nColors = 2 << (buf[8] & 0x07);
					int bGray = false;
					if (gif_read_cmap(fp, nColors, Colormap, &bGray))
					{
						fclose(fp);
						return clrTransparent;
					}
				}

				if (iTransparent >= 0)
				{
					BYTE r = Colormap[iTransparent][0];
					BYTE g = Colormap[iTransparent][1];
					BYTE b = Colormap[iTransparent][2];
					clrTransparent = RGB(r, g, b);
				}

				xsize = (buf[5] << 8) | buf[4];
				ysize = (buf[7] << 8) | buf[6];

				// Check the dimensions of the image; since the dimensions are
				// a 16-bit integer we just need to check for 0...
				if (xsize == 0 || ysize == 0)
				{
					// ERROR: Bad GIF image dimensions
					fclose(fp);
					return clrTransparent;
				}

				fclose(fp);
				return clrTransparent;
			}
		}
	}

	return clrTransparent;
}

/////////////////////////////////////////////////////////////////////////////
static int gif_read_cmap(FILE* fp, int nColors, gif_cmap_t Colormap, int* bGray)
{
	// Read the colormap...
	int	i;
	for (i = 0; i < nColors; i ++)
		if (fread(Colormap[i], 3, 1, fp) < 1)
			return -1;

	// Check to see if the colormap is a grayscale ramp...
	for (i = 0; i < nColors; i ++)
		if (Colormap[i][0] != Colormap[i][1] || Colormap[i][1] != Colormap[i][2])
			break;

	if (i == nColors)
	{
		*bGray = 1;
		return 0;
	}

	// If this needs to be a grayscale image, convert the RGB values to luminance values...
	if (*bGray)
	{
		for (i = 0; i < nColors; i ++)
			Colormap[i][0] = (Colormap[i][0] * 31 + Colormap[i][1] * 61 + Colormap[i][2] * 8) / 100;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
static int gif_get_block(FILE* fp, unsigned char* buf)
{
	// Read the count byte followed by the data from the file...
	int	count;
	if ((count = getc(fp)) == EOF)
	{
		gif_eof = 1;
		return -1;
	}
	else
	if (count == 0)
		gif_eof = 1;
	else
	if ((int)fread(buf, 1, count, fp) < count)
	{
		gif_eof = 1;
		return -1;
	}
	else
		gif_eof = 0;

	return count;
}

/////////////////////////////////////////////////////////////////////////////
static int gif_get_code(FILE* fp, int code_size, int first_time)
{
	static unsigned char buf[280];
	static unsigned
		curbit,		// Current bit
		lastbit,	// Last bit in buffer
		done,		// Done with this buffer?
		last_byte;	// Last byte in buffer
	static const unsigned char bits[8] =	// Bit masks for codes
	{
		0x01, 0x02, 0x04, 0x08,
		0x10, 0x20, 0x40, 0x80
	};

	if (first_time)
	{
		// Just initialize the input buffer...
		curbit    = 0;
		lastbit   = 0;
		last_byte = 0;
		done      = 0;

		return 0;
	}

	if ((curbit + code_size) >= lastbit)
	{
		// Don't have enough bits to hold the code...
		if (done)
			return -1;	// Sorry, no more...

		// Move last two bytes to front of buffer...
		if (last_byte > 1)
		{
			buf[0]    = buf[last_byte - 2];
			buf[1]    = buf[last_byte - 1];
			last_byte = 2;
		}
		else
		if (last_byte == 1)
		{
			buf[0]    = buf[last_byte - 1];
			last_byte = 1;
		}

		// Read in another buffer...
		int count;
		if ((count = gif_get_block (fp, buf + last_byte)) <= 0)
		{
			// Whoops, no more data!
			done = 1;
			return -1;
		}

		// Update buffer state...
		curbit = (curbit - lastbit) + 8 * last_byte;
		last_byte += count;
		lastbit = last_byte * 8;
	}

	int ret = 0;
	int i = curbit + code_size - 1;
	for (int j = code_size; j > 0; j--)
	{
		ret = (ret << 1) | ((buf[i / 8] & bits[i & 7]) != 0);
		i--;
	}

	curbit += code_size;
	return ret;
}
