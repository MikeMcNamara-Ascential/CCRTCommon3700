//******************************************************************************
//
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/SystemCommunication/IBepCommunication.cpp 6     10/26/06 10:02a Rwiersem $
//
// Description:
//  Class to encapsulate Burke Porter Communication protocol details
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/SystemCommunication/IBepCommunication.cpp $
// 
// 6     10/26/06 10:02a Rwiersem
// Changes for the 06102701 core release:
// 
// Added Activate() and Deactivate().  These are used to keep the test 
// components running when a new drive curve starts.
// 
// 5     4/13/06 3:46a Cward
// Init vars
//
// 2     2/15/06 3:57p Bmeinke
// Initialize some uninitialized variables
//
// 4     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 5     10/24/03 1:01p Khykin
// Updated with latest mutex changes.
//
// 3     10/15/03 8:46a Khykin
// Updated with latest Korea changes.
//
// 2     10/10/03 9:36a Khykin
// Updated with latest Flatrock changes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 2     8/07/03 10:22a Khykin
// Updated with latest changes for Flatrock.
//
// 1     6/12/03 4:53p Khykin
// Initial checkin.
//
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:03:43  tbochene
//
//     PooBag
//
//
// 1     1/27/03 6:26a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 120   11/21/02 8:27a Khykin
// Added the maintanence of the timeout and buffer size for workers to
// use.
//
// 119   11/19/02 8:59p Tbochene
// Updated GetName() to return string representation of socket for TcpIp
// communication objects inside
//
// 118   9/24/02 7:10p Tbochene
// Put try/catch around logic in HandleRequst
//
// 117   9/12/02 6:17p Tbochene
// Updated error response
//
// 116   9/10/02 5:53p Tbochene
// Changed Initialize to match changes in IOpenChannelIPC class
// initializer
//
// 115   8/23/02 9:04a Kh
// Synchronizing VSS with the pit machine.
//
// 114   8/14/02 9:47a Skypig13
// Removed copy from GetNext() thus removing momory leak
//
// 113   7/31/02 10:49a Skypig13
// Added update of server status on receipt of a terminate message
//
// 112   6/26/02 3:19p Skypig13
// Update so error tags contain Error as value
//
// 111   6/26/02 2:26p Skypig13
// Updated handling of not well formed messages
//
// 110   6/25/02 5:00p Skypig13
// Update for GetNext() returning tag/value properly if an Error response
// is received
//
// 109   6/19/02 2:01p Skypig13
// Updated with new GetNext() method
//
// 108   6/18/02 12:48p Dan.erickson
// Modified HandleRequest().
//  - Added messagePopulated flag to support subscriptions to
// TestResultServer.
//  - Made the method virtual so ITestResultServer could extend.
//
// 107   6/07/02 11:57a Dan.erickson
// Updated FindNode() to use if else with an iterator instead of
// the try catch block.
// Updated HandleRequest() to only get the message from the client
// if the message is not passed in.
//
// 106   5/06/02 3:41p Dan.erickson
// Node maps were not always being cleared beofre they went out
// of scope.  This was causing a memory leak.  All node maps
// are now cleared before going out of scope.
//
// 105   4/25/02 4:36p Skypig13
// Updated with new changes
//     Revision 1.1.1.1  2002/04/08 19:49:05  erickson
//     started
//
//
// 103   4/08/02 2:05p Skypig13
// Updates for improving efficiency
//
// 102   4/02/02 10:43p Skypig13
// Changes to improve performance and added interface IPublish
//
// 101   3/19/02 11:04a Kh
// Updating to handle pulses of types other than system.
//
// 100   3/11/02 10:19a Dan.erickson
// Reparsed document after ValidateMessage is called.  The buffer was
// getting whacked.
//
// 99    2/28/02 2:05p Skypig13
// Updated Initialize()
//
// 98    2/19/02 4:46p Skypig13
// Removed debugging stuff in HandleRequest()
//
// 97    2/19/02 4:39p Skypig13
// Optimizations
//
// 96    2/18/02 10:09a Skypig13
// Conditioned population of out parameter message only if debug is on
//
// 95    2/12/02 5:10p Dan.erickson
// Updated to make better use of XmlNode type data.
//
// 94    2/07/02 6:25p Skypig13
// Updated Initialize() to match change in ITcpIp
//
// 93    2/07/02 1:07p Skypig13
// Changed Initialize for TCP/IP object to reflect
// changes in ITcpIp::Initialize() and made print
// in SendResponse() more descriptive
//
// 92    2/05/02 12:04a Skypig13
// Fixed oops in HandleRequest()
//
// 91    2/04/02 11:55p Skypig13
// Updating to include performace enhancements
//
// 89    1/17/02 1:11p Brian.meinke
// Add SetDebug( debug) to Initialize() with a gazillion parameters
//
// 88    1/04/02 9:49a Skypig13
// Updated GetNext() to return Error in value if message type was Error
//
// 87    12/20/01 3:37p Skypig13
// Removed THREAD_DEATH case in HandleRequest()
//
// 86    12/19/01 7:32p Skypig13
// Improved inititalize method
//
// 85    12/18/01 11:31a Kh
// Chainging the Register from a pulse to a Method.
//
// 84    12/14/01 5:45p Kh
// Adding a Resigter method to deal with subscription issues.
//
// 83    12/10/01 4:18p Skypig13
// Added better diagnostic in GetNext()
//
// 82    12/06/01 8:16a Dan.erickson
// Added handling for Write messages in SpecialProcessing.
//
// 81    12/04/01 1:08p Skypig13
// Added backlog configuration for TCP/IP communications
//
// 80    11/29/01 5:56p Dan.erickson
// Added Read, Write, Publish and Command methods that accept
// a XmlNode as an argument.
//
// 79    11/20/01 4:26p Kh
// Adding detection for the TIMER pulse.
//
// 78    11/15/01 9:02a Dan.erickson
// Modified HandleRequest method to reparse the message after
// Validating it in case the parser buffer gets whacked.
//
// 77    11/14/01 7:17p Dan.erickson
// Updated SpecialProcessing method to call the server
// SpecialProcessing method for Publish messages.
//
// 76    11/02/01 10:30a Skypig13
// Changed printout in GetNext()
//
// 75    11/01/01 9:49p Skypig13
// Moved bodies of some methods from header file
//
// 74    11/01/01 6:05p Dan.erickson
// Removed const for BepComponent item in SpecialProcessing prototype.
//
// 73    11/01/01 4:27p Dan.erickson
// In the Initialize method, moved Reply id to conform with
// IOpenChannelIpc initialize prototype.
//
// 72    11/01/01 3:31p Dan.erickson
// Added name to Initialize method prototype for
// IInterProcessCommunication.
//
// 71    11/01/01 11:24a Dan.erickson
// Modified BuildMessage to add the existing node to the message instead
// of creating a new node with just the tag and value (attributes were
// being lost).
// Added SpecialProcessing method to handle attributes to nodes in a
// message.
//
// 70    10/25/01 5:13p Skypig13
// Fixed diagnostics in BuildMessage()
//
// 69    10/25/01 12:47p Skypig13
// Improved initialization and fixed validate message
//
// 68    10/24/01 2:34p Skypig13
// Changed doc->ToString() to doc->writeNode() in HandleRequest() on line
// 716
//
// 67    10/23/01 6:28p Skypig13
// Added number of items left to printout in GetNext()
//
// 66    10/22/01 10:12p Skypig13
// Better printouts in DoOperation()
//
// 65    10/22/01 5:35p Skypig13
// Made creating workers more efficient for IPC
//
// 63    10/19/01 1:59p Skypig13
// Make ValidateMessage() faster and a few other time optimizations
//
// 62    10/19/01 11:40a Skypig13
// Make ValidateMessage() faster
//
// 60    10/18/01 8:13p Skypig13
// Next optimization in Initialize()
//
// 59    10/18/01 3:51p Skypig13
// Made initializer more efficient eliminating unnecessary new and delete
// calls
//
// 58    10/17/01 10:00a Skypig13
// Changed prototype of Read() to include data reate
//
// 57    10/16/01 4:27p Dan.erickson
// Added initialization of m_description member.
//
// 56    10/12/01 10:28a Dan.erickson
// Modified Initialize method to allow initialization of Tcp/Ip objects.
//
// 55    10/08/01 10:03p Skypig13
// Fixed printout and initializer in Initialize() and put diagnostic at
// end of HandleRequest()
//
// 54    10/04/01 3:01p Skypig13
// Changed BuildMessage() to use AddAttribute()
//
// 53    10/03/01 2:54p Skypig13
// Added NULL check in Initialize() and response to Terminate() and other
// command methods
//
// 52    10/02/01 9:29a Skypig13
// Changes in Initialize() due to timeout being moved to
// ISystemCommunication
//
// 51    10/01/01 4:31p Skypig13
// Made diagnostic prints more descriptive
//
// 50    9/27/01 3:55p Skypig13
// Added NULL check of communication object inside
//
// 49    9/26/01 9:00a Skypig13
// Added GetCommunicationType() method
//
// 48    9/18/01 7:19a Skypig13
// Cleared data for diagnostics
//
// 47    9/14/01 2:34p Skypig13
// Add publish method and thread death pulse handler and cleaned up some
// msic. details
//
// 46    9/11/01 2:19p Dan.erickson
// Corrections from testing ClientGetInValidDataTest method.
//
// 45    9/10/01 3:15p Dan.erickson
// Corrections from testing UseWorker method.
//
// 44    9/07/01 10:50a Skypig13
// Allow TcpIp object to GetRequest()
//
// 43    9/06/01 2:42p Skypig13
// Added isFinal == true to terminate abort etc methods.
//
// 42    8/30/01 4:14p Skypig13
// Add HandlePulse() method
//
// 41    8/28/01 8:24p Skypig13
// Change parameters to not take defaults and add timeout to Write()
//
// 40    8/22/01 8:14a Kh
// Updating deletes to set the deleted item to NULL for later check.
//
// 39    8/15/01 9:29a Kh
// Making changes for updated Subscribe and Unsubscribe BepCompone
//
// 38    8/14/01 1:48p Skypig13
// Changed DoOperation() so it will not send a bad message.
//
// 37    8/02/01 4:23p Skypig13
// Client side commands return BEP_STATUS_FAILURE if any item
// requested has invalid data (Unavailable, Error, etc);
//
// 36    8/02/01 3:52p Skypig13
// Fixed deletion of OldDoc in BuildMessage() so it only happens on good
// message.
//
// 35    7/31/01 4:16p Kh
// Updating for new clear method from the XmlNodeMap.
//
// 34    7/25/01 2:41p Skypig13
// Added GetByTag() for Kevin
//
// 33    7/25/01 9:47a Kh
// Changed the static local oper in DoOperation to m_oper in the class.
// It behaved like a class static variable (Bad!).
//
// 32    7/24/01 11:18a Skypig13
// Fixed false sucess if write failed in DoOperation
//
// 31    7/23/01 4:54p Kh
// Fixing bug in server type detection.
//
// 30    7/23/01 4:29p Kh
// Adding Exceptions for improper initialization.
//
// 29    7/20/01 10:03a Skypig13
// Changed prototype for Subscribe() and Unsubscribe() for Kevin
//
// 28    7/20/01 9:25a Skypig13
// Change for consistent handling of pulses
//
// 26    7/19/01 10:24a Skypig13
// Added ReloadConfiguration()
//
// 25    7/18/01 4:37p Skypig13
// Fixed IPC initializer in Initialize(string, bool)
//
// 24    7/18/01 4:15p Skypig13
// Fixed switch statement in handle pulse
//
// 23    7/17/01 7:01p Skypig13
// Add pulse handling
//
// 21    7/17/01 8:56a Skypig13
//
// 20    7/11/01 7:25p Skypig13
// Changed m_sendData to XmlNodeMap and tested
//
// 19    7/11/01 5:55p Skypig13
// Removed XmlNodeMap for m_sendData and added discrete parameter
// initialization.
//
// 17    7/06/00 10:09p Skypig13
// Derive from Debuggable
//
// 16    7/06/00 4:03p Skypig13
// Added Write data test
//
// 15    7/03/00 9:28p Skypig13
// Server-side testing complete
//
// 14    7/03/00 6:22p Skypig13
// Tweak GetNext() to return -1 and Unavailable if no more data exists
//
// 13    7/03/00 4:40p Skypig13
// First client-side test working for happy-day read
//
// 12    7/03/00 11:08a Skypig13
// Changed methods GetRequestType() and ValidateMessage() to accept a bool
// parameter to determine whether or not to parse message or use document
// already parsed.
//
// 11    6/28/00 6:53p Skypig13
// Updated for client-side tests
//
// 10    6/28/00 1:06p Skypig13
// Updating entire project to get latest version
//
// 9     6/27/00 8:23p Skypig13
// All server-side tests complete
//
// 8     6/26/00 2:36p Skypig13
// Testing server-side happy-day complete
//
// 7     6/22/00 4:53p Skypig13
// Addded worker class and tested
//
// 6     6/21/00 5:48p Skypig13
// Testing functionality of IPC and OpenChannel tests.
// Made some interface changes
//
// 5     6/13/01 7:23p Admin
// Change Boolean to bool
//
// 4     6/13/01 7:06p Skypig13
// Created visitor pattern interfaces
//
// 3     6/08/01 12:53p Skypig13
// Weekly backup
//
//******************************************************************************
#include <typeinfo>
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif
#include "BepComponent.h"
#include "IBepCommunication.h"
#include "ISystemCommunication.h"
#include "IInterProcessCommunication.h"

