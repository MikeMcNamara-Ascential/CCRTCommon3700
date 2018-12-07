//******************************************************************************
// Description:
//  PLC I/O manager
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcMgr.h $
// 
// 5     1/18/07 2:51p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added support for Rk512 PLC communication.
// 
// 4     5/24/06 2:55p Cward
// Updated PlcMgr to allow reloading of config files without rebooting.
//
// 1     5/16/06 1:19p Rwiersem
// Made the test class a friend.  Added LoadAdditionalConfigurationItems()
// to support reloading the configuration without restarting the process.
//
// 3     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:20p Bmeinke
// Added ClosePlcBoards() method to close all PLC board objects
//
// 2     8/18/04 11:36p Cward
// Removed VME support
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
// 2     6/20/03 8:50a Khykin
// Updated for Ford changes.
//
// 1     6/19/03 2:02p Bmeinke
//
// 1     4/30/03 4:43p Bmeinke
// Migration to new personalized configuration management structure
//
// 18    3/12/03 2:10p Bmeinke
// Added Register() method because we have to wait until system
// registration until we can open the DF1 logical port
//
// 17    3/12/03 12:44a Bmeinke
// Initial machine testing for Ford Chicago
// Added m_inCountList and m_outCountList data members in order to
// maintain a list of how many I/O channels each PLC board in the system
// has
//
// 16    1/18/03 3:33p Bmeinke
// Chngaed PLC object architecture to use a pure virtual base class
// interface (allows for easier plugging of new PLC board types)
//
// 15    10/14/02 7:32p Bmeinke
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
// 14    6/27/02 7:24p Bmeinke
// Changed logic for publishing data in ResManager base class; required
// changes to publish logic here
//
// 13    4/05/02 1:57p Bmeinke
// Added CheckDiagItem() to publish/clear fault items from the
// WriteDataItem() method
//
// 12    2/19/02 7:54p Bmeinke
// Changed data types used for reading/writing VME space from UINT32 to
// UINT16
// Added data member m_toPlcInode to ensure that read() and write()
// operate on the correct PLC data area
// Added m_vmeSpaceLock to guard simultaneous read/writes to vme space in
// the PLC manager
//
// 11    11/06/01 12:27a Brian.meinke
// Changed ReadDataItem/WriteDataItem to take string references instead of
// XmlNode pointers
//
// 10    11/05/01 10:23a Brian.meinke
// Fixed arguments to ReadDataItem/WriteDataItem methods
// Added virtual declaration to all inherited virtual methods
//
// 9     10/31/01 2:09p Brian.meinke
// Added support for reading diagnostic data items from the config file
//
// 8     10/23/01 2:30p Brian.meinke
// Added method SendUpdateOutputsPulse() to send a pulse to the resource
// manager's I/O channel to indicate a need to write outputs to the PLC in
// response to a client write operation
//
// 7     10/11/01 11:32a Brian.meinke
// Replaced PlcMgr::m_timerPulseValue with PLC_TIMER_VALUE enum value
// Replaced PlcMgr::m_writePulseValue with PLC_WRITE_VALUE enum value
//
// 6     10/09/01 1:54p Brian.meinke
// Removed XmlThreadInit() method: base class initialization is sufficient
// Removed XmlHandlerFunc() method: handled by base class and
// IBepCommunication object
// Added HandlePulse() method to handle pulses on the XML message channel
// Added HandleResMgrPulse() to handle pulses delivered to the resource
// manager's I/O channel
//
// 5     10/04/01 10:26a Brian.meinke
// Integration with IInterProcessCommunication
// Changed timer pulse code from SYSTEM_PULSE to SYSTEM_PULSE+1
//
// 4     9/14/01 5:50p Brian.meinke
// Added CalculateTagIndex() methods to calculate the index of a data
// item's tag basd on board, channel and bit indices
// Added CheckPublish() method to initiate a publish operation for changed
// data items
//
// 3     9/04/01 10:11a Brian.meinke
// Removed static OcbCalloc/OcbFree methods (now handled in ResManager
// base class)
//
// 2     8/29/01 12:45a Brian.meinke
// Removed Execute method: now just uses base class ResManager
// implementation
// Removed InitHandlers (not needed with new virtual interface)
// Removed call to PathDetach( m_resMgrId) because m_resmgrId is not used
// Changed static I/O and XML handler methods to virtual methods which
// override the base class methods)
// Seperated I/O func attributes for inputs and outputs
// Removed WriteChannelPulseHandler() (not used)
//
// 1     6/21/01 1:44p Brian.meinke
// PLC Resource Manager
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _PLC_MGR_H_
#define _PLC_MGR_H_
//==============================================================================

