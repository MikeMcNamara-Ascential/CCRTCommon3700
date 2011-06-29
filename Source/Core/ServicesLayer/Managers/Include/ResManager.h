//******************************************************************************
// Description:
//      Definition of Resource Manager Base Class
//
//==============================================================================
//
//     Copyright  1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Include/ResManager.h $
// 
// 3     10/30/07 4:00p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:29p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 3     6/26/03 3:56p Khykin
// Updated with Brians latest changes.
//
// 1     6/19/03 2:02p Bmeinke
//
// 1     4/30/03 4:42p Bmeinke
// Migration to new personalized configuration management structure
//
// 60    3/27/03 6:37p Tbochene
// BrianM: Initialize the return value in Read()
// Set replyString = BepComponent::Subscribe() in Subscribe()
//
// 59    3/24/03 1:14p Bmeinke
// Replaced BepComponent interface methods Read(), Write(), etc with
// versions which take XmlNode pointer arguments
//
// 58    2/06/03 10:34a Bmeinke
// Removed Subscribe(const XmlNode *node) (caused a recursive loop in
// derived classes)
// Removed call to ReadDataItem() from Subscribe() (handled by
// BepComponent::Subscribe())
//
// 57    1/03/03 4:26p Bmeinke
// Added the static SignalHandler() function
//
// 56    10/15/02 11:00a Bmeinke
// Added IFaultServer object (m_faultServer) for diagnostic/fault
// reporting
// Added Register() method so m_faultServer can attach to the fault server
// properly
// Changed return value of CalculateDataTagIndex() to uint64_t
// Changed WholeChannelItemMap_t key from uint32_t to uint64_t
//
// 55    10/10/02 12:19p Bmeinke
// Removed redundant data member m_cfgFilename
//
// 54    10/10/02 12:13p Khykin
// Removing prints from the PublishedTagList.
//
// 53    10/08/02 6:49p Bmeinke
// Added an optional priority argument to ConnectTimerPulse
//
// 52    10/08/02 6:20p Bmeinke
// Updated javadoc comments for InitHandlers()
//
// 51    10/08/02 11:21a Khykin
// Removed overridden HandlePulse() method (let BepComponent handle the
// pulse)
//
// 50    8/01/02 11:54a Bmeinke
// Removed compiler warnings
//
// 49    6/27/02 7:26p Bmeinke
// Changed publish logic: all published data items are stored in 1 of 2
// multimap containers: Bit-based data items are stored seperate from the
// whole-channel items; maps sorted based on a key calculated from the
// data item's board, channel and bit index. This allows any number of
// tags to point at the exact same data.
//
// 48    6/19/02 2:48p Skypig13
// Removed AddNotification in Subscribe() method
//
// 47    5/24/02 2:59p Kh
// Updating diagnostics.
//
// 46    5/03/02 5:16p Kh
// Updating to check for a valid notification list prior to sending a
// notification.
//
// 45    4/25/02 4:42p Skypig13
// Updated with new changes
//     Revision 1.2  2002/04/25 18:23:44  erickson
//     merged from  VSS
//
//
// 44    4/04/02 3:01a Bmeinke
// Moved the definition of mSEC_nSEC to BepDefs.h
//
// 43    3/19/02 12:08p Kh
// Updating due to changes in BepComponent.
//
// 42    2/27/02 1:54p Bmeinke
// Added a tab character inside Read() method's Logging of attempts to
// read an unknown data tag
//
// 41    2/19/02 8:03p Bmeinke
// Subscribe() method now replies with the current value of the data item
// being subscribed for if it is a supported tag
//
// 40    2/07/02 10:20p Bmeinke
// m_nList data member of BepComponent changed to a pointer data type
//
// 39    12/11/01 5:29p Brian.meinke
// Made CalculateDataTagIndex() methods virtual
//
// 38    11/06/01 4:34p Brian.meinke
// Added #define RESMGR_OCB_T struct _resMgrIoOcb_t
//
// 37    11/06/01 12:30a Brian.meinke
// Changed ReadDataItem/WriteDataItem to take string references instead of
// XmlNode pointers
//
// 36    11/05/01 11:47a Brian.meinke
// Removed virtual declaration from destructor
// Added return() to ocbCalloc
//
// 35    10/17/01 9:54p Brian.meinke
// Changed the prototype for Read() method (from BepComponent): added
// 'rate' parameter
//
// 34    10/11/01 11:43a Brian.meinke
// ReadDataItem()/WriteDataItem() now take a pointer to a dataMapItem_t
// instead of a reference
//
// 33    10/09/01 1:36p Brian.meinke
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
// 32    10/04/01 10:22a Brian.meinke
// Integrated the IInterProcessCommunication object for IPC messaging
//
// 31    9/20/01 11:52a Brian.meinke
// Moved ThreadList_t (and it's supporting data types) to seperate header
// file (ThreadList.h)
//
// 30    9/14/01 3:11p Brian.meinke
// Added mehtod InitializeIndexedTags() to fill in the list of our
// published tags based on data item board, channel and bit indices
// Added mehtod CalculateDataTagIndex() to calculate the index of a data
// item in our published tag list based on data item board, channel and
// bit indices
// Added data memebr m_publishedItems to hold a list of our published tags
// board, channel and bit indices
// Added PublishThreadArg_t data type; used to pass subscribtion data to
// publisher thread
// Added PublishData() methods to spin threads to perform subscription
// update notification messages
// Added PublishThread() methods; Thread entry point to perform
// subscription update notification messages
// Added IsWholeChannelItem(); determines if a data item occupies an
// entire I/O channel as opposed to a single bit of a channel
//
// 29    9/14/01 2:00a Brian.meinke
// Changed Check***Messages() method names to Handle***Messages()
//
// 28    9/14/01 12:09a Brian.meinke
// Added #include "tchar.h"
//
// 27    9/04/01 9:51a Brian.meinke
// Changed OcbFree/OcbCalloc to ocbFree/ocbCalloc
// OcbFree/OcbCalloc are now virtual functions to allow each sub-class to
// more easily override this functionality
//
// 26    8/31/01 6:20p Brian.meinke
// Added static ResManager* data member m_ioMgrForOcb used to initialize
// the ResManager* inside the OCB.
// Added common OcbCalloc/OcbFree static methods
// Cleaned up some RMTRACE calls
//
// 25    8/31/01 3:31p Brian.meinke
// Removed unused data type 'ResMgrPulseCodes' and unused define
// 'ALLDIE_CODE'
// Added new 'init' parameter and moved 'size' parameter to original
// PathAttch() method
// Added second PathAttach() method which takes a path and a
// pre-initialized iofunc_attr_t structure
//
// 24    8/30/01 3:35p Brian.meinke
// static I/O handler methods now call the iofunc_..._verify() (privelege
// checks) functions before dispatching messages to virtual I/O handler
// methods
// Added some comments
// Removed commented out blocks
//
// 23    8/28/01 11:12p Brian.meinke
// Changed thread pool min size from 2 to 5 threads
// Changed the base data type of ThreadList_t from vector to a map because
// vector was inserting an empty entry
// Added a size parameter to PathAttach
// Added static xmlBlockFunc() method
// Added m_xmlThreadPoolAttributes data member to allow control over
// thread pool thread attributes
//
// 22    8/17/01 2:24p Brian.meinke
// Install all (almost) I/O handlers (static class methods)
// Added interface (virtual functions) for I/O function callouts (called
// by static methods)
// Added m_timerConnectId to hold timer connection ID (for use with
// scanner timers)
// Added virtual XmlBlock/Handler methods (called by static class XML
// handler methods)
// Added virtual methods to handle Subscription requests
//
// 21    6/22/01 1:11p Brian.meinke
// Removed references to the "printf" mutex (intended for the Log()
// methods)
// Added javadoc comment to the ReloadConfig() method
//
// 20    6/20/01 4:08p Brian.meinke
// Removed m_verbose data member (uses new verbosity control of ILogger)
// Removed Log() methods (uses new ILogger method)
// Removed SetTaskName()/GetTaskName() (uses Get/SetProcessName() of
// ILogger)
//
// 19    6/11/01 9:06a Brian.meinke
// ResManager now inherits from BepComponent
// Added Initialize() method (override of base class method)
// Changed hard coded XML Tag strings to defines in header files
// Removed references to m_logger and replaced with *this
//
// 18    5/23/01 9:19p Brian.meinke
// Now inherits from BepComponent
//
// 17    5/10/01 9:55a Brian.meinke
// Moved xmlThreadInit implementation to cpp file
// Added pthread_barrier_t for XML thread startup synchronization
//
// 16    5/07/01 5:49p Brian.meinke
// Added CleanupManager method to facilitate re-loading configuration data
//
// 15    4/30/01 10:54a Brian.meinke
// Added ParseCommandLineMethod
// Added defines for commen XML setup tags
// Added LoadConfigFile method
// Made ReadXmlOptions method virtual
//
// 14    4/25/01 5:05p Brian.meinke
// Added default Execute method
// Changed XML Setup node tags for BEP standard naming conventions
// Changed semaphore used inside Log method to a pthread_mutex
// Added ReadDataItem and WriteDataItem methods for reading/writing data
// per an XML message
// Moved LogMask enum to the LogDefs.h file
//
// 13    4/24/01 2:06p Brian.meinke
// Added check for successful name registration in XmlThreadInit
//
// 12    4/24/01 1:53p Brian.meinke
// Added common XML Thread Pool framework
// Added call to KillXmlThreads in destructor to terminate all XML thread
// pool threads
// Made AbortHandler a global extern C function
// Default XmlThreadInit registers process with the OS and initializes the
// thread pool handle structure
//
// 11    4/23/01 2:59p Brian.meinke
// Added method ReadDataMapItem to parse an XML node into a dataMapItem_t
// object
// Changed class name for DataMap to HwDataMap
//
// 10    4/20/01 6:19p Brian.meinke
// Added ReadDataMap method
//
// 9     4/19/01 3:57p Brian.meinke
// Changed printf semaphore back to native POSIX semaphore (from Bpos
// semaphore)
//
// 8     4/18/01 6:40p Brian.meinke
// Added HwDataMap data member to hold data items and their tags
//
// 7     4/17/01 4:42p Brian.meinke
// Added ReadXmlOptions method to read common options from XML file
//
// 6     4/16/01 10:55a Brian.meinke
// Added _threadPoolParam_t structure: common to resource managers
// Added class ThreadList: common to resource managers
//
// 5     4/12/01 2:23p Brian.meinke
// Added ALIGN define for returning dirent structures from a manager
//
// 4     4/10/01 2:35p Brian.meinke
// Restructured manager classes to use BepException for error conditions
//
// 3     4/10/01 1:15p Brian.meinke
// Apparently, m_resMgrId is used...
//
// 1     4/10/01 10:46a Admin
//
// 5     3/19/01 8:45a Brian.meinke
// Added BposINit to copnstructor
// Use BPOS Semaphores
// Use standard BposSend/BposREceive instead of Sendmx/Receivemx
// Added methods SetTaskName/GetTaskName
// DevNoListMgr now inherits from std::vector<int>
//
// 4     3/07/01 5:14p Brian.meinke
// Changed all method names to start with capital letters to comply with
// BEP standard
//
// 3     3/06/01 7:31p Brian.meinke
// Moved local console control to the Logger Interface
//
// 2     2/16/01 4:02p Brian.meinke
// Added Logger support
//
// 1     2/15/01 12:12p Brian.meinke
// Base class for resource managers (I/O managers, Sys Message managers,
// etc). Contains the kernale interface functions.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef _RESMANAGER_H_INCLUDED
#define _RESMANAGER_H_INCLUDED

