//******************************************************************************
// Description:
//    Toyopuc PLC Object
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /CMake Core 062909/Ford/Source/Core/CORE/ServicesLayer/Managers/PlcMgr/PlcToyopuc.h $
// 
// 1     02/09/13 1:24p Rsmith
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _PLCTOYO_H_INCLUDED_
#define _PLCTOYO_H_INCLUDED_

//------------------------------------------------------------------------------
// SYSTEM HEADER FILES INCLUSIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// USER DEFINED HEADER FILE INCLUSIONS
//------------------------------------------------------------------------------
#include <pthread.h>
#include <string>

#include "IPlc.h"           // Base PLC interface definition
#include "ILogger.h"
#include "Krnl.h"
#include "Bepos.h"
#include "ResManager.h"     // For config tag strings
#include "PlcDefs.h"
#include "BepComponent.h"
#include "ToyopucProtocol.h"   // EIP Protocol Wrapper
//#include "ILogicalPort.h"   // Logical port interface
#include "SerialChannel.h"    // Interface via ethernet without AB wrapper
#include "INamedDataBroker.h"   // for SystemWrit()

class PlcToyopuc;

/**
 * Data type passed to a EIP serial communication thread
 */
typedef struct
{
    /**
     * Parent Toyoppuc plc object
     */
    PlcToyopuc                      *parent;

    /**
     * EIP thread id
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
     * Object used to interface to the Toyopuc serial PLC
     */
    ToyopucProtocol                *plc;

    /**
     * Connection on whcih the comm thread will be listening for termination
     * pulses (in order to shutdown gracefully).
     */
    int                         coId;

    /**
     * Object used to synchronize access to the data arrays
     */
    pthread_rwlock_t            *rwLock;

    /**
     * Number of nanoseconds to delya between consecutive PLC reads/writes
     */
    uint64_t                    nsecLoopTime;

    /**
     * Toyopuc source node ID for our system to use when sending Toyopuc messages to a PLC
     */
    uint8_t                     toyopucSourceId;

    /**
     * EIP destination node ID for the PLC with which we want to communicate
     */
    uint8_t                     toyopucDestId;

    /**
     * PLC address to read our inputs from (i.e. $N16)
     */
    std::string                 readAddress;

    /**
     * PLC address to read our inputs from (i.e. $N17)
     */
    std::string                 writeAddress;

    volatile bool               threadStop;

	string                      ipAddress;

} ToyopucThreadArg_t;

/**
 * Allen Bradley Serial Toyopuc protocol Board Object. Used to control an AB PLC board.
 *
 * @author Brian Meinke
 * @version 1.0
 * @since 12/11/2002
 */
class PlcToyopuc : public IPlc
{
    //friend class PlcToyopucTest; TESTING HA.  WHO NEEDS IT

public:
    /**
     * Default constructor
     */
    PlcToyopuc();

    /**
     * Copy constructor
     *
     * @param copy   Object to be copied
     */
    PlcToyopuc( const PlcToyopuc &copy);

    /**
     * Default destructor
     */
    virtual ~PlcToyopuc();

    /**
     * Reads the channel at the given index
     *
     * @param channelIdx Channel index to read
     * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
     * @return Value of the channel at the given index
     */
    uint32_t ReadChannel( uint32_t channelIdx, portDir_t portDir=PORT_DIR_IN);

    /**
     * Opens a Toyopuc serial PLC board
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
    PlcToyopuc& operator=( const PlcToyopuc& rhs);

protected:
    /**
     * Load the additional configuration items needed by the PlcToyopuc object.
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
     * @since Version 1.1
     */
    INT32 SystemWrite(const std::string &tag, const std::string &value);

private:
    /**
     * Launches the Toyopuc serial comm thread (used to update PLC data arrays
     * via serial messages to and from the Toyopuc PLC). Alos opens the logical
     * port we need to use for PLC communication and loads/initializes the
     * appropriate PLC DLL.
     *
     * @param portName Name of the logical port to communicate with the PLC on
     * @exception BepException
     */
    void LaunchCommThread( const std::string &portName) throw( BepException);

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
     * Thread entry point used to perform Toyopuc serial communication
     *
     * @param threadArg Pointer to a ToyopucThreadArg_t data structure
     */
    static void* ToyopucCommThread( void *threadArg);

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
    ToyopucProtocol                m_plc;

    /**
     * Object used for serial communication
     *
     * @see ILogicalPort
     */
    ILogicalPort                m_serCom;

    /**
     * Argument to pass to the Toyopuc serial comm thread
     */
    ToyopucThreadArg_t              m_toyopucThreadArg;

    /**
     * Name of the logical port to communicate with the PLC on
     */
    std::string                 m_toyopucPortName;

    /**
     * Number of milliseconds between consecutive PLC serial reads/writes
     */
    uint32_t                    m_commScanTime;

    INamedDataBroker            m_ndb;
};

#endif      // _PLCEIP_H_INCLUDED_
//==============================================================================
