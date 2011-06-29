//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/Include/ResManagerXml.h $
// $Author:: Rwiersem                                                         $
//   $Date:: 10/30/07 4:03p                                                   $
//
// Description:
//      Multi-threaded resource manager base class which provides methods
//      for handling BEP XML messages. Uses a thread pool to handle XML
//      based messages and the main thread of execution to handle I/O
//      messages. Adapted from ResourceManager.cpp.
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/Include/ResManagerXml.h $
// 
// 3     10/30/07 4:03p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
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
// 2     6/26/03 3:56p Khykin
// Updated with Brians latest changes.
//
// 1     6/24/03 2:09p Bmeinke
//
// 1     4/30/03 4:44p Bmeinke
// Migration to new personalized configuration management structure
//
// 9     4/30/03 2:56p Bmeinke
// Removed Write( strin, string)
// Removed Subscribe(), Unsubscribe() and all the PublishData() methods
//
// 8     3/18/03 7:00p Bmeinke
// Read() and Write() now accept ONLY XmlNode* arguments
//
// 7     3/13/03 12:24p Bmeinke
// Added Read() and Write() methods which take XML Node pointers
//
// 6     10/24/02 8:15p Bmeinke
// Removed unused overloaded virtual methods inherited form BepComponent
//
// 5     8/01/02 5:39p Bmeinke
// Removed compiler warnings
//
// 4     5/24/02 3:02p Kh
// Updating due to the possibility that the notification list does not
// exist.
//
// 3     2/27/02 3:04p Bmeinke
// Changed threadPoolParam_t to THREAD_POOL_PARAM_T in static thread pool
// methods for compatability with other thread pool objects
// Removed HandlePulse() method (method is provided in the base
// ResManagerMt class)
//
// 2     12/07/01 2:54p Brian.meinke
// Changed thread pool interface prototypes in the base class ResManagerMt
//
// 1     11/21/01 8:40p Brian.meinke
// Multi-threaded resource manager base class which provides methods for
// handling BEP XML messages.
//
//******************************************************************************

#ifndef _RESMANAGER_H_INCLUDED
#define _RESMANAGER_H_INCLUDED

// Forward Class Definitions
class ResManagerXml;
class TimeToDie;
class ThreadList;
struct _xmlPoolHandle_t;
struct _xmlPoolParam_t;

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

#include "ResManagerMt.h"       // Resource manager base class


// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Header Files

#include <sys/neutrino.h>       // Needed for _msg_info
#include <string>

#include "IBepCommunication.h"  // IBepCommunication objects


///////////////////////////////////////////////////////////////////////////////
//      THREAD POOL STUFF
///////////////////////////////////////////////////////////////////////////////

/**
 * Data type passed to the ThreadPool handler functions
 *
 * @author Brian Meinke
 * @version 1
 */
typedef struct _xmlPoolParam_t
{
    /**
     * Parameter type expected by base class thread pool functions
     */
    threadPoolParam_t   baseParam;

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
} xmlPoolParam_t;

/**
 * Ain Thread Argument type
 */
