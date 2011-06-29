//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcRk512.h $
// $Author:: Rwiersem                                                         $
//   $Date:: 1/18/07 2:51p                                                    $
//
// Description:
//  Siemens serial PLC
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
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcRk512.h $
// 
// 1     1/18/07 2:51p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.

//******************************************************************************

#ifndef _PLCRK512_H_INCLUDED_
#define _PLCRK512_H_INCLUDED_

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
#include "IRk512Protocol.h" // Siemens Protocol Wrapper
#include "ILogicalPort.h"   // Logical port interface

/**
 * Data type passed to a Siemens PLC serial communication thread
 */
typedef struct
{
    /**
     * thread id
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
     * Object used to interface to the RK512 serial PLC
     */    
    IRk512Protocol              *plc;

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
     * PLC address to read our inputs from (i.e. $N17)
     */
    //std::string                 readAddress;
    //Siemens PLC uses data block instead of $N16 address
    uint8_t                      readAddress;

    /**
     * PLC address to write our inputs to (i.e. $N16)
     */
    //std::string                 writeAddress;
    //Siemens PLC uses data block instead of $N16 address
    uint8_t                      writeAddress;

} Rk512ThreadArg_t;

/**
 * Siemens Serial PLC protocol Board Object. Used to control a Siemens PLC board.
 *
 * @author
 * @version
 * @since
 */
class PlcRk512 : public IPlc
{
    friend class PlcRk512Test;

public:
    /**
     * Default constructor
     */ 
    PlcRk512();

    /**
     * Copy constructor
     *
     * @param copy   Object to be copied
     */
    PlcRk512( const PlcRk512 &copy);

    /**
     * Default destructor
     */
    ~PlcRk512();

    /**
     * Reads the channel at the given index
     *
     * @param channelIdx Channel index to read
     * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
     * @return Value of the channel at the given index
     */
    uint32_t ReadChannel( uint32_t channelIdx, portDir_t portDir=PORT_DIR_IN);
    
    /**
     * Opens a Siemens serial PLC board
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
    PlcRk512& operator=( const PlcRk512& rhs);

private:
    /**
     * Launches the plc serial comm thread (used to update PLC data arrays
     * via serial messages to and from the PLC). Also opens the logical
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
     * Thread entry point used to perform RK512 serial communication
     * 
     * @param threadArg Pointer to a Rk512ThreadArg_t data structure
     */
    static void* Rk512CommThread( void *threadArg);

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
    IRk512Protocol              m_plc;

    /**
     * Object used for serial communication
     * 
     * @see ILogicalPort
     */
    ILogicalPort                m_serCom;

    /**
     * Argument to pass to the RK512 serial comm thread
     */
    Rk512ThreadArg_t                m_rk512ThreadArg;

    /**
     * Name of the logical port to communicate with the PLC on
     */
    std::string                 m_rk512PortName;    

    /**
     * Number of milliseconds between consecutive PLC serial reads/writes
     */
    uint32_t                    m_commScanTime;
};

#endif      // _PLCRK512_H_INCLUDED_

