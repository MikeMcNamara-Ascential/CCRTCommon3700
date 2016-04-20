//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/SystemCommunication/IInterProcessCommunication.cpp 3     4/13/06 3:47a Cward $
//
// FILE DESCRIPTION:
//   This defines the interface for system communication objects.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/SystemCommunication/IInterProcessCommunication.cpp $
// 
// 3     4/13/06 3:47a Cward
// Init some vars
//
// 2     2/15/06 3:58p Bmeinke
// Initialize some uninitialized variables
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 2     10/24/03 1:02p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:52p Khykin
// Initial checkin.
//
// 1     4/22/03 9:33a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:43  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:26a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 72    10/21/02 11:31a Khykin
// Updated the Write method to not timeout if there was not a timeout
// value provided.
//
// 71    10/08/02 8:02a Khykin
// Updating debugging information.
//
// 70    10/07/02 7:16a Khykin
// Updating the Write for changes to the timeout strategy.
//
// 69    7/31/02 10:50a Skypig13
// Using SignalCheck() to monitor interruptions
//
// 68    4/25/02 4:36p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:05  erickson
//    started
//
//
// 66    2/05/02 1:44p Skypig13
// Removed warnings
//
// 65    2/05/02 12:07a Skypig13
// Updated to remove compiler warnings and restructure
//
// 64    12/19/01 3:23p Skypig13
// Imporved diagnostics in some methods to include name and type
//
// 63    11/20/01 4:27p Kh
// Updating the handle pulse method.
//
// 62    11/01/01 3:31p Dan.erickson
// Added name to Initialize method prototype for
// IInterProcessCommunication.
//
// 61    10/11/01 4:44p Skypig13
// Took out print statement in MsgSenvs() loop in method Write()
//
// 60    10/10/01 10:45a Brian.meinke
// Changed 'Chanel' inside Initialize() to 'Channel'
//
// 59    10/09/01 1:46p Skypig13
// Put diagnostics in Write() and Reply()
//
// 58    10/04/01 9:26a Skypig13
// Fixed Read() so that buffer reading reply works properly
//
// 57    10/03/01 4:51p Skypig13
// Remove do/while around BposReceive() since it is done inside
// BposReceive()
//
// 56    10/03/01 4:06p Brian.meinke
// Only do a GetBuffer() inside Read() if we received a message and not a
// pulse
//
// 55    10/02/01 9:30a Skypig13
// moved timeout to ISystemCommunication
//
// 54    9/25/01 2:16p Dan.erickson
// In destructor: Invalidate the id before deleting the buffer and added a
// delay as a result of testing the thread pool class.
//
// 53    9/17/01 8:52a Skypig13
// Added pthread_testcancel() to all loops around interruptable
// kernel calls
//
// 52    9/11/01 4:29p Kh
// Updating the EINTR handling.
//
// 51    8/30/01 4:15p Skypig13
// Restored default parameters
//
// 50    8/28/01 9:49p Skypig13
// Update initializers
//
// 48    8/23/01 9:24a Skypig13
// Removed annoying printout.
//
// 46    8/21/01 3:54p Kh
// Updating the ProcessRequest method.
//
// 44    8/21/01 2:13p Skypig13
// Still testing
//
// 43    8/21/01 12:00p Skypig13
// Fixed destructor so channel is closed properly
//
// 42    8/21/01 8:41a Skypig13
// Added closing channel on exit
//
// 41    8/17/01 4:42p Skypig13
// Put retries around blocking functions if they are interrupted
// (MsgReceive() and MsgSendvs())
//
// 40    7/24/01 6:43p Kh
// Changing the validation of the name_attach, in SetupChannel.  Will now
// use BP_ERR_KRNL_FAIL.
//
// 39    7/24/01 11:18a Skypig13
// Made Write() throw exception on error
//
// 38    7/19/01 4:22p Skypig13
// Made valid range for id >= 0
//
// 37    7/19/01 3:24p Skypig13
// Unregister name before creating registered name for channel
//
// 36    7/18/01 4:29p Skypig13
// Fixed another bug in SetRxLength()
//
// 35    7/18/01 4:14p Skypig13
// Fix bug in SetRxLength()
//
// 34    7/18/01 3:29p Skypig13
// Added dosconnect notification
//
// 33    7/17/01 7:01p Skypig13
// Add pulse handling
//
// 32    7/17/01 8:56a Skypig13
//
// 31    7/11/01 5:53p Skypig13
// Change from multi-part response to fixed and made minimum buffer size =
// 1024 so that object can handle large messages in response.  Parameter
// for buffer size will allow for configurable message sizes.
//
// 30    7/07/00 1:32p Skypig13
// Changed return value for response read to 0.
//
// 29    7/03/00 4:39p Skypig13
// Changed printout in Read to properly print the message received
//
// 28    6/26/00 2:21p Skypig13
// Return value of ProcessRequest() = BEP_STATUS_ERROR instead of -1
//
// 27    6/22/00 10:40a Skypig13
// Changes to intitialize(int, int, bool) and added reply id to
// class and change functionality to use accessor methods
//
// 26    6/21/00 5:48p Skypig13
// Testing functionality of IPC and OpenChannel tests.
// Made some interface changes
//
// 25    6/20/00 2:37p Skypig13
// Fix printf with CheckId()
//
// 24    6/15/01 4:19p Skypig13
// Added CheckId() useage
//
// 23    6/15/01 4:00p Skypig13
// Created helper functions to make derived IOpenChannelIpc class
// easier to impliment
//
// 22    6/13/01 7:23p Admin
// Change Boolean to bool
//
// 21    6/13/01 7:06p Skypig13
// Modify to add ProcessRequest()
//
// 20    6/13/01 3:52p Skypig13
// If Read() fails for any reason it will return -1 instead of raising an
// exception
//
// 19    6/13/01 3:23p Skypig13
// Add functionality to connect to an open channel that has
// already been unblocked.  Restructured Read() and added
// ReadSendersBuffer().
//
// 18    6/11/01 4:30p Skypig13
// Added functionality  connecting to a channel already open
// by another IPC object
//
// 17    6/05/01 8:54a Skypig13
// Fix a few diagnostic printout bugs
//
// 16    6/04/00 1:40p Kh
// Made an ordering mistake on the move.
//
// 14    5/31/01 9:37a Skypig13
// Change cout to printf for thread-safe operation and added Inititalize
// with descrete parameters for type, name and debug
//
// 13    5/24/01 4:17p Skypig13
// Safe backup
//
// 12    5/22/01 10:20a Skypig13
// Finish testing
//
// 11    5/21/01 3:34p Skypig13
// Add try/catch around SetDebug() in Intitialize() to make 'Debug' node
// optional
//
// 10    5/21/01 3:28p Skypig13
// Modified Ack setup in Initialize(), Reply(), and Write()
//
// 9     5/21/01 2:43p Skypig13
// Put BposRegisterTaskName() inside class.
//
// 8     5/17/01 5:26p Skypig13
// Weekly
//
// 7     5/17/01 11:16a Skypig13
// Fixed so sending to any channel Id works
//
// 6     5/14/01 1:05p Skypig13
// Add multi-part messages done
//
// 5     5/11/01 2:54p Skypig13
// Add multi-part messages.  Working on multi-part reply
//
// 2     5/04/01 12:50p Skypig13
// Initial work on class
//
// 1     5/04/01 7:06a Skypig13
// Class for performing Inter-Process COmmunication
//
//*************************************************************************
#include <errno.h>
#include <stdio.h>
#include <typeinfo>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/siginfo.h>
#include <sys/neutrino.h>

