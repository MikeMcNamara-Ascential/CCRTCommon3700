//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/IThread.h $
// $Author: Cward $
//   $Date: 12/21/03 6:27p $
//
// Description:
//  Thread interface class
//
//==============================================================================
//
//     Copyright © 2002 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Core/ServicesLayer/Include/IThread.h $
// 
// 1     12/21/03 6:27p Cward
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
// 1     6/12/03 4:15p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:40  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 7:06a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
// 
// 5     5/30/02 4:21p Skypig13
// Added constructor and destructor and header
// 
//******************************************************************************

#ifndef IThread_h
#define IThread_h

#include "BepDefs.h"

struct ThreadPoolInfo;

/**
 * Interface class to define the behavior of a class that
 * is threaded
 *
 * @author Tim Bochenek
 * @version 1.0
 * @since Version 1.0
 */
class IThread
{
public:
	/**
	 * Class Constructor (default)
	 * @since Version 1.0
	 */
	IThread(void)
	{
	}
	/**
	 * Class Destructor
	 * @since Version 1.0
	 */
	virtual ~IThread(void)
	{
	}
	/**
	 * Method called upon creation threads.
	 * @param param
	 * @since Version 1.0
	 */
	virtual void Allocate(ThreadPoolInfo *param) = 0;
	/**
	 * Method called to block inside thread.
	 * @since Version 1.0
	 */
	virtual void Block(ThreadPoolInfo *param) = 0;
	/**
	 * Method called when thread unblocks to do work.
	 * @since Version 1.0
	 */
	virtual INT32 Handle(ThreadPoolInfo *param) = 0;
	/**
	 * Method called upon destruction threads.
	 * @since Version 1.0
	 */
	virtual void Free(ThreadPoolInfo *param) = 0;
};

#endif // IThread_h
