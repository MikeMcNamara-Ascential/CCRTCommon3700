//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef GatewayModule_h
#define GatewayModule_h

//---------------------------------------------------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"

//---------------------------------------------------------------------------------------------------------------------
template<class ProtocolType>
class GatewayModule : public GenericModuleTemplate<ProtocolType>
{
public:

	GatewayModule();
	virtual ~GatewayModule();

	/**
	 * Determine if the ignition is in the On position.
	 * <p><b>Description:</b><br>
	 * A message will be sent to the module.  If the module responds to the message, it is assumed the ignition is in
	 * the On position.
	 * <p>
	 *
	 * @return Flag indicating if the ignition is in the On position.
	 */
	virtual bool IsIgnitionOn(void);
};

//---------------------------------------------------------------------------------------------------------------------
#endif //GatewayModule_h
