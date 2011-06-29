//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/AVT/CAN/AvtCanProxyMain.cpp $
// $Author: Cward $
//   $Date: 4/13/06 3:53a $
//
// Description:
//	Logical serial port driver for CAN serial communication
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/AVT/CAN/AvtCanProxyMain.cpp $
// 
// 3     4/13/06 3:53a Cward
// removed rev and date from usage block
//
// 2     2/15/06 7:54p Cward
// move usage block to main
//
// 1     12/21/03 6:28p Cward
//
// 2     6/24/03 9:58a Bmeinke
// Changed ModuleId_t data type from uint8_t to uint16_t
// Changed the processing of the 'dataReady' flags in the m_moduleList
// list: Only clear the flag when a client empties his FIFO and set it to
// true after we have received a full packet from a module in
// ProcessVehicleMessage()
// When we receive data from a module we always put the received data into
// ur client's fifo(s) (even if we reply to the client right away). This
// way, if a client is  read-blocked waiting for less data than we can
// send to him, his fifo will contain the remaining data on his next
// read() call.
//
// 1     6/20/03 9:17p Bmeinke
//
//******************************************************************************

#include "AvtCanProxy.h"

const uint8_t AvtAckString[] = AVT_ACK_STRING;

const uint32_t	AvtTimeoutVal	=	10;

int main( int argc, char *argv[])
{
	AvtCanProxy	*canDrv;
	int			retVal = RES_MGR_CMD_NONE;

	while( retVal != RES_MGR_CMD_TERMINATE)
	{
		canDrv = new AvtCanProxy( argc, argv);

		retVal = canDrv->Execute();

		delete canDrv;
	}

	return( retVal);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

AVT CAN Proxy

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
    - Registers name as 'logport/CanPortDriver' by default

#endif
*******************************************************************************/

