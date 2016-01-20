#ifndef ERROR_MESSAGE_H
#define ERROR_MESSAGE_H

#include "afxwin.h"

#define	ERR_NOERROR		0
#define	ERR_CANCELLED	1
#define	ERR_TERMINATED	2
#define	ERR_GENERAL		3
#define	ERR_MEMORY		4

//general device error
#define	ERR_DEVICE		200
#define	ERR_NODEVICE	201
#define	ERR_NOPORT		202
#define	ERR_TIMEOUT		203
#define	ERR_DEVICE_RESET	204
#define	ERR_DEVICE_ILLUMINATION	205
#define	ERR_DEVICE_MEASUREMENT_TYPE 206
#define	ERR_DEVICE_READ		207
#define	ERR_INITIALIZATION	208
#define	ERR_TURN_ONLINE		209
#define ERR_TURN_OFFLINE	210
#define	ERR_MOVE_HEAD		211
#define	ERR_HEAD_UP			212
#define	ERR_HEAD_DOWN		213
#define	ERR_DEVICE_WARM_UP	214
#define	ERR_DEVICE_MEASUREMENT	215
#define	ERR_SEND_REQUEST	216
#define	ERR_MEASUREMENT_DATA	217
#define	ERR_CALIBRATE		218
#define	ERR_MAXTRIES		219
#define	ERR_OPENPORT		220


//thread error
#define	ERR_NOTHREAD	300

void ErrorMessage(DWORD error);


#endif //ERROR_MESSAGE_H
