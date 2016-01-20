//////////////////////////////////////////////////////////////////////////////////
//  patchset.cpp																//
//  load a set of patch definitions in 											//
//																				//
//  James Vogh																	//
//  Nov 19, 1997																//
//////////////////////////////////////////////////////////////////////////////////

#include "patchset.h"

#include "pascal_string.h"
#include "fullname.h"
#include "monacoprint.h"
#include "profileDocinf.h"
#include "think_bugs.h"

PatchSet::PatchSet(void)
{
	int i;
	short num_files;
 	OSErr sysErr = noErr;
 	OSErr	myErr;
	Str32	filename;
	CInfoPBRec	cipbr;
	DirInfo		*dpb = (DirInfo *)&cipbr;
	int16		idx = 1;
	int default_patch = 0;
	
	
	HFileInfo	myCPB;
	short	myVRef;	
	long	myDirID;
	char tempname[60];
	
	filenum = 0;
	
	for (i=0; i<MAX_PATCH_NAMES; i++) 
		{
		patches[i] = NULL;
		}

 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);

//find dir 		
  	myCPB.ioFDirIndex = 0;
 	myCPB.ioDirID = myDirID;
 	myCPB.ioNamePtr = MONACO_FOLDER;
 	myCPB.ioVRefNum = myVRef;
 	
 	myErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);
 
 	sysErr = MCO_FILE_OPEN_ERROR;
 	if (myErr == noErr) {
 		if(((myCPB.ioFlAttrib >> 4) & 0x01) == 1) sysErr = 0;
 	}



//find subdir
	if(sysErr == 0){
	  	myCPB.ioFDirIndex = 0;
	 	myCPB.ioNamePtr = MONACO_PATCHES;
	 	myCPB.ioVRefNum = myVRef;
	 	
	 	myErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);
	 
	 	sysErr = MCO_FILE_OPEN_ERROR;
	 	if (myErr == noErr) {
	 		if(((myCPB.ioFlAttrib >> 4) & 0x01) == 1) sysErr = 0;
	 	}
	}
	
#ifdef IN_POLAROID	
	filenum = 1;
	//add anything in the reference   	
	//make a copy of all the files that need to be processed  
 	//check whether folder is empty
 	if(sysErr == 0){
		while(1){
		    dpb->ioNamePtr = filename;
		    dpb->ioVRefNum = myVRef;
		    dpb->ioDrDirID = myCPB.ioDirID;
		    dpb->ioFDirIndex = idx++;
		   	sysErr = PBGetCatInfo(&cipbr, FALSE);
		    if( sysErr == fnfErr )
		    	break; //go out of the loop	
		    	  
		    
		     if(!(dpb->ioFlAttrib & 16)) 
		     	{
		     	if (cmp_str(filename,DEFAULT_PATCH))
		     		{
		     		 memcpy(patchames[0], filename, 33);
		     		_loadpatchfile(0,myCPB.ioDirID,myVRef,filename);
		     		default_patch = 1;
		     		}
		     	else 
		     		{
		     		 memcpy(patchames[filenum], filename, 33);
		     		_loadpatchfile(filenum,myCPB.ioDirID,myVRef,filename);  
		     		filenum++;
		     		}
		     	} 
		    
		    
		    if (filenum == MAX_PATCH_NAMES) break;
		    	    	
		}
	
	
	}
	if (!default_patch) _setDefaultPatch(0);
#else
	filenum = 0;
	//add anything in the reference   	
	//make a copy of all the files that need to be processed  
 	//check whether folder is empty
 	if(sysErr == 0){
		while(1){
		    dpb->ioNamePtr = filename;
		    dpb->ioVRefNum = myVRef;
		    dpb->ioDrDirID = myCPB.ioDirID;
		    dpb->ioFDirIndex = idx++;
		   	sysErr = PBGetCatInfo(&cipbr, FALSE);
		    if( sysErr == fnfErr )
		    	break; //go out of the loop	
		    	
		    memcpy(patchames[filenum], filename, 33);
		  
		    
		     if(!(dpb->ioFlAttrib & 16)) 
		     	{
		     	_loadpatchfile(filenum,myCPB.ioDirID,myVRef,filename);  
		     	} 
		    
		    filenum++;
		    if (filenum == MAX_PATCH_NAMES) break;
		    	    	
		}
		
	}	
#endif	
}


PatchSet::~PatchSet(void)
{
int i;
for (i=0; i<MAX_PATCH_NAMES; i++) if (patches[i] != NULL) delete patches[i];
}

// if default file can not be found then set up the default 
McoStatus PatchSet::_setDefaultPatch(int filenum)
{
McoStatus state;
int i,j,m;
#ifdef IN_POLAROID

long dims[] = {9, 6, 5, 5, 4, 4, 4};
double slowVals[] = {0, 20, 40, 60, 80, 90, 100};
double vals[] = {0, 10, 22, 34, 47, 60, 73, 86, 100,
				 0, 20, 40, 60, 80, 100,
				 0, 25, 50, 75, 100,
				 0, 25, 50, 75, 100,
				 0, 33, 66, 100,
				 0, 33, 66, 100,
				 0, 33, 66, 100 };
double linvals[] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100 };
int 	numslow = 7;
int		numlinear = 26;
int		type = 1;
int		numc = 4;

