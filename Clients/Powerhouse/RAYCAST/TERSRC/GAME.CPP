#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>
#include <time.h>
#include <dos.h>
#include "KeyCodes.h"
#include "Mouse.hpp"
#include "Screen.hpp"
#include "FixMath.hpp"
#include "Surface.hpp"
#include "Fract.hpp"
#include "Color.hpp"

#define random(x) (rand() % (x))

extern char *offMap;
extern int 	 gX, gY, gZ;
extern void  Caster();
extern int   HeightAt(int x, int z);

void Terrain()
{
    // Allocate the offscreen bitmap
    offMap = CreateBitmap(256, 200);
    if (!offMap) return;

    // Allocate the surface maps
    if (!ISurface()) return;

	// Build tables
	BuildProjectionTable();
                
    // Attempt to randomize
    gSeed = time(NULL) + clock();
        
    // Generate the fractal surface
    Fractify(0, 0, 256, 256);
	Smoothify();
	
    // Build obelisk
    for (int j=-3; j<4; j++)
    	for (int i=-3; i<4; i++)
        	Point(128+i, 128+j) = 240;
        
    // Generate the palette
    BuildPalette();
    // Colorize the surface (w/ sealevel  parameter)
    Colorize(80);

    // Initialize values
	int elev = 10 << (YFIX+FIX);
    gX = gY = gZ = 0;
   	gY = HeightAt(gX, gZ) + elev;

    // Do some display
    int  mx, my;
    char q = 0, clr=0;
    int  c, f = 0;
    char msg[80];
	
    clock_t clk = clock();
    while (!q)
    {
		// Count the frame
        f++;

        //------------Refresh the display-------------
        // Clear the offmap
        ClearScreen(254, 256, 200, offMap);
        // Draw the surface
		Caster();
        // Copy the offmap to the screen                
        Display(offMap, 256, 200);

        //------------Handle user input-------------
        MouseMove(&mx, &my);
        if (mx || my)
        {
            gZ -= my;
            gZ = gZ & 4095;
                        
            gX += mx;
            gX = gX & 4095;
        	gY = HeightAt(gX, gZ) + elev;
        }
        else if (kbhit())
        {
            c = getch();
            switch (c)
            {
                case 0:
                	c = getch();
                    switch (c)
                    {
                        case UP_ARROW_KEY:
                                gZ += 1;
                                break;

                        case DN_ARROW_KEY:
                                gZ -= 1;
                                break;
                                        
                        case LF_ARROW_KEY:
                                gX -= 1;
                                break;

                        case RT_ARROW_KEY:
                                gX += 1;
                            break;
                    }   
                    break;
                                
                case 'w':
                	elev += 8;
                	break;

                case 'x':
                	elev -= 8;
                	 break;

                case ESC_KEY:
                	q = 1;
               		break;
            }

            gZ &= 4095;
            gX &= 4095;
        	gY = HeightAt(gX, gZ) + elev;
        }                                   
    }
    clk = clock() - clk;
    
    _settextposition(1, 2);
    _settextcolor(255);
    sprintf(msg, "Frames: %ld,  Time: %5.2fs,  FPS: %5.2f\n", f, clk/(float)CLOCKS_PER_SEC, f * (float)CLOCKS_PER_SEC / clk);
    _outtext(msg);
}

int HeightAt(int x, int z)
{
	int wx = x >> 4;
	int wz = z >> 4;

	// Fractional portion of x
	int xf = x & 15;
	// Interpolate on bottom of polygon
    int y0 = Point(wx, wz);
	y0 = y0*16 + (Point(wx+1, wz)-y0) * xf;
	// Interpolate on top of polygon
	int y1 = Point(wx, wz+1);
	y1 = y1*16 + (Point(wx+1, wz+1)-y1) * xf;

	// Fractional portion of z
	int zf = z & 15;
	// Interpolate between the two points
	int y = y0 * 16 + (y1 - y0) * zf;

	return y;
}


