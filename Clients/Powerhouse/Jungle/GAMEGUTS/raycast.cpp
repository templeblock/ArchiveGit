
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <stdio.h> // for reading world maps
#include <stdlib.h> // for srand()
#include <memory.h> // for fmemset()
#include <malloc.h> // for fmalloc()
#include <math.h> // for fabs(), sin(), cos(), tan()
#include "d:\gamedev\tricks.gam\chap_19\graphics.h"	 // load our graphics library
// includeing SCREEN_WIDTH, SCREEN_HEIGHT
// sprite type and class w/ function Sprite_Init
// pcx_picture and pcx_picture_ptr types
// functions PCX_Init, PCX_Load, PCX_Show_Buffer, PCX_Gray_Bitmap, PCX_Delete
// RGB_color and RGB_color_ptr types
// the double_buffer pointer and function Init_Double_Buffer
// function Set_Palette_Register function

// D E F I N E S /////////////////////////////////////////////////////////////

#define UP		1
#define DOWN	2
#define LEFT	3
#define RIGHT	4
#define SPACE	5
#define ESCAPE	6
#define KEYDOWN 1

// these are for the door system

#define DOOR_DEAD			 0	   // the door is gone
#define DOOR_DYING			 1	   // the door is phasing
#define PROCESS_DOOR_DESTROY 0	   // tells the door engine to continue processing
#define START_DOOR_DESTROY	 1	   // telsl the door engine to begin
#define OVERBOARD			   52  // the closest a player can get to a wall
#define INTERSECTION_FOUND		1  // used by ray caster to flag an intersection
#define MAX_SCALE			  201  // maximum size and wall "sliver" can be
#define WINDOW_HEIGHT		  152  // height of the game view window
#define WINDOW_MIDDLE		   76  // the center or horizon of the view window
#define VERTICAL_SCALE		13312  // used to scale the "slivers" to get proper
								   // perspective and aspect ratio

// constants used to represent angles for the ray caster

#define ANGLE_0		0
#define ANGLE_1		5
#define ANGLE_2		10
#define ANGLE_4		20
#define ANGLE_5		25
#define ANGLE_6		30
#define ANGLE_15	80
#define ANGLE_30	160
#define ANGLE_45	240
#define ANGLE_60	320
#define ANGLE_90	480
#define ANGLE_135	720
#define ANGLE_180	960
#define ANGLE_225	1200
#define ANGLE_270	1440
#define ANGLE_315	1680
#define ANGLE_360	1920	 // note: the circle has been broken up into 1920
							 // sub-arcs

#define STEP_LENGTH 15		 // number of units player moves foward or backward


#define WORLD_ROWS	  64	 // number of rows in the game world
#define WORLD_COLUMNS 64	 // number of columns in the game world

#define CELL_X_SIZE	  64	 // size of a cell in the gamw world
#define CELL_Y_SIZE	  64

#define CELL_X_SIZE_FP	 6	 // log base 2 of 64 (used for quick division)
#define CELL_Y_SIZE_FP	 6

// size of overall game world

#define WORLD_X_SIZE  (WORLD_COLUMNS * CELL_X_SIZE)
#define WORLD_Y_SIZE  (WORLD_ROWS	 * CELL_Y_SIZE)

// P R O T O T Y P E S //////////////////////////////////////////////////////

void Render_Sliver(sprite_ptr sprite,long scale, int column);
void Create_Scale_Data(int scale, int *row);
void Build_Tables(void);
void Allocate_World(void);
int Load_World(char *file);
void Ray_Caster(long x,long y,long view_angle);
void Draw_Ground(void);
void Destroy_Door(int x_cell, int y_cell, int command);

// A S S E M B L Y	E X T E R N S ///////////////////////////////////////////

extern far Draw_Ground_32();   // renders the sky and ground
extern far Render_Buffer_32(); // copies the double buffer to the video buffer
extern far Render_Sliver_32(); // draws a textured "sliver"

// T Y P E S ////////////////////////////////////////////////////////////////

typedef long fixed;	   // fixed point is 32 bits

