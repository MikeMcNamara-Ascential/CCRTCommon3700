//******************************************************************************
//
// Description:
//    Melsec PLC Object.
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

/*******************
 include files
 ******************/
#include <errno.h>
#include <sync.h>           // BARRIER_SERIAL_THREAD
#include <sys/netmgr.h>     // ND_LOCAL_NODE
#include "PlcMelsec.h"
#include "BepException.h"

/**
 * Default constructor
 */
PlcMelsec::PlcMelsec() : IPlc(),
m_fromPlc( NULL),  m_toPlc( NULL), m_melsecPortName( ""),
m_commScanTime( 20)
{
    // Initialize the read/write lock
    pthread_rwlock_init( &m_plcLock, NULL);
    m_channelBits = 16;
}

/**
 * Copy constructor
 *
 * @param copy   Object to be copied
 */
PlcMelsec::PlcMelsec( const PlcMelsec &copy) : IPlc( copy),
m_fromPlc( NULL),  m_toPlc( NULL), m_melsecPortName( ""),
m_commScanTime( 20)
{
    // Initialize the read/write lock
    pthread_rwlock_init( &m_plcLock, NULL);

    *this = copy;
}

/**
 * Default destructor
 */
PlcMelsec::~PlcMelsec()
{
    Log( LOG_FN_ENTRY, "Enter PlcMelsec::~PlcMelsec\n");
    Close();

    // Destroy the read/write lock
    pthread_rwlock_destroy( &m_plcLock);
    Log( LOG_FN_ENTRY, "Exit PlcMelsec::~PlcMelsec\n");
}

/**
 * Opens a Melsec serial PLC board
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
bool PlcMelsec::Open(uint32_t inputCount, uint32_t outputCount, bool demoMode)
{
    bool    retVal = true;

    Log( LOG_FN_ENTRY, "Enter PlcMelsec::Open()\n");

    Log( LOG_DEV_DATA, "inputCount: %d  outputCount: %d",inputCount, outputCount);
    m_fromPlcCount = inputCount;
    m_toPlcCount = outputCount;

    CreateDataArrays();

    try
    {
        // Launch the Melsec comm thread
        LaunchCommThread( m_melsecPortName);
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "\tError launching Melsec comm thread: %s\n",
             err.GetReason());
        retVal = false;
    }
    Log( LOG_FN_ENTRY, "Exit PlcMelsec::Open()\n");

    return( retVal);
}

/**
 * Initializes the underlying PLC object
 *
 * @param plcSetup XML Parent Node containing PLCBoard configuration
 */
void PlcMelsec::Initialize( const XmlNode *plcSetup)
{
    Log( LOG_FN_ENTRY, "Enter PlcMelsec::Initialize( %s)\n", plcSetup->getName().c_str());

    // Initialize our logger interface
    try
    {
        ILogger::Initialize( plcSetup);
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tXML Exception in ILogger::Initialize(): %s\n",
             err.GetReason());
    }

    // Read the input configuration
    try
    {
        // Read input configuration
        ReadInputConfiguration( plcSetup->getChild( XML_T( "Inputs"))->getChildren());
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "No Input configuration specified in config file\n");
        m_inputAddress = "D0050";  // So set the default
        Log( LOG_ERRORS, "Defaulted to D0050\n");
    }

    // Read the output configuration
    try
    {
        // Read output configuration
        ReadOutputConfiguration( plcSetup->getChild( XML_T( "Outputs"))->getChildren());
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "No Output configuration specified in config file\n");
        m_outputAddress = "D0000";  // So set the default
        Log( LOG_ERRORS, "Defaulted to D0000\n");
    }

    // Read the name of the logical port to talk on
    try
    {
        const XmlString& tmp = plcSetup->getChild( XML_T("LogicalPort"))->getValue();
        m_melsecPortName = XmlToAscii( tmp);
    }
    catch( XmlException &err)
    {
        m_melsecPortName = "PlcMelsec";
    }
    Log( LOG_DEV_DATA, "Melsec PLC Logical Port Name: %s\n", m_melsecPortName.c_str());

    // Read the serial comm interval
    try
    {
        const XmlString& tmp = plcSetup->getChild( XML_T("ScanTime"))->getValue();
        m_commScanTime = BposReadInt( XmlToAscii( tmp).c_str());
    }
    catch( XmlException &err)
    {
        m_commScanTime = 20;
    }
    Log( LOG_DEV_DATA, "Melsec PLC Scan Time: %d ms\n", m_commScanTime);

    Log( LOG_FN_ENTRY, "Exit PlcMelsec::Initialize()\n");
}

