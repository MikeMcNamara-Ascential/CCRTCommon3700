//*************************************************************************
// FILE:
//    $Header: /Core/ServicesLayer/Library/SystemCommunication/IOpenChannelIpc.cpp 2     1/16/04 4:45p Cward $
//
// FILE DESCRIPTION:
//   This defines the interface for system communication objects that will only read and write to a
//   senders communication buffer and reply to sender.
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
//    $Log: /Core/ServicesLayer/Library/SystemCommunication/IOpenChannelIpc.cpp $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:52p Khykin
// Initial checkin.
//
// 1     4/22/03 9:33a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:43  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:26a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 18    9/10/02 5:54p Tbochene
// Updated intializer to be more consistent with other communication
// classes
//
// 17    2/19/02 2:01p Skypig13
//
// 16    2/05/02 12:08a Skypig13
// Update to remove compiler warnings and restructure
//
// 15    11/01/01 3:31p Dan.erickson
// Added name to Initialize method prototype for
// IInterProcessCommunication.
//
// 14    10/25/01 2:21p Skypig13
// remove pulse stuff in initialize
//
// 13    10/22/01 5:33p Skypig13
// Added pulse code and pulse value to initialize method
//
// 12    10/19/01 3:49p Skypig13
// Made Initialze() a little quicker
//
// 11    9/05/01 4:20p Kh
// Making change to not do a second Readv for a pulse.
//
// 10    8/28/01 9:49p Skypig13
// Update initializers
//
// 8     8/27/01 5:58p Skypig13
// Update to use size parameter in initialize and parameterized
// constructor
//
// 7     7/20/01 10:04a Skypig13
// Change diagnostic prints to check if debug is on
//
// 6     7/20/01 9:25a Skypig13
// Change for consistent handling of pulses
//
// 5     6/22/00 4:52p Skypig13
// Tweaks needed for IBepCommunication
//
// 4     6/21/00 5:48p Skypig13
// Testing functionality of IPC and OpenChannel tests.
// Made some interface changes
//
// 3     6/18/00 10:51a Skypig13
// Testing complete for this phase
//
// 2     6/15/01 5:24p Skypig13
// Weekly check in
//
// 1     6/15/01 4:06p Skypig13
// Initial version
//
//
//*************************************************************************
#include <errno.h>
#include <typeinfo>
#include <unistd.h>
#include <sys/neutrino.h>

#include "Bepos.h"
#include "IOpenChannelIpc.h"

IOpenChannelIpc::IOpenChannelIpc(void) : IInterProcessCommunication()
{	// Point nowhere
}
IOpenChannelIpc::IOpenChannelIpc(const INT32 id, const std::string name,
								 const bool debug /* = false */,
								 const INT32 rid /* = -1 */,
								 const INT32 code /* = 0 */,
								 const INT32 value /* = 0 */,
								 const INT32 size /* = DEFAULT_BUFFER_SIZE */,
								 const INT32 timeout /* = DEFAULT_TIMEOUT */) :
IInterProcessCommunication()
{
	IOpenChannelIpc::Initialize(id, name, debug, rid, code, value, size, timeout);
}
IOpenChannelIpc::~IOpenChannelIpc(void)
{
}
void IOpenChannelIpc::Initialize(const INT32 id, const std::string name,
								 const bool debug /* = false */,
								 const INT32 rid /* = -1 */,
								 const INT32 code /* = 0 */,
								 const INT32 value /* = 0 */,
								 const INT32 size /* = DEFAULT_BUFFER_SIZE */,
								 const INT32 timeout /* = DEFAULT_TIMEOUT */)
{
	if(IsDebugOn())	printf("IOpenChannelIpc::Initialize(%d, %d, %d, %d, %d, %d, %d)\n",
						   id, debug, rid, code, value, size, timeout);
	IInterProcessCommunication::Initialize(id, name, debug, rid, size, timeout);
	SetPulseCode(code);
	SetPulseValue(value);
	string type = "IOC";
	CommType(&type);
}
void IOpenChannelIpc::Initialize(const XmlNode *config, const INT32 id /* =-1 */, const INT32 rid /* =-1 */)
{
	bool debug = false;
	INT32 size = DEFAULT_BUFFER_SIZE, timeout = DEFAULT_TIMEOUT;
	const XmlNodeMap nodes = config->getChildren();
	try
	{	// Set debug status
		debug = (nodes.getNode("Debug")->getValue() == "On");
	}
	catch(XmlException &XmlErr)
	{	// If node does not exist, leave set to false (off)
	}
	try
	{	// Set buffer size
		size = atoi(nodes.getNode("BufferSize")->getValue().c_str());
	}
	catch(XmlException &XmlErr)
	{	// If node does not exist, leave set DEFAULT_BUFFER_SIZE
	}
	try
	{	// Set buffer size
		timeout = atoi(nodes.getNode("Timeout")->getValue().c_str());
	}
	catch(XmlException &XmlErr)
	{	// If node does not exist, leave set DEFAULT_BUFFER_SIZE
	}
	IInterProcessCommunication::Initialize(id, nodes.getNode("Name")->getValue(), debug, rid, size, timeout);
	string type = "IOC";
	CommType(&type);
}
INT32 IOpenChannelIpc::Read(std::string &message, INT32 id /*=-1*/)
{	// Receive message
	INT32 bytes=-1;		// Number of bytes read
	try
	{
		if(id == -1) id = GetReplyId();
		if(id != 0)	// if it is not a pulse get the buffer
			bytes = IInterProcessCommunication::ReadSendersBuffer(message, 0, id);
		else bytes = BEP_STATUS_SUCCESS;
	}
	catch(...)
	{
		bytes = -1;
	}
	if(IsDebugOn())	printf("%s'%s' Read(%s)\n", GetType().c_str(), GetName().c_str(), message.c_str());
	return(bytes);
}

void IOpenChannelIpc::SetPulseCode(const INT32 code)
{
    m_pulse.code = code;
}

void IOpenChannelIpc::SetPulseValue(const INT32 value)
{
    m_pulse.value.sival_int = value;
}
