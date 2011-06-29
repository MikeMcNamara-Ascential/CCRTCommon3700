//******************************************************************************
//
// $Archive: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tools/TOOLS/bincat/bincat.cpp $
// $Author: Derickso $
//   $Date: 11/02/07 8:08a $
//
// Description:
//    Binary 'cat' utility. Used for displaying Hardware I/O Manager data from
//		the command line
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tools/TOOLS/bincat/bincat.cpp $
// 
// 1     11/02/07 8:08a Derickso
// Test report generator tool
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     12/21/03 6:32p Cward
// 
// 1     7/08/03 1:14p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:32p Khykin
// Initial checkin
// 
// 1     6/12/03 4:43p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:53a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:04:52  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 6:28a Khykin
// Initial Checkin
// 
// 1     1/18/03 7:59a Khykin
// Initial Checkin From DCTC
// 
// 13    10/24/02 8:14p Bmeinke
// Removed various debug printf statements
// 
// 12    8/01/02 5:35p Bmeinke
// Removed compiler warnings
// 
// 11    2/27/02 3:06p Bmeinke
// Added -a command line option to dump ICM data arrays
// General cleanup
// 
// 10    2/19/02 8:10p Bmeinke
// Modified to allow data to be read in 8, 16 or 32 bit chunks (readSize)
// Default value for readSize based on output format
// 
// 9     1/11/02 2:42a Brian.meinke
// Updated the 'use' message
// 
// 8     11/07/01 12:44p Brian.meinke
// Added FMT_LONG to default output format
// 
// 7     10/31/01 2:51p Brian.meinke
// Added command line option '#' to prefix channel number to data output
// 
// 6     10/18/01 11:42a Brian.meinke
// Moved #include's and #define's to bincat.h
// 
// 5     8/31/01 5:17p Brian.meinke
// Break out of loop if read() fails
//
// 4     7/20/01 3:16p Brian.meinke
// Changed format flag values
// Added ability to view data as 16-bit ints
//
// 3     5/16/01 3:08p Brian.meinke
// Added ability to loop repeatedly at a specifed period
// Read all data in one block and read from buffer as needed
//
// 2     5/11/01 12:35p Brian.meinke
// Added ability to view data as 8-bit integer (signed or unsigned)
//
// 1     4/10/01 10:45a Admin
//
// 2     3/07/01 5:15p Brian.meinke
// Changed all method names to start with capital letters to comply with
// BEP standard
//
// 1     2/15/01 11:25a Brian.meinke
// Binary 'cat' utility. Used to display I/O manager data from the command
// line
//
//******************************************************************************

/*******************
 include files
 ******************/
#include "bincat.h"
void Read32( int file, int start, int count);
void Read16( int file, int start, int count);
void Read8( int file, int start, int count);
void ReadArray( int file, int start, int count);

bool	useChnlIdx=false;
int		outFmt=0;

