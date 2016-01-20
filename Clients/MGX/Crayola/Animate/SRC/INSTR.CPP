/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#include "app.h"
#include "instr.h"


Instr::Instr(BYTE newInstr, int X, int Y, int newParam)                         // constructor
{
	next = 0;

	instruction = newInstr;
	x           = X;
	y           = Y;
	param       = newParam;
}