// G L O B A L S /////////////////////////////////////////////////////////////

// world map of nxn cells, each cell is 64x64 pixels

char far *world[WORLD_ROWS];	   // pointer to matrix of cells that make up
								   // world

float far *tan_table;			   // tangent tables used to compute initial
float far *inv_tan_table;		   // intersections with ray


float far *y_step;				   // x and y steps, used to find intersections
float far *x_step;				   // after initial one is found


float far *cos_table;			   // used to cacell out fishbowl effect

float far *inv_cos_table;		   // used to compute distances by calculating
float far *inv_sin_table;		   // the hypontenuse

int *scale_table[MAX_SCALE+1];	   // table with pre-computed scale indices

sprite object;					   // general sprite object used by everyone

pcx_picture walls_pcx,			   // holds the wall textures
		 controls_pcx,			   // holds the control panel at bottom of screen
		 intro_pcx;				   // holds the intro screen


// parmeter block used by assembly language sliver engine

char far *sliver_texture; // pointer to texture being rendered
int sliver_column;		  // index into texture i.e. which column of texture
int sliver_top;			  // starting Y position to render at
int sliver_scale;		  // overall height of sliver
int sliver_ray;			  // current ray being cast
int sliver_clip;		  // index into texture after clipping
int *scale_row;			  // row of scale value look up table to use

// the player

int player_x,				  // the players X position
	player_y,				  // the players Y position
	player_view_angle;		  // the current view angle of the player

// used for color FX

RGB_color red_glow;						  // red glowing objects

int red_glow_index = 254;				  // index of color register to glow

// variables to track status of a door

int door_state = DOOR_DEAD;				  // state of door
int door_clock = 0;						  // global door clock, counts
										  // number of frames to do door
										  // animation

// F U N C T I O N S /////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

#if 0

void Render_Sliver(sprite_ptr sprite,long scale, int column)
{

// this function will scale a single sliver of texture data.  it uses fixed point
// numbers.

char far *work_sprite;

int work_offset=0,offset,x,y,scale_int;

unsigned char data;

fixed scale_index,scale_step;

scale_int = scale;
scale = (scale<<8);

scale_index = 0;
scale_step = (fixed)(((fixed)64) << 16) / scale;

// alias a pointer to sprite for ease of access

work_sprite = sprite->frames[sprite->curr_frame];

// compute offset of sprite in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<scale_int; y++)
	{

	double_buffer[offset] = work_sprite[work_offset+column];

	scale_index+=scale_step;

	offset		+= SCREEN_WIDTH;
	work_offset =  ((scale_index & 0xff00)>>2);

	} // end for y

} // end Draw_Sliver

#endif

///////////////////////////////////////////////////////////////////////////////

#if 0

Render_Sliver(sprite_ptr sprite,int scale, int column)
{
// this is yet another version of the sliver scaler, however it uses look up
// tables with pre-computed scale indices.	in the end I converted this to
// assembly for speed

char far *work_sprite;
int far *row;

int work_offset=0,offset,y,scale_off;

unsigned char data;

// alias proper data row

row = scale_table[scale];

if (scale>(WINDOW_HEIGHT-1))
   {
   scale_off = (scale-(WINDOW_HEIGHT-1)) >> 1;
   scale=(WINDOW_HEIGHT-1);
   sprite->y = 0;
   }

// alias a pointer to sprite for ease of access

work_sprite = sprite->frames[sprite->curr_frame];

// compute offset of sprite in video buffer

offset = (sprite->y << 8) + (sprite->y << 6) + sprite->x;

for (y=0; y<scale; y++)
	{

	double_buffer[offset] = work_sprite[work_offset+column];

	offset		+= SCREEN_WIDTH;
	work_offset =  row[y+scale_off];

	} // end for y

} // end Draw_Sliver

#endif


///////////////////////////////////////////////////////////////////////////////