#include "Bepos.h"
#include "GeneralUtilities.h"
#include "IInterProcessCommunication.h"
#include "ISystemCommunication.h"

IInterProcessCommunication::IInterProcessCommunication(void) : ISystemCommunication(),
m_rxBuffer(NULL), m_rxLength(0), m_readChannel(true), m_reply(false), m_nameStruct(NULL),
m_pulseHandled(true), m_replyId(-1), m_disconnect(false)
{
}
IInterProcessCommunication::IInterProcessCommunication(const XmlNode *config) : ISystemCommunication(),
m_rxBuffer(NULL), m_rxLength(0), m_readChannel(true), m_reply(false), m_nameStruct(NULL),
m_pulseHandled(true), m_replyId(-1), m_disconnect(false)
{
	Initialize(config);
}
IInterProcessCommunication::IInterProcessCommunication(const INT32 id, const std::string name,
													   const bool debug	/* = false */,
													   const INT32 rid /* = -1 */,
													   const INT32 size	/* = DEFAULT_BUFFER_SIZE */,
													   const INT32 timeout /* = DEFAULT_TIMEOUT */) :
ISystemCommunication(), m_rxBuffer(NULL), m_rxLength(0), m_name(name), m_readChannel(true),
m_reply(false), m_nameStruct(NULL), m_pulseHandled(true), m_replyId(-1), m_disconnect(false)
{
	Initialize(id, name, debug, rid, size, timeout);
}
IInterProcessCommunication::IInterProcessCommunication(const std::string name, const std::string type,
													   const bool debug	/* = false */,
													   const INT32 size	/* = DEFAULT_BUFFER_SIZE */,
													   const INT32 timeout /* = DEFAULT_TIMEOUT */) :
