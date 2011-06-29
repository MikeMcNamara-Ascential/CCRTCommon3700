//******************************************************************************
//
// Description:
//	Mitsubishi Melsec serial PLC
//
//==============================================================================
//
//     Copyright © 2005 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
// 
//******************************************************************************
#ifndef _PLCMELSEC_H_INCLUDED_
#define _PLCMELSEC_H_INCLUDED_

//------------------------------------------------------------------------------
// SYSTEM HEADER FILES INCLUSIONS
//------------------------------------------------------------------------------
#include <pthread.h>
#include <string>

//------------------------------------------------------------------------------
// USER DEFINED HEADER FILE INCLUSIONS
//------------------------------------------------------------------------------
#include "IPlc.h"			           // Base PLC interface definition
#include "ILogger.h"
#include "Krnl.h"
#include "Bepos.h"
#include "ResManager.h"		           // For config tag strings
#include "PlcDefs.h"
#include "BepComponent.h"
#include "IMelsecProtocol.h"	       // Melsec Protocol Wrapper
#include "ILogicalPort.h"	           // Logical port interface

/**
 * Data type passed to a Melsec serial communication thread
 */
typedef struct
{
    /**
     * Melsec thread id
     */
    pthread_t                   threadId;

    /**
     * Object to use when logging data
     */
    ILogger                     *logger;

    /**
     * Buffer to use when reading our inputs from the PLC
     */
    volatile uint32_t           *fromPlc;

    /**
     * Number of data items to read from the PLC
     */
    uint32_t                    fromCount;

    /**
     * Buffer to use when writing our outputs to the PLC
     */
    volatile uint32_t const     *toPlc;

    /**
     * Number of data items to write to the PLC
     */
    uint32_t                    toCount;

    /**
     * Barrier used to synchronize the comm thread startup. The thread which 
     * launches the comm thread will wait at this barrier until the comm thread
     * has finished initialization. The comm thread will wait at the barrier 
     * after successful initialization. When both threads are waiting on the 
     * barrier, the kernel will unblock both threads and they will resume
     * their respective operations. If any errors occur during the comm 
     * thread's initialization, the comm thread will destory the barrier and 
     * exit which will subsequently unblock the comm thread's parent.
     */
    pthread_barrier_t           barrier;

    /**
     * Object used to interface to the Melsec serial PLC
     */
    IMelsecProtocol             *plc;

    /**
     * Connection on which the comm thread will be listening for termination
     * pulses (in order to shutdown gracefully).
     */
    int                         coId;

    /**
     * Object used to synchronize access to the data arrays
     */
    pthread_rwlock_t            *rwLock;

    /**
     * Number of nanoseconds to delay between consecutive PLC reads/writes
     */
    uint64_t                    nsecLoopTime;

} MelsecThreadArg_t;

/**
 * Melsec Serial protocol Object. Used to control a Melsec PLC.
 */
class PlcMelsec : public IPlc
{
public:
    /**
     * Default constructor
     */
    PlcMelsec();

    /**
     * Copy constructor
     *
     * @param copy   Object to be copied
     */
    PlcMelsec( const PlcMelsec &copy);

    /**
     * Default destructor
     */
    ~PlcMelsec();

    /**
     * Reads the channel at the given index
     *
     * @param channelIdx Channel index to read
     * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
     * 
     * @return Value of the channel at the given index
     */
    uint32_t ReadChannel( uint32_t channelIdx, portDir_t portDir=PORT_DIR_IN);

    /**
     * Opens a Melsec serial PLC object
     * 
     * @param inputCount Number of inputs from the PLC we need to read
     * @param outputCount
     *                   Number of outputs to the PLC we need to write
     * @param demoMode   Optional demo mode flag. If this argument is true, the Open() will
     *                   ALWAYS succeed
     * 
     * @return true if board successfully opened
     *         false if any errors occur
     */
    bool Open(uint32_t inputCount, uint32_t outputCount, bool demoMode=false);

    /**
     * Initializes the underlying PLC object
     *
     * @param plcSetup XML Parent Node containing PLC configuration
     */
    void Initialize(const XmlNode *plcSetup);

    /**
     * Closes the object
     */
    void Close();

    /**
     * Reads from specified port on the channel at the given index
     *
     * @param channelIdx Channel number to read
     * @param portIdx    Port (bit) index to read to
     * @param logicLvl   Logic level for the specified port (positive true=1)
     * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
     * 
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
     * 
     * @return this
     */
    PlcMelsec& operator=( const PlcMelsec& rhs);

private:
    /**
     * Launches the Melsec serial comm thread (used to update PLC data arrays
     * via serial messages to and from the Melsec PLC). Also opens the logical
     * port we need to use for PLC communication and loads/initializes the
     * appropriate PLC DLL.
     * 
     * @param portName Name of the logical port to communicate with the PLC on
     * 
     * @exception BepException
     */
    void LaunchCommThread( const std::string &portName) throw( BepException);

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
    void ResolvePathName( string &pathToResolve);

    /**
     * Thread entry point used to perform Melsec serial communication
     * 
     * @param threadArg Pointer to a MelsecThreadArg_t data structure
     */
    static void* MelsecCommThread( void *threadArg);

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
    IMelsecProtocol             m_plc;

    /**
     * Object used for serial communication
     * 
     * @see ILogicalPort
     */
    ILogicalPort                m_serCom;

    /**
     * Argument to pass to the Melsec serial comm thread
     */
    MelsecThreadArg_t           m_melsecThreadArg;

    /**
     * Name of the logical port to communicate with the PLC on
     */
    std::string                 m_melsecPortName;

    /**
     * Number of milliseconds between consecutive PLC serial reads/writes
     */
    uint32_t                    m_commScanTime;

    /**
     * The base address for Melsec PLC reads
     */
    std::string                 m_inputAddress;

    /**
     * The base address for Melsec PLC reads
     */
    std::string                 m_outputAddress;
};

#endif		// _PLCMELSEC_H_INCLUDED_