void Create_Scale_Data(int scale, int *row)
{

// this function synthesizes the scaling of a texture sliver to all possible
// sizes and creates a huge look up table of the data.

int y;

float y_scale_index=0,
	  y_scale_step;

// compute scale step or number of source pixels to map to destination/cycle

y_scale_step = (float)64/(float)scale;

y_scale_index+=y_scale_step;

for (y=0; y<scale; y++)
	{
	// place data into proper array position for later use

	row[y] = ((int)(y_scale_index+.5)) * CELL_X_SIZE;

	// test if we slightly went overboard

	if	(row[y] > 63*CELL_X_SIZE) row[y] = 63*CELL_X_SIZE;

	// next index please

	y_scale_index+=y_scale_step;

	} // end for y

} // end Create_Scale_Data

///////////////////////////////////////////////////////////////////////////////

void Build_Tables(void)
{

// this function builds all the look up tables for the system

int ang,scale;
float rad_angle;

// allocate memory for all look up tables

// tangent tables equivalent to slopes

tan_table	  = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );
inv_tan_table = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );

// step tables used to find next intersections, equivalent to slopes
// times width and height of cell

y_step		  = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );
x_step		  = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );


// cos table used to fix view distortion caused by caused by radial projection

cos_table	  = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );


// 1/cos and 1/sin tables used to compute distance of intersection very
// quickly

inv_cos_table = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );
inv_sin_table = (float far *)_fmalloc(sizeof(float) * (ANGLE_360+1) );

// create the lookup tables for the scaler
// there have the form of an array of pointers, where each pointer points
// another another array of data where the 'data' are the scale indices

for (scale=0; scale<=MAX_SCALE; scale++)
	{

	scale_table[scale] = (int *)malloc(scale*sizeof(int)+1);

	} // end for scale

// create tables, sit back for a sec!

for (ang=ANGLE_0; ang<=ANGLE_360; ang++)
	{

	rad_angle = (float)((3.272e-4) + ang * 2*3.141592654/ANGLE_360);

	tan_table[ang]	   = (float)tan(rad_angle);
	inv_tan_table[ang] = (float)(1/tan_table[ang]);


	// tangent has the incorrect signs in all quadrants except 1, so
	// manually fix the signs of each quadrant since the tangent is
	// equivalent to the slope of a line and if the tangent is wrong
	// then the ray that is case will be wrong

	if (ang>=ANGLE_0 && ang<ANGLE_180)
	   {
	   y_step[ang]		  = (float)(fabs(tan_table[ang]	 * CELL_Y_SIZE));
	   }
	else
	   y_step[ang]		  = (float)(-fabs(tan_table[ang] * CELL_Y_SIZE));

	if (ang>=ANGLE_90 && ang<ANGLE_270)
	   {
	   x_step[ang]		  = (float)(-fabs(inv_tan_table[ang] * CELL_X_SIZE));
	   }
	else
	   {
	   x_step[ang]		  = (float)(fabs(inv_tan_table[ang]	 * CELL_X_SIZE));
	   }



	// create the sin and cosine tables to copute distances

	inv_cos_table[ang] = (float)(1/cos(rad_angle));
	inv_sin_table[ang] = (float)(1/sin(rad_angle));

	} // end for ang

// create view filter table.  There is a cosine wave modulated on top of
// the view distance as a side effect of casting from a fixed point.
// to cancell this effect out, we multiple by the inverse of the cosine
// and the result is the proper scale.	Without this we would see a
// fishbowl effect, which might be desired in some cases?

for (ang=-ANGLE_30; ang<=ANGLE_30; ang++)
	{

	rad_angle = (float)((3.272e-4) + ang * 2*3.141592654/ANGLE_360);

	cos_table[ang+ANGLE_30] = (float)(VERTICAL_SCALE/cos(rad_angle));

	} // end for

// build the scaler table.	This table holds MAX_SCALE different arrays.  Each
// array consists of the pre-computed indices for an object to be scaled

for (scale=1; scale<=MAX_SCALE; scale++)
	{

	// create the indices for this scale

	Create_Scale_Data(scale, (int *)scale_table[scale]);

	} // end for scale

} // end Build_Tables

/////////////////////////////////////////////////////////////////////////////