IBepCommunication::IBepCommunication(void) : Debuggable(), IPublish(),
m_ISystemCommunication(NULL), m_terminate(false), m_rxBufferSize(DEFAULT_BUFFER_SIZE), m_rate(0)
{
    memset( &m_lockAttr, 0, sizeof( m_lockAttr));
    memset( &m_lock, 0, sizeof( m_lock));

    if( (errno=pthread_mutexattr_init( &m_lockAttr)) != EOK)
    {
        perror( "IBepCommunication: pthread_mutexattr_init error");
    }
    else if( (errno=pthread_mutexattr_settype( &m_lockAttr, PTHREAD_MUTEX_RECURSIVE)) != EOK)
    {
        perror( "IBepCommunication: pthread_mutexattr_settype error");
    }
    else if( (errno=pthread_mutexattr_setrecursive( &m_lockAttr, PTHREAD_RECURSIVE_ENABLE)) != EOK)
    {
        perror( "IBepCommunication: pthread_mutexattr_setrecursive error");
    }
    else if((errno=pthread_mutex_init( &m_lock, &m_lockAttr)) != EOK)
    {
        perror( "IBepCommunication: pthread_mutex_init error");
    }
    // Pre-parse to get a document to speed things up later
    char* temp = "<"BEP_XML_ROOT"/>";
    m_parser.Parse(temp, strlen(temp), true, "");
}
IBepCommunication::~IBepCommunication(void)
{
    pthread_mutexattr_destroy( &m_lockAttr);
    pthread_mutex_destroy( &m_lock);

    // If a communication object was created
    if(m_ISystemCommunication) delete m_ISystemCommunication;
    m_ISystemCommunication = NULL;
}
void IBepCommunication::Initialize(const INT32 id, const std::string name,
                                   const bool debug /* = false */, const INT32 rid /* = -1 */,
                                   const INT32 size /* = DEFAULT_BUFFER_SIZE */,
                                   const INT32 timeout /* = DEFAULT_TIMEOUT */)
{
    SetDebug(debug);
    if(IsDebugOn()) printf("IBepCommunication::Initialize(%d, %s, %d, %d, %d, %d) from thread %d\n",
                           id, name.c_str(), debug, rid, size, timeout, pthread_self());
    IInterProcessCommunication* ipc = NULL;
    if((ipc = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) == NULL)
    {
        if(m_ISystemCommunication)
        {
            if(IsDebugOn()) printf("Deleting IPC object\n");
            delete m_ISystemCommunication;
            m_ISystemCommunication = NULL;
        }
        if(IsDebugOn()) printf("Creatng IPC object\n");
        m_ISystemCommunication = new IInterProcessCommunication(id, name, debug, rid, size, timeout);    // Initialize object
        SetRxBufferSize(size);  // update the size of the buffer
        SetTimeout(timeout);    // update the timeout time
    }
    else ipc->Initialize(id, name, debug, rid, size, timeout);    // Initialize object
}
void IBepCommunication::Initialize(const INT32 id, const INT16 port /* = 0 */,
                                   const std::string address    /*= localhost */,
                                   const std::string frameStart /* = BEP_XML_ROOT */,
                                   const std::string frameEnd    /* = BEP_XML_ROOT_END */,
                                   const bool debug    /* = false */, const INT32 backlog    /* = 5 */)
{
    SetDebug(debug);
    if(IsDebugOn()) printf("IBepCommunication::Initialize(%d, %d, %s, %s, %s, %d, %d) from thread %d\n",
                           id, port, address.c_str(), frameStart.c_str(), frameEnd.c_str(), debug, backlog, pthread_self());
    ITcpIp* tcp = NULL;
    if((tcp = dynamic_cast<ITcpIp *>(m_ISystemCommunication)) == NULL)
    {
        if(m_ISystemCommunication)
        {
            if(IsDebugOn()) printf("Deleting TCP/IP object\n");
            delete m_ISystemCommunication;
            m_ISystemCommunication = NULL;
        }
        if(IsDebugOn()) printf("Creatng TCP/IP Server object\n");
        m_ISystemCommunication = new ITcpIp("Server", port, address, id, debug, frameStart, frameEnd, backlog);
    }
    else tcp->Initialize("Server", port, address, id, debug, frameStart, frameEnd, backlog);
}
void IBepCommunication::Initialize(const XmlNode *config, const INT32 id /* = -1 */, const INT32 rid /* = -1 */)
{
    XmlNodeMap nodes;
    try
    {
        // Process configuration information
        if(config)
        {
            // Get children of configuration node
            nodes.DeepCopy(config->getChildren());

            const std::string commType = nodes.getNode("Comm")->getValue();
            SetCommunicationType(commType);
            SetDebug(nodes.getNode("Debug")->getValue());

            if(IsDebugOn()) printf("IBepCommunication::Initialize(%s, %d, %d) from thread %d\n", config->ToString().c_str(), id, rid, pthread_self());
            const std::string type = nodes.getNode("Type")->getValue();

            if(IsDebugOn()) printf("Initialize a");

            if(commType == "IPC")
            {
                // Create an IPC object
                if(IsDebugOn()) printf("n IPC %s object\n", type.c_str());
                if(dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication) == NULL)
                {
                    if(m_ISystemCommunication)
                    {
                        if(IsDebugOn()) printf("Deleting IPC object\n");
                        delete m_ISystemCommunication;
                        m_ISystemCommunication = NULL;
                    }
                    if(IsDebugOn()) printf("Creatng %s object\n", type.c_str());
                    m_ISystemCommunication = new IInterProcessCommunication();
                    SetRxBufferSize(atoi(nodes.getNode("BufferSize")->getValue().c_str())); // update the size of the buffer
                    SetTimeout(atoi(nodes.getNode("Timeout")->getValue().c_str()));         // update the timeout time
                }
                else if(IsDebugOn()) printf(" is ok\n");
            }
            else if(commType == "TCP/IP")
            {
                // Create a TCP/IP object
                if(IsDebugOn()) printf(" TCP/IP %s object\n", type.c_str());
                if(dynamic_cast<ITcpIp *>(m_ISystemCommunication) == NULL)
                {
                    if(m_ISystemCommunication)
                    {
                        if(IsDebugOn()) printf("Deleting TCP/IP object\n");
                        delete m_ISystemCommunication;
                        m_ISystemCommunication = NULL;
                    }
                    if(IsDebugOn()) printf("Creatng TCP/IP %s object\n", type.c_str());
                    m_ISystemCommunication = new ITcpIp();
                }
            }
            else throw BepException("Unknown communication type '%s' incurred in IBepCommunication::Inititalize()", commType.c_str());
            m_ISystemCommunication->Initialize(config, id, rid);    // Initialize object
            // Clear out our temporary map
            if(!nodes.empty()) nodes.clear(true);
        }
        else throw XmlException("Node passed to IBepCommunication::Initialize() is NULL");
    }
    catch(BepException &BepErr)
    {
        // If error delete object and rethrow exception
        if(IsDebugOn()) printf("IBepCommunication::Initialize() failed due Bep error: %s\n", BepErr.what());
        if(m_ISystemCommunication) delete m_ISystemCommunication;
        m_ISystemCommunication = NULL;
        if(!nodes.empty()) nodes.clear(true);
        throw;
    }
    catch(...)
    {
        // If error delete object and rethrow exception
        if(m_ISystemCommunication) delete m_ISystemCommunication;
        m_ISystemCommunication = NULL;
        if(!nodes.empty()) nodes.clear(true);
        throw;
    }
}
void IBepCommunication::Initialize(IBepCommunication &creator, INT32 id)
{
    // Set diagnostic state
    SetDebug(creator.IsDebugOn());
    if(IsDebugOn()) printf("IBepCommunication::Initialize(%p, %d) from thread %d\n", &creator, id, pthread_self());
    if(this != &creator)
    {
        // Ensure that the component is not being initialized with itself
        ITcpIp* tcp, *newTcp;               // pointer to use in cast attempt
        IInterProcessCommunication *ipc;    // same as above
        IOpenChannelIpc *ioc;               // same same
        XmlString type;                     // What type of communication object do we have
        if((ipc = dynamic_cast<IInterProcessCommunication *>(creator.m_ISystemCommunication)) != NULL)
        {
            // InterPorcessCommunication is used in the creator
            INT32 length = ipc->GetRxLength(), code = ipc->GetPulseCode(), value = ipc->GetPulseValue();
            if((ioc = dynamic_cast<IOpenChannelIpc *>(m_ISystemCommunication)) == NULL)
            {
                // InterPorcessCommunication is used in the creator
                if(m_ISystemCommunication)
                {
                    if(IsDebugOn()) printf("Deleting non-OpenChannel object\n");
                    delete m_ISystemCommunication;
                    m_ISystemCommunication = NULL;
                }
                ioc = new IOpenChannelIpc();
            }
            ioc->Initialize(ipc->GetId(), ipc->GetName(), creator.IsDebugOn(), ipc->GetReplyId(), code, value, length, ipc->GetTimeout());
            m_ISystemCommunication = static_cast<ISystemCommunication *>(ioc);
        }
        else if((tcp = dynamic_cast<ITcpIp *>(creator.m_ISystemCommunication)) != NULL)
        {
            // TCP/IP was used in the creator
            if((newTcp = dynamic_cast<ITcpIp *>(m_ISystemCommunication)) == NULL)
            {
                if(m_ISystemCommunication)
                {
                    if(IsDebugOn()) printf("Deleting non-TcpIp object\n");
                    delete m_ISystemCommunication;
                    m_ISystemCommunication = NULL;
                }
                newTcp = new ITcpIp();
            }
            newTcp->Initialize("Worker", tcp->GetPort(), tcp->GetIPAddress(), id, creator.IsDebugOn(),
                               tcp->GetFrameStart(), tcp->GetFrameEnd(), tcp->GetBacklog());
            m_ISystemCommunication = static_cast<ISystemCommunication *>(newTcp);
        }
        else throw BepException("Type of object (%s) cannot be created\n", typeid(creator.m_ISystemCommunication).name());
    }
    else throw BepException("Tried to initialize with itself, initialize with a server component");
}
void IBepCommunication::Initialize(const std::string &name, const std::string &type,
                                   const bool debug /* = false */,
                                   const INT32 size /* = DEFAULT_BUFFER_SIZE */,
                                   const INT32 timeout /* = DEFAULT_TIMEOUT */)
{
    SetDebug(debug);    // Set debug status
    SetCommunicationType(std::string("IPC"));
    if(IsDebugOn()) printf("Create an IPC client: %s, debug: %d, from thread %d\n", name.c_str(), debug, pthread_self());
    IInterProcessCommunication *ipc = NULL;
    if((ipc = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) == NULL)
    {
        if(m_ISystemCommunication != NULL)
        {   // Delete old communication object
            delete m_ISystemCommunication;
            m_ISystemCommunication = NULL;
        }   // Create new IPC client object
        m_ISystemCommunication = new IInterProcessCommunication();
        SetRxBufferSize(size);  // update the size of the buffer
        SetTimeout(timeout);    // update the timeout time
    }
    if((ipc = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != NULL)
    {
        ipc->Initialize(name, type, debug, size, timeout);  // Initialize object
    }
    else throw BepException("Error in IBepCommunication::Initialize(string, int, int, bool)\n");
}
INT32 IBepCommunication::Clear()
{
    printf("IBepCommunication::Clear - Enter\n");
    
    IInterProcessCommunication *comm;   // Pointer to communication object

    // Lock the node map while using it
    m_sendData.Lock();

    // Manually send the clear message
    m_sendData.clear(true);

    // Unlock the node map when done
    m_sendData.Unlock();


    if((comm = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != 0)
    {
        printf("ClearPublicBuffer - cleared\n");
        comm->ClearPublicBuffer();
        //comm->Write("");
        //m_ISystemCommunication->Write("");
    }
    else
    {
        printf("ClearPublicBuffer - Did not clear\n");
    }

    
    printf("More powerful clearing option\n");

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
              XmlNode *doc = const_cast<XmlNode *>(m_parser.getDocElement());
              if(doc->getChildren().size() > 0)
              { // If there are children,
                // clear them
                  printf("A. There are %d children\n", (int)doc->getChildren().size());
                  doc->clear();
                  printf("  B. There are %d children\n", (int)doc->getChildren().size());
              }
              else
              {
                  printf("The node has no children\n");
              }

        }
        catch (BepException &BepErr)
        {
            // dont do anything
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        printf( "\tError locking mutex in IBepCommunication::GetByTag(): %s\n", strerror( errno));
    }

    printf("IBepCommunication::Clear - Exit\n");
    
    return 1;

}
INT32 IBepCommunication::Read(const std::string &tag, std::string &response,
                              const bool IsFinal /* = false */)
{
    // Read Stuff
    XmlNode temp(tag,"");
    return(DoOperation(BEP_READ, &temp, response, IsFinal));
}
INT32 IBepCommunication::Read(const XmlNode *node, std::string &response, const bool IsFinal /*= false*/)
{
    // Read Stuff
    return(DoOperation(BEP_READ, node, response, IsFinal));
}
INT32 IBepCommunication::Write(const std::string &tag, const std::string &value,
                               std::string &response, const bool IsFinal /* = false */)
{
    // Write stuff
    XmlNode temp(tag,value);
    return(DoOperation(BEP_WRITE, &temp, response, IsFinal));
}
INT32 IBepCommunication::Write(const XmlNode *node, std::string &response, const bool IsFinal /* = false */)
{
    // Write stuff
    return(DoOperation(BEP_WRITE, node, response, IsFinal));
}
INT32 IBepCommunication::Command(const std::string &tag, const std::string &value,
                                 std::string &response, const bool IsFinal /* = false */)
{
    // Command stuff
    XmlNode temp(tag,value);
    return(DoOperation(BEP_COMMAND, &temp, response, IsFinal));
}
INT32 IBepCommunication::Command(const XmlNode *node, std::string &response, const bool IsFinal /* = false */)
{
    // Command stuff
    return(DoOperation(BEP_COMMAND, node, response, IsFinal));
}
INT32 IBepCommunication::Subscribe(const std::string &tag, const std::string &value,
                                   std::string &response, const bool IsFinal /* = false */)
{
    // Subscribe for stuff
    XmlNode temp(tag,value);
    return(DoOperation(BEP_SUBSCRIBE, &temp, response, IsFinal));
}
INT32 IBepCommunication::Subscribe(const XmlNode *node, std::string &response, const bool IsFinal /*= false*/)
{
    // Subscribe for stuff
    return(DoOperation(BEP_SUBSCRIBE, node, response, IsFinal));
}
INT32 IBepCommunication::Unsubscribe(const std::string &tag, const std::string &value,
                                     std::string &response, const bool IsFinal /* = false */)
{
    // Unsubscribe to stuff you subscribed to
    XmlNode temp(tag,value);
    return(DoOperation(BEP_UNSUBSCRIBE, &temp, response, IsFinal));
}
INT32 IBepCommunication::Unsubscribe(const XmlNode *node, std::string &response, const bool IsFinal /*= false*/)
{
    // Unsubscribe to stuff you subscribe to
    return(DoOperation(BEP_UNSUBSCRIBE, node, response, IsFinal));
}
INT32 IBepCommunication::Publish(const std::string &tag, const std::string &value,
                                 std::string &response, const bool IsFinal /* = false */)
{
    // Publish stuff to subscribers
    XmlNode temp(tag,value);
    return(DoOperation(BEP_PUBLISH, &temp, response, IsFinal));
}
INT32 IBepCommunication::Publish(const XmlNode *node, std::string &response, const bool IsFinal /*= false*/)
{
    // Publish stuff to subscribers
    return(DoOperation(BEP_PUBLISH, node, response, IsFinal));
}

INT32 IBepCommunication::Register(const std::string &server, const std::string &status,
                                  std::string &response, const bool IsFinal /*= false*/)
{
    // Register processes
    XmlNode node(server, status);
    return(DoOperation(BEP_REGISTER, &node, response, IsFinal));
}
INT32 IBepCommunication::Register(const XmlNode *node, std::string &response, const bool IsFinal /*= false*/)
{
    // Register processes
    return(DoOperation(BEP_REGISTER, node, response, IsFinal));
}
INT32 IBepCommunication::Terminate(std::string &response)
{
    INT32 stat = BEP_STATUS_ERROR;     // Set staus variable to error
    IInterProcessCommunication *comm;  // Pointer to communication object

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if((comm = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != 0)
            {
                // If an IPC object, send a pulse else send message via TCP/IP
                stat = comm->SendPulse(SYSTEM_PULSE, TERMINATE_PULSE);
                if(stat == BEP_STATUS_SUCCESS)  response = "Pulse sent";
                else                            response = "Pulse send failed";
            }
            else  stat = Command(BEP_TERMINATE, BEP_TERMINATE, response, true);
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in Terminate");
        stat = BEP_STATUS_ERROR;
    }

    return(stat);
}
INT32 IBepCommunication::Reset(std::string &response)
{
    INT32 stat = BEP_STATUS_ERROR;      // Set staus variable to error
    IInterProcessCommunication *comm;   // Pointer to communication object
    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if((comm = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != 0)
            {
                // If an IPC object, send a pulse else send message via TCP/IP
                stat = comm->SendPulse(SYSTEM_PULSE, RESET_PULSE);
                if(stat == BEP_STATUS_SUCCESS)  response = "Pulse sent";
                else                            response = "Pulse send failed";
            }
            else stat = Command(BEP_RESET_RESPONSE, BEP_RESET_RESPONSE, response, true);
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in Reset");
        stat = BEP_STATUS_ERROR;
    }

    return(stat);
}
INT32 IBepCommunication::Abort(std::string &response)
{
    INT32 stat = BEP_STATUS_ERROR;      // Set staus variable to error
    IInterProcessCommunication *comm;   // Pointer to communication object

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if((comm = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != 0)
            {
                // If an IPC object, send a pulse else send message via TCP/IP
                stat = comm->SendPulse(SYSTEM_PULSE, ABORT_PULSE);
                if(stat == BEP_STATUS_SUCCESS)  response = "Pulse sent";
                else                            response = "Pulse send failed";
            }
            else stat = Command(BEP_ABORT_RESPONSE, BEP_ABORT_RESPONSE, response, true);
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in Abort");
        stat = BEP_STATUS_ERROR;
    }

    return(stat);
}
INT32 IBepCommunication::Activate(std::string &response)
{
    INT32 stat = BEP_STATUS_ERROR;      // Set staus variable to error
    IInterProcessCommunication *comm;   // Pointer to communication object

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if((comm = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != 0)
            {
                // If an IPC object, send a pulse else send message via TCP/IP
                stat = comm->SendPulse(SYSTEM_PULSE, ACTIVATE_PULSE);
                if(stat == BEP_STATUS_SUCCESS)  response = "Pulse sent";
                else                            response = "Pulse send failed";
            }
            else stat = Command(BEP_ACTIVATE, BEP_ACTIVATE, response, true);
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in Activate");
        stat = BEP_STATUS_ERROR;
    }

    return(stat);
}
INT32 IBepCommunication::Deactivate(std::string &response)
{
    INT32 stat = BEP_STATUS_ERROR;      // Set staus variable to error
    IInterProcessCommunication *comm;   // Pointer to communication object

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if((comm = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != 0)
            {
                // If an IPC object, send a pulse else send message via TCP/IP
                stat = comm->SendPulse(SYSTEM_PULSE, DEACTIVATE_PULSE);
                if(stat == BEP_STATUS_SUCCESS)  response = "Pulse sent";
                else                            response = "Pulse send failed";
            }
            else stat = Command(BEP_DEACTIVATE_RESPONSE, BEP_DEACTIVATE_RESPONSE, response, true);
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in Deactivate");
        stat = BEP_STATUS_ERROR;
    }

    return(stat);
}
INT32 IBepCommunication::ReloadConfiguration(std::string &response)
{
    INT32 stat = BEP_STATUS_ERROR;      // Set staus variable to error
    IInterProcessCommunication *comm;   // Pointer to communication object
    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if((comm = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != 0)
            {
                // If an IPC object, send a pulse else send message via TCP/IP
                stat = comm->SendPulse(SYSTEM_PULSE, RELOAD_PULSE);
                if(stat == BEP_STATUS_SUCCESS)  response = "Pulse sent";
                else                            response = "Pulse send failed";
            }
            else stat = Command(BEP_RELOAD, BEP_RELOAD, response, true);
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in ReloadConfiguration");
        stat = BEP_STATUS_ERROR;
    }


    return(stat);
}
const INT32 IBepCommunication::WaitForRequest(void)
{
    INT32 id = -1;
    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if(m_ISystemCommunication)  id = m_ISystemCommunication->ProcessRequest();
            else                        throw BepException("The communication channel has not been properly set up in WaitForRequest()");
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in WaitForRequest");
    }

    return(id);
}

