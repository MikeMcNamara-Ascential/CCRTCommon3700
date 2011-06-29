//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Tests/CommProxyTest.cpp $
// $Author: Cward $
//   $Date: 4/13/06 4:02a $
//
// Description:
//	Serial communication proxy test application
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Tests/CommProxyTest.cpp $
// 
// 2     4/13/06 4:02a Cward
// Cleanup
//
// 2     2/15/06 4:08p Bmeinke
// Removed the $Revision: 2 $ and $Date: 4/13/06 4:02a $ strings from the __USAGE block
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:34a Khykin
// Initial checkin
//
// 1     8/07/03 1:18p Khykin
// Initial checkin.
//
// 1     6/12/03 4:42p Khykin
// Initial checkin.
//
// 1     4/30/03 4:55p Bmeinke
// Migration to new personalized configuration management structure
//
// 1     3/24/03 12:38p Bmeinke
// Initial checkin
//
//
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>

void ParseCommandLine( int argc, char *argv[]);

char	portName[ _POSIX_PATH_MAX+1];
int		portFd;
int		loopCount = 1;

int main( int argc, char *argv[])
{
	int 			loops;
	const uint8_t	rawMssg = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

	ParseCommandLine(argc,argv);

	// Make sure port name specified
	if( strlen( portName) > 0)
	{
		// Open desired port
		if( (portFd=open( portName.c_str(), O_RDWR)) != -1)
		{
			printf( "Port %s opened\n", portName.c_str());

			// Loop the specified numebr of times
			for( loops=0; loops<loopCount; loops++)
			{
				printf( "Sending data\n");
				if( write( portFd, rawmssg, sizeof( rawMssg)) > 0)
				{
					printf( "Data sent\n");
				}
				else
				{
					perror( "Error sending message");
					delay( 1000);
				}
			}
		}
		else
		{
			printf( "Error opening port %s: %s\n", portName, strerror( errno));
		}
	}
	return( 0);
}


void ParseCommandLine( int argc, char *argv[])
{
	char optId;

	// Read port name from command line
	/////////////////////////////////////
	// Do not print errors for unknown arguments
	opterr = 0;
	// Start reparsing from the first argument
	optind = 1;

	while( optind < argc)
	{
		// Clear aoption argument string
		optarg = NULL;
		// Get next command line option
		optId = getopt( argc, argv, "n:p:");
		switch( optId)
		{
		case 'p':		// Client side port name to use
			if( optarg != NULL)
			{
				strncpy( portName, optarg, _POSIX_PATH_MAX);
			}
			break;
		case 'n':
			if( optarg != NULL)
			{
				loopCount = atoi( optarg);
			}
			break;
		default:
			if( optId == '?')						optind++;
			else if(argv[ optind] == NULL)			optind++;
			else if(argv[optind][0] != '-')	   		optind++;
			else if((argv[ optind][0] == '-') && (strlen(argv[ optind]) == 1)) optind++;
			break;
		}
	}
}



/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

Comm Proxy Test Application

%C -ffile -pport -tTag -nLoops &

    -p    Use 'port' as logical port name to attach to
    -n    Loop this many times

#endif
*******************************************************************************/

