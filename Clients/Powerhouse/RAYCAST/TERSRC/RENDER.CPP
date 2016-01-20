#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>
#include <time.h>
#include <dos.h>
#include <math.h>
#include <assert.h>
#include "Screen.hpp"
#include "FixMath.hpp"
#include "Surface.hpp"
#include "Color.hpp"
#include "Ray.hpp"

// Cache the highest row painted per column 
int   row;
// Pointer to 'bottom' pixel of current column 
char *rowP;

// Previously plotted height 'y' and color 'c'
int   py;
int   pc;

// Pointer to offscreen bitmap
char *offMap;

// Viewing position
int gX, gY, gZ;

extern char *_FillCol(char *row, int color, int dc, int count);
#pragma aux _FillCol = \
	"inc ecx"\
	"shr ecx, 1"\
	"jnc short oddIters"\
	"fill_loop: mov [ebx], ah"\
	"add eax, edx"\
	"sub ebx, 256"\
	"oddIters: mov [ebx], ah"\
	"add eax, edx"\
	"sub ebx, 256"\
	"loop fill_loop"\
	parm [ebx] [eax] [edx] [ecx]\
	value [ebx]

void DrawCol(int c1, int y1)
{
	// Clip
    if (y1<0) y1 = 0;
	// Convert row to fixed point	
	int fr = row << FIX;
    // Is this segment visible?
    if (y1<fr)
    {
		// Convert 'y1' to integer
		int yi = y1 >> FIX;
	        
		// How many pixels
		int i = row - yi;
		
        // Interpolate color:  find color step
		int dc = ((c1 - pc) << FIX) / (py - y1);
        // Find initial color
        int cr = pc + ROUND;
        // Interpolate color over non visible portion
        if (py > row) cr += (dc * (py - fr)) >> FIX;

        // Draw pixels into offscreen buffer
		rowP = _FillCol(rowP, cr, dc, i);
        
        // Remember the last painted row
        row = yi;
    }
        
    // Remember last point
    pc = c1;
    py = y1;
}

void SectZ(int pz, int z, int x, int xerr, int dm)
{
	// Clamp values
	z = z&255;
	x = x&255;
	
	// Find array indices
	ushort w0 = (ushort)((z<<8) + x);
	ushort w1 = (ushort)((z<<8) + ((x+dm)&255));
	
    // Use data that is 'xerr' from
    //    (x, z) to (x + deltaX, z)

	// Interpolate height
	int yi = Surface[w0];
	yi = yi*256 + (Surface[w1] - yi) * xerr;

	// Interpolate color
	int ci = Colors[w0];
	ci = ci*256 + (Colors[w1] - ci) * xerr;

	// Project 'y'	
	int yp = (gY - yi) * gPrj[(pz<<9)] * 2 / 128 + (100<<FIX);
	
	// Paint the ray
	DrawCol(ci, yp);
}

int Depth = 60;
void Caster()
{
	// Initialize intersection server
	InitRayCaster(gZ, gX);
	
    // The bottom left pel of the off map
    char *pels = offMap + 199L*256;

	int ex, deltaX;
	// For each screen column (256 columns)
    for (int x=-128; x<128; x++)
    {
        // Start at bottom of screen, work our way up
        row  = 199;
        // Pointer to bottom row of memory pels
        rowP = pels++;

        // First point
        py = Point((gX/16), 1);
        pc = Color((gX/16), 1);
		// Project first y
		py = (gY - py) * gPrj[(1<<9)] * 2 / 128 + (100<<FIX);

        // Calculate end of ray on x-axis
        ex = (x * Depth * 2) * 256 / FOV;
        // X direction
        deltaX = (x < 0 ? -1 : 1);

		// Cast ray
		CastRayMajor(Depth*256*2, abs(ex), deltaX, Depth, SectZ);
    }
}