/**
 * Launches the Melsec serial comm thread (used to update PLC data arrays
 * via serial messages to and from the Melsec PLC). Also opens the logical
 * port we need to use for PLC communication and loads/initializes the
 * appropriate PLC DLL.
 *
 * @param portName Name of the logical port to communicate with the PLC on
 */
void PlcMelsec::LaunchCommThread( const std::string &portName) throw( BepException)
{
    std::string errStr;
    int         status;

    Log( LOG_FN_ENTRY, "Enter PlcMelsec::LaunchCommThread()\n");

    // Try to open the serial prot
    if( m_serCom.OpenPort( portName, O_RDWR) == false)
    {
        errStr = "Unable to open Melsec logical port: ";
        errStr += strerror( errno);
        throw( BepException( errStr));
    }
    else
    {
        // Flush FIFOs at startup
        m_serCom.ResetConnection();

        try
        {
            m_plc.Init( *this);
            if( (status=m_plc.Open( m_serCom.GetPortFd(), m_outputAddress, m_inputAddress)) != EOK)
            {
                Log( LOG_ERRORS, "\tm_plc.PlcOpen() failed: %s\n", strerror(status));
                throw( BepException( "Unable to initialize Melsec DLL: %s"), strerror(status) );
            }

            pthread_barrier_init( &m_melsecThreadArg.barrier, NULL, 2);
            m_melsecThreadArg.fromPlc = m_fromPlc;  
            m_melsecThreadArg.fromCount = m_fromPlcCount;
            m_melsecThreadArg.toPlc = m_toPlc;      
            m_melsecThreadArg.toCount = m_toPlcCount;
            m_melsecThreadArg.plc = &m_plc;
            m_melsecThreadArg.logger = (ILogger *)this;
            m_melsecThreadArg.rwLock = (pthread_rwlock_t*)&m_plcLock;
            m_melsecThreadArg.nsecLoopTime = mSEC_nSEC( m_commScanTime);

            pthread_create( &m_melsecThreadArg.threadId, NULL, 
                            &PlcMelsec::MelsecCommThread, &m_melsecThreadArg);
            // Wait for serial comm thread to start
            status = pthread_barrier_wait( &m_melsecThreadArg.barrier);
            if( (status != BARRIER_SERIAL_THREAD) && (status != 0))
            {
                errStr = "Melsec comm thread init error: ";
                errStr += strerror( status);
                throw( BepException( errStr));
            }
            Log( LOG_DEV_DATA, "Melsec comm thread launched with thread id of %d\n",
                 m_melsecThreadArg.threadId);
            // Wait for the first scan to complete
            delay( m_commScanTime*2);
        }
        catch( BepException &err)
        {
            Log( LOG_ERRORS, "\tError initializing Melsec PLC DLL: %s\n", err.GetReason());
            throw;
        }
    }

    Log( LOG_FN_ENTRY, "Exit PlcMelsec::LaunchCommThread()\n");
}

/**
 * Reads the input channel configuration from the given node map
 *
 * @param inputNodes XmlNodeMap containing the setup nodes for the PLC inputs
 */