/*******************
 Forward definitions
 ******************/
class PlcMgr;

/*******************
 include files
 ******************/
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

#include "ResManager.h"     // I/O Manager base class

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#include <stdio.h>          // perror, printf
#include <stdlib.h>         // rand
#include <string.h>         // memmove, strlen, etc
#include <time.h>           // POSIX Timers
#include <signal.h>         // POSIX signals
#include <semaphore.h>      // POSIX semaphores
#include <process.h>        // tfork
#include <sys/iofunc.h>     // Resource Manager Stuff
#include <sys/dispatch.h>   // dispatch_create
#include <sys/sched.h>      // QNX scheduler

#include "Bepos.h"          // Standard BEP wrappers
#include "IPlc.h"           // PLC Interface definition
#include "PlcDf1.h"         // Allen Bradley DF1 serial PLC board object
#include "PlcRk512.h"       // Siemens Rk512 serial PLC board object
#include "PlcMelsec.h"      // Mitsubishi Melsec serial PLC object
#include "PlcEip.h"         // Allen Bradley Df1 EIP PLC board object
#include "PlcToyopuc.h"     // Toyopuc Ethernet PLC Object
#include "PlcDiag.h"        // PLC Diagnostic Data Types
#include "PlcDefs.h"        // PLC definitions
#include "XmlParser.h"      // XML Parser definition
#include "XmlDefs.h"        // XML definitions
#include "XmlNode.h"        // XML Node definitions
#include "BepXmlUtils.h"    // XML Utilities
#include "BepException.h"   // Standard Exception class

/**
 * Pulse code to send when the scan timer expires
 */
#define PLC_TIMER_CODE  SYSTEM_PULSE+1

typedef enum
{
    /**
     * Pulse code sent when the scan timer expires
     */
    PLC_TIMER_VALUE,

    /**
     * Pulse code to send when we need to write data to the DIO ports
     */
    PLC_WRITE_VALUE

} PlcPulseValue_t;


/**
 * Data type used to hold a list of IPlc objects. Each object in this
 * list represents a physical PLC board in the system
 */
typedef vector<IPlc*>                   PlcBoardList_t;
typedef PlcBoardList_t::iterator        PlcBoardListItr_t;
typedef PlcBoardList_t::const_iterator  PlcBoardListCItr_t;

/**
 * List used to maintain the number of inputs or output words associated with
 * each PLC board in the system
 */
typedef vector<uint32_t>                IoCountList_t;
typedef IoCountList_t::iterator         IoCountListItr_t;
typedef IoCountList_t::const_iterator   IoCountListCItr_t;

/**
 * PLC Card Manager
 *
 * @author Brian Meinke
 * @version 2.0
 * @see ResManager
 */
class PlcMgr : public ResManager
{
    friend class PlcMgrTest;

public:     // PLC Specific interface
    /**
     * PLC Resource Manager. Used to handle I/O messages
     * regarding an PLC data items
     *
     * @param channelCnt Number of channels to convert (Default=32)
     * @param verbose    Desired verbose level (default=0- no printouts)
     * @param cnvTime    Time (int milliseconds) required to convert an analog channel.
     *                   Default is 20
     */
    PlcMgr( unsigned long channelCnt=256, int verbose=0, long cnvTime=CHANNEL_TIME);

    /**
     * PLC Resource Manager. Used to handle I/O messages
     * regarding an PLC data items
     *
     * @param argc   Number of arguments in the argv array
     * @param argv   Array of command line options
     */
    PlcMgr( int argc, char *argv[]);

    /**
     * Default destructor
     */
    ~PlcMgr();

    /**
     * Perform any server initialization
     *
     * @return true if successful
     *         false if error
     */
    virtual bool InitManager();

protected:

    /**
     * method for component initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Load the additional configuration items needed by the plc manager.
     * The "standard" items will be handled by the call to
     * ResManager::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         Success, Fail
     * @since Version 2.5
     */
    virtual const std::string Register(void);

    /**
     * Instantiates and intializes IPlc objects for each PLC board listed
     * in the given XmlNodeMap( taken from PLC config file)
     *
     * @param boardList List of XML nodes describing PLC boards in the system
     */
    void InitializePlcBoards( const XmlNodeMap &boardList);

private:
    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    void CleanupManager();

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
    int PathAttach( const char *pathToAdopt);

