//****************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Include/CmdLineProcessor.h 1     12/21/03 6:26p Cward $
//
// DESCRIPTION:
//    Processor to parse and store command line information to be used later
//
//============================================================================
//
//    Copyright (c) 1999 Burke E. Porter Machinery
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//
// LOG:
//    $Log: /Core/ServicesLayer/Include/CmdLineProcessor.h $
// 
// 1     12/21/03 6:26p Cward
// 
// 1     10/07/03 11:37a Khykin
// Initial checkin
// 
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
// 
// 1     6/12/03 4:07p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:35  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:03a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
// 
// 4     11/15/01 3:47p Skypig13
// Updated
//
// 3     11/07/01 7:43p Brian.meinke
// Added #include <unistd.h> in order to use the getopt() library function
// inside ParseArguments()
//
// 2     6/13/01 7:30p Skypig13
// Change Boolean to bool
//
// 1     6/13/01 7:29p Skypig13
// Add to VSS
//
// 3     5/01/01 10:28a Skypig13
// Update to meet BEP coding standards
//
// 2     5/01/01 9:09a Skypig13
// Minor tweaks updating constructor and switch statement
//
// 1     4/10/01 10:46a Admin
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
//  1    02/18/01 1:00p Tim.Bochenek
// Initial version.
//
//****************************************************************************
#ifndef CMDLINEPROCESSOR_H
#define CMDLINEPROCESSOR_H
//==============================================================================
//	Includes
//==============================================================================

#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "Bepos.h"
#include "BepDefs.h"
#include "Debuggable.h"

/**
 * Class used to prcess the arguments passed in at the command line
 *
 * @author Tim Bochenek
 * @author Kevin Hykin
 * @version 2.1
 * @since Version 1.0
 */
class CmdLineProcessor : public Debuggable
{
public:
	/**
	 * Class constructor (Default)
	 * @since Version 1.0
	 */
    CmdLineProcessor();
	/**
	 * Class constructor (Default)
	 *
	 * @param argc   Number of arguments passed in at the command line
	 * @param argv   Command line arguments
	 * @param debug  Debug status
	 * @since Version 1.0
	 */
	CmdLineProcessor(int argc, char *argv[], const bool debug = false);
	/**
	 * Class destructor
	 * @since Version 1.0
	 */
    virtual ~CmdLineProcessor();
	/**
	 * Process all user configurations
	 *
	 * @param argc   Number of arguments passed in at the command line
	 * @param argv   Command line arguments
	 * @since Version 1.0
	 */
	void ParseArguments(int argc, char *argv[]);
	/**
	 * Get Demo Mode status (TRUE/FALSE)
	 *
	 * @return Whether demo mode is active
	 * @since Version 1.0
	 */
	const bool GetDemoModeStatus();
	/**
	 * Get console Mode status (TRUE/FALSE)
	 *
	 * @return Whether demo mode is active
	 * @since Version 1.0
	 */
	const bool GetConsoleStatus();
	/**
	 * Get Verbose Mode status (TRUE/FALSE)
	 *
	 * @return Value 1 to ...
	 * @since Version 1.0
	 */
	const UINT32 GetVerboseStatus(void);
	/**
	 * Get Configuration file name
	 *
	 * @return Configuration file name
	 * @since Version 1.0
	 */
	const std::string GetConfigFile(void);
	/**
	 * Get the configuration port
	 *
	 * @return Configuration port
	 * @since Version 1.0
	 */
	const UINT16 GetPort(void);
	/**
	 * Get the configuration address
	 *
	 * @return  Configuration address
	 * @since Version 1.0
	 */
	const std::string GetAddress(void);
private:
	/**
	 * Demo Mode Status
	 * @since Version 1.0
	 */
	bool	m_demoMode;
	/**
	 * Whether the program diagnostics are to be sent to the console
	 * @since Version 1.0
	 */
	bool	m_console;
	/**
	 * Level Of Program Information
	 * @since Version 1.0
	 */
	UINT32 m_verbose;
	/**
	 * Configuration File Name
	 * @since Version 1.0
	 */
	std::string	m_configFile;
	/**
	 * Address to configure to (IP, ... )
	 * @since Version 1.0
	 */
	std::string	m_address;
	/**
	 * Port to configure to
	 * @since Version 1.0
	 */
	UINT16 m_port;
};

#endif  //CMDLINEPROCESSOR_H
