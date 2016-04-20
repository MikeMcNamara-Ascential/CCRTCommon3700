//*************************************************************************
//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/ISystemCommunication.cpp $
// $Author: Cward $
// $Date: 2/26/04 6:33p $
//
// Description:
//   This defines the interface for system communication objects.
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
//     $Log: /Core/ServicesLayer/Library/ISystemCommunication.cpp $
// 
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004. 
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "ISystemCommunication.h"

//#include <string>

//#include "BepDefs.h"
//#include "XmlNode.h"
//#include "Debuggable.h"

	
ISystemCommunication::ISystemCommunication()
    : Debuggable(), m_type("Undefined"), m_id(-1), m_timeout(DEFAULT_TIMEOUT)
{
}

ISystemCommunication::~ISystemCommunication()
{
}

INT32 ISystemCommunication::ProcessRequestSpecial(void)
{
	return 0;
}

const std::string ISystemCommunication::GetType(void) const
{
    return(m_type);
}

const INT32 ISystemCommunication::GetId(void) const
{
    return(m_id);
}

const UINT64 ISystemCommunication::GetNanoTimeout(void)
{	// Get timeout value
    return(m_timeout);
}

const UINT32 ISystemCommunication::GetTimeout(void)
{	// Get timeout value
    return((INT32)(m_timeout/NANOSINAMILLI));
}

const std::string ISystemCommunication::GetPrependString(void)
{
    return(m_prepend);
}

const std::string ISystemCommunication::GetAppendString(void)
{
    return(m_append);
}

void ISystemCommunication::SetId(const INT32 &id) const
{
    m_id = id;
}

void ISystemCommunication::SetType(const std::string &type)
{	// Set the type
    m_type = type;
}

void ISystemCommunication::CheckId(INT32 &id)
{
	if(IsDebugOn())	printf("CheckId(%d) ", id);
	if(id == -1)
	{
		id = GetId();
		if(IsDebugOn())	printf("changing to %d", id);
	}
	if(IsDebugOn())	printf("\n");
}

void ISystemCommunication::SetTimeout(const UINT32 timeout)
{
	if(IsDebugOn())	printf("ISystemCommunication::SetTimeout(%d)\n", timeout);
	UINT64 tempTime = timeout * NANOSINAMILLI;	// Scale value
	// set the timeout time if at least the minimum or 0 (never)
	if(tempTime < DEFAULT_TIMEOUT && tempTime != 0) tempTime = DEFAULT_TIMEOUT;
	m_timeout = tempTime;						// Set timeout value
}
void ISystemCommunication::SetPrependString(const std::string &data)
{
	m_prepend = data;
}
void ISystemCommunication::SetAppendString(const std::string &data)
{
	m_append = data;
}
void ISystemCommunication::FindAndReplaceSpecialCharacter(std::string &data, char special)
{
	std::string::size_type index;	// string position index
	bool done = false;
	UINT8 tries = 5;
	do
	{   // Look for backslash and replace it and following character with one special character
		if((index = data.find('\\')) != std::string::npos)
		{
			if(IsDebugOn())	printf("data = %s\n", data.c_str());
			data = data.substr(0, index>0?index-1:0) + std::string(TO_SPECIAL(data[index+1])) + data.substr(index+1, data.size()-1);
			if(!tries--) done = true;
		}
		else done = true;
		if(IsDebugOn())	printf("index = %d, std::string::npos = %d\n", index, std::string::npos);
	}
	while(!done);
}

const string& ISystemCommunication::CommType(const string *type /*= NULL*/)
{
	if(type != NULL)  m_commType = *type;
	return m_commType;
}
