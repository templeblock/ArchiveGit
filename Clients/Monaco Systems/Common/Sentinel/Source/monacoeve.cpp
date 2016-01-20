#include "monacoeve.h"
#include <stdio.h>
#include <string.h>

BOOL IsColorGenie(void)
{
	RB_SPRO_APIPACKET      ApiPacket;
	SP_STATUS status;
	RB_WORD gDevID =0x4779;
	RB_BYTE query[] = {0xB5, 0xED, 0xD6, 0xAD};
	RB_BYTE	response[SPRO_MAX_QUERY_SIZE+1];
	RB_BYTE	answer[] = {0x89, 0xED, 0x90, 0xE4};
	RB_DWORD	response32 = 0;
	int i;


	status = RNBOsproFormatPacket( ApiPacket, sizeof(ApiPacket) );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproInitialize( ApiPacket );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproFindFirstUnit(ApiPacket,gDevID );
	if(status != SP_SUCCESS) return FALSE;

	for(i = 0; i < 5; i++){
	//	memset(response, 0, SPRO_MAX_QUERY_SIZE+1);
		status = RNBOsproQuery( ApiPacket, 8,
								  (RBP_VOID)query,
								  (RBP_VOID)response,
								  (RBP_DWORD)&response32,
								  4);
		if(status != SP_SUCCESS) return FALSE;
		if(!memcmp((void*)response, (void*)answer, 4))	return TRUE;
	}

	return FALSE;
}	

BOOL IsMonacoSCAN(void)
{
	RB_SPRO_APIPACKET      ApiPacket;
	SP_STATUS status;
	RB_WORD gDevID =0x4779;
	RB_BYTE query[] = {0xD4, 0x6A, 0x6B, 0x02};
	RB_BYTE	response[SPRO_MAX_QUERY_SIZE+1];
	RB_BYTE	answer[] = {0x83, 0x32, 0x87, 0x21};
	RB_DWORD	response32 = 0;
	int i;


	status = RNBOsproFormatPacket( ApiPacket, sizeof(ApiPacket) );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproInitialize( ApiPacket );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproFindFirstUnit(ApiPacket,gDevID );
	if(status != SP_SUCCESS) return FALSE;

	for(i = 0; i < 5; i++){
	//	memset(response, 0, SPRO_MAX_QUERY_SIZE+1);
		status = RNBOsproQuery( ApiPacket, 8,
								  (RBP_VOID)query,
								  (RBP_VOID)response,
								  (RBP_DWORD)&response32,
								  4);
		if(status != SP_SUCCESS) return FALSE;
		if(!memcmp((void*)response, (void*)answer, 4))	return TRUE;
	}

	return FALSE;
}	

BOOL IsMonacoVIEW(void)
{
	RB_SPRO_APIPACKET      ApiPacket;
	SP_STATUS status;
	RB_WORD gDevID =0x4779;
	RB_BYTE query[] = {0xD4, 0x6A, 0x6B, 0x02};
	RB_BYTE	response[SPRO_MAX_QUERY_SIZE+1];
	RB_BYTE	answer[] = {0x83, 0x32, 0x87, 0x21};
	RB_DWORD	response32 = 0;
	int i;


	status = RNBOsproFormatPacket( ApiPacket, sizeof(ApiPacket) );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproInitialize( ApiPacket );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproFindFirstUnit(ApiPacket,gDevID );
	if(status != SP_SUCCESS) return FALSE;

	for(i = 0; i < 5; i++){
	//	memset(response, 0, SPRO_MAX_QUERY_SIZE+1);
		status = RNBOsproQuery( ApiPacket, 8,
								  (RBP_VOID)query,
								  (RBP_VOID)response,
								  (RBP_DWORD)&response32,
								  4);
		if(status != SP_SUCCESS) return FALSE;
		if(!memcmp((void*)response, (void*)answer, 4))	return TRUE;
	}

	return FALSE;
}	

BOOL ActivateColorGenie(void)
{
	RB_SPRO_APIPACKET      ApiPacket;
	SP_STATUS status;
	RB_WORD devID =0x4779;
	RB_WORD	writePW = 0x18E2;
	RB_WORD activeatePW1 = 0x1111;
	RB_WORD activeatePW2 = 0x1111;
	RB_WORD activateCell = 8;

	status = RNBOsproFormatPacket( ApiPacket, sizeof(ApiPacket) );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproInitialize( ApiPacket );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproFindFirstUnit(ApiPacket,devID );
	if(status != SP_SUCCESS) return FALSE;

	status = RNBOsproActivate( ApiPacket, writePW, activeatePW1, activeatePW2, activateCell );
	if(status != SP_SUCCESS) return FALSE;

	return TRUE;
}