// Forward Class Definitions
class ResManager;
class TimeToDie;
class ThreadList;
struct _threadPoolHandle_t;
class threadPoolParam_t;
struct _resMgrIoOcb_t;


// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

// Define this here so it does not get defined in the neutrino headers

#ifndef IOFUNC_OCB_T
struct _resMgrIoOcb_t;
#define IOFUNC_OCB_T            struct _resMgrIoOcb_t
#endif

#ifndef RESMGR_OCB_T
struct _resMgrIoOcb_t;
#define RESMGR_OCB_T            struct _resMgrIoOcb_t
#endif

#ifndef THREAD_POOL_HANDLE_T
struct _threadPoolHandle_t;
#define THREAD_POOL_HANDLE_T    struct _threadPoolHandle_t
#endif

#ifndef THREAD_POOL_PARAM_T
#define THREAD_POOL_PARAM_T     threadPoolParam_t
#endif

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Header Files
#include <errno.h>
#include <stdarg.h>             // Variable argument stuff
#include <unistd.h>             // getpid
#include <fcntl.h>              // O_CREAT
#include <semaphore.h>          // POSIX semaphores
#include <sys/resmgr.h>         // Resource Manager Stuff
#include <sys/iofunc.h>
#include <sys/dispatch.h>       // Message dispatch interface
#include <sys/types.h>          // pid_t, etc
#include <pthread.h>            // basic thread and mutex

#include <string>
#include <map>

#include "BepComponent.h"       // Standard BEP base component
#include "Bepos.h"              // Standard types
#include "BepException.h"       // Standard Exception
#include "XmlParser.h"          // XML Parser definition
#include "XmlDefs.h"            // XML definitions
#include "XmlNode.h"            // XML Node definitions
#include "BepXmlUtils.h"        // XML Utilities
#include "LogDefs.h"            // Logger Interface
#include "GeneralUtilities.h"   // Miscellaneous utilities
#include "HwDataMap.h"          // Data Table
#include "ThreadList.h"         // Thread safe thread id list
#include "tchar.h"              // Generic wide/ascii character functions
#include "IBepCommunication.h"  // Comm objects
#include "IFaultServer.h"       // Fault publication

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * SIGABRT signal handler
 *
 * @param SigNo  Signal numnber being handled
 * @see ResManager
 */
