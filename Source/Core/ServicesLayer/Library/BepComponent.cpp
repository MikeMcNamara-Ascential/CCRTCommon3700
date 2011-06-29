//*************************************************************************
// Description:
//   The is the base system component.  This provides the base functionality
//   for components that operate in the RTOS system.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/BepComponent.cpp $
// 
// 11    2/22/07 11:02a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added a separator between the methods.
// 
// 10    1/18/07 12:45p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added a brake for the TASK_MON_PULSE case in HandlePulse().
// 
// 9     10/26/06 9:53a Rwiersem
// Changes for the 06102701 core release:
// 
// Added Activate() and Deactivate() methods, commands, and pulses.  These
// are used to keep the test components running when a new drive curve
// starts.
// 
// 8     2/15/06 7:41p Cward
// Enable thread spinner
//
// 7     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 6     7/20/05 8:43p Cward
// Added support for new TaskMon. Turned off Notification Thread Pool to
// enable the Notification Thread Pool Spinner.
//
// 5     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 4     5/09/05 5:00p Gpattison
// Combined the GenericTC::Initialize(filename) with the BepComponent
// and added a member variable to store the parser which contains
// a copy of the config node pointer.
//
// 4     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 3     6/09/04 11:34p Cward
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 3     10/24/03 12:04p Khykin
// Updated to be able to use the NotificationThreadSpinner class.
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 2     8/15/03 2:07p Khykin
// Updated to catch BepException when loading config file.
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:54p Khykin
// Initial checkin.
//
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:21a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:39  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:25a Khykin
// Initial Checkin
//
// 1     1/18/03 7:56a Khykin
// Initial Checkin From DCTC
//
// 50    9/12/02 5:43p Tbochene
// Updated configuration due to changes in notification list
//
// 49    8/23/02 7:50a Dan.erickson
// Added Verbosity to log statements.
//
// 48    7/31/02 10:45a Skypig13
// Changes due to notification list prototype changes
//
// 47    6/19/02 1:47p Skypig13
// Updated to set status to BEP_NONE in constructor initializer list and
// save NotificationList configuration
//
// 46    5/30/02 4:04p Skypig13
// Added Start() for notification thread pool in subscribe() method
//
// 45    5/24/02 2:14p Kh
// Adding debug to notification.
//
// 44    4/25/02 5:23p Skypig13
// Removed scope qualifier
//
// 43    4/25/02 12:15a Skypig13
// Scoped methods
//
// 42    4/15/02 11:21a Skypig13
// Updated thread pool base and component to take #define types for thread
// pool handle and parameters instead of void
//
// 41    4/08/02 11:45a Skypig13
// New structure crating notification list on demand.
// NOTE:  m_nList should be NULL checked when used by derived classes
//
// 40    4/04/02 3:34a Bmeinke
// Only printout the string form of the XML config file if Debug is turned
// on
//
// 39    4/02/02 10:38p Skypig13
// Updated to create notification list on demand (after a subscription
// request was received).
//
// 38    3/25/02 6:21p Kh
// Updating for chanes to the Debugging base class.
//
// 37    3/20/02 10:40a Kh
// Adding the setting of the debug level.
//
// 36    3/19/02 2:19p Kh
// Adding the ability to change the process priority via config file.
//
// 35    3/19/02 1:04p Bmeinke
// Added newline characters to end of strings passed to ILogger::Log()
// Wrapped call to SetDebug() in Initialize() with try/catch
// Modified XML document handling inside Initialize() method
//
// 34    2/25/02 8:46p Skypig13
// Updated Initialize()
//
// 33    2/19/02 4:34p Skypig13
// Removed scope qualifiers from Read(), Write(), etc.
//
// 32    2/18/02 6:57p Skypig13
// Updated Initialize()
//
// 31    2/18/02 9:55a Skypig13
// Changed so Read() will be performed even if Notification list does not
// exist
//
// 30    2/12/02 5:06p Dan.erickson
// Created XmlNode versions of Read, Write, Command, Subscribe,
// Unsubscribe methods.
//
// 29    2/04/02 11:29p Skypig13
// Refactored to not use multiple inheritance, aggregation of ThreadPool
// is new structure
//
// 28    12/18/01 11:36a Kh
// Updating diagnostic printouts.
//
// 27    12/06/01 5:07p Kh
// Adding an Initialize method that will take a configuration file.
//
// 26    12/04/01 11:11a Kh
// Updating the ReloadConfiguration method name.
//
// 25    11/29/01 3:02p Skypig13
// Updated Subscribe to return data instead of Success
//
// 24    11/02/01 2:47p Dan.erickson
// Added SpecialProcessing method.
//
// 23    11/01/01 6:03p Dan.erickson
// Added GetSpecialInfo method to help various servers
// pass information around the system.
//
// 22    10/17/01 9:54a Skypig13
// Changed prototype of Read() to include data rate
//
// 21    10/04/01 3:04p Skypig13
// Updated to return BEP_UNSUPPORTED_RESPONSE in Read(), Write(), and
// Publish()
//
// 20    10/03/01 2:56p Skypig13
// Added NULL check in Initialize()
//
// 19    9/14/01 12:03a Skypig13
// Added namespace in front of Publish method and put catch(...) in a
// couple others to catch errant exceptions
//
// 18    9/13/01 10:08a Kh
// Adding the Publish method in the component to support
// IBepCommunication.
//
// 17    9/04/01 12:15p Skypig13
// Fixed typo in assignment of result in Subscribe() and Unsubscribe()
//
// 16    9/04/01 11:00a Skypig13
// Add return values to Subscribe() and Unsubscribe().  Changed second
// parameter name from value to Process to clarify use of parameter in
// both methods.
//
// 15    8/28/01 4:20p Skypig13
// Updating subscribe and Unsubscribe so they do something
//
// 14    8/15/01 9:20a Kh
// Updating Subscribe and Unsubscribe definitions.
//
// 13    7/25/01 2:32p Brian.meinke
// Removed Log() calls inside initialize if Debug node not found in Setup
// block
//
// 12    7/19/01 10:19a Kh
// Updating my spelling mistake which was so humbly pointed out by Tim.
//
// 11    7/19/01 10:12a Kh
// Updating the ReloadConfiguration method.
//
// 10    7/05/01 1:51p Kh
// Updating the Command method.
//
// 9     6/27/01 8:36a Kh
// Changing the access of m_debug to use methods.
//
// 8     6/13/01 7:05p Skypig13
// Fixed scope qualifiers on added functions
//
// 7     6/13/01 5:42p Skypig13
// Added Read(), Write(), Command(), Subscribe(), and Unsubscribe()
//
// 6     6/12/00 8:37a Kh
// Update debug methods for common name usage.
//
// 5     6/01/00 3:52p Kh
// Adding debug method and variables.
//
// 4     5/25/00 3:09p Kh
// Updating the component for changes with NotificationList.
//
// 3     5/23/00 3:23p Kh
// Changing the notification list functionality.
//
// 2     4/26/01 10:36a Kh
// Finished Testing the component with the new tester.
//
// 1     2/22/01 5:36p Tim.bochenek
// Unit testing of Logger, Parser, and CmdLine Processor
//
// 1     12/07/00 1:00p Kevin.Hykin
// Initial Revision
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#define NOTIFICATION_THREADPOOL 0

