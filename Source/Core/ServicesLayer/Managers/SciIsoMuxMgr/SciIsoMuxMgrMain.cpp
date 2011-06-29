//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/SciIsoMuxMgr/SciIsoMuxMgrMain.cpp $
// $Author: Cward $
//   $Date: 4/13/06 4:10a $
//
// Description:
//	SCI/ISO mux board manager main
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
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/SciIsoMuxMgr/SciIsoMuxMgrMain.cpp $
// 
// 2     4/13/06 4:10a Cward
// Init var
//
// 2     3/20/06 10:53a Bmeinke
// Initialize muxMgr to NULL
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

#include "SciIsoMuxMgr.h"

int main( int argc, char *argv[])
{
	int 			retVal = RES_MGR_CMD_NONE;
	ILogger			logger;
	SciIsoMuxMgr	*muxMgr = NULL;

	logger.SetProcessName( SCI_ISO_MUX_NAME"(main)");
	logger.EnableLogger();

	// Give our thread highest privity (inport/outport)
	if( ThreadCtl(_NTO_TCTL_IO, 0) != -1)
	{
		while( retVal != RES_MGR_CMD_TERMINATE)
		{
			try
			{
				//  Create Universe Chip Manager object
				muxMgr = new SciIsoMuxMgr( argc, argv);
				// Execute the manager message loop
				retVal = muxMgr->Execute();
			}
			catch( XmlException &xmlErr)
			{
				logger.Log( "Caught unhandled XmlException: %s\n", xmlErr.GetReason());
				retVal = RES_MGR_CMD_TERMINATE;
			}
			catch( BepException &bepErr)
			{
				logger.Log( "Caught unhandled BepException: %s\n", bepErr.GetReason());
				retVal = RES_MGR_CMD_TERMINATE;
			}
			catch( ...)
			{
				logger.Log( "Caught unhandled unknown Exception\n");
				retVal = RES_MGR_CMD_TERMINATE;
			}
			if( NULL != muxMgr)	delete muxMgr;
		}
	}
	else
	{
		logger.Log( "Unable to gain I/O privity: %s\n", strerror( errno));
	}

	return( retVal);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

SCI/ISO Mux Board Control Line Resource Manager

%C [-d] [-e] [-rName] [-vx]  &

    -d      Run in demo mode
    -e      Echo logger messages to stdout
    -rName  Register task using 'Name' (default=mgr/MuxCtrl)
    -vx     Use verbose mask 'x' (in hex, see notes)

    Notes:
    -Verbose Mask bit definitions (leading 0s optional):
        00000000: No Log Messages
        00000001: Log Errors
        00000002: Log Function Entry/Exit
        00000004: Log I/O message data
        00000008: Log device general messages
        FFFFFFFF: Log everything
    - Registers name as 'mgr/IcmMgr'
    - Must be run as Super User.

#endif
*******************************************************************************/

