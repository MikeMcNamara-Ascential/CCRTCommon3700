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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcDf1.cpp $
// 
// 4     5/24/06 2:55p Cward
// Updated PlcMgr to allow reloading of config files without rebooting.
//
// 1     5/16/06 1:18p Rwiersem
// Added LoadAdditionalConfigurationItems() to support reloading the
// configuration without restarting the process.
//
// 3     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:16p Bmeinke
// Added an IDf1Protocol::Initialize( XmlNode*) method to read config file
// data
// IDf1Protocol::Read() and IDf1Protocol::Write() now return true/false
// Better thread termination logic
//
// 2     8/18/04 11:34p Cward
// Added initialization of base class ivar. No change needed in copy
// constructor because base class copy constructor is used,and ios already
// handled there.
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 2     8/14/03 11:35a Khykin
// Update provided by Brian.
//
// 3     6/24/03 8:56a Khykin
// Brians latest changes.
//
// 1     6/20/03 9:58a Khykin
// Initial checkin.
//
// 2     6/20/03 9:02a Khykin
// Updated Ford code.
//
// 2     6/18/03 7:13p Bmeinke
// Set faults if we lose DF1 communication
//
// 1     4/30/03 4:46p Bmeinke
// Migration to new personalized configuration management structure
//
// 4     4/10/03 6:38p Bmeinke
// Fixed the logic inside Df1CommThread() when checking the return status
// of pthread_barrier_wait()
//
// 3     3/12/03 12:41a Bmeinke
// Initial machine testing for Ford Chicago
// No longer read the input/output channel count from the config file:
// they are passed as arguments to Open()
// The serial comm thread uses local data arrays (instead of the data
// members) when reading/writing data (performance benefit)
//
// 2     1/18/03 3:48p Bmeinke
// Maintenance checkin (not fully tested)
//
// 1     12/11/02 11:03p Bmeinke
// Initial check-in
//
//******************************************************************************

/*******************
 include files
 ******************/
#include <errno.h>
#include <sync.h>			// BARRIER_SERIAL_THREAD
#include <sys/netmgr.h>		// ND_LOCAL_NODE
#include "PlcDf1.h"
#include "BepException.h"

/**
 * Default constructor
 */
PlcDf1::PlcDf1() : IPlc(),
	m_fromPlc( NULL),  m_toPlc( NULL), m_df1PortName( ""),
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
PlcDf1::PlcDf1( const PlcDf1 &copy) : IPlc( copy),
	m_fromPlc( NULL),  m_toPlc( NULL), m_df1PortName( ""),
	m_commScanTime( 20)
{
	// Initialize the read/write lock
	pthread_rwlock_init( &m_plcLock, NULL);

	*this = copy;
}

/**
 * Default destructor
 */
PlcDf1::~PlcDf1()
{
	Log( LOG_FN_ENTRY, "Enter PlcDf1::~PlcDf1\n");
	Close();

	// Destroy the read/write lock
	pthread_rwlock_destroy( &m_plcLock);
	Log( LOG_FN_ENTRY, "Exit PlcDf1::~PlcDf1\n");
}

/**
 * Opens a DF1 serial PLC board
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
bool PlcDf1::Open(uint32_t inputCount, uint32_t outputCount, bool demoMode)
{
	bool	retVal = true;

	Log( LOG_FN_ENTRY, "Enter PlcDf1::Open()\n");

	m_fromPlcCount = inputCount;
	m_toPlcCount = outputCount;

	CreateDataArrays();

	try
	{
		// Launch the DF1 comm thread
		LaunchCommThread( m_df1PortName);
	}
	catch( BepException &err)
	{
		Log( LOG_ERRORS, "\tError launching DF1 comm thread: %s\n",
			 err.GetReason());
		retVal = false;
	}
	Log( LOG_FN_ENTRY, "Exit PlcDf1::Open()\n");

	return( retVal);
}

/**
 * Initializes the underlying PLC object
 *
 * @param plcSetup XML Parent Node containing PLCBoard configuration
 */
void PlcDf1::Initialize( const XmlNode *plcSetup)
{
	Log( LOG_FN_ENTRY, "Enter PlcDf1::Initialize( %s)\n", plcSetup->getName().c_str());

    // Load the additional configuration items needed by the PlcDf1 object
    LoadAdditionalConfigurationItems(plcSetup);

    Log( LOG_FN_ENTRY, "Exit PlcDf1::Initialize()\n");
}

