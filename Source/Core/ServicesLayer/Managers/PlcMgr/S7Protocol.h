//******************************************************************************
// Description:
//    Siemens S7 protocol wrapper class
//
//==============================================================================
//
// Copyright © 2015 Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef S7Protocol_h
#define S7Protocol_h
//-------------------------------------------------------------------------------------------------
#include "ILogger.h"
#include "BepDefs.h"
#include "BepSync.h"
#include "./S7/release/Wrappers/c-cpp/snap7.h"
//-------------------------------------------------------------------------------------------------
class S7Protocol
{
public:
	S7Protocol();
	virtual ~S7Protocol();

	/** 
	 * Close the connection to the PLC
	 * 
	 * @return  EOK if successful.
	 */
	int Close();

	/**
	 * Initialize the data for S7 communications.
	 * 
	 * @param config Communication config data.
	 */
	void Initialize(const XmlNode *config);

	/**
	 * Read data from the PLC
	 * 
	 * @param buff      Buffer into which the data will be read
	 * @param count     Number of 16-bit integers to read
	 * @param dbNum      PLC tag to read data from (default = DB1)
	 * 
	 * @return true if PLC data was read successfully
	 */
	bool Read(uint32_t *buff, uint32_t count, int dbNum=1);

	/**
	 * Write data to the PLC
	 * 
	 * @param buff      Buffer from which the data will be written
	 * @param count     Number of 16-bit integers to write
	 * @param addr      PLC tag to write data to (default = DB1)
	 * 
	 * @return True if data was written successfully
	 */
	bool Write(const uint32_t *buff, uint32_t count, int dbNum=1);





private:

	/**
	 * Combine the PLC data into words
	 * 
	 * @param rawData   S7 byte data
	 * @param byteCount Number of bytes read from the PLC
	 * @param plcData   Array into which converted data will be written
	 * @param dataCount Number of elements in the plcData array
	 * 
	 * @return Number of words converted and written to the plcData array
	 */
	int ExtractData(byte rawData[], int byteCount, uint32_t *plcData, int dataCount);

	/**
	 * Logs a PLC response string
	 *
	 * @param logMask Our logger interface verbosity bit which must be set in order to log
	 *                the string
	 * @param prefix  String to prepend to the PLC data string when logged
	 * @param plcData The PLC data string to be logged
	 * @param dataLen The length of the PLC data string
	 */
	void LogPlcString( uint32_t logMask, const char *prefix, const uint8_t *plcData, const uint32_t dataLen);

	// Object used to print/log data
	ILogger                 m_logger;

	// Mutex used to lock S7 operations
	BepMutex                m_s7Lock;

	// Flag to indicate if it is time to terminate
	volatile bool           m_terminate;

	// Object to interface with the Siemens PLC
	TS7Client               *m_s7Comm;

	// IP Address of the PLC
	string                  m_ipAddress;
};
//-------------------------------------------------------------------------------------------------
#endif //S7Protocol_h