void PlcMelsec::ReadInputConfiguration( const XmlNodeMap &inputNodes)
{
    Log( LOG_FN_ENTRY, "Enter PlcMelsec::ReadInputConfiguration()\n");
    
    try
    {
        const XmlNode *addressNode = inputNodes.getNode( "Address");
        m_inputAddress = XmlToAscii( addressNode->getValue());
    }
    catch( XmlException &err)
    {
        m_inputAddress = "D0050";
        Log( LOG_DEV_DATA, "Using default Input Address");
    }
    Log( LOG_DEV_DATA, "Input Address: %s\n", m_inputAddress.c_str());
    
    Log( LOG_FN_ENTRY, "Exit PlcMelsec::ReadInputConfiguration()\n");
}


/**
 * Reads the output channel configuration from the given node map
 *
 * @param outputNodes
 *               XmlNodeMap containing the setup nodes for the PLC outputs
 */
void PlcMelsec::ReadOutputConfiguration( const XmlNodeMap &outputNodes)
{
    Log( LOG_FN_ENTRY, "Enter PlcMelsec::ReadOutputConfiguration()\n");
        
    // Check for output channel PLC address
    try
    {
        const XmlNode *addressNode = outputNodes.getNode( "Address");
        m_outputAddress = XmlToAscii( addressNode->getValue());
    }
    catch( XmlException &err)
    {
        m_outputAddress = "D0000";
        Log( LOG_DEV_DATA, "Using default Output Address");
    }
    Log( LOG_DEV_DATA, "Output Address: %s\n", m_outputAddress.c_str());
    
    Log( LOG_FN_ENTRY, "Exit PlcMelsec::ReadOutputConfiguration()\n");
}

/**
 * Closes the object
 */
void PlcMelsec::Close()
{
    Log( LOG_FN_ENTRY, "Enter PlcMelsec::Close\n");

    // If comm thread is running
    if( m_melsecThreadArg.threadId != -1)
    {
        Log( LOG_FN_ENTRY, "Terminating comm thread\n");

        // Send comm thread a terminate pulse to shut it down
        MsgSendPulse( m_melsecThreadArg.coId, -1, SYSTEM_PULSE, TERMINATE_PULSE);

        // Wait for the thread to die
        pthread_join( m_melsecThreadArg.threadId, NULL);

        Log( LOG_FN_ENTRY, "Comm thread dead\n");
    }
    m_plc.Close();

    // De-allocate data array memory
    DestroyDataArrays();

    Log( LOG_FN_ENTRY, "Exit PlcMelsec::Close\n");
}

/**
 * Reads the channel at the given index
 *
 * @param channelIdx Channel index to read
 * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
 * 
 * @return Value of the channel at the given index
 */
uint32_t PlcMelsec::ReadChannel( uint32_t channelIdx, portDir_t portDir)
{
    uint32_t            retVal;
    volatile uint32_t   *plcMem;
    int                 errVal;

    // Choose the proper buffer to read from
    if( portDir == PORT_DIR_IN) plcMem = m_fromPlc;
    else                        plcMem = m_toPlc;

    if( plcMem != NULL)
    {
        // Lock for read
        if( (errVal=pthread_rwlock_rdlock( &m_plcLock)) == EOK)
        {
            if( (channelIdx < m_fromPlcCount) && (portDir == PORT_DIR_IN) ||
                (channelIdx < m_toPlcCount) && (portDir == PORT_DIR_OUT))
            {
                // Read data from the specified port
                retVal = plcMem[ channelIdx];
            }
            else
            {
                Log( LOG_ERRORS, "\tAttempt to read from invalid channel #%d\n", channelIdx);
                retVal = 0;
            }

            pthread_rwlock_unlock( &m_plcLock);
        }
        else
        {
            ThrowOsException( errVal, "Error attaining read lock in PlcMelsec::ReadChannel");
        }
    }
    else
    {
        throw( BepException( "PlcMelsec not initialized"));
    }

    return( retVal);
}

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
uint32_t PlcMelsec::ReadPort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, portDir_t portDir)
{
    uint32_t    retVal;

    // Lock for read
    // Read the current channel value
    retVal = ReadChannel( channelIdx, portDir);

    Log( LOG_DEV_DATA, "ReadPort( %d, %d, %d, %d) = $%04X\n",
         channelIdx, portIdx, logicLvl, portDir, retVal);

    // If this port does not occupy an entire channel
    if( portIdx != (uint16_t)-1)
    {
        Log( LOG_DEV_DATA, "$%08X >> %d = $%08X\n", retVal, portIdx, retVal >> portIdx);

        // Isolate the desired bit
        retVal = (retVal >> portIdx) & 0x01;

        // Inverse logic (positive false)
        if( !(logicLvl))    retVal ^= 0x01;
    }
    // Inverse logic (positive false)
    else if( !(logicLvl))
    {
        retVal = ~(retVal);
    }
    Log( LOG_DEV_DATA, "ReadPort ====> $%08hX\n", retVal);


    return( retVal);
}

