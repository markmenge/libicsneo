/*
This program continuously tests Intrepid ValueCan3, NeoVI Fire and NeoVI Fire2 devices.
It assumes loopback pairs are installed as such:

ValueCan3, Fire, Fire2
NETID_HSCAN  - NETID_MSCAN
NETID_HSCAN2 - NETID_HSCAN3
NETID_HSCAN4 - NETID_HSCAN5
NETID_HSCAN6 - NETID_HSCAN7

ValueCan4
NETID_HSCAN  - NETID_HSCAN2

The tester first transmits CAN messages on each bus as such:
CAN ID = 0x100 + NETID
DATA = 0,1,2,3,4,5,6,7,8
If the device supports FD:
DATA = 0,1,2,3,4,5,6,7,8,9..63 (for FD)

Next the tester verifies that all the messages have been properly received
*/

#include "stdafx.h"
#include "icsneo40DLLAPI.h"
#include "VCExample.h"
#include <Windows.h>
#include <string>
#include <stdlib.h>
#include <conio.h>
using namespace std;

char getCharInput(int numArgs, ...);

// 04/22/2016 - menge add loopback tests and ProductionLoopbackTest
//                    
#define SLEEP_MS 1
#define TIMEOUT_MS 1000
#define MAX_NUM_NETWORKS 8
#define NUMFDBYTES 64

//int NetWorkIds[MAX_NUM_NETWORKS]               = {NETID_HSCAN, NETID_MSCAN, NETID_HSCAN2, NETID_HSCAN3, NETID_HSCAN4, NETID_HSCAN5, NETID_HSCAN6, NETID_HSCAN7};
//int NetWorkIdLoopbackPartner[MAX_NUM_NETWORKS] = {NETID_MSCAN, NETID_HSCAN, NETID_HSCAN3, NETID_HSCAN2, NETID_HSCAN5, NETID_HSCAN4, NETID_HSCAN7, NETID_HSCAN6};

int NetWorkIds[MAX_NUM_NETWORKS]               = {NETID_HSCAN, NETID_HSCAN2, NETID_HSCAN2, NETID_HSCAN3, NETID_HSCAN4, NETID_HSCAN5, NETID_HSCAN6, NETID_HSCAN7};
int NetWorkIdLoopbackPartner[MAX_NUM_NETWORKS] = {NETID_HSCAN2, NETID_HSCAN, NETID_HSCAN3, NETID_HSCAN2, NETID_HSCAN5, NETID_HSCAN4, NETID_HSCAN7, NETID_HSCAN6};


size_t m_hObject; //handle for device
bool m_bPortOpen; //tells the port status of the device
int g_iNumCanNetworks; // number of actual CAN networks
int g_bSupportsFd; // CAN busses support FD

HINSTANCE hDLL;
void Loopbacktest(BOOL bUseFd);
void LoopbacktestProduction();   // suggested loopbacktest for production
int LoopbackNetworkID(int NetworkId);  // For any given network ID, return other network ID of the loopback pair
void SetAllCANBaudRates(int iBitRateToUse);
void ConnectDisconnectTest(int device);

//----------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	char cInputCommand;
	bool bKeepRunning = 1;
	int iVerNumber;

	//Load DLL functions
	if(!LoadDLLAPI(hDLL)) 
	{	
		//problem, close the application
		cInputCommand = _getch();
		return(1);
	}

	system("Color A"); //Change window color

	//Get the DLL Version number


	while (bKeepRunning)
	{
		//Make Main Menu
		//printf("\r\n");
		printf("You are %s\n\n", m_bPortOpen ? "Connected" : "Disconnected");
		printf("Press the letter next to the function you want to use:\n");
		printf("A - List Attached Devices\n");
		printf("1 - Connect to First Device\n");
		printf("2 - Connect to Second Device\n");
		printf("C - Get Messages\n");
		printf("D - Send CANFD Message\n");
		printf("E - Send Extended Message\n");
		printf("F - Get Errors\n");
		printf("G - Set HS CAN to 250K\n");
		printf("H - Set HS CAN to 500K\n");

		printf("I - Disconnect from device\n");
		printf("4 - Set CAN buses to 250000 baud rate\n");
		printf("5 - Set CAN buses to 500000 baud rate\n");
		printf("6 - Set CAN buses to 666667 baud rate\n");
		printf("L - Loopback test with CAN FD (press any key to stop)\n");
		printf("M - Loopback test with normal CAN (press any key to stop)\n");
		printf("P - Loopback production test (4 hour PASS/FAIL) (press any key to stop)\n");
		printf("Q - Connect/disconnect on first device\n");
		printf("R - Connect/disconnect on second device\n");
		printf("X - Exit\n");
		printf("\r\n\r\n");
		//Get User intput
		cInputCommand = toupper(_getch());

		//Find command chosen
		switch(cInputCommand) 
		{
		case 'A': //A - List Attached Devices
			ListDevices();
			break;
		case '1': //B - Connect to Device
			ConnectToDevice(0);
			break;
		case '2': //B - Connect to Device
			ConnectToDevice(1);
			break;
		case 'C': //C - Get Messages
			GetMessagesFromDevice();
			break;
		case 'D': //D - Send CANFD Message
			SendMessageFromDevice(false, true, true);
			break;
		case 'E': // Send CAN29 Message
			SendMessageFromDevice(false, false, true);
			break;
		case 'F': // Get errors
			GetErrorsFromDevice();
			break;
		case 'G': // Set HS CAN to 250K
			SetHSCANBaudRatetoDevice(250000);
			break;
		case 'H': // Set HS CAN to 500K
			SetHSCANBaudRatetoDevice(500000);
			break;
		case '4': //4 - Set CAN to 250000
			SetAllCANBaudRates(250000);
			break;
		case '6': //G - Set CAN to 666667
			SetAllCANBaudRates(666667);
			break;
		case '5': //G - Set CAN to 500000
			SetAllCANBaudRates(500000);
			break;
		case 'I':  //H - Disconnect from device
			DisconnectFromDevice();
			break;
		case 'L':  //L - Loopbacktest
			Loopbacktest(TRUE);
			break;
		case 'M':  //M - Loopbacktest
			Loopbacktest(FALSE);
			break;
		case 'P':  //P - Loopbacktest
			LoopbacktestProduction();
			break;
		case 'Q':  //
			ConnectDisconnectTest(0);
			break;
		case 'R':  //
			ConnectDisconnectTest(1);
			break;
		case 'X': //X - Exit
			if(m_bPortOpen==true) DisconnectFromDevice();
			UnloadDLLAPI(hDLL);
			bKeepRunning = 0;
			break;
		default : 
			// Process for all other cases.
			break;
		}
		printf("Press Key to Continue");
		cInputCommand = _getch();
		system("cls");
	}
}

