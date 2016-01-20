#include "fscdefs.h"
#include "fontmath.h"
#include "fnt.h"
#include "sc.h"
#include "effects.h"

extern FX Fx;

#define random                                     \
{                                                  \
	uint32 x = Fx.fx_random_seed + 0x92489248L;     \
	Fx.fx_random_seed = (x << 29) | (x >> 3);       \
}

uint32 sunset_function(void)
{
	int32 t, u;

	t = - (50 * (int32)Fx.fx_cell_height) / 256 - Fx.fx_fill_y;

	if (t <= 0)
	{
		return 1;
	}

	u = 2;

	while (t >= u)
	{
		t -= u;
		u += 2;
	}

	return (t == 0 || t == u/2) ? 1 : 0;
} // end sunset_function


uint32 fountain_function(void)
{
	int32 t, u, v, w, l[2];

	t = Fx.fx_cell_height/4;

	u = - Fx.fx_fill_y;

	if (u <= 0)
	{
		return 1;
	}

	if (u > t)
	{
		return 0;
	}

	v = Fx.fx_fill_x;

	if (v < 0)
	{
		v = -v;
	}

	// At this point:  0 < u <= t and v >= 0 
	MulAdd64(v, u, t/2, l);

	if (l[0] >= t)
	{
		return 1;
	}

	w = Mod64(l, t);

	return(w + u >= t ? 0 : 1);
} // end fountain_function


uint32 starburst_function(void)
{
	int32 x_xc,  y_yc;
	int32 XmodY, Xdiv8;
	int32 YmodX, Ydiv8;

	// Return 1 iff 
	// (x-xc = 0) or (y-yc = 0)
	// or  | (x-xc) mod (y-yc) | < | (x-xc) div 8 |
	// or  | (y-yc) mod (x-xc) | < | (y-yc) div 8 | 

	x_xc = (Fx.fx_char_x0 + Fx.fx_char_x1)/2 - Fx.fx_fill_x;

	if (x_xc == 0)
	{
		return 1;              // (x-xc = 0)
	}

	y_yc = (Fx.fx_char_y0 + Fx.fx_char_y1)/2 - Fx.fx_fill_y;

	if (y_yc == 0)
	{
		return 1;              // (y-yc = 0)
	}

	// Now test if | (x-xc) mod (y-yc) | < | (x-xc) div 8 |

	XmodY = x_xc % y_yc;
	if (XmodY < 0)
	{
		XmodY = - XmodY;
	}

	Xdiv8 = x_xc;
	if (Xdiv8 < 0)
	{
		Xdiv8 = -Xdiv8;
	}

	Xdiv8 /= 8;

	if (XmodY < Xdiv8)
	{
		return 1;      // | (x-xc) mod (y-yc) | < | (x-xc) div 8 |
	}

	// Now test if | (y-yc) mod (x-xc) | < | (y-yc) div 8 |

	YmodX = y_yc % x_xc;
	if (YmodX < 0)
	{
		YmodX = - YmodX;
	}

	Ydiv8 = y_yc;
	if (Ydiv8 < 0)
	{
		Ydiv8 = -Ydiv8;
	}

	Ydiv8 /= 8;

	if (YmodX < Ydiv8)
	{
		return 1;      // | (y-yc) mod (x-xc) | < | (y-yc) div 8 |
	}

	// Get rid of the square in the middle

	if (y_yc < -8 || y_yc > 8 || x_xc < -8 || x_xc > 8)
	{
		return 0;
	}

	return 1;
} // end starburst_function


uint32 london_fog_function(void)
{
	int32 t;
	uint32 d[2];

	t = Fx.fx_fill_y + Fx.fx_cell_ascent;

	if (t < 0)
	{
		return 0;
	}

	// Get random * cell height

	random;

	UMul64(Fx.fx_cell_height, Fx.fx_random_seed, d);

	// Return 1 iff current raster row > random*fx_cell_height

	return ((uint32)t <= d[0]) ? 0 : 1;
} // end london_fog_function


