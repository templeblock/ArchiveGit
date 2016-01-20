#include "ErrorMessage.h"
#include "resource.h"

void ErrorMessage( DWORD error)
{
	CString msg;

	switch( error) {
		case ERR_NOERROR:
		case ERR_CANCELLED:
		case ERR_TERMINATED:
			return;

		case ERR_GENERAL:
			msg.LoadString( IDS_ERR_GENERAL);
			break;
			
		case ERR_MEMORY:
			msg.LoadString( IDS_ERR_MEMORY);
			break;

		//device errors
		case ERR_MAXTRIES:
			msg.LoadString( IDS_NOPORTERR);
			break;

		case ERR_DEVICE:
			msg.LoadString( IDS_ERR_DEVICE);
			break;
		case ERR_NODEVICE:
			msg.LoadString( IDS_ERR_NODEVICE);
			break;
		case ERR_NOPORT:
			msg.LoadString( IDS_ERR_NOPORT);
			break;
		case ERR_TIMEOUT:
			msg.LoadString( IDS_ERR_TIMEOUT);
			break;
		case ERR_DEVICE_RESET:
			msg.LoadString( IDS_ERR_DEVICE_RESET);
			break;
		case ERR_DEVICE_ILLUMINATION:
			msg.LoadString( IDS_ERR_DEVICE_ILLUMINATION);
			break;
		case ERR_DEVICE_MEASUREMENT_TYPE:
			msg.LoadString( IDS_ERR_DEVICE_MEASUREMENT_TYPE);
			break;
		case ERR_DEVICE_READ:
			msg.LoadString( IDS_ERR_DEVICE_READ);
			break;
		case ERR_INITIALIZATION:
			msg.LoadString( IDS_ERR_INITIALIZATION);
			break;
		case ERR_TURN_ONLINE:
			msg.LoadString( IDS_ERR_TURN_ONLINE);
			break;
		case ERR_TURN_OFFLINE:
			msg.LoadString( IDS_ERR_TURN_OFFLINE);
			break;
		case ERR_MOVE_HEAD:
			msg.LoadString( IDS_ERR_MOVE_HEAD);
			break;
		case ERR_HEAD_UP:
			msg.LoadString( IDS_ERR_HEAD_UP);
			break;
		case ERR_HEAD_DOWN:
			msg.LoadString( IDS_ERR_HEAD_DOWN);
			break;
		case ERR_DEVICE_WARM_UP:
			msg.LoadString( IDS_ERR_DEVICE_WARM_UP);
			break;
		case ERR_DEVICE_MEASUREMENT:
			msg.LoadString( IDS_ERR_DEVICE_MEASUREMENT);
			break;
		case ERR_SEND_REQUEST:
			msg.LoadString( IDS_ERR_SEND_REQUEST);
			break;
		case ERR_MEASUREMENT_DATA:
			msg.LoadString( IDS_ERR_MEASUREMENT_DATA);
			break;
		case ERR_CALIBRATE:
			msg.LoadString( IDS_ERR_CALIBRATE);
			break;
		case ERR_OPENPORT:
			msg.LoadString( IDS_ERR_OPENPORT);
			break;

		//thread error
		case ERR_NOTHREAD:
			msg.LoadString( IDS_ERR_NOTHREAD);
			break;

		default:
		{
			msg.Format( IDS_GENERALERR, error);
			break;
		}
	}

	AfxMessageBox( msg, MB_OK|MB_ICONSTOP);
}