ISystemCommunication(), m_rxBuffer(NULL), m_rxLength(0), m_name(name), m_readChannel(true),
m_reply(false), m_nameStruct(NULL), m_pulseHandled(true), m_replyId(-1), m_disconnect(false)
{
	Initialize(name, type, debug, size, timeout);
}
IInterProcessCommunication::~IInterProcessCommunication(void)
{	// Get rid of memory
	if(IsDebugOn())	printf("~IInterProcessCommunication('%s'%s) id = %d, rid = %d\n", GetType().c_str(), GetName().c_str(), GetId(), GetReplyId());
	InvalidateId();	// Close channel and set Id to invalid
	if(m_rxBuffer) delete[] m_rxBuffer;
//	if(IsDebugOn())	printf("~IInterProcessCommunication done\n");
}
void IInterProcessCommunication::Initialize(const std::string &name, const std::string &type,
											const bool debug /* = false */,
											const INT32 size /* = DEFAULT_BUFFER_SIZE */,
											const INT32 timeout	/* = DEFAULT_TIMEOUT */)
{
	SetDebug(debug);				// Set debug status
	if(type != "") SetType(type);	// Set Client/Server
	if(name != "") SetName(name);	// Set process name
	SetTimeout(timeout);			// Set write timeout
	SetRxLength(size);				// Set size of buffer
	string commType = "IPC";
	CommType(&commType);
}
void IInterProcessCommunication::Initialize(const XmlNode *config, const INT32 id /* =-1 */, const INT32 rid /* =-1 */)
{
	INT32 size = DEFAULT_BUFFER_SIZE, timeout = DEFAULT_TIMEOUT;
	try
	{	// Set debug status
		SetDebug(config->getChildren().getNode("Debug")->getValue());
	}
	catch(XmlException &XmlErr)
	{	// If node does not exist, leave set to false (off)
		SetDebug(false);
	}
	try
	{	// Set buffer size
		size = atoi(config->getChildren().getNode("BufferSize")->getValue().c_str());
	}
	catch(XmlException &XmlErr)
	{	// If node does not exist, leave set 1024
	}
	try
	{	// Set timeout
		timeout = atol(config->getChildren().getNode("Timeout")->getValue().c_str());
	}
	catch(XmlException &XmlErr)
	{	// If node does not exist, leave set 1024
	}
	if(id == -1)
	{
		Initialize(config->getChildren().getNode("Name")->getValue(),
				   config->getChildren().getNode("Type")->getValue(),
				   IsDebugOn(), size, timeout);
	}
	else Initialize(id, config->getChildren().getNode("Name")->getValue(), IsDebugOn(), rid, size, timeout);
	string commType = "IPC";
	CommType(&commType);
}
void IInterProcessCommunication::Initialize(const INT32 id, const std::string name, const bool debug /* = false */,
											const INT32 rid	/* =-1 */, const INT32 size	/* = DEFAULT_BUFFER_SIZE */,
											const INT32 timeout	/* = DEFAULT_TIMEOUT */)
{	// Set it all up
	SetDebug(debug);				// Set debug status
	SetType(std::string("Worker"));	// Set type
	SetId(id);						// Set Id
	SetReplyId(rid);				// Set Id of who needs to be replied to
	SetName(name, false);
	SetTimeout(timeout);			// Set write timeout
	SetRxLength(size);				// Set size of receive buffer
	string commType = "IPC";
	CommType(&commType);
}
INT32 IInterProcessCommunication::Read(std::string &message, INT32 id /*=-1*/)
{	// Receive message
	INT32 bytes = 0, retVal;	// Id of sender, number of bytes waiting
	CheckId(id);
	if(ShouldReadChannel())
	{	// Read from the IPC channel opened
		if(IsDebugOn())	printf("%s'%s' Read channel %d\n", GetType().c_str(), GetName().c_str(), id);
		retVal = ProcessRequest();
		try
		{
			if(GetReplyId() > 0)
			{	// If an IPC was received
				if(IsDebugOn())	printf("%s'%s' Read() got header.type = %d, bytes = %d, reply id = %d\n",
									   GetType().c_str(), GetName().c_str(), m_header.type, bytes, retVal);
				ReadSendersBuffer(message, sizeof(m_header.io_write), GetReplyId());
			}
			else if(GetReplyId() == 0)
			{	// A pulse was received
			}
			else HandleError("Read()");
		}
		catch(...)
		{
			SetReplyId(-1);
			retVal = -1;
		}
	}
	else
	{	// Read data from a Reply()
		SetReadChannel(true);	// Reset flag to read channel
		message = GetBuffer();	// Get data if only resding the buffer
		retVal = 0;
	}
	if(IsDebugOn())	printf("%s'%s' Read(%s) reply id = %d\n", GetType().c_str(), GetName().c_str(), message.c_str(), GetReplyId());
	return(retVal);
}
INT32 IInterProcessCommunication::ReadSendersBuffer(std::string &message, const INT32 offset, INT32 id)
{	// Receive message
	INT32 bytes;
	iov_t iov;		// Placeholder for data
	CheckId(id);
	try
	{
		std::string source = GetType() + std::string("'") + GetName() + std::string("' ");
		if(offset == 0)
		{
			SETIOV(&iov, &m_header, sizeof(m_header));	// Set up structure to receive the msg
			if(MsgReadv(id, &iov, 1, 0) == -1)			// Get message
			{
				source += "ReadSendersBuffer()->MsgReadv1()";
				HandleError(source.c_str());
			}
			bytes = m_header.io_write.nbytes;
			if(IsDebugOn())	printf("%s'%s' ReadSendersBuffer() got header.type = %d, bytes = %d, id = %d\n",
								   GetType().c_str(), GetName().c_str(), m_header.type,
								   bytes, id);
		}
		SETIOV(&iov, m_rxBuffer, GetRxLength());	// Set up structure to receive the msg
		ClearBuffer();
		if((bytes = MsgReadv(id, &iov, 1, sizeof(m_header.io_write))) == -1)	 // Get message
		{
			source += "ReadSendersBuffer()->MsgReadv2()";
			HandleError(source.c_str());
		}
		SetReply(true);			// Write operation should reply to sender
		SetPulseHandled(true);	// No need to handle pulse
		message = GetBuffer();
	}
	catch(...)
	{
		bytes = -1;
	}
	if(IsDebugOn())	printf("%s'%s' ReadSendersBuffer(%s)\n", GetType().c_str(), GetName().c_str(), GetBuffer().c_str());
	return(bytes);
}
void IInterProcessCommunication::Write(const std::string &message, INT32 id	/*=-1*/)
{
	if(!ShouldReply())
	{
		if(IsDebugOn())
			printf("%s'%s' Write(%s, %d)\n", GetType().c_str(), GetName().c_str(), message.c_str(), id);
		INT32 stat = 0;							// Create status varible for operation and a counter
		iov_t       iov[2];									// Placeholder for data
		header_t    wHdr;									// Message write information

		SETIOV(&iov[0], &wHdr, sizeof(wHdr));				// Set up structure to send the msg
		SETIOV(&iov[1], message.c_str(), message.size());	// Set up structure to send the msg
		wHdr.type   = _IO_WRITE;							// Indicate a message write
		wHdr.io_write.nbytes = message.size();				// How many bytes are we sending
		CheckId(id);										// Make sure id is valid
		ClearBuffer();										// Clear receive buffer

		INT32 interruptCount = 10;							// interruption counter
		INT32 timeoutCount = 10;  							// timeout counter
		sigevent event;										// Event structure informing what to do if timed out
		uint64_t timeout = GetNanoTimeout();					// Get timeout value
		event.sigev_notify = SIGEV_UNBLOCK;					// Unblock if time out
		struct timeval timeVal[2];
		do
		{
			// Set up timeout for send and reply operations
            if(IsDebugOn()) printf("Waiting for the response to message: %s\n", message.c_str());
			if(IsDebugOn()) printf("Setting the timeout to: %lld\n", timeout);
			// if a timeout value has been set
			if(timeout) stat = TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_SEND | _NTO_TIMEOUT_REPLY , &event, &timeout, NULL);
			if(stat != -1)
			{   // Set up timeout for send and reply operations
				stat = MsgSendvs(id, iov, 2, m_rxBuffer, GetRxLength());	// Send message
				if(IsDebugOn())
					printf("Message received: %d, %d\n", stat, errno);
			}
			else if(IsDebugOn())	printf("Had A TimerTimeout Error: %s\n", strerror(errno));
			pthread_testcancel();											// If in a thread get out if canceled
		}
		while( (SignalCheck(stat, EINTR, interruptCount, IsDebugOn()) == false) || 		// over if interrupted
			   (SignalCheck(stat, ETIMEDOUT, timeoutCount, IsDebugOn()) == false) );	// or a timeout

		SetReadChannel(false);					// Dont't read channel with next Read(), just read buffer

		if(stat == -1)
		{
			if(IsDebugOn() && (errno == ETIMEDOUT))
			{
				gettimeofday(&timeVal[1], NULL);
				printf("Current system time: %d.%d.  delta: %d.%d\n", timeVal[1].tv_sec, timeVal[1].tv_usec, timeVal[1].tv_sec - timeVal[0].tv_sec, timeVal[1].tv_usec - timeVal[0].tv_usec);
			}
			HandleError("Write()");	// If failed, handle it
		}
		else if(IsDebugOn())	printf("%s'%s' Write(%s) id = %d done\n", GetType().c_str(), GetName().c_str(), message.c_str(), id);
	}
	else Reply(message, id);	// Must reply
}
const INT32 IInterProcessCommunication::SendPulse(const INT32 code, const INT32 value, INT32 priority /* = 100 */, INT32 id	/* = -1 */)
{
	INT32 stat = BEP_STATUS_ERROR;
	CheckId(id);	// Validate id
	if(IsDebugOn())	printf("%s'%s' SendPulse(code: %d, value: %d, priority: %d, Id: %d)\n",
						   GetType().c_str(), GetName().c_str(), code, value, priority, id);
	if(priority < sched_get_priority_min(SCHED_FIFO))
	{	// Priority too low
		priority = sched_get_priority_min(SCHED_FIFO);
	}
	else if(priority > sched_get_priority_max(SCHED_FIFO))
	{	// priority too high
		priority = sched_get_priority_max(SCHED_FIFO);
	}

	if((code >= _PULSE_CODE_MINAVAIL) || (code <= _PULSE_CODE_MAXAVAIL))
	{
		stat = MsgSendPulse(id, priority, code, value);
		if(stat != -1)
		{
			stat = BEP_STATUS_SUCCESS;
		}
		else if(IsDebugOn()) perror("MsgSendPulse failed: ");
	}
	else throw BepException("Invalid pulse code: %d.  Valid range is %d <= code <= %d\n",
							code, _PULSE_CODE_MINAVAIL, _PULSE_CODE_MAXAVAIL);
	return(stat);
}
INT32 IInterProcessCommunication::ProcessRequest(void)
{
	try
	{
		bool shouldBreak;
		do
		{
			shouldBreak = true;	// set to true each time through
			char *header = (char *)&m_header;
//			SetReplyId(BposReceive(GetId(), (char *)&m_header, sizeof(m_header)));
			SetReplyId(BposReceive(GetId(), header, sizeof(m_header)));
			if(IsDebugOn())	printf("Process Request Received Message: %d\n", GetReplyId());
			switch(GetReplyId())
			{
			case -1:
				HandleError("ProcessRequest()");
				break;
			case 0:
				memcpy(&m_pulse, &m_header, sizeof(m_pulse));
				if(HandlePulse(m_pulse))
				{	// If pulse was handled and channel was disconnected break out
					shouldBreak = WasDisconnected();
				}
				break;
			default:
				if(IsDebugOn())	printf("%s'%s' ProcessRequest() there are %d bytes waiting at chid %d\n",
									   GetType().c_str(), GetName().c_str(), m_header.io_write.nbytes, GetReplyId());
				break;
			}
		}
		while(!shouldBreak);
	}
	catch(...)
	{
		SetReplyId(-1);
	}
	return(GetReplyId());
}
INT32 IInterProcessCommunication::ProcessRequestSpecial(void)
{
	try
	{
		bool shouldBreak;
        SetReplyId(BposReceive(GetId(), (char *)&m_header, sizeof(m_header)));
        if(IsDebugOn())	printf("Process Request Special Received Message: %d\n", GetReplyId());
        switch(GetReplyId())
        {
        case -1:
            HandleError("ProcessRequest()");
            break;
        case 0:
            memcpy(&m_pulse, &m_header, sizeof(m_pulse));
            if(HandlePulse(m_pulse))
            {	// If pulse was handled and channel was disconnected break out
                shouldBreak = WasDisconnected();
            }
            break;
        default:
            if(IsDebugOn())	printf("%s'%s' ProcessRequestSpecial() there are %d bytes waiting at chid %d\n",
                                   GetType().c_str(), GetName().c_str(), m_header.io_write.nbytes, GetReplyId());
            break;
        }
	}
	catch(...)
	{
		SetReplyId(-1);
	}
	return(GetReplyId());
}