void Allocate_World(void)
{
// this function allocates the memory for the world

int index;

// allocate each row

for (index=0; index<WORLD_ROWS; index++)
	{
	world[index] = (char far *)_fmalloc(WORLD_COLUMNS+1);

	} // end for index

} // end Allocate_World


////////////////////////////////////////////////////////////////////////////////

int Load_World(char *file)
{
// this function opens the input file and loads the world data from it

FILE  *fp;
int row,column;
char ch;

// open the file

if (!(fp = fopen(file,"r")))
   return(0);

// load in the data

for (row=0; row<WORLD_ROWS; row++)
	{
	// load in the next row

	for (column=0; column<WORLD_COLUMNS; column++)
		{

		while((ch = getc(fp))==10){} // filter out CR

		// translate character to integer

		if (ch == ' ')
		   ch=0;
		else
		   ch = ch - '0';

		// insert data into world

		world[(WORLD_ROWS-1) - row][column] = ch;

		} // end for column

	// process the row

	} // end for row

// close the file

fclose(fp);

return(1);

} // end Load_World

/////////////////////////////////////////////////////////////////////////////

void Ray_Caster(long x,long y,long view_angle)
{
// This is the heart of the system.	 it casts out 320 rays and builds the
// 3-D image from their intersections with the walls.  It was derived from
// the previous version used in "RAY.C", however, it has been extremely
// optimized for speed by the use of many more lookup tables and fixed
// point math

int

cell_x,		  // the current cell that the ray is in
cell_y,
ray,		  // the current ray being cast 0-320
casting=2,	  // tracks the progress of the X and Y component of the ray
x_hit_type,	  // records the block that was intersected, used to figure
y_hit_type,	  // out which texture to use
x_bound,	  // the next vertical and horizontal intersection point
y_bound,
next_y_cell,  // used to figure out the quadrant of the ray
next_x_cell,
xray=0,		  // tracks the progress of a ray looking for Y interesctions
yray=0,		  // tracks the progress of a ray looking for X interesctions

x_delta,	  // the amount needed to move to get to the next cell
y_delta,	  // position
xb_save,
yb_save,
xi_save,	  // used to save exact x and y intersection points
yi_save,
scale;


long
	 cast=0,
	 dist_x,  // the distance of the x and y ray intersections from
	 dist_y;  // the viewpoint

float xi,	  // used to track the x and y intersections
	  yi;

// S E C T I O N  1 /////////////////////////////////////////////////////////v

// initialization

// compute starting angle from player.	Field of view is 60 degrees, so
// subtract half of that current view angle

if ( (view_angle-=ANGLE_30) < 0)
   {
   view_angle=ANGLE_360 + view_angle;
   } // end if

// loop through all 320 rays

for (ray=319; ray>=0; ray--)
	{

// S E C T I O N  2 /////////////////////////////////////////////////////////

	// compute first x intersection

	// need to know which half plane we are casting from relative to Y axis

	if (view_angle >= ANGLE_0 && view_angle < ANGLE_180)
	   {

	   // compute first horizontal line that could be intersected with ray
	   // note: it will be above player

	   y_bound = (CELL_Y_SIZE + ((unsigned)y & 0xffc0));

	   // compute delta to get to next horizontal line

	   y_delta = CELL_Y_SIZE;

	   // based on first possible horizontal intersection line, compute X
	   // intercept, so that casting can begin

	   xi = inv_tan_table[view_angle] * (y_bound - y) + x;

	   // set cell delta

	   next_y_cell = 0;

	   } // end if upper half plane
	else
	   {
	   // compute first horizontal line that could be intersected with ray
	   // note: it will be below player

	   y_bound = (int)(y & 0xffc0);

	   // compute delta to get to next horizontal line

	   y_delta = -CELL_Y_SIZE;

	   // based on first possible horizontal intersection line, compute X
	   // intercept, so that casting can begin

	   xi = inv_tan_table[view_angle] * (y_bound - y) + x;

	   // set cell delta

	   next_y_cell = -1;

	   } // end else lower half plane

// S E C T I O N  3 /////////////////////////////////////////////////////////

	// compute first y intersection

	// need to know which half plane we are casting from relative to X axis

	if (view_angle < ANGLE_90 || view_angle >= ANGLE_270)
	   {

	   // compute first vertical line that could be intersected with ray
	   // note: it will be to the right of player

	   x_bound = (int)(CELL_X_SIZE + (x & 0xffc0));

	   // compute delta to get to next vertical line

	   x_delta = CELL_X_SIZE;

	   // based on first possible vertical intersection line, compute Y
	   // intercept, so that casting can begin

	   yi = tan_table[view_angle] * (x_bound - x) + y;

	   // set cell delta

	   next_x_cell = 0;

	   } // end if right half plane
	else
	   {

	   // compute first vertical line that could be intersected with ray
	   // note: it will be to the left of player

	   x_bound = (int)(x & 0xffc0);

	   // compute delta to get to next vertical line

	   x_delta = -CELL_X_SIZE;

	   // based on first possible vertical intersection line, compute Y
	   // intercept, so that casting can begin

	   yi = tan_table[view_angle] * (x_bound - x) + y;

	   // set cell delta

	   next_x_cell = -1;

	   } // end else right half plane


// begin cast

	casting		  = 2;				  // two rays to cast simultaneously
	xray=yray	  = 0;				  // reset intersection flags


// S E C T I O N  4 /////////////////////////////////////////////////////////

	while(casting)
		 {

		 // continue casting each ray in parallel

		 if (xray!=INTERSECTION_FOUND)
			{

			// compute current map position to inspect

			cell_x = ( (x_bound+next_x_cell) >> CELL_X_SIZE_FP);

			cell_y = (int)yi;
			cell_y>>=CELL_Y_SIZE_FP;

			// test if there is a block where the current x ray is intersecting

			if ((x_hit_type = world[cell_y][cell_x])!=0)
			   {
			   // compute distance

			   dist_x  = (long)((yi - y) * inv_sin_table[view_angle]);
			   yi_save = (int)yi;
			   xb_save = x_bound;

			   // terminate X casting

			   xray = INTERSECTION_FOUND;
			   casting--;

			   } // end if a hit
			else
			   {

			   // compute next Y intercept

			   yi += y_step[view_angle];

			   // find next possible x intercept point

			   x_bound += x_delta;

			   } // end else

			} // end if x ray has intersected

// S E C T I O N  5 /////////////////////////////////////////////////////////

		 if (yray!=INTERSECTION_FOUND)
			{

			// compute current map position to inspect

			cell_x = (int)xi;
			cell_x>>=CELL_X_SIZE_FP;

			cell_y = ( (y_bound + next_y_cell) >> CELL_Y_SIZE_FP);

			// test if there is a block where the current y ray is intersecting

			if ((y_hit_type = world[cell_y][cell_x])!=0)
			   {

			   // compute distance

			   dist_y  = (long)((xi - x) * inv_cos_table[view_angle]);
			   xi_save = (int)xi;
			   yb_save = y_bound;

			   yray = INTERSECTION_FOUND;
			   casting--;

			   } // end if a hit
			else
			   {

			   // terminate Y casting

			   xi += x_step[view_angle];

			   // compute next possible y intercept

			   y_bound += y_delta;

			   } // end else

			} // end if y ray has intersected

		 } // end while not done

// S E C T I O N  6 /////////////////////////////////////////////////////////

	// at this point, we know that the ray has succesfully hit both a
	// vertical wall and a horizontal wall, so we need to see which one
	// was closer and then render it

	if (dist_x < dist_y)
	   {

	   // there was a vertical wall closer than the horizontal

	   // compute actual scale and multiply by view filter so that spherical
	   // distortion is cancelled

	   scale = (int)(cos_table[ray]/dist_x);

	   // clip wall sliver against view port

	   if (scale>(MAX_SCALE-1)) scale=(MAX_SCALE-1);

	   scale_row	  = scale_table[scale-1];

	   if (scale>(WINDOW_HEIGHT-1))
		  {
		  sliver_clip = (scale-(WINDOW_HEIGHT-1)) >> 1;
		  scale=(WINDOW_HEIGHT-1);
		  }
	   else
		  sliver_clip = 0;

	   sliver_scale	  = scale-1;

	   // set up parameters for assembly language

	   sliver_texture = object.frames[x_hit_type];
	   sliver_column  = (yi_save & 0x003f);
	   sliver_top	  = WINDOW_MIDDLE - (scale >> 1);
	   sliver_ray	  = ray;

	   // render the sliver in assembly

	   Render_Sliver_32();

	   } // end if

	else // must of hit a horizontal wall first
	   {

	   // there was a vertical wall closer than the horizontal

	   // compute actual scale and multiply by view filter so that spherical
	   // distortion is cancelled

	   scale = (int)(cos_table[ray]/dist_y);

	   // do clipping again

	   if (scale>(MAX_SCALE-1)) scale=(MAX_SCALE-1);

	   scale_row	  = scale_table[scale-1];

	   if (scale>(WINDOW_HEIGHT-1))
		  {
		  sliver_clip = (scale-(WINDOW_HEIGHT-1)) >> 1;
		  scale=(WINDOW_HEIGHT-1);
		  }
	   else
		  sliver_clip = 0;

	   sliver_scale	  = scale-1;

	   // set up parameters for assembly language

	   sliver_texture = object.frames[y_hit_type+1];
	   sliver_column  = (xi_save & 0x003f);
	   sliver_top	  = WINDOW_MIDDLE - (scale >> 1);
	   sliver_ray	  = ray;

	   // render the sliver

	   Render_Sliver_32();

	   } // end else

// S E C T I O N  7 /////////////////////////////////////////////////////////

	// cast next ray

	// test if view angle need to wrap around

	if (++view_angle>=ANGLE_360)
	   {

	   view_angle=0;

	   } // end if

	} // end for ray

} // end Ray_Caster

