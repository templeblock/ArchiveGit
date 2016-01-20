//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "routines.h"
#include "swirltab.h"

void sample_table( LPBYTE lpSampleBuffer, int count, int brush_size );
static void get_template( LPBYTE *, int * );

//////////////////////////////////////////////////////////////////////////////
void sample_table( LPBYTE lpSampleBuffer, int count, int brush_size )
//////////////////////////////////////////////////////////////////////////////
{
    LFIXED yrate, xrate, xoffset, yoffset;
    int row_count = 0, buf_offset = 0, row = 0, col = 0;
    int template_dim;
    LPBYTE lpTemplateUsed = NULL;

    get_template( &lpTemplateUsed, &template_dim );

    // compute x and y sample rates
    xrate = yrate = FGET( template_dim, brush_size );

    yoffset = ( (long)yrate >> 1 );

    while( buf_offset < count )         // while there is room in the buffer
    {
#ifdef WIN32
    	row = WHOLE( yoffset );        // current row to sample
	    yoffset += yrate;
        xoffset = ( (long)xrate >> 1 );       // reset column offset
    	col = WHOLE( xoffset );        // first element to sample in row
#else
    	row = HIWORD( yoffset );        // current row to sample
	    yoffset += yrate;
        xoffset = ( (long)xrate >> 1 );       // reset column offset
    	col = HIWORD( xoffset );        // first element to sample in row
#endif
        while( col < template_dim )
        {
            *( lpSampleBuffer + buf_offset ) = *( lpTemplateUsed + row * template_dim + col );

            buf_offset++;

	        xoffset += xrate;       
#ifdef WIN32
        	col = WHOLE( xoffset );    // next column to sample
#else
        	col = HIWORD( xoffset );    // next column to sample
#endif
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
static void get_template( LPBYTE *lpTemplate, int *template_dim )
//////////////////////////////////////////////////////////////////////////////
{
    if( Retouch.BrushSize <= 7 )
    {
        *lpTemplate = &SwirlTemplate7[0][0];
        *template_dim = 7;
    }
    else
    if( Retouch.BrushSize <= 9 )
    {
        *lpTemplate = &SwirlTemplate9[0][0];
        *template_dim = 9;
    }
    else
    if( Retouch.BrushSize <= 11 )
    {
        *lpTemplate = &SwirlTemplate11[0][0];
        *template_dim = 11;
    }
    else
    if( Retouch.BrushSize <= 13 )
    {
        *lpTemplate = &SwirlTemplate13[0][0];
        *template_dim = 13;
    }
    else
    if( Retouch.BrushSize <= 15 )
    {
        *lpTemplate = &SwirlTemplate15[0][0];
        *template_dim = 15;
    }
    else
    if( Retouch.BrushSize <= 19 )
    {
        *lpTemplate = &SwirlTemplate19[0][0];
        *template_dim = 19;
    }
    else
    if( Retouch.BrushSize <= 29 )
    {
        *lpTemplate = &SwirlTemplate29[0][0];
        *template_dim = 29;
    }
    else
    if( Retouch.BrushSize <= 39 )
    {
        *lpTemplate = &SwirlTemplate39[0][0];
        *template_dim = 39;
    }
    else
    {
        *lpTemplate = &SwirlTemplate51[0][0];
        *template_dim = 51;
    }
}



