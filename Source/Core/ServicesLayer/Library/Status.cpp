//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Library/Status.cpp $
// $Author: Rwiersem $
// $Date: 2/22/07 11:04a $
//
// Description:
//   This provides the methods to set and read the status of the class. It
//   will manage the m_status state and allow the user to check GetStatus
//   to determine the status of the class.
//
//===========================================================================
// Copyright (c) 1999-2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/Status.cpp $
// 
// 2     2/22/07 11:04a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added a separator between the methods.
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "Status.h"

//=============================================================================
Status::Status(): m_status("Running")
{
};

//=============================================================================
Status::Status(const std::string &status): m_status(status)
{
};

//=============================================================================
Status::Status(const char *status): m_status(status)
{
};

//=============================================================================
Status::~Status()
{
};

//=============================================================================
const std::string Status::GetStatus(void)
{
    return(m_status);
};

//=============================================================================
void Status::SetStatus(const char *status)
{
    SetStatus(std::string(status));
};

//=============================================================================
void Status::SetStatus(const std::string &status)
{
    m_status = status;
};


