//******************************************************************************
// Description:
//  Logical port driver for communication on Gryphon
//  This file contains the common routines used by all gryphon communications
//
//==============================================================================
//
//     Copyright © 2003 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/Common/IGryphonChannel.cpp $
// 
// 2     12/14/07 7:51a Derickso
// REmoved attempted fix for ==.  Module ID is 0000 for some reason.  Need
// to investigate further.
// 
// 1     11/02/07 12:54p Derickso
// Updated to support UDS protocol.
// Added functions (but did not make active) for only transmitting one
// message at a time to each module.
// 
// 3     4/13/06 3:56a Cward
// removed deafult parameter, initialized local var
//
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
// added wrappers around several statements that would cause problems if
// Gryphon was not available during registration. tag 20051028
//
// 1     5/11/05 9:39p Cward
// Initial Version
//
// 1     4/25/05 2:17p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 9     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 8     9/01/04 9:34p Cward
// Added updates for debug and test.
//
// 7     6/24/04 1:09a Cward
// Removed pound if 0 blocks. Updated the header block comments to the new
// standard.
//
// 6     5/25/04 4:19p Cward
// Updated to catch the bus off event from the box and re-initialize the
// bus. Added Reset Driver Handler to reset the driver at the beginning of
// the test
//
// 5     3/04/04 5:15p Cward
// Replaced Update Client Fifos method with Can Add To Client Fifo method
//
// 4     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 3     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:29p Cward
//
// 3     11/13/03 6:02p Derickso
// Updates to register gryphon usdt server to allow larger range of module
// IDs.
//
// 2     10/07/03 1:05p Derickso
// Latest Gryphon Comm proxy.
//
// 1     9/24/03 6:45a Derickso
// Gryphon Comm Proxy files.
//
// 5     7/03/03 7:24a Gswope
//
// 4     7/01/03 11:25a Gswope
//
// 1     5/08/03 8:41a Gswope
// tested on CAMI machines with vehicle
//
// 3     5/05/03 8:22a Gswope
// placing in stasis
//
// 2     4/22/03 11:25a Gswope
//
// 1     4/22/03 10:38a Gswope
// First Entry
//
// 6     4/15/03 8:30a Gswope
//
// 5     4/07/03 3:14p Gswope
//
// 4     4/05/03 10:22a Gswope
// Daily Update -- class 2 working
//
// 3     4/01/03 8:45a Gswope
//
// 2     3/25/03 12:58p Gswope
//
// 1     3/17/03 4:30p Gswope
// Initial entry into source safe
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <string.h>

#include <netdb.h> // for gethostbyname and hostent
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
#endif

#include "IGryphonChannel.h"
#include "gryphon2.h"
#include "LogPortDefs.h"
#include "IGCDebug.h"
//==============================================================================

IOFUNC_OCB_T* IGryphonChannel::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::OcbCalloc()\n");

	GryphonIoOcb_t    *iocb = new GryphonIoOcb_t;

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::OcbCalloc()\n");
	return( (IOFUNC_OCB_T*)iocb);
}

void IGryphonChannel::OcbInit(resMgrIoOcb_t *ioOcb)
{
	GryphonIoOcb_t    *newOcb = (GryphonIoOcb_t*)ioOcb;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::OcbInit()\n");

	RawCommProxy::OcbInit( ioOcb);

	newOcb->moduleIDs.clear();
	newOcb->expectedResponse = 0;

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::OcbInit()\n");
}

void IGryphonChannel::OcbFree(resMgrIoOcb_t *ocb)
{
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::OcbFree()\n");
	GryphonIoOcb_t    *iocb = (GryphonIoOcb_t*)ocb;

	delete iocb;
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::OcbFree()\n");
}

void IGryphonChannel::OcbFinalize(resMgrIoOcb_t *ioOcb)
{
	GryphonIoOcb_t    *oldOcb = (GryphonIoOcb_t*)ioOcb;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::OcbFinalize(Module ID: %s)\n", GetModuleIDsString(oldOcb->moduleIDs).c_str());

#if 0
	RawCommProxy::OcbFinalize( ioOcb);
#else
	CommIoOcb_t     *orgOcb = (CommIoOcb_t*)ioOcb;
	BlockedReader_t readClient;
	BlockedWriter_t writeClient;

	if (oldOcb != NULL)
	{
		if ( (errno=m_blockedReaders.Lock()) == EOK)
		{
			// If this client is in a blocked read condition
			if ( m_blockedReaders.FindBlockedReader(orgOcb, readClient) == true)
			{
				// Remove client from blocked reader list
				m_blockedReaders.RemoveBlockedReader( orgOcb);

				// Tell blocked client that read was cancelled
				MsgError( readClient.replyId, ECANCELED);
			}
			m_blockedReaders.Unlock();
		}
		else
		{
			Log( LOG_ERRORS, "\tError locking blocked reader list: %s\n", strerror( errno));
		}
		Log(LOG_FN_ENTRY, "\t\t\tRemoved blocked readers...");

		m_blockedWriters.Lock();
		// If this client is in a blocked read condition
		if ( m_blockedWriters.FindBlockedWriter(orgOcb, writeClient) == true)
		{
			// Remove client from blocked reader list
			m_blockedWriters.RemoveBlockedWriter( orgOcb);
			// Tell blocked client that read was cancelled
			MsgError( writeClient.replyId, ECANCELED);
		}
		m_blockedWriters.Unlock();
		Log(LOG_FN_ENTRY, "\t\t\tRemoved blocked writers...");

		// If this client had an exclusive on the port
		if ( orgOcb->portLockCount > 0)
		{	// Make sure any locks help by this client are released
			ReleaseClientPortLock( orgOcb, true);
		}
		Log(LOG_FN_ENTRY, "\t\t\tReleased client port lock...");

		// Free memory allocated for subscriptions
		if ( orgOcb->rxSubscription != NULL)
		{
			m_rxSubscribers.DelSubscriber( orgOcb->rxSubscription->id);
			delete orgOcb->rxSubscription;
			orgOcb->rxSubscription = NULL;
		}
		Log(LOG_FN_ENTRY, "\t\t\tFreed RX subscribers...");
		if ( orgOcb->txSubscription != NULL)
		{
			m_txSubscribers.DelSubscriber( orgOcb->txSubscription->id);
			delete orgOcb->txSubscription;
			orgOcb->txSubscription = NULL;
		}
		Log(LOG_FN_ENTRY, "\t\t\tFreed TX subscribers...");
		if ( orgOcb->lockSubscription != NULL)
		{
			m_lockSubscribers.DelSubscriber( orgOcb->lockSubscription->id);
			delete orgOcb->lockSubscription;
			orgOcb->lockSubscription = NULL;
		}
		Log(LOG_FN_ENTRY, "\t\t\tFreed Lock subscribers...");
		if ( orgOcb->unlockSubscription != NULL)
		{
			m_unlockSubscribers.DelSubscriber( orgOcb->unlockSubscription->id);
			delete orgOcb->unlockSubscription;
			orgOcb->unlockSubscription = NULL;
		}
		Log(LOG_FN_ENTRY, "\t\t\tFreed Unlock subscribers...");

		oldOcb->moduleIDs.clear();

		ResManagerBase::OcbFinalize( (resMgrIoOcb_t *)oldOcb);
#endif
	}
	else
	{
		Log(LOG_ERRORS, "ERROR - IGryphonChannel::OcbFinalize() -Trying to finalize a NULL Ocb\n");
	}
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::OcbFinalize()\n");
}

int IGryphonChannel::IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
	GryphonIoOcb_t    *clientOcb = (GryphonIoOcb_t*)ioOcb;
	int             retVal = ENOSYS;
	vector<uint32_t>        clientModIds;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::IoRead(), module=%s\n",
		 GetModuleIDsString(clientOcb->moduleIDs).c_str());
	// Get the module ID this client shuld be reading from
	clientModIds = clientOcb->moduleIDs;
	// IF this is not a "combine" read
	if ( (msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE)
	{	// Make sure client has performed valid write
		if( !IsModuleIDPresent(clientModIds,0) )
		{
			Log(LOG_DEV_DATA, "IGryphonChannel::IoRead() calling RawCommProxy::IoRead()\n");
			retVal = RawCommProxy::IoRead(ctp,msg,ioOcb);
			Log(LOG_DEV_DATA, "RawCommProxy::IoRead() returned %d\n", retVal);
		}
		else
		{
			Log( LOG_IO_MSSG, "Invalid module ID: $%s\n", GetModuleIDsString(clientModIds).c_str());
			retVal = ENXIO;
		}
	}
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::IoRead(), module=%s\n",
		 GetModuleIDsString(clientOcb->moduleIDs).c_str());
	return( retVal);
}

int IGryphonChannel::IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
	GryphonIoOcb_t  *clientOcb = (GryphonIoOcb_t*)ioOcb;
	SerialString_t  rawMessage, busMessage;
	int             bytesRead, bytesWritten;
	int             retVal = EINVAL;
	vector<uint32_t>        moduleIds;

	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::IoWrite()\n");
	// Make sure OK for client to write (port not locked)
	if ((CanClientWrite( &clientOcb->proxyOcb) == true) &&
	   ((msg->i.xtype & _IO_XTYPE_MASK) == _IO_XTYPE_NONE))
	{	// Read the message data from the sender
		if ( (bytesRead=ReadSendersBuffer( ctp, rawMessage, msg)) > 1)
		{
			PrintSerialString( LOG_SER_COM, "Gryphon Sending", rawMessage);
			// Log communication for this serial send (use rawMessage so
			// byte count not added to comm log)
			//UpdateBusCommLog( ComDirTx, rawMessage.c_str(), rawMessage.length(), &clientOcb->proxyOcb);
			// See which module we are sending to
			// get module id from string at this point
			clientOcb->moduleIDs = getModuleIdsFromRaw(rawMessage);
			clientOcb->expectedResponse = getExpectedFromRaw(rawMessage);
			moduleIds = clientOcb->moduleIDs;


			if(UsingGryphonUSDT() == false)
			{

				// Ensure a response is not pending prior to sending to the module
				int timeout = 250;	// allow 250 ms for response pending to clear
				while(IsResponsePending(moduleIds) && (--timeout > 0))
				{
					BposSleep(1);
				}
				// Indicate a response is now pending ofr this module ID
				SetModuleResponsePending(moduleIds);
			}
			// add wrapper
			BuildCardMessage(busMessage, rawMessage);
			// Send this message to the vehicle
			bytesWritten = JustWrite( ctp, ioOcb, busMessage.c_str(), busMessage.size());
			// IF we wrote (at least) the whole message
			if ( bytesWritten >= (int)busMessage.size())
			{
				// Tell client we write his message
				_IO_SET_WRITE_NBYTES( ctp, bytesRead);
			}
			// Tell the client we did it.
			retVal = EOK;
		}
		else if ( bytesRead == -1)
		{
			retVal = errno;
		}
		else
		{
			retVal = EINVAL;
		}
	}
	else
	{
		retVal = EPERM;
	}
	// Log the function exit
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::IoWrite(), %d bytes status=%d module=%s\n",
		 bytesRead, retVal, GetModuleIDsString(clientOcb->moduleIDs).c_str());
	// Return the status
	return( retVal);
}

int IGryphonChannel::IoUnblock(resmgr_context_t *ctp, io_pulse_t *msg, resMgrIoOcb_t *ioOcb)
{
	GryphonIoOcb_t  *clientOcb = (GryphonIoOcb_t*)ioOcb;
	int retVal = -1;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::IoUnblock(), module: $%s\n", GetModuleIDsString(clientOcb->moduleIDs).c_str());

	retVal = RawCommProxy::IoUnblock( ctp, msg, ioOcb);

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::IoUnblock(), module: $%s\n", GetModuleIDsString(clientOcb->moduleIDs).c_str());

	return( retVal);
}