patches[filenum] = new RawData;
if (!patches[filenum]) return MCO_MEM_ALLOC_ERROR;
patches[filenum]->patches.strips = 0;
patches[filenum]->patches.sheets = 100000;
patches[filenum]->patches.rows = 36;
patches[filenum]->patches.columns = 42;
patches[filenum]->patches.starting = 0;
patches[filenum]->patches.direction = 1;
patches[filenum]->patches.width = 252.3;
patches[filenum]->patches.height = 216.3;

#else

long dims[] = {9, 6, 5, 5, 5, 4};
double slowVals[] = {0, 20, 40, 60, 80, 100};
double vals[] = {0, 10, 22, 34, 47, 60, 73, 86, 100,
				 0, 20, 40, 60, 80, 100,
				 0, 25, 50, 75, 100,
				 0, 25, 50, 75, 100,
				 0, 25, 50, 75, 100,
				 0, 33, 66, 100 };
double linvals[] = {0, 7, 13, 20, 27, 33, 40, 47, 53, 60, 67, 73, 80, 87, 93, 100 };
int 	numslow = 6;
int		numlinear = 16;
int		type = 1;
int		numc = 4;

patches[filenum] = new RawData;
if (!patches[filenum]) return MCO_MEM_ALLOC_ERROR;
patches[filenum]->patches.strips = 0;
patches[filenum]->patches.sheets = 100000;
patches[filenum]->patches.rows = 32;
patches[filenum]->patches.columns = 46;
patches[filenum]->patches.starting = 0;
patches[filenum]->patches.direction = 1;
patches[filenum]->patches.width = 292.1;
patches[filenum]->patches.height = 203.7;

#endif

state = patches[filenum]->SetUp(type,numc,numslow,numlinear,dims);
if (state) return state;

for (i=0; i<numslow; i++)
	patches[filenum]->format.slowValues[i] = slowVals[i];

	
m = 0;	
for (i=0; i<numslow; i++) 
	{
	for (j=0; j<patches[filenum]->format.cubedim[i]; j++)
		{
		patches[filenum]->format.cmpValues[m] = vals[m];
		m++;
		}
	}
		
for (i=0; i<patches[filenum]->format.numLinear; i++)
	{
	patches[filenum]->format.linearValues[i] = linvals[i];
	}

memcpy(patchames[filenum], DEFAULT_PATCH, 33);	
		
return MCO_SUCCESS;
}



McoStatus PatchSet::_loadpatchfile(int32 filenum,long ioDirID,short myVRef,unsigned char *filename)
{
	FILE 	*fs = NULL;
	Str255	inname;
	int32		strips,sheets,rows,columns,starting,direction;
	int32				type,numc,numslow,numlinear;
	McoStatus	state = MCO_SUCCESS;
	int32				*dims = NULL;	
	short		i,j,k,m;
	double		width,height;
	
		
	memcpy(inname, filename,33);  
	
	
	PathNameFromDirID(ioDirID, myVRef, inname);
	ptocstr(inname);
	
	fs = fopen( (char*)inname, "r" );
	if(!fs)	{state = MCO_FAILURE; goto bail;}
	
	
	if (!feof(fs)) fscanf(fs,"%ld %ld %ld %ld %ld %ld %lf %lf",&strips,&sheets,&rows,&columns,&starting,&direction,&width,&height);
	else {state = MCO_FAILURE; goto bail;}
	
	if ((strips < 0) || (strips > 1)) {state = MCO_FAILURE; goto bail;}
	if ((starting < 0) || (starting > 3)) {state = MCO_FAILURE; goto bail;}
	if ((direction < 0) || (direction > 1)) {state = MCO_FAILURE; goto bail;}

	
	patches[filenum] = new RawData;
	patches[filenum]->patches.strips = strips;
	patches[filenum]->patches.sheets = sheets;
	patches[filenum]->patches.rows = rows;
	patches[filenum]->patches.columns = columns;
	patches[filenum]->patches.starting = starting;
	patches[filenum]->patches.direction = direction;
	patches[filenum]->patches.width = width;
	patches[filenum]->patches.height = height;
	
	if (!feof(fs)) fscanf(fs, "%ld %ld %ld %ld",&type,&numc,&numslow,&numlinear);
	else {state = MCO_FAILURE; goto bail;}
	
	dims = new int32 [numslow];
	
	for (i=0; i<numslow; i++)
		fscanf(fs, "%ld",&dims[i]);
	
	
	state = patches[filenum]->SetUp(type,numc,numslow,numlinear,dims);
	if (state != MCO_SUCCESS)  goto bail;
	
	for (i=0; i<numslow; i++)
		if (!feof(fs)) fscanf(fs, "%lf",&patches[filenum]->format.slowValues[i]);
		else {state = MCO_FAILURE; goto bail;}
	
	m = 0;	
	for (i=0; i<numslow; i++) 
		{
		for (j=0; j<patches[filenum]->format.cubedim[i]; j++)
			{
			if (!feof(fs)) fscanf(fs, "%lf",&patches[filenum]->format.cmpValues[m++]);
			else {state = MCO_FAILURE; goto bail;}
			}
		}
		
	for (i=0; i<patches[filenum]->format.numLinear; i++)
		{
		if (!feof(fs)) fscanf(fs,"%lf",&patches[filenum]->format.linearValues[i]);
		else {state = MCO_FAILURE; goto bail;}
		}
	
bail:
	if (fs) fclose(fs);
	if (dims) delete dims;
	
	if ((state != MCO_SUCCESS) && ( patches[filenum])) delete patches[filenum];
	return state;
}

