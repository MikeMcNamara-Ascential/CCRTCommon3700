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
#include <sys/time.h>
//#include "ILogger.h"
#include <stdarg.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/mman.h>


#if _NTO_VERSION >= 630
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif



DE10Nano::DE10Nano()
{
    //!!! Moved to init
    //!outFile = fopen("DE10NanoLog.txt", "w");

    // LOG_FN_ENTRY = 1;
    m_portNum = 25565;
    m_sampleRate = 20;
    m_delayRate = 5;
    m_numChannels = 4;
    //m_connected = false;
    m_sending = false;
    m_closing = false;
    fifoSize = 50;
    m_samplerTail = 0;
    m_noTock = 0;
    m_childPID = 0;
    m_logoCount = 0;

    IPC.response = (bool *)mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    IPC.command = (char *)mmap(NULL, 14 * sizeof(char), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    IPC.position = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    m_connected = (bool *)mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    m_setSampleRate = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    m_setNumChannels = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sampler = (ICMSample_t *)mmap(NULL, fifoSize * sizeof(ICMSample_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    //init stuff
    m_connected[0] = false;
    m_setSampleRate[0] = 20;
    m_setNumChannels[0] = 4;

    DE10NanoInfo.VersionARM = 0;
    DE10NanoInfo.VersionFPGA = 0;
    DE10NanoInfo.config.SampleRate = 0;
    DE10NanoInfo.config.NumChannels = 0;
    DE10NanoInfo.config.KeepAliveTime = 200;

    IPC.response[0] = false;
    IPC.position[0] = 0;

    m_terminating = false;
    // Hard set keep alive timeout - deal with it...
    m_keepAliveRate = 450;
    clock_gettime(CLOCK_REALTIME, &m_timeSinceTick);
}


DE10Nano::~DE10Nano()
{
    //check ICM.m_sending
    //if false disconnect and set all the fields to default
    /*
    if (m_connected[0] && !m_sending)
    {
        m_server->Log(LOG_DEV_DATA, "Stick the landing...");
        StopSending();
    }
    */
}

void DE10Nano::Initialize(const ILogger &loggerCfg)
{
    Log(LOG_FN_ENTRY, "Enter DE10Nano::Initialize( Logger)\n\r");
    SetAdditionalLogFileName(loggerCfg.GetAdditionalLogFileName());
    SetProcessName(loggerCfg.GetProcessName());
    SetVerboseMask(loggerCfg.GetVerboseMask());
    SetDebug(loggerCfg.IsDebugOn());
    SetLocalEcho(loggerCfg.GetLocalEcho());
    if (loggerCfg.GetLogStatus())
    {
        EnableLogger();
    }

    Log(LOG_FN_ENTRY, "External logging %s", m_de10NanoExternalLogging ? "Enabled" : "Disabled");
    if (m_de10NanoExternalLogging)
        outFile = fopen("DE10NanoLog.txt", "w");


    Log(LOG_FN_ENTRY, "Exit DE10Nano::Initialize( Logger)\n\r");
}

//-------------------------------------------------------------------------------------------------
void* DE10Nano::TcpThread_PsuedoServer(void *context)
{
    return((DE10Nano *)context)->PsuedoServer(context);
}

bool DE10Nano::Initialize(int portNum)
{
    bool retVal = false;

    Log(LOG_FN_ENTRY, "Enter DE10Nano::Initialize(portNum: %d) -- See pthread()ed connecting here!\n", portNum);

    m_portNum = portNum;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(NULL, &attr, &DE10Nano::TcpThread_PsuedoServer, (void *)this);
    //pthread_create(NULL, &attr, DE10Nano::TcpThread_PsuedoServer, this);

    Log(LOG_FN_ENTRY, "Exiting DE10Nano::Initialize()");
    //return retVal;
    return true;
}

void* DE10Nano::PsuedoServer(void *arg)
{
    //DE10Nano *server = (DE10Nano *)arg;
    m_server = (DE10Nano *)arg;
    m_server->Log(LOG_FN_ENTRY, "Enter pthread()ed DE10Nano::PsuedoServer( )");

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int i = 0;
    int missed = 0;
    m_terminating = false;
    m_server->Log(LOG_DEV_DATA, "Allocating Mem...");
    uint32_t *data = (uint32_t *)mmap(NULL, (m_numChannels * 2 + 1) * 4, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    //set up the connection and connect
    while (!m_terminating)
    {
        missed  = 0;
        m_noTock = 0;
        Connect(clilen, serv_addr, cli_addr);
        //Infinite loop that checks for ipc strings and sleeps at the end
        delay(500);
        if (m_connected[0])
        {
            SetSampleRate(m_setSampleRate[0]);
            SetNumChannels(m_setNumChannels[0]);

            delay(50);
            GetConf();
        }
        while (m_connected[0] && !m_terminating)
        {
            //Logo(LOG_DEV_DATA, "m_connected KeepAlive();\n");
            KeepAlive();
            if (IPC.response[0])
            {
                //Logo(LOG_DEV_DATA, "Response Recieved!\n\r");
                //server->Log(LOG_FN_ENTRY, "Waiting for sync...");
                msync(IPC.command, 14, MS_SYNC);
                //server->Log(LOG_FN_ENTRY, "Sync'd!");
                HandleCommand();
            }
            if (m_sending)
            {
                do
                {
                    //n = recv(newsockfd,data,20,MSG_DONTWAIT)
                    //Logo(LOG_DEV_DATA, "Attempting Read");
                    n = read(newsockfd, data, (m_numChannels * 4 * 2 + 4));
                    if (n == m_numChannels * 4 * 2 + 4)
                    {
                        missed  = 0;
                        SaveData(data);
                        m_noTock = 0;
                        KeepAlive();
                    }
                    else
                    {
                        missed++;
                        m_server->Log(LOG_DEV_DATA, "[%d] -- Empty read :& \n", missed);
                        KeepAlive();
                    }
                    if (missed >= 5)
                    {
                        m_server->Log(LOG_DEV_DATA, "[%d] empty reads, closing connection...\n", missed);
                        Logo(LOG_DEV_DATA, "[%d] empty reads, closing connection...\n", missed);
                        m_closing = true;
                        m_connected[0] = false;
                    }
                }while (n > 0 && !IPC.response[0] && !m_closing);

                if (m_closing)
                {
                    m_sending = false;
                    //m_connected = false;
                    m_closing = false;
                    // Clear out any remaining packets
                    while (n > 0)
                        n = recv(newsockfd, NULL, m_numChannels * 4 * 2, MSG_DONTWAIT);
                }
                //			printf("data: %u, %u, %u, %u, %u\n",data[0],data[1],data[2],data[3],data[4]);

            }
            //printf("Delaying for %d", m_delayRate);
            if (!m_terminating)
                delay(m_delayRate);
        }
        Logo(LOG_DEV_DATA, "Conneciton lost!!!\n\r");
        m_server->Log(LOG_DEV_DATA, "Conneciton lost!!!\n\r");

        close(newsockfd);
        close(sockfd);
        m_connected[0] = false;
    }
    m_server->Log(LOG_DEV_DATA, "Exit pthread()ed DE10Nano::PsuedoServer( )");
    return this;
}

int DE10Nano::makeBatman()
{
    Logo(LOG_FN_ENTRY, "Entering makeBatman()\n\r");
    Log(LOG_FN_ENTRY, "Entering makeBatman()\n\r");
    try
    {
        m_childPID = fork();

        if (m_childPID == 0)
        {
            //child process
            outFile = fopen("DE10NanoChildLog.txt", "w");
            Logo(LOG_DEV_DATA, "DE10Nano fork()ed and Running\n\r");
            //BposSetPriority(0,30);
            run();
        }
    }
    catch (...)
    {
        return 0;
    }
    Logo(LOG_FN_ENTRY, "Exiting makeBatman()\n\r");
    Log(LOG_FN_ENTRY, "Exiting makeBatman()\n\r");
    return m_childPID;
}

void DE10Nano::run()
{
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int i = 0;
    int missed = 0;
    Logo(LOG_DEV_DATA, "Allocating Mem\n");
    uint32_t *data = (uint32_t *)mmap(NULL, (m_numChannels * 2 + 1) * 4/*sizeof(uint32_t)*/, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    //set up the connection and connect
    Logo(LOG_FN_ENTRY, "Enter fork()ed DE10Nano::run( )\n");
    Logo(LOG_FN_ENTRY, "Enter fork()ed DE10Nano::run( )\n");
    while (true)
    {
        missed  = 0;
        m_noTock = 0;
        Connect(clilen, serv_addr, cli_addr);
        //Infinite loop that checks for ipc strings and sleeps at the end
        delay(500);
        if (m_connected[0])
        {
            SetSampleRate(m_setSampleRate[0]);
            SetNumChannels(m_setNumChannels[0]);
        }
        while (m_connected[0])
        {
            //Logo(LOG_DEV_DATA, "m_connected KeepAlive();\n");
            KeepAlive();
            if (IPC.response[0])
            {
                Logo(LOG_DEV_DATA, "Response Recieved!\n\r");
                HandleCommand();
            }
            if (m_sending)
            {
                do
                {
                    //n = recv(newsockfd,data,20,MSG_DONTWAIT)
                    //Logo(LOG_DEV_DATA, "Attempting Read");
                    n = read(newsockfd, data, (m_numChannels * 4 * 2 + 4));
                    if (n == m_numChannels * 4 * 2 + 4)
                    {
                        missed  = 0;
                        SaveData(data);
                        KeepAlive();
                    }
                    else
                    {
                        missed++;
                        Logo(LOG_DEV_DATA, "[%d] -- Empty read :& \n", missed);
                        KeepAlive();
                    }
                    if (missed >= 5)
                    {
                        Logo(LOG_DEV_DATA, "[%d] empty reads, closing connection...\n", missed);
                        m_closing = true;
                        m_connected[0] = false;
                    }
                }while (n > 0 && !IPC.response[0] && !m_closing);
                if (m_closing)
                {
                    m_sending = false;
                    //m_connected = false;
                    m_closing = false;
                    // Clear out any remaining packets
                    while (n > 0)
                        n = recv(newsockfd, NULL, m_numChannels * 4 * 2, MSG_DONTWAIT);
                }
                //			printf("data: %u, %u, %u, %u, %u\n",data[0],data[1],data[2],data[3],data[4]);

            }
            //printf("Delaying for %d", m_delayRate);
            delay(m_delayRate);
        }
        Logo(LOG_DEV_DATA, "Conneciton lost!!!\n\r");
        close(newsockfd);
        close(sockfd);
        m_connected[0] = false;
    }
    exit(i);
}

void DE10Nano::Connect(socklen_t clilen, struct sockaddr_in serv_addr, struct sockaddr_in cli_addr)
{
    Logo(LOG_FN_ENTRY, "Enter DE10Nano::Connect() :| \n");
    m_server->Log(LOG_FN_ENTRY, "Enter DE10Nano::Connect()");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        Logo(LOG_DEV_DATA, "ERROR opening socket\n\rGo Home Socket, you're drunk.\n\r");
        m_server->Log(LOG_DEV_DATA, "ERROR opening socket!");
        close(newsockfd);
        close(sockfd);
        return;
    }
    memset(&serv_addr, sizeof(serv_addr), sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(m_portNum);
    int reusePort = 1;
    struct timeval tv;
    tv.tv_sec = 1;  /* 1 Sec(s) Timeout */
    tv.tv_usec = 0L;

    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        Logo(LOG_DEV_DATA, "ERROR on binding\n\rIf I can't have this port, life isn't worth living...\n\r");
        m_server->Log(LOG_DEV_DATA, "ERROR on binding");
        close(newsockfd);
        close(sockfd);
        return;
    }
    /*
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));
    */
    Logo(LOG_DEV_DATA, "Socket Bound! Listening....\r\n");
    m_server->Log(LOG_DEV_DATA, "Socket Bound! Listening...");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        Logo(LOG_DEV_DATA, "ERROR on accept\n\r");
        m_server->Log(LOG_DEV_DATA, "ERROR on accept\n\r");
        close(sockfd);
        close(newsockfd);
    }
    else
    {
        setsockopt(newsockfd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
        setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
        m_connected[0] = true;
    }
    IPC.response[0] = false;
    Logo(LOG_FN_ENTRY, "Exit DE10Nano::Connect()\n");
    m_server->Log(LOG_DEV_DATA, "Exit DE10Nano::Connect(%s)", m_connected[0] ? "Connected" : "Not Connected");
}

bool DE10Nano::SetSampleRate(int inSampleRate)
{
    bool retVal = false;
    Logo(LOG_FN_ENTRY, "Enter DE10Nano::SetSampleRate( %d)\n", inSampleRate);
    m_server->Log(LOG_DEV_DATA, "Enter DE10Nano::SetSampleRate( %d)\n", inSampleRate);

    if (m_connected[0] && !m_sending)
    {
        char command[16];
        int n;
        sprintf(command, "conf msec %*d", 2, inSampleRate);
        n = write(newsockfd, command, strlen(command));
        if (n < 0)
            m_server->Log(LOG_DEV_DATA, "ERROR writing to socket\n\r");
        else
        {
            retVal = true;
            m_sampleRate = inSampleRate;
        }
    }
    else
        m_server->Log(LOG_DEV_DATA, "ERROR, Can't write configuration to device while it is sending data.\n\r");

    m_delayRate = inSampleRate / 4;
    m_server->Log(LOG_DEV_DATA, "Exiting DE10Nano::SetSampleRate(%d) - m_delayRate: %d\n", m_sampleRate, m_delayRate);
    return retVal;
}

bool DE10Nano::SetNumChannels(int inNumChannels)
{
    bool retVal = false;
    m_server->Log(LOG_DEV_DATA, "Enter DE10Nano::SetNumChannels( %d)\n", inNumChannels);

    if (m_connected[0] && !m_sending)
    {
        char command[12];
        int n;
        sprintf(command, "conf chan %*d", 2, inNumChannels);
        n = write(newsockfd, command, strlen(command));
        if (n < 0)
            m_server->Log(LOG_DEV_DATA, "ERROR writing to socket\n\r");
        else
        {
            retVal = true;
            m_numChannels = inNumChannels;
        }
    }
    else
        m_server->Log(LOG_DEV_DATA, "ERROR, Can't write configuration to device while it is sending data.\n\r");


    m_server->Log(LOG_DEV_DATA, "Exiting DE10Nano::SetNumChannels()\n");
    return retVal;
}

bool DE10Nano::SetPortNum(int inPortNum)
{
    m_server->Log(LOG_DEV_DATA, "DE10Nano::SetPortNum(%d)", inPortNum);
    m_portNum = inPortNum;
    return true;
}

bool DE10Nano::StartSending()
{
    bool retVal = false;
    int n;
    m_server->Log(LOG_DEV_DATA, "Enter DE10Nano::StartSending()\n");

    if (m_connected[0] && !m_sending)
    {
        n = write(newsockfd, "send", 4);
        if (n < 0)
            m_server->Log(LOG_DEV_DATA, "ERROR writing to socket\n\r");
        else
        {
            m_sending = true;
            retVal = true;
        }
    }
    else
        m_server->Log(LOG_DEV_DATA, "ERROR, either no device is available, or it is already sending data.\n\r");

    m_server->Log(LOG_DEV_DATA, "Exiting DE10Nano::StartSending()\n");
    return retVal;
}

bool DE10Nano::StopSending()
{
    bool retVal = false;
    int n;
    m_server->Log(LOG_DEV_DATA, "Enter DE10Nano::StopSending()\n");
    m_noTock = 0;

    if (m_sending && m_connected[0])
    {
        Logo(LOG_DEV_DATA, "Sending Stop Command\n");
        n = write(newsockfd, "stop", 4);
        if (n < 0)
            Logo(LOG_DEV_DATA, "ERROR writing to socket\n\r");
        else
        {
            //m_closing = true;
            retVal = true;
            m_sending = false; // Would be cool to get a double check slash handshake here...
        }
    }
    else
        Logo(LOG_DEV_DATA, "ERROR,  either no device is available, or it is not sending data.\n\r");

    /* Leave the connect/disconnect to someone else
    if (!m_connected[0])
        m_closing = true;
    */
    if (m_logoCount >= 9)
        m_logoCount = 0;
    else
        m_logoCount++;

    m_server->Log(LOG_DEV_DATA, "Exiting DE10Nano::StopSending() -- Next log index: %d\n", m_logoCount);
    if (m_de10NanoExternalLogging)
    {
        // Should prob function this out.. but what eves
        char buff[64];
        string logFilename = CreateMessage(buff, sizeof(buff), "DE10NanoLog_%d.txt", m_logoCount);
        outFile = fopen(logFilename.c_str(), "w");
    }
    return retVal;
}

bool DE10Nano::ClearCount()
{
    bool retVal = false;
    int n;
    m_server->Log(LOG_DEV_DATA, "Enter DE10Nano::ClearCount()\n");

    if (m_connected[0] && !m_sending)
    {
        n = write(newsockfd, "rset", 4);
        if (n < 0)
            m_server->Log(LOG_DEV_DATA, "ERROR writing to socket\n\r");
        else
        {
            Logo(1, "Count Reset\n");
            retVal = true;
            IPC.position[0] = 0;
        }
    }
    else
        m_server->Log(LOG_DEV_DATA, "ERROR, either no device is available, or it is already sending data.\n\r");

    m_server->Log(LOG_DEV_DATA, "Exiting DE10Nano::ClearCount()\n");
    return retVal;
}

DE10NanoConf_t DE10Nano::GetConf()
{
    DE10NanoConf_t conf;
    bool retVal = false;
    int n;
    char inConf[100];
    memset(inConf, 0, sizeof(inConf));
    char temp[7];
    memset(temp, 0, sizeof(temp));
    m_server->Log(LOG_DEV_DATA, "Enter DE10Nano::GetConf()\n");

    /* Don't do this
    //Clear out any remaining packets
    while (n > 0)
        n = recv(newsockfd, inConf, sizeof(inConf), MSG_DONTWAIT);
    */

    if (m_connected[0] && !m_sending)
    {
        n = write(newsockfd, "info", 4);
        if (n < 0)
        {
            Logo(LOG_DEV_DATA, "ERROR writing to socket\n\r");
            //m_connected[0] = false;
        }
        else
        {
            m_server->Log(LOG_DEV_DATA, "\tRequested Conf");
        }
       
               
        int timeout = 0;
        n = 0;
        do
        {
            timeout++;
            delay(20);
            n += recv(newsockfd, inConf, 60, MSG_DONTWAIT);
        }while (timeout < 5 && n <= 0);

        //Version: XXXX  mSec Latch: XXXXXX  Channels XX
        //Version: XXXXXXXXX\n\rmSec Latch: XXXXXX\n\rChannels: XX\n\r

        m_server->Log(LOG_DEV_DATA, "%s", inConf);
    }
    else
        m_server->Log(LOG_DEV_DATA, "ERROR, either no device is available, or it is already sending data.\n\r");
    if (!retVal)
    {
        return DE10NanoInfo.config;
    }


    //n = read(newsockfd, inConf, 60/*46*/);
    //Version: XXXX  mSec Latch: XXXXXX  Channels XX
    //Version: XXXXXXXXX\n\rmSec Latch: XXXXXX\n\rChannels: XX\n\r

    //m_server->Log(LOG_DEV_DATA, "%s", inConf);
    
    printf(inConf);
    printf("\n\r");
    memcpy(temp, inConf + 27, 6);
    printf(temp);
    printf("\n\r");
    conf.SampleRate = atoi(temp);
    memset(temp, 0, sizeof(temp));
    memcpy(temp, inConf + 44, 2);
    printf(temp);
    printf("\n\r");
    conf.NumChannels = atoi(temp); 
    

    m_server->Log(LOG_DEV_DATA, "Exiting DE10Nano::GetConf()\n");

    return conf;
}
bool DE10Nano::HandleCommand()
{
    Logo(LOG_FN_ENTRY, "Entering DE10Nano::HandleCommand()\n");
    m_server->Log(LOG_FN_ENTRY, "Entering DE10Nano::HandleCommand()\n");
    char temp[12] = "";
    memcpy(temp, IPC.command, 4);
    Logo(LOG_DEV_DATA, " command: [%s]\n\r", temp);
    if (!strcmp(temp, "conf"))
    {
        memcpy(temp, IPC.command + 5, 4);
        Logo(LOG_DEV_DATA, " parameter %s\n\r", temp);
        if (!strcmp(temp, "msec"))
        {
            memcpy(temp, IPC.command + 10, 6);
            Logo(LOG_DEV_DATA, " value: %s\n\r", temp);
            SetSampleRate(atoi(temp));
        }
        if (!strcmp(temp, "chan"))
        {
            memcpy(temp, IPC.command + 10, 2);
            Logo(LOG_DEV_DATA, " value: %s\n\r", temp);
            SetNumChannels(atoi(temp));
        }
    }
    else if (!strcmp(temp, "send"))
    {
        StartSending();
    }
    else if (!strcmp(temp, "rset"))
    {
        ClearCount();
        DE10NanoInfo.config = GetConf(); 
    }
    else if (!strcmp(temp, "stop"))
    {
        StopSending();
    }
    else if (!strcmp(temp, "tick")) /*Not commanded...*/
    {
        TickTock();
    }
    else if (!strcmp(temp, "gcon"))
    {
        DE10NanoInfo.config = GetConf();
    }
    else if (!strcmp(temp, "term"))
    {
        //IpIcmMgr runs a Cleanup on terminate which Detaches ISR and we already get a stop command
        //StopSending();
        //!!!fclose(outFile);
        m_connected[0] = false;
        m_terminating = true;
    }
    IPC.response[0] = false;
    Logo(LOG_DEV_DATA, "Exiting DE10Nano::HandleCommand(%s) -- m_connected[0]: %s, m_sending: %s\n",
         temp, m_connected[0] ? "Connected" : "NOT Connected", m_sending ? "Sending" : "NOT Sending");
    m_server->Log(LOG_DEV_DATA, "Exiting DE10Nano::HandleCommand(%s) -- m_connected[0]: %s, m_sending: %s\n",
                  temp, m_connected[0] ? "Connected" : "NOT Connected", m_sending ? "Sending" : "NOT Sending");
    return true;
}

void DE10Nano::SaveData(uint32_t *data)
{

    float freq[m_numChannels];
    char caTimeBuff[22];
    char buff[128];
    GetTimeString((INT8 *)caTimeBuff);
    //Logo(LOG_DEV_DATA, "%s   SavingData: ", caTimeBuff);
    //Logo(LOG_DEV_DATA, "SavingData: ");
    string dataOut = "SavingData: ";

    for (int i = 0; i < (m_numChannels * 2) + 1; i++)
    {
        if (i == 0)
        {
            //handle the index from the DE10Nano
            //Logo(LOG_DEV_DATA, "index[%d]: ", data[i]);
            //sampler[IPC.position[0]%fifoSize].index = data[i];

            dataOut += CreateMessage(buff, sizeof(buff), "index[%d]: ", data[i]);
        }
        else if (i % 2 == 1)
        {
            //Logo(LOG_DEV_DATA, "data[%d] = %d, ", (i - 1) / 2, data[i]);
            dataOut += CreateMessage(buff, sizeof(buff), "data[%d] = %d, ", (i - 1) / 2, data[i]);
            sampler[IPC.position[0] % fifoSize].channels[(i - 1) / 2] = data[i];


        }
        else
        {
            freq[(i - 1) / 2] = *((float *)(&data[i]));
            dataOut += CreateMessage(buff, sizeof(buff), "freq[%f], ", freq[(i - 1) / 2]);
            //Logo(LOG_DEV_DATA, "freq[%f], ", freq[(i - 1) / 2]);
            sampler[IPC.position[0] % fifoSize].freqs[(i - 1) / 2] = freq[(i - 1) / 2];
        }
    }
    //Logo(LOG_DEV_DATA, " index[%d] \n", IPC.position[0] % fifoSize);
    dataOut += CreateMessage(buff, sizeof(buff), " index[%d] \n", IPC.position[0] % fifoSize);
    sampler[IPC.position[0] % fifoSize].index = IPC.position[0]++;
    //Logo(LOG_DEV_DATA, "\n");
    /* Need details?
    if (m_logoCount++ >= 500 )
    {
        Logo(LOG_DEV_DATA, "[Periodic Logging] %s\n", dataOut.c_str());
        m_logoCount = 0;
    }
    */
    Logo(LOG_DEV_DATA, "%s\n", dataOut.c_str());
    m_server->Log(LOG_DETAILED_DATA, "%s\n", dataOut.c_str());
    //IPC.position[0]++;
}

void DE10Nano::TickTock()
{
    int n = -1;
    //Logo(LOG_FN_ENTRY, "Enter DE10Nano::TickTock()\n");

    if (m_connected[0])
    {
        try
        {
            n = send(newsockfd, "tick", 4, 0);
            if (n < 0)
            {
                Logo(LOG_DEV_DATA, "ERROR writing to socket\n\r");
                m_connected[0] = false;
            }
            else
            {
                Logo(LOG_DEV_DATA, "\tSent [tick]\n");
                m_server->Log(LOG_DEV_DATA, "\tSent [tick]");
            }
            if (!m_sending && m_connected[0])
            {
                char temp[7];
                memset(temp, 0, sizeof(temp));
                int timeout = 0;
                n = 0;
                do
                {
                    timeout++;
                    delay(20);
                    n += recv(newsockfd, temp, 4, MSG_DONTWAIT);
                    //n = read(newsockfd, temp, 4);
                }while (timeout < 5 && n <= 0);
                Logo(LOG_DEV_DATA, "\t\tGot [%s]\n", temp);
                m_server->Log(LOG_DEV_DATA, "\t\tGot [%s]", temp);
                if (strcmp(temp, "tock"))
                {
                    Logo(LOG_DEV_DATA, "No tock? No problem ;)\n");
                    m_server->Log(LOG_DEV_DATA, "No tock? No problem ;)");
                    /*
                    if (m_noTock >= 1500)
                    {
                        //m_connected[0] = false;
                        Logo(LOG_DEV_DATA, "No tock limit[1500] reached! Pressuming no connect..\n");
                    }
                    else
                        m_noTock++;
                        */
                }
                else
                    m_noTock = 0;

                do
                {
                    n = recv(newsockfd, NULL, 4, MSG_DONTWAIT);
                }while (n > 0);

            }
            else
                Logo(LOG_DEV_DATA, "Not waiting around for a tock? -- m_connected[0]: %s, m_sending: %s\n",
                     m_connected[0] ? "Connected" : "NOT Connected", m_sending ? "Sending" : "NOT Sending");
        }
        catch (...)
        {
            Logo(LOG_DEV_DATA, "Caught an Exception Yo.\n\r");
        }
    }
    else
    {
        m_server->Log(LOG_DEV_DATA, "ERROR, either no device is available, or it is not sending data.\n\r");
        //m_connected = false;
    }

    //Logo(LOG_FN_ENTRY, "Exiting DE10Nano::TickTock()\n");
}

void DE10Nano::KeepAlive()
{

    //Logo(LOG_FN_ENTRY, "Enter DE10Nano::KeepAlive()\n");
    m_keepAliveTimer += m_delayRate;

    // How about an actual timer...
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    double elapsedTime = (currentTime.tv_sec - m_timeSinceTick.tv_sec) * 1000 + (currentTime.tv_nsec - m_timeSinceTick.tv_nsec) / 1000000;


    //if (m_keepAliveTimer >= DE10NanoInfo.config.KeepAliveTime)
    if (elapsedTime > m_keepAliveRate)
    {
        TickTock();
        m_keepAliveTimer = 0;
        clock_gettime(CLOCK_REALTIME, &m_timeSinceTick);
    } /*
 else 
     Logo(LOG_DEV_DATA, "Elapsed: %.3f; Timeout: %.3f", 
          elapsedTime, m_keepAliveRate);*/
    //Logo(LOG_FN_ENTRY, "Exiting DE10Nano::KeepAlive()\n");
}

void DE10Nano::IsAlive()
{
    if (m_childPID)
    {
        //Logo(LOG_FN_ENTRY, "DE10Nano::IsAlive()\n");
        //this is the Parent
        int status;
        pid_t result = waitpid(m_childPID, &status, WNOHANG);
        if (result == 0)
        {
            //Logo(LOG_DEV_DATA,"Child Process still alive.\n\r");
            // Child still alive
            return;
        }
        else if (result == -1)
        {
            Logo(LOG_DEV_DATA, "Error Checking Child Process.\n\r");
            // Error
            return;
        }
        else
        {
            // Child exited
            Logo(LOG_DEV_DATA, "Child Process has terminated.\n\r");
            //makeBatman();
        }
    } /*
     else
         Logo(0x00000060, "Child knows that it's alive...\n\r");
         */
}
//REMOVE WHEN DONE TESTING
void  DE10Nano::Logo(unsigned int mask, const char *cpFmt, ...)
{
    if (m_de10NanoExternalLogging)
    {
        char caMessage[200];
        va_list         vaArgs;

        // This string gets built up and sent over ipc
        std::string stOutMesg;

        // Next 21 bytes: human-readable time stamp
        char caTimeBuff[22];
        GetTimeString((INT8 *)caTimeBuff);
        stOutMesg.append(caTimeBuff);
        //stOutMesg.append(1, (char)('0x80'));

        //printf("LOG:%s ",cpFmt);
        // parse out the varaible argument message
        // clear the print buffer
        memset(caMessage, 0, sizeof(caMessage));
        // format the input into a String
        va_start(vaArgs, cpFmt);
        // Limit the size of the message so that it fits into the buffer (else SIGSEGV)
        vsnprintf(caMessage, sizeof(caMessage) - 1, (char *)cpFmt, vaArgs);
        // remove leftovers
        va_end(vaArgs);

        //if (!m_childPID)
        //    return;

        string newOut = string(stOutMesg) + string(" -- ") + string(caMessage);
        //fprintf(outFile, "%s", caMessage);
        fprintf(outFile, "%s", newOut.c_str());
        fflush(outFile);
    }
}
