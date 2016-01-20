/******************* ( Animation Construction Kit 3D ) ******************/
/*			Deluxe Paint file reader			     															*/
/* CopyRight (c) 1993  	Authors: Jaimi McEntire, Lary Myers		     			*/
/* CopyRight (c) 1994/5 John Lundy		     															*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/
//
// This function will return a pointer to a buffer that holds the raw image.
// just free the pointer to delete this buffer. After returning, the array
// colordat will hold the adjusted palette of this pic.
//
// Also, this has been modified to only read in form PBM brushes. form ILBM's
// (the "old" type) are not supported. use the "new" deluxe paint .lbm type
// and do not choose "old".

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"
#include "iff.h"


unsigned char far  colordat[768];		/* maximum it can be...256 colors	*/

unsigned char far  cplanes[8][240];	/* setting max at 1920 pixels width	*/
																		/* thats 8 pixels per byte per plane */
unsigned char far  *pplanes=(unsigned char far *)&cplanes[0][0];	/* for a form pbm	    */

HGLOBAL AckReadiff(char *picname)
{
FILE			*pic;
form_chunk		fchunk;
ChunkHeader		chunk;
BitMapHeader	bmhd;
char			value;	   // must remain signed, no matter what. ignore any warnings.
int				sofar;
int				height;
unsigned char far *destx, *savedestx;
HGLOBAL			hdestx;

	if ((pic = fopen(picname,"r+b"))== NULL)
	{
		ErrorCode = ERR_BADPICNAME;
		return(0L);
	}

	fread(&fchunk,1,sizeof(form_chunk),pic); /* read in the first 12 bytes*/

	if (fchunk.type != FORM)
	{
    	fclose(pic);
    	ErrorCode = ERR_INVALIDFORM;
    	return(0L);
    }

	if (fchunk.subtype != ID_PBM)
	{
      	fclose(pic);
      	ErrorCode = ERR_NOPBM;
      	return(0L);
	}
	// now lets loop...Because the Chunks can be in any order!
	while(1)
    {
		fread(&chunk,1,sizeof(ChunkHeader),pic);
		ByteFlipLong((long *)&chunk.ckSize);
		if (chunk.ckSize & 1) chunk.ckSize ++;	// must be word aligned
		if(chunk.ckID == ID_BMHD)
		{
			fread(&bmhd,1,sizeof(BitMapHeader),pic);
			bmhd.w=swab(bmhd.w);			// the only things we need.
			bmhd.h=swab(bmhd.h);
			hdestx = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, (long)(((long)bmhd.w * (long)bmhd.h) + 4L));
			if (hdestx == NULL)
			{
		    	fclose(pic);
		    	ErrorCode = ERR_NOMEMORY;
				return(0L);
			}
			destx = (unsigned char far *)GlobalLock(hdestx);
			//	memset(destx, 0, ((long)bmhd.w * (long)bmhd.h) + 4L);
			savedestx = destx;

			destx[0] = (char)bmhd.w%256;
			destx[1] = (char)bmhd.w/256;
			destx[2] = (char)bmhd.h%256;
			destx[3] = (char)bmhd.h/256;
			destx += 4;
			continue;
		}
    	if(chunk.ckID == ID_CMAP)
		{
		int i;
			unsigned char r,g;

			fread(colordat,1,(size_t)chunk.ckSize,pic);
			for (i=0;i<768;i++)
	    	{
				r = colordat[i];	  // r,g do not stand for red and green
				g = r >> 2;
				colordat[i] = g;
	    	}
			continue;
		}
    	if(chunk.ckID == ID_BODY)
		{
			for(height = 0; height<(int)bmhd.h; height ++)
			{
	    	unsigned char *dest;
	    		dest = (unsigned char *)&(pplanes[0]); /* point at first char  */
	    		sofar = bmhd.w;			    /* number of bytes = 8  */
	    		if (sofar&1) sofar++;
	    		while (sofar)
				{
					if (bmhd.compression)
					{
						value=fgetc(pic);	  /* get the next byte	  */
						// if (value == 128) continue; /* NOP..just get another*/
						if (value > 0)
						{
						int len;
		    				len = value +1;
		    				sofar -= len;
		    				if(!(fread(dest,len,1,pic)))
							{
								fclose(pic);
								ErrorCode = ERR_BADPICFILE;
								GlobalUnlock(hdestx);
								GlobalFree(hdestx);
								return(0L);
							}
			    			dest +=len;
		       			}
		 				else
		    			{
		    			int count;
							count = -value; /* get amount to dup */
		    				count ++;
		    				sofar -= count;
		    				value=fgetc(pic);
		    				while (--count >= 0) *dest++ = value;
		    			}
					}
					else
					{
						fread(dest,sofar,1,pic); /* just throw on plane */
			   			sofar = 0;
					}
				}
	    		if (sofar < 0)
				{
					fclose(pic);
				}
	    		memmove(destx,pplanes,bmhd.w);
	    		destx += bmhd.w;
			}
			break; /* leave if we've unpacked the BODY*/
		}
    	fseek(pic,chunk.ckSize,SEEK_CUR);
    }

	fclose(pic);
	GlobalUnlock(hdestx);
	return(hdestx);
}

void ByteFlipLong(long *NUMBER)
   {
   // Hey, I didn;t write this function!!!
   long int Y, T;
   int I;

   T = *NUMBER;
   Y=0;for (I=0;I<4;I++){Y = Y | (T & 0xFF);if (I<3) {Y = Y << 8;T = T >> 8;}}
   *NUMBER = Y;
   }

int swab(unsigned short number)
   {
   unsigned int xx1,xx2;
   unsigned int result;

   xx1 = number <<8; xx2 = number >>8; result = xx1|xx2;
   return(result);
   }