void IInterProcessCommunication::Reply(const std::string &message, INT32 id	/*=-1*/)
{
	try
	{
		if(IsDebugOn())	printf("%s'%s' Reply(%s, %d)\n", GetType().c_str(), GetName().c_str(), message.c_str(), id);
		if(BposReply(GetReplyId(), (char *)message.c_str(), message.size()) != -1)	// Send data
		{
			if(IsDebugOn())	printf("%s'%s' Reply() success!\n", GetType().c_str(), GetName().c_str());
		}
		else HandleError("Reply()");	// Handle error (throws exception)
	}
	catch(...)
	{
	}
	SetReply(false);
}
INT32 IInterProcessCommunication::GetPulse(Pulse &pulse)
{
	memset(&pulse, 0, sizeof(Pulse));
	memcpy(&pulse, &m_pulse, sizeof(Pulse));
	return(WasPulseHandled());
}


const INT32 IInterProcessCommunication::GetPulseCode(void)
{
    return(m_pulse.code);
}

const INT32 IInterProcessCommunication::GetPulseValue(void)
{
    return(m_pulse.value.sival_int);
}

const bool IInterProcessCommunication::IsBepPulse(void)
{
    return(GetPulseCode() == SYSTEM_PULSE);
}

UINT32 IInterProcessCommunication::GetRxLength(void)
{
    return(m_rxLength);
}

