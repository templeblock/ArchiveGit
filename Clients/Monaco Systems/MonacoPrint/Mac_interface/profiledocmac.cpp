///////////////////////////////////////////////////////////////////////////////////
//	profileDocMac.c															  	 //
//																			  	 //
//	The code for dealing with the profile data, replaces much of printdialog.c	 //
//  This is the Mac specific part											     //
//	James Vogh																 	 //
// June 4, 1996																  	 //
///////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <math.h>
#include <CMICCProfile.h>
#include "mpfileio.h"
#include "printdialog.h"
#include "cmysurface.h"
#include "colortran.h"
#include "monacoprint.h"
//#include "pagesetupdialog.h"
#include "rchoosedialog.h"
//#include "inputdialog2.h"
#include "mcofile.h"
//#include "PrintComments.h"
#include "csprofile.h"
//#include "gamutCmpDial.h"
//#include "finddirection.h"
#include "tiffformat.h"
#include "Thermometer.h"
#include "scancal.h"
#include "xyztorgb.h"
//#include "preprocess.h"
//#include "yapscalib.h"
//#include "calib_near.h"
#include "errordial.h"
#include "eve_stuff_2&3.h"
#include "gammutwin.h"
#include "gammut_comp.h"
#include "calib_multi.h"
#include "add_black.h"
#include "find_blacklab.h"
#include "pascal_string.h"
#include "fullname.h"
#include "think_bugs.h"




#include "spline3.h"

//#include "globalValues.h"

#include "profiledocmac.h"
#include "mcolyout.h"
//#include "mcoxccriccluts.h"
#include "scantarg.h"
#include "imagetrans.h"
//#include "fortestonly.h"

#include "codetable.h"
#include "waitforegrd.h"

unsigned char	clip_round(double x);
unsigned char	clip_round(double x)
{
unsigned char     t;
if (x>255.5) x = 255;
if (x<0) x = 0;


t=(unsigned char)x;
if (x-(double)t>=0.5) t++;
return(t);
}

short	clip_round2(double x);
short	clip_round2(double x)
{
short     t;
if (x>127.5) x = 127;
if (x<-128.5) x = -128;


t=(short)x;
if (x-(double)t>=0.5) t++;
return(t);
}


ProfileDocMac::ProfileDocMac(void):ProfileDocInf()
{

_scaned_rgb = 0L;
_patches = 0L;
scanner = 0L;

printerData = new PrinterType;
if (!printerData) return;

// determine if a monacoprint dongle is attached

_monacoprint = is_monaco_print();

// load in the default printer if this is a polaroid
#ifdef IN_POLAROID
FileFormat filef;
FSSpec fspec;
McoStatus  state;
Str255  def = MONACO_DEFAULT_SETTINGS;
Str255  fold = MONACO_FOLDER;

ptocstr(def);
ptocstr(fold);



state = filef.findFilefromPref(&fspec,(char*)def,(char*)fold);
//if (state != MCO_SUCCESS) return;
state = printerData->Load(&fspec);

// copy data into the 
initPrintData();

//copyIntoPrintData();

valid_fsspec = 0;
DoSafeSave = 0;
#endif
}

ProfileDocMac::~ProfileDocMac(void)
{
if (_scaned_rgb) delete _scaned_rgb;
_scaned_rgb = 0L;
if (_patches) delete _patches;
_patches = 0L;
if (scanner) delete scanner;
scanner = 0L;
}

McoStatus ProfileDocMac::GetFileName(int flag)
{
StandardFileReply 	soutReply;
Str255				prompt;
Str255				outname = "\p";
FILE 				*ofsb;
long 				i, j, k,c,m;
double 				*dataptr;
int					total_patches;
McoStatus			status = MCO_SUCCESS;
FileFormat			file;

GetIndString(prompt,PROMPT_STRINGS,LINK_PROMPT);

DoSafeSave = 0;

if ((valid_fsspec) && (flag))  // duplicate the file
	{
	StandardPutFile(prompt, outname, &soutReply);
	if(!soutReply.sfGood) return MCO_CANCEL;
	copy_str(patchFName,soutReply.sfFile.name);
	duplicateFile(&soutReply.sfFile);
	}
else if (!valid_fsspec)						 // get the filename of the file
	{
	StandardPutFile(prompt, outname, &soutReply);
	if(!soutReply.sfGood) return MCO_CANCEL;
	copy_str(patchFName,soutReply.sfFile.name);
	if (file.doesFileExist(&soutReply.sfFile)) DoSafeSave = 1;
	status = _setupSafeSave();
	
	icc_fsspec.vRefNum = soutReply.sfFile.vRefNum;
	icc_fsspec.parID = soutReply.sfFile.parID;
	copy_str(icc_fsspec.name,soutReply.sfFile.name);
	valid_fsspec = 1;
	}
return status;
}

McoStatus ProfileDocMac::handle_read_iccfile(void)
{
	McoStatus 			status;
	StandardFileReply 	sinReply;
	SFTypeList			filelist;
	Str255				inname;
	Mpfileio			*mpfileio;
	McoColorSync 		*prof;
	CMProfileRef 		pref;

	filelist[0] = 'prof';
	StandardGetFile(0L, 1, filelist, &sinReply);
	if(sinReply.sfGood == 0)
		return MCO_CANCEL;
		
	icc_fsspec.vRefNum = sinReply.sfFile.vRefNum;
	icc_fsspec.parID = sinReply.sfFile.parID;
	copy_str(icc_fsspec.name,sinReply.sfFile.name);
	
	copy_str(patchFName,sinReply.sfFile.name);
	
	valid_fsspec = 1;

	if (!printData) return MCO_OBJECT_NOT_INITIALIZED;
	
	prof = new McoColorSync;
	if (!prof) return MCO_MEM_ALLOC_ERROR;
	
	status = prof->setFSSpec(&icc_fsspec);
	if (status) return status;
	
	status = prof->getProfRef(&pref);
	if (status) return status;
	if (getProfileType(pref) != 'link') return MCO_NOT_ICC_LINK;
	
	if (getProfileDataSpace(pref) != cmCMYK32Space) return MCO_NOT_ICC_LINK;
	
	delete prof;
	
	mpfileio = new Mpfileio(printData);
	if (!mpfileio) return MCO_MEM_ALLOC_ERROR;
	
	status = mpfileio->openProfile(&icc_fsspec);
	if (status != MCO_SUCCESS) return status;
	
	status = copyOutOfPrintData();
	
	if ((printData->icctype == NOT_MONACO_ICC) || (printData->icctype == MONACOP_CREATED_ICC)) monacoProfile = 0;
	else monacoProfile = 1;
	
	if ((printData->icctype == MONACO_EDITED_ICC) || (printData->icctype == MONACOP_EDITED_ICC)) monacoEdited = 1;
	else monacoEdited= 0;
	
	changedProfile = 0;
	createdProfile = 0;
	changedLinearTone = 0;
	
	didGammutSurface = 0;
	didGammutCompression = 0;
	
	delete mpfileio;

	return status;	
}	

McoStatus ProfileDocMac::handle_read_iccfile(FSSpec *fspec)
{
	McoStatus 			status;
	Mpfileio			*mpfileio;
	OSErr				iErr;
	FInfo				fndrInfo;
	McoColorSync 		*prof;
	CMProfileRef 		pref;

		
	icc_fsspec.vRefNum = fspec->vRefNum;
	icc_fsspec.parID = fspec->parID;
	copy_str(icc_fsspec.name,fspec->name);
	
	copy_str(patchFName,fspec->name);
	
	valid_fsspec = 1;

	if (!printData) return MCO_OBJECT_NOT_INITIALIZED;
	
	prof = new McoColorSync;
	if (!prof) return MCO_MEM_ALLOC_ERROR;
	
	status = prof->setFSSpec(fspec);
	if (status) return status;
	
	status = prof->getProfRef(&pref);
	if (status) return status;
	if (getProfileType(pref) != 'link') return MCO_NOT_ICC_LINK;
	
	if (getProfileDataSpace(pref) != cmCMYK32Space) return MCO_NOT_ICC_LINK;
	
	delete prof;
	
	mpfileio = new Mpfileio(printData);
	if (!mpfileio) return MCO_MEM_ALLOC_ERROR;
	
	iErr = FSpGetFInfo(&icc_fsspec,&fndrInfo);
	
	if (fndrInfo.fdType != 'prof') return MCO_FILE_DATA_ERROR;
	
	status = mpfileio->openProfile(&icc_fsspec);
	if (status != MCO_SUCCESS) return status;
	
	status = copyOutOfPrintData();
	
	if ((printData->icctype == NOT_MONACO_ICC) || (printData->icctype == MONACOP_CREATED_ICC)) monacoProfile = 0;
	else monacoProfile = 1;
	
	if ((printData->icctype == MONACO_EDITED_ICC) || (printData->icctype == MONACOP_EDITED_ICC)) monacoEdited = 1;
	else monacoEdited= 0;

	
	changedProfile = 0;
	createdProfile = 0;
	changedLinearTone = 0;

	didGammutSurface = 0;
	didGammutCompression = 0;
	
	delete mpfileio;

	return status;	
}	

McoStatus ProfileDocMac::handle_reread_iccfile(void)
{
McoStatus status;
status = initPrintData();
if (status) return status;
return handle_read_iccfile(&icc_fsspec);
}	


McoStatus ProfileDocMac::_makelinkProfile(McoColorSync *prof,McoXformIccMac *link)
{
McoStatus state;
McoColorSync *lab = 0L,*list[2];

if (!link) return MCO_MEM_ALLOC_ERROR;

lab = new McoColorSync;
if (!lab) return MCO_MEM_ALLOC_ERROR;
state = lab->GetProfile(LAB_PROFILE_NAME);
if (state) goto bail;

list[0] = prof;
list[1] = lab;

state = link->buildCW(list,2);
bail:

if (lab) delete lab;
return state;
}

//read the file, now only read the data file
McoStatus ProfileDocMac::_handle_read_datafile(void)
{
	McoStatus 			status;
	StandardFileReply 	sinReply;
	SFTypeList			filelist;
	Str255				inname;
	FILE 				*fs;
	short				i,j,k,m;
	double *patch, *patchcopy;
	int32				type,numc,numslow;
	int32				*dims,total = 0, dimCount = 0;
	int32				dimp = 1;
	OSErr				iErr;
	FInfo				fndrInfo;

	
//	StandardGetFile(/*(FileFilterProcPtr)*/NULL, -1, NULL, &sinReply);

	//we have to load the txt file for the time being
	filelist[0] = 'TEXT';
	filelist[1] = 'prof';
	StandardGetFile(0L, 2, filelist, &sinReply);
	if(sinReply.sfGood == 0)
		return MCO_CANCEL;
	
	#ifndef IN_POLAROID	
	copy_str(patchFName,sinReply.sfFile.name);
	#endif


	iErr = FSpGetFInfo(&sinReply.sfFile,&fndrInfo);
	
	if (fndrInfo.fdType == 'prof')
		{
		McoColorSync prof;
		McoXformIccMac link;
		CMProfileRef pref;
		
		status = prof.setFSSpec(&sinReply.sfFile);
		if (status) return status;
		
		status = prof.getProfRef(&pref);
		if (status) return status;
		if (getProfileType(pref) != 'prtr') return MCO_FILE_DATA_ERROR;
		
		if (getProfileDataSpace(pref) != cmCMYK32Space) return MCO_FILE_DATA_ERROR;
		
		// reinitialize the document
		_initAll();
		
		if ((defaults->patch_format < 0) || (defaults->patch_format > patchset->filenum)) 
			return MCO_FILE_OPEN_ERROR;
		if (patchset->patches[defaults->patch_format] == NULL)
			return MCO_OBJECT_NOT_INITIALIZED;	
		patchD->copyRawData(patchset->patches[defaults->patch_format]);
		
		status = _makelinkProfile(&prof,&link);
		if (status) return status;
		
		status = patchD->readICCFile(&link);
		if (status) return status;
		
		memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);
		ptocstr(inname);	

		patchD->SetDesc((char*)inname);
		
		changedProfile = 1;
		
		status = loadLinearData();
		return status;
		
		}
	else
		{
	
		memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);
		PathNameFromDirID(sinReply.sfFile.parID, sinReply.sfFile.vRefNum, inname);
		ptocstr(inname);	
		
		fs = fopen( (char*)inname, "r" );
		if(!fs)
			return MCO_FILE_OPEN_ERROR;

		// reinitialize the document
		_initAll();

		status = _read_datafile(&patchD,fs);
		if (status) return status;
		
		memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);
		ptocstr(inname);	

		patchD->SetDesc((char*)inname);
		
		fclose(fs);

		patchDcopy->copyRawData(patchD);
			
		didGammutSurface = 0;
		didGammutCompression = 0;
		_smoothedPatches = FALSE;
		
		changedProfile = 1;
			
		return loadLinearData();
		}	
		
}


