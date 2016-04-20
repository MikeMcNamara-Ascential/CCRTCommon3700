//******************************************************************************
//
// Description:
//    Allen BRadley PLC V30B Board Object. Used to control a V30B board.
//
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcTcp.cpp $
// 
// 1     12/11/02 11:03p Bmeinke
// Initial check-in
//
//******************************************************************************

/*******************
 include files
 ******************/
#include <errno.h>
#include <sync.h>           // BARRIER_SERIAL_THREAD
#include <sys/netmgr.h>     // ND_LOCAL_NODE
#include "PlcTcp.h"
#include "BepException.h"

/**
 * Default constructor
 */
PlcTcp::PlcTcp() : IPlc(),
    m_fromPlc( NULL),  m_toPlc( NULL), m_tcpPortName( ""),
m_commScanTime( 20), m_ndb(false)
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
PlcTcp::PlcTcp( const PlcTcp &copy) : IPlc( copy),
    m_fromPlc( NULL),  m_toPlc( NULL), m_tcpPortName( ""),
    m_commScanTime( 20)
{
    // Initialize the read/write lock
    pthread_rwlock_init( &m_plcLock, NULL);

    *this = copy;
}

/**
 * Default destructor
 */
PlcTcp::~PlcTcp()
{
    Log( LOG_FN_ENTRY, "Enter PlcTcp::~PlcTcp\n");
    Close();

    // Destroy the read/write lock
    pthread_rwlock_destroy( &m_plcLock);
    Log( LOG_FN_ENTRY, "Exit PlcTcp::~PlcTcp\n");
}

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
bool PlcTcp::Open(uint32_t inputCount, uint32_t outputCount, bool demoMode)
{
    bool    retVal = true;

    Log( LOG_FN_ENTRY, "Enter PlcTcp::Open()\n");

    m_fromPlcCount = inputCount;
    m_toPlcCount = outputCount;

    CreateDataArrays();

    try
    {
        // Launch the TCP comm thread
        LaunchCommThread( m_tcpPortName, m_tcpPortNameRead, m_tcpPortNameWrite);
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "\tError launching TCP comm thread: %s\n",
             err.GetReason());
        retVal = false;
    }
    Log( LOG_FN_ENTRY, "Exit PlcTcp::Open()\n");

    return( retVal);
}

/**
 * Initializes the underlying PLC object
 *
 * @param plcSetup XML Parent Node containing PLCBoard configuration
 */
void PlcTcp::Initialize( const XmlNode *plcSetup)
{
    Log( LOG_FN_ENTRY, "Enter PlcTcp::Initialize( %s)\n", plcSetup->getName().c_str());

    // Load the additional configuration items needed by the PlcTcp object
    LoadAdditionalConfigurationItems(plcSetup);

    Log( LOG_FN_ENTRY, "Exit PlcTcp::Initialize()\n");
}

