//*************************************************************************
// Description:
//   This class will launch the system as specified by the configuration file.
//	 It will also wait for a terminate, abort, or reset message to be send and
// 	 then it will command all of its children to terminate, abort, or reset.
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
//    $Log: /Core/ServicesLayer/Servers/Launcher/LauncherMain.cpp $
// 
// 5     3/11/05 3:37p Cward
// Fixed OrderToLaunch problems, including duplicate values, and missing
// values
//
// 4     6/24/04 1:13a Cward
// Removed pound if 0 block. Updated Header block comments to the new
// standard.
//
// 3     5/19/04 10:26p Cward
// Removed the dash d option
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:41p Cward
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:03p Khykin
// Initial checkin.
//
// 1     4/22/03 9:47a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:52  tbochene
//
//    PooBag
//Revision:  $
//
// 1     1/27/03 7:13a Khykin
// Initial Checkin
//
// 1     1/18/03 8:09a Khykin
// Initial Checkin From DCTC
//
// 6     5/28/02 3:55p Kh
// Updating due to unusual checkin problem.
//
// 5     5/24/02 3:07p Kh
// Adding usage information.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "Launcher.h"

#include "CmdLineProcessor.h"

/**
 * Task that launches the system.  It takes a configuration
 * file that specifies tasks and their operational parameters
 * and launches them.  Once the tasks are launched, the
 * program terminates.
 *
 * @param argc   Number of arguments inputted.
 * @param argv   Character array of arguments.
 */
int main(int argc, char *argv[])
{	// command line parser to interpret the provided data
	CmdLineProcessor clp;				// create a command line parser
	clp.ParseArguments(argc, argv);		// parse the command line arguments
	BepComponent *launcher = NULL;		// create a launcher object

	// if config file name has been given
	if(clp.GetConfigFile() != "")
	{
        try
		{
			launcher = new Launcher();					// create a launcher
            launcher->Initialize(clp.GetConfigFile());	// initialize it
            launcher->Run();    						// and run
		}
		catch(BepException &e)
		{
			printf("Launcher Exception: %s\n",  e.Reason().c_str());
		}
		catch(...)
		{
			printf("Launcher Unknown Exception\n");
		}
	}
	else
		printf("Launcher Could Not Open: %s\n", clp.GetConfigFile().c_str());


    // delete the launcher if it has been created
    if(launcher != NULL)    delete launcher;

	return(0);
};

#ifdef __USAGE

Launcher Version 1.0

%C [-f file]

options:

	-f 		indicates the XML configuration file to use for initialization


Note:	The launcher must be compiled as root due to the types of
		tasks that are managed.

#endif	// __USAGE


