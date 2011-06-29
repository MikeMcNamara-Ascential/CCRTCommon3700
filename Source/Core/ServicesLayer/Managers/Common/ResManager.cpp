//******************************************************************************
// Description:
// 	Base class for resource managers (I/O managers, Sys Message managers,
// 	etc). Contains the kernel interface functions.
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Common/ResManager.cpp $
// 
// 6     4/13/06 3:48a Cward
// Re-order the BepException and XmlException catch blocks
//
// 2     2/15/06 4:00p Bmeinke
// Re-order the catch(BepException) and catch(XmlException) blocks
//
// 5     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 2     9/20/05 10:11a Bmeinke
// Need to call RegisterWithTaskMon from Initialize()
//
// 1     9/16/05 10:51a Bmeinke
// Updating to Tata Core
//
// 4     7/20/05 9:04p Cward
// Added support for new TaskMon
//
// 3     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     6/24/04 1:03a Cward
// Removed pound if 0 blocks. Updated the header block comments to the new
// standard.
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
// 3     6/26/03 1:44p Bmeinke
// Fixed bug where no error was detected if we tried to register a name
// with the OS and that name was already registered
// Changed the BposRegisterTaskName() call to BposAttachName()
//
// 2     6/20/03 9:04a Khykin
// Updates due to Ford changes.
//
// 1     6/19/03 2:01p Bmeinke
//
// 1     4/30/03 4:46p Bmeinke
// Migration to new personalized configuration management structure
//
// 61    3/12/03 2:09p Bmeinke
// Added better handling og unknown command line parameters in
// ParseCommandLine()
//
// 60    1/03/03 4:25p Bmeinke
// Added SignalHandler() function
// Removed reading of XML Verbose node located directly under the Setup
// node (use the Verbose node inside the logger setup node)
//
// 59    12/13/02 5:45p Bmeinke
// Explicitly attach the _PULSE_CODE_UNBLOCK pulse code to our I/O channel
// so we know when a client wants to unblock from a blocking operation
//
// 58    10/15/02 12:00p Bmeinke
// Removed printout of thread ID in thread pool logger statements
//
// 57    10/15/02 11:00a Bmeinke
// Added IFaultServer object (m_faultServer) for diagnostic/fault
// reporting
// Added Register() method so m_faultServer can attach to the fault server
// properly
// Changed return value of CalculateDataTagIndex() to uint64_t
//
// 56    10/11/02 2:56p Bmeinke
// Inside Execute(), if InitManager() failed, return value was not being
// set to RES_MGR_CMD_TERMINATE
//
// 55    10/10/02 12:18p Bmeinke
// Removed redundant data member m_cfgFilename
//
// 54    10/08/02 6:49p Bmeinke
// Added an optional priority argument to ConnectTimerPulse
//
// 53    10/08/02 6:20p Bmeinke
// Updated javadoc comments for InitHandlers()
//
// 52    8/05/02 1:14p Dan.erickson
// Corrected Notify calls.
//
// 51    8/01/02 11:54a Bmeinke
// Removed compiler warnings
//
// 50    6/27/02 7:26p Bmeinke
// Changed publish logic: all published data items are stored in 1 of 2
// multimap containers: Bit-based data items are stored seperate from the
// whole-channel items; maps sorted based on a key calculated from the
// data item's board, channel and bit index. This allows any number of
// tags to point at the exact same data.
//
// 49    6/13/02 3:11p Dan.erickson
// Clear message before calling HandleRequest().
//
// 48    5/03/02 5:18p Kh
// Updating to check for a valid notification list prior to sending a
// notification.
//
// 47    2/27/02 2:52p Bmeinke
// Wrap call to BepComponent::Initialize() with try/catch
// Added support for command line option 'c' to echo Log() statements to
// console
// Changed order of arguments to IBepCommunication::Initialize()
//
// 46    2/19/02 8:02p Bmeinke
// PublishData() method no longer launches a thread (NotificationList
// already now uses a thread pool internally)
//
// 45    2/07/02 10:20p Bmeinke
// m_nList data member of BepComponent changed to a pointer data type
//
// 44    11/07/01 6:34p Brian.meinke
// Changed arguments to IBepCommunication::Initialize because the
// prototype changed
//
// 43    11/06/01 12:31a Brian.meinke
// Changed ReadDataItem/WriteDataItem to take string references instead of
// XmlNode pointers
//
// 42    11/05/01 11:50a Brian.meinke
// Added return( NULL) to PublishThread() method
// Added Log statements to ReadDataItem/WriteDataItem
//
// 41    10/18/01 4:57p Brian.meinke
// Removed debug related delay() calls
//
// 40    10/16/01 1:23a Brian.meinke
// Added function entry/exit Log() calls
// Clear m_dataTable inside CleanupManager()
// Don't print optarg if it is NULL in ParseCommandLine()
// #if 0'd printing of data table inside ReadDataTable() (too time
// consuming)
// Removed #if 0'd sections
//
// 39    10/11/01 11:42a Brian.meinke
// Detach pulses/mesages from the dispatch interface inside
// CleanupManager()
// LoadConfig() now uses the ReturnXMLDocument() method of the XML parser
// Call m_dataTable.clear() when entering ReadDataMap()
// Terminating and Reloading config handled by sending a pulse from our
// XML thread pool to our I/O channel instead of a signal( SIGURG)
// LaunchXmlThread() no longer launches a seperate thread to start the
// thread pool; not necessary if the flags arg to thread_pool_create is 0
//
// 38    10/09/01 4:08p Brian.meinke
// Don't BposUnregisterTaskName() in CleanupManager() if already
// unregistered
//
// 37    10/09/01 3:06p Brian.meinke
// Fixed error in ConnectTimerPulse(): not setting pulse value to the
// value passed in
//
// 36    10/09/01 1:34p Brian.meinke
// Added ability to attach pulses to the resource manager's I/O message
// channel via the PulseHandler() static method
// Changed IInterprocessCommunication to an IBepCommunication object
// Each thread in our thread pool gets its own IBepCommunication object to
// handle XML messages
// ConnectTimerPulse() method now accepts a pulse code and a pulse value
// threadPoolParam_t data type is now a class instead of a structure
// XmlHandlerFunc() now simply calls HandleRequest() of the
// IBepCommunication object
// KillXmlThreads() now makes a single pass when
// cancelling/killing/joining thread pool threads
// Removed the XML message handling methods: XML messages now handled
// through the IBepCommunication object
//
// 35    10/04/01 10:22a Brian.meinke
// Integrated the IInterProcessCommunication object for IPC messaging
//
// 34    10/02/01 10:25a Brian.meinke
// Really fixed the pulse handling in XmlHandlerFunc()
//
// 33    10/02/01 10:21a Brian.meinke
// Fixed pulse handling in XmlHandlerFunc()
//
// 32    9/14/01 2:18p Brian.meinke
// Added PublishThreadArg_t data type; used to pass subscribtion data to
// publisher thread
// Added PublishData() methods to spin threads to perform subscription
// update notification messages
// Added PublishThread() methods; Thread entry point to perform
// subscription update notification messages
// Added IsWholeChannelItem(); determines if a data item occupies an
// entire I/O channel as opposed to a single bit of a channel
//
// 31    9/14/01 11:48a Brian.meinke
// Ooops...forgot to add retVal variable to HandleUnsubscriptionRequest()
// method
//
// 30    9/14/01 11:30a Brian.meinke
// Added mehtod InitializeIndexedTags() to fill in the list of our
// published tags based on data item board, channel and bit indices
// Added mehtod CalculateDataTagIndex() to calculate the index of a data
// item in our published tag list based on data item board, channel and
// bit indices
// Added data memebr m_publishedItems to hold a list of our published tags
// board, channel and bit indices
// HandleCommandMessages was not replying to senders if the received
// command was not recognized
// Change BposREply() calls to MssgReply() for error conditions (i.e.
// unrecognized tags) in order to send EINVAL to sending process
// Implemented proper processing of unsubscribe messages
//
// 29    9/14/01 2:00a Brian.meinke
// Cleaned up verbosity masks in various Log() calls
// Changed Check***Messages() method names to Handle***Messages()
// Added support for Subscribe and Unsubscribe XML messages
//
// 28    9/04/01 9:51a Brian.meinke
// Changed OcbFree/OcbCalloc to ocbFree/ocbCalloc
// OcbFree/OcbCalloc are now virtual functions to allow each sub-class to
// more easily override this functionality
//
// 27    8/31/01 6:20p Brian.meinke
// Added static ResManager* data member m_ioMgrForOcb used to initialize
// the ResManager* inside the OCB.
// Added common OcbCalloc/OcbFree static methods
// InitManager calls InitHandlers with the new common OcbCalloc and
// OcbFree methods
//
// 26    8/31/01 3:28p Brian.meinke
// Added new parameter 'init' and moved 'size' parameter in PathAttach()
//
// 25    8/30/01 3:37p Brian.meinke
// Explicitly call BepComponent constructor from ResManager constructor
// If BposRegisterTaskName fails, set m_nameId to NULL
// XmlBlockFunc no longer checks m_xmlCommand before calling MsgReceive
//
// 24    8/28/01 11:09p Brian.meinke
// Explicitly set thread poll thread attributes to joinable
// Add call to KillXmlThreads to CleanupManager()
// Modified Execute() method to call InitManager and loop until we get a
// terminate pulse
// Added a size parameter to PathAttach() method
// PathDetach() now passes_RESMGR_DETACH_All to resmgr_detach()
// KillXmlThreads now performs 2 loops; first loop to cancel all thread
// pool threads, second loop sends each thread a signal to release them
// from the Reseive and then joins each thread
// Added static xmlBlockFunc() method which calls the virtual interface
// method XmlBlockFunc()
// Added virtual interface method XmlBlockFunc()
//
//
// 23    8/17/01 2:24p Brian.meinke
// Install all (almost) I/O handlers (static class methods)
// Added interface (virtual functions) for I/O function callouts (called
// by static methods)
// Added m_timerConnectId to hold timer connection ID (for use with
// scanner timers)
// Added virtual XmlBlock/Handler methods (called by static class XML
// handler methods)
// Added virtual methods to handle Subscription requests
// Added "ResManager::" to logger text
//
// 22    8/08/01 4:29p Brian.meinke
// Set m_xmlThreadPool = NULL in KillXmlThreads() after calling
// thread_pool_destroy
// Do not do anything in KillXmlThreads() if m_xmlThreadPool == NULL
//
// 21    7/26/01 7:41p Brian.meinke
// Changed spelling of Set/GetVerboseMask
// Added "ResManager::" to Log() strings
//
// 20    6/22/01 4:24p Brian.meinke
// Fixed bug in CheckCommandMessages() on conditional test in the for loop
// (no comparison on loop variable was being made)
// Replaced hard coded XML tag strings with #define values from BepDefs.h
//
// 19    6/22/01 1:11p Brian.meinke
// Removed references to the "printf" mutex (intended for the Log()
// methods)
// Added javadoc comment to the ReloadConfig() method
//
// 18    6/20/01 4:07p Brian.meinke
// Removed m_verbose data member (uses new verbosity control of ILogger)
// Removed Log() methods (uses new ILogger method)
// Removed SetTaskName()/GetTaskName() (uses Get/SetProcessName() of
// ILogger)
//
// 17    6/11/01 9:05a Brian.meinke
// ResManager now inherits from BepComponent
// Added Initialize() method (override of base class method)
// Moved XML Config file parsing to new Initialize() method
// Changed hard coded XML Tag strings to defines in header files
// Removed references to m_logger and replaced with *this
//
// 16    5/18/01 11:28a Brian.meinke
// Fixed bug in ReadDataMapItem when reading the Port Direction Attribute
// from an XML node (was using the wrong tag)
//
// 15    5/16/01 10:11p Brian.meinke
// Cleanup commented out lines inside Log() methods
//
// 14    5/16/01 4:29p Brian.meinke
// Removed GeneralUtilities object (methods weer made global functions)
//
// 13    5/16/01 3:52p Brian.meinke
// XML Node being passed to CheckWriteMessages was the root node, changed
// it to the dataNode
//
// Modified CheckReadMessages
//   - Added Log() for entry/exit
//   - Reply with formatted XML message
//
// 12    5/10/01 10:15a Brian.meinke
// Added pthread_barrier_t to synchronize XML thread startup
// Moved call to LoadConfig() to the InitManager() method so config files
// are re-loaded during on a LOAD_COONFIG command
// Unregister task name in CleanupManager()
// Moved XmlThreadInit() implementation from h file to cpp file
// Lower priority of calling thread by 2 points in KillXmlThread
//
// 11    5/07/01 5:49p Brian.meinke
// Added CleanupManager method to facilitate re-loading configuration data
// Minor touch-ups for syntax warnings
//
// 10    5/07/01 1:57p Brian.meinke
// Reply with empty message to unknown command messages (temporary)
//
// 9     4/30/01 2:12p Brian.meinke
// Changes for new ILogger methods
//
// 8     4/30/01 10:53a Brian.meinke
// Added ParseCommandLine Method
// Changed processing of TERMINATE command in CheckCommandMessages
//
// 7     4/25/01 5:03p Brian.meinke
// Added default Execute method
// Changed XML Setup node tags for BEP standard naming conventions
// Changed semaphore used inside Log method to a pthread_mutex
//
// 6     4/24/01 3:50p Brian.meinke
// Added verbose mask of LOG_DEV_DATA for printing data map items in
// ReadDataMap
//
// 5     4/24/01 1:52p Brian.meinke
// Added common XML Thread Pool framework
// Added call to KillXmlThreads in destructor to terminate all XML thread
// pool threads
// Made AbortHandler a global extern C function
//
// 4     4/23/01 2:58p Brian.meinke
// Added implementation for ReadDataMap method
// Added method ReadDataMapItem to parse an XML node into a dataMapItem_t
// object
//
// 3     4/20/01 6:19p Brian.meinke
// Added ReadDataMap method
//
// 2     4/19/01 3:57p Brian.meinke
// Changed printf semaphore back to native POSIX semaphore (from Bpos
// semaphore)
//
// 1     4/19/01 2:08p Brian.meinke
//
// 5     4/17/01 4:42p Brian.meinke
// Added ReadXmlOptions method to read common options from XML file
//
// 4     4/17/01 10:45a Brian.meinke
// changed log message in PathDetach from PAthAttach to PathDetach
//
// 3     4/12/01 3:16p Brian.meinke
// Log message inside GetTaskName was 'Enter SetTaskName';changed to
// GetTaskName
//
// 2     4/10/01 2:35p Brian.meinke
// Restructured manager classes to use BepException for error conditions
//
// 1     4/10/01 10:46a Admin
//
// 6     3/19/01 8:44a Brian.meinke
// Added BposINit to copnstructor
// Use BPOS Semaphores
// Use standard BposSend/BposREceive instead of Sendmx/Receivemx
// Added methods SetTaskName/GetTaskName
//
// 5     3/07/01 5:15p Brian.meinke
// Changed all method names to start with capital letters to comply with
// BEP standard
//
// 4     3/06/01 7:30p Brian.meinke
// Moved local console control to the Logger Interface
//
// 3     2/19/01 11:25p Brian.meinke
// Changed constructor to initialize m_schoCon to false
//
// 2     2/16/01 4:01p Brian.meinke
// Added Logger support
//
// 1     2/15/01 12:36p Brian.meinke
// Base class for resource managers (I/O managers, Sys Message managers,
// etc). Contains the kernel interface functions.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "ResManager.h"
#include <sys/netmgr.h>