const INT32 IBepCommunication::WaitForRequestWithTimeout(uint64_t timeout)
{
    INT32 id = -1;
    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if(m_ISystemCommunication)
            {
                sigevent event;                         // Event structure informing what to do if timed out
                event.sigev_notify = SIGEV_UNBLOCK;     // Unblock if time out
                int errorCount = 0;                     // timer setup error count
                int timerStatus = 0;                    // timer setup status
                do
                {
                    timerStatus = TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE , &event, &timeout, NULL);
                }while((timerStatus == -1) && (errorCount++ < 5));
                if(timerStatus == -1)
                    throw BepException("Unable To Set Up Timer In WaitForRequestWithTimeout: %s\n", strerror(errno));
                // process the request
                id = m_ISystemCommunication->ProcessRequestSpecial();
            }
            else
                throw BepException("The communication channel has not been properly set up in WaitForRequest()");
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in WaitForRequest");
    }

    return(id);
}
const INT32 IBepCommunication::HandleRequest(std::string &message, BepComponent *server /*= NULL*/, const bool &messagePopulated /*= false*/)
{
    INT32 stat = BEP_STATUS_SUCCESS;
    IInterProcessCommunication *ipc = NULL;

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            ipc = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication);
            if(!messagePopulated) GetRequest(message); // Get request from client
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }

        // Process the request
        if((ipc != 0) && (ipc->GetReplyId() == 0))
        {
            // We have an IPC object and a pulse was sent
            if(IsDebugOn()) printf("We got a pulse with code: %d, value: %d\n", ipc->GetPulseCode(), ipc->GetPulseValue());
            if(server)
            {
                try
                {
                    // if dealing with a server
                    stat = server->HandlePulse(ipc->GetPulseCode(), ipc->GetPulseValue());
                    if(stat == BEP_STATUS_SUCCESS)
                    {
                        /*
                            Who's the jack ass that did this? Whoever it is should be fired...oh wait a minute...they were

                                            if(ipc->GetPulseCode() == TERMINATE_PULSE) // if a terminate pulse was received
                        */
                        // if a terminate pulse was received
                        if( (ipc->GetPulseCode() == SYSTEM_PULSE) && (ipc->GetPulseValue() == TERMINATE_PULSE))
                        {
                            SetTerminate(true);                     // set the terminate status
                            server->SetStatus(BEP_TERMINATE_STATUS);
                        }
                    }
                    else if(ipc->GetPulseCode() == _PULSE_CODE_DISCONNECT)
                    {
                        ipc->SetDisconnected(ipc->GetPulseCode() == _PULSE_CODE_DISCONNECT);
                    }
                    else stat = BEP_STATUS_ERROR;
                }
                catch( ...)
                {
                    pthread_mutex_unlock( &m_lock);
                    throw;
                }
            }
            else if(ipc->GetPulseCode() == _PULSE_CODE_DISCONNECT)
            {
                ipc->SetDisconnected(ipc->GetPulseCode() == _PULSE_CODE_DISCONNECT);
            }
            else stat = BEP_STATUS_ERROR;
        }
        else
        {
            if(IsDebugOn()) printf("HandleRequest(%s, %p)\n", message.c_str(), server);
            m_parser.Parse(message.c_str(), message.length(), true, "");    // parse request
            XmlNode* currentNode, *doc = const_cast<XmlNode *>(m_parser.getDocElement());   // Get document
            if(ValidateMessage(doc))
            {
                const std::string type = GetRequestType(doc);   // What do you want?
                const XmlNodeMap    &nodes = doc->getChildren();
                XmlNodeMapCItr      xmlItr;
                if( (errno=nodes.Lock()) == EOK)
                {
                    for(xmlItr=nodes.begin(); xmlItr!=nodes.end(); xmlItr++)
                    {
                        // Get request
                        currentNode = const_cast<XmlNode *>(xmlItr->second);
                        if(IsDebugOn()) printf("processing: %s, type %s, server %p\n",
                                               currentNode->ToString().c_str(), type.c_str(), server);
                        if(server)
                        {
                            // If there is a server to process request call the appropriate handler in the server
                            try
                            {
                                SpecialProcessing(currentNode, type, server);
                                if(type == BEP_READ)
                                {
                                    currentNode->setValue(server->Read(currentNode, GetDataRate()));    // Set the value in the message
                                }
                                else if(type == BEP_WRITE)
                                {
                                    currentNode->setValue(server->Write(currentNode));    // Set the value in the message
                                }
                                else if(type == BEP_COMMAND)
                                {
                                    if(IsTerminateMessage(currentNode))
                                    {
                                        if(IsDebugOn()) printf("We got a terminate\n");
                                        SetTerminate(true);
                                        server->Terminate();
                                        currentNode->setValue(BEP_SUCCESS_RESPONSE);    // Set the value in the message
                                    }
                                    else if(IsAbortMessage(currentNode))
                                    {
                                        server->Abort();
                                        currentNode->setValue(BEP_SUCCESS_RESPONSE);    // Set the value in the message
                                    }
                                    else if(IsResetMessage(currentNode))
                                    {
                                        server->Reset();
                                        currentNode->setValue(BEP_SUCCESS_RESPONSE);    // Set the value in the message
                                    }
                                    else
                                    {
                                        currentNode->setValue(server->Command(currentNode));     // Set the value in the message
                                    }
                                }
                                else if(type == BEP_SUBSCRIBE)
                                {
                                    currentNode->setValue(server->Subscribe(currentNode));    // Set the value in the message
                                }
                                else if(type == BEP_UNSUBSCRIBE)
                                {
                                    currentNode->setValue(server->Unsubscribe(currentNode));    // Set the value in the message
                                }
                                else if(type == BEP_PUBLISH)
                                {
                                    currentNode->setValue(server->Publish(currentNode));    // Set the value in the message
                                }
                                else if(type == BEP_REGISTER)
                                {
                                    currentNode->setValue(server->Register());    // Set the value in the message
                                }
                                else
                                {
                                    if(IsDebugOn()) printf("Unknown request type: %s\n", type.c_str());
                                    currentNode->setValue(BEP_ERROR_RESPONSE);    // Set the value in the message
                                    break;
                                }
                            }
                            catch(BepException& Err)
                            {
                                if(IsDebugOn()) printf("Error: %s\n", Err.what());
                                currentNode->setValue(XML_T(BEP_SOFTWAREFAIL_RESPONSE));
                                stat = BEP_STATUS_ERROR;
                            }
                            catch(...)
                            {
                                if(IsDebugOn()) printf("Unknown error\n");
                                currentNode->setValue(XML_T(BEP_SOFTWAREFAIL_RESPONSE));
                                stat = BEP_STATUS_ERROR;
                            }
                        }
                        else
                        {
                            // No server was provided
                            if(IsDebugOn()) printf("No server\n");
                            currentNode->setValue(XML_T(BEP_UNAVAILABLE_RESPONSE));
                            stat = BEP_STATUS_ERROR;
                        }
                        if(IsDebugOn()) printf("Node: %s\n", currentNode->ToString().c_str());
                    }
                    nodes.Unlock();
                }
                else
                {
                    printf( "\tError locking node map in IBepCommunication::HandleRequest(): %s\n", strerror( errno));
                    stat = BEP_STATUS_ERROR;
                }

                message.erase();            // Clear message  (do not condition with debug some servers depend on this)
                doc->writeNode(message);    // Return message (do not condition with debug some servers depend on this)
                if(IsDebugOn()) printf("HandleRequest() will send: %s\n", message.c_str());
            }
            else
            {
                // Bad Data
                if(IsDebugOn()) printf("Message is not valid:\n%s\n", message.c_str());
                stat = BEP_STATUS_ERROR;
                BuildMessage(BEP_ERROR_RESPONSE, message);
            }
            SendResponse(doc, server);  // Send response to client
            if(IsDebugOn()) printf("Sent:\n%s\n", message.c_str());
        }

        pthread_mutex_unlock( &m_lock);

    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in HandleRequest");
        stat = BEP_STATUS_ERROR;
    }

    if(IsDebugOn()) printf("HandleRequest() done\n");
    return(stat);
}
const INT32 IBepCommunication::GetNext(std::string &tag, std::string &value, std::string &message)
{
    if(IsDebugOn()) printf("IBepCommunication::GetNext(tag, value)\n");
    XmlNode node(XML_T(""), XML_T(""));                     // Temporary node
    INT32 num = IBepCommunication::GetNext(node, message);   // Get the data

    if(IsDebugOn()) printf("node = %s\n", node.ToString().c_str());
    tag = node.getName();                                   // Set name
    value = node.getValue();                                    // Set value
    if(IsDebugOn()) printf("IBepCommunication::GetNext(%s, %s, %s) [%d] done\n", tag.c_str(), value.c_str(), message.c_str(), num);
    return(num);                                            // How many left
}
const INT32 IBepCommunication::GetNext(XmlNode &node, std::string &message)
{
    INT32 num = -1; // the number of children left
    if(IsDebugOn()) printf("IBepCommunication::GetNext(node)\n");

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            XmlNode* doc = const_cast<XmlNode *>(m_parser.getDocElement());
            if(IsDebugOn())
            {
                message.erase();
                doc->writeNode(message);
                if(IsDebugOn()) printf("message = %s\n", message.c_str());
            }

            num = doc->getChildren().size() - 1;
            if(num >= 0)
            {
                // Message has some data
                node = *doc->getChildren().getNode(0);  // get the child from the list
                if(IsDebugOn()) printf("node = %s\n", node.ToString().c_str());
                if(node.getName() == BEP_ERROR_RESPONSE)
                {
                    // Message is error message
                    if(IsDebugOn()) printf("Processing Error Node\n");
                    try
                    {
                        // Get error reason from root node
                        SetMessageStatus(doc->getAttributes().getNode(BEP_REASON)->getValue()); // Set error
                        if(IsDebugOn()) printf("Resetting doc from %s\n", doc->ToString().c_str());
                        doc = const_cast<XmlNode *>(doc->getChild(BEP_ERROR_RESPONSE)->getChildren().getNode(0));   // Move pointer to error node doc
                        if(IsDebugOn()) printf("New doc = %s\n", doc->ToString().c_str());
                        num = doc->getChildren().size() - 1;    // Reset to the number of error children
                        if(doc->getChildren().size() > 0)
                        {
                            node = *doc->getChildren().getNode(0);
                            node.setValue(BEP_ERROR_RESPONSE);
                        }
                    }
                    catch(BepException& BepErr)
                    {
                        // Set error (could not find message type attrinbute
                        SetMessageStatus(BEP_NO_MSG_TYPE_ATTRIBUTE);
                    }
                }
                else SetMessageStatus(BEP_SUCCESS_RESPONSE);            // Clear error
                doc->delChild(doc->getChildren().getNode(0));   // Delete it from the message
                if(IsDebugOn()) printf("IBepCommunication::GetNext(%s) %d items left\n", node.ToString().c_str(), num);
            }
            else
            {
                // No data in message
                SetMessageStatus(BEP_NO_DATA);
                node.setName(BEP_UNAVAILABLE_RESPONSE);
                node.setValue(BEP_UNAVAILABLE_RESPONSE);
            }
        }
        catch(BepException &BepErr)
        {
            num = -1;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        printf( "\tError locking mutex in IBepCommunication::GetNext(): %s\n", strerror( errno));
    }
    return(num);    // How many left
}
const INT32 IBepCommunication::GetByTag(const std::string tag, std::string &value, std::string &message)
{
    INT32 stat = BEP_STATUS_ERROR;
    value = BEP_UNAVAILABLE_RESPONSE;
    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            XmlNode *doc = const_cast<XmlNode *>(m_parser.getDocElement());
            if(IsDebugOn())
            {
                message.erase();
                doc->writeNode(message);
            }
            if(doc->getChildren().size() > 0)
            {
                // If there are children
                value = doc->getChildren().getNode(tag)->getValue();
                stat = BEP_STATUS_SUCCESS;
                if(IsDebugOn()) printf("GetByTag(%s, %s)\n", tag.c_str(), value.c_str());
            }
            else stat = BEP_STATUS_FAILURE;
        }
        catch(BepException &BepErr)
        {
            // Don't do anything
        }

        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        printf( "\tError locking mutex in IBepCommunication::GetByTag(): %s\n", strerror( errno));
    }
    return(stat);                      // How many childrens
}

