//*************************************************************************
// Description:
//   Serial Motor interface class
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/Motor/SerialMotor.cpp $
// 
// 1     8/17/05 7:50p Cward
// Removed implementation from the header files
//
// 1     5/11/05 10:06p Cward
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:44p Bmeinke
// Initial full core integration for Ford
//
// 1     6/14/04 10:56a Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:01a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 2     1/13/04 4:43p Bmeinke
// Adapted from Motor class in order to remove serial communication
// dependency from the base Motor class
// NOTE: Any calls made to Log() MUIST be scoped to Motor::Log() due to
// multiple ILogger inheritance from ILogicalPort and Motor base classes
//
//
//*************************************************************************
// $NoKeywords: $
//============================================================================

#include "SerialMotor.h"

SerialMotor::SerialMotor(int detachState/*=PTHREAD_CREATE_DETACHED*/) : Motor(detachState), ILogicalPort()
{
}

SerialMotor::~SerialMotor(void)
{
}

const INT32 SerialMotor::Initialize(const XmlNode *config)
{
	INT32 status = EINVAL;
	Motor::Log(LOG_FN_ENTRY, "SerialMotor::Initialize()\n");

	try
	{
		// initialize the base class
		if((status = Motor::Initialize( config)) == EOK)
		{
			if((status = ILogicalPort::Initialize(config)) == true)
			{
				// Try to open the comm port
				if( OpenPort(config->getChild("Setup/LogicalPort")->getValue()) != true)
				{
					status = errno;
					Motor::Log(LOG_ERRORS, "Failed to open logical port because: %s\n", strerror(status));
				}
			}
			else Motor::Log(LOG_ERRORS, "Failed to initialize logical port because: %s\n", strerror(errno));
		}
		else Motor::Log(LOG_ERRORS, "Failed to acquire command synchronization object because: %s\n", strerror(status));
	}
	catch(BepException &Err)
	{
		Motor::Log(LOG_ERRORS, "SerialMotor Configuration Error: %s\n", Err.what());
		throw;
	}
	Motor::Log(LOG_FN_ENTRY, "SerialMotor::Initialize() done\n");

	return(status);
}

void SerialMotor::LogSerialString(const UINT32 mask, const std::string Prefix, const SerialString_t& response)
{
	Motor::Log(mask, "%s%s\n", Prefix.c_str(), (char*)response.c_str());
}

