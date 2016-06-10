//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/SystemCommunication/ITcpIp.cpp 3     9/29/04 10:23p Cward $
//
// FILE DESCRIPTION:
//   This is the TCP/IP communiation class.  It is the wrapper for TCP/IP
//       communication on the system.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Header: /Core/ServicesLayer/Library/SystemCommunication/ITcpIp.cpp 3     9/29/04 10:23p Cward $
//
//*************************************************************************

#include "ITcpIp.h"



#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "BepDefs.h"
#include "Bepos.h"
#include "XmlParser.h"
#include "XmlNode.h"

#if _NTO_VERSION >= 630
    #include <errno.h>
    #include <arpa/inet.h>
#endif

#include "netinet/tcp.h"		// must include for TCP_NODELAY /usr/include/netinet/tcp.h

UINT32 ITcpIp::m_openSockets = 0;

ITcpIp::ITcpIp(void): ISystemCommunication(), m_port(-1), m_bufferSize(DEFAULT_BUFFER_SIZE),
	m_backlog(5), m_closeOnExit(false)
{
}
ITcpIp::ITcpIp(const std::string type, const INT16 port, const std::string address, const INT32 id /* = -1 */,
			   const bool debug	/* = false */, const std::string frameStart	/* = BEP_XML_ROOT */,
			   const std::string frameEnd /* = BEP_XML_ROOT_END */, const INT32 backlog	/* = 5*/) :
