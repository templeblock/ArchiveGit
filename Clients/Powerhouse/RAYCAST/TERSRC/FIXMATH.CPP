#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include "FixMath.hpp"

long cs[ARC], sn[ARC];

#define M_PI 3.14159265358979323846
void BuildTrigTables()
{
    // Build cos,sin tables
    for (short i=0; i<ARC; i++)
    {
        cs[i] = cos(i * 2 * M_PI / ARC) * (1L << FIX);
        sn[i] = sin(i * 2 * M_PI / ARC) * (1L << FIX);
    }
}


short *Interp;
void BuildInterpolationTable()
{
    Interp = (short *)malloc(512 * 256 * 2);
    if (!Interp) exit(1);

    for (int i=-255; i<256; i++)
    {
        int off = (i+255) << 8;
        for (int f=0; f<256; f++)
            Interp[off+f] = (short)(i * f);
    }
}