static icsSpyMessage stMessages[20000];
//----------------------------------------------------------------------------
void GetMessagesFromDevice(BOOL bQuiet)
{
	long lResult;
	int iNumberOfErrors;
	long lCount;
	int lNumberOfMessages;
	double dTime;
	long lByteCount;
	icsSpyMessageJ1850 icsJ1850Msg;
	const unsigned char *ptr;
	int BRS = 0;

	//check to see if device is open, if so exit
	if (m_bPortOpen==false)
	{
		printf("neoVI not opened\r\n");
		return;  
	}

	//Call Get Messages
	lResult = icsneoGetMessages(m_hObject, stMessages, &lNumberOfMessages, &iNumberOfErrors); //Call get message function
	if(lResult==0)//See if Call was successful
	{
		printf("Problem Getting Messages\r\n");
		return;
	}

	//if there are no messages get out of function
	if(lNumberOfMessages==0) return;

	//Loop to acquire all of the messages in the buffer
	for(lCount=0;lCount<=lNumberOfMessages;lCount++)   
	{

		lByteCount = stMessages[lCount - 1].NumberBytesData;

		lResult = icsneoGetTimeStampForMsg(m_hObject, &stMessages[lCount - 1], &dTime);

		if (bQuiet)
			continue;

		printf("%f ",dTime);

		//check if the message is Tx or RX
		if((stMessages[lCount - 1].StatusBitField & SPY_STATUS_TX_MSG)!=0) 
		{
			printf("Tx ");
		}
		else
		{
			printf("Rx ");
		}

		//Get the network of the message
		switch(stMessages[lCount - 1].NetworkID)
		{
		case NETID_HSCAN:
			printf("HSCAN ");
			break;
		case NETID_MSCAN:
			printf("MSCAN ");
			break;
		case NETID_SWCAN:
			printf("SWCAN ");
			break;
		case NETID_LSFTCAN:
			printf("LSFTCAN ");
			break;
		case NETID_FORDSCP:
			printf("FORD SCP ");
			break;
		case NETID_J1708:
			printf("J1708 ");
			break;
		case NETID_JVPW:
			printf("J1850 VPW ");
			break;
		case NETID_ISO:
			printf("ISO/UART ");
			break;
		default:
			printf("Unknown ");
		}

		//Decode the databytes
		switch (stMessages[lCount-1].Protocol)
		{
		case SPY_PROTOCOL_CAN:
			printf("ArbID-%08X ",stMessages[lCount - 1].ArbIDOrHeader);
			printf("RTR:%d ", (stMessages[lCount-1].StatusBitField & SPY_STATUS_REMOTE_FRAME)!=0);
			if(lByteCount>=1) printf("Data:%02X",stMessages[lCount - 1].Data[0]);
			if(lByteCount>=2) printf(" %02X",stMessages[lCount - 1].Data[1]);
			if(lByteCount>=3) printf(" %02X",stMessages[lCount - 1].Data[2]);
			if(lByteCount>=4) printf(" %02X",stMessages[lCount - 1].Data[3]);
			if(lByteCount>=5) printf(" %02X",stMessages[lCount - 1].Data[4]);
			if(lByteCount>=6) printf(" %02X",stMessages[lCount - 1].Data[5]);
			if(lByteCount>=7) printf(" %02X",stMessages[lCount - 1].Data[6]);
			if(lByteCount>=8) printf(" %02X",stMessages[lCount - 1].Data[7]);
			break;
		case SPY_PROTOCOL_CANFD:
			//Use Extra data ptr to receive all data bytes	
			if(stMessages[lCount-1].NumberBytesData > 8)
				ptr = (const unsigned char*) stMessages[lCount-1].pExtraDataPtr;

			if(stMessages[lCount-1].StatusBitField3 & 16) BRS = 1;

			if(stMessages[lCount-1].StatusBitField & SPY_STATUS_TX_MSG)			
				printf("CANFD TX: [Network ID: %0x] BRS[%d] [Datalen:%d] RTR:%d\t",stMessages[lCount-1].NetworkID,BRS,stMessages[lCount-1].NumberBytesData, (stMessages[lCount-1].StatusBitField & SPY_STATUS_REMOTE_FRAME)!=0);
			else
				printf("CANFD RX: [Network ID: %0x] BRS[%d] [Datalen:%d] RTR:%d\t",stMessages[lCount-1].NetworkID,BRS,stMessages[lCount-1].NumberBytesData, (stMessages[lCount-1].StatusBitField & SPY_STATUS_REMOTE_FRAME)!=0);

			if(stMessages[lCount-1].NumberBytesData > 8)
			{
				for(int j=0;j< stMessages[lCount-1].NumberBytesData;j++)
				{
					printf("%0x\t",*(ptr++));
				}
				printf("\n");
			}
			else
			{
				if(lByteCount>=1) printf("Data:%02X",stMessages[lCount - 1].Data[0]);
				if(lByteCount>=2) printf(" %02X",stMessages[lCount - 1].Data[1]);
				if(lByteCount>=3) printf(" %02X",stMessages[lCount - 1].Data[2]);
				if(lByteCount>=4) printf(" %02X",stMessages[lCount - 1].Data[3]);
				if(lByteCount>=5) printf(" %02X",stMessages[lCount - 1].Data[4]);
				if(lByteCount>=6) printf(" %02X",stMessages[lCount - 1].Data[5]);
				if(lByteCount>=7) printf(" %02X",stMessages[lCount - 1].Data[6]);
				if(lByteCount>=8) printf(" %02X",stMessages[lCount - 1].Data[7]);
				printf("\n");
			}

			break;
		default:
			memcpy((void*)&icsJ1850Msg,(void*)&stMessages[lCount-1],sizeof(icsSpyMessageJ1850));			
			printf("Header-%d %d %d ",icsJ1850Msg.Header[0],icsJ1850Msg.Header[1],icsJ1850Msg.Header[2]);
			if(lByteCount>=1) printf("D %d ",icsJ1850Msg.Data[0]);
			if(lByteCount>=2) printf(" %d ",icsJ1850Msg.Data[1]);
			if(lByteCount>=3) printf(" %d ",icsJ1850Msg.Data[2]);
			if(lByteCount>=4) printf(" %d ",icsJ1850Msg.Data[3]);
			if(lByteCount>=5) printf(" %d ",icsJ1850Msg.Data[4]);
			if(lByteCount>=6) printf(" %d ",icsJ1850Msg.Data[5]);
			if(lByteCount>=7) printf(" %d ",icsJ1850Msg.Data[6]);
			if(lByteCount>=8) printf(" %d ",icsJ1850Msg.Data[7]);
			break;
		}

		//Display the number of messages and errors
		//printf("Number Read : %d\r\n",lNumberOfMessages);
		//printf("Number Errors : %d\r\n",iNumberOfErrors);
		printf("\r\n");
	}
}

