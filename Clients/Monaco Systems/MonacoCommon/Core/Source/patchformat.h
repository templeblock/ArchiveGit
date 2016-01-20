#pragma once
// The patchformat used to interchange data
// patchformat.h

typedef struct PatchFormat{
	long 							patchType;
	long 							numComp;			
	long 							numCube;
	long 							numLinear;				
	long							*steps;	
	double							*blacks;					
	double							*cmy;						
	double							*linear;					
	double							*data;					
	double							*ldata;	
	char							name[100];					
} PatchFormat;