// NOTE:: DO NOT CHANGE THIS INCLUDE ORDER !!!!!!!!
//        IBepCommunication.h must be included after NotificationThreadPool.h
//        This allows ThreadPoolBase to compile properly
#include "BepComponent.h"
#include "NotificationThreadPool.h"
#include "NotificationThreadSpinner.h"
#include "IBepCommunication.h"

//=============================================================================
BepComponent::BepComponent(void) : ILogger(), Status(BEP_NONE), m_nList(NULL), m_nListConfig(NULL),
    m_taskMon( NULL), m_taskMonTaskName(""), m_priority( -1), m_cfgModTime( 0)
{
}

//=============================================================================
BepComponent::~BepComponent(void)
{
    if(m_nList)
    {
        delete m_nList;
        m_nList = NULL;
    }
    if(m_nListConfig)
    {
        delete m_nListConfig;
        m_nListConfig = NULL;
    }

    ReportTaskDown();
}

//=============================================================================
void BepComponent::Initialize(const std::string &fileName)
{
    // printf() used to print errors because Logger is not initialized
    //   until after base class initialization complete
    try
    {
        // Get the file status for our config file
        m_cfgModTime = GetFileModificationTime( fileName);

        SetConfigFile(fileName);    // Set name of configuration file
        const XmlNode *document = m_parser.ReturnXMLDocument(fileName); // Parse configuration file
        if (document)
        {
            Initialize(document);       // initialize server
        }
        else printf("BepComponent::Initialize() Failed to parse configuration file\n");
    }
    catch (BepException &BepErr)
    {
        printf("BepComponent::Initialize(%s) failed: %s\n", fileName.c_str(), BepErr.what());
        throw;
    }
    catch (...)
    {
        printf("BepComponent::Initialize(%s) failed: for an unknown reason\n", fileName.c_str());
        throw;
    }
}

