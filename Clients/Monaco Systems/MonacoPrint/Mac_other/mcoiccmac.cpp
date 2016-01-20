// Confidential
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1996 Monaco Systems Inc.
//
//	mcoicc.c
//
//	defines ICC color transforms
// 	makes use of Colorsync
//
//	Create Date:	9/14/96
//	By James Vogh
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////


// search for a a set of profiles in the prefrence dir that satisfy the
// search critera
// returns a list of file descriptors

#include "mcoiccmac.h"
#include "monacoprint.h"
#include <string.h>

McoXformIccMac::McoXformIccMac(void)
{

cw = NULL;

validRef = FALSE;

NullXForm = FALSE;
}

McoXformIccMac::~McoXformIccMac(void)
{
if (validRef) CWDisposeColorWorld(cw); 
cw = NULL;

validRef = FALSE;
}






// build a transform given a list of profiles
McoStatus McoXformIccMac::buildCW(McoColorSync **colsynList,int32 numProf)
{
	int i;
	OSErr	iErr;
	CMError cmErr;
	McoStatus status = MCO_SUCCESS;
	FSSpec fspec;
	Boolean wasChanged;
	int16	in_space,out_space;
	
	CMProfileRef ref;
	int32	profileCount = 0;
	
	CMAppleProfileHeader	header;
	struct {
		CMConcatProfileSet	head;
		CMProfileRef		ref[MaxProfiles];
	}	set;
	
	if (numProf > MaxProfiles) return MCO_OUT_OF_RANGE;
	
	if (numProf == 0) 
		{
		NullXForm = TRUE;
		goto bail;
		}	
	
	// clean out the old color transform
	if (validRef) CWDisposeColorWorld(cw); 
	cw = NULL;
	
	
	for (i=0; i<numProf; i++)
		{
		status = colsynList[i]->getProfRef(&ref);
		if (status != MCO_SUCCESS) return status;
		if (ref != NULL)
			{
			if (profileCount == 0) 
				{
				set.head.profileSet[0] = ref;
				in_space = getProfileDataSpace(ref);
				}
			else set.ref[profileCount-1] = ref;
			profileCount++;
			}
		}
	
	if (profileCount > 0) out_space = getProfileDataSpace(ref);
	in_dataspace = in_space;
	out_dataspace = out_space;
	// if more than one profile was passed and one was null 
	// then assume that a null transform is desired
	// otherwise an error will most likely occur
	if ((profileCount == 1) && (numProf > 1))
		{
		NullXForm = TRUE;
		goto bail;
		}
	if (profileCount > 0)
		{
		set.head.keyIndex = 1;
		set.head.count = profileCount;
		
		cmErr = CWConcatColorWorld(&cw, (CMConcatProfileSet*)&set);
		if(cmErr != noErr) 
			{
			status = MCO_FAILURE;
			goto bail;
			}
		validRef = TRUE;
		}
	else NullXForm = TRUE;
	
bail:
		
	for (i=0; i<numProf; i++)
		{
		status = colsynList[i]->closeRef();	
		}
		
	return status;
}

#define bigger(a, b) ((a >= b) ? a :b)
#define smaller(a, b) ((a < b) ? a :b)