//----------------------------------------------------------------------------
void SendMessageFromDevice(BOOL bQuiet, BOOL bFd, BOOL b29)
{

	long lResult; 
	icsSpyMessage stMessagesTx;
//	long lNetworkID;
	int iNumberSent = 0;

	unsigned char CanFDptr[NUMFDBYTES];
	for(int i=0;i<NUMFDBYTES;i++)
		CanFDptr[i] = i;

	memset(&stMessagesTx, 0, sizeof(stMessagesTx));
	//make sure the device is open
	if (m_bPortOpen==false)
	{
		printf("neoVI not opened\r\n");
		return;  
	}

	for(int i=0;i<8;i++)
		stMessagesTx.Data[i] = i;

	//Send on HS CAN
	stMessagesTx.NetworkID = NETID_HSCAN;

	//Set the ID
	stMessagesTx.ArbIDOrHeader = 0x123;    

	if (bFd)
	{
		// The number of Data Bytes
		stMessagesTx.NumberBytesData = 64;
		stMessagesTx.NumberBytesHeader = 0;

		//Enable Extra Data Pointer
		stMessagesTx.ExtraDataPtrEnabled = 1;
		stMessagesTx.pExtraDataPtr = &CanFDptr;
		stMessagesTx.Protocol = SPY_PROTOCOL_CANFD;
		stMessagesTx.StatusBitField2 = 0;
		stMessagesTx.StatusBitField3 = 16; //Enable bitrate switch
		stMessagesTx.StatusBitField4 = 0;
	}
	else
	{
		// The number of Data Bytes
		stMessagesTx.NumberBytesData = 8;
		stMessagesTx.NumberBytesHeader = 0;

		//Enable Extra Data Pointer
		stMessagesTx.ExtraDataPtrEnabled = 0;
		stMessagesTx.pExtraDataPtr = FALSE;
		stMessagesTx.Protocol = SPY_PROTOCOL_CAN;
		stMessagesTx.StatusBitField2 = 0;
		stMessagesTx.StatusBitField3 = 0; //Enable bitrate switch
		stMessagesTx.StatusBitField4 = 0;
	}
	if (b29)
		stMessagesTx.StatusBitField |= SPY_STATUS_XTD_FRAME;

	// Transmit the assembled message
	lResult = icsneoTxMessagesEx(m_hObject, &stMessagesTx, stMessagesTx.NetworkID, 1, &iNumberSent, 0);
	//lResult = icsneoTxMessagesEx(m_hObject, &TxMsgCANFDEth[0], TxMsgCANFDEth[0].NetworkID, NUMMSGTOTRANSMIT,&Numtxed,0)) || Numtxed != NUMMSGTOTRANSMIT )
	if (!bQuiet)
		printf("Message Sent!\r\n");
}

