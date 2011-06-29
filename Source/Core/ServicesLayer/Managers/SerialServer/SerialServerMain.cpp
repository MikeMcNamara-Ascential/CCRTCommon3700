//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/SerialServer/SerialServerMain.cpp $
// $Author: Cward $
//   $Date: 4/13/06 4:11a $
//
// Description:
//	Serial Communication Server
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
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/SerialServer/SerialServerMain.cpp $
// 
// 2     4/13/06 4:11a Cward
// cleanup usage block
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

#include "SerialServer.h"

int main( int argc, char *argv[])
{
	SerialServer	*serialSrvr;
	int				retVal = RES_MGR_CMD_NONE;

	while( retVal != RES_MGR_CMD_TERMINATE)
	{
		serialSrvr = new SerialServer( argc, argv);

		retVal = serialSrvr->Execute();

		delete serialSrvr;
	}

	return( retVal);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

Serial Communication Resource Manager

%C [-d] [-e] [-ffile] [-rname] [-vx] [-R] &

    -d        Run in demo mode
    -e        Echo logger messages to stdout
    -ffile    Read config from file
    -rname    Register using 'name'
    -vx       Use verbose mask 'x' (in hex, see notes)
    -R        Dont wait for a register pulse to Register...do it during initialization

    Notes:
    -Verbose Mask bit definitions (leading 0s optional):
        00000000: No Log Messages
        00000001: Log Errors
        00000002: Log Function Entry/Exit
        00000004: Log I/O message data
        00000008: Log device general messages
        FFFFFFFF: Log everything
    - Registers name as 'mgr/Serial' by default

#endif
*******************************************************************************/