void PlcTcp::LoadAdditionalConfigurationItems(const XmlNode *plcSetup)
{
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
        ReadInputConfiguration( plcSetup->getChild( XML_T( "Inputs"))->getChildren());
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "No Input configuration specified in config file\n");
    }

    // Read the output configuration
    try
    {
        ReadOutputConfiguration( plcSetup->getChild( XML_T( "Outputs"))->getChildren());
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "No Output configuration specified in config file\n");
    }

    // Read the name of the default logical port to talk on
    try
    {
        const XmlString& tmp = plcSetup->getChild( XML_T("LogicalPort"))->getValue();
        m_tcpPortName = XmlToAscii( tmp);
    }
    catch( XmlException &err)
    {
        m_tcpPortName = "PlcTcp";
    }
    Log( LOG_DEV_DATA, "TCP PLC Logical Port Name: %s\n", m_tcpPortName.c_str());

    // Read the name of the logical port to talk on for reads
    try
    {
        const XmlString& tmp = plcSetup->getChild( XML_T("LogicalPortRead"))->getValue();
        m_tcpPortNameRead = XmlToAscii( tmp);
    }
    catch( XmlException &err)
    {
        m_tcpPortNameRead = "";
    }
    Log( LOG_DEV_DATA, "TCP PLC Read Logical Port Name: %s\n", m_tcpPortNameRead.c_str());

    // Read the name of the logical port to talk on for writes
    try
    {
        const XmlString& tmp = plcSetup->getChild( XML_T("LogicalPortWrite"))->getValue();
        m_tcpPortNameWrite = XmlToAscii( tmp);
    }
    catch( XmlException &err)
    {
        m_tcpPortNameWrite = "";
    }
    Log( LOG_DEV_DATA, "TCP PLC Write Logical Port Name: %s\n", m_tcpPortNameWrite.c_str());

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
    Log( LOG_DEV_DATA, "TCP PLC Scan Time: %d ms\n", m_commScanTime);

    // Read the serial comm retry interval
    try
    {
        const XmlString& tmp = plcSetup->getChild( XML_T("TcpRetryInterval"))->getValue();
        m_commRetryInterval = BposReadInt( XmlToAscii( tmp).c_str());
    }
    catch( XmlException &err)
    {
        m_commRetryInterval = 5000;
    }
    Log( LOG_DEV_DATA, "TCP Comm Retry Interval: %d ms\n", m_commRetryInterval);

    // Read the serial comm retry interval
    try
    {
        const XmlString& tmp = plcSetup->getChild( XML_T("ReadWriteFaultLimit"))->getValue();
        m_commFaultLimit = BposReadInt( XmlToAscii( tmp).c_str());
    }
    catch( XmlException &err)
    {
        m_commFaultLimit = 10;
    }
    Log( LOG_DEV_DATA, "TCP Comm Fault Limit: %d faults before retry\n", m_commFaultLimit);

//     // Read the TCP source node IDs                                                                //
//     try                                                                                            //
//     {                                                                                              //
//         const XmlString& tmp = plcSetup->getChild( XML_T("TCPNodeIds/SysConNodeId"))->getValue();  //
//         m_tcpThreadArg.tcpSourceId = BposReadInt( XmlToAscii( tmp).c_str());                       //
//     }                                                                                              //
//     catch( XmlException &err)                                                                      //
//     {                                                                                              //
//         Log( LOG_ERRORS, "\tNO TCP SOURCE NODE ID SPECIFIED!!!\n");                                //
//         m_tcpThreadArg.tcpSourceId = 0xFF;                                                         //
//     }                                                                                              //
//                                                                                                    //
//     // Read the TCP destination node IDs                                                           //
//     try                                                                                            //
//     {                                                                                              //
//         const XmlString& tmp = plcSetup->getChild( XML_T("TCPNodeIds/PlcNodeId"))->getValue();     //
//         m_tcpThreadArg.tcpDestId = BposReadInt( XmlToAscii( tmp).c_str());                         //
//     }                                                                                              //
//     catch( XmlException &err)                                                                      //
//     {                                                                                              //
//         Log( LOG_ERRORS, "\tNO TCP DESTINATION NODE ID SPECIFIED!!!\n");                           //
//         m_tcpThreadArg.tcpDestId = 0xFF;                                                           //
//     }                                                                                              //

    m_plc.Initialize( plcSetup);
}

const INT32 PlcTcp::GetCommFaultLimit()
{
    return m_commFaultLimit;
}

/**
 * Launches the TCP serial comm thread (used to update PLC data arrays
 * via serial messages to and from the TCP PLC). Alos opens the logical
 * port we need to use for PLC communication and loads/initializes the
 * appropriate PLC DLL.
 *
 * @param portName Name of the logical port to communicate with the PLC on
 */
