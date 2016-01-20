#include "mpfileio.h"

Mpfileio::Mpfileio(PrintData* printdata)
{
	_printdata = printdata;
}

Mpfileio::~Mpfileio()
{
}

void Mpfileio::cleanup(long which)
{
int i;

	// 0 = destination patchdata
	// 1 = source patchdata
	// 2 = everything
	// 3 = linear
	// 4 = tone
	// 5 = tweak data

	if ((which == 1) || (which == 2)){

		if(_printdata->srcpatch.steps){
			delete 	[]_printdata->srcpatch.steps;
			_printdata->srcpatch.steps = 0;
		}
		if(_printdata->srcpatch.blacks){
			delete 	[]_printdata->srcpatch.blacks;
			_printdata->srcpatch.blacks = 0;
		}
		if(_printdata->srcpatch.cmy){
			delete []_printdata->srcpatch.cmy;
			_printdata->srcpatch.cmy = 0;
		}	
		if(_printdata->srcpatch.linear){
			delete []_printdata->srcpatch.linear;
			_printdata->srcpatch.linear = 0;
		}	
		if(_printdata->srcpatch.data){
			delete []_printdata->srcpatch.data;
			_printdata->srcpatch.data = 0;
		}	
		if(_printdata->srcpatch.ldata){
			delete []_printdata->srcpatch.ldata;
			_printdata->srcpatch.ldata = 0;
		}	
	}

	if ((which == 0) || (which == 2)) {
		
		if(_printdata->destpatch.steps){
			delete 	[]_printdata->destpatch.steps;
			_printdata->destpatch.steps = 0;
		}
		if(_printdata->destpatch.blacks){
			delete 	[]_printdata->destpatch.blacks;
			_printdata->destpatch.blacks = 0;
		}
		if(_printdata->destpatch.cmy){
			delete []_printdata->destpatch.cmy;
			_printdata->destpatch.cmy = 0;
		}	
		if(_printdata->destpatch.linear){
			delete []_printdata->destpatch.linear;
			_printdata->destpatch.linear = 0;
		}	
		if(_printdata->destpatch.data){
			delete []_printdata->destpatch.data;
			_printdata->destpatch.data = 0;
		}	
		if(_printdata->destpatch.ldata){
			delete []_printdata->destpatch.ldata;
			_printdata->destpatch.ldata = 0;
		}
	}	

	if ((which == 3) || (which == 2))
		{
		for (i=0; i<6; i++)
			{
			if (_printdata->linear[i]) delete _printdata->linear[i];
			_printdata->linear[i] = 0L;
			}
		}
		
	if ((which == 4) || (which == 2))
		{
		// delete linear and tone data
		for (i=0; i<6; i++)
			{
			if (_printdata->tone[i]) delete _printdata->tone[i];
			_printdata->tone[i] = 0L;
			}
		}
		
	if ((which == 5) || (which == 2))
		{
		// delete the tweakele 
		if (_printdata->tweakele) delete _printdata->tweakele;
		_printdata->tweakele = 0L;
		}
	
	if(which == 2){
		if(_printdata->tableH){
			McoDeleteHandle(_printdata->tableH);
			_printdata->tableH = 0L;
		}
		_printdata->A2B0.input_channels = 0;
		_printdata->A2B0.output_channels = 0;
		_printdata->A2B0.grid_points = 0;
		_printdata->A2B0.identity_matrix = 1;
		_printdata->A2B0.input_entries = 0;
		_printdata->A2B0.output_entries = 0;
		_printdata->A2B0.offset = 0;
		
		for(i = 0; i < 6; i++){	
			if(_printdata->A2B0.input_tables[i]){
				delete []_printdata->A2B0.input_tables[i];
				_printdata->A2B0.input_tables[i] = 0L;
			}	
			if(_printdata->A2B0.output_tables[i]){
				delete []_printdata->A2B0.output_tables[i];
				_printdata->A2B0.output_tables[i] = 0L;
			}
		}		
	}
	
}

#if defined(_WIN32)

McoStatus Mpfileio::openProfile(char *fsspec)
{
	McoStatus 	status;
	CsFormat icc;

	if( (status = icc.createFile(fsspec, _O_BINARY | _O_RDWR)) != MCO_SUCCESS)
		return status;

	return loadprofile(&icc);
}

McoStatus Mpfileio::saveProfile(char *fsspec)
{
	McoStatus 	status;
	CsFormat icc;

	if(icc.doesFileExist(fsspec)){
		status = icc.createFile(fsspec, _O_BINARY | _O_RDWR);
		if(status != MCO_SUCCESS)	return status;
		return writetoexistfile(&icc);
	}	
	else{	
		if( (status = icc.createFile(fsspec, _O_CREAT | _O_BINARY | _O_RDWR)) != MCO_SUCCESS)
			return status;
		return writetonewfile(&icc);
	}
		
}

