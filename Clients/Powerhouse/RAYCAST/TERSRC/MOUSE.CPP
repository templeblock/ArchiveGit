#include <dos.h>

void MouseMove(int *x, int *y)
{
    union REGS r;
    struct SREGS s;

    s.ds = s.es = s.fs = s.gs = FP_SEG( &s );

    r.h.ah = 0x00;
    r.h.al = 0x0B;
    int386x( 0x33, &r, &r, &s );

    *x = (short)r.w.cx;
    *y = (short)r.w.dx;  
}