void PlcDf1::LoadAdditionalConfigurationItems(const XmlNode *plcSetup)
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

	// Read the name of the logical port to talk on
	try
	{
		const XmlString& tmp = plcSetup->getChild( XML_T("LogicalPort"))->getValue();
		m_df1PortName = XmlToAscii( tmp);
	}
	catch( XmlException &err)
	{
		m_df1PortName = "PlcDf1";
	}
	Log( LOG_DEV_DATA, "DF1 PLC Logical Port Name: %s\n", m_df1PortName.c_str());

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
	Log( LOG_DEV_DATA, "DF1 PLC Scan Time: %d ms\n", m_commScanTime);

	// Read the DF1 source node IDs
	try
	{
		const XmlString& tmp = plcSetup->getChild( XML_T("DF1NodeIds/SysConNodeId"))->getValue();
		m_df1ThreadArg.df1SourceId = BposReadInt( XmlToAscii( tmp).c_str());
	}
	catch( XmlException &err)
	{
		Log( LOG_ERRORS, "\tNO DF1 SOURCE NODE ID SPECIFIED!!!\n");
		m_df1ThreadArg.df1SourceId = 0xFF;
	}

	// Read the DF1 destination node IDs
	try
	{
		const XmlString& tmp = plcSetup->getChild( XML_T("DF1NodeIds/PlcNodeId"))->getValue();
		m_df1ThreadArg.df1DestId = BposReadInt( XmlToAscii( tmp).c_str());
	}
	catch( XmlException &err)
	{
		Log( LOG_ERRORS, "\tNO DF1 DESTINATION NODE ID SPECIFIED!!!\n");
		m_df1ThreadArg.df1DestId = 0xFF;
	}

	m_plc.Initialize( plcSetup);
}

/**
 * Launches the DF1 serial comm thread (used to update PLC data arrays
 * via serial messages to and from the DF1 PLC). Alos opens the logical
 * port we need to use for PLC communication and loads/initializes the
 * appropriate PLC DLL.
 *
 * @param portName Name of the logical port to communicate with the PLC on
 */
void PlcDf1::LaunchCommThread( const std::string &portName) throw( BepException)
{
	std::string	errStr;
	int			status;

	Log( LOG_FN_ENTRY, "Enter PlcDf1::LaunchCommThread()\n");

	// Try to open the serial prot
	if( m_serCom.OpenPort( portName, O_RDWR) == false)
	{
		errStr = "Unable to open DF1 logical port: ";
		errStr += strerror( errno);
		throw( BepException( errStr));
	}
	else
	{
		// Flush FIFOs at startup
		m_serCom.ResetConnection();

		try
		{
			if( (status=m_plc.Open( m_serCom.GetPortFd(), m_df1ThreadArg.df1SourceId)) != EOK)
			{
				Log( LOG_ERRORS, "\tm_plc.PlcOpen() failed: %s\n", strerror(status));
				throw( BepException( "Unable to initialize DF1 DLL: %s"), strerror(status) );
			}

			pthread_barrier_init( &m_df1ThreadArg.barrier, NULL, 2);
			m_df1ThreadArg.parent = this;
			m_df1ThreadArg.fromPlc = m_fromPlc;	m_df1ThreadArg.fromCount = m_fromPlcCount;
			m_df1ThreadArg.toPlc = m_toPlc;		m_df1ThreadArg.toCount = m_toPlcCount;
			m_df1ThreadArg.plc = &m_plc;
			m_df1ThreadArg.logger = (ILogger *)this;
			m_df1ThreadArg.rwLock = (pthread_rwlock_t*)&m_plcLock;
			m_df1ThreadArg.nsecLoopTime = mSEC_nSEC( m_commScanTime);
			m_df1ThreadArg.threadStop = false;

			pthread_create( &m_df1ThreadArg.threadId, NULL, &PlcDf1::Df1CommThread, (void*)&m_df1ThreadArg);
			// Wait for serial comm thread to start
			status = pthread_barrier_wait( &m_df1ThreadArg.barrier);
			if( (status != BARRIER_SERIAL_THREAD) && (status != 0))
			{
				errStr = "DF1 comm thread init error: ";
				errStr += strerror( status);
				throw( BepException( errStr));
			}
			Log( LOG_DEV_DATA, "DF1 comm thread launched with thread id of %d\n",
				 m_df1ThreadArg.threadId);
            // Wait for the first scan to complete
            delay( m_commScanTime*2);
		}
		catch( BepException &err)
		{
			Log( LOG_ERRORS, "\tError initializing DF1 PLC DLL: %s\n", err.GetReason());
			throw;
		}
	}

	Log( LOG_FN_ENTRY, "Exit PlcDf1::LaunchCommThread()\n");
}

