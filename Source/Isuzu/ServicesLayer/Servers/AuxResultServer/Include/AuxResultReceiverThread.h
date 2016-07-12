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
//     $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/Include/AuxResultReceiverThread.h $
// 
// 2     9/14/05 4:36p Bmeinke
// Fixed the spelling of m_te(e)stResultPort
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
// 
// 3     8/22/05 2:19p Bmeinke
// Changed ResultTagList_t to HLAResultFieldList_t and moved its
// implementation to a new header file
// Added a const string used to define the value BBK uses to indicate pass
// status
// 
// 2     8/19/05 4:33p Bmeinke
// Moved implementation from header file to cpp file
// 
// 1     8/18/05 3:43p Bmeinke
// Initial check-in (not compiled)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

class AuxResultReceiverThread;

#ifndef _AUX_RESULT_RECEIVER_THREAD_H_INCLUDED_
#define _AUX_RESULT_RECEIVER_THREAD_H_INCLUDED_

#include "AtomicThread.h"
#include "ILogger.h"
#include "ILogicalPort.h"
#include "BepSync.h"
#include "AuxResultServer.h"
#include "AuxResultFieldList.h"

/**
 * State definitions used to control the state of the receiver thread
 */
typedef enum
{
	/**
	 * Tell the thread to start/resume execution
	 */
	THREAD_CMD_RUN = 0x00,
	
	/**
	 * Thread is currently running
	 */
	THREAD_RUNNING = 0x01,

	/**
	 * Command the thread to pause execution
	 */
	THREAD_CMD_PAUSE = 0x02,
	
	/**
	 * Thread is currently paused
	 */
	THREAD_PAUSED = 0x03,

	/**
	 * Command the thread to stop
	 */
	THREAD_CMD_STOP = 0x04,
	
	/**
	 * Thread is stopped
	 */
	THREAD_STOPPED = 0x05
	
} AuxThreadState_t;

/**
 * Condvar used to control the state of the receiver thread
 */
typedef BepCondVar<AuxThreadState_t>	AuxThreadControl_t;

/**
 * Thread object used to receive serial test results from an external system
 */
class AuxResultReceiverThread : public AtomicThread , public ILogger
{
public:
	  /**
	   * Class constructor
	   * 
	   * @param auxServer Server object we will be working for.
	   * @param portName  Logical port to listen for results on
	   * @param detachState
	   *                  Set up detached state (PTHREAD_CREATE_JOINABLE, PTHREAD_CREATE_DETACHED)
	   */
	  AuxResultReceiverThread(AuxResultServer *auxServer, const std::string &portName, 
							  UINT32 detachState=PTHREAD_CREATE_JOINABLE);

	  /**
	   * Class destructor
	   */
	  virtual ~AuxResultReceiverThread();

	  /**
	   * Method used to initialize from a config file
	   * 
	   * @param document Reference to a parsed configuration document.
	   */
	  void Initialize(const XmlNode *document);
	  
	  /**
	   * Command (and wait for) the thread to stop
	   * 
	   * @return true if the thread was successfully stopped
	   */
	  bool Stop();
	
protected:
	/**
	 * Function to be executed when thread is run
	 *
	 * @return Status
	 * @since Version 1.0
	 */
	virtual void *Execute(void);

	/**
	 * Checks to see what state the thread should transition to (in
	 * case we have been requested to stop or pause)
	 * 
	 * @param threadState
	 *               Currently requested thread state
	 * 
	 * @return The state the thread should transition to
	 */
	virtual AuxThreadState_t GetNextThreadState( AuxThreadState_t threadState);

private:

	/**
	 * Reference to the server we are working for
	 */
	AuxResultServer				*m_auxServer;
	
	/**
	 * ILogicalPort object used to receive the serial test results
	 */
	ILogicalPort				m_testResultPort;

	/**
	 * Cond var used to control the thread's execution
	 */
	AuxThreadControl_t			m_threadControl;

	/**
	 * Serial port to receive data on
	 */
	std::string					m_portName;
	
};
#endif // _AUX_RESULT_RECEIVER_THREAD_H_INCLUDED_

