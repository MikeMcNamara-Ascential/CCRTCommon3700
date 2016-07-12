//******************************************************************************
// Description:
//  Thread used to receive a serial test result string from an external tester
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/Library/AuxResultReceiverThread.cpp $
// 
// 3     3/06/06 3:41p Bmeinke
// Added some delays in the receiver thread to avoid tight loops when the
// system is shutting down
// 
// 2     9/14/05 4:35p Bmeinke
// Need to call ILogger() constructor
// Added more Log() statements
// Lowered the ReadPort() timeouts from 500/50 to 100/10
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
// 
// 3     8/22/05 2:17p Bmeinke
// Fixed syntax errors for replacing spaces in the VIN used for the parent
// result node tag
// 
// 2     8/22/05 2:14p Bmeinke
// Changed the ResultTagList_t type to HLAResultFieldList_t
// Added a const string to define the BBK pass field value
// Changed the underlying data type of the HLAResultFieldList_t to a
// HLAResultField object
// Convert pass/fail fields from "1"/"0" to "Pass"/"Fail"
// Prepend "VIN_" to the beginning of the HLA result node tag (the VIN) to
// avoid the possibility of a VIN being an invalid XML node tag
// Replace any spaces in the VIN string used as the parent node tag with
// underscores
// 
// 1     8/19/05 4:33p Bmeinke
// Moved implementation from header file to cpp file
// 
// 1     8/18/05 3:43p Bmeinke
// Initial check-in (not compiled)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "AuxResultReceiverThread.h"
#include <dlfcn.h>

AuxResultReceiverThread::AuxResultReceiverThread(AuxResultServer *auxServer, 
												 const std::string &portName,
												 UINT32 detachState /*= PTHREAD_CREATE_JOINABLE */) : 
	AtomicThread(detachState), ILogger(), m_auxServer( auxServer), m_portName( portName)
{
}

AuxResultReceiverThread::~AuxResultReceiverThread()
{
	Stop();
}

void AuxResultReceiverThread::Initialize(const XmlNode *document)
{
	ILogger::Initialize( document);
}

bool AuxResultReceiverThread::Stop()
{
	bool    	threadStopped = false;
	uint64_t    timeOut = mSEC_nSEC( 5000LL);

	Log( LOG_FN_ENTRY, "Enter AuxResultReceiverThread::Stop()\n");

	// Lock the cond var
	if( EOK == m_threadControl.Acquire())
	{
		// Check if currently stopped
		if( THREAD_STOPPED != m_threadControl.GetValue())
		{
			// Tell the thread to stop
			if( EOK == m_threadControl.Signal( THREAD_CMD_STOP))
			{
				// Wait (up to 5 sec) for the thread to stop
				if( EOK == m_threadControl.Wait( THREAD_STOPPED, timeOut))
				{
					threadStopped = true;
				}
			}
		}
		else
		{
			threadStopped = true;
		}

		m_threadControl.Release();
	}
	Log( LOG_FN_ENTRY, "Exit AuxResultReceiverThread::Stop(%s)\n", threadStopped ? "true" : "false");

	return( threadStopped);
}

void* AuxResultReceiverThread::Execute(void)
{
	uint8_t             tempBuff[ 1024];
	int                 bytesRead;
	std::string         resultBuff, resultString;
	AuxThreadState_t    oldState, threadState = THREAD_RUNNING;

	Log( LOG_FN_ENTRY, "Enter AuxResultReceiverThread::Execute()\n");
	
	// Try to open the port
	if( true != m_testResultPort.OpenPort(m_portName))
	{
		// Couldn't open the port, so terminate
		threadState = THREAD_CMD_STOP;
		Log( LOG_ERRORS, "Error opening port <%s>...terminating\n", m_portName.c_str());
	}

	// Publish our current state
	Log( LOG_DEV_DATA, "AuxResultReceiverThread signalling state=%d\n", threadState);
	if( EOK == (errno=m_threadControl.Acquire()))
	{
		m_threadControl.Signal( threadState);
		m_threadControl.Release();
	}
	else
	{
		Log( LOG_ERRORS, "AuxResultReceiverThread error locking m_threadControl: %s\n", strerror( errno));
	}

	Log( LOG_DEV_DATA, "AuxResultReceiverThread entering loop\n");
	// Loop until told to stop
	while( THREAD_CMD_STOP != threadState)
	{
		oldState = threadState;
		// If we are in a RUNNING state
		if( THREAD_RUNNING == threadState)
		{
			/**
			 * Try to read data from the port waiting up to 100 msec for the first
			 * byte and up to 10 msec for each consecutive byte
			 */
			bytesRead = m_testResultPort.ReadPort( tempBuff, sizeof( tempBuff), 100, 10);
			if( bytesRead > 0)
			{
				// Add this data to the result buffer
				resultBuff += std::string( (char*)tempBuff, bytesRead);

				// Publish this new result
				if( NULL != m_auxServer)
				{
					m_auxServer->ProcessStream( resultBuff);
				}
			}
			else
			{
				BposSleep( 100);
			}
		}
		else if( (THREAD_CMD_PAUSE == threadState) || (THREAD_PAUSED == threadState))
		{
			// Wait 500 msec while paused
			BposSleep( 500);
		}
		else
		{
			BposSleep( 100);
		}

		// Check to see if we should terminate or pause
		threadState = GetNextThreadState( threadState);
		if( oldState != threadState)
		{
			Log( LOG_DEV_DATA, "AuxResultReceiverThread state transition from %d to %d\n", oldState, threadState);
		}
	}

	// Close the port
	Log( LOG_DEV_DATA, "AuxResultReceiverThread closing port\n");
	m_testResultPort.ClosePort();

	// Say we are stopped
	Log( LOG_DEV_DATA, "AuxResultReceiverThread signalling THREAD_STOPPED state\n");
	m_threadControl.Acquire();
	m_threadControl.Signal( THREAD_STOPPED);
	m_threadControl.Release();

	Log( LOG_FN_ENTRY, "Exit AuxResultReceiverThread::Execute()\n");
	
	return( NULL);
}

AuxThreadState_t AuxResultReceiverThread::GetNextThreadState( AuxThreadState_t threadState)
{
	// Check to see if we should terminate or pause
	if( EOK == m_threadControl.Acquire())
	{
		threadState = m_threadControl.GetValue();
		switch( threadState)
		{
		case THREAD_CMD_RUN:
			threadState = THREAD_RUNNING;
			m_threadControl.Signal( threadState);
			break;
		case THREAD_CMD_PAUSE:
			threadState = THREAD_PAUSED;
			m_threadControl.Signal( threadState);
			break;
		case THREAD_CMD_STOP:
		default:
			break;
		}

		m_threadControl.Release();
	}

	return( threadState);
}