void AbortHandler( int SigNo);

// I/O Handler Helper Function type-defs

/**
 * Function type used to allocate new resource manager OCBs
 */
typedef IOFUNC_OCB_T    *(*ocb_calloc_t)(resmgr_context_t *ctp, IOFUNC_ATTR_T *attr);
/**
 * Function type used to de-allocate old resource manager OCBs
 */
typedef void            (*ocb_free_t)(IOFUNC_OCB_T *ocb);
/**
 * Function type used for custom message types
 *
 * @deprecated Use io_devctl instead of custom message ID's
 */
typedef int             (*resMgrOtherFn)(resmgr_context_t *, void *msg);
/**
 * Function type used for resource manager timer pulses
 */
typedef int             (*timerTickFn_t)(message_context_t *ctp, int code, unsigned flags, void *handle);

#ifdef __cplusplus
};
#endif

#define DIE_TAG             "Terminate"
#define LOAD_CFG_TAG        "LoadConfig"

/**
 * Config File Setup Section node tag
 */
#define CFG_SETUP_TAG       XML_T("Setup")

/**
 * Config File Data Map Section Tag
 */
#define CFG_DATAMAP_TAG     XML_T("DataMap")

/**
 * Config file tag name for a BAse Address specifier node
 */
#define CFG_BASEADDR_TAG    XML_T("BaseAddress")

/**
 * Config File Verbose Node Tag
 */
#define CFG_VERBOSE_TAG     XML_T("Verbose")

/**
 * Config File Local Echo Node Tag
 */
#define CFG_LOCALECHO_TAG   XML_T("LocalEcho")

/**
 * Config File Demo Mode Node Tag
 */
#define CFG_DEMO_TAG        XML_T("Demo")

/**
 * Base Address AUTO detection node value
 */
#define CFG_AUTO_VAL        XML_T("AUTO")

/**
 * XML node tag of the thread pool configuration node
 */
#define CFG_THREAD_POOL_TAG     XML_T("MgrPool")

/**
 * XML attribute tag for the thread pool low water attribute
 */
#define CFG_POOL_LOW_TAG        XML_T("lowWater")

/**
 * XML attribute tag for the thread pool high water attribute
 */
#define CFG_POOL_HI_TAG         XML_T("hiWater")

/**
 * XML attribute tag for the thread pool maximum attribute
 */
#define CFG_POOL_MAX_TAG        XML_T("maximum")

/**
 * XML attribute tag for the thread pool thread increment attribute
 */
#define CFG_POOL_INC_TAG        XML_T("increment")

/**
 * XML attribute tag for the thread pool thread increment attribute
 */
#define CFG_POOL_PRIO_TAG       XML_T("priority")

/**
 * Class thrown when the TERMINATE command is received
 *
 * @author Brian Meinke
 * @version 1
 * @see ioManagerIfc
 * @since $Date: 12/21/03 6:29p $
 */
class TimeToDie
{
public:
    TimeToDie( bool dum=true) : dummy( dum) {};
private:
    bool dummy;
};

///////////////////////////////////////////////////////////////////////////////
//      THREAD POOL STUFF
///////////////////////////////////////////////////////////////////////////////
/**
 * Minimum number of blocked/waiting thread in the thread pool
 */
#define THREAD_POOL_MIN 5
/**
 * Maximum number of blocked/waiting thread in the thread pool
 */
#define THREAD_POOL_MAX 50

typedef struct _threadPoolHandle_t
{
    ResManager  *ioMgr;
    int         channelId;
} threadPoolHandle_t;


/**
 * Size, in bytes, of the receive message buffer
 */
#define RX_BUFF_SIZE    2048

/**
 * Data type passed to the ThreadPool handler functions
 */
class threadPoolParam_t
{
public:

    /**
     * Constructor for thread pool argument data type
     *
     * @param handle     Handle to the thread pool to which the thread receiving this data belongs
     * @param parentProc PArent process of the thread being created
     */
    threadPoolParam_t(threadPoolHandle_t *handle=NULL, pid_t parentProc=-1) :
        poolHandle( handle), parent( parentProc) {

        memset( &info, 0, sizeof( info));
    };

    threadPoolHandle_t  *poolHandle;

    /**
     * PID of parent process
     */
    pid_t               parent;
    /**
     * PID of the process to reply to
     */
    int                 rcvId;
    /**
     * Information about the received message
     */
    struct _msg_info    info;

    /**
     * Comm object used to process requests
     */
    IBepCommunication   commObj;

    /**
     * Message buffer to receive XML messages into
     */
    string              mssgString;

};

/**
 * Resource Manager Open Control Block type
 * Used to hold data for a client connection
 */
typedef struct _resMgrIoOcb_t
{
    /**
     * POSIX I/O dispatch OCB structure
     */
    iofunc_ocb_t    m_ioOcb;

    /**
     * Pointer to the controlling I/O manager
     */
    ResManager      *m_mgr;

} resMgrIoOcb_t;

/**
 * Ain Thread Argument type
 */
typedef struct _resMgrThreadArg_t
{
    /**
     * Resource Manager object for whom the thread is being read
     */
    ResManager          *ioMgr;

    /**
     * Attributes to use when creating the XML thread pool. If NULL, default
     * attributes will be used.
     *
     * @see thread_pool_attr_t
     */
    thread_pool_attr_t  *poolAttribs;

    /**
     * Process ID of the thread's parent
     */
    pid_t               parent;

    /**
     * Channel on which the parent Resource Manager is handling IO messages on
     */
    int                 parentChannel;

} resMgrThreadArg_t;


typedef enum
{
    RES_MGR_CMD_NONE,
    RES_MGR_CMD_CONFIG,
    RES_MGR_CMD_TERMINATE
} ResManagerCmd_t;


/**

 */
typedef std::multimap< uint64_t, dataTag_t> PublishedTagList_t;
typedef PublishedTagList_t::iterator        PublishedTagListItr_t;
typedef PublishedTagList_t::const_iterator  PublishedTagListCItr_t;
typedef PublishedTagList_t::value_type      PublishedTagListVal_t;

/**
 * Class used to map a (board,channel,bit) triplet to a data tag. Used to
 * hold the list of tags we publish. Index based on data item board, channel
 * and bit indices.
 *
 * @author Brian Meinek
 * @version 1
 * @since June 25, 2002
 */
class PublishedTagList : public PublishedTagList_t
{
public:
    /**
     * Constructor
     */
    PublishedTagList() : PublishedTagList_t()
    {
    }

    /**
     * Destructor
     */
    ~PublishedTagList()
    {
    }

    /**
     * Add a new entry to the list
     *
     * @param boardIdx Board index of the whole channel data item
     * @param portIdx  Channel index of the whole channel data item
     * @param bitIdx   Bit index of the whole channel data item
     * @param dataTag  Data tag of the whole channel data item
     */
    void Insert( uint16_t boardIdx, uint16_t portIdx, uint16_t bitIdx,
                 const dataTag_t &dataTag)
    {
        uint64_t    tagKey = CalculateKey( boardIdx, portIdx, bitIdx);

//      printf( "Inserting bit-based data item %s with key $%016llX\n", dataTag.c_str(), tagKey);
        insert( PublishedTagListVal_t( tagKey, dataTag));
    }

