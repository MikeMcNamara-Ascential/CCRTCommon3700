//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/Status.h 3     2/22/07 11:00a Rwiersem $
//
// FILE DESCRIPTION:
//   This provides the methods to set and read the status of the class. It
//   will manage the m_status state and allow the user to check GetStatus
//   to determine the status of the class.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/Status.h $
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
// 2     10/24/03 11:58a Khykin
// Removing print statement.
//
// 1     10/07/03 11:36a Khykin
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
// 1     1/18/03 8:06a Khykin
// Initial Checkin From DCTC
//
// 2     7/31/02 11:51a Skypig13
// Promoted SetStatus() methods to public
//
// 1     7/19/01 10:15a Kh
// Status class that provides the methods to maintain a processes status.
//
//*************************************************************************
#ifndef _STATUS_H
#define _STATUS_H

#include <string>

/**
 * The Status class is responsible for managing the status of the object.  It is 
 * up to the user to decide what is the "Useable" values but the words:
 * "Started", "Running", "Terminate", "Abort", "ReloadConfig" are some of the 
 * recommended ones.
 *
 * @author Kevin Hykin
 */
class Status
{
public:
    /** 
     * <b>Description:</b><br>
     * Status class constructor. 
     * <br><p>
     */
    Status();
    
    /**
     * <b>Description:</b><br>
     * Status class constructor that initializes with a string.
     * <br><p>
     * 
     * @param status The initial status value.
     */
    Status(const std::string &status);
    
    /** 
     * <b>Description:</b><br>
     * Status class constructor that initializes with a char array.
     * <br><p>
     * 
     * @param status The initial status value.
     */
    Status(const char *status);
    
    /** 
     * <b>Description:</b><br>
     * Status class destructor. 
     * <br><p>
     */
    virtual ~Status();
    
    /**
     * <b>Description:</b><br>
     * Get the current status.
     * <br><p>
     *
     * @return The current status.
     */
    const std::string GetStatus(void);
    
    /**
     * <b>Description:</b><br>
     * Set the current status.
     * <br><p>
     *
     * @param status The new status to set.
     */
    void SetStatus(const char *status);
    
    /**
     * <b>Description:</b><br>
     * Set the current status.
     * <br><p>
     *
     * @param status The new status to set.
     */
    void SetStatus(const std::string &status);

private:
    /**  The status of the process. */
    std::string m_status;
};

#endif // _STATUS_H