const std::string IInterProcessCommunication::GetName(void)
{
    return(m_name);
}


const INT32 IInterProcessCommunication::GetReplyId(void)
{
    return(m_replyId);
}

const bool IInterProcessCommunication::WasDisconnected(void)
{
    return(m_disconnect);
}

void IInterProcessCommunication::SetDisconnected(const bool status)
{
    m_disconnect = status;
}

void IInterProcessCommunication::Close(void) const
{
    InvalidateId();
}

void IInterProcessCommunication::SetRxLength(UINT32 len)
{
	if(len < DEFAULT_BUFFER_SIZE)
		len = DEFAULT_BUFFER_SIZE;	// Must be able to hold _pulse struct
	if(len > GetRxLength())
	{
		if(IsDebugOn())	printf("%s'%s' SetRxLength(%d)\n", GetType().c_str(),
							   GetName().c_str(), len);
		if(m_rxBuffer)			 delete[] m_rxBuffer;	// Delete old data if it exists
		m_rxBuffer = new char[len + 1];					// Make buffer
		m_rxLength = len;								// Set length of buffer
	}
	ClearBuffer();										// Clear memory
}
void IInterProcessCommunication::SetName(const std::string &name, bool Setup /* = true */)
{
	if(IsDebugOn())	printf("%s SetName(%s)\n", GetType().c_str(), name.c_str());
	if(name != "") m_name = name;
	else throw BepException("Name cannot be empty string");
	if(Setup) SetupChannel(GetName());	// Configure channel
}
void IInterProcessCommunication::SetId(const INT32 &id)
{	// Set the Id of who we are talking to
	if(id >= 0)
	{	// If it is ok
		if(IsDebugOn())	printf("%s'%s' SetId(%d) ", GetType().c_str(), GetName().c_str(), id);
		ISystemCommunication::SetId(id);
		if(IsDebugOn())
		{
			if(id == GetId())   printf("Ok\n");
			else                printf("Failed\n");
		}
	}
	else throw BepException("%d is not a valid Id", id);
}
void IInterProcessCommunication::SetId(const std::string &name)
{	// Get the channel Id and set it
	INT32 Id = -1, ii = 20;

	if(IsDebugOn())	printf("IInterProcessCommunication::SetId(%s)\n", name.c_str());
	do
	{	// Hang out for a while
		InvalidateId();
		Id = BposGetTaskId(name.c_str());
		if(Id != -1) SetId(Id);
		else if(ii-- <= 0)
		{
			throw BepException("Could not get id for '%s': %s", name.c_str(), strerror(errno));
		}
		else
		{
			if(IsDebugOn())	printf("SetId Could Not Get The Task Name: %s\n", strerror(errno));
			BposSleep(100);
		}
	}
	while(Id == -1);
}
void IInterProcessCommunication::SetType(const std::string &type)
{
	if((type == "Server") || (type == "Client") || (type == "Worker"))
	{
		if(IsDebugOn())	printf("SetType(%s)\n", type.c_str());
		ISystemCommunication::SetType(type);
	}
	else throw BepException("Cannot set up a channel with communication type = " +
							type + ".  Options are Client, Server, or Worker");
}
void IInterProcessCommunication::SetupChannel(const std::string &name /* = "" */)
{	// Do special stuff for clients and servers here
	if(IsDebugOn())	printf("%s'%s' SetupChannel(%s)\n", GetType().c_str(),
						   GetName().c_str(), name.c_str());
	if(GetType() == "Server")
	{	// Create channel with no special flag
		INT32 id = -1;	// Temporary location for return value

		InvalidateId();
		if((id = BposRegisterTaskName(name.c_str())) != BP_ERR_KRNL_FAIL)
		{
			m_nameStruct = (name_attach_t *)id;
			SetId(m_nameStruct->chid);
		}
		else throw BepException("Failed to create IPC channel with name '" + name + "'");
	}
	else if(GetType() == "Client") SetId(name);	   // Attach to a named process for Client
}
void IInterProcessCommunication::ClearPublicBuffer(void)
{
    printf("ClearPublicBuffer\n");
    ClearBuffer();
}
void IInterProcessCommunication::ClearBuffer(void)
{	// Clear buffer if it exists
	if(IsDebugOn())	printf("%s'%s' ClearBuffer()\n", GetType().c_str(), GetName().c_str());
	if(m_rxBuffer) memset(m_rxBuffer, 0, GetRxLength()+1);
}
void IInterProcessCommunication::SetBuffer(const std::string &data)
{
	ClearBuffer();
	memcpy(m_rxBuffer, data.c_str(), strlen(data.c_str())+1);	// Copy data
}
bool IInterProcessCommunication::HandlePulse(Pulse &pulse)
{
	switch(GetPulseCode())	// Decode pulse
	{
	case SYSTEM_PULSE:
		switch(GetPulseValue())
		{
		case ABORT_PULSE:
		case RESET_PULSE:
		case TERMINATE_PULSE:
		case TIMER_PULSE:
		default:
			SetPulseHandled(false);
			break;
		}
		break;
	default:	// It was an OS pulse
		SetPulseHandled(!BposHandlePulse(&pulse));
		break;
	}
	if(IsDebugOn())
	{
		printf("%s'%s' Pulse with code = %d, value = %d was handled = %d\n",
			   GetType().c_str(), GetName().c_str(), GetPulseCode(),
			   GetPulseValue(), WasPulseHandled());
	}
	return(WasPulseHandled());
}
void IInterProcessCommunication::HandleError(const std::string &source) throw (BepException)
{	// Handle error
	std::string error = GetType() + "'" + GetName() + "' IInterProcessCommunication::" + source + " failed: ";
	if(IsDebugOn())	perror(error.c_str());
	throw BepException(error + strerror(errno));
}


