#include <graph.h>
#include "FixMath.hpp"
#include "Surface.hpp"
#include "Color.hpp"

void BuildPalette()
{
    rgb6 pal[256];
      
    for (int d=0; d<256; d++)
    {
        // First 64 levels will be shades of green
        if (d < 64)
        {
            pal[d].red = 1;
            pal[d].grn = (unsigned char)d;
            pal[d].blu = 1;
        }
    }

    // 0 should be black
    pal[0].red = 0;
    pal[0].grn = 0;
    pal[0].blu = 0;
    // 253 should be red
    pal[253].red = 53;
    pal[253].grn = 0;
    pal[253].blu = 0;
    // 254 should be gray
    pal[254].red = 53;
    pal[254].grn = 53;
    pal[254].blu = 53;
    // 255 should be white
    pal[255].red = 63;
    pal[255].grn = 63;
    pal[255].blu = 63;

    _remapallpalette((long *)pal);
}

void Colorize(int /*seaLevel*/)
{
    int y0, y, y1, y2, y3;
    uchar c;

    // Build color table
    for (long j=0; j<SIDE; j++)
    {
        for (long i=0; i<SIDE; i++)
        {
            y = Point(i, j);

            y0 = Point(i-1, j);
            y1 = Point(i, j);

            y2 = Point(i, j-1);
            y3 = Point(i, j);
			
            double s = ((y0 - y1) + (y3 - y2)) / 20.0;

            if (s>1.0)  s = 1.0;
            if (s<-1.0) s = -1.0;
            s = s / 2.0;

            c = (uchar)(63*s + 31);
            Colors[i + (j<<SIDEBITS)] = c;
        }
    }
}


