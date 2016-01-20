///////////////////////////////////////////////////////////////////////////////////
//	profiledocmac.h															  	 //
//																			  	 //
//	The code for dealing with the profile data, replaces much of printdialog.c	 //
//  This is the Mac specific part											     //
//	James Vogh																 	 //
// June 4, 1996																  	 //
///////////////////////////////////////////////////////////////////////////////////

#ifndef PRINT_PROFILE_MAC_H
#define PRINT_PROFILE_MAC_H

#include "profiledocinf.h"
#include "csprofile.h"
#include "mcoiccmac.h"

class ProfileDocMac:public ProfileDocInf{

private:
protected:



	//double _adjustangle(double angle);
	long _transfer_to_pos(double *lch);

	OSErr FindAProcess(OSType typeToFind, OSType creatorToFind,
			ProcessSerialNumberPtr processSN);

	OSErr OpenSelection(FSSpecPtr theDoc);
		
	
	//McoStatus _send_target(short which);

	//added on 9/19
	McoStatus _handle_read_datafile(void);
	McoStatus _handle_read_sim_datafile(void);
	McoStatus _LoadPatchLocations(McoPatches* patches,RawData *PatchD);
	McoStatus _buildPatchData(RawData *PatchD);
	
	// set up for saving safely
	McoStatus _setupSafeSave(void);
	// exchange contents of the two files
	McoStatus _cleanupSafeSave(void);	
	
	int DoSafeSave;
	FSSpec	safe_fsspec;

	
public:
	McoPatches *_patches;	//for reading from a Tiff file
	unsigned char	*_scaned_rgb;//for reading from a Tiff file
	
	McoXformIccMac *scanner;	// the icc transform for the scanner

	int		valid_fsspec;
	FSSpec	icc_fsspec;


	ProfileDocMac(void);
	~ProfileDocMac(void);
	
	McoStatus GetFileName(int flag);
	McoStatus _makelinkProfile(McoColorSync *prof,McoXformIccMac *link);
	McoStatus handle_read_iccfile(void);
	McoStatus handle_read_iccfile(FSSpec *fspec);
	McoStatus handle_reread_iccfile(void);
	McoStatus _handle_read_datafile(FSSpec *fspec);
	McoStatus savetoPhototable(int32 pass);
	McoStatus savetoRGBtoRGB(void);
	McoStatus savetoIcctable(void);
	McoStatus savetoCrdtable(void);
	McoStatus savetoCrdtable_old(void);
	McoStatus handle_save_datafile(RawData *p);
	McoStatus duplicateFile(FSSpec *new_fsspec);
	McoStatus savetoIccLinktable(void);
	McoStatus saveToneAndLinear(void);
	McoStatus savetoTekCmyktable(void);
	McoStatus _handle_read_tifffile(RawData *PatchD);
	McoStatus	aftercrop(Rect	*selectrect, RawData *PatchD);
	int	validFile(void);

};

McoStatus setUpCType(Ctype *CT);
void cleanupCType(Ctype *CT);
#endif