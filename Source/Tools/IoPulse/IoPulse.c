//******************************************************************************
//
// $Archive: /Ccrt/Source/Tools/IoPulse/IoPulse.c $
// $Author:: Cward                                                            $
//   $Date:: 7/20/05 8:13p                                                    $
//
// Description:
//    Utility used to send a pulse code to a resource manager's I/O channel
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
//     $Log: /Ccrt/Source/Tools/IoPulse/IoPulse.c $
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     12/21/03 6:32p Cward
// 
// 1     7/08/03 1:15p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:32p Khykin
// Initial checkin
// 
// 1     6/12/03 4:43p Khykin
// Initial checkin.
// 
// 1     4/30/03 4:39p Bmeinke
// Migration to new personalized configuration management structure
// 
// 1     4/30/03 11:18a Bmeinke
// Initial checkin
// 
//******************************************************************************

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <fcntl.h>


int main( int argc, char *argv[])
{
	int ii;
	int pulseVal;
	int pulseCode;
	int ioFd;
	char devName[ _POSIX_PATH_MAX+1];

	devName[ 0] = 0;
	
	// Check the command line for float format
	opterr = 0;
	for( ii=1; ii<argc; ii++)
	{
		switch( getopt( argc, argv, "p:n:"))
		{
		case 'p':		// Send a pulse
			if( sscanf( optarg, "%d,%d", &pulseCode, &pulseVal) != 2)
			{
				pulseVal = 0;
			}
			break;
		case 'n':	
			if( optarg != NULL)
			{
				strncpy( devName, optarg, _POSIX_PATH_MAX);
			}
			break;
		case -1:
			break;
		default:
			break;
		}
	}
	
	// If a valid deive path was given
	if( devName[ 0] != 0)
	{
		// Try to open the device path
		if( (ioFd = open( devName, O_RDWR)) != -1)
		{
			// Send a pulse to the device
			MsgSendPulse( ioFd, -1, pulseCode, pulseVal);
			close( ioFd);
		}
		else
		{
			perror( "open failed");
		}
	}
	else
	{
		// Show usage
		sprintf( devName, "use %s", argv[ 0]);
		system( devName);
	}
	
	return( 0);
}


/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

Usage: 
%C -n devPath -p code,value
  Sends a pulse to the device given by 'devPath'

    -n devPath  Full path to the device manager we want to send the pulse to (i.e. /dev/Icm)
    -p	        Sends a pulse
      code      Pulse code to send to the given device
      value     Pulse value to send to the given device
	  
#endif

*******************************************************************************/