//=============================================================================
void BepComponent::Initialize(const XmlNode *document)
{
    try
    {
        if(document)
        {
            ILogger::Initialize(document);   // initialize the base class

            Log(LOG_FN_ENTRY, "BepComponent::Initialize()\n");

            try
            {
                if (m_nListConfig != NULL)
                {
                    delete m_nListConfig;
                    m_nListConfig = NULL;
                }
                m_nListConfig = document->Copy();
            }
            catch(BepException &err)
            {
            }
            try
            {   // set the debugging status
                SetDebug(document->getChild("Setup")->getChild("Debug")->getValue());
            }
            catch(BepException &err)
            {
            }
            try
            {   // set debugging level
                SetDebugLevel(document->getChild("Setup")->getChild("DebugLevel")->getValue());
            }
            catch(BepException &err)
            {
            }
            try
            {   // set the process priority
                INT32 priority = atoi(document->getChild("Setup/ProcessPriority")->getValue().c_str());
                m_priority = priority;
            }
            catch(BepException &err)
            {
                if( -1 == m_priority)
                {
                    m_priority = BposGetPriority( 0);
                }
                Log(LOG_DEV_DATA, "Process %s running at priority %d\n", GetProcessName().c_str(), m_priority);
            }

            if(BposSetPriority( 0 , m_priority) == -1)
            {
                Log(LOG_ERRORS, "Could Not Set The Priority %d of %s: %s\n", m_priority,
                        GetProcessName().c_str(), strerror(errno));
            }
            else
            {
                Log(LOG_DEV_DATA, "Updated process %s running at priority %d\n", GetProcessName().c_str(), m_priority);
            }

            try
            {
                m_taskMonTaskName = document->getChild("Setup/TaskMon/TaskName")->getValue();
            }
            catch(...)
            {
                m_taskMonTaskName = GetProcessName();
            }
            Log(LOG_FN_ENTRY, "BepComponent::Initialize(%s) done\n", document->ToString().c_str());
        }
        else throw BepException("Node passed to BepComponent::Initialize(XmlNode *) is NULL");
    }
    catch(BepException &Err)
    {
        SetDebug(false);
        Log(LOG_ERRORS, "BepComponent::Initialize(%s) failed\n", Err.what());
        throw;
    }
    catch(...)
    {
        SetDebug(false);
        Log(LOG_ERRORS, "BepComponent::Initialize() failed for an unknown reason\n");
        throw;
    }
}

//=============================================================================
void BepComponent::ReloadConfiguration(void)
{
    Log(LOG_FN_ENTRY, "BepComponent::ReloadConfig()\n");
    // re-initialize the component
    try
    {
        const XmlNode *document = m_parser.ReturnXMLDocument(GetConfigFile());
        Initialize(document);
    }
    catch(BepException &Err)
    {
        Log(LOG_ERRORS, "%s Error ReloadConfiguration(): %s\n", GetProcessName().c_str(), Err.what());
    }
}

//=============================================================================
void BepComponent::Abort(void)
{
    Log(LOG_DEV_DATA, "BepComponent Abort method called\n");
    SetStatus(BEP_ABORT_STATUS);
}

//=============================================================================
void BepComponent::Reset(void)
{
    Log(LOG_DEV_DATA, "BepComponent Reset Method Called");
}

//=============================================================================
void BepComponent::Terminate(void)
{
    Log(LOG_DEV_DATA, "BepComponent Terminate method called\n");
    SetStatus(BEP_TERMINATE_STATUS);

    ReportTaskEnding();
}

//=============================================================================
void BepComponent::Activate(void)
{
    Log(LOG_FN_ENTRY,"Enter BepComponent::Activate()\n");

    ReloadConfiguration();

    // Change our status with taskmon
    Log( LOG_DEV_DATA, "Changing taskmon status\n");
    ReportTaskUp();

    Log(LOG_FN_ENTRY,"Exit BepComponent::Activate()\n");
}

