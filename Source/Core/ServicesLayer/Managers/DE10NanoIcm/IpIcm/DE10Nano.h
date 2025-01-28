
class DE10Nano;

#ifndef _DE10NANO_H_INCLUDED_
#define _DE10NANO_H_INCLUDED_

#include <sys/mman.h>
#include <vector>
#include <string>
#include <stdint.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "IpIcmDefs.h"
#include "ILogger.h"	// Logger interface

// DE10Nano Configuration Information
typedef struct
{
    int SampleRate;   //Sample rate for the DE10Nano
    int NumChannels;  //Number of channels to read from the DE10Nano
    int KeepAliveTime;

} DE10NanoConf_t;

// DE10Nano Device information
typedef struct
{
    int VersionFPGA; //Firmware Version
    int VersionARM;  //Software Version
    DE10NanoConf_t config; //DE10Nano Configuration info

} DE10NanoInfo_t;



typedef struct
{
    char *command; //String that holds the command to send to the DE10Nano
    bool *response; //true if the has responed to the last command
    int *position; //holds the position in the FIFO that the DE10Nano last wrote to

} ICMIPC;

typedef struct
{
    int channels[MAX_CHANNELS];
    float freqs[MAX_CHANNELS];
    uint32_t index;
} ICMSample_t;

typedef struct
{
    /**
     * Port that the DE10Nano connects to.
     */
    int     m_portNum;

    /**
     * how quickly the DE10Nano should latch and send the count. 
     * Expressed in mSec. 
     */
	int		m_sampleRate;
    int     m_delayRate;

    /**
     * Number of channels that the DE10Nano is to transmit. 
     */
	int		m_numChannels;
    /**
     * true if the DE10Nano is in the sending state.
     */
    bool    m_sending;

    /**
     * holds the current count for the KeepAlive Method
     */
    int m_keepAliveTimer;

} DE10Nano_t;

class DE10Nano : public ILogger, public DE10Nano_t
{
public:
    DE10Nano();
    ~DE10Nano();
    /**
     * Hold the file pointer for the log file
     */
    FILE *outFile;
    /**
     * Holds the file descripters for the sockets, and the size of 
     * the fifo for the received samples.
     * 
     * 
     */
    int sockfd, newsockfd, fifoSize;
    /**
     * Holds the variables used for Inter Process Communication. 
     */
    ICMIPC IPC;
    /**
     * Holds the samples received from the DE10Nano ICM
     */
    ICMSample_t *sampler;
    /**
     * Holds the version numbers and configuration imformation for 
     * the connected DE10Nano. 
     */
    DE10NanoInfo_t DE10NanoInfo;
    /**
     * Points to the last sample in the Sampler Array
     */
    int m_samplerTail;
    /**
     * The PID of the child process 
     */
    pid_t m_childPID;
    /**
     * True if a DE10Nano is connected to the port.
     */
    bool    *m_connected;

    int *m_setSampleRate;
    int *m_setNumChannels;

    int m_logoCount;

    struct timespec m_currentTime;
    struct timespec m_timeSinceTick;
    double m_keepAliveRate;




    /**
     * Initializes our logger interface object
     *
     * @param loggerCfg The logger object whose configuration we want to copy
     */
    void Initialize(const ILogger &loggerCfg);

    /**
     * Attempts to initialize the DE10Nano Device on the given port
     * number
     *
     * @param portNum The port that the DE10Nano will be trying to
     *  			  connect to. 
     *
     * @return true if successfully initialized, false if an error occurrs
     */
    bool Initialize(int portNum);

    /**
     * Tells the DE10Nano Device to begin transmitting data
     *
     * @return true if device is connected, false if an error
     *  	   occurrs
     */
    bool StartSending();

    /**
     * Tells the DE10Nano Device to stop transmitting data
     *
     * @return true if device is connected, false if an error
     *  	   occurrs
     */
    bool StopSending();