const std::string IBepCommunication::GetMessageStatus(void)
{
    return(m_msgStat);
}

const bool IBepCommunication::WasTermiateReceived(void)
{
    return(m_terminate);
}

const std::string IBepCommunication::GetName(void)
{
    std::string name;   // Name to return
    ITcpIp *tcp;
    IInterProcessCommunication *ipc;
    if((ipc = dynamic_cast<IInterProcessCommunication *>(m_ISystemCommunication)) != 0)
    {
        // If we have an ipc object inside
        name = ipc->GetName();
    }
    else if((tcp = dynamic_cast<ITcpIp *>(m_ISystemCommunication)) != 0)
    {
        char buffer[128];       // buffer to create messages with
        name = CreateMessage(buffer, sizeof(buffer), "%d", tcp->GetSock());
    }
    return(name);
}
const INT16 IBepCommunication::GetPort(void)
{
    INT16 port = 0;
    ITcpIp *tcp;
    if((tcp = dynamic_cast<ITcpIp *>(m_ISystemCommunication)) != 0)
    {
        // If we have an ipc object inside
        port = tcp->GetPort();
    }
    return(port);
}
const std::string IBepCommunication::GetIPAddress(void)
{
    std::string address;
    ITcpIp *tcp;
    if((tcp = dynamic_cast<ITcpIp *>(m_ISystemCommunication)) != 0)
    {
        // If we have an ipc object inside
        address = tcp->GetIPAddress();
    }
    return(address);
}
const std::string IBepCommunication::GetFrameStart(void)
{
    std::string frameStart;
    ITcpIp *tcp;
    if((tcp = dynamic_cast<ITcpIp *>(m_ISystemCommunication)) != 0)
    {
        // If we have an ipc object inside
        frameStart = tcp->GetFrameStart();
    }
    return(frameStart);
}
const std::string IBepCommunication::GetFrameEnd(void)
{
    std::string frameEnd;
    ITcpIp *tcp;
    if((tcp = dynamic_cast<ITcpIp *>(m_ISystemCommunication)) != 0)
    {
        // If we have an ipc object inside
        frameEnd = tcp->GetFrameEnd();
    }
    return(frameEnd);
}

