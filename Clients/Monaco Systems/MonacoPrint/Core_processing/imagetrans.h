#ifndef _IMAGE_TRANS_H
#define _IMAGE_TRANS_H

#include <math.h>
#include "mcotypes.h"

void image_simple_size(unsigned char* image, long src_w, long src_l, long des_w, long des_l, 
	long comp);

void image_simple_size_non(unsigned char* insrc, unsigned char* indest, 
	long src_w, long src_l, long des_w, long des_l, long comp);

void find_selection(unsigned char* image, long width, long length, Rect* select, long comp);

//void imagerotate(unsigned char* src, long slen, long swid,
//				unsigned char* des, Rect *r, double ang, long comp);
				
void imagerotate(unsigned char* src, long slen, long swid,
				unsigned char* des, long dh, long dv, long dlen, long dwid,
				double ang, long comp);				
#endif