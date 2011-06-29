//******************************************************************************
//
// Description:
//	Exception Thrown due to a timeout in AVT processing
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Managers/CommProxy/AVT/Include/AvtTimeoutException.h $
// 
// 1     8/18/04 10:54p Cward
// Refactored AvtTimeoutException class
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef AvtTimeoutException_h
#define AvtTimeoutException_h
//==============================================================================

#include "BepException.h"	// Base class
#include <string>
#include <inttypes.h>		// uint*_t


/**
 * Exception thrown if the AVT fails to respond to a command in a
 * specified amount of time
 */
class AvtTimeoutException : public BepException
{
public:
	/**
	 * Default constructor
	 */
	AvtTimeoutException();

	/**
	 * Constructor with a specified timeout error
	 *
	 * @param timeOut Amount of time we waited for the AVT to respond back to us (in milliseconds)
	 */
	AvtTimeoutException( uint32_t timeOut);

	/**
	 * Constructor with description
	 *
	 * @param desc Description to be applied to the exception.
	 */
	AvtTimeoutException(const std::string &desc);

	/**
	 * Default Destructor
	 */
	virtual ~AvtTimeoutException();


};


#endif // AvtTimeoutException_h
