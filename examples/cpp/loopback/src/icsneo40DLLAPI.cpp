#include "icsneo40DLLAPI.h"

//Basic Functions
FINDNEODEVICES icsneoFindDevices;
OPENNEODEVICE icsneoOpenNeoDevice;
OPENNEODEVICEBYCHANNELS icsneoOpenNeoDeviceByChannels;

CLOSEPORT icsneoClosePort;
FREEOBJECT icsneoFreeObject;
OPENPORTEX icsneoOpenPortEx;
OPENPORT   icsneoOpenPort; 
FINDALLUSBDEVICES icsneoFindAllUSBDevices; 
FINDALLCOMDEVICES icsneoFindAllCOMDevices; 

//Message Functions
GETMESSAGES icsneoGetMessages;
TXMESSAGES icsneoTxMessages;
TXMESSAGESEX icsneoTxMessagesEx;
WAITFORRXMSGS icsneoWaitForRxMessagesWithTimeOut;
GETTSFORMSG icsneoGetTimeStampForMsg;
ENABLERXQUEUE icsneoEnableNetworkRXQueue;
GETISO15765STATUS icsneoGetISO15765Status;
SETISO15765RXPARMS icsneoSetISO15765RxParameters;
GETOPENPORTS icsneoGetOpenPorts; 
GETPERFTIMER icsneoGetPerformanceTimer; 
GETDLLFIRMWARE icsneoGetDLLFirmwareInfo; 
GETHWFIRMWAREINFO icsneoGetHWFirmwareInfo;  
GETCANCONTROLLERCLKFREQ icsneoGetCANControllerClockFrequency;
FIREMWAREUPDATEREQUIRED icsneoFirmwareUpdateRequired;

//Device Functions
GETCONFIG icsneoGetConfiguration;
SENDCONFIG icsneoSendConfiguration;
GETFIRESETTINGS icsneoGetFireSettings;
SETFIRESETTINGS icsneoSetFireSettings;
GETFIRE2SETTINGS icsneoGetFire2Settings;
SETFIRE2SETTINGS icsneoSetFire2Settings;
GETVCAN3SETTINGS icsneoGetVCAN3Settings;
SETVCAN3SETTINGS icsneoSetVCAN3Settings;
SETBITRATE icsneoSetBitRate;
SETBITRATEEX icsneoSetBitRateEx;
GETDEVICEPARMS icsneoGetDeviceParameters;
SETDEVICEPARMS icsneoSetDeviceParameters;
ENABLENETWORKCOM icsneoEnableNetworkCom; 
ENABLENETWORKCOMEX icsneoEnableNetworkComEx; 
GETPERFORMANCEPARAMETERS icsneoGetPerformanceParameters; 
FORCEFWUPDATE icsneoForceFirmwareUpdate; 
OPENDEODEVICEBYSERIALNUMBER icsneoOpenNeoDeviceBySerialNumber; 
SETUSBLATENCY icsneoSetUSBLatency; 
SETCANPARAMETERS icsneoSetCANParameters; 
SETCANPARAMETERSPHILIPSJA1000 icsneoSetCANParametersPhilipsSJA1000; 
INITTXRXAPP icsneoInitTxRxApp; 
PERFORMTXRXPAIRS icsneoPerformTxRxPairs; 
CLEARAPP icsneoClearApp;  //Deprecated
ENABLEBITSMASH icsneoEnableBitSmash;
SENDHWTIMEREQUEST icsneoSendHWTimeRequest;
RECEIVEHWTIMERESP icsneoReceiveHWTimeResponse; 
LOADDEFAULTSETTINGS icsneoLoadDefaultSettings;
SETFDBITRATE icsneoSetFDBitRate;
PERFORMTXRXPAIRSEX icsneoPerformTxRxPairsEx;
GETDLLVERSIONEX icsneoGetDLLVersionEx;
SENDTEXTAPI icsneoSendTextApi;
GETTEXTAPI icsneoGetTextApi;

//Error Functions
GETLASTAPIERROR icsneoGetLastAPIError;
GETERRMSGS icsneoGetErrorMessages;
GETERRORINFO icsneoGetErrorInfo;

