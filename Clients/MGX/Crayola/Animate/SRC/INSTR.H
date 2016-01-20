/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/


class Instr                         // instructions chained together
{   friend class CAnimateObj;

    Instr   *next;                  // next one in the chain of Aobject instructions

    BYTE    instruction;            // what is the object supposed to do
    int     x;                      // X, Y point to head for next
    int     y;
    int     param;                  // pause period

    Instr(BYTE newInstr, int X, int Y, int newParam);
};


