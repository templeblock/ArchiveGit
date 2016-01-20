/*
**  wt -- a 3d game engine
**
**  Copyright (C) 1994 by Chris Laurel
**  email:  claurel@mr.net
**  snail mail:  Chris Laurel, 5700 W Lake St #208,  St. Louis Park, MN  55416
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Watcom C specific bitmap scaling routines in inline assembly.
 * by Petteri Kangaslampi <pekanga@freeport.uwasa.fi>
*/



void WallSlice64(int count, unsigned char *tex_base, fixed tex_dy, Pixel *end,
                 fixed tex_y);
#pragma aux WallSlice64 = \
"t1:    mov     eax,edi"            \
"       shr     eax,26"             \
"       add     edi,edx"            \
"       mov     al,[ecx+eax]"       \
"       mov     [ebx+esi],al"       \
"       sub     ebx,320"            \
"       jns     t1"                 \
    parm    [ebx] [ecx] [edx] [esi] [edi] \
    modify exact [eax edi ebx];



void WallSlice(int count, Pixel *tex_base, fixed tex_dy, Pixel *end,
    fixed tex_y, int npix);
#pragma aux WallSlice = \
"       and     eax,3"              \
"       jz      t4"                 \
"       cmp     eax,1"              \
"       je      t3"                 \
"       cmp     eax,2"              \
"       je      t2"                 \
"t1:    mov     eax,edi"            \
"       shr     eax,25"             \
"       add     edi,edx"            \
"       mov     al,[ecx+eax]"       \
"       mov     [ebx+esi],al"       \
"       sub     ebx,320"            \
"t2:    mov     eax,edi"            \
"       shr     eax,25"             \
"       add     edi,edx"            \
"       mov     al,[ecx+eax]"       \
"       mov     [ebx+esi],al"       \
"       sub     ebx,320"            \
"t3:    mov     eax,edi"            \
"       shr     eax,25"             \
"       add     edi,edx"            \
"       mov     al,[ecx+eax]"       \
"       mov     [ebx+esi],al"       \
"       sub     ebx,320"            \
"t4:    mov     eax,edi"            \
"       shr     eax,25"             \
"       add     edi,edx"            \
"       mov     al,[ecx+eax]"       \
"       mov     [ebx+esi],al"       \
"       sub     ebx,320"            \
"       jns     t1" \
    parm [ebx] [ecx] [edx] [esi] [edi] [eax] \
    modify exact [eax ebx edi];



void FloorSlice128(Pixel *start, unsigned char *tex, fixed x, fixed y,
    fixed dx, fixed dy);
#pragma aux FloorSlice128 = \
"t1:    mov     ecx,edx"            \
"       and     eax,33554431"       \
"       and     ecx,4261412864"     \
"       add     ecx,eax"            \
"       shr     ecx,18"             \
"       add     edx,[esp]"          \
"       mov     cl,[ecx+edi]"       \
"       mov     [ebx],cl"           \
"       inc     ebx"                \
"       add     eax,esi"            \
"       cmp     byte ptr [ebx],255" \
"       je      t1"                 \
    parm [ebx] [edi] [eax] [edx] [esi] caller \
    modify [eax ebx ecx edx];



void FloorSlice(Pixel *start, unsigned char *tex, fixed x, fixed y,
    fixed dx, fixed dy);
#pragma aux FloorSlice = \
"t1:    mov     ecx,edx"            \
"       and     eax,67108863"       \
"       and     ecx,4227858432"     \
"       add     ecx,eax"            \
"       shr     ecx,20"             \
"       add     edx,[esp]"          \
"       mov     cl,[ecx+edi]"       \
"       mov     [ebx],cl"           \
"       inc     ebx"                \
"       add     eax,esi"            \
"       cmp     byte ptr [ebx],255" \
"       je      t1"                 \
    parm [ebx] [edi] [eax] [edx] [esi] caller \
    modify [eax ecx ebx edx];



inline void draw_wall_slice(Pixel *start, Pixel *end, unsigned char *tex_base,
			    fixed tex_y, fixed tex_dy, int fb_width,
                int tex_height, int npix)
{
    int         count = start - end;

    if (npix < 0)
        return;
    if ( tex_height == 64 )
    {
        tex_y <<= 10;
        tex_dy <<= 10;
        WallSlice64(count, tex_base, tex_dy, end, tex_y);

    }
    else
    {
     tex_y <<= 9;
     tex_dy <<= 9;
     WallSlice(count, tex_base, tex_dy, end, tex_y, npix);

    }
}




inline void draw_floor_slice(Pixel *start, unsigned char *tex,
			     fixed x, fixed y, fixed dx, fixed dy,
			     int tex_width)
{
    if ( *start != 255 )
        return;

    if ( tex_width == 128 )
    {
        y <<= 9;
        dy <<= 9;
        x <<= 2;
        dx <<= 2;
        FloorSlice128(start, tex, x, y, dx, dy);
    }
    else
    {

        y <<= 10;
        dy <<= 10;
        x <<= 4;
        dx <<= 4;
        FloorSlice(start, tex, x, y, dx, dy);

    }
}