void PlcTcp::LaunchCommThread( const std::string &portName, const std::string &portNameRead, const std::string &portNameWrite) throw( BepException)
{
    std::string errStr = "";
    int         status;
    bool        separatePorts = false;
    bool        openPortStatus = false;

    Log( LOG_FN_ENTRY, "Enter PlcTcp::LaunchCommThread()\n");

    // Check if we should use separate serial ports
    if((portNameRead != "") && (portNameWrite != ""))
    {
        separatePorts = true;
    }
    m_useSeparatePorts = separatePorts;

    // Try to open the serial port
    if(m_useSeparatePorts)
    {
        openPortStatus = m_serCom.OpenPort(portNameRead, O_RDWR);
        if(!openPortStatus)
        {
            errStr += "Error with port " + portNameRead + ": ";
        }
        else
        {
            openPortStatus = m_serCom2.OpenPort(portNameWrite, O_RDWR);
            if(!openPortStatus)
            {
                errStr += "Error with port " + portNameWrite + ": ";
            }
        }
    }
    else
    {
        openPortStatus = m_serCom.OpenPort( portName, O_RDWR);
        if(!openPortStatus)
        {
            errStr += "Error with port " + portName + ": ";
        }
    }

    if( openPortStatus == false)
    {
        errStr += "Unable to open TCP logical port: ";
        errStr += strerror( errno);
        throw( BepException( errStr));
    }
    else
    {
        // Flush FIFOs at startup
        m_serCom.ResetConnection();
        if(m_useSeparatePorts) m_serCom2.ResetConnection();

        try
        {
            if(m_useSeparatePorts)
            {
                if( (status=m_plc.OpenSeparateRW( m_serCom.GetPortFd(), m_serCom2.GetPortFd(), m_tcpThreadArg.tcpSourceId)) != EOK)
                {
                    Log( LOG_ERRORS, "\tm_plc.PlcOpenSeparateRW() failed: %s\n", strerror(status));
                    throw( BepException( "Unable to initialize TCP DLL: %s"), strerror(status) );
                }
            }
            else
            {
                if( (status=m_plc.Open( m_serCom.GetPortFd(), m_tcpThreadArg.tcpSourceId)) != EOK)
                {
                    Log( LOG_ERRORS, "\tm_plc.PlcOpen() failed: %s\n", strerror(status));
                    throw( BepException( "Unable to initialize TCP DLL: %s"), strerror(status) );
                }
            }

            pthread_barrier_init( &m_tcpThreadArg.barrier, NULL, 2);
            m_tcpThreadArg.parent = this;
            m_tcpThreadArg.fromPlc = m_fromPlc; m_tcpThreadArg.fromCount = m_fromPlcCount;
            m_tcpThreadArg.toPlc = m_toPlc;     m_tcpThreadArg.toCount = m_toPlcCount;
            m_tcpThreadArg.plc = &m_plc;
            m_tcpThreadArg.logger = (ILogger *)this;
            m_tcpThreadArg.rwLock = (pthread_rwlock_t*)&m_plcLock;
            m_tcpThreadArg.nsecLoopTime = mSEC_nSEC( m_commScanTime);
            m_tcpThreadArg.threadStop = false;

            pthread_create( &m_tcpThreadArg.threadId, NULL, &PlcTcp::TcpCommThread, (void*)&m_tcpThreadArg);
            // Wait for serial comm thread to start
            status = pthread_barrier_wait( &m_tcpThreadArg.barrier);
            if( (status != BARRIER_SERIAL_THREAD) && (status != 0))
            {
                errStr = "TCP comm thread init error: ";
                errStr += strerror( status);
                throw( BepException( errStr));
            }
            Log( LOG_DEV_DATA, "TCP comm thread launched with thread id of %d\n",
                 m_tcpThreadArg.threadId);
            // Wait for the first scan to complete
            delay( m_commScanTime*2);
        }
        catch( BepException &err)
        {
            Log( LOG_ERRORS, "\tError initializing TCP PLC DLL: %s\n", err.GetReason());
            throw;
        }
    }

    Log( LOG_FN_ENTRY, "Exit PlcTcp::LaunchCommThread()\n");
}

/**
 * Reads the input channel configuration from the given node map
 *
 * @param inputNodes XmlNodeMap containing the setup nodes for the PLC inputs
 */
void PlcTcp::ReadInputConfiguration( const XmlNodeMap &inputNodes)
{
    Log( LOG_FN_ENTRY, "Enter PlcTcp::ReadInputConfiguration()\n");

    try
    {
        const XmlNode *addressNode = inputNodes.getNode( "Address");
        m_tcpThreadArg.readAddress = XmlToAscii( addressNode->getValue());
    }
    catch( XmlException &err)
    {
        m_tcpThreadArg.readAddress = "$N17";
    }

    Log( LOG_DEV_DATA, "Input Address: %s\n", m_tcpThreadArg.readAddress.c_str());

    Log( LOG_FN_ENTRY, "Exit PlcTcp::ReadInputConfiguration()\n");
}


