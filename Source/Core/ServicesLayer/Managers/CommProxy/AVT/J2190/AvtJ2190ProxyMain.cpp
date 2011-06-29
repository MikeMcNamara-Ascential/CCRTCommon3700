//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/AVT/J2190/AvtJ2190ProxyMain.cpp $
// $Author: Cward $
//   $Date: 4/13/06 3:54a $
//
// Description:
//	Logical serial port driver main for the AVT manager using J2190 serial communication
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
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/AVT/J2190/AvtJ2190ProxyMain.cpp $
// 
// 2     4/13/06 3:54a Cward
// claened up usage block
//
// 2     2/15/06 4:04p Bmeinke
// Removed the $Revision: 2 $ and $Date: 4/13/06 3:54a $ strings from the __USAGE block
//
// 1     12/21/03 6:28p Cward
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
//
//
//******************************************************************************

#include "AvtJ2190Proxy.h"

int main( int argc, char *argv[])
{
	AvtJ2190Proxy	*j2190Drv;
	int				retVal = RES_MGR_CMD_NONE;

	while( retVal != RES_MGR_CMD_TERMINATE)
	{
		j2190Drv = new AvtJ2190Proxy( argc, argv);

		retVal = j2190Drv->Execute();

		delete j2190Drv;
	}

	return( retVal);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

AVT J2190 Proxy

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

