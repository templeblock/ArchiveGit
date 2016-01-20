// Source file: select.c

#define SL_BLOCK        1		
#define SL_BOXHANDLES   2		// Draw a box with handles
#define SL_GRID	        3		// Draw a grid
#define SL_LINE	        4		// Draw a line

#define SL_BOX          5		// Draw a rectanglar frame
#define SL_ELLIPSE      6		// Draw an elliptical frame
#define SL_HTRIANGLE    7		// Draw a horizontal triangle
#define SL_VTRIANGLE    8		// Draw a vertical triangle
#define SL_HEXAGON	    9		// Draw a six-sided box
#define SL_OCTAGON	    10      // Draw an eight-sided box
#define SL_HSEMICIRCLE  11	    // Draw a left/right-half semi-circle
#define SL_VSEMICIRCLE  12	    // Draw a top/bottom-half semi-circle
#define SL_CUBE         13		// Draw a cube
#define SL_CYLINDER     14		// Draw a cylinder
#define SL_PYRAMID      15		// Draw a pyramid
#define SL_WEDGE        16		// Draw an extruded triangle
#define SL_HCONE        17		// Draw a horizontal cone
#define SL_VCONE        18		// Draw a vertical cone
#define SL_PCUBE        19		// Draw a perspective cube
#define SL_PCYLINDER    20	    // Draw a perspective cylinder
#define SL_PPYRAMID     21		// Draw a perspective pyramid
#define SL_PWEDGE       22		// Draw a perspective extruded triangle

#define SL_TYPE    0x00FF	    // Mask out everything but the type flags
#define SL_SPECIAL 0x0100	    // special flag, draw rect on StartSelection
#define SL_NOLIMIT 0x0200   	// flag to allow user to go outside the image

#define CLOSENESS  3

