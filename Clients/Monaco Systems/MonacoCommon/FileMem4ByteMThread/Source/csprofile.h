#ifndef COLOR_SYNC_PROFILE_H
#define COLOR_SYNC_PROFILE_H

#include "CMICCProfile.h"
#include "mcotypes.h"
#include "mcostat.h"
#include "mcomem.h"
#include "FileFormat.h"
#include "icctypes.h"


#define MCOP_DESC			("Profile by Monaco Systems Inc.")
#define MCOP_INPUT_DESC		("Input profile by Monaco Systems Inc.")
#define MCOP_DISPLAY_DESC	("Display profile by Monaco Systems Inc.")
#define MCOP_OUTPUT_DESC	("Output profile by Monaco Systems Inc.")
#define MCOP_CPRT	("Monaco Systems Inc.")
#define MCOP_IDF	("/Monaco Systems")
#define MONACO_CMM		'mnco'
#define MCOP_MAJOR_VERSION	(cmCurrentProfileMajorVersion)
#define MCOP_MINOR_VERSION	(0x00000000)
#define MCOP_DEFAULT_CON_SPACE	(cmLabData)
#define MCOP_DEFAULT_PLATFORM	('APPL')
#define MCOP_DEFAULT_FLAGS	(0x00020000)
#define MCOP_DEFAULT_MANUFACTURER	('   ')
#define MCOP_DEFAULT_MODEL	(0x00000000)
#define MCOP_DEFAULT_ATTRIBUTES	(0x00000000)
#define MCOP_DEFAULT_RENDERINGINTENT	(0x00000000)
#define MCOP_DEFAULT_WHITE_X	(0x0000F6D4)
#define MCOP_DEFAULT_WHITE_Y	(0x00010000)
#define MCOP_DEFAULT_WHITE_Z	(0x0000D32B)

//some tag information
#define MCOP_DEFAULT_ENTRIES	(256)

#define MC0P_SETTING_TAG		('mn01')
#define MCOP_SRC_PATCH_TAG		('mn02')
#define MCOP_DEST_PATCH_TAG		('mn03')

#define MC0P_C_LINEAR_TAG		('mn04')
#define MC0P_M_LINEAR_TAG		('mn05')
#define MC0P_Y_LINEAR_TAG		('mn06')
#define MC0P_K_LINEAR_TAG		('mn07')

#define MC0P_C_TONE_TAG			('mn08')
#define MC0P_M_TONE_TAG			('mn09')
#define MC0P_Y_TONE_TAG			('mn10')
#define MC0P_K_TONE_TAG			('mn11')

#define MC0P_TWEAK_TAG			('mn12')

//monaco private tag
//print setting tag

#define MNSETTINGTYPE			('pset')
typedef struct MNSettingType {
//	unsigned long 					typeDescriptor;				/* 'pset' */
//	unsigned long 					reserved;					/* fill with 0x00 */
	char							toneDesc[100];
	char							linearDesc[100];
	unsigned long 					tableType;
	unsigned long 					separationType;			
	unsigned long 					blackType;
	unsigned long 					totalInkLimit;				
	unsigned long 					blackInkLimit;				
	unsigned long 					saturation;		
	unsigned long 					colorBalance;
	unsigned long 					colorBalanceCut;
	unsigned long 					linear;
	unsigned long 					inkLinearize;
	unsigned long 					inkNeutralize;
	unsigned long 					inkHighlight;
	unsigned long 					simulate;
	unsigned long 					smooth;
	unsigned long 					revTableSize;
	unsigned long 					smoothSimu;
	unsigned long 					kOnly;
	Fixed							blackCurve[101];
	unsigned long					numBlackHand;
	Fixed							blackX[31];
	Fixed							blackY[31];
	unsigned long					numLinearHand[6];
	Fixed							linearX[MAX_LINEAR_HAND*6];
	Fixed							linearY[MAX_LINEAR_HAND*6];
	unsigned long					numToneHand[6];
	Fixed							toneX[MAX_TONE_HAND*6];
	Fixed							toneY[MAX_TONE_HAND*6];
	unsigned long					flag;
	unsigned long					reserved[99];
} MNSettingType;