//read the file, now only read the data file
McoStatus ProfileDocMac::_handle_read_sim_datafile(void)
{
	McoStatus 			status;
	StandardFileReply 	sinReply;
	SFTypeList			filelist;
	Str255				inname;
	FILE 				*fs;
	short				i,j,k,m;
	double *patch, *patchcopy;
	int32				type,numc,numslow;
	int32				*dims,total = 0, dimCount = 0;
	int32				dimp = 1;
	OSErr				iErr;
	FInfo				fndrInfo;

	
//	StandardGetFile(/*(FileFilterProcPtr)*/NULL, -1, NULL, &sinReply);

	//we have to load the txt file for the time being
	filelist[0] = 'TEXT';
	filelist[1] = 'prof';
	StandardGetFile(0L, 2, filelist, &sinReply);
	if(sinReply.sfGood == 0)
		return MCO_CANCEL;
		
		
	iErr = FSpGetFInfo(&sinReply.sfFile,&fndrInfo);
	
	if (fndrInfo.fdType == 'prof')
		{
		McoColorSync prof;
		CMProfileRef pref;
		McoXformIccMac link;
		
		status = prof.setFSSpec(&sinReply.sfFile);
		if (status) return status;
		
		status = prof.getProfRef(&pref);
		if (status) return status;
		if (getProfileType(pref) != 'prtr') return MCO_FILE_DATA_ERROR;
		
		if (getProfileDataSpace(pref) != cmCMYK32Space) return MCO_FILE_DATA_ERROR;
		
		// reinitialize the document
		_initAll();
		
		if ((defaults->patch_format < 0) || (defaults->patch_format > patchset->filenum)) 
			return MCO_FILE_OPEN_ERROR;
		if (patchset->patches[defaults->patch_format] == NULL)
			return MCO_OBJECT_NOT_INITIALIZED;	
		simupatchD->copyRawData(patchset->patches[defaults->patch_format]);
		
		status = _makelinkProfile(&prof,&link);
		if (status) return status;
		
		status = simupatchD->readICCFile(&link);
		if (status) return status;
		
		memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);
		ptocstr(inname);	

		simupatchD->SetDesc((char*)inname);
		
		changedProfile = 1;
		
		return status;
		
		}
	else	
		{
	//	copy_str(patchFName,sinReply.sfFile.name);

		memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);
		PathNameFromDirID(sinReply.sfFile.parID, sinReply.sfFile.vRefNum, inname);
		ptocstr(inname);	
		
		fs = fopen( (char*)inname, "r" );
		if(!fs)
			return MCO_FILE_OPEN_ERROR;

		// reinitialize the document
		_initAll();

		status = _read_datafile(&simupatchD,fs);
		if (status) return status;
		
		memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);
		ptocstr(inname);	

		simupatchD->SetDesc((char*)inname);
		
		fclose(fs);
		
		simupatchDcopy->copyRawData(simupatchD);
			
		didGammutSurface = FALSE;
		_smoothedPatches = FALSE;
		
		changedProfile = 1;
				
		return MCO_SUCCESS;
		}
}

//read the file, now only read the data file
McoStatus ProfileDocMac::_handle_read_datafile(FSSpec *fspec)
{
	McoStatus 			status;
	Str255				inname;
	FILE 				*fs;
	short				i,j,k,m;
	double *patch, *patchcopy;
	int32				type,numc,numslow;
	int32				*dims,total = 0, dimCount = 0;
	int32				dimp = 1;

		
	copy_str(patchFName,fspec->name);

	memcpy(inname, fspec->name, *(fspec->name)+1);
	PathNameFromDirID(fspec->parID, fspec->vRefNum, inname);
	ptocstr(inname);	
	
	fs = fopen( (char*)inname, "r" );
	if(!fs)
		return MCO_FILE_OPEN_ERROR;

	// reinitialize the document
	_initAll();

	status = _read_datafile(&patchD,fs);
	
	memcpy(inname, fspec->name, *(fspec->name)+1);
	ptocstr(inname);	

	patchD->SetDesc((char*)inname);
	
	fclose(fs);

	patchDcopy->copyRawData(patchD);
		
	didGammutSurface = FALSE;
	_smoothedPatches = FALSE;
		
	return loadLinearData();	
		
}



McoStatus ProfileDocMac::_handle_read_tifffile(RawData *PatchD)
{
	McoStatus 			status;
	StandardFileReply 	sinReply;
	SFTypeList			filelist;
	TiffFormat			*intiff;
	Str255				inname;
	FILE 				*fs;
	short				i,j,k,m;
	int32 				imagesize;
	int16				end = 0;
	

	if (_patches) delete _patches;
	if (_scaned_rgb) delete _scaned_rgb;
	_patches = new McoPatches(PatchD->patches.rows*PatchD->patches.columns);
	_scaned_rgb = new unsigned char[3*(PatchD->patches.rows*PatchD->patches.columns)];

	status = _LoadPatchLocations(_patches,PatchD);
	if( status != MCO_SUCCESS){
		return status; 
	}	

	filelist[0] = 'TIFF';
	StandardGetFile(NULL, 1, filelist, &sinReply);
	if(sinReply.sfGood){
		intiff = new TiffFormat;
		if(!intiff)	return MCO_MEM_ALLOC_ERROR;
		status = intiff->openFile(&(sinReply.sfFile));
	}	
	else 
		return MCO_CANCEL;
				
	copy_str(patchFName,sinReply.sfFile.name);
	
	if( status != MCO_SUCCESS){
		delete intiff;
		return status;
	}
		
	if( (status = intiff->prepareRead()) != MCO_SUCCESS) {
		intiff->closeFile();
		delete intiff;
		return status;
	}

	if( (status = intiff->getSamplesperpixel(&samplesperpixel)) != MCO_SUCCESS){
		intiff->closeFile();
		delete intiff;
		return status;
	}	
	if( (status = intiff->getWidth(&width)) != MCO_SUCCESS){
		intiff->closeFile();
		delete intiff;
		return status;
	}
	if( (status = intiff->getLength(&length)) != MCO_SUCCESS){
		intiff->closeFile();
		delete intiff;
		return status;
	}
	if( (status = intiff->getBitspersample(&bitspersample)) != MCO_SUCCESS){
		intiff->closeFile();
		delete intiff;
		return status;
	}
	if( (status = intiff->getPhotometric(&photometric)) != MCO_SUCCESS){
		intiff->closeFile();
		delete intiff;
		return status;
	}
	if( (status = intiff->getRowsperstrip(&rowsperstrip)) != MCO_SUCCESS){
		intiff->closeFile();
		delete intiff;
		return status;
	}

	if(photometric != PHOTOMETRIC_RGB){
		intiff->closeFile();
		delete intiff;
		return MCO_UNSUPPORTED_NUM_COMPONENTS;
	}
		
	if (targetBufferH != NULL) McoDeleteHandle(targetBufferH);
	targetBufferH = McoNewHandle(length*width*samplesperpixel);
	if(targetBufferH==NULL) {
		intiff->closeFile();
		delete intiff;
		return MCO_MEM_ALLOC_ERROR;
	}

	imagesize = length*width*samplesperpixel;
	if( (status = intiff->startRead(targetBufferH, imagesize)) != MCO_SUCCESS){
		McoDeleteHandle(targetBufferH);
		targetBufferH = 0;
		intiff->closeFile();
		delete intiff;
		return MCO_MEM_ALLOC_ERROR;
	}
	
	while( !end )
		if( (status = intiff->continueRead(&end)) != MCO_SUCCESS){
			McoDeleteHandle(targetBufferH);
			targetBufferH = 0;
			intiff->closeFile();
			delete intiff;
			return MCO_MEM_ALLOC_ERROR;
		}
		
	intiff->closeFile();
	delete intiff;

	return MCO_SUCCESS;

}


McoStatus ProfileDocMac::aftercrop(Rect	*selectrect, RawData *PatchD)
{
	int32	i,j,k;
	
	int32 rows, cols, planes;
	char *targetBuffer, *outBuffer;	
//	McoLayout in, out;
//	unsigned char *srcPtr, *dstPtr;
//	McoXformCrrIccLuts *icc;
	
	McoStatus	status = MCO_SUCCESS;


	Point	tr,tl,bl;
	SCPoint	p1,p2,p3;
	Rect	theRect;
	ScanTarg *gTrans;
	long 	gBufferBytes;
	McoHandle	bufferH, patchesH;
	char	*buffer;

	status = _LoadPatchLocations(_patches,PatchD);
	if( status != MCO_SUCCESS){
		McoDeleteHandle(targetBufferH);
		targetBufferH = 0;
		return status; 
	}	
	
	gTrans = new ScanTarg;
	if(!gTrans){
		McoDeleteHandle(targetBufferH);
		targetBufferH = 0;
		return MCO_MEM_ALLOC_ERROR;
	}
	
	//setup the neccessary field
	tr.h = width;
	tr.v = 0;
	tl.h = 0;
	tl.v = 0;
	bl.h = 0;
	bl.v = length;

	gTrans->SetTransform(&tr,&tl,&bl);
	gTrans->num_cmp = samplesperpixel;

	p1.h = _patches->patches[0].left;
	p1.v = _patches->patches[0].top;
	gTrans->Transform(&p1,&p2);
	
	p1.h = _patches->patches[0].right;
	p1.v = _patches->patches[0].bottom;
	gTrans->Transform(&p1,&p3);

	gTrans->GetFullRect(&p2,&p3,&theRect);
	
// Need to be sure that enough space is allocated  so add 10 to each direction	
	theRect.top -= 10;
	theRect.top = MaxVal(theRect.top,0);
	theRect.left -= 10;
	theRect.left = MaxVal(theRect.left,0);
	theRect.bottom += 10;
	theRect.bottom = MinVal(theRect.bottom, length);
	theRect.right += 10;
	theRect.right = MinVal(theRect.right, width);
	
	gTrans->rbyte= (((theRect.right-theRect.left) * (int32)bitspersample + 7) >> 3)*samplesperpixel;
	gBufferBytes = gTrans->rbyte*(theRect.bottom-theRect.top);

//buffer for holding a single patch
	bufferH = McoNewHandle(gBufferBytes);
	if (!bufferH){
		delete gTrans;
		McoDeleteHandle(targetBufferH);
		targetBufferH = 0;
		return MCO_MEM_ALLOC_ERROR;
	}		
	buffer = (char*)McoLockHandle(bufferH);
	
// Set rectangle back to correct values
	targetBuffer = (char*)McoLockHandle(targetBufferH);
	char *head, *temp, *tempbuf;
	
	for( i = 0; i < _patches->num_patches; i++){
			
		p1.h = _patches->patches[i].left;
		p1.v = _patches->patches[i].top;
		gTrans->Transform(&p1,&p2);
		
		p1.h = _patches->patches[i].right;
		p1.v = _patches->patches[i].bottom;
		gTrans->Transform(&p1,&p3);
		
		gTrans->GetFullRect(&p2,&p3,&theRect);
		gTrans->rbyte = (((theRect.right-theRect.left) * (int32)bitspersample + 7) >> 3)*samplesperpixel;
		
		tempbuf = buffer;
		head = targetBuffer + (theRect.top*width + theRect.left)*samplesperpixel;
		for( k = 0; k < theRect.bottom - theRect.top; k++){
			temp = head + k*width*samplesperpixel;
			for( j = 0; j < (theRect.right - theRect.left)*samplesperpixel; j++)
				*tempbuf++ = *temp++;
		}	

		status = gTrans->ScanSingTarg2(&theRect,&_patches->patches[i],
			(unsigned char*)(buffer),&_scaned_rgb[i*3]);
		 
	}



	McoUnlockHandle(targetBufferH);
	McoDeleteHandle(targetBufferH);	
	targetBufferH = 0;
	McoUnlockHandle(bufferH);
	McoDeleteHandle(bufferH);	
//end of scan

	delete gTrans;

//convert RGB data to LAB by using ICC table
	status = _buildPatchData(PatchD);
	if(status != MCO_SUCCESS)	return status;

	 patchtype = patchD->type;
	
	didGammutSurface = FALSE;
	_smoothedPatches = FALSE;
	needsSave = TRUE;
	return status; //_extend_ref_patch();
}


