//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "GatewayModule.h"

//---------------------------------------------------------------------------------------------------------------------
template<class ProtocolType>
GatewayModule<ProtocolType>::GatewayModule() : GenericModuleTemplate<ProtocolType>()
{   // Nothing special to do here
}

//---------------------------------------------------------------------------------------------------------------------
template<class ProtocolType>
GatewayModule<ProtocolType>::~GatewayModule()
{   // Nothing special to do here
}

//---------------------------------------------------------------------------------------------------------------------
template<class ProtocolType>
bool GatewayModule<ProtocolType>::IsIgnitionOn(void)
{
	bool ignitionOn = false;
	string messageTag("IgnitionStatusMsg");
	// Get the bus broadcast message
	SerialString_t ignitionMessage;
	BEP_STATUS_TYPE status = m_protocolFilter->GetBusBroadcastMessage(messageTag, 10000, ignitionMessage);
	if(BEP_STATUS_SUCCESS == status)
	{
		ignitionOn = ParseBooleanResponse(messageTag, ignitionMessage);
	}
	else
	{
		Log(LOG_ERRORS, "IsIgnitionOn - Error reading %s: %s", messageTag.c_str(), 
			ConvertStatusToResponse(status).c_str());
	}
	return ignitionOn;
}