uint32 radial_function(void)
{
	int32 t, u, z;
	uint32 d[2], a;

	// Adjust point relative to center of character cell and reflect 
	// into first quadrant.

	t = (Fx.fx_char_x0 + Fx.fx_char_x1)/2 - Fx.fx_fill_x;
	if (t < 0)
	{
		t = -t;
	}

	u = (Fx.fx_char_y0 + Fx.fx_char_y1)/2 - Fx.fx_fill_y;
	if (u < 0)
	{
		u = -u;
	}

	// Check if point is on the line X=Y.  If so, pixel is on by 
	// convention. Note that this also handles the case of the origin.

	if (t == u)
	{
		return 1;
	}

	// Reflect coordinate into first quadrant

	if (t < u)
	{
		z = t;
		t = u;
		u = z;
	}

	// Check the slope of the line running through the origin to the 
	// coordinate.  If it is in one of the following ranges, turn the 
	// pixel on.  Note that we are guaranteed that the slope is less 
	// than one. 
	//
	//       0 -  4 degrees                  0 to  300333370
	//      11 - 19 degrees          834857071 to 1478875838
	//      26 - 34 degrees         2094795517 to 2896992020
	//      41 - 45 degrees         3733558109 to 4294967294

	d[0] = (uint32)u;
	d[1] = 0;
	a = UDiv64(d, t);

	// Check if the slope is in range

	if (a <= 300333370UL)
	{
		return 1;
	}

	if (a < 834857071UL)
	{
		return 0;
	}

	if (a <= 1478875838UL)
	{
		return 1;
	}

	if (a < 2094795517UL)
	{
		return 0;
	}

	if (a <= 2896992020UL)
	{
		return 1;
	}

	if (a < 3733558109UL)
	{
		return 0;
	}

	return 1;
} // end radial_function


uint32 sunrise_function(void)
{
	int32 t, u;

	t = - ((int32)Fx.fx_cell_height/4) - Fx.fx_fill_y;

	if (t >= 0)
	{
		return 1;
	}

	t = - t;
	u = 2;

	while (t >= u)
	{
		t -= u;
		u += 2;
	}

	return (t == 0 || t == u/2) ? 1 : 0;
} // end sunrise_function


uint32 plaid_function(void)
{
	int32 xDim, yDim, xCorr, yCorr;

	// Load the coordinates

	xDim  = Fx.fx_pattern_descriptor.nXSize;
	yDim  = Fx.fx_pattern_descriptor.nYSize;
	xCorr = Fx.fx_fill_x;
	yCorr = Fx.fx_fill_y;

	// Convert plaid cell dimension to plaid square dimension:
	//
	//      Plaid cell:  XO   Plaid square:  X
	//                   OX

	xDim /= 2;
	yDim /= 2;

	// If the X or Y coordinate is negative, reflect it so that it will
	// produce the proper result:
	//
	//      C' = (-C)+(plaid dimension)

	if (xCorr < 0)
	{
		xCorr = xDim - xCorr;
	}

	if (yCorr < 0)
	{
		yCorr = yDim - yCorr;
	}

	// Compute the plaid coordinates of the rasterization X and Y coordinates.

	if (xDim <= 0)
	{
		return 1;
	}

	xDim = xCorr / xDim;

	if (yDim <= 0)
	{
		return 1;
	}

	yDim = yCorr / yDim;

	// If the parity of the plaid coordinates is the same then turn 
	// on the bit. Otherwise, leave it off.

	xDim &= 1;
	yDim &= 1;

	return (xDim == yDim) ? 1 : 0;
} // end plaid_function


void plaid_initialize(void)
{
	int32 t, u;

	// Compute the size of the plaid.  This is a function of the cell height:
	//
	//      2*max(floor(fx_cell_height/32),2)

	t = Fx.fx_cell_height/32;

	if (t < 2)
	{
		t = 2;
	}

	u = 2*t;

	Fx.fx_pattern_descriptor.nXSize = u;
	Fx.fx_pattern_descriptor.nYSize = u;

	Fx.fx_pattern_descriptor.nXOffset = 0;
	Fx.fx_pattern_descriptor.nYOffset = 0;
} // end plaid_initialize


uint32 stripes_function(void)
{
	int32 t, u, v;

	t = Fx.fx_fill_y;

	u = Fx.fx_cell_height/16;

	if (u <= 1)
	{
		return 1;
	}

	v = t % u;

	if (v < 0)
	{
		v += u;
	}

	if (v < 0)
	{
		v = -v;
	}

	return (v < (u + 3)/4) ? 0 : 1;
} // end stripes_function


