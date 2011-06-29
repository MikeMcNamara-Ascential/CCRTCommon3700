//****************************************************************************
//
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/SystemSetup.h 3     10/30/07 3:50p Rwiersem $
//
// DESCRIPTION:
//    Header file for the System Setup methods.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/SystemSetup.h $
// 
// 3     10/30/07 3:50p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:24p Khykin
// Initial checkin.
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:47  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:10a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 4     8/17/01 2:34p Kh
// Adding error printouts to the SIGSEGV printout and not requiring debug
// to be on to print out the error.
//
// 3     6/13/01 7:23p Admin
// Change Boolean to bool
//
// 2     5/22/01 4:13p Skypig13
// Updated to turn on debugging
//
// 1     4/10/01 10:46a Admin
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 2    11/30/00 1:00p Kevin.Hykin
// Converted to C++
//
// 1     9/21/99 2:49p Ross.wiersema
// Initial version.
//
//****************************************************************************
#ifndef SYSTEMSETUP_H
#define SYSTEMSETUP_H
//==============================================================================

#include "BepDefs.h"


#ifdef __cplusplus
extern "C"
{
    void ChildSigHandler(INT32 sigNo);              // child termination handeler
    void SignalSegmentHandler(INT32 sigNo);         // signal segment handler
    void FloatingPointErrorHandler(INT32 sigNo);    // floating point error handler         
}
#endif

/**
 * Class to set up handling of signals from the operating system
 *
 * @author Tim Bochenek
 * @version 2.0
 * @since Version 1.0
 */
class SystemSetup
{
public:
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    SystemSetup();
    /**
     * Class constructor
     *
     * @param debug Configuration to set up diagnostic print
     * @since Version 1.0
     */
    SystemSetup(const bool debug);
    /**
     * Class destructor
     * @since Version 1.0
     */
    virtual ~SystemSetup();
    /**
     * Sets up the system to respond to signals.
     * (Installs signal handlers)
     *
     * @param debug  Sets diagnostic print variable
     */
    void Initialize(const bool debug);
    /**
     * Sets up the system to respond to signals.
     * (Installs signal handlers)
     */
    void Initialize(void);
    /**
     * Gets diagnostic print varible state.
     *
     * @return State of diagnostic print
     * @since Version 2.0
     */
    static const bool IsDebugOn(void);
protected:
    /**
     * Sets the state of the diagnostic print
     *
     * @param debug  state of diagnostic print
     * @since Version 2.0
     */
    static void SetDebug(const bool debug);

private:
    /**
     * Variable controlling diagnostic printouts
     * @since Version 2.0
     */
    static bool m_debug;
};

#endif // SYSTEMSETUP_H