int IGryphonChannel::ReadPortDataUnlocked(uint8_t *buff, size_t buffSz)
{
	unsigned int locSize;
	BEP_STATUS_TYPE messageStatus = BEP_STATUS_ERROR;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::ReadPortDataUnlocked()\n");
	// Determine the size of the incoming data
	locSize = m_rxFifo.Peek(buff, 8);
	while (locSize >= 8)
	{
		locSize = Align32(256* buff[4] + buff[5]);

		if (m_rxFifo.GetSize() >= locSize + 8)
		{
			m_rxFifo.GetBytes(buff,locSize+8);
			messageStatus = processNewMessage(buff);	// non-protocol (control) interpret
			if (BEP_STATUS_SUCCESS == messageStatus)
			{
				DecodeRead(buff, locSize+8); // to clients
			}
			else
			{
				Log(LOG_ERRORS, "Error processing new message, not decoding the message");
			}
			// Log communication for this client
			UpdateBusCommLog( ComDirRx, buff, locSize+8, NULL);

			locSize = m_rxFifo.Peek(buff, 8);
		}
		else locSize = 0; // done for now
	}
	// Log the function exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::ReadPortDataUnlocked(%d)\n", locSize);
	return(0);
}

int IGryphonChannel::ConfigurePort( const SerialProtocol &portCfg, int mode)
{
	(void) portCfg;
	(void) mode;
	return(0);
}

const std::string IGryphonChannel::Register(void)
{
	int retVal;
	int linkStatus;
	std::string retString(BEP_ERROR_RESPONSE);
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::Register() \n");
	// Call the base class register
	if (RawCommProxy::Register() == BEP_SUCCESS_RESPONSE)
	{	// Login to the Gryphon server
		Log(LOG_DEV_DATA, "Login to Gryphon server...\n");
		GlobalContext = 0x00;
		if (EOK == Login(UserId, Password, m_channelType))
		{	// find the channel we want to use
			Log(LOG_DEV_DATA, "Gathering Gryphon config data...\n");
			if (EOK == GetGryphonConfiguration())
			{	// tell the gryphon we want all messages
				Log(LOG_DEV_DATA, "Informing Gryphon server to send all messages...\n");
				if (EOK == BroadcastOn())
				{	// get event names for our channel
					Log(LOG_DEV_DATA, "Getting channel event names from the Gryphon server...\n");
					if (EOK == GetEventNames())
					{	// tell our card we want all events
						Log(LOG_DEV_DATA, "Enabling all events for our card...\n");
						if (EOK == EventEnable())
						{	// reset the hardware
							Log(LOG_DEV_DATA, "Resetting the hardware...\n");
							if (EOK == InitCard())
							{	// protocol specific initialization
								Log(LOG_DEV_DATA, "Performing channel specific initialization...\n");
								if (EOK == ChannelSpecificInit())
								{
									retString = BEP_SUCCESS_RESPONSE;
								}
								else
								{
									Log(LOG_ERRORS, "Error performing channel specifiv initialization\n");
								}
							}
							else
							{
								Log(LOG_ERRORS, "Error resetting the Gryphon hardware\n");
							}
						}
						else
						{
							Log(LOG_ERRORS, "Error enabling all events for our card\n");
						}
					}
					else
					{
						Log(LOG_ERRORS, "Error getting channel event names from the Gryphon server\n");
					}
				}
				else
				{
					Log(LOG_ERRORS, "Error Informing Gryphon server to send all messages\n");
				}
			}
			else
			{
				Log(LOG_ERRORS, "Error Gathering Gryphon config data\n");
			}
		}
		else
		{
			Log(LOG_ERRORS, "Error During Login to Gryphon server\n");
		}
	}
	else
	{
		Log(LOG_ERRORS, "Error During RawCommProxy::Initialize()\n");
	}

	try
	{
		if (PortPathAlias != "")
		{
			if (unlink(PortPathAlias.c_str()))
				Log(LOG_DEV_DATA,"IGryphonChannel::Register() Could not unlink symlink (may be first time?)\n");
			else
				Log(LOG_DEV_DATA,"IGryphonChannel::Register() symlink successfully unlinked.\n");


			Log(LOG_DEV_DATA,"IGryphonChannel::Register() Attempting to set alias to %s -> %s.\n",
				PortPathAlias.c_str(),PortDevPath.c_str());
			linkStatus = symlink(PortDevPath.c_str(),PortPathAlias.c_str());
			if (0 == linkStatus)
				Log(LOG_ERRORS,"IGryphonChannel::Register() symbolic link successfully created\n");
			else
				Log(LOG_ERRORS,"IGryphonChannel::Register() Unable to create symbolic link %d (%d)\n",linkStatus, errno);
		}
		else
		{
			Log(LOG_DEV_DATA,"IGryphonChannel::Register() No alias supplied\n");
			linkStatus = 0;
		}

	}
	catch (...)
	{
		Log(LOG_ERRORS,"IGryphonChannel::Register() Fatal error in alias functionality\n");
	}


	// log the function exit
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::Register() RetVal: %d  Return: %s\n",
		 retVal, retString.c_str());
	// Return the result
	return(retString);
}

IGryphonChannel::IGryphonChannel(void) : RawCommProxy()
{	// Nothing special to do here
}

IGryphonChannel::IGryphonChannel( int argc, char *argv[]) : RawCommProxy(argc, argv)
{	// Nothing special to do here
}

IGryphonChannel::~IGryphonChannel(void)
{	// Nothing special to do here
}

void IGryphonChannel::Initialize(const XmlNode *document)
{
	// Nothing special to do, just use the base class
	RawCommProxy::Initialize(document);
}

int IGryphonChannel::OpenPort( const char *devName, int mode)
{
	int myEPort;
	// we dont need to do these.
	(void) devName;
	(void) mode;
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::OpenPort()\n");
	// return value from this function is our system port number
	myEPort = Connect(IPAddr, myPort);
	// Log the function exit
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::OpenPort() myEPort = %d\n", myEPort);
	return(myEPort);
}

int IGryphonChannel::FlushDriverFifos(int mode)
{
	return(0);
}

int IGryphonChannel::UnlockForClient( CommIoOcb_t *clientOcb, bool releaseAll)
{
	int retVal= 0;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::UnlockForClient()\n");
	retVal = RawCommProxy::UnlockForClient( clientOcb,releaseAll);
	if (0 == retVal)
	{
		((GryphonIoOcb_t *) clientOcb)->moduleIDs.clear();
	}
	// Log the function exit
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::UnlockForClient() retVal = %d\n", retVal);
	return(retVal);
}

// return socket id # or -1 if failed
int IGryphonChannel::Connect( std::string ipAddr, uint16_t port)
{
	struct hostent *hp;
	struct sockaddr_in server;
	int retVal = -1;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::Connect() ipAddr = \"%s\" \n",ipAddr.c_str());
	// Clear out the server info
	memset ((char *) &server, 0, sizeof(server));
	// Get the host name
	hp = gethostbyname(ipAddr.c_str());
	Log( LOG_DETAILED_DATA, "hp = %d\n", hp);
	// if hp = NULL, we have a problem, need to write a trap for it.
	// at some point in the future.
	memcpy ((char *) &server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(port);
	server.sin_family = hp->h_addrtype;
	// return from this call is our system "port" number
	MySocket = socket(AF_INET, SOCK_STREAM, 0);
	// following wrapper added 20051028 - ews
	if(NULL != MySocket)
	{
		Log( LOG_DETAILED_DATA, "MySocket = %d  ready to call connect()\n", MySocket);
		retVal = connect(MySocket, (struct sockaddr *)&server, sizeof(server));
		// will return a retVal < 0 for an error, another thing to handle.
		if(retVal < 0)
		{   // Error connecting the socket, close the socket
			close(MySocket);
		}
	}
	else
	{
		Log( LOG_ERRORS, "** *** ** MySocket = %d : Invalid socket\n", MySocket);
		Log( LOG_ERRORS, "** *** ** Gryphon not Connected\n", MySocket);
		retVal = -1;
	}
	if(retVal < 0) MySocket = -1;

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::Connect()    retVal = %d\n", MySocket);
	return(MySocket);
}

int IGryphonChannel::Login( std::string userId, std::string passwd,
							const uint16_t ctype)
{
	struct LoginBlock locMsg; // arbitray big enough, holds data only
	char *asChar = (char *) &locMsg;
	int locSize;
	uint8_t *locStr;
	int retVal;
	// Get the size of the message
	locSize = sizeof(locMsg);
	locStr = (uint8_t *) &locMsg;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::Login()\n");
	m_clientChannel = 0;
	m_channelId = 0;			 // clear it, until we get a new one
	m_channelType = ctype;		 // set these so we can find the right channel
	// Clear out the message structure
	memset(locStr,0,sizeof(locMsg));

	locMsg.header.cmd = CMD_SERVER_REG;
	locMsg.header.context = UseContext();

	locMsg.header.reserved = 0;	// already 0

	strncpy(locMsg.userID, userId.c_str(), 15);
	strncpy(locMsg.passwd, passwd.c_str(), 31);


	Log(LOG_DEV_DATA, "IGryphonChannel::Login() - Login message: \n");
	std::string logString = "";
	char locChar[10];
	for (unsigned int ii = 0; ii < sizeof(locMsg); ii++)
	{
		sprintf(locChar,"%02x ",asChar[ii]);
		logString.append(locChar);
	}
	logString.append("\n");
	Log(LOG_DEV_DATA,logString.c_str());


#if IGCDEBUG
	printf("Dumping login message\n");
	dump(&locMsg, sizeof(locMsg));
	printf("\n");
#endif

	retVal = EOK; // assume success

	// use the LoggeIn CondVar to wait for a response with timeout
	if (LoggedIn.Acquire()== EOK)
	{
		LoggedIn.SetValue(false);
		retVal = SendRawFrame(locStr, locSize, FT_CMD, SD_SERVER);
		if (LoggedIn.Wait(true,mSEC_nSEC(5000)) == EOK)
		{
			Log( LOG_IO_MSSG, "Gryphon Login Successful\n");
		}
		else
		{
			Log( LOG_IO_MSSG, "Gryphon Login Failed, no response\n");
		}
		LoggedIn.Release();
	}
	// error handling
	else
	{
		Log( LOG_IO_MSSG, "Login Failed, could not acquire\n");
	}
	// Log the function exit
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::Login()    retVal = %d\n", retVal);
	return(retVal);

}

void IGryphonChannel::Disconnect()
{
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::Disconnect()\n");
	m_channelId = 0;  // clear it, assuming we dont have a valid id
	// pull the plug on the ethernet?
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::Disconnect()\n");
}

int IGryphonChannel::GetChannelId()
{
	return( m_channelId);
}

void IGryphonChannel::SetChannelOverride(int value)
{
	m_channelOverride = value;
}

int IGryphonChannel::ResetDriverHandler( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
	int retVal = EOK;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::ResetDriverHandler()\n");

	// Do base class suff first (resets fifos)
	if( RawCommProxy::ResetDriverHandler( ctp, msg, ioOcb) == EOK)
	{
		// Re-initialize the Gryphon box
		RegisterWithUsdt();
	}
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::ResetDriverHandler()\n");

	return( _RESMGR_ERRNO(retVal));
}

int IGryphonChannel::Align32(int locLen, bool noPadding /*= false*/)
{
    unsigned int returnLength = locLen;
    if(!noPadding)
        returnLength = (locLen + 3) & 0xFFFC;
	return(returnLength);
}

