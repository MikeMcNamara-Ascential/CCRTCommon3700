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
#ifndef FawPlantHostInbound_h
#define FawPlantHostInbound_h
//-------------------------------------------------------------------------------------------------
#include "PlantHostInbound.h"

//-------------------------------------------------------------------------------------------------
class FawPlantHostInbound : public PlantHostInbound
{
public:
	FawPlantHostInbound();
	virtual ~FawPlantHostInbound();




protected:
	/**
	 * Update the state of the InputServer if it needs to be changed.
	 */
	virtual void UpdateInputServerState(void);
};

//-------------------------------------------------------------------------------------------------
#endif //FawPlantHostInbound_h