McoStatus ProfileDocMac::_LoadPatchLocations(McoPatches* patches,RawData *PatchD)
{
	FileFormat *patchfile;
	FSSpec		patchfspec;
	McoStatus	status;
	long		i, j, k, m,c;
	long 		headersize;
	double col_dx,col_dy;
	double row_dx,row_dy;
	double		xs,ys;
	int			numCol,numRow;
	double		w = 1;
	double		h = 1;
	double	x0,y0,x1,y1;
	
	numCol = PatchD->patches.columns;
	numRow = PatchD->patches.rows;
	
	patches->num_patches = PatchD->format.total+PatchD->format.numLinear*PatchD->format.numc;
	
	switch (PatchD->patches.direction) {
	case 0:	//  move horizontally
	switch (PatchD->patches.starting) {
		case 0: 
			col_dx = 0;
			col_dy = h/(double)(numCol+1);
			row_dx = w/(double)(numRow+1);
			row_dy = 0;
			break;
		case 1:
			col_dx = 0;
			col_dy = h/(double)(numCol+1);
			row_dx = -w/(double)(numRow+1);
			row_dy = 0;
			break;
		case 2:
			col_dx = 0;
			col_dy = -h/(double)(numCol+1);
			row_dx = w/(double)(numRow+1);
			row_dy = 0;
			break;
		case 3:
			col_dx = 0;
			col_dy = -h/(double)(numCol+1);
			row_dx = -w/(double)(numRow+1);
			row_dy = 0;
			break;
		}
		break;
	case 1: // move vertically
		switch (PatchD->patches.starting) {
		case 0: 
			col_dx = w/(double)(numCol+1);
			col_dy = 0;
			row_dx = 0;
			row_dy = h/(double)(numRow+1);
			break;
		case 1:
			col_dx = -w/(double)(numCol+1);
			col_dy = 0;
			row_dx = 0;
			row_dy = h/(double)(numRow+1);
			break;
		case 2:
			col_dx = w/(double)(numCol+1);
			col_dy = 0;
			row_dx = 0;
			row_dy = -h/(double)(numRow+1);
			break;
		case 3:
			col_dx = -w/(double)(numCol+1);
			col_dy = 0;
			row_dx = 0;
			row_dy = -h/(double)(numRow+1);
			break;
		}
		break;
	}		
	
	ys = 0.333333/PatchD->patches.rows;
	xs = 0.333333/PatchD->patches.columns;
	
	
	
	x0 = (col_dx+row_dx)/2;
	y0 = (col_dy+row_dy)/2;

	
	c = 0;
	for (i=0; i<PatchD->patches.columns; i++)
		{
		x1 = x0;
		y1 = y0;
		for (j=0; j<PatchD->patches.rows; j++)
			{
			patches->patches[c].top = y1 + ys;
			patches->patches[c].bottom = y1 + 2*ys;
			patches->patches[c].left = x1 + xs;
			patches->patches[c].right = x1 + 2*xs;
			c++;
			x1 += row_dx;
			y1 += row_dy;
			}
		x0 += col_dx;
		y0 += col_dy;
		}
		
	return MCO_SUCCESS;
}		



McoStatus ProfileDocMac::_buildPatchData(RawData *PatchD) 
{
	McoStatus status;
	unsigned char	lab[3];
	
	long c,i, j, k;
	double 			*patchdata;
	double				*patch, *patchcopy;
	
	if (!scanner) return MCO_OBJECT_NOT_INITIALIZED;

	if (!PatchD->dataH) return MCO_FAILURE;
	
	patchdata = (double*)McoLockHandle(PatchD->dataH);
	
	c = 0;
	for (i=0; i<PatchD->format.total; i++)
		{
		scanner->evalColors(&_scaned_rgb[c*3],lab,1);
		patchdata[i*3] = (double)lab[0]/2.55;
		patchdata[i*3+1] = (double)lab[1]-128.0;
		patchdata[i*3+2] = (double)lab[2]-128.0;
		c++;
		}
	
	McoUnlockHandle(PatchD->dataH);
	
	if (PatchD->linearDataH)
		{
		patchdata = (double*)McoLockHandle(PatchD->linearDataH);
		for (i=0; i<PatchD->format.numc*(PatchD->format.numLinear-1); i++)
			{
			scanner->evalColors(&_scaned_rgb[c*3],lab,1);
			patchdata[i*3] = (double)lab[0]/2.55;
			patchdata[i*3+1] = (double)lab[1]-127.0;
			patchdata[i*3+2] = (double)lab[2]-127.0;
			c++;
			}
		McoUnlockHandle(PatchD->linearDataH);
		}

	_initAll();

	copyWaiting(PatchD);

		
	didGammutSurface = FALSE;
	_smoothedPatches = FALSE;
		
			
	return MCO_SUCCESS;
}






McoStatus  ProfileDocMac::handle_save_datafile(RawData *PatchD)
{
	StandardFileReply 	soutReply;
	Str255				prompt = "\pSave the Current File As";
	Str255				outname = "\p";
	FILE 				*ofsb;
	long 				i, j, k,c,m;
	double 				*dataptr;
	int					total_patches;
	McoStatus			status;

	StandardPutFile(prompt, outname, &soutReply);
	if(!soutReply.sfGood) return MCO_CANCEL;
	memcpy(outname, soutReply.sfFile.name, *(soutReply.sfFile.name)+1);	
	PathNameFromDirID(soutReply.sfFile.parID, soutReply.sfFile.vRefNum, outname);
	ptocstr(outname);

	ofsb = fopen( (char*)outname, "w");
	if(!ofsb)	return MCO_FILE_OPEN_ERROR;

	status = PatchD->saveData(ofsb);
	if (status != MCO_SUCCESS) return status;
	fclose(ofsb);
	
	copy_str(patchFName,soutReply.sfFile.name);
	
	needsSave = FALSE;
	FileFormat ff;
	
	ff.setInfo(&soutReply.sfFile,MONACO_SIG,'TEXT');

	return MCO_SUCCESS;
}



// Search through the current process list to find the given application. See
// Using the Process Manager for a similar way of doing this.
OSErr ProfileDocMac::FindAProcess(OSType typeToFind, OSType creatorToFind,
			ProcessSerialNumberPtr processSN)
{
	ProcessInfoRec	tempInfo;
	FSSpec	procSpec;
	Str31		processName;
	OSErr		myErr = noErr;

	// start at the beginning of the process list
	processSN->lowLongOfPSN = kNoProcess;
	processSN->highLongOfPSN = kNoProcess;

	// initialize the process information record
	tempInfo.processInfoLength = sizeof(ProcessInfoRec);
	tempInfo.processName = (StringPtr)&processName;
	tempInfo.processAppSpec = &procSpec;

	while((tempInfo.processSignature != creatorToFind ||
			tempInfo.processType != typeToFind) ||
			myErr != noErr)
	{
			myErr = GetNextProcess(processSN);
			if (myErr == noErr)
					GetProcessInformation(processSN, &tempInfo);
	}
	return(myErr);
}




McoStatus ProfileDocMac::savetoRGBtoRGB(void)
{
	McoStatus status;
	StandardFileReply 	soutReply;
	FileFormat 			*jobfile;	
	Str255				prompt;
	Str255				defaultfilename = "\p";
	McoHandle			tableH;
	unsigned char		*table, *tableh;
	McoHandle			ltableH;
	unsigned char		*ltable, *ltableh;	
	unsigned char		*clip;
	long				i;
	unsigned short		version;
	unsigned char		exist;
	
	double	R,G,B;
	double	t,u,v;
	double	a,b,c;
	int32	a1,a2,b1,b2,c1,c2;	 	
	CsFormat	*csformat;
	Ctype _A2B0, _A2B1, _B2A0, _out_of_gamut;
	CM2Header header;
	char	name[256];
	
	GetIndString(prompt,PROMPT_STRINGS,ICC_PROMPT);

//some init	
	_A2B0.input_channels = 3;
	_A2B0.output_channels = 3;
	_A2B0.grid_points	= _rev_table_grid;
	_A2B0.identity_matrix = 1;	//is it identity matrix 1 == yes
	_A2B0.input_entries = 256;
	_A2B0.output_entries = 256;	
	
	setUpCType(&_A2B0);
	buildInverseCuvre(&_A2B0);
	
	_A2B1.input_channels = 3;
	_A2B1.output_channels = 3;
	_A2B1.grid_points	= _rev_table_grid;
	_A2B1.identity_matrix = 1;	//is it identity matrix 1 == yes
	_A2B1.input_entries = 256;
	_A2B1.output_entries = 256;	
	
	setUpCType(&_A2B1);
	buildInverseCuvre(&_A2B1);

	_B2A0.input_channels = 3;
	_B2A0.output_channels = 3;
	_B2A0.grid_points	= 33;
	_B2A0.identity_matrix = 1;	//is it identity matrix 1 == yes
	_B2A0.input_entries = 256;
	_B2A0.output_entries = 256;	
	
	setUpCType(&_B2A0);
	buildOutputCuvre(&_B2A0);
	
	_out_of_gamut.input_channels = 3;
	_out_of_gamut.output_channels = 1;
	_out_of_gamut.grid_points	= 33;
	_out_of_gamut.identity_matrix = 1;	//is it identity matrix 1 == yes
	_out_of_gamut.input_entries = 256;
	_out_of_gamut.output_entries = 256;
		
	setUpCType(&_out_of_gamut);
	
	waitTillForeground();
		 	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

	tableH = McoNewHandle(sizeof(char)*TABLE_ENTRIES*3);
	if(!tableH)	return MCO_MEM_ALLOC_ERROR;
	table = (unsigned char*)McoLockHandle(tableH);
	tableh = table;
				
				
	csformat = new CsFormat;
	if(!csformat){
		McoDeleteHandle(tableH);
		return MCO_MEM_ALLOC_ERROR;
	}
		
	status = csformat->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, MONACO_SIG, 'prof');
	if( status != MCO_SUCCESS ){
		McoDeleteHandle(tableH);
		return status;
	}
	
	clip = &_clipt[5120];
	_table = (double*)McoLockHandle(_con_to_dev_tableH);
	for(i = 0; i < TABLE_ENTRIES; i++){
		table[0] = clip_round(_table[0]*2.55);
		table[1] = clip_round(_table[1]*2.55);
		table[2] = clip_round(_table[2]*2.55);
		table += 3;
		_table += 4;		
	}
	McoUnlockHandle(_con_to_dev_tableH);

	