//////////////////////////////////////////////////////////////////////////////

void Draw_Ground(void)
{
// clear the screen and draw the ground using 16 bit instructions.
// the sky is black and the ground is grey using the standard Dpaint pallete

_asm
   {
   push di						; save di
   cld							; set direction to foward
   les di, double_buffer		; get address of double buffer
   xor ax,ax					; zero out ax
   mov cx,320*WINDOW_MIDDLE/2	; lets fill the top with black
   rep stosw					; do it

   mov ax,0x1E1E				; lets fill the bottom with grey
   mov cx,320*WINDOW_MIDDLE/2
   rep stosw					; do it

   pop di						; restore registers and blaze
   } // end asm

} // end Draw_Ground

///////////////////////////////////////////////////////////////////////////////

void Destroy_Door(int x_cell, int y_cell, int command)
{
// this function is called every frame when a door is being destroyed.
// Basically it cycles a color on the door and makes it glow red as if it
// were energizing.	 the function does this a specific number of times and
// then turns itself off and takes the door out of the world

static int door_x_cell,	  // used to hold the position of the door
		   door_y_cell;

// test what is happening i.e. door starting destruction or
// continuing

if (command==START_DOOR_DESTROY)
   {

   // play opening sound

   // reset glow color

   red_glow.red	  = 0;
   red_glow.green = 0;
   red_glow.blue  = 0;

   Set_Palette_Register(red_glow_index,(RGB_color_ptr)&red_glow);

   // set door sequence in motion and number of clicks

   door_state = DOOR_DYING;
   door_clock = 30;

   // remember where door is so we can make it disapear

   door_x_cell = x_cell;
   door_y_cell = y_cell;

   } // end start up door
else
   {

   // increase intensity of glow

   red_glow.red+=2;

   Set_Palette_Register(red_glow_index,(RGB_color_ptr)&red_glow);

   // test if we are done with door

   if (--door_clock < 0)
	  {

	  // reset state of door

	  door_state = DOOR_DEAD;

	  // say bye-bye to door

	  world[door_y_cell][door_x_cell] = 0;

	  // reset pallete register

	  red_glow.red=0;

	  Set_Palette_Register(red_glow_index,(RGB_color_ptr)&red_glow);

	  } // end if

   } // end else door must be dying

} // end Destroy_Door