ISystemCommunication(), m_port(port), m_bufferSize(DEFAULT_BUFFER_SIZE), m_address(address),
	m_backlog(5), m_closeOnExit(false)
{
	SetDebug(debug);
	if(debug) printf("ITcpIp::ITcpIp(%s, %d, %s, %d, %s, %s, %d, %d)\n", type.c_str(), port, address.c_str(),
					 id, frameStart.c_str(), frameEnd.c_str(), backlog, debug);
	if(id != -1)
	{
		SetSock(id);
	}
	Initialize(type, port, address, id, debug, frameStart, frameEnd, backlog);
}
ITcpIp::~ITcpIp(void)
{
	if(IsDebugOn())	printf("ITcpIp::~ITcpIp(%d)\n", GetSock());
	CloseSocket(GetSock());
	if(IsDebugOn())	printf("ITcpIp::~ITcpIp(%d) done\n", GetSock());
}
void ITcpIp::Initialize(void)
{
	if(IsDebugOn())	printf("ITcpIp::Initialize()\n");
	if(GetSock() != -1)
	{	// if connecting to an open socket
		SetCloseOnExit(GetType() == std::string("Worker"));
		if(IsDebugOn())	printf("Connected To Socket: %d\n", GetSock());
	}
	else if((GetType() == "Server") && (GetPort() != -1))
	{	// open a socket
		OpenSocket();
		BindSocket();	// bind the socket
		SetCloseOnExit(true);
		listen(GetSock(), GetBacklog());	// listen for requests
	}
	else if((GetType() == "Client") && (GetPort() != -1) && (GetIPAddress() != ""))
	{	// open a socket
		OpenSocket();
		struct hostent *hp;	// get the server address by name Note: address must have alread been set
		if((hp = gethostbyname((char *) GetIPAddress().c_str())) == 0)
			throw BepException("ITcpIp: " + GetIPAddress() + ": Unknown Host");
		if(IsDebugOn())	printf("Host Found\n");
		memcpy(&m_server.sin_addr, hp->h_addr, hp->h_length);	// set the address
		m_server.sin_port = htons(GetPort());					// set the port
		m_server.sin_family = AF_INET;							// set the type
		// Connect socket using configured address
		if(connect(GetSock(), (struct sockaddr *) &m_server, sizeof(m_server)) < 0)
		{   // Make sure to close the socket so we do not use up buffers.
			CloseSocket();
			BepException("Connect failed because: %s", strerror(errno));
		}
		if(IsDebugOn())	printf("Connection Made\n");
	}
	else throw BepException("ITcpIp: Invalid ITcpIp Configuration: " + GetType());
}
void ITcpIp::Initialize(const XmlNode *config, const INT32 id /* =-1 */, const INT32 rid /* =-1 */)
{
	try
	{
		SetDebug(config->getChildren().getNode("Debug")->getValue());
	}
	catch(XmlException &e)
	{
		SetDebug(false);
		printf("Warning, ITcpIp configuration is missing the Debug information\n");
	}
	if(IsDebugOn())	printf("ITcpIp::Initialize(%s, %d, %d)\n", config->ToString().c_str(), id, rid);
	if(id == -1)
	{	// if this is not connecting to a currently open socket
        if(IsDebugOn()) printf("ITcpIp Calling CloseSocket from id = -1 function\n");
		SetType(config->getChildren().getNode("Type")->getValue());
		SetPort(config->getChildren().getNode("Port")->getValue());
		SetIPAddress(config->getChildren().getNode("Address")->getValue());
		CloseSocket(GetSock());	// Close socket if connected
	}
	else SetSock(id);	// set the socket number

	try
	{
		std::string frame;
		frame = config->getChildren().getNode("FrameStart")->getValue();
		ConvertSGandLString(frame);	// convert any "non-text" symbols
		SetFrameStart(frame);
		frame = config->getChildren().getNode("FrameEnd")->getValue();
		ConvertSGandLString(frame);	// convert any "non-text" symbols
		SetFrameEnd(frame);
	}
	catch(XmlException &e)
	{	// if either of the framing nodes does not exist, set both to empty
		SetFrameStart(std::string(""));
		SetFrameEnd(std::string(""));
	}
	try
	{	// Set string to be prepended to data
		std::string data;
		data = config->getChildren().getNode("PrependString")->getValue();
		SetPrependString(data);
	}
	catch(...)
	{
	}
	try
	{	// Set string to be appended to data
		std::string data;
		data = config->getChildren().getNode("AppendString")->getValue();
		SetAppendString(data);
	}
	catch(...)
	{
	}
	try
	{	// Set backlog for connection requests
		const std::string data = config->getChildren().getNode("Backlog")->getValue();
		SetBackLog(atoi(data.c_str()));
	}
	catch(...)
	{
		SetBackLog(5);
	}
	// Store the buffer size
	try
	{
		SetBufferSize(BposReadInt(config->getChild("Size")->getValue().c_str()));
	}
	catch(...)
	{
		SetBufferSize(DEFAULT_BUFFER_SIZE);
	}
	Initialize();
}
void ITcpIp::Initialize(const std::string &type, const INT16 port, const std::string address, const bool debug)
{
	if(IsDebugOn())	printf("ITcpIp::Initialize(%s, %d, %s, %d)\n", type.c_str(), port, address.c_str(), debug);
	try
	{
		SetDebug(debug);
		SetType(type);								// Set Type
//		CloseSocket(GetSock());						// Close socket if connected
		if(port != -1) SetPort(port);				// set the port to operate on if valid
		if(address != "") SetIPAddress(address);	// Set the IP address to operate on if valid (client only)
	}
	catch(XmlException &e)
	{
		if(IsDebugOn())	printf("Warning, XmlException Did not enter: %s\n", e.Reason().c_str());
	}
	catch(BepException &e)
	{
		if(IsDebugOn())	printf("Warning, BepException Did not enter: %s\n", e.Reason().c_str());
	}
	catch(...)
	{
		printf("Error: Initialize Unknown Exception\n");
	}
	Initialize();
}
void ITcpIp::Initialize(const char *type, const INT16 port, const char *address, const bool debug)
{
	Initialize(std::string(type), port, std::string(address), debug);
}
void ITcpIp::Initialize(const std::string type, const INT16 port, const std::string address, const INT32 id,
						const bool debug /* = false */, const std::string frameStart /* = BEP_XML_ROOT */,
						const std::string frameEnd /* = BEP_XML_ROOT_END */, const INT32 backlog /* = 5 */)
{
	if(IsDebugOn())	printf("ITcpIp::Initialize(%s, %d, %s, %d, %d, %s, %s, %d)\n", type.c_str(), port,
						   address.c_str(), id, debug, frameStart.c_str(), frameEnd.c_str(), backlog);
	if(id != -1)
	{
		SetSock(id);
	}
	SetFrameStart(frameStart);
	SetFrameEnd(frameEnd);
	SetBackLog(backlog);
	Initialize(type, port, address, debug);
}
INT32 ITcpIp::ProcessRequest(void)
{
	INT16 msgsock=-1;		// new port of operation
	if(GetType() == "Server")
	{
		if(IsDebugOn())	printf("ITcpIp::ProcessResquest(%d)...\n", GetSock());
		msgsock = accept(GetSock(), 0, 0);	// accept a connection
		if(msgsock == -1)
		{	// if there was an error,
			if(errno == 4)
			{	// if just a child disconnection
				if(IsDebugOn())	printf("Child Disconnect");
				else throw BepException("ITcpIp: Accept failed because: %s", strerror(errno));
			}
			else
			{
				perror("accept");
				throw BepException("accept: %s", strerror(errno));
			}
		}
		else
		{
			m_openSockets++;
			if(IsDebugOn())	printf("%d listening accepted new connction %d\n", GetSock(), msgsock);
		}
	}
	else printf("ProcessRequest Warning, Server Method Being Called by a Client.\n");
	return(msgsock);			// return the socket that the request was received on
}
INT32 ITcpIp::Read(std::string &message, INT32 id /*id=-1*/)
{
	char buf[GetBufferSize()];	// the buffer to read in the data
	INT32 stat = -1, counter = 10, loop = 0;	// status flag and counter

	if(IsDebugOn())	printf("ITcpIp::Read(%d)...\n", GetSock());

	CheckId(id);	// If id was -1 set to socket number, if still -1 error
	if(id == -1) throw BepException("ITcpIp:Could Not Read, Invalid Socket I.D.");

	while((stat <= 0) && (loop++ < 3))
	{	// sit in this loop until the full message is received or an error occurrs	
		memset(buf, 0, sizeof(buf));
		if((stat = GetMessage(message)) == false)
		{	// If there was not a message in buffer read from socket
			do
			{	// read from the socket
				stat = read(id, buf, sizeof(buf)-1);
				pthread_testcancel();
			}
			while(SignalCheck(stat, EINTR, counter, IsDebugOn()) == false);	// Do over if interrupted
			if(stat > 0)
			{	// store the message
				stat = GetMessage(message, buf);
				loop = 0;	 // Reset loop counter if we got good data
			}
			else printf("TcpIp::Read(%d) Error:  %s, stat:%d\n", GetSock(), strerror(errno),stat);
		}
	}
	if(IsDebugOn())	printf("TcpIp::Read() - loop:%d, stat:%d, message:>%s<\n", loop,stat,message.c_str());
	if((loop > 2) && (stat <= 0))
	{	// There was a problem reading the message from the port.
		stat = BEP_STATUS_ERROR;
	}
	if(IsDebugOn())	printf("TcpIp::Read() returning stat:%d\n", stat);
	return(stat);		// return success if this did not throw an exception
}
void ITcpIp::Write(const std::string &message, INT32 id/*id=-1*/)
{
	INT32 localID;														// the socket to send information on
	std::string localMessage;
	std::string::size_type firstPos;					// string start position
	std::string::size_type secondPos;					// string end position

	if(IsDebugOn())	printf("ITcpIp::Write(%s, %d)\n", message.c_str(), GetSock());
	// check the id that was passed in, if -1 use the default id of the object
	// if the default is -1 then throw an exception
	if(id == -1)
	{
		localID = GetSock();
		if(localID == -1)
			throw BepException("ITcpIp: Could Not Read, Invalid Socket I.D.");
	}
	else localID = id;

	if(!GetFrameStart().empty() && !GetFrameEnd().empty())
	{	// if message should be framed, validate the beginning and end of the frame
		firstPos = message.find(GetFrameStart());				// check for the start of frame
		if(firstPos == std::string::npos)						// if not valid flag invalid
			throw BepException("ITcpIp: Invalid Message Start (%s)", message.c_str());
		firstPos--;												// move the first pointer to the beginning
		if(IsDebugOn())	printf("Write Message Valid Start\n");
		secondPos = message.find(GetFrameEnd());				// check for the end of frame
		if(secondPos == std::string::npos)						// if not valid flag invalid
			throw BepException("ITcpIp: Invalid Message End (%s)", message.c_str());
		secondPos += GetFrameEnd().length() + 1;				// move the second pointer to the end
		if(IsDebugOn())	printf("ITcpIp: Write Message Valid End\n");
		localMessage = message.substr(firstPos, secondPos);
		if(IsDebugOn())	printf("ITcpIp: Write Length: %d,  %s\n", localMessage.length(), localMessage.c_str());
	}
	else localMessage = message;

	sigevent event;						// Event structure informing what to do if timed out
	event.sigev_notify = SIGEV_UNBLOCK;	// Unblock if time out
	INT32 counter = 10, stat = -1;
#if 0	// Causing Connection timed out errors
	UINT64 timeout = GetNanoTimeout();	// Get timeout value
	do
	{	// Set up timeout for send and reply operations
		if(IsDebugOn())	printf("Setting Timeout Time\n");
		stat = TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_SEND | _NTO_TIMEOUT_REPLY, &event, &timeout, NULL);
		if(stat == -1)	printf("Had An Error: %s\n", strerror(errno));
		pthread_testcancel();	// If in a thread get out if canceled
	}
	while(SignalCheck(stat, EINTR, counter, IsDebugOn()) == false);	// Do over if interrupted
	if((stat == -1) && (IsDebugOn())) perror("TimerTimeout()");		// If failed, handle it

	counter = 10;
