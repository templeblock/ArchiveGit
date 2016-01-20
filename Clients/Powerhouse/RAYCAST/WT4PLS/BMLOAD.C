
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wt.h"
#include "error.h"
#include "texture.h"

int   log2( int );

int read_bitmap (FBM *image, char *rfname);

Texture *LoadFile(char *fname)
{
  Texture           *texture;

  FBM image;
  int colors;
  image.bm=NULL;
  image.cm=NULL;
  
  
  read_bitmap(&image,fname);
   
  printf("LoadFile %s : %d x %d x %d\n",
	 fname,
	 image.hdr.cols,image.hdr.rows,image.hdr.bits);
  printf("aspect=%g\n",image.hdr.aspect);
  colors=1<<image.hdr.bits;
  
  texture = new_texture( image.hdr.cols/8,image.hdr.rows);
  memcpy(texture->texels,image.bm,image.hdr.cols/8*image.hdr.rows);
  return texture;
}