    /**
     * Attaches path name for PLC inputs (my inputs from PLC)
     *
     * @param path   Root path to add oour input name to
     * @return Device number for the adopted path
     */
    int AttachInputPath( const string &path);

    /**
     * Attaches path name for PLC outputs (my outputs to PLC)
     *
     * @param path   Root path to add oour input name to
     * @return Device number for the adopted path
     */
    int AttachOutputPath( const string &path);

    /**
     * Set data to "safe" default values
     */
    void SetDefaults();

    /**
     * Parses command line parameters
     *
     * @param argc   Number of command line arguments in the argv parameter
     * @param argv   Command line option strings
     */
    void ParseCommandLine( int argc, char **argv);

    /**
     * Reads all PLC ports on all PLC boards in the system
     */
    void ReadChannels();

    /**
     * Writes all PLC outputs
     */
    void WriteChannels();

    /**
     * Check if a data item has changed and publish the new value to subscribers
     *
     * @param boardIdx Board on which the data item is located
     * @param portIdx  Channel on which the data item is ocated
     * @param newVal   Bew value for the given channel
     * @param oldVal   Old data value
     */
    void CheckPublish( int boardIdx, int portIdx, uint32_t newVal, uint32_t oldVal);

    /**
     * Publiched a fault value for the given data item
     *
     * @param brdIdx   Index of the PLC board
     * @param chnlIdx  Channel index
     * @param bitIdx   Bit index
     * @param faultVal Fault value to publish
     */
    void PublishFault( int brdIdx, int chnlIdx, int bitIdx, uint32_t faultVal);

    /**
     * Calculates the index of the given tag in our list of published tags.
     * Index is calculated based on board index, channel index, bit index
     * and channels per board
     *
     * @param dataItem Item to calculate index for
     * @return Index for the given data item
     */
    uint64_t CalculateDataTagIndex( const dataMapItem_t *dataItem);

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
    uint64_t CalculateDataTagIndex( int board, int channel, uint16_t bit);

    /**
     * Opens all PLC boards in the system
     *
     * @return true if all boards successfully opened
     *         false if error
     */
    bool OpenPlcBoards();

    /**
     * Closes all PLC boards in the system
     */
    void ClosePlcBoards();

    /**
     * Reads configuration options from an XML document node
     *
     * @param setupNode Parent node from which option data will be read
     * @return 0 if successful
     *         -1 if error
     */
    virtual int ReadXmlOptions( const XmlElement *setupNode);

    /**
     * Reads a data item's attributes from the given node
     *
     * @param itemNode Node to read data item properties from
     * @param dataItem Data Item property structure to fill in
     */
    virtual void ReadDataMapItem( const XmlNode *itemNode, dataMapItem_t &dataItem);

    /**
     * Adds a published data item to the published item maps
     *
     * @param dataItem Data item attributes
     * @param itemTag  Tag of the data item
     */
    virtual void AddPublishedDataItem( const dataMapItem_t *dataItem, const dataTag_t &itemTag);

    /**
     * Reads the list of diagnostic items from the config file's DIAG node
     *
     * @param diagNode Parent node of the nodes defining diagnostic items
     */
    void ReadDiagMap( const XmlNode *diagNode);

    /**
     * Fills in an XmlNode with the data value for a data item
     *
     * @param dataItem Properties for the data item to read
     * @param value    String to fill in with the value of the requested data item
     * @return 0 if valueNode is filled in
     *         -1 if unable to handle the request
     */
    virtual int ReadDataItem( const dataMapItem_t *dataItem, string &value);

