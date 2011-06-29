//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/Common/ResManagerBase.cpp $
// $Author:: Cward                                                            $
//   $Date:: 4/13/06 3:49a                                                    $
//
// Description:
//	Single threaded resource manager base class
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Common/ResManagerBase.cpp $
// 
// 4     4/13/06 3:49a Cward
// Moved the code for Register() and ReloadConfiguration() from the header
// file to the cpp file Removed the chId parameter to IoMssgLoop()
//
// 2     2/15/06 4:02p Bmeinke
// Moved the code for Register() and ReloadConfiguration() from the header
// file to the cpp file
// Removed the chId parameter to IoMssgLoop()
//
// 3     7/20/05 9:05p Cward
// Added support for new TaskMon
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
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
// 3     6/26/03 4:27p Khykin
// Updated with Brians latest changes.
//
// 3     6/18/03 6:14p Bmeinke
// Wrapped ioRead, ioWrite, ioDevCtl, ioUnblock and ioLseek with calls to
// iofunc_attr_lock/iofunc_attr_unlock
//
// 2     5/29/03 3:49p Bmeinke
// Added a logger printout at the end of PulseHandler()
//
// 1     4/30/03 4:46p Bmeinke
// Migration to new personalized configuration management structure
//
// 31    4/29/03 11:27p Bmeinke
// If -e command line argument is passed in, enable logger as well as
// enable local echo
//
// 30    3/17/03 3:19p Bmeinke
// Need to pass a NULL XmlNode* to initialize if no config file specified
// on command line in order to properly initialize the thread pool of
// ResManagerMt objects
//
// 29    3/14/03 9:25p Bmeinke
// Command line arguments are processed using the new
// HandleCommandLineArg() virtual method. This allows for proper
// processing of arguments required by sub-classes
//
// 28    3/10/03 6:32p Bmeinke
// ReadConfigFile() no longer passes a NULL XmlNode pointer to
// Initialize() if no config file was specified on the command line
//
// 27    1/27/03 10:21p Bmeinke
// Printout logger string when attaching or detaching a pulse from our I/O
// channel
// When detaching a pulse code from our I/O channel, stop iterating
// through the list of attached pusle codes after we find the first
// matching pulse code in the attached pulse code list
//
// 26    1/22/03 3:30a Bmeinke
// Added DetachIoPulse() method to detach a pulse from our I/O channel
//
// 24    1/16/03 8:14a Bmeinke
// Changed IoUnblock() from using iofunc_unblock_default() to MsgError(
// ctp->rcvid, ETIMEDOUT) (iofunc_unblock_default did not appear to do
// anything)
//
// 23    12/16/02 10:44a Bmeinke
// Removed attaching of the _PULSE_CODE_UNBLOCK pulse to our I/O channel.
// Should be handled by the resource manager library via the io_unblock()
// functions
//
// 22    12/13/02 5:45p Bmeinke
// Explicitly attach the _PULSE_CODE_UNBLOCK pulse code to our I/O channel
// so we know when a client wants to unblock from a blocking operation
//
// 21    8/26/02 7:40p Bmeinke
// Added Log() calls from SignalHandler()
//
// 20    8/01/02 12:06p Bmeinke
// Removed compiler warnings
//
// 19    6/05/02 11:23p Bmeinke
// Added static ivar m_resMgrStatus: used by derived classes to know when
// it is time to terminate or re-load config when derived classes do not
// block on MsgReceive() and therefore do not receive the TERMINATE/RELOAD
// pulses
//
// 18    4/15/02 1:42p Bmeinke
// Modified ReadConfigFile() to use BepSomponent::Initialize(string)
// instead of parsing the config file inside ReadConfigFile()
//
// 17    4/04/02 2:44a Bmeinke
// Rearranged order of ivars in initialization list of constructor
// Removed call to SetVerboseMask( 0xFF) inside Initialize() method
// Fixed ReadSendersBuffer() method
//
// 16    3/22/02 1:11p Bmeinke
// Added helper method ReadSendersBuffer() to read a client's buffer whn
// processing a IoWrite message
// Added return( retVal) statement to Execute() method
//
// 15    2/27/02 2:55p Bmeinke
// Added call to BepComponent::HandlePulse() to HandleResMgrPulse() for
// SYSTEM_PULSE's
//
// 14    2/04/02 4:31p Bmeinke
// Added OcbInit() and OcbFinalize() methods to initialize and cleanup
// OCB's
//
// 13    2/01/02 9:52p Brian.meinke
// Made xmlNode a const XMlNode* in Initialize()
//
// 12    12/05/01 10:07p Brian.meinke
// Changed BposHandlePulse( (void*)&m_ctp->msg->pulse) to BposHandlePulse(
// (void*)&pulse) inside HandleResMgrPulse()
// Made the global SignalHandler() method a static member of
// ResManagerBase() in order to use Log() functionality
//
// 11    12/05/01 12:14p Brian.meinke
// Added data member m_attachedPulses to hold all the pulse codes attached
// to the I/O channel
// CleanupManager() now detahced all pulses contained in the
// m_attachedPulses list
// Added AttachIoPulse() method for attaching a single pulse to the I/O
// channel
//
// 10    11/28/01 10:36p Brian.meinke
// Added virtual AttachIoPulses() method to allow descendant classes to
// attach their own specific pulses to the I/O channel
//
// 9     11/26/01 1:30p Brian.meinke
// Fixed handling of unknown command line options in ParseCommandLine()
//
// 8     11/21/01 10:02p Brian.meinke
// Added newline characters to the end of some string passed to Log()
//
// 7     11/21/01 8:41p Brian.meinke
// First integration with existing ResourceManager derived classes
// (UniverseMgr)
//
// 6     11/16/01 3:10p Brian.meinke
// Added newline chars to a couple of Log() calls that need it
//
// 5     11/16/01 9:42a Brian.meinke
// Removed constructor which took command line arguments
// Cleaned up ParseCommandLine() to not spit out as much garbage
//
// 4     11/15/01 2:54p Brian.meinke
// Added constructor using command line arguments
// Added ParseCommandLine method
//
// 3     11/14/01 11:03p Brian.meinke
// Added inode argument to PathAttach
//
// 2     11/14/01 1:56a Brian.meinke
// Added SignalHandler()
// Added connection to send control pulses to
// Attach SYSTEM_PULSE's to the I/O channel
// Added static PulseHandler() method to handle the SYSTEM_PULSes
// delivered through the I/O channel
//
// 1     11/13/01 10:35p Brian.meinke
// Single threaded Resource Manager base class with no XML message support
//
//******************************************************************************