ResManager* ResManager::m_ioMgrForOcb;
/*******************************************************************************
	AbortHandler
	handles the system ABORT signal
*******************************************************************************/
void AbortHandler( int SigNo)
{
}


/**
 * Function invoked inresponse to a signal to our process
 *
 * @param SigNo  Siagnal we caught
 */
void ResManager::SignalHandler( int SigNo)
{
	ResManager *ioMgr = ResManager::m_ioMgrForOcb;

	ioMgr->Log( LOG_FN_ENTRY, "Enter ResManagerBase::SignalHandler: %d\n",
				SigNo);
	// Make pulse very high priority
	int prio = sched_get_priority_max( sched_getscheduler(0));
	switch( SigNo)
	{
	case SIGINT:
	case SIGQUIT:
		ioMgr->m_xmlCommand = RES_MGR_CMD_TERMINATE;
		ioMgr->Log( LOG_DEV_DATA, "Handling SIGQUIT: %d\n",
					ioMgr->m_mssgCoid);
		MsgSendPulse( ioMgr->m_mssgCoid, prio, SYSTEM_PULSE,
					  TERMINATE_PULSE);
		break;
	case SIGWINCH:
		ioMgr->m_xmlCommand = RES_MGR_CMD_CONFIG;
		ioMgr->Log( LOG_DEV_DATA, "Handling SIGWINCH: %d\n",
					ioMgr->m_mssgCoid);
		MsgSendPulse( ioMgr->m_mssgCoid, prio, SYSTEM_PULSE,
					  RELOAD_PULSE);
		break;
	default:
		break;

	}
	ioMgr->Log( LOG_FN_ENTRY, "Exit ResManagerBase::SignalHandler: %d\n",
				SigNo);
}

