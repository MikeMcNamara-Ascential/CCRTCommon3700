//******************************************************************************
// Description:
//		Multi-threaded resource manager base class which provides methods
//		for handling BEP XML messages. Uses a thread pool to handle XML
//		based messages and the main thread of execution to handle I/O
//		messages. Adapted from ResourceManager.cpp.
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Common/ResManagerXml.cpp $
// 
// 5     4/13/06 3:51a Cward
// Re-order the BepException and the XmlException catch blocks
//
// 2     2/15/06 4:03p Bmeinke
// Re-order the catch(BepException) and catch(XmlException) blocks
//
// 4     6/24/04 1:04a Cward
// Removed pound if 0 block. Updated the header block comments to the new
// standard.
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
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
// 2     6/26/03 4:27p Khykin
// Updated with Brians latest changes.
//
// 1     6/20/03 9:57a Khykin
// Initial checkin.
//
// 1     6/12/03 3:59p Khykin
// Initial checkin.
//
// 1     4/22/03 9:50a Khykin
// Kevins Working Checkin
//
// 12    3/26/03 10:32p Bmeinke
// Restore thread pool thread priorities to desired level (based on config
// file) before blocking in PoolBlockFunc()
//
// 11    2/21/03 7:36p Bmeinke
// If dispatch_block fails in IoMssgLoop(), do not return
// RESMGR_CMD_TERMINATE
//
// 10    1/03/03 4:22p Bmeinke
// Added checks on ResManagerBase::m_resMgrStatus value when creating new
// thread-pool threads to make sure we are not terminating
// Added more Logger statements
//
// 9     10/24/02 8:10p Bmeinke
// Added a printout after the IBepCommunication::HandleRequest() call in
// PoolHandlerFunc()
//
// 8     8/27/02 9:03p Bmeinke
// Removed extra thread ID data from thread pool logger statements
//
// 7     8/01/02 12:29p Bmeinke
// Removed compiler warnings
//
// 6     6/24/02 9:46a Bmeinke
// Added additional output to logger string when initializing the comm
// object inside InitializePoolParam()
//
// 5     6/13/02 3:12p Dan.erickson
// Clear message before calling HandleRequest().
// Make sure there is a notification list before trying to Notify.
//
// 4     4/16/02 2:27p Bmeinke
//
// 3     2/27/02 3:03p Bmeinke
// Changed order of arguments passed to IBepCommunication::Initialize
// Changed threadPoolParam_t to THREAD_POOL_PARAM_T in static thread pool
// methods for compatability with other thread pool objects
//
// 2     12/07/01 2:54p Brian.meinke
// Changed thread pool interface prototypes in the base class ResManagerMt
//
// 1     11/21/01 8:40p Brian.meinke
// Multi-threaded resource manager base class which provides methods for
// handling BEP XML messages.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "ResManagerXml.h"
#include <sys/netmgr.h>

//******************************************************************************

/**
 * Default constructor
 *
 * @param name   Name to register with the OS as
 */
ResManagerXml::ResManagerXml( const char *name) : ResManagerMt(),
	m_nameId( NULL), m_scanTimerId( -1), m_timerConnectId( -1)
{
	Log( LOG_FN_ENTRY, "Enter ResManagerXml::ResManagerXml( %s)\n", name);

	SetProcessName( name);

	ResManagerXml::m_ioMgrForOcb = this;

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::ResManagerXml( %s)\n", name);
}