//set header stuff
	header.CMMType = MONACO_CMM;
	header.profileVersion = (MCOP_MAJOR_VERSION + MCOP_MINOR_VERSION);
	header.profileClass = cmOutputClass;
	header.dataColorSpace = cmRGBData;
	header.profileConnectionSpace = MCOP_DEFAULT_CON_SPACE;
		
	header.CS2profileSignature = cmMagicNumber;	//?
	header.platform = MCOP_DEFAULT_PLATFORM;
	header.flags = MCOP_DEFAULT_FLAGS;
	header.deviceAttributes[0] = MCOP_DEFAULT_ATTRIBUTES;
	header.deviceAttributes[1] = MCOP_DEFAULT_ATTRIBUTES;
	header.renderingIntent = MCOP_DEFAULT_RENDERINGINTENT;
	header.white.X = MCOP_DEFAULT_WHITE_X;
	header.white.Y = MCOP_DEFAULT_WHITE_Y;
	header.white.Z = MCOP_DEFAULT_WHITE_Z;

	status = csformat->setHeader(&header, cmSigLut8Type);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	ptocstr(soutReply.sfFile.name);
	strcpy(name,(char*)soutReply.sfFile.name);
	strcat(name,MCOP_IDF);
	status = csformat->setDesctypedata(name, strlen(name)+1);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(1, &_A2B0, _dev_to_con_tableH2);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}


	status = csformat->setLuttypedata(2, &_B2A0, tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(5, &_B2A0, tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(7, &_B2A0, tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(3, &_out_of_gamut, _out_of_gamutH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(4, &_A2B1, _dev_to_con_tableH2);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(6, &_A2B1, _dev_to_con_tableH2);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	CMXYZType white;

	white.typeDescriptor = cmMediaWhitePointTag;
	white.reserved = 0x00000000;
	white.XYZ[0].X = MCOP_DEFAULT_WHITE_X;
	white.XYZ[0].Y = MCOP_DEFAULT_WHITE_Y;
	white.XYZ[0].Z = MCOP_DEFAULT_WHITE_Z;

	status = csformat->setXYZtypedata(cmMediaWhitePointTag, &white);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setCprttypedata(MCOP_CPRT, strlen(MCOP_CPRT));
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->writeTag(1);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
	
	csformat->closeFile(); 
	delete csformat; 
	
	cleanupCType(&_A2B0);
	cleanupCType(&_A2B1);
	cleanupCType(&_B2A0);
	cleanupCType(&_out_of_gamut);

	McoDeleteHandle(tableH);
	return status;
}		



McoStatus ProfileDocMac::savetoPhototable(int32	pass)
{
	McoStatus status;
	StandardFileReply 	soutReply;
	FileFormat 			*jobfile;	
	Str255				prompt;
	Str255				defaultfilename = "\p";
	McoHandle			tableH;
	char				*table, *tableh;
	McoHandle			ltableH;
	char				*ltable, *ltableh;	
	unsigned char		*clip;
	long				i;
	unsigned short		version;
	unsigned char		exist;
	FSSpec				fspec;
	
	GetIndString(prompt,PROMPT_STRINGS,PHOTOSHOP_PROMPT);
	
// first create the inverse CMYK to LAB transform

	_create_PS_CMYK_to_Lab(1);
	_normalizeInvTable(1);
	
	waitTillForeground();
		 	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

	tableH = McoNewHandle(sizeof(char)*TABLE_ENTRIES*4);
	if(!tableH)	return MCO_MEM_ALLOC_ERROR;
	table = (char*)McoLockHandle(tableH);
	tableh = table;
	
	ltableH = McoNewHandle(sizeof(char)*CMYK_TABLE_ENTRIES*3);
	if(!ltableH)	return MCO_MEM_ALLOC_ERROR;
	ltable = (char*)McoLockHandle(ltableH);
	ltableh = ltable; 
				
	jobfile = new FileFormat;
	if(!jobfile){
		McoDeleteHandle(tableH);
		return MCO_MEM_ALLOC_ERROR;
	}
		
	status = jobfile->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, MONACO_SIG, '8BST');
	if( status != MCO_SUCCESS ){
		McoDeleteHandle(tableH);
		return status;
	}
	
	// if this is a first pass calibration then create a file with an alias to the table and the info from the 
	// patches file
	
/*	if (pass == 1)
		{
		FileFormat *firstpass_file;
		OSErr iErr;
		AliasHandle aliashandle;
		void *alias;
		int32	new_length;
			
		iErr = NewAlias(nil, &(soutReply.sfFile), &aliashandle);		
		if(iErr != 0)
			return MCO_FAILURE;
		
		new_length = (**aliashandle).aliasSize;
		firstpass_file = new FileFormat;
		firstpass_file->createFileinPref(&fspec,"MonacoPRINT PassOne","MonacoCOLOR 1.51 Scanners");
		
		firstpass_file->relWrite(sizeof(int32),(char *)&new_length);		//alias length
		alias = McoLockHandle((Handle)aliashandle);
		firstpass_file->relWrite(new_length,(char*)alias);		//alias contents
		McoUnlockHandle((Handle)aliashandle);
		firstpass_file->relWrite(sizeof(Str255),(char*)patchFName);	//name of the patch file
		_patch = (double*)McoLockHandle(_patchH);
		firstpass_file->relWrite(sizeof(double)*3*NUM_TOTAL_PATCHES,(char*)_patch); // data from the patch file
		McoUnlockHandle(_patchH);
		firstpass_file->closeFile();
		delete firstpass_file;
		McoDeleteHandle((Handle)aliashandle);
		} */
	
	
	clip = &_clipt[5120];
	_table = (double*)McoLockHandle(_con_to_dev_tableH);
	
	if (_PaperWhite == 100)
		{
		_table[141568] = 0;
		_table[141569] = 0;
		_table[141570] = 0;
		_table[141571] = 0;
		}
	for(i = 0; i < TABLE_ENTRIES; i++){
		//table[0] = ~clip[(short)(_table[0]*255.0)];
		//table[1] = ~clip[(short)(_table[1]*255.0)];
		//table[2] = ~clip[(short)(_table[2]*255.0)];
		//table[3] = ~clip[(short)(_table[3]*255.0)];
		table[0] = ~clip_round(linear_data->CnvtLinearInv(_table[0],0)*2.550);
		table[1] = ~clip_round(linear_data->CnvtLinearInv(_table[1],1)*2.550);
		table[2] = ~clip_round(linear_data->CnvtLinearInv(_table[2],2)*2.550);
		table[3] = ~clip_round(linear_data->CnvtLinearInv(_table[3],3)*2.550);
		table += 4;
		_table += 4;		
	}
	McoUnlockHandle(_con_to_dev_tableH);
	
	
	if (!isPressed(0x3a)) codeTable((unsigned char *)tableh);


	_table = (double*)McoLockHandle(_cmyktableH);
	for(i = 0; i < CMYK_TABLE_ENTRIES; i++){
		ltable[0] = clip_round(_table[0]*2.55);
		ltable[1] = clip_round2(_table[1]*0.87)+128;
		ltable[2] = clip_round2(_table[2]*0.87)+128;
		ltable += 3;
		_table += 3;		
	} 
	McoUnlockHandle(_cmyktableH); 

	//save
	version = 300;
	status = jobfile->relWrite(2, (char *)&version);
	exist = 1;
	status = jobfile->relWrite(1, (char *)&exist);
	exist = 1;
	status = jobfile->relWrite(1, (char *)&exist);
	
	status = jobfile->relWrite(TABLE_ENTRIES*4, (char *)tableh);
	status = jobfile->relWrite(CMYK_TABLE_ENTRIES*3, (char *)ltableh);
	jobfile->closeFile(); 
	delete jobfile; 
	McoDeleteHandle(tableH);
	McoDeleteHandle(ltableH);
	
	return status;
}		

char* hextable[] = {
"00","01","02","03","04","05","06","07",
"08","09","0A","0B","0C","0D","0E","0F",
"10","11","12","13","14","15","16","17",
"18","19","1A","1B","1C","1D","1E","1F",
"20","21","22","23","24","25","26","27",
"28","29","2A","2B","2C","2D","2E","2F",
"30","31","32","33","34","35","36","37",
"38","39","3A","3B","3C","3D","3E","3F",
"40","41","42","43","44","45","46","47",
"48","49","4A","4B","4C","4D","4E","4F",
"50","51","52","53","54","55","56","57",
"58","59","5A","5B","5C","5D","5E","5F",
"60","61","62","63","64","65","66","67",
"68","69","6A","6B","6C","6D","6E","6F",
"70","71","72","73","74","75","76","77",
"78","79","7A","7B","7C","7D","7E","7F",
"80","81","82","83","84","85","86","87",
"88","89","8A","8B","8C","8D","8E","8F",
"90","91","92","93","94","95","96","97",
"98","99","9A","9B","9C","9D","9E","9F",
"A0","A1","A2","A3","A4","A5","A6","A7",
"A8","A9","AA","AB","AC","AD","AE","AF",
"B0","B1","B2","B3","B4","B5","B6","B7",
"B8","B9","BA","BB","BC","BD","BE","BF",
"C0","C1","C2","C3","C4","C5","C6","C7",
"C8","C9","CA","CB","CC","CD","CE","CF",
"D0","D1","D2","D3","D4","D5","D6","D7",
"D8","D9","DA","DB","DC","DD","DE","DF",
"E0","E1","E2","E3","E4","E5","E6","E7",
"E8","E9","EA","EB","EC","ED","EE","EF",
"F0","F1","F2","F3","F4","F5","F6","F7",
"F8","F9","FA","FB","FC","FD","FE","FF"
};

/*
McoStatus ProfileDocMac::savetoCrdtable(void)
{
	McoStatus status;
	StandardFileReply 	soutReply;
	FileFormat 			*jobfile;	
	Str255				prompt = "\pSave the Table File As";
	Str255				defaultfilename = "\p";
	McoHandle			tableH;
	char				*table, *tableh;
	McoHandle			ltableH;
	char				*ltable, *ltableh;	
	unsigned char		*clip;
	long				i, j, k;
	unsigned short		version;
	char *psheader =
	"%!\r% Copyright (c) 1996, Monaco Systems, Inc\r"
	"% Note: This file will execute properly only on a level 2 PostScript printer!\r"
	"% exitserver mode to change the initial VM image\r%\r"
	"% NOTE: The exitserver password is assumed to be 0.  If it has been set to\r"
	"%       any other value by the system administrator then the following line and\r"
	"%       the line at the end of this file must be altered to replace the 0 with\r"
	"%       the correct exitserver password.\r\r"
	"true 0 startjob not {/exitserver errordict /invalidaccess get exec} if\r\r"
	"% Check to see if this patch file has already been executed and skip doing\r"
	"%  anything if it has (applying the patch multiple times would result in\r"
	"%  incorrect behavior).\r\r"
	"userdict /MyOldInstallProc known\r"
	"{\r\t(Patch already installed - Old InstallProc not saved again.\\n)\r"
	"\tprint flush\r}\r"
	"{\r\t% Save the original Install procedure\r"
	"\t/OldInstallProc currentpagedevice /Install get def\r} ifelse\r\r"
	"<< /Install {\r"
	"\tuserdict /OldInstallProc get exec\r\r";

	char *colortable[] = {	
	"<<\r/ColorRenderingType 1\r/WhitePoint [0.96422 1.0 0.82521]\r/BlackPoint [0.0 0.0 0.0]\r",
	"/MatrixPQR [1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0]\r/RangePQR [0.0 1.0 0.0 1.0 0.0 2.0]\r",
	"/TransformPQR [ {4 index 3 get div 2 index 3 get mul 4 {exch pop} repeat} bind\r",
	"{4 index 4 get div 2 index 4 get mul 4 {exch pop} repeat} bind\r",
	"{4 index 5 get div 2 index 5 get mul 4 {exch pop} repeat} bind\r",
	//MatrixLMN is a matrix from intermediate L M N to X Y Z
	//[XL YL ZL XM YM ZM XN YN ZN]
	//X = DL(L)*XL + DM(M)*XM + DN(N)*XN
	//Y = DL(L)*YL + DM(M)*YM + DN(N)*YN
	//Z = DL(L)*ZL + DM(M)*ZM + DN(N)*ZN
	"]\r/MatrixLMN [1.0 0.96422 div 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 0.82521 div]\r",
	"/EncodeLMN [{dup 216 24389 div ge {1 3 div exp} {7.787 mul 16 116 div add} ifelse} bind dup dup]\r",
	//RangeLMN intermediate L M N ranges
	//MatrixABC from A B C (here is L A B) to intermediate L M N
	//L = DA(A)*LA + DB(B)*LB + DC(C)*LC
	//M = DA(A)*MA + DB(B)*MB + DC(C)*MC
	//N = DA(A)*NA + DB(B)*NB + DC(C)*NC
	"/RangeLMN [0.0 1.5 0.0 1.5 0.0 1.5]\r/MatrixABC [0.0 1.0 0.0 1.0 -1.0 1.0 0.0 0.0 -1.0]\r",
	//from XYZ to LAB
	"/EncodeABC [{dup dup dup mul mul 216 24389 div ge {116 mul 16 sub}\r",
	"{16 116 div sub 7.787 div 24389 mul 27 div} ifelse } bind {500 mul} bind {200 mul} bind]\r",
	//LAB range
	"/RangeABC [0 100 -128 127 -128 127]\r/RenderTable [33 33 33 [\r",
	// INSERT TABLE ARRAY HERE 
	"]\r4 {} {} {} {} ]\r>>\r"
	};

	char *pstrailer =
	"setcolorrendering\r} bind\r"
	">> setpagedevice\r\r"	
	"false 0 startjob not {/exitserver errordict /invalidaccess get exec} if\r";

				 	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

	tableH = McoNewHandle(sizeof(char)*TABLE_ENTRIES*4);
	if(!tableH)	return MCO_MEM_ALLOC_ERROR;
	table = (char*)McoLockHandle(tableH);
	tableh = table;	

	jobfile = new FileFormat;
	if(!jobfile){
		McoDeleteHandle(tableH);
		return MCO_MEM_ALLOC_ERROR;
	}
		
	status = jobfile->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, 'MCOS', 'TEXT');
	if( status != MCO_SUCCESS ){
		McoDeleteHandle(tableH);
		return status;
	}

	status = jobfile->relWrite((long)strlen(psheader), psheader);
	status = jobfile->relWrite((long)strlen(colortable[0]), colortable[0]);
	status = jobfile->relWrite((long)strlen(colortable[1]), colortable[1]);
	status = jobfile->relWrite((long)strlen(colortable[2]), colortable[2]);
	status = jobfile->relWrite((long)strlen(colortable[3]), colortable[3]);
	status = jobfile->relWrite((long)strlen(colortable[4]), colortable[4]);
	status = jobfile->relWrite((long)strlen(colortable[5]), colortable[5]);
	status = jobfile->relWrite((long)strlen(colortable[6]), colortable[6]);
	status = jobfile->relWrite((long)strlen(colortable[7]), colortable[7]);
	status = jobfile->relWrite((long)strlen(colortable[8]), colortable[8]);
	status = jobfile->relWrite((long)strlen(colortable[9]), colortable[9]);
	status = jobfile->relWrite((long)strlen(colortable[10]), colortable[10]);
	if( status != MCO_SUCCESS ){
		McoDeleteHandle(tableH);
		jobfile->closeFile();
		delete jobfile;
		return status;
	}	
	
	clip = &_clipt[5120];
	_table = (double*)McoLockHandle(_con_to_dev_tableH);
	for(i = 0; i < TABLE_ENTRIES; i++){
		table[0] = clip[(short)(_table[0]*255.0)];
		table[1] = clip[(short)(_table[1]*255.0)];
		table[2] = clip[(short)(_table[2]*255.0)];
		table[3] = clip[(short)(_table[3]*255.0)];
		table += 4;
		_table += 4;		
	}
	McoUnlockHandle(_con_to_dev_tableH);


	unsigned char		*tem;
	char 		conv[1000];
	char*		convtemp;
	long 		csize;	//channel size
	char*		null = "\r";
	
	tem = (unsigned char *)tableh;
	
	//for some testing for the printer
//	
//	for( i = 0; i < TABLE_ENTRIES*4; i++)
//		*tem++ = 255;
//		
//	tem = (unsigned char *)tableh;

	
	csize = 33*4;
	conv[csize*2] = null[0];
	for( i = 0; i < 33; i++){
		status = jobfile->relWrite(2, "<\r");
		for( j = 0; j < 33; j++){
			convtemp = conv;
			for( k = 0; k < csize; k++){
				memcpy(convtemp, hextable[*tem++], 2);
				convtemp += 2;
			}	
			jobfile->relWrite(csize*2+1, conv);
		}
		jobfile->relWrite(2, ">\r");
	}	
	McoDeleteHandle(tableH);
	
	status = jobfile->relWrite((long)strlen(colortable[11]), colortable[11]);
	status = jobfile->relWrite((long)strlen(pstrailer), pstrailer);
	jobfile->closeFile();
	delete jobfile;
	return MCO_SUCCESS;
}

*/

McoStatus ProfileDocMac::savetoCrdtable(void)
{
	McoStatus status;
	StandardFileReply 	soutReply;
	FILE				*fs;
	FileFormat 			*jobfile;	
	Str255				prompt;
	Str255				defaultfilename = "\p";
	McoHandle			tableH;
	char				*table, *tableh;
	McoHandle			ltableH;
	char				*ltable, *ltableh;	
	unsigned char		*clip;
	long				i, j, k;
	unsigned short		version;
	char *psheader =
	"%!PS-Monaco 2.5\r"
	"%%Cop right Monaco Systems\r"
	"%%ColorRenderingDictionary\r"
	"%%Title: 89XX.PAPER.CMYK.400\r"
	"%%Date Created: 10/10/1997 0:14:26\r"
	"%%Out-of-Gamut Mapping Type: Minimum DeltaE\r"
	"%%Rendering Intent: RelativeColorimetric\r"
	"%%BeginResource: colorrendering 89XX.PAPER.CMYK.400\r";

	char *colortable[] = {	
	//"<<\r/ColorRenderingType 1\r/WhitePoint [0.96422 1.0 0.82521]\r/BlackPoint [0.0 0.0 0.0]\r",
	"<<\r/ColorRenderingType 1\r/WhitePoint [0.9505 1.0 1.089]\r/BlackPoint [0.0 0.0 0.0]\r",
	"/MatrixPQR [1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0]\r/RangePQR [0.0 1.0 0.0 1.0 0.0 2.0]\r",
	"/TransformPQR [ {4 index 3 get div 2 index 3 get mul 4 {exch pop} repeat} bind\r",
	"{4 index 4 get div 2 index 4 get mul 4 {exch pop} repeat} bind\r",
	"{4 index 5 get div 2 index 5 get mul 4 {exch pop} repeat} bind\r",
	//MatrixLMN is a matrix from intermediate L M N to X Y Z
	//[XL YL ZL XM YM ZM XN YN ZN]
	//X = DL(L)*XL + DM(M)*XM + DN(N)*XN
	//Y = DL(L)*YL + DM(M)*YM + DN(N)*YN
	//Z = DL(L)*ZL + DM(M)*ZM + DN(N)*ZN
	//"]\r/MatrixLMN [1.0 0.96422 div 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 0.82521 div]\r",
	"]\r/MatrixLMN [1.0 0.9505 div 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 1.089 div]\r",
	"/EncodeLMN [{dup 216 24389 div ge {1 3 div exp} {7.787 mul 16 116 div add} ifelse} bind dup dup]\r",
	//RangeLMN intermediate L M N ranges
	//MatrixABC from A B C (here is L A B) to intermediate L M N
	//L = DA(A)*LA + DB(B)*LB + DC(C)*LC
	//M = DA(A)*MA + DB(B)*MB + DC(C)*MC
	//N = DA(A)*NA + DB(B)*NB + DC(C)*NC
	"/RangeLMN [0.0 1.5 0.0 1.5 0.0 1.5]\r/MatrixABC [0.0 1.0 0.0 1.0 -1.0 1.0 0.0 0.0 -1.0]\r",
	//from XYZ to LAB
	"/EncodeABC [{dup dup dup mul mul 216 24389 div ge {116 mul 16 sub}\r",
	"{16 116 div sub 7.787 div 24389 mul 27 div} ifelse } bind {500 mul} bind {200 mul} bind]\r",
	//LAB range
	"/RangeABC [0 100 -128 127 -128 127]\r/RenderTable [33 33 33 [\r",
	/* INSERT TABLE ARRAY HERE */
	"]\r4\r"
	};

	char *pstrailer =
	"]\r"
	">> /89XX.PAPER.CMYK.400 exch /ColorRendering defineresource pop\r";

	waitTillForeground();
	
	GetIndString(prompt,PROMPT_STRINGS,CRD_PROMPT);		
				 	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

	tableH = McoNewHandle(sizeof(char)*TABLE_ENTRIES*4);
	if(!tableH)	return MCO_MEM_ALLOC_ERROR;
	table = (char*)McoLockHandle(tableH);
	tableh = table;	

	jobfile = new FileFormat;
	if(!jobfile){
		McoDeleteHandle(tableH);
		return MCO_MEM_ALLOC_ERROR;
	}
		
	status = jobfile->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, 'MCOS', 'TEXT');
	if( status != MCO_SUCCESS ){
		McoDeleteHandle(tableH);
		return status;
	}

	status = jobfile->relWrite((long)strlen(psheader), psheader);
	status = jobfile->relWrite((long)strlen(colortable[0]), colortable[0]);
	status = jobfile->relWrite((long)strlen(colortable[1]), colortable[1]);
	status = jobfile->relWrite((long)strlen(colortable[2]), colortable[2]);
	status = jobfile->relWrite((long)strlen(colortable[3]), colortable[3]);
	status = jobfile->relWrite((long)strlen(colortable[4]), colortable[4]);
	status = jobfile->relWrite((long)strlen(colortable[5]), colortable[5]);
	status = jobfile->relWrite((long)strlen(colortable[6]), colortable[6]);
	status = jobfile->relWrite((long)strlen(colortable[7]), colortable[7]);
	status = jobfile->relWrite((long)strlen(colortable[8]), colortable[8]);
	status = jobfile->relWrite((long)strlen(colortable[9]), colortable[9]);
	status = jobfile->relWrite((long)strlen(colortable[10]), colortable[10]);
	if( status != MCO_SUCCESS ){
		McoDeleteHandle(tableH);
		jobfile->closeFile();
		delete jobfile;
		return status;
	}	
	
	
	clip = &_clipt[5120];
	_table = (double*)McoLockHandle(_con_to_dev_tableH);
	for(i = 0; i < TABLE_ENTRIES; i++){
	
		table[0] = clip[(short)(_table[0]*2.550+0.5)];
		table[1] = clip[(short)(_table[1]*2.550+0.5)];
		table[2] = clip[(short)(_table[2]*2.550+0.5)];
		table[3] = clip[(short)(_table[3]*2.550+0.5)];
	
		table += 4;
		_table += 4;		
	}
	McoUnlockHandle(_con_to_dev_tableH);
	
//	McoUnlockHandle(_con_to_dev_tableH);

//	if (!isPressed(0x3a)) codeTable((unsigned char *)tableh);
	
	unsigned char		*tem;
	char 		conv[1000];
	char*		convtemp;
	long 		csize;	//channel size
	char*		null = "\r";
	
	tem = (unsigned char *)tableh;
	
	
	csize = 33*4;
	conv[csize*2] = null[0];
	for( i = 0; i < 33; i++){
		status = jobfile->relWrite(2, "<\r");
		for( j = 0; j < 33; j++){
			convtemp = conv;
			for( k = 0; k < csize; k++){
				memcpy(convtemp, hextable[*tem++], 2);
				convtemp += 2;
			}	
			jobfile->relWrite(csize*2+1, conv);
		}
		jobfile->relWrite(2, ">\r");
	}	
	McoDeleteHandle(tableH);
	
	jobfile->closeFile();
	delete jobfile;
	
	char filename[256];

	memset(filename, 0x00, 256);
	
	PathNameFromDirID(soutReply.sfFile.parID, soutReply.sfFile.vRefNum, StringPtr(filename));
	pstrcat((StringPtr)filename, soutReply.sfFile.name);
	ptocstr((StringPtr)filename);
		
	fs = fopen(filename, "ab");
	if(!fs)	return MCO_FILE_ERROR;

	
	char *linearheader =
	"{\r"
	"[\r";
	
	char *lineartrailer = 
	"]\r"
	"exch 255 mul get\r"
	"}\r";

	double lineartable[256];
	double t;

	for(k = 0; k < 4; k++){	
		fprintf(fs, linearheader);
	//	getLinear(lineartable, k);

		for(i = 0; i < 16; i++){	
			for(j = 0; j < 16; j++)
				{
				t = (double)(i*16+j)/2.55;
				t = linear_data->CnvtLinearInv(t,k)/100.0;
				fprintf(fs, "%f ", t);
				}
			fprintf(fs, "\r");
		}
		
		fprintf(fs, lineartrailer);	
	}
	
	fprintf(fs, pstrailer);
	
	fclose(fs);
		
	return MCO_SUCCESS;
}


McoStatus ProfileDocMac::savetoCrdtable_old(void)
{
	McoStatus status;
	StandardFileReply 	soutReply;
	FileFormat 			*jobfile;	
	Str255				prompt;
	Str255				defaultfilename = "\p";
	McoHandle			tableH;
	char				*table, *tableh;
	McoHandle			ltableH;
	char				*ltable, *ltableh;	
	unsigned char		*clip;
	long				i, j, k;
	unsigned short		version;
	char *psheader =
	"%!\r% Copyright (c) 1996, Monaco Systems, Inc\r"
	"% Note: This file will execute properly only on a level 2 PostScript printer!\r"
	"% exitserver mode to change the initial VM image\r%\r"
	"% NOTE: The exitserver password is assumed to be 0.  If it has been set to\r"
	"%       any other value by the system administrator then the following line and\r"
	"%       the line at the end of this file must be altered to replace the 0 with\r"
	"%       the correct exitserver password.\r\r"
	"true 0 startjob not {/exitserver errordict /invalidaccess get exec} if\r\r"
	"% Check to see if this patch file has already been executed and skip doing\r"
	"%  anything if it has (applying the patch multiple times would result in\r"
	"%  incorrect behavior).\r\r"
	"userdict /MyOldInstallProc known\r"
	"{\r\t(Patch already installed - Old InstallProc not saved again.\\n)\r"
	"\tprint flush\r}\r"
	"{\r\t% Save the original Install procedure\r"
	"\t/OldInstallProc currentpagedevice /Install get def\r} ifelse\r\r"
	"<< /Install {\r"
	"\tuserdict /OldInstallProc get exec\r\r";

	char *colortable[] = {	
	//"<<\r/ColorRenderingType 1\r/WhitePoint [0.96422 1.0 0.82521]\r/BlackPoint [0.0 0.0 0.0]\r",
	"<<\r/ColorRenderingType 1\r/WhitePoint [0.9505 1.0 1.089]\r/BlackPoint [0.0 0.0 0.0]\r",
	"/MatrixPQR [1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0]\r/RangePQR [0.0 1.0 0.0 1.0 0.0 2.0]\r",
	"/TransformPQR [ {4 index 3 get div 2 index 3 get mul 4 {exch pop} repeat} bind\r",
	"{4 index 4 get div 2 index 4 get mul 4 {exch pop} repeat} bind\r",
	"{4 index 5 get div 2 index 5 get mul 4 {exch pop} repeat} bind\r",
	//MatrixLMN is a matrix from intermediate L M N to X Y Z
	//[XL YL ZL XM YM ZM XN YN ZN]
	//X = DL(L)*XL + DM(M)*XM + DN(N)*XN
	//Y = DL(L)*YL + DM(M)*YM + DN(N)*YN
	//Z = DL(L)*ZL + DM(M)*ZM + DN(N)*ZN
	//"]\r/MatrixLMN [1.0 0.96422 div 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 0.82521 div]\r",
	"]\r/MatrixLMN [1.0 0.9505 div 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 1.089 div]\r",
	"/EncodeLMN [{dup 216 24389 div ge {1 3 div exp} {7.787 mul 16 116 div add} ifelse} bind dup dup]\r",
	//RangeLMN intermediate L M N ranges
	//MatrixABC from A B C (here is L A B) to intermediate L M N
	//L = DA(A)*LA + DB(B)*LB + DC(C)*LC
	//M = DA(A)*MA + DB(B)*MB + DC(C)*MC
	//N = DA(A)*NA + DB(B)*NB + DC(C)*NC
	"/RangeLMN [0.0 1.5 0.0 1.5 0.0 1.5]\r/MatrixABC [0.0 1.0 0.0 1.0 -1.0 1.0 0.0 0.0 -1.0]\r",
	//from XYZ to LAB
	"/EncodeABC [{dup dup dup mul mul 216 24389 div ge {116 mul 16 sub}\r",
	"{16 116 div sub 7.787 div 24389 mul 27 div} ifelse } bind {500 mul} bind {200 mul} bind]\r",
	//LAB range
	"/RangeABC [0 100 -128 127 -128 127]\r/RenderTable [33 33 33 [\r",
	/* INSERT TABLE ARRAY HERE */
	"]\r4 {} {} {} {} ]\r>>\r"
	};

	char *pstrailer =
	"setcolorrendering\r} bind\r"
	">> setpagedevice\r\r"	
	"false 0 startjob not {/exitserver errordict /invalidaccess get exec} if\r";

	waitTillForeground();
	
	GetIndString(prompt,PROMPT_STRINGS,CRD_PROMPT);		
				 	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

	tableH = McoNewHandle(sizeof(char)*TABLE_ENTRIES*4);
	if(!tableH)	return MCO_MEM_ALLOC_ERROR;
	table = (char*)McoLockHandle(tableH);
	tableh = table;	

	jobfile = new FileFormat;
	if(!jobfile){
		McoDeleteHandle(tableH);
		return MCO_MEM_ALLOC_ERROR;
	}
		
	status = jobfile->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, 'MCOS', 'TEXT');
	if( status != MCO_SUCCESS ){
		McoDeleteHandle(tableH);
		return status;
	}

	status = jobfile->relWrite((long)strlen(psheader), psheader);
	status = jobfile->relWrite((long)strlen(colortable[0]), colortable[0]);
	status = jobfile->relWrite((long)strlen(colortable[1]), colortable[1]);
	status = jobfile->relWrite((long)strlen(colortable[2]), colortable[2]);
	status = jobfile->relWrite((long)strlen(colortable[3]), colortable[3]);
	status = jobfile->relWrite((long)strlen(colortable[4]), colortable[4]);
	status = jobfile->relWrite((long)strlen(colortable[5]), colortable[5]);
	status = jobfile->relWrite((long)strlen(colortable[6]), colortable[6]);
	status = jobfile->relWrite((long)strlen(colortable[7]), colortable[7]);
	status = jobfile->relWrite((long)strlen(colortable[8]), colortable[8]);
	status = jobfile->relWrite((long)strlen(colortable[9]), colortable[9]);
	status = jobfile->relWrite((long)strlen(colortable[10]), colortable[10]);
	if( status != MCO_SUCCESS ){
		McoDeleteHandle(tableH);
		jobfile->closeFile();
		delete jobfile;
		return status;
	}	
	
	clip = &_clipt[5120];
	_table = (double*)McoLockHandle(_con_to_dev_tableH);
	for(i = 0; i < TABLE_ENTRIES; i++){
	
		table[0] = clip[(short)(linear_data->CnvtLinearInv(_table[0],0)*2.550+0.5)];
		table[1] = clip[(short)(linear_data->CnvtLinearInv(_table[1],1)*2.550+0.5)];
		table[2] = clip[(short)(linear_data->CnvtLinearInv(_table[2],2)*2.550+0.5)];
		table[3] = clip[(short)(linear_data->CnvtLinearInv(_table[3],3)*2.550+0.5)];
	
		table += 4;
		_table += 4;		
	}
	McoUnlockHandle(_con_to_dev_tableH);

	if (!isPressed(0x3a)) codeTable((unsigned char *)tableh);
	
	unsigned char		*tem;
	char 		conv[1000];
	char*		convtemp;
	long 		csize;	//channel size
	char*		null = "\r";
	
	tem = (unsigned char *)tableh;
	
	//for some testing for the printer
/*	
	for( i = 0; i < TABLE_ENTRIES*4; i++)
		*tem++ = 255;
		
	tem = (unsigned char *)tableh;
*/
	
	csize = 33*4;
	conv[csize*2] = null[0];
	for( i = 0; i < 33; i++){
		status = jobfile->relWrite(2, "<\r");
		for( j = 0; j < 33; j++){
			convtemp = conv;
			for( k = 0; k < csize; k++){
				memcpy(convtemp, hextable[*tem++], 2);
				convtemp += 2;
			}	
			jobfile->relWrite(csize*2+1, conv);
		}
		jobfile->relWrite(2, ">\r");
	}	
	McoDeleteHandle(tableH);
	
	status = jobfile->relWrite((long)strlen(colortable[11]), colortable[11]);
	status = jobfile->relWrite((long)strlen(pstrailer), pstrailer);
	jobfile->closeFile();
	delete jobfile;
	return MCO_SUCCESS;
}


