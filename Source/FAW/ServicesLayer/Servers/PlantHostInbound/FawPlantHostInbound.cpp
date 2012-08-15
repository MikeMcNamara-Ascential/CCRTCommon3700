//******************************************************************************
// Description:
//   This defines the FawPlantHostInbound object.
//
//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "FawPlantHostInbound.h"

//-------------------------------------------------------------------------------------------------
FawPlantHostInbound::FawPlantHostInbound() : PlantHostInbound()
{   // Nothing special to be done here
}

//-------------------------------------------------------------------------------------------------
FawPlantHostInbound::~FawPlantHostInbound()
{   // Nothing special to be done here
}

//-------------------------------------------------------------------------------------------------
void FawPlantHostInbound::UpdateInputServerState(void)
{   // Make sure to keep the input server in VIN state
	string response;
	if(m_inputServerComm.Read(GetInputServerStateTag(), response, true) == BEP_STATUS_SUCCESS)
	{
		string currentState("Unknown");
		m_inputServerComm.GetByTag(GetInputServerStateTag(), currentState, response);
		if(currentState.compare(INPUT_SERVER_VIN_STATE))
		{   // Input server is not in VIN state, send it there
			m_inputServerComm.Write(GetInputServerStateTag(), INPUT_SERVER_VIN_STATE, response, true);
			Log(LOG_DEV_DATA,"Set InputServer state to %s\n", INPUT_SERVER_VIN_STATE);
		}
	}
}
