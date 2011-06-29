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
//     $Log: /Core/ApplicationLayer/ModuleInterfaces/Common/ModuleException.cpp $
// 
// 3     8/18/04 11:00p Cward
// Changed angle brackets to quotes
//
// 2     6/09/04 10:40p Cward
// Added new standard header block
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ModuleException.h"

ModuleException::ModuleException(string message)
	:BepException(message)
{
	// Nothing
}

string ModuleException::message()
{ // This is from BepException
	return Reason();
}

/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */
