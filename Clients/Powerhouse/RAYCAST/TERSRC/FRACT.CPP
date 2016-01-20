#pragma warn -sig
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "FixMath.hpp"
#include "Surface.hpp"

long gSeed;
#define Seed(i, j) srand(gSeed + ((i)&(SIDE-1)) + ((j)&(SIDE-1))*SIDE)
#define random(x)  (x ? ((unsigned)rand() % (x)) : 0)

//
// Surface is 256x256x8bits
//
// This function is toyed with to change the
// surface.
//
const int BUMP4 =  4;
const int BUMP3 =  6;
const int BUMP2 =  9;
const int BUMP1 =  2;
void Fractify(int i, int j, int w, int h)
{
    if (w<2) return;
    int w2 = w >> 1;
    int h2 = h >> 1;

    Point(i+w2, j  )  = (uchar)((Point(i,   j  ) + Point(i+w,j)  ) >> 1);
    Point(i+w2, j+h)  = (uchar)((Point(i,   j+h) + Point(i+w,j+h)) >> 1);
    Point(i,    j+h2) = (uchar)((Point(i,   j  ) + Point(i,j+h)  ) >> 1);
    Point(i+w,  j+h2) = (uchar)((Point(i+w, j  ) + Point(i+w,j+h)) >> 1);

    if (w2<=4)
    {
        Seed( i+w2, j);
        Point(i+w2, j)    += random(w2*BUMP4);
        Seed( i+w2, j+h);
        Point(i+w2, j+h)  += random(w2*BUMP4);
        Seed( i,    j+h2);
        Point(i,    j+h2) += random(w2*BUMP4);
        Seed( i+w,  j+h2);
        Point(i+w,  j+h2) += random(w2*BUMP4);
    }
    else
    if (w2<=8)
    {
        Seed( i+w2, j);
        Point(i+w2, j)    += random(w2*BUMP3);
        Seed( i+w2, j+h);
        Point(i+w2, j+h)  += random(w2*BUMP3);
        Seed( i,    j+h2);
        Point(i,    j+h2) += random(w2*BUMP3);
        Seed( i+w,  j+h2);
        Point(i+w,  j+h2) += random(w2*BUMP3);
    }
    else
	if (w2<=16)
    {
        Seed( i+w2, j);
        Point(i+w2, j)    += random(w2*BUMP2);
        Seed( i+w2, j+h);
        Point(i+w2, j+h)  += random(w2*BUMP2);
        Seed( i,    j+h2);
        Point(i,    j+h2) += random(w2*BUMP2);
        Seed( i+w,  j+h2);
        Point(i+w,  j+h2) += random(w2*BUMP2);
    }
    else if (w2<=32)
    {
        Seed( i+w2, j);
        Point(i+w2, j)    += random(w2*BUMP1);
        Seed( i+w2, j+h);
        Point(i+w2, j+h)  += random(w2*BUMP1);
        Seed( i,    j+h2);
        Point(i,    j+h2) += random(w2*BUMP1);
        Seed( i+w,  j+h2);
        Point(i+w,  j+h2) += random(w2*BUMP1);
    }

    Point(i+w2, j+h2) = (uchar)((Point(i+w2, j)
                        +Point(i,    j+h2)
                        +Point(i+w2, j+h)
                        +Point(i+w,  j+h2)) >> 2);

    Fractify(i,    j,    w2, h2);
    Fractify(i+w2, j,    w2, h2);
    Fractify(i,    j+h2, w2, h2);
    Fractify(i+w2, j+h2, w2, h2);
}

const int SIZE = 3;
void Smoothify()
{
    uchar *filt = (uchar *)malloc(PTS);

	for (int j=0; j<SIDE; j++)
		for (int i=0; i<SIDE; i++)
		{
			int sum = 0;
			for (int v=-SIZE/2; v<=SIZE/2; v++)
				for (int u=-SIZE/2; u<=SIZE/2; u++)
					sum += Point(i+u, j+v);
			filt[i+j*SIDE] = (uchar)(sum/SIZE/SIZE);
		}
		
	memcpy(Surface, filt, PTS);
	free(filt);
}	