void stripes_initialize(void)
{
	// Compute the vertical size of the stripes function:
	//
	//      fx_cell_height shr 4

	Fx.fx_pattern_descriptor.nYSize = Fx.fx_cell_height/16;
	Fx.fx_pattern_descriptor.nXSize = 0;

	Fx.fx_pattern_descriptor.nXOffset = 0;
	Fx.fx_pattern_descriptor.nYOffset = 0;
} // end stripes_initialize


uint32 cloud_function(void)
{
	int32 t, u;
	uint32 d[2], v, w;

	t = (Fx.fx_char_x0 + Fx.fx_char_x1)/2 - Fx.fx_fill_x;

	if (t < 0)
	{
		t = - t;
	}

	u = ((Fx.fx_char_x1 - Fx.fx_char_x0) >> 1) + 2;

	if (t >= u)
	{
		return 0;
	}

	d[0] = t;
	d[1] = 0;
	v = UDiv64(d, (uint32)u);

	t = (Fx.fx_char_y0 + Fx.fx_char_y1)/2 - Fx.fx_fill_y;

	if (t < 0)
	{
		t = - t;
	}

	u = ((Fx.fx_char_y1 - Fx.fx_char_y0) >> 1) + 2;

	if (t >= u)
	{
		return 1;
	}

	d[0] = t;
	d[1] = 0;
	w = UDiv64(d, (uint32)u);

	if (v < w)
	{
		v = w;
	}

	random;

	return (Fx.fx_random_seed < v) ? 0 : 1;
} // end cloud_function


uint32 brick_function(void)
{
	int32 t, u, v, w;
	int32 xSize, ySize;
	int brick3;
	uint32 randValue;

	t = Fx.fx_fill_x;
	u = Fx.fx_fill_y;

	xSize = Fx.fx_pattern_descriptor.nXSize;
	ySize = Fx.fx_pattern_descriptor.nYSize;

	// Get the X coordinate normalized to the pattern cell

	if (xSize == 0)
	{
		return 1;
	}

	v = t % xSize;

	if (v < 0)
	{
		v += xSize;
	}

	// Get the Y coordinate normalized to the pattern cell

	if (ySize == 0)
	{
		return 1;
	}

	w = u % ySize;

	if (w < 0)
	{
		w += ySize;
	}

	// Determine if we are inside a brick or inside the grout area

	if (w < Fx.brick_y1)
	{
		brick3 = (v < Fx.brick_x3);
	}
	else
	{
		if (w < Fx.brick_y2 || w >= Fx.brick_y3)
		{
			brick3 = false;
		}
		else
		{
			if (v < Fx.brick_x1)
			{
				brick3 = true;
			}
			else
			{
				brick3 = (v > Fx.brick_x2);
			}
		}
	}

	random;

	randValue = (brick3 ? 3604479999 : 524288000);

	return (Fx.fx_random_seed >= randValue) ? 0 : 1;
} // end brick_function


void brick_initialize(void)
{
	int32 t, u, v;

	// Pattern cell width is (font cell width/5)

	t = Fx.fx_pattern_descriptor.nXSize = Fx.fx_cell_width / 5;

	// Compute the various horizontal positions on the brick pattern.
	//
	//      brick_x1:       start of middle grout line
	//      brick_x2:       end of middle grout line
	//      brick_x3:       start of ending grout line
	//
	// The width of the grout line is (pattern cell width/13).

	u = t / 13;

	v = Fx.brick_x1 = (t - 2*u)/2;

	Fx.brick_x2 = v + u;
	Fx.brick_x3 = 2*v + u;

	// Pattern cell height is pattern cell width adjusted for device 
	// resolution.  This will ensure the same aspect ratio across 
	// devices.

	t = Fx.fx_pattern_descriptor.nYSize = MulDiv64(t, Fx.fx_y_resolution, Fx.fx_x_resolution);

	// Compute the various vertical positions on the brick pattern.
	//
	//      brick_y1:       start of middle grout line
	//      brick_y2:       end of middle grout line
	//      brick_y3:       start of ending grout line
	//
	// The width of the grout line is (pattern cell height/13).

	u = t / 13;

	v = Fx.brick_y1 = (t - 2*u)/2;

	Fx.brick_y2 = v + u;
	Fx.brick_y3 = 2*v + u;

	// Clear out the pattern offsets

	Fx.fx_pattern_descriptor.nXOffset = 0;
	Fx.fx_pattern_descriptor.nYOffset = 0;
} // end brick_initialize


