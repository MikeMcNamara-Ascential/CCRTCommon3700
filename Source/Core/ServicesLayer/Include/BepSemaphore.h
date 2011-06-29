//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Include/BepSemaphore.h $
// $Author: Rwiersem $
// $Date: 10/30/07 3:30p $
//
// Description:
//  Semaphore class that provides the methods necessary to ensure critical resource
//  protection in a multi-threaded system.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/BepSemaphore.h $
// 
// 4     10/30/07 3:30p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:22p Khykin
// Initial checkin.
//
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:46  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:09a Khykin
// Initial Checkin
//
// 1     1/18/03 8:06a Khykin
// Initial Checkin From DCTC
//
// 3     11/08/02 8:28a Tbochene
// Included GeneralUtilities.h to include list so it would compile
// properly
//
// 2     7/31/02 11:51a Skypig13
// Updated from pit machine
//
// 1     5/24/02 3:03p Kh
// Initial Checkin.
//
//******************************************************************************
#ifndef BEPSEMAPHORE_H
#define BEPSEMAPHORE_H

#include "Debuggable.h"
#include "BepDefs.h"

#include <semaphore.h>
#include <string>

/**
 * BepSemaphore class for guarding a resource in a
 * milti-threaded program. This Bepsemaphore class
 * enforces the count that is passed in by the
 * user.
 *
 * @author Kevin Hykin
 * @version Version 1.0
 */
class BepSemaphore : public Debuggable
{
public:
    /**
     * BepSemaphore constructor.
     *
     * @param count  The count to initialize the semaphore to.
     * @param debug  The debugging information enable flag.
     * @param name   The name of the semaphore.
     */
    BepSemaphore(int count = 1, bool debug = false, std::string name = "Blank");
    
    /**
     * BepSemaphore destructor.
     * @since Version 1.0
     */
    virtual ~BepSemaphore();

    /**
     * Signal that the critical section processing is complete.
     * @since Version 1.0
     */
    void CriticalSectionExit(void);
    /**
     * Try to enter the critical section.
     *
     * @return The status of the entry.
     *         BEP_STATUS_SUCCESS - Resource locked
     *         BEP_STATUS_FAILURE - Resource not avaliable
     */
    BEP_STATUS_TYPE CriticalSectionTryEnter(void);
    /**
     * Wait until critical section processing can be performed.
     * If a timeout is entered, it will return when the timeout
     * occurs
     *
     * @param timeout The amount of time in seconds to wait for the Bepsemaphore
     *                to be released.
     * @return The status of the operation.
     * @since Version 1.0
     */
    BEP_STATUS_TYPE CriticalSectionEnter(INT32 timeout = -1);

protected:
    /**
     * Create the Bepsemaphore.
     *
     * @param initialVal The initial value to create the Bepsemaphore with.
     * @since Version 1.0
     */
    void Create(int initialVal);
    /**
     * Destroy the Bepsemaphore.
     */
    void Destroy(void);

private:
    /**
     * The number of requests that can be made to the Bepsemaphore
     * class prior to blocking a requestor.
     * @since Version 1.0
     */
    int m_count;
    /**
     * Semaphore that controls access to the protected region
     * @since Version 1.0
     */
    sem_t *m_semaphore;
    /**
     * Name to use for debugging messages.
     * @since Version 1.0
     */
    std::string m_name;
};


#endif // BEPSEMAPHORE_H

