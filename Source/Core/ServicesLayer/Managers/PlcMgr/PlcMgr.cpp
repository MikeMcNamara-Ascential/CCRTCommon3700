//******************************************************************************
// Description:
//    PLC I/O manager
//
//==============================================================================
//
//     Copyright ? 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcMgr.cpp $
// 
// 8     1/18/07 2:50p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added support for Rk512 PLC communication.
// 
// 7     5/24/06 2:55p Cward
// Updated PlcMgr to allow reloading of config files without rebooting.
//
// 1     5/16/06 1:19p Rwiersem
// Added LoadAdditionalConfigurationItems() to support reloading the
// configuration without restarting the process.  Added handling for the
// reload configuration pulse.
//
// 6     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:20p Bmeinke
// Added calls for new TaskMon daemon interface
// Added ClosePlcBoards() method to close all PLC board objects
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 4     9/01/04 9:35p Cward
// Emergency temporary fix for change that causes sig seg.
//
// 3     8/18/04 11:36p Cward
// Removed VME support. Refactored for loop in Check Publish routine
//
// 2     6/24/04 1:12a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 4     6/26/03 1:43p Bmeinke
// Fixed a couple potential bugs in calls to Log(): check to see if a
// std::string.length() > 0 before passing std::string.c_str()
//
// 3     6/24/03 8:56a Khykin
// Brians latest changes.
//
// 1     6/20/03 9:58a Khykin
// Initial checkin.
//
// 2     6/20/03 9:02a Khykin
// Updated Ford code.
//
// 1     6/19/03 2:01p Bmeinke
//
// 1     4/30/03 4:46p Bmeinke
// Migration to new personalized configuration management structure
//
// 34    4/10/03 6:36p Bmeinke
// Added log error print statements in ReadDataItem() if the channel or
// board index is invalid
//
// 33    3/18/03 6:59p Bmeinke
// ReadXmlOptions only returns 0 if ResManager::ReadXmlOoptione() succeeds
//
// 32    3/12/03 2:10p Bmeinke
// Added Register() method because we have to wait until system
// registration until we can open the DF1 logical port
// Added -R command line option to override the normal registration
// sequence
//
// 31    3/12/03 12:49a Bmeinke
// Initial machine testing for Ford Chicago
// Added m_inCountList and m_outCountList data members in order to
// maintain a list of how many I/O channels each PLC board in the system
// has
//
// 30    1/18/03 3:49p Bmeinke
// Redesigned the PLC Object architecture to use a pure virtual base class
// (IPlc) to facilitate the future addition of different types of PLCs
// Maintenance checkin (not fully tested)
//
// 29    11/21/02 8:56a Khykin
// Disabling the auto enabling of the logger.
//
// 28    10/14/02 7:32p Bmeinke
// Changed data tag index (used to map a board/channel/bit triplet to a
// data tag) from UINT32 to uint64_t
// Whole channel items (data items w/ bit index = -1) have msb set in
// their data tag index value
// Added PublishFault() method
// Added IFaultServer object m_faultServer to base class
// DaigItem::CheckTrigger() now takes IFaultServer pointer as second
// parameter: used to publish fault data
// Fixed demo mode
//
// 27    8/23/02 12:08p Kh
// Updating for changes at the pit machine.
//
// 26    8/01/02 5:14p Bmeinke
// Removed compiler warnings
//
// 25    6/27/02 7:24p Bmeinke
// Changed logic for publishing data in ResManager base class; required
// changes to publish logic here
//
// 24    4/25/02 4:44p Skypig13
// Updated with new changes
//     Revision 1.1  2002/04/25 18:39:19  erickson
//     added from VSS
//
//
// 23    4/05/02 1:57p Bmeinke
// Changed initialization of the PlcV30B objects logger interface to use
// the new Initialize( ILogger &) method
// Added CheckDiagItem() to publish/clear fault items from the
// WriteDataItem() method
//
// 22    2/27/02 2:49p Bmeinke
// Reversed the processing order of input and outputs
// Removed heartbeat check (it was a bug)
//
// 21    2/19/02 8:00p Bmeinke
// Changed data types used for reading/writing VME space from UINT32 to
// UINT16
// Added data member m_toPlcInode to ensure that read() and write()
// operate on the correct PLC data area
// Added m_vmeSpaceLock to guard simultaneous read/writes to vme space in
// the PLC manager
// Switched the "roles" of the m_fromPlc and m_toPlc arrays
// Uncommented the call to InitializeIndexedTags()
// Modified theAttachInputPath() and AttachOutputPath() to use the
// m_toPlcInode inode value
// Cleaned up ReadChannels() mehtod
// Fixed bug in CheckPublish() where bit mask use to test value of
// individual PLC word bits was not properly left shifted
// Added a call to WriteChannels() at the end of InitializeBoards() to
// perform an initiali update of VME space
// Added more logger statements
// Commented out the call to SendUpdateOutputsPulse() inside
// WriteDataItem()
//
// 20    1/11/02 2:43a Brian.meinke
// "stat" size for the Inputs 'file' now only reports the number of bytes
// needed for all the inputs, not both the inputs and outputs
//
// 19    11/06/01 9:59a Brian.meinke
// main() now loops until ResManager::Execute() returns
// RES_MGR_CMD_TERMINATE
//
// 18    11/06/01 12:26a Brian.meinke
// Changed ReadDataItem/WriteDataItem to take string references instead of
// XmlNode pointers
//
// 17    11/05/01 10:23a Brian.meinke
// Fixed arguments to ReadDataItem/WriteDataItem methods
//
// 16    10/31/01 2:08p Brian.meinke
// Added support for reading diagnostic data items from the config file
// Removed PLC heartbeat monitoring
//
// 15    10/23/01 2:31p Brian.meinke
// Added method SendUpdateOutputsPulse() to send a pulse to the resource
// manager's I/O channel to indicate a need to write outputs to the PLC in
// response to a client write operation
// Call SendUpdateOutputsPulse() from WriteDataItem()
//
// 14    10/23/01 1:05p Brian.meinke
// Added heartbeat update/monitor inside ReadChannels() method
//
// 13    10/18/01 11:45a Brian.meinke
// Removed calls to iofunc_read_verify and iofunc_write_verify: they are
// handled in the base class
//
// 12    10/16/01 1:15a Brian.meinke
// Channel count(s) were being improperly calculated resulting in hard to
// find SIGSEGV's
//
// 11    10/11/01 11:31a Brian.meinke
// Replaced PlcMgr::m_timerPulseValue with PLC_TIMER_VALUE enum value
// Replaced PlcMgr::m_writePulseValue with PLC_WRITE_VALUE enum value
// main() loops until the resource manager Execute() method returns
// RES_MGR_COMMAND_TERMINATE
// Reset optind=1 AFTER calling base class ParseCommandLine()
// Detach our timer pulse from the dispatch interface in CleanupManager
//
// 10    10/09/01 1:57p Brian.meinke
// Scan timer is now attached to the resource manager's I/O channel
// instead of the XML message channel
// Removed XmlThreadInit() method: base class implementation is sufficient
// Removed XmlHandlerFunc() method: base class and IBepCommunication
// objects handle this functionality
// Added HandleResMgrPulse() method to handle pulses delivered to the
// resource manager's I/O channel
// Added HandlePulse() method to handle pulses delivered to the XML
// message channel
//
// 9     10/04/01 10:25a Brian.meinke
// Integration with IInterProcessCommunication
// Changed timer pulse code from SYSTEM_PULSE to SYSTEM_PULSE+1
//
// 8     9/14/01 5:49p Brian.meinke
// Added CalculateTagIndex() methods to calculate the index of a data
// item's tag basd on board, channel and bit indices
// Added CheckPublish() method to initiate a publish operation for changed
// data items
// Modified ReadChannels() to check for changes in data items and publish
// changed data items
//
// 7     9/04/01 10:11a Brian.meinke
// Removed call to InitHandlers (now handled in ResManager base class)
// Removed static OcbCalloc/OcbFree methods (now handled in ResManager
// base class)
//
// 6     8/31/01 3:18p Brian.meinke
// Re-ordered arguments in calls to ResManager::PathAttach
//
// 5     8/30/01 4:53p Brian.meinke
//
// 4     8/29/01 12:45a Brian.meinke
// Removed Execute method: now just uses base class ResManager
// implementation
// Removed InitHandlers (not needed with new virtual interface)
// Moved call to KillXmlThreads into ResManager::CleanupManager()
// Removed call to PathDetach( m_resMgrId) because m_resmgrId is not used
// Changed static I/O and XML handler methods to virtual methods which
// override the base class methods)
// Use new data member m_timerConnectId to hold timer connection ID
// Seperated I/O func attributes for inputs and outputs
// Removed WriteChannelPulseHandler() (not used)
//
// 3     8/01/01 5:52p Brian.meinke
// Initialize PlcV30B objects' logging interface to be the same as the
// PlcMgr's
// Changed InitializeBoards() method to perform PLC auto detection
//
//
// 2     7/26/01 7:36p Brian.meinke
// Changed spelling of Set/GetVerboseMask
// PlcMgr::xmlBlockFunc() calls ResManager::xmlBlockFunc and adds
// processing for read/write pulses
// Reset optind to 1 after calling ResManager::ParseCommandLine()
// Added ThrowOsException callsChanged spelling of Set/GetVerboseMask
// PlcMgr::xmlBlockFunc() calls ResManager::xmlBlockFunc and adds
// processing for read/write pulses
//
// 1     6/21/01 1:44p Brian.meinke
// PLC Resource Manager
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