int main( int argc, char *argv[])
{
	int		file, argIdx=argc-1, ii;
	timer_t	scanTimer;
	int		scanPrd=0;
	int		chId, coId;
	long	start = 0, count = -1;
	struct _pulse rxPulse;
	int		readSize=-1;
	bool	readArray=false;

	// Check the command line for float format
	opterr = 0;
	for( ii=1; ii<argc; ii++)
	{
		switch( getopt( argc, argv, "#acfulb:s:n:r:t:"))
		{
		case 'a':	readArray = true;			break;
		case 'f':	outFmt |= FMT_FLOAT;		break;
		case 'u':	outFmt |= FMT_UNSIGNED;		break;
		case 'c':	outFmt |= FMT_CHAR;			break;
		case 's':	outFmt |= FMT_SHORT;		break;
		case 'b':	start = atol( optarg);		break;
		case 'n':	count = atol( optarg);		break;
		case 'l':	outFmt |= FMT_LONG;			break;
		case 't':	scanPrd = atol( optarg);	break;
		case '#':	useChnlIdx = true;			break;
		case 'r':	readSize = atol( optarg);	break;
		case -1:								break;
		default:								break;
		}
	}

	// If no format given
	if( outFmt == 0)
	{
		// Print raw bytes
		outFmt |= FMT_UNSIGNED | FMT_LONG;
	}
	
	if( readSize == -1)
	{
		if( outFmt & FMT_CHAR)	readSize = 8;
		if( outFmt & FMT_SHORT)	readSize = 16;
		if( outFmt & FMT_LONG)	readSize = 32;
		if( outFmt & FMT_FLOAT)	readSize = 32;
	}

	// If no count given, read all
	if( count == -1)
	{
		struct stat	fStat;
		stat( argv[ argIdx], &fStat);
		count = (fStat.st_size/( readSize/8)) - start;
	}
	
	// Open the file
	if( (file = open( argv[ argIdx], O_RDONLY )) == -1)
	{
		printf(" Cannot open file '%s': %s\n", argv[ argIdx], strerror( errno));
		exit( errno);
	}
	
	// If a scan period was specified
	if( scanPrd > 0)
	{
		// Create a periodic pulse to use within our loop
		scanTimer = BposCreateTimerWithPulse( &chId, &coId, 0);
		// If timer is valid
		if( scanTimer != -1)
		{
			// Start the timer
			BposStartTimer( scanTimer, scanPrd, 0);
		}
	}
	
	do
	{
		if( readArray == true)
		{
			scanPrd = 0;
			ReadArray(file, start, count);
		}
		else
		{
			switch( readSize)
			{
			case 8:		Read8( file, start, count);			break;
			case 16:	Read16( file, start, count);		break;
			default:	Read32( file, start, count);		break;
			}
		}
		
		
		// If a loop time was specified
		if( scanPrd > 0)
		{
			// Wait for the next timer tick and re-read
			if( BposReceive( chId, (char*)&rxPulse, sizeof( rxPulse)) == -1)
			{
				perror( "Receive error");
				break;
			}
			// Add a blank line before printing next block of data
			//printf("\n");
			fflush( stdout);

			// If timer ID is valid
			if( scanTimer != -1)
			{
				// Start another scan
				BposStartTimer( scanTimer, scanPrd, 0);
			}
		}
	}
	while( scanPrd != 0);

	BposDeleteTimerWithPulse( scanTimer, chId, coId);
	close( file);

 	return( 0);
}

void Read32( int file, int start, int count)
{
	int		readCount;
	long	*rxBuff;
	char 	*cBuff;
	float	*fBuff;
	short	*sBuff;
	register int ii;
	
//	printf( "Read32( %d, %d, %d)\n", file, start, count);
	// Allocate buffer to read data into
	rxBuff = new long[ count];
	
	// Move to desired start point
	if( lseek( file, start*4, SEEK_SET) == -1)
	{
		perror( "lseek failed");
		exit( 1);
	}

	// Clear buffer
	memset( rxBuff, 0, 4*count);

	// Read data in one big block
	if( (readCount=read( file, rxBuff, count * 4)) != -1)
	{
//		printf( "Read %d bytes\n", readCount);
		for( ii=0; ii<count; ii++)
		{
			// Use pointers for easy type conversions
			cBuff = (char*)&(rxBuff[ ii]);
			fBuff = (float*)&(rxBuff[ ii]);
			sBuff = (short*)&(rxBuff[ ii]);

			if( useChnlIdx == true)
			{
				printf( "%3d. ", ii);
			}
			// If output in unsigned
			if( outFmt & FMT_UNSIGNED)
			{
				if( outFmt & FMT_LONG)		printf("$%08X\t", (uint32_t)rxBuff[ ii] );
				if( outFmt & FMT_SHORT)		printf("$%04X $%04X\t", sBuff[ 0] & 0xFFFF, sBuff[ 1] & 0xFFFF);
				if( outFmt & FMT_CHAR)		printf("$%02X $%02X $%02X $%02X\t",
												   cBuff[ 0] & 0xFF, cBuff[ 1] & 0xFF,
												   cBuff[ 2] & 0xFF, cBuff[ 3] & 0xFF);
			}
			else if( outFmt & FMT_FLOAT)
			{
				printf("%7.3f\t", *fBuff);
			}
			else
			{
				if( outFmt & FMT_LONG)		printf("%ld\t", rxBuff[ ii] );
				if( outFmt & FMT_SHORT)		printf("%d %d\t", sBuff[ 0], sBuff[ 0]);
				if( outFmt & FMT_CHAR)		printf("%d %d %d %d\t",  cBuff[ 0],
												   cBuff[ 1], cBuff[ 2], cBuff[ 3]);
			}

			printf("\n");
			fflush( stdout);
		}
	}
	else
	{
		perror( "read failed");
	}
	
	delete[] rxBuff;
}