uint32 zigzag_function(void)
{
	int32 t, u, v, w, z;
	int32 xSize, ySize;

	t = Fx.fx_fill_x;
	u = Fx.fx_fill_y;

	Fx.zigzag_x = t;                // Save X and Y coordinates
	Fx.zigzag_y = u;

	xSize = Fx.fx_pattern_descriptor.nXSize;
	ySize = Fx.fx_pattern_descriptor.nYSize;

	// Get the X coordinate normalized to the pattern cell

	if (xSize == 0)
	{
		return 1;
	}
	v = t % xSize;
	if (v < 0)
	{
		v += xSize;
	}

	// Get the Y coordinate normalized to the pattern cell

	if (ySize == 0)
	{
		return 1;
	}
	w = u % ySize;
	if (w < 0)
	{
		w += ySize;
	}

	// The pattern cell is symmetric about the line Y = zigzag_slope_denominator.
	// Reflect it into the first half.

	z = w;
	if (w > Fx.zigzag_slope_denominator)
	{
		w = ySize - w;
	}

	// We now have the X and Y coordinates within the pattern cell.  
	// Convert the Y coordinate to the corresponding X coordinate of 
	// the stripe.  We can then check if the given X coordinate is in 
	// range.

	t = MulDiv64(w, Fx.zigzag_slope_numerator, Fx.zigzag_slope_denominator);

	// Check if the X coordinate lies within the stripe

	if ((v >= t) && (v <= t + Fx.zigzag_thickness))
	{
		return 1;
	}

	// We are not in one of the black zigzag bands.  Use the gap 
	// pattern instead:
	//
	//      X---
	//      --X-
	//      -X--
	//      ---X

	v = ((Fx.zigzag_x & 1) << 1) | ((Fx.zigzag_x & 2) >> 1);
	v -= Fx.zigzag_y;

	return ((v & 3) != 0) ? 0 : 1;
} // end zigzag_function


void zigzag_initialize(void)
{
	// Pattern cell width is (font cell height/8) adjusted for the 
	// device resolution.

	Fx.fx_pattern_descriptor.nYSize = (((Fx.fx_cell_height + 4)/8) + 2) & ~3;

	// Compute the denonminator of the slope fraction.  This will be 
	// used to convert the given Y coordinate to the corresponding X 
	// coordinate which is used to check if we are in the stripe.

	Fx.zigzag_slope_denominator = Fx.fx_pattern_descriptor.nYSize/2;

	// Pattern cell width is (font cell width/8)

	Fx.fx_pattern_descriptor.nXSize = (((Fx.fx_cell_width + 4)/8) + 2) & ~3;

	// Compute the thickness of the zigzag stripe: (pattern cell width/2)

	Fx.zigzag_thickness = ((Fx.fx_pattern_descriptor.nXSize/2) + 2) & ~3;

	// Compute the numerator of the slope fraction.  This will be used 
	// to convert the given Y coordinate to the corresponding X 
	// coordinate which is used to check if we are in the stripe.

	Fx.zigzag_slope_numerator = Fx.fx_pattern_descriptor.nXSize - Fx.zigzag_thickness;

	// Clear out the pattern offsets

	Fx.fx_pattern_descriptor.nXOffset = 0;
	Fx.fx_pattern_descriptor.nYOffset = 0;
} // end zigzag_initialize


uint32 tubular_function(void)
{
	int32 t, u, v;
	uint32 d[2];

	u = Fx.fx_fill_x - Fx.fx_char_x0;
	v = Fx.fx_char_x1 - Fx.fx_char_x0;
	if (v <= 0)
	{
		return 1;
	}

	t = v >> 1;

	if (u >= t)
	{
		u = v-u;
		if (u >= t)
		{
			return 0;
		}
	}

	if ((u >= t) || (u < 0))
	{
		return 1;
	}

	d[0] = u;
	d[1] = 0;
	v = UDiv64(d, t);

	random;

	return ((uint32)v >= Fx.fx_random_seed) ? 0 : 1;
} // end tubular_function


