//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/CmdLineProcessor.cpp 1     12/21/03 6:27p Cward $
//	
//
//
// FILE DESCRIPTION:
//   The is the base component for the RTOS system it will provide the base
//   functionality for the majority of the system.
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
//    $Log: /Core/ServicesLayer/Library/CmdLineProcessor.cpp $
// 
// 1     12/21/03 6:27p Cward
// 
// 1     10/07/03 11:35a Khykin
// Initial checkin
// 
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
// 
// 1     6/12/03 4:53p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:21a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:40  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 6:25a Khykin
// Initial Checkin
// 
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
// 
// 14    10/08/02 3:26p Khykin
// Updating the demo selection due it should just take a -d not -d
// 
// 13    6/04/02 6:25p Skypig13
// Updating error conditions in ParseCommandLine()
// 
// 12    2/04/02 11:39p Skypig13
// Removed compiler warnings
//
// 11    1/15/02 10:39p Skypig13
// Removed conditional print if argument is not recognized
//
// 10    12/18/01 1:16p Skypig13
// Implimented debuggable funtionality
//
// 9     11/15/01 3:44p Skypig13
// Updated
//
// 8     11/13/01 11:06a Skypig13
// Put \n at end of printout of unrecognized option and
// replaced exception with printf.
//
// 7     11/13/01 11:01a Skypig13
// Updated to handle & properly
//
// 6     11/07/01 7:43p Brian.meinke
// Changed ParseArguments to use the getopt() library function
//
// 5     6/13/01 7:23p Admin
// Change Boolean to bool
//
// 4     5/02/01 6:39p Skypig13
// Update for use with Base reuse directory
//
// 3     5/01/01 10:29a Skypig13
// Update initializer list in constructor, update to conform to BEP coding
// standards
//
// 2     5/01/01 9:08a Skypig13
// Minor tweaks to update constructor and switch statement
//
// 1     4/25/01 10:53a Skypig13
// Initial version
//
//  1    02/18/01 1:00p Tim.Bochenek
// Initial version.
//
//*************************************************************************
#include "BepException.h"
#include "CmdLineProcessor.h"
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif

CmdLineProcessor::CmdLineProcessor() :  Debuggable(),
m_demoMode(false), m_console(false), m_verbose(0), m_port(0)
{	// initialize internal data
}
CmdLineProcessor::CmdLineProcessor(int argc, char *argv[], bool debug /* = false */) : Debuggable(debug),
m_demoMode(false), m_console(false), m_verbose(0), m_port(0)
{	// Parse arguments
	if(IsDebugOn())	printf("CmdLineProcessor(%d, %s, %d)\n", argc, argv[1], debug);
	ParseArguments(argc, argv);
}
CmdLineProcessor::~CmdLineProcessor()
{
}
void CmdLineProcessor::ParseArguments(int argc, char *argv[])
{	// Start at second argument (skip program name)
	optind = 1;
	if(IsDebugOn())
	{
		for(int ii = 0; ii < argc; ii++)
		{
			if(argc) printf("argv[%d] = %s\n", ii, argv[ii]);
			else printf("argument %d is NULL\n", ii);
		}
	}
	// Read all options specified on command line
	while(optind < argc)
	{	// get pointer to next argurment String
		errno = EOK;
		int opt = getopt(argc, argv, "A:a:CcDdF:f:P:p:V:v:&");
		if(IsDebugOn())	printf("CmdLineProcessor: processing: #%d, arg: %c\n", optind, opt);
		// process this argument
		switch(opt)
		{
		case 'd':
		case 'D':	// Demo Mode Flag
			m_demoMode = true;
			if(IsDebugOn())	printf("Demo mode on\n");
			break;
		case 'c':
		case 'C':	// Console Mode Flag
			m_console = true;
			SetDebug(true);
			if(IsDebugOn())	printf("Console print on\n");
			break;
		case 'a':
		case 'A':	// Address Value
			m_address = optarg;
			if(IsDebugOn())	printf("Address: %s\n", GetAddress().c_str());
			break;
		case 'p':
		case 'P':	// Port Value
			m_port = atoi(optarg);
			if(IsDebugOn())	printf("Port: %d\n", GetPort());
			break;
		case 'v':
		case 'V':	// Verbose Mode Value
			m_verbose = atoi(optarg);
			if(IsDebugOn())	printf("Verbocity: %u\n", GetVerboseStatus());
			break;
		case 'f':
		case 'F':	// Configuration File Name
			m_configFile = optarg;
			if(IsDebugOn())	printf("Configuration file: %s\n", GetConfigFile().c_str());
			break;
		case '&':				// Background execution requested
			if(IsDebugOn())	printf("Background mode requested\n");
			break;
		default:	// Unknown Data
			if(argv[optind] == NULL) 
			{
				if(IsDebugOn()) printf("CmdLineProcessor: argument: #%d == NULL\n", optind);
			}
			else if(opt == 0)  
			{
				if(IsDebugOn()) printf("opt = %d\n", opt);
			}
			else
			{
				if(IsDebugOn()) printf("CmdLineProcessor: argument: #%d, Unknown: %c (%s)\n", optind, opt, argv[optind]);
			}
			optind++;
			break;
		}
	}
}
const bool CmdLineProcessor::GetDemoModeStatus(void)
{
	return(m_demoMode);
}
const bool CmdLineProcessor::GetConsoleStatus(void)
{
	return(m_console);
}
const UINT32 CmdLineProcessor::GetVerboseStatus(void)
{
	return(m_verbose);
}
const std::string CmdLineProcessor::GetConfigFile(void)
{
	return(m_configFile);
}
const UINT16 CmdLineProcessor::GetPort(void)
{
	return(m_port);
}
const std::string CmdLineProcessor::GetAddress(void)
{
	return(m_address);
}