ResManagerXml::~ResManagerXml()
{
	Log( LOG_FN_ENTRY, "Enter ResManagerXml::~ResManagerXml\n");

	// Clean up threads
	KillPoolThreads();

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::~ResManagerXml\n");
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void ResManagerXml::CleanupManager()
{
	Log( LOG_FN_ENTRY, "Enter ResManagerXml::CleanupManager\n");

	// Detach the terminate pulse from our resource manager's channel
	if( m_ioChnlPulseCode != -1)
	{
		Log( LOG_DEV_DATA, "Detach I/O channel pulse\n");
		pulse_detach(m_dispatchPtr, m_ioChnlPulseCode, 0);
		Log( LOG_DEV_DATA, "I/O channel pulse detached\n");
		m_ioChnlPulseCode = -1;
	}

	// Do base class cleanup
	ResManagerMt::CleanupManager();

	// Unregister our process
	if( m_nameId != NULL)
	{
		Log( LOG_DEV_DATA, "Unregister task name\n");
		BposUnRegisterTaskName( (int)m_nameId);
		m_nameId = NULL;
	}

	// Destroy the connection on which the pulse can be received
	if( m_mssgCoid != -1)
	{
		Log( LOG_DEV_DATA, "Detach pulse connection\n");
		ConnectDetach( m_mssgCoid);
		m_mssgCoid = -1;
	}

	// Clear data map
	m_dataTable.clear();

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::CleanupManager\n");
}

/**
 * method for component initialization.
 *
 * @param document Reference to a parsed configuration document.
 */
void ResManagerXml::Initialize(const XmlNode *document)
{
	XmlString	setupTag( CFG_SETUP_TAG);
	XmlString	dataTag( CFG_DATAMAP_TAG);

	Log( LOG_FN_ENTRY, "Enter ResManagerXml::Initialize\n");

	try
	{
		// Perform base class initialization
		ResManagerMt::Initialize( document);
		if( document != NULL)
		{
			try
			{
				// Read data map items from config file
				ReadDataMap( (const XmlElement*)document->getChild( dataTag));
			}
			catch( BepException &err)
			{
				Log( LOG_ERRORS, "\tError reading options in Initialize: %s\n",
					 err.GetReason());
			}
		}
	}
	catch( XmlException &xmlErr)
	{
		Log( LOG_ERRORS, "\tParse error in BepComponent::Initialize: %s\n",
			 xmlErr.GetReason());
	}
	catch( BepException &bepErr)
	{
		Log( LOG_ERRORS, "\tError in BepComponent::Initialize: %s\n",
			 bepErr.GetReason());
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::Initialize\n");
}

/**
 * Handles I/O mesages
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t ResManagerXml::IoMssgLoop()
{
	ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
	dispatch_context_t	*dpp, *newDpp;

	Log( LOG_FN_ENTRY, "Enter ResManagerXml::IoMssgLoop\n");

	dpp = dispatch_context_alloc( m_dispatchPtr);
	// Service resource manager type messages
	while( retVal == RES_MGR_CMD_NONE)
	{
		// Wait for data to come in
		if( (newDpp=dispatch_block( dpp)) != NULL)
		{
			// Check for a newly allocated dispatch context pointer
			if( dpp != newDpp)	dpp = newDpp;

			if( dpp->message_context.rcvid == 0)
			{
				retVal = HandleResMgrPulse(dpp->message_context.msg->pulse);
			}
			else
			{
				// Dispatch the message to the proper handler function
				dispatch_handler( dpp);
			}
		}
		else
		{
			Log( LOG_ERRORS, "\tdispatch_block error: %s\n", strerror( errno));
//			retVal = RES_MGR_CMD_TERMINATE;
		}

	}
	dispatch_context_free( dpp);
	Log( LOG_FN_ENTRY, "Exit ResManagerXml::IoMssgLoop\n");

	return( retVal);
}

/**
 * Reads the data tag names and properties
 *
 * @param setupNode Node to use as the root DATA_MAP node
 * @return 0 if successful
 *         -1 if error
 */
int ResManagerXml::ReadDataMap( const XmlElement *setupNode)
{
	const XmlNodeMap &childNodes = setupNode->getChildren();
	XmlNode			*xmlNode;
	dataMapItem_t	dataItem;
	dataTag_t		itemTag;
	int				nodeIdx, nodeCnt=childNodes.size();

	Log( LOG_FN_ENTRY, "Enter ResManagerXml::ReadDataMap\n");

	// Read each child node
	for( nodeIdx=0; nodeIdx<nodeCnt; nodeIdx++)
	{
		// Get node and tag
		xmlNode = (XmlNode*)childNodes.getNode( nodeIdx);
		itemTag = XmlToAscii( xmlNode->getName());

		try
		{
			// Parse the data item's attributes
			ReadDataMapItem( xmlNode, dataItem);

			// Add it to the table if no parsing errors
			m_dataTable.insert( itemTag, dataItem);
		}
		catch( XmlException &xmlErr)
		{
			Log( LOG_ERRORS, "\tXML Error parsing Data Map Item %s: %s\n",
				 itemTag.c_str(), xmlErr.GetReason());
		}
		catch( BepException &bepErr)
		{
			Log( LOG_ERRORS, "\tError parsing Data Map Item %s: %s\n",
				 itemTag.c_str(), bepErr.GetReason());
		}
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::ReadDataMap\n");
	return( 0);
}

/**
 * Reads a data item's attributes from the given node
 *
 * @param itemNode Node to read data item properties from
 * @param dataItem Data Item property structure to fill in
 */
void ResManagerXml::ReadDataMapItem( const XmlNode *itemNode, dataMapItem_t &dataItem)
{
	const XmlNodeMap &itemProps = itemNode->getAttributes();
	XmlNode			*propNode;

	dataItem.bitIdx = (UINT16)-1;
	dataItem.boardIdx = 0;
	dataItem.logicLvl = true;
	dataItem.portDir = PORT_DIR_BI;

	// Try to read the channel index for this item
	try
	{
		propNode = (XmlNode*)itemProps.getNode( HwDataMap::channelIdxTag.c_str());
		dataItem.channelIdx = atoi( propNode->getValue().c_str());
	}
	catch( XmlException &xmlErr)
	{
		string	badErr( "No channel defined for ");
		badErr += XmlToAscii( itemNode->getName());
		throw BepException( badErr);
	}

	// Try to read the bit index for this item
	try
	{
		propNode = (XmlNode*)itemProps.getNode( HwDataMap::bitIdxTag.c_str());
		dataItem.bitIdx = atoi( propNode->getValue().c_str());
	}
	catch( XmlException &xmlErr)
	{
	}

	// Try to read the board index for this item
	try
	{
		propNode = (XmlNode*)itemProps.getNode( HwDataMap::boardIdxTag.c_str());
		dataItem.boardIdx = atoi( propNode->getValue().c_str());
	}
	catch( XmlException &xmlErr)
	{
	}

	// Try to read the logic level for this item
	try
	{
		propNode = (XmlNode*)itemProps.getNode( HwDataMap::logicLvlTag.c_str());
		dataItem.logicLvl = (atoi( propNode->getValue().c_str()) == 1);
	}
	catch( XmlException &xmlErr)
	{
	}

	// Try to read the port direction for this item
	try
	{
		propNode = (XmlNode*)itemProps.getNode( HwDataMap::portDirTag.c_str());
		string dirStr = XmlToAscii( propNode->getValue());

		if( dirStr == "IN")			dataItem.portDir = PORT_DIR_IN;
		else if( dirStr == "OUT")	dataItem.portDir = PORT_DIR_OUT;
		else 						dataItem.portDir = PORT_DIR_BI;
	}
	catch( XmlException &xmlErr)
	{
	}
}

/**
 * Fills in our list of publishable tags based on board, channel and bit indices
 */
void ResManagerXml::InitializeIndexedTags()
{
	rawDataMapItr_t	itr;

	m_publishedItems.clear();

	// Evaluate all data table items
	for( itr=m_dataTable.begin(); itr!=m_dataTable.end(); itr++)
	{
		// Put data tag into array at proper index
		int idx = CalculateDataTagIndex(itr->second);
		Log( LOG_DEV_DATA, "Adding %s at index %d\n", itr->first.c_str(), idx);

		m_publishedItems.insert( PublishedTagListVal( idx, itr->first));
	}

	Log( LOG_DEV_DATA, "Publishing %d items\n", m_publishedItems.size());
}

/**
 * Calculates the index of the given tag in our list of published tags.
 * Index is calculated based on board index, channel index, bit index
 * and channels per board
 *
 * @param dataItem Item to calculate index for
 * @return Index for the given data item
 */
int ResManagerXml::CalculateDataTagIndex( const dataMapItem_t *dataItem)
{
	// We have no way of knowing how to calculate this here
	return( 0);
}

/**
 * Calculates the index of the given tag in our list of published tags.
 * Index is calculated based on board index, channel index, bit index
 * and channels per board
 *
 * @param board   Board index for the data item being checked
 * @param channel Channel index for the data item being checked
 * @param bit     Bit index for the data item being checked
 * @return Index for the given data item
 */
int ResManagerXml::CalculateDataTagIndex( int board, int channel, UINT16 bit)
{
	// We have no way of knowing how to calculate this here
	return( 0);
}

/**
 * Checks if the given channel on the given board should be treated as a
 * single value as opposed to a group of individual bits
 *
 * @param boardIdx   Board index of the data item to evaluate
 * @param channelIdx Channel index of the data item being evaluated
 * @return true if the channel should be treated as a singel value
 *         false if it should be treated as 8 individual bit-type data items
 */
bool ResManagerXml::IsWholeChannelItem( int boardIdx, int channelIdx)
{
	// Get tag for board and channel, bit 0
	bool	retVal = false;
	int 	tagIdx = CalculateDataTagIndex( boardIdx, channelIdx, 0);
	const dataMapItem_t* dataItem;

	// If tag published
	if( (dataItem = m_dataTable.find( m_publishedItems[ tagIdx])) != NULL)
	{
		// If bit idx == -1 it is an entire channel data item
		retVal = (dataItem->bitIdx == (UINT16)-1);
	}

	return( retVal);
}

/**
 * Method called when a LOAD_CONFIG command message is received.
 */
void ResManagerXml::ReloadConfig()
{
	int pulsePrio = sched_get_priority_max( sched_getscheduler(0));

	Log( LOG_FN_ENTRY, "Enter ResManagerXml::ReloadConfig\n");

	// Send signal to main I/O loop (to stop it)
	if( MsgSendPulse( m_mssgCoid, pulsePrio, m_ioChnlPulseCode, RELOAD_PULSE) == -1)
	{
		Log( LOG_FN_ENTRY, "MsgSendPulse error: %s", strerror( errno));
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::ReloadConfig\n");
}

/**
 * Method called to process a terminate XML command message
 *
 * @param pid    Process to whom we need to reply
 * @return 0 if successful, -1 if error
 */
int ResManagerXml::HandleTerminate( pid_t pid)
{
	int retVal = 0;
	int pulsePrio = sched_get_priority_max( sched_getscheduler(0));
//	int coId = ConnectAttach( ND_LOCAL_NODE, 0,  GetWaitChid(), 0, _NTO_SIDE_CHANNEL);
	int coId = ResManagerBase::m_pulseSigCoId;

	Log( LOG_FN_ENTRY, "Enter ResManagerXml::HandleTerminate( %d)\n", pid);
	Log( LOG_DEV_DATA, "\tSending TEMINATE_PULSE to coid %d\n", m_mssgCoid);

	if( MsgSendPulse( coId, pulsePrio, m_ioChnlPulseCode, TERMINATE_PULSE) == -1)
	{
		Log( LOG_ERRORS, "\tMsgSendPulse error: %s", strerror( errno));
		retVal = -1;
	}
	ConnectDetach( coId);
	Log( LOG_FN_ENTRY, "Exit ResManagerXml::HandleTerminate( %d)\n", pid);

	return( retVal);
}

///////////////////////////////////////////////////////////////////////////////
// Thread pool handler functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Function called by kernel to allocate a new struct to pass to a new thread
 *
 * @param handle OUr user defined handle: it is the connection ID to listen on
 * @return Pointer to the new structure to use with the new thread
 */
threadPoolParam_t *ResManagerXml::PoolContextAlloc(threadPoolHandle_t *handle)
{
	xmlPoolParam_t	*retVal = NULL;

	Log( LOG_FN_ENTRY, "Enter ResManagerXml::PoolContextAlloc\n");

	retVal = new xmlPoolParam_t;

	if( InitializePoolParam(handle, (threadPoolParam_t*)retVal) == -1)
	{
		Log( LOG_ERRORS, "\tError initializing XML pool param\n");
		delete retVal;
		retVal = NULL;
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::PoolContextAlloc\n");

	return( (threadPoolParam_t*)retVal);
}

/**
 * Initializes the data in the thread pool parameter data type
 *
 * @param handle    Thread pool handle
 * @param poolParam Thread instance data
 * @return 0 if successful, -1 if an error occurrs
 */
int ResManagerXml::InitializePoolParam(threadPoolHandle_t *handle,
									   threadPoolParam_t *poolParam)
{
	int 			retVal=0;
	xmlPoolParam_t	*threadData = (xmlPoolParam_t*)poolParam;
	string			ipcChnlName( GetProcessName());

	Log( LOG_FN_ENTRY, "Enter ResManagerXml::InitializePoolParam()\n");

	retVal = ResManagerMt::InitializePoolParam(handle, &threadData->baseParam);

	memset( &threadData->info, 0, sizeof( threadData->info));

	Log( LOG_DEV_DATA, "Initialize commObj object: %d, %s, %d\n", m_nameId->chid,
		 ipcChnlName.c_str(), IsDebugOn());

	if( (m_nameId != NULL) && ( retVal == 0) )
	{
		threadData->commObj.Initialize( m_nameId->chid, ipcChnlName, IsDebugOn());
		Log( LOG_DEV_DATA, "CommObj object initialized\n");
	}
	else
	{
		Log( LOG_ERRORS, "\tInvalid name_t*\n");
		retVal = -1;
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::InitializePoolParam()\n");

	return( retVal);
}

/**
 * Function called to free memory used for a thread argument struct
 *
 * @param param  Thread context structure to free
 */
void ResManagerXml::PoolContextFree(threadPoolParam_t *ctp)
{
	if( ctp != NULL)
	{
		xmlPoolParam_t	*threadData = (xmlPoolParam_t*)ctp;

		Log( LOG_DEV_DATA, "Freeing context\n");

		FinalizePoolParam( &threadData->baseParam);

		delete threadData;

		Log( LOG_DEV_DATA, "Thread Context freed\n");
	}
}


//
///////////////////////////////////////////////////////////////////////////////

/**
 * Function in which each thread in the thread pool will block
 *
 * @param ctp    Thread context structure
 * @return
 */
threadPoolParam_t* ResManagerXml::PoolBlockFunc(threadPoolParam_t *threadData)
{
	xmlPoolParam_t		*xmlParam = (xmlPoolParam_t*)threadData;
	int					replyId;
	struct sched_param	schedParam;

	// Make sure I am at the deired priority
	SchedGet(0, pthread_self(), &schedParam);
	schedParam.sched_priority = m_poolThreadPriority;
	SchedSet(0, pthread_self(), SCHED_NOCHANGE, &schedParam);

	// Add ourself to the list of active threads
	m_threadList.add( pthread_self());

	if( ResManagerBase::m_resMgrStatus == RES_MGR_CMD_NONE)
	{
		try
		{
			// Wait for data
			replyId = xmlParam->commObj.WaitForRequest();
		}
		catch( BepException &err)
		{
			Log( LOG_ERRORS, "\tm_ipcComm.WaitForRequest() error: %s\n", err.GetReason());
			replyId = -1;
		}
		pthread_testcancel();


		if( replyId >= 0)
		{
			xmlParam->rcvId = replyId;
			switch( xmlParam->rcvId)
			{
			case 0:		// Pulse
				break;
			default:
				Log( LOG_DEV_DATA, "Received data\n");
				break;
			}
		}
		else
		{
			Log( LOG_ERRORS, "\tRead error: %s\n", strerror( errno));
		}
	}
	else
	{
		// just wait here to be killed
		Log( LOG_DEV_DATA, "Thread exiting\n");
		m_threadList.remove( pthread_self(), false);
		pthread_exit( NULL);
	}

	return( threadData);
}


/**
 * Function called when our thread returns from the block function
 *
 * @param ctp    Thread context structure
 */
void ResManagerXml::PoolUnBlockFunc(threadPoolParam_t *ctp)
{
	ThreadListCItr_t	itr;
	int 				threadId;

	Log( LOG_FN_ENTRY, "Enter ResManagerXml::PoolUnBlockFunc()\n");

	if( (itr = m_threadList.begin()) != m_threadList.end())
	{
		threadId = itr->first;
		Log( LOG_DEV_DATA, "\tRemoving thread %d\n", threadId);
		m_threadList.remove( threadId, true);
	}
	else
	{
		Log( LOG_DEV_DATA, "\tThread list is empty\n");
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::PoolUnBlockFunc()\n");
}

/**
 * Function called after our thread has left the blocked state and is ready
 * to do some work
 *
 * @param ctp    Thread context structure for this thread to work with
 * @return 0
 */
int ResManagerXml::PoolHandlerFunc( threadPoolParam_t *threadData)
{
	xmlPoolParam_t		*xmlParam = (xmlPoolParam_t*)threadData;
	IBepCommunication	handlerObj;

	if( ResManagerBase::m_resMgrStatus == RES_MGR_CMD_NONE)
	{
		Log( LOG_DEV_DATA, "Initialize handler object\n");
		handlerObj.Initialize( xmlParam->commObj, xmlParam->rcvId);
		Log( LOG_DEV_DATA, "Handler object initialized\n");
		xmlParam->mssgString.clear();
		handlerObj.HandleRequest( xmlParam->mssgString, this);
		Log( LOG_DEV_DATA, "Request Handled:\n'%s'\n",
			 xmlParam->mssgString.c_str());

		// Check if we should exit
		pthread_testcancel();
	}
	else
	{
		// just wait here to be killed
		Log( LOG_DEV_DATA, "Waiting to be cancelled...\n");
		delay( 200);
	}

	// Remove ourselved from the active thread list
	m_threadList.remove( pthread_self(), false);

	return( 0);
}

/**
 * Kills any XML threads that are hanging around
 */
void ResManagerXml::KillPoolThreads()
{
	Log( LOG_FN_ENTRY, "Enter ResManagerXml::KillPoolThreads( %d), threadId=%d\n",
		 m_threadList.size(), pthread_self());

	// If not already destroyed
	if( m_threadPool != NULL)
	{
	/**********************************************************************
		Prior to QNX 6.1, we had to manually kill all the threads in our
		thread pool. Now we simply need to call thread_pool_destroy
		and let the kernel take care of killing the threads in our
		thread pool :)
	**********************************************************************/

		ResManagerBase::m_resMgrStatus = RES_MGR_CMD_TERMINATE;

#define MANUALLYKILLTHREADS 0

#if MANUALLYKILLTHREADS

		coId = name_open( GetProcessName().c_str(), 0);
		// Cancel all threads first
		for( itr=m_threadList.begin(); itr!=m_threadList.end(); itr++)
		{
			threadId = itr->first;
			// If not current thread and not main thread of execution
			if( (threadId != pthread_self()) && (threadId > 1))
			{
				Log( LOG_DEV_DATA, "Cancelling thread %d on connection %d\n", threadId, coId);

				// Cancel the thread after it's unblocked
				pthread_cancel( threadId);
				// Send a dummy pulse in order to unblock the thread
				if( coId != -1)	MsgSendPulse( coId, _PULSE_CODE_THREADDEATH, threadId, 0);
				// Invalid connection so try to outright kill the thread
				else			pthread_kill( threadId, SIGURG);
			}
		}
		// Join threads after cancelling them
		for( itr=m_threadList.begin(); itr!=m_threadList.end(); itr++)
		{
			threadId = itr->first;
			// If not current thread and not main thread of execution
			if( (threadId != pthread_self()) && (threadId > 1))
			{
				Log( LOG_DEV_DATA, "Joining thread %d\n", threadId);

				// Join the thread now that it's been cancelled
				pthread_join( threadId, NULL);
			}
		}
		name_close( coId);

		m_threadList.clear();
#endif

		// Destroy the thread pool
		Log( LOG_DEV_DATA, "Destroying thread pool\n");
		thread_pool_destroy( m_threadPool);
		m_threadPool = NULL;
		Log( LOG_DEV_DATA, "Thread pool destroyed\n");

	}

	Log( LOG_FN_ENTRY, "Exit ResManagerXml::KillPoolThreads( %d)\n", m_threadList.size());
}

/**
 * Fills in an XmlNode with the data value for a data item
 *
 * @param dataItem Properties for the data item to read
 * @param value    String to fill in with the value of the requested data item
 * @return 0 if valueNode is filled in
 *         -1 if unable to handle the request
 */
int ResManagerXml::ReadDataItem( const dataMapItem_t *dataItem, string &value)
{
	Log( LOG_FN_ENTRY, "Enter ResManagerXml::ReadDataItem()\n");
	Log( LOG_FN_ENTRY, "Exit ResManagerXml::ReadDataItem()\n");
	return( -1);
}

/**
 * Reads a data item's value from an XmlNode
 *
 * @param dataItem Properties for the data item to write to
 * @param value    String containing the value of the specified data item
 * @return 0 if new value was written
 *         -1 if unable to handle the request
 */
int ResManagerXml::WriteDataItem( const dataMapItem_t *dataItem, const string &value)
{
	Log( LOG_FN_ENTRY, "Enter ResManagerXml::WriteDataItem()\n");
	Log( LOG_FN_ENTRY, "Exit ResManagerXml::WriteDataItem()\n");
	return( -1);
}