/**
 * Writes to the the channel at the given index
 *
 * @param channelIdx Channel number to write to
 * @param channelVal Value to write to the given channel
 */
void PlcMelsec::WriteChannel( uint32_t channelIdx, uint32_t channelVal)
{
    int errVal;

    // If opened
    if( m_toPlc != NULL)
    {
        // Lock for write
        if( (errVal=pthread_rwlock_wrlock( &m_plcLock)) == EOK)
        {
            // Write data to the specified port
            m_toPlc[ channelIdx] = channelVal;

            pthread_rwlock_unlock( &m_plcLock);
        }
        else
        {
            ThrowOsException( errVal, "Error attaining write lock in PlcMelsec::WriteChannel");
        }
    }
    else
    {
        throw( BepException( "Melsec not initialized"));
    }
}

/**
 * Writes to specified port on the channel at the given index
 *
 * @param channelIdx Channel number to write to
 * @param portIdx    Port (bit) index to write to
 * @param logicLvl   Logic level for the specified port (positive true=1)
 * @param portVal    Value to write to the given port
 */
void PlcMelsec::WritePort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, uint32_t portVal)
{
    uint32_t    channelVal, dataMask = 1 << portIdx;

    // If this port does not occupy an entire channel
    if( portIdx != (uint16_t)-1)
    {
        // Read the current channel value
        channelVal = ReadChannel( channelIdx, PORT_DIR_OUT);

        // If inverse logic (positive false)
        if( !(logicLvl))
        {
            // Clear the bit
            if( portVal)    channelVal &= ~(dataMask);
            // Set the bit
            else            channelVal |= dataMask;
        }
        // If normal logic (positive true)
        else
        {
            // Clear the bit
            if( !(portVal)) channelVal &= ~(dataMask);
            // Set the bit
            else            channelVal |= dataMask;
        }
    }
    else
    {
        if( !(logicLvl))    channelVal = ~(portVal);
        else                channelVal = portVal;
    }

    // Write data to the specified port
    WriteChannel( channelIdx, channelVal);
}

/**
 * Assignment operator. The newly copied object WILL NOT be opened automatically
 *
 * @param rhs    Object to be copied
 * 
 * @return this
 */
PlcMelsec& PlcMelsec::operator=( const PlcMelsec& rhs)
{
    if( this != &rhs)
    {
        Close();

        m_fromPlc = rhs.m_fromPlc;
        m_fromPlcCount = rhs.m_fromPlcCount;

        m_melsecPortName = rhs.m_melsecPortName;

        CreateDataArrays();

        SetLocalEcho( rhs.GetLocalEcho());
        SetProcessName( rhs.GetProcessName());
        SetVerboseMask( rhs.GetVerboseMask());
        if( rhs.GetLogStatus() == true) EnableLogger();

        // Destrot the existing read/write lock
        pthread_rwlock_destroy( &m_plcLock);

        // Initialize the read/write lock
        pthread_rwlock_init( &m_plcLock, NULL);

    }
    return( *this);
}

