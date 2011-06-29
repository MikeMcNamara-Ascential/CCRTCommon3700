//******************************************************************************
// Description:
//	Module Exception is an exception that is thrown when there is a problem
//	in a module.  It inherits from BepException, so it can be treated as one.
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
//    $Log: /Core/ApplicationLayer/ModuleInterfaces/Include/ModuleException.h $
// 
// 3     8/18/04 11:06p Cward
// changed angle brackets to quotes for includes of non system files
//
// 2     6/09/04 10:46p Cward
// Added new standard header comment block
//
// 1     12/21/03 6:32p Cward
//
// 2     10/27/03 1:31p Bmeinke
// Fixed ParkBrake / ParkPawl test failures
//
// 2     8/14/03 10:30a Khykin
// Updated with make install changes.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef MODULEEXCEPTION_H
#define MODULEEXCEPTION_H
//==============================================================================

/**
 * @author Chris TenHarmsel
 * @class ModuleException
 *
 * @date Release Date:
 * @since Feb 2003
 *
 * @brief Module Exception
 *
 * Module Exception is an exception that is thrown when there is a problem
 * in a module.  It inherits from BepException, so it can be treated as one.
 *
 * <hr>
 * @attention Copyright (c) 2003 Burke E. Porter Machinery
 * All Rights Reserved
 * @attention This file contains confidential information of Burke E. Porter
 * Machinery and is not to be used in any way detrimental to the
 * interests thereof.
 * Unauthorized use, distribution, copying, or transmittal of this
 * file in any way is strictly prohibited.
 *
 * <hr>
 *
 * $Log: /Core/ApplicationLayer/ModuleInterfaces/Include/ModuleException.h $
 * Revision 1.1  2003/05/27 13:29:15  tenharmsel
 * Moving customer specific module interfaces into a Customers directory
 *
 * Revision 1.1.1.1  2003/05/07 15:11:59  tenharmsel
 * Bringing in the GenericModuleInterfaces
 *
 * Revision 1.3  2003/04/21 17:23:59  tenharmsel
 * Updated header comments to match BepCo standard for what information needs to go in header comments
 *
 *
 */


#include <string>
#include "BepException.h"
using namespace std;

class ModuleException : public BepException
{
public:
	/**
	 * Constructs a module exception with
	 * information
	 */
	ModuleException(string message);
	
	/**
	 * Method to obtain the information from the exception
	 * Kept for backwards compatability, use Reason()
	 * instead.
	 * @return Reason for the exception
	 */
	string message();

private:
	string m_message;
};

#endif

 /*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