// eval using monaco's bitmap layout
McoStatus McoXformIccMac::eval(McoLayout *in, McoLayout *out)
 {
 unsigned char *inPtr8, *outPtr8, *inLine8, *outLine8;
 int32 i,j,temp;
 CMBitmap srcmap, desmap;
 int32	mimLineStride;
 CMError cmErr;
 McoStatus status = MCO_SUCCESS;
 unsigned char *minLineStride;
 
 // convert 	
//if ((in->colorspace != cmGrayASpace) && (out->colorspace == cmGrayASpace))
 	
if (NullXForm == TRUE) 
	{
	// do nothing if in and out are the same
	if (in == out) return MCO_SUCCESS;
	if (in->compPtrs[0] == out->compPtrs[0]) return MCO_SUCCESS;
	
	// copy otherwise
	if (in->lineStride == out->lineStride)
		memcpy(out->compPtrs[0],in->compPtrs[0],in->lineStride*in->numLines);
	else 
		{
		minLineStride = (unsigned char *)smaller(in->lineStride,out->lineStride);
		inPtr8 = (unsigned char*)in->compPtrs[0];
		outPtr8 = (unsigned char*)out->compPtrs[0];
		for (i=0; i<in->numLines; i++) 
			{
			memcpy(outPtr8,outPtr8,(long)minLineStride);
			inPtr8 += in->lineStride;
			outPtr8 += out->lineStride;
			}
		}
	return MCO_SUCCESS;
	}
	
if (validRef != TRUE) return MCO_OBJECT_NOT_INITIALIZED;

if (in->numLines <= 0) return MCO_SUCCESS;

srcmap.image= (char*)in->compPtrs[0]; 
srcmap.width= in->numPixels; 
srcmap.height= in->numLines; // mask QD rowBytes flag bits
srcmap.rowBytes= in->lineStride;
srcmap.pixelSize= in->compStride*in->compBits; 
srcmap.space = in->colorspace; 
srcmap.user1= 0; 
srcmap.user2= 0; 	

desmap.image= (char*)out->compPtrs[0]; 
desmap.width= out->numPixels; 
desmap.height= out->numLines; // mask QD rowBytes flag bits
desmap.rowBytes= out->lineStride;
desmap.pixelSize= out->compStride*out->compBits;
desmap.space = out->colorspace; 
desmap.user1= 0; 
desmap.user2= 0; 		

cmErr = CWMatchBitmap(cw, &srcmap, (CMBitmapCallBackUPP) NULL, NULL, (CMBitmap*)&desmap);
if(cmErr != noErr){ status = MCO_FAILURE; goto bail; }

bail:
 return status;
}
	
// apply the transform to the pixmap
// the pixmap must be locked
McoStatus McoXformIccMac::evalPixMap(PixMap *pixmap)
 {
 McoStatus status = MCO_SUCCESS;
 unsigned char *inPtr8, *outPtr8, *inLine8, *outLine8;
 int32 i,j,temp;
 CMBitmap srcmap, desmap;
 CMError cmErr;
 	
if (NullXForm == TRUE) return MCO_SUCCESS;
	
if (validRef != TRUE) return MCO_OBJECT_NOT_INITIALIZED;

cmErr = CWMatchPixMap( cw,pixmap , nil, nil );

if(cmErr != noErr) { status = MCO_FAILURE; goto bail; }
bail:
 return status;
}
	
	
// give an array of colors, match the colors using the colorworld
	
McoStatus McoXformIccMac::evalColors(unsigned char *in, unsigned char *out,int32 num)
 {
 unsigned char *inPtr8, *outPtr8, *inLine8, *outLine8;
 int32 i,j,temp;
 CMBitmap srcmap, desmap;
 CMError cmErr;
 McoStatus	status = MCO_SUCCESS;	
 	
if (NullXForm == TRUE) 
	{
	// do nothing if in and out are the same
	if (in == out) return MCO_SUCCESS;
	
	// copy otherwise
	memcpy(out,in,num*getColoSpacePixelChar(in_dataspace));
	return MCO_SUCCESS;
	}
	
if (validRef != TRUE) return MCO_OBJECT_NOT_INITIALIZED;
if (!cw) return MCO_OBJECT_NOT_INITIALIZED;

srcmap.image= (char*)in; 
srcmap.width= num; 
srcmap.height= 1; 
srcmap.rowBytes= num*getColoSpacePixelChar(in_dataspace);
srcmap.pixelSize= getColoSpacePixelChar(in_dataspace)*8; 
srcmap.space = in_dataspace; 
srcmap.user1= 0; 
srcmap.user2= 0; 	

desmap.image= (char*)out; 
desmap.width= num; 
desmap.height= 1; 
desmap.rowBytes= num*getColoSpacePixelChar(out_dataspace);
desmap.pixelSize= getColoSpacePixelChar(out_dataspace)*8;
desmap.space = out_dataspace; 
desmap.user1= 0; 
desmap.user2= 0; 		

cmErr = CWMatchBitmap(cw, &srcmap, (CMBitmapCallBackUPP) nil, nil, (CMBitmap*)&desmap);
if(cmErr != noErr){ status = MCO_FAILURE; goto bail; }

bail:
 return status;
}
	