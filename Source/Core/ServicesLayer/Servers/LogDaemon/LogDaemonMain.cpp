//******************************************************************************
//
// $Author: Cward $
//   $Date: 4/26/06 7:01p $
//
// Description:
//
//
//==============================================================================
//
//     Copyright <A9> 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Servers/LogDaemon/LogDaemonMain.cpp $
// 
// 2     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/25/06 3:37p Rwiersem
// Updated to pass the configuration file name instead of an XmlNode
// containing the parsed configuration data.
//
// 1     12/21/03 6:41p Cward
//     Revision 1.3  2003/10/02 23:07:50  swieton
//     Output now using internal logger
//
//     Revision 1.2  2003/10/01 18:46:40  crosby
//     roll back to korea-like code
//
//
// 2     8/18/03 7:49a Khykin
// Updated with Flatrock changes.
//
// 1     8/07/03 1:52p Khykin
// Initial checkin.
//     Revision 1.6  2003/07/22 22:01:49  crosby
//     Now using -f option for config file name
//
//     Revision 1.5  2003/07/22 21:45:00  crosby
//     Rewrote to demand command-line args.
//
//     Revision 1.4  2003/07/18 21:13:26  crosby
//     Added Xml parser
//
//     Revision 1.3  2003/07/18 20:10:59  crosby
//     *** empty log message ***
//
//     Revision 1.2  2003/07/10 22:51:46  crosby
//     Added devel main
//
//     Revision 1.1  2003/07/10 19:08:02  crosby
//     Added LogDaemonMain
//
//
//==============================================================================

#include "LogDaemon.h"
#include "XmlParser.h"
#include "XmlNode.h"
#include "BepException.h"

#include <iostream.h>
#include <string>
#include <string.h>

int main( int argc, char *argv[])
{

	// parse the config file from cmd line opts
	std::string fname("");
	for(int i = 0; i < argc; i++)
	{
		if(!strcmp("-f", argv[i]))
		{
			if(i == argc-1)
			{
				fname = "";	// busted, no more vals to choose from
			}
			else
			{
				fname = string(argv[i+1]);
			}
			break;
		}
		else if(argv[i][0] == '-' && argv[i][1] == 'f')
		{
			// starts with -f but is not just -f
			fname = string(argv[i] + 2); // skip -f
		}
	}
	if(fname.empty())
	{
		LogDaemon::out("Usage: LogDaemonMain.qxe -f <config_file>\n");
		exit(1);
	}
	try
	{
		LogDaemon::out("Starting LogDaemonMain with config file: %s\n", fname.c_str());

		try
		{
			LogDaemon logger;
            logger.Initialize(fname);
			logger.Run();
		}
		catch( XmlException &xmlErr)
		{
			LogDaemon::out( "\n\n\tXmlException in "__FILE__": %s\n", xmlErr.GetReason());
		}
		catch( BepException &bepErr)
		{
			LogDaemon::out( "\n\n\tBepException in "__FILE__": %s\n", bepErr.GetReason());
		}
		catch( exception &genErr)
		{
			LogDaemon::out( "\n\n\tUnknown exception in "__FILE__": %s\n", genErr.what());
		}
		catch( ...)
		{
			LogDaemon::out( "\n\n\tUnknown exception in "__FILE__"\n");
		}
	}
	catch(BepException &e)
	{
		LogDaemon::out("LogDaemonMain Exception: %s\n", e.what());
	}

	LogDaemon::out("Ending LogDaemonMain\n");

	return(0);

}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE
%C [-ffilename.ext]
mgr/Log  Registered Name
Normal User.
	-f        Force new configuration file

  Example:
  %C -f config.dat &

#endif

*******************************************************************************/