#include "ResManagerBase.h"

/**
 * Pointer needed to initialize ResManagerBase* inside the OCB
 */
ResManagerBase*				ResManagerBase::m_ioMgrForOcb=NULL;

/**
 * Mutex used to guard our signal handler
 */
pthread_mutex_t				ResManagerBase::m_signalMtx = PTHREAD_MUTEX_INITIALIZER;

/**
 * Connection to deliver control pulses to
 */
int							ResManagerBase::m_pulseSigCoId = -1;

/**
 * Current run status for the resource manager. Used to re-start the
 * resource manager when a Terminate or Reload pulse is received
 */
volatile ResManagerCmd_t	ResManagerBase::m_resMgrStatus = RES_MGR_CMD_NONE;

/**
 * Default constructor
 */
ResManagerBase::ResManagerBase() : BepComponent(),
	m_ioChnlPulseCode( -1), m_mssgCoid( -1), m_demoMode( false),
	m_dispatchPtr( NULL), m_ctp( NULL), m_resMgrId( -1)
{
	SetProcessName( "ResManagerBase");
	ResManagerBase::m_ioMgrForOcb = this;
}

/**
 * Class destructor
 */
ResManagerBase::~ResManagerBase()
{
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool ResManagerBase::InitManager()
{
	bool	retVal=false;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::InitManager\n");

	ResManagerBase::m_resMgrStatus = RES_MGR_CMD_NONE;

	signal( SIGINT, ResManagerBase::SignalHandler);
	signal( SIGQUIT, ResManagerBase::SignalHandler);
	signal( SIGWINCH, ResManagerBase::SignalHandler);

	try
	{
		// Initialize the dispatch interface
		if( InitDispatchInterface() == true)
		{
			// Load config file data
			if( ReadConfigFile() == true)
			{
				// Install our I/O handlers
				InitHandlers( (void*)ResManagerBase::ocbCalloc, (void*)ResManagerBase::ocbFree);
				retVal = true;
			}
			else
			{
				Log(LOG_ERRORS, "\tError loading config file: %s\n", strerror( errno));
			}
		}
		else
		{
			Log(LOG_ERRORS, "\tError initializing kernel interface: %s\n", strerror( errno));
		}
	}
	catch( XmlException &err)
	{
		Log( LOG_ERRORS, "\tError initializing manager '%s': %s\n",
			 GetProcessName().c_str(), err.GetReason());
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerBase::InitManager\n");
	return( retVal);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void ResManagerBase::CleanupManager()
{
	Log( LOG_FN_ENTRY, "Enter ResManagerBase::CleanupManager\n");

	// Destroy the connection on which the pulse can be received
	if( m_mssgCoid != -1)
	{
		Log( LOG_DEV_DATA, "Detach pulse connection\n");
		ConnectDetach( m_mssgCoid);
		m_mssgCoid = -1;
	}

	// Detach any pulses currently attached to our I/O channel
	while( m_attachedPulses.empty() == false)
	{
		pulse_detach( m_dispatchPtr, m_attachedPulses[0], 0);
		m_attachedPulses.erase( m_attachedPulses.begin());
	}

	m_ioChnlPulseCode = -1;

	// Release our context pointer
	if( m_ctp != NULL)
	{
		resmgr_context_free( m_ctp);
		m_ctp = NULL;
	}

	// Release our dispatch interface pointer
	if( m_dispatchPtr != NULL)
	{
		dispatch_destroy( m_dispatchPtr);
		m_dispatchPtr = NULL;
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerBase::CleanupManager\n");
}

/**
 * Method called to enter I/O message processing loop. Will not
 * return until we get a reload config pulse or a terminate pulse
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t ResManagerBase::Execute()
{
	ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::Execute\n");
	while( retVal == RES_MGR_CMD_NONE)
	{
		retVal = RES_MGR_CMD_NONE;
		if( InitManager() == true)
		{
			retVal = IoMssgLoop();

			CleanupManager();
		}
		else
		{
			retVal = RES_MGR_CMD_TERMINATE;
		}
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerBase::Execute\n");

	return( retVal);
}

/**
 * Reads configuration data from the given XML node
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void ResManagerBase::Initialize(const XmlNode *document)
{
	const XmlNodeMap& cfgNodes = document->getChildren();
	const XmlNode	*xmlNode;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::Initialize\n");

	if( document != NULL)
	{
		try
		{
			// Initialize base class
			BepComponent::Initialize( document);
		}
		catch( XmlException &err)
		{
		}

#if 0
		//
		try
		{
			// Try to set verbose mask from config file setting
			XmlString verboseTag( CFG_VERBOSE_TAG);
			xmlNode = cfgNodes.getNode( verboseTag);
			SetVerboseMask(BposReadInt( xmlNode->getValue().c_str()));
			Log( LOG_DEV_DATA, "Verbose: %ld\n", GetVerboseMask());
		}
		catch( XmlException &err)
		{
		}
#endif

		try
		{
			XmlString demoTag( CFG_DEMO_TAG);
			xmlNode = cfgNodes.getNode( demoTag);
			m_demoMode = atol( xmlNode->getValue().c_str()) == 1;
			Log( LOG_DEV_DATA, "Demo mode=%d\n", m_demoMode);
		}
		catch( XmlException &err)
		{
		}
	}

	RegisterWithTaskMon();

	Log( LOG_FN_ENTRY, "Exit ResManagerBase::Initialize\n");
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string ResManagerBase::Register(void)
{
	return( BepComponent::Register());
}

/**
 * Reloads the configuration file and resets the
 * base components objects.
 * @since Version 1.0
 */
void ResManagerBase::ReloadConfiguration(void)
{

}

/**
 * Parses command line parameters
 *
 * @param argc      Number of command line arguments in the argv parameter
 * @param argv      Command line option strings
 * @param optstring getopt() style list of command line switches to look for
 */
void ResManagerBase::ParseCommandLine( int argc, char *argv[], const char *optstring)
{
	char		optId;
	const char	*myOpts = "f:der:v:";
	std::string	optString( myOpts);;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::ParseCommandLine\n");

	if( optstring != NULL)
	{
		optString += std::string( optstring);
	}

	// Disable "Invalid argument" message from getopt
	opterr = 0;

	// Start reparsing from the first argument
	optind = 1;

	// Get Command line options
	while( optind < argc)
	{
		optId = getopt( argc, argv, optString.c_str());

		if( HandleCommandLineArg( optId, optarg) == false)
		{
			Log( LOG_DEV_DATA, "\tUnknown arg: %c @ %d\n", optId, optind++);
    		if( optId == '?')						optind++;
			else if(argv[ optind] == NULL)			optind++;
			else if(argv[optind][0] != '-')	   		optind++;
			else if((argv[ optind][0] == '-') && (strlen(argv[ optind]) == 1)) optind++;
		}
		optarg = NULL;
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerBase::ParseCommandLine\n");
}

/**
 * Method used to handle a command line argument. Called from
 * ParseCommandLine() for each comamnd line argument encountered.
 *
 * @param option The command line option (i.e. 'f' if -f specified on command line)
 * @param value  Value associated with the command line option being processed.
 *               May be NULL is no argument is associated with the command line
 *               option
 *
 * @return true if the argument was processed, false if not recognized
 */
bool ResManagerBase::HandleCommandLineArg( int option, const char *value)
{
	bool	retVal = true;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

	switch( option)
	{
	case 'd':
		m_demoMode = true;
		break;
	case 'e':
		EnableLogger();
		SetLocalEcho( true);
		break;
	case 'f':
		if( value != NULL)	SetConfigFile( value);
		break;
	case 'r':
		if( value != NULL)	SetProcessName( value);
		break;
	case 'v':
		if( value != NULL)
		{
			char *tmp;
			UINT32	verbose;
			if( (tmp=strstr( optarg, "$")) != NULL)			sscanf( optarg, "$%X", &verbose);
			else if( (tmp=strstr( optarg, "0x")) != NULL)	sscanf( optarg, "0x%X", &verbose);
			else if( (tmp=strstr( optarg, "0X")) != NULL)	sscanf( optarg, "0X%X", &verbose);
			else 											sscanf( optarg, "%X", &verbose);
			SetVerboseMask( verbose);
			Log(LOG_DEV_DATA, "Verbose: $%08X\n", GetVerboseMask());
		}
		break;
	default:
		retVal = false;
		break;
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerBase::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

	return( retVal);
}

/**
 * Reads configuration data from config file
 *
 * @return true if config data successfully loaded, false otherwise
 */
bool ResManagerBase::ReadConfigFile()
{
	bool			retVal = true;
	const string	&cfgFile = GetConfigFile();

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::ReadConfigFile()\n");

	// If config file was not specified
	if( cfgFile.empty() == false)
	{
		Log( LOG_DEV_DATA, "Loading config file from %s\n", cfgFile.c_str());
		BepComponent::Initialize( cfgFile);
	}
	else
	{
		Log( LOG_ERRORS, "\tNo configuration file specified\n");
		Initialize( NULL);
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerBase::ReadConfigFile\n");

	return( retVal);
}

/**
 * Initializes the message dispatch interface
 *
 * @return true on success
 *         false on error
 */
bool ResManagerBase::InitDispatchInterface()
{
	Log( LOG_FN_ENTRY, "Enter ResManagerBase::InitDispatchInterface\n");
	if( m_dispatchPtr == NULL)
	{
		m_dispatchPtr = dispatch_create();
	}
	else
	{
		Log( LOG_DEV_DATA, "\t---> Dispatch pointer already allocated\n");
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerBase::InitDispatchInterface\n");
	return( m_dispatchPtr != NULL);
}

/**
 * Registers the calling process as a resource manager
 *
 * @return 0 if successful
 *         -1 if error; use errno to obtain more error information
 */
int ResManagerBase::RegisterManager()
{
	string	strErr;
	int retVal = 0;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::RegisterManager\n");
	if( m_ctp == NULL)
	{
		if( (m_ctp = resmgr_context_alloc( m_dispatchPtr)) != NULL)
		{
			if( AttachIoPulses() != -1)
			{
				// Create a connection on which the pulse can be received
				m_mssgCoid = message_connect( m_dispatchPtr, MSG_FLAG_SIDE_CHANNEL);
				if( m_mssgCoid == -1)
				{
					strErr = "\tmessage_connect failed: " + string( strerror( errno));
					strErr += "\n";
					Log( LOG_ERRORS, strErr.c_str());
					throw( BepException( strErr));
				}
				Log( LOG_DEV_DATA, "\tm_mssgCoid = %d\n", m_mssgCoid);

				// Tell outside world where to deliver control pulses
				ResManagerBase::m_pulseSigCoId = m_mssgCoid;
			}
			else
			{
				strErr = "\tAttachIoPulses failed: " + string( strerror( errno));
				strErr += "\n";
				Log( LOG_ERRORS, strErr.c_str());
				throw( BepException( strErr));
			}
		}
		else
		{
			strErr = "\tresmgr_context_alloc failed: " + string( strerror( errno));
			strErr += "\n";
			Log( LOG_ERRORS, strErr.c_str());
			throw( BepException( strErr));
		}
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerBase::RegisterManager\n");
	return( retVal);
}

/**
 * Method used to attach pulses to the resource manager's I/O
 * channel
 *
 * @return -1 on error, any other value indicates success
 */
int ResManagerBase::AttachIoPulses()
{
	int retVal=-1;

	// Attach the terminate pulse to our resource manager's channel
	if( (m_ioChnlPulseCode = AttachIoPulse( SYSTEM_PULSE)) != -1)
	{
		retVal = 0;
	}

	return( retVal);
}

/**
 * Method used to attach a pulse to the resource manager's I/O
 * channel
 *
 * @param pulseCode Pulse code to attach to the resource manager's I/O channel
 * @return -1 on error, any other value indicates success
 */
int ResManagerBase::AttachIoPulse( int pulseCode)
{
	int retVal;

	Log( LOG_DEV_DATA, "\tAttaching pulse code %d to I/O channel\n", pulseCode);

	// Attach the terminate pulse to our resource manager's channel
	retVal = pulse_attach( m_dispatchPtr, 0, pulseCode,
						   ResManagerBase::PulseHandler, this);
	if( retVal != -1)
	{
		m_attachedPulses.push_back( pulseCode);
	}

	return( retVal);
}

/**
 * Method used to detach a pulse from the resource manager's I/O
 * channel
 *
 * @param pulseCode Pulse code to detach to the resource manager's I/O channel
 * @return -1 on error, any other value indicates success
 */
void ResManagerBase::DetachIoPulse( int pulseCode)
{
	vector<int>::iterator	itr;

	for( itr=m_attachedPulses.begin(); itr!=m_attachedPulses.end(); itr++)
	{
		if( *itr == pulseCode)
		{
			Log( LOG_DEV_DATA, "\tDetaching pulse code %d from I/O channel\n", pulseCode);
			pulse_detach( m_dispatchPtr, pulseCode, 0);
			m_attachedPulses.erase( itr);
			break;
		}
	}
}

/**
 * Initializes the I/O message handler functions to the default OS values.
 *
 * @param ocbAlloc Function to be used to allocate OCBs
 * @param ocbFree  Function to be used to free OCBs
 */
void ResManagerBase::InitHandlers( void* ocbAllocFn, void* ocbFreeFn)
{
	Log( LOG_FN_ENTRY, "Enter ResManagerBase::InitHandlers\n");

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
	//	methods og the ResManagerBase interface)
	m_connectFuncs.open = ResManagerBase::ioOpen;
	m_ioFuncs.read = ResManagerBase::ioRead;
	m_ioFuncs.write = ResManagerBase::ioWrite;
	m_ioFuncs.close_ocb = ResManagerBase::ioCloseOcb;
	m_ioFuncs.stat = ResManagerBase::ioStat;
	m_ioFuncs.notify = ResManagerBase::ioNotify;
	m_ioFuncs.devctl = ResManagerBase::ioDevCtl;
	m_ioFuncs.unblock = ResManagerBase::ioUnblock;
	m_ioFuncs.pathconf = ResManagerBase::ioPathconf;
	m_ioFuncs.lseek = ResManagerBase::ioLseek;
	m_ioFuncs.chmod = ResManagerBase::ioChmod;
	m_ioFuncs.chown = ResManagerBase::ioChown;
	m_ioFuncs.utime = ResManagerBase::ioUtime;
	m_ioFuncs.openfd = ResManagerBase::ioOpenFd;
	m_ioFuncs.space = ResManagerBase::ioSpace;
	m_ioFuncs.mmap = ResManagerBase::ioMmap;
	m_ioFuncs.msg = ResManagerBase::ioMsg;
	m_ioFuncs.sync = ResManagerBase::ioSync;

	Log( LOG_FN_ENTRY, "Exit ResManagerBase::InitHandlers\n");
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
 * @param inode  inode value to assign to this path attachment
 * @param flags  Flags to pass to the kernel (0=default)
 * @param ioAttr Attribute structure to use for this path (default=NULL).
 *               If NULL, a default attribute structure will be used
 * @param init   Flag to indicate if we need to initialize the attributes
 *               structure (normally false)
 * @return the device number associated with the path.
 */
int ResManagerBase::PathAttach( const char *pathToAdopt, int size, int mode,
				int inode, int flags, IOFUNC_ATTR_T *ioAttr, bool init)
{
	string	path = MakeDevPath( pathToAdopt);
	int		devNo;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::PathAttach( %s)\n", pathToAdopt);
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
	ioAttr->inode = inode;

	// Patch in our "mount" handler (for allocating/freeing client OCBs)
	if( NULL == ioAttr->mount)
	{
	ioAttr->mount = &m_mountPoint;
	}

	// attach our device name
	devNo = resmgr_attach(m_dispatchPtr,
							   &m_resmgrAttr,
							   path.c_str(),
							   _FTYPE_ANY,
							   flags,
							   &m_connectFuncs,
							   &m_ioFuncs,
							   ioAttr);

	if( init == true)
	{
		m_resMgrId = devNo;
	}

	Log( LOG_FN_ENTRY, "Exit ResManagerBase::PathAttach( %s)\n", pathToAdopt);
	return( devNo);
}

/**
 * Attaches the calling process as the IO manager of the given 'pathToAdopt'
 * path. The OS will send messages to our process when IO occurs within the
 * 'pathToAdopt' path. The OS will include a unit number in IO messages
 * pertaining to our path. This unit number is identical to the device number
 * returned by this function. The OS does not include the path in its IO
 * messages, so a process must keep track of device number/path relationships.
 * THe attributes structure is assumed to have already been intialized (i.e.
 * this function WILL NOT initialize it)
 *
 * @param pathToAdopt
 *               Path namespace to take over
 * @param ioAttr Attribute structure to use for this path (default=NULL).
 *               If NULL, a default attribute structure will be used
 * @return the device number associated with the path.
 */
int ResManagerBase::PathAttach( const char *pathToAdopt, IOFUNC_ATTR_T *ioAttr)
{
	return( PathAttach(pathToAdopt, ioAttr->nbytes, ioAttr->mode, ioAttr->inode,
					   ioAttr->flags,ioAttr, false));
}

/**
 * Forms a device path for a manager to adopt
 *
 * @param pathToAdopt
 *               Suffix to be adopted (i.e Ain, Icm, etc)
 * @return
 */
string ResManagerBase::MakeDevPath( const char *pathToAdopt)
{
	string	path( pathToAdopt);

	// IF the given path is not an absolute path name
	if( pathToAdopt[ 0] != '/')
	{
		// Adopt path under /dev
		path = string( "/dev/") + path;
	}

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
int ResManagerBase::PathDetach( int deviceNumber)
{
	int retVal = -1;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::PathDetach( %d)\n", deviceNumber);
	retVal = resmgr_detach( m_dispatchPtr, deviceNumber, _RESMGR_DETACH_ALL);
	Log( LOG_FN_ENTRY, "Exit ResManagerBase::PathDetach( %d)\n", deviceNumber);

	return( retVal);
}


///////////////////////////////////////////////////////////////////////////
//		I/O Handler methods
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

/**
 * Handles I/O mesages
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t ResManagerBase::IoMssgLoop()
{
	ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

	Log( LOG_FN_ENTRY, "Enter ResManagerBase::IoMssgLoop()\n");

	// Service resource manager type messages
	while( retVal == RES_MGR_CMD_NONE)
	{
		// Reset pulse command
		retVal = RES_MGR_CMD_NONE;

		// Wait for data to come in
		if( (m_ctp=resmgr_block( m_ctp)) != NULL)
		{
			// If we received a PULSE
			if( m_ctp->rcvid == 0)
			{
				// Handle the pulse
				retVal = HandleResMgrPulse( m_ctp->msg->pulse);
			}
			// Dispatch the message to the proper handler function
			resmgr_handler( m_ctp);
		}
		else
		{
			Log( LOG_ERRORS, "\tError in resmgr_block: %s\n", strerror( errno));
		}
	}
	Log( LOG_FN_ENTRY, "Exit ResManagerBase::IoMssgLoop()\n");

	return( retVal);
}

/**
 * Handles pulses delivered to our resource manager I/O channel
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t ResManagerBase::HandleResMgrPulse( io_pulse_t &pulse)
{
	ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

	Log( LOG_DEV_DATA, "Enter ResManagerBase::HandleResMgrPulse( %d, %d)\n",
		pulse.code, pulse.value.sival_int);

	if( pulse.code == SYSTEM_PULSE)
	{
		switch( pulse.value.sival_int)
		{
		case TERMINATE_PULSE:
			retVal = RES_MGR_CMD_TERMINATE;
			break;
		case RELOAD_PULSE:
			retVal = RES_MGR_CMD_CONFIG;
			break;
		default:
			break;
		}
		BepComponent::HandlePulse( pulse.code, pulse.value.sival_int);
	}

	// Let BPOS handle all other pulses
	BposHandlePulse( (void*)&pulse);

	Log( LOG_DEV_DATA, "Exit ResManagerBase::HandleResMgrPulse( %d, %d)\n",
		pulse.code, pulse.value.sival_int);

	return( retVal);
}

/**
 * The purpose of this is to give us a place to allocate our own ocb. It is
 * called as a result of the open being done (e.g. iofunc_open_default
 * causes it to be called). We registered it through the mount structure.
 *
 * @param ctp    Resource manager's context block
 * @param device Attributes for this client connection (an OCB)
 * @return Pointer to newly allocated OCB or NULL if error
 */
IOFUNC_OCB_T* ResManagerBase::ocbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
	IOFUNC_OCB_T	*retOcb = NULL;
	ResManagerBase*	mgr = (ResManagerBase*)ResManagerBase::m_ioMgrForOcb;

	retOcb = mgr->OcbCalloc( ctp, device);
	if( retOcb != NULL)
	{
		// Hold onto the info for this message
		MsgInfo( ctp->rcvid, &retOcb->mssgInfo);

		mgr->OcbInit( retOcb);
	}

	return( retOcb);

}

/**
 * The purpose of this is to give us a place to allocate our own ocb. It is
 * called by ocbCalloc, which in turn is called as a result of an open being done
 * on our reource manager's adopted path(e.g. iofunc_open_default causes it to be
 * called). ocbCalloc was registered through our mount structure.
 *
 * @param ctp    Resource manager's context block
 * @param device Attributes for this client connection (an OCB)
 * @return Pointer to newly allocated OCB or NULL if error
 * @see ResManagerBase::ocbCalloc
 */
IOFUNC_OCB_T* ResManagerBase::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
	// Allocate and clear a new OCB for this connection
	resMgrIoOcb_t *ocb = new resMgrIoOcb_t;
	return( ocb);
}

/**
 * Method used to initialize an allocated OCB structure. Called
 * immediately after the OCB is allocated
 *
 * @param ioOcb  OCB structure to be initialized
 */
void ResManagerBase::OcbInit( resMgrIoOcb_t *ioOcb)
{
	memset( ioOcb, 0, sizeof( ioOcb));

	ioOcb->m_mgr = this;
}

/**
 * The purpose of this is to give us a place to free our ocb.
 * It is called as a result of the close being done
 * (e.g. iofunc_close_ocb_default causes it to be called). We
 * registered it through the mount structure.
 *
 * @param ocb    Client's Open Context Block for this connection
 */
void ResManagerBase::ocbFree(IOFUNC_OCB_T *ocb)
{
	resMgrIoOcb_t	*icmOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)icmOcb->m_mgr;

	ioMgr->OcbFinalize( ocb);
	ioMgr->OcbFree( ocb);
}

/**
 * The purpose of this is to give us a place to free our ocb. It is called
 * by ResManagerBase::ocbFree, which in turn is called as a result of the close
 * being done (e.g. iofunc_close_ocb_default causes it to be called).
 * ResManagerBase::ocbFree was registered through our mount structure.
 *
 * @param ocb    Client's Open Context Block for this connection
 * @see ResManagerBase::ocbFree
 */
void ResManagerBase::OcbFree(resMgrIoOcb_t *ocb)
{
	// Free the memory
	delete ocb;
	ocb = NULL;
}


/**
 * Method used to finalize an allocated OCB structure. Called immediately
 * before the OCB is destroyed
 *
 * @param ioOcb  OCB structure to be finalized
 */
void ResManagerBase::OcbFinalize( resMgrIoOcb_t *ioOcb)
{
}

/**
 * Message handler for _IO_OPEN I/O messages
 *
 * @param ctp    Resource Manager context block
 * @param msg    Received data message data
 * @param handle Pointer to client connection attributes (an OCB)
 * @param extra  Pointer to manager specific private data (optional)
 * @return 0 the number of bytes sent to the client
 *         <0 if error occurs
 * @see open
 */
int ResManagerBase::ioOpen( resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra)
{
	return( iofunc_open_default( ctp, msg, (iofunc_attr_t*)handle, extra));
}
/**
 * Message handler for _IO_OPEN I/O messages
 *
 * @param ctp    Resource Manager context block
 * @param msg    Received data message data
 * @param handle Pointer to client connection attributes (an OCB)
 * @param extra  Pointer to manager specific private data (optional)
 * @return 0 the number of bytes sent to the client
 *         <0 if error occurs
 * @see ResManagerBase::ioOpen
 */
int ResManagerBase::IoOpen( resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra)
{
	return( iofunc_open_default( ctp, msg, (iofunc_attr_t*)handle, extra));
}

/**
 * Message handler for _IO_READ I/O messages
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ocb    Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 */
int ResManagerBase::ioRead(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;
	int				retVal = ENOSYS;

	// Lock our device while reading
	if( (retVal=iofunc_attr_lock(ioOcb->m_ioOcb.attr)) == EOK)
	{
		// Check read priveleges
		if( (retVal=iofunc_read_verify( ctp, msg, (iofunc_ocb_t*)ocb, NULL)) == EOK)
		{

			retVal = ioMgr->IoRead(ctp, msg, ioOcb);
		}
		iofunc_attr_unlock( ioOcb->m_ioOcb.attr);
	}
	else
	{
		ioMgr->Log( LOG_ERRORS, "\tError locking attributes for read: %s\n", strerror( retVal));
	}

	return( retVal);
}

/**
 * Message handler for _IO_READ I/O messages. Called by ResManagerBase::ioRead
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ioOcb  Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioRead
 */
int ResManagerBase::IoRead( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_read_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Message handler for _IO_WRITE I/O messages
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Write message data
 * @param ocb    Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 */
int ResManagerBase::ioWrite( resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;
	int				retVal = ENOSYS;

	// Lock our device while writing
	if( (retVal=iofunc_attr_lock(ioOcb->m_ioOcb.attr)) == EOK)
	{
		// Check write priveleges
		if( (retVal=iofunc_write_verify( ctp, msg, (iofunc_ocb_t*)ocb, NULL)) == EOK)
		{

			retVal = ioMgr->IoWrite(ctp, msg, ioOcb);
		}
		iofunc_attr_unlock( ioOcb->m_ioOcb.attr);
	}
	else
	{
		ioMgr->Log( LOG_ERRORS, "\tError locking attributes for write: %s\n", strerror( retVal));
	}

	return( retVal);
}

/**
 * Message handler for _IO_WRITE I/O messages. Called ny ResManagerBase::ioWrite
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Write message data
 * @param ioOcb  Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioWrite
 */
int ResManagerBase::IoWrite( resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_write_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Function synthesized by the base-layer library when the last
 * close has been received for a particular OCB.
 *
 * @param ctp      Dispatch interface pointer
 * @param reserved Reserved for ther base-layer library
 * @param ocb      Control block for the client
 * @return 0 if successful
 *         < 0 if error occurs
 */
int ResManagerBase::ioCloseOcb(resmgr_context_t *ctp, void *reserved, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoCloseOcb(ctp,reserved,ioOcb));
}

/**
 * Function synthesized by the base-layer library when the last
 * close has been received for a particular OCB. Called by ioCloseOcb
 *
 * @param ctp      Dispatch interface pointer
 * @param reserved Reserved for ther base-layer library
 * @param ioOcb    Control block for the client
 * @return 0 if successful
 *         < 0 if error occurs
 * @see ResManagerBase::ioCloseOcb
 */
int ResManagerBase::IoCloseOcb(resmgr_context_t *ctp, void *reserved, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_close_ocb_default( ctp, reserved, (iofunc_ocb_t*)ioOcb));
}

/**
 * Handles the message that requests information about the
 * resource associated with the passed OCB
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Stat structure to return to the client
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioStat(resmgr_context_t *ctp, io_stat_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoStat(ctp, msg, ioOcb));
}

/**
 * Handles the message that requests information about the
 * resource associated with the passed OCB. Called by ioStat
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Stat structure to return to the client
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioStat
 */
int ResManagerBase::IoStat(resmgr_context_t *ctp, io_stat_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_stat_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Handler responsible for installing, polling, or removing a notification
 * handler
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Notification message
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioNotify(resmgr_context_t *ctp, io_notify_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoNotify(ctp, msg, ioOcb));
}

/**
 * Handler responsible for installing, polling, or removing a notification
 * handler. Called by ioNotify
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Notification message
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioNotify
 */
int ResManagerBase::IoNotify(resmgr_context_t *ctp, io_notify_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( ENOSYS);
}

/**
 * Handler routine for custom device commands (in response to
 * a client devctl() call)
 *
 * @param ctp    Resource Manager context block
 * @param msg    Received data message data
 * @param ocb    Client Open Context Block block
 * @return 0 if successful
 *         < 0 if error occurs
 */
int ResManagerBase::ioDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;
	int				retVal = ENOSYS;

	// Lock our device while devctl'ing
	if( (retVal=iofunc_attr_lock(ioOcb->m_ioOcb.attr)) == EOK)
	{
		retVal = ioMgr->IoDevCtl(ctp, msg, ioOcb);
		iofunc_attr_unlock( ioOcb->m_ioOcb.attr);
	}
	else
	{
		ioMgr->Log( LOG_ERRORS, "\tError locking attributes for devctl: %s\n", strerror( retVal));
	}

	return( retVal);
}

/**
 * Handler routine for custom device commands (in response to
 * a client devctl() call)
 *
 * @param ctp    Resource Manager context block
 * @param msg    Received data message data
 * @param ioOcb  Client Open Context Block
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManagerBase::ioDevCtl
 */
int ResManagerBase::IoDevCtl( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_devctl_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Handler for kernel pulses issued as a result of a alient's attempt
 * to unblock during the I/O mesage phase of an I/O operation
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Pulse from the kernel
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioUnblock(resmgr_context_t *ctp, io_pulse_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;
	int				retVal = ENOSYS;

	// Lock our device while unblocking client
	if( (retVal=iofunc_attr_lock(ioOcb->m_ioOcb.attr)) == EOK)
	{
		retVal = ioMgr->IoUnblock(ctp, msg, ioOcb);
		iofunc_attr_unlock( ioOcb->m_ioOcb.attr);
	}
	else
	{
		ioMgr->Log( LOG_ERRORS, "\tError locking attributes for unblock: %s\n", strerror( retVal));
	}

	return( retVal);
}

/**
 * Handler for kernel pulses issued as a result of a alient's attempt
 * to unblock during the I/O mesage phase of an I/O operation.
 * Called by ioUnblock
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Pulse from the kernel
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioUnblock
 */
int ResManagerBase::IoUnblock(resmgr_context_t *ctp, io_pulse_t *msg, resMgrIoOcb_t *ioOcb)
{
	// Send the client an ETIMEDOUT error
	MsgError( ctp->rcvid, ETIMEDOUT);
	return(_RESMGR_NOREPLY);
}

/**
 * Handler responsible for returning the value of a configurable
 * parameter (name) for the resource associated with the passed OCB
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Pathconf message
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 0if error
 */
int ResManagerBase::ioPathconf(resmgr_context_t *ctp, io_pathconf_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoPathconf(ctp, msg, ioOcb));
}

/**
 * Handler responsible for returning the value of a configurable
 * parameter (name) for the resource associated with the passed OCB.
 * Called by ioPathconf
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Pathconf message
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 0if error
 * @see ResManagerBase::ioPathconf
 */
int ResManagerBase::IoPathconf(resmgr_context_t *ctp, io_pathconf_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_pathconf_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Handler called in response to a client lseek() call
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Lseek message (how far to seek and from where)
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioLseek(resmgr_context_t *ctp, io_lseek_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;
	int				retVal = ENOSYS;

	// Lock our device while unblocking client
	if( (retVal=iofunc_attr_lock(ioOcb->m_ioOcb.attr)) == EOK)
	{
		retVal = ioMgr->IoLseek(ctp, msg, ioOcb);
		iofunc_attr_unlock( ioOcb->m_ioOcb.attr);
	}
	else
	{
		ioMgr->Log( LOG_ERRORS, "\tError locking attributes for lseek: %s\n", strerror( retVal));
	}

	return( retVal);
}

/**
 * Handler called in response to a client lseek() call. Called by ioLseek
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Lseek message (how far to seek and from where)
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioLseek
 */
int ResManagerBase::IoLseek(resmgr_context_t *ctp, io_lseek_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_lseek_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Handler responsible for changing the mode of the resource associated
 * with the passed OCB to the mode passed in the msg parameter
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Chmod message (new mode value)
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioChmod(resmgr_context_t *ctp, io_chmod_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoChmod(ctp, msg, ioOcb));
}

/**
 * Handler responsible for changing the mode of the resource associated
 * with the passed OCB to the mode passed in the msg parameter.
 * Called by ioChmod
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Chmod message (new mode value)
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioChmod
 */
int ResManagerBase::IoChmod(resmgr_context_t *ctp, io_chmod_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_chmod_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Handler responsible for changin the User ID/Groiup ID for the resource
 * associated with the passed OCB
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Chown message (new user/group ID)
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioChown(resmgr_context_t *ctp, io_chown_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoChown( ctp, msg, ioOcb));
}

/**
 * Handler responsible for changin the User ID/Groiup ID for the resource
 * associated with the passed OCB. Called by ioChown
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Chown message (new user/group ID)
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioChown
 */
int ResManagerBase::IoChown(resmgr_context_t *ctp, io_chown_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_chown_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Handler responsible for modifying the update time for the resource
 * associated with the passed OCB
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Update time message (new update time to be applied)
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioUtime(resmgr_context_t *ctp, io_utime_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoUtime(ctp, msg, ioOcb));
}

/**
 * Handler responsible for modifying the update time for the resource
 * associated with the passed OCB. Called by ioUtime
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Update time message (new update time to be applied)
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioUtime
 */
int ResManagerBase::IoUtime(resmgr_context_t *ctp, io_utime_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_utime_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Similar to the io_open() connect handler except an already open file
 * descriptor is given instead of a pathname
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Openfd message (file descriptor to open)
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioOpenFd(resmgr_context_t *ctp, io_openfd_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoOpenFd( ctp, msg, ioOcb));
}

/**
 * Similar to the io_open() connect handler except an already open file
 * descriptor is given instead of a pathname. Called by ioOpenFd
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Openfd message (file descriptor to open)
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioOpenFd
 */
int ResManagerBase::IoOpenFd(resmgr_context_t *ctp, io_openfd_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_openfd_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
}

/**
 * Handler used to allocate or free space occupied by the resource
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Space message (how much to free and from where to start)
 * @param ocb    Control block for the calling client
 * @return Number of bytes
 */
int ResManagerBase::ioSpace(resmgr_context_t *ctp, io_space_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;
	int				retVal = ENOSYS;

	// Check client priveleges
	if( (retVal=iofunc_space_verify( ctp, msg, (iofunc_ocb_t*)ocb, NULL)) == EOK)

{

		retVal = ioMgr->IoSpace( ctp, msg, ioOcb);
	}

	return( retVal);
}

/**
 * Handler used to allocate or free space occupied by the resource.
 * Called by ioSpace
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Space message (how much to free and from where to start)
 * @param ioOcb  Control block for the calling client
 * @return Number of bytes
 * @see ResManagerBase::ioSpace
 */
int ResManagerBase::IoSpace(resmgr_context_t *ctp, io_space_t *msg, resMgrIoOcb_t *ioOcb)
{
	int noBlock;
	return( iofunc_space_verify(ctp, msg, (iofunc_ocb_t*)ioOcb, &noBlock));
}

/**
 * Handler to allow the process manager to mmap() files from your
 * resource manager
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Mmap message (protection, size, offset, etc)
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioMmap(resmgr_context_t *ctp, io_mmap_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoMmap(ctp,msg,ioOcb));
}

/**
 * Handler to allow the process manager to mmap() files from your
 * resource manager. Called by ioMmap
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Mmap message (protection, size, offset, etc)
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioMmap
 */
int ResManagerBase::IoMmap(resmgr_context_t *ctp, io_mmap_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( ENOSYS);
}

/**
 * Handler for custom resource manager messages (more general variation of
 * devctl() handler, but less portable)
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Custom message
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioMsg(resmgr_context_t *ctp, io_msg_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;

	return( ioMgr->IoMsg( ctp, msg, ioOcb));
}

/**
 * Handler for custom resource manager messages (more general variation of
 * devctl() handler, but less portable). Called by ioMsg
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Custom message
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioMsg
 */
int ResManagerBase::IoMsg(resmgr_context_t *ctp, io_msg_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( ENOSYS);
}

/**
 * Handler for a flush command
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Sync message (what to flush to disk)
 * @param ocb    Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 */
int ResManagerBase::ioSync(resmgr_context_t *ctp, io_sync_t *msg, RESMGR_OCB_T *ocb)
{
	resMgrIoOcb_t	*ioOcb = (resMgrIoOcb_t*)ocb;
	ResManagerBase	*ioMgr = (ResManagerBase*)ioOcb->m_mgr;
	int				retVal = ENOSYS;

	// Check client priveleges
	if( (retVal=iofunc_sync_verify( ctp, msg, (iofunc_ocb_t*)ocb)) == EOK)
	{

		retVal = ioMgr->IoSync( ctp, msg, ioOcb);
	}

	return( retVal);
}

/**
 * Handler for a flush command. Called by ioSync
 *
 * @param ctp    Dispatch interface pointer
 * @param msg    Sync message (what to flush to disk)
 * @param ioOcb  Control block for the calling client
 * @return 0 if successful
 *         < 0 if error
 * @see ResManagerBase::ioSync
 */
int ResManagerBase::IoSync(resmgr_context_t *ctp, io_sync_t *msg, resMgrIoOcb_t *ioOcb)
{
	return( iofunc_sync_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
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
int ResManagerBase::PulseHandler( message_context_t *ctp, int code, unsigned flags,
							  void *handle)
{
	io_pulse_t		&pulse = ctp->msg->pulse;
	ResManagerBase	*ioMgr = (ResManagerBase*)handle;

	ioMgr->Log( LOG_FN_ENTRY, "Enter ResManagerBase::PulseHandler\n");
	(void)ioMgr->HandleResMgrPulse(pulse);
	ioMgr->Log( LOG_FN_ENTRY, "Exit ResManagerBase::PulseHandler\n");
	return( 0);
}

/**
 * Function invoked inresponse to a signal to our process
 *
 * @param SigNo  Siagnal we caught
 */
void ResManagerBase::SignalHandler( int SigNo)
{
	ResManagerBase *ioMgr = ResManagerBase::m_ioMgrForOcb;

	ioMgr->Log( LOG_FN_ENTRY, "Enter ResManagerBase::SignalHandler: %d\n",
				SigNo);
	// Lock this operation
	if( pthread_mutex_lock( &ResManagerBase::m_signalMtx) == EOK)
	{
		// Make pulse very high priority
		int prio = sched_get_priority_max( sched_getscheduler(0));
		switch( SigNo)
		{
		case SIGINT:
		case SIGQUIT:
			ResManagerBase::m_resMgrStatus = RES_MGR_CMD_TERMINATE;
			ioMgr->Log( LOG_DEV_DATA, "Handling SIGQUIT: %d\n",
						ResManagerBase::m_pulseSigCoId);
			MsgSendPulse( ResManagerBase::m_pulseSigCoId, prio, SYSTEM_PULSE,
						  TERMINATE_PULSE);
			break;
		case SIGWINCH:
			ResManagerBase::m_resMgrStatus = RES_MGR_CMD_CONFIG;
			ioMgr->Log( LOG_DEV_DATA, "Handling SIGWINCH: %d\n",
						ResManagerBase::m_pulseSigCoId);
			MsgSendPulse( ResManagerBase::m_pulseSigCoId, prio, SYSTEM_PULSE,
						  RELOAD_PULSE);
			break;
		default:
			break;

		}
		pthread_mutex_unlock( &ResManagerBase::m_signalMtx);
	}
	ioMgr->Log( LOG_FN_ENTRY, "Exit ResManagerBase::SignalHandler: %d\n",
				SigNo);
}

/**
 * Helper function to read a client's IoWrite buffer
 *
 * @param ctp    Resource Manager context block
 * @param buff   Buffer to read the data into
 * @param len    Length of the buff parameter
 * @return Number of bytes read if successful
 *         -1 if error occurs
 */
int ResManagerBase::ReadSendersBuffer( resmgr_context_t *ctp, uint8_t* buff, size_t len)
{
	int	   	bytesRead = 0;
	int	   	retVal = 0;
	off_t	offset;

	Log( LOG_FN_ENTRY, "Enter ReadSendersBuffer()\n");

	// Starting offset into senders buffer to start reading at
	offset = sizeof( struct _io_write);

	// Loop until all data read from sender
	while( (bytesRead=resmgr_msgread( ctp, &buff[ retVal], (len-bytesRead), offset)) > 0)
	{
		// Keep track of how many bytes we have read
		retVal += bytesRead;
		offset += bytesRead;

		// Don't overflow the caller's buffer
		if( retVal >= (int)len)
		{
			break;
		}
	}

	// If an error occurred
	if( bytesRead == -1)
	{
		// Return error status
		retVal = -1;
	}
	Log( LOG_FN_ENTRY, "Exit ReadSendersBuffer()\n");

	return( retVal);

}