/*McoStatus ProfileDocMac::savetoIcctable(void)
{
	McoStatus 			status;
	StandardFileReply 	soutReply;
	FileFormat 			*jobfile;	
	Str255				prompt = "\pSave the Table File As";
	Str255				defaultfilename = "\p";
	McoHandle			tableH;
	char				*table, *tableh;
	unsigned char		*clip;
	long				i;
	unsigned short		version;
	unsigned char		exist;
	CsFormat	*csformat;
	Ctype _A2B0, _B2A0, _out_of_gamut;

//some init	
	_A2B0.input_channels = 4;
	_A2B0.output_channels = 3;
	_A2B0.grid_points	= 9;
	_A2B0.identity_matrix = 1;	//is it identity matrix 1 == yes

	_B2A0.input_channels = 3;
	_B2A0.output_channels = 4;
	_B2A0.grid_points	= 33;
	_B2A0.identity_matrix = 1;	//is it identity matrix 1 == yes
	
	_out_of_gamut.input_channels = 3;
	_out_of_gamut.output_channels = 1;
	_out_of_gamut.grid_points	= 33;
	_out_of_gamut.identity_matrix = 1;	//is it identity matrix 1 == yes

		 	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

//new con_to_dev table (data is unsigned char in stead of double)
	tableH = McoNewHandle(sizeof(char)*TABLE_ENTRIES*4);
	if(!tableH)	return MCO_MEM_ALLOC_ERROR;
	table = (char*)McoLockHandle(tableH);
	tableh = table;
					
	clip = &_clipt[5120];
	_table = (double*)McoLockHandle(_con_to_dev_tableH);
	for(i = 0; i < TABLE_ENTRIES; i++){
		table[0] = clip[(short)(_table[0]*255.0)];
		table[1] = clip[(short)(_table[1]*255.0)];
		table[2] = clip[(short)(_table[2]*255.0)];
		table[3] = clip[(short)(_table[3]*255.0)];
		table += 4;
		_table += 4;		
	}
	McoUnlockHandle(_con_to_dev_tableH);
	McoUnlockHandle(tableH);

//save table to ICC file
	csformat = new CsFormat();
	if(!csformat){
		McoDeleteHandle(tableH);
		return MCO_MEM_ALLOC_ERROR;
	}
	
	status = csformat->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, 'sync', 'prof');
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
	
	status = csformat->setHeader(cmOutputClass, cmSigLut8Type);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setABTag(1, &_A2B0, _dev_to_con_tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
	status = csformat->setABTag(2, &_B2A0, tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
	status = csformat->setABTag(3, &_out_of_gamut, _out_of_gamutH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
	
	status = csformat->writeTag(1);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
	csformat->closeFile();
	
	delete csformat;
	McoDeleteHandle(tableH);
	
	return status;
}		
*/