uint32 swirl_function(void)
{
	int32 yAdj, xCorr, yCorr, t, d[2];
	int32 xT, yT;
	int32 xN, yN;
	int32 xN2[2], yN2[2], sum[2];
	uint32 bitNum, u;

	bitNum = 58;

	yAdj = Fx.fx_cell_height/4;

	t = Fx.fx_pattern_descriptor.nYOffset & ~3;
	yCorr = Fx.fx_fill_y - t;

	t = Fx.fx_pattern_descriptor.nXOffset & ~3;
	xCorr = Fx.fx_fill_x - t;

	// Compute the transformed Y coordinate

	yT = yCorr - yAdj;

	if (yT < 0)
	{
		yT = -yT;
	}

	// Compute the transformed X coordinate

	xT = xCorr;

	if (xT < 0)
	{
		xT = -xT;
	}

	// Adjust the X coordinate and cell width so they can be normalized

	t = 2*Fx.fx_cell_width;

	while (xT >= t)
	{
		xT >>= 1;
		yT >>= 1;
		bitNum -= 2;
	}

	// Adjust the Y coordinate & cell height so they can be normalized

	t = 2*Fx.fx_cell_height;

	while (yT >= t)
	{
		xT >>= 1;
		yT >>= 1;
		bitNum -= 2;
	}

	// Compute the X coordinate normalized to the cell width

	t = 2*Fx.fx_cell_width + 1;

	if (xT >= t)
	{
		return 1;
	}

	d[0] = xT;
	d[1] = 0;
	xN = Div64(d, t);

	// Compute the Y coordinate normalized to the cell height

	t = 2*Fx.fx_cell_height + 1;

	if (yT >= t)
	{
		return 1;
	}

	d[0] = yT;
	d[1] = 0;
	yN = Div64(d, t);

	// Compute the square of the distance of the point from the origin.
	// A bit in this quantity is tested to see if we are in a swirl band.

	Mul64(xN, xN, xN2);
	Mul64(yN, yN, yN2);
	Add64(xN2, yN2, sum);

	// Test bit number specified in bitNum in the 64-bit sum

	Shr64(sum, bitNum, sum);
	if ((sum[1] & 1) != 0)
	{
		return 1;
	}

	// We are not in one of the black swirl bands. Use the gap pattern instead:
	//
	//      X---
	//      --X-
	//      -X--
	//      ---X

	u = ((yCorr & 1) << 1) | ((yCorr & 2) >> 1);
	u -= xCorr;

	return ((u & 3) != 0) ? 0 : 1;
} // end swirl_function



void swirl_initialize(void)
{
	Fx.fx_pattern_descriptor.nXOffset = 0;
	Fx.fx_pattern_descriptor.nYOffset = 0;
} // end swirl_initialize



Pattern Gray2PercentPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	0,								// don't rotate
	0,								// don't expand

	8,                      // X size
	16,                     // Y size

	0,                      // X offset
	0,                      // Y offset

	0x80,                   // pattern data
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x08,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};
        
Pattern Gray5PercentPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	0,								// don't rotate
	0,								// don't expand

	8,                      // X size
	10,                     // Y size

	0,                      // X offset
	0,                      // Y offset

	0x80,                   // pattern data
	0x00,
	0x00,
	0x00,
	0x00,
	0x08,
	0x00,
	0x00,
	0x00,
	0x00
};
        
Pattern Gray15PercentPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	0,								// don't rotate
	0,								// don't expand

	8,                      // X size
	4,                      // Y size

	0,                      // X offset
	0,                      // Y offset

	0x80,                   // pattern data
	0x00,
	0x08,
	0x00
};
        
Pattern Gray30PercentPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	0,								// don't rotate
	0,								// don't expand

	6,                      // X size
	4,                      // Y size

	0,                      // X offset
	0,                      // Y offset

	0x80,                   // pattern data
	0x00,
	0x10,
	0x00
};
        
Pattern Gray50PercentPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	0,								// don't rotate
	0,								// don't expand

	4,                      // X size
	4,                      // Y size

	0,                      // X offset
	0,                      // Y offset

	0x80,                   // pattern data
	0x00,
	0x20,
	0x00
};
        
Pattern Gray70PercentPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	0,								// don't rotate
	0,								// don't expand

	4,                      // X size
	4,                      // Y size

	0,                      // X offset
	0,                      // Y offset

	0x80,                   // pattern data
	0x20,
	0x40,
	0x10
};
        
