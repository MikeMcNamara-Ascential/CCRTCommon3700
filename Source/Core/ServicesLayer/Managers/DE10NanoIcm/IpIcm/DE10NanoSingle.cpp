//******************************************************************************
// Description:
//  Ethernet driver for communication with the DE10Nano ICM.
//  Contains all parameters and mothods for communication with the DE10Nano
//
//==============================================================================
//
//     Copyright © 2017 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "DE10Nano.h"
#include <netdb.h> // for gethostbyname and hostent
#include <sys/neutrino.h>
#include <stdlib.h>
//#include "ILogger.h"
#include <stdarg.h>
#include <unistd.h>


DE10Nano::DE10Nano() 
{
	// LOG_FN_ENTRY = 1;
	m_portNum = 25565;
	m_sampleRate = 20;
	m_delayRate = 5;
	m_numChannels = 4;
	m_connected = false;
	m_sending = false;
	m_closing = false;
	fifoSize = 50;
	m_samplerTail = 0;

	IPC.response = (bool*)mmap(NULL, sizeof(bool), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	IPC.command = (char*)mmap(NULL, 14*sizeof(char), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	IPC.position = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	sampler = (ICMSample_t*)mmap(NULL, fifoSize*sizeof(ICMSample_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);

	DE10NanoInfo.VersionARM = 0;
	DE10NanoInfo.VersionFPGA = 0;
	DE10NanoInfo.config.SampleRate = 0;
	DE10NanoInfo.config.NumChannels = 0;

	IPC.response[0] = false;
	IPC.position[0] = 0;

	outFile = fopen("DE10NanoLog.txt","w");

}


DE10Nano::~DE10Nano() 
{

	//check ICM.m_sending
	//if false disconnect and set all the fields to default

}

int DE10Nano::makeBatman() 
{
	Logo( LOG_FN_ENTRY, "Entering makeBatman()\n\r");
	try
	{ 
		//if(!fork())
		//{
		//child process
			//outFile = fopen("DE10NanoChildLog.txt","w");
			Logo( LOG_DEV_DATA, "DE10Nano Running\n\r");
			BposSetPriority(0,35);
			run();
		//}
		/* else 
		{
		//parent process
			return 	1;
		} */
	}
	catch(...) 
	{
		return 0;
	}
	Logo( LOG_FN_ENTRY, "Exiting makeBatman()\n\r");
	return 1;
}
void DE10Nano::Initialize(const ILogger& loggerCfg) 
{ 
	Logo( LOG_FN_ENTRY, "Enter DE10Nano::Initialize( Logger)\n\r");
 SetAdditionalLogFileName( loggerCfg.GetAdditionalLogFileName());
    SetProcessName( loggerCfg.GetProcessName());
    SetVerboseMask( loggerCfg.GetVerboseMask());
    SetDebug(loggerCfg.IsDebugOn());
    SetLocalEcho( loggerCfg.GetLocalEcho());
    if( loggerCfg.GetLogStatus())
    {
		Logo( LOG_DEV_DATA, "Enabling Logger\n\r");
        EnableLogger();
    }	
	Logo( LOG_FN_ENTRY, "Exit DE10Nano::Initialize( Logger)\n\r");
}

bool DE10Nano::Initialize(int portNum) 
{ 
	bool retVal = false;

	Logo( LOG_FN_ENTRY, "Enter DE10Nano::Initialize( %d)\n", portNum);
	//printf("Enter DE10nano::Initialize( %d)\n", portNum);

	m_portNum = portNum;

	IPC.response[0] = true;
	// open socket and add listener
	makeBatman();
	int time = 0;
	while ((IPC.response[0]) && time < 1000) 
	{
		delay(10);
		time++;
	}

	if(IPC.response[0])
	{
		//Failed to connect
		retVal = false;
		Logo( LOG_DEV_DATA, "ERROR: Failed to initialize DE10Nano!\n");
	}	
	else 
	{
		//connected
		retVal = true;
		Logo( LOG_DEV_DATA, "DE10Nano succesfully initialized.\n");
	}
	Logo(LOG_FN_ENTRY,"Exiting DE10Nano::Initialize( %d)\n",portNum);
	return retVal;
}

void DE10Nano::run() 
{
    
	int n;
    int i = 0;
//set up the connection and connect
	Logo(LOG_FN_ENTRY,"Enter DE10Nano::run( )\n");

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if(sockfd < 0)
	{
       Logo(LOG_DEV_DATA,"ERROR opening socket\n\rGo Home Socket, you're drunk.\n\r"); 
	   return;
	}
    memset(&serv_addr, sizeof(serv_addr),sizeof(serv_addr));
	
	data = (uint32_t*)mmap(NULL, 5*sizeof(uint32_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(m_portNum);
	if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		Logo(LOG_DEV_DATA,"ERROR on binding\n\rIf I can't have this port, life isn't worth living...\n\r");
		return;
	}
    Logo(LOG_DEV_DATA,"Socekt Bound! Listening....\r\n");
	listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        Logo(LOG_DEV_DATA,"ERROR on accept\n\r");
	else 
		m_connected = true;
	IPC.response[0] = false;
	//Infinite loop that checks for ipc strings and sleeps at the end
	
}
bool DE10Nano::SetSampleRate(int inSampleRate) 
{
	bool retVal = false;
	Logo(LOG_FN_ENTRY,"Enter DE10Nano::SetSampleRate( %d)\n", inSampleRate);

	if(m_connected && !m_sending)
	{
		char command[16];
		int n;
		sprintf(command,"conf msec %*d",6,inSampleRate);
		n = write(newsockfd,command,strlen(command)); 
		if (n < 0) Logo(LOG_FN_ENTRY,"ERROR writing to socket\n\r");
		else 
		{
			retVal = true;
			m_sampleRate = inSampleRate;
		}
	}
	else 
		Logo(LOG_FN_ENTRY,"ERROR, Can't write configuration to device while it is sending data.\n\r");

	Logo(LOG_FN_ENTRY,"Exiting DE10Nano::SetSampleRate()\n");
	m_delayRate = inSampleRate/4;
	return retVal;
}

bool DE10Nano::SetNumChannels(int inNumChannels) 
{
	bool retVal = false;
	Logo( LOG_FN_ENTRY, "Enter DE10Nano::SetNumChannels( %d)\n", inNumChannels);

	if(m_connected && !m_sending)
	{
		char command[12];
		int n;
		sprintf(command,"conf chan %*d",2,inNumChannels);
		n = write(newsockfd,command,strlen(command)); 
		if (n < 0) Logo(LOG_FN_ENTRY,"ERROR writing to socket\n\r");
		else 
		{
			retVal = true;
			m_numChannels = inNumChannels;
		}
	}
	else 
		Logo(LOG_FN_ENTRY,"ERROR, Can't write configuration to device while it is sending data.\n\r");


	Logo( LOG_FN_ENTRY, "Exiting DE10Nano::SetNumChannels()\n");
	return retVal;
}

bool DE10Nano::SetPortNum(int inPortNum) 
{
	m_portNum = inPortNum;
}

bool DE10Nano::StartSending() 
{
	bool retVal = false;
	int n;
	Logo(LOG_FN_ENTRY,"Enter DE10Nano::StartSending()\n");

	if(m_connected && !m_sending)
	{
		n = write(newsockfd,"send",4); 
		if (n < 0) Logo(LOG_FN_ENTRY,"ERROR writing to socket\n\r");
		else 
		{
			m_sending = true;
			retVal = true;
		}
	}
	else 
		Logo(LOG_FN_ENTRY,"ERROR, either no device is available, or it is already sending data.\n\r");

	Logo(LOG_FN_ENTRY,"Exiting DE10Nano::StartSending()\n");
	return retVal;
}

bool DE10Nano::StopSending() 
{
	bool retVal = false;
	int n;
	Logo(LOG_FN_ENTRY,"Enter DE10Nano::StopSending()\n");

	if(m_connected && m_sending)
	{
		n = write(newsockfd,"stop",4); 
		if (n < 0) Logo(LOG_FN_ENTRY,"ERROR writing to socket\n\r");
		else 
		{
			m_closing = true;
			retVal = true;
		}
	}
	else 
		Logo(LOG_FN_ENTRY,"ERROR,  either no device is available, or it is not sending data.\n\r");

	Logo(LOG_FN_ENTRY,"Exiting DE10Nano::StopSending()\n");
	return retVal;
}

bool DE10Nano::ClearCount() 
{
	bool retVal = false;
	int n;
	Logo(LOG_FN_ENTRY,"Enter DE10Nano::ClearCount()\n");

	if(m_connected && !m_sending)
	{
		n = write(newsockfd,"rset",4); 
		if (n < 0) Logo(LOG_FN_ENTRY,"ERROR writing to socket\n\r");
		else 
		{
			Logo(1,"Count Reset");
			retVal = true;
		}
	}
	else 
		Logo(LOG_FN_ENTRY,"ERROR, either no device is available, or it is already sending data.\n\r");

	Logo(LOG_FN_ENTRY,"Exiting DE10Nano::ClearCount()\n");
	return retVal;
}

DE10NanoConf_t DE10Nano::GetConf() 
{
	DE10NanoConf_t conf;
	bool retVal = false;
	int n;
	char inConf[100];
	memset(inConf, 0 , sizeof(inConf));
	char temp[7];
	memset(temp, 0 , sizeof(temp));
	Logo(LOG_FN_ENTRY,"Enter DE10Nano::GetConf()\n");

	//Clear out any remaining packets
	while (n > 0) 
		n = recv(newsockfd,inConf,sizeof(inConf),MSG_DONTWAIT);

	if(m_connected && !m_sending)
	{
		n = write(newsockfd,"info",4); 
		if (n < 0) Logo(LOG_FN_ENTRY,"ERROR writing to socket\n\r");
		else 
		{
			retVal = true;
		}
	}
	else 
		Logo(LOG_FN_ENTRY,"ERROR, either no device is available, or it is already sending data.\n\r");
	if(!retVal)
	{
		return DE10NanoInfo.config;
	}
	n = read(newsockfd,inConf,46);
	//Version: XXXX  mSec Latch: XXXXXX  Channels XX
	printf(inConf);
	printf("\n\r");
	memcpy(temp,inConf+27,6);
	printf(temp);
	printf("\n\r");
	conf.SampleRate = atoi(temp);
	memset(temp,0,sizeof(temp));
	memcpy(temp,inConf+44,2);
	printf(temp);
	printf("\n\r");
	conf.NumChannels = atoi(temp);
	
	Logo(LOG_FN_ENTRY,"Exiting DE10Nano::GetConf()\n");

	return conf;
}
bool DE10Nano::HandleCommand() 
{
	Logo(LOG_FN_ENTRY,"Entering DE10Nano::HandleCommand()\n");
	char temp[12];
	memcpy(temp,IPC.command,4);
	Logo(LOG_DEV_DATA," command: %s\n\r",temp);
	if(!strcmp(temp,"conf"))
	{
		memcpy(temp,IPC.command+5,4);
		Logo(LOG_DEV_DATA," parameter %s\n\r",temp);
		if(!strcmp(temp,"msec"))
		{

			memcpy(temp,IPC.command+10,6);
			Logo(LOG_DEV_DATA," value: %s\n\r",temp);
			SetSampleRate(atoi(temp));
		}
		if(!strcmp(temp,"chan"))
		{

			memcpy(temp,IPC.command+10,2);
			Logo(LOG_DEV_DATA," value: %s\n\r",temp);
			SetNumChannels(atoi(temp));
		}
	}
	else if(!strcmp(temp,"send"))
	{
		StartSending();
	}
	else if(!strcmp(temp,"rset"))
	{
		ClearCount();
	}
	else if(!strcmp(temp,"stop"))
	{
		StopSending();
	}
	else if(!strcmp(temp,"gcon"))
	{
		DE10NanoInfo.config = GetConf();
	}
	else if(!strcmp(temp,"term"))
	{
		fclose(outFile);
		m_connected = false;
	}
	IPC.response[0] = false;
	Logo(LOG_FN_ENTRY,"Exiting DE10Nano::HandleCommand()\n");
	return true;
}

void DE10Nano::SaveData(uint32_t *data) 
{
	
	char caTimeBuff[22];
    GetTimeString( (INT8*)caTimeBuff );
	Logo(LOG_DEV_DATA, "%s   SavingData: ",caTimeBuff);
	for (int i = 0; i < m_numChannels; i++) 
	{
		Logo(LOG_DEV_DATA, "data[%d], ", data[i]);
		sampler[IPC.position[0]%fifoSize].channels[i] = data[i];
	}
	Logo(LOG_DEV_DATA, " index[%d] \n",IPC.position[0]%fifoSize);
	sampler[IPC.position[0]%fifoSize].index = IPC.position[0]++;
}

void DE10Nano::GetData(){
	
	int n = 0;
    int i = 0;
	
	if(m_connected) 
	{
		if(IPC.response[0])
		{
			Logo(LOG_DEV_DATA,"Response Recieved!\n\r");
			HandleCommand();
		}
		if(m_sending)
		{
			do
			{
				//n = recv(newsockfd,data,20,MSG_DONTWAIT);
				n = read(newsockfd,data,20);
				if(n == 20)
					SaveData(data);
			} 
			while (n>0 && !IPC.response[0] && !m_closing);
			if(m_closing)
			{
				m_sending = false;
				//m_connected = false;
				m_closing = false;
				// Clear out any remaining packets
				while (n > 0) 
					n = recv(newsockfd,NULL,20,MSG_DONTWAIT);
			}
		}
		delay(m_delayRate);
	}
	else{
		return;
		close(newsockfd);
		close(sockfd);
		exit(i);	
	}
}

//REMOVE WHEN DONE TESTING
void  DE10Nano::Logo( unsigned int mask, const char *cpFmt, ...)
{
	char caMessage[200];
	va_list         vaArgs;

	//printf("LOG:%s ",cpFmt);

	// parse out the varaible argument message
	// clear the print buffer
	memset(caMessage, 0 , sizeof(caMessage));
	// format the input into a String
	va_start( vaArgs, cpFmt);
	// Limit the size of the message so that it fits into the buffer (else SIGSEGV)
	vsnprintf(caMessage, sizeof(caMessage)-1, (char *)cpFmt, vaArgs);
	// remove left overs
	va_end( vaArgs);
	fprintf(outFile, "%s", caMessage);
}