McoStatus setUpCType(Ctype *CT)
{
int i,j;
char *dev_con,*con_dev;

	for (i=0; i< 6; i++) 
		{
		CT->input_tables[i] = 0L;
		CT->output_tables[i] = 0L;
		}
	
	for(i = 0; i <CT->input_channels; i++)
		{
		dev_con = new char[256];
		if (!dev_con) return MCO_MEM_ALLOC_ERROR;
		for(j = 0; j < 256; j++){
			dev_con[j] = j;
			}	
		CT->input_tables[i] = dev_con;	
		}
		
	for(i = 0; i < CT->output_channels; i++)
		{
		con_dev = new char[256];
		if (!con_dev) return MCO_MEM_ALLOC_ERROR;
		for(j = 0; j < 256; j++){
			con_dev[j] = j;
			}	
		CT->output_tables[i] = con_dev;	
		}

return MCO_SUCCESS;	
}


void cleanupCType(Ctype *CT)
{
int i;

for (i=0; i< 6; i++) 
	{
	if (CT->input_tables[i]) delete CT->input_tables[i];
	if (CT->output_tables[i]) delete CT->output_tables[i];
	}
}

McoStatus ProfileDocMac::savetoIcctable(void)
{
	McoStatus 			status;
	StandardFileReply 	soutReply;
	FileFormat 			*jobfile;	
	Str255				prompt;
	Str255				defaultfilename = "\p";
	McoHandle			tableH;
	char				*table, *tableh;
	unsigned char		*clip;
	long				i;
	unsigned short		version;
	unsigned char		exist;
	CsFormat	*csformat;
	Ctype _A2B0, _A2B1, _B2A0, _out_of_gamut;
	char 				name[255];
	CM2Header header;
	
	GetIndString(prompt,PROMPT_STRINGS,ICC_PROMPT);

//some init	
	_A2B0.input_channels = 4;
	_A2B0.output_channels = 3;
	_A2B0.grid_points	= _rev_table_grid;
	_A2B0.identity_matrix = 1;	//is it identity matrix 1 == yes
	_A2B0.input_entries = 256;
	_A2B0.output_entries = 256;

	setUpCType(&_A2B0);
	buildInverseCuvre(&_A2B0);
		
		
	_A2B1.input_channels = 4;
	_A2B1.output_channels = 3;
	_A2B1.grid_points	= _rev_table_grid;
	_A2B1.identity_matrix = 1;	//is it identity matrix 1 == yes
	_A2B1.input_entries = 256;
	_A2B1.output_entries = 256;

	setUpCType(&_A2B1);
	buildInverseCuvre(&_A2B1);

	_B2A0.input_channels = 3;
	_B2A0.output_channels = 4;
	_B2A0.grid_points	= 33;
	_B2A0.identity_matrix = 1;	//is it identity matrix 1 == yes
	_B2A0.input_entries = 256;
	_B2A0.output_entries = 256;

	setUpCType(&_B2A0);
	buildOutputCuvre(&_B2A0);
	
	_out_of_gamut.input_channels = 3;
	_out_of_gamut.output_channels = 1;
	_out_of_gamut.grid_points	= 33;
	_out_of_gamut.identity_matrix = 1;	//is it identity matrix 1 == yes
	_out_of_gamut.input_entries = 256;
	_out_of_gamut.output_entries = 256;
	
	setUpCType(&_out_of_gamut);
	
	waitTillForeground();

		 	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

//new con_to_dev table (data is unsigned char in stead of double)
	tableH = McoNewHandle(sizeof(char)*TABLE_ENTRIES*4);
	if(!tableH)	return MCO_MEM_ALLOC_ERROR;
	table = (char*)McoLockHandle(tableH);
	tableh = table;
					
	clip = &_clipt[5120];
	_table = (double*)McoLockHandle(_con_to_dev_tableH);
	//white point is at 141568
	
	if (_PaperWhite == 100)
		{
		_table[141568] = 0;
		_table[141569] = 0;
		_table[141570] = 0;
		_table[141571] = 0;
		}
	
	for(i = 0; i < TABLE_ENTRIES; i++){
		table[0] = clip[(short)(_table[0]*2.550+0.5)];
		table[1] = clip[(short)(_table[1]*2.550+0.5)];
		table[2] = clip[(short)(_table[2]*2.550+0.5)];
		table[3] = clip[(short)(_table[3]*2.550+0.5)];
		table += 4;
		_table += 4;		
	}

	
	if (!isPressed(0x3a)) codeTable((unsigned char *)tableh);
		
	McoUnlockHandle(_con_to_dev_tableH);
	McoUnlockHandle(tableH);

//save table to ICC file
	csformat = new CsFormat();
	if(!csformat){
		McoDeleteHandle(tableH);
		return MCO_MEM_ALLOC_ERROR;
	}
	
	status = csformat->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, MONACO_SIG, 'prof');
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
	