//******************************************************************************

ResManager::ResManager( const char *name) : BepComponent(),
	m_ioChnlPulseCode( -1), m_mssgCoid( -1), m_demoMode( false), m_nameId( NULL),
	m_scanTimerId( -1), m_timerConnectId( -1), m_dispatchPtr( NULL),
	m_ctp( NULL), m_resMgrId( -1), m_xmlThreadPool( NULL),
	m_xmlCommand( RES_MGR_CMD_NONE), m_faultServer( NULL), m_haveSubscriptions( false),
	m_taskName( "ResManager")
{
	Log( LOG_FN_ENTRY, "Enter ResManager::ResManager( %s)\n", name);

	// Initialize barrier for XML thread startup
	pthread_barrier_init( &m_xmlThreadBarrier, NULL, 2);

	SetProcessName( name);

	ResManager::m_ioMgrForOcb = this;

	Log( LOG_FN_ENTRY, "Exit ResManager::ResManager( %s)\n", name);
}

ResManager::~ResManager()
{
	Log( LOG_FN_ENTRY, "Enter ResManager::~ResManager\n");

	// Clean up threads
	KillXmlThreads();

	Log( LOG_FN_ENTRY, "Exit ResManager::~ResManager\n");
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool ResManager::InitManager()
{
	bool    			retVal;

	Log( LOG_FN_ENTRY, "Enter ResManager::InitManager\n");

	// Load config file if specified
	if( m_configFile.empty() == false)
	{
		try
		{
			// Read setup options from the XML flle
			LoadConfig( m_configFile);
		}
		catch( XmlException &err)
		{
			Log(LOG_ERRORS, "Error Loading Config File: %s\n", err.GetReason());
			retVal = false;
		}
	}

	// Initialize the kernel fd interface
	retVal = InitKernelInterface();

	// Attach the "client wants to unblock' pulse to our I/O channel
	pulse_attach( m_dispatchPtr, 0, _PULSE_CODE_UNBLOCK, ResManager::PulseHandler, this);

	// Set channel ID to invlid value
	m_xmlPoolHandle.channelId = -1;

	m_scanTimerId = -1;
	m_timerConnectId = -1;

	// Install our I/O handlers
	InitHandlers( (void*)ResManager::ocbCalloc, (void*)ResManager::ocbFree);

	Log( LOG_FN_ENTRY, "Exit ResManager::InitManager\n");

	return( retVal);
}

/**
 * Routine invoked by the resource manager library in response to an
 * attached pulse.
 *
 * @param ctp    Dispatch context pointer (from dispatch_context_alloc)
 * @param code   Code for the pulse we received
 * @param flags  ?????
 * @param handle Pointer to a ResManager object
 * @return 0 if successful; -1 if an error occurred
 */
int ResManager::PulseHandler( message_context_t *ctp, int code, unsigned flags,
							  void *handle)
{
	io_pulse_t	&pulse = ctp->msg->pulse;
	ResManager	*ioMgr = (ResManager*)handle;

	return( ioMgr->HandleResMgrPulse(pulse));
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void ResManager::CleanupManager()
{
	Log( LOG_FN_ENTRY, "Enter ResManager::CleanupManager\n");

	// Shutdown the thread pool
	KillXmlThreads();

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

	// Detach the terminate pulse from our resource manager's channel
	if( m_ioChnlPulseCode != -1)
	{
		Log( LOG_DEV_DATA, "Detach pulse\n");
		pulse_detach(m_dispatchPtr, m_ioChnlPulseCode, 0);
		Log( LOG_DEV_DATA, "Pulse detached\n");
		m_ioChnlPulseCode = -1;
	}

	// Release our context pointer
	if( m_ctp != NULL)
	{
		Log( LOG_DEV_DATA, "Destroying resource manager context\n");
		resmgr_context_free( m_ctp);
		m_ctp = NULL;
	}

	// Release our dispatch interface pointer
	if( m_dispatchPtr != NULL)
	{
		Log( LOG_DEV_DATA, "Destroying dispatch interface\n");
		dispatch_destroy( m_dispatchPtr);
		m_dispatchPtr = NULL;
	}

	// Destroy fault server interface object
	if( m_faultServer != NULL)
	{
		m_faultServer->Close();
		delete m_faultServer;
		m_faultServer = NULL;
	}

	// Clear data map
	m_dataTable.clear();

	Log( LOG_FN_ENTRY, "Exit ResManager::CleanupManager\n");
}

/**
 * Main message pump for the resource manager.
 *
 * @return 0 if no errors
 *         -1 on error (use errno for further error information)
 */
int ResManager::Execute()
{
	Log( LOG_FN_ENTRY, "Enter ResManager::Execute\n");

	try
	{
		// Loop until told to terminate
		while( m_xmlCommand == RES_MGR_CMD_NONE)
		{
			// Normal excecution (reset from possible re-config)
			m_xmlCommand = RES_MGR_CMD_NONE;

			// Make sure we're initialized
			if( InitManager() == true)
			{
				// Launch the XML Thread
				try
				{
					LaunchXmlThread();
				}
				catch( BepException &err)
				{
					Log( LOG_ERRORS, "tError starting XML thread pool: %s\n", err.GetReason());
					m_xmlCommand = RES_MGR_CMD_TERMINATE;
					continue;
				}

				// Wait for the XML thread to start
				//pthread_barrier_wait( &m_xmlThreadBarrier);

				// Handle I/O messages (file descriptor based)
				IoMssgLoop();

				// Clean up after ourselves (close channels etc)
				CleanupManager();
			}
			else
			{
				// No universe manager...so leave
				m_xmlCommand = RES_MGR_CMD_TERMINATE;
				break;
			}
		}
	}
	catch( XmlException &xmlErr)
	{
		Log( "Caught unhandled XmlException: %s\n", xmlErr.GetReason());
		m_xmlCommand = RES_MGR_CMD_TERMINATE;
	}
	catch( BepException &bepErr)
	{
		Log( "Caught unhandled BepException: %s\n", bepErr.GetReason());
		m_xmlCommand = RES_MGR_CMD_TERMINATE;
	}
	catch( ...)
	{
		Log( "Caught unhandled exception\n");
		m_xmlCommand = RES_MGR_CMD_TERMINATE;
	}

	Log( LOG_FN_ENTRY, "Exit ResManager::Execute\n");
	return( m_xmlCommand);
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string ResManager::Register(void)
{
	std::string faultServerName( FAULT_SERVER_NAME);

	Log( LOG_FN_ENTRY, "Enter ResManager::Register()\n");

	// Close existing interface
	if( m_faultServer != NULL)	m_faultServer->Close();

	try
	{
		// Create new interface if needed
		if( m_faultServer == NULL)	m_faultServer = new IFaultServer;

		// Initialize interface
		m_faultServer->Initialize( faultServerName);

		// Call base class method to register with TaskMon
		faultServerName = BepComponent::Register();
	}
	catch( BepException &err)
	{
		Log( LOG_FN_ENTRY, "\tError initializing fault server interface: %s\n", err.GetReason());
		faultServerName = BEP_FAILURE_RESPONSE;
	}

	Log( LOG_FN_ENTRY, "Exit ResManager::Register( %s)\n", faultServerName.c_str());

	return( faultServerName);
}

/**
 * method for component initialization.
 *
 * @param document Reference to a parsed configuration document.
 */
void ResManager::Initialize(const XmlNode *document)
{
	XmlString	setupTag( CFG_SETUP_TAG);
	XmlString	dataTag( CFG_DATAMAP_TAG);

	Log( LOG_FN_ENTRY, "Enter ResManager::Initialize\n");

	try
	{
		m_haveSubscriptions = false;
		try
		{
			// Perform base class initialization
			BepComponent::Initialize( document);
		}
		catch( BepException &err)
		{
			Log( LOG_ERRORS, "Error in BepComponent::Initialize: %s\n",
				 err.GetReason());
		}

		try
		{
			// Read general options from config file
			ReadXmlOptions( (const XmlElement*)document->getChild( setupTag));

			// Read data map items from config file
			ReadDataMap( (const XmlElement*)document->getChild( dataTag));
		}
		catch( BepException &err)
		{
			Log( LOG_ERRORS, "Error reading options in Initialize: %s\n",
				 err.GetReason());
		}
	}
	catch( XmlException &xmlErr)
	{
		Log( LOG_ERRORS, "Parse error in BepComponent::Initialize: %s\n",
			 xmlErr.GetReason());
	}
	catch( BepException &bepErr)
	{
		Log( LOG_ERRORS, "Error in BepComponent::Initialize: %s\n",
			 bepErr.GetReason());
	}

	RegisterWithTaskMon();

	Log( LOG_FN_ENTRY, "Exit ResManager::Initialize\n");
}

/**
 * Initializes the interface to the OS kernel
 *
 * @return true on success
 *         false on error
 */
bool ResManager::InitKernelInterface()
{
	Log( LOG_FN_ENTRY, "Enter ResManager::InitKernelInterface\n");
	m_dispatchPtr = dispatch_create();

	if( m_dispatchPtr == NULL)
	{
		throw( BepException( "Error creating dispatch interface"));
	}

	Log( LOG_FN_ENTRY, "Exit ResManager::InitKernelInterface\n");
	return( true);
}

/**
 * Initializes the I/O message handler functions to the default OS values.
 *
 * @param ocbAllocFn Function to be used to allocate OCBs
 * @param ocbFreeFn  Function to be used to free OCBs
 */
void ResManager::InitHandlers( void* ocbAllocFn, void* ocbFreeFn)
{
	Log( LOG_FN_ENTRY, "Enter ResManager::InitHandlers\n");

	/**
	 * Structure to allow us to patch in our own ocb allocating & freeing functions
	 */
	m_ocbFuncs.nfuncs = _IOFUNC_NFUNCS;
	m_ocbFuncs.ocb_calloc = (ocb_calloc_t)ocbAllocFn;
	m_ocbFuncs.ocb_free = (ocb_free_t)ocbFreeFn;

	/**
	 * The mount structure used to patch in our OCB alloc/free functions
	 */
	memset( &m_mountPoint, 0, sizeof(m_mountPoint));
	m_mountPoint.funcs = &m_ocbFuncs;

	// initialize resource manager attributes
	memset(&m_resmgrAttr, 0, sizeof(m_resmgrAttr));
	m_resmgrAttr.nparts_max = 1;
	m_resmgrAttr.msg_max_size = 2048;

	// initialize functions for handling messages (use OS defaults)
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &m_connectFuncs, _RESMGR_IO_NFUNCS,
					 &m_ioFuncs);

	// Patch in our handler routines (these statics will call the virtual
	//	methods of the ResManager interface)
	m_connectFuncs.open = ResManager::ioOpen;
	m_ioFuncs.read = ResManager::ioRead;
	m_ioFuncs.write = ResManager::ioWrite;
	m_ioFuncs.close_ocb = ResManager::ioCloseOcb;
	m_ioFuncs.stat = ResManager::ioStat;
	m_ioFuncs.notify = ResManager::ioNotify;
	m_ioFuncs.devctl = ResManager::ioDevCtl;
	m_ioFuncs.unblock = ResManager::ioUnblock;
	m_ioFuncs.pathconf = ResManager::ioPathconf;
	m_ioFuncs.lseek = ResManager::ioLseek;
	m_ioFuncs.chmod = ResManager::ioChmod;
	m_ioFuncs.chown = ResManager::ioChown;
	m_ioFuncs.utime = ResManager::ioUtime;
	m_ioFuncs.openfd = ResManager::ioOpenFd;
	m_ioFuncs.space = ResManager::ioSpace;
	m_ioFuncs.mmap = ResManager::ioMmap;
	m_ioFuncs.msg = ResManager::ioMsg;
	m_ioFuncs.sync = ResManager::ioSync;

	Log( LOG_FN_ENTRY, "Exit ResManager::InitHandlers\n");
}

/**
 * Registers the calling process as a resource manager
 *
 * @return 0 if successful
 *         -1 if error; use errno to obtain more error information
 */
int ResManager::RegisterManager()
{
	string	strErr;

	Log( LOG_FN_ENTRY, "Enter ResManager::RegisterManager\n");

	m_ctp = resmgr_context_alloc( m_dispatchPtr);
	if( m_ctp == NULL)
	{
		strErr = "resmgr_context_alloc failed: " + string( strerror( errno));
		throw( BepException( strErr));
	}

	// Attach the terminate pulse to our resource manager's channel
	m_ioChnlPulseCode = pulse_attach( m_dispatchPtr, 0, SYSTEM_PULSE,
									  &ResManager::PulseHandler, this);
	if( m_ioChnlPulseCode == -1)
	{
		strErr = "pulse_attach failed: " + string( strerror( errno));
		Log( strErr.c_str());
		throw( BepException( strErr));
	}

	// Create a connection on which the pulse can be received
	m_mssgCoid = message_connect( m_dispatchPtr, MSG_FLAG_SIDE_CHANNEL);
	if( m_mssgCoid == -1)
	{
		strErr = "message_connect failed: " + string( strerror( errno));
		Log( strErr.c_str());
		throw( BepException( strErr));
	}

	Log( LOG_FN_ENTRY, "Exit ResManager::RegisterManager\n");
	return( 0);
}

/**
 * Attaches the calling process as the IO manager of the given 'pathToAdopt'
 * path. The OS will send messages to our process when IO occurs within the
 * 'pathToAdopt' path. The OS will include a unit number in IO messages
 * pertaining to our path. This unit number is identical to the device number
 * returned by this function. The OS does not include the path in its IO
 * messages, so a process must keep track of device number/path relationships.
 *
 * @param pathToAdopt
 *               Path namespace to take over
 * @param size   Size of the resource (in bytes)
 * @param mode   File 'type' our path will appear as (i.e. Regular File, Directory, etc)
 * @param flags  Flags to pass to the kernel (0=default)
 * @param ioAttr Attribute structure to use for this path (default=NULL).
 *               If NULL, a default attribute structure will be used
 * @param init   Flag to indicate if we need to initialize the attributes
 *               structure (normally false)
 * @return the device number associated with the path.
 */
int ResManager::PathAttach( const char *pathToAdopt, int size, int mode, int flags,
							IOFUNC_ATTR_T *ioAttr, bool init)
{
	string	path = MakeDevPath( pathToAdopt);

	Log( LOG_FN_ENTRY, "Enter ResManager::PathAttach( %s)\n", pathToAdopt);

	// If no func attribs specified, initialize and use common attribs
	if( ioAttr == NULL)
	{
		ioAttr = &m_ioFuncAttr;
	}

	// initialize attribute structure
	if( init == true)
	{
		iofunc_attr_init(ioAttr, mode, 0, 0);
	}
	ioAttr->nbytes = size;

	// Patch in our "mount" handler (for allocating/freeing client OCBs)
	ioAttr->mount = &m_mountPoint;

	// attach our device name
	m_resMgrId = resmgr_attach(m_dispatchPtr,
							   &m_resmgrAttr,
							   path.c_str(),
							   _FTYPE_ANY,
							   flags,
							   &m_connectFuncs,
							   &m_ioFuncs,
							   ioAttr);
	if(m_resMgrId == -1)
	{
		string	strErr("resmgr_attach '");
		strErr += path + string("' failed: ") + string( strerror( errno));
		throw( BepException( strErr));
	}
	else
	{
		Log( LOG_DEV_DATA, "path %s attached\n", path.c_str());
	}
	Log( LOG_FN_ENTRY, "Exit ResManager::PathAttach( %s)\n", pathToAdopt);

	return( m_resMgrId);
}

/**
 * Forms a device path for a manager to adopt
 *
 * @param pathToAdopt
 *               Suffix to be adopted (i.e Ain, Icm, etc)
 * @return
 */
string ResManager::MakeDevPath( const char *pathToAdopt)
{
	string	path( "/dev/");
	path += string( pathToAdopt);

	return( path);
}
/**
 * Detaches the calling process from the path identified by deviceNumber. The
 * deviceNumber should have been obtained through a previously successful call
 * to PathAttach.
 *
 * @param deviceNumber
 *               Device to detach the calling process from (obtained from PathAttach)
 *
 * @return 0 if successful
 *         -1 if error; use errno to obtain more error information
 *
 * @see PathAttach
 */
int ResManager::PathDetach( int deviceNumber)
{
	int status;

	Log( LOG_FN_ENTRY, "Enter ResManager::PathDetach( %d)\n", deviceNumber);
//	status = resmgr_detach( m_dispatchPtr, deviceNumber, _RESMGR_DETACH_PATHNAME);
	status = resmgr_detach( m_dispatchPtr, deviceNumber, _RESMGR_DETACH_ALL);
	Log( LOG_FN_ENTRY, "Exit ResManager::PathDetach( %d)\n", deviceNumber);

	return( status);
}

/**
 * Connects a timer and pulse to our manager's dispatch context
 *
 * @param timerCode Pulse code to use when the timer expires
 * @param timerVal  Pulse value to attach
 * @param tickFun   Optional priority for the pulse to be delivered at
 * @param priority
 * @return >0 Timer ID if successful
 *         -1 if error; use errno to obtain more error information
 */
int ResManager::ConnectTimerPulse( int timerCode, int timerVal, timerTickFn_t tickFun,
								   int priority)
{
	int 			retVal = -1;
	struct sigevent	timerEvent;

	// Initialize an sigevent structure, and attach a pulse to it
	timerEvent.sigev_code = pulse_attach( m_dispatchPtr,	// Dispath to use for notifications
											0,				// Use a pulse for the dispatch
											timerCode,		// Pulse code to use
											tickFun,		// Routine to execute when we ge a pulse
											this);			// handle (????)
	if( timerEvent.sigev_code != -1)
	{
		// Connect to our channel
		timerEvent.sigev_coid = m_mssgCoid;
		if(timerEvent.sigev_coid  != -1)
		{
			timerEvent.sigev_notify = SIGEV_PULSE;
			timerEvent.sigev_priority = priority;//sched_get_priority_max(sched_getscheduler(0));
			timerEvent.sigev_value.sival_int = timerVal;
			if((retVal = TimerCreate(CLOCK_REALTIME, &timerEvent)) == -1)
			{
				string strErr(" TimerCreate failed: ");
				strErr += string( strerror( errno));
				throw( BepException( strErr));
			}
		}
		else
		{
			string strErr(" message_connect failed: ");
			strErr += string( strerror( errno));
			throw( BepException( strErr));
		}
	}
	else
	{
		string strErr(" pulse_attach failed: ");
		strErr += string( strerror( errno));
		throw( BepException( strErr));
	}

	return( retVal);

}

/**
 * Parses command line parameters
 *
 * @param argc   Number of command line arguments in the argv parameter
 * @param argv   Command line option strings
 */
void ResManager::ParseCommandLine( int argc, char **argv)
{
	char		optId;

	Log( LOG_FN_ENTRY, "Enter ResManager::ParseCommandLine\n");

	// Disable "Invalid argument" message from getopt
	opterr = 0;

	// Get Command line options
	for( register int ii=1; ii<argc; ii++)
	{
		optId = getopt( argc, argv, "cdef:r:v:");
		if( optId != -1)
		{
			if( optarg != NULL)
			{
				Log( LOG_DEV_DATA, "\t-%c\t%s\n", optId, optarg);
			}
			else
			{
				Log( LOG_DEV_DATA, "\t-%c\n", optId);
			}
		}

		switch( optId)
		{
		case 'd':
			m_demoMode = true;
			Log(LOG_DEV_DATA, "Demo mode enabled\n");
			break;
		case 'c':
		case 'e':
			SetLocalEcho( true);
			Log(LOG_DEV_DATA, "Console Echo Enabled\n");
			break;
		case 'f':
			SetConfigFile( optarg);
			Log(LOG_DEV_DATA, "Config File: %s\n", m_configFile.c_str());
			break;
		case 'r':
			SetProcessName( optarg);
			break;
		case 'v':
			char *tmp;
			UINT32	verbose;
			if( (tmp=strstr( optarg, "$")) != NULL)			sscanf( optarg, "$%X", &verbose);
			else if( (tmp=strstr( optarg, "0x")) != NULL)	sscanf( optarg, "0x%X", &verbose);
			else if( (tmp=strstr( optarg, "0X")) != NULL)	sscanf( optarg, "0X%X", &verbose);
			else 											sscanf( optarg, "%X", &verbose);
			SetVerboseMask( verbose);
			Log(LOG_DEV_DATA, "Verbose: $%08X\n", GetVerboseMask());
			break;
		default:
			if( optId == '?')						optind++;
			else if(argv[ optind] == NULL)			optind++;
			else if(argv[optind][0] != '-')	   		optind++;
			else if((argv[ optind][0] == '-') && (strlen(argv[ optind]) == 1)) optind++;
			break;
		}
	}

	Log( LOG_FN_ENTRY, "Exit ResManager::ParseCommandLine\n");
}

/**
 * Reads configuration data from the given XML file
 *
 * @param fileName Name of file to read configuration data from
 */
void ResManager::LoadConfig( const string &fileName)
{
	XmlParser	xmlParser;

	// Read setup options from the XML document
	try
	{
		// Get document root node
		Log( LOG_DEV_DATA, "ReturnXMLDocument( %s)\n", fileName.c_str());
		const XmlNode *rootNode = xmlParser.ReturnXMLDocument( fileName);
		Log( LOG_DEV_DATA, "File %s loaded and parsed\n", fileName.c_str());
		if( rootNode != NULL)
		{
			Initialize( rootNode);
		}
		else
		{
			throw( XmlException( XML_T("NO ROOT NODE")));
		}
	}
	catch( XmlException &err)
	{
		Log(LOG_ERRORS, "Error Reading Config File: %s\n", err.GetReason());
	}
	catch( ...)
	{
		Log(LOG_ERRORS, "Unknown Error Reading Config File\n");
	}
}

/**
 * Reads a configuration file
 *
 * @param fileName Name of config file to rread
 * @param parser   XML Parser object into which the given file will be loaded and parsed
 * @return 0 if successful
 *         -1 on error
 */
int ResManager::LoadXmlFile( const char *fileName, XmlParser &parser)
{
	int					retVal=-1;
	XmlString			docName( XML_T("ROOT"));

	// Read XML file (raw read)
	Log( LOG_DEV_DATA, "LoadFile( %s)\n", fileName);
	string	fileStr = LoadFile( fileName);
	Log( LOG_DEV_DATA, "File  %s loaded\n", fileName);

	try
	{
		// Parse the XML file
		if( parser.Parse( fileStr.c_str(), fileStr.length(), 1, docName) != 0)
		{
			Log(LOG_DEV_DATA, "File %s parsed\n", fileName);
			retVal = 0;
		}
		else
		{
			Log(LOG_ERRORS, "Error parsing XML file %s\r\n", fileName);
		}
	}
	catch( ...)
	{
		Log(LOG_ERRORS, "Unknown exception parsing XML file %s\r\n", fileName);
	}

	return( retVal);
}

/**
 * Reads configuration options from an XML document node
 *
 * @param setupNode Parent node from which option data will be read
 * @return 0 if successful
 *         -1 if error
 */
int ResManager::ReadXmlOptions( const XmlElement *setupNode)
{
	const XmlNodeMap &childNodes = setupNode->getChildren();
	XmlNode	*xmlNode;
	const XmlNode *poolNode = NULL;

	Log( LOG_FN_ENTRY, "Enter ResManager::ReadXmlOptions()\n");
	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XmlString( CFG_DEMO_TAG));
		m_demoMode = atol( xmlNode->getValue().c_str()) == 1;
		Log( LOG_DEV_DATA, "Demo mode=%d\n", m_demoMode);
	}
	catch( XmlException &err)
	{
	}

	try
	{
		poolNode = setupNode->getChild( CFG_THREAD_POOL_TAG);
	}
	catch( ...)
	{
		Log( LOG_ERRORS, "Error reading thread pool parameters from config file\n");
		poolNode = NULL;
	}

	// Initialize thread pool with defaults
	memset(&m_xmlPoolAttribs, 0, sizeof(m_xmlPoolAttribs));
	m_xmlPoolAttribs.context_alloc = ResManager::xmlContextAlloc;
	m_xmlPoolAttribs.block_func = ResManager::xmlBlockFunc;
	//m_xmlPoolAttribs.unblock_func = ResManager::xmlUnblockFunc;
	m_xmlPoolAttribs.handler_func = ResManager::xmlHandlerFunc;
	m_xmlPoolAttribs.context_free = ResManager::xmlContextFree;

	if( poolNode != NULL)
	{
		const XmlNodeMap &poolAttribs = poolNode->getAttributes();
		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_LOW_TAG)->getValue();
			m_xmlPoolAttribs.lo_water = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_xmlPoolAttribs.lo_water = THREAD_POOL_MIN;
		}

		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_HI_TAG)->getValue();
			m_xmlPoolAttribs.hi_water = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_xmlPoolAttribs.hi_water = THREAD_POOL_MAX;
		}
		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_MAX_TAG)->getValue();
			m_xmlPoolAttribs.maximum = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_xmlPoolAttribs.maximum = THREAD_POOL_MAX;
		}
		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_INC_TAG)->getValue();
			m_xmlPoolAttribs.increment = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_xmlPoolAttribs.increment = 1;
		}
		try
		{
			const XmlString &val = poolAttribs.getNode( CFG_POOL_PRIO_TAG)->getValue();
			m_poolThreadPriority = atol( val.c_str());
		}
		catch( XmlException &err)
		{
			m_poolThreadPriority = getprio( 0);
		}
	}
	else
	{
		// Default to the priority that our task was launched at
		m_poolThreadPriority = getprio( 0);

		m_xmlPoolAttribs.lo_water = THREAD_POOL_MIN;
		m_xmlPoolAttribs.hi_water = THREAD_POOL_MAX;
		m_xmlPoolAttribs.increment = 1;
		m_xmlPoolAttribs.maximum = THREAD_POOL_MAX;
	}

	// Set priority for pool threads
	struct sched_param	threadSched;
	pthread_attr_init(&m_xmlThreadAttributues);
	pthread_attr_getschedparam( &m_xmlThreadAttributues, &threadSched);
	threadSched.sched_priority = m_poolThreadPriority;
	pthread_attr_setinheritsched( &m_xmlThreadAttributues, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedparam( &m_xmlThreadAttributues, &threadSched);

	// Set XML threads to detached
	pthread_attr_setdetachstate( &m_xmlThreadAttributues, PTHREAD_CREATE_JOINABLE);
	m_xmlPoolAttribs.attr = &m_xmlThreadAttributues;

	Log( LOG_FN_ENTRY, "Exit ResManager::ReadXmlOptions()\n");

	return( 0);
}

