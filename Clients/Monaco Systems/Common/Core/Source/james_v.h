/*   The standard header file which containes generally usefull stuff */









#ifndef Loaded_standard_header

#define Loaded_standard_header





#include <stdio.h>
#include <stdlib.h>
#include <math.h>



/* definition for multi-dim arrays which get around C's brain dead way of

    handeling dynamically allocated multi-dim arrays */



/* NOT COMPATIBLE WITH SM DECLARATIONS, use spacemap.h definitions for that

   project   */



#ifndef IN_SPACEMAP

#define  ARRAY2(x,y,num_x) x+(y)*num_x

#define  ARRAY3(x,y,z,num_x,num_y)  (x)+(y)*num_x+(z)*num_x*num_y

#define  ARRAY4(x,y,z,v,num_x,num_y,num_z) (x)+(y)*num_x+(z)*num_x*num_y+(y)*num_x*num_y*num_z

#endif



/* a set of debugging definitions  */



#define DEBUG1  if (debug>0)

#define DEBUG2  if (debug>1)

#define DEBUG3  if (debug>2)

#define DEBUG4  if (debug>3)



#define SMALL 0.0001



/* a set of standard variables used by most programs  */



#ifdef IN_MAIN

int debug;

#endif

#ifndef IN_MAIN

extern int debug;

#endif

#endif



