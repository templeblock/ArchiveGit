#ifndef RCHOOSE_DIALOG_H
#define RCHOOSE_DIALOG_H

#include "mdialog.h"
#include "profiledocinf.h"



#ifndef IN_POLAROID
#define Rchoose_Dialog		(2005)

#define FileType				(4)
#define PatchFormat				(5)
#define DevicePort				(6)

#define DataFile				(-1)
#define TiffFile				(1)
#define DTP51					(3)
#define DTP41					(-2)
#define XR408					(-3)
#define TechkonCP300			(4)
#define TechkonSP820			(5)
#define TechkonTCRCP300			(6)
#define TechkonTCRSP820			(7)
#define GretagHand				(8)
#define GretagTable				(9)

#define DeviceMenuID			(10002)

#else
#define Rchoose_Dialog		(2007)

#define FileType				(4)
#define PatchFormat				(5)
#define DevicePort				(6)

#define DataFile				(-1)
#define TiffFile				(-2)
#define DTP51					(-3)
#define DTP41					(-4)
#define XR408					(1)
#define TechkonCP300			(-5)
#define TechkonSP820			(3)
#define TechkonTCRCP300			(-6)
#define TechkonTCRSP820			(4)
#define GretagHand				(5)
#define GretagTable				(6)

#define DeviceMenuID			(10005)
#endif

class RchooseDialog:public MDialog{

private:
protected:
	short	_which;		//which way to read
	short	_which_patch;	//which target size
	short	_which_port; // the port the device is connected to
	
	int32	convert[MAX_PATCH_NAMES+1];
	int32	convert2[MAX_PATCH_NAMES+1];
	
	int		density;

	ProfileDocInf *doc;

	void setDialogState(void);

	void 	_Savesettings(void);
	
	int		convertToDefault(int which);

public:
	RchooseDialog(ProfileDocInf *doc,int density);
	~RchooseDialog(void);
	

	virtual int16	hit(void);
	
	short getchoice(void);
};

#endif //LOGO_DIALOG_H