/**
 * Reads the input channel configuration from the given node map
 *
 * @param inputNodes XmlNodeMap containing the setup nodes for the PLC inputs
 */
void PlcDf1::ReadInputConfiguration( const XmlNodeMap &inputNodes)
{
	Log( LOG_FN_ENTRY, "Enter PlcDf1::ReadInputConfiguration()\n");

	try
	{
		const XmlNode *addressNode = inputNodes.getNode( "Address");
		m_df1ThreadArg.readAddress = XmlToAscii( addressNode->getValue());
	}
	catch( XmlException &err)
	{
		m_df1ThreadArg.readAddress = "$N17";
	}

	Log( LOG_DEV_DATA, "Input Address: %s\n", m_df1ThreadArg.readAddress.c_str());

	Log( LOG_FN_ENTRY, "Exit PlcDf1::ReadInputConfiguration()\n");
}


/**
 * Reads the output channel configuration from the given node map
 *
 * @param outputNodes
 *               XmlNodeMap containing the setup nodes for the PLC outputs
 */
void PlcDf1::ReadOutputConfiguration( const XmlNodeMap &outputNodes)
{
	Log( LOG_FN_ENTRY, "Enter PlcDf1::ReadOutputConfiguration()\n");

	// Check for output channel PLC address
	try
	{
		const XmlNode *addressNode = outputNodes.getNode( "Address");
		m_df1ThreadArg.writeAddress = XmlToAscii( addressNode->getValue());
	}
	catch( XmlException &err)
	{
		m_df1ThreadArg.writeAddress = "$N16";
	}
	Log( LOG_DEV_DATA, "Output Address: %s\n", m_df1ThreadArg.writeAddress.c_str());

	Log( LOG_FN_ENTRY, "Exit PlcDf1::ReadOutputConfiguration()\n");
}

/**
 * Closes the object
 */
void PlcDf1::Close()
{
	Log( LOG_FN_ENTRY, "Enter PlcDf1::Close\n");

	// If comm thread is running
	if( m_df1ThreadArg.threadId != -1)
	{
		Log( LOG_FN_ENTRY, "Terminating comm thread\n");

		m_plc.Close();

		// Send comm thread a terminate pulse to shut it down
		m_df1ThreadArg.threadStop = true;
		MsgSendPulse( m_df1ThreadArg.coId, 62, SYSTEM_PULSE, TERMINATE_PULSE);

		// Wait for the thread to die
		pthread_join( m_df1ThreadArg.threadId, NULL);

		Log( LOG_FN_ENTRY, "Comm thread dead\n");
	}

	// De-allocate data array memory
	DestroyDataArrays();

	Log( LOG_FN_ENTRY, "Exit PlcDf1::Close\n");
}

/**
 * Reads the channel at the given index
 *
 * @param channelIdx Channel index to read
 * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
 * @return Value of the channel at the given index
 */
