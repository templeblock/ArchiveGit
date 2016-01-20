#include "imagetrans.h"

//overwrite version
void image_simple_size(unsigned char* image, long src_w, long src_l, long des_w, long des_l, 
	long comp)
{
	long i, j, k;
	double scale_l, scale_w;
	unsigned char* src, *dest;
	long src_l_i, src_w_i;	//index of src
	long src_row_byte, dest_row_byte;
	long src_start, dest_start, src_row_start, dest_row_start;
	long max_l, max_w;
	long dest_total, gap;
	
	src_row_byte = comp*src_w;
	dest_row_byte = comp*des_w;
	scale_l = (double)src_l/(double)des_l; 
	scale_w = (double)src_w/(double)des_w; 
	max_l = src_l;
	if(des_l > max_l)
		max_l = des_l;
	max_w = src_w;
	if(des_w > max_w)
		max_w = des_w;	
	
	//no need to change
	if((src_w == des_w) && (src_l == des_l))
		return;
	else if( scale_l >= 1.0 && scale_w >= 1.0){	//shrink in both directions
		src = image;
		dest = image;
		for(i = 0; i < des_l; i++){
			src_l_i = (long)(i*scale_l+0.5);
			src_row_start = src_l_i*src_row_byte;
			for(j = 0; j < des_w; j++){
				src_w_i = (long)(j*scale_w+0.5);
				src_start = src_row_start + src_w_i*comp;
				for(k = 0; k < comp; k++)
					dest[k] = src[src_start+k];
				dest += comp;
			}
		}		
	}
	else{//expand in one or both directions
		gap = max_w*max_l - des_w*des_l;
		src = image;
		dest = image + comp*gap;
		for(i = des_l-1; i >= 0; i--){
			src_l_i = (long)(i*scale_l+0.5);
			src_row_start = src_l_i*src_row_byte;
			dest_row_start = i*dest_row_byte;
			for(j = des_w-1; j >= 0; j--){
				src_w_i = (long)(j*scale_w+0.5);
				src_start = src_row_start + src_w_i*comp;
				dest_start = dest_row_start + j*comp;
				for(k = 0; k < comp; k++)
					dest[dest_start+k] = src[src_start+k];
			}
		}	
		
		//move up
		if(gap == 0) return;
		else{
			src = image + gap;
			dest = image;
			dest_total = des_w*des_l*comp;
			for( i = 0; i < dest_total; i++)
				*dest++ = *src++;	
		}		
	}
		
}


//non overwrite version
void image_simple_size_non(unsigned char* insrc, unsigned char* indest, long src_w, long src_l, long des_w, long des_l, 
	long comp)
{
	long i, j, k;
	double scale_l, scale_w;
	unsigned char* src, *dest;
	long src_l_i, src_w_i;	//index of src
	long src_row_byte;
	long src_start, src_row_start;
	
	src_row_byte = comp*src_w;
	scale_l = (double)(src_l - 1)/(double)des_l; 
	scale_w = (double)(src_w - 1)/(double)des_w; 
	
	src = insrc;
	dest = indest;
	for(i = 0; i < des_l; i++){
		src_l_i = (long)(i*scale_l+0.5);
		src_row_start = src_l_i*src_row_byte;
		for(j = 0; j < des_w; j++){
			src_w_i = (long)(j*scale_w+0.5);
			src_start = src_row_start + src_w_i*comp;
			for(k = 0; k < comp; k++)
				dest[k] = src[src_start+k];
			dest += comp;
		}
	}	
			
}


//overwrite image
void find_selection(unsigned char* image, long width, long length, Rect* select, long comp)
{
	long i, j, k;
	long left, top;
	long select_l, select_w;
	unsigned char* desstart, *srcstart, *src;
	long bump;
	
	srcstart = image + (select->top*width + select->left)*comp;
	desstart = image;
	
	select_w = select->right - select->left;
	select_l = select->bottom - select->top;
	
	for( i = 0; i < select_l; i++){
		bump = i*width;	
		for( j = 0; j < select_w; j++){
			src = srcstart + (bump + j)*comp;
			for(k = 0; k < comp; k++)
				*desstart++ = *src++;
		}	
	}
			
}


//not overwriting
//src is source image data
//des is destination image data
//dh and dv are the initial point
//len and wid are the lengths and widths
//ang is the rotation angle
//comp is the component
/*
void imagerotate(unsigned char* src, long slen, long swid,
				unsigned char* des, Rect *r, double ang, long comp)
{
	long i, j, k;
	double ch, cv;
	double co;		//cosine of ang
	double si;		//sine of ang
	long sh, sv;	//source h and v
	long srowlen = comp*swid;	//row length of source data
	long drowlen;	//row length of destination data
	long dst, sst;
	long dwid;

	dwid = r->right - r->left;
	drowlen = comp*dwid;
	ch = r->left/2.0 + r->right/2.0;
	cv = r->top/2.0 + r->bottom/2.0;
	co = cos(ang);
	si = sin(ang);
	
	for(i = r->top; i < r->bottom; i++){
		for(j = r->left; j < r->right; j++){
			sh = (long)((i - ch)*co - (j - cv)*si + ch + 0.5);
			sv = (long)((i - ch)*si + (j - cv)*co + cv + 0.5);
			dst = i*drowlen + j*comp;
			sst = sh*srowlen + sv*comp;
			for(k = 0; k < comp; k++)
				des[dst++] = src[sst++];			
		}
	}
}		
*/


//src is source image data
//des is destination image data
//dh and dv are the initial point
//len and wid are the lengths and widths
//ang is the rotation angle
//comp is the component
void imagerotate(unsigned char* src, long slen, long swid,
				unsigned char* des, long dv, long dh, long dlen, long dwid,
				double ang, long comp)
{
	long i, j, k;
	long dhend = dh + dwid;
	long dvend = dv + dlen;
	double ch = dh/2.0 + dhend/2.0;
	double cv = dv/2.0 + dvend/2.0;
	double co;		//cosine of ang
	double si;		//sine of ang
	long sh, sv;	//source h and v
	long srowlen = comp*swid;	//row length of source data
	long drowlen = comp*dwid;	//row length of destination data
	long dst, sst;
	
	co = cos(ang);
	si = sin(ang);

	for(i = dh; i < dhend; i++){
		for(j = dv; j < dvend; j++){
			sh = (long)((i - ch)*co - (j - cv)*si + ch + 0.5);
			sv = (long)((i - ch)*si + (j - cv)*co + cv + 0.5);
			dst = (j-dv)*drowlen + (i-dh)*comp;
			sst = sv*srowlen + sh*comp;
			for(k = 0; k < comp; k++)
				des[dst++] = src[sst++];			
		}
	}
	
}		
