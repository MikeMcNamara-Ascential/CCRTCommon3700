// ****************************************************************************
//
// FILE:
//    $Header: /Core/ServicesLayer/Library/SystemSetup.cpp 2     1/16/04 4:45p Cward $
//
// DESCRIPTION:
//    System Setup methods.  These methods will provide the necessary setup to
//    run under the current OS.  This class should be extended to add any
//    additional functionality necessary.
//
//============================================================================
//
//    Copyright (c) 1999 Burke E. Porter Machinery
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//
// LOG:
//    $Log: /Core/ServicesLayer/Library/SystemSetup.cpp $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:52p Khykin
// Initial checkin.
//
// 1     4/22/03 9:33a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:44  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:26a Khykin
// Initial Checkin
//
// 1     1/18/03 7:58a Khykin
// Initial Checkin From DCTC
//
// 11    2/05/02 8:53p Skypig13
// Added abort to SIGSEGV handler
//
// 10    2/05/02 12:10a Skypig13
// Updated to remove compiler warnings
//
// 9     11/29/01 9:58p Brian.meinke
// Added printout of thread id to SIGSEGV handler
//
// 8     10/02/01 2:46p Skypig13
// Fixed spelling in debug
//
// 7     10/02/01 1:03p Skypig13
// Add process id to printout in SIGSEGV handler
//
// 6     8/22/01 7:39a Kh
// Updating the default constructor and the printout from the SIGSEGV
// handler.
//
// 5     6/13/01 7:23p Admin
// Change Boolean to bool
//
// 4     5/22/01 4:13p Skypig13
// Update to enable prints
//
// 3     5/02/01 6:27p Skypig13
// Fix bug in header
//
// 1     2/22/01 5:33p Tim.bochenek
// Unit testing of Logger, Parser, and CmdLine Processor
//
// 2     11/30/00 1:00p Kevin.Hykin
// Converted to C++
//
// 1     9/21/99 2:48p Ross.wiersema
// Initial version.
//
// ****************************************************************************

#include <signal.h>

#include "BepException.h"

#include "Bepos.h"
#include "SystemSetup.h"
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif

#include <pthread.h>


bool SystemSetup::m_debug=false;
SystemSetup::SystemSetup()
{
	Initialize(false);
}
SystemSetup::SystemSetup(const bool debug)
{
	Initialize(debug);
}
SystemSetup::~SystemSetup()
{
}
void SystemSetup::Initialize(const bool debug)
{
	SetDebug(debug);
	Initialize();
}
void SystemSetup::Initialize(void)
{
    if(IsDebugOn())	printf("SystemSetup Init\n");
	if( signal (SIGHUP, SIG_IGN) == SIG_ERR)
		throw BepException("Error: Cant Install SIGHUP Handeler");
	if( signal (SIGINT, SIG_IGN) == SIG_ERR)
		throw BepException("Error: Cant Install SIGINT Handeler");
	if( signal (SIGCHLD, &ChildSigHandler) == SIG_ERR)
		throw BepException("Error: Cant Install SIGCHLD Handeler");
	if( signal (SIGSEGV, &SignalSegmentHandler) == SIG_ERR)
		throw BepException("Error: Cant Install SIGSEGV Handeler");
	if( signal (SIGFPE, &FloatingPointErrorHandler)  == SIG_ERR)
		throw BepException("Error: Cant Install SIGFPE Handeler");
}

const bool SystemSetup::IsDebugOn(void)
{
    return(m_debug | DEBUG);
}
/**
 * Sets the state of the diagnostic print
 *
 * @param debug  state of diagnostic print
 * @since Version 2.0
 */
void SystemSetup::SetDebug(const bool debug)
{
    m_debug = debug;
}


/**
 * Handler function to respond to child signals
 *
 * @param sigNo  Identifier of signal that was raised
 */
void ChildSigHandler(INT32 sigNo)
{
	pid_t pid;
	INT32 status;

	if(SystemSetup::IsDebugOn()) printf("SystemSetup ChildSigHandler\n");;

	if(sigNo == SIGCHLD)
	{
		pid = waitpid(0, &status, 0);
		if((WIFEXITED( status ) == 0 ) && (DEBUG))
			printf("Child Done: %d\n", pid);
	}
	else if(SystemSetup::IsDebugOn()) printf("Signal Received: %d\n", sigNo);

	if(SystemSetup::IsDebugOn()) printf("SystemSetup ChildSigHandler Complete\n");
}
/**
 * Handler function to respond to segment violations (memory faults)
 *
 * @param sigNo  Identifier of signal that was raised
 */
void SignalSegmentHandler(INT32 sigNo)
{
	printf("Signal Segment Handler Complete in process with id = %d, tid %d.\nError: %s\nExiting with return value of 3\n", BposGetMyTaskId(), pthread_self(), strerror(errno));
	abort();
	exit(3);
}
/**
 * Handler function to respond to floating point errors
 *
 * @param sigNo  Identifier of signal that was raised
 */
void FloatingPointErrorHandler(INT32 sigNo)
{
	printf("Floating Point Handler Complete.  Exiting with return value of 4\n");
	exit(4);
}

