//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcMgrMain.cpp $
// $Author: Cward $
//   $Date: 4/13/06 4:09a $
//
// Description:
//    PLC I/O manager main
//
//
//==============================================================================
//
//     Copyright © 2003 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
//==============================================================================
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/PlcMgr/PlcMgrMain.cpp $
// 
// 2     4/13/06 4:09a Cward
// Updated the PLC logic
//
// 2     2/15/06 4:20p Bmeinke
// Removed the $Revision: 2 $ and $Date: 4/13/06 4:09a $ strings from the __USAGE block
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
//
//******************************************************************************

/*******************
 include files
 ******************/
#include "PlcMgr.h"

PlcMgr		*g_ioMgr;

//==============================================================================
//==============================================================================

int main( int argc, char *argv[])
{
	int	retVal = RES_MGR_CMD_NONE;
	ILogger		logger;

	logger.SetProcessName( PLC_MGR_NAME"(main)");
	logger.EnableLogger();

	while( retVal != RES_MGR_CMD_TERMINATE)
	{
		try
		{
			//  Create PLC Manager object
			g_ioMgr = new PlcMgr( argc, argv);
			// Execute the manager message loop
			retVal = g_ioMgr->Execute();
		}
		catch( XmlException &xmlErr)
		{
			logger.Log( "Caught unhandled XmlException: %s\n", xmlErr.GetReason());
		}
		catch( BepException &bepErr)
		{
			logger.Log( "Caught unhandled BepException: %s\n", bepErr.GetReason());
		}
		catch( ...)
		{
			logger.Log( "Caught unhandled unknown Exception\n");
		}
		delete g_ioMgr;
	}

	return( retVal);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

PLC Board Resource Manager

%C [-cz] [-d] [-e] [-ffile] [-na] [-rname] [-py] [-vx] &

    -cz       Thread scan period = 'z' (milliseconds). Default=20ms
    -d        Run in demo mode
    -e        Echo logger messages to stdout
    -ffile    Read config from file
    -na       Force numbmer of channels to 'a'
    -py       Run scanner at priority 'y'
    -rname    Register using 'name'
    -vx       Use verbose mask 'x' (in hex, see notes)
    -R        Register immediately during initialization instead of waiting for
              the register command

    Notes:
    -Verbose Mask bit definitions (leading 0s optional):
        00000000: No Log Messages
        00000001: Log Errors
        00000002: Log Function Entry/Exit
        00000004: Log I/O message data
        00000008: Log device general messages
        FFFFFFFF: Log everything
    - Registers name as 'mgr/PLC' by default
    - Must be run as Super User.

#endif

*******************************************************************************/