    /**
     * Remove an existing entry from the list
     *
     * @param boardIdx Board index of the whole channel data item
     * @param portIdx  Channel index of the whole channel data item
     * @param bitIdx   Bit index of the whole channel data item
     */
    void Erase( uint16_t boardIdx, uint16_t portIdx, uint16_t bitIdx)
    {
        uint64_t    tagKey = CalculateKey( boardIdx, portIdx, bitIdx);
        PublishedTagListItr_t   itr;

        if( (itr=find( tagKey)) != end())
        {
            erase( itr);
        }
    }

    /**
     * Search for a data item in the list
     *
     * @param boardIdx Board index of the data item to look for
     * @param portIdx  Channel index of the data item to look for
     * @param bitIdx   Bit index of the whole channel data item
     * @param dataTag  (Optional Output)Data tag for the item with the given baord and
     *                 channel indices
     * @return true if a data item exists in the list with the given board and
     *         channel indices, false otherwise
     */
    bool Find( uint16_t boardIdx, uint16_t portIdx, uint16_t bitIdx, dataTag_t *dataTag=NULL)
    {
        bool        retVal = false;
        uint64_t    tagKey = CalculateKey( boardIdx, portIdx, bitIdx);
        PublishedTagListItr_t   itr;

        if( (itr=find( tagKey)) != end())
        {
            retVal = true;
            if( dataTag != NULL)
            {
                *dataTag = itr->second;
            }
        }

        return( retVal);
    }

    /**
     * Search for a data item in the list
     *
     * @param boardIdx Board index of the data item to look for
     * @param portIdx  Channel index of the data item to look for
     * @param bitIdx   Bit index of the whole channel data item
     * @param itr      Pointer to the previously found instance of the item in
     *                 the list. Pass end() in the first call to initialize the search
     * @return Pointer to the desired data item, or end() if no more instances exist
     */
    PublishedTagListItr_t Find( uint16_t boardIdx, uint16_t portIdx, uint16_t bitIdx,
                             PublishedTagListItr_t itr)
    {
        uint64_t    tagKey = CalculateKey( boardIdx, portIdx, bitIdx);

        // If this is the first call
        if( itr == end())
        {
            // Try to find the first occurrence
            itr = find( tagKey);

            if( itr == end())
            {
//              printf( "\n\n\tUNABLE TO FIND KEY $%llX in PublishedTagList::Find( %d, %d, %d)\n",
//                  tagKey, boardIdx, portIdx, bitIdx);
            }
        }
        else
        {
            // Look at the next item in the list
            itr++;
            // If the key of the next item is not the same as that desired by
            //  the caller
            if( itr->first != tagKey)
            {
                // Return 'no more found'
                itr = end();

//              printf( "\n\n\tEND OF LIST FOR KEY $%llX in PublishedTagList::Find( %d, %d, %d)\n",
//                  tagKey, boardIdx, portIdx, bitIdx);
            }
        }

        return( itr);
    }

private:
    /**
     * Converts a (board,channel,bit) index triplet into a 64-bit key to use
     * when sorting the list
     *
     * @param boardIdx Board index to use when calculating the sort key
     * @param portIdx  Channel index to use when calculating the sort key
     * @param bitIdx   Bit index of the whole channel data item
     * @return 64-bit key used for mapping the (board,channel) indices to data tag
     */
    uint64_t CalculateKey( uint16_t boardIdx, uint16_t portIdx, uint16_t bitIdx)
    {
        uint64_t tagKey=0, hi = boardIdx;
        uint32_t mid = portIdx;

        tagKey = (hi << 32) | (mid << 16) | (bitIdx);
        return( tagKey & 0x0000FFFFFFFFFFFFLL );
    }
};


/**
 * Class used to map a (board,channel) pair to a data tag
 *
 * @author Brian Meinek
 * @version 1
 * @since June 25, 2002
 */
typedef std::multimap< uint64_t, const dataTag_t>   WholeChannelItemMap_t;
typedef WholeChannelItemMap_t::iterator             WholeChannelItemMapItr_t;
typedef WholeChannelItemMap_t::const_iterator       WholeChannelItemMapCItr_t;
typedef WholeChannelItemMap_t::value_type           WholeChannelItemMapVal_t;

/**
 * Class used to map a (board,channel) pair to a data tag
 *
 * @author Brian Meinek
 * @version 1
 * @see WholeChannelItemMap_t
 * @since June 25, 2002
 */
class WholeChannelItemMap : public WholeChannelItemMap_t
{
public:
    /**
     * Constructor
     */
    WholeChannelItemMap() : WholeChannelItemMap_t()
    {
    }

    /**
     * Destructor
     */
    ~WholeChannelItemMap()
    {
    }

    /**
     * Add a new entry to the list
     *
     * @param boardIdx Board index of the whole channel data item
     * @param portIdx  Channel index of the whole channel data item
     * @param dataTag  Data tag of the whole channel data item
     */
    void Insert( uint16_t boardIdx, uint16_t portIdx, const dataTag_t &dataTag)
    {
        uint64_t    tagKey = CalculateKey( boardIdx, portIdx);

        insert( WholeChannelItemMapVal_t( tagKey, dataTag));
    }

    /**
     * Remove an existing entry from the list
     *
     * @param boardIdx Board index of the whole channel data item
     * @param portIdx  Channel index of the whole channel data item
     */
    void Erase( uint16_t boardIdx, uint16_t portIdx)
    {
        uint64_t    tagKey = CalculateKey( boardIdx, portIdx);
        WholeChannelItemMapItr_t    itr;

        if( (itr=find( tagKey)) != end())
        {
            erase( itr);
        }
    }

    /**
     * Search for a data item in the list
     *
     * @param boardIdx Board index of the data item to look for
     * @param portIdx  Channel index of the data item to look for
     * @param dataTag  (Optional Output)Data tag for the item with the given baord and
     *                 channel indices
     * @return true if a data item exists in the list with the given board and
     *         channel indices, false otherwise
     */
    bool Find( uint16_t boardIdx, uint16_t portIdx, dataTag_t *dataTag=NULL)
    {
        bool        retVal = false;
        uint64_t    tagKey = CalculateKey( boardIdx, portIdx);
        WholeChannelItemMapItr_t    itr;

        if( (itr=find( tagKey)) != end())
        {
            retVal = true;
            if( dataTag != NULL)
            {
                *dataTag = itr->second;
            }
        }

        return( retVal);
    }

