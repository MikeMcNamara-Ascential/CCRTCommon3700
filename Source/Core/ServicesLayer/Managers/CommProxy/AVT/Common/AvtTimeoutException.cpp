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
//    $Log: /Core/ServicesLayer/Managers/CommProxy/AVT/Common/AvtTimeoutException.cpp $
// 
// 1     8/18/04 10:54p Cward
// Refactored AvtTimeoutException class
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "AvtTimeoutException.h"
#include <stdio.h>


AvtTimeoutException::AvtTimeoutException() : BepException( "AVT Communication timeout")
{
}

AvtTimeoutException::AvtTimeoutException( uint32_t timeOut) : BepException()
{
	char	Buffer[255];

	sprintf( Buffer, "AVT Communication timeout (%d ms)", timeOut);
	
	m_description = std::string( Buffer);
}

AvtTimeoutException::AvtTimeoutException(const std::string &desc)  : BepException( desc)
{
}

AvtTimeoutException::~AvtTimeoutException()
{
}