#endif
	do
	{	// Add special characters to beginning and end of message before sending
		localMessage += "\n";
//		localMessage = GetPrependString() + localMessage + GetAppendString() + "\n";
		stat =  write(localID, localMessage.c_str(), localMessage.length());
		pthread_testcancel();
	}
	while(SignalCheck(stat, EINTR, counter, IsDebugOn()) == false);	// Do over if interrupted
	if(stat < 0) throw BepException("ITcpIp: %s", strerror(errno));
	if(IsDebugOn())	printf("Write Complete\n");
}
void ITcpIp::Write(const char *message, INT32 id/*id=-1*/)
{
	Write(std::string(message), id);
}
void ITcpIp::OpenSocket(void)
{	// AF_INET - IPv4 Protocol (AF_INET6 - IPv6 Protocol)
	// SOCK_STREAM - stream socket (TCP)  (SOCK_DGRAM - datagram socket (UDP)
	INT32 sock = socket(AF_INET, SOCK_STREAM, 0);	// create a socket
	if(sock >= 0)
	{	// if a socket was opened
		m_openSockets++;
		SetSock(sock);			// store the socket file descriptor
		unsigned int value;		// create a value and length variable for setting the options
#if(_NTO_VERSION < 620)
		unsigned int length;	// create a value and length variable for setting the options
#else
		socklen_t length;		// create a value and length variable for setting the options
#endif
		length = sizeof(value);	// set the length
		if(getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &value, &length) != -1)
		{
			if(IsDebugOn())	printf("Current TCP_NODELAY status: %s\n", ((value == 0) ? "off" : "on"));
			value = 1;	// enable the TCP_NODELAY option
			if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &value, length) != -1)
			{
				if(IsDebugOn())	printf("Updated TCP_NODELAY status: %s\n", ((value == 0) ? "off" : "on"));
			}
			else printf("TCP_NODELAY could not be set in RTP\n");
		}
		else printf("TCP_NODELAY is not currently supported in RTP\n");

		if(setsockopt(sock, IPPROTO_TCP, SO_REUSEADDR, &value, length) != -1)
		{
			if(IsDebugOn())	printf("Updated SO_REUSEADDR status: %s\n", ((value == 0) ? "off" : "on"));
		}
		else printf("SO_REUSEADDR could not be set in RTP\n");