char *IGryphonChannel::GetEventName( const uint8_t eNumber)
{
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::GetEventName()\n");
	for (int ii=0 ; ii < MAX_EVNAMES; ii++)
	{
		EventIDs.event[ii].terminator=0;
		if (eNumber == EventIDs.event[ii].number)
		{
			Log( LOG_FN_ENTRY, "Exit IGryphonChannel::GetEventName() found\n");
			return(EventIDs.event[ii].name);
		}
	}
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::GetEventName() not found\n");
	return("Unidentified event");
}

int IGryphonChannel::GetEventNames(void)
{
	uint8_t locMsg[20];	// arbitray big enough
	int locSize;
	int retVal;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::GetEventNames()\n");
	// Clear out the message structure
	memset(locMsg,0,sizeof(locMsg));

	locMsg[0] = CMD_CARD_GET_EVNAMES;
	locMsg[1] = UseContext();

	locSize = 2;

	if (EventNamesLoaded.Acquire()== EOK)
	{
		EventNamesLoaded.SetValue(false);
		retVal = SendRawFrame(locMsg, locSize, FT_CMD);
		if (EventNamesLoaded.Wait(true,mSEC_nSEC(5000)) == EOK)
		{

		}
		EventNamesLoaded.Release();
	}

#if IGCDEBUG
	dumpEventNames(&EventIDs);
#endif
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::GetEventNames()\n");
	return(EOK);
}

int IGryphonChannel::GetGryphonConfiguration(void)
{
	uint8_t localBuffer[20];
	int locSize;
	int retVal;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::GetGryphonConfiguration()\n");
	memset(localBuffer,0,sizeof(localBuffer));
	// Clear the channel ID
	m_channelId = 0;			 // clear it, until we get a new one
	localBuffer[0] = CMD_GET_CONFIG;
	localBuffer[1] = UseContext();
	locSize = 2;
	// Send the data
	retVal = SendRawFrame(localBuffer, locSize, FT_CMD, SD_SERVER);
	// need to wait until configuration is complete, and our channel is selected.
	// added timeout 20051028 - ews
	int locTimeout = 1000;
	while((0 == m_channelId) && (--locTimeout > 0))	delay(2);
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::GetGryphonConfiguration()\n");
	return(EOK); // temporary for compile warning
}

int IGryphonChannel::BroadcastOn(void) // tell the gryphon we want all messages
{
	uint8_t localBuffer[20];
	int locSize;
	int retVal;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::BroadcastOn()\n");
	// Clear out the message data
	memset(localBuffer,0,sizeof(localBuffer));
	localBuffer[0] = CMD_BCAST_ON;
	localBuffer[1] = UseContext();
	locSize = 2;
	// need to wait for acknowledgement
	if (BroadcastOnAck.Acquire()== EOK)
	{
		BroadcastOnAck.SetValue(false);
		retVal = SendRawFrame(localBuffer, locSize, FT_CMD, SD_SERVER);
		if (BroadcastOnAck.Wait(true,mSEC_nSEC(5000)) == EOK)
		{
		}
		BroadcastOnAck.Release();
	}
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::BroadcastOn()\n");
	return(EOK);
}

int IGryphonChannel::SetFilterMode(uint8_t locMode)
{

	uint8_t localBuffer[20];
	int locSize;
	int retVal;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::SetFilterMode(%d)\n", locMode);
	// Clear the buffer area
	memset(localBuffer,0,sizeof(localBuffer));
	// Set up the message
	localBuffer[0] = CMD_CARD_SET_FILTER_MODE;
	localBuffer[1] = UseContext();
	localBuffer[2] = 0;
	localBuffer[3] = 0;
	localBuffer[4] = locMode; // filter off, pass everything
	locSize = 5;
	// need to wait for acknowledgement
	if (PassAllAck.Acquire()== EOK)
	{
		PassAllAck.SetValue(false);
		retVal = SendRawFrame(localBuffer, locSize, FT_CMD, SD_CARD);
		if (PassAllAck.Wait(true,mSEC_nSEC(5000)) == EOK)
		{

		}
		PassAllAck.Release();
	}
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::SetFilterMode()\n");
	return(EOK);
}

int IGryphonChannel::SetDefaultFilterMode(uint8_t locMode)
{

	uint8_t localBuffer[20];
	int locSize;
	int retVal;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::SetDefaultFilterMode(%d)\n", locMode);
	// Clear the buffer area
	memset(localBuffer,0,sizeof(localBuffer));
	// Set up the message
	localBuffer[0] = CMD_CARD_SET_DEFAULT_FILTER;
	localBuffer[1] = UseContext();
	localBuffer[2] = 0;
	localBuffer[3] = 0;
	localBuffer[4] = locMode; // filter pass all or block non-conforming messages
	locSize = 5;
	// need to wait for acknowledgement
	if (PassAllAck.Acquire()== EOK)
	{
		PassAllAck.SetValue(false);
		retVal = SendRawFrame(localBuffer, locSize, FT_CMD, SD_CARD);
		if (PassAllAck.Wait(true,mSEC_nSEC(5000)) == EOK)
		{

		}
		PassAllAck.Release();
	}
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::SetDefaultFilterMode()\n");
	return(EOK);
}

int IGryphonChannel::AddFilter(const uint8_t locFlags, const uint16_t locOffset,
							   const uint16_t locLength, const uint8_t locField, const uint8_t locOperator,
							   const char *firstValue, const char *secondValue)
{
	int retVal = 0;
	int locSize;
	unsigned char localBuffer[100];
	struct gfilter *myFilter;
	// Log the function entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::AddFilter(%02x, %d, %d, %d, %d, %02x)\n",
		 locFlags, locOffset, locLength, locField, locOperator,
		 *firstValue);
	// Error check the filter length
	if(locLength > 30) return(EINVAL);
	// Clear the buffer area
	memset(localBuffer,0,sizeof(localBuffer));	 // shove the trailing nulls
	// Set up the message
	localBuffer[0] = CMD_CARD_ADD_FILTER;
	localBuffer[1] = UseContext();
	localBuffer[2] = 0;
	localBuffer[3] = 0;
	// Setup the filter
	myFilter = (gfilter *)&localBuffer[4];
	myFilter->header.flags = locFlags;
	myFilter->header.num_fields = 1;
	myFilter->header.reserved = 0; // 6 reserved bytes
	myFilter->header.reserved1 = 0;
	myFilter->body.offset = htons(locOffset);
	myFilter->body.length=htons(locLength);
	myFilter->body.data_type=locField;
	myFilter->body.op=locOperator;
	myFilter->body.step_size = 0;
	myFilter->body.reserved = 0;

	memcpy(myFilter->body.values,firstValue,locLength);
	if (BIT_FIELD_CHECK == locOperator)
	{
		memcpy(&(myFilter->body.values[locLength]),secondValue,locLength);
	}

	locSize = 20 + locLength;

	if(BIT_FIELD_CHECK == locOperator) locSize += locLength;

	locSize = Align32(locSize);	// the card wants the aligned length

	// need to wait for acknowledgement
	if (PassAllAck.Acquire()== EOK)
	{
		PassAllAck.SetValue(false);
		retVal = SendRawFrame(localBuffer, locSize, FT_CMD, SD_CARD);
		if (PassAllAck.Wait(true,mSEC_nSEC(5000)) == EOK)
		{
			Log( LOG_DETAILED_DATA, "Added Filter\n");
		}
		else
		{
			Log( LOG_DETAILED_DATA, "Timed out adding Filter\n");
		}

		PassAllAck.Release();
	}
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::AddFilter()\n");
	return(EOK);
}

int IGryphonChannel::EventEnable(void) // tell our card we want all events
{
	uint8_t localBuffer[20];
	int locSize;
	int retVal;
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::EventEnable()\n");
	// Clear the buffer area
	memset(localBuffer,0,sizeof(localBuffer));
	// Setup the message
	localBuffer[0] = CMD_EVENT_ENABLE;
	localBuffer[1] = UseContext();
	localBuffer[2] = 0;
	localBuffer[3] = 0;
	localBuffer[4] = 0;	// enable all
	// Set the message size
	locSize = 5;
	// need to wait for acknowledgement
	if (EventEnableAck.Acquire()== EOK)
	{
		EventEnableAck.SetValue(false);
		retVal = SendRawFrame(localBuffer, locSize, FT_CMD, SD_CARD);
		if (EventEnableAck.Wait(true,mSEC_nSEC(5000)) == EOK)
		{	// Do nothing
		}
		EventEnableAck.Release();
	}
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::EventEnable()\n");
	return(EOK); // temporary for compile warning
}

int IGryphonChannel::InitCard(void)		// reset the hardware
{

	uint8_t localBuffer[20];
	int locSize;
	int retVal;
	// log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::InitCard()\n");
	// Clear the buffer area
	memset(localBuffer,0,sizeof(localBuffer));
	// Setup the message
	localBuffer[0] = CMD_INIT;
	localBuffer[1] = UseContext();
	localBuffer[2] = 0;
	localBuffer[3] = 0;
	localBuffer[4] = 0;	// force initialization
	// Set the message size
	locSize = 5;
	// need to wait for acknowledgement
	if (InitAck.Acquire()== EOK)
	{
		InitAck.SetValue(false);
		retVal = SendRawFrame(localBuffer, locSize, FT_CMD, SD_CARD);
		if (InitAck.Wait(true,mSEC_nSEC(5000)) == EOK)
		{

		}
		InitAck.Release();
	}
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::InitCard()\n");
	return(EOK);
}
void* IGryphonChannel::InitCardThread(void *arg)
{
	IGryphonChannel *ch = (IGryphonChannel *)arg;
    if(ch->InitCard() != EOK)
        {	// Unable to re-initialize CAN bus
            ch->Log(LOG_ERRORS, "ERROR - Unable to Re-init on Can Bus Off\n");
        }
        else
        {	// Successfully Re-initialized CAN bus
            ch->Log(LOG_DETAILED_DATA, "Re-init of Gyphon Successful\n");
        }
}

void IGryphonChannel::copyConfig(const uint8_t *inBuf, const uint16_t  datalen)
{
	serverinfo *si;
	deviceinfo *diS;
	deviceinfo *diD;
	int locOverride;
	int scratch;
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::copyConfig()\n");
	for (int ii = 0; ii < MAXCHANS; ii++) Channels.dinfo[ii].channel=0;
	si = (serverinfo *)&inBuf[16];
	for (int ii = 0; ii < si->numdevs; ii++)
	{
		diS = (deviceinfo *) &inBuf[80+80*ii];
		scratch = diS->channel;
		if (scratch < MAXCHANS)
		{
			diD = &Channels.dinfo[scratch];
			memcpy(diD,diS,80);
		}
	}
	// set up for which channel we are going to use.
	locOverride = m_channelOverride; // get global override value
	for (int ii = 1; ii <= MAXCHANS; ii++)
	{
		if (Channels.dinfo[ii].type == m_channelType)
		{
			if (0 == locOverride)
			{
				m_channelId = ii;  // found our channel
				Log( LOG_DEV_DATA, "IGryphonCannel::copyConfig()-found channelId(%d) to match channelType(%d)\n", m_channelId, m_channelType);
				break;			   // stop looking
			}
			else
			{
				Log( LOG_FN_ENTRY, "VV Skipping\n");
				locOverride--;
			}
		}
	}
	// Log the exit
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::copyConfig(channel=%d)\n", m_channelId);
}

