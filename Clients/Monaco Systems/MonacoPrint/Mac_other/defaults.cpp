//////////////////////////////////////////////////////////////////////////////////////////
//  default parameters for monacoprint													//
//  defaults.cpp																		//
//  James Vogh																			//
//////////////////////////////////////////////////////////////////////////////////////////

#include "defaults.h"
#include "mcostat.h"
#include "fileformat.h"
#include "monacoprint.h"
#include "think_bugs.h"

Defaults::Defaults(void)
{
FileFormat *filef;
FSSpec fspec;
McoStatus status;
int		magic_num;
int		version;
Str255  def = MONACO_DEFAULT;
Str255  fold = MONACO_FOLDER;

// set factory defaults
printer_type = 1;
input_type = DT_None;
patch_format = 1;
port = 1;

// now try to load in the user defults
filef = new FileFormat;

ptocstr(def);
ptocstr(fold);

status = filef->openFilefromPref(&fspec,(char*)def,(char*)fold);
if (status != MCO_SUCCESS) goto bail;

status = filef->relRead(sizeof(int),(char*)&magic_num);
if (status != MCO_SUCCESS) goto bail;

if (magic_num != DEFAULTS_MAGIC_NUM) goto bail;

status = filef->relRead(sizeof(int),(char*)&version);
if (status != MCO_SUCCESS) goto bail;

if (version > DEFAULTS_VERSION) goto bail;

status = filef->relRead(sizeof(int),(char*)&printer_type);
if (status != MCO_SUCCESS) goto bail;

status = filef->relRead(sizeof(int),(char*)&input_type);
if (status != MCO_SUCCESS) goto bail;

status = filef->relRead(sizeof(int),(char*)&patch_format);
if (status != MCO_SUCCESS) goto bail;

status = filef->relRead(sizeof(int),(char*)&port);
if (status != MCO_SUCCESS) goto bail;

filef->closeFile();

bail:
delete filef;

}


Defaults::~Defaults(void)
{
FileFormat *filef;
FSSpec fspec;
McoStatus status;
int		magic_num = DEFAULTS_MAGIC_NUM;
int		version = DEFAULTS_VERSION;
Str255  def = MONACO_DEFAULT;
Str255  fold = MONACO_FOLDER;

// now try to load in the user defults
filef = new FileFormat;

ptocstr(def);
ptocstr(fold);

status = filef->createFileinPref(&fspec,(char*)def,(char*)fold);
if (status != MCO_SUCCESS) return;

status = filef->openFile(&fspec);
if (status != MCO_SUCCESS) return;

status = filef->relWrite(sizeof(int),(char*)&magic_num);
if (status != MCO_SUCCESS) return;

status = filef->relWrite(sizeof(int),(char*)&version);
if (status != MCO_SUCCESS) return;

status = filef->relWrite(sizeof(int),(char*)&printer_type);
if (status != MCO_SUCCESS) return;

status = filef->relWrite(sizeof(int),(char*)&input_type);
if (status != MCO_SUCCESS) return;

status = filef->relWrite(sizeof(int),(char*)&patch_format);
if (status != MCO_SUCCESS) return;

status = filef->relWrite(sizeof(int),(char*)&port);
if (status != MCO_SUCCESS) return;

filef->setInfo(&fspec,MONACO_SIG,'MCO7');

filef->closeFile();
delete filef;
}