#if 0
		if(setsockopt(sock, IPPROTO_TCP, SO_REUSEPORT, &value, length) != -1)
		{
			if(IsDebugOn())	printf("Updated SO_REUSEPORT status: %s\n", ((value == 0) ? "off" : "on"));
		}
		else printf("SO_REUSEPORT could not be set in RTP\n");
#endif
	}
	else throw BepException("ITcpIp: Socket create failed because: %s", strerror(errno));
	if(IsDebugOn())	printf("Socket Created: %d\n",GetSock());
}
void ITcpIp::CloseSocket(const INT16& newValue /* = -1*/) const
{
    if(IsDebugOn()) printf("ITcpIp::CloseSocket(%d, %s) connected to: %d, should close %d\n", newValue, GetType().c_str(), GetSock(), GetCloseOnExit());
	INT32 counter = 10, stat = -1;
	if(IsDebugOn()) printf("ITcpIp::CloseSocket(%d, %s) connected to: %d, should close %d\n", newValue, GetType().c_str(), GetSock(), GetCloseOnExit());
	if((GetSock() != -1) && GetCloseOnExit())
	{	// If connected to a socket
		do
		{	// Close socket repeating if interrupted by signal
			stat = close(GetSock());
//			stat = shutdown(GetSock(), 2);	// Passing 2 will close both directions of socket
			pthread_testcancel();
		}
		while(SignalCheck(stat, EINTR, counter, IsDebugOn()) == false);	// Do over if interrupted

		if(stat != -1)
		{
			m_openSockets--;
			if(IsDebugOn()) printf("Closed Socket %d\n", GetSock());
			SetId(-1);	// Set id to invalid
		}
		else if(IsDebugOn()) printf("Failed to close socket %d\n", GetSock());
	}
}
void ITcpIp::BindSocket(void)
{
	// bind the opened socket
	m_server.sin_family = AF_INET;						// set the domain to the Internet
	m_server.sin_addr.s_addr = INADDR_ANY;		// set the socket to accept any valid address
	m_server.sin_port = htons(GetPort());		// get the server port number
	if(bind(GetSock(), (struct sockaddr *) &m_server, sizeof(m_server)))
		throw BepException("Bind failed because: %s", strerror(errno));
	if(IsDebugOn())	printf("Socket Bound: %d\n", GetSock());
}
const bool ITcpIp::GetMessage(std::string &message, char *buffer /* = NULL */)
{
	bool found = false;
	std::string::size_type firstPos, secondPos;	// string position finders
	if(IsDebugOn())	printf("GetMessage(%s)\n", message.c_str());
	if(buffer && strlen(buffer))
	{
		if(IsDebugOn())	printf("Received: %d, %s\n", strlen(buffer), buffer);
		m_receivedData += buffer;
	}
	if(IsDebugOn())	printf("Full Message: %s\n", m_receivedData.c_str());
	// look for the beginning of an XML message if a framing specifier has been provided
	// Warning: Do not use find_last_of() or find_first_of() string methods they dont work!
	if(!GetFrameStart().empty() && !GetFrameEnd().empty())
	{
		firstPos = m_receivedData.find(GetFrameStart());	// check if the beginning was found
		if((firstPos != std::string::npos) && (firstPos != 0))
		{	// since we are using XML, the first position is one place before the start tag look for the end
			firstPos--;
			if(IsDebugOn())	printf("First Position: %d\n",firstPos);
			// Warning: Do not use find_last_of() or find_first_of() string methods they dont work!
			secondPos = m_receivedData.find(GetFrameEnd());	// find the string and move to the end
			if(IsDebugOn())	printf("Second Position: %d, %d\n", secondPos, std::string::npos);
			if((secondPos != std::string::npos) && (secondPos != 1))
			{	// if the full message is received
				secondPos += GetFrameEnd().length() + 1;	// move the second pointer to the end
				int number = secondPos - firstPos;			// the number of characters to read
				// because we are using XML and the editor does not allow the "<" symbol to be
				// specified, it will be added to the beginning of the message
				message = m_receivedData.substr(firstPos, number);				// store the sub string
				if(IsDebugOn())	printf("TCP Message: %s\n", message.c_str());	// store the message
				m_receivedData.erase(0, secondPos+1);			// remove up to the end of the string
				if(IsDebugOn())	printf("receivedData: %s\n", m_receivedData.c_str());	// show the message left
				found = true;	// flag the read complete
			}
		}
	}
	else
	{	// if no framing specifiers have been provided
		message = m_receivedData;
		found = !message.empty();
		m_receivedData.erase();
		if(IsDebugOn())	printf("No Frame TCP Message: %s\n", message.c_str());
	}
	return(found);
}


