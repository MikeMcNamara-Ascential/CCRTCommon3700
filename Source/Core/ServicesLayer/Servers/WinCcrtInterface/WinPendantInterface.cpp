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
#include "WinPendantInterface.h"

//-------------------------------------------------------------------------------------------------
WinPendantInterface::WinPendantInterface() : BepServer(), m_ndb(NULL)
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
WinPendantInterface::~WinPendantInterface()
{	
	if(m_ndb != NULL)  delete m_ndb;
}

//-------------------------------------------------------------------------------------------------
int WinPendantInterface::OpenSocket(int port, WinPendantInterface* server)
{
	struct sockaddr_in sockServer;
	int sock, status, length;

	// Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		server->Log(LOG_DEV_DATA,"TcpServer sock error: %s\n", strerror(errno));
		exit(-1);
	}

	// Name socket using wildcards -> Ethernet to Serial Port
	sockServer.sin_family = AF_INET;
	sockServer.sin_addr.s_addr = INADDR_ANY;
	sockServer.sin_port = htons(port);

	// allow multiple connections to same address / port
	status = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &status, sizeof(status));
	status = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &status, sizeof(status));

	if(bind(sock, (struct sockaddr *)&sockServer, sizeof(sockServer)))
	{
		server->Log(LOG_DEV_DATA,"TcpServer error binding to socket: %s\n", strerror(errno));
		exit(-1);
	}

	// Find out assigned port number and print it out
	length = sizeof(sockServer);
	if(getsockname(sock, (struct sockaddr *)&sockServer, &length))
	{
		server->Log(LOG_DEV_DATA,"TcpServer getsockname error: %s\n", strerror(errno));
	}

	if(listen(sock, 5) == -1)
	{
		server->Log(LOG_DEV_DATA,"TcpServer error listening on socket: %s\n", strerror(errno));
		exit(-1);
	}

	server->Log(LOG_DEV_DATA,"TcpServer Listening on TCP port #%d\n", ntohs(sockServer.sin_port));
	return(sock);
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE WinPendantInterface::ProcessWinCcrtMessage(string &message, int serSocket)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	Log(LOG_FN_ENTRY, "Processing Win CCRT Message: %s", message.c_str());
	char *msgP = strtok(&message[0], ";");
	while(msgP != NULL)
	{
		string indMsg(msgP);
		Log(LOG_DEV_DATA, "\tProcessing individual message: %s", indMsg.c_str());
		int msgTypeLength = message.find_first_of(":");
		int tagStart = msgTypeLength + 1;
		int tagEnd = indMsg.find_first_of(",", tagStart);
		string msgType = indMsg.substr(0, msgTypeLength);
		string tag = indMsg.substr(tagStart, tagEnd - tagStart);
		Log(LOG_DEV_DATA, "\t\tMsg Type: %s, Tag: %s", msgType.c_str(), tag.c_str());
		if(msgType == BEP_READ)
		{	// Read the tag from the system
			string tempVal = tag.compare("Notes") ? SystemRead(tag) : m_notes;
			// Send the value back to the Windows CCRT system
			if(tempVal.length() == 0)
			{
				tempVal = BEP_UNAVAILABLE_RESPONSE;
			}
			tempVal = tag + "=" + tempVal + ";";
			unsigned char *value = (unsigned char *)&tempVal[0];
			Log(LOG_DEV_DATA, "Responding to pendant with: %s", tempVal.c_str());
			send(serSocket, value, tempVal.length(), 0);
			status = BEP_STATUS_SUCCESS;
		}
		else if(msgType == BEP_WRITE)
		{
			int valStart = tagEnd + 1;
			string value(indMsg.substr(valStart));
			Log(LOG_DEV_DATA, "\tWriting %s to %s", tag.c_str(), value.c_str());
			if(tag.find("BeltTension") != tag.npos)
			{
				XmlNode *beltNode = new XmlNode(tag, value);
				m_beltTensions.addNode(beltNode);
				Log(LOG_DEV_DATA, "Added new belt tension: %s, %s", tag.c_str(), value.c_str());
				status = BEP_STATUS_SUCCESS;
			}
			else if(tag.find("Notes") != tag.npos)
			{
				m_notes += value;
				Log(LOG_DEV_DATA, "Added new note: %s", value.c_str());
				Log(LOG_DEV_DATA, "Current Notes: %s", m_notes.c_str());
				status = BEP_STATUS_SUCCESS;
			}
			else
			{
				status = SystemWrite(tag, value);
			}
		}
		else
		{
			Log(LOG_ERRORS, "Unknown message type: %s", msgType.c_str());
		}
		msgP = strtok(NULL, ";");
	}
	Log(LOG_FN_ENTRY, "Processed Win CCRT message: %s", ConvertStatusToResponse(status).c_str());
	return status;
}

//-------------------------------------------------------------------------------------------------
const string WinPendantInterface::Register(void)
{
	string result = BepServer::Register();

	// Create an INamedDataBroker object
	if(m_ndb != NULL)
	{
		delete m_ndb;
		m_ndb = NULL;
	}
	m_ndb = new INamedDataBroker(false);

	// For Tcp Connection
	// Launching in new thread because accept and read will block, and this is not
	// deserable in the main thread
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(NULL, &attr, &WinPendantInterface::TcpThread_helper, (void *)this);

	return result;
}