int IGryphonChannel::SendRawFrame(const uint8_t *inBuf,
								  const uint16_t inSize, const uint8_t frameType,
								  const uint8_t hooToo /* = SD_CARD*/, bool noPadding /*= false*/, int msgLength /*= 0*/)
{
	char locBuf[4096];
	unsigned int locSize;
	Msg *locMsg;
	int retVal;
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::SendRawFrame()\n");

	// added wrapper 200510228 - ews
	if(MySocket > 0)
	{
		locMsg = (Msg *) locBuf;

		locSize = inSize;
		if(locSize > 4000) locSize = 4000;
        if(msgLength <= 0)
            msgLength = locSize;

		locMsg->fh.src = SD_CLIENT;
		locMsg->fh.srcchan = m_clientChannel;
		locMsg->fh.dst = hooToo;
		locMsg->fh.dstchan = m_channelId;
		locMsg->fh.msglen=htons(msgLength);
		locMsg->fh.frametype = frameType;
		locMsg->fh.reserved = 0;

		memcpy(&locMsg->subhdr.misc.data, inBuf, locSize);

            // What is this for? Why 4 bytes of padding? Why not 7? What's the rational for this?
            locMsg->subhdr.misc.data[locSize]   = 0;   // pad, in case needed
    		locMsg->subhdr.misc.data[locSize+1] = 0;
    		locMsg->subhdr.misc.data[locSize+2] = 0;
    		locMsg->subhdr.misc.data[locSize+3] = 0;

		

		locSize = Align32(locSize + 8, noPadding);

//#if IGCDEBUG
		printf("Dumping full ethernet message (%d)\n",locSize);
		dump(&locBuf, locSize);
		printf("\n");
//#endif

		UpdateBusCommLog(ComDirTx, (uint8_t *)locBuf, locSize, NULL);
		// send it to the ethernet. use locSize as corrected length
		retVal = write(MySocket,locBuf,locSize);
		// Check the reuslts of the write
		if(retVal > 0) retVal = EOK;
		else			retVal = errno;
	}
	else
	{
		retVal = -1;
	}

	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::SendRawFrame() retVal = %d\n", retVal);
	return(retVal);
}

void IGryphonChannel::BuildCardMessage(SerialString_t &outBuf, const SerialString_t &inBuf)
{
	SerialString_t locDS; // intermediate buffer
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::BuildCardMessage(inBuf(%d))\n", inBuf.size());
	BuildMessage(locDS, inBuf);	// add the protocol info
	WrapMessage(outBuf, locDS);	// add gryphon data frame
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::BuildCardMessage()\n");
}

void IGryphonChannel::WrapMessage(SerialString_t &frameData, const SerialString_t &inBuf)
{
	char locBuf[30];
	unsigned int locSize;
	Msg *locMsg;
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::WrapMessage()\n");
	locMsg = (Msg *) locBuf;

	locMsg->fh.src = SD_CLIENT;
	locMsg->fh.srcchan = m_clientChannel;
	locMsg->fh.dst = m_destinationType;
	locMsg->fh.dstchan = m_channelId;

	locSize = inBuf.size();

	locMsg->fh.msglen=htons(locSize);
	locMsg->fh.frametype = FT_DATA;
	locMsg->fh.reserved = 0;

	frameData = SerialString_t ((uint8_t *)locBuf, 8);
	frameData += inBuf;

	BufferGryphonMessage(frameData);
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::WrapMessage()\n");
}

void IGryphonChannel::BufferGryphonMessage(SerialString_t &frameData)
{
	SerialString_t  nullByte((uint8_t *)"\0",1);
	int bufferedBytes = 0;

	//ERIC LOOK HERE TO WHY THIS IS BEING ANDED WITH 3 AND ADDING NULL BYTES
	while(frameData.size() & 3)
	{
		frameData+=nullByte;
		bufferedBytes++;
	};


	//Log( LOG_FN_ENTRY, "Exit IGryphonChannel::BufferGryphonMessage() - Null Bytes Added %d\n", bufferedBytes);
	return;
}

int IGryphonChannel::DecodeRead(const uint8_t *inBuf, const uint16_t inSize)
{
	int dataLen = 0 ;
	const int moduleResponseIndex = 24;
	const char *locMessage;
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::DecodeRead()\n");
	// does not include header bytes in this count
	dataLen = 256 * inBuf[4] + inBuf[5];
	switch (inBuf[6])
	{
	case FT_CMD:	// we should never get these, just ignore them (looped back?)
		break;
	case FT_EVENT:	// currently only processing CAN Bus off message
		Log(LOG_DETAILED_DATA, "FTEVENT 0x%02X\n", inBuf[8]);
		// Check if FT_EVENT is CAN bus off event
		if((inBuf[8] == GCANBUSOFF) || (inBuf[8] == GCANBUSWARN))
		{	// CAN bus turned off
			Log(LOG_DETAILED_DATA, "CAN BUS %s error received from gryphon", inBuf[8] == GCANBUSOFF ? "Off" : "Warn");
            //start thread to init card
            pthread_attr_t attr;			// the attribute of the thread
            pthread_attr_init( &attr );
            pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
            if(pthread_create( NULL, &attr, IGryphonChannel::InitCardThread, this) != EOK)
            {
				Log(LOG_ERRORS, "ERROR - Unable to start init thread\n");
            }
		}
		break;
	case FT_DATA:
		// need to block messages from the card if it also is going to come from the USDT handler
		if(CheckForBlock(inBuf)) break;
		// calculate length of the data we care about to get around bad info from the frame header
		//Length of the header + length of the data + extra data (this is all from the data header)
		dataLen = inBuf[8] + 256 * inBuf[10] + inBuf[11] + inBuf[12];
		// call the data handler for this channel, to disassemble, then pass to client
		locMessage = (const char *)&inBuf[moduleResponseIndex];
		//UpdateClientFifos(locMessage, dataLen-16, PORT_SUBSCRIBE_RX);
		UpdateClientFifos(locMessage, dataLen, PORT_SUBSCRIBE_RX);
		// this is where we will be filling the FIFO for the read() functionality
		break;
	case FT_RESP:
		// processing for these should already be handled in processNewMessage() above
		break;
	default:
		break;
	} // end of switch
	dataLen = Align32(dataLen + 8);
	// Log the exit
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::DecodeRead()  retVal = %d\n", dataLen);
	return(dataLen);
}

BEP_STATUS_TYPE IGryphonChannel::processNewCardMsg(const uint8_t *inBuf, const uint16_t  datalen)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::ProcessNewCardMessage()\n");

	switch (inBuf[6])  // check type of message
	{
	case FT_RESP: // this is a response to a request
		Log( LOG_DETAILED_DATA, "Gryphon Response Message\n");
		if(RESP_OK == inBuf[15])   // only process if acknowledged
		{
			Log( LOG_DETAILED_DATA, "Gryphon Acknowledgement\n");
			switch(inBuf[8]) // check type of request
			{
			// others are pass all, event enable and init.
			case CMD_CARD_SET_FILTER_MODE:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_CARD_SET_FILTER_MODE\n");
				if(PassAllAck.Acquire()== EOK)
				{
					PassAllAck.Signal(true);
					PassAllAck.Release();
				}
				break;
			case CMD_CARD_ADD_FILTER:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_CARD_ADD_FILTER\n");
				if(PassAllAck.Acquire()== EOK)
				{
					PassAllAck.Signal(true);
					PassAllAck.Release();
				}
				break;
			case CMD_EVENT_ENABLE:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_EVENT_ENABLE\n");
				if(EventEnableAck.Acquire()== EOK)
				{
					EventEnableAck.Signal(true);
					EventEnableAck.Release();
				}
				break;
			case CMD_INIT:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_INIT\n");
				if(InitAck.Acquire()== EOK)
				{
					InitAck.Signal(true);
					InitAck.Release();
				}
				break;

			case CMD_CARD_GET_EVNAMES:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_CARD_GET_EVNAMES\n");
				CopyEventNames(inBuf, datalen);
				break;	// end of event names
			} // end of switch on request type
		} // end of acknowledge
		break;	// end of case FT_RESP
	case FT_EVENT: // this is an event report.
		Log( LOG_DETAILED_DATA, "Gryphon event received: %d, card %d - %s * * * * * Gryphon Event\n",
			 inBuf[8], inBuf[1], GetEventName(inBuf[8]));
		break;
	case FT_DATA:
		{            
			if(UsingGryphonUSDT() == false)
			{
				// need to block messages from the card if it also is going to come from the USDT handler
				if(CheckForBlock(inBuf)) break;
				// calculate length of the data we care about to get around bad info from the frame header
				//Length of the header + length of the data + extra data (this is all from the data header)
				int moduleDataLen = inBuf[8] + 256 * inBuf[10] + inBuf[11] + inBuf[12];            
				const int moduleResponseIndex = 24;	 //start of the module's data after all the gryphon wrappers and headers
				const char *moduleMessage = (const char *)&inBuf[moduleResponseIndex];
				PrintSerialString( LOG_SER_COM, "IGryphonChannel::ProcessNewCardMessage() Gryphon Incoming Msg: ", inBuf, datalen);

				//SerialString_t modData( (const uint8_t*)inBuf, datalen);
				PrintSerialString( LOG_SER_COM, "IGryphonChannel::ProcessNewCardMessage() Gryphon Module Msg: ", (const uint8_t*)moduleMessage, moduleDataLen);

				UpdateClientFifos(moduleMessage, moduleDataLen, PORT_SUBSCRIBE_RX);
				// this is where we will be filling the FIFO for the read() functionality
			}
			else
			{
				// Nothing special to do except not set error for default case
			}
		}
		break;
	default:
		status = BEP_STATUS_HARDWARE;
		Log(LOG_ERRORS, "Unknown card message type [%02X] returned from Gryphon box!", inBuf[6]);
		break;
	}// end of switch on message type
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::ProcessNewCardMessage() - status: %s\n", ConvertStatusToResponse(status).c_str());
	return status;
}

bool IGryphonChannel::IsBroadcastModuleID(const UINT32 locModule)
{//not implemented
	return(false);
}

BEP_STATUS_TYPE IGryphonChannel::processNewUsdtMsg(const uint8_t *inBuf, const uint16_t  datalen)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::processNewUsdtMessage()\n");
	switch (inBuf[6])  // check type of message
	{
	case FT_RESP: // this is a response to a request
		Log( LOG_DETAILED_DATA, "Gryphon Response Message (0x%02X)\n", inBuf[8]);
		if(0 == inBuf[15])	 // only process if acknowledged
		{
			Log( LOG_DETAILED_DATA, "Gryphon Acknowledge Message\n");
			switch(inBuf[8]) // check type of request
			{
			case CMD_USDT_REGISTER:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_USDT_REGISTER\n");
				if(UsdtRegistered.Acquire()== EOK)
				{
					UsdtRegistered.Signal(true);
					UsdtRegistered.Release();
				}
				break;	// end of event names
                
            case CMD_USDT_REGISTER_NON_LEGACY:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_USDT_REGISTER_NON_LEGACY\n");
				if(UsdtRegistered.Acquire()== EOK)
				{
					UsdtRegistered.Signal(true);
					UsdtRegistered.Release();
				}
				break;	// end of event names

			case CMD_USDT_SET_STMIN_MULT:
				Log(LOG_DETAILED_DATA, "Gryphon CMD_USDT_SET_STMIN_MULT");
				if(UsdtStMinMultiplier.Acquire() == EOK)
				{
					UsdtStMinMultiplier.Signal(true);
					UsdtStMinMultiplier.Release();
				}
				break;

            case CMD_USDT_SET_STMIN_FC:
				Log(LOG_DETAILED_DATA, "Gryphon CMD_USDT_SET_STMIN_FC");
				if(UsdtStMinFlowControl.Acquire() == EOK)
				{
					UsdtStMinFlowControl.Signal(true);
					UsdtStMinFlowControl.Release();
				}
				break;
			} // end of switch on request type
		} // end of acknowledge
		break;	// end of case FT_RESP
	case FT_DATA:
		// Nothing special to do except not set error for default case
		break;
	default:
		status = BEP_STATUS_HARDWARE;
		Log(LOG_ERRORS, "Unknown USDT message type [%02X] returned from Gryphon box!", inBuf[6]);
		break;
	} // end of switch on message type
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::processNewUsdtMessage() - status: %s\n", ConvertStatusToResponse(status).c_str());
	return status;
}

