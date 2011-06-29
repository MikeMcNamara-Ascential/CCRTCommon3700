//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/Debuggable.h 3     2/22/07 11:00a Rwiersem $
//
// FILE DESCRIPTION:
//   This provides the methods to implement debuggability in a class. It will
//   manage the m_debug status and allow the user to check IsDebugOn to enable
//   any diagnostic printouts in their class.
//
//===========================================================================
// Copyright (c) 1999-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/Debuggable.h $
// 
// 3     2/22/07 11:00a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Updated and added Javadoc commments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
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
// 1     6/12/03 4:08p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:36  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:04a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 9     5/30/02 4:13p Skypig13
// Make IsDebugOn() const
//
// 8     4/08/02 2:22p Skypig13
// Removed compiler warning on line 79 by re-ordering initializer list
//
// 7     3/25/02 6:32p Kh
// Updating to enable debugging if the level is above 0.
//
// 6     3/20/02 10:17a Dan.erickson
// Included stdlib.h so this file will compile.
//
// 5     3/19/02 3:54p Kh
// Adding levels for the evaluation of IsDebugOn().
//
// 4     8/30/01 4:15p Skypig13
// Add parameterized constructor
//
// 3     7/11/00 11:53a Skypig13
// Change IsDebugOn() to be virtual
//
// 2     6/27/01 8:36a Kh
// Adding headder information.
//
//*************************************************************************
#ifndef DEBUGGABLE_H
#define DEBUGGABLE_H

#include <string>

/**
 * Debuggable class for providing the ability to enable/disable debugging 
 * printouts.
 *
 * @author Kevin Hykin
 */
class Debuggable
{
public:
    /**
     * <b>Description:</b><br>
     * Debuggable class constructor.
     * <br><p>
     */
    Debuggable();
    
    /**
     * <b>Description:</b><br>
     * Debuggable class constructor with a boolean debug flag passed.
     * <br><p>
     * 
     * @param debug The debug enabled flag.
     */
    Debuggable(const bool debug);

    /**
     * <b>Description:</b><br>
     * Debuggable class constructor with an integer level passed.
     * <br><p>
     * 
     * @param level The debug level value.
     */
    Debuggable(const int level);
    
    /**
     * <b>Description:</b><br>
     * Debuggable class destructor.
     * <br><p>
     */
    virtual ~Debuggable();

    /**
     * <b>Description:</b><br>
     * Get the debug message status.
     * <br><p>
     *
     * @return The current debug flag.
     */
    virtual bool IsDebugOn(void) const;

    /**
     * <b>Description:</b><br>
     * Get the debug message status.
     * <br><p>
     *
     * @param level The debug level to check.
     * 
     * @return If the debug level is greater than the passed in level, return 
     *         true.  Otherwise return false.
     */
    virtual bool IsDebugOn(int level) const;

protected:
    /**
     * <b>Description:</b><br>
     * Set the status of the debug messages.
     * <br><p>
     *
     * @param value The new debug flag value.  If the value is "On", the flag 
     *              will be set to true.  Otherwise it will be set to false.
     */
    void SetDebug(const char *value);
    
    /**
     * <b>Description:</b><br>
     * Set the status of the debug messages.
     * <br><p>
     *
     * @param value The new debug flag value
     */
    void SetDebug(const bool value);
    
    /**
     * <b>Description:</b><br>
     * Set the status of the debug messages.
     * <br><p>
     *
     * @param value The new debug flag value.  If the value is "On", the flag 
     *              will be set to true.  Otherwise it will be set to false.
     */
    void SetDebug(const std::string &value);
    
    /**
     * <b>Description:</b><br>
     * Set the level of the debug messages.
     * <br><p>
     *
     * @param level The new debug level.  A value of "-1" will set the debug
     *              flag to false.
     */
    void SetDebugLevel(const char *level);
    
    /**
     * <b>Description:</b><br>
     * Set the level of the debug messages.
     * <br><p>
     *
     * @param level The new debug level.  A value of -1 will set the debug
     *              flag to false.
     */
    void SetDebugLevel(const int level);
    
    /**
     * <b>Description:</b><br>
     * Set the level of the debug messages.
     * <br><p>
     *
     * @param level The new debug level.  A value of "-1" will set the debug
     *              flag to false.
     */
    void SetDebugLevel(const std::string &level);

private:
    /** Process debug variable. */
    bool m_debug;
    
    /**
     * The level at which the amount of debugging statments should be printed 
     * out.
     */
    int m_level;
};

#endif // DEBUGGABLE_H