/**
 * Allocates memory for the data arrays used to hold the PLC data
 */
void PlcMelsec::CreateDataArrays()
{
    Log( LOG_FN_ENTRY, "Enter PlcMelsec::CreateDataArrays()\n");
    if( (m_fromPlcCount > 0) && ( m_toPlcCount > 0))
    {
        DestroyDataArrays();
        m_fromPlc = new uint32_t[ m_fromPlcCount];
        memset( (void*)m_fromPlc, 0, m_fromPlcCount*sizeof( uint32_t));
        m_toPlc = new uint32_t[ m_toPlcCount];
        memset( (void*)m_toPlc, 0, m_toPlcCount*sizeof( uint32_t));
    }
    Log( LOG_FN_ENTRY, "Exit PlcMelsec::CreateDataArrays()\n");
}

/**
 * De-allocates memory for the data arrays used to hold the PLC data
 */
void PlcMelsec::DestroyDataArrays()
{
    Log( LOG_FN_ENTRY, "Enter PlcMelsec::DestroyDataArrays()\n");

    if( m_fromPlc != NULL)  delete[] m_fromPlc;
    if( m_toPlc != NULL)    delete[] m_toPlc;

    m_fromPlc = m_toPlc = NULL;
    Log( LOG_FN_ENTRY, "Exit PlcMelsec::DestroyDataArrays()\n");
}

/**
 * Expands environment variable names in a path name
 *
 * @param pathToResolve
 *               Path name to be resolved
 */
void PlcMelsec::ResolvePathName( string &pathToResolve)
{
    // Make a copy of the original
    string::size_type   envStart, envEnd;
    char                *envValue;

    Log( LOG_FN_ENTRY, "Enter PlcMelsec::ResolvePathName( %s)\n", pathToResolve.c_str());

    // Look for simple HOME character (~)
    while( (envStart=pathToResolve.find('~')) != pathToResolve.npos)
    {
        // Erase the '~' and replace it with $(HOME)
        pathToResolve.erase( envStart, 1);
        pathToResolve.insert( envStart, getenv( "HOME"));
    }

    // Now replace any '$()' delimitted environ vars
    while( (envStart=pathToResolve.find("$(")) != pathToResolve.npos)
    {
        // Look for the end of the environ var name
        if( (envEnd = pathToResolve.find(')', envStart)) != pathToResolve.npos)
        {
            // Get the environment variable name
            string envName( pathToResolve.substr( envStart+2, envEnd-envStart-2));
            // If it is a valid envirnoment variable name
            if( (envValue=getenv( envName.c_str())) != NULL)
            {
                // Replace it in the string
                pathToResolve.erase( envStart, envEnd+1);
                pathToResolve.insert( envStart, getenv( envName.c_str()));
            }
        }
    }
    Log( LOG_FN_ENTRY, "Exit PlcMelsec::ResolvePathName( %s)\n", pathToResolve.c_str());
}

/**
 * Thread entry point used to perform Melsec serial communication
 *
 * @param threadArg Pointer to a MelsecThreadArg_t data structure
 */