/*******************
 include files
 ******************/
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>

#include "PlcMgr.h"
#include "tchar.h"

//==============================================================================
//==============================================================================

/**
 * Connection ID to send the WriteChannel pulse to
 */
int         PlcMgr::m_writeChCoid;

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

/**
 * PLC Resource Manager. Used to handle I/O messages
 * regarding an PLC data items
 *
 * @param channelCnt Number of channels to convert (Default=32)
 * @param verbose    Desired verbose level (default=0- no printouts)
 * @param cnvTime    Time (int milliseconds) required to convert an analog channel.
 *                   Default is CHANNEL_TIME
 */
PlcMgr::PlcMgr( unsigned long channelCnt, int verbose, long cnvTime) :
    ResManager( PLC_MGR_NAME),  m_convertTime( cnvTime), m_channelCnt( 0),
    m_fromPlc( NULL), m_toPlc( NULL), m_fromPlcCount( 0), m_toPlcCount( 0),
    m_inDevNo( -1), m_outDevNo( -1), m_plcChnlFd( -1), m_scannerPrio( 13),
    m_threadPool( NULL), m_autoDetect( true), m_scanTimerId( -1), m_toPlcInode( 1),
    m_waitForRegister( true), m_needFirstPublish( true)
{
    // Set all data to "safe" values
    SetDefaults();

    // Make sure conversion time is valid
    if( cnvTime < 0)    cnvTime = CHANNEL_TIME;

    m_convertTime = cnvTime;
    SetVerboseMask( verbose);
    m_channelCnt = channelCnt;

}

/**
 * PLC Resource Manager. Used to handle I/O messages
 * regarding an PLC data items
 *
 * @param argc   Number of arguments in the argv array
 * @param argv   Array of command line options
 */
PlcMgr::PlcMgr( int argc, char *argv[]) :
    ResManager( PLC_MGR_NAME),  m_convertTime( CHANNEL_TIME), m_channelCnt( 0),
    m_fromPlc( NULL), m_toPlc( NULL), m_fromPlcCount( 0), m_toPlcCount( 0),
    m_inDevNo( -1), m_outDevNo( -1), m_plcChnlFd( -1), m_scannerPrio( 13),
    m_threadPool( NULL), m_autoDetect( true), m_scanTimerId( -1), m_toPlcInode( 1),
    m_waitForRegister( true), m_needFirstPublish( true)
{
    // Set all data to "safe" values
    SetDefaults();

    ParseCommandLine( argc, argv);
}

PlcMgr::~PlcMgr()
{
    Log( LOG_FN_ENTRY, "Enter ~PlcMgr\n");

    ReportTaskDown();

    Log( LOG_FN_ENTRY, "Exit ~PlcMgr\n");
}

/**
 * Set data to "safe" default values
 */
void PlcMgr::SetDefaults()
{
    Log( LOG_FN_ENTRY, "Enter SetDefaults\n");
    m_channelCnt = 8;
    m_convertTime = CHANNEL_TIME;

    m_fromPlc = NULL;
    m_toPlc = NULL;

    m_outDevNo = -1;
    m_inDevNo = -1;

    m_autoDetect = false;

    // Default to no demo
    m_demoMode = false;

    // Default to using the current priority for the scanner threads
    m_scannerPrio = BposGetPriority( 0);

    PlcMgr::m_writeChCoid = -1;

    //SetVerboseMask( 0x1F);
    //SetLocalEcho( true);
    //EnableLogger();

    Log( LOG_FN_ENTRY, "Exit SetDefaults\n");
}

/**
 * Parses command line parameters
 *
 * @param argc   Number of command line arguments in the argv parameter
 * @param argv   Command line option strings
 */