void Read16( int file, int start, int count)
{
	short	*rxBuff;
	char 	*cBuff;
	float	*fBuff;
	short	*sBuff;
	register int ii;
	
	// Allocate buffer to read data into
	rxBuff = new short[ count];
	
	// Move to desired start point
	lseek( file, start*2, SEEK_SET);

	// Clear buffer
	memset( rxBuff, 0, 2*count);

	// Read data in one big block
	if( read( file, rxBuff, count * 2) != -1)
	{
		for( ii=0; ii<count; ii++)
		{
			// Use pointers for easy type conversions
			cBuff = (char*)&(rxBuff[ ii]);
			fBuff = (float*)&(rxBuff[ ii]);
			sBuff = (short*)&(rxBuff[ ii]);

			if( useChnlIdx == true)
			{
				printf( "%3d. ", ii);
			}
			// If output in unsigned
			if( outFmt & FMT_UNSIGNED)
			{
//				if( outFmt & FMT_LONG)		printf("$%08X\t", rxBuff[ ii] );
				if( outFmt & FMT_SHORT)		printf("$%04X\t", sBuff[ 0] & 0xFFFF );
				if( outFmt & FMT_CHAR)		printf("$%02X $%02X\t",
												   cBuff[ 0] & 0xFF, cBuff[ 1] & 0xFF);
			}
			else if( outFmt & FMT_FLOAT)
			{
				printf("%7.3f\t", *fBuff);
			}
			else
			{
  //  			if( outFmt & FMT_LONG)		printf("%ld\t", rxBuff[ ii] );
				if( outFmt & FMT_SHORT)		printf("%d\t", sBuff[ 0] );
				if( outFmt & FMT_CHAR)		printf("%d %d\t",  cBuff[ 0],
												   cBuff[ 1]);
			}

			printf("\n");
			fflush( stdout);
		}
	}
	else
	{
		perror( "read failed");
	}
	
	delete[] rxBuff;
}

void Read8( int file, int start, int count)
{
	char	*rxBuff;
	char 	*cBuff;
	float	*fBuff;
	short	*sBuff;
	register int ii;
	
	// Allocate buffer to read data into
	rxBuff = new char[ count];
	
	// Move to desired start point
	lseek( file, start, SEEK_SET);

	// Clear buffer
	memset( rxBuff, 0, count);

	// Read data in one big block
	if( read( file, rxBuff, count) != -1)
	{
		for( ii=0; ii<count; ii++)
		{
			// Use pointers for easy type conversions
			cBuff = (char*)&(rxBuff[ ii]);
			fBuff = (float*)&(rxBuff[ ii]);
			sBuff = (short*)&(rxBuff[ ii]);

			if( useChnlIdx == true)
			{
				printf( "%3d. ", ii);
			}
			// If output in unsigned
			if( outFmt & FMT_UNSIGNED)
			{
				if( outFmt & FMT_CHAR)		printf("$%02X", cBuff[ 0] & 0xFF);
			}
			else
			{
				if( outFmt & FMT_CHAR)		printf("%d",  cBuff[ 0]);
			}

			printf("\n");
			fflush( stdout);
		}
	}
	else
	{
		perror( "read failed");
	}
	
	delete[] rxBuff;
}

void ReadArray( int file, int start, int count)
{
	register int ii;//, count=end-start+1;
	typedef struct
	{
		float lf;
		float rf;
		float lr;
		float rr;
	} ForceSample;

	ForceSample	*array;
	int			readCount = count*sizeof( ForceSample);

	array = new ForceSample[ count];
	memset( array, 0, readCount);

	// Move to desired start point
	lseek( file, start*16, SEEK_SET);

	// Read data in one big block
	if( (readCount=read( file, array, readCount)) != -1)
	{
//		printf( "Read %d bytes\n", readCount);
		printf( "        LF       RF        LR         RR\n");
		for( ii=0; ii<count; ii++)
		{
			if( useChnlIdx == true)	printf( "%5d. ", ii+start);
			else					printf("       ");
			
			printf("%+8.3f %+8.3f %+8.3f %+8.3f\n", array[ ii].lf, array[ ii].rf,
				   array[ ii].lr, array[ ii].rr);
			fflush( stdout);
		}
	}
	else
	{
		perror( "read failed");
	}
	
	delete[] array;

}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

Binary cat utility
'cat's binary resource manager data from the given path to the active console in ascii format.

%C [-f] [-l] [-u] [-sx] [-ny] [-tz] [-r size] path

    -a    Dump array data for 'path'
    -f    Output in floating point format
    -u    Output ints/chars in hex format (requires l or c option)
    -l    Output as 32-bit ints
    -s    Output as 16-bit ints
    -c    Output as 8-bit ints
    -bx   Begin at offset 'x' (1=4 bytes, etc)
    -ny   Read this many entities
    -tz   Loop using a period of z milliseconds
    -#    Prefix output with channel numbers
    -r    Read entities as this size (bit count)

#endif

*******************************************************************************/

