//******************************************************************************
//
// $Archive: /Working/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComException.cpp $
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
//     $Log: /Working/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComException.cpp $
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

#include "MultiComException.h"

/**
 * Default constructor
 */
MultiComTimeoutException::MultiComTimeoutException() : BepException( "MultiCom Communication timeout")
{
}

/**
 * Constructor with a specified timeout error
 * 
 * @param timeOut Amount of time we waited for the MultiCom to respond back to us (in milliseconds)
 */
MultiComTimeoutException::MultiComTimeoutException( uint32_t timeOut) : BepException()
{
	char	Buffer[255];

	sprintf( Buffer, "MultiCom Communication timeout (%d ms)", timeOut);
	
	m_description = std::string( Buffer);
}

MultiComTimeoutException::MultiComTimeoutException(const std::string &desc) : BepException( desc)
{
}