typedef struct _resMgrThreadArg_t
{
    /**
     * Resource Manager object for whom the thread is being read
     */
    ResManagerXml       *ioMgr;

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

/**
 * Data type used to hold the list of tags we publish. Index based on
 * data item board, channel adn bit indices.
 */
typedef map< int, XmlString>                PublishedTagList;
typedef PublishedTagList::iterator          PublishedTagListItr;
typedef PublishedTagList::const_iterator    PublishedTagListCItr;
typedef PublishedTagList::value_type        PublishedTagListVal;

/**
 * Argument data type passed to our publisher threads
 */
class PublishThreadArg_t
{
public:
    PublishThreadArg_t( const XmlString &tag, bool val, ResManagerXml *mgr) :
    dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%d"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const XmlString &tag, char val, ResManagerXml *mgr) :
    dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%c"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const XmlString &tag, UINT8 val, ResManagerXml *mgr) :
    dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%d"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const XmlString &tag, int val, ResManagerXml *mgr) :
    dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%d"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const XmlString &tag, long val, ResManagerXml *mgr) :
    dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 10];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%ld"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const XmlString &tag, float val, ResManagerXml *mgr) :
    dataTag( tag), ioMgr( mgr)
    {
        XML_Char    buf[ 100];
        pthread_barrier_init( &barrier, NULL, 2);
        _tsprintf( buf, XML_T("%f"), val);
        dataVal = XmlString( buf);
    }

    PublishThreadArg_t( const XmlString &tag, const XmlString &val, ResManagerXml *mgr) :
    dataTag( tag), ioMgr( mgr)
    {
        pthread_barrier_init( &barrier, NULL, 2);
        dataVal = XmlString( val);
    }

    PublishThreadArg_t( const XmlString &tag, const XML_Char *val, ResManagerXml *mgr) :
    dataTag( tag), ioMgr( mgr)
    {
        pthread_barrier_init( &barrier, NULL, 2);
        dataVal = XmlString( val);
    }

    ~PublishThreadArg_t()
    {
        pthread_barrier_destroy( &barrier);
    }

    XmlString           dataTag;
    XmlString           dataVal;
    pthread_barrier_t   barrier;
    ResManagerXml       *ioMgr;
};

/**
 * Base class for all Resource Managers
 *
 * @author Brian Meinke
 * @version 2
 */
class ResManagerXml : public ResManagerMt
{
public:

    /**
     * Default constructor
     *
     * @param name   Name to register with the OS as
     */
    ResManagerXml( const char *name="");

    /**
     * Class destructor
     */
    ~ResManagerXml();