/**
 * Reads the output channel configuration from the given node map
 *
 * @param outputNodes
 *               XmlNodeMap containing the setup nodes for the PLC outputs
 */
void PlcTcp::ReadOutputConfiguration( const XmlNodeMap &outputNodes)
{
    Log( LOG_FN_ENTRY, "Enter PlcTcp::ReadOutputConfiguration()\n");

    // Check for output channel PLC address
    try
    {
        const XmlNode *addressNode = outputNodes.getNode( "Address");
        m_tcpThreadArg.writeAddress = XmlToAscii( addressNode->getValue());
    }
    catch( XmlException &err)
    {
        m_tcpThreadArg.writeAddress = "$N16";
    }
    Log( LOG_DEV_DATA, "Output Address: %s\n", m_tcpThreadArg.writeAddress.c_str());

    Log( LOG_FN_ENTRY, "Exit PlcTcp::ReadOutputConfiguration()\n");
}

/**
 * Closes the object
 */
void PlcTcp::Close()
{
    Log( LOG_FN_ENTRY, "Enter PlcTcp::Close\n");

    // If comm thread is running
    if( m_tcpThreadArg.threadId != -1)
    {
        Log( LOG_FN_ENTRY, "Terminating comm thread\n");

        m_plc.Close();

        // Send comm thread a terminate pulse to shut it down
        m_tcpThreadArg.threadStop = true;
        MsgSendPulse( m_tcpThreadArg.coId, 62, SYSTEM_PULSE, TERMINATE_PULSE);

        // Wait for the thread to die
        pthread_join( m_tcpThreadArg.threadId, NULL);

        Log( LOG_FN_ENTRY, "Comm thread dead\n");
    }

    // De-allocate data array memory
    DestroyDataArrays();

    Log( LOG_FN_ENTRY, "Exit PlcTcp::Close\n");
}

/**
 * Reads the channel at the given index
 *
 * @param channelIdx Channel index to read
 * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
 * @return Value of the channel at the given index
 */
uint32_t PlcTcp::ReadChannel( uint32_t channelIdx, portDir_t portDir)
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
            ThrowOsException( errVal, "Error attaining read lock in PlcTcp::ReadChannel");
        }
    }
    else
    {
        throw( BepException( "PlcTcp not initialized"));
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
 * @return 1 if port is "true"
 *         0 if port is "off"
 */
uint32_t PlcTcp::ReadPort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, portDir_t portDir)
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
void PlcTcp::WriteChannel( uint32_t channelIdx, uint32_t channelVal)
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
            ThrowOsException( errVal, "Error attaining write lock in PlcTcp::WriteChannel");
        }
    }
    else
    {
        throw( BepException( "PlcTcp not initialized"));
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
void PlcTcp::WritePort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, uint32_t portVal)
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
 * @return this
 */
