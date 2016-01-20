#include <stdlib.h>
#include <malloc.h>
#include <mem.h>
#include "FixMath.hpp"
#include "Surface.hpp"

uchar *Surface;
uchar *Colors;

char ISurface()
{
    Surface = (uchar *)malloc(PTS);
    Colors   = (uchar *)malloc(PTS);

	if (Surface==NULL || Colors==NULL) return 0;

	memset(Surface, 0, PTS);
	memset(Colors, 0, PTS);

	return 1;
}

ushort *gPrj;
void BuildProjectionTable()
{
	gPrj = (ushort *)malloc(65536 * sizeof(ushort));
	if (!gPrj) exit(1);
	
	for (int z=0; z<65536; z++)
		gPrj[z] = (double)FOV / (z/256.0) * 256.0 / 8;
}

