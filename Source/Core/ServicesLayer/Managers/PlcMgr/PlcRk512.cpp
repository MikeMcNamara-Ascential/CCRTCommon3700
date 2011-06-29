//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcRk512.cpp $
// $Author:: Rwiersem                   $Date:: 2/01/06                                                   $
//
// Description:
//    Siemens RK512 PLC Board Object.
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcRk512.cpp $
// 
// 1     1/18/07 2:51p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
// 
//
//******************************************************************************

/*******************
 include files
 ******************/
#include <errno.h>
#include <sync.h>			// BARRIER_SERIAL_THREAD
#include <sys/netmgr.h>		// ND_LOCAL_NODE
#include "PlcRk512.h"
#include "BepException.h"

/**
 * Default constructor
 */
PlcRk512::PlcRk512() : IPlc(),
	m_fromPlc( NULL),  m_toPlc( NULL), m_rk512PortName( ""),
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
PlcRk512::PlcRk512( const PlcRk512 &copy) : IPlc( copy),
	m_fromPlc( NULL),  m_toPlc( NULL), m_rk512PortName( ""),
	m_commScanTime( 20)
{
	// Initialize the read/write lock
	pthread_rwlock_init( &m_plcLock, NULL);
	
	*this = copy;
}

/**
 * Default destructor
 */
PlcRk512::~PlcRk512()
{
	Log( LOG_FN_ENTRY, "Enter PlcRk512::~PlcRk512\n");
	Close();
	
	// Destroy the read/write lock
	pthread_rwlock_destroy( &m_plcLock);
	Log( LOG_FN_ENTRY, "Exit PlcRk512::~PlcRk512\n");
}

/**
 * Opens a RK512 serial PLC board
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
bool PlcRk512::Open(uint32_t inputCount, uint32_t outputCount, bool demoMode)
{
	bool	retVal = true;

	Log( LOG_FN_ENTRY, "Enter PlcRk512::Open()\n");
	

	Log( LOG_FN_ENTRY, "PlcRk512::Open() inputCount=0x%02x\n, outputCount=0x%02x", inputCount, outputCount);
	m_fromPlcCount = inputCount;
	m_toPlcCount = outputCount;
	
	CreateDataArrays();

	try
	{
		// Launch the RK512 comm thread
		LaunchCommThread( m_rk512PortName);
	}
	catch( BepException &err)
	{
		Log( LOG_ERRORS, "\tError launching RK512 comm thread: %s\n",
			 err.GetReason());
		retVal = false;
	}
	Log( LOG_FN_ENTRY, "Exit PlcRk512::Open()\n");
	
	return( retVal);
}

/**
 * Initializes the underlying PLC object
 *
 * @param plcSetup XML Parent Node containing PLCBoard configuration
 */
void PlcRk512::Initialize( const XmlNode *plcSetup)
{
	Log( LOG_FN_ENTRY, "Enter PlcRk512::Initialize( %s)\n", plcSetup->getName().c_str());
	
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
        const XmlString& tmp = plcSetup->getChild( XML_T("Inputs/Address"))->getValue();
        m_rk512ThreadArg.readAddress = BposReadInt( XmlToAscii( tmp).c_str());
	}
	catch( XmlException &err)
	{
		Log( LOG_ERRORS, "No Input configuration specified in config file\n");
        m_rk512ThreadArg.readAddress = 200;
	}

	// Read the output configuration
	try
	{
		// Read output configuration
        const XmlString& tmp = plcSetup->getChild( XML_T("Outputs/Address"))->getValue();
		m_rk512ThreadArg.writeAddress = BposReadInt( XmlToAscii( tmp).c_str());
	}
	catch( XmlException &err)
	{
		Log( LOG_ERRORS, "No Output configuration specified in config file\n");
        m_rk512ThreadArg.writeAddress = 201;
	}

	// Read the name of the logical port to talk on
	try
	{
		const XmlString& tmp = plcSetup->getChild( XML_T("LogicalPort"))->getValue();
		m_rk512PortName = XmlToAscii( tmp);
	}
	catch( XmlException &err)
	{
		m_rk512PortName = "PlcRk512";
	}
	Log( LOG_DEV_DATA, "RK512 PLC Logical Port Name: %s\n", m_rk512PortName.c_str());

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
	Log( LOG_DEV_DATA, "RK512 PLC Scan Time: %d ms\n", m_commScanTime);

	Log( LOG_FN_ENTRY, "Exit PlcRk512::Initialize()\n");
}

