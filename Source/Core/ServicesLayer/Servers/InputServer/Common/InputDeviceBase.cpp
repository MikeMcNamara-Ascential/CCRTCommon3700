//*************************************************************************
// FILE DESCRIPTION:
//	The InputDeviceBase is a base class that all individual input devices
//  should inherit from to provide easy to reuse base functionality.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Working/Source/Core/ServicesLayer/Servers/InputServer/Common/InputDeviceBase.cpp $
// 
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
// 
//*************************************************************************

#include "InputDeviceBase.h"

InputDeviceBase::InputDeviceBase() : BepServer(), m_inputServerComm(NULL),
m_parentName("core/InputServer")
{   // Nothing to do here
}

InputDeviceBase::~InputDeviceBase()
{
    // remove the input server communication obj memory
    if (NULL != m_inputServerComm)
    {
        delete m_inputServerComm;
        m_inputServerComm = NULL;
    }
}

void InputDeviceBase::Initialize(int argc, char *argv[])
{
    ParseCommandLine(argc,argv);
    Initialize(GetConfigFile());
}

void InputDeviceBase::Initialize(const std::string &fileName)
{
    try
    {
        // Parse configuration file
        const XmlNode *document = m_parser.ReturnXMLDocument(fileName); 

        if (document)
        {
            // initialize server
            Initialize(document->getChild("Setup/InputDevices")->getChild(m_pathName));       
        }
        // use printf() because the logger has not been setup until BepServer::Initialize is called
        else printf("InputDeviceBase::Initialize() Failed to parse configuration file\n");
    }
    catch (BepException &BepErr)
    {
        printf("InputDeviceBase::Initialize(%s) failed: %s\n", fileName.c_str(), BepErr.what());
        throw;
    }
    catch (...)
    {
        printf("InputDeviceBase::Initialize(%s) failed: for an unknown reason\n", fileName.c_str());
        throw;
    }
}

void InputDeviceBase::Initialize(const XmlNode *configNode)
{
    // Initialize the base component
    BepServer::Initialize(configNode);

    // Initialize the scanner comm object
    Log(LOG_FN_ENTRY, "InputDeviceBase::Initialize()\n");

    if (NULL == m_inputServerComm)
    {
        // create a communication node to talk with the parent class who spawned the input device
        m_inputServerComm = new IBepCommunication();
        // Initialize the communications
        m_inputServerComm->Initialize(m_parentName,"Client");
    }

    Log(LOG_FN_ENTRY, "InputDeviceBase::Initialize() done\n");
}

void InputDeviceBase::ParseCommandLine(int argc, char *argv[])
{
    char        optId;
    std::string optString("f:der:v:P:p:");

    Log(LOG_FN_ENTRY,"Enter InputDeviceBase::ParseCommandLine\n");

    // Disable "Invalid argument" message from getopt
    opterr = 0;

    // Start reparsing from the first argument
    optind = 1;

    // Get Command line options
    while ( optind < argc)
    {
        optId = getopt(argc, argv, optString.c_str());

        if (HandleCommandLineArg(optId, optarg) == false)
        {
            printf("\tUnknown arg: %c @ %d\n", optId, optind++);
            if (optId == '?')                        optind++;
            else if (argv[ optind] == NULL)          optind++;
            else if (argv[optind][0] != '-')         optind++;
            else if ((argv[ optind][0] == '-') && (strlen(argv[ optind]) == 1)) optind++;
        }
        optarg = NULL;
    }
    Log(LOG_FN_ENTRY,"Exit InputDeviceBase::ParseCommandLine\n");
}


bool InputDeviceBase::HandleCommandLineArg(char option, const char *value)
{
    bool    retVal = true;

    Log(LOG_FN_ENTRY,"Enter InputDeviceBase::HandleCommandLineArg ( %c, '%s')\n",
        option, optarg == NULL ? "" : optarg);

    switch ( option)
    {
    case 'p':       // Path to adopt
        m_pathName = optarg;
        break;
    case 'P':       // Name of parent process to notify
        if ( value != NULL)      m_parentName = value;
        else                    m_parentName = "core/InputServer";
        Log(LOG_DEV_DATA,"parent process = %s\n", m_parentName.c_str());
        break;
    case 'e':
        EnableLogger();
        SetLocalEcho(true);
        break;
    case 'f':
        if (value != NULL)   SetConfigFile(value);
        break;
    case 'r':
        if (value != NULL)   SetProcessName(value);
        break;
    case 'v':
        if (value != NULL)
        {
            char *tmp;
            UINT32  verbose;
            if ( (tmp=strstr( optarg, "$")) != NULL)         sscanf( optarg, "$%X", &verbose);
            else if ( (tmp=strstr( optarg, "0x")) != NULL)   sscanf( optarg, "0x%X", &verbose);
            else if ( (tmp=strstr( optarg, "0X")) != NULL)   sscanf( optarg, "0X%X", &verbose);
            else                                            sscanf( optarg, "%X", &verbose);
            SetVerboseMask( verbose);
            Log(LOG_DEV_DATA, "Verbose: $%08X\n", GetVerboseMask());
        }
        break;
    default:
        break;
    }
    Log(LOG_FN_ENTRY, "Exit InputDeviceBase::HandleCommandLineArg( %c, '%s')\n",
        option, NULL == optarg ? "" : optarg);

    return( retVal);
}