Pattern Gray90PercentPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	0,								// don't rotate
	0,								// don't expand

	4,                      // X size
	4,                      // Y size

	0,                      // X offset
	0,                      // Y offset

	0xc0,                   // pattern data
	0xc0,
	0x30,
	0x30
};
        
Pattern HorizontalPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	1,								// rotate
	1,								// expand

	8,                      // X size
	8,                      // Y size

	0,                      // X offset
	0,                      // Y offset

	0xff,                   // pattern data
	0x00,
	0x00,
	0x00,
	0xff,
	0x00,
	0x00,
	0x00
};
        
Pattern VerticalPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	1,								// rotate
	1,								// expand

	8,                      // X size
	8,                      // Y size

	0,                      // X offset
	0,                      // Y offset

	0x88,                   // pattern data
	0x88,
	0x88,
	0x88,
	0x88,
	0x88,
	0x88,
	0x88
};
        
Pattern GraphPaperPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	1,								// rotate
	1,								// expand

	8,                      // X size
	8,                      // Y size

	0,                      // X offset
	0,                      // Y offset

	0x10,                   // pattern data
	0x10,
	0x10,
	0xff,
	0x10,
	0x10,
	0x10,
	0x10
};
        
Pattern HorizontalWoodgrainPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	1,								// rotate
	1,								// expand

	81,                     // X size
	113,                    // Y size

	0,                      // X offset
	0,                      // Y offset

	0xfd,0xff,0xff,0x7f,0xfe,0xff,0xef,0xbb,0x7f,0xff,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfd,0x80,
	0xff,0x7f,0xfe,0xff,0x67,0xff,0xf7,0xff,0x3e,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x7b,0xff,0x3f,0x7c,0x00,0x01,0xf0,0x00,0x1f,0x80,
	0xff,0x80,0x00,0x00,0x03,0xef,0xde,0x07,0xff,0xe0,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x0f,0xff,0xea,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,
	0xef,0x60,0x00,0x00,0x01,0xe0,0x00,0xef,0xc3,0xf7,0x80,
	0x00,0x00,0x00,0x00,0x00,0x0f,0xfe,0x00,0x1c,0x00,0x00,
	0x0b,0xdf,0x7f,0xbf,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
	0xb0,0x00,0x00,0x00,0x3f,0x7b,0xf3,0xed,0xff,0x7e,0x80,
	0x00,0x00,0x7d,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0xfb,0x80,0x07,0xf8,0x00,0x00,0x1f,0xf0,0x00,0x00,
	0xf6,0x00,0x00,0x00,0x07,0xef,0xff,0xe0,0x05,0xff,0x80,
	0x00,0x01,0xef,0xfc,0x00,0x00,0x00,0x03,0x80,0x00,0x00,
	0x00,0x7e,0x00,0x03,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,
	0xfb,0x80,0xde,0xe8,0x1e,0xff,0xff,0xcf,0xdf,0xef,0x80,
	0x00,0x03,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x3c,0x7f,0xf8,0xef,0x9d,0xfe,0x00,0x00,0x00,0x00,
	0xdd,0xc1,0x80,0x04,0x00,0x00,0x01,0xef,0xff,0xff,0x80,
	0x00,0x0e,0x7f,0xe2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x30,0x80,0x19,0xef,0xfb,0xff,0x80,0x00,0x00,0x00,
	0xff,0xc1,0x7f,0xc4,0x00,0x00,0x00,0x7b,0xfb,0xff,0x80,
	0x00,0x02,0x80,0x24,0x0f,0x00,0x70,0x00,0x00,0x00,0x00,
	0x00,0x02,0x97,0xa8,0x70,0xfb,0x0f,0x3f,0xc0,0x00,0x00,
	0xef,0xf2,0x40,0x49,0x80,0x00,0x00,0xc0,0x3f,0xff,0x80,
	0x00,0x09,0xbf,0x92,0x07,0xff,0xf0,0x00,0x00,0x00,0x00,
	0xfc,0x04,0x40,0x24,0x38,0x00,0x0f,0xef,0xfd,0xff,0x80,
	0x03,0xf2,0x3f,0xc9,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x09,0x80,0x12,0x03,0xff,0xb0,0x00,0x00,0x00,0x00,
	0x00,0x04,0x7f,0xe4,0x3c,0x00,0x0f,0xff,0xdf,0xff,0x80,
	0xbf,0xf2,0x00,0x08,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x09,0xff,0xf3,0x00,0xf6,0x80,0x00,0x00,0x00,0x00,
	0x00,0x06,0x00,0x0c,0x0f,0x00,0x50,0x00,0x00,0x00,0x00,
	0x00,0x01,0xff,0xf0,0x70,0x00,0x07,0xfe,0xff,0xff,0x80,
	0xff,0xf0,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x1f,0xff,0xff,0x00,0x0e,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x0f,0xe0,0xf8,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xf0,0x00,0x07,0xf7,0x2f,0xff,0x80,
	0xda,0xf7,0xdf,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x03,0xf7,0xfb,0xff,0xed,0xdf,0x80,
	0xfa,0x80,0x00,0x07,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x37,0xdf,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x7f,0xe0,0x00,0x00,0x00,0x00,
	0xf7,0xdf,0xfc,0x7f,0xdf,0x00,0x16,0xf7,0xff,0xff,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xff,0xf0,0x00,0x7f,0xff,0xe0,0x00,0x7f,0xfc,0x3f,0x80,
	0x00,0x0f,0xef,0x80,0x00,0x1f,0xdb,0x80,0x03,0xc0,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xf7,0xff,0xfe,0xff,0xff,0xfe,0x00,0x3d,0xff,0xef,0x80,
	0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xc0,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xf7,0x80,0x00,0xee,0xf8,0x00,0x00,0x00,
	0xfd,0xfe,0xff,0x00,0x77,0xff,0x00,0x07,0xef,0xff,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0xff,0xde,0x00,0x00,0x00,0x00,
	0x00,0x00,0x3f,0xf7,0xff,0x00,0x01,0x40,0x00,0x00,0x00,
	0xff,0xfd,0xc0,0x00,0x00,0x00,0x00,0x3f,0xfe,0xff,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xff,0xef,0xd8,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xff,0x00,0x00,0x06,0x00,0x00,0x00,0x00,
	0xd5,0xff,0xfb,0x00,0x00,0x0e,0xc1,0xff,0xff,0xff,0x80,
	0x00,0x00,0x00,0x00,0x7f,0x70,0x1c,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xff,0x80,0x00,0x02,0x00,0x00,0x00,0x00,
	0x00,0x00,0x1f,0x80,0x00,0x07,0xe1,0xde,0xff,0xff,0x80,
	0xfb,0xff,0xe0,0x00,0x01,0xf0,0x18,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x06,0x00,0x04,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x78,0x7b,0xc3,0x80,0x00,0x00,0x00,
	0x00,0x00,0x07,0xfb,0x80,0x80,0x30,0x3e,0xf7,0xf7,0x80,
	0xff,0xff,0xf8,0x00,0x00,0x8e,0x88,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x20,0x10,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x3d,0xfc,0x1f,0xa1,0xdf,0xfe,0xff,0x80,
	0xfe,0xfd,0xff,0xc0,0x01,0x80,0x02,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x5f,0xbc,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xff,0xfe,0xff,0xff,0xcf,0x00,0x00,0xf7,0xfb,0xff,0x80,
	0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xdf,0x80,
	0x00,0x7f,0xff,0xff,0xfb,0xef,0x7f,0xff,0xf0,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xdf,0xf7,0x80,
	0x00,0xff,0xff,0xff,0xfb,0xff,0xff,0x70,0x00,0x00,0x00,
	0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x7f,0xc0,0x00,0x00,0xff,0xff,0xf0,0x0f,0xff,0x80,
	0xff,0x80,0x2e,0xff,0xf7,0x00,0x00,0x0f,0xf0,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xff,0xff,0xa0,0x00,0x00,0x00,0x3f,0xf7,0xdd,0xff,0x80,
	0x00,0x00,0x1f,0xff,0xf7,0xf7,0xc0,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x07,0xde,0x00,0x00,0x00,0x00,0x01,0xff,0x7d,0xf0,0x00,
	0xf8,0x01,0xdf,0xff,0xff,0xff,0xbe,0x00,0x00,0x0f,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