//General Utility Functions
VALIDATEHOBJECT icsneoValidateHObject;
GETDLLVERSION icsneoGetDLLVersion;
GETSERIALNUMBER icsneoGetSerialNumber;
STARTSOCKSERVER icsneoStartSockServer;
STOPSOCKSERVER icsneoStopSockServer;

//CoreMini Functions
SCRIPTLOAD icsneoScriptLoad;
SCRIPTSTART icsneoScriptStart;
SCRIPTSTOP icsneoScriptStop;
SCRIPTCLEAR icsneoScriptClear;
SCRIPTSTARTFBLOCK icsneoScriptStartFBlock;
SCRIPTSTOPFBLOCK icsneoScriptStopFBlock;
SCRIPTGETFBLOCKSTATUS icsneoScriptGetFBlockStatus;
SCRIPTGETSCRIPTSTATUS icsneoScriptGetScriptStatus;
SCRIPTREADAPPSIGNAL icsneoScriptReadAppSignal;
SCRIPTWRITEAPPSIGNAL icsneoScriptWriteAppSignal;
SCRIPTREADRXMESSAGE icsneoScriptReadRxMessage; //Deprecated
SCRIPTREADTXMESSAGE icsneoScriptReadTxMessage; //Deprecated
SCRIPTWRITERXMESSAGE icsneoScriptWriteRxMessage; //Deprecated
SCRIPTWRITETXMESSAGE icsneoScriptWriteTxMessage; //Deprecated
SCRIPTREADISO15765TXMESSAGE icsneoScriptReadISO15765_2_TxMessage; //Obsolete
SCRIPTWRITEISO15765TXMESSAGE icsneoScriptWriteISO15765_2_TxMessage;  //Obsolete

//HWSupport functions
ISO15765HWSUPPORT icsneoSetupISO15765HwSupport; 
ISO15765HWSUPPORTEX icsneoSetupISO15765HwSupportEx;

J2534CMD icsneoJ2534Cmd;
ENABLEBUSVOLTMON icsneoEnableBusVoltageMonitor;
ISO15765_DISABLENETWORKS icsneoISO15765_DisableNetworks;
ENABLECMUSBFILTER icsneoEnableCoreMiniUsbFilter;
ISO15765_TXMESSAGE icsneoISO15765_TransmitMessage;
GETBUSVOLTAGE icsneoGetBusVoltage;
SCRIPTREADISO15765_2_RX_MSG icsneoScriptReadISO15765_2_RxMessage;
ISO15765_2_ENABLENETWORKS icsneoISO15765_EnableNetworks;
SCRIPTWRITE_ISO15765_2_RX_MSG icsneoScriptWriteISO15765_2_RxMessage;

OPENREMOTEDEVICE icsneoOpenRemoteNeoDevice;
FINDREMOTEDEVICES icsneoFindRemoteNeoDevices;

LOCKCHANNELS icsneoLockChannels;
UNLOCKCHANNELS icsneoUnlockChannels;
GETCHANNELLOCKSTATUS icsneoGetChannelLockStatus;

//The following are valid strings for setting parameters on devices 
//using the icsneoGetDeviceParameters() and icsneoSetDeviceParameters() functions
char *FireParameters[] =
{
	"can1", "can2", "can3", "can4", "swcan", "lsftcan", "lin1", "lin2",
	"lin3", "lin4", "cgi_baud", "cgi_tx_ifs_bit_times",
	"cgi_rx_ifs_bit_times", "cgi_chksum_enable", "network_enables", 
	"network_enabled_on_boot", "pwm_man_timeout", "pwr_man_enable", 
	"misc_io_initial_ddr", "misc_io_initial_latch", "misc_io_analog_enable", 
	"misc_io_report_period", "misc_io_on_report_events", "ain_sample_period", 
	"ain_threshold", "iso15765_separation_time_offset", "iso9141_kwp_settings", 
	"perf_en", "iso_parity", "iso_msg_termination", "network_enables_2"
};


char *VCAN3Parameters[] =
{
	"can1", "can2", "network_enables", "network_enabled_on_boot", "iso15765_separation_time_offset",
	"perf_en", "misc_io_initial_ddr", "misc_io_initial_latch", "misc_io_report_period", 
	"misc_io_on_report_events"
};

