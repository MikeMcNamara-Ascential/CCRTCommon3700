//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Raw/SCI/SciCommProxyMain.cpp $
// $Author: Cward $
//   $Date: 4/13/06 3:59a $
//
// Description:
//	Logical serial port driver for SCI serial communication
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Raw/SCI/SciCommProxyMain.cpp $
// 
// 2     4/13/06 3:59a Cward
// cleanup usage block
//
// 2     2/15/06 4:07p Bmeinke
// Removed the $Revision: 2 $ and $Date: 4/13/06 3:59a $ strings from the __USAGE block
//
// 1     1/23/04 9:40a Cward
// Restructuring Raw Comm Proxies
//
// 1     8/21/03 9:05a Derickso
// 05WK CCRT Project
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
// 6     2/13/03 10:10a Bmeinke
// Removed data members m_reflCoid and m_reflChid (not used)
// Removed InitManager(), CleanupManager() and WaitForReflection() methods
// (no longer needed)
//
// 5     2/11/03 12:19p Bmeinke
// Replaced LockPortHandler() and UnlockPortHandler() with LockForClient()
// and UnlockForClient()
//
// 4     2/04/03 5:34p Bmeinke
// Fixed UnlockPortHandler(): Only release Mux lock if the client is
// releasing his last port lock
//
// 3     1/29/03 5:30p Bmeinke
// Added main() (oops)
//
// 2     1/22/03 3:21a Bmeinke
// Dont explicitly call SetProcessName() in the command line argument
// constructor (let the base class hndel it after it parses the command
// line and can determine what our logical port name is)
//
// 1     1/21/03 3:16a Bmeinke
// Logical serial port driver for SCI serial communication
//
//******************************************************************************

#include "SciCommProxy.h"

int main( int argc, char *argv[])
{
	SciCommProxy	*commProxy;
	int				retVal = RES_MGR_CMD_NONE;

	while( retVal != RES_MGR_CMD_TERMINATE)
	{
		commProxy = new SciCommProxy( argc, argv);

		retVal = commProxy->Execute();

		delete commProxy;
	}

	return( retVal);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

SCI serial communication proxy

%C [-d] [-e] [-ffile] [-rname] [-vx] [-pSerialPort] [-Pname] &

    -d    Run in demo mode
    -e    Echo logger messages to stdout
    -f    Read config from 'file'
    -r    Register using 'name'
    -v    Use verbose mask 'x' (in hex, see notes)
    -p    Communicate using serial port 'SerialPort' (i.e. /dev/ser1)
    -P    Send pulse to process registered as 'name' when up and running

    Notes:
    -Verbose Mask bit definitions (leading 0s optional):
        00000000: No Log Messages
        00000001: Log Errors
        00000002: Log Function Entry/Exit
        00000004: Log I/O message data
        00000008: Log device general messages
        00000010: Log serial communications
        FFFFFFFF: Log everything
    - Registers name as 'logport/J2190PortDriver' by default

#endif
*******************************************************************************/

