#include "stdafx.h"
#include "marquee.h"

#include "misc.h"

static void near
xor_line(HDC hdc,	int x0, int y0, int x1, int y1)
{
	PatBlt(hdc, x0, y0, x1-x0, y1-y0, DSTINVERT);
}

static void near
xor_box(HDC hdc, RECT r)
{
	xor_line(hdc, r.left, r.top, r.right, r.top+1);
	xor_line(hdc, r.right-1, r.top+1, r.right, r.bottom-1);
	xor_line(hdc, r.left, r.bottom-1, r.right, r.bottom);
	xor_line(hdc, r.left, r.top+1, r.left+1, r.bottom-1);
}

void Marquee::drawSelf( TRISTATE ts, HDC in_hDC)
{
	if (ts != ANIMATE)
	{
		HDC hDC;

		if ((hDC = in_hDC) != NULL || (hDC = GetDC( hWnd )) != NULL)
		{
			xor_box(hDC, r);

			if (in_hDC == NULL)
			{
				ReleaseDC(hWnd, hDC);
			}
		}
	}
}

void Marquee::draw(HDC hDC)
{
	if (!drawn)
	{
		drawn = TRUE;
		drawSelf(DRAW, hDC);
	}
}

void Marquee::undraw(HDC hDC)
{
	if (drawn)
	{
		drawn = FALSE;
		drawSelf(UNDRAW, hDC);
	}
}

void Marquee::animate(HDC hDC)
{
	if (drawn)
	{
		drawSelf(ANIMATE, hDC);
	}
}

