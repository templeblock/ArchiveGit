#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "wt.h"
#include "error.h"
#include "wtmem.h"
#include "framebuf.h"
#include "graphics.h"

void
expand(register char *bufp, register char *buf2p)
{
register	int  	x, xpix;

	for(xpix=SCREEN_WIDTH,x=0; x<SCREEN_WIDTH*SCREEN_HEIGHT; x++,xpix--)
	{
		if (!xpix)
		{
#if BLOCKY>1
			bcopy(buf2p-SCREEN_WIDTH*BLOCKY,
				buf2p,
				SCREEN_WIDTH*BLOCKY);
			buf2p+=SCREEN_WIDTH*BLOCKY;
#endif
#if BLOCKY>2
			bcopy(buf2p-SCREEN_WIDTH*BLOCKY,
				buf2p,
				SCREEN_WIDTH*BLOCKY);
			buf2p+=SCREEN_WIDTH*BLOCKY;
#endif
#if BLOCKY>3
			bcopy(buf2p-SCREEN_WIDTH*BLOCKY,
				buf2p,
				SCREEN_WIDTH*BLOCKY);
			buf2p+=SCREEN_WIDTH*BLOCKY;
#endif
			xpix=SCREEN_WIDTH;
		}
		*buf2p++ = *bufp;
#if BLOCKY>1
		*buf2p++ = *bufp;
#endif
#if BLOCKY>2
		*buf2p++ = *bufp;
#endif
#if BLOCKY>3
		*buf2p++ = *bufp;
#endif
		bufp++;
	}
}