BEP_STATUS_TYPE IGryphonChannel::processNewServerMsg(const uint8_t *inBuf, const uint16_t  datalen)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::processNewServerMsg()\n");

	switch (inBuf[6])  // check type of message
	{
	case FT_RESP: // this is a response to a request
		Log( LOG_DETAILED_DATA, "Gryphon Response Message\n");
		if(0 == inBuf[15])	 // only process if acknowledged
		{
			Log( LOG_DETAILED_DATA, "Gryphon Acknowledge Message\n");
			switch(inBuf[8]) // check type of request
			{
			case CMD_SERVER_REG: // registration command
				Log( LOG_DETAILED_DATA, "Gryphon CMD_SERVER_REG\n");
				// need to store client channel number
				m_clientChannel = inBuf[16];
				if(LoggedIn.Acquire()== EOK)
				{
					LoggedIn.Signal(true);
					LoggedIn.Release();
				}
				break; // end of server registration
			case CMD_GET_CONFIG:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_GET_CONFIG\n");
				copyConfig(inBuf, datalen);
				break; // end of configuration
			case CMD_BCAST_ON:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_BCAST_ON\n");
				// set the ack flag for this
				if(BroadcastOnAck.Acquire()== EOK)
				{
					BroadcastOnAck.Signal(true);
					BroadcastOnAck.Release();
				}
				break;
			case CMD_SERVER_SET_SERVICE:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_SERVER_SET_SERVICE\n");
				// set the ack flag for this
				if(J1939TPEnableAck.Acquire()== EOK)
				{
					J1939TPEnableAck.Signal(true);
					J1939TPEnableAck.Release();
				}
				break;
			case CMD_J1939_ADDR_CLAIM:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_J1939_ADDR_CLAIM\n");
				if(J1939AddressClaim.Acquire()== EOK)
				{
					J1939AddressClaim.Signal(true);
					J1939AddressClaim.Release();
				}
				break;	// end of event names
			default:
				status = BEP_STATUS_HARDWARE;
				Log(LOG_ERRORS, "Unknown response frame type [%02X] returned from Gryphon box!", inBuf[8]);
				break;
			} // end of switch on request type
		} // end of acknowledge
		break;	// end of case FT_RESP
	default:
		status = BEP_STATUS_HARDWARE;
		Log(LOG_ERRORS, "Unknown Server message type [%02X] returned from Gryphon box!", inBuf[6]);
		break;
	}// end of switch on message type}
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::processNewServerMsg() - status: %s\n", ConvertStatusToResponse(status).c_str());
	return status;
}

BEP_STATUS_TYPE IGryphonChannel::processNewJ1939ConfigurationtMsg(const uint8_t *inBuf, const uint16_t  datalen)
{
	BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::processNewJ1939ConfigurationtMsg()\n");
	switch (inBuf[6])  // check type of message
	{
	case FT_RESP: // this is a response to a request
		Log( LOG_DETAILED_DATA, "Gryphon Response Message (0x%02X)\n", inBuf[8]);
		if(0 == inBuf[15])	 // only process if acknowledged
		{
			Log( LOG_DETAILED_DATA, "Gryphon Acknowledge Message\n");
			switch(inBuf[8]) // check type of request
			{
			case CMD_J1939_ADDR_CLAIM:
				Log( LOG_DETAILED_DATA, "Gryphon CMD_J1939_ADDR_CLAIM\n");
				if(J1939AddressClaim.Acquire()== EOK)
				{
					J1939AddressClaim.Signal(true);
					J1939AddressClaim.Release();
				}
				break;	// end of event names
                
			} // end of switch on request type
		} // end of acknowledge
		break;	// end of case FT_RESP
	case FT_DATA:
		// Nothing special to do except not set error for default case
		break;
	default:
		status = BEP_STATUS_HARDWARE;
		Log(LOG_ERRORS, "Unknown j1939 message type [%02X] returned from Gryphon box!", inBuf[6]);
		break;
	} // end of switch on message type
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::processNewJ1939ConfigurationtMsg() - status: %s\n", ConvertStatusToResponse(status).c_str());
	return status;
}

BEP_STATUS_TYPE IGryphonChannel::processNewMessage(const uint8_t *inBuf)
{
	unsigned short datalen;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::processNewMessage()\n");
	datalen = 256 * inBuf[4] + inBuf[5];
	// pull out pertinant data received, if any
	switch (inBuf[0])  // check who this message is from
	{
	case SD_SERVER:		  // from the server
		status = processNewServerMsg(inBuf, datalen);
		break;	// end of case SD_SERVER
	case SD_CARD:  // we should only get messages for our card (channel)
		status = processNewCardMsg(inBuf, datalen);
		break;
	case SD_USDT:
		status = processNewUsdtMsg(inBuf, datalen);
		break;
	case SD_J1939TP:
		status = processNewJ1939ConfigurationtMsg(inBuf, datalen);
		break;
	default:
		status = BEP_STATUS_HARDWARE;
		Log(LOG_ERRORS, "Unknown message [%02X] returned from the Gryphon box!", inBuf[0]);
		break;
	}  // end of switch on source of message
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::processNewMessage() - status: %s\n", ConvertStatusToResponse(status).c_str());
	return status;
}

void IGryphonChannel::CopyEventNames(const void *inBuf, const uint16_t datalen)
{
	unsigned char *locMsg;
	int scratch = 0;
	EvNames *evNS;
	EvNames *evND;
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::CopyEventNames()\n");
	locMsg = (unsigned char *) inBuf;
	// first one is located at offset 16
	for (int ii = 16 ; (ii < datalen) && (scratch < MAX_EVNAMES); scratch++,ii+=20)
	{
		evNS = (EvNames *) &locMsg[ii];
		evND = & EventIDs.event[scratch];
		evND->number = evNS->number;

		strncpy(evND->name,evNS->name,19);
		evND->terminator = 0;
		for (int jj=18; jj>0;jj--)
		{
			if (evND->name[jj] != ' ') break;
			else evND->name[jj]	= 0;
		}
	}
	if (scratch < MAX_EVNAMES)
	{	// after last one is copied, set next entry to {0,"\0",0xFF}
		evND = & EventIDs.event[scratch + 1];
		evND->number = 0;
		evND->name[0] = 0xFF;
		evND->terminator = 0xFF;
	}

	if (EventNamesLoaded.Acquire()== EOK)
	{
		EventNamesLoaded.Signal(true);
		EventNamesLoaded.Release();
	}
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::CopyEventNames()\n");
}

uint8_t IGryphonChannel::UseContext()
{
	return( GlobalContext++);
}


int IGryphonChannel::ClosePort()
{
	int retVal = 0;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::ClosePort()\n");

	if ( m_comPortFd != -1)
	{
		// Unregister with the USDT
		UnRegisterWithUsdt();

		// Stop receiving incoming TCP data from this socket
		shutdown( m_comPortFd, 0);

		// Let the base class clean up
		retVal = RawCommProxy::ClosePort();
	}

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::ClosePort()\n");

	return( retVal);
}

ResManagerCmd_t IGryphonChannel::HandleReplyTimeoutPulse(io_pulse_t &pulse)
{
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::HandleReplyTimeoutPulse($%X)\n", pulse.value.sival_int);

	if (m_blockedReaders.Lock() == EOK)
	{
		BlockedReaderListItr_t itr = m_blockedReaders.begin();

		while (itr != m_blockedReaders.end())
		{
			GryphonIoOcb_t*   client;
			if ( (client = (GryphonIoOcb_t*)itr->first) != NULL)
			{
				if( IsModuleIDPresent(client->moduleIDs,pulse.value.sival_int))
				{
					const BlockedReader_t blockedClient = itr->second;

					itr = m_blockedReaders.erase(itr);
					MsgError(blockedClient.replyId,ETIMEDOUT);
					Log( LOG_FN_ENTRY, "Unblocking blocked reader on module %s\n", GetModuleIDsString(client->moduleIDs).c_str());
				}
				else
				{
					itr++;
				}
			}
			else
			{
				Log( LOG_ERRORS, "Found NULL client reference in blocked readers list\n");
				itr++;
			}
		}
		m_blockedReaders.Unlock();
	}

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::HandleReplyTimeoutPulse($%X)\n", pulse.value.sival_int);

	return(RES_MGR_CMD_NONE);
}

int IGryphonChannel::ArmReplyTimeoutTimer( CommIoOcb_t *clientOcb, bool forceArm)/*=false*/
{
	GryphonIoOcb_t*     gryphClient = (GryphonIoOcb_t*)clientOcb;
	int                 retVal = -1;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::ArmReplyTimeoutTimer(), module $%s\n", GetModuleIDsString(gryphClient->moduleIDs).c_str());
	// Set the pulse value for the reply timeout timer to the module ID of
	//	the client that we are acting on
	clientOcb->replyTimeoutTimer.Initialize( m_mssgCoid,REPLY_TIMEOUT_TIMER, gryphClient->moduleIDs[0]);

	retVal = RawCommProxy::ArmReplyTimeoutTimer( clientOcb, forceArm);

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::ArmReplyTimeoutTimer(), module $%s\n", GetModuleIDsString(gryphClient->moduleIDs).c_str());

	return( retVal);
}

#if 0
void IGryphonChannel::UpdateClientFifos( const char *buff, size_t len, int flag)
{
	size_t locLen;
	PortSubscrListItr_t rxItr;
	uint16_t locModuleId;
	uint16_t locRespCode;
	uint16_t anyone = 0;
	uint8_t shortBuff[4];
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::UpdateClienFifos()\n");
	// Get the module ID and response code
	locModuleId = getModuleId(buff);
	locRespCode = getRespCode(buff);
	// Log the data
	Log( LOG_DETAILED_DATA, "Found message from module ID: %04X\n", locModuleId);
	if (m_blockedReaders.Lock() == EOK)
	{
		BlockedReaderListItr_t itr;
		for (rxItr = m_rxSubscribers.begin(); rxItr != m_rxSubscribers.end(); rxItr++)
		{
			GryphonIoOcb_t*   client = (GryphonIoOcb_t*)rxItr->second;
			if ( client->moduleID == locModuleId)
			{
				if (( client->expectedResponse = locRespCode) ||
				   ( client->expectedResponse = 0x7F))	// NAK code
				{
					// Got a reply from the module, so stop the reply timer
					client->proxyOcb.replyTimeoutTimer.Stop();

					Log( LOG_DETAILED_DATA, "Found subscribing client wanting it: %d\n",
						 client->proxyOcb.baseOcb.m_ioOcb.attr->uid);
					anyone++;
					shortBuff[0] = (len / 0x1000000) & 0xFF;
					shortBuff[1] = (len / 0x10000) & 0xFF;
					shortBuff[2] = (len / 0x100) & 0xFF;
					shortBuff[3] = (len / 0x1) & 0xFF;

					SerialString_t clientData( shortBuff, 4);
					clientData += SerialString_t( (uint8_t*)buff, len);

					AddToClientFifo( clientData, (CommIoOcb_t *)client);
				}
			}
		}

		for (itr = m_blockedReaders.begin(); itr != m_blockedReaders.end();itr++)
		{
			GryphonIoOcb_t*   client = (GryphonIoOcb_t*)itr->first;
			if ( client->moduleID == locModuleId)
			{
				if (( client->expectedResponse = locRespCode) ||
				   ( client->expectedResponse = 0x7F))	// NAK code
				{
					Log( LOG_DETAILED_DATA, "Found blocked client wanting it: %d\n",
						 client->proxyOcb.baseOcb.m_ioOcb.attr->uid);
					BlockedReader_t &client2 = itr->second;

					locLen = min(client2.readMsg.i.nbytes, (int32_t)len);

					const size_t rxLen = locLen;
					uint8_t rxBuf[rxLen];

					locLen = client->proxyOcb.rxSubscription->fifo.GetBytes(rxBuf,rxLen);
					m_blockedReaders.erase(itr);
					if (locLen > 0)
					{
						MsgReply(client2.replyId,locLen,rxBuf,locLen);
					}
					else
					{
						MsgReply(client2.replyId,ENODATA,NULL,0);
					}
				}

			}
		}
		m_blockedReaders.Unlock();
	}
	// log error message, could not lock blocked client list
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::UpdateClienFifos()  %d client(s)\n", anyone);
}
#else