// M A I N ///////////////////////////////////////////////////////////////////

void main(void)
{

// S E C T I O N   1 /////////////////////////////////////////////////////////



int done=0,			   // flag to exit game
	x_cell,			   // celluar position of player in game grid
	y_cell,
	x_sub_cell,		   // position of player within a single cell
	y_sub_cell,
	door_x,			   // cell position of door player is trying to open
	door_y;

float dx,dy;


// S E C T I O N   2 /////////////////////////////////////////////////////////

// seed random number genrerator

srand(13);

// load the intro screen for a second and bore everyone

PCX_Init((pcx_picture_ptr)&intro_pcx);
PCX_Load("warintr2.pcx", (pcx_picture_ptr)&intro_pcx,1);
PCX_Show_Buffer((pcx_picture_ptr)&intro_pcx);

Allocate_World();

// build all the lookup tables

Build_Tables();

Load_World("raymap.dat");

// initialize the double buffer

Init_Double_Buffer();

sprite_width  = 64;
sprite_height = 64;

// load up the textures

PCX_Init((pcx_picture_ptr)&walls_pcx);
PCX_Load("walltext.pcx", (pcx_picture_ptr)&walls_pcx,1);
Sprite_Init((sprite_ptr)&object,0,0,0,0,0,0);


// grab a blank

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,0,0,0);