    /**
     * Reads a data item's value from an XmlNode
     *
     * @param dataItem Properties for the data item to write to
     * @param value    String containing the value of the specified data item
     * @return 0 if new value was written
     *         -1 if unable to handle the request
     */
    virtual int WriteDataItem( const dataMapItem_t *dataItem, const string &value);

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
    virtual int IoDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

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
    virtual int IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);


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
    virtual int IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Sends a pulse to the resource manager's I/O channel to indicate a
     * need to write our outputs to the PLC inresponse to a client Write
     * operation.
     */
    void SendUpdateOutputsPulse()
    {
        BposSendPulse( PlcMgr::m_writeChCoid,
                       sched_get_priority_max(sched_getscheduler(0)),
                       PLC_TIMER_CODE,
                       PLC_WRITE_VALUE);
    }

    /**
     * Checks if a data item is a diagnostic (fault) item and publishes
     * or clears the fault status if it is a diagnostic
     *
     * @param dataItem Data item whose diagnostic status is to be published/cleared
     * @param diagVal  Valueof the Data item
     * @return Diagnostic level for the given value
     */
    DiagLevel_t CheckDiagItem( const dataMapItem_t *dataItem, uint32_t diagVal)
    {
        DiagLevel_t retVal = DIAG_NONE;
        uint64_t    chId = CalculateDataTagIndex( dataItem);

        PlcDiagMapItr_t diagItr = m_diagList.find( chId);
        if( diagItr != m_diagList.end())
        {
            PlcDiagItem& diagItem = diagItr->second;
            retVal = diagItem.CheckTrigger( diagVal, m_faultServer);
        }

        return( retVal);
    }

    /**
     * Handles pulses delivered to our resource manager I/O channel
     *
     * @param pulse  Pulse received on the I/O channel
     * @return 0 if successful; -1 if error; 1 if no further processing is required
     */
    virtual int HandleResMgrPulse(io_pulse_t &pulse);

    /**
     * Will handle pulse if overriden.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);

    /**
     * Clears the IPlc objects from our PLC board list and frees the memory
     * allocated for each IPlc object
     */
    void ClearBoardList();

    /**
     * Instantiates a new IPlc interface object based on a type identifier
     * string
     *
     * @param boardTag PLC type identifier string (i.e. "V30B" or "Df1") used to determine
     *                 which type of IPlc objec tto instantiate
     * @return Pointer to a new IPlc object
     * @exception BepException
     *                   If the board type identifier is unrecognized
     */
    IPlc* CreatePlcObject( const std::string &boardTag) throw( BepException);

    /**
     * Time (in milliseconds) required to convert an anlog channel (defulat is CHANNEL_TIME)
     */
    long                m_convertTime;

    /**
     * Number of channels for this analog input board
     */
    unsigned long       m_channelCnt;


    /**
     * Inputs from the PLC
     */
    uint32_t            *m_fromPlc;

    /**
     * Outputs to the PLC
     */
    uint32_t            *m_toPlc;

    /**
     * Number of words to transfer from the PLC
     */
    unsigned long       m_fromPlcCount;

    /**
     * Number of words to transfer to the PLC
     */
    unsigned long       m_toPlcCount;

    /**
     * Device number for the path we adopt for inputs
     */
    int                 m_inDevNo;

    /**
     * Device number for the path we adopt for outputs
     */
    int                 m_outDevNo;

    /**
     * File Descriptor for Channel Data Shared Memory
     */
    int                 m_plcChnlFd;

    /**
     * Priority at which to run the scanner fork
     */
    int                 m_scannerPrio;

    /**
     * Function attributes for I/O functions (offset, file size, etc)
     */
    IOFUNC_ATTR_T       m_inFuncAttr;

    /**
     * Function attributes for I/O functions (offset, file size, etc)
     */
    IOFUNC_ATTR_T       m_outFuncAttr;

    /**
     * Attributes for the server's thread pool
     */
    thread_pool_attr_t  m_poolAttr;

    /**
     * Thread pool pointer used by the kernel
     */
    thread_pool_t       *m_threadPool;

    /**
     * Flag to indicate board autodetection needed
     */
    bool                m_autoDetect;

    /**
     * Timer timeout/repeat values
     */
    struct _itimer      m_timerTime;

    /**
     * ID for our timer to be used to scan VME space
     */
    int                 m_scanTimerId;

    /**
     * Connection ID to send the WriteChannel pulse to
     */
    static int          m_writeChCoid;

    /**
     * List of PLC board structures
     *
     * @see PlcBoardList_t
     */
    PlcBoardList_t      m_plcBoards;

    /**
     * List of diagnostic bits
     */
    PlcDiagMap_t        m_diagList;

    /**
     * Inode value to assign to the Inputs "file"
     */
    const ino_t         m_toPlcInode;

    /**
     * R/W lock used to synchronize vme space reads and writes
     */
    pthread_rwlock_t    m_vmeSpaceLock;

    /**
     * Object used to hold the number of input channels associated with
     * each of the PLC boards in the system
     */
    IoCountList_t       m_inCountList;

    /**
     * Object used to hold the number of output channels associated with
     * each of the PLC boards in the system
     */
    IoCountList_t       m_outCountList;

    /**
     * Flag to indicate whether or not we should wait for the register
     * command before we register. If false, we will register during
     * Initialization
     */
    bool                m_waitForRegister;

    /**
     * Flag used to indicate whether or not we must perform publications at strt-up
     * (when the notification list becomes valid)
     */
    bool                m_needFirstPublish;
};

#endif // _PLC_MGR_H_