//set header stuff
	header.CMMType = MONACO_CMM;
	header.profileVersion = (MCOP_MAJOR_VERSION + MCOP_MINOR_VERSION);
	header.profileClass = cmOutputClass;
	header.dataColorSpace = cmCMYKData;
	header.profileConnectionSpace = MCOP_DEFAULT_CON_SPACE;
		
	header.CS2profileSignature = cmMagicNumber;	//?
	header.platform = MCOP_DEFAULT_PLATFORM;
	header.flags = MCOP_DEFAULT_FLAGS;
	header.deviceAttributes[0] = MCOP_DEFAULT_ATTRIBUTES;
	header.deviceAttributes[1] = MCOP_DEFAULT_ATTRIBUTES;
	header.renderingIntent = MCOP_DEFAULT_RENDERINGINTENT;
	header.white.X = MCOP_DEFAULT_WHITE_X;
	header.white.Y = MCOP_DEFAULT_WHITE_Y;
	header.white.Z = MCOP_DEFAULT_WHITE_Z;

	status = csformat->setHeader(&header, cmSigLut8Type);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	ptocstr(soutReply.sfFile.name);
	strcpy(name,(char*)soutReply.sfFile.name);
	strcat(name,MCOP_IDF);
	status = csformat->setDesctypedata(name, strlen(name)+1);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

//modified here to fix the cmyk 0 to cmyk 0
	unsigned char* fix;

	if(_condata.ink_linerize == 1){	
		fix = (unsigned char*)McoLockHandle(_dev_to_con_tableH2);
		fix[0] = 255;
		McoUnlockHandle(_dev_to_con_tableH2);
	}
	
	if(_condata.ink_neutralize == 1){	
		fix = (unsigned char*)McoLockHandle(_dev_to_con_tableH2);
		fix[1] = 128;
		fix[2] = 128;
		McoUnlockHandle(_dev_to_con_tableH2);
	}
	
	if((_condata.ink_linerize == 1) || (_condata.ink_highlight))
		status = csformat->setLuttypedata(1, &_A2B0, _dev_to_con_tableH2);
	else
		status = csformat->setLuttypedata(1, &_A2B0, _dev_to_con_tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
		
	status = csformat->setLuttypedata(2, &_B2A0, tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(5, &_B2A0, tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(7, &_B2A0, tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setLuttypedata(3, &_out_of_gamut, _out_of_gamutH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	if(_condata.ink_linerize == 0){
		status = csformat->setLuttypedata(4, &_A2B1, _dev_to_con_tableH2);
		if(status != MCO_SUCCESS){
			McoDeleteHandle(tableH);
			return status;
		}
	}	
	else{
		status = csformat->setLuttypedata(4, &_A2B1, _dev_to_con_tableH);
		if(status != MCO_SUCCESS){
			McoDeleteHandle(tableH);
			return status;
		}
	}

	status = csformat->setLuttypedata(6, &_A2B1, _dev_to_con_tableH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	CMXYZType white;

	white.typeDescriptor = cmMediaWhitePointTag;
	white.reserved = 0x00000000;
	white.XYZ[0].X = MCOP_DEFAULT_WHITE_X;
	white.XYZ[0].Y = MCOP_DEFAULT_WHITE_Y;
	white.XYZ[0].Z = MCOP_DEFAULT_WHITE_Z;

	status = csformat->setXYZtypedata(cmMediaWhitePointTag, &white);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->setCprttypedata(MCOP_CPRT, strlen(MCOP_CPRT));
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}

	status = csformat->writeTag(1);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(tableH);
		return status;
	}
	csformat->closeFile();
	
	delete csformat;
	McoDeleteHandle(tableH);
	
	cleanupCType(&_A2B0);
	cleanupCType(&_A2B1);
	cleanupCType(&_B2A0);
	cleanupCType(&_out_of_gamut);
	
	
	return status;
}		


#ifndef IN_POLAROID

McoStatus ProfileDocMac::savetoIccLinktable(void)
{
	McoStatus 			status;
	CsFormat*	link;
	StandardFileReply 	soutReply;
	Str255				prompt;
	Str255				defaultfilename = "\p";
	Ctype 				A2B0;
	McoHandle			linkH;
	char 				name[255];
	CM2Header 			header;
	
	GetIndString(prompt,PROMPT_STRINGS,LINK_PROMPT);
	
	A2B0.input_channels = 4;
	A2B0.output_channels = 4;
	A2B0.grid_points	= _rev_table_grid;
	A2B0.identity_matrix = 1;	//is it identity matrix 1 == yes
	A2B0.input_entries = 256;
	A2B0.output_entries = 256;
	
	setUpCType(&A2B0);
	buildOutputCuvre(&A2B0);
	buildInputCuvre(&A2B0);

	linkH = McoNewHandle(sizeof(char)*_rev_table_size*4);
	if(!linkH)	return MCO_MEM_ALLOC_ERROR;	

//new con_to_dev table (data is unsigned char in stead of double)
	status = _createLinktable(linkH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(linkH);
		return status;
	}
	
	waitTillForeground();

	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

	link = new CsFormat();
	if(!link)
		return MCO_MEM_ALLOC_ERROR;

	status = link->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, MONACO_SIG, 'prof');
	if(status != MCO_SUCCESS){
		return status;
	}
	
//set header stuff
	header.CMMType = MONACO_CMM;
	header.profileVersion = (MCOP_MAJOR_VERSION + MCOP_MINOR_VERSION);
	header.profileClass = cmLinkClass;
	header.dataColorSpace = cmCMYKData;
	header.profileConnectionSpace = cmCMYKData;
		
	header.CS2profileSignature = cmMagicNumber;	//?
	header.platform = MCOP_DEFAULT_PLATFORM;
	header.flags = MCOP_DEFAULT_FLAGS;
	header.deviceAttributes[0] = MCOP_DEFAULT_ATTRIBUTES;
	header.deviceAttributes[1] = MCOP_DEFAULT_ATTRIBUTES;
	header.renderingIntent = MCOP_DEFAULT_RENDERINGINTENT;
	header.white.X = MCOP_DEFAULT_WHITE_X;
	header.white.Y = MCOP_DEFAULT_WHITE_Y;
	header.white.Z = MCOP_DEFAULT_WHITE_Z;
	
	status = link->setHeader(&header, cmSigLut8Type);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(linkH);
		return status;
	}

	ptocstr(soutReply.sfFile.name);
	strcpy(name,(char*)soutReply.sfFile.name);
	strcat(name,MCOP_IDF);
	status = link->setDesctypedata(name, strlen(name)+1);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(linkH);
		return status;
	}

	status = link->setCprttypedata(MCOP_CPRT, strlen(MCOP_CPRT));
	if(status != MCO_SUCCESS){
		McoDeleteHandle(linkH);
		return status;
	}

	status = link->setPseqtypedata(2);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(linkH);
		return status;
	}
	
	status = link->setLuttypedata(1, &A2B0, linkH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(linkH);
		return status;
	}
	
	status = link->writeTag(1);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(linkH);
		return status;
	}
	link->closeFile();

	delete link;
	McoDeleteHandle(linkH);
	
	cleanupCType(&A2B0);

	return MCO_SUCCESS;
}