void ITcpIp::Close(void) const
{
    if(IsDebugOn()) printf("ITcpIp Calling CloseSocket from Close() function\n");
    CloseSocket(GetSock());
}

const UINT32 ITcpIp::GetNumSockets(void)
{
    return(m_openSockets);
}



void ITcpIp::SetCloseOnExit(const bool close)
{
    m_closeOnExit = close;
}

const bool ITcpIp::GetCloseOnExit(void) const
{
    return(m_closeOnExit);
}

const std::string ITcpIp::GetIPAddress()
{
    if(IsDebugOn()) printf("GetIPAddress: %s\n", m_address.c_str());
    return(m_address);
}
void ITcpIp::SetIPAddress(const std::string &value)
{
    if(IsDebugOn()) printf("SetIPAddress: %s\n", value.c_str());
    if(!value.empty())  m_address = value;
    else                throw BepException("Invalid IP Address");
}
const INT16 ITcpIp::GetPort()
{
    if(IsDebugOn())  printf("GetPort %d\n", m_port);
    return(m_port);
}
void ITcpIp::SetPort(INT16 value)
{
    if(IsDebugOn()) printf("SetPort %d\n", value);
    if(value >= 1024)   // force the port to be in the user defineable area
        m_port = value;
    else throw BepException("Invalid Port Number %d", value);
}
void ITcpIp::SetPort(const std::string &value)
{
    if(IsDebugOn()) printf("SetPort %s\n", value.c_str());
    if(!value.empty())
        SetPort(atoi(value.c_str()));
    else throw BepException("Invalid Port Value: %s", value.c_str());
}
const INT32 ITcpIp::GetSock() const
{
    return((const INT32) GetId());
}
void ITcpIp::SetSock(const INT32 &value)
{
    if(IsDebugOn()) printf("ITcpIp::SetSock(%d)\n", value);
    if(value > 0)
    {   // If id is valid
        if(IsDebugOn()) printf("ITcpIp Calling CloseSocket from setsock function\n");
        CloseSocket(value);
        SetId((const INT32) value);
    }
    else throw BepException("Invalid Socket Vaule: %d", value);
}
const struct sockaddr_in ITcpIp::GetServer()
{
    return(m_server);
}
void ITcpIp::SetServer(const struct sockaddr_in &value)
{
    m_server = value;
}
const std::string ITcpIp::GetFrameStart()
{
    return(m_frameStart);
}
void ITcpIp::SetFrameStart(const std::string &value)
{
    if(IsDebugOn()) printf("SetFrameStart: %s\n", value.c_str());
    m_frameStart = value;
}
const std::string ITcpIp::GetFrameEnd()
{
    return(m_frameEnd);
}
void ITcpIp::SetFrameEnd(const std::string &value)
{
    if(IsDebugOn()) printf("SetFrameEnd: %s\n", value.c_str());
    m_frameEnd = value;
}
void ITcpIp::SetBackLog(INT32 backlog)
{
    if(IsDebugOn()) printf("SetBacklog: %d\n", backlog);
    if(backlog > 5) m_backlog = backlog;
    else m_backlog = 5;
}
const INT32 ITcpIp::GetBacklog(void)
{
    return(m_backlog);
}

const INT32& ITcpIp::GetBufferSize(void)
{
	return m_bufferSize;
}

void ITcpIp::SetBufferSize(const INT32 &bufferSize)
{   // Ensure we are using a sufficient size for the buffer
	m_bufferSize =  bufferSize >= DEFAULT_BUFFER_SIZE ? bufferSize : DEFAULT_BUFFER_SIZE;
}
