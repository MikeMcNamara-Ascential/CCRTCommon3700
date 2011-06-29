//******************************************************************************
// Description:
//    Resource manager for Altera IP Icm card
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IpIcmMgr.cpp $
// 
// 14    5/01/07 12:55p Rwiersem
// Changes for the 07050201 core release:
// 
// Removed the leading $'s when reading the FTP_ROOT environment variable.
// 
// 13    1/18/07 12:58p Rwiersem
// Changes for the 07011901 core release:
// 
// - Fixed WritePlcWheelSpeeds() to work for both two and three axle
// machines.
// 
// 12    10/26/06 10:40a Rwiersem
// Changes for the 06102701 core release:
// 
// Now read in the zerospeed value from the configuration file.
// 
// 11    7/19/06 6:10p Cward
// Added support for Tandem Wheel Speeds
//
// 10    4/13/06 4:05a Cward
// Fixed a couple of spots where the board index of a data item was not
// being considered
//
// 2     2/15/06 4:10p Bmeinke
// Fixed a couple of spots where the board index of a data item was not
// being considered
// 9     7/20/05 9:12p Cward
// Added new TaskMon support. Added support for commandable overspeed.
// Slowed down write interval
//
// 8     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 7     3/11/05 3:35p Cward
// Fixed backwards logic in Calculate Average Data. Removed check and
// processing for disable system in Calculate Inst Data.
//
// 6     3/03/05 11:46p Cward
// Updated for 05030302. Needed quick fix for new licensing
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 3     7/07/04 6:41p Cward
// Added reloading of the config file on Start of Test.
//
// 2     6/09/04 11:41p Cward
// Moved header comment block to front of file. Updae the header comment
// block to the new standard.
//
// 1     12/21/03 6:28p Cward
//
// 5     10/24/03 1:20p Khykin
// Updated with encoder phasing changes.
//
// 1     10/07/03 11:34a Khykin
// Initial checkin
//
// 3     8/18/03 7:47a Khykin
// Updated with Flatrock changes.
//
// 2     8/07/03 1:48p Khykin
// Updating with changes for Flatrock.
//
// 2     6/26/03 4:01p Khykin
// Updated with Brians latest changes.
//
// 6     6/24/03 2:10p Bmeinke
// Reset the distance accumulators at the start of each test
//
// 5     6/23/03 10:12a Bmeinke
// Fixed conversion constant swapping bug
//
// 4     6/20/03 3:36p Bmeinke
// Fixed bug when in Write() and channel key lookup
//
// 3     6/18/03 7:10p Bmeinke
// Added ReloadConfig() method to re-read config data while running
// Changed "Kspeed" to "KSpeed"
// IoWrite() inode value was being decomposed improperly
// AttachInstPaths() inode value was calculated wrong
// Added CalculateConversionFactors()
// Moved calls to ReadDataLogConfig() and ReadDataTagList() from
// ReadGeneralConfig() to Initialize()
// Fixed bug in ReadBlockData() where wrong varaible (ii) was being used
// as the index into the data arrays
//
// 2     5/29/03 2:31p Bmeinke
// Data items mapped to invalid board or channel numbers are no longer
// presented under the /dev/Icm directory
// Changed the algorithm used to encode a board/channel/sub-channel
// triplet into an instantaneous data item's inode
//
// 1     4/30/03 4:37p Bmeinke
// Migration to new personalized configuration management structure
//
// 12    4/30/03 2:13p Bmeinke
// Added support to write wheelspeed values to the PLC for FordChicago
//
// 11    4/10/03 6:28p Bmeinke
// Moved the operation of manually latching the ICM channel values from
// SampelData(IpIcmModList_t&) to the SampelData(IpIcm*)
// Added special log print statements to ProcessData()
// Fixed bug in the calculation of the acceleration conversion constant
//
// 10    4/04/03 12:22p Bmeinke
// Due to interrupt problems, changed from interrupt driven sampling to
// timer based sampling
// Added USE_ISR to allow switching between timed sampling and ISR driven
// sampling
// Do not attach our ISR until we have been told to register
//
// 9     3/28/03 12:11a Bmeinke
// Moved the call to LoadMachineLosses() from Initialize() to
// InitManager() because need to know how many ICM boards (and
// subsequently channels) are in the system
// Fixed the saving of machine losses to an external data file
// Fixed the array index returned when a client tags an array sample
//
// 8     3/21/03 7:22p Bmeinke
// Fixed ZerospeedToPLC bug: comparing m_filteredZerospeed to zspd was
// wrong; need to check if m_filteredZerospeed changed since last sample
//
// 7     3/18/03 6:23p Bmeinke
// Need to read IXM specific configuration data before calling
// ResManagerXml::Initialize()
// Added 'm_waitToRegister' to allow command line option to register at
// initialization instead of waiting for the XML register message
// Changed Read(), Write(), Subscribe(), Publish() and Command() to use
// XmlNode* instaed of tag and/or value
// Fixed the array device size returned from IoStat()
// Added m_filteredZerospeed and m_zerospeedCount variables to perform
// simple hysteresis filtering on instantaneous zerospeed value
// Replaced ParseCommandLine() with HandleCommandLineArg()
// Fixed the calculation of the array sample index in UpdateArrays()
// UpdateDataLog() checks to see if the IcmLogFile object passed in is
// enabled before attempting to log a data sampel to the log file
// WriteZerospeed() uses the value of m_filteredZerospeed instead of
// instantaneous m_zspd when notifying subscribed clients of zerospeed
// status
// Fixed the inode comparison in ReadArrayData()
//
// 6     3/13/03 12:36p Bmeinke
// Initialize all simpel data members in SetDefaults()
//
// 5     3/12/03 11:28p Bmeinke
// Wasnt publishing/updating zerospeed or overspeed status
//
// 4     3/12/03 9:27p Bmeinke
// Removed printf() if our watchdog expires
//
// 3     3/12/03 9:25p Bmeinke
// Added IsrWatchdogTimer class to help detect when the ICM is not
// generating interrupts to us.
// If our ISR watchdog timer expires, disable and then re-enable ISR on
// the ICM IP's
// Added DetachIsr() method
//
// 2     3/10/03 7:23p Bmeinke
// Removed references to the IsrThread (no longer used)
// Replaced all defined time periods and sample counts with configurable
// data members
// Removed KillPoolThreads (no longer needed because we don't have the isr
// thread anymore)
// Disable interrupts from the IP modules and the carrier card in
// CleanupManager()
//
// 1     3/10/03 11:08a Bmeinke
// Initial checkin. Compiles and runs
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#define USE_ISR 0

#include "IpIcmMgr.h"
#include <math.h>
#include <algorithm>

// ===============================================================================

/**
 * Default constructor
 */
IpIcmMgr::IpIcmMgr() : ResManagerXml( "mgr/IpIcm"), m_encoderPhasing(0), m_disableSystem(false), m_reloadingConfigs(false)
{
	SetDefaults();
}

/**
 * Constructor with command line arguments
 *
 * @param argc   Number of arguments in the argc array
 * @param argv   Array of command line options
 */
IpIcmMgr::IpIcmMgr( int argc, char *argv[]) : ResManagerXml( "mgr/IpIcm"), m_encoderPhasing(0), m_reloadingConfigs(false)
{
	SetDefaults();

	ParseCommandLine( argc, argv, "p:R");
}

/**
 * Destructor
 */
IpIcmMgr::~IpIcmMgr()
{
}

/**
 * Handles I/O mesages
 *
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t IpIcmMgr::IoMssgLoop()
{
	ResManagerCmd_t retVal = RES_MGR_CMD_NONE;

	retVal = ResManagerXml::IoMssgLoop();

	return( retVal);
}

/**
 * method for component initialization.
 *
 * @param document Reference to a parsed configuration document.
 */
void IpIcmMgr::Initialize(const XmlNode *document)
{
	const XmlNode *setupNode;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::Initialize()\n");
	m_reloadingConfigs = true;
	try
	{
		setupNode = document->getChild( XML_T( "Setup"));

		const XmlNodeMap &childNodes = setupNode->getChildren();

		ReadGeneralConfig( childNodes);

		// Read array logging configuration
		ReadDataLogConfig( childNodes.getNode( CFG_DATA_LOG_TAG));

		// Read the mapping of ICM tag names to system tag names
		ReadDataTagList( childNodes.getNode( CFG_TAG_LIST_TAG));

		// Read all channel configuration data
		ReadChannelConfig( childNodes, m_tagList[ LF_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ RF_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ LR_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ RR_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ LT_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ RT_CHANNEL_NAME].c_str());

		try
		{
			// Determine if the ICM is single phase or multiple phase
			m_encoderPhasing = (bool) (setupNode->getChild(XML_T("EncoderPhasing"))->getValue() == "Single");
			Log(LOG_DEV_DATA, "Using Encoder Phasing: %s, %d\n", setupNode->getChild(XML_T("EncoderPhasing"))->getValue().c_str(), m_encoderPhasing);
		}
		catch( XmlException &err)
		{
			// Determine if the ICM is single phase or multiple phase
			m_encoderPhasing = false;
			Log(LOG_DEV_DATA, "Assuming Encoder Phasing: Single, %d\n", m_encoderPhasing);
		}

		// Configure board(s)
		InitializeBoards();

		// Read general config (for all ICM boards we manage)
		ResManagerXml::Initialize( document);

		// Load losses from file
		LoadMachineLosses();
	}
	catch(XmlException &err)
	{
		Log( LOG_ERRORS, "Error initializing IpIcmMgr: %s\n", err.GetReason());
		throw;
	}

	m_reloadingConfigs = false;
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::Initialize()\n");
}

/**
 * Reloads the configuration file and resets the
 * base components objects.
 * @since Version 1.0
 */