void PlcMgr::ParseCommandLine( int argc, char **argv)
{
    int optId, ii;

    Log( LOG_FN_ENTRY, "Enter ParseCommandLine\n");

    // Disable "Invalid argument" message from getopt
    opterr = 0;

    // Read common options
    ResManager::ParseCommandLine( argc, argv);

    // Start reparsing arguments from the beginning
    optind = 1;

    // Get Command line options
    for( ii=1; ii<argc; ii++)
    {
        switch( (optId=getopt( argc, argv, "c:n:p:R")))
        {
        case 'c':
            m_convertTime = BposReadInt( optarg);
            Log(LOG_DEV_DATA, "Conversion time: %ld ms\n", m_convertTime);
            break;
        case 'n':
            if( BposReadInt( optarg) > (int)m_channelCnt)
            {
                m_channelCnt = BposReadInt( optarg);
                Log(LOG_DEV_DATA, "Channel Count: %d\n", m_channelCnt);
            }
            break;
        case 'p':
            m_scannerPrio = BposReadInt( optarg);
            Log( LOG_DEV_DATA, "Scanner priority: %d\n", m_scannerPrio);
            break;
        case 'R':
            m_waitForRegister = false;
            Log( LOG_DEV_DATA, "Not waiting to register\n");
            break;
        default:
            if( optId == '?')                       optind++;
            else if(argv[ optind] == NULL)          optind++;
            else if(argv[optind][0] != '-')         optind++;
            else if((argv[ optind][0] == '-') && (strlen(argv[ optind]) == 1)) optind++;
            break;
        }
    }

    // Validate Command line options
    //////////////////////////////////////////////
    if( m_channelCnt == 0)  m_channelCnt = 8;
    if( m_convertTime < 0)  m_convertTime = CHANNEL_TIME;

    Log( LOG_FN_ENTRY, "Exit ParseCommandLine\n");
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void PlcMgr::CleanupManager()
{
    int delayTime = m_convertTime * 4;

    Log( LOG_FN_ENTRY, "Enter CleanupManager, %d\n", delayTime);

    ReportTaskEnding();

    ClosePlcBoards();

    // Stop our scan timer (no more pulses to our threads)
    if( m_scanTimerId != -1)
    {
        BposStopTimer( m_scanTimerId);
        delay( delayTime);
        BposDeleteTimer( m_scanTimerId);
        m_scanTimerId = -1;
    }

    // If previous timer connection not detached
    if( m_timerConnectId != -1)
    {
        BposConnectDetach( m_timerConnectId);
        m_timerConnectId = -1;
    }

    // Detach input namespace path
    if( m_inDevNo != -1)
    {
        PathDetach( m_inDevNo);
        m_inDevNo = -1;
    }

    // Detach output namespace path
    if( m_outDevNo != -1)
    {
        PathDetach( m_outDevNo);
        m_outDevNo = -1;
    }

    Log( "Detaching PLC_TIMER_PULSE\n");
    pulse_detach( m_dispatchPtr, PLC_TIMER_CODE, 0);
    Log( "PLC_TIMER_PULSE detached\n");

    // Base class cleanup
    ResManager::CleanupManager();
    delay( delayTime);

    // Detach the "write channels" pulse
    if( PlcMgr::m_writeChCoid != -1)
    {
        ConnectDetach( PlcMgr::m_writeChCoid);
        PlcMgr::m_writeChCoid = -1;
    }

    // Free memory used to buffer outputs
    if( m_toPlc != NULL)
    {
        delete[] m_toPlc;
        m_toPlc = NULL;
    }

    // No need to delete (it just points into the m_toPlc buffer)
    m_fromPlc = NULL;

    // Destroy read/write lock
    pthread_rwlock_destroy( &m_vmeSpaceLock);

    Log( LOG_FN_ENTRY, "Exit CleanupManager\n");

}

/**
 * Reads configuration options from an XML document node
 *
 * @param setupNode Parent node from which option data will be read
 * @return 0 if successful
 *         -1 if error
 */
int PlcMgr::ReadXmlOptions( const XmlElement *setupNode)
{
    int     retVal = 0;
    const XmlNodeMap &childNodes = setupNode->getChildren();
    XmlNode     *xmlNode;

    // Clear counters for number of words to transfer
    m_fromPlcCount = 0;
    m_toPlcCount = 0;

    if( (retVal=ResManager::ReadXmlOptions( setupNode)) == 0)
    {
        try
        {
            xmlNode = (XmlNode*)childNodes.getNode( XmlString( CFG_CHNL_TIME_TAG));

            m_convertTime = BposReadInt( xmlNode->getValue().c_str());
        }
        catch( XmlException &err)
        {
            m_convertTime = 25;
        }
        Log( LOG_DEV_DATA, "Conversion time: %ld ms\n", m_convertTime);

    }

    return( retVal);
}

/**
 * Reads a data item's attributes from the given node
 *
 * @param itemNode Node to read data item properties from
 * @param dataItem Data Item property structure to fill in
 */
void PlcMgr::ReadDataMapItem( const XmlNode *itemNode, dataMapItem_t &dataItem)
{
    std::string itemTag;
    uint32_t    realIdx;

    // Let the base class do all the work
    ResManager::ReadDataMapItem( itemNode, dataItem);

    realIdx = dataItem.channelIdx + 1;

    // Keep count of number of inputs and outputs
    if( dataItem.portDir == PORT_DIR_IN)
    {
        if( realIdx > m_fromPlcCount)                       m_fromPlcCount = realIdx;
        if( dataItem.boardIdx >= m_inCountList.size())      m_inCountList.resize(dataItem.boardIdx+1);
        if( realIdx > m_inCountList[ dataItem.boardIdx])    m_inCountList[ dataItem.boardIdx] = realIdx;
    }
    if( dataItem.portDir == PORT_DIR_OUT)
    {
        if( realIdx > m_toPlcCount)                         m_toPlcCount = realIdx;
        if( dataItem.boardIdx >= m_outCountList.size())     m_outCountList.resize(dataItem.boardIdx+1);
        if( realIdx > m_outCountList[ dataItem.boardIdx])   m_outCountList[ dataItem.boardIdx] = realIdx;
    }
}

/**
 * Adds a published data item to the published item maps
 *
 * @param dataItem Data item attributes
 * @param itemTag  Tag of the data item
 */
void PlcMgr::AddPublishedDataItem( const dataMapItem_t *dataItem, const dataTag_t &itemTag)
{
    dataMapItem_t   newItem( *dataItem);

    // Compensate input channel number
    if( dataItem->portDir == PORT_DIR_IN)
    {
        // Compensate chann ID by the number of inputs
        newItem.channelIdx += m_toPlcCount;
    }

    ResManager::AddPublishedDataItem( &newItem, itemTag);
}


/**
 * Reads the list of diagnostic items from the config file's DIAG node
 *
 * @param diagNode Parent node of the nodes defining diagnostic items
 */
void PlcMgr::ReadDiagMap( const XmlNode *diagNode)
{
    const XmlNodeMap&   diagNodes = diagNode->getChildren();
    XmlNodeMapCItr      diagItr;
    XmlString           dataTag;

    Log( LOG_FN_ENTRY, "Enter PlcMgr::ReadDiagMap( %s)\n", diagNode->getName().c_str());

    Log( LOG_FN_ENTRY, "Diag Node has %d children\n", diagNode->getChildren().size());
    // Clear current diag entries
    m_diagList.clear();

    // Loop through all diag nodes
    for( diagItr=diagNodes.begin(); diagItr!=diagNodes.end(); diagItr++)
    {
        // Get pointer to next node
        const XmlNode* diagItem = diagItr->second;

        try
        {
            // Read diagnostic data stuff
            PlcDiagItem newItem = PlcDiagItem::ReadXmlItem( diagItem, dataTag);

            // Look up properties of the data item associated with the diag item
            const dataMapItem_t* dataItem = m_dataTable.find( dataTag);
            if( dataItem != NULL)
            {
                // Tell the fault server who is publiching the fault
                newItem.SetProcessName( GetProcessName());

                // Calculate the diag item's channel ID
                uint64_t chId = CalculateDataTagIndex( dataItem);
                if( m_diagList.find( chId) != m_diagList.end())
                {
                    Log( LOG_ERRORS, "ERROR: Duplicate diag %lld: %s mapped to %s\n",
                          chId, newItem.GetFaultTag().c_str(), dataTag.c_str());
                }
                // Add diag item to the diag list
                m_diagList.insert( PlcDiagMapVal_t( chId, newItem));

                Log( LOG_DEV_DATA, "Added Diag item '%s', Level %d, Min %d, Max %d\n",
                     newItem.GetFaultTag().c_str(), newItem.GetLevel(),
                     newItem.GetMin(), newItem.GetMax());
            }
            else
            {
                Log( LOG_ERRORS, "\tERROR: Data tag %s not found for diag item %s\n",
                     dataTag.c_str(), newItem.GetFaultTag().c_str());
            }
        }
        catch( XmlException &err)
        {
            Log(LOG_ERRORS, "Bad Diag Item - Reason:%s", err.what());
        }
        catch( exception &er)
        {
            Log(LOG_ERRORS, "Really Bad Diag Item - Reason:%s", er.what());
        }
        catch(...)
        {
            Log(LOG_ERRORS, "Something horrible happened, but we don't know what\n");
        }
    }
    Log( LOG_FN_ENTRY, "Diag Map has %d items\n", m_diagList.size());


    Log( LOG_FN_ENTRY, "Exit PlcMgr::ReadDiagMap( %s)\n", diagNode->getName().c_str());
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool PlcMgr::InitManager()
{
    bool    retVal = true;

    Log( LOG_FN_ENTRY, "Enter PlcMgr::InitManager\n");
    try
    {
        // Perform base class initialization
        if( (retVal=ResManager::InitManager()) == true)
        {
            // Initialize rwlock used for vme read/writes
            pthread_rwlock_init( &m_vmeSpaceLock, NULL);

            // Fill in list of published data tags
            InitializeIndexedTags();

            // Adopt proper path
            m_resMgrId = PathAttach( PLC_PRFX_ROOT);

            // Get a context block for our manager
            RegisterManager();

            PlcMgr::m_writeChCoid = m_mssgCoid;

            // Connect our timer to the resource manager I/O channel
            m_scanTimerId = ConnectTimerPulse( PLC_TIMER_CODE,
                                               PLC_TIMER_VALUE,
                                               ResManager::PulseHandler);
            if( m_scanTimerId != -1)
            {
                if( m_waitForRegister == false)
                {
                    Register();
                }
            }
            else
            {
                Log( LOG_ERRORS, "\tError attaching timer to I/O channel: %s\n", strerror( errno));
                retVal = false;
            }

        }
        else
        {
            Log( LOG_ERRORS, "\tError in base class InitManager\n");
            retVal = false;
        }
    }
    catch( BepException &err)
    {
        Log( LOG_ERRORS, "\tInitManager error: %s\n", err.GetReason());
        retVal = false;
    }
    Log( LOG_FN_ENTRY, "Exit  PlcMgr::InitManager\n");

    return( retVal);
}

/**
 * method for component initialization.
 *
 * @param document Reference to a parsed configuration document.
 */
void PlcMgr::Initialize(const XmlNode *document)
{
    Log( LOG_FN_ENTRY, "Enter PlcMgr::Initialize\n");

    // Make sure all existing PLC boards are properly closed
    ClearBoardList();

    m_needFirstPublish = true;

    // Initialize base class
    ResManager::Initialize( document);

    // Load the additional configuration items needed by the PLC manager
    LoadAdditionalConfigurationItems(document);

    Log( LOG_FN_ENTRY, "Exit PlcMgr::Initialize\n");
}

void PlcMgr::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    // Read Diag definitions
    ReadDiagMap( document->getChild( XML_T("DiagnosticMap")));

    // Document is the root node of the XML document, so get the Setup specific node
    try
    {
        // Get the list of boards in the system
        const XmlNodeMap&   boardList = document->getChild( XML_T( "Setup/PlcBoards"))->getChildren();

        InitializePlcBoards( boardList);
    }
    catch( XmlException &err)
    {
        Log( LOG_ERRORS, "\tError initializing PLC board objects: %s\n",err.GetReason());
        throw;
    }
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string PlcMgr::Register(void)
{
    std::string         retVal( BEP_FAILURE_RESPONSE);


    Log( LOG_FN_ENTRY, "Enter PlcMgr::Register()\n");

    // Open the PLC Boards
    if( OpenPlcBoards() == true)
    {
        // Start our scan timer
        if( m_scanTimerId != -1)
        {
            // Read current data values
            ReadChannels();

            // setup our repetative timer to start after 5s with a repeat of m_convertTime ms
            BposStartTimer( m_scanTimerId, 1, m_convertTime);
            Log( LOG_DEV_DATA, "Timer %d started at %d ms\n", m_scanTimerId, m_convertTime);

            try
            {
                retVal = ResManager::Register();
            }
            catch( BepException &err)
            {
                Log( LOG_ERRORS, "\tError registering base class: %s\n", err.GetReason());
            }
        }
        else
        {
            Log( LOG_ERRORS, "\tError attaching timer to I/O channel: %s\n", strerror( errno));
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tError opening PLC board objects\n");
    }
    Log( LOG_FN_ENTRY, "Exit PlcMgr::Register( %s)\n", retVal.c_str());


    return( retVal);
}

/**
 * Instantiates and intializes IPlc objects for each PLC board listed
 * in the given XmlNodeMap( taken from PLC config file)
 *
 * @param boardList List of XML nodes describing PLC boards in the system
 */
void PlcMgr::InitializePlcBoards( const XmlNodeMap &boardList)
{
    IPlc            *plcBoard;
    XmlNodeMapCItr  plcItr;
    bool            boardActive;
    std::string     typeTag, nodeTag;
    uint32_t        boardIdx, totalCountTo=0, totalCountFrom=0;

    Log( LOG_FN_ENTRY, "Enter PlcMgr::InitializePlcBoards()\n");

    // Iterate through all the board specifier nodes and instantiate PLC objects to handle them
    for( plcItr=boardList.begin(); plcItr!=boardList.end(); plcItr++)
    {
        const XmlNode *plcNode = plcItr->second;
        const XmlNodeMap& boardAttr = plcNode->getAttributes();
        try
        {
            nodeTag = XmlToAscii( plcNode->getName());
            boardActive = false;
            // Check if the active attribute for this board is set to Yes
            if( boardAttr.getNode( XML_T("active"))->getValue() == XmlString( XML_T("Yes")))
            {
                // Say board is active
                boardActive = true;
                // Read the board type identifier
                typeTag = XmlToAscii( boardAttr.getNode( XML_T( "type"))->getValue());
                Log( LOG_DEV_DATA, "PLC Board '%s' is active (type='%s')\n",
                     nodeTag.c_str(), typeTag.c_str());
            }
        }
        catch( XmlException &err)
        {
            // Assume board is not active
            boardActive = false;
        }

        // If this PLC board is active
        if( boardActive == true)
        {
            try
            {
                const XmlNode *idxNode = boardAttr.getNode( XML_T("index"));
                // Check if the active attribute for this board is set to Yes
                boardIdx = BposReadInt( XmlToAscii(idxNode->getValue()).c_str());
            }
            catch( XmlException &err)
            {
                // If board is supposed to be active
                if( boardActive)
                {
                    Log( LOG_ERRORS, "\tERROR: Missing board index attribute for PLC Board '%s' (type '%s')\n",
                         nodeTag.c_str(), typeTag.c_str());
                    boardActive = false;
                }
                boardIdx = 0;
            }


            // If this board index falls outside the current list bounds
            if( boardIdx > m_plcBoards.size())
            {
                // Resize the list and fill new entries with NULL
                m_plcBoards.resize( boardIdx, NULL);
            }
            // Make sure we dont overwrite an existing valid entry in the list
            else if( m_plcBoards.size() > 0)
            {
                if( m_plcBoards[ boardIdx] != NULL)
                {
                    Log( LOG_ERRORS, "\tERROR: Duplicate PLC board index (%d) for board '%s' (type '%s')\n",
                         boardIdx, nodeTag.c_str(), typeTag.c_str());
                    continue;
                }
            }

            try
            {
                // Create a new PLC object to handle this board
                plcBoard = CreatePlcObject(typeTag);
                // Intialize logging interface of the PLC object
                plcBoard->Initialize( *this);
                // Initialize PLC attributes
                plcBoard->Initialize( plcNode);
                // Add this PLC object to the list
                m_plcBoards.push_back( plcBoard);
            }
            catch( XmlException &err)
            {
                Log( LOG_ERRORS, "\tError initializing '%s' type PLC object: %s\n",
                     nodeTag.c_str(), err.GetReason());
            }
            catch( BepException &err)
            {
                Log( LOG_ERRORS, "\tError creating PLC object '%s' (type '%s'): %s\n",
                     nodeTag.c_str(), typeTag.c_str(), err.GetReason());
            }
            //totalCountTo += plcBoard->GetToPlcCount();
            //totalCountFrom += plcBoard->GetFromPlcCount();

            totalCountTo += m_inCountList[ boardIdx];
            totalCountFrom += m_outCountList[ boardIdx];

        }
    }

    // Go back throug\ the board list and remove NULL entries (in case of
    //  non-sequential board entries
    PlcBoardListItr_t   itr;
    for( itr=m_plcBoards.begin(); itr!=m_plcBoards.end(); itr++)
    {
        IPlc    *plc = *itr;
        if( plc == NULL)
        {
            m_plcBoards.erase( itr);
            itr = m_plcBoards.begin();
        }
    }

    Log( LOG_DEV_DATA, "Number of outputs to PLC: %d\n", totalCountFrom);
    Log( LOG_DEV_DATA, "Number of inputs from PLC: %d\n", totalCountTo);
    if( totalCountFrom > m_fromPlcCount)    m_fromPlcCount = totalCountFrom;
    if( totalCountTo > m_toPlcCount)        m_toPlcCount = totalCountTo;

    // Calculate the total number of channels in the system (all boards)
    uint32_t tempChnlCnt = totalCountFrom + totalCountTo;
    if( tempChnlCnt > m_channelCnt)
    {
        m_channelCnt = tempChnlCnt;
    }

    // Allocate buffer big enough for all words (can read all words)
    Log( LOG_DEV_DATA, "Allocating %d channels in array\n", m_channelCnt);
    m_toPlc = new uint32_t[ m_channelCnt];
    memset( m_toPlc, 0, m_channelCnt*sizeof( uint32_t));
    // Use second "half" of data buffer for writeable data items
    Log( LOG_DEV_DATA, "Pointing m_fromPlc at channel %d\n", m_toPlcCount);
    m_fromPlc = &(m_toPlc[ m_toPlcCount]);

    Log( LOG_FN_ENTRY, "Exit PlcMgr::InitializePlcBoards()\n");
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
 * @return the device number associated with the path.
 */
int PlcMgr::PathAttach( const char *pathToAdopt)
{
    string  path( pathToAdopt);

    Log( LOG_FN_ENTRY, "Enter PathAttach( %s)\n", pathToAdopt);

    AttachInputPath( path);

    AttachOutputPath( path);

    Log( LOG_FN_ENTRY, "Exit PathAttach( %s)\n", pathToAdopt);

    return( m_inDevNo);
}


/**
 * Attaches path name for PLC inputs (my inputs from PLC)
 *
 * @param path   Root path to add oour input name to
 * @return Device number for the adopted path
 */
int PlcMgr::AttachInputPath( const string &path)
{
    string  inPath = path + "/"PLC_INPUT_PATH;
    int     inSize = m_fromPlcCount * sizeof( uint32_t);
    int     mode = 0666;

    // Initialize this func attrib struct
    iofunc_attr_init( &m_inFuncAttr, S_IFNAM|mode, 0, 0);
    m_inFuncAttr.inode = m_toPlcInode;
    m_inFuncAttr.nbytes = inSize;
    m_inFuncAttr.mount = &m_mountPoint;

    m_inDevNo = ResManager::PathAttach( inPath.c_str(), &m_inFuncAttr);

    if(m_inDevNo == -1)
    {
        inPath = string( "resmgr_attach '") + inPath + string( "failed: ");
        ThrowOsException( errno, inPath.c_str());
    }
    else
    {
        Log( LOG_DEV_DATA, "path %s attached\n", inPath.c_str());
    }

    return( m_inDevNo);
}

/**
 * Attaches path name for PLC outputs (my outputs to PLC)
 *
 * @param path   Root path to add oour input name to
 * @return Device number for the adopted path
 */
int PlcMgr::AttachOutputPath( const string &path)
{
    string  outPath = path + "/"PLC_OUTPUT_PATH;
    int     outSize = m_toPlcCount * sizeof( uint32_t);
    int     mode = 0444;

    // Initialize this func attrib struct
    iofunc_attr_init( &m_outFuncAttr, S_IFNAM|mode, 0, 0);
    m_outFuncAttr.inode = m_toPlcInode + 1;
    m_outFuncAttr.nbytes = outSize;
    m_outFuncAttr.mount = &m_mountPoint;

    m_outDevNo = ResManager::PathAttach( outPath.c_str(), &m_outFuncAttr);
    if(m_outDevNo == -1)
    {
        outPath = string( "resmgr_attach '") + outPath + string( "failed: ");
        ThrowOsException( errno, outPath.c_str());
    }
    else
    {
        Log( LOG_DEV_DATA, "path %s attached\n", outPath.c_str());
    }


    return( m_outDevNo);
}

/**
 * Reads all PLC ports on all PLC boards in the system
 */
void PlcMgr::ReadChannels()
{
    PlcBoardListItr_t   itr;
    int                 boardIdx=0;
    uint32_t            portIdx;
    uint32_t            newVal;

    if( pthread_rwlock_rdlock( &m_vmeSpaceLock) == EOK)
    {
        // Loop through each board in the system
        for( itr=m_plcBoards.begin(), boardIdx=0; itr!=m_plcBoards.end(); itr++, boardIdx++)
        {
            // Get reference to next board in the system
            IPlc    *plcBoard = *itr;

            // See how many inputs this board has
            uint32_t inCount = plcBoard->GetFromPlcCount();

            // Read inputs from PLC
            for( portIdx=0; portIdx<inCount; portIdx++)
            {
                newVal = plcBoard->ReadChannel( portIdx);

                // If this is the first time that we have valid subscriptions
                if( (m_haveSubscriptions == true) && (m_needFirstPublish == true))
                {
                    // Make sure we publish new data
                    m_fromPlc[ portIdx] = newVal ^ 0xFF;
                }

                // If value has changed
                if( newVal != m_fromPlc[ portIdx])
                {
                    // See if anybody subscribed for changes
                    CheckPublish(boardIdx, portIdx+m_toPlcCount, newVal, m_fromPlc[ portIdx]);
                    m_fromPlc[ portIdx] = newVal;
                }
            }
            // Check outputs for changes
            for( portIdx=0; portIdx<m_toPlcCount; portIdx++)
            {
                newVal = plcBoard->ReadChannel( portIdx, PORT_DIR_OUT);

                // If this is the first time that we have valid subscriptions
                if( (m_haveSubscriptions == true) && (m_needFirstPublish == true))
                {
                    // Make sure we publish new data
                    m_toPlc[ portIdx] = newVal ^ 0xFF;
                }

                // If value has changed
                if( newVal != m_toPlc[ portIdx])
                {
                    // See if anybody subscribed for changes
                    CheckPublish(boardIdx, portIdx, newVal, m_toPlc[ portIdx]);
                    m_toPlc[ portIdx] = newVal;
                }
            }
        }

        // If this is the first time that we had valid subscriptions
        if( (m_haveSubscriptions == true) && (m_needFirstPublish == true))
        {
            Log( LOG_DEV_DATA, "Performed initial publish after m_haveSubscriptions == true\n");

            // Clear the 'need to publish' flag
            m_needFirstPublish = false;
        }

        pthread_rwlock_unlock( &m_vmeSpaceLock);
    }
}

/**
 * Writes all PLC outputs
 */
void PlcMgr::WriteChannels()
{
    PlcBoardListItr_t   itr;
    uint32_t            portIdx, outCount;

    if( pthread_rwlock_wrlock( &m_vmeSpaceLock) == EOK)
    {
        // Loop through each board in the system
        for( itr=m_plcBoards.begin(); itr!=m_plcBoards.end(); itr++)
        {
            // Get reference to next board in the system
            IPlc    *plcBoard = *itr;

            outCount = plcBoard->GetToPlcCount();
            for( portIdx=0; portIdx<outCount; portIdx++)
            {
                plcBoard->WriteChannel( portIdx, m_toPlc[ portIdx]);
            }
        }

        pthread_rwlock_unlock( &m_vmeSpaceLock);
    }
}

/**
 * Check if a data item has changed and publish the new value to subscribers
 *
 * @param boardIdx Board on which the data item is located
 * @param portIdx  Channel on which the data item is ocated
 * @param newVal   Bew value for the given channel
 * @param oldVal   Old data value
 */
void PlcMgr::CheckPublish( int boardIdx, int portIdx, uint32_t newVal, uint32_t oldVal)
{
    uint32_t    changed = newVal ^ oldVal; // what bits have changed
    uint32_t    itemVal;

    // if any bits have changed and the notification list exists
    if( changed)
    {
        // If this data item is defined as the entire channel
        if( IsWholeChannelItem( boardIdx, portIdx) == true)
        {
            WholeChannelItemMapItr_t    itr = m_wholeChannelItemMap.end();
            itr = m_wholeChannelItemMap.Find( boardIdx, portIdx, itr);
            const dataTag_t &itemTag = itr->second;

            Log( LOG_DEV_DATA, "Publishing whole channel item '%s': board %d channel %d value $%04X\n",
                 itemTag.empty() == false ? itemTag.c_str() : " ", boardIdx, portIdx, newVal);

            // Publish value for channel as a whole first
            PublishData( boardIdx, portIdx, -1, newVal);

            // Publish fault data
            PublishFault(  boardIdx, portIdx, -1, newVal);
        }

        // evaluate each bit for this port
        uint32_t    bitMask;
        int bitIdx;
        int bitsPerChannel;
        if( (unsigned)boardIdx < m_plcBoards.size())
        {
            bitsPerChannel = m_plcBoards[boardIdx]->GetBitsPerChannel();
        }
        else
        {
            bitsPerChannel = 16; // set default
        }

        for( bitIdx=0; bitIdx<bitsPerChannel; bitIdx++)
        {
            // set bit to Look at next
            bitMask  =  uint32_t(1) << bitIdx;

            // If this bit has changed
            if( changed & bitMask)
            {
                // based on proper bit - set itemVal to either 1 or 0
                itemVal = ((newVal & bitMask) ? uint32_t(1) : uint32_t(0));

                Log( LOG_DEV_DATA, "Publishing bit %d board %d channel %d value $%04X\n",
                    bitIdx, boardIdx, portIdx, itemVal);

                // Publish value for this bit
                PublishData( boardIdx, portIdx, bitIdx, itemVal);

                // Publish fault data
                PublishFault(  boardIdx, portIdx, bitIdx, itemVal);
            }
        }
    }
}

/**
 * Publiched a fault value for the given data item
 *
 * @param brdIdx   Index of the PLC board
 * @param chnlIdx  Channel index
 * @param bitIdx   Bit index
 * @param faultVal Fault value to publish
 */
void PlcMgr::PublishFault( int brdIdx, int chnlIdx, int bitIdx, uint32_t faultVal)
{
    uint64_t            itemIdx;
    PlcDiagMapItr_t     diagItr;

    // Calculate the data tag for this item
    itemIdx = CalculateDataTagIndex( brdIdx, chnlIdx, bitIdx);
    // If we have a diag item mapped to this data item
    if( (diagItr=m_diagList.find( itemIdx)) != m_diagList.end())
    {
        PlcDiagItem &diagItem = diagItr->second;
        diagItem.CheckTrigger( faultVal, m_faultServer);
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
uint64_t PlcMgr::CalculateDataTagIndex( const dataMapItem_t *dataItem)
{
    uint16_t channel = dataItem->channelIdx;

    // IF this is an output
    if( dataItem->portDir == PORT_DIR_IN)
    {
        // Compensate chann ID by the number of inputs
        channel += m_toPlcCount;
    }

    return( CalculateDataTagIndex( dataItem->boardIdx, channel, dataItem->bitIdx));
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
uint64_t PlcMgr::CalculateDataTagIndex( int board, int channel, uint16_t bit)
{
    uint64_t retVal;

    // Calculate channel index for data item

    if( (unsigned)board < m_plcBoards.size())
    {
        retVal = (m_plcBoards[board]->GetBitsPerChannel()) * channel;
    }
    else
    {
        retVal = 16 * channel; // use default
    }

    // If data item is not a "whole channel" item
    if( bit != (uint16_t )-1)
    {
        // Offset by bit position
        retVal += bit;
    }
    else
    {
        // MSB set for whole channel item (so not confused with bit index = 0)
        retVal |= 0x8000000000000000LL;
    }

    return( retVal);
}

/**
 * Opens all PLC boards in the system
 *
 * @return true if all boards successfully opened
 *         false if error
 */
bool PlcMgr::OpenPlcBoards()
{
    bool        retVal = true;
    uint32_t    brdIdx=0, brdCount=m_plcBoards.size();
    uint32_t    inCount, outCount;

    Log( LOG_FN_ENTRY, "Enter OpenPlcBoards( %d)\n", brdCount);

    // Open each board in the list
    for( brdIdx=0; brdIdx<brdCount; brdIdx++)
    {
        IPlc    *plcBoard = m_plcBoards[ brdIdx];

        inCount = m_inCountList[ brdIdx];
        outCount = m_outCountList[ brdIdx];
        // If board object is vaid ann there are any I/O items defined for it
        if( (plcBoard != NULL) && ((inCount > 0) || (outCount > 0)) )
        {
            // Attempt to open next PLC board
            retVal = plcBoard->Open( inCount, outCount, m_demoMode);
            if(retVal == false)
            {
                Log( LOG_ERRORS, "\tError opening PLC object %d\n", brdIdx);
                break;
            }
        }
    }

    // Clear all outputs at startup
    WriteChannels();

    Log( LOG_FN_ENTRY, "Exit OpenPlcBoards( %d)\n", m_plcBoards.size());

    return( retVal);
}

/**
 * Closes all PLC boards in the system
 */
void PlcMgr::ClosePlcBoards()
{
    PlcBoardListItr_t   plcItr;

    Log( LOG_FN_ENTRY, "Enter ClosePlcBoards( %d)\n", m_plcBoards.size());

    for( plcItr=m_plcBoards.begin(); plcItr!=m_plcBoards.end(); plcItr++)
    {
        IPlc    *plc = *plcItr;
        if( plc != NULL)    plc->Close();
    }

    Log( LOG_FN_ENTRY, "Exit ClosePlcBoards( %d)\n", m_plcBoards.size());
}

/**
 * Fills in an XmlNode with the data value for a data item
 *
 * @param dataItem Properties for the data item to read
 * @param value    String to fill in with the value of the requested data item
 * @return 0 if valueNode is filled in
 *         -1 if unable to handle the request
 */
int PlcMgr::ReadDataItem( const dataMapItem_t *dataItem, string &value)
{
    int     retVal=-1;
    uint16_t    portVal;
    char    val[ 256];

    Log( LOG_FN_ENTRY, "Enter PlcMgr::ReadDataItem()\n");

    Log( LOG_DEV_DATA, "Reading Board %d Channel %d, Bit %d\n",
         dataItem->boardIdx, dataItem->channelIdx, dataItem->bitIdx);

    // If the specified channel is valid
    if( dataItem->channelIdx < m_channelCnt)
    {
        // If specified board is valid
        if( dataItem->boardIdx < m_plcBoards.size())
        {
            // Get reference to the desired PLC board
            IPlc    *plcBoard = m_plcBoards[ dataItem->boardIdx];

            // Get desired bit and translate for desired logiv level
            portVal = plcBoard->ReadPort( dataItem->channelIdx, dataItem->bitIdx,
                                          dataItem->logicLvl, dataItem->portDir);

            // Put value into XML node's value
            sprintf( val, "%d", portVal);
            value = string( val);
            retVal = 0;
        }
        else
        {
            Log( LOG_ERRORS, "\tInvalid board index: %d >= %d\n", dataItem->boardIdx, m_plcBoards.size());
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tInvalid channel index: %d >= %d\n", dataItem->channelIdx, m_channelCnt);
    }
    Log( LOG_FN_ENTRY, "Exit PlcMgr::ReadDataItem( %s)\n", value.empty() == false ? value.c_str() : " ");

    return(retVal);
}

/**
 * Reads a data item's value from an XmlNode
 *
 * @param dataItem Properties for the data item to write to
 * @param value    String containing the value of the specified data item
 * @return 0 if new value was written
 *         -1 if unable to handle the request
 */
int PlcMgr::WriteDataItem( const dataMapItem_t *dataItem, const string &value)
{
    int     retVal = -1;
    int     portVal;

    Log( LOG_FN_ENTRY, "Enter PlcMgr::WriteDataItem()\n");

    // If the specified channel is valid
    if( dataItem->channelIdx < m_channelCnt)
    {
        // If specified board is valid
        if( dataItem->boardIdx < m_plcBoards.size())
        {
            // Get reference to the desired PLC board
            IPlc    *plcBoard = m_plcBoards[ dataItem->boardIdx];
            // Make sure the desired port is an output
            if( dataItem->portDir == PORT_DIR_OUT)
            {
                // Get value from XML node's value
                sscanf( value.c_str(), "%d", &portVal);

                Log( LOG_DEV_DATA, "Writing %d to channel %d port %d\n", portVal,
                     dataItem->channelIdx, dataItem->bitIdx);
                try
                {
                    // Write data to the port
                    plcBoard->WritePort( dataItem->channelIdx, dataItem->bitIdx,
                                        dataItem->logicLvl, portVal);
                }
                catch( BepException &err)
                {
                    Log( LOG_ERRORS, "\tError writing to port: %s\n",
                         err.GetReason());
                }

                // Check if this is a diagnostic item
                switch( CheckDiagItem( dataItem, portVal) )
                {
                case DIAG_INF:
                    Log( LOG_DEV_DATA, "Published diag inf on board %d channel %d bit %d: %s\n",
                         dataItem->boardIdx, dataItem->channelIdx, dataItem->bitIdx, value.c_str());
                    break;
                case DIAG_WARN:
                    Log( LOG_DEV_DATA, "Published diag warn on board %d channel %d bit %d: %s\n",
                         dataItem->boardIdx, dataItem->channelIdx, dataItem->bitIdx, value.c_str());
                    break;
                case DIAG_FATAL:
                    Log( LOG_DEV_DATA, "Published diag fatal on board %d channel %d bit %d: %s\n",
                         dataItem->boardIdx, dataItem->channelIdx, dataItem->bitIdx, value.c_str());
                    break;
                case DIAG_NONE:
                default:
                    break;

                }

                retVal = 0;
            }
        }
    }
    Log( LOG_FN_ENTRY, "Exit PlcMgr::WriteDataItem()\n");

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
 * @see ResManager::ioDevCtl
 */
int PlcMgr::IoDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb)
{
    int             status;

    Log( LOG_FN_ENTRY, "Enter IoDevCtl\n");
    switch( msg->i.type)
    {
        /*****************************************
            Add stuff for setting gains/offsets
        *****************************************/
    default:
        status = iofunc_devctl_default( ctp, msg, (iofunc_ocb_t*)ioOcb);
        break;
    }
    Log( LOG_FN_ENTRY, "Exit IoDevCtl\n");

    return( status);
}


/**
 * Message handler for _IO_READ I/O messages. Called by ResManager::ioRead
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Read message data
 * @param ioOcb  Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManager::ioRead
 */
int PlcMgr::IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb)
{
    IOFUNC_ATTR_T   *ioAttr = (IOFUNC_ATTR_T*)(ioOcb->m_ioOcb.attr);
    off_t   off, nleft, nbytes;
    int     retVal = ENOSYS;
    char    *dataPtr;

    Log( LOG_FN_ENTRY, "Enter IoRead\n");

    switch( msg->i.xtype & _IO_XTYPE_MASK)
    {
    case _IO_XTYPE_OFFSET:
        struct _xtype_offset *xoff;
        xoff = (struct _xtype_offset *)(&msg->i + 1);
        off = xoff->offset;
        retVal = EOK;
        break;
    default:
    case _IO_XTYPE_NONE:
        off = ioOcb->m_ioOcb.offset;
        retVal = EOK;
        break;
    }

    // If client has read privileges
    if( retVal == EOK)
    {
        if( ioAttr->inode == m_toPlcInode)  dataPtr = (char*)m_toPlc;
        else                                dataPtr = (char*)m_fromPlc;

        nleft = ioOcb->m_ioOcb.attr->nbytes - off;
        nbytes = min(  nleft, msg->i.nbytes);
        Log( LOG_IO_MSSG, "Reading %d bytes at offset %d\n", nbytes, off);
        if( nbytes > 0)
        {
            ((iofunc_attr_t*)(ioOcb->m_ioOcb.attr))->flags |= IOFUNC_ATTR_ATIME |
                                                                IOFUNC_ATTR_DIRTY_TIME;

            MsgReply( ctp->rcvid, nbytes, dataPtr+off, nbytes);

            if( (msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_OFFSET)
            {
                ioOcb->m_ioOcb.offset += nbytes;
            }
        }
        else
        {
            MsgReply( ctp->rcvid, EOK, NULL, 0);
            nbytes = 0;
        }

        _IO_SET_READ_NBYTES( ctp, nbytes);
        retVal = _RESMGR_NOREPLY;
    }

    Log( LOG_FN_ENTRY, "Exit IoRead\n");

    return( retVal);
}


/**
 * Message handler for _IO_WRITE I/O messages. Called ny ResManager::ioWrite
 *
 * @param ctp    Dispatch Interface pointer
 * @param msg    Write message data
 * @param ioOcb  Control Block for this client
 * @return 0 if successful
 *         <0 if error occurs
 * @see ResManager::ioWrite
 */
int PlcMgr::IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb)
{
    off_t           off, nleft, nbytes;
    int             retVal = ENOSYS;
    int             dataOffset=sizeof( msg->i);
    char            *dataBuff = (char*)m_toPlc;

    Log( LOG_FN_ENTRY, "Enter IoWrite\n");

    // Check if this is  an XTYPE override
    switch( msg->i.xtype & _IO_XTYPE_MASK)
    {
    case _IO_XTYPE_OFFSET:
        struct _xtype_offset *xoff;
        xoff = (struct _xtype_offset *)(&msg->i + 1);
        off = xoff->offset;
        dataOffset += sizeof( *xoff);
        retVal = EOK;
        break;
    default:
    case _IO_XTYPE_NONE:
        off = ioOcb->m_ioOcb.offset;
        retVal = EOK;
        break;
    }

    // If ok to do the write operation
    if( retVal == EOK)
    {
        // Calculate how much "buffer" is left
        nleft = ioOcb->m_ioOcb.attr->nbytes - off;
        nbytes = min(  nleft, msg->i.nbytes);
        // If room left in buffer
        if( nbytes > 0)
        {
            Log( LOG_IO_MSSG, "\tWriting %d outputs @ offset %d\n", nbytes / sizeof( uint32_t), off);
            // Read rest of the write message data
            resmgr_msgread( ctp, dataBuff + off, nbytes, dataOffset);

            // Tell kernel to update last modification time
            ((iofunc_attr_t*)(ioOcb->m_ioOcb.attr))->flags |= IOFUNC_ATTR_MTIME |
                                                                IOFUNC_ATTR_DIRTY_TIME;

            // Advance the offset inside the OCB structure
            if( (msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_OFFSET)
            {
                ioOcb->m_ioOcb.offset += nbytes;
            }
        }

        // Tell the client how many bytes were written
        _IO_SET_WRITE_NBYTES( ctp, nbytes);

        // Send us a pulse to update VME space
        WriteChannels();
        //SendUpdateOutputsPulse();

        retVal = EOK;
    }

    Log( LOG_FN_ENTRY, "Exit IoWrite\n");

    return( retVal);
}

/**
 * Handles pulses delivered to our resource manager I/O channel
 *
 * @param pulse  Pulse received on the I/O channel
 * @return 0 if successful; -1 if error; 1 if no further processing is required
 */
int PlcMgr::HandleResMgrPulse(io_pulse_t &pulse)
{
    int retVal;

    // If we got a pulse
    if( m_xmlCommand != RES_MGR_CMD_TERMINATE)
    {
        if( pulse.code == PLC_TIMER_CODE)
        {
            switch( pulse.value.sival_int)
            {
            case PLC_TIMER_VALUE:
                try
                {
                    ReadChannels();
                }
                catch( ...)
                {

                }
                break;
            case PLC_WRITE_VALUE:
                Log( LOG_DEV_DATA, "WriteChannels\n");
                WriteChannels();
                break;
            default:
                break;
            }
        }
    }

    retVal = ResManager::HandleResMgrPulse( pulse);

    return( retVal);
}

/**
 * Will handle pulse if overriden.
 *
 * @param code   Pulse code
 * @param value  Pulse value
 * @return If pulse is handled.
 *         Handled     == BEP_STATUS_SUCCESS
 *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
 */
const INT32 PlcMgr::HandlePulse(const INT32 code, const INT32 value)
{
    INT32   retVal = BEP_STATUS_FAILURE;

    if( code == PLC_TIMER_CODE)
    {
        retVal = BEP_STATUS_SUCCESS;
        switch( value)
        {
        case PLC_TIMER_VALUE:
            try
            {
                ReadChannels();
            }
            catch( ...)
            {

            }
            break;
        case PLC_WRITE_VALUE:
            WriteChannels();
            break;
        default:
            retVal = BEP_STATUS_FAILURE;
            break;
        }
    }
    // if this is a reload configuration pulse
    else if((code == SYSTEM_PULSE) && (value == RELOAD_PULSE))
    {
        // Make sure all existing PLC boards are properly closed
        ClearBoardList();

        m_needFirstPublish = true;

        if(m_fromPlc != NULL)
        {
            delete m_fromPlc;
            m_fromPlc = NULL;
        }

        if(m_toPlc != NULL)
        {
            delete m_toPlc;
            m_toPlc = NULL;
        }

        const XmlNode *document =
            m_parser.ReturnXMLDocument(GetConfigFile());

        // load the additional configuration items needed by the
        // vehicle data server
        LoadAdditionalConfigurationItems(document);

        retVal = BEP_STATUS_SUCCESS;
    }
    else
    {
        retVal = BepComponent::HandlePulse(code, value);
    }

    return( retVal);
}

/**
 * Clears the IPlc objects from our PLC board list and frees the memory
 * allocated for each IPlc object
 */
void PlcMgr::ClearBoardList()
{
    PlcBoardListItr_t   plcItr;

    for( plcItr=m_plcBoards.begin(); plcItr!=m_plcBoards.end(); plcItr++)
    {
        IPlc    *plc = *plcItr;
        if( plc != NULL)
        {
            delete plc;
        }
    }
    m_plcBoards.clear();

}

/**
 * Instantiates a new IPlc interface object based on a type identifier
 * string
 *
 * @param boardTag PLC type identifier string (i.e. "Df1") used to determine
 *                 which type of IPlc object to instantiate
 * @return Pointer to a new IPlc object
 * @exception BepException
 *                   If the board type identifier is unrecognized
 */
IPlc* PlcMgr::CreatePlcObject( const std::string &boardTag) throw( BepException)
{
    IPlc    *plcBoard = NULL;

    if( boardTag == "Df1")
    {
        plcBoard = new PlcDf1;
    }
    else if( boardTag == "Rk512")
    {
        plcBoard = new PlcRk512;
    }
    else if(boardTag == "Melsec")
    {
        plcBoard = new PlcMelsec;
    }
    else if(boardTag == "Eip")
    {
        plcBoard = new PlcEip;
    }

    if( plcBoard == NULL)
    {
        std::string errStr( "Unrecognized PLC board type identifier: ");
        errStr += boardTag;
        throw( BepException( errStr));
    }
    return( plcBoard);
}