    /**
     * Performs any necessary manager cleanup (free memory, detach any
     * adopted path names, etc)
     */
    virtual void CleanupManager();

    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     * @since Version 2.0
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate=0)
    {
        const std::string tag( node->getName());
        string              replyVal;
        const dataMapItem_t *dataItem;

        Log( LOG_FN_ENTRY, "Enter ResManagerXml::Read( %s)\n", tag.c_str());
                                            
        // Try to find this tag in our data item table
        if((dataItem = m_dataTable.find( tag)) != NULL)
        {
            // Fill in node with data value
            if(ReadDataItem( dataItem, replyVal) == -1)
            {
                replyVal = BEP_INVALID_RESPONSE;
            }
        }
        else
        {
            Log( LOG_ERRORS, "Invalid name for reading: <%s>\n", tag.c_str());
            replyVal = BEP_INVALID_RESPONSE;
        }

        Log( LOG_FN_ENTRY, "Exit ResManagerXml::Read( %s)\n", tag.c_str());
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
    virtual const std::string Write(const XmlNode *node)
    {
        const string        tag( node->getName()), value( node->getValue());
        string              replyVal;
        const dataMapItem_t *dataItem;

        Log( LOG_FN_ENTRY, "Enter ResManagerXml::Write( %s, %s)\n", tag.c_str(), value.c_str());

        // Try to find this tag in our data item table
        if((dataItem = m_dataTable.find( tag)) != NULL)
        {
            // Fill in node with data value
            if(WriteDataItem( dataItem, value) != -1)
            {
                replyVal = BEP_SUCCESS_RESPONSE;
            }
        }
        else
        {
            Log( LOG_ERRORS, "Invalid name for writing: <%s>\n", tag.c_str());
            replyVal = BEP_INVALID_RESPONSE;
        }

        Log( LOG_FN_ENTRY, "Exit  ResManagerXml::Write( %s, %s)\n", tag.c_str(), value.c_str());
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
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     * @since Version 1.0
     */
    virtual void Terminate(void)
    {
        HandleTerminate( 0);
    }

protected:

    /**
     * Kills any thread-pool threads that are hanging around
     */
    virtual void KillPoolThreads();

    /**
     * The purpose of this is to give us a place to allocate our own ocb. It is
     * called by ocbCalloc, which in turn is called as a result of an open being done
     * on our reource manager's adopted path(e.g. iofunc_open_default causes it to be
     * called). ocbCalloc was registered through our mount structure.
     *
     * @param ctp    Resource manager's context block
     * @param device Attributes for this client connection (an OCB)
     * @return Pointer to newly allocated OCB or NULL if error
     * @see ResManagerXml::ocbCalloc
     */
    virtual IOFUNC_OCB_T* OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
    {
        Log( LOG_FN_ENTRY, "Enter OcbCalloc\n");

        // Allocate and clear a new OCB for this connection
        resMgrIoOcb_t *ocb = new resMgrIoOcb_t;
        memset( ocb, 0, sizeof( ocb));

        ocb->m_mgr = this;

        Log( LOG_FN_ENTRY, "Exit OcbCalloc\n");

        return( ocb);
    }

    /**
     * The purpose of this is to give us a place to free our ocb. It is called
     * by ResManagerXml::ocbFree, which in turn is called as a result of the close
     * being done (e.g. iofunc_close_ocb_default causes it to be called).
     * ResManagerXml::ocbFree was registered through our mount structure.
     *
     * @param ocb    Client's Open Context Block for this connection
     * @see ResManagerXml::ocbFree
     */
    virtual void OcbFree(resMgrIoOcb_t *ocb)
    {
        Log( LOG_FN_ENTRY, "Enter OcbFree\n");

        // Free the memory
        delete ocb;
        ocb = NULL;

        Log( LOG_FN_ENTRY, "Exit OcbFree\n");
    }

    /**
     * method for component initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * Handles I/O mesages
     *
     * @return RES_MGR_CMD_TERMINATE if a terminate pulse was received
     *         RES_MGR_CMD_CONFIG if a reload config pulse was received
     *         RES_MGR_CMD_NONE for all other pulses
     */
    virtual ResManagerCmd_t IoMssgLoop();

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
     * Calculates the index of the given tag in our list of published tags.
     * Index is calculated based on board index, channel index, bit index
     * and channels per board
     *
     * @param dataItem Item to calculate index for
     * @return Index for the given data item
     */
    int CalculateDataTagIndex( const dataMapItem_t *dataItem);

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
    int CalculateDataTagIndex( int board, int channel, UINT16 bit);

    /**
     * Checks if the given channel on the given board should be treated as a
     * single value as opposed to a group of individual bits
     *
     * @param boardIdx   Board index of the data item to evaluate
     * @param channelIdx Channel index of the data item being evaluated
     * @return true if the channel should be treated as a singel value
     *         false if it should be treated as 8 individual bit-type data items
     */
    bool IsWholeChannelItem( int boardIdx, int channelIdx);

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
    virtual threadPoolParam_t *PoolContextAlloc(threadPoolHandle_t *handle);

    /**
     * Initializes the data in the thread pool parameter data type
     *
     * @param handle    Thread pool handle
     * @param poolParam Thread instance data
     * @return 0 if successful, -1 if an error occurrs
     */
    virtual int InitializePoolParam(threadPoolHandle_t *handle, threadPoolParam_t *poolParam);

    /**
     * Function called to free memory used for a thread argument struct.
     *
     * @param ctp  Thread context structure to free
     */
    virtual void PoolContextFree(threadPoolParam_t *ctp);

    /**
     * Function called after our thread has left the blocked state and is ready
     * to do some work
     *
     * @param threadData Thread context structure for this thread to work with
     * @return 0
     */
    virtual int PoolHandlerFunc( threadPoolParam_t *threadData);

    /**
     * Function in which each thread in the thread pool will block
     *
     * @param threadData Thread context structure
     * @return
     */
    virtual threadPoolParam_t *PoolBlockFunc( threadPoolParam_t *threadData);

    /**
     * Function called when our thread returns from the block function
     *
     * @param ctp    Thread context structure
     */
    virtual void PoolUnBlockFunc(threadPoolParam_t *ctp);

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


    /**
     * Name of this object's configuration file
     */
    string                  m_cfgFileName;

    /**
     * Table of named data items
     * @since 04/18/2001
     */
    HwDataMap               m_dataTable;

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
};

#endif  // _RESMANAGER_H_INCLUDED

