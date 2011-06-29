//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Raw/RawCommProxyMain.cpp $
// $Author: Cward $
//   $Date: 4/13/06 3:58a $
//
// Description:
//	Logical serial port driver for raw serial communication
//
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Raw/RawCommProxyMain.cpp $
// 
// 3     4/13/06 3:58a Cward
// cleanup usage block
//
// 2     2/15/06 7:57p Cward
// Updated for 06021501 iteration
//
// 1     12/21/03 6:28p Cward
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 4:43p Khykin
// Initial checkin.
//
// 2     5/19/03 9:26a Khykin
// Updated with Brians latest changes.
//
// 1     4/30/03 4:55p Bmeinke
// Migration to new personalized configuration management structure
//
// 2     1/21/03 3:18a Bmeinke
// Fixed header comments
//
// 1     1/20/03 10:41p Bmeinke
// Serial communication proxies for raw (unformatted) serial data
//
//******************************************************************************

#include "RawCommProxy.h"

int main( int argc, char *argv[])
{
	RawCommProxy	*comProxy;
	int				retVal = RES_MGR_CMD_NONE;

	while( retVal != RES_MGR_CMD_TERMINATE)
	{
		comProxy = new RawCommProxy( argc, argv);

		retVal = comProxy->Execute();

		delete comProxy;
	}

	return( retVal);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

Raw serial communication proxy

%C [-d] [-e] [-ffile] [-rname] [-vx] [-pSerialPort] [-Pname] &

    -d    Run in demo mode
    -e    Echo logger messages to stdout
    -f    Read config from 'file'
    -r    Register using 'name'
    -v    Use verbose mask 'x' (in hex, see notes)
    -p    Communicate using serial port 'SerialPort' (i.e. /dev/ser1)
    -P    Send pulse to process registered as 'name' when up and running
    -R    Dont wait for a register pulse to Register...do it during initialization

	Notes:
	-Verbose Mask bit definitions (leading 0s optional):
		00000000: No Log Messages
		00000001: Log Errors
		00000002: Log Function Entry/Exit
		00000004: Log I/O message data
		00000008: Log device general messages
		00000010: Log serial communications
		FFFFFFFF: Log everything
	- Registers name as 'logport/CommProxy' by default

#endif
*******************************************************************************/