void LoopbacktestTxRx(int *pNumMessages, int *pNumLost, int *pNumErrors, BOOL bUseFd)
{

	long lResult; 
	icsSpyMessage stMessagesTx;
	int iNumberSent = 0;

	unsigned char CanFDptr[NUMFDBYTES];
	int i, j, iMsg;

	memset(&stMessagesTx, 0, sizeof(stMessagesTx));


	//make sure the device is open
	if (m_bPortOpen==false)
	{
		printf("neoVI not opened\r\n");
		return;  
	}

	// The number of Data Bytes
	if (bUseFd)
	{
		stMessagesTx.NumberBytesData = 64;
		//Enable Extra Data Pointer
		stMessagesTx.ExtraDataPtrEnabled = 1;
		stMessagesTx.pExtraDataPtr = &CanFDptr[0];	
		stMessagesTx.Protocol = SPY_PROTOCOL_CANFD;
		stMessagesTx.StatusBitField2 = 0;
		stMessagesTx.StatusBitField3 = 16; //Enable bitrate switch
		stMessagesTx.StatusBitField4 = 0;
		for (i=0; i<NUMFDBYTES; i++)
			CanFDptr[i] = i;	
	}
	else
	{
		stMessagesTx.NumberBytesData = 8;
		stMessagesTx.Protocol = SPY_PROTOCOL_CAN;
		for (i = 0; i < 8; i++)
			stMessagesTx.Data[i] = i;
	}

	// Transmit the assembled message on each network
	for (i = 0; i < g_iNumCanNetworks; i++)
	{
		//Set the ID.  0x100 + Network ID 
		stMessagesTx.ArbIDOrHeader = 0x100 + NetWorkIds[i];
		stMessagesTx.NetworkID = NetWorkIds[i];
		if (bUseFd)
			lResult = icsneoTxMessagesEx(m_hObject, &stMessagesTx, NetWorkIds[i], 1, &iNumberSent, 0);
		else
			lResult = icsneoTxMessages(m_hObject, &stMessagesTx, NetWorkIds[i], 1);
	}
	int iNumberOfErrors;
	int lNumberOfMessages;
	*pNumMessages = 0;
	*pNumErrors = 0;
	*pNumLost = 0;
	int RxOk = 0;
	int timems = 0;
	while (1)
	{

		Sleep(SLEEP_MS);
		timems += SLEEP_MS;
		if (timems > TIMEOUT_MS)
		{
			printf("Failed to get %d messages in %d ms\n!", g_iNumCanNetworks * 2, TIMEOUT_MS);
			*pNumErrors += 1;
			*pNumLost = g_iNumCanNetworks * 2 - RxOk;
			MessageBeep(0);
			return;
		}
		lNumberOfMessages = 10000;
		lResult = icsneoGetMessages(m_hObject, stMessages, &lNumberOfMessages, &iNumberOfErrors); //Call get message function
		*pNumMessages += lNumberOfMessages;
		//*pNumErrors += iNumberOfErrors;  <<< why these errors?

		// Check the messages are correct
		for (iMsg = 0; iMsg < lNumberOfMessages; iMsg++)
		{
			if (NETID_ETHERNET == stMessages[iMsg].NetworkID || 52 == stMessages[iMsg].NetworkID || 0 == stMessages[iMsg].NetworkID)
				continue;
			if (0 == LoopbackNetworkID(stMessages[iMsg].NetworkID))
			{
				printf("Unexpected Network ID:%d! It contained CAN ID:0x%x data length:%d", stMessages[iMsg].NetworkID, stMessages[iMsg].ArbIDOrHeader, stMessages[iMsg].NumberBytesData);
				*pNumErrors += 1;
				for (i = 0; i < stMessages[iMsg].NumberBytesData; i++)
					printf(" %02X", stMessages[iMsg].Data[i]);
				printf("\n");
				MessageBeep(0);
				continue;
			}

			//printf("[%s:%d]", (stMessages[iMsg].StatusBitField  & SPY_STATUS_TX_MSG) ? "Tx" : "Rx", stMessages[iMsg].NetworkID);
			// Tx check
			if (stMessages[iMsg].StatusBitField  & SPY_STATUS_TX_MSG)
			{

				if (stMessages[iMsg].ArbIDOrHeader != 0x100 + stMessages[iMsg].NetworkID)
				{
					printf("Tx Can ID:0x%x (data length:%d, Network ID:%d) != expected 0x%x \n", stMessages[iMsg].ArbIDOrHeader, stMessages[iMsg].NumberBytesData, stMessages[iMsg].NetworkID, 0x100 + stMessages[iMsg].NetworkID);
					*pNumErrors += 1;
					MessageBeep(0);
					continue;
				}
			}
			else
			{   // Rx check
				if (stMessages[iMsg].ArbIDOrHeader != 0x100 + LoopbackNetworkID(stMessages[iMsg].NetworkID))
				{
					printf("Rx Can ID:0x%x (data length:%d, Network ID:%d) != expected 0x%x \n", stMessages[iMsg].ArbIDOrHeader, stMessages[iMsg].NumberBytesData, stMessages[iMsg].NetworkID, 0x100 + LoopbackNetworkID(stMessages[iMsg].NetworkID));
					*pNumErrors += 1;
					MessageBeep(0);
					continue;
				}
			}
			if (bUseFd)
			{
				if (stMessages[iMsg].Protocol != SPY_PROTOCOL_CANFD)
				{
					printf("Protocol != SPY_PROTOCOL_CANFD received on Network:%d\n!", stMessages[iMsg].NetworkID);
					*pNumErrors += 1;
					MessageBeep(0);
					continue;
				}
				if (stMessages[iMsg].NumberBytesData != NUMFDBYTES)
				{
					printf("NumberBytesData != %d\n!", NUMFDBYTES);
					*pNumErrors += 1;
					MessageBeep(0);
					continue;
				}
				if (stMessages[iMsg].ExtraDataPtrEnabled == 0)
				{
					printf("ExtraDataPtrEnabled == 0!\n");
					*pNumErrors += 1;
					MessageBeep(0);
					continue;
				}
				for (j = 0; j < NUMFDBYTES; j++)
				{
					if (((UINT8 *)stMessages[iMsg].pExtraDataPtr)[j] != j)
					{
						printf("Corrupted Data byte #:%d, %d != %d!\n", j, j, ((UINT8 *)stMessages[iMsg].pExtraDataPtr)[j]);
						*pNumErrors += 1;
						MessageBeep(0);
						break;
					}
				}
				if (j < NUMFDBYTES)
					continue;
			}
			else
			{
				for (j = 0; j < 8; j++)
				{
					if (stMessages[iMsg].Data[j] != j)
					{
						printf("Corrupted Data byte #:%d, %d != %d!\n", j, j, stMessages[iMsg].Data[j]);
						*pNumErrors += 1;
						MessageBeep(0);
						break;
					}
				}
				if (j < 8)
					continue;

			}
			RxOk++; // Message is OK

		} // for (i = 0; i < lNumberOfMessages; i++)
		if (*pNumMessages >= g_iNumCanNetworks * 2)			// We have received all the transmitted and received messages
			return;
	} // while(1)
}