/**
 * Reads the data tag names and properties
 *
 * @param setupNode Node to use as the root DATA_MAP node
 * @return 0 if successful
 *         -1 if error
 */
int ResManager::ReadDataMap( const XmlElement *setupNode)
{
	const XmlNodeMap &childNodes = setupNode->getChildren();
	XmlNode			*xmlNode;
	dataMapItem_t	dataItem;
	dataTag_t		itemTag;
	int				nodeIdx, nodeCnt=childNodes.size();

	Log( LOG_FN_ENTRY, "Enter ResManager::ReadDataMap\n");

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
			Log( LOG_ERRORS, "XML Error parsing Data Map Item %s: %s\n",
				 itemTag.c_str(), xmlErr.GetReason());
		}
		catch( BepException &bepErr)
		{
			Log( LOG_ERRORS, "Error parsing Data Map Item %s: %s\n",
				 itemTag.c_str(), bepErr.GetReason());
		}
	}

	Log( LOG_FN_ENTRY, "Exit ResManager::ReadDataMap\n");
	return( 0);
}

/**
 * Reads a data item's attributes from the given node
 *
 * @param itemNode Node to read data item properties from
 * @param dataItem Data Item property structure to fill in
 */
void ResManager::ReadDataMapItem( const XmlNode *itemNode, dataMapItem_t &dataItem)
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
void ResManager::InitializeIndexedTags()
{
	rawDataMapItr_t	itr;

	m_publishedItems.clear();
	m_wholeChannelItemMap.clear();

	// Evaluate all data table items
	for( itr=m_dataTable.begin(); itr!=m_dataTable.end(); itr++)
	{
		const dataMapItem_t *dataItem = itr->second;
		const dataTag_t 	&itemTag = itr->first;

		AddPublishedDataItem( dataItem, itemTag);
	}
	Log( LOG_DEV_DATA, "Publishing %d,%d items\n", m_publishedItems.size(),
		 m_wholeChannelItemMap.size());
}

