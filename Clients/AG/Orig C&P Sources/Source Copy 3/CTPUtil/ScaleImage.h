#ifndef _ScaleImage_
#define _ScaleImage_

typedef struct
{
	int xsize;	/* horizontal size of the image in Pixels */
	int ysize;	/* vertical size of the image in Pixels */
	BYTE* data;	/* pointer to first scanline of image */
	int span;	/* byte offset between two scanlines */
} Image;

void ScaleImage(Image* dst, Image* src);

#endif _ScaleImage_