void Loopbacktest(BOOL bUseFd)
{
	int NumMessages = 0, NumLost = 0, NumErrors = 0;
	int TotalNumMessages = 0, TotalNumLost = 0, TotalNumErrors = 0;
	//make sure the device is open
	if (m_bPortOpen==false)
	{
		printf("neoVI not opened\r\n");
		return;  
	}

	if (bUseFd)
		printf("Loopback test for CAN FD\n");
	else
		printf("Loopback test for normal CAN\n");

	SetAllCANBaudRates(500000);
	while (!_kbhit())
	{
		LoopbacktestTxRx(&NumMessages, &NumLost, &NumErrors, bUseFd);
		TotalNumMessages += NumMessages;
		TotalNumLost += NumLost;
		TotalNumErrors += NumErrors;
		printf("\rMessages:%d lost:%d errors:%d  ", TotalNumMessages, TotalNumLost, TotalNumErrors);
	}
}
#define PRODUCTION_TEST_TIME_SEC (4 * 3600) // 4 hours
//#define PRODUCTION_TEST_TIME_SEC (10) // 4 hours

void LoopbacktestProduction()
{
	int NumMessages = 0, NumLost = 0, NumErrors = 0;
	int TotalNumMessages = 0, TotalNumLost = 0, TotalNumErrors = 0;

	UINT32 StartTimeMs = GetTickCount();

	//make sure the device is open
	if (m_bPortOpen==false)
	{
		ConnectToDevice(0);
		if (m_bPortOpen==false)
		{
			printf("neoVI not opened\r\n");
			return;
		}
	}
	SetAllCANBaudRates(500000);
	while (!_kbhit())
	{
		LoopbacktestTxRx(&NumMessages, &NumLost, &NumErrors, false);
		TotalNumMessages += NumMessages;
		TotalNumLost += NumLost;
		TotalNumErrors += NumErrors;
		printf("\rMessages:%d lost:%d errors:%d  ", TotalNumMessages, TotalNumLost, TotalNumErrors);

		if (g_bSupportsFd)
		{
			LoopbacktestTxRx(&NumMessages, &NumLost, &NumErrors, true);
			TotalNumMessages += NumMessages;
			TotalNumLost += NumLost;
			TotalNumErrors += NumErrors;
			printf("\rMessages:%d lost:%d errors:%d  ", TotalNumMessages, TotalNumLost, TotalNumErrors);
		}
		if (PRODUCTION_TEST_TIME_SEC < labs(StartTimeMs - GetTickCount())/1000L)
		{
			if (NumErrors == 0)
				printf("\nProduct test: PASS\n");
			else
				printf("\nProduct test: FAIL\n");
			return;
		}
	}
}