bool IGryphonChannel::CanAddToClientFifo(const SerialString_t &data, CommIoOcb_t *ocb)
{
	GryphonIoOcb_t  *client = (GryphonIoOcb_t*)ocb;
	const char      *buff = (const char *)data.c_str();
	uint32_t        locModuleId;
	uint16_t        locRespCode;
	bool            canAdd = false;
	bool 			subscriptionFilterMatches = false;
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::CanAddToClientFifo()\n");

	// Check with base class first for filter strings
	if ( RawCommProxy::CanAddToClientFifo(data,ocb) == true)
	{
		// Get the module ID and response code
		locModuleId = getModuleId(buff);
		locRespCode = getRespCode(buff);

		Log( LOG_DEV_DATA, "IGryphonChannel::CanAddToClientFifo() stored module IDs: $%s incoming module ID: $%04X\n",
			 GetModuleIDsString(client->moduleIDs).c_str(), locModuleId);
//		bool isBroadcastModuleID = IsBroadcastModuleID(locModuleId);
//		bool filterMatches = false;
//		if(isBroadcastModuleID)
//		{//check if client has a matching filter
		//JPS do not require serial server to contain PGNs we will be requesting, this will be handled by filter
			subscriptionFilterMatches = SubscriptionFilterMatchCheck(data, ocb);  //note, this will return false if no filter exists
		//}
		// If the data is from the client's module or client subscribed
		if( IsModuleIDPresent(client->moduleIDs,locModuleId) || subscriptionFilterMatches)//(isBroadcastModuleID &&	filterMatches))
		{
			Log( LOG_DEV_DATA, "IGryphonChannel::CanAddToClientFifo() stored resp code: $%04X incoming resp code: $%04X\n",
				 client->expectedResponse, locRespCode);
			// And it is the response the client is expecting or a NAK response
			//Logic is incorrect - else will never be reached... somehow everything is working?
			if ((client->expectedResponse = locRespCode) || (client->expectedResponse = 0x7F))
			{
				canAdd = true;
				// Clear the response pending flag for this module ID
				if(UsingGryphonUSDT() == false)
				{
					ClearModuleResponsePending(client->moduleIDs);
				}
			}
			else
			{
				Log( LOG_DEV_DATA, "Not OK to add to client fifo because response was unexpected: $%04X != $%04X \n",
					 locRespCode, client->expectedResponse);
			}
		}
		else
		{
			Log( LOG_DEV_DATA, "Not OK to add to client fifo because module ID does not match: $%04X != $%s\n",
				 locModuleId, GetModuleIDsString(client->moduleIDs).c_str());
		}
	}

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::CanAddToClientFifo(%s)\n", canAdd ? "true" : "false");

	return( canAdd);
}

/**
 * Method used to check if a serial response string can be added to a
 * client's rx FIFO
 *
 * @param data   Serial string reseived from the port
 * @param ocb    Client connection identifier
 * @return true if the data should be added to the client's FIFO, false otherwise
 */
bool IGryphonChannel::SubscriptionFilterMatchCheck( const SerialString_t &data, CommIoOcb_t *ocb)
{
	bool                            retVal = true;
	LogPortFilterStringList         &fltrList = ocb->rxSubscription->filterList;
	LogPortFilterStringListItr_t    itr;

	// If client has any filter strings
	if( fltrList.size() > 0)
	{
		fltrList.Lock();
		for( itr=fltrList.begin(); itr!=fltrList.end(); itr++)
		{
			// Get reference to the filter string
			const LogPortFilterString &filter = *itr;
			// IF data matches the filter string
			if( filter.IsStringValid( data) == true)
			{
				// Ok to ADD to FIFO
				Log( LOG_DETAILED_DATA, "Response matches filter string\n");
				retVal = true;
				break;
			}
			else
			{
				// No match...look at next filter
				Log( LOG_DETAILED_DATA, "Response DOES NOT match filter string\n");

				SerialString_t::const_iterator  sItr = sItr=data.begin();
				LogPortFilterStringCItr_t       lItr = lItr=filter.begin();

				// Size must match first or filter must be set to variable length
				if( (filter.size() == data.size()) || (filter.GetRespLenType() == 1))
				{
					// Examine each element of each string
					while( (sItr!=data.end()) && (lItr!=filter.end()))
					{
						Log( LOG_DETAILED_DATA, "data: 0x%02X : filter0x%02X\n");
						// Look at next element
						sItr++;
						lItr++;
					}
				}
				else
				{
					Log( LOG_DETAILED_DATA, "Filter sizes do not match\n");
				}
				//Test Log message, remove
				Log( LOG_DETAILED_DATA, "Not expecting NULL character reflection\n");

				retVal = false;
			}
		}
		fltrList.Unlock();
	}
	else
	{
		Log( LOG_DETAILED_DATA, "No filter strings for client\n");
		retVal = false;
	}
	return( retVal);
}

bool IGryphonChannel::AddToClientFifo(const char *buff, size_t len, CommIoOcb_t *ocb)
{
	SerialString_t  gryphMssg;
	uint8_t         shortBuff[4];
	bool            retVal;

	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::AddToClientFifo( %d)\n", len);
	if(UsingGryphonUSDT())
	{
		// Prepend the length of the message to the original message
		shortBuff[0] = (len / 0x1000000) & 0xFF;
		shortBuff[1] = (len / 0x10000) & 0xFF;
		shortBuff[2] = (len / 0x100) & 0xFF;
		shortBuff[3] = (len / 0x1) & 0xFF;
		gryphMssg = SerialString_t( shortBuff, 4) + SerialString_t( (uint8_t*)buff, len);
	}
	else
	{
		gryphMssg = SerialString_t( (uint8_t*)buff, len);
	}

	// Let the base class do all the real work
	retVal = RawCommProxy::AddToClientFifo((const char*)gryphMssg.c_str(), gryphMssg.size(), ocb);
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::AddToClientFifo( %d)\n", gryphMssg.size());

	return( retVal);
}

#endif

bool IGryphonChannel::RegisterWithUsdt(string pathName /*=""*/, int attempt /*=1*/)
{
	RegMsg locMsg;
	int locSize;
	uint8_t *locStr;
	int retVal;
    bool registrationOk = false;
    m_usdtRegistrationAttempts = 1; 
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::RegisterWithUsdt(Attempt: %d)\n", attempt);

	locStr = (uint8_t *) &locMsg;
	locSize = sizeof(locMsg);
	memset(locStr,0,sizeof(locMsg));
	// Start populating the message fields
	locMsg.header.cmd = CMD_USDT_REGISTER;
	locMsg.header.context = UseContext();
	locMsg.header.reserved = 0;
	if(m_moduleIDByteLength == 6)
	{
		//locMsg.doRegOn     = 3;	//0011 29 bit headers
		locMsg.doRegOn     = 5;	//0101 29 bit and 11 bit headers
	}
	else if(m_moduleIDByteLength == 4)
	{
		locMsg.doRegOn     = 3;	//0011 29 bit headers
	}
	else
	{
		locMsg.doRegOn     = 1;	//0001 11 bit headers
	}
	locMsg.doEchoMsg   = 0;
	locMsg.doRecvOpt   = RECV_NORMAL;	  // 0x01,
	locMsg.funcAddrCnt = 0;
	// Setup the range of addresses to communicate with
	// NOTE: Use m_nodePairSetupMap.incoming and m_nodePairSetupMap.outgoing.
	// ALGORITHM: Setup a node pairing for each node pair in the map.
	for (int index = 0; index < m_nodePairSetupCount; index++)
	{
		vector <UINT32> responseIDs;
		locMsg.usdtBlock[index].numberOfIDs         = m_nodePairSetupMap[index].numberOfPairs;	 // Number of IDs in this range
		locMsg.usdtBlock[index].firstUSDTRequestID  = m_nodePairSetupMap[index].outgoing;
		locMsg.usdtBlock[index].firstUSDTResponseID = m_nodePairSetupMap[index].incoming;
		locMsg.usdtBlock[index].firstUUDTResponseID = m_nodePairSetupMap[index].uudtIncoming;
		// Need to reverse the order of the bytes for the USDT block
		ReverseBytes(locMsg.usdtBlock[index].numberOfIDs);
		ReverseBytes(locMsg.usdtBlock[index].firstUSDTRequestID);
		ReverseBytes(locMsg.usdtBlock[index].firstUSDTResponseID);
		ReverseBytes(locMsg.usdtBlock[index].firstUUDTResponseID);
	}
	// Perform the registration
	if (UsdtRegistered.Acquire()== EOK)
	{
		UsdtRegistered.SetValue(false);

		Log( LOG_DETAILED_DATA,
			 "\nUSDT request:\n"
			 "\t\tHeader:                           %02x %02x %02x %02x\n"
			 "\t\tOptions:                          %02x %02x %02x %02x\n",
			 locStr[0], locStr[1], locStr[2], locStr[3],
			 locStr[4], locStr[5], locStr[6], locStr[7]);
		retVal = SendRawFrame(locStr, locSize, FT_CMD, SD_USDT);
		if (UsdtRegistered.Wait(true,mSEC_nSEC(5000)) == EOK)
		{
			Log( LOG_DEV_DATA, "Usdt Registered for channel %d\n", m_channelId);
            registrationOk = true;
		}
		else
		{
			Log( LOG_ERRORS, "Usdt FAILED for channel %d (no response)\n", m_channelId);
            /*UsdtRegistered.Release();
            BposSleep(750);
            m_usdtRegistrationAttempts++;
            RegisterWithUsdt(pathName); */
		}
		UsdtRegistered.Release();
	}
	else
	{
		Log( LOG_ERRORS, "Usdt FAILED for channel %d (could not acquire)\n", m_channelId);
	}

    if (pathName != "")
    {
        try
        {
            Log(LOG_DEV_DATA, "Archiving USDT Registration(%s) results for issue tracking...", registrationOk ? "OK" : "NOK");
            char testDate[256], testTime[256];
            struct tm *localTime;
            time_t currentTime;
        
            // Get the time and machine number for reporting purposes
            currentTime = time(NULL);
            localTime = localtime(&currentTime);
            string dateFormat = "%F";
            string timeFormat = "%H:%M:%S";
            strftime(testDate, sizeof(testDate), dateFormat.c_str(), localTime);
            strftime(testTime, sizeof(testTime), timeFormat.c_str(), localTime);
            
            FILE *dataFile;
            string fileName = "/home/CCRT/Rewrite/Logs/UsdtReg_" + pathName + ".txt";
            if((dataFile = fopen(fileName.c_str(), "a+")) != NULL)
            {
                fprintf(dataFile, "%s %s - USDT Registration %s\n", 
                        testDate, testTime, registrationOk ? "OK" : "NOK");
            }
            // Close the file
            fclose(dataFile);
        }
        catch(...)
        {
            Log(LOG_DEV_DATA, "Error archiving USDT registration results.");
        }
    }

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::RegisterWithUsdt()\n");
	return(registrationOk);
}