void IBepCommunication::ResetCommunications(void)
{
    m_oper.erase();         // Clear out the operation
    m_sendData.clear(true); // Clear old data
}

const std::string IBepCommunication::GetCommunicationType(void)
{
    return(m_commType);
}

const INT32 IBepCommunication::GetDataRate(void)
{
    return(m_rate);
}

void IBepCommunication::SetDataRate(const INT32 rate)
{
    if(rate >= 0)   m_rate = rate;
    else            m_rate  = 0;
}

const UINT32 IBepCommunication::GetTimeout(void)
{
    return(m_ISystemCommunication->GetTimeout());
}

void IBepCommunication::SetTimeout(const UINT32 timeout)
{
    m_ISystemCommunication->SetTimeout(timeout);
}

const INT32 IBepCommunication::GetId(void) const
{
    return(m_ISystemCommunication->GetId());
}


bool IBepCommunication::IsDataOk(const XmlString &data)
{
    // Check data for bad response
    return((data != BEP_ERROR_RESPONSE) && (data != BEP_FAILURE_RESPONSE) &&
           (data != BEP_INVALID_RESPONSE) && (data != BEP_UNAVAILABLE_RESPONSE) &&
           (data != BEP_UNSUPPORTED_RESPONSE));
}
INT32 IBepCommunication::Error(const std::string &tag, std::string &response)
{
    // Build a node from the data
    XmlNode node(tag, "");
    return(Error(&node, response));
}
INT32 IBepCommunication::Error(const XmlNode *node, std::string &response)
{
    // Unsubscibe to stuff you subscribed to
    return(DoOperation(BEP_ERROR_RESPONSE, node, response, true));
}
void IBepCommunication::SpecialProcessing(XmlNode *node, const std::string type, BepComponent *server)
{
    if(type == BEP_READ)
    {
        try
        {
            SetDataRate(atol(node->getParent()->getAttributes().getNode(BEP_DATA_RATE)->getValue().c_str()));
        }
        catch(...)
        {
            SetDataRate(0);
        }
    }
    else if(type == BEP_PUBLISH || type == BEP_WRITE)
    {
        server->SpecialProcessing(node, type, server);
    }
}
INT32 IBepCommunication::DoOperation(const std::string operation,
                                     const XmlNode *dataNode,
                                     std::string &message,
                                     const bool IsFinal)
{
//    SetDebug("On");
    if(IsDebugOn()) printf("DoOperation(%s, %s, %s, %d)\n", operation.c_str(),
                           dataNode->getName().c_str(), dataNode->getValue().c_str(), IsFinal);
    INT32 status = BEP_STATUS_ERROR;// Status of operation

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if(m_ISystemCommunication)
            {
                if(m_oper.empty() || (operation == BEP_ERROR_RESPONSE))
                {
                    // Set new operation
                    m_oper = operation;
                }
                if(m_oper == operation)
                {
                    // Lock the node map while using it
                    m_sendData.Lock();

                    // Add message to list
                    m_sendData.addNode(dataNode->Copy());
                    if(IsFinal)
                    {
                        // Build message to be sent
                        BuildMessage(operation, message);
                        // Clear old data since BuildMessage() is done with it
                        m_sendData.clear(true);
                        // Unlock the node map when done
                        m_sendData.Unlock();

                        if(IsDebugOn()) printf("The data that BuildMessage() returned is:\n%s\n", message.c_str());
                        try
                        {
                            m_ISystemCommunication->Write(message);             // Write message
                            status = m_ISystemCommunication->Read(message);     // Get response

                            if(IsDebugOn()) printf("DoOperation() read returned %d\n", status);
                            if(status != (INT32)BEP_STATUS_ERROR)
                            {
                                if(IsDebugOn()) printf("DoOperation() response was:\n%s\n", message.c_str());
                                m_parser.Parse(message.c_str(), message.length(), true, "");
                                if(ValidateMessage(m_parser.getDocElement()))
                                {
                                    // Get number of children
                                    UINT32              numTags = m_parser.getDocElement()->getChildren().size(), okTags = 0;
                                    const XmlNodeMap    &childNodes = m_parser.getDocElement()->getChildren();
                                    XmlNodeMapCItr      itr;

                                    childNodes.Lock();
                                    for( itr=childNodes.begin(); (itr!=childNodes.end()) && (status != BEP_STATUS_FAILURE); itr++)
                                    {
                                        const XmlNode *child = itr->second;
                                        // Check if the kids are alright
                                        if(!IsDataOk(child->getValue()))
                                        {
                                            // Bad things man...
                                            if(IsDebugOn()) printf("\nData item is NOT ok: %s\n", child->getValue().c_str());
                                            status = BEP_STATUS_FAILURE;
                                        }
                                        else
                                        {
                                            // This kid is OK
                                            okTags++;
                                        }
                                    }
                                    childNodes.Unlock();
                                    if(okTags ==  numTags)  status = BEP_STATUS_SUCCESS;
                                    else                    status = BEP_STATUS_FAILURE;
                                }
                                else
                                {
                                    // All tags not read properly
                                    status  = BEP_STATUS_ERROR;
                                }
                            }
                        }
                        catch(BepException &BepErr)
                        {
                            // read or write operation failed
                            if(IsDebugOn()) printf("Error: %s\n", BepErr.what());
                        }

                        // Clear operation
                        m_oper.clear();
                    }
                    else
                    {
                        // Unlock the node map when done
                        m_sendData.Unlock();
                        status  = BEP_STATUS_SUCCESS;   // Tag added to list (waiting for last tag)
                    }
                }
                else throw BepException("Cannot perform %s before completing %s\n", operation.c_str(), m_oper.c_str());
            }
            else throw BepException("The communication channel has not been properly set up in DoOperation()");
        }
        catch( BepException &err)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else throw BepException("Cannot lock in DoOperation(): %s", strerror( errno));

    if(IsDebugOn()) printf("DoOperation(%s, %s, %s, %s, %d) done\n",
                           operation.c_str(), dataNode->getName().c_str(), dataNode->getValue().c_str(), message.c_str(), IsFinal);
    return(status);
}
const INT32 IBepCommunication::GetRequest(std::string &message)
{
    INT32   stat;

    if(m_ISystemCommunication)
    {
        if(((dynamic_cast<IOpenChannelIpc *>(m_ISystemCommunication)) == 0) &&
           ((dynamic_cast<ITcpIp *>(m_ISystemCommunication)) == 0))
            throw BepException("Failure! Request Must Be Handled By Workers, Not Servers!");
    }
    else throw BepException("The communication channel has not been properly set up in GetRequest()");
    message.erase();

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            stat = m_ISystemCommunication->Read(message);
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in GetRequest");
        stat = BEP_STATUS_ERROR;
    }

    return( stat);
}
bool IBepCommunication::ValidateMessage(const XmlNode* doc)
{
    bool status = false;    // Operation status
    SetMessageStatus(BEP_SUCCESS_RESPONSE); // Set status to OK
    try
    {
        // Check message
        if(IsDebugOn()) printf("ValidateMessage(%s)\ndocument:\n%s\n", doc->ToString().c_str(), doc->ToString().c_str());
        if(doc->getName() == BEP_XML_ROOT)
        {
            if(doc->getChildren().size() != 0)
            {
                if(doc->getChildren().getNode(0)->getName() == BEP_ERROR_RESPONSE)
                {
                    status = true;
                    if(IsDebugOn()) printf("Error document validated\n");
                }
                else
                {
                    try
                    {
                        if(CheckAttribute(doc->getAttributes().getNode(BEP_XML_MSG_TYPE)))
                        {
                            // If attribute is valid
                            if(IsDebugOn()) printf("Attribute verified\n");
                            status = true;  // Message is Ok if no exception was raised
                            if(IsDebugOn()) printf("Message is valid\n");
                        }
                        else
                        {
                            // Attribute is present but not a valid value
                            SetMessageStatus(BEP_ATTRIBUTE_VALUE_WRONG);
                            if(IsDebugOn()) printf(BEP_XML_MSG_TYPE" attribute not valid\n");
                        }
                    }
                    catch(XmlException &XmlErr)
                    {
                        // Attribute does not exist
                        SetMessageStatus(BEP_NO_MSG_TYPE_ATTRIBUTE);
                        if(IsDebugOn()) printf("XmlException '%s' in ValidateMessage()\n", XmlErr.what());
                    }
                }
            }
            else
            {
                if(doc->getAttributes().size())
                {
                    try
                    {
                        // If BEP_XML_MSG_TYPE attibute exists, there are no children
                        doc->getAttributes().getNode(BEP_XML_MSG_TYPE);
                        SetMessageStatus(BEP_NO_CHILDREN);
                        if(IsDebugOn()) printf("Message has no children = %s\n", doc->getName().c_str());
                    }
                    catch(BepException& BepErr)
                    {
                        // If BEP_XML_MSG_TYPE attibute does not exist, the attibute name is wrong
                        SetMessageStatus(BEP_ATTRIBUTE_VALUE_WRONG);
                        if(IsDebugOn()) printf(BEP_XML_MSG_TYPE" attribute not valid\n");
                    }
                }
                else
                {
                    // If no attibutes exists, message is empty
                    SetMessageStatus(BEP_EMPTY_MESSAGE);
                    if(IsDebugOn()) printf("Message is empty = %s\n", doc->getName().c_str());
                }
            }
        }
        else
        {
            SetMessageStatus(BEP_ROOT_NAME_WRONG);
            if(IsDebugOn()) printf("root node name = %s, s/b %s\n", doc->getName().c_str(), BEP_XML_ROOT);
        }
    }
    catch(XmlException &XmlErr)
    {
        SetMessageStatus(BEP_ROOT_NAME_WRONG);
        if(IsDebugOn()) printf("XmlException '%s' in ValidateMessage()\n", XmlErr.what());
    }
    return(status);
}
void IBepCommunication::BuildMessage(const std::string function, std::string& message)
{
    if(IsDebugOn()) printf("IBepCommunication::BuildMessage(%s)\n", function.c_str());
    XmlNode *error, *node = NULL, *doc = NULL;

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            try
            {
                // Get document
                doc = const_cast<XmlNode *>(m_parser.getDocElement());
            }
            catch(BepException &BepErr)
            {
                if(IsDebugOn()) printf("IBepCommunication::BuildMessage()...invalid document\n");

                // Document was not parsable, create new one
                m_parser.Parse("<"BEP_XML_ROOT"/>", strlen("<"BEP_XML_ROOT"/>"), true, "");
                doc = const_cast<XmlNode *>(m_parser.getDocElement());
            }
            if(function == BEP_ERROR_RESPONSE)
            {
                // Build an error message
                node = doc->Copy();                                     // Make a copy in case of error
                doc->clear();                                           // Clear all children
                doc->setName(BEP_XML_ROOT);                             // Make sure root node name is good
                AddAttribute(BEP_XML_MSG_TYPE, function, doc);          // Add message type
                AddAttribute(BEP_REASON, m_msgStat, doc);               // Add message status

                // Add a child node so we can add the original document contents to it
                //  in order to show the user the original bad document
                error = const_cast<XmlNode *>(doc->addChild(BEP_ERROR_RESPONSE, "", ELEMENT_NODE)); // Add error node
                if(node) error->addChild(node); // Add old document that was crap
            }
            else
            {
                // Lock send data while examining it
                if( (errno=m_sendData.Lock()) == EOK)
                {
                    try
                    {
                        // Build good message
                        doc->clear();
                        AddAttribute(BEP_XML_MSG_TYPE, function, doc);  // Add message type
                        for(XmlNodeMapItr iter = m_sendData.begin(); iter != m_sendData.end(); ++iter)
                        {
                            // Build list containing all requested tags using iterator for speed
                            try
                            {
                                if(IsDebugOn()) printf("IBepCommunication::BuildMessage()...adding child %s\n", iter->first.c_str());
                                // Add to document, allocating memory for a new node
                                XmlNode *newNode = iter->second->Copy();
                                doc->addChild(newNode);
                            }
                            catch(XmlException &e)
                            {
                                printf("XmlException in BuildMessage() adding child %s -- %s\n",
                                       iter->second->ToString().c_str(), e.GetReason());
                            }
                        }
                    }
                    catch( ...)
                    {
                    }
                    m_sendData.Unlock();
                }
                else
                {
                    printf("Error locking m_sendData in IBepCommunication::BuildMessage() - %s\n", strerror( errno));
                }

                if(GetDataRate() >= 0)
                {
                    // If data rate was not 0
                    char temp[32];                          // Temporary buffer
                    memset(temp, 0, sizeof(temp));          // Clear buffer
                    itoa(GetDataRate(), temp, 10);          // Convert to string
                    AddAttribute(BEP_DATA_RATE, temp, doc); // Add data rate
                    SetDataRate(0);                         // Clear data rate
                }
            }
            message.erase();
            doc->writeNode(message);    // Write message
            if(IsDebugOn()) printf("IBepCommunication::BuildMessage(): Document:\n%s\n", message.c_str());
        }
        catch(BepException &e)
        {
            printf("BepException during IBepCommunication::BuildMessage() - %s\n", e.GetReason());
        }
        catch(...)
        {
            printf("Unknown exception during IBepCommunication::BuildMessage()\n");
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        printf( "\tERROR locking in IBepCommunication::BuildMessage: %s\n", strerror( errno));
    }
}
const bool IBepCommunication::SendResponse(XmlNode* doc, BepComponent* component)
{
    if(IsDebugOn()) printf("IBepCommunication::SendResponse(%s, %p)\n", doc->ToString().c_str(), component);
    std::string message;
    doc->writeNode(message);
    return(SendResponse(message));
}
const bool IBepCommunication::SendResponse(const std::string &message)
{
    bool stat = true;
    if(IsDebugOn()) printf("IBepCommunication::SendResponse(%s)\n", message.c_str());

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            if(m_ISystemCommunication)  m_ISystemCommunication->Write(message);
            else                        throw BepException("The communication channel has not been properly set up in SendResponse()");
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }
        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        if(IsDebugOn()) perror("Error locking mutex in SendResponse");
        stat = BEP_STATUS_ERROR;
    }

    return(stat);
}
const std::string IBepCommunication::GetRequestType(const std::string &message)
{
    std::string retVal;

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        try
        {
            m_parser.Parse(message.c_str(), message.length(), true, "");
            retVal = m_parser.getDocElement()->getAttributes().getNode(BEP_XML_MSG_TYPE)->getValue();
        }
        catch( ...)
        {
            pthread_mutex_unlock( &m_lock);
            throw;
        }

        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        printf( "\tERROR locking in IBepCommunication::GetRequestType: %s\n", strerror( errno));
    }

    return( retVal);
}
const std::string IBepCommunication::GetRequestType(const XmlNode* document)
{
    return(document->getAttributes().getNode(BEP_XML_MSG_TYPE)->getValue());
}

