
class DE10Nano;

#ifndef _DE10NANO_H_INCLUDED_
#define _DE10NANO_H_INCLUDED_

#include <sys/mman.h>
#include <vector>
#include <string>
#include <stdint.h>
#include <stdio.h>

#include <netinet/in.h>
#include <sys/mman.h>

#if _NTO_VERSION >= 630
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

#include "IpIcmDefs.h"
#include "ILogger.h"	// Logger interface

// DE10Nano Configuration Information
typedef struct
{
	int SampleRate;   //Sample rate for the DE10Nano
	int NumChannels;  //Number of channels to read from the DE10Nano

} DE10NanoConf_t;

// DE10Nano Device information
typedef struct
{
	int VersionFPGA; //Firmware Version
	int VersionARM;  //Software Version
	DE10NanoConf_t config; //DE10Nano Configuration info

} DE10NanoInfo_t;



typedef struct {
	char *command; //String that holds the command to send to the DE10Nano
	bool *response; //true if the has responed to the last command
	int *position; //holds the position in the FIFO that the DE10Nano last wrote to

} ICMIPC;

typedef struct {
	uint32_t channels[MAX_CHANNELS];
	uint32_t index;
} ICMSample_t;

typedef struct
{
	/**
	 * Port that the DE10Nano connects to.
	 */
	int		m_portNum;

	/**
	 * how quickly the DE10Nano should latch and send the count. 
	 * Expressed in mSec. 
	 */
	int		m_sampleRate;
	int		m_delayRate;

	/**
	 * Number of channels that the DE10Nano is to transmit. 
	 */
	int		m_numChannels;

	/**
	 * True if a DE10Nano is connected to the port.
	 */
	bool 	m_connected;

	/**
	 * true if the DE10Nano is in the sending state.
	 */
	bool	m_sending;

} DE10Nano_t;

class DE10Nano : public ILogger, public DE10Nano_t
{
public:
	DE10Nano();
	~DE10Nano();
	FILE * outFile;
	int sockfd, newsockfd, fifoSize;
	ICMIPC IPC;
	ICMSample_t * sampler;
	DE10NanoInfo_t DE10NanoInfo;
	int m_samplerTail;
	

	/**
	 * Initializes our logger interface object
	 *
	 * @param loggerCfg The logger object whose configuration we want to copy
	 */
	void Initialize( const ILogger &loggerCfg);

	/**
	 * Attempts to initialize the DE10Nano Device on the given port
	 * number
	 *
	 * @param portNum The port that the DE10Nano will be trying to
	 *  			  connect to. 
	 *
	 * @return true if successfully initialized, false if an error occurrs
	 */
	bool Initialize( int portNum);

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

	bool SetPortNum(int inPortNum);

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


	bool ClearCount();

	void GetData();

protected:
	
	socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
	bool SendMsg(std::string message);
	void SaveData(uint32_t *data);
	uint32_t *data ;
	void Logo(unsigned int mask, const char *cpFmt, ...);


private:

	bool m_closing;
};

#endif // _DE10NANO_H_INCLUDED_