// grab first wall

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,1,0,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,2,1,0);

// grab second wall

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,3,2,0);
PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,4,3,0);

// grab third wall

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,5,2,1);
PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,6,3,1);


// grab doors

PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,7,0,2);
PCX_Grap_Bitmap((pcx_picture_ptr)&walls_pcx,(sprite_ptr)&object,8,1,2);

// dont need textures anymore

PCX_Delete((pcx_picture_ptr)&walls_pcx);

// load up the control panel

PCX_Init((pcx_picture_ptr)&controls_pcx);

PCX_Load("controls.pcx", (pcx_picture_ptr)&controls_pcx,1);

PCX_Show_Buffer((pcx_picture_ptr)&controls_pcx);

PCX_Delete((pcx_picture_ptr)&controls_pcx);

PCX_Delete((pcx_picture_ptr)&intro_pcx);

// initialize the generic sprite we will use to access the textures with

object.curr_frame = 0;
object.x		  = 0;
object.y		  = 0;

// position the player somewhere interseting

player_x=53*64+25;
player_y=14*64+25;
player_view_angle=ANGLE_60;

// render initial view

// clear the double buffer

_fmemset(double_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT + 1);

red_glow.red   = 0;
red_glow.green = 0;
red_glow.blue  = 0;

Set_Palette_Register(red_glow_index,(RGB_color_ptr)&red_glow);

// S E C T I O N   3 /////////////////////////////////////////////////////////

// render the initial view

Draw_Ground_32();

Ray_Caster(player_x,player_y,player_view_angle);

Render_Buffer_32();

// main event loop, wait for user to press ESC to quit