void IBepCommunication::Close(void) const
{
    if( NULL != m_ISystemCommunication)
    {
        m_ISystemCommunication->Close();
    }
}

INT32 IBepCommunication::GetRxBufferSize(void)
{
    return(m_rxBufferSize);
}

void IBepCommunication::SetMessageStatus(const std::string& stat)
{
    m_msgStat = stat;
}


void IBepCommunication::SetCommunicationType(const std::string &commType)
{
    m_commType = commType;
}

const XmlNode* IBepCommunication::FindNode(const std::string &tag)
{
    XmlNode *node = NULL;
    XmlNodeMapItr foundNode;

    if( (errno=pthread_mutex_lock( &m_lock)) == EOK)
    {
        if((foundNode = m_sendData.find(tag)) != m_sendData.end())
        {
            // point to the node if it was found
            node = foundNode->second;
        }
        else
        {
            // Create a new node and add it to the list
            node = new XmlNode(tag, "");
            m_sendData.addNode(node);
        }

        pthread_mutex_unlock( &m_lock);
    }
    else
    {
        printf( "\tERROR locking in IBepCommunication::FindNode: %s\n", strerror( errno));
    }

    return(node);
}

void IBepCommunication::SetTerminate(const bool terminate)
{
    m_terminate = terminate;
}

