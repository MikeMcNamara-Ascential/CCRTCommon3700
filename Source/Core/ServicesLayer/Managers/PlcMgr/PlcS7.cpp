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
#include <errno.h>
#include <sync.h>           // BARRIER_SERIAL_THREAD
#include <sys/netmgr.h>     // ND_LOCAL_NODE
#include "PlcS7.h"
#include "BepException.h"

//-------------------------------------------------------------------------------------------------
PlcS7::PlcS7() : IPlc(), m_fromPlc(NULL), m_toPlc(NULL), m_commScanTime(20), m_ndb(NULL)
{
	Log(LOG_FN_ENTRY, "PlcS7::PlcS7() - Enter");
	pthread_rwlock_init(&m_plcLock, NULL);
	m_channelBits = 16;
	Log(LOG_FN_ENTRY, "PlcS7::PlcS7() - Exit");
}

//-------------------------------------------------------------------------------------------------
PlcS7::PlcS7(const PlcS7 &copy) : IPlc(copy), m_fromPlc(NULL), m_toPlc(NULL), m_commScanTime(20)
{
	pthread_rwlock_init(&m_plcLock, NULL);
	*this = copy;
}

//-------------------------------------------------------------------------------------------------
PlcS7::~PlcS7()
{
	Log(LOG_FN_ENTRY, "PlcS7::~PlcS7() - Enter");
	Close();
	pthread_rwlock_destroy(&m_plcLock);
	if(m_ndb != NULL)  delete m_ndb;
	Log(LOG_FN_ENTRY, "PlcS7::~PlcS7() - Exit");
}

//-------------------------------------------------------------------------------------------------
bool PlcS7::Open(uint32_t inputCount, uint32_t outputCount, bool demoMode/*=false*/)
{
	bool retVal = true;
	Log(LOG_FN_ENTRY, "PlcS7::Open() - Enter");
	m_fromPlcCount = inputCount;
	m_toPlcCount = outputCount;
	CreateDataArrays();
	try
	{
		LaunchCommThread();
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "Error launching S7 PLC comm thread - %s", excpt.GetReason());
		retVal = false;
	}
	Log(LOG_FN_ENTRY, "PlcS7::Open() - Exit");
	return retVal;
}

//-------------------------------------------------------------------------------------------------
void PlcS7::Initialize(const XmlNode *plcSetup)
{
	Log(LOG_FN_ENTRY, "PlcS7::Initialize() - Enter");
	LoadAdditionalConfigurationItems(plcSetup);
	if (m_ndb == NULL)
    {
           m_ndb = new INamedDataBroker(false);
    }
	Log(LOG_FN_ENTRY, "PlcS7::Initialize() - Exit");
}

//-------------------------------------------------------------------------------------------------
void PlcS7::ReadInputConfiguration(const XmlNodeMap &inputNodes)
{
	m_s7ThreadArg.fromPlcDbNum = BposReadInt(inputNodes.getNode("DbNumber")->getValue().c_str());
}

//-------------------------------------------------------------------------------------------------
void PlcS7::ReadOutputConfiguration(const XmlNodeMap &outputNodes)
{
	m_s7ThreadArg.toPlcDbNum = BposReadInt(outputNodes.getNode("DbNumber")->getValue().c_str());
}

//-------------------------------------------------------------------------------------------------
void PlcS7::Close()
{
	Log(LOG_FN_ENTRY, "PlcS7::Close() - Enter");
	if(m_s7ThreadArg.threadId != -1)
	{
		Log(LOG_DEV_DATA, "Terminating S7 Comm Thread");
		m_plc.Close();
		m_s7ThreadArg.threadStop = true;
		MsgSendPulse(m_s7ThreadArg.connectionId, 62, SYSTEM_PULSE, TERMINATE_PULSE);
		pthread_join(m_s7ThreadArg.threadId, NULL);
		Log(LOG_FN_ENTRY, "S7 Comm Thread Terminated");
	}
	DestroyDataArrays();
	Log(LOG_FN_ENTRY, "PlcS7::Close() - Exit");
}