PlcTcp& PlcTcp::operator=( const PlcTcp& rhs)
{
    if( this != &rhs)
    {
        Close();

        m_fromPlc = rhs.m_fromPlc;
        m_fromPlcCount = rhs.m_fromPlcCount;

        m_tcpPortName = rhs.m_tcpPortName;

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
void PlcTcp::CreateDataArrays()
{
    Log( LOG_FN_ENTRY, "Enter PlcTcp::CreateDataArrays()\n");
    if( (m_fromPlcCount > 0) && ( m_toPlcCount > 0))
    {
        DestroyDataArrays();
        m_fromPlc = new uint32_t[ m_fromPlcCount];
        memset( (void*)m_fromPlc, 0, m_fromPlcCount*sizeof( uint32_t));
        m_toPlc = new uint32_t[ m_toPlcCount];
        memset( (void*)m_toPlc, 0, m_toPlcCount*sizeof( uint32_t));
    }
    Log( LOG_FN_ENTRY, "Exit PlcTcp::CreateDataArrays()\n");
}

/**
 * De-allocates memory for the data arrays used to hold the PLC data
 */
void PlcTcp::DestroyDataArrays()
{
    Log( LOG_FN_ENTRY, "Enter PlcTcp::DestroyDataArrays()\n");

    if( m_fromPlc != NULL)  delete[] m_fromPlc;
    if( m_toPlc != NULL)    delete[] m_toPlc;

    m_fromPlc = m_toPlc = NULL;
    Log( LOG_FN_ENTRY, "Exit PlcTcp::DestroyDataArrays()\n");
}

/**
 * Expands environment variable names in a path name
 *
 * @param pathToResolve
 *               Path name to be resolved
 */
void PlcTcp::ResolvePathName( string &pathToResolve)
{
    // Make a copy of the original
    string::size_type   envStart, envEnd;
    char                *envValue;

    Log( LOG_FN_ENTRY, "Enter PlcTcp::ResolvePathName( %s)\n", pathToResolve.c_str());

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
    Log( LOG_FN_ENTRY, "Exit PlcTcp::ResolvePathName( %s)\n", pathToResolve.c_str());
}

INT32 PlcTcp::SystemWrite(const std::string &tag, const std::string &value)
{
    INT32 result = BEP_STATUS_ERROR;    // the result of the operation
    std::string response;

    try
    {
        result = m_ndb.Write(tag, value, response, true);
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "PlcTcp::SystemWrite %s Error: %s\n", tag.c_str(), e.what());
    }

    return(result);
}

/**
 * Thread entry point used to perform TCP serial communication
 *
 * @param threadArg Pointer to a TcpThreadArg_t data structure
 */
void* PlcTcp::TcpCommThread( void *threadArg)
{
    TcpThreadArg_t      *arg = (TcpThreadArg_t*)threadArg;
    int                 chId, lockStat;
    PlcTcp              *parentPlc = arg->parent;
    timer_t             timerId;
    struct _itimer      timerTime;
    struct sigevent     timerEvent;
    struct _pulse       timerPulse;
    pthread_rwlock_t    *rwLock = arg->rwLock;
    ILogger*        logger = arg->logger;
    ITcpProtocol        *serialPlc = arg->plc;
    const int           toCount = arg->toCount, fromCount=arg->fromCount;
    uint32_t            toPlc[ toCount], fromPlc[ fromCount];
    std::string         faultRsp;
    uint8_t             faultSet=0;
    int                 readFaultCount=0;
    int                 writeFaultCount=0;
    const int           faultLimit=parentPlc->GetCommFaultLimit();

    // Initialize
    logger->Log( LOG_FN_ENTRY, "Enter PlcTcp::TcpCommThread()\n");

    memset( toPlc, 0, toCount * sizeof( uint32_t));
    memset( fromPlc, 0, fromCount * sizeof( uint32_t));

    for( int ii=0; ii<toCount; ii++)    toPlc[ ii] = ii;

    // Create a channel and connection to receive timer pulses on
    if( (timerId=BposCreateTimerWithPulse( &chId, &arg->coId, SYSTEM_PULSE+1)) != -1)
    {
        // Send a timer pulse to do an immediate read
        MsgSendPulse( arg->coId, -1, SYSTEM_PULSE+1, 0);

        // Wait at barrier
        logger->Log( LOG_DEV_DATA, "\tWaiting at barrier\n");
        lockStat = pthread_barrier_wait( &arg->barrier);
        if( (lockStat == BARRIER_SERIAL_THREAD) || (lockStat == 0))
        {
            // Loop until TERMINATE_PULSE received
            logger->Log( LOG_DEV_DATA, "\tLooping (chId=%d, coId=%d\n", chId, arg->coId);
            while( false == arg->threadStop)
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
                        if( serialPlc->Read( arg->tcpDestId, fromPlc, fromCount, arg->readAddress.c_str()) == true)
                        {
                            if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
                            {
                                memmove( (uint32_t*)arg->fromPlc, fromPlc, fromCount*sizeof( uint32_t));
                                pthread_rwlock_unlock( rwLock);
                            }
                            else
                            {
                                logger->Log( LOG_ERRORS, "\tTCPCommThread Error locking rwlock for read: %s\n",
                                    strerror( lockStat));
                            }

                            // If read fault is currently set
                            if( faultSet & 0x01)
                            {
                                logger->Log( LOG_ERRORS, "\tTCPCommThread clearing PLCSerialReadFault\n");
                                parentPlc->SystemWrite( "PLCSerialReadFault", "0");
                                // Clear read fault
                                faultSet &= ~(0x01);
                                readFaultCount = 0;
                            }
                        }
                        // IF read fault not currently set
                        else if( !(faultSet & 0x01))
                        {
                            logger->Log( LOG_ERRORS, "\tTCPCommThread setting PLCSerialReadFault\n");
                            parentPlc->SystemWrite( "PLCSerialReadFault", "1");
                            // Set read fault and increment the fault count
                            faultSet |= 0x01;
                            readFaultCount++;
                            parentPlc->FlushBuffers();
                        }
                        else
                        {
                            // just increment the fault count
                            readFaultCount++;
                            parentPlc->FlushBuffers();
                        }

                        // Write our outputs to the PLC
                        if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
                        {
                            memmove( toPlc, (uint32_t*)arg->toPlc, toCount * sizeof( uint32_t));
                            pthread_rwlock_unlock( rwLock);
                            if( serialPlc->Write( arg->tcpDestId, toPlc, toCount, arg->writeAddress.c_str()) == true)
                            {
                            // If write fault currently set
                                if( faultSet & 0x02)
                                {
                                    logger->Log( LOG_ERRORS, "\tTCPCommThread clearing PLCSerialWriteFault\n");
                                    parentPlc->SystemWrite( "PLCSerialWriteFault", "0");
                                // Clear write fault
                                faultSet &= ~(0x02);
                                writeFaultCount = 0;
                            }
                        }
                            // If write fault not currently set
                            else if( !(faultSet & 0x02))
                            {
                                logger->Log( LOG_ERRORS, "\tTCPCommThread setting PLCSerialWriteFault\n");
                                parentPlc->SystemWrite( "PLCSerialWriteFault", "1");
                                // Set write fault and increment the fault count
                                faultSet |= 0x02;
                                writeFaultCount++;
                                parentPlc->FlushBuffers();
                            }
                            else
                            {
                                // Just increment the fault count
                                writeFaultCount++;
                                parentPlc->FlushBuffers();
                            }
                        }
                        else
                        {
                            logger->Log( LOG_ERRORS, "\tTCPCommThread Error locking rwlock for write: %s\n",
                                             strerror( lockStat));
                        }
                        break;
                    case SYSTEM_PULSE:
                        logger->Log( LOG_DEV_DATA, "TCP Comm thread received SYSTEM_PULSE %d\n",
                                         timerPulse.value.sival_int);
                        if( timerPulse.value.sival_int == TERMINATE_PULSE)  arg->threadStop = true;

                        break;
                    default:
                        logger->Log( LOG_DEV_DATA, "TCP Comm thread received unexpected pulse: %d,%d\n",
                                         timerPulse.code, timerPulse.value.sival_int);
                        break;
                    }
                }
                else
                {
                    logger->Log( LOG_ERRORS, "\tTCP Comm thread MsgReceive error: %s\n",
                                     strerror( errno));
                }

                // fault count comm check
                // if we get too many comm faults, assume we lost the port. try to re-initialize the port
                if((readFaultCount >= faultLimit) || (writeFaultCount >= faultLimit))
                {
                    parentPlc->ResetTcpComms();
                    readFaultCount = 0;
                    writeFaultCount = 0;
                }
            }
            logger->Log( LOG_DEV_DATA, "TCP Comm thread received TERMINATE_PULSE...ending\n");
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

    logger->Log( LOG_FN_ENTRY, "Exit PlcTcp::TcpCommThread()\n");

    return( NULL);
}