const bool IBepCommunication::IsTerminateMessage(const XmlNode* node)
{
    return((node->getName() == BEP_TERMINATE) && (node->getValue() == BEP_TERMINATE));
}

const bool IBepCommunication::IsAbortMessage(const XmlNode* node)
{
    return((node->getName() == BEP_ABORT_RESPONSE) && (node->getValue() == BEP_ABORT_RESPONSE));
}

const bool IBepCommunication::IsResetMessage(const XmlNode* node)
{
    return((node->getName() == BEP_RESET_RESPONSE) && (node->getValue() == BEP_RESET_RESPONSE));
}

bool IBepCommunication::CheckAttribute(const XmlNode *attribute)
{
    bool status = false;    // Operation status
    if(attribute->getName() == BEP_XML_MSG_TYPE)
    {
        // If attribute name is Ok
        const XmlString value = attribute->getValue();
        if((value == BEP_READ) || (value == BEP_WRITE) ||
           (value == BEP_COMMAND) || (value == BEP_SUBSCRIBE) ||
           (value == BEP_UNSUBSCRIBE) || (value == BEP_PUBLISH) ||
           (value == BEP_REGISTER))
        {
            // If attribute value is Ok
            status = true;
        }
    }
    return(status);
}


void IBepCommunication::AddAttribute(const std::string &name, const std::string &value, XmlNode* &node)
{
    XmlNodeMapCItr  itr;
    const XmlNodeMap &attrs = node->getAttributes();
    XmlNode         *attrib = NULL;

    if( (errno=attrs.Lock()) == EOK)
    {
        if(IsDebugOn()) printf("AddAttribute(%s, %s, %s)\n", name.c_str(), value.c_str(), node->ToString().c_str());

        if( (attrs.size() > 0) && (itr=attrs.find( name)) != attrs.end())
        {
            if(IsDebugOn()) printf("Updating node %s\n", node->ToString().c_str());
            attrib = itr->second;
            attrib->setValue(value);
            if(IsDebugOn()) printf("Updated node %s\n", node->ToString().c_str());
        }
        else
        {
            if(IsDebugOn()) printf("Creating node %s \n", node->ToString().c_str());
            attrib = node->addChild(name, value, ATTRIBUTE_NODE);   // Add attribute
            if(IsDebugOn()) printf("Creatied node %s \n", node->ToString().c_str());
        }
        if(IsDebugOn()) printf("AddAttribute(%s, %s, %s) done\n", name.c_str(), value.c_str(), node->ToString().c_str());

        attrs.Unlock();
    }
    else
    {
        printf( "\tERROR locking in IBepCommunication::AddAttribute: %s\n", strerror( errno));
    }
}

void IBepCommunication::SetRxBufferSize(INT32 length)
{
    m_rxBufferSize = length;
}