bool IGryphonChannel::RegisterWithUsdtNonLegacy(string pathName /*=""*/, int attempt /*=1*/)
{
	RegMsgNonLegacy locMsg;
	int locSize;
	uint8_t *locStr;
	int retVal;
    bool registrationOk = false;
    m_usdtRegistrationAttempts = 1; 
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::RegisterWithUsdtNonLegacy(Attempt: %d)\n", attempt);

	locStr = (uint8_t *) &locMsg;
	locSize = sizeof(locMsg) - ((MAX_USDT_RANGES - m_nodePairSetupCount) * 20);
    if(locSize < 0)
     locSize = sizeof(locMsg);
	memset(locStr,0,sizeof(locMsg));
	// Start populating the message fields
	locMsg.header.cmd = CMD_USDT_REGISTER_NON_LEGACY;
	locMsg.header.context = UseContext();
	locMsg.header.reserved = 0;   
    /*Action    */  locMsg.doRegOn = 1;     // 11/29 bit header designation now set elsewhere - Set to 1 for registration 
	/*TX Options*/  locMsg.doEchoMsg   = 0;
	/*RX Opitons*/  locMsg.doRecvOpt   = RECV_NORMAL;	  // 0x01,
	/*Reserved  */  locMsg.funcAddrCnt = 0; // This byte is simply a reserve byte now - MUST BE ZERO
     
	// Setup the range of addresses to communicate with
	// NOTE: Use m_nodePairSetupMap.incoming and m_nodePairSetupMap.outgoing.
	// ALGORITHM: Setup a node pairing for each node pair in the map.
	for (int index = 0; index < m_nodePairSetupCount; index++)
	{
		vector <UINT32> responseIDs;
		locMsg.usdtBlock[index].numberOfIDs         = m_nodePairSetupMap[index].numberOfPairs;	 // Number of IDs in this range
		locMsg.usdtBlock[index].firstUSDTRequestID  = m_nodePairSetupMap[index].outgoing;
		locMsg.usdtBlock[index].firstUSDTResponseID = m_nodePairSetupMap[index].incoming;
		locMsg.usdtBlock[index].firstUUDTResponseID = m_nodePairSetupMap[index].uudtIncoming;

        // Set control bits (31-29) For number of Ids
		// 31 -	reserved
		// 30 -
		// 		1 J1939 style length where source / destination bytes are swapped
		// 		0 regular non J1939 style
		// 29 -	reserved 
		// 0 - 28 length
		 
		// Set Control bits (31-29) for Ids / header blocks (USDT request / response and UUDT)
        // Bit 31 -	
		// 			1 Block contains 29-bit IDs / headers
		// 			0 Block contains 11-bit IDs / headers	
        // Bit 30 - reserved
        // Bit 29 -	
		// 			1 Uses extended addressing
		// 			0 Does not use extended addressing
		// 0 - 28 -	Address

		if ( m_nodePairSetupMap[index].is29BitHeader )
        {
            locMsg.usdtBlock[index].numberOfIDs = (locMsg.usdtBlock[index].numberOfIDs & 0x1FFFFFFF);
			if (m_j1939Channel)
			{//bit 30 = 1
				locMsg.usdtBlock[index].numberOfIDs = (locMsg.usdtBlock[index].numberOfIDs | 0x40000000);
			}
			locMsg.usdtBlock[index].firstUSDTRequestID  = (locMsg.usdtBlock[index].firstUSDTRequestID & 0x1FFFFFFF);
			locMsg.usdtBlock[index].firstUSDTResponseID = (locMsg.usdtBlock[index].firstUSDTResponseID & 0x1FFFFFFF);
			locMsg.usdtBlock[index].firstUUDTResponseID = (locMsg.usdtBlock[index].firstUUDTResponseID & 0x1FFFFFFF);
			// Set as 29 bit header, Bit 31 = 1
			locMsg.usdtBlock[index].firstUSDTRequestID  = (locMsg.usdtBlock[index].firstUSDTRequestID | 0x80000000);
			locMsg.usdtBlock[index].firstUSDTResponseID = (locMsg.usdtBlock[index].firstUSDTResponseID | 0x80000000);
			locMsg.usdtBlock[index].firstUUDTResponseID = (locMsg.usdtBlock[index].firstUUDTResponseID | 0x80000000);
        }
        else
            locMsg.usdtBlock[index].numberOfIDs = (locMsg.usdtBlock[index].numberOfIDs & 0x000007FF);
        
		// Need to reverse the order of the bytes for the USDT block
		ReverseBytes(locMsg.usdtBlock[index].numberOfIDs);
		ReverseBytes(locMsg.usdtBlock[index].firstUSDTRequestID);
		ReverseBytes(locMsg.usdtBlock[index].firstUSDTResponseID);
		ReverseBytes(locMsg.usdtBlock[index].firstUUDTResponseID);
	}
	// Perform the registration
	if (UsdtRegistered.Acquire()== EOK)
	{
		UsdtRegistered.SetValue(false);

		Log( LOG_DETAILED_DATA,
			 "\nUSDT request:\n"
			 "\t\tHeader:                           %02x %02x %02x %02x\n"
			 "\t\tOptions:                          %02x %02x %02x %02x\n",
			 locStr[0], locStr[1], locStr[2], locStr[3],
			 locStr[4], locStr[5], locStr[6], locStr[7]);
		retVal = SendRawFrame(locStr, locSize, FT_CMD, SD_USDT);
		if (UsdtRegistered.Wait(true,mSEC_nSEC(5000)) == EOK)
		{
			Log( LOG_DEV_DATA, "Usdt Registered for channel %d\n", m_channelId);
            registrationOk = true;
		}
		else
		{
			Log( LOG_ERRORS, "Usdt FAILED for channel %d (no response)\n", m_channelId);
            /*UsdtRegistered.Release();
            BposSleep(750);
            m_usdtRegistrationAttempts++;
            RegisterWithUsdt(pathName); */
		}
		UsdtRegistered.Release();
	}
	else
	{
		Log( LOG_ERRORS, "Usdt FAILED for channel %d (could not acquire)\n", m_channelId);
	}

    if (pathName != "")
    {
        try
        {
            Log(LOG_DEV_DATA, "Archiving USDT Registration(%s) results for issue tracking...", registrationOk ? "OK" : "NOK");
            char testDate[256], testTime[256];
            struct tm *localTime;
            time_t currentTime;
        
            // Get the time and machine number for reporting purposes
            currentTime = time(NULL);
            localTime = localtime(&currentTime);
            string dateFormat = "%F";
            string timeFormat = "%H:%M:%S";
            strftime(testDate, sizeof(testDate), dateFormat.c_str(), localTime);
            strftime(testTime, sizeof(testTime), timeFormat.c_str(), localTime);
            
            FILE *dataFile;
            string fileName = "/home/CCRT/Rewrite/Logs/UsdtReg_" + pathName + ".txt";
            if((dataFile = fopen(fileName.c_str(), "a+")) != NULL)
            {
                fprintf(dataFile, "%s %s - USDT Registration %s\n", 
                        testDate, testTime, registrationOk ? "OK" : "NOK");
            }
            // Close the file
            fclose(dataFile);
        }
        catch(...)
        {
            Log(LOG_DEV_DATA, "Error archiving USDT registration results.");
        }
    }

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::RegisterWithUsdtNonLegacy()\n");
	return(registrationOk);
}

int IGryphonChannel::UnRegisterWithUsdt(void)
{
	RegMsg locMsg;
	int locSize;
	uint8_t *locStr;
	int retVal;
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::UnRegisterWithUsdt()\n");

	locStr = (uint8_t *) &locMsg;
	locSize = sizeof(locMsg);
	memset(locStr,0,locSize);
	// Start populating the message fields
    if(m_nonLegacyDevice)
        locMsg.header.cmd = CMD_USDT_REGISTER_NON_LEGACY;
    else
        locMsg.header.cmd = CMD_USDT_REGISTER;
	locMsg.header.context = UseContext();
	locMsg.header.reserved = 0;
	locMsg.doRegOn     = 0;				// 0 to unregister
	locMsg.doEchoMsg   = 0;				// Ignored when unregistering
	locMsg.doRecvOpt   = RECV_NORMAL;	// Ignored when unregistering
	locMsg.funcAddrCnt = 0;				// Ignored when unregistering

	// Perform the registration
	if (UsdtRegistered.Acquire()== EOK)
	{
		UsdtRegistered.SetValue(false);

		Log( LOG_DETAILED_DATA,
			 "\nUSDT request:\n"
			 "\t\tHeader:                           %02x %02x %02x %02x\n"
			 "\t\tOptions:                          %02x %02x %02x %02x\n",
			 locStr[0], locStr[1], locStr[2], locStr[3],
			 locStr[4], locStr[5], locStr[6], locStr[7]);
		retVal = SendRawFrame(locStr, locSize, FT_CMD, SD_USDT);
		if (UsdtRegistered.Wait(true, mSEC_nSEC(5000)) == EOK)
		{
			UsdtRegistered.SetValue(false);
			Log( LOG_DETAILED_DATA, "Usdt Unregistered for channel %d\n", m_channelId);
		}
		else
		{
			Log( LOG_ERRORS, "Usdt Unregister FAILED for channel %d (no response)\n", m_channelId);
		}
		UsdtRegistered.Release();
	}
	else
	{
		Log( LOG_ERRORS, "Usdt Unregister FAILED for channel %d (could not acquire)\n", m_channelId);
	}

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::UnRegisterWithUsdt()\n");
	return(EOK);
}

int IGryphonChannel::SetMinimumMessageSeperationTimeMultiplier(float seperationTimeMultiplier)
{
	int retVal = EOK;
	if(seperationTimeMultiplier > 0.0)
	{	// Set a mulitiplier for the ST Min parameter
		Log(LOG_DEV_DATA, "Setting ST Min Multiplier to %f", seperationTimeMultiplier);
		StMinMsg locMsg;
		int locSize = sizeof(locMsg);  // Get the size of the message structure
		uint8_t *locStr = (uint8_t *) &locMsg;	 // Get a pointer to the message structure
		// Make sure the message structure is clear
		memset(locStr,0,locSize);
		// Populate the message structure
		locMsg.header.cmd      = CMD_USDT_SET_STMIN_MULT;
		locMsg.header.context  = UseContext();
		locMsg.header.reserved = 0;
		// Set the ST Min time
		union
		{
			float fVal;
			UINT32 hVal;
		} conversion;
		conversion.fVal = seperationTimeMultiplier;
		locMsg.stMinMultiplier = conversion.hVal;
		// Send the ST min multiplier to the box
		if(UsdtStMinMultiplier.Acquire() == EOK)
		{
			UsdtStMinMultiplier.SetValue(false);
			Log(LOG_DETAILED_DATA, 
				"\nUSDT Request:\n"
				"\t\tHeader:          %02X %02X %02X %02X\n"
				"\t\tOptions:         %02X %02X %02X %02X\n",
				locStr[0], locStr[1], locStr[2], locStr[3],
				locStr[4], locStr[5], locStr[6], locStr[7]);
			retVal = SendRawFrame(locStr, locSize, FT_CMD, SD_USDT);
			if(UsdtStMinMultiplier.Wait(true,mSEC_nSEC(5000)) == EOK)
			{
				Log(LOG_DETAILED_DATA, "Usdt set ST min multiplier to %f", seperationTimeMultiplier);
			}
			else
			{
				Log(LOG_ERRORS, "Usdt FAILED to set ST min multiplier to %f", seperationTimeMultiplier);
			}
			UsdtStMinMultiplier.Release();
		}
		else
		{
			Log(LOG_ERRORS, "Usdt FAILED ST min multiplier - could not acquire");
		}
	}
	else
	{
		Log(LOG_DEV_DATA, "Minimum seperation time set to 0.0.  Using system default and not sending command");
	}
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::SetMinimumMessageSeperationTimeMultiplier()");
	return retVal;
}

