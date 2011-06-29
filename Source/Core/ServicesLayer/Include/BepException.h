//*************************************************************************
// FILE:
// $Archive: /Core/ServicesLayer/Include/BepException.h $
// $Author: Cward $
//   $Date: 12/21/03 6:27p $
//
// FILE DESCRIPTION:
//   The is the BEP exception class. It encapsulates the watcom exception
// 	 class.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Include/BepException.h $
// 
// 1     12/21/03 6:27p Cward
// 
// 2     10/24/03 11:13a Khykin
// Changing so that when compiling and VSS adds the CRLF on the end of
// lines the compiler will not complain.
// 
// 2     10/10/03 9:20a Khykin
// Updated to compile without textto.
// 
// 1     10/07/03 11:37a Khykin
// Initial checkin
// 
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
// 
// 1     6/12/03 4:06p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:34  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:03a Khykin
// Initial Checkin
// 
// 5     8/03/01 10:58p Brian.meinke
// Added 'using namespace std' (RTP v 6.1)
//
// 4     7/26/01 2:11p Brian.meinke
// Added the ThrowOsException macro to report OS errors (from library
// functions)
//
// 3     5/16/01 4:28p Brian.meinke
// Added GetReason method
//
// 2     4/26/01 1:30p Skypig13
// Update to use varibale arguments in constructor
//
// 5     3/09/01 4:39p Kevin.hykin
//
// 4     3/07/01 3:16p Kevin.hykin
// changing method names to capitals
//
// 3     3/06/01 7:15p Brian.meinke
// Removed dependencies fromstd::exception due to compatability problems
// with STLPort 3.2 and greater (3.12.3 does not have std::exception)
// Moved all source back to the CPP file
//
// 2     3/06/01 2:19p Brian.meinke
// Made BepException a derived class from std::exception
// Made all functions inline (no need for cpp file)
// Modified format of file header comment
// Modified Function comments (javadoc format)
//
// 1     2/22/01 5:34p Tim.bochenek
// Unit testing of Logger, Parser, and CmdLine Processor
//
// 1     02/01/01 1:00p Kevin.Hykin
// Initial Revision
//
//*************************************************************************
#ifndef BEPEXCEPTION_H
#define BEPEXCEPTION_H

#include <string>
#include <libgen.h>
#include <exception>

#include "BepDefs.h"

using namespace std;

/**
 * Throws a BepException for OS errors.
 *
 * @param errNo     OS error number (int)
 * @param prefixStr String to prepend to the OS error string (const char*)
 */
// NOTE: This is on the same line because the compiler does not like #define continuations using / after it has been checked into VSS and the CRLF has been added
#define ThrowOsException( errNo, prefixStr) {char lineStr[ 256]; itoa( __LINE__, lineStr, 10); string errStr( basename(__FILE__)); errStr += string(", Line ") + string(lineStr) + string(": "); errStr += string( prefixStr); errStr += string( strerror( errNo)); throw( BepException( errStr)); }

/**
 * Bep Exception class
 *
 * @author Tim Bochenek
 * @version 2.0
 * @since Version 1.0
 */
class BepException : public exception
{
public:
	/**
	 * Default constructor (empty reason String)
	 * @since Version 1.0
	 */
	BepException();
	/**
	 * Constructor
	 *
	 * @since Version 1.0
	 * @param Format   Format string to build exception description (printf-style)
	 */
	BepException(const char *Format, ...);
	/**
	 * Constructor
	 *
	 * @since Version 1.0
	 * @param desc   Reason for exception
	 */
	BepException(const std::string &desc);
	/**
	 * Destructor
	 * @since Version 1.0
	 */
	virtual ~BepException();
	/**
	 * Get reason for exception
	 *
	 * @return C-String reason for exception
	 * @since Version 1.0
	 */
	virtual const std::string &Reason() const;
	/**
	 * Get reason for exception
	 *
	 * @return C-String reason for exception
	 * @since Version 1.0
	 */
	virtual const char *GetReason() const;
	/**
	 * Assignmet operator
	 *
	 * @param other  Object to copy
	 * @return new object
	 * @since Version 1.0
	 */
	BepException& operator= (const BepException& other);
	/**
	 * Reason for exception
	 *
	 * @return Reason
	 * @see #Reason
	 * @since Version 2.0
	 */
	virtual const char* what() const;
protected:
	/**
	 * Textual description of fault
 	 * @since Version 1.0
	 */
	std::string m_description;
};
#endif	// BEPEXCEPTION_H
