//*************************************************************************
// Description:
//   Interface to the Windows based CCRT system.
//   This class allows reading and writting data from the Windows based 
//   CCRT system so all data available to the QNX CCRT system can be available
//   to the Windows based CCRT system.  This is mainly intended to assist
//   the migration of CCRT from QNX to Windows.
//
//===========================================================================
// Copyright (c) 2015- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef WinPendantInterface_h
#define WinPendantInterface_h
//-------------------------------------------------------------------------------------------------
#include "BepServer.h"
#include "INamedDataBroker.h"
//-------------------------------------------------------------------------------------------------
#include <arpa/inet.h>
#include <netdb.h>          // socket stuff
#include <sys/neutrino.h>
#include <sys/types.h>      // socket stuff
#include <sys/socket.h>     // socket stuff
#include <netinet/tcp.h>    // socket stuff
#include <netinet/in.h>     // socket stuff
#include <pthread.h>
//-----------------------------------------------------------------------------
class WinPendantInterface : public BepServer
{
public:

	WinPendantInterface();
	virtual ~WinPendantInterface();

	/**
	 * Open a socket and start litening for a connection.
	 * 
	 * @param port  port number to begin listening on.
	 * @param server  Pointer to the object that instantiated this thread.
	 * 
	 * @return The opened socket to listen on.
	 */
	int OpenSocket(int port, WinPendantInterface* server);

	/**
	 * Process the message received from the Windows CCRT system.
	 * 
	 * @param message Message received from the Windows CCRT system.
	 * 
	 * @return Status of processing the Windows CCRT message.
	 */
	BEP_STATUS_TYPE ProcessWinCcrtMessage(string &message, int serSocket);

	/**
	 * <b>Description:</b>
	 * Method that is called to command the component to perform any system
	 * registration functions necessary.
	 *
	 * @return BEP_SUCCESS_RESPONSE if the registration was successful
	 *         BEP_FAILURE_RESPONSE if the registration was not successful
	 */
	virtual const string Register(void);

	/**
	 * Read the value of the specified system tag from the system.
	 * 
	 * @param tag    System data tag to read.
	 * 
	 * @return Value of the requested system data tag.
	 */
	string SystemRead(string &tag);

	/**
	 * Write the specified value to the specified system data tag.
	 * 
	 * @param tag    System data tag to write.
	 * @param value  Value to write to the system data tag.
	 * 
	 * @return Status of writing the value to the system data tag.
	 */
	BEP_STATUS_TYPE SystemWrite(const string &tag, const string &value);

	/**
	 * Thread to listen for incoming connections and handle incoming messages.
	 * 
	 * @param arg  Pointer to the server that created the TCP thread.
	 */
	void* TcpThread( void* arg );

	/**
	 * Function to kick off the TCP thread.
	 * 
	 * @param context  Pointer to the server that created the TCP thread.
	 */
	static void *TcpThread_helper(void *context);





protected:





private:

	// INamedDataBroker object to use for reading and writing system level data tags
	INamedDataBroker *m_ndb;
	// Notes entered by the operator.
	string m_notes;
	// Belt tensions received from the floor interface system
	XmlNodeMap m_beltTensions;

};

#endif //WinPendantInterface_h
