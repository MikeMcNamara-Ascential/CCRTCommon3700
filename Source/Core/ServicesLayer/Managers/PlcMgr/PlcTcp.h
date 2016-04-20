//******************************************************************************
//
// Description:
//  Allen Bradley TCP serial PLC
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcTcp.h $
// 
// 3     5/24/06 2:55p Cward
// Updated PlcMgr to allow reloading of config files without rebooting.
//
// 1     5/16/06 1:18p Rwiersem
// Made the test class a friend.  Made the destructor virtual.  Added
// LoadAdditionalConfigurationItems() to support reloading the
// configuration without restarting the process.
//
// 2     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:18p Bmeinke
// ITcpProtocol::Read() and ITcpProtocol::Write() now return true/false
// Better thread termination logic
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
// 1     4/30/03 4:42p Bmeinke
// Migration to new personalized configuration management structure
//
// 3     3/12/03 12:42a Bmeinke
// Initial machine testing for Ford Chicago
// No longer read the input/output channel count from the config file:
// they are passed as arguments to Open()
// Read() and Write() now take an additional PLC address argument
//
// 2     1/18/03 3:31p Bmeinke
// Changed from using shared library objects to source level C++ PLC
// objects
//
// 1     12/11/02 9:43p Bmeinke
// Initial check-in
//
//******************************************************************************

#ifndef _PLCTCP_H_INCLUDED_
#define _PLCTCP_H_INCLUDED_

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
#include "ITcpProtocol.h"   // TCP Protocol Wrapper
#include "ILogicalPort.h"   // Logical port interface
#include "INamedDataBroker.h"   // for SystemWrit()

class PlcTcp;

/**
 * Data type passed to a TCP serial communication thread
 */
typedef struct
{
    /**
     * Parent TCP plc object
     */
    PlcTcp                      *parent;

    /**
     * TCP thread id
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
     * Object used to interface to the TCP serial PLC
     */
    ITcpProtocol                *plc;

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
     * TCP source node ID for our system to use when sending TCP messages to a PLC
     */
    uint8_t                     tcpSourceId;

    /**
     * TCP destination node ID for the PLC with which we want to communicate
     */
    uint8_t                     tcpDestId;

    /**
     * PLC address to read our inputs from (i.e. $N16)
     */
    std::string                 readAddress;

    /**
     * PLC address to read our inputs from (i.e. $N17)
     */
    std::string                 writeAddress;

    volatile bool               threadStop;

} TcpThreadArg_t;

/**
 * Allen Bradley Serial TCP protocol Board Object. Used to control an AB PLC board.
 *
 * @author Brian Meinke
 * @version 1.0
 * @since 12/11/2002
 */
class PlcTcp : public IPlc
{
    friend class PlcTcpTest;

public:
    /**
     * Default constructor
     */
    PlcTcp();

    /**
     * Copy constructor
     *
     * @param copy   Object to be copied
     */
    PlcTcp( const PlcTcp &copy);

    /**
     * Default destructor
     */
    virtual ~PlcTcp();

    /**
     * Reads the channel at the given index
     *
     * @param channelIdx Channel index to read
     * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
     * @return Value of the channel at the given index
     */
    uint32_t ReadChannel( uint32_t channelIdx, portDir_t portDir=PORT_DIR_IN);

    /**
     * Opens a TCP serial PLC board
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
    PlcTcp& operator=( const PlcTcp& rhs);

    /**
     * Attempts to close and reopen the TCP comm port
     * 
     */
    void ResetTcpComms(void) throw( BepException);

    /**
     * Flushes read and write buffers
     * 
     */
    void FlushBuffers(void);

    /**
     * Returns the read/write comm fault limit
     * 
     */
    const INT32 GetCommFaultLimit(void);

protected:
    /**
     * Load the additional configuration items needed by the PlcTcp object.
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
     * Launches the TCP serial comm thread (used to update PLC data arrays
     * via serial messages to and from the TCP PLC). Alos opens the logical
     * port we need to use for PLC communication and loads/initializes the
     * appropriate PLC DLL.
     *
     * @param portName Name of the logical port to communicate with the PLC on
     * @exception BepException
     */
    void LaunchCommThread( const std::string &portName, const std::string &portNameRead, const std::string &portNameWrite) throw( BepException);

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
     * Thread entry point used to perform TCP serial communication
     *
     * @param threadArg Pointer to a TcpThreadArg_t data structure
     */
    static void* TcpCommThread( void *threadArg);

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
    ITcpProtocol                m_plc;

    /**
     * Object used for serial communication
     *
     * @see ILogicalPort
     */
    ILogicalPort                m_serCom;

    /**
     * Object used for serial communication
     *
     * @see ILogicalPort
     */
    ILogicalPort                m_serCom2;

    /**
     * Argument to pass to the TCP serial comm thread
     */
    TcpThreadArg_t              m_tcpThreadArg;

    /**
     * Name of the logical port to communicate with the PLC on
     */
    std::string                 m_tcpPortName;
    std::string                 m_tcpPortNameRead;
    std::string                 m_tcpPortNameWrite;

    /**
     * Number of milliseconds between consecutive PLC serial reads/writes
     */
    uint32_t                    m_commScanTime;
    INT32                       m_commRetryInterval;
    INT32                       m_commFaultLimit;

    INamedDataBroker            m_ndb;

    /**
     * Flag indicating if we should use one port or two ports
     */
    bool                        m_useSeparatePorts;
};

#endif      // _PLCTCP_H_INCLUDED_