    /**
     * Search for a data item in the list
     *
     * @param boardIdx Board index of the data item to look for
     * @param portIdx  Channel index of the data item to look for
     * @param itr      Pointer to the previously found instance of the item in
     *                 the list. Pass end() in the first call to initialize the search
     * @return Pointer to the desired data item, or end() if no more instances exist
     */
    WholeChannelItemMapItr_t Find( uint16_t boardIdx, uint16_t portIdx,
                                   WholeChannelItemMapItr_t itr)
    {
        uint64_t    tagKey = CalculateKey( boardIdx, portIdx);

        // If this is the first call
        if( itr == end())
        {
            // Try to find the first occurrence
            itr = find( tagKey);
        }
        else
        {
            // Look at the next item in the list
            itr++;
            // If the key of the next item is not the same as that desired by
            //  the caller
            if( itr->first != tagKey)
            {
                // Return 'no more found'
                itr = end();
            }
        }

        return( itr);
    }

private:
    /**
     * Converts a (board,channel) index pair into a 32-bit key to use
     * when sorting the list
     *
     * @param boardIdx Board index to use when calculating the sort key
     * @param portIdx  Channel index to use when calculating the sort key
     * @return 32-bit key used for mapping the (board,channel) indices to data tag
     */
    uint64_t CalculateKey( uint16_t boardIdx, uint16_t portIdx)
    {
        uint64_t    hi = boardIdx | (0x8000000000000000LL);
        return( (hi << 16) | portIdx);
    }
};

/**
 * Argument data type passed to our publisher threads
 */
class PublishThreadArg_t
{
public:
    PublishThreadArg_t( const dataTag_t &tag, bool val, ResManager *mgr) :
        dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%d"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const dataTag_t &tag, char val, ResManager *mgr) :
        dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%c"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const dataTag_t &tag, UINT8 val, ResManager *mgr) :
        dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%d"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const dataTag_t &tag, int val, ResManager *mgr) :
        dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%d"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const dataTag_t &tag, long val, ResManager *mgr) :
        dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%ld"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const dataTag_t &tag, float val, ResManager *mgr) :
        dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 100];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%f"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const dataTag_t &tag, const XmlString &val, ResManager *mgr) :
        dataTag( tag), ioMgr( mgr)
    {
        pthread_barrier_init( &barrier, NULL, 2);
        dataVal = XmlString( val);
    }

    PublishThreadArg_t( const dataTag_t &tag, const XML_Char *val, ResManager *mgr) :
        dataTag( tag), ioMgr( mgr)
    {
        pthread_barrier_init( &barrier, NULL, 2);
        dataVal = XmlString( val);
    }

    ~PublishThreadArg_t()
    {
        pthread_barrier_destroy( &barrier);
    }

    dataTag_t           dataTag;
    XmlString           dataVal;
    pthread_barrier_t   barrier;
    ResManager          *ioMgr;
};

/**
 * Base class for all Resource Managers
 *
 * @author Brian Meinke
 * @version 2
 */
class ResManager : public BepComponent
{
public:
    /**
     * Default constructor
     *
     * @param name   Name to register with the OS as
     */
    ResManager( const char *name="");

    /**
     * Class destructor
     */
    ~ResManager();

    /**
     * Perform any server initialization
     *
     * @return true if successful
     *         false if error
     */
    virtual bool InitManager();

    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    virtual void CleanupManager();

    /**
     * Main message pump for the resource manager.
     *
     * @return 0 if no errors
     *         -1 on error (use errno for further error information)
     */
    int Execute();

