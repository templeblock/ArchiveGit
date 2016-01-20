// GretagScan.cpp: implementation of the CGretagScan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GretagScan.h"
#include "PositionDialog.h"
#include "stringfunc.h"
#include "ErrorMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGretagScan::CGretagScan():CDeviceControl(), m_table()
{

}

CGretagScan::~CGretagScan()
{
	MoveToHome();
}

BOOL	CGretagScan::StartComm(void)
{
	char	data[200];
	int		datalen = 200;
	int		code[5];

	// check device type
	if(!Command("; 43\r\n", 6, data, datalen, 5) )
	{
		ErrorMessage(ERR_NODEVICE);
		return FALSE;
	}
	if(!sscanf_n_i(data, 27, 1, &code[0]) || code[0] != 2)
	{
		ErrorMessage(ERR_NODEVICE);
		return FALSE;
	}

	//reset
	if(!Command("; 90 1 4 5\r\n", 12, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,2,code))
	{
		ErrorMessage(ERR_DEVICE_RESET);
		return FALSE;
	}
	if( !CheckError(code) )
		return FALSE;

	//set measurment type
	if(!Command("; 77 155\r\n", 10, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,2,code))
	{
		ErrorMessage(ERR_DEVICE_MEASUREMENT_TYPE);
		return FALSE;
	}
	if( !CheckError(code) )
		return FALSE;
	
	// set the illuminante, angle
	if(!Command("; 22 1 1 3 0\r\n", 14, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,2,code))
	{
		ErrorMessage(ERR_DEVICE_ILLUMINATION);
		return FALSE;
	}
	if( !CheckError(code) )
		return FALSE;

	//calibrate before reading
	if( !Calibrate() )
		return FALSE;

	//align table with patch target 
	if( !AlignTable() )
		return FALSE;

	return TRUE;
}

BOOL CGretagScan::PostPositionDialog(int which)
{
	CPositionDialog positiondialog;

	positiondialog.m_type = 1;
	positiondialog.m_position = which;

	if(positiondialog.DoModal() == IDOK)
		return TRUE;
	else
		return FALSE;
}

BOOL	CGretagScan::AlignTable()
{
	double patchpos[6];
	int		headpos[6];
	
	//setup table

	m_patchLayout.strips = 0;
	m_patchLayout.sheets = 1000000;
	m_patchLayout.rows = 34;
	m_patchLayout.columns = 44;
	m_patchLayout.starting = 0;
	m_patchLayout.direction = 1;
	m_patchLayout.width = 279.3;
	m_patchLayout.height = 215.8;

/*
	m_patchLayout.strips = 0;
	m_patchLayout.sheets = 1000000;
	m_patchLayout.rows = 13;
	m_patchLayout.columns = 8;
	m_patchLayout.starting = 0;
	m_patchLayout.direction = 1;
	m_patchLayout.width = 142.5;
	m_patchLayout.height = 198.5;
*/
	m_tableRect.min_x = 186;
	m_tableRect.max_x = 3360;
	m_tableRect.min_y = 230;
	m_tableRect.max_y = 2617;

	m_table.setUpPatches(m_patchLayout.columns,m_patchLayout.rows,
				m_patchLayout.width,m_patchLayout.height,
				m_patchLayout.starting,m_patchLayout.direction, &m_tableRect);

	//get default patch position
	GetPatchPos(0, &patchpos[0]);
	GetPatchPos(m_patchLayout.rows-1, &patchpos[2]);
	GetPatchPos(m_patchLayout.rows*(m_patchLayout.columns-1), &patchpos[4]);

	//move to the first patch
	if(!MoveToPatch(0,1))
		return FALSE;

	//move head down
//	if(!PutHeadDown())
//		return FALSE;
	
	//post position image, and let user to adjust the head
	if(!TurnOffLine())
		return FALSE;

	if( !PostPositionDialog(0) )
		return FALSE;

	//get the current position of patch
	if(!GetHeadLocation(headpos, 1) )
		return FALSE;

	//realign the position of the table
	m_table.setPoints_1((long*)headpos, patchpos);


	//move to the second patch
	if(!MoveToPatch(m_patchLayout.rows-1, 1))
		return FALSE;

	//move head down
//	if(!PutHeadDown())
//		return FALSE;

	//post position image, and let user to adjust the head
	if(!TurnOffLine())
		return FALSE;

	if( !PostPositionDialog(1) )
		return FALSE;

	//get the current position of patch
	if(!GetHeadLocation(&headpos[2], 1) )
		return FALSE;

	//realign the position of the table
	m_table.setPoints_2((long*)headpos, patchpos);


	//move to the second patch
	if(!MoveToPatch(m_patchLayout.rows*(m_patchLayout.columns-1), 1))
		return FALSE;

	//move head down
//	if(!PutHeadDown())
//		return FALSE;

	//post position image, and let user to adjust the head
	if(!TurnOffLine())
		return FALSE;

	if( !PostPositionDialog(2) )
		return FALSE;

	//get the current position of patch
	if(!GetHeadLocation(&headpos[4], 1) )
		return FALSE;

	//realign the position of the table
	m_table.setPoints_3((long*)headpos, patchpos);

	return TRUE;
}

