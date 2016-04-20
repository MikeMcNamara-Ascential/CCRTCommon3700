//******************************************************************************
// Description:
//    Siemens S7 PLC Object
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
#ifndef PlcS7_h
#define PlcS7_h
//-------------------------------------------------------------------------------------------------
#include <pthread.h>
#include <string>

#include "IPlc.h"           // Base PLC interface definition
#include "ILogger.h"
#include "Krnl.h"
#include "Bepos.h"
#include "ResManager.h"     // For config tag strings
#include "PlcDefs.h"
#include "BepComponent.h"
#include "S7Protocol.h"
#include "ILogicalPort.h"   // Logical port interface
#include "INamedDataBroker.h"
//-------------------------------------------------------------------------------------------------
class PlcS7;

typedef struct
{
	PlcS7                       *parent;
	pthread_t                   threadId;
	ILogger                     *logger;

	volatile uint32_t           *fromPlc;
	uint32_t                    fromPlcCount;
	volatile uint32_t const     *toPlc;
	uint32_t                    toPlcCount;

	pthread_barrier_t           barrier;

	S7Protocol                  *plc;

	int                         connectionId;
	pthread_rwlock_t            *rwLock;
	uint64_t                    nsecLoopTime;

	int                         fromPlcDbNum;
	int                         toPlcDbNum;

	int                         fromPlcStartIndex;
	int                         toPlcStartIndex;

	volatile bool               threadStop;

	string                      ipAddress;
} S7ThreadArg_t;

class PlcS7 : public IPlc
{
public:
	PlcS7();
	PlcS7(const PlcS7 &copy);
	virtual ~PlcS7();


	/**
	 * Opens a S7 PLC connection
	 *
	 * @param inputCount Number of inputs from the PLC we need to read
	 * @param outputCount
	 *                   Number of outputs to the PLC we need to write
	 * @param demoMode   Optional demo mode flag. If this argument is true, the Open() will
	 *                   ALWAYS succeed
	 *
	 * @return true if plc connection successfully opened
	 *         false if any errors occur
	 */
	bool Open(uint32_t inputCount, uint32_t outputCount, bool demoMode=false);

	/**
	 * Initializes the underlying PLC object
	 *
	 * @param plcSetup XML Parent Node containing PLCBoard configuration
	 */
	void Initialize(const XmlNode *plcSetup);

	/**
	 * Reads the input channel configuration from the given node map
	 *
	 * @param inputNodes XmlNodeMap containing the setup nodes for the PLC inputs
	 */
	void ReadInputConfiguration(const XmlNodeMap &inputNodes);

	/**
	 * Reads the output channel configuration from the given node map
	 *
	 * @param outputNodes
	 *               XmlNodeMap containing the setup nodes for the PLC outputs
	 */
	void ReadOutputConfiguration(const XmlNodeMap &outputNodes);

	/**
	 * Closes the object
	 */
	void Close();

	/**
	 * Reads the channel at the given index
	 *
	 * @param channelIdx Channel index to read
	 * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
	 * @return Value of the channel at the given index
	 */
	uint32_t ReadChannel( uint32_t channelIdx, portDir_t portDir=PORT_DIR_IN);

	/**
	 * Reads from specified port on the channel at the given index
	 *
	 * @param channelIdx Channel number to read
	 * @param portIdx    Port (bit) index to read to
	 * @param logicLvl   Logic level for the specified port (positive true=1)
	 * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
	 * @return 1 if port is "true"
	 *         0 if port is "off"
	 */
	uint32_t ReadPort(uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, portDir_t portDir=PORT_DIR_IN);

	/**
	 * Writes to the the channel at the given index
	 *
	 * @param channelIdx Channel number to write to
	 * @param channelVal Value to write to the given channel
	 */
	void WriteChannel(uint32_t channelIdx, uint32_t channelVal);

	/**
	 * Writes to specified port on the channel at the given index
	 *
	 * @param channelIdx Channel number to write to
	 * @param portIdx    Port (bit) index to write to
	 * @param logicLvl   Logic level for the specified port (positive true=1)
	 * @param portVal    Value to write to the given port
	 */
	void WritePort(uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, uint32_t portVal);

	/**
	 * Assignment operator. The newly copied object WILL NOT be opened automatically
	 *
	 * @param rhs    Object to be copied
	 * @return this
	 */
	PlcS7& operator=( const PlcS7& rhs);




protected:
	/**
	 * Load the additional configuration items needed by the S7 PLC object.
	 *
	 * @param plcSetup XML Parent Node containing PLCBoard configuration
	 */
	virtual void LoadAdditionalConfigurationItems(const XmlNode *plcSetup);

	/**
	 * Write a system tag to a specific value.
	 *
	 * @param tag    The data to write.
	 * @param value  The value to write to the system.
	 * @return The status of the write.
	 */
	INT32 SystemWrite(const string &tag, const string &value);




private:
	/**
	 * Launches the S7 comm thread (used to update PLC data arrays
	 * via messages to and from the Siemens PLC). Also opens the logical
	 * port we need to use for PLC communication and loads/initializes the
	 * appropriate PLC DLL.
	 *
	 * @param ipAddress IP Address to communicate with the PLC on
	 * @exception BepException
	 */
	void LaunchCommThread(void) throw( BepException);

	/**
	 * Allocates memory for the data arrays used to hold the PLC data
	 */
	void CreateDataArrays();

	/**
	 * De-allocates memory for the data arrays used to hold the PLC data
	 */
	void DestroyDataArrays();

	/**
	 * Expands environment variable names in a path name
	 *
	 * @param pathToResolve
	 *               Path name to be resolved
	 */
//	void ResolvePathName( string &pathToResolve);

	/**
	 * Thread entry point used to perform Siemens S7 communication
	 *
	 * @param threadArg Pointer to a S7ThreadArg_t data structure
	 */
	static void* S7CommThread( void *threadArg);

	/**
	 * Inputs from PLC
	 */
	volatile uint32_t           *m_fromPlc;

	/**
	 * Outputs to PLC
	 */
	volatile uint32_t           *m_toPlc;

	/**
	 * Object used to guard read/write operations
	 */
	mutable pthread_rwlock_t    m_plcLock;

	/**
	 * Object used to load and interface with the PLC dll
	 */
	S7Protocol                  m_plc;

	/**
	 * Argument to pass to the S7 comm thread
	 */
	S7ThreadArg_t              m_s7ThreadArg;

	/**
	 * Number of milliseconds between consecutive PLC serial reads/writes
	 */
	uint32_t                    m_commScanTime;

	INamedDataBroker            *m_ndb;
};
//-------------------------------------------------------------------------------------------------
#endif //PlcS7_h
