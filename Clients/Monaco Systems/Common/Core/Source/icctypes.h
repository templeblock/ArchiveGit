#pragma once
#include "mcotypes.h"

#define MAX_LINEAR_HAND		(101)
#define MAX_TONE_HAND		(101)

typedef struct Ctype {
	char input_channels;
	char output_channels;
	char grid_points;
	char identity_matrix;	//is it identity matrix 1 == yes
	Fixed e00;
	Fixed e01;
	Fixed e02;
	Fixed e10;
	Fixed e11;
	Fixed e12;
	Fixed e20;
	Fixed e21;
	Fixed e22;
	u_int16	input_entries;
	u_int16 output_entries;
	long	offset;			//the lut table offset from beginning of the file
	char	*input_tables[6];	
	char	*output_tables[6];
} Ctype;	