#else

McoStatus ProfileDocMac::savetoIccLinktable(void)
{
	McoStatus 			status;

	McoHandle			linkH = 0L;
	char 				name[255];
	CM2Header 			header;
	Mpfileio			*mpfileio = 0L;
		

	linkH = McoNewHandle(sizeof(char)*_rev_table_size*4);
	if(!linkH)	return MCO_MEM_ALLOC_ERROR;	

//new con_to_dev table (data is unsigned char in stead of double)
	status = _createLinktable(linkH);
	if(status != MCO_SUCCESS) goto bail;

	
	printData->tableH = linkH;
	
	
	
	mpfileio = new Mpfileio(printData);
	status = mpfileio->saveProfile(&icc_fsspec);
	if (status) goto bail;
	
	status = _cleanupSafeSave();
	
bail:	
	
	if (linkH) McoDeleteHandle(linkH);
	printData->tableH = 0L;
	
	if (mpfileio) delete mpfileio;
	return status;
}

#endif


McoStatus ProfileDocMac::saveToneAndLinear(void)
{
McoStatus 			status;
Mpfileio			*mpfileio;

mpfileio = new Mpfileio(printData);

printData->tableH = 0L;

status = mpfileio->saveProfile(&icc_fsspec);
if (status) return status;
status = _cleanupSafeSave();

delete mpfileio;

return status;
}

// set up for saving safely
McoStatus ProfileDocMac::_setupSafeSave(void)
{
	OSErr	error;
	unsigned long seconds;
	Str255 tempFileName;
	short tempVRefNum;
	long tempDirID;
	FSSpec tempFSSpec;
	
	if (DoSafeSave)
		{
		// make up a temporary filename
		GetDateTime(&seconds);
		NumToString(seconds, tempFileName);

		// find the temporary folder;
		// create it if necessary
		error = FindFolder(icc_fsspec.vRefNum,
				kTemporaryFolderType, kCreateFolder,
				&tempVRefNum, &tempDirID);
				
		if (error) return MCO_FILE_OPEN_ERROR;
		// make an FSSpec for the
		// temporary filename
		error = FSMakeFSSpec(tempVRefNum, tempDirID,
					tempFileName, &tempFSSpec);
					
		if (error) return MCO_FILE_OPEN_ERROR;
		
		// copy the FSSpec
		safe_fsspec.vRefNum = icc_fsspec.vRefNum;
		safe_fsspec.parID = icc_fsspec.parID;
		copy_str(safe_fsspec.name,icc_fsspec.name);
		
		icc_fsspec.vRefNum = tempFSSpec.vRefNum;
		icc_fsspec.parID = tempFSSpec.parID;
		copy_str(icc_fsspec.name,tempFSSpec.name);
		}
		
	return MCO_SUCCESS;
}
	
// exchange contents of the two files
McoStatus ProfileDocMac::_cleanupSafeSave(void)
{
	OSErr error;


	if (DoSafeSave)
		{	
		// exchange the files	
		error = FSpExchangeFiles(&safe_fsspec,&icc_fsspec);
		if (error) return MCO_FAILURE;
		
		// delete the old file
		error = FSpDelete(&safe_fsspec);
		if (error) return MCO_FAILURE;
		}
	return MCO_SUCCESS;
}

// copy the current icc file into the specified file and
// change the current file pointer to point to the new file
McoStatus ProfileDocMac::duplicateFile(FSSpec *new_fsspec)
{
McoStatus status = MCO_SUCCESS;
FileFormat	file1,file2;
int32	length,count,write;
char 	*buf;


status = file1.openFile(&icc_fsspec);
if (status) return status;

icc_fsspec.vRefNum = new_fsspec->vRefNum;
icc_fsspec.parID = new_fsspec->parID;
copy_str(icc_fsspec.name,new_fsspec->name);

if (file2.doesFileExist(&icc_fsspec)) DoSafeSave = 1;
else  DoSafeSave = 0;

status = _setupSafeSave();
if (status) goto close2;

status = file2.createFilewithtype(&icc_fsspec,smSystemScript, MONACO_SIG, 'prof');
if (status) goto close1;
status = file1.getFilelength(&length);
if (status) goto close2;
count = length;

buf = new char[4096];
if (!buf) { status = MCO_MEM_ALLOC_ERROR; goto close2;}

while (count > 0)
	{
	write = MinVal(4096,count);
	status = file1.relRead(write,buf);
	if (status) goto close2;
	status = file2.relWrite(write,buf);
	if (status) goto close2;
	count -= write;
	}


close2:
file2.closeFile();
close1:
file1.closeFile();
if (buf) delete buf;
return status;
}

McoStatus ProfileDocMac::savetoTekCmyktable(void)
{
	McoStatus status;
	StandardFileReply 	soutReply;
	FileFormat 			*jobfile;	
	Str255				prompt;
	Str255				defaultfilename = "\p";
	McoHandle			linkH, tableH;
	char				*table, *tableh;
	McoHandle			ltableH;
	char				*ltable, *ltableh;	
	unsigned char		*clip;
	long				i, j, k, l;
	unsigned short		version;
	char *psheader =
	"%!\r% Copyright (c) 1996, Monaco Systems, Inc\r"
	"% Note: This file will execute properly only on a level 2 PostScript printer!\r"
	"% exitserver mode to change the initial VM image\r%\r"
	"% NOTE: The exitserver password is assumed to be 0.  If it has been set to\r"
	"%       any other value by the system administrator then the following line and\r"
	"%       the line at the end of this file must be altered to replace the 0 with\r"
	"%       the correct exitserver password.\r\r"
	"true 0 startjob not {/exitserver errordict /invalidaccess get exec} if\r\r"
	"<<\t/DeviceRenderingInfo\r"
	"\t<<\t/Type 2\r"
	"\t\t/RenderingMethod 1\r"
	"\t\t/AdjustCIEColors false\r"
	"\t\t/VirtualColorDevice\r"
	"\t\t<<\t/Type 2\r"
	"\t\t\t/SourceColorModel /CMYK\r";
	
	char *psheader2by17 =
	"\t\t\t/PrimaryInterpolationTableSize [17 17 17 17]\r"
	"\t\t\t/PrimaryInterpolationTableWidth 4\r"
	"\t\t\t/PrimaryInterpolationTable \r"
	"\t\t\t[\r";

	char *psheader2by9 =
	"\t\t\t/PrimaryInterpolationTableSize [9 9 9 9]\r"
	"\t\t\t/PrimaryInterpolationTableWidth 4\r"
	"\t\t\t/PrimaryInterpolationTable \r"
	"\t\t\t[\r";

	
	char *pstrailer =
	"]\r"
	"\t\t>>\r"
	"\t>>\r"
	">> setpagedevice\r\r"
	"false (0) startjob not {/exitserver errordict /invalidaccess get exec} if\r";

	waitTillForeground();

	GetIndString(prompt,PROMPT_STRINGS,LINK_PROMPT);

	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(!soutReply.sfGood)
		return MCO_CANCEL;

	jobfile = new FileFormat;
	if(!jobfile){
		return MCO_MEM_ALLOC_ERROR;
	}
		
	status = jobfile->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, 'MCOS', 'TEXT');
	if( status != MCO_SUCCESS ){
		return status;
	}

	status = jobfile->relWrite((long)strlen(psheader), psheader);
	if( status != MCO_SUCCESS ){
		jobfile->closeFile();
		delete jobfile;
		return status;
	}

	if(_rev_table_grid == 9){
		status = jobfile->relWrite((long)strlen(psheader2by9), psheader2by9);
		if( status != MCO_SUCCESS ){
			jobfile->closeFile();
			delete jobfile;
			return status;
		}
	}
	else{
		status = jobfile->relWrite((long)strlen(psheader2by17), psheader2by17);
		if( status != MCO_SUCCESS ){
			jobfile->closeFile();
			delete jobfile;
			return status;
		}		
	}
	
//create cmyk to cmyk table	
	linkH = McoNewHandle(sizeof(char)*_rev_table_size*4);
	if(!linkH)	return MCO_MEM_ALLOC_ERROR;	

	status = _createLinktable(linkH);
	if(status != MCO_SUCCESS){
		McoDeleteHandle(linkH);
		return status;
	}

//here for test only
//	saveCMYKTiff(linkH, 81, 81);
//end of testing	
	
	table = (char*)McoLockHandle(linkH);

//here for testing only
/*
	char* testtable;
	testtable = table;
	for( i = 0; i < _rev_table_size*4; i++)
		*testtable++ = 0;
*/	
	
	unsigned char		*tem;
	char 		conv[1000];
	char*		convtemp;
	long 		csize;	//channel size
	char*		null = "\r";
	
	tem = (unsigned char *)table;
	
	long step;
	
	step = _rev_table_grid;
	csize = step*4;
	conv[csize*2] = null[0];
	for( i = 0; i < step; i++){
		status = jobfile->relWrite(2, "<\r");
		for( j = 0; j < step; j++){
			for( k = 0; k < step; k++){
				convtemp = conv;
				for(l = 0; l < csize; l++){
					memcpy(convtemp, hextable[*tem++], 2);
					convtemp += 2;
				}
				jobfile->relWrite(csize*2+1, conv);
			}	
		}
		jobfile->relWrite(2, ">\r");
	}	
	
	McoUnlockHandle(linkH);
	McoDeleteHandle(linkH);

	status = jobfile->relWrite((long)strlen(pstrailer), pstrailer);
	jobfile->closeFile();
	delete jobfile;
	return MCO_SUCCESS;
}

int	ProfileDocMac::validFile(void)
{
return valid_fsspec;
}