//-------------------------------------------------------------------------------------------------
uint32_t PlcS7::ReadChannel( uint32_t channelIdx, portDir_t portDir/*=PORT_DIR_IN*/)
{
	uint32_t            retVal;
	volatile uint32_t   *plcMem;
	int                 errVal;

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
			ThrowOsException( errVal, "Error attaining read lock in PlcEip::ReadChannel");
		}
	}
	else
	{
		throw( BepException( "PlcEip not initialized"));
	}

	return( retVal);
}

//-------------------------------------------------------------------------------------------------
uint32_t PlcS7::ReadPort(uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, portDir_t portDir/*=PORT_DIR_IN*/)
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

//-------------------------------------------------------------------------------------------------
void PlcS7::WriteChannel(uint32_t channelIdx, uint32_t channelVal)
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
			ThrowOsException( errVal, "Error attaining write lock in PlcS7::WriteChannel");
		}
	}
	else
	{
		throw( BepException( "PlcS7 not initialized"));
	}
}

//-------------------------------------------------------------------------------------------------
void PlcS7::WritePort(uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, uint32_t portVal)
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

//-------------------------------------------------------------------------------------------------
PlcS7& PlcS7::operator=( const PlcS7& rhs)
{
	if( this != &rhs)
	{
		Close();

		m_fromPlc = rhs.m_fromPlc;
		m_fromPlcCount = rhs.m_fromPlcCount;

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

//-------------------------------------------------------------------------------------------------
void PlcS7::LoadAdditionalConfigurationItems(const XmlNode *plcSetup)
{
	Log(LOG_DEV_DATA, "PlcS7::LoadAdditionalConfigurationItems() - Enter");
	ILogger::Initialize(plcSetup); 
	try
	{
		ReadInputConfiguration(plcSetup->getChild("Inputs")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error reading input config - %s", excpt.GetReason());
	}
	try
	{
		ReadOutputConfiguration(plcSetup->getChild("Outputs")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error reading output config - %s", excpt.GetReason());
	}
	try
	{
		m_commScanTime = BposReadInt(plcSetup->getChild("ScanTime")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error setting scan time - %s", excpt.GetReason());
	}
	m_plc.Initialize(plcSetup);
	Log(LOG_DEV_DATA, "PlcS7::LoadAdditionalConfigurationItems() - Exit");
}

//-------------------------------------------------------------------------------------------------
INT32 PlcS7::SystemWrite(const string &tag, const string &value)
{
	INT32 result = BEP_STATUS_ERROR;	// the result of the operation
	string response;

	try
	{
		result = m_ndb->Write(tag, value, response, true);
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "PlcS7::SystemWrite %s Error: %s\n", tag.c_str(), e.what());
	}

	return(result);
}

//-------------------------------------------------------------------------------------------------
void PlcS7::LaunchCommThread(void) throw( BepException)
{
	string errStr;
	int         status;

	Log( LOG_FN_ENTRY, "Enter PlcS7::LaunchCommThread()");

	try
	{
		pthread_barrier_init( &m_s7ThreadArg.barrier, NULL, 2);
		m_s7ThreadArg.parent = this;
		m_s7ThreadArg.fromPlc = m_fromPlc; m_s7ThreadArg.fromPlcCount = m_fromPlcCount;
		m_s7ThreadArg.toPlc = m_toPlc;     m_s7ThreadArg.toPlcCount = m_toPlcCount;
		m_s7ThreadArg.plc = &m_plc;
		m_s7ThreadArg.logger = (ILogger *)this;
		m_s7ThreadArg.rwLock = (pthread_rwlock_t*)&m_plcLock;
		m_s7ThreadArg.nsecLoopTime = mSEC_nSEC( m_commScanTime);
		m_s7ThreadArg.threadStop = false;

		pthread_create( &m_s7ThreadArg.threadId, NULL, &PlcS7::S7CommThread, (void*)&m_s7ThreadArg);
		// Wait for serial comm thread to start
		status = pthread_barrier_wait( &m_s7ThreadArg.barrier);
		if( (status != BARRIER_SERIAL_THREAD) && (status != 0))
		{
			errStr = "S7 comm thread init error: ";
			errStr += strerror( status);
			throw( BepException( errStr));
		}
		Log( LOG_DEV_DATA, "S7 comm thread launched with thread id of %d",
			 m_s7ThreadArg.threadId);
		// Wait for the first scan to complete
		delay( m_commScanTime*2);
	}
	catch( BepException &err)
	{
		Log( LOG_ERRORS, "\tError initializing S7 PLC DLL: %s", err.GetReason());
		throw;
	}


	Log( LOG_FN_ENTRY, "Exit PlcS7::LaunchCommThread()");
}

//-------------------------------------------------------------------------------------------------
void PlcS7::CreateDataArrays()
{
	Log( LOG_FN_ENTRY, "Enter PlcS7::CreateDataArrays()");
	if( (m_fromPlcCount > 0) && ( m_toPlcCount > 0))
	{
		DestroyDataArrays();
		m_fromPlc = new uint32_t[ m_fromPlcCount];
		memset( (void*)m_fromPlc, 0, m_fromPlcCount*sizeof( uint32_t));
		m_toPlc = new uint32_t[ m_toPlcCount];
		memset( (void*)m_toPlc, 0, m_toPlcCount*sizeof( uint32_t));
	}
	Log( LOG_FN_ENTRY, "Exit PlcS7::CreateDataArrays()");
}

//-------------------------------------------------------------------------------------------------
void PlcS7::DestroyDataArrays()
{
	Log( LOG_FN_ENTRY, "Enter PlcS7::DestroyDataArrays()");

	if( m_fromPlc != NULL)	delete[] m_fromPlc;
	if( m_toPlc != NULL)	delete[] m_toPlc;

	m_fromPlc = m_toPlc = NULL;
	Log( LOG_FN_ENTRY, "Exit PlcS7::DestroyDataArrays()");
}

//-------------------------------------------------------------------------------------------------
void* PlcS7::S7CommThread( void *threadArg)
{
	S7ThreadArg_t      *arg = (S7ThreadArg_t*)threadArg;
	int                 chId, lockStat;
	PlcS7              *parentPlc = arg->parent;
	timer_t             timerId;
	struct _itimer      timerTime;
	struct sigevent     timerEvent;
	struct _pulse       timerPulse;
	pthread_rwlock_t    *rwLock = arg->rwLock;
	ILogger*            logger = arg->logger;
	S7Protocol         *serialPlc = arg->plc;
	const int           toCount = arg->toPlcCount, fromCount=arg->fromPlcCount;
	uint32_t            toPlc[ toCount], fromPlc[ fromCount];
	std::string         faultRsp;
	uint8_t             faultSet=0;

	// Initialize
	logger->Log( LOG_FN_ENTRY, "Enter PlcS7::S7CommThread()");

	memset( toPlc, 0, toCount * sizeof( uint32_t));
	memset( fromPlc, 0, fromCount * sizeof( uint32_t));

	for( int ii=0; ii<toCount; ii++)	toPlc[ ii] = ii;

	// Create a channel and connection to receive timer pulses on
	if( (timerId=BposCreateTimerWithPulse( &chId, &arg->connectionId, SYSTEM_PULSE+1)) != -1)
	{
		// Send a timer pulse to do an immediate read
		MsgSendPulse( arg->connectionId, -1, SYSTEM_PULSE+1, 0);

		// Wait at barrier
		logger->Log( LOG_DEV_DATA, "\tWaiting at barrier");
		lockStat = pthread_barrier_wait( &arg->barrier);
		if( (lockStat == BARRIER_SERIAL_THREAD) || (lockStat == 0))
		{
			// Loop until TERMINATE_PULSE received
			logger->Log( LOG_DEV_DATA, "\tLooping (chId=%d, coId=%d", chId, arg->connectionId);
			while( false == arg->threadStop)
			{
				// Bump my priority up higher
				BposSetThreadPriority( getpid(), pthread_self(), 35);

				// Initialize the event struct to deliver us a pulse (instead of just unblocking)
				SIGEV_PULSE_INIT( &timerEvent, arg->connectionId, 50, SYSTEM_PULSE+1, 0);

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
						if( serialPlc->Read(fromPlc, fromCount, arg->fromPlcDbNum) == true)
						{
							if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
							{
								memmove( (uint32_t*)arg->fromPlc, fromPlc, fromCount*sizeof( uint32_t));
								pthread_rwlock_unlock( rwLock);
							}
							else
							{
								logger->Log( LOG_ERRORS, "\tS7CommThread Error locking rwlock for read: %s",
											 strerror( lockStat));
							}

							// If read fault is currently set
							if( faultSet & 0x01)
							{
								logger->Log( LOG_ERRORS, "\tS7CommThread clearing PLCSerialReadFault");
								parentPlc->SystemWrite( "PLCSerialReadFault", "0");
								// Clear read fault
								faultSet &= ~(0x01);
							}
						}
						// IF read fault not currently set
						else if( !(faultSet & 0x01))
						{
							logger->Log( LOG_ERRORS, "\tS7CommThread setting PLCSerialReadFault");
							parentPlc->SystemWrite( "PLCSerialReadFault", "1");
							// Set read fault
							faultSet |= 0x01;
						}

						// Write our outputs to the PLC
						if( (lockStat=pthread_rwlock_wrlock( rwLock)) == EOK)
						{
							memmove( toPlc, (uint32_t*)arg->toPlc, toCount * sizeof( uint32_t));
							pthread_rwlock_unlock( rwLock);
							if( serialPlc->Write(toPlc, toCount, arg->toPlcDbNum) == true)
							{
								// If write fault currently set
								if( faultSet & 0x02)
								{
									logger->Log( LOG_ERRORS, "\tS7CommThread clearing PLCSerialWriteFault");
									parentPlc->SystemWrite( "PLCSerialWriteFault", "0");
									// Clear write fault
									faultSet &= ~(0x02);
								}
							}
							// If write fault not currently set
							else if( !(faultSet & 0x02))
							{
								logger->Log( LOG_ERRORS, "\tS7CommThread setting PLCSerialWriteFault");
								parentPlc->SystemWrite( "PLCSerialWriteFault", "1");
								// Set write fault
								faultSet |= 0x02;
							}
						}
						else
						{
							logger->Log( LOG_ERRORS, "\tS7CommThread Error locking rwlock for write: %s",
										 strerror( lockStat));
						}
						break;
					case SYSTEM_PULSE:
						logger->Log( LOG_DEV_DATA, "S7 Comm thread received SYSTEM_PULSE %d",
									 timerPulse.value.sival_int);
						if( timerPulse.value.sival_int == TERMINATE_PULSE)	arg->threadStop = true;

						break;
					default:
						logger->Log( LOG_DEV_DATA, "S7 Comm thread received unexpected pulse: %d,%d",
									 timerPulse.code, timerPulse.value.sival_int);
						break;
					}
				}
				else
				{
					logger->Log( LOG_ERRORS, "\tS7 Comm thread MsgReceive error: %s",
								 strerror( errno));
				}
			}
			logger->Log( LOG_DEV_DATA, "S7 Comm thread received TERMINATE_PULSE...ending");
		}
		else
		{
			logger->Log( LOG_ERRORS, "\tError #%d waiting at barrier: %s",
						 lockStat, strerror( lockStat));
		}
		timerTime.nsec = timerTime.interval_nsec = 0;
		TimerSettime( timerId, 0, &timerTime, NULL);
		TimerDestroy( timerId);
		ConnectDetach( arg->connectionId);
		ChannelDestroy( chId);

		arg->connectionId = -1;
		chId = -1;
	}
	else
	{
		logger->Log( LOG_ERRORS, "\tError creating thread timer channel/connection");
	}
	pthread_barrier_destroy( &arg->barrier);

	logger->Log( LOG_FN_ENTRY, "Exit PlcS7::EipCommThread()");

	return( NULL);
}