    /**
     * Request to read a data tag.
     *
     * @param node Data to be read.
     * @param rate The rate to read the data.  This parameter is not used.
     * 
     * @return Value of tag.
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0 )
    {
        const string        tag( node->getName());
        string              replyVal(BEP_INVALID_RESPONSE);
        const dataMapItem_t *dataItem;

        Log( LOG_FN_ENTRY, "Enter ResManager::Read( %s)\n", tag.c_str());

        // Try to find this tag in our data item table
        if( (dataItem = m_dataTable.find( tag)) != NULL)
        {
            // Fill in node with data value
            if( ReadDataItem( dataItem, replyVal) == -1)
            {
                replyVal = BEP_INVALID_RESPONSE;
            }
        }
        else
        {
            Log( LOG_ERRORS, "\tInvalid name for reading: <%s>\n", tag.c_str());
            replyVal = BEP_INVALID_RESPONSE;
        }

        Log( LOG_FN_ENTRY, "Exit ResManager::Read( %s, %s)\n", tag.c_str(), replyVal.c_str());
        return( replyVal);
    }

    /**
     * Request to write a data tag
     *
     * @param node  Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    const std::string Write(const XmlNode *node)
    {
        const string        tag( node->getName()), value( node->getValue());
        string              replyVal;
        const dataMapItem_t *dataItem;

        Log( LOG_FN_ENTRY, "Enter ResManager::Write( %s, %s)\n", tag.c_str(), value.c_str());

        // Try to find this tag in our data item table
        if( (dataItem = m_dataTable.find( tag)) != NULL)
        {
            // Fill in node with data value
            if( WriteDataItem( dataItem, value) != -1)
            {
                replyVal = BEP_SUCCESS_RESPONSE;
            }
            else
                replyVal = BEP_ERROR_RESPONSE;
        }
        else
        {
            Log( LOG_ERRORS, "Invalid name for writing: <%s>\n", tag.c_str());
            replyVal = BEP_INVALID_RESPONSE;
        }

        Log( LOG_FN_ENTRY, "Exit  ResManager::Write( %s, %s)\n", tag.c_str(), value.c_str());
        return( replyVal);
    }

    /**
     * Reloads the configuration file and resets the
     * base components objects.
     * @since Version 1.0
     */
    virtual void ReloadConfiguration(void)
    {
        ReloadConfig();
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
    virtual const std::string Subscribe(const XmlNode *node)
    {
        const std::string       tag( node->getName()), procName( node->getValue());
        std::string             replyString;
        const dataMapItem_t     *dataItem;

        Log( LOG_FN_ENTRY, "Enter ResManager::Subscribe(%s, %s)\n", tag.c_str(), procName.c_str());
        // Check if we supply the requested data tag
        if( (dataItem=m_dataTable.find( tag)) != NULL)
        {
            Log( LOG_DEV_DATA, "Adding subscription for %s\n", tag.c_str());
            replyString = BepComponent::Subscribe( node);
            m_haveSubscriptions = true;
        }
        else
        {
            Log( LOG_DEV_DATA, "Unable to add subscription for %s\n", tag.c_str());
            replyString = BEP_INVALID_RESPONSE;
        }

        Log( LOG_DEV_DATA, "Replied with %s\n", replyString.c_str());

        Log( LOG_FN_ENTRY, "Exit ResManager::Subscribe(%s, %s)\n", tag.c_str(), procName.c_str());
        return( replyString);
    }

    /**
     * Method used to abort the current processing based
     * on the needs of the component.
     * @since Version 1.0
     */
    virtual void Abort(void)
    {
    }

    /**
     * Method used to set all of the objects in the component
     * to their original states.  This would includes all
     * member variables such as buffers, lists, and stored
     * data.
     * @since Version 1.0
     */
    virtual void Reset(void)
    {
    }

    /**
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     * @since Version 1.0
     */
    virtual void Terminate(void)
    {
        HandleTerminate( 0);
    }

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         Success, Fail
     * @since Version 2.5
     */
    virtual const std::string Register(void);

protected:

    /**
     * method for component initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Initializes the interface to the OS kernel
     *
     * @return true on success
     *         false on error
     */
    bool InitKernelInterface();

    /**
     * Registers the calling process as a resource manager
     *
     * @return 0 if successful
     *         -1 if error; use errno to obtain more error information
     */
    int RegisterManager();

    /**
     * Initializes the I/O message handler functions to the default OS values.
     *
     * @param ocbAllocFn Function to be used to allocate OCBs
     * @param ocbFreeFn  Function to be used to free OCBs
     */
    void InitHandlers( void* ocbAllocFn, void* ocbFreeFn);

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
    int PathAttach( const char *pathToAdopt, int size=0, int mode=S_IFNAM|0666,
                    int flags=0, IOFUNC_ATTR_T *ioAttr=NULL, bool init=true);

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
    int PathAttach( const char *pathToAdopt, IOFUNC_ATTR_T *ioAttr) {
        return( PathAttach(pathToAdopt, ioAttr->nbytes, ioAttr->mode, ioAttr->flags,ioAttr, false));
    }

    /**
     * Forms a device path for a manager to adopt
     *
     * @param pathToAdopt
     *               Suffix to be adopted (i.e Ain, Icm, etc)
     * @return
     */
    string MakeDevPath( const char *pathToAdopt);

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
    int PathDetach( int deviceNumber);

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
    int ConnectTimerPulse( int timerCode, int timerVal, timerTickFn_t tickFun,
                           int priority=-1);


    /**
     * Parses command line parameters
     *
     * @param argc   Number of command line arguments in the argv parameter
     * @param argv   Command line option strings
     */
    void ParseCommandLine( int argc, char **argv);

    /**
     * Reads configuration data from the given XML file
     *
     * @param fileName Name of file to read configuration data from
     */
    virtual void LoadConfig( const string &fileName);

    /**
     * Reads a configuration file
     *
     * @param fileName Name of config file to rread
     * @param parser   XML Parser object into which the given file will be loaded and parsed
     * @return 0 if successful
     *         -1 on error
     */
    int LoadXmlFile( const char *fileName, XmlParser &parser);

    /**
     * Reads configuration options from an XML document node
     *
     * @param setupNode Parent node from which option data will be read
     * @return 0 if successful
     *         -1 if error
     */
    virtual int ReadXmlOptions( const XmlElement *setupNode);

    /**
     * Reads the data tag names and properties
     *
     * @param setupNode Node to use as the root DATA_MAP node
     * @return 0 if successful
     *         -1 if error
     */
    virtual int ReadDataMap( const XmlElement *setupNode);

    /**
     * Reads a data item's attributes from the given node
     *
     * @param itemNode Node to read data item properties from
     * @param dataItem Data Item property structure to fill in
     */
    virtual void ReadDataMapItem( const XmlNode *itemNode, dataMapItem_t &dataItem);

    /**
     * Fills in our list of publishable tags based on board, channel and bit indices
     */
    void InitializeIndexedTags();

    /**
     * Adds a published data item to the published item maps
     *
     * @param dataItem Data item attributes
     * @param itemTag  Tag of the data item
     */
    virtual void AddPublishedDataItem( const dataMapItem_t *dataItem, const dataTag_t &itemTag);

    /**
     * Calculates the index of the given tag in our list of published tags.
     * Index is calculated based on board index, channel index, bit index
     * and channels per board
     *
     * @param dataItem Item to calculate index for
     * @return Index for the given data item
     */
    virtual uint64_t CalculateDataTagIndex( const dataMapItem_t *dataItem);

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
    virtual uint64_t CalculateDataTagIndex( int board, int channel, UINT16 bit);

    /**
     * Checks if the given channel on the given board should be treated as a
     * single value as opposed to a group of individual bits
     *
     * @param boardIdx   Board index of the data item to evaluate
     * @param channelIdx Channel index of the data item being evaluated
     * @return true if the channel should be treated as a singel value
     *         false if it should be treated as 8 individual bit-type data items
     */
    virtual bool IsWholeChannelItem( int boardIdx, int channelIdx);

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
    virtual bool GetDataItemTag( int boardIdx, int channelIdx, int bitIdx,
                                 dataTag_t &dataTag);

    /**
     * Publishes changed data values by spinning of a thread to perform
     * the notifications
     *
     * @param board  Board on which the changed data item is located
     * @param port   Channel number of the changed data item
     * @param bit    Bit index of the data tag to publish
     * @param val    Value to publish
     */
    virtual void PublishData( int board, int port, int bit, bool val) {
        XML_Char    buff[ 10];
        _tsprintf( buff, XML_T("%d"), val);
        PublishData( board, port, bit, buff);
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
    virtual void PublishData( int board, int port, int bit, char  val) {
        XML_Char    buff[ 10];
        _tsprintf( buff, XML_T("%c"), val);
        PublishData( board, port, bit, buff);
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
    virtual void PublishData( int board, int port, int bit, UINT8 val) {
        XML_Char    buff[ 10];
        _tsprintf( buff, XML_T("%d"), val);
        PublishData( board, port, bit, buff);
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
    virtual void PublishData( int board, int port, int bit, UINT16 val) {
        XML_Char    buff[ 10];
        _tsprintf( buff, XML_T("%d"), val);
        PublishData( board, port, bit, buff);
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
    virtual void PublishData( int board, int port, int bit, int  val) {
        XML_Char    buff[ 10];
        _tsprintf( buff, XML_T("%d"), val);
        PublishData( board, port, bit, buff);
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
    virtual void PublishData( int board, int port, int bit, long val) {
        XML_Char    buff[ 10];
        _tsprintf( buff, XML_T("%ld"), val);
        PublishData( board, port, bit, buff);
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
    virtual void PublishData( int board, int port, int bit, UINT32 val) {
        XML_Char    buff[ 10];
        _tsprintf( buff, XML_T("%d"), val);
        PublishData( board, port, bit, buff);
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
    virtual void PublishData( int board, int port, int bit, float val) {
        XML_Char    buff[ 100];
        _tsprintf( buff, XML_T("%f"), val);
        PublishData( board, port, bit, buff);
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
    virtual void PublishData( int board, int port, int bit, const XmlString &val) {
        PublishData( board, port, bit, val.c_str());
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
    virtual void PublishData( int board, int port, int bit, XML_Char *val);

    /**
     * Notification thread entry point. Thread is used to publish data values
     * to subscribed clients
     *
     * @param arg    PublishThreadArg_t data type contiaining th data to publish
     * @return NULL
     */
    static void* PublishThread( void *arg);

    /**
     * Indexed list of published tags (index based on baord/channel/bit)
     */
    PublishedTagList            m_publishedItems;

    ///////////////////////////////////////////////////////////////////////////
    //      XML Handler methods
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    /**
     * Function called by kernel to allocate a new struct to pass to a new thread
     *
     * @param handle OUr user defined handle: it is the connection ID to listen on
     * @return Pointer to the new structure to use with the new thread
     */
    static THREAD_POOL_PARAM_T *xmlContextAlloc(THREAD_POOL_HANDLE_T *handle);

    /**
     * Function called to free memory used for a thread argument struct.
     *
     * @param ctp Thread context structure to free.
     */
    static void xmlContextFree(THREAD_POOL_PARAM_T *ctp);

    /**
     * Function called after our thread has left the blocked state and is ready
     * to do some work
     *
     * @param ctp    Thread context structure for this thread to work with
     * @return 0
     */
    static int xmlHandlerFunc(THREAD_POOL_PARAM_T *ctp);

    /**
     * Function in which each thread in the thread pool will block
     *
     * @param ctp    Thread context structure
     * @return
     */
    static THREAD_POOL_PARAM_T *xmlBlockFunc(THREAD_POOL_PARAM_T *ctp);

    /**
     * Function in which each thread in the thread pool will unblock. This method
     * appears to be called once for each "live" thread in the thread pool
     * from a thread with an ID of 1.
     *
     * @param ctp    Thread context structure
     */
    static void xmlUnblockFunc(THREAD_POOL_PARAM_T *ctp);

    /**
     * XML Message Handler
     */
    static void* XmlThreadEntryPoint( void *arg);

    /**
     * Initializer for XmlThread
     *
     * @param arg    Thread argument
     * @return 0 if successful
     *         -1 if error
     */
    virtual int XmlThreadInit( void *arg);

    /**
     * Function called after our thread has left the blocked state and is ready
     * to do some work
     *
     * @param threadData Thread context structure for this thread to work with
     * @return 0
     */
    virtual int XmlHandlerFunc( threadPoolParam_t *threadData);

    /**
     * Function in which each thread in the thread pool will block
     *
     * @param threadData Thread context structure
     * @return
     */
    virtual THREAD_POOL_PARAM_T *XmlBlockFunc( threadPoolParam_t *threadData);

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
     * Launches the XML thread pool
     *
     * @return 0 if no errors
     *         -1 on error (use errno for further error information)
     */
    int LaunchXmlThread();

    /**
     * Kills any XML threads that are hanging around
     */
    void KillXmlThreads();

    /**
     * Method called when a LOAD_CONFIG command message is received.
     */
    virtual void ReloadConfig();

    /**
     * Method called to process a terminate XML command message
     *
     * @param pid    Process to whom we need to reply
     * @return 0 if successful, -1 if error
     */
    virtual int HandleTerminate( pid_t pid);

    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //      XML Handler methods
    ///////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////
    //      I/O Handler methods
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    /**
     * Handles I/O mesages (file descriptor based, not XML)
     */
    void IoMssgLoop();

    /**
     * Handles pulses delivered to our resource manager I/O channel
     *
     * @param pulse  Pulse received on the I/O channel
     * @return 0 if successful; -1 if error; 1 if no further processing is required
     */
    virtual int HandleResMgrPulse( io_pulse_t &pulse);

    /**
     * The purpose of this is to give us a place to allocate our own ocb. It is
     * called as a result of the open being done (e.g. iofunc_open_default
     * causes it to be called). We registered it through the mount structure.
     *
     * @param ctp    Resource manager's context block
     * @param device Attributes for this client connection (an OCB)
     * @return Pointer to newly allocated OCB or NULL if error
     */
    static IOFUNC_OCB_T* ocbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device) {
        ResManager* mgr = (ResManager*)ResManager::m_ioMgrForOcb;
        return( mgr->OcbCalloc( ctp, device));
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
     * @see ResManager::ocbCalloc
     */
    virtual IOFUNC_OCB_T* OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device) {
        Log( LOG_FN_ENTRY, "Enter OcbCalloc\n");

        // Allocate and clear a new OCB for this connection
        resMgrIoOcb_t *ocb = new resMgrIoOcb_t;
        memset( ocb, 0, sizeof( ocb));

        ocb->m_mgr = this;

        Log( LOG_FN_ENTRY, "Exit OcbCalloc\n");

        return( ocb);
    }

    /**
     * The purpose of this is to give us a place to free our ocb.
     * It is called as a result of the close being done
     * (e.g. iofunc_close_ocb_default causes it to be called). We
     * registered it through the mount structure.
     *
     * @param ocb    Client's Open Context Block for this connection
     */
    static void ocbFree(IOFUNC_OCB_T *ocb) {
        resMgrIoOcb_t   *icmOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)icmOcb->m_mgr;

        ioMgr->OcbFree( ocb);
    }

    /**
     * The purpose of this is to give us a place to free our ocb. It is called
     * by ResManager::ocbFree, which in turn is called as a result of the close
     * being done (e.g. iofunc_close_ocb_default causes it to be called).
     * ResManager::ocbFree was registered through our mount structure.
     *
     * @param ocb    Client's Open Context Block for this connection
     * @see ResManager::ocbFree
     */
    virtual void OcbFree(resMgrIoOcb_t *ocb) {
        Log( LOG_FN_ENTRY, "Enter OcbFree\n");

        // Free the memory
        delete ocb;
        ocb = NULL;

        Log( LOG_FN_ENTRY, "Exit OcbFree\n");
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
    static int ioOpen( resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra) {
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
     * @see ResManager::ioOpen
     */
    virtual int IoOpen( resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra) {
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
    static int ioRead(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;
        int             retVal = ENOSYS;

        // Check read priveleges
        if( (retVal=iofunc_read_verify( ctp, msg, (iofunc_ocb_t*)ocb, NULL)) == EOK)
        {
            retVal = ioMgr->IoRead(ctp, msg, ioOcb);
        }

        return( retVal);
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
    virtual int IoRead( resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioWrite( resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;
        int             retVal = ENOSYS;

        // Check write priveleges
        if( (retVal=iofunc_write_verify( ctp, msg, (iofunc_ocb_t*)ocb, NULL)) == EOK)
        {
            retVal = ioMgr->IoWrite(ctp, msg, ioOcb);
        }

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
    virtual int IoWrite( resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioCloseOcb(resmgr_context_t *ctp, void *reserved, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioCloseOcb
     */
    virtual int IoCloseOcb(resmgr_context_t *ctp, void *reserved, resMgrIoOcb_t *ioOcb) {
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
    static int ioStat(resmgr_context_t *ctp, io_stat_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioStat
     */
    virtual int IoStat(resmgr_context_t *ctp, io_stat_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioNotify(resmgr_context_t *ctp, io_notify_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioNotify
     */
    virtual int IoNotify(resmgr_context_t *ctp, io_notify_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioDevCtl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

        return( ioMgr->IoDevCtl(ctp, msg, ioOcb));
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
    virtual int IoDevCtl( resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioUnblock(resmgr_context_t *ctp, io_pulse_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

        return( ioMgr->IoUnblock(ctp, msg, ioOcb));
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
     * @see ResManager::ioUnblock
     */
    virtual int IoUnblock(resmgr_context_t *ctp, io_pulse_t *msg, resMgrIoOcb_t *ioOcb) {
        return( iofunc_unblock_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
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
    static int ioPathconf(resmgr_context_t *ctp, io_pathconf_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioPathconf
     */
    virtual int IoPathconf(resmgr_context_t *ctp, io_pathconf_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioLseek(resmgr_context_t *ctp, io_lseek_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

        return( ioMgr->IoLseek(ctp, msg, ioOcb));
    }

    /**
     * Handler called in response to a client lseek() call. Called by ioLseek
     *
     * @param ctp    Dispatch interface pointer
     * @param msg    Lseek message (how far to seek and from where)
     * @param ioOcb  Control block for the calling client
     * @return 0 if successful
     *         < 0 if error
     * @see ResManager::ioLseek
     */
    virtual int IoLseek(resmgr_context_t *ctp, io_lseek_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioChmod(resmgr_context_t *ctp, io_chmod_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioChmod
     */
    virtual int IoChmod(resmgr_context_t *ctp, io_chmod_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioChown(resmgr_context_t *ctp, io_chown_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioChown
     */
    virtual int IoChown(resmgr_context_t *ctp, io_chown_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioUtime(resmgr_context_t *ctp, io_utime_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioUtime
     */
    virtual int IoUtime(resmgr_context_t *ctp, io_utime_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioOpenFd(resmgr_context_t *ctp, io_openfd_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioOpenFd
     */
    virtual int IoOpenFd(resmgr_context_t *ctp, io_openfd_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioSpace(resmgr_context_t *ctp, io_space_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;
        int             retVal = ENOSYS;

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
     * @see ResManager::ioSpace
     */
    virtual int IoSpace(resmgr_context_t *ctp, io_space_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioMmap(resmgr_context_t *ctp, io_mmap_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioMmap
     */
    virtual int IoMmap(resmgr_context_t *ctp, io_mmap_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioMsg(resmgr_context_t *ctp, io_msg_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;

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
     * @see ResManager::ioMsg
     */
    virtual int IoMsg(resmgr_context_t *ctp, io_msg_t *msg, resMgrIoOcb_t *ioOcb) {
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
    static int ioSync(resmgr_context_t *ctp, io_sync_t *msg, RESMGR_OCB_T *ocb) {
        resMgrIoOcb_t   *ioOcb = (resMgrIoOcb_t*)ocb;
        ResManager      *ioMgr = (ResManager*)ioOcb->m_mgr;
        int             retVal = ENOSYS;

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
     * @see ResManager::ioSync
     */
    virtual int IoSync(resmgr_context_t *ctp, io_sync_t *msg, resMgrIoOcb_t *ioOcb) {
        return( iofunc_sync_default( ctp, msg, (iofunc_ocb_t*)ioOcb));
    }

    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //      I/O Handler methods
    ///////////////////////////////////////////////////////////////////////////

    /**
     * Routine invoked by the resource manager library in response to an
     * attached pulse.
     *
     * @param ctp    Dispatch context pointer (from dispatch_context_alloc)
     * @param code   Code for the pulse we received
     * @param flags  ????? (no documentation in API)
     * @param handle Pointer to a ResManager object
     * @return 0 if successful; -1 if an error occurred
     */
    static int PulseHandler( message_context_t *ctp, int code, unsigned flags, void *handle);

    /**
     * Function invoked inresponse to a signal to our process
     *
     * @param SigNo  Siagnal we caught
     */
    static void SignalHandler( int SigNo);

    /**
     * Pulse code we have attached to our resource manager's I/O message channel
     */
    int                     m_ioChnlPulseCode;

    /**
     * Connection created in order for our resource manager to receive
     * generic messages and pulses on his I/O message channel
     */
    int                     m_mssgCoid;

    /**
     * Parser to maintain setup/config settings
     *
     * @see XmlParser
     */
    XmlParser               m_cfgParser;

    /**
     * Table of named data items
     * @since 04/18/2001
     */
    HwDataMap               m_dataTable;

    /**
     * Flag to say if in demo mode or real mode
     */
    bool                    m_demoMode;

    /**
     * Registered name identifier
     *
     * @see name_attach
     */
    name_attach_t           *m_nameId;

    /**
     * Event used to signal us when our timer expires
     */
    struct sigevent         m_timerEvent;

    /**
     * ID for our timer to be used to scan VME space
     */
    int                     m_scanTimerId;

    /**
     * Connection ID for connecting a timer to our "named" channel
     */
    int                     m_timerConnectId;

    /**
     * Structure to allow us to patch in our own ocb allocating & freeing functions
     */
    iofunc_funcs_t          m_ocbFuncs;

    /**
     * The mount structure used to patch in our OCB alloc/free functions
     */
    iofunc_mount_t          m_mountPoint;

    /**
     * Dispatch interface handle (used by the kernel)
     */
    dispatch_t              *m_dispatchPtr;

    /**
     * Attributes for our manager
     */
    resmgr_attr_t           m_resmgrAttr;

    /**
     * Context pointer for our manager (message info, etc)
     */
    resmgr_context_t        *m_ctp;

    int                     m_resMgrId;

    /**
     * Connection handler pointers (to handle client connections)
     */
    resmgr_connect_funcs_t  m_connectFuncs;

    /**
     * I/O Handlre pointers (to handle client I/O requests)
     */
    resmgr_io_funcs_t       m_ioFuncs;

    /**
     * Function attributes for I/O functions (offset, file size, etc)
     */
    IOFUNC_ATTR_T           m_ioFuncAttr;

    /**
     * List of 'cancelable' XML threads
     */
    ThreadList              m_threadList;

    /**
     * Handle data passed into XML Thread Pool Functions
     */
    threadPoolHandle_t      m_xmlPoolHandle;

    /**
     * Default attributes for our XML Thread pool
     *
     * @see thread_pool_attr_t
     */
    thread_pool_attr_t      m_xmlPoolAttribs;

    /**
     * Attributes for threads in the thread pool
     */
    pthread_attr_t          m_xmlThreadAttributues;

    /**
     * Thread pool handle
     */
    thread_pool_t           *m_xmlThreadPool;

    /**
     * Last XML command received
     */
    ResManagerCmd_t         m_xmlCommand;

    /**
     * Map used to hold only data items that represent full PLC word data
     */
    WholeChannelItemMap     m_wholeChannelItemMap;

    /**
     * Fault server interface object. Used to publish fault data to the
     * fault server
     */
    IFaultServer            *m_faultServer;

    /**
     * Flag used to indicate whether or not we have a valid notification list
     */
    bool                    m_haveSubscriptions;

    /**
     * Priority to run the pool threads at
     */
    int                     m_poolThreadPriority;

private:
    /**
     * Pointer needed to initialize ResManager* inside the OCB
     */
    static ResManager*      m_ioMgrForOcb;

    /**
     * Object name to include in print statements
     */
    string                  m_taskName;

    /**
     * Barrier used to wait for XML thread pool to initialize
     */
    pthread_barrier_t       m_xmlThreadBarrier;

};

#endif  // _RESMANAGER_H_INCLUDED