/**
 * Launches the RK512 serial comm thread (used to update PLC data arrays
 * via serial messages to and from the RK512 PLC). Alos opens the logical
 * port we need to use for PLC communication and loads/initializes the
 * appropriate PLC DLL.
 *
 * @param portName Name of the logical port to communicate with the PLC on
 */
void PlcRk512::LaunchCommThread( const std::string &portName) throw( BepException)
{
	std::string	errStr;
	int			status;

	Log( LOG_FN_ENTRY, "Enter PlcRk512::LaunchCommThread()\n");
	
	// Try to open the serial prot
	if( m_serCom.OpenPort( portName, O_RDWR) == false)
	{
		errStr = "Unable to open RK512 logical port: ";
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
			if( (status=m_plc.Open( m_serCom.GetPortFd())) != EOK)
			{
				Log( LOG_ERRORS, "\tm_plc.PlcOpen() failed: %s\n", strerror(status));
				throw( BepException( "Unable to initialize RK512 DLL: %s"), strerror(status) );
			}

			pthread_barrier_init( &m_rk512ThreadArg.barrier, NULL, 2);
			m_rk512ThreadArg.fromPlc = m_fromPlc;
			m_rk512ThreadArg.fromCount = m_fromPlcCount;
			m_rk512ThreadArg.toPlc = m_toPlc;
			m_rk512ThreadArg.toCount = m_toPlcCount;
			m_rk512ThreadArg.plc = &m_plc;
			m_rk512ThreadArg.logger = (ILogger *)this;
			m_rk512ThreadArg.rwLock = (pthread_rwlock_t*)&m_plcLock;
			m_rk512ThreadArg.nsecLoopTime = mSEC_nSEC( m_commScanTime);

			pthread_create( &m_rk512ThreadArg.threadId, NULL, &PlcRk512::Rk512CommThread, &m_rk512ThreadArg);
			// Wait for serial comm thread to start
			status = pthread_barrier_wait( &m_rk512ThreadArg.barrier);
			if( (status != BARRIER_SERIAL_THREAD) && (status != 0))
			{
				errStr = "RK512 comm thread init error: ";
				errStr += strerror( status);
				throw( BepException( errStr));
			}
			Log( LOG_DEV_DATA, "RK512 comm thread launched with thread id of %d\n",
				 m_rk512ThreadArg.threadId);
            // Wait for the first scan to complete
            delay( m_commScanTime*2);
		}
		catch( BepException &err)
		{
			Log( LOG_ERRORS, "\tError initializing RK512 PLC DLL: %s\n", err.GetReason());
			throw;
		}
	}
	Log( LOG_FN_ENTRY, "Exit PlcRk512::LaunchCommThread()\n");
}

/**
 * Reads the input channel configuration from the given node map
 *
 * @param inputNodes XmlNodeMap containing the setup nodes for the PLC inputs
 */
void PlcRk512::ReadInputConfiguration( const XmlNodeMap &inputNodes)
{
	Log( LOG_FN_ENTRY, "Enter PlcRk512::ReadInputConfiguration()\n");
	
	try
	{
		const XmlNode *addressNode = inputNodes.getNode( "Address");
		m_rk512ThreadArg.readAddress = atoi(addressNode->getValue().c_str());
	}
	catch( XmlException &err)
	{
        m_rk512ThreadArg.readAddress = 201;
	}
	
    Log( LOG_DEV_DATA, "Input Address: %d\n", m_rk512ThreadArg.readAddress);
	
	Log( LOG_FN_ENTRY, "Exit PlcRk512::ReadInputConfiguration()\n");
}


/**
 * Reads the output channel configuration from the given node map
 *
 * @param outputNodes
 *               XmlNodeMap containing the setup nodes for the PLC outputs
 */
void PlcRk512::ReadOutputConfiguration( const XmlNodeMap &outputNodes)
{
	Log( LOG_FN_ENTRY, "Enter PlcRk512::ReadOutputConfiguration()\n");
	
	// Check for output channel PLC address
	try
	{
		const XmlNode *addressNode = outputNodes.getNode( "Address");
		m_rk512ThreadArg.writeAddress = atoi( addressNode->getValue().c_str());
	}
	catch( XmlException &err)
	{
        m_rk512ThreadArg.writeAddress = 200;
	}
    Log( LOG_DEV_DATA, "Output Address: %d\n", m_rk512ThreadArg.writeAddress);
	
	Log( LOG_FN_ENTRY, "Exit PlcRk512::ReadOutputConfiguration()\n");
}

