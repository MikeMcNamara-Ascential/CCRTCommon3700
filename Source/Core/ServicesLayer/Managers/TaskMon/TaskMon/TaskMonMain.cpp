//******************************************************************************
// Description:
//  Main entry point for TaskMon
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/TaskMon/TaskMon/TaskMonMain.cpp $
// 
// 1     7/20/05 8:24p Cward
// Added new TaskMon
// 
// 1     5/17/05 6:57p Bmeinke
// Core R05051101 release with TaskMon updates
// 
// 1     4/25/05 4:11p Bmeinke
// 
// 1     4/25/05 3:39p Bmeinke
// NewTaskMon task monitor
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

/*******************
 include files
 ******************/
#include "TaskMon.h"

TaskMon	*taskMon;

//==============================================================================
//==============================================================================

int main( int argc, char *argv[])
{
	int	retVal = RES_MGR_CMD_NONE;
	ILogger		logger;
	
	logger.SetProcessName( "TaskMon(main)");
	logger.EnableLogger();

	while( retVal != RES_MGR_CMD_TERMINATE)
	{
		try
		{
			//  Create PLC Manager object
			taskMon = new TaskMon( argc, argv);
			// Execute the manager message loop
			retVal = taskMon->Execute();
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
		delete taskMon;
	}
	
	return( retVal);
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

%C [-n nodeName] [-l period] [-p priority] [-k]

    -n    Node to monitor process on
    -l    Loop at the specified period (in ms, default=100ms)
    -p    Run at the specified priority
    -k    Keep dead processes on the screen (use 'r' to refresh)

#endif
*******************************************************************************/