// get the current location of the head on the table
BOOL CGretagScan::GetHeadLocation(int *position, int refrence)
{
	char command[100];
	char data[200];
	int	datalen = 200;
	int	code[5];
	
	sprintf(command,"; 208 5 %d\r\n",refrence);
	if(!Command(command, strlen(command), data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,5,2,code))
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}

	position[0] = code[0];
	position[1] = code[1];

	return TRUE;
}

BOOL	CGretagScan::Calibrate()
{
	char	data[200];
	int		datalen = 200;
	int		code[5];
	static	int first = 5;

	//turn online
	if(!TurnOnLine())
		return FALSE;

	//initialization
	if(!Command("; 208 6 0\r\n", 11, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_INITIALIZATION);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	// put head down
	if(!Command("; 208 4\r\n", 9, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_HEAD_DOWN);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	//warm it up
	for (int i = 0; i < first; i++)
	{		
		if( !Command("; 32\r\n", 6, data, datalen, 5) )
		{
			ErrorMessage(ERR_DEVICE_READ);
			return FALSE;
		}
		if(!sscanf_n_i(data,1,2,code))
		{
			ErrorMessage(ERR_DEVICE_WARM_UP);
			return FALSE;
		}
		if( !CheckError(code) )
			return FALSE;
	}

	// calibrate
	if( !Command("; 34 9 7\r\n", 10, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,2,code))
	{
		ErrorMessage(ERR_CALIBRATE);
		return FALSE;
	}
	if( !CheckError(code) )
		return FALSE;

	// move the head up
	if( !Command("; 208 3\r\n", 9, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_HEAD_UP);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	return TRUE;
}

BOOL CGretagScan::TurnOnLine(void)
{
	char	data[200];
	int		datalen = 200;
	int		code[5];

	//turn online
	if(!Command("; 208 16\r\n", 10, data, datalen) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_TURN_ONLINE);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	return TRUE;
}

BOOL CGretagScan::TurnOffLine(void)
{
	char	data[200];
	int		datalen = 200;
	int		code[5];

	//turn online
	if(!Command("; 208 17\r\n", 10, data, datalen) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_TURN_OFFLINE);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	return TRUE;
}

BOOL CGretagScan::PutHeadDown()
{
	char	data[200];
	int		datalen = 200;
	int		code[5];

	// put head down
	if(!Command("; 208 4\r\n", 9, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_HEAD_DOWN);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	return TRUE;
}

BOOL CGretagScan::PutHeadUp()
{
	char	data[200];
	int		datalen = 200;
	int		code[5];

	// put head down
	if(!Command("; 208 3\r\n", 9, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_HEAD_DOWN);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	return TRUE;
}

BOOL CGretagScan::MoveToHome()
{
	char	data[200];
	int		datalen = 200;

	if(!Command("; 208 16\r\n", 10, data, datalen))
		return FALSE;
	if(!Command("; 208 2\r\n", 9, data, datalen, 5))
		return FALSE;

	return TRUE;
}

BOOL CGretagScan::MoveToPatch(int patchnum, int ref)
{
	char command[200];
	char data[200];
	int	datalen = 200;
	int	code[5];
	int32 dx_i,dy_i;
	int32	table[2];

	//turn online
	if(!TurnOnLine())
		return FALSE;

	m_table.translateToTable(patchnum,table);

	sprintf(command,"; 208 0 %d %ld %ld\r\n",ref,(long)table[0],(long)table[1]);

//	dx_i = table[0]-current_x;
//	dy_i = table[1]-current_y;

	if(!Command(command, strlen(command), data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_MOVE_HEAD);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	return TRUE;
}

BOOL	CGretagScan::SendRequest(long whichpatch, CWnd *wnd)
{
	char	data[200];
	int		datalen = 200;
	int		code[5];

	//put head up
	if(!Command("; 208 3\r\n", 9, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_HEAD_UP);
		return FALSE;
	}
//	if( !CheckErrorChart(code) )
//		return FALSE;

	// move to the patch
	if(!MoveToPatch(whichpatch,0))
		return FALSE;

	// put head down
	if(!Command("; 208 4\r\n", 9, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,3,code))
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if( !CheckErrorChart(code) )
		return FALSE;

	// make a measurment
	if(!Command("; 32\r\n", 6, data, datalen, 5) )
	{
		ErrorMessage(ERR_DEVICE_READ);
		return FALSE;
	}
	if(!sscanf_n_i(data,1,2,code))
	{
		ErrorMessage(ERR_DEVICE_MEASUREMENT);
		return FALSE;
	}
	if( !CheckError(code) )
		return FALSE;

	//send a request to read the data in
	if(m_type == 1) //LAB
	{
		if(!RequestCommand("; 186 9 1\r\n", datalen, wnd))
		{
			ErrorMessage(ERR_SEND_REQUEST);
			return FALSE;
		}
	}
	else
	{
		if(!RequestCommand("; 188 9\r\n", datalen, wnd))
		{
			ErrorMessage(ERR_SEND_REQUEST);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL	CGretagScan::ProcessResult( CCommRequest *pRequest, double *data)
{
	int	code[5];

	if(pRequest->m_dwRequestType == REQUEST_COMMAND){
		if(m_type == 1) //LAB
			return GetLab(pRequest, data);
		else
			return GetDensity(pRequest, data);
	}

	return FALSE;
}

BOOL	CGretagScan::GetLab(CCommRequest *pRequest, double *data)
{
	int	code[5];

	if(!sscanf_n_i(pRequest->m_lpszResult, 1, 1, code))
	{
		ErrorMessage(ERR_MEASUREMENT_DATA);
		return FALSE;
	}
	if(code[0] != 187)
	{
		ErrorMessage(ERR_MEASUREMENT_DATA);
		return FALSE;
	}

	//convert to data
	sscanf_n(pRequest->m_lpszResult ,4 ,3, data);

	return TRUE;
}

BOOL	CGretagScan::GetDensity(CCommRequest *pRequest, double *data)
{
	int	code[5];
	int i;
	double temp;
	BOOL	status = FALSE;

	if(!sscanf_n_i(pRequest->m_lpszResult, 1, 1, code))
	{
		ErrorMessage(ERR_MEASUREMENT_DATA);
		goto bail;
	}
	if(code[0] != 189)
	{
		ErrorMessage(ERR_MEASUREMENT_DATA);
		goto bail;
	}

	//convert to data
	sscanf_n(pRequest->m_lpszResult ,3 ,4, data);

	//resequence
	temp = data[0];
	for(i = 0; i < 3; i++)
		data[i] = data[i+1];
	data[3] = temp;
	status = TRUE;

bail:
	return status;
}


BOOL	CGretagScan::GetPatchPos(int patchnum, double *pos)
{
	m_table.getPatchPoint(patchnum, pos);
	return TRUE;
}

BOOL	CGretagScan::CheckStart(char *data)
{
	if( (data[0] != ';') && (data[0] != ':') )
		return FALSE;

	return TRUE;
}

BOOL	CGretagScan::CheckError(int *code)
{
	//c = sscanf_n_i(st,1,2,code_error);	
	if ((code[0] != 31) && (code[0] != 37)) // is code the right code?
	{
		ErrorMessage(ERR_DEVICE);
		return FALSE;
	}
	if ((code[1] != 0) && (code[1] != 19)) 
	{
		ErrorMessage(ERR_DEVICE);
		return FALSE;
	}
	return TRUE;
}
		
BOOL	CGretagScan::CheckErrorChart(int *code)
{
//	c = sscanf_n_i(data,1,3,code_error);
	if (code[0] != 209)
	{
		ErrorMessage(ERR_DEVICE);
		return FALSE;
	}
	if (code[1] != 128)
	{
		ErrorMessage(ERR_DEVICE);
		return FALSE;
	}
	if (code[2] != 0)
	{
		ErrorMessage(ERR_DEVICE);
		return FALSE;
	}

	return TRUE;
}