/**
 * Closes the object
 * 
 */
void PlcRk512::Close()
{
	Log( LOG_FN_ENTRY, "Enter PlcRk512::Close\n");
	
	// If comm thread is running
	if( m_rk512ThreadArg.threadId != -1)
	{
		Log( LOG_FN_ENTRY, "Terminating comm thread\n");
		
		// Send comm thread a terminate pulse to shut it down
		MsgSendPulse( m_rk512ThreadArg.coId, -1, SYSTEM_PULSE, TERMINATE_PULSE);

		// Wait for the thread to die
		pthread_join( m_rk512ThreadArg.threadId, NULL);
		
		Log( LOG_FN_ENTRY, "Comm thread dead\n");
	}
	m_plc.Close();

	// De-allocate data array memory
	DestroyDataArrays();

	Log( LOG_FN_ENTRY, "Exit PlcRk512::Close\n");
}

/**
 * Reads the channel at the given index
 *
 * @param channelIdx Channel index to read
 * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
 * @return Value of the channel at the given index
 */
uint32_t PlcRk512::ReadChannel( uint32_t channelIdx, portDir_t portDir)
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
				Log( LOG_DEV_DATA, "PlcRk512::ReadChannel() channelIdx=%02d, plcMem[channelIdx]=0x%04x\n", channelIdx, plcMem[channelIdx]);
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
			ThrowOsException( errVal, "Error attaining read lock in PlcRk512::ReadChannel");
		}
	}
	else
	{
		throw( BepException( "PlcRk512 not initialized"));
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
uint32_t PlcRk512::ReadPort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, portDir_t portDir)
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
void PlcRk512::WriteChannel( uint32_t channelIdx, uint32_t channelVal)
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
			ThrowOsException( errVal, "Error attaining write lock in PlcRk512::WriteChannel");
		}
	}
	else
	{
		throw( BepException( "PlcRk512 not initialized"));
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
void PlcRk512::WritePort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, uint32_t portVal)
{
	Log( LOG_FN_ENTRY, "Enter PlcRk512::WritePort(): channelIdx = %d, portIdx = %d, logicLvl = %d, portVal = 0x%04x\n", channelIdx, portIdx, logicLvl, portVal);
	uint32_t	channelVal, dataMask = 1 << portIdx;

	// If this port does not occupy an entire channel
	if( portIdx != (uint16_t)-1)
	{
		// Read the current channel value
		channelVal = ReadChannel( channelIdx, PORT_DIR_OUT);
		Log( LOG_FN_ENTRY, "PlcRk512::WritePort() ReadChannel(%d) = %04x\n", channelIdx, channelVal);
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
	Log( LOG_FN_ENTRY, "PlcRk512::WritePort() WriteChannel(%d, 0x%04x)\n", channelIdx, channelVal);
	// Write data to the specified port
	WriteChannel( channelIdx, channelVal);
}

/**
 * Assignment operator. The newly copied object WILL NOT be opened automatically
 *
 * @param rhs    Object to be copied
 * @return this
 */
PlcRk512& PlcRk512::operator=( const PlcRk512& rhs)
{
	if( this != &rhs)
	{
		Close();

		m_fromPlc = rhs.m_fromPlc;
		m_fromPlcCount = rhs.m_fromPlcCount;

		m_rk512PortName = rhs.m_rk512PortName;

		CreateDataArrays();

		SetLocalEcho( rhs.GetLocalEcho());
		SetProcessName( rhs.GetProcessName());
		SetVerboseMask( rhs.GetVerboseMask());
		if( rhs.GetLogStatus() == true)	EnableLogger();

		// Destroy the existing read/write lock
		pthread_rwlock_destroy( &m_plcLock);

		// Initialize the read/write lock
		pthread_rwlock_init( &m_plcLock, NULL);

	}
	return( *this);
}

/**
 * Allocates memory for the data arrays used to hold the PLC data
 */
void PlcRk512::CreateDataArrays()
{
	Log( LOG_FN_ENTRY, "Enter PlcRk512::CreateDataArrays()\n");
	if( (m_fromPlcCount > 0) && ( m_toPlcCount > 0))
	{
		DestroyDataArrays();
		m_fromPlc = new uint32_t[ m_fromPlcCount];
		memset( (void*)m_fromPlc, 0, m_fromPlcCount*sizeof( uint32_t));
		m_toPlc = new uint32_t[ m_toPlcCount];
		memset( (void*)m_toPlc, 0, m_toPlcCount*sizeof( uint32_t));
	}
	Log( LOG_FN_ENTRY, "Exit PlcRk512::CreateDataArrays()\n");
}

/**
 * De-allocates memory for the data arrays used to hold the PLC data
 */
void PlcRk512::DestroyDataArrays()
{
	Log( LOG_FN_ENTRY, "Enter PlcRk512::DestroyDataArrays()\n");
	
	if( m_fromPlc != NULL)	delete[] m_fromPlc;
	if( m_toPlc != NULL)	delete[] m_toPlc;

	m_fromPlc = m_toPlc = NULL;
	Log( LOG_FN_ENTRY, "Exit PlcRk512::DestroyDataArrays()\n");
}

/**
 * Expands environment variable names in a path name
 *
 * @param pathToResolve
 *               Path name to be resolved
 */
void PlcRk512::ResolvePathName( string &pathToResolve)
{
	// Make a copy of the original
	string::size_type	envStart, envEnd;
	char				*envValue;

	Log( LOG_FN_ENTRY, "Enter PlcRk512::ResolvePathName( %s)\n", pathToResolve.c_str());
	
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
	Log( LOG_FN_ENTRY, "Exit PlcRk512::ResolvePathName( %s)\n", pathToResolve.c_str());
}

/**
 * Thread entry point used to perform RK512 serial communication
 *
 * @param threadArg Pointer to a Rk512ThreadArg_t data structure
 */
void* PlcRk512::Rk512CommThread( void *threadArg)
{
	Rk512ThreadArg_t	*arg = (Rk512ThreadArg_t*)threadArg;
	int			  		chId, lockStat;
	timer_t				timerId;
	struct _itimer		timerTime;
	struct sigevent		timerEvent;
	struct _pulse 		timerPulse;
	bool		  		threadStop=false;
	pthread_rwlock_t	*rwLock = arg->rwLock;
	ILogger*			logger = arg->logger;
	
	//const int 			toCount = arg->toCount;
	//const int			fromCount=arg->fromCount;
	const int				readAddress = arg->readAddress;
	const int				writeAddress = arg->writeAddress;
	
	arg->logger->Log( LOG_FN_ENTRY, "Enter PlcRk512::Rk512CommThread() arg->toCount=0x%02x, arg->fromCount=0x%02x, arg->readAddress=0x%02x, arg->writeAddress=0x%02x\n", arg->toCount, arg->fromCount, arg->readAddress, arg->writeAddress);

	//TEMP
	const int			toCount = 0x1A;
	const int			fromCount = 0x1C;
	//const int			readAddress = 0xC9;
	//const int			writeAddress = 0xC8;
	//TEMP
	
	uint32_t			toPlc[ toCount], fromPlc[ fromCount];
	IFaultServer		faultIfc;
	std::string			faultRsp;
	uint8_t				faultSet=0;

	// Initialize
	arg->logger->Log( LOG_FN_ENTRY, "Enter PlcRk512::Rk512CommThread()\n");
	
	memset( toPlc, 0, toCount * sizeof( uint32_t));
	memset( fromPlc, 0, fromCount * sizeof( uint32_t));
	
	for( int ii=0; ii<toCount; ii++)	toPlc[ ii] = ii;

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
			logger->Log( "\tError initializing  fault interface in RK512Comm Thread: %s\n",
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
                        // TODO: update 0 to valid offset
						logger->Log(LOG_DEV_DATA, "PlcRk512: Read() fromCount = %d, readAddress = %d\n", fromCount, readAddress);
						if( arg->plc->Read(fromPlc, fromCount, readAddress, 0) > 0)
						{
							logger->Log( LOG_DEV_DATA, "RK512CommThread fromPlc[0] = 0x%04x, fromPlc[1] = 0x%04x, fromPlc[2] = 0x%04x\n", fromPlc[0], fromPlc[1], fromPlc[2]);
							if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
							{
								memmove( (uint32_t*)arg->fromPlc, fromPlc, fromCount*sizeof( uint32_t));
								pthread_rwlock_unlock( rwLock);
							}
							else
							{
								logger->Log( LOG_ERRORS, "\tRK512CommThread Error locking rwlock for read: %s\n",
									strerror( lockStat));
							}

							// If read fault is currently set
							if( faultSet & 0x01)
							{
								logger->Log( LOG_ERRORS, "\tRK512CommThread clearing RK512ReadFault\n");
								try
								{
									faultIfc.ClearFault( "RK512ReadFault", faultRsp, true);
								}
								catch( BepException &err)
								{
									logger->Log( LOG_ERRORS, "\tException clearing RK512ReadFault: %s\n",
												 err.GetReason());
								}
								catch( ...)
								{
									logger->Log( LOG_ERRORS, "\tUnknown exception clearing RK512ReadFault\n");
								}
								// Clear read fault
								faultSet &= ~(0x01);
							}
						}
						// IF read fault not currently set
						else if( !(faultSet & 0x01))
						{
							logger->Log( LOG_ERRORS, "\tRK512CommThread setting RK512ReadFault\n");
							try
							{
								faultIfc.SetFault( "RK512ReadFault", "Error reading RK512 response",
												   "PLC", faultRsp, true, 3);
							}
							catch( BepException &err)
							{
								logger->Log( LOG_ERRORS, "\tException setting RK512ReadFault: %s\n",
											 err.GetReason());
							}
							catch( ...)
							{
								logger->Log( LOG_ERRORS, "\tUnknown exception setting RK512ReadFault\n");
							}
							// Set read fault
							faultSet |= 0x01;
						}

						// Write our outputs to the PLC
						if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
						{
                            memmove( toPlc, (uint32_t*)arg->toPlc, toCount * sizeof( uint32_t));
							pthread_rwlock_unlock( rwLock);
							//if( arg->plc->Write( arg->rk512DestId, toPlc, toCount, arg->writeAddress.c_str()) <= 0)
                            
							logger->Log(LOG_DEV_DATA, "PlcRk512: Write() toCount = 0x%02x, writeAddress = 0x%02x\n", toCount, writeAddress);
							
							if( arg->plc->Write(toPlc, toCount, writeAddress, 0) <= 0)
							{
								logger->Log( LOG_ERRORS, "\tRK512CommThread setting RK512WriteFault\n");
								
								// If write fault not currently set
								if( !(faultSet & 0x02))
								{
									try
									{
										faultIfc.SetFault( "RK512WriteFault", "Error writing RK512 data",
														   "PLC", faultRsp, true, 3);
									}
									catch( BepException &err)
									{
										logger->Log( LOG_ERRORS, "\tException setting RK512WriteFault: %s\n",
													 err.GetReason());
									}
									catch( ...)
									{
										logger->Log( LOG_ERRORS, "\tUnknown exception setting RK512WriteFault\n");
									}
									// Set write fault
									faultSet |= 0x02;
								}
							}
							// If write fault currently set
							else if( faultSet & 0x02)
							{
								logger->Log( LOG_ERRORS, "\tRK512CommThread clearing RK512WriteFault\n");
								try
								{
									faultIfc.ClearFault( "RK512WriteFault", faultRsp, true);
								}
								catch( BepException &err)
								{
									logger->Log( LOG_ERRORS, "\tException clearing RK512WriteFault: %s\n",
												 err.GetReason());
								}
								catch( ...)
								{
									logger->Log( LOG_ERRORS, "\tUnknown exception elearing RK512WriteFault\n");
								}
								// Clear write fault
								faultSet &= ~(0x02);
							}
						}
						else
						{
							logger->Log( LOG_ERRORS, "\tRK512CommThread Error locking rwlock for write: %s\n",
											 strerror( lockStat));
						}
						break;
					case SYSTEM_PULSE:
						logger->Log( LOG_DEV_DATA, "RK512 Comm thread received SYSTEM_PULSE %d\n",
										 timerPulse.value.sival_int);
						if( timerPulse.value.sival_int == TERMINATE_PULSE)	threadStop = true;

						break;
					default:
						logger->Log( LOG_DEV_DATA, "RK512 Comm thread received unexpected pulse: %d,%d\n",
										 timerPulse.code, timerPulse.value.sival_int);
						break;
					}
				}
				else
				{
					logger->Log( LOG_ERRORS, "\tRK512 Comm thread MsgReceive error: %s\n",
									 strerror( errno));
				}
			}
			logger->Log( LOG_DEV_DATA, "RK512 Comm thread received TERMINATE_PULSE...ending\n");
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
	
	logger->Log( LOG_FN_ENTRY, "Exit PlcRk512::Rk512CommThread()\n");

	return( NULL);
}