//----------------------------------------------------------------------------
void GetErrorsFromDevice(void)
{
	int lResult=0;
	int iErrors[600]={0};
	int lNumberOfErrors=0;
	int lCount;
	char szDescriptionShort[255]={char(0)};
	char szDescriptionLong[255]={char(0)};
	int lMaxLengthShort,lMaxLengthLong,lErrorSeverity,lRestartNeeded;


	// Read the errors from the DLL
	lResult = icsneoGetErrorMessages(m_hObject,iErrors,&lNumberOfErrors);

	if (lResult == 0)
	{
		printf("Problem Reading errors\r\n");
	}

	// dump the neoVI errors
	if (lNumberOfErrors > 0)
	{    
		for (lCount=0;lCount <lNumberOfErrors;lCount++)
		{
			lMaxLengthShort = 255;
			lMaxLengthLong = 255;
			icsneoGetErrorInfo(iErrors[lCount], szDescriptionShort, szDescriptionLong, &lMaxLengthShort,&lMaxLengthLong, &lErrorSeverity, &lRestartNeeded);
			printf("Error - %s\r\n",szDescriptionShort);
		}
	}
	else
	{
		printf("No Errors to report\r\n");			
	}

}
//----------------------------------------------------------------------------
void SetHSCANBaudRatetoDevice(int iRateToUse)
{
	int iBitRateToUse;
	int iNetworkID;
	int iResult; 

	//Make sure the device is open
	if (m_bPortOpen==false)
	{
		printf("neoVI not opened\r\n");
		return;   
	}

	//Get the network index to set the baud rate to use
	iNetworkID = NETID_HSCAN;
	iBitRateToUse = 250000;

	//Set the bit rate
	iResult = icsneoSetBitRate(m_hObject, iBitRateToUse, iNetworkID);
	if (iResult == 0)  
	{
		printf("Problem setting bit rate\r\n");
	}
	else
	{
		printf("Baudrate Set\r\n");
	}

}
void SetAllCANBaudRates(int iBitRateToUse)
{
	int i, iResult;
	//Make sure the device is open
	if (m_bPortOpen==false)
	{
		printf("neoVI not opened\r\n");
		return;   
	}
	for (i = 0; i < g_iNumCanNetworks; i++)
	{

		icsneoEnableNetworkComEx(m_hObject, NetWorkIds[i], false);
		Sleep(100);
		//Set the bit rate
		iResult = icsneoSetBitRate(m_hObject, iBitRateToUse, NetWorkIds[i]);
		icsneoEnableNetworkComEx(m_hObject, NetWorkIds[i], true);
		Sleep(100);
		if (iResult == 0)  
		{
			printf("Problem setting bit rate for network ID:%d \r\n", NetWorkIds[i]);
		}
		else
		{
			//  printf("Baudrate Set\r\n");
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void DisconnectFromDevice(void)
{
	int iResult;
	int iNumberOfErrors;

	//Call close port
	iResult = icsneoClosePort(m_hObject,&iNumberOfErrors);
	if(iResult==0)
	{
		printf("Problem Closing Port\r\n");
	}
	else
	{
		printf("Port Closed\r\n");
	}
	//clear open flag
	m_bPortOpen = false;
}
//----------------------------------------------------------------------------
//#define NEODEVICE_VCAN4 0x00400000
//#define NEODEVICE_FIRE2 0x04000000

void ConnectToDevice(int device /* starts at 0 */)
{
	int iResult; 
	NeoDeviceEx ndNeoToOpen[10];
	int iNumberOfDevices;
	string sTempString;
	char SerialNumber[80];

	//Make sure the device is NOT open
	if (m_bPortOpen==true)
	{
		printf("Device already opened\r\n");
		return;   
	}

	//search for the first device
	iNumberOfDevices = 10;
	iResult = icsneoFindDevices(ndNeoToOpen, &iNumberOfDevices, NULL, 0, NULL, 0);
	if(iResult == false)
	{
		printf("Problem Finding Device\r\n");
		return;
	}

	//Check for the number of devices2
	if (iNumberOfDevices < device + 1)
	{
		printf("No Device %d Found\r\n", device + 1);
		return;
	}
	m_hObject = NULL;
	m_hObject = 0;
	//Connect to the first device found
	iResult = icsneoOpenNeoDevice(&ndNeoToOpen[device].neoDevice, &m_hObject, NULL,1,0);
	if(iResult==false)
	{
		printf("Problem Opening Port\r\n");
		return;
	}
	printf("Port Opened OK!\r\n");
	g_bSupportsFd = false;

	//Display the type of device
	switch(ndNeoToOpen[device].neoDevice.DeviceType)
	{
	case 1:
		sTempString = "neoVI Blue SN ";
		break;
	case 4:
		sTempString= "ValueCAN 2 SN ";
		break;
	case NEODEVICE_FIRE:
		sTempString= "neoVI FIRE SN ";
		g_iNumCanNetworks = 4;
		break;
	case NEODEVICE_VCAN3:
		sTempString= "ValueCAN 3 SN ";
		g_iNumCanNetworks = 2;
		break;
	case NEODEVICE_VCAN42:
		sTempString= "ValueCAN 4-2 SN ";
		g_iNumCanNetworks = 2;
		break;
	case NEODEVICE_VCAN41:
		sTempString= "ValueCAN 4-1 SN ";
		g_iNumCanNetworks = 1;
		break;
	case NEODEVICE_FIRE2:
		sTempString= "NEODEVICE_FIRE2";
		g_iNumCanNetworks = 8;
		g_bSupportsFd = true;
		break;
	case NEODEVICE_RADGALAXY:
		sTempString= "RAD GALAXY";
		g_iNumCanNetworks = 8;
		g_bSupportsFd = true;
		break;
	case NEODEVICE_VCAN4_IND:
		sTempString = "ValueCAN4 Industrial";
		g_iNumCanNetworks = 2;
		g_bSupportsFd = true;
		break;
	default:
		sTempString = "Unknown neoVI SN ";
		break;
	}
	if (ndNeoToOpen[device].neoDevice.DeviceType == NEODEVICE_VCAN42 || ndNeoToOpen[device].neoDevice.DeviceType == NEODEVICE_VCAN4_IND)
	{
		NetWorkIds[1] = NETID_HSCAN2;
		NetWorkIdLoopbackPartner[0] = NETID_HSCAN2;
	}
	else
	{
		NetWorkIds[1] = NETID_MSCAN;
		NetWorkIdLoopbackPartner[0] = NETID_MSCAN;

	}
	if (ndNeoToOpen[device].neoDevice.DeviceType >= NEODEVICE_VCAN42 || ndNeoToOpen[device].neoDevice.DeviceType == NEODEVICE_VCAN4_IND || ndNeoToOpen[device].neoDevice.DeviceType == NEODEVICE_VCAN41)
		itoa(ndNeoToOpen[device].neoDevice.SerialNumber, SerialNumber, 36);
	else
		itoa(ndNeoToOpen[device].neoDevice.SerialNumber, SerialNumber, 10);


	printf("DeviceType:%0X =%s %s is connected\r\n", ndNeoToOpen[device].neoDevice.DeviceType, sTempString.c_str(), SerialNumber);

	m_bPortOpen = true;
}
//----------------------------------------------------------------------------
void ListDevices(void)
{
	int iResult; 
	NeoDeviceEx ndNeoToOpen [10];
	int iNumberOfDevices;
	int iCounter;
	string sTempString;


	iNumberOfDevices = 10;
	//Search for attached devices
	iResult = icsneoFindDevices(ndNeoToOpen, &iNumberOfDevices, NULL, NULL, NULL, NULL);
	if(iResult == false)
	{
		printf("Problem Finding Device\r\n");
		return;
	}

	if(iNumberOfDevices<1)
	{
		printf("No Devices Found\r\n");
		return;
	}

	//Print list of devices
	for (iCounter = 0;iCounter < iNumberOfDevices;iCounter++)
	{
		switch(ndNeoToOpen[iCounter].neoDevice.DeviceType)
		{
		case 1:
			sTempString = "neoVI Blue SN ";
			break;
		case 4:
			sTempString= "ValueCAN 2 SN ";
			break;
		case 8:
			sTempString= "neoVI FIRE SN ";
			break;
		case 16:
			sTempString= "ValueCAN 3 SN ";
			break;
		case NEODEVICE_VCAN42:
			sTempString= "ValueCAN 4-1 SN ";
			break;
		case NEODEVICE_FIRE2:
			sTempString = "NEODEVICE_FIRE2";
			break;
		case NEODEVICE_VCAN4_IND:
			sTempString = "ValueCAN 4 Industrial SN ";
			break;
		case NEODEVICE_VCAN41:
			sTempString = "ValueCAN 4-1 Industrial SN ";
			break;
		default:

			sTempString = "Unknown neoVI SN ";
		}
		char SerialNumber[80];
		if (ndNeoToOpen[iCounter].neoDevice.DeviceType >= NEODEVICE_VCAN42 || ndNeoToOpen[iCounter].neoDevice.DeviceType == NEODEVICE_VCAN4_IND || ndNeoToOpen[iCounter].neoDevice.DeviceType == NEODEVICE_VCAN41)
			itoa(ndNeoToOpen[iCounter].neoDevice.SerialNumber, SerialNumber, 36);
		else
			itoa(ndNeoToOpen[iCounter].neoDevice.SerialNumber, SerialNumber, 10);
		printf("%s %s DeviceType:0x%X\r\n",sTempString.c_str(), SerialNumber, ndNeoToOpen[iCounter].neoDevice.DeviceType);
	}
}

//----------------------------------------------------------------------------
// For any given network ID, return other network ID of the loopback pair. Return 0 if unable to find partner
// Assumes these networks are looped back this way:
// NETID_HSCAN  - NETID_MSCAN
// NETID_HSCAN2 - NETID_HSCAN3
// NETID_HSCAN4 - NETID_HSCAN5
// NETID_HSCAN6 - NETID_HSCAN7
int LoopbackNetworkID(int NetworkId)
{
	int i;
	for (i = 0; i < g_iNumCanNetworks; i++)
	{
		if (NetworkId == NetWorkIds[i])
			return NetWorkIdLoopbackPartner[i];
	}
	return 0;
}

/* I started but gave up
const int NETID_LIN = 16;
const int NETID_ISO3 = 41;
const int NETID_HSCAN2 = 42;
const int NETID_HSCAN3 = 44;
const int NETID_ISO4 = 47;
const int NETID_LIN2 = 48;
const int NETID_LIN3 = 49;
const int NETID_LIN4 = 50;

#define NUM_LIN_BUS 4
void LoopbacktestProductionLIN()
{
	int NumMessages = 0, NumLost = 0, NumErrors = 0;
	int TotalNumMessages = 0, TotalNumLost = 0, TotalNumErrors = 0;
	int NetWorkIdsLIN[] = { NETID_LIN, NETID_LIN2, NETID_LIN3, NETID_LIN4 };
	int i;

	UINT32 StartTimeMs = GetTickCount();

	//make sure the device is open
	if (m_bPortOpen==false)
	{
		ConnectToDevice(0);
		if (m_bPortOpen==false)
		{
			printf("neoVI not opened\r\n");
			return;
		}
	}
	for (i = 0; i < NUM_LIN_BUS; i++)
		icsneoSetBitRate(m_hObject, 19200, NetWorkIdsLIN[i]);
	
	while (!_kbhit())
	{
		LoopbacktestTxRxLIN(&NumMessages, &NumLost, &NumErrors);
		TotalNumMessages += NumMessages;
		TotalNumLost += NumLost;
		TotalNumErrors += NumErrors;
		printf("\rMessages:%d lost:%d errors:%d  ", TotalNumMessages, TotalNumLost, TotalNumErrors);

		if (PRODUCTION_TEST_TIME_SEC < labs(StartTimeMs - GetTickCount())/1000L)
		{
			if (NumErrors == 0)
				printf("\nProduct test: PASS\n");
			else
				printf("\nProduct test: FAIL\n");
			return;
		}
	}
}

void LoopbacktestTxRxLIN(int *pNumMessages, int *pNumLost, int *pNumErrors)
{

	long lResult; 
	icsSpyMessage stMessagesTx;
	int iNumberSent = 0;
	int i, j, iMsg;

	memset(&stMessagesTx, 0, sizeof(stMessagesTx));


	//make sure the device is open
	if (m_bPortOpen==false)
	{
		printf("neoVI not opened\r\n");
		return;  
	}

		stMessagesTx.NumberBytesData = 8;
		stMessagesTx.Protocol = SPY_PROTOCOL_LIN;
		for (i = 0; i < 8; i++)
			stMessagesTx.Data[i] = i;

	// Transmit the assembled message on each network
	stMessagesTx.ArbIDOrHeader = NETID_LIN * 2;
	stMessagesTx.NetworkID = NETID_LIN;
	lResult = icsneoTxMessages(m_hObject, &stMessagesTx, NETID_LIN, 1);

	stMessagesTx.ArbIDOrHeader = NETID_LIN3 * 2;
	stMessagesTx.NetworkID = NETID_LIN3;
	lResult = icsneoTxMessages(m_hObject, &stMessagesTx, NETID_LIN, 1);

	int iNumberOfErrors;
	int lNumberOfMessages;
	*pNumMessages = 0;
	*pNumErrors = 0;
	*pNumLost = 0;
	int RxOk = 0;
	int timems = 0;

	#define TIMEOUT_LIN_MS 200
		Sleep(TIMEOUT_LIN_MS);
		timems += TIMEOUT_LIN_MS;

		//lNumberOfMessages = 10000;

		lResult = icsneoGetMessages(m_hObject, stMessages, &lNumberOfMessages, &iNumberOfErrors); //Call get message function
		*pNumMessages += lNumberOfMessages;
		*pNumErrors += iNumberOfErrors;

		if (lNumberOfMessages < 2)
		{
			printf("Failed to get %d messages in %d ms\n!", g_iNumCanNetworks * 2, TIMEOUT_LIN_MS);
			*pNumErrors += 1;
			*pNumLost = g_iNumCanNetworks * 2 - RxOk;
			MessageBeep(0);
			return;
		}

		// Check the messages are correct
		for (iMsg = 0; iMsg < lNumberOfMessages; iMsg++)
		{
			if (NETID_ETHERNET == stMessages[iMsg].NetworkID)
				continue;
			if (0 == LoopbackNetworkID(stMessages[iMsg].NetworkID))
			{
				printf("Unexpected Network ID:%d! It contained CAN ID:0x%x data length:%d", stMessages[iMsg].NetworkID, stMessages[iMsg].ArbIDOrHeader, stMessages[iMsg].NumberBytesData);
				*pNumErrors += 1;
				for (i = 0; i < stMessages[iMsg].NumberBytesData; i++)
					printf(" %02X", stMessages[iMsg].Data[i]);
				printf("\n");
				MessageBeep(0);
				continue;
			}

			//printf("[%s:%d]", (stMessages[iMsg].StatusBitField  & SPY_STATUS_TX_MSG) ? "Tx" : "Rx", stMessages[iMsg].NetworkID);
			// Tx check
			if (stMessages[iMsg].StatusBitField  & SPY_STATUS_TX_MSG)
			{

				if (stMessages[iMsg].ArbIDOrHeader != 0x100 + stMessages[iMsg].NetworkID)
				{
					printf("Tx Can ID:0x%x (data length:%d, Network ID:%d) != expected 0x%x \n", stMessages[iMsg].ArbIDOrHeader, stMessages[iMsg].NumberBytesData, stMessages[iMsg].NetworkID, 0x100 + stMessages[iMsg].NetworkID);
					*pNumErrors += 1;
					MessageBeep(0);
					continue;
				}
			}
			else
			{   // Rx check
				if (stMessages[iMsg].ArbIDOrHeader != 0x100 + LoopbackNetworkID(stMessages[iMsg].NetworkID))
				{
					printf("Rx Can ID:0x%x (data length:%d, Network ID:%d) != expected 0x%x \n", stMessages[iMsg].ArbIDOrHeader, stMessages[iMsg].NumberBytesData, stMessages[iMsg].NetworkID, 0x100 + LoopbackNetworkID(stMessages[iMsg].NetworkID));
					*pNumErrors += 1;
					MessageBeep(0);
					continue;
				}
			}
			
				for (j = 0; j < 8; j++)
				{
					if (stMessages[iMsg].Data[j] != j)
					{
						printf("Corrupted Data byte #:%d, %d != %d!\n", j, j, stMessages[iMsg].Data[j]);
						*pNumErrors += 1;
						MessageBeep(0);
						break;
					}
				}
				if (j < 8)
					continue;

			}
			RxOk++; // Message is OK

	} // while(1)
}
*/

void ConnectDisconnectTest(int device)
{
	int i;

	while (!kbhit())
	{
		ConnectToDevice(device);
		if (m_bPortOpen == FALSE)
		{
			MessageBox(GetConsoleWindow(), "Failed to connect", "LoopbackTest.exe", MB_ICONHAND);
			continue;
		}
		GetMessagesFromDevice(TRUE);
		for (i = 0; i < 1000; i++)
			SendMessageFromDevice(true);
		GetMessagesFromDevice(TRUE);
		DisconnectFromDevice();
		if (m_bPortOpen == TRUE)
		{
			MessageBox(GetConsoleWindow(), "Failed to disconnect", "LoopbackTest.exe", MB_ICONHAND);
			continue;
		}
	}
}


/**
* \brief Used to check character inputs for correctness (if they are found in an expected list)
* \param[in] numArgs the number of possible options for the expected character
* \param[in] ... the possible options for the expected character
* \returns the entered character
*
* This function repeatedly prompts the user for input until a matching input is entered
* Example usage: char input = getCharInput(5, 'F', 'u', 'b', 'a', 'r');
*/
char getCharInput(int numArgs, ...) 
{
	// 99 chars shold be more than enough to catch any typos
	char input[99];
	bool found = false;

	va_list vaList;
	va_start(vaList, numArgs);

	char* list = (char*) calloc(numArgs, sizeof(char));
	for(int i = 0; i < numArgs; ++i) {
		*(list + i) = va_arg(vaList, int);
	}

	va_end(vaList);

	while(!found) {
		fgets(input, 99, stdin);
		if(strlen(input) == 2) {
			for(int i = 0; i < numArgs; ++i) {
				if(input[0] == *(list + i)) {
					found = true;
					break;
				}
			}
		}

		if(!found) {
			printf("Input did not match expected options. Please try again.\n");
		}
	}

	free(list);

	return input[0];
}