Pattern StarryNightPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	1,								// rotate
	1,								// expand

	31,                     // X size
	29,                     // Y size

	0,                      // X offset
	0,                      // Y offset

	0x00,0x00,0x20,0x00,    // pattern data
	0x04,0x00,0x20,0x00,
	0x04,0x00,0x70,0x00,
	0x0e,0x01,0xfc,0x00,
	0x0e,0x00,0x70,0x00,
	0xff,0xe0,0xd8,0x00,
	0x7f,0xc0,0x88,0x00,
	0x1f,0x00,0x00,0x00,
	0x3b,0x80,0x02,0x00,
	0x31,0x80,0x02,0x00,
	0x60,0xc0,0x02,0x00,
	0x40,0x40,0x07,0x00,
	0x00,0x00,0x07,0x00,
	0x00,0x00,0x07,0x00,
	0x00,0x00,0x0f,0x80,
	0x00,0x03,0xff,0xfe,
	0x00,0x00,0xff,0xf8,
	0x00,0x00,0x3f,0xe0,
	0x00,0x00,0x1f,0xc0,
	0x00,0x00,0x1f,0xc0,
	0x00,0x00,0x3d,0xe0,
	0x08,0x00,0x38,0xe0,
	0x08,0x00,0x70,0x70,
	0x1c,0x00,0x60,0x30,
	0x7f,0x00,0xc0,0x18,
	0x1c,0x00,0x80,0x08,
	0x36,0x00,0x00,0x00,
	0x22,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00
};