/**
 * Adds a published data item to the published item maps
 *
 * @param dataItem Data item attributes
 * @param itemTag  Tag of the data item
 */
void ResManager::AddPublishedDataItem( const dataMapItem_t *dataItem, const dataTag_t &itemTag)
{
	if( dataItem->bitIdx == (UINT16)-1)
	{
		Log( LOG_DEV_DATA, "Adding whole channel item %s\n", itemTag.c_str());

		// Add it to the whole channel table if no parsing errors
		m_wholeChannelItemMap.Insert(dataItem->boardIdx, dataItem->channelIdx,
									 itemTag);
	}
	else
	{
		Log( LOG_DEV_DATA, "Adding  bit-based item %s\n", itemTag.c_str());

		// Add it to the bit-wise table if no parsing errors
		m_publishedItems.Insert( dataItem->boardIdx, dataItem->channelIdx,
								 dataItem->bitIdx, itemTag);
	}
}

/**
 * Calculates the index of the given tag in our list of published tags.
 * Index is calculated based on board index, channel index, bit index
 * and channels per board
 *
 * @param dataItem Item to calculate index for
 * @return Index for the given data item
 */
uint64_t ResManager::CalculateDataTagIndex( const dataMapItem_t *dataItem)
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
uint64_t ResManager::CalculateDataTagIndex( int board, int channel, UINT16 bit)
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
bool ResManager::IsWholeChannelItem( int boardIdx, int channelIdx)
{
	return( m_wholeChannelItemMap.Find( boardIdx, channelIdx));
}