void IpIcmMgr::ReloadConfiguration(void)
{
	XmlParser       parser;			  // Create a parser
	const uint64_t  isrStopDelay = mSEC_nSEC(100LL);

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::ReloadConfiguration()\n");

	// Stop sampling
	DetachIsr();
	TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_NANOSLEEP, NULL, &isrStopDelay, NULL);

	try
	{
		const XmlNode *xmlDoc = parser.ReturnXMLDocument(GetConfigFile());
		const XmlNode *setupNode = xmlDoc->getChild( XML_T( "Setup"));
		const XmlNodeMap &childNodes = setupNode->getChildren();

		// Clear our existing lists
		m_channelIndexes.clear();
		m_channelIdxMap.clear();
		m_rollAttribs.clear();

		// Read config common to all channels/boards
		ReadGeneralConfig( childNodes);

		// Read all channel configuration data
		ReadChannelConfig( childNodes, m_tagList[ LF_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ RF_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ LR_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ RR_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ LT_CHANNEL_NAME].c_str());
		ReadChannelConfig( childNodes, m_tagList[ RT_CHANNEL_NAME].c_str());

		// Tell the ICM's to recalculate thier constants
		CalculateConversionFactors();

		// Load losses from file
		LoadMachineLosses();
	}
	catch(XmlException &err)
	{
		Log( LOG_ERRORS, "\tXML Exception in IpIcmMgr::ReloadConfiguration: %s\n", err.GetReason());
		throw;
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "IpIcmMgr::ReloadConfiguration", e.what());
	}
	// Start sampling data again
	AttachIsr();

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::ReloadConfiguration()\n");
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool IpIcmMgr::InitManager()
{
	bool        retVal = true;
	uint32_t    arraySize;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::InitManager()\n");

	try
	{
		// Enable I/O privity for ISR processing
		if(ThreadCtl( _NTO_TCTL_IO, 0) == -1)
		{
			throw( BepException( "Cannot attain I/O privity; must be run as root"));
		}

		// Initialize the kernel fd interface
		ResManagerBase::InitManager();

		// Pre-allocate enough room for all the instantaneous data values
		m_instData.reserve( m_icmIps.size());   m_instData.resize( m_icmIps.size());
		arraySize = m_arraySize * m_icmIps.size() * MAX_CHANNELS;
		m_distArray = new float[ arraySize];    memset( m_distArray, 0,  arraySize * sizeof( float));
		m_speedArray = new float[ arraySize];   memset( m_speedArray, 0,  arraySize * sizeof( float));
		m_freqArray = new float[ arraySize];    memset( m_freqArray, 0,  arraySize * sizeof( float));
		m_accelArray = new float[ arraySize];   memset( m_accelArray, 0,  arraySize * sizeof( float));
		m_forceArray = new float[ arraySize];   memset( m_forceArray, 0,  arraySize * sizeof( float));
		Log( LOG_DEV_DATA, "\tData logs contain %d floats\n", arraySize);

		// Register our path
		PathAttach( ICM_PRFX_ROOT);

		// Initialize message dispatch context
		RegisterManager();

		if(BposAttachName( GetProcessName().c_str(), (void**)(&m_nameId)) != SUCCESS)
		{
			Log( LOG_ERRORS, "\tError registering name %s: %s\n", GetProcessName().c_str(),
				 strerror( errno));
			retVal = false;
		}

		if(m_waitToRegister == false)
		{
			Register();
		}
	}
	catch(BepException &err)
	{
		Log( LOG_ERRORS, "\tInitManager error: %s\n", err.GetReason());
		retVal = false;
	}

	if(m_priority > 10)
	{
		BposSetPriority(0, m_priority);
	}
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::InitManager()\n");

	return( retVal);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void IpIcmMgr::CleanupManager()
{
	IOFUNC_ATTR_T*  ioAttr;
	uint32_t ii;

	// Detach our ISR event and disable interrupts from the ICM modules
	DetachIsr();

	// Detach from our adopted paths
	for(ii=0; ii<m_ioFuncAttrs.size(); ii++)
	{
		if((ioAttr = m_ioFuncAttrs[ ii]) != NULL)
		{
			delete ioAttr;
		}
	}
	m_ioFuncAttrs.clear();

	// Empty out our "temporary" data map table
	m_channelIndexes.clear();

	// Base class cleanup
	ResManagerXml::CleanupManager();

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::CleanupManager()\n");
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string IpIcmMgr::Register(void)
{
	std::string retVal;

	// Start the boards sampling
	AttachIsr();

	// Initialize the named data broker interface
	InitializeNdb();

	// Call base class method to register with TaskMon
	retVal = ResManagerBase::Register();

	return( retVal);
}

/**
 * Request to be inserted into the list to be notified
 * of a change in a data tag
 *
 * @param node     Data and Name for who is requesting the subscription information.
 * @return Status of operation
 *         BEP_SUCCESS_RESPONSE == Subscriber successfully added
 *         BEP_FAILURE_RESPONSE == Error adding subscriber
 * @since Version 2.0
 */
const std::string IpIcmMgr::Subscribe(const XmlNode *node)
{
	const std::string dataTag( node->getName());
	string retVal( BEP_UNSUPPORTED_RESPONSE);

	// See if it is zerospeed (only thing I want to publish)
	if(dataTag == m_tagList[ ZEROSPEED_NAME])
	{
		retVal = BepComponent::Subscribe( node);
	}

	return( retVal);
}

/**
 * Handle the subscribed for data by the process.
 *
 * @param node    Tag and data that was subscribed for
 * @return Status of the updating of the published tag
 *         Success, Fail
 * @since Version 2.1
 */
const std::string IpIcmMgr::Publish(const XmlNode *node)
{
	const std::string   tag( node->getName()), value( node->getValue());
	const int           sampleSize = (m_icmIps.size() == 1 ? 4 : 6);

	Log( LOG_FN_ENTRY, "Enter IcmMgr::Publish( %s, %s)\n", tag.c_str(), value.c_str());

	// If this is a start test event
	if(tag == m_tagList[ "StartTestTag"])
	{
		// If starting to test, turn on logging
		if(atoi( value.c_str()) != 0)
		{
			Log( LOG_DEV_DATA, "Starting Array logging\n");
			m_reloadingConfigs = true;
			// Reload the roll factors and re-calculate conversion factors
			ReloadConfiguration();

			// Reset the array logging index back to 0
			m_arrayIndex  = 0;

			// Restart logging at index = 0
			m_lastDataLogIndex = 0;

			// Set flag to say loggin array data is active
			m_arrayLogActive = true;

			// Initialize the data log objects
			m_distanceLog.StartLog();
			m_speedLog.StartLog();
			m_brakeForceLog.StartLog();
			m_accelLog.StartLog();
			m_freqLog.StartLog();

			// Clear the distance accumulators at the start of a test
			IpIcmListItr_t  itr;
			for(itr=m_icmIps.begin(); itr!=m_icmIps.end(); itr++)
			{
				IpIcm *icm = *itr;
				icm->ClearDistance( -1);
			}

			// Clear out the current drag values
			memset( &m_currDrags, 0, sizeof( m_currDrags));

			// Reset overspeed to its default
			m_currentOverSpeedVal = m_defaultOverSpeedVal;
			m_reloadingConfigs = false;
		}
		// If this is the end of the test, shut off logging
		else
		{
			Log( LOG_DEV_DATA, "Stopping Array logging @ %d\n", (int)(m_arrayIndex / sampleSize));

			// Set flag to say loggin array data is active
			m_arrayLogActive = false;

			// Finalize the data log objects
			m_distanceLog.EndLog();
			m_speedLog.EndLog();
			m_brakeForceLog.EndLog();
			m_accelLog.EndLog();
			m_freqLog.EndLog();

			// Clear out the current drag values
			memset( &m_currDrags, 0, sizeof( m_currDrags));

			// Reset overspeed to its default
			m_currentOverSpeedVal = m_defaultOverSpeedVal;
		}
	}
	// If this is a start test event
	else if(tag == m_tagList[ "EndTestTag"])
	{
		// If this is the end of the test, shut off logging
		Log( LOG_DEV_DATA, "Stopping Array logging @ %d\n", (int)(m_arrayIndex / sampleSize));

		// Set flag to say loggin array data is active
		m_arrayLogActive = false;

		// Finalize the data log objects
		m_distanceLog.EndLog();
		m_speedLog.EndLog();
		m_brakeForceLog.EndLog();
		m_accelLog.EndLog();
		m_freqLog.EndLog();

		// Clear out the current drag values
		memset( &m_currDrags, 0, sizeof( m_currDrags));

		// Reset overspeed to its default
		m_currentOverSpeedVal = m_defaultOverSpeedVal;
	}
	else if( tag == m_tagList[ "LossCompensationInProgress"])
	{
		m_lossComp = (value == "1");
	}

	return( BEP_SUCCESS_RESPONSE);
}

/**
 * Request to Perform an action
 *
 * @param node    Action to be performed
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 * @since Version 2.0
 */
const std::string IpIcmMgr::Command(const XmlNode *node)
{
	const std::string tag( node->getName()), value( node->getValue());
	string retVal( BEP_UNSUPPORTED_RESPONSE);
	const int   sampleSize = (m_icmIps.size() == 1 ? 4 : 6);
	uint32_t    currIndex = m_arrayIndex / sampleSize;

	// If client wants to
	if(tag == m_tagList[ TAG_IDX_NAME])
	{
		if( m_dataTagList.Lock() == EOK)
		{
			// Add new tag name
			m_dataTagList.AddTag( currIndex, value);
			// put array index into string for return to client
			char retBuff[ 256];
			retVal = ultoa( currIndex, retBuff, 10);

			Log( LOG_DEV_DATA, "=====>Tagging array index %d as '%s'\n", currIndex, value.c_str());

			m_dataTagList.Unlock();
		}
	}
	else if(tag == "LicenseViolation")
	{
		m_disableSystem = true;
		retVal = BEP_SUCCESS_RESPONSE;
	}
	else
	{
		retVal = BepComponent::Command( node);
	}

	return( retVal);
}

/**
 * Request to read a data tag
 *
 * @param node    Data to be read
 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
 * @return Value of tag
 * @since Version 2.0
 */
const std::string IpIcmMgr::Read(const XmlNode *node, const INT32 rate)
{
	const std::string tag( node->getName());
	string retVal("");
	char    reply[ 256];

	Log( LOG_FN_ENTRY, "Enter IcmMgr::Read( %s, %d)\n", tag.c_str(), rate);

	// If a client is asking for number of rollers
	if(tag == m_tagList[ ROLLER_COUNT_NAME])
	{
		// If 2 boards present
		if(m_icmIps.size() == 2)   retVal = "6";
		else						retVal = "4";
	}
	// If client is requesting zerospeed status
	else if(tag == m_tagList[ ZEROSPEED_NAME])
	{
		if(m_zeroSpeed)			   retVal = "1";
		else						retVal = "0";
	}
	// If client is requesting overspeed status
	else if(tag == m_tagList[ OVERSPEED_NAME])
	{
		if(m_overSpeedActive)	   retVal = "1";
		else						retVal = "0";
	}
	// If client is requesting current array index
	else if(tag == m_tagList[ TAG_IDX_NAME])
	{
		const int       sampleSize = (m_icmIps.size() == 1 ? 4 : 6);
		unsigned long   currentIndex = m_arrayIndex / sampleSize;

		itoa( currentIndex, reply, 10);
		retVal = reply;
	}
	else if(tag == m_tagList[ SAMPLE_DELTA_NAME])
	{
		itoa( m_sampleDelta, reply, 10);
		retVal = reply;
	}
	else if(!tag.compare(m_tagList[LF_ROLL_FACTOR_NAME]))
	{	// Get the left front roller factor
		itoa((int)m_rollAttribs[0].rollerFactor[0], reply, 10);
		retVal = reply;
	}
	else if(!tag.compare(m_tagList[RF_ROLL_FACTOR_NAME]))
	{	// Get the left front roller factor
		itoa((int)m_rollAttribs[0].rollerFactor[1], reply, 10);
		retVal = reply;
	}
	else if(!tag.compare(m_tagList[LR_ROLL_FACTOR_NAME]))
	{	// Get the left front roller factor
		itoa((int)m_rollAttribs[0].rollerFactor[2], reply, 10);
		retVal = reply;
	}
	else if(!tag.compare(m_tagList[RR_ROLL_FACTOR_NAME]))
	{	// Get the left front roller factor
		itoa((int)m_rollAttribs[0].rollerFactor[3], reply, 10);
		retVal = reply;
	}
	else if(!tag.compare(m_tagList[LT_ROLL_FACTOR_NAME]))
	{	// Get the left front roller factor
		itoa((int)m_rollAttribs[1].rollerFactor[0], reply, 10);
		retVal = reply;
	}
	else if(!tag.compare(m_tagList[RT_ROLL_FACTOR_NAME]))
	{	// Get the left front roller factor
		itoa((int)m_rollAttribs[1].rollerFactor[1], reply, 10);
		retVal = reply;
	}
	else
	{
		retVal = ResManagerXml::Read( node, rate);
	}
	Log( LOG_FN_ENTRY, "Exit IcmMgr::Read( %s, %d) = %s\n", tag.c_str(), rate,
		 retVal.c_str());

	return( retVal);
}

/**
 * Request to write a data tag
 *
 * @param node  Data to be written
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 * @since Version 2.0
 */
const std::string IpIcmMgr::Write(const XmlNode *node)
{
	const std::string tag( node->getName()), value( node->getValue());
	string      retVal(BEP_SUCCESS_RESPONSE);
	uint16_t    ipIdx, chnlIdx=ICM_CHANNEL_COUNT;
	uint32_t    chnlIdxKey;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::Write( %s, %s)\n", tag.c_str(), value.c_str());
	if(tag == m_tagList[ SAMPLE_DELTA_NAME])
	{
		m_sampleDelta = atoi( value.c_str());
	}
	else if( tag == m_tagList[ OVERSPEED_NAME])
	{
		m_currentOverSpeedVal = atof(value.c_str());
	}
	else
	{
		string chnlName = tag.substr( 0, 2);
		string subChnlTag = tag.substr( 2, tag.length() - 2);

		Log( LOG_DETAILED_DATA, "\tChannel Name: %s\n", chnlName.c_str());
		Log( LOG_DETAILED_DATA, "\t    Item Tag: %s\n", subChnlTag.c_str());

		if(chnlName == m_tagList[ m_channelNames[ LF_IDX]])		   chnlIdxKey = LF_IDX;
		else if(chnlName == m_tagList[ m_channelNames[ RF_IDX]])   chnlIdxKey = RF_IDX;
		else if(chnlName == m_tagList[ m_channelNames[ LR_IDX]])   chnlIdxKey = LR_IDX;
		else if(chnlName == m_tagList[ m_channelNames[ RR_IDX]])   chnlIdxKey = RR_IDX;
		else if(chnlName == m_tagList[ m_channelNames[ LT_IDX]])   chnlIdxKey = LT_IDX;
		else if(chnlName == m_tagList[ m_channelNames[ RT_IDX]])   chnlIdxKey = RT_IDX;

		if(chnlIdxKey < ICM_CHANNEL_COUNT)
		{
			DecodeChnlIndexKey( m_channelIdxMap[ chnlIdxKey], ipIdx, chnlIdx);

			Log( LOG_DETAILED_DATA, "\t   Key Index: %d\n", chnlIdxKey);
			Log( LOG_DETAILED_DATA, "\t Channel Key: %d\n", m_channelIdxMap[ chnlIdxKey]);
			Log( LOG_DETAILED_DATA, "\t Board Index: %d\n", ipIdx);
			Log( LOG_DETAILED_DATA, "\t     Channel: %d\n", chnlIdx);

			if(subChnlTag == "Loss")		   m_convFactors[ ipIdx].offset[ chnlIdx].force = atof( value.c_str());
			else if(subChnlTag == "Kspeed")	   m_convFactors[ ipIdx].scale[ chnlIdx].speed = atof( value.c_str());
			else if(subChnlTag == "Kaccel")	   m_convFactors[ ipIdx].scale[ chnlIdx].accel = atof( value.c_str());
			else if(subChnlTag == "Kforce")	   m_convFactors[ ipIdx].scale[ chnlIdx].force = atof( value.c_str());
			else								Log( LOG_ERRORS, "Invalid data name '%s' for writing\n", subChnlTag.c_str());

			if(subChnlTag == "Loss")		   WriteChannelLoss( ipIdx, chnlIdx, m_convFactors[ ipIdx].offset[ chnlIdx].force);
		}
		else
		{
			Log( LOG_ERRORS, "Invalid channel name '%s' for writing\n", chnlName.c_str());
		}
	}
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::Write( %s, %s)\n", tag.c_str(), value.c_str());

	return( retVal);
}

/**
 * Starts the pseudo-ISR sampling thread used to sample data from
 * the ICM IP modules
 */
void IpIcmMgr::AttachIsr()
{
#if USE_ISR
	uint32_t    jj, kk, mm = 0;
	float       dummyData;
	InstDataArrayItr_t  itr;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::AttachIsr()\n");

	if(m_demoMode)
	{
		Log( LOG_DEV_DATA, "Generating dummy data for demo mode\n");

		// Re-seed the random number generator
		srand( time(NULL));

		for(itr=m_instData.begin(); itr!=m_instData.end(); itr++)
		{
			IcmDataSample_t &dataSample = *itr;

			for(jj=0; jj<4; jj++)
			{
				for(kk=DIST_INDEX; kk<ICM_SUBCHNL_CNT; kk++)
				{
					dummyData = (float)(mm)  + (float)(rand() % 1000) / 1000.0;
					switch(kk)
					{
					case DIST_INDEX:        dataSample.data[ jj].distance = dummyData;  break;
					case FREQ_INDEX:        dataSample.data[ jj].freq = dummyData;      break;
					case SPEED_INDEX:       dataSample.data[ jj].speed = dummyData;     break;
					case ACCEL_INDEX:       dataSample.data[ jj].accel = dummyData;     break;
					case FORCE_INDEX:       dataSample.data[ jj].force = dummyData;     break;
					default:                break;
					}
				}
			}
		}
	}
	else
	{
		uint64_t    timerInterval = mSEC_nSEC( 1000);

		// Initialize the event to use for update notifications
		SIGEV_PULSE_INIT( &m_newDataEvent, ResManagerBase::m_pulseSigCoId,
						  SIGEV_PULSE_PRIO_INHERIT, ICM_PULSE_CODE, ICM_TIMER_PULSE);

		// Attach our interrupt event
		m_irqId = InterruptAttachEvent( m_pci40Card.GetPicIrq(), &m_newDataEvent, 0);
		if(m_irqId == -1)
		{
			Log( LOG_ERRORS, "\tInterruptAttachEvent failed: %s\n", strerror( errno));
			return;
		}
		else
		{
			Log( LOG_DEV_DATA, "Attached to interrupt %d\n",m_pci40Card.GetPicIrq());
		}

		IpIcmListItr_t  ipItr;
		// Check to see if the interrupts are being generated by our IP ICMs
		for(ipItr=m_icmIps.begin(); ipItr!=m_icmIps.end(); ipItr++)
		{
			IpIcm *ipIcm = *ipItr;
			// Enable interrupts
			ipIcm->EnableIrq( 0x10 + ipIcm->ModuleIndex(), m_samplePeriod);
		}
		m_pci40Card.EnableIrq();

		// Initialize the event to use for ISR timeout notifications
		SIGEV_PULSE_INIT( &m_watchdogEvent, ResManagerBase::m_pulseSigCoId,
						  SIGEV_PULSE_PRIO_INHERIT, ICM_PULSE_CODE, ICM_TIMER_PULSE+1);

		// Start our watchdof timer
		m_isrWatchdog.Start( timerInterval, &m_watchdogEvent);
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::AttachIsr()\n");
#else


	// Initialize the event to use for update notifications
	SIGEV_PULSE_INIT( &m_newDataEvent, m_mssgCoid, 60, ICM_PULSE_CODE, ICM_TIMER_PULSE);

	uint64_t    timerRate = mSEC_nSEC( m_samplePeriod);
	m_pseudoIsrTimer.Start( timerRate, timerRate, &m_newDataEvent);

#endif
}

/**
 * Detaches our interupt handler and disables interrupts from the
 * ICM modules
 */
void IpIcmMgr::DetachIsr()
{
#if USE_ISR
	IpIcmListItr_t  ipItr;
	uint32_t        ipIdx=0;

	printf( "\n\n\tEnter IpIcmMgr::DetachIsr\n");

	// Check to see if the interrupts are being generated by our IP ICMs
	for(ipItr=m_icmIps.begin(); ipItr!=m_icmIps.end(); ipItr++)
	{
		IpIcm *ipIcm = *ipItr;

		Log( "\tDisableIrq for ICM %d\n", ipIdx);
		// Disable interrupts
		ipIcm->DisableIrq();
		Log( "\tipIcm %d IRQ disabled\n", ipIdx);

		ipIdx++;
	}
	// Wait for any extraneous interrupts to come in
	delay( 100);

	Log( "\tDisableIrq for PCI-40\n");
	m_pci40Card.DisableIrq();
	Log( "\tPCI-40 IRQ disabled\n");

	// Wait for any extraneous interrupts to come in
	delay( 100);

	// No more interrupt pulses
	if(m_irqId != -1)
	{
		Log( "\tDetach handler\n");
		InterruptDetach( m_irqId);
		m_irqId = -1;
		Log( "\tHandler detached\n");
	}

	printf( "\n\n\tExit IpIcmMgr::DetachIsr\n\n");
#else

	m_pseudoIsrTimer.Stop();
#endif
}

/**
 * Fills in an XmlNode with the data value for a data item
 *
 * @param dataItem Properties for the data item to read
 * @param value    String to fill in with the value of the requested data item
 * @return 0 if valueNode is filled in
 *         -1 if unable to handle the request
 */
int IpIcmMgr::ReadDataItem(const dataMapItem_t *dataItem, string &value)
{
	int             retVal = -1;
	char            val[ 256];
	const uint32_t  brdIdx = dataItem->boardIdx,
	chnlIdx = dataItem->channelIdx,
	subChnlIdx = dataItem->bitIdx;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::ReadDataItem()\n");
	// If the specified channel/sub-channel is valid
	if((chnlIdx < MAX_CHANNELS) && (subChnlIdx < ICM_SUBCHNL_CNT))
	{
		// If specified board is valid
		if(brdIdx < m_instData.size())
		{
			if(m_dataLock.Acquire() == EOK)
			{
				const IcmDataSample_t   &dataSample = m_instData[ brdIdx];
				const IcmChannelConv_t  &conv = m_convFactors[ brdIdx];
				float                   icmData;

				retVal = 0;
				switch(subChnlIdx)
				{
				case DIST_INDEX:        icmData = dataSample.data[ chnlIdx].distance;   break;
				case FREQ_INDEX:        icmData = dataSample.data[ chnlIdx].freq;       break;
				case SPEED_INDEX:       icmData = dataSample.data[ chnlIdx].speed;      break;
				case ACCEL_INDEX:       icmData = dataSample.data[ chnlIdx].accel;      break;
				case FORCE_INDEX:       icmData = dataSample.data[ chnlIdx].force;      break;
				case SPEED_K_INDEX:     icmData = conv.scale[ chnlIdx].speed;           break;
				case ACCEL_K_INDEX:     icmData = conv.scale[ chnlIdx].accel;           break;
				case FORCE_K_INDEX:     icmData = conv.scale[ chnlIdx].force;           break;
				case LOSS_INDEX:        icmData = conv.offset[ chnlIdx].force;          break;
				default:                retVal = -1;                                    break;
				}

				// Translate board/channel to array index
				Log( LOG_DEV_DATA, "Channel %d, sub %d = %f\n", chnlIdx, subChnlIdx, icmData);
				// Put value into XML node's value
//              sprintf( val, "%7.3f", icmData);
				sprintf( val, "%8.4f", icmData);
				value = string( val);

				m_dataLock.Release();
			}
		}
	}
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::ReadDataItem()\n");

	return( retVal);
}

/**
 * Reads a data item's value from an XmlNode
 *
 * @param dataItem Properties for the data item to write to
 * @param value    String containing the value of the specified data item
 * @return 0 if new value was written
 *         -1 if unable to handle the request
 */
int IpIcmMgr::WriteDataItem(const dataMapItem_t *dataItem, const string &value)
{
	int             retVal = -1;
	float           dataVal;
	const uint32_t  brdIdx = dataItem->boardIdx,
	chnlIdx = dataItem->channelIdx,
	subChnlIdx = dataItem->bitIdx;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::WriteDataItem()\n");
	// If the specified channel/sub-channel is valid
	if((chnlIdx < MAX_CHANNELS) && (subChnlIdx < ICM_SUBCHNL_CNT))
	{
		// If specified board is valid and port is an output
		if((brdIdx < m_icmIps.size()) && (dataItem->portDir & PORT_DIR_OUT))
		{
			// Read value from the XML node
			dataVal = atof( value.c_str());
			if(m_dataLock.Acquire() == EOK)
			{
				IcmChannelData_t    &dataSample = m_instData[ brdIdx].data[ chnlIdx];
				IcmChannelConst_t   &conv = m_convFactors[ brdIdx].scale[ chnlIdx];

				// If we are this far, the indices are OK
				retVal = 0;
				try
				{
					switch(subChnlIdx)
					{
					case DIST_INDEX:        dataSample.distance = dataVal;                  break;
					case FREQ_INDEX:        dataSample.freq = dataVal;                      break;
					case SPEED_INDEX:       dataSample.speed = dataVal;                     break;
					case ACCEL_INDEX:       dataSample.accel = dataVal;                     break;
					case FORCE_INDEX:       dataSample.force = dataVal;                     break;
					case SPEED_K_INDEX:     conv.speed = dataVal;                           break;
					case ACCEL_K_INDEX:     conv.accel = dataVal;                           break;
					case FORCE_K_INDEX:     conv.force = dataVal;                           break;
					case LOSS_INDEX:        WriteChannelLoss( brdIdx, chnlIdx, dataVal);    break;
					case COUNT_INDEX:       dataSample.count = (uint32_t)dataVal;           break;
					default:                retVal = -1;                                    break;
					}
				}
				catch(BepException &err)
				{
					Log( LOG_ERRORS, "\tCaught BepException: %s\n", err.GetReason());
					retVal = -1;
				}
				m_dataLock.Release();
			}
		}
	}
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::WriteDataItem()\n");

	return( retVal);
}

/**
 * Method used to attach pulses to the resource manager's I/O
 * channel
 *
 * @return -1 on error, any other value indicates success
 */
int IpIcmMgr::AttachIoPulses()
{
	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::AttachIoPulses()\n");

	if(ResManagerBase::AttachIoPulses() == 0)
	{
		AttachIoPulse( m_newDataEvent.sigev_code);
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::AttachIoPulses()\n");
	return( 0);
}

/**
 * Handles pulses delivered to our resource manager I/O channel
 *
 * @param pulse  Pulse received on the I/O channel
 * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
 *         RES_MGR_CMD_CONFIG if a reload config pulse was received
 *         RES_MGR_CMD_NONE for all other pulses
 */
ResManagerCmd_t IpIcmMgr::HandleResMgrPulse(io_pulse_t &pulse)
{
	ResManagerCmd_t retVal = RES_MGR_CMD_NONE;
	uint64_t        timerInterval = mSEC_nSEC( 1000);

	Log( 0x80000000, "Enter IpIcmMgr::HandleResMgrPulse( %d, %d)\n", pulse.code, pulse.value.sival_int);
	switch(pulse.code)
	{
	case ICM_PULSE_CODE:
		switch(pulse.value.sival_int)
		{
		// new data is avaailable
		case ICM_TIMER_PULSE:
			ProcessIsr();
			m_isrWatchdog.Start( timerInterval, &m_watchdogEvent);
			break;
		case ICM_TIMER_PULSE + 1:
			Log( LOG_ERRORS, "\tISR Watchdog expired\n");
			// new data is avaailable
			DetachIsr();
			AttachIsr();
			break;
		default:
			break;
		}
		break;
	default:
		retVal = ResManagerBase::HandleResMgrPulse(pulse);
		break;

	}
	Log( 0x80000000, "Exit IpIcmMgr::HandleResMgrPulse( %d, %d)\n", pulse.code, pulse.value.sival_int);
	return( retVal);
}

/**
 * Process new data from the ICM after an interrupt event
 */
void IpIcmMgr::ProcessIsr()
{
	register int        brdIdx, chnlIdx;
	bool                zspd = true;
	bool                ovrspd = false;

#if USE_ISR
	IpIcmListItr_t      itr;
	IpIcm               *ipIcm;
	uint8_t             ipIrqMask, ipMask;

	// if our IP carrier does not have an active interrupt
	if((ipIrqMask=m_pci40Card.GetActiveIpIrqs()) == 0)
	{
		// Go wait some more
		InterruptUnmask( m_pci40Card.GetPicIrq(), m_irqId);
	}
	else
	{
		// Check to see if the interrupts are being generated by our IP ICMs
		for(itr=m_icmIps.begin(); itr!=m_icmIps.end(); itr++)
		{
			ipIcm = *itr;
			ipMask = 0x11 << ipIcm->ModuleIndex();
			// IF this ICM is generating an interrupt
			if(ipMask & ipIrqMask)
			{
				// get all data
				SampleData( ipIcm);

				ipIcm->AckIrq();

				// Stop the ISR watchdog because we got an ICM interrupt
				m_isrWatchdog.Stop();
			}
		}

		InterruptUnmask( m_pci40Card.GetPicIrq(), m_irqId);

		m_isrFifo.Lock();

		// Go calculate values
		ProcessData( m_icmIps);

		// Copy latest samples to instantaneous data
		for(brdIdx=0; brdIdx<(int)m_icmIps.size(); brdIdx++)
		{
			IcmDataSample_t &dataSample = m_instData[ brdIdx];
			IcmSamples_t    &isrData = m_isrFifo[ brdIdx];

			for(chnlIdx=0; chnlIdx<MAX_CHANNELS; chnlIdx++)
			{
				dataSample.data[ chnlIdx] = isrData.avgValues[ chnlIdx];

				// Check for zerospeed conditions
				// If no wheel above zerospeed yet and this wheel is above zerospeed threshold
				if((zspd == true) && (fabs( dataSample.data[ chnlIdx].speed) > m_zeroSpeedVal))
				{
					// Say not at zerospeed
					zspd = false;
				}

				// If this wheel is above Overspeed value and
				//  overspeed has valid value
				if((ovrspd == false) && (fabs( dataSample.data[ chnlIdx].speed) > m_currentOverSpeedVal) &&
				   (m_currentOverSpeedVal > 0))
				{
					ovrspd = true;
				}

			}
		}
		m_isrFifo.Unlock();

		// Check for Zerospeed conditions
		WriteZerospeed( zspd);

		// Check overspeed conditions
		WriteOverspeed( ovrspd);

		// Write the current wheel speed values to the PLC
		WritePlcWheelSpeeds();

		// Copy latest samples to arrays
		UpdateArrays();

		// Update the data logs used for the GUI graphs
		LogAsciiData();
	}
#else

	SampleData( m_icmIps);

	m_isrFifo.Lock();

	// Go calculate values
	ProcessData( m_icmIps);

	// Copy latest samples to instantaneous data
	for(brdIdx=0; brdIdx<(int)m_icmIps.size(); brdIdx++)
	{
		IcmDataSample_t &dataSample = m_instData[ brdIdx];
		IcmSamples_t    &isrData = m_isrFifo[ brdIdx];

		for(chnlIdx=0; chnlIdx<MAX_CHANNELS; chnlIdx++)
		{
			dataSample.data[ chnlIdx] = isrData.avgValues[ chnlIdx];

			// Check for zerospeed conditions
			// If no wheel above zerospeed yet and this wheel is above zerospeed threshold
			if((zspd == true) && (fabs( dataSample.data[ chnlIdx].speed) > m_zeroSpeedVal))
			{
				// Say not at zerospeed
				zspd = false;
			}

			// If this wheel is above Overspeed value and
			//  overspeed has valid value
			if((ovrspd == false) && (fabs( dataSample.data[ chnlIdx].speed) > m_currentOverSpeedVal) &&
			   (m_currentOverSpeedVal > 0))
			{
				ovrspd = true;
			}

		}
	}
	m_isrFifo.Unlock();

	// Check for Zerospeed conditions
	WriteZerospeed( zspd);

	// Check overspeed conditions
	WriteOverspeed( ovrspd);

	// Write the current wheel speed values to the PLC
	WritePlcWheelSpeeds();

	// Check for over-acceleration
	CheckAccelLimit();

	// Copy latest samples to arrays
	UpdateArrays();

	// Update the data logs used for the GUI graphs
	LogAsciiData();
#endif
}

/**
 * Reads the time, count and direction values for each ICM in the
 * icmModList
 *
 * @param icmModList List of ICM objects to read data from
 */
void IpIcmMgr::SampleData( IpIcmList_t &icmModList)
{
	IpIcm               *ipIcm;
	IpIcmListItr_t      itr;

	// Read all data into locals and compute
	for(itr=icmModList.begin(); itr!=icmModList.end(); itr++)
	{
		ipIcm = *itr;
		if(ipIcm != NULL)
		{
			SampleData( ipIcm);
		}
	}
}

/**
 * Reads the time, count and direction values from the given ICM
 *
 * @param ipIcm  ICM to read data from
 */
void IpIcmMgr::SampleData( IpIcm *ipIcm)
{
	int         chnl;
	int         ipIdx = ipIcm->ModuleIndex();
	const int   arrayIdx = m_isrFifo[ ipIdx].head;
	uint32_t    count, dir, time;
	float       freq;
	int         dist;

	// If not using interrupts to latch the data
	if(m_irqId == -1)
	{
		// Manually latch the ICM data
		ipIcm->LatchChannelValues( -1);
	}

	// Sampel each channel on this board
	for(chnl=0; chnl<MAX_CHANNELS; chnl++)
	{
		// Read this channel's data
		ipIcm->ReadWheelValues( chnl, freq, dist, dir, time);
		ipIcm->ReadChannelValues( chnl, time, count);

		m_isrFifo[ ipIdx].isrSamples[ chnl][ arrayIdx].count = count;
		m_isrFifo[ ipIdx].isrSamples[ chnl][ arrayIdx].time = time;
		m_isrFifo[ ipIdx].isrSamples[ chnl][ arrayIdx].direction = dir;
		m_isrFifo[ ipIdx].isrSamples[ chnl][ arrayIdx].freq = freq;
		m_isrFifo[ ipIdx].isrSamples[ chnl][ arrayIdx].distance = dist;
		m_isrFifo[ ipIdx].isrSamples[ chnl][ arrayIdx].direction = dir;
		m_isrFifo[ ipIdx].isrSamples[ chnl][ arrayIdx].freq = freq;
		m_isrFifo[ ipIdx].isrSamples[ chnl][ arrayIdx].distance = dist;
	}
	m_isrFifo[ ipIdx].head = (m_isrFifo[ ipIdx].head + 1) % ISR_FIFO_SZ;
}

/**
 * Process data after the ISR has signalled that more samples are
 * available
 *
 * @param icmModList list of pointers to IpIcm objects to read data from
 */
void IpIcmMgr::ProcessData( IpIcmList_t &icmModList)
{
	uint32_t            head, tail1, tail2, ipIdx, chnl;
	IcmChannelData_t    *sample[ 2];
	IpIcmListCItr_t     itr;

	Log( 0x10000000, "Enter IpIcmMgr::ProcessData()\n");
	// Calculate data for each of the modules installed
	ipIdx = 0;
	for(itr=icmModList.begin(); itr!=icmModList.end(); itr++)
	{
		// Sample the ISR indices
		head = m_isrFifo[ ipIdx].head;
		tail1 = m_isrFifo[ ipIdx].tail;
		tail2 = (tail1 + 1) % ISR_FIFO_SZ;

		// Process all samples since last time
		while(tail2 != head)
		{
			// Calculate data for each channel on this module
			for(chnl=0; chnl<MAX_CHANNELS; chnl++)
			{
				sample[ 0] = &m_isrFifo[ ipIdx].isrSamples[ chnl][ tail1];
				sample[ 1] = &m_isrFifo[ ipIdx].isrSamples[ chnl][ tail2];

				// Calculate instantaneous values
				CalculateInstData( sample[ 0], sample[ 1], ipIdx, chnl);

				m_isrFifo[ ipIdx].avgValues[ chnl] = *sample[ 0];
				CalculateAvgData( m_isrFifo[ ipIdx].avgValues[ chnl], ipIdx, chnl, tail1);
			}
			m_isrFifo[ ipIdx].tail = tail1 = tail2;

			tail2 = (tail2 + 1) % ISR_FIFO_SZ;
		}
		ipIdx++;
	}
	Log( 0x10000000, "Exit IpIcmMgr::ProcessData()\n");
}

/**
 * Calculates instantaneous channel values (distance, speed, freq, etc) using adjacent data samples
 *
 * @param sample0 Sample just prior to sample0
 * @param sample1 Most recent data sample
 * @param ipIdx   Index of the IP module we are performing calculations on
 * @param chnlIdx The index of the channel we are calculating data for
 */
void IpIcmMgr::CalculateInstData( IcmChannelData_t *sample0, IcmChannelData_t *sample1, uint16_t ipIdx, uint16_t chnlIdx)
{
	const IcmChannelConv_t  &conv = m_convFactors[ ipIdx];
	float       freqDiff, forceOffset = conv.offset[ chnlIdx].force;

	// change in frequency
	freqDiff = sample1->freq - sample0->freq;

	// wheel speed
	sample1->speed = sample1->freq * conv.scale[ chnlIdx].speed + conv.offset[ chnlIdx].speed;

	// accumulated distance
	sample1->distance = (int)(sample1->freq * conv.scale[ chnlIdx].distance + conv.offset[ chnlIdx].distance);

	// Acceleration
	sample1->accel = freqDiff * conv.scale[ chnlIdx].accel + conv.offset[ chnlIdx].accel;

	// brake force
	sample1->force = freqDiff * conv.scale[ chnlIdx].force - forceOffset - m_currDrags[ ipIdx*MAX_CHANNELS+chnlIdx];
}

/**
 * Calculates average values for wheel data
 *
 * @param avg       Sample to store the averaged data in
 * @param ipIdx     The index of the IP ICM we are averaging data from
 * @param chnlIdx   The channel on the ICM we are averaging data from
 * @param sampleIdx The lates sample index to use for calculating average values
 */
void IpIcmMgr::CalculateAvgData( IcmChannelData_t &avg, int ipIdx, int chnlIdx, int sampleIdx)
{
	int     ii, jj = sampleIdx-m_sampleDelta;

	if(jj < 0) jj += ISR_FIFO_SZ;

	avg.accel = avg.force = avg.speed = 0;
	for(ii=0; ii<m_sampleDelta; ii++)
	{
		avg.accel += m_isrFifo[ ipIdx].isrSamples[ chnlIdx][ jj].accel;
		avg.force += m_isrFifo[ ipIdx].isrSamples[ chnlIdx][ jj].force;
		avg.speed += m_isrFifo[ ipIdx].isrSamples[ chnlIdx][ jj].speed;

		jj = (jj + 1) % ISR_FIFO_SZ;
	}

	if(m_disableSystem)
	{
		avg.accel = 0;
		avg.force = 0;
	}
	else
	{
		avg.accel /= m_sampleDelta;
		avg.force /= m_sampleDelta;
	}
	avg.speed /= m_sampleDelta;
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
int IpIcmMgr::IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
	IOFUNC_ATTR_T   *ioAttr = NULL;
	off_t           off, nbytes;
	int             retVal=ENOSYS, maxInode;
	// Make sure configs are not being reloaded
	if(!m_reloadingConfigs)
	{
		// Make sure we have a valid control block
		if(ioOcb != NULL)
		{
			ioAttr = (IOFUNC_ATTR_T*)(ioOcb->m_ioOcb.attr);
		}
		else
		{
			Log(LOG_ERRORS, "IpIcmMgr::IoRead() - ioOcb is NULL!");
		}

		if((ioAttr != NULL) && (msg != NULL))
		{
			Log( LOG_FN_ENTRY, "Enter IpIcmMgr::IoRead(), inode %lld\n", ioAttr->inode);
			// Check if this is  an XTYPE override
			switch(msg->i.xtype & _IO_XTYPE_MASK)
			{
			case _IO_XTYPE_OFFSET:
				Log(LOG_DEV_DATA, "IpIcmMgr::IoRead() handling XTYPE override");
				struct _xtype_offset *xoff;
				xoff = (struct _xtype_offset *)((&(msg->i)) + 1);
				if(xoff != NULL)
				{
					off = xoff->offset;
					retVal = EOK;
				}
				else
				{
					retVal = -1;
				}
				break;
			case _IO_XTYPE_NONE:
				off = ioOcb->m_ioOcb.offset;
				retVal = EOK;
				break;
			default:
				break;
			}

			// Calculate the highest valid inode value
			maxInode = NumberOfInstantaneousDevices() +
					   NumberOfArrayDevices() +
					   NumberOfBlockDevices() +
					   m_baseInstInode;

			// Make sure data doesn't change while we're reading
			if((retVal=m_dataLock.Acquire()) == EOK)
			{
				// We don't support extended types
				if(( retVal == EOK) && ( ioAttr->inode < maxInode) && (ctp != NULL))
				{
					// If this read refers to an array data item
					if(IsBlockDevice(ioOcb))	   nbytes = ReadBlockData( ctp, msg, ioOcb);
					else if(IsArrayDevice( ioOcb)) nbytes = ReadArrayData( ctp, msg, ioOcb);
					else						   nbytes = ReadInstData( ctp, msg, ioOcb);

					if(nbytes)
					{
						// update flags and offset
						ioAttr->flags |= IOFUNC_ATTR_ATIME | IOFUNC_ATTR_DIRTY_TIME;

						// Advance client's offset if not a combine message
						if((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_OFFSET)
						{
							ioOcb->m_ioOcb.offset += nbytes;
						}
					}
					// Say we read this many bytes
					_IO_SET_READ_NBYTES( ctp, nbytes);

					// already done the reply ourselves
					retVal = _RESMGR_NOREPLY;
				}
				else
				{
					if(ioAttr->inode > maxInode)
					{
						Log( LOG_ERRORS, "\tInvalid inode %lld for reading\n", ioAttr->inode);
					}
				}

				m_dataLock.Release();
			}
			else
			{
				Log( LOG_ERRORS, "\tError acquiring data lock in IoRead: %s\n", strerror( retVal));
				retVal = _RESMGR_ERRNO( retVal);
			}
		}
		else
		{
			Log(LOG_ERRORS, "IpIcmMgr::IoRead - ioAttr is NULL, skipping IoRead!");
		}
	}
	else
	{
		Log(LOG_ERRORS, "Reloading configs, not performing IoRead()");
		retVal = EOK;
	}
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::IoRead\n");

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
int IpIcmMgr::IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
	IOFUNC_ATTR_T   *ioAttr = (IOFUNC_ATTR_T*)(ioOcb->m_ioOcb.attr);
	off_t           off, nbytes;
	int             maxInode, retVal = ENOSYS;
	int             dataOffset=sizeof( msg->i);

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::IoWrite()\n", ioAttr->inode);

	// Calculate the highest valid inode value
//  maxInode = NumberOfInstantaneousDevices() + NumberOfArrayDevices() + NumberOfBlockDevices() + 1;
	maxInode = NumberOfInstantaneousDevices() +
			   NumberOfArrayDevices() +
			   NumberOfBlockDevices() +
			   m_baseInstInode;

	// Check if this is  an XTYPE override
	switch(msg->i.xtype & _IO_XTYPE_MASK)
	{
	case _IO_XTYPE_OFFSET:
		struct _xtype_offset *xoff;
		xoff = (struct _xtype_offset *)(&msg->i + 1);
		off = xoff->offset;
		dataOffset += sizeof( *xoff);
		retVal = EOK;
		break;
	case _IO_XTYPE_NONE:
		off = ioOcb->m_ioOcb.offset;
		retVal = EOK;
		break;
	default:
		break;
	}

	// If ok to do the write operation
	if((retVal == EOK) && (ioAttr->inode < maxInode))
	{
		// If this write refers to an array data item
		if(IsBlockDevice(ioOcb))		nbytes = WriteBlockData( ctp, msg, ioOcb);
		else							nbytes = WriteInstData( ctp, msg, ioOcb);

		if(nbytes)
		{
			// update flags and offset
			ioAttr->flags |= IOFUNC_ATTR_ATIME | IOFUNC_ATTR_DIRTY_TIME;

			// Advance client's offset if not a combine message
			if((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_OFFSET)
			{
				ioOcb->m_ioOcb.offset += nbytes;
			}
		}

		// Tell the client how many bytes were written
		_IO_SET_WRITE_NBYTES( ctp, nbytes);

		// already done the reply ourselves
		retVal = EOK;

#if 0
		// Calculate array offsets
		int inode = ioAttr->inode;
		// Calculate which data item is being read
		int brdIdx = ((inode-m_baseInstInode) / ICM_SUBCHNL_CNT) / ICM_CHNL_BRD;
		int chnlIdx = ((inode-m_baseInstInode) / ICM_SUBCHNL_CNT) % ICM_CHNL_BRD;
		int subIdx = (inode-m_baseInstInode) % ICM_SUBCHNL_CNT;

		if((subIdx == SPEED_K_INDEX) || (subIdx == ACCEL_K_INDEX) ||
		   (subIdx == FORCE_K_INDEX) || (subIdx == LOSS_INDEX) ||
		   (subIdx == DIST_INDEX))
		{
			// Calculate how much "buffer" is left
			nleft = ioAttr->nbytes - off;
			nbytes = min(  nleft, msg->i.nbytes);
			// If room left in buffer and valid data being written
			if(nbytes == sizeof( float))
			{
				// Read rest of the write message data
				resmgr_msgread( ctp, &dataVal, nbytes, dataOffset);

				// Tell kernel to update last modification time
				((iofunc_attr_t*)(ioOcb->m_ioOcb.attr))->flags |= IOFUNC_ATTR_MTIME |
																  IOFUNC_ATTR_DIRTY_TIME;

				// Advance client's offset if not a combine message
				if((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_OFFSET)
				{
					ioOcb->m_ioOcb.offset += nbytes;
				}

				// Assume OK to write
				retVal = EOK;

				// Write new data to VME space
				switch(subIdx)
				{
				case SPEED_K_INDEX: m_convFactors[ brdIdx].scale[ chnlIdx].speed = dataVal; break;
				case ACCEL_K_INDEX: m_convFactors[ brdIdx].scale[ chnlIdx].accel = dataVal; break;
				case FORCE_K_INDEX: m_convFactors[ brdIdx].scale[ chnlIdx].force = dataVal; break;
				case LOSS_INDEX:    WriteChannelLoss( brdIdx, chnlIdx, dataVal);            break;
				case DIST_INDEX:
					// IF writing 0 to a distance sub-channel
					if(dataVal == 0)
					{
						// Reset distance values on each of the IP ICMs
						IpIcmListItr_t  itr;
						for(itr=m_icmIps.begin(); itr!=m_icmIps.end(); itr++)
						{
							IpIcm   *ipIcm = *itr;
							ipIcm->ClearDistance( -1);
						}
					}
					else
					{
						retVal = EINVAL;
						nbytes = 0;
					}
					break;
					// Shouldn't get these- they're read only
				case FREQ_INDEX:
				case SPEED_INDEX:
				case ACCEL_INDEX:
				case FORCE_INDEX:
				default:
					nbytes = 0;
					retVal = EROFS;
					break;
				}
			}

			// Tell the client how many bytes were written
			_IO_SET_WRITE_NBYTES( ctp, nbytes);

		}
		else
		{
			Log( LOG_ERRORS, "\nAttempt to write to invalid ICM path %d\n",
				 chnlIdx);
			retVal = EROFS;
		}
#endif
	}
	else
	{
		if(ioAttr->inode > maxInode)
		{
			Log( LOG_ERRORS, "\tInvalid inode %d for writing\n", ioAttr->inode);
		}
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::IoWrite\n");

	return( retVal);
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
int IpIcmMgr::IoStat(resmgr_context_t *ctp, io_stat_t *msg, resMgrIoOcb_t *ioOcb)
{
	IOFUNC_ATTR_T   *ioAttr = (IOFUNC_ATTR_T*)(ioOcb->m_ioOcb.attr);
	const int       sampleSize = (m_icmIps.size() == 1 ? 4 : 6);

	// If this stat refers to an array data item
	if(IsArrayDevice(ioOcb))
	{
		ioAttr->nbytes = m_arrayIndex * sizeof( float);
	}
	else if(IsBlockDevice(ioOcb))
	{
		ioAttr->nbytes = sampleSize * sizeof( float);
	}
	else
	{
		ioAttr->nbytes = 4;
		msg->o.st_blocksize = 4;
		msg->o.st_blksize = 4;
	}

	return( ResManagerBase::IoStat( ctp, msg, ioOcb));
}

/**
 * Initializes the named data broker interface object
 */
void IpIcmMgr::InitializeNdb()
{
	INamedDataBroker    *junk;
	string              ndbRsp, message;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::InitializeNdb()\n");

	if(m_dataBroker != NULL)   delete  m_dataBroker;
	m_dataBroker = NULL;

	try
	{
		junk = new INamedDataBroker( IsDebugOn());

		// Subscribe for start test/end test events for data logging
		(void)junk->Subscribe( m_tagList[ "StartTestTag"], GetProcessName(), ndbRsp, false);
		(void)junk->Subscribe( m_tagList[ "EndTestTag"], GetProcessName(), ndbRsp, true);
		(void)junk->Subscribe( m_tagList[ "LossCompensationInProgress"], GetProcessName(), ndbRsp, true);
	}
	catch(BepException &err)
	{
		junk = NULL;
	}
	// Do this (allocate into temporary and then assign the temp to our global) because
	//  of thread scheduling issues with when the memory (m_dataBroker) is 'non-null'
	//  and when it is actually valid to use
	m_dataBroker = junk;

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::InitializeNdb()\n");
}
/**
 * Initialize the local named broker with an already created
 * named data broker
 * Implemented for use in testing
 */
void IpIcmMgr::InitializeNdb(INamedDataBroker *dataBroker)
{
	string ndbRsp, message;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::InitializeNdb(INamedDataBroker)\n");

	if(m_dataBroker != NULL)   delete  m_dataBroker;
	m_dataBroker = NULL;

	// Do this (allocate into temporary and then assign the temp to our global) because
	//  of thread scheduling issues with when the memory (m_dataBroker) is 'non-null'
	//  and when it is actually valid to use
	m_dataBroker = dataBroker;

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::InitializeNdb(INamedDataBroker)\n");
}

/**
 * Checks if the passed OCB refers to a path for an instantaneous data item
 *
 * @param ioOcb  OCB to be checked for being an instantaneous data item
 * @return true if the OCB refers to an instantaneous data item, false otherwise
 */
bool IpIcmMgr::IsInstantaneousDevice( resMgrIoOcb_t *ioOcb)
{
	const ino_t     &inode = ioOcb->m_ioOcb.attr->inode;
	bool            retVal = false;

	if((inode >= m_baseInstInode) && (m_baseInstInode != -1) &&
	   (inode < m_baseArrayInode))
	{
		retVal = true;
	}
	return( retVal);
}

/**
 * Checks if the passed OCB refers to a path for an array data item
 *
 * @param ioOcb  OCB to be checked for being an array data item
 * @return true if the OCB refers to an array data item, false otherwise
 */
bool IpIcmMgr::IsArrayDevice( resMgrIoOcb_t *ioOcb)
{
	const ino_t     &inode = ioOcb->m_ioOcb.attr->inode;
	bool            retVal = false;

	if(inode >= m_baseArrayInode)
	{
		if((inode < m_baseBlockInode) || (m_baseBlockInode == -1))
		{
			retVal = true;
		}
	}
	return( retVal);
}

/**
 * Checks if the passed OCB refers to a path for a block data item
 *
 * @param ioOcb  OCB to be checked for being a block data item
 * @return true if the OCB refers to a block data item, false otherwise
 */
bool IpIcmMgr::IsBlockDevice( resMgrIoOcb_t *ioOcb)
{
	const ino_t     &inode = ioOcb->m_ioOcb.attr->inode;
	bool            retVal = false;

	if((inode >= m_baseBlockInode) && (m_baseBlockInode != -1))
	{
		retVal = true;
	}
	return( retVal);
}

/**
 * Opens an ascii data log file
 *
 * @param logStruct AsciiLogFile_t strusture describing the data log file to open
 */
void IpIcmMgr::OpenDataLog( AsciiLogFile_t &logStruct)
{
	// Remove the FTP link to the data log file
	unlink( logStruct.logFileLink.c_str());

	if((logStruct.logFile == NULL) && (logStruct.logFileName.empty() == false))
	{
		if((logStruct.logFile = fopen(logStruct.logFileName.c_str(), "rw")) == NULL)
		{
			Log( LOG_ERRORS, "\tError opening data log '%s': %s\n",
				 logStruct.logFileName.c_str(), strerror( errno));
		}
	}
}

/**
 * Creates the symbolic link required for the GUI to access a data log
 *
 * @param logStruct AsciiLogFile_t strusture describing the data log file to link
 */
void IpIcmMgr::UpdateLogLink( AsciiLogFile_t &logStruct)
{
	if(logStruct.logFile != NULL)
	{
		// Close data log file
		fclose( logStruct.logFile);

		// Create FTP link to data log file
		if(symlink( logStruct.logFileName.c_str(), logStruct.logFileLink.c_str()) == -1)
		{
			Log( LOG_ERRORS, "\tError creating data log '%s' link: %s\n",
				 logStruct.logFileLink.c_str(), strerror( errno));
		}
		logStruct.logFile = NULL;
	}
}

/**
 * Set cold default values for data members
 */
void IpIcmMgr::SetDefaults()
{
	// Initialize this here (even though the connection ID might not be valid yet)
	//  in oreder to fill in good values for the code/value of the pulse
	SIGEV_PULSE_INIT( &m_newDataEvent, ResManagerBase::m_pulseSigCoId,
					  SIGEV_PULSE_PRIO_INHERIT, ICM_PULSE_CODE, ICM_TIMER_PULSE);

	// Tags for the primary ICM channels
	m_channelNames.clear();
	m_channelNames.push_back( std::string( LF_CHANNEL_NAME));
	m_channelNames.push_back( std::string( LR_CHANNEL_NAME));
	m_channelNames.push_back( std::string( RF_CHANNEL_NAME));
	m_channelNames.push_back( std::string( RR_CHANNEL_NAME));
	m_channelNames.push_back( std::string( LT_CHANNEL_NAME));
	m_channelNames.push_back( std::string( RT_CHANNEL_NAME));

	// Tags for the indeividual elements in each ICM channel
	m_subChannels.clear();
	m_subChannels.push_back( IcmSubChannel( "Kspeed", O_RDWR));
	m_subChannels.push_back( IcmSubChannel( "Kaccel", O_RDWR));
	m_subChannels.push_back( IcmSubChannel( "Kforce", O_RDWR));
	m_subChannels.push_back( IcmSubChannel( "Distance", O_RDONLY));
	m_subChannels.push_back( IcmSubChannel( "Frequency", O_RDONLY));
	m_subChannels.push_back( IcmSubChannel( "Speed", O_RDONLY));
	m_subChannels.push_back( IcmSubChannel( "Acceleration", O_RDONLY));
	m_subChannels.push_back( IcmSubChannel( "Force", O_RDONLY));
	m_subChannels.push_back( IcmSubChannel( "Loss", O_RDWR));
	m_subChannels.push_back( IcmSubChannel( "PulseCount", O_RDONLY));

	// Path names to adopt for the data arrays
	m_dataArrayNames.clear();
	m_dataArrayNames.push_back( std::string( "ArrayDistance"));
	m_dataArrayNames.push_back( std::string( "ArraySpeed"));
	m_dataArrayNames.push_back( std::string( "ArrayAccel"));
	m_dataArrayNames.push_back( std::string( "ArrayForce"));
	m_dataArrayNames.push_back( std::string( "ArrayFrequency"));

	// Path names to adopt for block data
	m_blockDataNames.clear();
	m_blockDataNames.push_back( std::string( "Block/Speed"));
	m_blockDataNames.push_back( std::string( "Block/Acceleration"));
	m_blockDataNames.push_back( std::string( "Block/Force"));
	m_blockDataNames.push_back( std::string( "Block/Loss"));
	m_blockDataNames.push_back( std::string( "Block/PulseCount"));
	m_blockDataNames.push_back( std::string( "Block/Drag"));

	// Initialize default tag names
	m_tagList.Clear();
	m_tagList.AddName( LF_CHANNEL_NAME, "LF");
	m_tagList.AddName( RF_CHANNEL_NAME, "RF");
	m_tagList.AddName( LR_CHANNEL_NAME, "LR");
	m_tagList.AddName( RR_CHANNEL_NAME, "RR");
	m_tagList.AddName( LT_CHANNEL_NAME, "LT");
	m_tagList.AddName( RT_CHANNEL_NAME, "RT");
	m_tagList.AddName( ZEROSPEED_NAME, "Zerospeed");
	m_tagList.AddName( OVERSPEED_NAME, "Overspeed");
	m_tagList.AddName( "StartTestTag", "TestInProgress");
	m_tagList.AddName( "EndTestTag", "TestComplete");
	m_tagList.AddName( TAG_IDX_NAME, "TagIndex");
	m_tagList.AddName( ROLLER_COUNT_NAME, "RollerCount");
	m_tagList.AddName( ZEROSPEED_LATCH_NAME, "ZerospeedToPLC");
	m_tagList.AddName( LF_ROLL_DIR_NAME, "LfRollerDirection");
	m_tagList.AddName( RF_ROLL_DIR_NAME, "RfRollerDirection");
	m_tagList.AddName( LR_ROLL_DIR_NAME, "LrRollerDirection");
	m_tagList.AddName( RR_ROLL_DIR_NAME, "RrRollerDirection");
	m_tagList.AddName( LT_ROLL_DIR_NAME, "LtRollerDirection");
	m_tagList.AddName( RT_ROLL_DIR_NAME, "RtRollerDirection");
	m_tagList.AddName( SAMPLE_DELTA_NAME, "SampleDelta");
	m_tagList.AddName( ACCUM_TARGET_NAME, "AccumulatorTarget");
	m_tagList.AddName( PRESCALE_MODE_NAME, "PrescaleMode");
	m_tagList.AddName( MACH_3600_ID_NAME, "Machine3600Id");
	m_tagList.AddName( "LFPlcSpeed", "LFWheelSpeed");
	m_tagList.AddName( "RFPlcSpeed", "RFWheelSpeed");
	m_tagList.AddName( "LRPlcSpeed", "LRWheelSpeed");
	m_tagList.AddName( "RRPlcSpeed", "RRWheelSpeed");
	m_tagList.AddName( "LTPlcSpeed", "LTWheelSpeed");
	m_tagList.AddName( "RTPlcSpeed", "RTWheelSpeed");
	m_tagList.AddName( "AccelLimitFaultTag", "HornSignal");
	m_tagList.AddName( "LossCompensationInProgress", "LossComponsation");
	m_tagList.AddName( LF_ROLL_FACTOR_NAME, "LFRollerFactor");
	m_tagList.AddName( RF_ROLL_FACTOR_NAME, "RFRollerFactor");
	m_tagList.AddName( LR_ROLL_FACTOR_NAME, "LRRollerFactor");
	m_tagList.AddName( RR_ROLL_FACTOR_NAME, "RRRollerFactor");
	m_tagList.AddName( LT_ROLL_FACTOR_NAME, "LTRollerFactor");
	m_tagList.AddName( RT_ROLL_FACTOR_NAME, "RTRollerFactor");

	m_distArray = NULL;
	m_speedArray = NULL;
	m_freqArray = NULL;
	m_accelArray = NULL;
	m_forceArray = NULL;
	m_arraySize = 120000;
	m_lastDataLogIndex = 0;
	m_baseInstInode = 0;
	m_baseArrayInode = 0;
	m_baseBlockInode = 0;
	m_dataBroker = NULL;
	m_zeroSpeed = false;
	m_zspdPublished = false;
	m_arrayLogActive = false;
	m_zeroSpeedVal = 0.5;
	m_zeroSpeedCount = 0;
	m_filteredZerospeed = false;
	m_defaultOverSpeedVal = 0;
	m_overSpeedActive = false;
	m_priority = 25;
	m_sampleDelta = 4;
	m_samplePeriod = 20;
	m_irqId = -1;
	m_waitToRegister = true;
	m_plcSpeedCount = 0;

	m_lossComp = false;
	m_accelLimit = 0;
	m_accelAlarmTime = 0;
	// Default to 1/4 second before sounding alarm
	m_accelAlarmFilterOnFilterCnt = 12;
	// Default to 1 second before silencing alarm
	m_accelAlarmFilterOffFilterCnt = 50;
	m_accelAlarmState = ACCEL_ALARM_OFF;
	m_accelAlarmOnTime = 0LL;
	m_accelAlarmOffTime = 0LL;
	m_alarmValue = "-1";
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
bool IpIcmMgr::HandleCommandLineArg( int option, const char *value)
{
	bool    retVal = true;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

	switch(option)
	{
	case 'p':
		if(value != NULL)
		{
			Log(LOG_DEV_DATA, "Process priority: %s\n", value);
			m_priority = atol( value);
		}
		break;
	case 'R':
		m_waitToRegister = false;
		break;
	default:
		ResManagerBase::HandleCommandLineArg( option, value);
		break;
	}
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::HandleCommandLineArg( %c, '%s')\n", option, optarg == NULL ? "" : optarg);

	return( retVal);
}

/**
 * Attaches the calling process as the IO manager of the given 'rootToAdopt'
 * path. The OS will send messages to our process when IO occurs within the
 * 'rootToAdopt' path. The OS will include a unit number in IO messages
 * pertaining to our path. This unit number is identical to the device number
 * returned by this function. The OS does not include the path in its IO
 * messages, so a process must keep track of device number/path relationships.
 *
 * @param rootToAdopt
 *               Path namespace to take over (i.e. MyResource would be mounted
 *               at /dev/MyResource)
 * @return 0 if successful, -1 if error
 */
int IpIcmMgr::PathAttach( const char *pathToAdopt)
{
	int             retVal = 0;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::PathAttach( %s)\n", pathToAdopt);

	// Attach paths for instantaneous data
	if((retVal = AttachInstPaths( pathToAdopt)) == 0)
	{
		// Attach paths for array data
		if((retVal = AttachArrayPaths( pathToAdopt) ) == 0)
		{
			retVal = AttachBlockPaths( pathToAdopt);
		}
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::PathAttach( %s)\n", pathToAdopt);

	return( retVal);
}

/**
 * Attaches path's in the process manager's namespace for instantaneous
 * data values for speed, force, acceleration, etc.
 *
 * @param rootToAdopt
 *               Path namespace to take over (i.e. MyResource would be mounted
 *               at /dev/MyResource)
 * @return 0 if successful, -1 if error
 */
int IpIcmMgr::AttachInstPaths( const char *rootToAdopt)
{
	int             retVal = 0;
	uint32_t        ii, jj;
	IOFUNC_ATTR_T   *ioAttr;
	string          icmPath;
	int             mode;
	ino_t           nextInode=0;
	const dataMapItem_t *dataItem;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::AttachInstPaths( %s)\n", rootToAdopt);

	// Calculate base inode value for instantaneous data devices
	m_baseInstInode = nextInode = 1;
	// Initialize attribute structs for each channel
	for(ii=0; ii<m_channelNames.size() && retVal == 0; ii++)
	{
		// Loop through all subchannel names
		for(jj=0; jj<m_subChannels.size() && retVal == 0; jj++)
		{
			ioAttr = new IOFUNC_ATTR_T;

			// Form path name to adopt for this channel
			const string tagName( m_channelNames[ ii]);
			const string &chnlTag = m_tagList[ tagName];
			const string dataTag = chnlTag + m_subChannels[jj].GetName();

			icmPath = rootToAdopt + string( "/") + dataTag;

			if((dataItem = m_dataTable.find( dataTag)) != NULL)
			{
				uint32_t brd = dataItem->boardIdx, chnl = dataItem->channelIdx, sub = dataItem->bitIdx;

				if((brd < m_icmIps.size()) && (chnl<ICM_CHNL_BRD))
				{
					// Set proper read/write mode
					if(m_subChannels[jj].IsReadOnly()) mode = 0444;
					else								mode = 0666;

					// Initialize this func attrib struct
					iofunc_attr_init( ioAttr, S_IFBLK|mode, 0, 0);

					nextInode = brd * 1000 + chnl * 100 + sub + m_baseInstInode;
					ioAttr->inode = nextInode;// + jj;
					ioAttr->nbytes = 4;
					ioAttr->mount = &m_mountPoint;

					m_resMgrId = ResManagerBase::PathAttach( icmPath.c_str(), ioAttr);
					if(m_resMgrId == -1)
					{
						delete ioAttr;
						retVal = -1;
						Log( LOG_ERRORS, "\tError adopting '%s': %s\n", icmPath.c_str(), strerror( errno));
						ThrowOsException( errno, "Error adopting ICM path(s)");
					}
					m_icmDevNos.push_back( m_resMgrId);
					m_ioFuncAttrs.push_back( ioAttr);
					Log( LOG_DEV_DATA, "Path %s adopted as device # %d, inode %lld\n", icmPath.c_str(), m_resMgrId, ioAttr->inode);
				}
				else
				{
					Log( LOG_ERRORS, "\tNot adopting path '%s' because board/channel invalid: %d,%d\n",
						 icmPath.c_str(), brd, chnl);
					m_dataTable.erase( dataTag);
				}
			}
		}
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::AttachInstPaths( %s)\n", rootToAdopt);

	return( retVal);
}

/**
 * Attaches path's in the process manager's namespace for array
 * data values for speed, force, acceleration, etc.
 *
 * @param rootToAdopt
 *               Path namespace to take over (i.e. MyResource would be mounted
 *               at /dev/MyResource)
 * @return 0 if successful, -1 if error
 */
int IpIcmMgr::AttachArrayPaths( const char *rootToAdopt)
{
	int             retVal = 0;
	int             ii, count=NumberOfArrayDevices();
	IOFUNC_ATTR_T   *ioAttr;
	string          icmPath;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::AttachArrayPaths( %s)\n", rootToAdopt);

	// Calculate base inode value for array data devices
	m_baseArrayInode = m_baseInstInode + NumberOfInstantaneousDevices();

	// Attach paths for the data arrays
	for(ii=0; (ii<count) && (retVal == 0); ii++)
	{
		ioAttr = new IOFUNC_ATTR_T;
		// Initialize this func attrib struct
		iofunc_attr_init( ioAttr, S_IFREG|0444, 0, 0);
		// Assign inode
		ioAttr->inode = m_baseArrayInode + ii;
		ioAttr->nbytes = 4 * m_arraySize * sizeof( float);
		ioAttr->mount = &m_mountPoint;

		// Form path name to adopt for this channel
		icmPath = string( m_dataArrayNames[ ii]);

		m_resMgrId = ResManagerBase::PathAttach( icmPath.c_str(), ioAttr);
		if(m_resMgrId == -1)
		{
			delete ioAttr;
			retVal = -1;
			Log( LOG_ERRORS, "\tError adopting '%s': %s\n", icmPath.c_str(), strerror( errno));
			ThrowOsException( errno, "Error adopting ICM path(s)");
		}
		m_arrayDevNos.push_back( m_resMgrId);
		m_ioFuncAttrs.push_back( ioAttr);
		Log( LOG_DEV_DATA, "Path %s adopted as device # %d, inode %lld\n", icmPath.c_str(), m_resMgrId, ioAttr->inode);
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::AttachArrayPaths( %s)\n", rootToAdopt);

	return( retVal);
}

/**
 * Attaches path's in the process manager's namespace for block
 * data values for speed, force, acceleration, etc.
 *
 * @param rootToAdopt
 *               Path namespace to take over (i.e. MyResource would be mounted
 *               at /dev/MyResource)
 * @return 0 if successful, -1 if error
 */
int IpIcmMgr::AttachBlockPaths( const char *rootToAdopt)
{
	int             retVal = 0;
	int             ii, count = NumberOfBlockDevices();
	IOFUNC_ATTR_T   *ioAttr;
	string          icmPath;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::AttachBlockPaths( %s)\n", rootToAdopt);

	// Calculate base inode value for block data devices
	m_baseBlockInode = m_baseArrayInode + NumberOfArrayDevices();

	// Attach paths for the data arrays
	for(ii=0; (ii<count) && (retVal == 0); ii++)
	{
		ioAttr = new IOFUNC_ATTR_T;
		// Initialize this func attrib struct
		iofunc_attr_init( ioAttr, _S_IFNAM|0666, 0, 0);
		// Assign inode
		ioAttr->inode = m_baseBlockInode + ii;
		ioAttr->nbytes = ICM_CHNL_BRD * sizeof( float) * m_icmIps.size();
		ioAttr->mount = &m_mountPoint;

		// Form path name to adopt for this channel
		icmPath = string(rootToAdopt) + string( "/") + string( m_blockDataNames[ ii]);

		m_resMgrId = ResManagerBase::PathAttach( icmPath.c_str(), ioAttr);
		if(m_resMgrId == -1)
		{
			delete ioAttr;
			retVal = -1;
			Log( LOG_ERRORS, "\tError adopting '%s': %s\n", icmPath.c_str(), strerror( errno));
			ThrowOsException( errno, "Error adopting ICM block path(s)");
		}
		m_blockDevNos.push_back( m_resMgrId);
		m_ioFuncAttrs.push_back( ioAttr);
		Log( LOG_DEV_DATA, "Path %s adopted as device # %d, inode %lld\n", icmPath.c_str(), m_resMgrId, ioAttr->inode);
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::AttachBlockPaths( %s)\n", rootToAdopt);

	return( retVal);
}

/**
 * Copies instantaneous data values to the data arrays
 */
void IpIcmMgr::UpdateArrays()
{
	register uint32_t   ii;
	const uint32_t      numBrds = m_instData.size();
	uint16_t            brdIdx, chnlIdx;
	const int           sampleSize = (m_icmIps.size() == 1 ? 4 : 6);
	int                 arrayIdx = m_arrayIndex;// * sampleSize;

	// If arrays are not full
	if((m_arrayIndex < ICM_CHNL_BRD*m_arraySize) && (m_arrayLogActive == true))
	{
		Log( 0x40000000, "Adding array sample at index %d\n", arrayIdx);

		// Update data arrays
		for(ii=LF_IDX; ii<ICM_CHANNEL_COUNT; ii++)
		{
			// Decode board and channel index
			DecodeChnlIndexKey( m_channelIdxMap[ ii], brdIdx, chnlIdx);

			// If board/channel indices are valid
			if((brdIdx < numBrds) && (chnlIdx < sampleSize))
			{
				const IcmDataSample_t   &dataSample = m_instData[ brdIdx];
				uint32_t                idx = brdIdx*MAX_CHANNELS + chnlIdx;

				m_speedArray[ arrayIdx + idx] = dataSample.data[ chnlIdx].speed;
				m_distArray[ arrayIdx + idx] = dataSample.data[ chnlIdx].count;
				m_forceArray[ arrayIdx + idx] = dataSample.data[ chnlIdx].force;
				m_accelArray[ arrayIdx + idx] = dataSample.data[ chnlIdx].accel;
				m_freqArray[ arrayIdx + idx] = dataSample.data[ chnlIdx].freq;
			}
		}
		m_arrayIndex += sampleSize;
	}
}

/**
 * Appends the latest data array samples to an ASCII text file
 */
void IpIcmMgr::LogAsciiData()
{
	const int   sampleSize = (m_icmIps.size() == 1 ? 4 : 6);

	// Loop until we've caught up with the data arrays
	while(m_lastDataLogIndex < m_arrayIndex)
	{
		UpdateDataLog( m_distanceLog, m_distArray);
		UpdateDataLog( m_speedLog, m_speedArray);
		UpdateDataLog( m_brakeForceLog, m_forceArray);
		UpdateDataLog( m_accelLog, m_accelArray);
		UpdateDataLog( m_freqLog, m_freqArray);

		// This many channels sampled
		m_lastDataLogIndex += sampleSize;//ICM_CHNL_BRD;

		// Clear the current tag list (this sample's tags written)
		m_dataTagList.Clear();
	}
}

/**
 * Appends the latest data point to a data log file
 *
 * @param dataLog   Data log object to log data to
 * @param dataArray Data array to log data from
 */
void IpIcmMgr::UpdateDataLog( IcmLogFile &dataLog, const float *dataArray)
{
	const int   sampleSize = (m_icmIps.size() == 1 ? 4 : 6);
	const int   sampleIdx = m_lastDataLogIndex / sampleSize;
	int         status = EOK;

	if(dataLog.IsEnabled())
	{
		// If only one board present
		if(sampleSize == 4)
		{
			// Log 4 channels
			status = dataLog.LogSample( sampleIdx,
										dataArray[ m_lastDataLogIndex + 0],
										dataArray[ m_lastDataLogIndex + 1],
										dataArray[ m_lastDataLogIndex + 2],
										dataArray[ m_lastDataLogIndex + 3]);
		}
		else if(sampleSize == 6)
		{
			// Log 6 channels
			status = dataLog.LogSample( sampleIdx,
										dataArray[ m_lastDataLogIndex + 0],
										dataArray[ m_lastDataLogIndex + 1],
										dataArray[ m_lastDataLogIndex + 2],
										dataArray[ m_lastDataLogIndex + 3],
										dataArray[ m_lastDataLogIndex + 4],
										dataArray[ m_lastDataLogIndex + 5]);
		}

		// if the status is a failure and the logs are active, log the error
		if((status != EOK) && (m_arrayLogActive))
		{
			const std::string &fileName = dataLog.GetName();
			Log( LOG_ERRORS, "\tError saving to ICM data log %s: %s\n",
				 fileName.empty() ? "<empty>" : fileName.c_str(), strerror( status));
		}
	}
}

/**
 * Check if all wheels are at zerospeed
 *
 * @param zspd   Zerospeed flag: true if at zerospeed, false otherwise
 */
void IpIcmMgr::WriteZerospeed( bool zspd)
{
	string zspdString;
	string ndbTag( m_tagList[ ZEROSPEED_LATCH_NAME]);
	string ndbResponse;
	const bool filteredZspd = m_filteredZerospeed;

	// If at zerospeed
	if(zspd)
	{
		// Need to be at zerospeed for at least 5 consecutive samples
		if(m_zeroSpeedCount < 5)   m_zeroSpeedCount++;
	}
	else
	{
		// Reset counter
		if(m_zeroSpeedCount != 0)  m_zeroSpeedCount--;
	}

	// Filter out noisy zerospeed
	if(m_zeroSpeedCount >= 5)		   m_filteredZerospeed = true;
	else if(m_zeroSpeedCount == 0)	   m_filteredZerospeed = false;

	// Update intstantaneous status
	m_zeroSpeed = zspd;

	// Even if status hasn't changed we want to tell PLC zerospeed status
	// at least once when we start
	if((m_dataBroker) && (m_zspdPublished == false))
	{
		std::string plcTag, plcVal, plcMssg;

		if(zspd == false)  zspdString = "0";
		else				zspdString = "1";

		// Update zerospeed status to the PLC manager
		m_dataBroker->Write( ndbTag, zspdString, ndbResponse, true);

		// Verify that value was written
		m_dataBroker->Read( ndbTag, ndbResponse, true);
		m_dataBroker->GetNext( plcTag, plcVal, plcMssg);
		if(plcVal == zspdString)
		{
			m_zspdPublished = true;
			Log( LOG_DEV_DATA, "Wrote %s = %s\n", ndbTag.c_str(), zspdString.c_str());

			Log( LOG_DEV_DATA, "\tSending initial ZEROSPEED TO PLC = %s\n", zspdString.c_str());
		}
		else if(m_demoMode == false)
		{
			Log( LOG_ERRORS, "\tError writing %s\n", ndbTag.c_str());
		}
	}

	// If status has changed
	if(m_filteredZerospeed != filteredZspd)
	{
		if(m_filteredZerospeed == false)   zspdString = "0";
		else								zspdString = "1";

		// Notify subscribed clients
		if(m_nList)	m_nList->Notify(m_tagList[ ZEROSPEED_NAME], zspdString);

		// Update zerospeed status to the PLC manager
		if(m_dataBroker)
		{
			Log( LOG_DEV_DATA, "\tSending ZEROSPEED TO PLC = %s\n", zspdString.c_str());
			if(m_dataBroker->Write( ndbTag, zspdString, ndbResponse, true) == (INT32)BEP_STATUS_ERROR)
			{
				Log( LOG_ERRORS, "\tError sending %s = %s\n", ndbTag.c_str(), zspdString.c_str());
			}

		}
		else
		{
			Log( LOG_DEV_DATA, "INamedDataBroker not initialized\n");
			delay( 1);
		}

		Log( LOG_DEV_DATA, "Setting %s = %d\n", m_tagList[ ZEROSPEED_NAME].c_str(),
			 zspd);
	}
}

/**
 * Publishes status of OverSpeed if it has changed
 *
 * @param overSpeed Overspeed flag: true if any wheel over the OverSpeed limit, false otherwise
 */
void IpIcmMgr::WriteOverspeed( bool overSpeed)
{
	string overSpeedString( "1");
	string ndbTag( m_tagList[ OVERSPEED_NAME]);
	string ndbResponse;

	// If status has changed
	if(m_overSpeedActive != overSpeed)
	{
		if(overSpeed == false) overSpeedString = "0";

		// Notify subscribed clients
		if(m_nList)	   m_nList->Notify( ndbTag, overSpeedString);

		// Update status
		m_overSpeedActive = overSpeed;

		Log( LOG_DEV_DATA, "Setting %s = %d\n", ndbTag.c_str(), overSpeed);
	}
}

/**
 * Writes the current wheel speeds to the PLC
 */
void IpIcmMgr::WritePlcWheelSpeeds()
{
	string      tags[] = {  m_tagList[ "LFPlcSpeed"], m_tagList[ "RFPlcSpeed"],
		m_tagList[ "LRPlcSpeed"], m_tagList[ "RRPlcSpeed"],m_tagList[ "LTPlcSpeed"],m_tagList[ "RTPlcSpeed"]};
	uint16_t    brdIdx;
	uint16_t    chnlIdx;
	string      ndbResp;
	char        ndbVal[32];
	int         ii;

	// Only write once every 300ms
	if( (m_plcSpeedCount % 15) == 0)
	{
		m_plcSpeedCount = 0;
		// Need to write LF through RR or RT wheel speeds
		int channelCount = (m_isrFifo.size() < 2) ? LT_IDX : ICM_CHANNEL_COUNT;
		Log(LOG_DEV_DATA, "Updating PLC for %d channels (m_isrFifo.size(): %d,  ICM_CHANNEL_COUNT: %d)",
			channelCount, m_isrFifo.size(), ICM_CHANNEL_COUNT);

		for(ii=LF_IDX; ii<channelCount; ii++)
		{
			// Decode board and channel index
			DecodeChnlIndexKey( m_channelIdxMap[ ii], brdIdx, chnlIdx);

			if((brdIdx < m_isrFifo.size()) && ( chnlIdx < MAX_CHANNELS))
			{
				// Put integer wheel speed into a string to send to the PLC
				const IcmChannelData_t  &avgVals = m_isrFifo[brdIdx].avgValues[chnlIdx];
				sprintf( ndbVal, "%d", (int)avgVals.speed);
				// Only write if named data broker interface has been created
				if(m_dataBroker != NULL)
				{
					// Only set isFinal=true when writing last channel
					Log(LOG_DEV_DATA, "Writing %s:%s to PLC", tags[ii].c_str(), ndbVal);
					m_dataBroker->Write( tags[ii], ndbVal, ndbResp, ii == (channelCount-1));
				}
				else
				{
					break;
				}
			}
#if 0
			else
			{
				Log( LOG_ERRORS, "\tInvalid board/channel for writing %s: %d, %d\n",
					 tags[ ii].c_str(), brdIdx, chnlIdx);
			}
#endif
		}
	}
	m_plcSpeedCount++;
}

/**
 * Updates a channel loss (machine loss) in the ICM config file and
 * on the board and channel being passed in
 *
 * @param icmBoard   Index of the ICM board to update the machine loss on
 * @param channelIdx Channel to update the machine loss on
 * @param newLoss    New machine loss value
 */
void IpIcmMgr::WriteChannelLoss( uint32_t icmBoard, uint32_t channelIdx, float newLoss)
{
	float           oldLoss;
	int             idx, fileFd;
	uint32_t        jj;
	const int       chnlCount = m_icmIps.size() * ICM_CHNL_BRD;
	float           oldLosses[ chnlCount];
	IpIcmListItr_t  itr;
	std::string     xmlLossString;
	int             chnlIdx;
	dataTag_t       lossTag;
	StringListItr_t tagItr;
	char            lossBuff[ 64];

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::WriteChannelLoss( %d, %d, %.3f)\n", icmBoard, channelIdx, newLoss);
	// Make sure indeices valid
	if((icmBoard < m_icmIps.size()) && ( channelIdx < ICM_CHNL_BRD))
	{
		idx = 0;
		// Read current losses for each board
		for(itr=m_icmIps.begin(); itr!=m_icmIps.end(); itr++)
		{
			for(jj=0; jj<ICM_CHNL_BRD; jj++)
			{
				oldLosses[idx++] = m_convFactors[ icmBoard].offset[ jj].force;
			}
		}

		// Replace current loss value with new loss value
		oldLoss = oldLosses[ icmBoard * ICM_CHNL_BRD + channelIdx];
		oldLosses[ icmBoard * ICM_CHNL_BRD + channelIdx] = newLoss;

		// Update our running loss value
		m_convFactors[ icmBoard].offset[ channelIdx].force = newLoss;

		// Put loss values into an XML formatted string
		xmlLossString = std::string( "<MachineLosses>\n");
		for(tagItr=m_channelNames.begin(); tagItr!=m_channelNames.end(); tagItr++)
		{
			const std::string &chnlTag = m_tagList.find( *tagItr)->second;

			// Make the data tag for this channel's loss value
			lossTag = chnlTag + std::string( "Loss");

			// Lookup this tag in our data map for the channel mapping
			const dataMapItem_t *dataItem = m_dataTable.find( lossTag);
			if(dataItem != NULL)
			{
				// Validate the channel index
				if((dataItem->boardIdx < m_icmIps.size()) && ( dataItem->channelIdx < ICM_CHNL_BRD))
				{
					chnlIdx = dataItem->boardIdx * ICM_CHNL_BRD + dataItem->channelIdx;
					sprintf( lossBuff, "\t<%s>%+7.3f</%s>\n", chnlTag.c_str(),
							 m_convFactors[ dataItem->boardIdx].offset[ dataItem->channelIdx].force,
							 chnlTag.c_str());
					Log( 0x20000000, "\tLoss node for %s: %s\n", lossTag.c_str(), lossBuff);
					xmlLossString += lossBuff;
				}
				else
				{
					Log( LOG_ERRORS, "\tInvalid board/channel for %s: %d,%d\n", lossTag.c_str(),
						 dataItem->boardIdx, dataItem->channelIdx);
				}
			}
			else
			{
				Log( LOG_ERRORS, "\tUnable to map %s to a channel\n", lossTag.c_str());
			}
		}
		xmlLossString += std::string( "</MachineLosses>");

		Log( 0x20000000, "\tWriting %s to file %s\n", xmlLossString.c_str(), m_lossFileName.c_str());
		// Open the file (create it if necessary and erase its contents)
		if((fileFd = open( m_lossFileName.c_str(), O_RDWR | O_CREAT | O_TRUNC)) != -1)
		{
			// Write new losses to our machine loss data file
			write( fileFd, xmlLossString.c_str(), xmlLossString.length());
			close( fileFd);
		}
		else
		{
			Log( LOG_ERRORS, "\tError opening %s for writing: %s\n", m_configFile.c_str(), strerror( errno));
		}
	}
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::WriteChannelLoss( %d, %d, %.3f)\n", icmBoard, channelIdx, newLoss);
}

/**
 * Checks to see if any acceleration value exceeds the accel limit. If any of the acceleration
 * values exceeds the accel limit, the PLC horn will be "beeped"
 */
void IpIcmMgr::CheckAccelLimit()
{
	std::string         ndbResp;
	int                 brdIdx, chnlIdx;
	bool                overAccel=false;
	std::string         valToWrite( "0");

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::CheckAccelLimit()\n");

	// If we need to monitor acceleration
	if( (m_accelLimit > 0) && (m_dataBroker != NULL))
	{
		// Only check accel limit if not performing loss compensation
		if( m_lossComp == false)
		{
			// Read acceleartion date for all channels
			for(brdIdx=0; brdIdx<(int)m_icmIps.size() && overAccel==false; brdIdx++)
			{
				for(chnlIdx=0; chnlIdx<MAX_CHANNELS && overAccel==false; chnlIdx++)
				{
					const IcmChannelData_t  &avgVals = m_isrFifo[ brdIdx].avgValues[ chnlIdx];

					// If any channel's acceleration exceeds the accel limit
					if( avgVals.accel > m_accelLimit)
					{
						// Sound the alarm
						overAccel = true;
					}
				}
			}

			// If we need to sound the alarm
			if( overAccel == true)
			{
				// If this is the first sample over the accel limit
				switch( m_accelAlarmState)
				{
				// Alarm is currently off...need to schedule the alarm
				case ACCEL_ALARM_OFF:
					// Reset our 'over accel' counter
					Log( LOG_DEV_DATA, "Accel limit (%.2f MPH/sec) exceeded\n", m_accelLimit);
					m_accelAlarmState = ACCEL_ALARM_ON_PENDING;
					m_accelAlarmTime = 1;
					m_accelAlarmOnTime = 0LL;
					m_accelAlarmOffTime = 0LL;
					break;
					// Waiting to turn the alarm off (waiting for 'alarm off' samples)
				case ACCEL_ALARM_OFF_PENDING:
					// Log a message
					Log( LOG_DEV_DATA, "Accel limit (%.2f MPH/sec) re-exceeded\n", m_accelLimit);
					m_accelAlarmState = ACCEL_ALARM_ON;
					m_accelAlarmTime = m_accelAlarmFilterOnFilterCnt;
					valToWrite = GetAccelAlarmValue( valToWrite);
					break;
					// Alarm is currently scheduled...wait for 'alarm time' samples above limit
				case ACCEL_ALARM_ON_PENDING:
					// If over the limit for more than 'n' consecutive samples
					if( m_accelAlarmTime++ > m_accelAlarmFilterOnFilterCnt)
					{
						Log( "Tripping accel warning @ %d consecutive samples\n", m_accelAlarmTime);
						m_accelAlarmState = ACCEL_ALARM_ON;
					}
					break;
				case ACCEL_ALARM_ON:
					// Toggle the alarm (make it beep)
					valToWrite = GetAccelAlarmValue( valToWrite);
					break;
				default:
					break;
				}
			}
			else
			{
				// If this is the first sample over the accel limit
				switch( m_accelAlarmState)
				{
				// Alarm is currently on...schedule a cancellation
				case ACCEL_ALARM_ON:
					// Reset our 'over accel' counter
					Log( LOG_DEV_DATA, "Scheduling accel alarm cancellation: below accel limit (%.2f MPH/sec)\n", m_accelLimit);
					// Dropped below the limit with alarm active...schedule alarm cancellation
					m_accelAlarmState = ACCEL_ALARM_OFF_PENDING;
					m_accelAlarmTime = m_accelAlarmFilterOffFilterCnt;
					valToWrite = GetAccelAlarmValue( valToWrite);
					break;
					// Waiting to turn the alarm on (waiting for 'alarm on' samples)
				case ACCEL_ALARM_ON_PENDING:
					// Log a message
					Log( LOG_DEV_DATA, "Accel alarm cancelled: not above limit (%.2f MPH/sec) long enough\n", m_accelLimit);
					// Not enough samples to trip alarm, so cancel the scheduled alarm
					m_accelAlarmState = ACCEL_ALARM_OFF;
					m_accelAlarmTime = 0;
					break;
					// Waiting to turn the alarm off (waiting for 'alarm off' samples)
				case ACCEL_ALARM_OFF_PENDING:
					// Log a message
					if( m_accelAlarmTime-- <= 0)
					{
						Log( LOG_DEV_DATA, "Accel alarm cancelled: %d samples below limit (%.2f MPH/sec)\n",
							 m_accelAlarmFilterOffFilterCnt, m_accelLimit);
						m_accelAlarmState = ACCEL_ALARM_OFF;
						m_accelAlarmTime = 0;
					}
					else
					{
						valToWrite = GetAccelAlarmValue( valToWrite);
					}
					break;
				case ACCEL_ALARM_OFF:
					valToWrite = GetAccelAlarmValue( valToWrite);
					m_accelAlarmTime = 0;
					m_accelAlarmOnTime = 0LL;
					m_accelAlarmOffTime = 0LL;
					break;
				default:
					break;
				}
			}
		}

		// If our NDB object is valid and the alarm value has changed
		if( (m_dataBroker != NULL) && (valToWrite != m_alarmValue))
		{
			Log( LOG_DEV_DATA, "Toggling accel alarm <%s> from %s to %s\n", m_tagList[ "AccelLimitFaultTag"].c_str(),
				 valToWrite.c_str(), m_alarmValue.c_str());
			m_alarmValue = valToWrite;
			m_dataBroker->Write( m_tagList[ "AccelLimitFaultTag"], m_alarmValue, ndbResp, true);
		}
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::CheckAccelLimit()\n");
}

/**
 * Determine what value to write to the accel alarm tag
 *
 * @param valToWrite The value to be written to the alarm output ("0" or "1")
 *
 * @return The value to be written to the alarm output ("0" or "1")
 */
std::string& IpIcmMgr::GetAccelAlarmValue( std::string& valToWrite)
{
	const uint64_t  beepTime = mSEC_nSEC( 500LL);	// .5 seconds (.5 on / .5 off)
	uint64_t        currTime;

	// Read current time
	ClockTime( CLOCK_REALTIME, NULL, &currTime);

	// What state is the alarm currently in
	switch( m_accelAlarmState)
	{
	case ACCEL_ALARM_OFF_PENDING:
	case ACCEL_ALARM_ON:
		// If beep is currently a logic '1'
		if( m_accelAlarmOnTime != 0LL)
		{
			// Check how long the horn has been beeped high
			if( nsDelta( m_accelAlarmOnTime, currTime) >= beepTime)
			{
				// Toggle the beep to off
				ClockTime( CLOCK_REALTIME, NULL, &m_accelAlarmOffTime);
				m_accelAlarmOnTime = 0LL;
			}
		}
		// If beep is currently a logic '0'
		else if( m_accelAlarmOffTime != 0LL)
		{
			// Check how long the horn has been beeped high
			if( nsDelta( m_accelAlarmOffTime, currTime) >= beepTime)
			{
				// Toggle the beep to on
				ClockTime( CLOCK_REALTIME, NULL, &m_accelAlarmOnTime);
				m_accelAlarmOffTime = 0LL;
			}
		}
		else
		{
			// Default o turning the beep on
			ClockTime( CLOCK_REALTIME, NULL, &m_accelAlarmOnTime);
			m_accelAlarmOffTime = 0LL;
		}

		// If beep should be turned on
		if( m_accelAlarmOnTime != 0)	valToWrite = "1";
		// Beep should be turned off
		else							valToWrite = "0";
		break;
		// Alarm is not currently active, so write a '0'
	default:
		valToWrite = "0";
		break;
	}

	return( valToWrite);
}

/**
 * Initialilzes an ICM board using the given board address
 */
void IpIcmMgr::InitializeBoards()
{
	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::InitializeBoards()\n");

	// Allocate the ISR sample array
	if(DetectBoards() > 0)
	{
		m_isrFifo.reserve( m_icmIps.size());        m_isrFifo.resize( m_icmIps.size());
		m_convFactors.reserve( m_icmIps.size());    m_convFactors.resize( m_icmIps.size());

		CalculateConversionFactors();
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::InitializeBoards()\n");
}

/**
 * Caluclates the conversion factors used to convert ICM counts into speed,
 * force and acceleration values
 */
void IpIcmMgr::CalculateConversionFactors()
{
	uint32_t    brd, chnl;
	float       rollFactor, encoderPulses, rollDiam;
	float       hzMph;
	float       inchPulse;

	m_isrFifo.Lock();
	for(brd=0; brd<m_icmIps.size(); brd++)
	{
		m_isrFifo[ brd].head = m_isrFifo[ brd].tail = 0;
		RollAttribs_t   &rollAttribs = m_rollAttribs[ brd];

		for(chnl=0; chnl<MAX_CHANNELS; chnl++)
		{
			rollFactor = rollAttribs.rollerFactor[ chnl];
			encoderPulses = rollAttribs.encoderPulse[ chnl];
			rollDiam = rollAttribs.rollDiameter[ chnl];
			hzMph = (encoderPulses * 12.0 * 5280.0) / ( 3600.0 * ICM_PI * rollDiam);
			inchPulse = (ICM_PI * rollDiam / encoderPulses);		// [inch/pulse]

			m_convFactors[ brd].scale[ chnl].distance = (inchPulse / (INCH_FEET * FEET_MILE));
			m_convFactors[ brd].scale[ chnl].speed = 1.0 / hzMph;
			m_convFactors[ brd].scale[ chnl].accel = 1000.0 / (m_samplePeriod * hzMph);
			m_convFactors[ brd].scale[ chnl].force = -1.0 * (rollFactor * m_samplePeriod) / (1000.0);

			m_convFactors[ brd].offset[ chnl].accel = 0;
			m_convFactors[ brd].offset[ chnl].distance = 0;
			m_convFactors[ brd].offset[ chnl].force = 0;
			m_convFactors[ brd].offset[ chnl].speed = 0;

			Log( LOG_DEV_DATA, "\tChannel %d\n", chnl);
			Log( LOG_DEV_DATA, "\t\tRollFactor = %f\n", rollFactor);
			Log( LOG_DEV_DATA, "\t\t    Period = %f\n", m_samplePeriod);
			Log( LOG_DEV_DATA, "\t\t    kSpeed = %f\n", m_convFactors[ brd].scale[ chnl].speed);
			Log( LOG_DEV_DATA, "\t\t    kAccel = %f\n", m_convFactors[ brd].scale[ chnl].accel);
			Log( LOG_DEV_DATA, "\t\t    kForce = %f\n", m_convFactors[ brd].scale[ chnl].force);
		}
	}
	m_isrFifo.Unlock();
}

/**
 * Attempts to autodetect ICM IP modules in the system
 *
 * @return Number of ICM IP modules detected
 */
int IpIcmMgr::DetectBoards()
{
	IpIcm   *ipIcm;
	int     modIdx = 0, carrIdx = 0;
	char    nameBuff[ 256];

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::DetectBoard\n");

	// Try to find the IP carrier with our ICM module son it
	while(carrIdx < MAX_PCI_40_CARDS)
	{
		snprintf( nameBuff, 255, "Pci40");
		m_pci40Card.Initialize( *this);
		m_pci40Card.SetProcessName( nameBuff);

		// Initialize the IP carrier card
		if((m_pci40Card.Initialize( carrIdx) == true) || (m_demoMode == true))
		{
			// Attempt to locate all the ICM IP modules on the carrier
			while((m_pci40Card.LocateIP( IP_ICM_MANUFACTURER, IP_ICM_MODEL, modIdx) != -1) || (m_demoMode == true))
			{
				ipIcm = new IpIcm;
				snprintf( nameBuff, 255, "Pci40/ip%d", modIdx);

				ipIcm->Initialize( *this);
				ipIcm->SetProcessName( nameBuff);

				if(ipIcm->Initialize( &m_pci40Card, modIdx, m_encoderPhasing) == true)
				{
					// Add this object to the list
					m_icmIps.push_back( ipIcm);
				}
				else
				{
					// If in demo mode and no cards in lise yet
					if((m_demoMode == true) && (m_icmIps.size() == 0))
					{
						// Add one false card to list and exit
						m_icmIps.push_back( ipIcm);
						break;
					}
					else
					{
						// Initialization faiiled
						delete ipIcm;
					}
				}
				modIdx++;
			}

			// If we found ICM IP modules on this carrier
			if(m_icmIps.size() > 0)
			{
				// Stop looking
				break;
			}
		}
		carrIdx++;
	}
	Log( LOG_FN_ENTRY,"Exit IpIcmMgr::DetectBoard\n");

	return( m_icmIps.size());
}

/**
 * Read general config data(for all ICM boards we manage)
 *
 * @param childNodes List of configuration nodes
 */
void IpIcmMgr::ReadGeneralConfig( const XmlNodeMap &childNodes)
{
	XmlNode *xmlNode;
	const XmlString autoTag( XML_T("AUTO"));

	Log( LOG_FN_ENTRY,"Enter ReadGeneralConfig\n");

	// Read the name of the file to save machine losses into
	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XML_T("MachineLossDataFile"));
		m_lossFileName = std::string( getenv("FTP_ROOT")) + std::string( "/") + XmlToAscii( xmlNode->getValue());
	}
	catch(XmlException &err)
	{
		m_lossFileName = std::string( getenv("FTP_ROOT")) + std::string( "/Configuration/Servers/MachineLosses.xml");
	}

	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XML_T("SampleDelta"));
		m_sampleDelta = atoi( XmlToAscii( xmlNode->getValue()).c_str());
	}
	catch(XmlException &err)
	{
		m_sampleDelta = 4;
	}

	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XML_T("SamplePeriod"));
		m_samplePeriod = atoi( XmlToAscii( xmlNode->getValue()).c_str());
		if( m_samplePeriod <= 0)
		{
			Log( LOG_DEV_DATA, "Changing sample period of %d ms to 20 ms\n", m_samplePeriod);
			m_samplePeriod = 20;
		}
	}
	catch(XmlException &err)
	{
		m_samplePeriod = 20;
	}
	Log( LOG_DEV_DATA, "\tSample Period = %d ms\n", m_samplePeriod);

	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XML_T("AccelLimit"));
		m_accelLimit = atof( XmlToAscii( xmlNode->getValue()).c_str());
	}
	catch( XmlException &err)
	{
		m_accelLimit = 0;
	}
	Log( LOG_DEV_DATA, "Acceleration fault limit: %+7.3f\n", m_accelLimit);

	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XML_T("AccelAlarmOnWait"));
		uint32_t    onWait = atoi( XmlToAscii( xmlNode->getValue()).c_str());
		if( onWait  < 0)	onWait = 250;
		m_accelAlarmFilterOnFilterCnt = onWait / m_samplePeriod;
	}
	catch( XmlException &err)
	{
		m_accelAlarmFilterOnFilterCnt = 12;
	}
	Log( LOG_DEV_DATA, "Acceleration Alarm On Wait: %+lld ns\n", m_accelAlarmOnTime);

	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XML_T("AccelAlarmOffWait"));
		uint32_t    offWait = atoi( XmlToAscii( xmlNode->getValue()).c_str());
		if( offWait  < 0)	offWait = 1000;

		m_accelAlarmFilterOffFilterCnt = offWait / m_samplePeriod;
	}
	catch( XmlException &err)
	{
		// Default to 1 second before silencing alarm
		m_accelAlarmFilterOffFilterCnt = 50;
	}
	Log( LOG_DEV_DATA, "Acceleration Alarm Off Wait: %+lld ns\n", m_accelAlarmOffTime);

	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XML_T("Overspeed"));
		float    overSpeed = atof( XmlToAscii( xmlNode->getValue()).c_str());
		if( overSpeed  < 0)	overSpeed = 80.0;

		m_defaultOverSpeedVal = overSpeed;
	}
	catch( XmlException &err)
	{
		// Default to no overspeed
		m_defaultOverSpeedVal = 0.0;
	}
	Log( LOG_DEV_DATA, "Overspeed limit: %.02f MPH\n", m_defaultOverSpeedVal);

	try
	{
		xmlNode = (XmlNode*)childNodes.getNode( XML_T("Zerospeed"));
		float    zeroSpeed = atof( XmlToAscii( xmlNode->getValue()).c_str());
		if( zeroSpeed  < 0)	zeroSpeed = 0.5;

		m_zeroSpeedVal = zeroSpeed;
	}
	catch( XmlException &err)
	{
		// Default to 0.1 MPH
		m_zeroSpeedVal = 0.1;
	}
	Log( LOG_DEV_DATA, "Zerospeed limit: %.02f MPH\n", m_zeroSpeedVal);

	Log( LOG_FN_ENTRY,"Exit ReadGeneralConfig\n");
}

/**
 * Reads the data logging data from the given XML configuration node
 *
 * @param dataLogNode
 *               XML node whose children each contain a data log file name and link name
 */
void IpIcmMgr::ReadDataLogConfig( const XmlNode *dataLogNode)
{
	Log( LOG_FN_ENTRY, "Enter IcmMgr::ReadDataLogConfig( %s)\n", dataLogNode->ToString().c_str());
	try
	{
		// Read data logging parameters for distance data
		m_distanceLog.Initialize(&m_dataTagList, dataLogNode->getChild( CFG_DIST_LOG_TAG));
	}
	catch(...)
	{
		Log( LOG_ERRORS, "\tError initializing distance log\n");
	}

	try
	{
		// Read data logging parameters for speed data
		m_speedLog.Initialize(&m_dataTagList, dataLogNode->getChild( CFG_SPEED_LOG_TAG));
	}
	catch(...)
	{
		Log( LOG_ERRORS, "\tError initializing speed log\n");
	}

	try
	{
		// Read data logging parameters for brake force data
		m_brakeForceLog.Initialize(&m_dataTagList, dataLogNode->getChild( CFG_FORCE_LOG_TAG));
	}
	catch(...)
	{
		Log( LOG_ERRORS, "\tError initializing force log\n");
	}

	try
	{
		// Read data logging parameters for acceleration data
		m_accelLog.Initialize(&m_dataTagList, dataLogNode->getChild( CFG_ACCEL_LOG_TAG));
	}
	catch(...)
	{
		Log( LOG_ERRORS, "\tError initializing accel log\n");
	}

	try
	{
		// Read data logging parameters for acceleration data
		m_freqLog.Initialize(&m_dataTagList, dataLogNode->getChild( CFG_FREQ_LOG_TAG));
	}
	catch(...)
	{
		Log( LOG_ERRORS, "\tError initializing frequency log\n");
	}
	Log( LOG_FN_ENTRY, "Exit IcmMgr::ReadDataLogConfig( %s)\n", dataLogNode->getName().c_str());
}

/**
 * Initializes the givern AsciiLogFile_t structure from the given
 * XML configuration node
 *
 * @param logStruct AsciiLogFile_t structure to be initialized
 * @param node      XML configuration node containing the log file name and link
 *                  name as attributes
 */
void IpIcmMgr::ReadDataLogConfig( AsciiLogFile_t &logStruct, const XmlNode *node)
{
	// Read file name to use for storing ICM data to in ASCII format
	try
	{
		if(logStruct.logFileName.empty() == true)
		{
			logStruct.logFileName = getenv("FTP_ROOT") + XmlToAscii( node->getValue());
			Log(LOG_DEV_DATA, "Storing Log File: %s\n", logStruct.logFileName.c_str());
		}
	}
	catch(XmlException &err)
	{
	}

	// Read name of the link to use for the gui to FTP the log file from
	try
	{
		if(logStruct.logFileLink.empty() == true)
		{
			logStruct.logFileLink = getenv("FTP_ROOT") + XmlToAscii( node->getValue());
			Log(LOG_DEV_DATA, "Storing Log File Link: %s\n", logStruct.logFileLink.c_str());
		}
	}
	catch(XmlException &err)
	{
	}
}

/**
 * Read configuration data for a given channel as specified by the
 * chnlTag parameter
 *
 * @param children List of setup data nodes (children of <SETUP>)
 * @param chnlTag  Tag of the channel to read config data for
 */
void IpIcmMgr::ReadChannelConfig( const XmlNodeMap &children, const char *chnlTag)
{
	uint32_t            chnlIdx = 0, brdIdx;
	const std::string   mapIdxTag( chnlTag);
	uint32_t            mapIdx;

	try
	{
		// Read this channel's board index and channel index
		GetChannelIndex( children.getNode( chnlTag), brdIdx, chnlIdx);
		Log(LOG_DEV_DATA, "Channel %s on board %ld, channel %ld\n", chnlTag, brdIdx, chnlIdx);

		dataMapItem_t   dataItem( chnlIdx, (UINT16)(-1), true, PORT_DIR_OUT, brdIdx);

		// Add this to our temporary data map
		m_channelIndexes.insert( chnlTag, dataItem);

		// Add the channel index mapping for this channel (i.e. LF data
		//  is mapped to board 3, channel 2).
		if(mapIdxTag == m_tagList[ m_channelNames[ LF_IDX]])	   mapIdx = LF_IDX;
		else if(mapIdxTag == m_tagList[ m_channelNames[ RF_IDX]])  mapIdx = RF_IDX;
		else if(mapIdxTag == m_tagList[ m_channelNames[ LR_IDX]])  mapIdx = LR_IDX;
		else if(mapIdxTag == m_tagList[ m_channelNames[ RR_IDX]])  mapIdx = RR_IDX;
		else if(mapIdxTag == m_tagList[ m_channelNames[ LT_IDX]])  mapIdx = LT_IDX;
		else if(mapIdxTag == m_tagList[ m_channelNames[ RT_IDX]])  mapIdx = RT_IDX;
		else													   mapIdx = ICM_CHANNEL_COUNT;

		if(mapIdx != ICM_CHANNEL_COUNT)
		{
			// Make sure our channel index list has enough room for the new entry
			if(m_channelIdxMap.size() <= mapIdx)
			{
				m_channelIdxMap.reserve( mapIdx+1);
				m_channelIdxMap.resize( mapIdx+1);
			}
			// Add mapped index for this channel
			m_channelIdxMap[ mapIdx] = EncodeChnlIndexKey( brdIdx, chnlIdx);
			Log( LOG_DEV_DATA, "\tKey: %d @ map index %d\n", m_channelIdxMap[ mapIdx], mapIdx);
		}
		else
		{
			Log( LOG_ERRORS, "\tUnable to find channel mapping for tag '%s'\n", chnlTag);
		}

		// Try to access the desired channel's children (channel setup values)
		const XmlNodeMap &chnlNodes = children.getNode( chnlTag)->getChildren();

		if(brdIdx >= m_rollAttribs.size())
		{
			m_rollAttribs.reserve( brdIdx+1);
			m_rollAttribs.resize( brdIdx+1);
		}
		RollAttribs_t &rollAttribs = m_rollAttribs[ brdIdx];
		////////////////////
		// GetRollFactor
		////////////////////
		try
		{
			rollAttribs.rollerFactor[ chnlIdx] = atof( chnlNodes.getNode( CFG_ROLL_FACTOR_TAG)->getValue().c_str());
			Log(LOG_DEV_DATA, "\troll factor %d = %f\n", chnlIdx, rollAttribs.rollerFactor[ chnlIdx]);
		}
		catch(XmlException &err)
		{
			rollAttribs.rollerFactor[ chnlIdx] = 436.0;
		}

		////////////////////
		// GetEncoderPulses
		////////////////////
		try
		{
			rollAttribs.encoderPulse[ chnlIdx] = atol(chnlNodes.getNode( CFG_ENC_PULSES_TAG)->getValue().c_str());
			Log(LOG_DEV_DATA, "\tencoder %d pulses = %ld pulses/sec\n", chnlIdx, rollAttribs.encoderPulse[ chnlIdx]);
		}
		catch(XmlException &err)
		{
			rollAttribs.encoderPulse[ chnlIdx] = 1250;
		}

		////////////////////
		// GetRollDiameter
		////////////////////
		try
		{
			rollAttribs.rollDiameter[ chnlIdx] = atof( chnlNodes.getNode( CFG_ROLL_DIAM_TAG)->getValue().c_str());
			Log(LOG_DEV_DATA, "\troll diameter %d = %f inches\n", chnlIdx, rollAttribs.rollDiameter[ chnlIdx]);
		}
		catch(XmlException &err)
		{
			rollAttribs.rollDiameter[ chnlIdx] = 35.625;
		}
	}
	catch(XmlException &err)
	{
		Log(LOG_DEV_DATA, "Channel tag '%s' does not exist in config file\n", chnlTag);
	}
}

/**
 * Reads the board index and channel index for a given ICM channel from a
 * configuration file XML node. The board and channel indices are stored
 * as attributes of a channel's XML configuration node.
 *
 * @return 0 if successful
 *         -1 if error
 */
int IpIcmMgr::LoadMachineLosses()
{
	int         retVal = -1;
	const int   chnlCount = m_icmIps.size() *  ICM_CHNL_BRD;
	float       losses[ chnlCount];
	uint32_t    ii, boardIdx, chnlIdx;
	const XmlNode   *lossNode;
	XmlParser   lossParser;

	Log( LOG_FN_ENTRY,"Enter IpIcmMgr::LoadMachineLosses()\n");

	// Initialize losses to 0
	memset( losses, 0, sizeof( losses));

	try
	{
		// Load the XML formatted data
		lossNode = lossParser.ReturnXMLDocument( m_lossFileName);
		const XmlNodeMap &lossNodes = lossNode->getChildren();
		XmlNodeMapCItr  itr;
		// Parse the machine losses from the XML doc
		for(itr=lossNodes.begin(); itr!=lossNodes.end(); itr++)
		{
			int             chnlIdx;
			const XmlNode   *chnlNode = itr->second;
			dataTag_t       lossTag;

			// Find out the channel mapping for this node
			//  the node name should be 'LF' or 'RF' etc
			lossTag = XmlToAscii(chnlNode->getName()) + std::string( "Loss");

			// Lookup this tag in our data map
			const dataMapItem_t *dataItem = m_dataTable.find( lossTag);
			if(dataItem != NULL)
			{
				// Validate the channel index
				if((dataItem->boardIdx < m_icmIps.size()) && ( dataItem->channelIdx < ICM_CHNL_BRD))
				{
					chnlIdx = dataItem->boardIdx * ICM_CHNL_BRD + dataItem->channelIdx;
					sscanf( chnlNode->getValue().c_str(), "%f", &losses[ chnlIdx]);
					Log( LOG_DETAILED_DATA, "\t%s (chnl %d) = %+7.3f\n", lossTag.c_str(), chnlIdx, losses[ chnlIdx]);
				}
				else
				{
					Log( LOG_ERRORS, "\tInvalid board/channel index for %s in machine loss file %d, %d\n",
						 lossTag.c_str(), dataItem->boardIdx, dataItem->channelIdx);
				}
			}
			else
			{
				Log( LOG_ERRORS, "\tInvalid channel name in machine loss data file: %s\n",
					 lossTag.c_str());
			}
		}
	}
	catch(XmlException &err)
	{
		Log( LOG_ERRORS, "\tError parsing machine loss data file %s: %s\n",
			 m_lossFileName.c_str(), err.GetReason());
	}
	catch(BepException &err)
	{
		Log( LOG_ERRORS, "\tError parsing machine loss data file %s: %s\n",
			 m_lossFileName.c_str(), err.GetReason());
	}

	// Update our running loss values
	ii = 0;
	for(boardIdx=0; boardIdx<m_icmIps.size(); boardIdx++)
	{
		for(chnlIdx=0; chnlIdx<MAX_CHANNELS; chnlIdx++)
		{
			m_convFactors[ boardIdx].offset[ chnlIdx].force = losses[ ii++];
		}
	}

	retVal = 0;

	Log( LOG_FN_ENTRY,"Exit IpIcmMgr::LoadMachineLosses()\n");
	return( retVal);
}

/**
 * Reads the board index and channel index for a given ICM channel from a
 * configuration file XML node. The board and channel indices are stored
 * as attributes of a channel's XML configuration node.
 *
 * @param chnlNode Node to read data from
 * @param brdIdx   Board Index on which the channel resides
 * @param chnlIdx  Channel's index on the board it resides on.
 */
void IpIcmMgr::GetChannelIndex( const XmlNode* chnlNode, UINT32 &brdIdx, UINT32 &chnlIdx)
{
	const XmlNodeMap &chnlNodes = chnlNode->getAttributes();

	try
	{
		// GetChannelAddress
		chnlIdx = atol( chnlNodes.getNode( CFG_CHNL_IDX_TAG)->getValue().c_str());
	}
	catch(XmlException &err)
	{
		chnlIdx = 0;
	}
	try
	{
		// GetChannelAddress
		brdIdx = atol( chnlNodes.getNode( CFG_BOARD_IDX_TAG)->getValue().c_str());
	}
	catch(XmlException &err)
	{
		brdIdx = 0;
	}
}

/**
 * Reads the data tag names and properties
 *
 * @param setupNode Node to use as the root DATA_MAP node
 * @return 0 if successful
 *         -1 if error
 */
int IpIcmMgr::ReadDataMap( const XmlElement *setupNode)
{
	const XmlNodeMap &childNodes = setupNode->getChildren();
	XmlNode             *xmlNode;
	dataMapItem_t       dataItem;
	dataTag_t           itemTag;
	int                 nodeIdx, nodeCnt=childNodes.size();
	rawDataMapCItr_t    chnlItr;
	string              chnlTag;

	Log( LOG_FN_ENTRY,"Enter IpIcmMgr::ReadDataMap()\n");

	Log( LOG_DEV_DATA, "\t%-20.20s\tCHNL\tBIT\tLVL\tDIR\tBRD\n", "TAG");
	for(nodeIdx=0; nodeIdx<nodeCnt; nodeIdx++)
	{
		// Get next Data Map node item
		xmlNode = (XmlNode*)childNodes.getNode( nodeIdx);
		// Convert xml tag to ASCII string
		itemTag = XmlToAscii( xmlNode->getName());

		try
		{
			// Parse the data item's attributes (sub-channel and direction)
			ReadDataMapItem( xmlNode, dataItem);

			/****************************************************************
				Need to swap channel index into bit index. The DATA_MAP items
				specify channelIdx in order to be parsed properly. But, this
				channelIdx is actually a sub-channel index (item's offset in
				the channel).
			****************************************************************/
			dataItem.bitIdx = dataItem.channelIdx;

			/****************************************************************
				The data map contains the channel block layout (SPEED, FORCE,
				etc). We need to pre-pend the actual channel name, channel
				index, and board  index to each of these items before putting
				them into the data table
			****************************************************************/
			for(chnlItr=m_channelIndexes.begin(); chnlItr!=m_channelIndexes.end(); chnlItr++)
			{
				// Pre-pend channel name
				chnlTag = chnlItr->first + itemTag;

				// Fill in channel index, board index
				dataItem.channelIdx = chnlItr->second->channelIdx;
				dataItem.boardIdx = chnlItr->second->boardIdx;

				if(((dataItem.boardIdx < m_icmIps.size()) && (dataItem.channelIdx < ICM_CHNL_BRD)) ||
				   (true == m_demoMode))
				{
					Log( LOG_DEV_DATA, "\t %-20.20s\t%d\t%d\t%d\t%d\t%d\n",
						 chnlTag.c_str(),
						 dataItem.channelIdx,
						 dataItem.bitIdx,
						 dataItem.logicLvl,
						 dataItem.portDir,
						 dataItem.boardIdx);

					// Add it to the table if no parsing errors
					m_dataTable.insert( chnlTag, dataItem);
				}
				else
				{
					Log( LOG_DEV_DATA, "\t*%-20.20s\t%d\t%d\t%d\t%d\t%d\n",
						 chnlTag.c_str(),
						 dataItem.channelIdx,
						 dataItem.bitIdx,
						 dataItem.logicLvl,
						 dataItem.portDir,
						 dataItem.boardIdx);


					// Remove the channel prefix (i.e. LF, RF, etc) from the channel names
					//  array if the board index is out of range
					if(dataItem.boardIdx < m_icmIps.size())
					{
						StringListItr_t itr;
						itr = std::find( m_channelNames.begin(), m_channelNames.end(), chnlItr->first);
						if(itr != m_channelNames.end())
						{
							m_channelNames.erase( itr);
						}
					}
				}
			}
		}
		catch(XmlException &xmlErr)
		{
			Log( LOG_ERRORS, "\tXML Error parsing Data Map Item %s: %s\n",
				 itemTag.c_str(), xmlErr.GetReason());
		}
		catch(BepException &bepErr)
		{
			Log( LOG_ERRORS, "\tError parsing Data Map Item %s: %s\n",
				 itemTag.c_str(), bepErr.GetReason());
		}
	}
	Log( LOG_FN_ENTRY,"Exit IpIcmMgr::ReadDataMap()\n");
	return( 0);
}

/**
 * Reads the list of data tags we will use
 *
 * @param tagListNode
 *               Parent node of the data tag names
 */
void IpIcmMgr::ReadDataTagList( const XmlNode *tagListNode)
{
	const XmlNodeMap &tagList = tagListNode->getChildren();
	XmlNodeMapCItr  listItr;

	Log( LOG_FN_ENTRY,"Enter IpIcmMgr::ReadDataTagList()\n");

	// Add all the tag names to our list
	for(listItr=tagList.begin(); listItr!=tagList.end(); listItr++)
	{
		const XmlNode *tagNode = listItr->second;
		if(tagNode->getNodeType() != COMMENT_NODE)
		{
			std::string tag( XmlToAscii( tagNode->getName()));
			std::string name( XmlToAscii( tagNode->getValue()));
			Log( LOG_DEV_DATA, "\t%s mapped to %s\n", tag.c_str(), name.c_str());
			m_tagList.AddName( tag, name);
		}
	}
	Log( LOG_FN_ENTRY,"Exit IpIcmMgr::ReadDataTagList()\n");
}

/**
 * Method used to read instantaneous data when a client performs a
 * read() on any of our instantaneous data devices
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ioOcb  Control Block for this client
 * @return Number of bytes sent to the client
 */
int IpIcmMgr::ReadInstData( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
	const ino_t     &inode = ioOcb->m_ioOcb.attr->inode;
	int             brdIdx, chnlIdx, subChnlIdx, nbytes = 4;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::ReadInstData()\n");

	// Must read only 1 float at a time
	if(msg->i.nbytes == nbytes)
	{
		// Calculate which data item is being read
		brdIdx = ((inode-m_baseInstInode) / 1000);
		chnlIdx = (inode-m_baseInstInode) / 100;
		subChnlIdx = (inode-m_baseInstInode) % 10;
		const IcmDataSample_t &icmSample = m_instData[ brdIdx];
		const IcmChannelConv_t &conv = m_convFactors[ brdIdx];
		float icmData;

		switch(subChnlIdx)
		{
		case DIST_INDEX:        icmData = icmSample.data[ chnlIdx].distance;    break;
		case FREQ_INDEX:        icmData = icmSample.data[ chnlIdx].freq;        break;
		case SPEED_INDEX:       icmData = icmSample.data[ chnlIdx].speed;       break;
		case ACCEL_INDEX:       icmData = icmSample.data[ chnlIdx].accel;       break;
		case FORCE_INDEX:       icmData = icmSample.data[ chnlIdx].force;       break;
		case SPEED_K_INDEX:     icmData = conv.scale[ chnlIdx].speed;           break;
		case ACCEL_K_INDEX:     icmData = conv.scale[ chnlIdx].accel;           break;
		case FORCE_K_INDEX:     icmData = conv.scale[ chnlIdx].force;           break;
		case LOSS_INDEX:        icmData = conv.offset[ chnlIdx].force;          break;
		case COUNT_INDEX:       icmData = icmSample.data[ chnlIdx].count;       break;
		default:                nbytes = 0;                                     break;
		}

		Log( LOG_DEV_DATA, "Replying with %d instantaneous bytes from board %d,"
			 " channel %d, subchannel %d (%f)\n", nbytes, brdIdx, chnlIdx,
			 subChnlIdx, icmData);

		MsgReply( ctp->rcvid, nbytes, &icmData, nbytes);
	}
	else
	{
		Log( LOG_ERRORS, "\tInvalid buffer size (%d) in ReadInstData()\n",
			 msg->i.nbytes);
		MsgReply( ctp->rcvid, ENOTBLK, NULL, 0);
		nbytes = 0;
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::ReadInstData()\n");

	return( nbytes);
}

/**
 * Method used to read array data when a client performs a
 * read() on any of our array data devices
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ioOcb  Control Block for this client
 * @return Number of bytes sent to the client
 */
int IpIcmMgr::ReadArrayData( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
	const ino_t     &inode = ioOcb->m_ioOcb.attr->inode - m_baseArrayInode;
	off_t           off, nleft, nbytes;
	char            *replyPtr;
	int             retVal=ENOSYS;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::ReadArrayData()\n");

	// Check if this is  an XTYPE override
	switch(msg->i.xtype & _IO_XTYPE_MASK)
	{
	case _IO_XTYPE_OFFSET:
		struct _xtype_offset *xoff;
		xoff = (struct _xtype_offset *)(&msg->i + 1);
		off = xoff->offset;
		retVal = EOK;
		break;
	case _IO_XTYPE_NONE:
		off = ioOcb->m_ioOcb.offset;
		retVal = EOK;
		break;
	default:
		break;
	}

	if(retVal == EOK)
	{
		Log( LOG_DEV_DATA, "Reading %d array bytes from offset %d\n",msg->i.nbytes, off);

		// see which array we are reading from
		switch(inode)
		{
		case DIST_ARRAY:    replyPtr = (char*)(m_distArray);    break;
		case SPEED_ARRAY:   replyPtr = (char*)(m_speedArray);   break;
		case ACCEL_ARRAY:   replyPtr = (char*)(m_accelArray);   break;
		case FORCE_ARRAY:   replyPtr = (char*)(m_forceArray);   break;
		case FREQ_ARRAY:    replyPtr = (char*)(m_freqArray);    break;
		default:            replyPtr = NULL;                    break;
		}

		// If valid array data requested
		if(replyPtr != NULL)
		{
			// figure out how many bytes are available
			//  Array index gets incremented by the total number of channels we sample (4 or 6)
			int avail = m_arrayIndex * sizeof( float);
			// figure how many bytes remain from the client's current offset
			nleft = avail - off;

			// and how many we can return to the client
			if((nbytes = min(nleft, msg->i.nbytes)) > 0)
			{
				Log( LOG_DEV_DATA, "ReadArrayData Replying with %d bytes from offset %d\n", nbytes, off);

				// return it to the client
				MsgReply( ctp->rcvid, nbytes, &replyPtr[ off], nbytes);
			}
			else
			{
				// nothing to return, indicate End Of File
				MsgReply(ctp->rcvid, EOK, NULL, 0);
				nbytes = 0;
			}
		}
		else
		{
			// nothing to return, indicate End Of File
			MsgReply(ctp->rcvid, ENOENT, NULL, 0);
			nbytes = 0;
		}
	}
	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::ReadArrayData()\n");

	return( nbytes);
}

/**
 * Method used to read block type data when a client performs a
 * read() on any of our block data devices
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ioOcb  Control Block for this client
 * @return Number of bytes sent to the client
 */
int IpIcmMgr::ReadBlockData( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
	IOFUNC_ATTR_T   *ioAttr = (IOFUNC_ATTR_T*)(ioOcb->m_ioOcb.attr);
	uint32_t        ii, retVal=0;
	uint16_t        blockIdx, brdIdx, chnlIdx;
	const uint32_t  sampleSize = (m_icmIps.size() == 1 ? 4 : 6);
	float           floatData[ sampleSize];

	Log( LOG_DETAILED_DATA, "Enter IpIcmMgr::ReadBlockData(), %d bytes requested from inode %lld\n",
		 msg->i.nbytes, ioAttr->inode);

	// Calculate block index based on device being read
	blockIdx = ioAttr->inode - m_baseBlockInode + SPEED_BLOCK_INDEX;

	memset( floatData, 0, sampleSize*sizeof( float));
	// If we found the requested block data device's inode number
	if(blockIdx <= COUNT_BLOCK_INDEX)
	{
		Log( LOG_DETAILED_DATA, "\tReplying to block request with %d values from block #%d\n",
			 sampleSize, blockIdx);

		if( blockIdx == DRAG_BLOCK_INDEX)
		{
			for(ii=LF_IDX; ii<sampleSize; ii++)
			{
				floatData[ ii] = m_currDrags[ ii];
				retVal += sizeof( float);
			}
		}
		else
		{
			for(ii=LF_IDX; ii<sampleSize; ii++)
			{
				// Decode the channel mapping for the next ICM channel (i.e.
				//  LF is mapped to board 3 channel 2).
				DecodeChnlIndexKey( m_channelIdxMap[ ii], brdIdx, chnlIdx);

				const IcmDataSample_t &icmSample = m_instData[ brdIdx];
				const IcmChannelConv_t &conv = m_convFactors[ brdIdx];
				uint32_t                idx = brdIdx*MAX_CHANNELS + chnlIdx;

				if(chnlIdx < sampleSize)
				{
					switch(blockIdx)
					{
					case SPEED_BLOCK_INDEX: floatData[ idx] = icmSample.data[ chnlIdx].speed;   break;
					case ACCEL_BLOCK_INDEX: floatData[ idx] = icmSample.data[ chnlIdx].accel;   break;
					case FORCE_BLOCK_INDEX: floatData[ idx] = icmSample.data[ chnlIdx].force;   break;
					case LOSS_BLOCK_INDEX:  floatData[ idx] = conv.offset[ chnlIdx].force;      break;
					case COUNT_BLOCK_INDEX: floatData[ idx] = icmSample.data[ chnlIdx].count;   break;
					default:            continue;
					}

					Log( LOG_DETAILED_DATA, "\t%d: %d => %d,%d = +%7.3f\n", blockIdx, m_channelIdxMap[ ii],
						 brdIdx, chnlIdx , floatData[ chnlIdx]);

					retVal += sizeof( float);
				}
				else
				{
					Log( LOG_ERRORS, "\tInvalid channel mapping: %d > %d\n", chnlIdx, sampleSize);
				}
			}
		}
		if(MsgReply( ctp->rcvid, retVal, floatData, retVal) == -1)
		{
			Log( LOG_ERRORS, "\tMsgReplyv failed: %s\n", strerror( errno));
		}
	}
	else
	{
		MsgReply( ctp->rcvid, 0, NULL, 0);
	}
	Log( LOG_DETAILED_DATA, "Exit IpIcmMgr::ReadBlockData(), %d bytes requested from inode %lld\n",
		 msg->i.nbytes, ioAttr->inode);

	return( retVal);
}

int IpIcmMgr::WriteInstData( resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
	IOFUNC_ATTR_T   *ioAttr = (IOFUNC_ATTR_T*)(ioOcb->m_ioOcb.attr);
	off_t           off, nleft, nbytes = 0;
	int             dataOffset=sizeof( msg->i);
	float           dataVal;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::WriteInstData()\n");

	// Calculate array offsets
	int inode = ioAttr->inode;
	// Calculate which data item is being read
	int brdIdx = ((inode-m_baseInstInode) / ICM_SUBCHNL_CNT) / ICM_CHNL_BRD;
	int chnlIdx = ((inode-m_baseInstInode) / ICM_SUBCHNL_CNT) % ICM_CHNL_BRD;
	int subIdx = (inode-m_baseInstInode) % ICM_SUBCHNL_CNT;

	if((subIdx == SPEED_K_INDEX) || (subIdx == ACCEL_K_INDEX) ||
	   (subIdx == FORCE_K_INDEX) || (subIdx == LOSS_INDEX) ||
	   (subIdx == DIST_INDEX))
	{
		// Calculate how much "buffer" is left
		nleft = ioAttr->nbytes - off;
		nbytes = min(  nleft, msg->i.nbytes);
		// If room left in buffer and valid data being written
		if(nbytes == sizeof( float))
		{
			// Read rest of the write message data
			resmgr_msgread( ctp, &dataVal, nbytes, dataOffset);

			// Tell kernel to update last modification time
			((iofunc_attr_t*)(ioOcb->m_ioOcb.attr))->flags |= IOFUNC_ATTR_MTIME |
															  IOFUNC_ATTR_DIRTY_TIME;

			// Advance client's offset if not a combine message
			if((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_OFFSET)
			{
				ioOcb->m_ioOcb.offset += nbytes;
			}

			// Write new data to VME space
			switch(subIdx)
			{
			case SPEED_K_INDEX: m_convFactors[ brdIdx].scale[ chnlIdx].speed = dataVal; break;
			case ACCEL_K_INDEX: m_convFactors[ brdIdx].scale[ chnlIdx].accel = dataVal; break;
			case FORCE_K_INDEX: m_convFactors[ brdIdx].scale[ chnlIdx].force = dataVal; break;
			case LOSS_INDEX:    WriteChannelLoss( brdIdx, chnlIdx, dataVal);            break;
			case DIST_INDEX:
				// IF writing 0 to a distance sub-channel
				if(dataVal == 0)
				{
					// Reset distance values on each of the IP ICMs
					IpIcmListItr_t  itr;
					for(itr=m_icmIps.begin(); itr!=m_icmIps.end(); itr++)
					{
						IpIcm   *ipIcm = *itr;
						ipIcm->ClearDistance( -1);
					}
				}
				else
				{
					nbytes = 0;
				}
				break;
				// Shouldn't get these- they're read only
			case FREQ_INDEX:
			case SPEED_INDEX:
			case ACCEL_INDEX:
			case FORCE_INDEX:
			default:
				nbytes = 0;
				break;
			}
		}
		else
		{
			Log( LOG_ERRORS, "\nAttempt to write invalid number of InstData bytes to channel %d\n",
				 chnlIdx);
			nbytes = 0;
		}
	}
	else
	{
		Log( LOG_ERRORS, "\nAttempt to write to invalid ICM path %d\n",
			 chnlIdx);
		nbytes = 0;
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::WriteInstData()\n");

	return( nbytes);
}

int IpIcmMgr::WriteBlockData( resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
	IOFUNC_ATTR_T   *ioAttr = (IOFUNC_ATTR_T*)(ioOcb->m_ioOcb.attr);
	off_t           off, dataOffset = sizeof( msg->i);
	int             nbytes = 0;
	int             inode = ioAttr->inode;
	int             ii;
	uint16_t        blockIdx;

	Log( LOG_FN_ENTRY, "Enter IpIcmMgr::WriteBlockData()\n");

	if( (inode >= m_baseBlockInode) && (inode < (m_baseBlockInode+NumberOfBlockDevices())) )
	{
		// Check if this is  an XTYPE override
		switch(msg->i.xtype & _IO_XTYPE_MASK)
		{
		case _IO_XTYPE_OFFSET:
			struct _xtype_offset *xoff;
			xoff = (struct _xtype_offset *)(&msg->i + 1);
			off = xoff->offset;
			dataOffset = dataOffset + sizeof(*xoff);
			break;
		case _IO_XTYPE_NONE:
			off = ioOcb->m_ioOcb.offset;
			break;
		default:
			break;
		}

		// Calculate block index based on device being read
		blockIdx = ioAttr->inode - m_baseBlockInode + SPEED_BLOCK_INDEX;

		// Calculate the max number of bytes we can accept
		nbytes = min(  (int)sizeof( m_currDrags), msg->i.nbytes);
		// If client is writing at least 1 whole float
		if(nbytes >= (int)sizeof( float))
		{
			// Determine how many 'floats' the client is writing to us
			int itemCount = nbytes / sizeof( float);

			// Tell kernel to update last modification time
			((iofunc_attr_t*)(ioOcb->m_ioOcb.attr))->flags |= IOFUNC_ATTR_MTIME |
															  IOFUNC_ATTR_DIRTY_TIME;

			// What data block is the client writing to?
			switch( blockIdx)
			{
			case DRAG_BLOCK_INDEX:
				Log( "Getting %d Drag block data items (%d bytes)\n", itemCount, nbytes);
				// Read rest of the io_write message data
				resmgr_msgread( ctp, m_currDrags, nbytes, dataOffset);
				for(ii=0; ii<itemCount; ii++)
				{
					Log( "\tDrag[%d] = %.2f\n", ii, m_currDrags[ ii]);
				}
				break;
			case SPEED_BLOCK_INDEX:
			case ACCEL_BLOCK_INDEX:
			case FORCE_BLOCK_INDEX:
			case LOSS_BLOCK_INDEX:
			case COUNT_BLOCK_INDEX:
			default:
				Log( LOG_ERRORS, "Attempt to write to read only block data item: %d\n", blockIdx);
				nbytes = 0;
				break;
			}
		}
		else
		{
			Log( LOG_ERRORS, "Attempt to write too few block data bytes: %d\n", nbytes);
			nbytes = 0;
		}
	}
	else
	{
		Log( LOG_ERRORS, "Cannot write block data to a non-block device\n");
		nbytes = 0;
	}

	Log( LOG_FN_ENTRY, "Exit IpIcmMgr::WriteBlockData()\n");

	return( nbytes);
}

/**
 * Returns the number of instantaneous devices for which we have adopted paths
 *
 * @return The number of instantaneous devices for which we have adopted paths
 */
int IpIcmMgr::NumberOfInstantaneousDevices()
{
	int retVal;

	retVal = m_icmIps.size() * 1000  + m_channelNames.size() * 100  + m_subChannels.size();

	return( retVal);
}

/**
 * Returns the number of array devices for which we have adopted paths
 *
 * @return The number of array devices for which we have adopted paths
 */
int IpIcmMgr::NumberOfArrayDevices()
{
	return( m_dataArrayNames.size());
}

/**
 * Returns the number of block devices for which we have adopted paths
 *
 * @return The number of block devices for which we have adopted paths
 */
int IpIcmMgr::NumberOfBlockDevices()
{
	return( m_blockDataNames.size());
}

/**
 * Calculates a unique base channel inode value based on a channel tag.
 * The base channel inode is common for all subchannels of a given
 * channel (i.e. BaseInode( LFSpeed) = BaseInode(LFForce)):
 *
 *      Channels Per Board=4, Sub Channel Count=10
 *
 *  Board Index     Channel Index   Base Inode Value
 *  ================================================
 *          0               0               0
 *          0               1               10
 *          0               2               20
 *          0               3               30
 *          1               0               40
 *          1               1               50
 *          1               2               60
 *          1               3               70
 *          2               0               80
 *          2               1               90
 *          2               2               100
 *          2               3               110
 *
 * @param channelTag Tag of the channel to calculate the base inode value
 *                   for (i.e. LF, RF)
 * @return Inode value for the given channel tag
 */
ino_t IpIcmMgr::CalculateBaseChnlInode( const dataTag_t &channelTag) const
{
	ino_t               baseInode = (ino_t)-1;
	const dataMapItem_t *item = m_channelIndexes.find( channelTag);

	// If the given channel tag exists
	if(item != NULL)
	{
		baseInode = item->boardIdx * ICM_CHNL_BRD * ICM_SUBCHNL_CNT;
		baseInode += item->channelIdx * ICM_SUBCHNL_CNT;
	}

	return( baseInode);
}

/**
 * Encodes a (board,channel) pair into a unique index key
 *
 * @param brdIdx  Board index to be encoded
 * @param chnlIdx Channel index to be encoded
 * @return Unique index key encoding for a (board,channel) pair
 */
uint32_t IpIcmMgr::EncodeChnlIndexKey( uint16_t brdIdx, uint16_t chnlIdx)
{
	return( brdIdx * 100 + chnlIdx);
}

/**
 * Decodes unique index key into a (board,channel) pair
 *
 * @param key     Key to be decoded
 * @param brdIdx  Decoded board index (Output)
 * @param chnlIdx Decoded channel index (Output)
 */
void IpIcmMgr::DecodeChnlIndexKey( uint32_t key, uint16_t &brdIdx, uint16_t &chnlIdx)
{
	brdIdx = key / 100;
	chnlIdx = key % 100;
}

