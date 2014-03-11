//******************************************************************************
// Description:
//    Allen Bradley DF1 serial protocol wrapper class
//
//==============================================================================
//
// Copyright © 1999 Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/EipProtocol.h $
// 
// 5     5/24/06 2:55p Cward
// Updated PlcMgr to allow reloading of config files without rebooting.
//
// 1     5/16/06 1:17p Rwiersem
// Made the test class a friend.  Made the destructor virtual.  Added
// LoadAdditionalConfigurationItems() to support reloading the
// configuration without restarting the process.
//
// 4     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:15p Bmeinke
// Added m_terminate flag to signal the thread when it should stop
// Added an Initialize( XmlNode*) method to read config file data
// Changed to more closely resemble the NCA DF1 implementation
// Read() and Write() now return true/false
//
// 3     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     2/16/05 9:29p Cward
// UPdated DF1 Protocol with many fixes found in the field.
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/20/03 8:50a Khykin
// Updated for Ford changes.
//
// 1     4/30/03 4:44p Bmeinke
// Migration to new personalized configuration management structure
//
// 4     4/16/03 1:15p Bmeinke
// Added LogPlcString() method to output a PLC serial data string via the
// logger interface
// General cleanup: removed unused methods, removed commented out blocks
//
// 3     3/12/03 12:39a Bmeinke
// Initial machine testing for Ford Chicago
// Added a ILogger argument to Init() to initialize our logger interface
//
// 2     1/18/03 3:43p Bmeinke
// Maintenance check-in (not tested yet)
//
// 1     12/12/02 1:13p Bmeinke
// Initial check-in
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _EIP_PROTOCOL_H_INCLUDED_
#define _EIP_PROTOCOL_H_INCLUDED_

#include <fcntl.h>
#include <errno.h>
#include <atomic.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/neutrino.h>
#include <termios.h>

#include "ILogger.h"
#include "BepDefs.h"
#include "BepSync.h"
#include "EipWrapper.h"

#define     NULc    0x00        // Null
#define     STXc    0x02        // Start Of Text
#define     ETXc    0x03        // End Of Text
#define     EOTc    0x04        // End Of Transmission
#define     ENQc    0x05        // Enquire
#define     ACKc    0x06        // Acknowledge
#define     BELc    0x07        // Bell
#define     BSc     0x08        // Back Space
#define     LFc     0x0A        // Line Feed
#define     FFc     0x0C        // Form Feed
#define     CRc     0x0D        // Carraige Return
#define     DLEc    0x10
#define     DC1c    0x11        // Device Command 1
#define     XONc    0x11        // XON
#define     DC2c    0x12        // Device Command 2
#define     DC3c    0x13        // Device Command 3
#define     XOFFc   0x13        // XOFF
#define     DC4c    0x14        // Device Command 4
#define     NAKc    0x15        // Negative Acknowledge
#define     SUBc    0x1A        // ?clr_scr?
#define     EOFc    0x1A        // End Of File
#define     ESCc    0x1B        // Escape
#define     DELc    0x7F        // Delete

#define     SRCc    0x01        // VME is source
#define     DSTc    0x4F        // PLC is destination

#define     RDrq    0x4C        // AB read request
#define     RDrp    0xCC        // AB read reply
#define     WRTrq   0x4D        // AB write request
#define     WRTrp   0xCD        // AB write reply

#define     abBOOL  0xC1        // AB Boolean
#define     abSINT  0xC2        // AB 8b INT
#define     abINT   0xC3        // AB 16b INT
#define     abDINT  0xC4        // AB 32b INT
#define     abLINT  0xC5        // AB 64b INT
#define     abREAL  0xCA        // AB 32b FLOAT
#define     abDWRD  0xD3        // AB 32b Boolean array


/**
* Allen bradley EIP protcol wrapper
*
*/
class EipProtocol
{
	friend class EipProtocolTest;

public:
	/**
	 * Default constructor
	 */
	EipProtocol();

	/**
	 * Destructor
	 */
	virtual ~EipProtocol();

	/**
	 * Close an instance of a DF1 PLC
	 *
	 * @return EOK if successful. Any other value indicates error
	 */
	int Close();

	/**
	 * Initialize data for DF1 communication from a configuration node
	 *
	 * @param df1Config Configuration node to read the DF1 parameters from
	 */
	void Initialize( const XmlNode *df1Config);

	/**
	 * Read data from the PLC
	 * 
	 * @param dest
	 * @param buff      Buffer into which the data will be read
	 * @param count     Number of 32-bit integers to read
	 * @param ipAddress IP Address of the PLC to commuinicate with.
	 * @param addr      PLC tag to read data from (default = N17)
	 * 
	 * @return true if PLC data was read successfully
	 */
	bool Read(uint32_t *buff, uint32_t count, const string &ipAddress, const char *addr="N17");

	/**
	 * Write data to a DF1 PLC
	 * 
	 * @param buff      Buffer from which the data will be written
	 * @param count     Number of 32-bit integers to write
	 * @param ipAddress IP Address of the PLC to commuinicate with.
	 * @param addr      PLC tag to write data to (default = N16)
	 * 
	 * @return True if data was written successfully
	 */
	bool Write(const uint32_t *buff, uint32_t count, const string &ipAddress, const char *addr="N16");




protected:
	/**
	 * Load the additional configuration items needed by the EipProtocol
	 * object.
	 *
	 * @param df1Config Configuration node to read the DF1 parameters from
	 */
	virtual void LoadAdditionalConfigurationItems(const XmlNode *df1Config);




private:

	/**
	 * Transfers integer data to the PLC
	 * 
	 * @param byteCnt   Number of integers to transfer
	 * @param plcOut    Pointer to start of array of integers to be transferred
	 * @param plcIpAddr IP Address of the PLC to send the data to
	 * @param plcTag    PLC Tag (i.e. N16) into which the integers should be transferred
	 * 
	 * @return Number of bytes transmitted
	 */
	long BufferWrite(short byteCnt, const uint32_t *plcOut, const string &plcIpAddr, const string &plcTag);

	/**
	 * Read integer data from the PLC
	 * 
	 * @param byteCnt   Number of integers to read
	 * @param plcIpAddr PLC IP Address from which the data should be read
	 * @param plcTag    PLC data tag to read.
	 * @param plcData   Data read from the PLC.
	 * 
	 * @return Number of integers read
	 */
	long BufferRead(short byteCnt, const string &plcIpAddr, const string &plcTag, uint32_t *plcData);

	/**
	 * Combine the PLC data into words
	 * 
	 * @param rawData   DF1 serial string to extract data from
	 * @param byteCount Number of bytes read from the PLC
	 * @param plcData   Array into which converted data will be written
	 * @param dataCount Number of elements in the plcData array
	 * 
	 * @return Number of 32-bit integers converted and written to the plcData array
	 */
	int ExtractData( uint8_t *rawData, int byteCount, uint32_t *plcData, int dataCount);

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

	// Mutex used to lock EIP operations
	BepMutex                m_df1Lock;

	// Flag to indicate if it is time to terminate
	volatile bool           m_terminate;

	// Ethernet/IP communication object
	EipWrapper* m_eipComm;
};

#endif // _EIP_PROTOCOL_H_INCLUDED_