    /**
     * Sets the sample rate for the DE10Nano to the passed value
     *  
     * @param the sample rate for the DE10Nano 
     * 
     * @return true if the device responds with the new sample rate
     */
    bool SetSampleRate(int inSampleRate);

    /**
     * Sets the number of channels to read from the DE10Nano to the 
     * passed value 
     *  
     * @param the number of channels to read from the DE10Nano
     * 
     * @return true if the device responds with the new number of 
     *  	   channels
     */
    bool SetNumChannels(int inNumChannels);

    /**
     * Gets the info string from the DE10Nano and returns it.
     * 
     * @return A string containing the configuration and device info 
     *  	   from the DE10Nano
     */
    DE10NanoInfo_t GetInfo();

    /**
     * Gets the configuration info string from the DE10Nano and 
     * returns it. 
     * 
     * @return A string containing the configuration info from the
     *  	   DE10Nano
     */
    DE10NanoConf_t GetConf();

    /**
     * Sets the port number that the DE10Nano ICM will be connecting 
     * to. 
     * 
     * @author JLyle (5/16/2018)
     * 
     * @param inPortNum the port number to use
     * 
     * @return bool returns true
     */
    bool SetPortNum(int inPortNum);

    /**
     * Forks the process and starts the child process's run method 
     * from which it should never excape. 
     * 
     * @author JLyle (5/16/2018)
     * 
     * @return Returns the PID of the child process if successful, 0 
     *         if unsuccessfull.
     */
    int makeBatman();


    /**
     * Runs this thread in the background, collecting informaiton 
     * from the ethernet module and storing it for processing by the 
     * system.  
     * 
     */
    void run();

    /**
     * takes a command passed to the ICM using it's IPC command and 
     * recieve locations. Reads the command and calls whatever 
     * function should be called from it. 
     * 
     * @return true if the command was handled.
     */
    bool HandleCommand();

    /**
     * Connects to an available DE10Nano ICM.
     * 
     * @author JLyle (5/16/2018)
     * 
     * @param clilen 
     * @param serv_addr 
     * @param cli_addr 
     */
    void Connect(socklen_t clilen, struct sockaddr_in serv_addr, struct sockaddr_in cli_addr);

    /**
     * Sends a message to a connected DE10Nano ICM to clear it's 
     * count registers.  
     * 
     * @author JLyle (5/16/2018)
     * 
     * @return bool True if the count was reset. False if there was 
     *         an issue connecting or communicating.
     */
    bool ClearCount();

    /**
     * Sends and receives a message with a connected DE10Nano ICM to 
     * keep the connection alive, and to disconnect if the other 
     * device is unavailable. 
     * 
     * @author JLyle (5/16/2018)
     */
    void TickTock();

    /**
     * Checks to see if the child process is still running, and if 
     * not, starts another one. 
     * 
     * @author JLyle (5/16/2018)
     */
    void IsAlive();

    static void* DE10Nano::TcpThread_PsuedoServer(void *context);
    void* DE10Nano::PsuedoServer(void *arg);
    bool m_de10NanoExternalLogging; 

protected:

    /**
     * Stores data in the sampler array for processing by the 
     * IpIcmMgr executable. 
     * @param data 
     */
    void SaveData(uint32_t *data);

    /**
     * Logs Data To a file given by the OutFile Variable.
     * 
     * @author JLyle (5/16/2018)
     */
    void Logo(unsigned int mask, const char *cpFmt, ...);


private:

    /**
     * Tells the process to stop new getting new data from the ICM. 
     * This is done by sending the stop command and then storing 
     * data that was trasmitted before the command was processed. 
     */
    bool m_closing;

    /**
     * The number of times a tick has been sent without receiving a 
     * tock message. 
     */
    int m_noTock;

    /**
     * Controls the timing of the TickTock Method so that it is not 
     * run all the time. 
     */
    void KeepAlive();

    DE10Nano *m_server;

    bool m_terminating;

};

#endif // _DE10NANO_H_INCLUDED_