while(!done)
	 {

// S E C T I O N   4 /////////////////////////////////////////////////////////

	 // is player moving due to input ?

	 if ( KEYDOWN )
		{

		// reset deltas

		dx=dy=0;

		// what is user doing

		if ( RIGHT )
		   {
		   // rotate player right

		   if ((player_view_angle-=ANGLE_6)<ANGLE_0)
			  player_view_angle=ANGLE_360;

		   } // end if right
		else
		if ( LEFT )
		   {
		   // rotate player to left

		   if ((player_view_angle+=ANGLE_6)>=ANGLE_360)
			  player_view_angle=ANGLE_0;

		   } // end if left

		if ( UP )
		   {
		   // move player along view vector foward

		   dx=(float)(cos(6.28*player_view_angle/ANGLE_360)*STEP_LENGTH);
		   dy=(float)(sin(6.28*player_view_angle/ANGLE_360)*STEP_LENGTH);

		   } // end if up
		else
		if ( DOWN )
		   {
		   // move player along view vector backward

		   dx=(float)(-cos(6.28*player_view_angle/ANGLE_360)*STEP_LENGTH);
		   dy=(float)(-sin(6.28*player_view_angle/ANGLE_360)*STEP_LENGTH);

		   } // end if down

// S E C T I O N   5 /////////////////////////////////////////////////////////

		// move player

		player_x= (int)((float)player_x+dx);
		player_y= (int)((float)player_y+dy);

		// test if user has bumped into a wall i.e. test if there
		// is a cell within the direction of motion, if so back up !

		// compute cell position

		x_cell = player_x/CELL_X_SIZE;
		y_cell = player_y/CELL_Y_SIZE;

		// compute position relative to cell

		x_sub_cell = player_x % CELL_X_SIZE;
		y_sub_cell = player_y % CELL_Y_SIZE;


		// resolve motion into it's x and y components

		if (dx>0 )
		   {
		   // moving right

		   if ( (world[y_cell][x_cell+1] != 0)	&&
				(x_sub_cell > (CELL_X_SIZE-OVERBOARD ) ) )
				{
				// back player up amount he steped over the line

				player_x-= (x_sub_cell-(CELL_X_SIZE-OVERBOARD ));

				} // end if need to back up
		   }
		else
		   {
		   // moving left

		   if ( (world[y_cell][x_cell-1] != 0)	&&
				(x_sub_cell < (OVERBOARD) ) )
				{
				// back player up amount he steped over the line

				player_x+= (OVERBOARD-x_sub_cell) ;

				} // end if need to back up

		   } // end else

		if (dy>0 )
		   {
		   // moving up

		   if ( (world[(y_cell+1)][x_cell] != 0)  &&
				(y_sub_cell > (CELL_Y_SIZE-OVERBOARD ) ) )
				{
				// back player up amount he steped over the line

				player_y-= (y_sub_cell-(CELL_Y_SIZE-OVERBOARD ));

				} // end if need to back up
		   }
		else
		   {
		   // moving down

		   if ( (world[(y_cell-1)][x_cell] != 0)  &&
				(y_sub_cell < (OVERBOARD) ) )
				{
				// back player up amount he steped over the line

				player_y+= (OVERBOARD-y_sub_cell);

				} // end if need to back up

		   } // end else

		}  // end if player in motion


// S E C T I O N   6 /////////////////////////////////////////////////////////

		// test non-motion keys

		if ( ESCAPE )
		   {
		   done=1;

		   } // end if user is exiting
		else
		if ( SPACE ) // trying to open a door
		   {
		   // test if there is a door in front of player

		   // project a "feeler" 3 steps in front of player and test for a door

		   door_x = (int)(player_x + cos(6.28*player_view_angle/ANGLE_360)*6*STEP_LENGTH);
		   door_y = (int)(player_y + sin(6.28*player_view_angle/ANGLE_360)*6*STEP_LENGTH);

		   // compute cell position

		   x_cell = door_x/CELL_X_SIZE;
		   y_cell = door_y/CELL_Y_SIZE;

		   // test for door

		   if (world[y_cell][x_cell] == 7 || world[y_cell][x_cell] == 8)
			  {

			  // make door disapear by starting process

			  Destroy_Door(x_cell,y_cell, START_DOOR_DESTROY);

			  } // end if a door was found

		   } // end if trying to open a door

		// call all responder and temporal functions that occur each frame

		Destroy_Door(0,0,PROCESS_DOOR_DESTROY);

// S E C T I O N   7 /////////////////////////////////////////////////////////

		// clear the double buffer and render the ground and ceiling

		Draw_Ground_32();

		// render the view

		Ray_Caster(player_x,player_y,player_view_angle);

		// do all rendering that goes on top of 3-D view here

		// show the double buffer

		Render_Buffer_32();

		// test if it's time for a random sound

	 } //end while

// S E C T I O N   8 /////////////////////////////////////////////////////////

// let's melt with some style

} // end main