//=============================================================================
void BepComponent::Deactivate(void)
{
    Log(LOG_FN_ENTRY,"Enter BepComponent::Deactivate()\n");

    // Change our status with taskmon
    Log( LOG_DEV_DATA, "Changing taskmon status\n");
    ReportTaskEnding();

    SetStatus( BEP_DEACTIVATE_STATUS);

    Log(LOG_FN_ENTRY,"Exit BepComponent::Deactivate()\n");
}

//=============================================================================
const std::string BepComponent::Read(const std::string &tag, const INT32 rate /* = 0 */)
{   // Create a node from the data
    XmlNode node(tag, "");
    return(Read(&node));
}

//=============================================================================
const std::string BepComponent::Read(const XmlNode *node, const INT32 rate /* = 0 */)
{
    return(BEP_UNSUPPORTED_RESPONSE);
}

//=============================================================================
const std::string BepComponent::Write(const std::string &tag, const std::string &value)
{   // Create a node from the data
    XmlNode node(tag, value);
    return(Write(&node));
}

//=============================================================================
const std::string BepComponent::Write(const XmlNode *node)
{
    return(BEP_UNSUPPORTED_RESPONSE);
}

//=============================================================================
const std::string BepComponent::Command(const std::string &tag, const std::string &value)
{   // Create a node from the data
    XmlNode node(tag, value);
    return(Command(&node));
}

//=============================================================================
const std::string BepComponent::Command(const XmlNode *node)
{
    std::string status(BEP_SUCCESS_RESPONSE);   // set the status to valid

    // look in the objective list for the command that was sequenced
    if( BEP_ACTIVATE == node->getName())
    {
        if( BEP_DEACTIVATE_STATUS == GetStatus())
        {
            Activate();
        }
    }
    else if( BEP_DEACTIVATE == node->getName())
    {
        if( BEP_DEACTIVATE_STATUS != GetStatus())
        {
            Deactivate();
        }
    }
    else if( BEP_TERMINATE == node->getName())
    {
        Terminate();
    }
    else
    {
        status = BEP_UNSUPPORTED_RESPONSE;
    }

    return( status);
}

//=============================================================================
const std::string BepComponent::Subscribe(const std::string &tag, const std::string &Process)
{   // Create a node from the data and call the Subscribe method
    XmlNode node(tag, Process);
    return(Subscribe(&node));
}

//=============================================================================
const std::string BepComponent::Subscribe(const XmlNode *node)
{
    Log(LOG_FN_ENTRY,  "BepComponent::Subscribe(%s, %s)\n", node->getName().c_str(), node->getValue().c_str());
    std::string result = BEP_FAILURE_RESPONSE;
    try
    {
        result = Read(node->getName());                                             // Return data!
        if(m_nList == NULL)
        {
//          testing notification thread spinner class
#if NOTIFICATION_THREADPOOL
            NotificationThreadPool<IBepCommunication>* pool = new NotificationThreadPool<IBepCommunication>();
#else
            NotificationThreadSpinner<IBepCommunication>* pool = new NotificationThreadSpinner<IBepCommunication>();
#endif
            if(m_nListConfig)
            {
                pool->Initialize(m_nListConfig);
                pool->Start();
                m_nList = pool;
            }
            else throw BepException("Notification list configuration was NULL");
        }
        if(m_nList) m_nList->AddNotification(node->getName(), node->getValue());    // I want data!
    }
    catch(BepException &BepErr)
    {
        Log(LOG_ERRORS, "BepComponent::Subscribe(%s %s) Falied Because: %s\n", node->getName().c_str(), node->getValue().c_str(), BepErr.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "BepComponent::Subscribe() Falied for an unknown reason\n");
    }
    Log(LOG_FN_ENTRY,  "BepComponent::Subscribe(%s %s) done returning %s\n", node->getName().c_str(), node->getValue().c_str(), result.c_str());
    return(result);
}

//=============================================================================
const std::string BepComponent::Unsubscribe(const std::string &tag, const std::string &Process)
{   // Build a node from the data and return the result of the Unsubscribe method
    XmlNode node(tag, Process);
    return(Unsubscribe(&node));
}

//=============================================================================
const std::string BepComponent::Unsubscribe(const XmlNode *node)
{
    std::string result = BEP_FAILURE_RESPONSE;
    try
    {
        if(m_nList)
        {
            m_nList->RemoveNotification(node->getName(), node->getValue()); // Get me off your list!
            result = BEP_SUCCESS_RESPONSE;              // It's all groovy!
        }
    }
    catch(BepException &BepErr)
    {
        Log(LOG_ERRORS, "BepComponent::Unsubscribe(%s %s) Falied Because: %s\n", node->getName().c_str(), node->getValue().c_str(),  BepErr.what());
    }
    catch(...)
    {
        Log(LOG_ERRORS, "BepComponent::Unsubscribe() Falied for an unknown reason\n");
    }
    return(result);
}

