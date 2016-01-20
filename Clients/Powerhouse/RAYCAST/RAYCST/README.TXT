HEIGHT MAPPING
--------------
Leo Sutic [100040,600]

DISCLAIMER/LEGAL STUFF/IT'S NOT MY FAULT DEPT.:
This is provided as an example of height mapping on the pc. Use it at your own
risk. If it destroys your hard drive, then that is your problem. Feel free to
modify it, use it in your own programs and distribute it.
All product names are trademarks or registered trademarks of their respective
holders.

DESCRIPTION
-----------
This is my height mapping engine. It is written in Borland C++ with some
assembler parts.

HOW IT WORKS
------------
The screen is divided into 320 columns. If you think about it, a ray that is
sent away from your eye through column 0 (the leftmost) won't be parallell to
a ray that goes through column 319. Now, what the mapper does is to trace all
320 rays as they come from the horizon into your eye.

_____________________________
| \   Visible area      /   |  Suppose that we followed the middle ray from the
|  \                   /    |  horizon to the user, and plotted the altitude
|   \                 /     |  on a diagram as we went along.
|    \    Looking    /      |
|     \   this way  /       |  The result would be something like this:
|      \     ^     /        |
|Rest   \    |    /         |  |                       ____
|of the  \   |   /          |  |      _               /####\__
|world    \  |  /           |  |     /#\   ____      /########\      _
|          \ | /            |  | _  /###\_/####\    /##########\_   /#\
|           \|/             |  |/#\/############\__/#############\_/###\______
|            o <- User      |  |##############################################
|___________________________|  |^--------------------------------------------^
			       Horizon                                     User

Now imagine that you cut out the filled parts of the diagram, and repeated the
process for ALL rays. Then looked at the diagrams head on. This is what the
height mapper does.

I know that this description isn't very good, so if you have any questions,
please contact me.

THE FUNCTIONS
-------------

Remember, RAYCAST.CPP uses a 1280 (320*4) degree system. 320 degrees is
a quarter of a circle.

void DrawSky (int Angle,int l)
Draws the correct column of the sky image. Angle is the angle of the column
you want to draw, and l is the screen x coordinate do draw it at.
DrawSky (320,160) would draw the sky as seen at 320 degrees at column 160.

void DrawIt(int startpos, int r, int xpos, int OldPos, unsigned char c)
Draws ONE "height line". This function is called only be DrawLine.
Startpos is the y coordinate to start drawing on, r is the distance to
that point, xpos is the x coordinate to draw it at, OldPos is the last
startpos value and c is the color.

void DrawLine (int xpos)
Height maps the column xpos. To create the entire image, call DrawLine in
a for loop like this:

for (int i=0;i<320;i++)
{
  DrawLine (i);
};
BlitBuffer();

void InitSinCos(void)
Initializes the sine and cosine tables used in DrawLine.

void InitStartPosTable(void)
Creates a lookup table for perspective transforms. Used only in DrawLine.

void DupCols(void)
Duplicates the columns on the screen. Column 1 will be identical to
column 0, column 3 to column 2 and so on.

void BlitBuffer (void)
Transfers Buffer to the screen.

void DrawCockpit(void)
Ovarlays the static instrument panel image on top of the landscape image.
Color 9 is transparent.

THE DATA FILES
--------------
The data files are standard Windows bitmaps. The mapper will skip the first
1078 bytes and read in the binary data. COLOR.BMP contains the color
information, and ALTITUDE.BMP contains the altitude data. SKY.BMP is the sky,
and RAYCAST.COL contains the palette. Both COLOR.BMP and ALTITUDE.BMP must be
256x256x256.

HOW I CREATED THE DATA FILES
----------------------------
Mostly, I used Fractint. First of all, I created a plasma cloud with a
resolution of 320x200x256. This was used for altitude data. Next, I did a
3D transform with "Light source after transformation" and Targa output. The
main thing here is to set the X Scale and Y Scale to 100 and to turn off
perspective transformations. If you do this, fractint will output an image
of the "world" as seen from above with shading etc. This image can then be used
as a COLOR.BMP.

Then I loaded the Targa file and the sky image into Photo Styler, and placed
them beside each other on the same image. Then I reduced the color depth to
8 bits (256 colors). This gave me a palette that I could use for both Color.BMP
and SKY.BMP. After that I splitted the image into two and saved them as 
COLOR.BMP and SKY.BMP. Ooops! Almost forgot... Before saving COLOR.BMP and 
ALTITUDE.BMP, I scaled them to 256x256.

OPTIMIZING
----------
Yes, yes, yes! I know it is a slow program! Ways to optimize it would be
1. Bresenham's algorithm is currently written in C (oh dear). I'm currently
   porting it to assembler.
2. When drawing every other column, use stosw instead of stosb, and skip
   DupCols().
3. Write it all in 386 assembler. Perhaps 386 Flat mode.
4. Disassemble Comanche and see how they do it.

VOXELS
------
What is a voxel? When you divide a plane in negative/positive parts you end
up with quadrants, right? Now, when you divide a cube the same way you'll get
octants. Imagine the following structure:

struct Voxel
{
  Voxel* Octants[8]
};

Every octant contains a pointer to another voxel, whose octants in turn
contains pointers to more voxels and so on. If an octant is empty, it doesn't
point to another voxel. This is repeated until every octant represents one
pixel.

From this description it is pretty obvious that any flight sim using voxels
would be terribly slow. The term "Voxel space" is actually misleading, since
Comanche doesn't use voxels (as far as I know).
______________________________________________________________________
Leo Sutic
CIS ID:[100040,600]
Internet: [100040.600@compuserve.com]
