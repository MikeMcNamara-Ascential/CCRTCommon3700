//******************************************************************************
//
// $Archive: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tools/TOOLS/bincat/bincat.h $
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tools/TOOLS/bincat/bincat.h $
// 
// 1     11/02/07 8:08a Derickso
// Test report generator tool
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
// 1     6/12/03 4:07p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:35  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 7:03a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
// 
// 3     8/01/02 5:39p Bmeinke
// removed compiler warnings
// 
// 2     10/17/01 10:01p Brian.meinke
// Added Common VSS header
// Moved #include's from cpp file to this h file
// Moved #defines from cpp file to this h file
//
//******************************************************************************

#ifndef _BINCAT_H_INCLUDED
#define  _BINCAT_H_INCLUDED


/*******************
 include files
 ******************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/neutrino.h>
#include <sys/stat.h>

#include "Bepos.h"

#define FMT_CHAR		0x01
#define FMT_SHORT		0x02
#define FMT_LONG		0x04
#define FMT_FLOAT		0x08
#define FMT_UNSIGNED	0x80

int main( int argc, char *argv[]);


#endif	// _BINCAT_H_INCLUDED