//=============================================================================
const std::string BepComponent::Publish(const std::string &tag, const std::string &value)
{   // Build a node from the data
    XmlNode node(tag, value);
    return(Publish(&node));
}

//=============================================================================
const std::string BepComponent::Publish(const XmlNode *node)
{
    return(BEP_UNSUPPORTED_RESPONSE);
}

//=============================================================================
const std::string BepComponent::Register(void)
{
    ReportTaskUp();

    return(BEP_SUCCESS_RESPONSE);
}

//=============================================================================
const std::string BepComponent::GetSpecialInfo(const std::string &tag, const std::string info)
{
    return("");
}

//=============================================================================
void BepComponent::SpecialProcessing(XmlNode *node, const std::string type, BepComponent *server)
{
}

//=============================================================================
const INT32 BepComponent::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 stat = BEP_STATUS_SUCCESS;    // set to pass
    Log(LOG_DEV_DATA, "BepComponent::HandlePulse(%d, %d)\n", code, value);

    switch(code)
    {
    case SYSTEM_PULSE:
        switch(value)
        {
        case ABORT_PULSE:   // handle the abort pulse
            Abort();
            if(IsDebugOn()) printf("Received Abort\n");
            break;
        case RESET_PULSE:   // handle the reset pulse
            Reset();
            if(IsDebugOn()) printf("Received Reset\n");
            break;
        case RELOAD_PULSE:  // handle the reload pulse
            ReloadConfiguration();
            if(IsDebugOn()) printf("Received ReloadConfiguration\n");
            break;
        case TERMINATE_PULSE:   // handle the terminate pulse
            Terminate();
            if(IsDebugOn()) printf("Received Terminate\n");
            break;
        case TASK_MON_PULSE:
            ReportTaskUp();
            if(IsDebugOn()) printf("Received TaskMon pulse\n");
            break;
        case ACTIVATE_PULSE:
            Activate();
            if(IsDebugOn()) printf("Received Activate pulse\n");
            break;
        case DEACTIVATE_PULSE:
            Deactivate();
            if(IsDebugOn()) printf("Received Deactivate pulse\n");
            break;
        default:
            stat = BEP_STATUS_FAILURE;
            break;
        }
        break;
    default:
        // build the pulse and have the system handle it.
        struct _pulse pulse;
        pulse.code = code;
        pulse.value.sival_int = value;
		void *tempPulse = &pulse;
//        stat = BposHandlePulse((void *) &pulse);
		stat = BposHandlePulse(tempPulse);
        break;
    }
    return(stat);
}

//=============================================================================
const std::string BepComponent::GetConfigFile(void)
{
    return(m_configFile);
}

//=============================================================================
void BepComponent::Run(volatile bool *terminateFlag/*=NULL*/)
{
}

//=============================================================================
void BepComponent::SetConfigFile(const std::string &configFile)
{
    m_configFile = configFile;
}

//=============================================================================
void BepComponent::SetConfigFile(const char *configFile)
{
    SetConfigFile(std::string(configFile));
}

//=============================================================================
TaskMonClass_t BepComponent::GetTaskClass(void) const
{
    return( TASKMON_CLASS_CORE);
}

//=============================================================================
void BepComponent::RegisterWithTaskMon(void)
{
    // m_taskMon gets deleted inside ~BepComponent () if it is not NULL
    if( m_taskMon == NULL)
    {
        m_taskMon = new ITaskMon( m_taskMonTaskName.c_str(), GetTaskClass());
    }

    if( m_taskMon != NULL)
    {
        m_taskMon->Register(m_taskMonTaskName.c_str(), GetTaskClass());
    }
}

//=============================================================================
void BepComponent::ReportTaskUp(void)
{
    if( m_taskMon != NULL)
    {
        m_taskMon->ReportUp(m_taskMonTaskName.c_str(), GetTaskClass());
    }
}

//=============================================================================
void BepComponent::ReportTaskEnding(void)
{
    if( m_taskMon != NULL)
    {
        m_taskMon->ReportTerminate();
    }
}

//=============================================================================
void BepComponent::ReportTaskDown(void)
{
    if( m_taskMon != NULL)
    {
        m_taskMon->ReportDone();
    }
}