/**
 * Method used to retrieve the tag for a data item based on board index,
 * channel indedx and bit index
 *
 * @param boardIdx   Index of the board to search for the tag on
 * @param channelIdx Index of the channel to search for the tag on
 * @param bitIdx     Index of the bit to search for the tag on
 * @param dataTag    Returned data tag for the given board,channel and index values
 * @return true if the dataTag return arg is valid, false if it is invalid
 */
bool ResManager::GetDataItemTag( int boardIdx, int channelIdx, int bitIdx, dataTag_t &dataTag)
{
	bool	retVal = false;

	// If this is a request for a whole channel item
	if( bitIdx == -1)
	{
		// Try to find the desired word
		retVal = m_wholeChannelItemMap.Find( boardIdx, channelIdx, &dataTag);
	}
	else
	{
		retVal = m_publishedItems.Find(boardIdx, channelIdx, bitIdx, &dataTag);
	}

	return( retVal);
}

/**
 * Publishes changed data values by spinning of a thread to perform
 * the notifications
 *
 * @param board  Board on which the changed data item is located
 * @param port   Channel number of the changed data item
 * @param bit    Bit index of the data tag to publish
 * @param val    Value to publish
 */
void ResManager::PublishData( int board, int port, int bit, XML_Char *val)
{
	// Make sure notification list is valid
	if(m_nList)
	{
		// If this is a whole word item
		if( bit == -1)
		{
			Log( LOG_DEV_DATA, "ResManager::PublishData word %d on board %d\n", board, port);
			WholeChannelItemMapItr_t	itr = m_wholeChannelItemMap.end();
			while( (itr=m_wholeChannelItemMap.Find(board, port, itr)) !=
				   m_wholeChannelItemMap.end())
			{
				const dataTag_t &tag = itr->second;
				m_nList->Notify( tag, val);
				Log( LOG_DEV_DATA, "ResManager::PublishData word( %s, %s)\n", tag.c_str(),  val);
			}
		}
		else
		{
			Log( 0x80000000, "ResManager::PublishData bit %d,%d on board %d\n", bit, port, board);
			PublishedTagListItr_t		itr = m_publishedItems.end();
			while( (itr=m_publishedItems.Find(board, port, bit, itr)) !=
				   m_publishedItems.end())
			{
				const dataTag_t &tag = itr->second;
				m_nList->Notify( tag, val);
				Log( 0x80000000, "ResManager::PublishData bit( %s, %s)\n", tag.c_str(),  val);
			}
		}
	}
	else
	{
		Log( LOG_ERRORS, "\tINVALID NOTIFICATION LIST IN ResManager::PublishData()\n");
	}
}

/**
 * Notification thread entry point. Thread is used to publish data values
 * to subscribed clients
 *
 * @param arg    PublishThreadArg_t data type contiaining th data to publish
 * @return NULL
 */
void* ResManager::PublishThread( void *arg)
{
	PublishThreadArg_t	*threadArg = (PublishThreadArg_t*)arg;
	XmlString			tag( threadArg->dataTag);
	XmlString			val( threadArg->dataVal);
	ResManager			*mgr = threadArg->ioMgr;

	mgr->Log( LOG_DEV_DATA, "\tPublisher publishsing %s=%s\n",
			  tag.c_str(), val.c_str());

	// Let parent thread continue
	pthread_barrier_wait( &threadArg->barrier);

	// Do notification
	if(mgr->m_nList)	mgr->m_nList->Notify( tag, val);

	mgr->Log( LOG_DEV_DATA, "\tPublisher thread done\n");

	return( NULL);
}

