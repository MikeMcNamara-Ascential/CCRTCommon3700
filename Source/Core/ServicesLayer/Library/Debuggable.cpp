//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/Debuggable.cpp 3     2/22/07 11:03a Rwiersem $
//
// FILE DESCRIPTION:
//   This provides the methods to implement debuggability in a class. It will
//   manage the m_debug status and allow the user to check IsDebugOn to enable
//   any diagnostic printouts in their class.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/Debuggable.cpp $
// 
// 3     2/22/07 11:03a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added a separator between the methods.
// 
// 2     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//*************************************************************************
#include <stdlib.h>
#include "Debuggable.h"

//=============================================================================
Debuggable::Debuggable(): m_debug(false), m_level(0)
{
}

//=============================================================================
Debuggable::Debuggable(const bool debug): m_debug(debug), m_level(0)
{
}

//=============================================================================
Debuggable::Debuggable(const int level): m_debug(true), m_level(level)
{
}

//=============================================================================
Debuggable::~Debuggable()
{
}

//=============================================================================
bool Debuggable::IsDebugOn(void) const
{
    return(m_debug);
}

//=============================================================================
bool Debuggable::IsDebugOn(int level) const
{   
    // if the debugging level is greater than the passed in level return true
    return(m_level >= level);
}

//=============================================================================
void Debuggable::SetDebug(const std::string &value)
{
    // if its not a null string
    if (value.length() > 0)
    {
        if(value == "On")
        {
            SetDebug(true);
        }
        else
        {
            SetDebug(false);
        }
    }
}

//=============================================================================
void Debuggable::SetDebug(const char *value)
{
    SetDebug(std::string(value));
}

//=============================================================================
void Debuggable::SetDebug(const bool value)
{
    m_debug = value;
}

//=============================================================================
void Debuggable::SetDebugLevel(const char *value)
{
    SetDebugLevel(std::string(value));
}

//=============================================================================
void Debuggable::SetDebugLevel(const int value)
{
    m_level = value;    
    if(m_level > 0) m_debug = true;
    else            m_debug = false;
}

//=============================================================================
void Debuggable::SetDebugLevel(const std::string &value)
{
    if (value.length() > 0)
    {
        int tmp = atoi(value.c_str());
        bool go = false;

        // if the integer and string values are 0
        if((tmp == 0) && (value == "0")) go = true;
        // if the integer and string values are -1
        else if ((tmp == -1) && (value == "-1")) go = true;
        // if the integer value is not 0 or -1
        else if((tmp != 0) && (tmp != -1)) go = true;

        if(go)
        {
            SetDebugLevel(tmp); 
        }
    }
}