//-------------------------------------------------------------------------------------------------
string WinPendantInterface::SystemRead(string &tag)
{
	string value, response;
	if(m_ndb != NULL)
	{
		INT32 status = m_ndb->Read(tag, response, true);
		if(status == BEP_STATUS_SUCCESS)
		{
			string reason;
			status = m_ndb->GetNext(tag, value, response);
		}
		// check for errors
		if(status != BEP_STATUS_SUCCESS)
		{
			Log(LOG_ERRORS, "Error Reading: %s, %d\n", tag.c_str(), status);
			value = BEP_ERROR_RESPONSE;
		}
	}
	else
	{
		Log(LOG_ERRORS, "SystemRead() - INamedDataBroker object is NULL!");
		value = BEP_SOFTWAREFAIL_RESPONSE;
	}
	return(value);
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE WinPendantInterface::SystemWrite(const string &tag, const string &value)
{
	BEP_STATUS_TYPE result = BEP_STATUS_SOFTWARE;	// the result of the operation
	string response;
	if(m_ndb != NULL)
	{
		try
		{
			result = ConvertIntToBepStatus(m_ndb->Write(tag, value, response, true));
			Log(LOG_DEV_DATA, "\t\tWrote %s,%s - result: %s", tag.c_str(), value.c_str(), ConvertStatusToResponse(result).c_str());
		}
		catch(BepException &excpt)
		{
			Log(LOG_ERRORS, "SystemWrite %s Error: %s\n", tag.c_str(), excpt.GetReason());
		}
	}
	return(result);
}

//-------------------------------------------------------------------------------------------------
void* WinPendantInterface::TcpThread( void* arg )
{
	WinPendantInterface* server = (WinPendantInterface *)arg;
	short shtdwn;
	int maxfiledes, sertemp;
	int rval, sersock, listenSock;
	char *err, buf[2048];
	UINT16 rcvSize;
	fd_set readtmpl;
	struct timeval wait;

	sersock = -1;			// connected serial Ethernet converter file descriptor.

	// exit flag
	shtdwn = 0;

	listenSock = OpenSocket(10011, server);

	// Start accepting connections
	if(listenSock < 0)
		exit(-1);

	server->Log(LOG_DEV_DATA, "Listening socket is open - socket: %d", listenSock);
	// wait for connection, accept connection, route data received
	while(BEP_TERMINATE != GetStatus())
	{
		// configure variables for the select call
		FD_ZERO(&readtmpl);

		// examine the Serial to Ethernet port
		if(listenSock > 0)
		{
			if(sersock > 0)
				FD_SET(sersock, &readtmpl);	// examine the connected file descriptor

			FD_SET(listenSock, &readtmpl);	 // examine the unconnected file descriptor (listen)
		}

		// select modifies these values (time to wait)
		wait.tv_sec = 5;
		wait.tv_usec = 0L;

		// include all relevant file descriptors in the 'select' statement
		maxfiledes = max(listenSock, sersock) + 1;

		// thread pauses here until something can be read from one of the file descriptors
		server->Log(LOG_DEV_DATA, "Waiting for 5s connection....");
		rval = select(maxfiledes, &readtmpl, 0, 0, &wait);

		if(rval == 0)
			server->Log(LOG_DEV_DATA,"select timed out with no activity, sersock = %d, listenSock = %d\n", sersock, listenSock);
		else if(rval < 0)
		{
			err = strerror(errno);
			server->Log(LOG_DEV_DATA,"%s: Main Select\n", err);
		}
		else
		{
			server->Log(LOG_DEV_DATA, "Connection ready on the port");
		}

		// check for change on the Serial to Ethernet conversion socket
		if(listenSock > 0)
		{
			// connection pending on socket
			if(FD_ISSET(listenSock, &readtmpl) && (sersock < 0))
			{
				sersock = accept(listenSock, (struct sockaddr *)0, (int *)0);
				server->Log(LOG_DEV_DATA,"Got connect on TCP port: %d", sersock);
			}
			else if(FD_ISSET(listenSock, &readtmpl))
			{
				// have to connect first so the second connection can
				// can be closed
				sertemp = accept(listenSock, (struct sockaddr *)0, (int *)0);

				// if close immediately, LabWindows programs get a GPF!!!!
				delay(100);
				close(sertemp);

				server->Log(LOG_DEV_DATA,"Refused connect on TCP port");
			}
		}
		// if the TCP/IP serial convert connection is open, examine
		// the descriptor
		if(sersock > 0)
		{
			if(FD_ISSET(sersock, &readtmpl))
			{
				// clear buffer before adding data
				memset(buf, 0, sizeof(buf));
				rval = recv(sersock, buf, 8*sizeof(char), 0);
				rcvSize = (buf[1] << 8) | (0x00FF & buf[0]);
				if((8*sizeof(char) + rcvSize*sizeof(char)) < sizeof(buf))
					rval += recv(sersock, &buf[8], rcvSize*sizeof(char), 0);
				else
					rval += recv(sersock, &buf[8], sizeof(buf) - (2*sizeof(char)), 0);
				if(rval < 0)
				{
					err = strerror(errno);
					server->Log(LOG_DEV_DATA,"%s: reading TCP data", err);
					close(sersock);
					sersock = -1;
				}
				else if(rval == 0)
				{
					server->Log(LOG_DEV_DATA,"closed connection");
					close(sersock);
					sersock = -1;
				}
				else
				{
					string pendantMsg((char *)&buf[0], rval);
					server->Log(LOG_DEV_DATA, "Rx Msg: %s", pendantMsg.c_str());
					server->ProcessWinCcrtMessage(pendantMsg, sersock);
				}
			}
		}
	}

	close(listenSock);
	return this;
}

//-------------------------------------------------------------------------------------------------
void* WinPendantInterface::TcpThread_helper(void *context)
{
	return((WinPendantInterface *)context)->TcpThread(context);
}
