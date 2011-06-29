//******************************************************************************
//
// $Archive: /Working/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComException.h $
// $Author: Cward $
//   $Date: 5/11/05 9:39p $
//
// Description:
//    Object used to encapsulate MultiCom communication exceptions
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
//     $Log: /Working/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComException.h $
// 
// 1     5/11/05 9:39p Cward
// Initial Version
// 
// 1     4/26/05 10:02a Bmeinke
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
// 
// 1     4/12/04 1:21p Bmeinke
// Changing VSS repositories
// 
// 1     4/12/04 10:34a Bmeinke
// 
//
//******************************************************************************

#ifndef _MULTICOM_EXCEPTION_H_INCLUDED
#define _MULTICOM_EXCEPTION_H_INCLUDED

// Forward class references
class MultiComTimeoutException;

#include "BepException.h"
#include <stdint.h>
#include <vector>

/**
 * Exception thrown if the MultiCom fails to respond to a command in a
 * specified amount of time
 */
class MultiComTimeoutException : public BepException
{
public:
	/**
	 * Default constructor
	 */
	MultiComTimeoutException();

	/**
	 * Constructor with a specified timeout error
	 * 
	 * @param timeOut Amount of time we waited for the MultiCom to respond back to us (in milliseconds)
	 */
	MultiComTimeoutException( uint32_t timeOut);

	MultiComTimeoutException(const std::string &desc);
};

#endif // _MULTICOM_EXCEPTION_H_INCLUDED
