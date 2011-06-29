//****************************************************************************
// FILE:
// $Archive: /Ccrt/Source/Core/ServicesLayer/Library/BepException.cpp $
// $Author: Cward $
//   $Date: 11/11/05 4:55p $
//
// DESCRIPTION:
//    Impementation file for the BepException class.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/BepException.cpp $
// 
// 2     11/11/05 4:55p Cward
// Changes for 0511101 iteration
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
// 1     6/12/03 4:54p Khykin
// Initial checkin.
//
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:21a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:39  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:25a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 5     5/16/01 4:28p Brian.meinke
// Added GetReason() method
//
// 4     5/02/01 6:31p Skypig13
// Fix bug in constructor
//
// 4     5/01/01 3:03p Skypig13
// Remove debug statement
//
// 3     5/01/01 1:44p Skypig13
// Fix bug with variable argument constructor
//
// 2     4/26/01 1:28p Skypig13
//
// 3     3/07/01 3:11p Kevin.hykin
// changing method names to upper case
//
// 2     3/06/01 7:15p Brian.meinke
// Removed dependencies fromstd::exception due to compatability problems
// with STLPort 3.2 and greater (3.12.3 does not have std::exception)
// Moved all source back to the CPP file
//
// 1     2/22/01 5:36p Tim.bochenek
// Unit testing of Logger, Parser, and CmdLine Processor
//
// 1    02/02/01 1:03:00p Tim.Bochenek
// Initial version.
//
//****************************************************************************

#include <stdarg.h>
#include "BepException.h"
#include <sys/neutrino.h>

#ifdef _NTO_VERSION
    #if (_NTO_VERSION >= 630)
        #include <stdio.h>
        #define vsnprintf std::vsnprintf
    #endif
#endif

/**
 * Default constructor (empty reason String)
 * @since Version 1.0
 */
BepException::BepException() : m_description("")
{
}

/**
 * Constructor
 *
 * @since Version 1.0
 * @param Format   Format string to build exception description (printf-style)
 */
BepException::BepException(const char *Format, ...)
{
	INT32   Count=0;
	char	Buffer[255];
	va_list vaArgs;

	memset(Buffer, 0 , sizeof(Buffer));		// clear the buffer
	va_start(vaArgs, Format);				// format the input into a String
	// Limit the size of the message so that it fits into the buffer (else SIGSEGV)
	Count = vsnprintf(Buffer, sizeof(Buffer)-1, Format, vaArgs);
	va_end(vaArgs);
	m_description = std::string(Buffer);	// Assign reason
}

/**
 * Constructor
 *
 * @since Version 1.0
 * @param desc   Reason for exception
 */
BepException::BepException(const std::string &desc) : m_description(desc)
{
}

/**
 * Destructor
 * @since Version 1.0
 */
BepException::~BepException()
{
}

/**
 * Get reason for exception
 *
 * @return C-String reason for exception
 * @since Version 1.0
 */
const std::string &BepException::Reason() const
{
	return( m_description);
}

/**
 * Get reason for exception
 *
 * @return C-String reason for exception
 * @since Version 1.0
 */
const char* BepException::GetReason() const
{
	return(Reason().c_str());
}

/**
 * Reason for exception
 *
 * @return Reason
 * @see #Reason
 * @since Version 2.0
 */
const char* BepException::what() const
{
	return(Reason().c_str());
}