#define MNPATCHTYPE				('ppat')
typedef struct MNPatchType {
//	unsigned long 					typeDescriptor;				/* 'ppat' */
//	unsigned long 					reserved;					/* fill with 0x00 */
	char							name[100];				
	unsigned long 					patchType;
	unsigned long 					numComp;			
	unsigned long 					numCube;
	unsigned long 					numLinear;				
	unsigned long					*steps;					/* Variable size */
	unsigned long					*blacks;					
	unsigned long					*cmy;						
	Fixed							*linear;					
	Fixed							*data;					
	Fixed							*ldata;					
} MNPatchType;

#define MNTWEAKTYPE			('ptwk')
typedef struct MNTweakType{
Fixed 	lab_d[3];
Fixed 	lab_g[3];
Fixed	lab_p[3];
Fixed	Lr,Cr;
unsigned long	type;
char	name[40];
}MNTweakType;


//end of monaco private tag	

class CsFormat:public FileFormat {
private:
	
protected:
	CM2Header _header;
	CMTagRecord _tags[40];	//support up to 40 functions
	long	_tagcount;

	McoHandle _A2B0_H;	//lut
	McoHandle _B2A0_H;	//lut
	McoHandle _out_of_gamut_H;	//lut
	McoHandle _A2B1_H;	//lut
	McoHandle _B2A1_H;	//lut
	McoHandle _A2B2_H;	//lut
	McoHandle _B2A2_H;	//lut
	
	long	_has_linear;	//has linear table or not
	char	*_input_entries;
	char 	*_output_entries;
	
	long	_luttype;
	long	_profileclass;
	
	Ctype _A2B0;	//1
	Ctype _B2A0;	//2
	Ctype _out_of_gamut;	//3
	Ctype _A2B1;	//4
	Ctype _B2A1;	//5
	Ctype _A2B2;	//6
	Ctype _B2A2;	//7

	CMXYZType _red;
	CMXYZType _green;
	CMXYZType _blue;
	CMXYZType _white;
	CMXYZType _black;

	//added for raw patch data
	MNPatchType *_srcpatch, *_destpatch;
	
	long	_numtweak;
	MNTweakType *_tweak;
	
	//setting data
	long	*_settingdata;

	//linear data
	Fixed	*_clineardata, *_mlineardata, *_ylineardata, *_klineardata;
	
	//tone data
	Fixed *_ctonedata, *_mtonedata, *_ytonedata, *_ktonedata; 
	
	CMCurveType _rcurveh, _gcurveh, _bcurveh;
	unsigned short _rcurve[256], _gcurve[256], _bcurve[256];

	char	_desc[256];
	long 	_desclen;	
	char	_cprt[256];
	long 	_cprtlen;	
	char	_pseq[256];

	long 		_next_offset(long offset, long add);

	long		_calCurveTypesize(CMCurveType *curve);
	long		_calLutTypesize(Ctype* AB);
	long		_calPatchTypesize(MNPatchType *patch);
	
	McoStatus	_writeCurvetype(CMTagRecord *tag, CMCurveType* curve, char* value);
	McoStatus	_writeLuttype(CMTagRecord *tag, long luttype, long rendertype, short cont);
	McoStatus	_writeTagdata(CMTagRecord *tag, char *data);
	McoStatus	_writes15Fixed16ArrayTypedata(CMTagRecord *tag, Fixed *value);
	McoStatus	_writeDatatype(CMTagRecord *tag, CMDataType* taghead, char* value);
	McoStatus	_writeSettingtype(CMTagRecord *tag);
	McoStatus	_writePatchtype(CMTagRecord *tag, MNPatchType *patch);
	McoStatus	_writeTweakTypedata(CMTagRecord *tag);
	
	
	void	_setDefaultTags(void);

	//return address of the tag record
	CMTagRecord* _getTagrecord(long tag);

	McoStatus	_extractHeader(void);