char *CANParameters[] = 
{
     "Mode", "SetBaudrate", "Baudrate", "NetworkType", "TqSeg1",
	 "TqSeg2", "TqProp", "TqSync", "BRP", "auto_baud"
};

char *SWCANParameters[] = 
{
	 "Mode", "SetBaudrate", "Baudrate", "NetworkType", "TqSeg1", "TqSeg2", 
	 "TqProp", "TqSync", "BRP", "high_speed_auto_switch", "auto_baud"
};

char *LINParameters[] = 
{
	 "Baudrate", "spbrg", "brgh", "MasterResistor", "Mode"
};

char *ISOKWPParms[] =
{
	 "Baudrate", "spbrg", "brgh", "init_steps", "init_step_count", 
	 "p2_500us", "p3_500us", "p4_500us", "chksum_enabled"
};

char getCharInput(int numArgs, ...);

bool LoadDLLAPI(HINSTANCE &hAPIDLL)
{
	char ans = 0;
	
	printf("Which DLL would you like to load?\n[1] - icsneo40.dll\n[2] - icsneolegacy.dll\n");
	ans = getCharInput(2, '1', '2');

	if (ans == '1')
	{
		if ((hAPIDLL = LoadLibrary("icsneo40.dll")) == NULL)
		{
			printf("Problem loading icsneo40.dll\nMake sure dll is accessible");
			return false;
		}
	}
	else
	{
		if ((hAPIDLL = LoadLibrary("../../../icsneolegacy.dll")) == NULL)
		{
			printf("LoadLibrary(\"../../../icsneolegacy.dll\") failed.\n");
			if ((hAPIDLL = LoadLibrary("icsneolegacy.dll")) == NULL)
			{
				printf("Problem loading icsneolegacy.dll\nMake sure dll is accessible");
				return false;
			}
			else
				printf("LoadLibrary(\"icsneolegacy.dll\") succeeded.");
		}
		else
		{

		}
	}
	char path[MAX_PATH];
	GetModuleFileName(hAPIDLL, path, MAX_PATH);
	printf("DLL module path:%s\n", path);

	icsneoFindDevices =    (FINDNEODEVICES) GetProcAddress(hAPIDLL,              "icsneoFindDevices");
	icsneoOpenNeoDevice =     (OPENNEODEVICE) GetProcAddress(hAPIDLL,               "icsneoOpenNeoDevice");
 	icsneoOpenNeoDeviceByChannels = (OPENNEODEVICEBYCHANNELS) GetProcAddress(hAPIDLL, "icsneoOpenNeoDeviceByChannels");
 
	icsneoClosePort =         (CLOSEPORT) GetProcAddress(hAPIDLL,                   "icsneoClosePort");	
	icsneoFreeObject =        (FREEOBJECT) GetProcAddress(hAPIDLL,                  "icsneoFreeObject");
    icsneoOpenPortEx =        (OPENPORTEX) GetProcAddress(hAPIDLL,                  "icsneoOpenPortEx");
	
    icsneoTxMessages =        (TXMESSAGES) GetProcAddress(hAPIDLL,                  "icsneoTxMessages");
	icsneoTxMessagesEx =       (TXMESSAGESEX) GetProcAddress(hAPIDLL,               "icsneoTxMessagesEx");
	icsneoGetMessages =       (GETMESSAGES) GetProcAddress(hAPIDLL,                 "icsneoGetMessages");
	icsneoWaitForRxMessagesWithTimeOut = (WAITFORRXMSGS) GetProcAddress(hAPIDLL,    "icsneoWaitForRxMessagesWithTimeOut");
    icsneoGetTimeStampForMsg = (GETTSFORMSG) GetProcAddress(hAPIDLL,                "icsneoGetTimeStampForMsg");
    icsneoEnableNetworkRXQueue = (ENABLERXQUEUE) GetProcAddress(hAPIDLL,            "icsneoEnableNetworkRXQueue");
    icsneoGetISO15765Status =  (GETISO15765STATUS) GetProcAddress(hAPIDLL,          "icsneoGetISO15765Status");
    icsneoSetISO15765RxParameters = (SETISO15765RXPARMS) GetProcAddress(hAPIDLL,    "icsneoSetISO15765RxParameters");

	icsneoGetConfiguration =  (GETCONFIG) GetProcAddress(hAPIDLL,                   "icsneoGetConfiguration");
  	icsneoSendConfiguration = (SENDCONFIG) GetProcAddress(hAPIDLL,                  "icsneoSendConfiguration");
    icsneoGetFireSettings =   (GETFIRESETTINGS) GetProcAddress(hAPIDLL,             "icsneoGetFireSettings");
	icsneoSetFireSettings =   (SETFIRESETTINGS) GetProcAddress(hAPIDLL,             "icsneoSetFireSettings");
    icsneoGetFire2Settings =  (GETFIRE2SETTINGS) GetProcAddress(hAPIDLL,            "icsneoGetFire2Settings");
	icsneoSetFire2Settings =  (SETFIRE2SETTINGS) GetProcAddress(hAPIDLL,            "icsneoSetFire2Settings");
	icsneoGetVCAN3Settings =  (GETVCAN3SETTINGS) GetProcAddress(hAPIDLL,            "icsneoGetVCAN3Settings");
    icsneoSetVCAN3Settings =  (SETVCAN3SETTINGS) GetProcAddress(hAPIDLL,            "icsneoSetVCAN3Settings");

    icsneoSetBitRate =        (SETBITRATE)       GetProcAddress(hAPIDLL,            "icsneoSetBitRate");
    icsneoSetBitRateEx =      (SETBITRATEEX)     GetProcAddress(hAPIDLL,            "icsneoSetBitRateEx");
	icsneoGetDeviceParameters = (GETDEVICEPARMS) GetProcAddress(hAPIDLL,            "icsneoGetDeviceParameters");
	icsneoSetDeviceParameters = (SETDEVICEPARMS) GetProcAddress(hAPIDLL,            "icsneoSetDeviceParameters");

	icsneoGetLastAPIError =   (GETLASTAPIERROR) GetProcAddress(hAPIDLL,             "icsneoGetLastAPIError");
    icsneoGetErrorMessages = (GETERRMSGS) GetProcAddress(hAPIDLL,                   "icsneoGetErrorMessages");
    icsneoGetErrorInfo =     (GETERRORINFO) GetProcAddress(hAPIDLL,                 "icsneoGetErrorInfo");

	icsneoScriptLoad =        (SCRIPTLOAD) GetProcAddress(hAPIDLL,                  "icsneoScriptLoad");
	icsneoScriptStart =       (SCRIPTSTART) GetProcAddress(hAPIDLL,                 "icsneoScriptStart");
	icsneoScriptStop =        (SCRIPTSTOP) GetProcAddress(hAPIDLL,                  "icsneoScriptStop");
	icsneoScriptClear =       (SCRIPTCLEAR) GetProcAddress(hAPIDLL,                 "icsneoScriptClear");
	icsneoScriptStartFBlock = (SCRIPTSTARTFBLOCK) GetProcAddress(hAPIDLL,           "icsneoScriptStartFBlock");
	icsneoScriptStopFBlock =  (SCRIPTSTOPFBLOCK) GetProcAddress(hAPIDLL,            "icsneoScriptStopFBlock");
	icsneoScriptGetFBlockStatus = (SCRIPTGETFBLOCKSTATUS) GetProcAddress(hAPIDLL,   "icsneoScriptGetFBlockStatus");
	icsneoScriptGetScriptStatus = (SCRIPTGETSCRIPTSTATUS) GetProcAddress(hAPIDLL, "icsneoScriptGetScriptStatus");
	icsneoScriptReadAppSignal = (SCRIPTREADAPPSIGNAL) GetProcAddress(hAPIDLL,       "icsneoScriptReadAppSignal");
    icsneoScriptWriteAppSignal = (SCRIPTWRITEAPPSIGNAL) GetProcAddress(hAPIDLL,     "icsneoScriptWriteAppSignal");

	icsneoScriptReadRxMessage = (SCRIPTREADRXMESSAGE) GetProcAddress(hAPIDLL,       "icsneoScriptReadRxMessage");
	icsneoScriptReadTxMessage = (SCRIPTREADTXMESSAGE) GetProcAddress(hAPIDLL,       "icsneoScriptReadTxMessage");
    icsneoScriptWriteRxMessage = (SCRIPTWRITERXMESSAGE)GetProcAddress(hAPIDLL,      "icsneoScriptWriteRxMessage");
	icsneoScriptWriteTxMessage = (SCRIPTWRITETXMESSAGE) GetProcAddress(hAPIDLL,     "icsneoScriptWriteTxMessage");
 	icsneoScriptReadISO15765_2_TxMessage = (SCRIPTREADISO15765TXMESSAGE) GetProcAddress(hAPIDLL, "icsneoScriptReadISO15765_2_TxMessage");
	icsneoScriptWriteISO15765_2_TxMessage = (SCRIPTWRITEISO15765TXMESSAGE) GetProcAddress(hAPIDLL, "icsneoScriptWriteISO15765_2_TxMessage");
	icsneoOpenPort 						  = (OPENPORT) GetProcAddress(hAPIDLL,"icsneoOpenPort");
	icsneoEnableNetworkCom 				  = (ENABLENETWORKCOM) GetProcAddress(hAPIDLL,"icsneoEnableNetworkCom");
	icsneoEnableNetworkComEx 			  = (ENABLENETWORKCOMEX) GetProcAddress(hAPIDLL,"icsneoEnableNetworkComEx");
	icsneoFindAllUSBDevices 			  = (FINDALLUSBDEVICES) GetProcAddress(hAPIDLL,"icsneoFindAllUSBDevices");
	icsneoFindAllCOMDevices 			  = (FINDALLCOMDEVICES) GetProcAddress(hAPIDLL,"icsneoFindAllCOMDevices"); 
	icsneoSetupISO15765HwSupport		  = (ISO15765HWSUPPORT) GetProcAddress(hAPIDLL,"icsneoSetupISO15765HwSupport"); 
	icsneoSetupISO15765HwSupportEx 		  = (ISO15765HWSUPPORTEX) GetProcAddress(hAPIDLL,"icsneoSetupISO15765HwSupportEx"); 
	icsneoGetPerformanceParameters 		  = (GETPERFORMANCEPARAMETERS) GetProcAddress(hAPIDLL,"icsneoGetPerformanceParameters"); 
	icsneoGetOpenPorts 		             = (GETOPENPORTS) GetProcAddress(hAPIDLL,"icsneoGetOpenPorts"); 
	icsneoGetPerformanceTimer 		     = (GETPERFTIMER) GetProcAddress(hAPIDLL,"icsneoGetPerformanceTimer");
	icsneoForceFirmwareUpdate  		     = (FORCEFWUPDATE) GetProcAddress(hAPIDLL,"icsneoForceFirmwareUpdate");
	icsneoGetHWFirmwareInfo  		     = (GETHWFIRMWAREINFO) GetProcAddress(hAPIDLL,"icsneoGetHWFirmwareInfo");
	icsneoOpenNeoDeviceBySerialNumber    = (OPENDEODEVICEBYSERIALNUMBER) GetProcAddress(hAPIDLL,"icsneoOpenNeoDeviceBySerialNumber");
	icsneoSetUSBLatency  		         = (SETUSBLATENCY) GetProcAddress(hAPIDLL,"icsneoSetUSBLatency");
	icsneoGetCANControllerClockFrequency = (GETCANCONTROLLERCLKFREQ) GetProcAddress(hAPIDLL,"icsneoGetCANControllerClockFrequency");
	icsneoSetCANParameters  		     = (SETCANPARAMETERS) GetProcAddress(hAPIDLL,"icsneoSetCANParameters");
	icsneoSetCANParametersPhilipsSJA1000 = (SETCANPARAMETERSPHILIPSJA1000) GetProcAddress(hAPIDLL,"icsneoSetCANParametersPhilipsSJA1000");
	icsneoInitTxRxApp  		          = (INITTXRXAPP) GetProcAddress(hAPIDLL,"icsneoInitTxRxApp");
	icsneoPerformTxRxPairs  		  = (PERFORMTXRXPAIRS) GetProcAddress(hAPIDLL,"icsneoPerformTxRxPairs");
	icsneoClearApp  		          = (CLEARAPP) GetProcAddress(hAPIDLL,"icsneoClearApp");
	icsneoEnableBitSmash  		      = (ENABLEBITSMASH) GetProcAddress(hAPIDLL,"icsneoEnableBitSmash");
	icsneoSendHWTimeRequest  		  = (SENDHWTIMEREQUEST) GetProcAddress(hAPIDLL,"icsneoSendHWTimeRequest");
	icsneoReceiveHWTimeResponse       = (RECEIVEHWTIMERESP) GetProcAddress(hAPIDLL,"icsneoReceiveHWTimeResponse");
	icsneoGetDLLFirmwareInfo  		  = (GETDLLFIRMWARE)  GetProcAddress(hAPIDLL,"icsneoGetDLLFirmwareInfo");
	icsneoJ2534Cmd  		          = (J2534CMD) GetProcAddress(hAPIDLL, "icsneoJ2534Cmd");
	icsneoEnableBusVoltageMonitor     = (ENABLEBUSVOLTMON) GetProcAddress(hAPIDLL, "icsneoEnableBusVoltageMonitor");
	icsneoISO15765_DisableNetworks    = (ISO15765_DISABLENETWORKS) GetProcAddress(hAPIDLL, "icsneoISO15765_DisableNetworks");
	icsneoEnableCoreMiniUsbFilter  	  = (ENABLECMUSBFILTER) GetProcAddress(hAPIDLL, "icsneoEnableCoreMiniUsbFilter");	
	icsneoISO15765_TransmitMessage    = (ISO15765_TXMESSAGE) GetProcAddress(hAPIDLL, "icsneoISO15765_TransmitMessage");
	icsneoGetBusVoltage 		      = (GETBUSVOLTAGE) GetProcAddress(hAPIDLL, "icsneoGetBusVoltage");
    icsneoScriptReadISO15765_2_RxMessage = (SCRIPTREADISO15765_2_RX_MSG) GetProcAddress(hAPIDLL, "icsneoScriptReadISO15765_2_RxMessage");
	icsneoISO15765_EnableNetworks = (ISO15765_2_ENABLENETWORKS) GetProcAddress(hAPIDLL, "icsneoISO15765_EnableNetworks");	
	icsneoScriptWriteISO15765_2_RxMessage = (SCRIPTWRITE_ISO15765_2_RX_MSG) GetProcAddress(hAPIDLL, "icsneoScriptWriteISO15765_2_RxMessage");
	icsneoOpenRemoteNeoDevice = (OPENREMOTEDEVICE) GetProcAddress(hAPIDLL, "icsneoOpenRemoteNeoDevice");	
    icsneoFindRemoteNeoDevices = (FINDREMOTEDEVICES) GetProcAddress(hAPIDLL, "icsneoFindRemoteNeoDevices");
	icsneoLockChannels  		       = (LOCKCHANNELS) GetProcAddress(hAPIDLL, "icsneoLockChannels");
	icsneoUnlockChannels  		       = (UNLOCKCHANNELS) GetProcAddress(hAPIDLL, "icsneoUnlockChannels");
	icsneoGetChannelLockStatus = (GETCHANNELLOCKSTATUS) GetProcAddress(hAPIDLL, "icsneoGetChannelLockStatus");
	icsneoFirmwareUpdateRequired = (FIREMWAREUPDATEREQUIRED) GetProcAddress(hAPIDLL, "icsneoFirmwareUpdateRequired");
	icsneoSetFDBitRate = (SETFDBITRATE) GetProcAddress(hAPIDLL, "icsneoSetFDBitRate");
	icsneoPerformTxRxPairsEx = (PERFORMTXRXPAIRSEX) GetProcAddress(hAPIDLL, "icsneoPerformTxRxPairsEx");
	icsneoGetDLLVersionEx = (GETDLLVERSIONEX) GetProcAddress(hAPIDLL, "icsneoGetDLLVersionEx");
	icsneoGetDLLVersion = (GETDLLVERSION) GetProcAddress(hAPIDLL, "icsneoGetDLLVersion");
	icsneoGetTextApi = (GETTEXTAPI) GetProcAddress(hAPIDLL, "icsneoGetTextApi");
	icsneoSendTextApi = (SENDTEXTAPI) GetProcAddress(hAPIDLL, "icsneoSendTextApi");


    if(!icsneoFindDevices || !icsneoOpenNeoDevice || !icsneoOpenNeoDeviceByChannels || !icsneoClosePort || !icsneoFreeObject ||
	   !icsneoOpenPortEx || !icsneoTxMessages || !icsneoGetMessages || !icsneoWaitForRxMessagesWithTimeOut ||
       !icsneoGetTimeStampForMsg || !icsneoEnableNetworkRXQueue || !icsneoGetISO15765Status ||
       !icsneoSetISO15765RxParameters || !icsneoGetConfiguration || !icsneoSendConfiguration ||
       !icsneoGetFireSettings || !icsneoSetFireSettings || !icsneoGetVCAN3Settings ||
       !icsneoSetVCAN3Settings || !icsneoSetBitRate || !icsneoGetDeviceParameters ||
       !icsneoSetDeviceParameters || !icsneoGetLastAPIError || !icsneoGetErrorMessages ||
       !icsneoGetErrorInfo || !icsneoScriptLoad || !icsneoScriptStart || !icsneoScriptStop ||
       !icsneoScriptClear || !icsneoScriptStartFBlock || !icsneoScriptStopFBlock ||
       !icsneoScriptGetFBlockStatus || !icsneoScriptGetScriptStatus || !icsneoScriptReadAppSignal ||
       !icsneoScriptWriteAppSignal || !icsneoScriptReadRxMessage || !icsneoScriptReadTxMessage ||
       !icsneoScriptWriteRxMessage || !icsneoScriptWriteTxMessage || !icsneoScriptReadISO15765_2_TxMessage ||
       !icsneoScriptWriteISO15765_2_TxMessage || !icsneoOpenPort || !icsneoEnableNetworkCom || !icsneoFindAllUSBDevices || !icsneoFindAllCOMDevices || !icsneoSetupISO15765HwSupport || 
	   !icsneoSetupISO15765HwSupportEx || !icsneoGetPerformanceParameters || !icsneoGetOpenPorts || !icsneoGetPerformanceTimer || !icsneoGetHWFirmwareInfo || 
	   !icsneoOpenNeoDeviceBySerialNumber || /*!icsneoSetUSBLatency || */ !icsneoGetCANControllerClockFrequency || !icsneoSetCANParameters || 
	   !icsneoSetCANParametersPhilipsSJA1000 || !icsneoInitTxRxApp || !icsneoPerformTxRxPairs || !icsneoClearApp || !icsneoEnableBitSmash || /*!icsneoSendHWTimeRequest ||*/
	   /*!icsneoReceiveHWTimeResponse || */ !icsneoGetDLLFirmwareInfo || !icsneoJ2534Cmd || !icsneoEnableBusVoltageMonitor ||
	   !icsneoISO15765_DisableNetworks || !icsneoEnableCoreMiniUsbFilter || !icsneoISO15765_TransmitMessage || !icsneoGetBusVoltage ||
	   !icsneoScriptReadISO15765_2_RxMessage || !icsneoISO15765_EnableNetworks || //!icsneoScriptWriteISO15765_2_RxMessage ||
	   !icsneoOpenRemoteNeoDevice || !icsneoFindRemoteNeoDevices || !icsneoGetChannelLockStatus || 	!icsneoLockChannels || !icsneoUnlockChannels)
	{
		return true;
		FreeLibrary(hAPIDLL);

		return false;
	}
	int iVerNumber=icsneoGetDLLVersion();
	printf("ICS dll version %d\r\n\r\n", iVerNumber);
    return true;
}	


void UnloadDLLAPI(HINSTANCE &hAPIDLL)
{
	if(hAPIDLL)
		FreeLibrary(hAPIDLL);
}