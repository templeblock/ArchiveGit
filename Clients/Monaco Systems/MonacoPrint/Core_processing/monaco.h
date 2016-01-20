//some difinitions used in several source files

#ifndef MONACO_H
#define MONACO_H

#define TIFF			1
#define PHOTOSHOP25 	2
#define EPS				3

//eps file format
#define EPS_SEP_OFF		0 //one file
#define EPS_SEP_ON		1 //five files

#define GRAY			1
#define RGB				2
#define CMYK			3

//raw image status after process
#define KEEP_RAW		1
#define REMOVE_RAW		2
#define MOVE_RAW		3

//processing status
#define IDLING			1
#define WORKING			2
#define WAITING			3

#endif	//MONACO_H