void PlcTcp::ResetTcpComms(void) throw( BepException)
{
    Log(LOG_ERRORS, "Enter PlcTcp::ResetTcpComms()\n");
    BposSleep(1000);
    string errStr;
    int status;
    bool portOpened = false;

    Log(LOG_DEV_DATA, "Closing plc link to TCP port\n");
    m_plc.Close();
    m_serCom.ClosePort();
    m_serCom2.ClosePort();
    Log(LOG_DEV_DATA, "Closed plc link to TCP port\n");

    while(!portOpened)
    {
        BposSleep(m_commRetryInterval);
        Log(LOG_DEV_DATA, "Attempting to open the port \n");
        // Try to open the serial port
        if(m_useSeparatePorts)
        {
            portOpened = m_serCom.OpenPort(m_tcpPortNameRead, O_RDWR);
            if(!portOpened)
            {
                errStr = "Error with port " + m_tcpPortNameRead + ": ";
            }
            else
            {
                portOpened = m_serCom2.OpenPort(m_tcpPortNameWrite, O_RDWR);
                if(!portOpened)
                {
                    errStr = "Error with port " + m_tcpPortNameWrite + ": ";
                    m_serCom.ClosePort();
                }
            }
        }
        else
        {
            portOpened = m_serCom.OpenPort( m_tcpPortName, O_RDWR);
            if(!portOpened)
            {
                errStr = "Error with port " + m_tcpPortName + ": ";
            }
        }
    
        try
        {
            if( portOpened == false)
            {
                errStr += "Unable to open TCP logical port: ";
                errStr += strerror( errno);
                Log( LOG_ERRORS, "Error: %s", errStr.c_str());
            }
            else
            {
                // Flush FIFOs at startup
                m_serCom.ResetConnection();
                if(m_useSeparatePorts) m_serCom2.ResetConnection();

                if(m_useSeparatePorts)
                {
                    if( (status=m_plc.OpenSeparateRW( m_serCom.GetPortFd(), m_serCom2.GetPortFd(), m_tcpThreadArg.tcpSourceId)) != EOK)
                    {
                        Log( LOG_ERRORS, "\tm_plc.PlcOpenSeparateRW() failed: %s\n", strerror(status));
                        m_serCom.ClosePort();
                        m_serCom2.ClosePort();
                        portOpened = false;
                    }
                }
                else
                {
                    if( (status=m_plc.Open( m_serCom.GetPortFd(), m_tcpThreadArg.tcpSourceId)) != EOK)
                    {
                        Log( LOG_ERRORS, "\tm_plc.PlcOpen() failed: %s\n", strerror(status));
                        m_serCom.ClosePort();
                        portOpened = false;
                    }
                }
            }
        }
        catch( BepException &err)
        {
        }
    }
    /*while(status=m_serCom.ReconnectDriver() != EOK)
    {
        Log(LOG_ERRORS, "TCP Comm (connection 1) Reconnect failed: %s. Trying again in %d ms\n", strerror(status), m_commRetryInterval);
        BposSleep(m_commRetryInterval);
    }
    while(status=m_serCom2.ReconnectDriver() != EOK)
    {
        Log(LOG_ERRORS, "TCP Comm (connection 2) Reconnect failed: %s. Trying again in %d ms\n", strerror(status), m_commRetryInterval);
        BposSleep(m_commRetryInterval);
    }*/
    FlushBuffers();

    Log(LOG_DEV_DATA, "Re-opening plc link to TCP port\n");
    try
    {
        if(m_useSeparatePorts)
        {
            if( (status=m_plc.OpenSeparateRW( m_serCom.GetPortFd(), m_serCom2.GetPortFd(), m_tcpThreadArg.tcpSourceId)) != EOK)
            {
                Log( LOG_ERRORS, "\tm_plc.PlcOpenSeparateRW() failed: %s\n", strerror(status));
                throw( BepException( "Unable to initialize TCP DLL: %s"), strerror(status) );
            }
        }
        else
        {
            if( (status=m_plc.Open( m_serCom.GetPortFd(), m_tcpThreadArg.tcpSourceId)) != EOK)
            {
                Log( LOG_ERRORS, "\tm_plc.PlcOpen() failed: %s\n", strerror(status));
                throw( BepException( "Unable to initialize TCP DLL: %s"), strerror(status) );
            }
        }
        // Wait for the first scan to complete
        delay( m_commScanTime*2);
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "\tError initializing TCP PLC DLL: %s\n", err.GetReason());
        throw;
    }
    Log(LOG_DEV_DATA, "Re-opened plc link to TCP port\n");

    Log(LOG_ERRORS, "Exit PlcTcp::ResetTcpComms()\n");
}

void PlcTcp::FlushBuffers(void)
{
    Log(LOG_ERRORS, "Enter PlcTcp::FlushBuffers()\n");

    m_serCom.ResetConnection();
    if(m_useSeparatePorts) m_serCom2.ResetConnection();

    Log(LOG_ERRORS, "Exit PlcTcp::FlushBuffers()\n");
}