Pattern SunsetPattern =
{
	sunset_function,        // data function
	NULL,                   // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern FountainPattern =
{
	fountain_function,      // data function
	NULL,                   // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern RattanPattern =
{
	NULL,                   // data function
	NULL,                   // initialization function

	1,								// rotate
	1,								// expand

	16,                     // X size
	16,                     // Y size

	0,                      // X offset
	0,                      // Y offset

	0xff,0xc0,              // pattern data
	0x7f,0x60,
	0x3e,0x30,
	0x1c,0x18,
	0x18,0x1c,
	0x30,0x3e,
	0x60,0x7f,
	0xc0,0xff,
	0x81,0xff,
	0x03,0x7f,
	0x06,0x3e,
	0x0c,0x1c,
	0x1c,0x0c,
	0x3e,0x06,
	0x7f,0x03,
	0xff,0x81
};

Pattern SunrisePattern =
{
	sunrise_function,       // data function
	NULL,                   // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern StarburstPattern =
{
	starburst_function,     // data function
	NULL,                   // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern RadialPattern =
{
	radial_function,        // data function
	NULL,                   // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern SwirlPattern =
{
	swirl_function,         // data function
	swirl_initialize,       // initialization function

	1,								// rotate
	0,								// don't expand

	4,                      // X size
	4,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern StripesPattern =
{
	stripes_function,       // data function
	stripes_initialize,     // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern ZigzagPattern =
{
	zigzag_function,        // data function
	zigzag_initialize,      // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern PlaidPattern =
{
	plaid_function,         // data function
	plaid_initialize,       // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern BrickPattern =
{
	brick_function,         // data function
	brick_initialize,       // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern LondonFogPattern =
{
	london_fog_function,    // data function
	NULL,                   // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern CloudPattern =
{
	cloud_function,         // data function
	NULL,                   // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

Pattern TubularPattern =
{
	tubular_function,       // data function
	NULL,                   // initialization function

	1,								// rotate
	0,								// don't expand

	0,                      // X size
	0,                      // Y size

	0,                      // X offset
	0,                      // Y offset
};

// PatternTable
//
//		Tables of  patterns indexed by MLS pattern number

Pattern* Patterns[] =
{
	(Pattern*)BLACK_PATTERN,
	(Pattern*)WHITE_PATTERN,
	&Gray2PercentPattern,
	&Gray5PercentPattern,
	&Gray15PercentPattern,
	&Gray30PercentPattern,
	&Gray50PercentPattern,
	&Gray70PercentPattern,
	&Gray90PercentPattern,
	&HorizontalPattern,
	&VerticalPattern,
	&GraphPaperPattern,
	&HorizontalWoodgrainPattern,
	&StarryNightPattern,
	&SunsetPattern,
	&FountainPattern,
	&RattanPattern,
	&SunrisePattern,
	&StarburstPattern,
	&RadialPattern,
	&SwirlPattern,
	&StripesPattern,
	&ZigzagPattern,
	&PlaidPattern,
	&BrickPattern,
	&LondonFogPattern,
	&CloudPattern,
	&TubularPattern
};

// end patterns.cpp