void* PlcMelsec::MelsecCommThread( void *threadArg)
{
    MelsecThreadArg_t   *arg = (MelsecThreadArg_t*)threadArg;
    int                 chId, lockStat;
    timer_t             timerId;
    struct _itimer      timerTime;
    struct sigevent     timerEvent;
    struct _pulse       timerPulse;
    bool                threadStop=false;
    pthread_rwlock_t    *rwLock = arg->rwLock;
    ILogger*            logger = arg->logger;
    const int           toCount = arg->toCount, fromCount=arg->fromCount;
    uint32_t            toPlc[ toCount], fromPlc[ fromCount];
    IFaultServer        faultIfc;
    std::string         faultRsp;
    uint8_t             faultSet=0;

    // Initialize
    arg->logger->Log( LOG_FN_ENTRY, "Enter PlcMelsec::MelsecCommThread()\n");

    memset( toPlc, 0, toCount * sizeof( uint32_t));
    memset( fromPlc, 0, fromCount * sizeof( uint32_t));

    for( int ii=0; ii<toCount; ii++)    toPlc[ ii] = ii;

    // Create a channel and connection to receive timer pulses on
    if( (timerId=BposCreateTimerWithPulse( &chId, &arg->coId, SYSTEM_PULSE+1)) != -1)
    {
        // Send a timer pulse to do an immediate read
        MsgSendPulse( arg->coId, -1, SYSTEM_PULSE+1, 0);

        // Initialize the fault interface
        try
        {
            faultIfc.Initialize( FAULT_SERVER_NAME);
        }
        catch( BepException &err)
        {
            logger->Log( "\tError initializing  fault interface in MelsecComm Thread: %s\n",
                         err.GetReason());
        }

        // Wait at barrier
        logger->Log( LOG_DEV_DATA, "\tWaiting at barrier\n");
        lockStat = pthread_barrier_wait( &arg->barrier);
        if( (lockStat == BARRIER_SERIAL_THREAD) || (lockStat == 0))
        {
            // Loop until TERMINATE_PULSE received
            logger->Log( LOG_DEV_DATA, "\tLooping (chId=%d, coId=%d\n", chId, arg->coId);
            while( threadStop == false)
            {
                // Bump my priority up higher
                BposSetThreadPriority( getpid(), pthread_self(), 35);

                // Initialize the event struct to deliver us a pulse (instead of just unblocking)
                SIGEV_PULSE_INIT( &timerEvent, arg->coId, 50, SYSTEM_PULSE+1, 0);

                // Initialize the timer
                timerTime.nsec = arg->nsecLoopTime; timerTime.interval_nsec = 0;
                TimerSettime( timerId, 0, &timerTime, NULL);

                // Wait for a pulse (timer pulse or TERMINATE_PULSE)
                if( (lockStat=MsgReceivePulse( chId, &timerPulse, sizeof( timerPulse), NULL))  == 0)
                {
                    // IF we know how to handle this type of pulse
                    switch( timerPulse.code)
                    {
                    case SYSTEM_PULSE+1:
                        // Read our inputs from the PLC
                        if( arg->plc->Read(fromPlc, fromCount) > 0)
                        {
                            if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
                            {
                                memmove( (uint32_t*)arg->fromPlc, fromPlc, fromCount*sizeof( uint32_t));
                                pthread_rwlock_unlock( rwLock);
                            }
                            else
                            {
                                logger->Log( LOG_ERRORS, "\tMelsecCommThread Error locking rwlock for read: %s\n",
                                             strerror( lockStat));
                            }

                            // If read fault is currently set
                            if( faultSet & 0x01)
                            {
                                logger->Log( LOG_ERRORS, "\tMelsecCommThread clearing MelsecReadFault\n");
                                try
                                {
                                    faultIfc.ClearFault( "MelsecReadFault", faultRsp, true);
                                }
                                catch( BepException &err)
                                {
                                    logger->Log( LOG_ERRORS, "\tException clearing MelsecReadFault: %s\n",
                                                 err.GetReason());
                                }
                                catch( ...)
                                {
                                    logger->Log( LOG_ERRORS, "\tUnknown exception clearing MelsecReadFault\n");
                                }
                                // Clear read fault
                                faultSet &= ~(0x01);
                            }
                        }
                        // IF read fault not currently set
                        else if( !(faultSet & 0x01))
                        {
                            logger->Log( LOG_ERRORS, "\tMelsecCommThread setting MelsecReadFault\n");
                            try
                            {
                                faultIfc.SetFault( "MelsecReadFault", "Error reading Melsec response",
                                                   "PLC", faultRsp, true, 3);
                            }
                            catch( BepException &err)
                            {
                                logger->Log( LOG_ERRORS, "\tException setting MelsecReadFault: %s\n",
                                             err.GetReason());
                            }
                            catch( ...)
                            {
                                logger->Log( LOG_ERRORS, "\tUnknown exception setting MelsecReadFault\n");
                            }
                            // Set read fault
                            faultSet |= 0x01;
                        }

                        // Write our outputs to the PLC
                        if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
                        {
                            memmove( toPlc, (uint32_t*)arg->toPlc, toCount * sizeof( uint32_t));
                            pthread_rwlock_unlock( rwLock);
                            if( arg->plc->Write(toPlc, toCount) <= 0)
                            {
                                logger->Log( LOG_ERRORS, "\tMelsecCommThread setting MelsecWriteFault\n");

                                // If write fault not currently set
                                if( !(faultSet & 0x02))
                                {
                                    try
                                    {
                                        faultIfc.SetFault( "MelsecWriteFault", "Error writing Melsec data",
                                                           "PLC", faultRsp, true, 3);
                                    }
                                    catch( BepException &err)
                                    {
                                        logger->Log( LOG_ERRORS, "\tException setting MelsecWriteFault: %s\n",
                                                     err.GetReason());
                                    }
                                    catch( ...)
                                    {
                                        logger->Log( LOG_ERRORS, "\tUnknown exception setting MelsecWriteFault\n");
                                    }
                                    // Set write fault
                                    faultSet |= 0x02;
                                }
                            }
                            // If write fault currently set
                            else if( faultSet & 0x02)
                            {
                                logger->Log( LOG_ERRORS, "\tMelsecCommThread clearing MelsecWriteFault\n");
                                try
                                {
                                    faultIfc.ClearFault( "MelsecWriteFault", faultRsp, true);
                                }
                                catch( BepException &err)
                                {
                                    logger->Log( LOG_ERRORS, "\tException clearing MelsecWriteFault: %s\n",
                                                 err.GetReason());
                                }
                                catch( ...)
                                {
                                    logger->Log( LOG_ERRORS, "\tUnknown exception elearing MelsecWriteFault\n");
                                }
                                // Clear write fault
                                faultSet &= ~(0x02);
                            }
                        }
                        else
                        {
                            logger->Log( LOG_ERRORS, "\tMelsecCommThread Error locking rwlock for write: %s\n",
                                         strerror( lockStat));
                        }
                        break;
                    case SYSTEM_PULSE:
                        logger->Log( LOG_DEV_DATA, "Melsec Comm thread received SYSTEM_PULSE %d\n",
                                     timerPulse.value.sival_int);
                        if( timerPulse.value.sival_int == TERMINATE_PULSE)  threadStop = true;

                        break;
                    default:
                        logger->Log( LOG_DEV_DATA, "Melsec Comm thread received unexpected pulse: %d,%d\n",
                                     timerPulse.code, timerPulse.value.sival_int);
                        break;
                    }
                }
                else
                {
                    logger->Log( LOG_ERRORS, "\tMelsec Comm thread MsgReceive error: %s\n",
                                 strerror( errno));
                }
            }
            logger->Log( LOG_DEV_DATA, "Melsec Comm thread received TERMINATE_PULSE...ending\n");
        }
        else
        {
            logger->Log( LOG_ERRORS, "\tError #%d waiting at barrier: %s\n",
                         lockStat, strerror( lockStat));
        }
        timerTime.nsec = timerTime.interval_nsec = 0;
        TimerSettime( timerId, 0, &timerTime, NULL);
        TimerDestroy( timerId);
        ConnectDetach( arg->coId);
        ChannelDestroy( chId);

        arg->coId = -1;
        chId = -1;
    }
    else
    {
        logger->Log( LOG_ERRORS, "\tError creating thread timer channel/connection\n");
    }
    pthread_barrier_destroy( &arg->barrier);

    logger->Log( LOG_FN_ENTRY, "Exit PlcMelsec::MelsecCommThread()\n");

    return( NULL);
}