void IInterProcessCommunication::SetReadChannel(bool read)
{
    m_readChannel = read;
}

bool IInterProcessCommunication::ShouldReadChannel(void)
{
    return(m_readChannel);
}


const std::string IInterProcessCommunication::GetBuffer(void)
{
    return(std::string(m_rxBuffer));
}

const bool IInterProcessCommunication::ShouldReply(void)
{
    return(m_reply);
}

void IInterProcessCommunication::SetReply(const bool reply)
{
    if(IsDebugOn())	printf("%s'%s' SetReply(%d)\n", GetType().c_str(), GetName().c_str(), reply);
    m_reply = reply;
}

const bool IInterProcessCommunication::WasPulseHandled(void)
{
    return(m_pulseHandled);
}

void IInterProcessCommunication::SetPulseHandled(const bool stat)
{
    m_pulseHandled = stat;
}

void IInterProcessCommunication::SetReplyId(const INT32 rid)
{
    m_replyId = rid;
    if(IsDebugOn())	printf("%s'%s' SetReplyId(%d)\n", GetType().c_str(), GetName().c_str(), GetReplyId());
}

void IInterProcessCommunication::InvalidateId(void) const
{
    INT32 id = -1;
    if(IsDebugOn())	printf("InvalidateId(%d)\n", GetId());
    if(GetType() == "Server")
    {	// Remove name from system created by server
        if(m_nameStruct != NULL)
        {
            if(BposUnRegisterTaskName((int)m_nameStruct) != EOK)
                printf("Error, Could Not Unregister Task Name: %s\n", strerror(errno));
            m_nameStruct = NULL;
        }
    }
    else if(GetType() == "Client")
    {
        if(GetId() != -1)
        {
            if(BposCloseTaskId(GetId()) != EOK)
            {
                if(IsDebugOn())	printf("Error, Could Not Close Task Id: %s\n", strerror(errno));
            }
            else if(IsDebugOn())
            {
                printf("Closed Task Id: %d\n", GetId());
            }
        }
        else if(IsDebugOn())	printf("Not Invalidating Id, due it is equal to %d\n", GetId());
    }

    ISystemCommunication::SetId(id);
}