/**
 * Method called when a LOAD_CONFIG command message is received.
 */
void ResManager::ReloadConfig()
{
	int pulsePrio = sched_get_priority_max( sched_getscheduler(0));

	Log( LOG_FN_ENTRY, "Enter ResManager::ReloadConfig\n");

	// Set flag to say load config, keep executing
	m_xmlCommand = RES_MGR_CMD_CONFIG;

	// Send signal to main I/O loop (to stop it)
	if( MsgSendPulse( m_mssgCoid, pulsePrio, m_ioChnlPulseCode, RELOAD_PULSE) == -1)
	{
		Log( LOG_FN_ENTRY, "MsgSendPulse error: %s", strerror( errno));
	}
//	kill( getpid(), SIGABRT);

	Log( LOG_FN_ENTRY, "Exit ResManager::ReloadConfig\n");
}

/**
 * Method called to process a terminate XML command message
 *
 * @param pid    Process to whom we need to reply
 * @return 0 if successful, -1 if error
 */
int ResManager::HandleTerminate( pid_t pid)
{
	int retVal = 0;
	int pulsePrio = sched_get_priority_max( sched_getscheduler(0));

	Log( LOG_FN_ENTRY, "Enter ResManager::HandleTerminate( %d)\n", pid);

	if( MsgSendPulse( m_mssgCoid, pulsePrio, m_ioChnlPulseCode, TERMINATE_PULSE) == -1)
	{
		Log( LOG_FN_ENTRY, "MsgSendPulse error: %s", strerror( errno));
		retVal = -1;
	}
	Log( LOG_FN_ENTRY, "Exit ResManager::HandleTerminate( %d)\n", pid);

	return( retVal);
}

///////////////////////////////////////////////////////////////////////////////
// Thread pool handler functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Function in which each thread in the thread pool will block
 *
 * @param ctp    Thread context structure
 * @return
 */
THREAD_POOL_PARAM_T *ResManager::xmlBlockFunc(THREAD_POOL_PARAM_T *ctp)
{
	threadPoolParam_t	*threadData = (threadPoolParam_t*)ctp;
	ResManager			*ioMgr = (ResManager*)threadData->poolHandle->ioMgr;

	return( ioMgr->XmlBlockFunc(threadData));
}

/**
 * Function in which each thread in the thread pool will unblock. This method
 * appears to be called once for each "live" thread in the thread pool
 * from a thread with an ID of 1.
 *
 * @param ctp    Thread context structure
 */
void ResManager::xmlUnblockFunc(THREAD_POOL_PARAM_T *ctp)
{
	threadPoolParam_t	*threadData = (threadPoolParam_t*)ctp;
	ResManager			*ioMgr = (ResManager*)threadData->poolHandle->ioMgr;

	ioMgr->Log( LOG_DEV_DATA, "Enter ResManager::xmlUnblockFunc\n");
	ioMgr->Log( LOG_DEV_DATA, "Exit ResManager::xmlUnblockFunc\n");
}

/**
 * Function called by kernel to allocate a new struct to pass to a new thread
 *
 * @param handle OUr user defined handle: it is the connection ID to listen on
 * @return Pointer to the new structure to use with the new thread
 */
THREAD_POOL_PARAM_T *ResManager::xmlContextAlloc(THREAD_POOL_HANDLE_T *handle)
{
	threadPoolParam_t	*retVal = NULL;
	ResManager			*ioMgr = (ResManager*)( ((threadPoolHandle_t*)handle)->ioMgr);
	string				ipcChnlName( ioMgr->GetProcessName());

	ioMgr->Log( LOG_DEV_DATA, "Enter ResManager::xmlContextAlloc\n");

	retVal = new threadPoolParam_t( (threadPoolHandle_t*)handle, getpid());

	ioMgr->Log( LOG_DEV_DATA, "Initialize commObj object\n");

	retVal->commObj.Initialize( ioMgr->m_nameId->chid, ipcChnlName, ioMgr->IsDebugOn());

	ioMgr->Log( LOG_DEV_DATA, "commObj object initialized\n");

	ioMgr->Log( LOG_DEV_DATA, "Exit ResManager::xmlContextAlloc\n");

	return( retVal);
}
/**
 * Function called to free memory used for a thread argument struct
 *
 * @param param  Thread context structure to free
 */
void ResManager::xmlContextFree(THREAD_POOL_PARAM_T *ctp)
{
	if( ctp != NULL)
	{
		threadPoolParam_t	*threadData = (threadPoolParam_t*)ctp;
		ResManager			*ioMgr = threadData->poolHandle->ioMgr;

		ioMgr->Log( LOG_FN_ENTRY, "Enter ResManager::xmlContextFree()\n");

		delete threadData;

		ioMgr->Log( LOG_FN_ENTRY, "Exit ResManager::xmlContextFree()\n");
	}
}
/**
 * Function called after our thread has left the blocked state and is ready
 * to do some work
 *
 * @param ctp    Thread context structure for this thread to work with
 * @return 0
 */
int ResManager::xmlHandlerFunc(THREAD_POOL_PARAM_T *ctp)
{
	threadPoolParam_t	*threadData = (threadPoolParam_t*)ctp;
	ResManager			*ioMgr = (ResManager*)threadData->poolHandle->ioMgr;

	return( ioMgr->XmlHandlerFunc( threadData));
}
//
///////////////////////////////////////////////////////////////////////////////

/**
 * XML Message Handler
 */
void* ResManager::XmlThreadEntryPoint( void *arg)
{
	resMgrThreadArg_t	*threadArg = (resMgrThreadArg_t*)arg;
	ResManager			*ioMgr = (ResManager*)threadArg->ioMgr;

	ioMgr->Log( LOG_DEV_DATA, "XmlThreadEntryPoint running\n");

	if( ioMgr->XmlThreadInit( arg) == 0)
	{
		// Register our process
		ioMgr->m_nameId = (name_attach_t*)BposRegisterTaskName( ioMgr->GetProcessName().c_str());
		ioMgr->m_xmlPoolHandle.channelId = ioMgr->m_nameId->chid;

		// This is the data that gets passed into xmlContextAlloc
		ioMgr->m_xmlPoolAttribs.handle = (THREAD_POOL_HANDLE_T*)&ioMgr->m_xmlPoolHandle;

		// Wait until everyone is ready to run
		pthread_barrier_wait( &ioMgr->m_xmlThreadBarrier);

		// Create the thread pool
		ioMgr->m_xmlThreadPool = thread_pool_create(&ioMgr->m_xmlPoolAttribs, 0);

		// Start the thread pool
		if( ioMgr->m_xmlThreadPool != NULL)
		{
			// Never Returns
			thread_pool_start(ioMgr->m_xmlThreadPool);
		}
		else
		{
			ThrowOsException( errno, "Error creating thread pool");
		}

		pthread_exit( (void*)NULL);
	}
	else
	{
		ioMgr->Log( LOG_ERRORS, "Error during XmlHandlerInit()\n");
	}
	return( 0);
}

/**
 * Initializer for XmlThread
 *
 * @param arg    Thread argument
 * @return 0 if successful
 *         -1 if error
 */
int ResManager::XmlThreadInit( void *arg)
{
	int retVal = 0;

	Log( LOG_FN_ENTRY, "Enter ResManager::XmlThreadInit\n");

	// If OK to continue
	if( retVal == 0)
	{
		// Let threads know who the parent manager is
		m_xmlPoolHandle.ioMgr = this;

		// Install SIGABRT signal handler
		signal( SIGABRT, ::AbortHandler);

		// Allow CTRL-C to stop us
		signal(SIGINT, SIG_DFL);
	}

	Log( LOG_FN_ENTRY, "Exit ResManager::XmlThreadInit\n");
	return( retVal);
}

/**
 * Function in which each thread in the thread pool will block
 *
 * @param ctp    Thread context structure
 * @return
 */