McoStatus Mpfileio::loadPatchdata(char *fsspec, long which)
{
	McoStatus status;
	FILE 				*fs;
	
	fs = fopen( (char*)fsspec, "r" );
	if(!fs)
		return MCO_FAILURE;
		
	status = _loadPatchdata(fs, which);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::savePatchdata(char *fsspec, long which)
{
	McoStatus status;
	FILE 				*fs;
	
	fs = fopen( (char*)fsspec, "w" );
	if(!fs)
		return MCO_FAILURE;
		
	status = _savePatchdata(fs, which);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::loadLineardata(char *fsspec)
{
	McoStatus status;
	FILE 				*fs;
	
	fs = fopen( (char*)fsspec, "r" );
	if(!fs)
		return MCO_FAILURE;
		
	status = _loadLineardata(fs);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::saveLineardata(char *fsspec)
{
	McoStatus status;
	FILE 				*fs;
	
	fs = fopen( (char*)fsspec, "w" );
	if(!fs)
		return MCO_FAILURE;
		
	status = _saveLineardata(fs);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::loadTonedata(char *fsspec)
{
	McoStatus status;
	FILE 				*fs;
	
	fs = fopen( (char*)fsspec, "r" );
	if(!fs)
		return MCO_FAILURE;
		
	status = _loadTonedata(fs);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::saveTonedata(char *fsspec)
{
	McoStatus status;
	FILE 				*fs;
	
	fs = fopen( (char*)fsspec, "w" );
	if(!fs)
		return MCO_FAILURE;
		
	status = _saveTonedata(fs);
	
	fclose(fs);
	return status;
}

#else

McoStatus Mpfileio::openProfile(FSSpec *fsspec)
{
	McoStatus 	status;
	CsFormat icc;
	
	if( (status = icc.openFile(fsspec)) != MCO_SUCCESS)
		return status;

	return loadprofile(&icc);
}


McoStatus Mpfileio::saveProfile(FSSpec *fsspec)
{
	McoStatus 	status;
	CsFormat icc;
	
	if(icc.doesFileExist(fsspec)){
		status = icc.openFile(fsspec);
		if(status != MCO_SUCCESS)	return status;
		return writetoexistfile(&icc);
	}	
	else{	
		status = icc.createFilewithtype(fsspec, smSystemScript, 'CLGE', 'prof');
		if(status != MCO_SUCCESS)	return status;
		return writetonewfile(&icc);
	}
	
}

#include "fullname.h"
#include "think_bugs.h"

McoStatus Mpfileio::loadPatchdata(FSSpec *fsspec, long which)
{
	McoStatus 	status;
	FILE 				*fs;
	Str255				inname;

	memcpy(inname, fsspec->name, *(fsspec->name)+1);
	PathNameFromDirID(fsspec->parID, fsspec->vRefNum, inname);
	ptocstr(inname);	
	
	if( (fs = fopen( (char*)inname, "r" )) == NULL )
		return MCO_FAILURE;
		
	status =  _loadPatchdata(fs, which);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::savePatchdata(FSSpec *fsspec, long which)
{
	McoStatus 	status;
	FILE 				*fs;
	Str255				inname;

	memcpy(inname, fsspec->name, *(fsspec->name)+1);
	PathNameFromDirID(fsspec->parID, fsspec->vRefNum, inname);
	ptocstr(inname);	
	
	if( (fs = fopen( (char*)inname, "w" )) == NULL )
		return MCO_FAILURE;
		
	status =  _savePatchdata(fs, which);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::loadLineardata(FSSpec *fsspec)
{
	McoStatus 	status;
	FILE 				*fs;
	Str255				inname;

	memcpy(inname, fsspec->name, *(fsspec->name)+1);
	PathNameFromDirID(fsspec->parID, fsspec->vRefNum, inname);
	ptocstr(inname);	
	
	if( (fs = fopen( (char*)inname, "r" )) == NULL )
		return MCO_FAILURE;
		
	status =  _loadLineardata(fs);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::saveLineardata(FSSpec *fsspec)
{
	McoStatus 	status;
	FILE 				*fs;
	Str255				inname;

	memcpy(inname, fsspec->name, *(fsspec->name)+1);
	PathNameFromDirID(fsspec->parID, fsspec->vRefNum, inname);
	ptocstr(inname);	
	
	if( (fs = fopen( (char*)inname, "w" )) == NULL )
		return MCO_FAILURE;
		
	status =  _saveLineardata(fs);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::loadTonedata(FSSpec *fsspec)
{
	McoStatus 	status;
	FILE 				*fs;
	Str255				inname;

	memcpy(inname, fsspec->name, *(fsspec->name)+1);
	PathNameFromDirID(fsspec->parID, fsspec->vRefNum, inname);
	ptocstr(inname);	
	
	if( (fs = fopen( (char*)inname, "r" )) == NULL )
		return MCO_FAILURE;
		
	status =  _loadTonedata(fs);
	
	fclose(fs);
	return status;
}

McoStatus Mpfileio::saveTonedata(FSSpec *fsspec)
{
	McoStatus 	status;
	FILE 				*fs;
	Str255				inname;

	memcpy(inname, fsspec->name, *(fsspec->name)+1);
	PathNameFromDirID(fsspec->parID, fsspec->vRefNum, inname);
	ptocstr(inname);	
	
	if( (fs = fopen( (char*)inname, "w" )) == NULL )
		return MCO_FAILURE;
		
	status =  _saveTonedata(fs);
	
	fclose(fs);
	return status;
}

#endif


void Mpfileio::_double2s15Fixed16(double* in, Fixed *out, long num)
{
	long i;
	short	*ipart, *fpart;
	double 	*temp;
	
	for(i = 0; i < num; i++){
#if defined(_WIN32)
		fpart = (short*)&out[i];
		ipart = fpart + 1;
#else	
		ipart = (short*)&out[i];
		fpart = ipart + 1;
#endif
		
		//positive 
		if(in[i] >= 0){
			*ipart = (short)in[i];
			*fpart = (short)(65535*(in[i] - *ipart));
		}
		else{	//negative
			*fpart = (short)(65535*(in[i] - (short)in[i] + 1.0));
			*ipart = (short)in[i] - 1;
		}			
	}		
}

void Mpfileio::_s15Fixed162double(Fixed *out, double* in, long num)
{
	long i;
	short	*ipart;
	unsigned short *fpart;
	
	for(i = 0; i < num; i++){
#if defined(_WIN32)
		fpart = (unsigned short*)&out[i];
		ipart = (short*)(fpart + 1);
#else
		ipart = (short*)&out[i];
		fpart = (unsigned short*)(ipart + 1);
#endif
		
		double d = (double)*ipart + (double)*fpart/65535.0;
		// Only 4 of the fractional digits are guaranteed to be accurate
		int n = (int)((d * 10000.0) + (d>0 ? 0.5 : -0.5));
		in[i] = (double)n / 10000.0;
	}		
}

// convert fields whose value may be greater than 32000
void  Mpfileio::_convertFromFixed(double *in,long num,int bits)
{
int i;

if (bits == 8) return;

for (i=0; i<num; i++) in[i] = in[i]*4;
}


// convert fields whose value may be greater than 32000
void  Mpfileio::_convertToFixed(double *in,long num,int bits)
{
int i;

if (bits == 8) return;

for (i=0; i<num; i++) in[i] = in[i]*0.25;
}

//convert PatchFormat to MNPatchType
McoStatus Mpfileio::convert(PatchFormat *patchdata, MNPatchType *patchtype)
{
	McoStatus 			status;
	long	i, j, m;
	long	totalcmy, calpatch;

	patchtype->patchType = patchdata->patchType;
	patchtype->numComp = patchdata->numComp;
	patchtype->numCube = patchdata->numCube;
	patchtype->numLinear = patchdata->numLinear;
	
	patchtype->steps = new unsigned long[patchtype->numCube];
	for(i = 0; i < (long)patchdata->numCube; i++)
		patchtype->steps[i] = patchdata->steps[i];

	patchtype->blacks = new unsigned long[patchtype->numCube];
	for(i = 0; i < (long)patchtype->numCube; i++)
		patchtype->blacks[i] = patchdata->blacks[i];
	
	totalcmy = 0;
	for(i = 0; i < (long)patchtype->numCube; i++)
		totalcmy += patchtype->steps[i];

	patchtype->cmy = new unsigned long[totalcmy];
	m = 0;
	for(i = 0; i < (long)patchtype->numCube; i++)
		for(j = 0; j < (long)patchtype->steps[i]; j++){
			patchtype->cmy[m] = patchdata->cmy[m];
			m++;
		}	
	
	if(patchtype->numLinear){
		patchtype->linear = new Fixed[patchtype->numLinear];
		_double2s15Fixed16(patchdata->linear, patchtype->linear, patchtype->numLinear);
		patchtype->ldata = new Fixed[patchtype->numLinear*patchtype->numComp*3];
		_double2s15Fixed16(patchdata->ldata, patchtype->ldata, patchtype->numLinear*patchtype->numComp*3);
	}

	calpatch = 0;
	for(i = 0; i < (long)patchtype->numCube; i++)
		calpatch += patchtype->steps[i]*patchtype->steps[i]*patchtype->steps[i];
		
	patchtype->data = new Fixed[calpatch*3];
	_double2s15Fixed16(patchdata->data, patchtype->data, calpatch*3);
	
	memcpy(patchtype->name, patchdata->name, 100);
	
	return MCO_SUCCESS;
}


//convert MNPatchType to PatchFormat
McoStatus Mpfileio::revconvert(MNPatchType *patchtype, PatchFormat *patchdata)
{
	McoStatus 			status;
	long	i, j, m;
	long	totalcmy, calpatch;

	patchdata->patchType = patchtype->patchType;
	patchdata->numComp = patchtype->numComp;
	patchdata->numCube = patchtype->numCube;
	patchdata->numLinear = patchtype->numLinear;
	
	patchdata->steps = new unsigned long[patchdata->numCube];
	for(i = 0; i < (long)patchtype->numCube; i++)
		patchdata->steps[i] = patchtype->steps[i];

	patchdata->blacks = new unsigned long[patchdata->numCube];
	for(i = 0; i < (long)patchdata->numCube; i++)
		patchdata->blacks[i] = patchtype->blacks[i];
	
	totalcmy = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		totalcmy += patchdata->steps[i];

	patchdata->cmy = new unsigned long[totalcmy];
	m = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		for(j = 0; j < (long)patchdata->steps[i]; j++){
			patchdata->cmy[m] = patchtype->cmy[m];
			m++;
		}	
	
	if(patchdata->numLinear){
		patchdata->linear = new double[patchdata->numLinear];
		_s15Fixed162double(patchtype->linear, patchdata->linear, patchdata->numLinear);
		patchdata->ldata = new double[patchdata->numLinear*patchdata->numComp*3];
		_s15Fixed162double(patchtype->ldata, patchdata->ldata, patchdata->numLinear*patchdata->numComp*3);
	}

	calpatch = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		calpatch += patchdata->steps[i]*patchdata->steps[i]*patchdata->steps[i];
		
	patchdata->data = new double[calpatch*3];
	_s15Fixed162double(patchtype->data, patchdata->data, calpatch*3);

	memcpy(patchdata->name, patchtype->name, 100);
		
	return MCO_SUCCESS;
}

void Mpfileio::convertTweak(TweakElement *tweakele, MNTweakType *tweaktype, long size)
{
	long i;
	
	for(i = 0; i < size; i++){
		_double2s15Fixed16( (double*)(tweakele+i), (Fixed*)(tweaktype+i), 11 );
		tweaktype[i].type = tweakele[i].type;
		memcpy(tweaktype[i].name, tweakele[i].name, 40);
	}	

}

void Mpfileio::revconvertTweak(MNTweakType *tweaktype, TweakElement *tweakele, long size)
{
	long i;
	
	for(i = 0; i < size; i++){
		_s15Fixed162double((Fixed*)(tweaktype+i), (double*)(tweakele+i), 11 );
		tweakele[i].type = tweaktype[i].type;
		memcpy(tweakele[i].name, tweaktype[i].name, 40);
	}	

}


McoStatus	Mpfileio::loadprofile(CsFormat *icc)
{
	McoStatus 	status;
	CM2Header	header;
	long 		tagcount;
	CMLut16Type	luttag;
	long		valid;
	char		*_copystr = 0L;
	char		*_descstr = 0L;
	MNPatchType patchtypesrc, patchtypedest;

    memset ( &patchtypesrc, 0, sizeof(patchtypedest) );
    memset ( &header, 0, sizeof(header) );
    memset ( &luttag, 0, sizeof(luttag) );
	
	// perform a cleanup
	cleanup(2);

	if( (status = icc->getHeader(&header, &tagcount)) != MCO_SUCCESS )
		goto bail;
	
	//make sure it is CMYK TO CMYK link profile
	status = MCO_FAILURE;	
	if(header.profileClass != cmLinkClass)
		goto bail;
	
	if(header.dataColorSpace != cmCMYKData)
		goto bail;
		
	if(header.profileConnectionSpace != cmCMYKData)
		goto bail;	
		
	valid = icc->validProfile();
	if(!valid) 
		goto bail;

	if(header.CMMType == MONACO_CMM)
		{
		if (icc->checkTagExist(MCOP_SRC_PATCH_TAG)) _printdata->icctype = MONACO_CREATED_ICC;
		else if(icc->checkTagExist(MC0P_C_LINEAR_TAG)) _printdata->icctype = MONACOP_EDITED_ICC;
		else  _printdata->icctype = MONACOP_CREATED_ICC;
		}
	else{
		_printdata->icctype = NOT_MONACO_ICC;
		if(icc->checkTagExist(MC0P_C_LINEAR_TAG))
			_printdata->icctype = MONACO_EDITED_ICC;
	}	
		
	
	if( (status = icc->getLutTagdatainfo(cmAToB0Tag, &luttag)) != MCO_SUCCESS)
		goto bail;

	status = MCO_FAILURE;
	
	//compute the lut size	
	long lutsize, inputsize, outputsize, start, i;
	if( luttag.typeDescriptor == cmSigLut16Type){
		_printdata->bitsize = 16;
		lutsize = 1; 
		for(i = 0; i < luttag.inputChannels; i++)
			lutsize *= luttag.gridPoints;			
		lutsize *= luttag.outputChannels*2;
		
		inputsize = luttag.inputTableEntries*2;
		outputsize = luttag.outputTableEntries*2;
		start = 52;
	}
	else if( luttag.typeDescriptor == cmSigLut8Type){	
		_printdata->bitsize = 8;
		lutsize = 1; 
		for(i = 0; i < luttag.inputChannels; i++)
			lutsize *= luttag.gridPoints;			
		lutsize *= luttag.outputChannels;
		
		inputsize = 256;
		outputsize = 256;
		start = 48;
	}
	else
		goto bail; 

	_printdata->A2B0.grid_points = luttag.gridPoints;
	_printdata->A2B0.input_channels = luttag.inputChannels;
	_printdata->A2B0.output_channels = luttag.outputChannels;
	_printdata->A2B0.input_entries = luttag.inputTableEntries;
	_printdata->A2B0.output_entries = luttag.outputTableEntries;

	for(i = 0; i < luttag.inputChannels; i++){
		_printdata->A2B0.input_tables[i] = new char[inputsize];
		if(!_printdata->A2B0.input_tables[i])	goto bail;
	}
	
	for(i = 0; i < luttag.outputChannels; i++){	
		_printdata->A2B0.output_tables[i] = new char[outputsize];
		if(!_printdata->A2B0.output_tables[i])	goto bail;
	}
	
	//load tone curves	
	for(i = 0; i < luttag.inputChannels; i++){
		status = icc->getPartialTagdata(cmAToB0Tag, start, inputsize, _printdata->A2B0.input_tables[i]);
		start += inputsize;
	}
	
	start += lutsize;
	//load linear curves
	for(i = 0; i < luttag.outputChannels; i++){
		status = icc->getPartialTagdata(cmAToB0Tag, start, outputsize, _printdata->A2B0.output_tables[i]);
		start += outputsize;
	}
			
	//get description tag
	long tagsize;
	
	tagsize = icc->getTagsize(cmProfileDescriptionTag);
	_descstr = new char[tagsize];
	if (!_descstr) goto bail;
	if( (status = icc->getPartialTagdata(cmProfileDescriptionTag, 12, tagsize - 12, _descstr)) != MCO_SUCCESS)
		goto bail;

	//get copyright tag
	tagsize = icc->getTagsize(cmCopyrightTag);
	_copystr = new char[tagsize];
	if (!_copystr) goto bail;
	if( (status = icc->getPartialTagdata(cmCopyrightTag, 8, tagsize, _copystr)) != MCO_SUCCESS)
		goto bail;

	//get setting tag
	MNSettingType	setting;
    memset ( &setting, 0, sizeof(setting) );

	if(icc->checkTagExist(MC0P_SETTING_TAG)){
		tagsize = icc->getTagsize(MC0P_SETTING_TAG);
		if( (status = icc->getMonacoSettingTagdata((char*)&setting)) != MCO_SUCCESS)
			goto bail;
			
		_printdata->setting.tableType = setting.tableType;
		_printdata->setting.separationType = setting.separationType;			
		_printdata->setting.blackType = setting.blackType;
		_printdata->setting.totalInkLimit = setting.totalInkLimit;				
		_printdata->setting.blackInkLimit = setting.blackInkLimit;				
		_printdata->setting.saturation = setting.saturation;		
		_printdata->setting.colorBalance = setting.colorBalance;
		_printdata->setting.colorBalanceCut = setting.colorBalanceCut;
		_printdata->setting.linear = setting.linear;
		_printdata->setting.inkLinearize = setting.inkLinearize;
		_printdata->setting.inkNeutralize = setting.inkNeutralize;
		_printdata->setting.inkHighlight = setting.inkHighlight;
		_printdata->setting.simulate = setting.simulate;
		_printdata->setting.smooth = setting.smooth;
		_printdata->setting.revTableSize = setting.revTableSize;
		_printdata->setting.smoothSimu = setting.smoothSimu;
		_printdata->setting.kOnly = setting.kOnly;
		_printdata->setting.numBlackHand = setting.numBlackHand;
		_printdata->setting.flag = setting.flag;
		for(i = 0; i < 6; i++)
			_printdata->setting.numLinearHand[i] = setting.numLinearHand[i];
		for(i = 0; i < 6; i++)
			_printdata->setting.numToneHand[i] = setting.numToneHand[i];
		for(i = 0; i < 99; i++)	
			_printdata->setting.reserved[i] = setting.reserved[i];
	
		_s15Fixed162double(setting.blackCurve, _printdata->setting.blackCurve, 101);
		
		_s15Fixed162double(setting.blackX, _printdata->setting.blackX, 31);
		_s15Fixed162double(setting.blackY, _printdata->setting.blackY, 31);
		
		_s15Fixed162double(setting.linearX, _printdata->setting.linearX, MAX_LINEAR_HAND*6);
		_s15Fixed162double(setting.linearY, _printdata->setting.linearY, MAX_LINEAR_HAND*6);
		
		_s15Fixed162double(setting.toneX, _printdata->setting.toneX, MAX_TONE_HAND*6);
		_s15Fixed162double(setting.toneY, _printdata->setting.toneY, MAX_TONE_HAND*6);

		memcpy(_printdata->setting.linearDesc, setting.linearDesc, 100);
		memcpy(_printdata->setting.toneDesc, setting.toneDesc, 100);
	}
	
	//get source patch tag
	if(icc->checkTagExist(MCOP_SRC_PATCH_TAG)){
		if( (status = icc->getMonacoPatchTagdata(MCOP_SRC_PATCH_TAG, &patchtypesrc)) != MCO_SUCCESS)
			goto bail;
			
		if( (status = revconvert( &patchtypesrc, &(_printdata->srcpatch))) != MCO_SUCCESS)
			goto bail;	
	}

	if(icc->checkTagExist(MCOP_DEST_PATCH_TAG)){
		if( (status = icc->getMonacoPatchTagdata(MCOP_DEST_PATCH_TAG, &patchtypedest)) != MCO_SUCCESS)
			goto bail;
			
		if( (status = revconvert( &patchtypedest, &(_printdata->destpatch) )) != MCO_SUCCESS)
			goto bail;	
	}
	
	//get the linear tag
	Fixed linear[4096];
    memset ( &linear, 0, sizeof(linear) );
	
	_printdata->numlinear = 0;
	
	if(icc->checkTagExist(MC0P_C_LINEAR_TAG)){
		tagsize = icc->getTagsize(MC0P_C_LINEAR_TAG);
		if( (status = icc->gets15Fixed16ArrayTagdata(MC0P_C_LINEAR_TAG, (char*)linear)) != MCO_SUCCESS)
			goto bail;
		
		_printdata->linear[0] = new double[(tagsize-8)/4];	
		if(!_printdata->linear[0])	goto bail;
		_s15Fixed162double(linear, _printdata->linear[0], (tagsize-8)/4);
		_convertFromFixed(_printdata->linear[0], (tagsize-8)/4,_printdata->bitsize);
		_printdata->numlinear++;
		_printdata->sizelinear = (tagsize-8)/4;
	}
	
	if(icc->checkTagExist(MC0P_M_LINEAR_TAG)){
		tagsize = icc->getTagsize(MC0P_M_LINEAR_TAG);
		if( (status = icc->gets15Fixed16ArrayTagdata(MC0P_M_LINEAR_TAG, (char*)linear)) != MCO_SUCCESS)
			goto bail;
			
		_printdata->linear[1] = new double[(tagsize-8)/4];	
		if(!_printdata->linear[1])	goto bail;
		_s15Fixed162double(linear, _printdata->linear[1], (tagsize-8)/4);
		_convertFromFixed(_printdata->linear[1], (tagsize-8)/4,_printdata->bitsize);
		_printdata->numlinear++;
	}
	
	if(icc->checkTagExist(MC0P_Y_LINEAR_TAG)){
		tagsize = icc->getTagsize(MC0P_Y_LINEAR_TAG);
		if( (status = icc->gets15Fixed16ArrayTagdata(MC0P_Y_LINEAR_TAG, (char*)linear)) != MCO_SUCCESS)
			goto bail;

		_printdata->linear[2] = new double[(tagsize-8)/4];	
		if(!_printdata->linear[2])	goto bail;
		_s15Fixed162double(linear, _printdata->linear[2], (tagsize-8)/4);
		_convertFromFixed(_printdata->linear[2], (tagsize-8)/4,_printdata->bitsize);
		_printdata->numlinear++;
	}
	
	if(icc->checkTagExist(MC0P_K_LINEAR_TAG)){
		tagsize = icc->getTagsize(MC0P_K_LINEAR_TAG);
		if( (status = icc->gets15Fixed16ArrayTagdata(MC0P_K_LINEAR_TAG, (char*)linear)) != MCO_SUCCESS)
			goto bail;
			
		_printdata->linear[3] = new double[(tagsize-8)/4];	
		if(!_printdata->linear[3])	goto bail;
		_s15Fixed162double(linear, _printdata->linear[3], (tagsize-8)/4);
		_convertFromFixed(_printdata->linear[3], (tagsize-8)/4,_printdata->bitsize);
		_printdata->numlinear++;
	}
	
	//get the tone tag
	Fixed tone[200];
    memset ( tone, 0, sizeof(tone) );

	_printdata->numtone = 0;
	
	if(icc->checkTagExist(MC0P_C_TONE_TAG)){
		tagsize = icc->getTagsize(MC0P_C_TONE_TAG);
		if( (status = icc->gets15Fixed16ArrayTagdata(MC0P_C_TONE_TAG, (char*)tone)) != MCO_SUCCESS)
			goto bail;
			
		_printdata->tone[0] = new double[(tagsize-8)/4];	
		if(!_printdata->tone[0])	goto bail;
		_s15Fixed162double(tone, _printdata->tone[0], (tagsize-8)/4);
		_convertFromFixed(_printdata->tone[0], (tagsize-8)/4,_printdata->bitsize);
		_printdata->numtone++;
		_printdata->sizetone = (tagsize-8)/4;
	}
	
	if(icc->checkTagExist(MC0P_M_TONE_TAG)){
		tagsize = icc->getTagsize(MC0P_M_TONE_TAG);
		if( (status = icc->gets15Fixed16ArrayTagdata(MC0P_M_TONE_TAG, (char*)tone)) != MCO_SUCCESS)
			goto bail;
			
		_printdata->tone[1] = new double[(tagsize-8)/4];	
		if(!_printdata->tone[1])	goto bail;
		_s15Fixed162double(tone, _printdata->tone[1], (tagsize-8)/4);
		_convertFromFixed(_printdata->tone[1], (tagsize-8)/4,_printdata->bitsize);
		_printdata->numtone++;
	}
	
	if(icc->checkTagExist(MC0P_Y_TONE_TAG)){
		tagsize = icc->getTagsize(MC0P_Y_TONE_TAG);
		if( (status = icc->gets15Fixed16ArrayTagdata(MC0P_Y_TONE_TAG, (char*)tone)) != MCO_SUCCESS)
			goto bail;
			
		_printdata->tone[2] = new double[(tagsize-8)/4];	
		if(!_printdata->tone[2])	goto bail;
		_s15Fixed162double(tone, _printdata->tone[2], (tagsize-8)/4);
		_convertFromFixed(_printdata->tone[2], (tagsize-8)/4,_printdata->bitsize);
		_printdata->numtone++;
	}
	
	if(icc->checkTagExist(MC0P_K_TONE_TAG)){
		tagsize = icc->getTagsize(MC0P_K_TONE_TAG);
		if( (status = icc->gets15Fixed16ArrayTagdata(MC0P_K_TONE_TAG, (char*)tone)) != MCO_SUCCESS)
			goto bail;
			
		_printdata->tone[3] = new double[(tagsize-8)/4];	
		if(!_printdata->tone[3])	goto bail;
		_s15Fixed162double(tone, _printdata->tone[3], (tagsize-8)/4);
		_convertFromFixed(_printdata->tone[3], (tagsize-8)/4,_printdata->bitsize);
		_printdata->numtone++;
	}

	if(icc->checkTagExist(MC0P_TWEAK_TAG)){
		tagsize = icc->getTagsize(MC0P_TWEAK_TAG);
		
		MNTweakType	*tweaktype;
			
		_printdata->numTweaks = (tagsize - 8)/sizeof(MNTweakType);
		_printdata->tweakele = new TweakElement[_printdata->numTweaks];
		if(!_printdata->tweakele)	goto bail;
		
		tweaktype = new MNTweakType[_printdata->numTweaks];
		if(!tweaktype)	goto bail;

		if( (status = icc->getTweakTagdata(MC0P_TWEAK_TAG, tweaktype)) != MCO_SUCCESS)
			goto bail;
		
		revconvertTweak(tweaktype, _printdata->tweakele, _printdata->numTweaks);
	
		delete 	tweaktype;
	}		
	
	status = MCO_SUCCESS;
	
bail:
	if (_copystr)
		delete _copystr;
	if (_descstr)
		delete _descstr;

	if (patchtypesrc.steps)		delete patchtypesrc.steps;
	if (patchtypesrc.blacks)	delete patchtypesrc.blacks;
	if (patchtypesrc.cmy)		delete patchtypesrc.cmy;
	if (patchtypesrc.linear)	delete patchtypesrc.linear;
	if (patchtypesrc.ldata)		delete patchtypesrc.ldata;
	if (patchtypesrc.data )		delete patchtypesrc.data;

	if (patchtypedest.steps)	delete patchtypedest.steps;
	if (patchtypedest.blacks)	delete patchtypedest.blacks;
	if (patchtypedest.cmy)		delete patchtypedest.cmy;
	if (patchtypedest.linear)	delete patchtypedest.linear;
	if (patchtypedest.ldata)	delete patchtypedest.ldata;
	if (patchtypedest.data )	delete patchtypedest.data;

	icc->closeFile();
	return status;	
}

McoStatus Mpfileio::writetoexistfile(CsFormat *icc)
{
	McoStatus status = MCO_MEM_ALLOC_ERROR;
	CM2Header	header;
	long 		tagcount;
	CMLut16Type	luttag;
	long		valid;
	long 		i;
	MNSettingType	*setting = 0L;
	MNPatchType	*srcpatchtype = 0L, *destpatchtype = 0L;
	Fixed	**curve = 0L;
	long 	numcurve; 
	unsigned long linear_tags[] = {MC0P_C_LINEAR_TAG, MC0P_M_LINEAR_TAG, MC0P_Y_LINEAR_TAG, MC0P_K_LINEAR_TAG};
	unsigned long tone_tags[] = {MC0P_C_TONE_TAG, MC0P_M_TONE_TAG, MC0P_Y_TONE_TAG, MC0P_K_TONE_TAG};
	char	*_copystr = 0L;
	char	*_descstr = 0L;
	char	*_sequstr = 0L;
	long 		haslut = 1;
	MNTweakType	*tweaktype = NULL;

	
    memset ( &header, 0, sizeof(header) );
    memset ( &luttag, 0, sizeof(luttag) );

	curve = new Fixed*[12];
	if (!curve) goto bail;
	for (i=0; i<12; i++)
		{
		curve[i] = new Fixed[4096];
		if (!curve[i]) goto bail;
		}
	
	setting = new MNSettingType;
	if (!setting) goto bail;
    memset ( setting, 0, sizeof(MNSettingType) );

	srcpatchtype = new MNPatchType;
	if (!srcpatchtype) goto bail;
    memset ( srcpatchtype, 0, sizeof(MNPatchType) );

	destpatchtype = new MNPatchType;
	if (!destpatchtype) goto bail;
    memset ( destpatchtype, 0, sizeof(MNPatchType) );

	if( (status = icc->getHeader(&header, &tagcount)) != MCO_SUCCESS )
		goto bail;
	
	//make sure it is CMYK TO CMYK link profile
	status = MCO_FAILURE;	
	if(header.profileClass != cmLinkClass)
		goto bail;
	
	if(header.dataColorSpace != cmCMYKData)
		goto bail;
		
	if(header.profileConnectionSpace != cmCMYKData)
		goto bail;	
		
	valid = icc->validProfile();
	if(!valid) 
		goto bail;

	if( (status = icc->getLutTagdatainfo(cmAToB0Tag, &luttag)) != MCO_SUCCESS)
		goto bail;
	
	//get description tag
	long tagsize;
	
	tagsize = icc->getTagsize(cmProfileDescriptionTag);
	_descstr = new char[tagsize];
	if (!_descstr) goto bail;
	if( (status = icc->getPartialTagdata(cmProfileDescriptionTag, 12, tagsize - 12, _descstr)) != MCO_SUCCESS)
		goto bail;

	//get the copyright tag
	tagsize = icc->getTagsize(cmCopyrightTag);
	_copystr = new char[tagsize];
	if (!_copystr) goto bail;
	if( (status = icc->getPartialTagdata(cmCopyrightTag, 8, tagsize, _copystr)) != MCO_SUCCESS)
		goto bail;

	//get the sequence tag
	tagsize = icc->getTagsize(cmProfileSequenceDescTag);
	//if( (status = icc->getPartialTagdata(cmProfileSequenceDescTag, 0, tagsize, _sequstr)) != MCO_SUCCESS)
	//	goto bail;
	_sequstr = new char[tagsize];
	if (!_sequstr) goto bail;
	if( (status = icc->getTagdata(cmProfileSequenceDescTag, (unsigned char*)_sequstr)) != MCO_SUCCESS)
		goto bail;

	//get the lut8/16 tag if necessary
	if(!_printdata->tableH){
		haslut = 0;
		
		if( (status = icc->getLutTagdatainfo(cmAToB0Tag, &luttag)) != MCO_SUCCESS)
			goto bail;

		//compute the lut size	
		long dataoffset, datasize;

		if( luttag.typeDescriptor == cmSigLut16Type){
			dataoffset = 52 + luttag.inputChannels*luttag.inputTableEntries*2;	
			
			datasize = 1; 
			for(i = 0; i < luttag.inputChannels; i++)
				datasize *= luttag.gridPoints;			
			datasize *= luttag.outputChannels*2;
		}
		else if( luttag.typeDescriptor == cmSigLut8Type){	
			dataoffset = 48 + luttag.inputChannels*256;
			
			datasize = 1; 
			for(i = 0; i < luttag.inputChannels; i++)
				datasize *= luttag.gridPoints;			
			datasize *= luttag.outputChannels;
		}
		else
			goto bail; 
		
		_printdata->tableH = McoNewHandle(datasize);	
		if(!_printdata->tableH)	goto bail;
		
		char* data;
		data = (char*)McoLockHandle(_printdata->tableH);
		
		status = icc->getPartialTagdata(cmAToB0Tag, dataoffset, datasize, data);
		McoUnlockHandle(_printdata->tableH);
		if(status != MCO_SUCCESS)
		{
			McoDeleteHandle(_printdata->tableH);
			goto bail;
		}
		_printdata->A2B0.input_channels = luttag.inputChannels;
		_printdata->A2B0.output_channels = luttag.outputChannels;	
		_printdata->A2B0.grid_points = luttag.gridPoints;
		_printdata->A2B0.identity_matrix = 1;
	}
			
//modify the file
	if( (status = icc->setFilelength(0)) != MCO_SUCCESS)
		goto bail;

//write to the existing file
	if( (status = icc->setHeader(&header, luttag.typeDescriptor)) != MCO_SUCCESS)
		goto bail;

//copy right tag
	if( (status = icc->setCprttypedata(_copystr, strlen(_copystr))) != MCO_SUCCESS )
		goto bail;

//description tage
	if( (status = icc->setDesctypedata(_descstr, strlen(_descstr)+1)) != MCO_SUCCESS)
		goto bail;

//sequence tag
	icc->setPseqtypedata(_sequstr, tagsize);

//setting tag
	setting->tableType = _printdata->setting.tableType;
	setting->separationType = _printdata->setting.separationType;			
	setting->blackType = _printdata->setting.blackType;
	setting->totalInkLimit = _printdata->setting.totalInkLimit;				
	setting->blackInkLimit = _printdata->setting.blackInkLimit;				
	setting->saturation = _printdata->setting.saturation;		
	setting->colorBalance = _printdata->setting.colorBalance;
	setting->colorBalanceCut = _printdata->setting.colorBalanceCut;
	setting->linear = _printdata->setting.linear;
	setting->inkLinearize = _printdata->setting.inkLinearize;
	setting->inkNeutralize = _printdata->setting.inkNeutralize;
	setting->inkHighlight = _printdata->setting.inkHighlight;
	setting->simulate = _printdata->setting.simulate;
	setting->smooth = _printdata->setting.smooth;
	setting->revTableSize = _printdata->setting.revTableSize;
	setting->smoothSimu = _printdata->setting.smoothSimu;
	setting->kOnly = _printdata->setting.kOnly;
	setting->numBlackHand = _printdata->setting.numBlackHand;
	setting->flag = _printdata->setting.flag;
	for(i = 0; i < 6; i++)
		setting->numLinearHand[i] = _printdata->setting.numLinearHand[i];
	for(i = 0; i < 6; i++)
		setting->numToneHand[i] = _printdata->setting.numToneHand[i];
	for(i = 0; i < 99; i++)	
		setting->reserved[i] = _printdata->setting.reserved[i];
		
	_double2s15Fixed16( _printdata->setting.blackCurve, setting->blackCurve, 101);
	_double2s15Fixed16(_printdata->setting.blackX, setting->blackX, 31);
	_double2s15Fixed16(_printdata->setting.blackY, setting->blackY, 31);
	_double2s15Fixed16(_printdata->setting.linearX, setting->linearX, MAX_LINEAR_HAND*6);
	_double2s15Fixed16(_printdata->setting.linearY, setting->linearY, MAX_LINEAR_HAND*6);
	_double2s15Fixed16(_printdata->setting.toneX, setting->toneX, MAX_TONE_HAND*6);
	_double2s15Fixed16(_printdata->setting.toneY, setting->toneY, MAX_TONE_HAND*6);

	memcpy(setting->linearDesc, _printdata->setting.linearDesc, 100);
	memcpy(setting->toneDesc, _printdata->setting.toneDesc, 100);

	if( (status = icc->setSettingTypedata(setting)) != MCO_SUCCESS)
		goto bail;

//monaco source patch tag	
	if(_printdata->srcpatch.patchType){
		if( (status = convert(&(_printdata->srcpatch), srcpatchtype)) != MCO_SUCCESS)
			goto bail;
			
		if( (status = icc->setPatchTypedata(MCOP_SRC_PATCH_TAG, srcpatchtype)) != MCO_SUCCESS)
			goto bail;
	}

//monaco destination patch tag
	if(_printdata->destpatch.patchType){
		if( (status = convert(&(_printdata->destpatch), destpatchtype)) != MCO_SUCCESS)
			goto bail;
			
		if( (status = icc->setPatchTypedata(MCOP_DEST_PATCH_TAG, destpatchtype)) != MCO_SUCCESS)
			goto bail;
	}

//monaco linear curve tag
	numcurve = _printdata->numlinear;
	while(numcurve--){
		_convertToFixed(_printdata->linear[numcurve], _printdata->sizelinear,_printdata->bitsize); // divide by 10 if 16 bit
		_double2s15Fixed16(_printdata->linear[numcurve], curve[numcurve], _printdata->sizelinear);
		_convertFromFixed(_printdata->linear[numcurve], _printdata->sizelinear,_printdata->bitsize); // change data back
		if( (status = icc->sets15Fixed16ArrayTypedata(linear_tags[numcurve], _printdata->sizelinear, curve[numcurve])) != MCO_SUCCESS)
			goto bail;
	}

//monaco tone curve	tag
	numcurve = _printdata->numtone;
	while(numcurve--){
		_convertToFixed(_printdata->tone[numcurve], _printdata->sizetone,_printdata->bitsize);	// divide by 10 if 16 bit
		_double2s15Fixed16(_printdata->tone[numcurve], curve[numcurve+_printdata->numlinear], _printdata->sizetone);
		_convertFromFixed(_printdata->tone[numcurve], _printdata->sizetone,_printdata->bitsize);  // change data back
		if( (status = icc->sets15Fixed16ArrayTypedata(tone_tags[numcurve], _printdata->sizetone, curve[numcurve+_printdata->numlinear])) != MCO_SUCCESS)
			goto bail;
	}

//monaco tweak tag
	if(_printdata->numTweaks > 0)
	{
		tweaktype = new MNTweakType[_printdata->numTweaks];
		if(!tweaktype)	goto bail;

		convertTweak(_printdata->tweakele, tweaktype, _printdata->numTweaks);
	
		if( (status = icc->setTweaktypedata(_printdata->numTweaks, tweaktype)) != MCO_SUCCESS)
			goto bail;
	}	
		
//lut 8 or 16 tag										
	if( (status = icc->setLuttypedata(1, &(_printdata->A2B0), _printdata->tableH)) != MCO_SUCCESS )
		goto bail;

	if( (status = icc->writeTag(1)) != MCO_SUCCESS)
		goto bail;
		
	if(haslut == 0){
		McoDeleteHandle(_printdata->tableH);
		_printdata->tableH = 0L;
	}		
		
	status = MCO_SUCCESS;

bail:
	if (tweaktype)
		delete tweaktype;

	if (curve) 
		{
		for (i=0; i<12; i++) if (curve[i]) delete curve[i];
		delete curve;
		}

	if (setting) delete setting;

	if (srcpatchtype->steps)	delete srcpatchtype->steps;
	if (srcpatchtype->blacks)	delete srcpatchtype->blacks;
	if (srcpatchtype->cmy)		delete srcpatchtype->cmy;
	if (srcpatchtype->linear)	delete srcpatchtype->linear;
	if (srcpatchtype->ldata)	delete srcpatchtype->ldata;
	if (srcpatchtype->data )	delete srcpatchtype->data;
	if (srcpatchtype) delete srcpatchtype;

	if (destpatchtype->steps)	delete destpatchtype->steps;
	if (destpatchtype->blacks)	delete destpatchtype->blacks;
	if (destpatchtype->cmy)		delete destpatchtype->cmy;
	if (destpatchtype->linear)	delete destpatchtype->linear;
	if (destpatchtype->ldata)	delete destpatchtype->ldata;
	if (destpatchtype->data )	delete destpatchtype->data;
	if (destpatchtype) delete destpatchtype;

	if (_copystr) delete _copystr;	
	if (_descstr) delete _descstr;	
	if (_sequstr) delete _sequstr;	

	icc->closeFile();
	return status;
}

McoStatus Mpfileio::writetonewfile(CsFormat *icc)
{
	char	*cprt_des = 0L;// = MCOP_CPRT; //"Monaco Systems Inc.";
	long		i, j, k;
	CM2Header header;						
	MNSettingType	*setting = 0L;
	MNPatchType	*srcpatchtype = 0L, *destpatchtype = 0L;
	Fixed	**curve = 0L;
	long	numcurve;
	unsigned long linear_tags[] = {MC0P_C_LINEAR_TAG, MC0P_M_LINEAR_TAG, MC0P_Y_LINEAR_TAG, MC0P_K_LINEAR_TAG};
	unsigned long tone_tags[] = {MC0P_C_TONE_TAG, MC0P_M_TONE_TAG, MC0P_Y_TONE_TAG, MC0P_K_TONE_TAG};
	McoStatus status = MCO_MEM_ALLOC_ERROR;
	MNTweakType	*tweaktype = NULL;
	
    memset ( &header, 0, sizeof(header) );

	cprt_des = new char[255];
	if (!cprt_des) goto bail;
	
	strcpy(cprt_des,MCOP_CPRT);
	
	curve = new Fixed*[12];
	if (!curve) goto bail;
	for (i=0; i<12; i++)
		{
		curve[i] = new Fixed[4096];
		if (!curve[i]) goto bail;
		}

	setting = new MNSettingType;
	if (!setting) goto bail;
    memset ( setting, 0, sizeof(MNSettingType) );

	srcpatchtype = new MNPatchType;
	if (!srcpatchtype) goto bail;
    memset ( srcpatchtype, 0, sizeof(MNPatchType) );

	destpatchtype = new MNPatchType;
	if (!destpatchtype) goto bail;
    memset ( destpatchtype, 0, sizeof(MNPatchType) );

//set header
	header.CMMType = MONACO_CMM;
	header.profileVersion = (MCOP_MAJOR_VERSION + MCOP_MINOR_VERSION);
	header.profileClass = cmLinkClass;
	header.dataColorSpace = cmCMYKData;
	header.profileConnectionSpace = cmCMYKData;
	header.CS2profileSignature = cmMagicNumber;	//?
	header.platform = MCOP_DEFAULT_PLATFORM;
	header.flags = MCOP_DEFAULT_FLAGS;
	header.deviceManufacturer = MCOP_DEFAULT_MANUFACTURER;
	header.deviceModel = MCOP_DEFAULT_MODEL;
	header.deviceAttributes[0] = MCOP_DEFAULT_ATTRIBUTES;
	header.deviceAttributes[1] = MCOP_DEFAULT_ATTRIBUTES;
	header.renderingIntent = MCOP_DEFAULT_RENDERINGINTENT;
	header.white.X = MCOP_DEFAULT_WHITE_X;
	header.white.Y = MCOP_DEFAULT_WHITE_Y;
	header.white.Z = MCOP_DEFAULT_WHITE_Z;

	if( _printdata->bitsize == 8)
		if( (status = icc->setHeader(&header, cmSigLut8Type)) != MCO_SUCCESS)
			goto bail;
	else if( _printdata->bitsize == 16)	
		if( (status = icc->setHeader(&header, cmSigLut16Type)) != MCO_SUCCESS)
			goto bail;
	else
		return MCO_SUCCESS;
	
//description tag
	char	filename[256];
	if( (status = icc->getFname(filename)) != MCO_SUCCESS)
		goto bail;
		
	if( (status = icc->setDesctypedata(filename, strlen(filename)+1)) != MCO_SUCCESS)
		goto bail;

//copyright tag
	if( (status = icc->setCprttypedata(cprt_des, strlen(cprt_des))) != MCO_SUCCESS )
		goto bail;

//sequence tag
	if( (status = icc->setPseqtypedata(2)) != MCO_SUCCESS )
		goto bail;

//setting tag
	setting->tableType = _printdata->setting.tableType;
	setting->separationType = _printdata->setting.separationType;			
	setting->blackType = _printdata->setting.blackType;
	setting->totalInkLimit = _printdata->setting.totalInkLimit;				
	setting->blackInkLimit = _printdata->setting.blackInkLimit;				
	setting->saturation = _printdata->setting.saturation;		
	setting->colorBalance = _printdata->setting.colorBalance;
	setting->colorBalanceCut = _printdata->setting.colorBalanceCut;
	setting->linear = _printdata->setting.linear;
	setting->inkLinearize = _printdata->setting.inkLinearize;
	setting->inkNeutralize = _printdata->setting.inkNeutralize;
	setting->inkHighlight = _printdata->setting.inkHighlight;
	setting->simulate = _printdata->setting.simulate;
	setting->smooth = _printdata->setting.smooth;
	setting->revTableSize = _printdata->setting.revTableSize;
	setting->smoothSimu = _printdata->setting.smoothSimu;
	setting->kOnly = _printdata->setting.kOnly;
	setting->numBlackHand = _printdata->setting.numBlackHand;
	setting->flag = _printdata->setting.flag;
	for(i = 0; i < 6; i++)
		setting->numLinearHand[i] = _printdata->setting.numLinearHand[i];
	for(i = 0; i < 6; i++)
		setting->numToneHand[i] = _printdata->setting.numToneHand[i];
	for(i = 0; i < 99; i++)	
		setting->reserved[i] = _printdata->setting.reserved[i];
		
	_double2s15Fixed16( _printdata->setting.blackCurve, setting->blackCurve, 101);
	_double2s15Fixed16(_printdata->setting.blackX, setting->blackX, 31);
	_double2s15Fixed16(_printdata->setting.blackY, setting->blackY, 31);
	_double2s15Fixed16(_printdata->setting.linearX, setting->linearX, MAX_LINEAR_HAND*6);
	_double2s15Fixed16(_printdata->setting.linearY, setting->linearY, MAX_LINEAR_HAND*6);
	_double2s15Fixed16(_printdata->setting.toneX, setting->toneX, MAX_TONE_HAND*6);
	_double2s15Fixed16(_printdata->setting.toneY, setting->toneY, MAX_TONE_HAND*6);

	memcpy(setting->linearDesc, _printdata->setting.linearDesc, 100);
	memcpy(setting->toneDesc, _printdata->setting.toneDesc, 100);
		
	if( (status = icc->setSettingTypedata(setting)) != MCO_SUCCESS)
		goto bail;

//monaco source patch tag	
	if(_printdata->srcpatch.patchType){
		if( (status = convert(&(_printdata->srcpatch), srcpatchtype)) != MCO_SUCCESS)
			goto bail;
			
		if( (status = icc->setPatchTypedata(MCOP_SRC_PATCH_TAG, srcpatchtype)) != MCO_SUCCESS)
			goto bail;
	}
	
//monaco destination patch tag
	if(_printdata->destpatch.patchType){
		if( (status = convert(&(_printdata->destpatch), destpatchtype)) != MCO_SUCCESS)
			goto bail;
			
		if( (status = icc->setPatchTypedata(MCOP_DEST_PATCH_TAG, destpatchtype)) != MCO_SUCCESS)
			goto bail;
	}
	
//monaco linear curve tag
	numcurve = _printdata->numlinear;
	while(numcurve--){
		_convertToFixed(_printdata->linear[numcurve], _printdata->sizelinear,_printdata->bitsize); // divide by 10 if 16 bit
		_double2s15Fixed16(_printdata->linear[numcurve], curve[numcurve], _printdata->sizelinear);
		_convertFromFixed(_printdata->linear[numcurve], _printdata->sizelinear,_printdata->bitsize); // change data back
		if( (status = icc->sets15Fixed16ArrayTypedata(linear_tags[numcurve], _printdata->sizelinear, curve[numcurve])) != MCO_SUCCESS)
			goto bail;
	}
		
//monaco tone curve	tag
	numcurve = _printdata->numtone;
	while(numcurve--){
		_convertToFixed(_printdata->tone[numcurve], _printdata->sizetone,_printdata->bitsize);	// divide by 10 if 16 bit
		_double2s15Fixed16(_printdata->tone[numcurve], curve[numcurve+_printdata->numlinear], _printdata->sizetone);
		_convertFromFixed(_printdata->tone[numcurve], _printdata->sizetone,_printdata->bitsize);  // change data back
		if( (status = icc->sets15Fixed16ArrayTypedata(tone_tags[numcurve], _printdata->sizetone, curve[numcurve+_printdata->numlinear])) != MCO_SUCCESS)
			goto bail;
	}

//monaco tweak tag
	if(_printdata->numTweaks > 0){

		tweaktype = new MNTweakType[_printdata->numTweaks];
		if(!tweaktype)	goto bail;

		convertTweak(_printdata->tweakele, tweaktype, _printdata->numTweaks);
	
		if( (status = icc->setTweaktypedata(_printdata->numTweaks, tweaktype)) != MCO_SUCCESS)
			goto bail;
	}		
		
//lut 8 or 16 tag										
	if( (status = icc->setLuttypedata(1, &(_printdata->A2B0), _printdata->tableH)) != MCO_SUCCESS )
		goto bail;
			
	if( (status = icc->writeTag(1)) != MCO_SUCCESS)
		goto bail;
		
	status = MCO_SUCCESS;
	
bail:
	if (tweaktype)
		delete tweaktype;

	if (curve) 
		{
		for (i=0; i<12; i++) if (curve[i]) delete curve[i];
		delete curve;
		}

	if (setting) delete setting;

	if (srcpatchtype->steps)	delete srcpatchtype->steps;
	if (srcpatchtype->blacks)	delete srcpatchtype->blacks;
	if (srcpatchtype->cmy)		delete srcpatchtype->cmy;
	if (srcpatchtype->linear)	delete srcpatchtype->linear;
	if (srcpatchtype->ldata)	delete srcpatchtype->ldata;
	if (srcpatchtype->data )	delete srcpatchtype->data;
	if (srcpatchtype) delete srcpatchtype;

	if (destpatchtype->steps)	delete destpatchtype->steps;
	if (destpatchtype->blacks)	delete destpatchtype->blacks;
	if (destpatchtype->cmy)		delete destpatchtype->cmy;
	if (destpatchtype->linear)	delete destpatchtype->linear;
	if (destpatchtype->ldata)	delete destpatchtype->ldata;
	if (destpatchtype->data )	delete destpatchtype->data;
	if (destpatchtype) delete destpatchtype;

	if (cprt_des) delete cprt_des;

	icc->closeFile();
	return status;	
		
}

//load patch data	from file
McoStatus Mpfileio::_loadPatchdata(FILE *fs, long which)
{
	if (which != 0 && which != 1)
		return MCO_FAILURE;

	McoStatus 			status;
	long	i, j, m;
	long	totalcmy, calpatch;
	PatchFormat		*patchdata;
	
	// perform a cleanup of the patch data only
	cleanup(which);
	
	if(which == 1)
		patchdata = &(_printdata->srcpatch);
	else	
		patchdata = &(_printdata->destpatch);
		
	fscanf(fs, "%ld %ld %ld %ld",&patchdata->patchType,&patchdata->numComp,
			&patchdata->numCube,&patchdata->numLinear);
	
	patchdata->steps = new unsigned long[patchdata->numCube];
	
	for(i = 0; i < (long)patchdata->numCube; i++)
		fscanf(fs, "%ld",&patchdata->steps[i]);

	patchdata->blacks = new unsigned long[patchdata->numCube];
	for(i = 0; i < (long)patchdata->numCube; i++)
		fscanf(fs, "%ld",&patchdata->blacks[i]);
	
	totalcmy = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		totalcmy += patchdata->steps[i];

	patchdata->cmy = new unsigned long[totalcmy];
	m = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		for(j = 0; j < (long)patchdata->steps[i]; j++)
			fscanf(fs, "%ld",&patchdata->cmy[m++]);
	
	if(patchdata->numLinear)
		patchdata->linear = new double[patchdata->numLinear];

	for (i=0; i < (long)patchdata->numLinear; i++)
		fscanf(fs,"%lf",&patchdata->linear[i]);

	calpatch = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		calpatch += patchdata->steps[i]*patchdata->steps[i]*patchdata->steps[i];
		
	patchdata->data = new double[calpatch*3];


	for(i = 0; i < calpatch; i++){
		j = i*3;
		if (!feof(fs)) fscanf(fs, "%lf \t %lf \t %lf\n", &patchdata->data[j], &patchdata->data[j+1], &patchdata->data[j+2]);
		else 
			{
			patchdata->data[j] = 0;
			patchdata->data[j+1] = 0; 
			patchdata->data[j+2] = 0;
			}
	}

	patchdata->ldata = 0;
	
	return MCO_SUCCESS;
}

//load patch data	from file
McoStatus Mpfileio::_savePatchdata(FILE *fs, long which)
{
	if (which != 0 && which != 1)
		return MCO_FAILURE;

	McoStatus 			status;
	long	i, j, m;
	long	totalcmy, calpatch;
	PatchFormat		*patchdata;
	
	if(which == 1)
		patchdata = &(_printdata->srcpatch);
	else	
		patchdata = &(_printdata->destpatch);
		
	fprintf(fs, "%ld %ld %ld %ld\n",patchdata->patchType,patchdata->numComp,
			patchdata->numCube,patchdata->numLinear);
	
	for(i = 0; i < (long)patchdata->numCube; i++)
		fprintf(fs, "%ld ",patchdata->steps[i]);

	fprintf(fs, "\n");

	for(i = 0; i < (long)patchdata->numCube; i++)
		fprintf(fs, "%ld ",patchdata->blacks[i]);

	fprintf(fs, "\n");
	
	totalcmy = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		totalcmy += patchdata->steps[i];

	m = 0;
	for(i = 0; i < (long)patchdata->numCube; i++){
		for(j = 0; j < (long)patchdata->steps[i]; j++)
			fprintf(fs, "%ld ",patchdata->cmy[m++]);
		fprintf(fs, "\n");
	}			
	
	for (i=0; i < (long)patchdata->numLinear; i++)
		fprintf(fs,"%lf ",patchdata->linear[i]);

	if (patchdata->numLinear >0) fprintf(fs, "\n");

	calpatch = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		calpatch += patchdata->steps[i]*patchdata->steps[i]*patchdata->steps[i];
		
	for(i = 0; i < calpatch; i++){
		j = i*3;
		fprintf(fs, "%f \t %f \t %f\n", patchdata->data[j], patchdata->data[j+1], patchdata->data[j+2]);
//		fprintf(fs, "%6.4g\t%6.4g\t%6.4g\n", patchdata->data[j], patchdata->data[j+1], patchdata->data[j+2]);
	}

	patchdata->ldata = 0;
	
	return MCO_SUCCESS;
}


McoStatus Mpfileio::_loadLineardata(FILE *fs)
{
	McoStatus 			status;
	long	i, j, m;
	double		*linear[6];
	
	cleanup(3);

	fscanf(fs, "%ld %ld",&_printdata->numlinear,&_printdata->sizelinear);
		
	for(i = 0; i < (long)_printdata->numlinear; i++)
		linear[i] = _printdata->linear[i];
		
	for(i = 0; i < (long)_printdata->sizelinear; i++)
		for(j = 0; j < (long)_printdata->numlinear; j++)
			fscanf(fs, "%lf",&(linear[j][i]) );

	return MCO_SUCCESS;
}

McoStatus Mpfileio::_saveLineardata(FILE *fs)
{
	McoStatus 			status;
	long	i, j, m;
	double		*linear[6];

	fprintf(fs, "%ld %ld\n",_printdata->numlinear, _printdata->sizelinear);
	
	for(i = 0; i < (long)_printdata->numlinear; i++)
		linear[i] = _printdata->linear[i];
		
	for(i = 0; i < (long)_printdata->sizelinear; i++){
		for(j = 0; j < (long)_printdata->numlinear; j++)
			fprintf(fs, "%f ",linear[j][i] );
		fprintf(fs, "\n");
	}		

	return MCO_SUCCESS;
}

McoStatus Mpfileio::_loadTonedata(FILE *fs)
{
	McoStatus 			status;
	long	i, j, m;
	double		*tone[6];
	
	cleanup(4);

	fscanf(fs, "%ld %ld",&_printdata->numtone,&_printdata->sizetone);
		
	for(i = 0; i < (long)_printdata->numtone; i++)
		tone[i] = _printdata->tone[i];
		
	for(i = 0; i < (long)_printdata->sizetone; i++)
		for(j = 0; j < (long)_printdata->numtone; j++)
			fscanf(fs, "%lf",&(tone[j][i]) );

	return MCO_SUCCESS;
}

McoStatus Mpfileio::_saveTonedata(FILE *fs)
{
	McoStatus 			status;
	long	i, j, m;
	double		*tone[6];

	fprintf(fs, "%ld %ld\n",_printdata->numtone, _printdata->sizetone);
	
	for(i = 0; i < (long)_printdata->numtone; i++)
		tone[i] = _printdata->tone[i];
		
	for(i = 0; i < (long)_printdata->sizetone; i++){
		for(j = 0; j < (long)_printdata->numtone; j++)
			fprintf(fs, "%f ",tone[j][i] );
		fprintf(fs, "\n");
	}		

	return MCO_SUCCESS;
}