	long	_validinputGrayProfile(void);
	long	_validinputRGBProfile(void);
	long	_validinputCMYKProfile(void);
	long	_validdisplayGrayProfile(void);
	long	_validdisplayRGBProfile(void);
	long	_validoutputGrayProfile(void);
	long	_validoutputRGBProfile(void);
	long	_validoutputCMYKProfile(void);
	long	_validlinkProfile(void);
	long	_validspaceProfile(void);
	long	_validabstractProfile(void);

	void	_swabTagRecord(CMTagRecord *record, u_int32 num);
	void	_swabLut8Type(CMLut8Type *luttype);
	void	_swabLut16Type(CMLut16Type *luttype);
	void	_swabTagType(u_int32* tagtype);
	void	_swabCurveType(u_int32* curvetype);
	void	_swabXYZType(u_int32* xyztype);
	void	_swabProfileSequenceDescTagType(u_int32* data);

public:
	CsFormat(void);
	~CsFormat(void);

	Boolean		isVersion2(void);

//set tag header functions	
	McoStatus	setHeader(long profileclass, long luttype);
	McoStatus	setHeader(CM2Header* header, long luttype);

//set tag data functions
	McoStatus	setCurvetypedata(long type, CMCurveType *ch, unsigned short *data);
	McoStatus	setDesctypedata(char* name, long namelen);
	McoStatus	setCprttypedata(char* name, long namelen);
	void		setPseqtypedata(char* name, long namelen);
	McoStatus	setLuttypedata(long rendertype, Ctype *ctype, McoHandle dataH);
	McoStatus	setPseqtypedata(long times);
	McoStatus	setXYZtypedata(long type, CMXYZType	*xyz);	
	
	McoStatus	sets15Fixed16ArrayTypedata(long tag, long size, Fixed *data);
	McoStatus	setDataTypedata(long tag, long size, char *data);

//monaco private tag
	McoStatus	setSettingTypedata(MNSettingType	*setting); 
	McoStatus	setPatchTypedata(long type, MNPatchType *patch);

	McoStatus 	setTweaktypedata(long numTweaks, MNTweakType *tweaktype);
	
	
	McoStatus	getCurveTypeinfo(long tag, CMCurveType *curve);
	McoStatus	getcurveTagdata(long tag, char *data);

	McoStatus	gets15Fixed16ArrayTagdata(long tag, char *data);
	McoStatus	getMonacoSettingTagdata(char *data);
	McoStatus	getMonacoPatchTagdata(long tag, MNPatchType *patchtype);
	McoStatus	getTweakTagdata(long tag, MNTweakType *tweaktype);
	
//write tags functions	
	McoStatus	writeTag(short	cont);	//continous writing or not

//get tag header functions
	McoStatus	getHeader(CM2Header* header, long* tagcount);

//get tag info functions	

	//get a tag info from index	
	//index is 1 based
	McoStatus	getIndexTaginfo(long index, CMTagRecord* tags);

	//get all the tag info once and for all
	McoStatus	getAllTaginfo(CMTagRecord* tags);

	long	getTagsize(long tag);

//get tag data functions	
	//get all the data from a tag
	McoStatus	getTagdata(CMTagRecord *tag, unsigned char *data);
	McoStatus	getTagdata(long tag, unsigned char *data);

	McoStatus	getPartialTagdata(CMTagRecord *tag, long offset, long datasize, char *data);
	McoStatus	getPartialTagdata(long tag, long offset, long datasize, char *data);

	//get raw data from the tag, not including the tag info
	//only for A2B0, A2B1, A2B2, B2A0, B2A1, B2A2
	McoStatus	getLutTagdata(long tag, unsigned char *data);
	McoStatus	getLutTagdatainfo(long tag, CMLut16Type *tag16info);

//tag check functions
	long	checkTagExist(long tag);
	
//return 1 if valid, otherwise return 0	
	long	validProfile(void);

	//added on 10/13 for continuing writing
	long		getA2B0position(void);
	long		getA2B1position(void);
	long		getA2B2position(void);
};

#endif //COLOR_SYNC_PROFILE_H