THREAD_POOL_PARAM_T* ResManager::XmlBlockFunc(threadPoolParam_t	*threadData)
{
	int	replyId;

	// Set my priority back to where it should be
	BposSetThreadPriority( 0, 0, m_poolThreadPriority);

	// Add ourself to the list of active threads
	m_threadList.add( pthread_self());

	try
	{
		// Wait for data
		replyId = threadData->commObj.WaitForRequest();
	}
	catch( BepException &err)
	{
		Log( LOG_ERRORS, "m_ipcComm.WaitForRequest() error: %s\n", err.GetReason());
		replyId = -1;
	}

	// Check if we should exit
	pthread_testcancel();

	if( replyId >= 0)
	{
		threadData->rcvId = replyId;
		switch( threadData->rcvId)
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
		Log( LOG_ERRORS, "Read error: %s\n", strerror( errno));
	}

	return( (THREAD_POOL_PARAM_T *)threadData);
}

/**
 * Function called after our thread has left the blocked state and is ready
 * to do some work
 *
 * @param ctp    Thread context structure for this thread to work with
 * @return 0
 */
int ResManager::XmlHandlerFunc( threadPoolParam_t *threadData)
{
	IBepCommunication	handlerObj;

	Log( LOG_DEV_DATA, "Initialize handler object\n");
	handlerObj.Initialize( threadData->commObj);
	Log( LOG_DEV_DATA, "Handler object initialized, ID=%d\n", threadData->commObj.GetId());

	try
	{	// Warning, ensure that the string is blank prior to calling the handle request
        threadData->mssgString = "";
	    handlerObj.HandleRequest( threadData->mssgString, this);
	}
	catch(BepException &e)
	{
		Log( LOG_ERRORS, "Unexpected Error: %s\n", e.what());
	}

	// Remove ourselved from the active thread list
	m_threadList.remove( pthread_self(), false);

	return( 0);
}

/**
 * Launches the XML thread pool
 *
 * @return 0 if no errors
 *         -1 on error (use errno for further error information)
 */
int ResManager::LaunchXmlThread()
{
	int 				retVal = -1;

	Log( LOG_FN_ENTRY, "Enter ResManager::LaunchXmlThread\n");

	resMgrThreadArg_t	threadArg;
	threadArg.ioMgr = this;
	threadArg.parent = BposGetMyTaskId();
	threadArg.parentChannel = m_ctp->info.chid;

	if( XmlThreadInit( &threadArg) == 0)
	{
		// Register our process
		if( BposAttachName(GetProcessName().c_str(), (void**)&m_nameId) != SUCCESS)
		{
			ThrowOsException( errno, "Error attaching process name");
		}
		m_xmlPoolHandle.channelId = m_nameId->chid;

		// This is the data that gets passed into xmlContextAlloc
		m_xmlPoolAttribs.handle = (THREAD_POOL_HANDLE_T*)&m_xmlPoolHandle;

		// Create the thread pool
		m_xmlThreadPool = thread_pool_create(&m_xmlPoolAttribs, 0);

		// Start the thread pool
		if( m_xmlThreadPool != NULL)
		{
			// Never Returns
			thread_pool_start(m_xmlThreadPool);
			retVal = 0;
		}
		else
		{
			ThrowOsException( errno, "Error creating thread pool");
		}
	}
	else
	{
		Log( LOG_ERRORS, "Error during XmlHandlerInit()\n");
	}
	Log( LOG_FN_ENTRY, "Exit ResManager::LaunchXmlThread\n");

	return( retVal);
}

/**
 * Handles I/O mesages (file descriptor based, not XML)
 */
void ResManager::IoMssgLoop()
{
	Log( LOG_FN_ENTRY, "Enter ResManager::IoMssgLoop\n");

	// Service resource manager type messages
	while( m_xmlCommand == RES_MGR_CMD_NONE)
	{
		// Wait for data to come in
//		Log( LOG_FN_ENTRY, "resmgr_block: $%08X\n", (UINT32)m_ctp);
		if( resmgr_block( m_ctp) == NULL)
		{
			Log( LOG_DEV_DATA|LOG_ERRORS, "resmgr_block error 1: %s\n", strerror( errno));
			break;
		}

		// If we received a PULSE
		if( m_ctp->rcvid == 0)
		{
			// Handle the pulse
			if( HandleResMgrPulse( m_ctp->msg->pulse) > 0)
			{
				continue;
			}
		}
		else
		{
			Log( LOG_IO_MSSG, "I/O Mssg ID=$%08X\n", m_ctp->msg->type);
		}
		// Dispatch the message to the proper handler function
		resmgr_handler( m_ctp);
	}
	Log( LOG_FN_ENTRY, "Exit ResManager::IoMssgLoop\n");
}

/**
 * Handles pulses delivered to our resource manager I/O channel
 *
 * @param pulse  Pulse received on the I/O channel
 * @return 0 if successful; -1 if error; 1 if no further processing is required
 */
int ResManager::HandleResMgrPulse( io_pulse_t &pulse)
{
	int retVal = 0;

	switch( pulse.code)
	{
	case _PULSE_CODE_UNBLOCK:
		Log( LOG_IO_MSSG, "ResMgr Pulse _PULSE_CODE_UNBLOCK\n");
		break;
	case _PULSE_CODE_DISCONNECT:
		Log( LOG_IO_MSSG, "ResMgr Pulse _PULSE_CODE_DISCONNECT\n");
		break;
	case _PULSE_CODE_THREADDEATH:
		Log( LOG_IO_MSSG, "ResMgr Pulse _PULSE_CODE_THREADDEATH\n");
		break;
	case _PULSE_CODE_COIDDEATH:
		Log( LOG_IO_MSSG, "ResMgr Pulse _PULSE_CODE_COIDDEATH\n");
		break;
	case _PULSE_CODE_NET_ACK:
		Log( LOG_IO_MSSG, "ResMgr Pulse _PULSE_CODE_NET_ACK\n");
		break;
	case SYSTEM_PULSE:
		//Log( LOG_IO_MSSG, "ResMgr Pulse Code %d,%d\n", pulse.code, pulse.value.sival_int);
		switch( pulse.value.sival_int)
		{
		case TERMINATE_PULSE:
			m_xmlCommand = RES_MGR_CMD_TERMINATE;
			retVal = 1;
			break;
		case RELOAD_PULSE:
			m_xmlCommand = RES_MGR_CMD_CONFIG;
			retVal = 1;
			break;
		default:
			break;
		}
		break;
	default:
		//Log( LOG_IO_MSSG, "ResMgr Pulse Code $%08X\n", pulse.code);
		break;
	}

	BposHandlePulse( (void*)&m_ctp->msg->pulse);

	return( retVal);
}

/**
 * Kills any XML threads that are hanging around
 */
void ResManager::KillXmlThreads()
{
	pthread_t		threadId;
	ThreadListItr_t	itr;

	Log( LOG_FN_ENTRY, "Enter ResManager::KillXmlThreads( %d)\n",
		 m_threadList.size());

	// If not already destroyed
	if( m_xmlThreadPool != NULL)
	{
		// Cancel all threads first
		for( itr=m_threadList.begin(); itr!=m_threadList.end(); itr++)
		{
			threadId = itr->first;
			// If not current thread and not main thread of execution
			if( (threadId != pthread_self()) && (threadId > 1))
			{
				Log( LOG_DEV_DATA, "Cancelling thread %d\n", threadId);

				// Cancel the thread after it's unblocked
				pthread_cancel( threadId);
				// Cancel the thread after it's unblocked
				pthread_kill( threadId, SIGURG);
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
		m_threadList.clear();

		// Destroy the thread pool
		Log( LOG_DEV_DATA, "Destroying thread pool\n");
		thread_pool_destroy( m_xmlThreadPool);
		m_xmlThreadPool = NULL;
		Log( LOG_DEV_DATA, "Thread pool destroyed\n");

	}

	Log( LOG_FN_ENTRY, "Exit ResManager::KillXmlThreads( %d)\n", m_threadList.size());
}

/**
 * Fills in an XmlNode with the data value for a data item
 *
 * @param dataItem Properties for the data item to read
 * @param value    String to fill in with the value of the requested data item
 * @return 0 if valueNode is filled in
 *         -1 if unable to handle the request
 */
int ResManager::ReadDataItem( const dataMapItem_t *dataItem, string &value)
{
	Log( LOG_FN_ENTRY, "Enter ResManager::ReadDataItem()\n");
	Log( LOG_FN_ENTRY, "Exit ResManager::ReadDataItem()\n");
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
int ResManager::WriteDataItem( const dataMapItem_t *dataItem, const string &value)
{
	Log( LOG_FN_ENTRY, "Enter ResManager::WriteDataItem()\n");
	Log( LOG_FN_ENTRY, "Exit ResManager::WriteDataItem()\n");
	return( -1);
}