int IGryphonChannel::SetMinimumMessageSeperationTime(UINT8 seperationTime)
{
	int retVal = EOK;
	if(seperationTime > 0)
	{	// Set a mulitiplier for the ST Min parameter
		Log(LOG_DEV_DATA, "Setting ST Min to %d", seperationTime);
		StMinFcMsg locMsg;
        // Using static size definition because of Data Structure Alignment protocol
        // - StMinFcMsg struct will get 1 byte padding added to the end
        // - total size has to be a multiple of the largest member (in this case that would be the short for reserve)
		int locSize =  sizeof(locMsg);//STMIN_FC_MSG_SIZE;  // Get the size of the message structure
		uint8_t *locStr = (uint8_t *) &locMsg;	 // Get a pointer to the message structure
		// Make sure the message structure is clear
		memset(locStr,0,locSize);
		// Populate the message structure
		locMsg.header.cmd      = CMD_USDT_SET_STMIN_FC;
		locMsg.header.context  = UseContext();
		locMsg.header.reserved = 0;
		// Set the ST Min time
        locMsg.stMinValue = seperationTime;
        locMsg.padding1 = 0x00;
        locMsg.padding2 = 0x0000;
		// Send the ST min multiplier to the box
		if(UsdtStMinFlowControl.Acquire() == EOK)
		{
			UsdtStMinFlowControl.SetValue(false);
			Log(LOG_DETAILED_DATA, 
				"\nUSDT Request:\n"
				"\t\tHeader:          %02X %02X %02X %02X\n"
				"\t\tOptions:         %02X\n",
				locStr[0], locStr[1], locStr[2], locStr[3],
				locStr[4]);
            Log(LOG_DETAILED_DATA, "Sending raw frame - locSize = %d", locSize);
			retVal = SendRawFrame(locStr, locSize, FT_CMD, SD_USDT, true, STMIN_FC_MSG_SIZE);
			if(UsdtStMinFlowControl.Wait(true,mSEC_nSEC(5000)) == EOK)
			{
				Log(LOG_DETAILED_DATA, "Usdt set ST min multiplier to %d", seperationTime);
			}
			else
			{   /*
                Log(LOG_ERRORS, "No response?");
                // Try again
                MsgSendPulse(m_threadCoId,SIGEV_PULSE_PRIO_INHERIT,SERIAL_DATA_READY,0);
                if(UsdtStMinFlowControl.Wait(true,mSEC_nSEC(5000)) == EOK)
    			{
    				Log(LOG_DETAILED_DATA, "Usdt set ST min multiplier to %d", seperationTime);
    			}
    			else
    			{
                    Log(LOG_ERRORS, "Usdt FAILED to set ST min to %d", seperationTime);
                }
                */
				Log(LOG_ERRORS, "Usdt FAILED to set ST min to %d", seperationTime);
                
			}
			UsdtStMinFlowControl.Release();
		}
		else
		{
			Log(LOG_ERRORS, "Usdt FAILED ST min - could not acquire");
		}
	}
	else
	{
		Log(LOG_DEV_DATA, "Minimum seperation time set to 0.  Using system default and not sending command");
	}
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::SetMinimumMessageSeperationTime()");
	return retVal;
}

void IGryphonChannel::ReverseBytes(unsigned int &byteString)
{
	UINT8 bytes[4];
	// Get the individual bytes
	for (UINT8 index = 0; index < 4; index++)
		bytes[index] = (byteString >> (8*index)) & 0x000000FF;
	// Clear the storage location
	byteString = 0x00000000;
	// Reassemble the bytes
	for (UINT8 index = 0; index < 4; index++)
		byteString = byteString | (bytes[index] << (8*(3-index)));
}

bool IGryphonChannel::IsResponsePending(const vector<UINT32> &moduleIds)
{
	bool responsePending = true;  // Default to response pending
	if((errno = m_responsePendingMutex.Acquire()) == EOK)
	{
		for(UINT16 index = 0; index < moduleIds.size(); index++)
		{
			ResponsePendingItr iter = find(m_moduleResponsePending.begin(), m_moduleResponsePending.end(), moduleIds);
			responsePending = (iter != m_moduleResponsePending.end());
			Log(LOG_DETAILED_DATA, "Response pending for module ID %s: %s", GetModuleIDsString(moduleIds).c_str(), responsePending ? "True" : "False");
			if((errno = m_responsePendingMutex.Release()) != EOK)
				Log(LOG_ERRORS, "Could not release mutex in IGryphonChannel::IsResponsePending() - %s", strerror(errno));
			if(responsePending)
			{
				return true;
			}
		}

	}
	else
	{
		Log(LOG_ERRORS, "Could not acquire mutex in IGryphonChannel::IsResponsePending() - %s", strerror(errno));
	}
	return responsePending;
}

bool IGryphonChannel::IsModuleIDPresent(const vector<UINT32> incomingModuleIDs, int keyID)
{
	for(UINT16 index = 0; index < incomingModuleIDs.size(); index++)
	{
		if(incomingModuleIDs[index] == (UINT32)keyID)
		{
			return true;
		}
	}
	return false;
}

string IGryphonChannel::GetModuleIDsString(const vector<UINT32> incomingModuleIDs)
{
	string moduleIDs = "";
	for(UINT16 index = 0; index < incomingModuleIDs.size(); index++)
	{
		char buff[16];
		moduleIDs += CreateMessage(buff, sizeof(buff), " 0x%08X ", incomingModuleIDs[index]);
	}
	return moduleIDs;
}

void IGryphonChannel::SetModuleResponsePending(const vector<UINT32> &moduleIds)
{
	if((errno = m_responsePendingMutex.Acquire()) == EOK)
	{
		m_moduleResponsePending.push_back(moduleIds);
		Log(LOG_DETAILED_DATA, "Set response pending for module ID %s", GetModuleIDsString(moduleIds).c_str());

		if((errno = m_responsePendingMutex.Release()) != EOK)
			Log(LOG_ERRORS, "Could not release mutex in IGryphonChannel::SetModuleResponsePending() - %s", strerror(errno));
	}
	else
	{
		Log(LOG_ERRORS, "Could not acquire mutex in IGryphonChannel::SetModuleResponsePending() - %s", strerror(errno));
	}
}

void IGryphonChannel::ClearModuleResponsePending(const vector<UINT32> &moduleIds)
{
	if(IsResponsePending(moduleIds))
	{
		if((errno = m_responsePendingMutex.Acquire()) == EOK)
		{
			m_moduleResponsePending.remove(moduleIds);
			Log(LOG_DETAILED_DATA, "Cleared response pending for module ID %s", GetModuleIDsString(moduleIds).c_str());

			if((errno = m_responsePendingMutex.Release()) != EOK)
				Log(LOG_ERRORS, "Could not release mutex in IGryphonChannel::ClearModuleResponsePending() - %s", strerror(errno));
		}
		else
		{
			Log(LOG_ERRORS, "Could not acquire mutex in IGryphonChannel::ClearModuleResponsePending() - %s", strerror(errno));
		}
	}
	else
	{
		Log(LOG_ERRORS, "WARNING: No response pending entry for module ID %s", GetModuleIDsString(moduleIds).c_str());
	}
}
int IGryphonChannel::EnableJ1939TransportProtocol()
{
	uint8_t localBuffer[20];
	int locSize;
	int retVal = EOK;
	Log( LOG_FN_ENTRY, "Enter IGryphonChannel::EnableJ1939TransportProtocol()\n");
	// Clear out the message data
	memset(localBuffer,0,sizeof(localBuffer));
	localBuffer[0] = CMD_SERVER_SET_SERVICE;
	localBuffer[1] = UseContext();
	localBuffer[2] = 0x00;
	localBuffer[3] = 0x00;
	localBuffer[4] = TP_J1939;
	locSize = 5;
	// need to wait for acknowledgement
	if (J1939TPEnableAck.Acquire()== EOK)
	{
		J1939TPEnableAck.SetValue(false);
		retVal = SendRawFrame(localBuffer, locSize, FT_CMD, SD_SERVER);
		if (J1939TPEnableAck.Wait(true,mSEC_nSEC(5000)) == EOK)
		{
		}
		J1939TPEnableAck.Release();
	}
	// Log the exit and return the status
	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::EnableJ1939TransportProtocol() retVal: %d\n", retVal);
	return(retVal);

}
int IGryphonChannel::ClaimJ1939Address(UINT8 addressToClaim)
{
	int retVal = EOK;
	Log(LOG_DEV_DATA, "Claiming address %d", addressToClaim);
	J1939AddressClaimMsg locMsg;
	// Using static size definition because of Data Structure Alignment protocol
	int locSize =  sizeof(locMsg);// Get the size of the message structure
	uint8_t *locStr = (uint8_t *) &locMsg;	 // Get a pointer to the message structure
	// Make sure the message structure is clear
	memset(locStr,0,locSize);
	// Populate the message structure
	locMsg.header.cmd      = CMD_J1939_ADDR_CLAIM;
	locMsg.header.context  = UseContext();
	locMsg.header.reserved = 0;
	// Set the ST Min time
	locMsg.addressToClaim = addressToClaim;
	locMsg.emulation = 0x01;
	locMsg.padding1 = 0x00;
	locMsg.padding2 = 0x0000;
	// Send the Address claim command to the server
	if(J1939AddressClaim.Acquire() == EOK)
	{
		J1939AddressClaim.SetValue(false);
		Log(LOG_DETAILED_DATA, 
			"\nUSDT Request:\n"
			"\t\tHeader:          %02X %02X %02X %02X\n"
			"\t\tOptions:         %02X\n",
			locStr[0], locStr[1], locStr[2], locStr[3],
			locStr[4]);
		Log(LOG_DETAILED_DATA, "Sending raw frame - locSize = %d", locSize);
		retVal = SendRawFrame(locStr, locSize, FT_CMD, SD_SERVER, true, J1939_ADDRESS_CLAIM_FC_MSG_SIZE);
		if(J1939AddressClaim.Wait(true,mSEC_nSEC(5000)) == EOK)
		{
			Log(LOG_DETAILED_DATA, "J1939 claimed address for tester %d", addressToClaim);
		}
		else
		{
			Log(LOG_ERRORS, "J1939 FAILED to claim address for tester %d", addressToClaim);
			
		}
		J1939AddressClaim.Release();
	}
	else
	{
		Log(LOG_ERRORS, "Claim address failure - could not acquire");
	}

	Log( LOG_FN_ENTRY, "Exit IGryphonChannel::ClaimJ1939Address() - return value %d", retVal);
	return retVal;
}