uint32_t PlcDf1::ReadChannel( uint32_t channelIdx, portDir_t portDir)
{
	uint32_t 			retVal;
	volatile uint32_t	*plcMem;
	int 				errVal;

	// Choose the proper buffer to read from
	if( portDir == PORT_DIR_IN)	plcMem = m_fromPlc;
	else						plcMem = m_toPlc;

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
			ThrowOsException( errVal, "Error attaining read lock in PlcDf1::ReadChannel");
		}
	}
	else
	{
		throw( BepException( "PlcDf1 not initialized"));
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
uint32_t PlcDf1::ReadPort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, portDir_t portDir)
{
	uint32_t	retVal;

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
		if( !(logicLvl))	retVal ^= 0x01;
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
void PlcDf1::WriteChannel( uint32_t channelIdx, uint32_t channelVal)
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
			ThrowOsException( errVal, "Error attaining write lock in PlcDf1::WriteChannel");
		}
	}
	else
	{
		throw( BepException( "PlcDf1 not initialized"));
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
void PlcDf1::WritePort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, uint32_t portVal)
{
	uint32_t	channelVal, dataMask = 1 << portIdx;

	// If this port does not occupy an entire channel
	if( portIdx != (uint16_t)-1)
	{
		// Read the current channel value
		channelVal = ReadChannel( channelIdx, PORT_DIR_OUT);

		// If inverse logic (positive false)
		if( !(logicLvl))
		{
			// Clear the bit
			if( portVal)	channelVal &= ~(dataMask);
			// Set the bit
			else			channelVal |= dataMask;
		}
		// If normal logic (positive true)
		else
		{
			// Clear the bit
			if( !(portVal))	channelVal &= ~(dataMask);
			// Set the bit
			else			channelVal |= dataMask;
		}
	}
	else
	{
		if( !(logicLvl))	channelVal = ~(portVal);
		else				channelVal = portVal;
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
PlcDf1& PlcDf1::operator=( const PlcDf1& rhs)
{
	if( this != &rhs)
	{
		Close();

		m_fromPlc = rhs.m_fromPlc;
		m_fromPlcCount = rhs.m_fromPlcCount;

		m_df1PortName = rhs.m_df1PortName;

		CreateDataArrays();

		SetLocalEcho( rhs.GetLocalEcho());
		SetProcessName( rhs.GetProcessName());
		SetVerboseMask( rhs.GetVerboseMask());
		if( rhs.GetLogStatus() == true)	EnableLogger();

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
void PlcDf1::CreateDataArrays()
{
	Log( LOG_FN_ENTRY, "Enter PlcDf1::CreateDataArrays()\n");
	if( (m_fromPlcCount > 0) && ( m_toPlcCount > 0))
	{
		DestroyDataArrays();
		m_fromPlc = new uint32_t[ m_fromPlcCount];
		memset( (void*)m_fromPlc, 0, m_fromPlcCount*sizeof( uint32_t));
		m_toPlc = new uint32_t[ m_toPlcCount];
		memset( (void*)m_toPlc, 0, m_toPlcCount*sizeof( uint32_t));
	}
	Log( LOG_FN_ENTRY, "Exit PlcDf1::CreateDataArrays()\n");
}

/**
 * De-allocates memory for the data arrays used to hold the PLC data
 */
void PlcDf1::DestroyDataArrays()
{
	Log( LOG_FN_ENTRY, "Enter PlcDf1::DestroyDataArrays()\n");

	if( m_fromPlc != NULL)	delete[] m_fromPlc;
	if( m_toPlc != NULL)	delete[] m_toPlc;

	m_fromPlc = m_toPlc = NULL;
	Log( LOG_FN_ENTRY, "Exit PlcDf1::DestroyDataArrays()\n");
}

/**
 * Expands environment variable names in a path name
 *
 * @param pathToResolve
 *               Path name to be resolved
 */
void PlcDf1::ResolvePathName( string &pathToResolve)
{
	// Make a copy of the original
	string::size_type	envStart, envEnd;
	char				*envValue;

	Log( LOG_FN_ENTRY, "Enter PlcDf1::ResolvePathName( %s)\n", pathToResolve.c_str());

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
	Log( LOG_FN_ENTRY, "Exit PlcDf1::ResolvePathName( %s)\n", pathToResolve.c_str());
}

INT32 PlcDf1::SystemWrite(const std::string &tag, const std::string &value)
{
	INT32 result = BEP_STATUS_ERROR;	// the result of the operation
	std::string response;

	try
	{
		result = m_ndb.Write(tag, value, response, true);
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "PlcDf1::SystemWrite %s Error: %s\n", tag.c_str(), e.what());
	}

	return(result);
}

/**
 * Thread entry point used to perform DF1 serial communication
 *
 * @param threadArg Pointer to a Df1ThreadArg_t data structure
 */
void* PlcDf1::Df1CommThread( void *threadArg)
{
	Df1ThreadArg_t		*arg = (Df1ThreadArg_t*)threadArg;
	int			  		chId, lockStat;
	PlcDf1              *parentPlc = arg->parent;
	timer_t				timerId;
	struct _itimer		timerTime;
	struct sigevent		timerEvent;
	struct _pulse 		timerPulse;
	pthread_rwlock_t	*rwLock = arg->rwLock;
	ILogger*		logger = arg->logger;
	IDf1Protocol        *serialPlc = arg->plc;
	const int 			toCount = arg->toCount, fromCount=arg->fromCount;
	uint32_t			toPlc[ toCount], fromPlc[ fromCount];
	std::string			faultRsp;
	uint8_t				faultSet=0;

	// Initialize
	logger->Log( LOG_FN_ENTRY, "Enter PlcDf1::Df1CommThread()\n");

	memset( toPlc, 0, toCount * sizeof( uint32_t));
	memset( fromPlc, 0, fromCount * sizeof( uint32_t));

	for( int ii=0; ii<toCount; ii++)	toPlc[ ii] = ii;

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
				timerTime.nsec = arg->nsecLoopTime;	timerTime.interval_nsec = 0;
				TimerSettime( timerId, 0, &timerTime, NULL);

				// Wait for a pulse (timer pulse or TERMINATE_PULSE)
				if( (lockStat=MsgReceivePulse( chId, &timerPulse, sizeof( timerPulse), NULL))  == 0)
				{
					// IF we know how to handle this type of pulse
					switch( timerPulse.code)
					{
					case SYSTEM_PULSE+1:
						// Read our inputs from the PLC
						if( serialPlc->Read( arg->df1DestId, fromPlc, fromCount, arg->readAddress.c_str()) == true)
						{
							if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
							{
								memmove( (uint32_t*)arg->fromPlc, fromPlc, fromCount*sizeof( uint32_t));
								pthread_rwlock_unlock( rwLock);
							}
							else
							{
								logger->Log( LOG_ERRORS, "\tDF1CommThread Error locking rwlock for read: %s\n",
									strerror( lockStat));
							}

							// If read fault is currently set
							if( faultSet & 0x01)
							{
								logger->Log( LOG_ERRORS, "\tDF1CommThread clearing PLCSerialReadFault\n");
								parentPlc->SystemWrite( "PLCSerialReadFault", "0");
								// Clear read fault
								faultSet &= ~(0x01);
							}
						}
						// IF read fault not currently set
						else if( !(faultSet & 0x01))
						{
							logger->Log( LOG_ERRORS, "\tDF1CommThread setting PLCSerialReadFault\n");
							parentPlc->SystemWrite( "PLCSerialReadFault", "1");
							// Set read fault
							faultSet |= 0x01;
						}

						// Write our outputs to the PLC
						if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
						{
                            memmove( toPlc, (uint32_t*)arg->toPlc, toCount * sizeof( uint32_t));
							pthread_rwlock_unlock( rwLock);
							if( serialPlc->Write( arg->df1DestId, toPlc, toCount, arg->writeAddress.c_str()) == true)
							{
							// If write fault currently set
								if( faultSet & 0x02)
								{
									logger->Log( LOG_ERRORS, "\tDF1CommThread clearing PLCSerialWriteFault\n");
									parentPlc->SystemWrite( "PLCSerialWriteFault", "0");
								// Clear write fault
								faultSet &= ~(0x02);
							}
						}
							// If write fault not currently set
							else if( !(faultSet & 0x02))
							{
								logger->Log( LOG_ERRORS, "\tDF1CommThread setting PLCSerialWriteFault\n");
								parentPlc->SystemWrite( "PLCSerialWriteFault", "1");
								// Set write fault
								faultSet |= 0x02;
							}
						}
						else
						{
							logger->Log( LOG_ERRORS, "\tDF1CommThread Error locking rwlock for write: %s\n",
											 strerror( lockStat));
						}
						break;
					case SYSTEM_PULSE:
						logger->Log( LOG_DEV_DATA, "DF1 Comm thread received SYSTEM_PULSE %d\n",
										 timerPulse.value.sival_int);
						if( timerPulse.value.sival_int == TERMINATE_PULSE)	arg->threadStop = true;

						break;
					default:
						logger->Log( LOG_DEV_DATA, "DF1 Comm thread received unexpected pulse: %d,%d\n",
										 timerPulse.code, timerPulse.value.sival_int);
						break;
					}
				}
				else
				{
					logger->Log( LOG_ERRORS, "\tDF1 Comm thread MsgReceive error: %s\n",
									 strerror( errno));
				}
			}
			logger->Log( LOG_DEV_DATA, "DF1 Comm thread received TERMINATE_PULSE...ending\n");
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

	logger->Log( LOG_FN_ENTRY, "Exit PlcDf1::Df1CommThread()\n");

	return( NULL);
}
