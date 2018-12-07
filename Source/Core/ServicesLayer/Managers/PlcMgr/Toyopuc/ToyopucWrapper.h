//*************************************************************************
// FILE:
//
//
// FILE DESCRIPTION:
//      Ethernet/IP Wrapper Class
//
//===========================================================================
// Copyright (c) 2007- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//
// 
//*************************************************************************
#ifndef TOYOPUCWRAPPER_h
#define TOYOPUCWRAPPER_h
//*************************************************************************

#include <stdio.h>
#include <pthread.h>
#include "ILogger.h"
/*extern "C" Will be using a Serial Channel instead for this to communicate via TcpCommProxy
{
    #include "Eip/platform.h"
    #include "Eip/eipinc.h"
} 
*/ 

#define     abBOOL  0xC1        // AB Boolean
#define     abSINT  0xC2        // AB 8b INT
#define     abINT   0xC3        // AB 16b INT
#define     abDINT  0xC4        // AB 32b INT
#define     abLINT  0xC5        // AB 64b INT
#define     abREAL  0xCA        // AB 32b FLOAT
#define     abDWRD  0xD3        // AB 32b Boolean array

/**
 * Ethernet/IP Wrapper Class.
 * 
 * 
 */
class ToyopucWrapper
{
public:
    /**
     * Class constructor.
     * 
     */
    ToyopucWrapper(ILogger *logger);
    /**
     * Class Destructor.
     * 
     */
    ~ToyopucWrapper();
    /**
     * Initialize Toyopuc communication link
     * 
     */
    bool Initialize(void);

	/**
	 * Send data to PLC
	 * 
	 * @param plcData Data to send to the PLC.
	 * @param byteCnt Number of bytes to send.
	 * @param plcIpAddress
	 *                IP Address of the PLC.
	 * @param plcTag  Tag in the PLC to write to.
	 * 
	 * @return Flag indicating if the data was successfully sent to the PLC.
	 */
    bool SendPlcData(const uint8_t *plcData, short byteCnt, const string &plcIpAddress, string plcTag);

	/**
	 * Read the data from the PLC
	 * 
	 * @param plcData   Data read from the PLC.
	 * @param bytesToRead
	 *                  Number of bytes to read from the PLC.
	 * @param bytesRead Number of bytes actually read from the PLC.
	 * @param plcIpAddress
	 *                  IP Address of the PLC.
	 * @param plcTag    Tag in the PLC to read data from.
	 * 
	 * @return Status of reading the data from the PLC.  Successful read will return 0.
	 */
    INT32 ReceivePlcData(uint8_t *plcData, const short &bytesToRead, INT32 &bytesRead, 
						 const string &plcIpAddress, const string &plcTag);

	/**
     *  Close connections 
     * 
     */
    void Close(void);




private:

	/**
	 * Toyopuc thread to moinitor for incoming messages.
	 * 
	 * @param param
	 * 
	 * @return 
	 */
	static void *EipTask(void *param);

	int m_toyoThreadId;

	// Flag to use to terminate the Eip Task thread
	bool m_terminate;

	/**
     * Object used to print/log data
     */
    ILogger *m_logger;
};

#endif  //EIPWRAPPER_h
