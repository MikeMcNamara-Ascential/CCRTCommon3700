//******************************************************************************
// Description:
//	This class provides the common functionality needed by all background
//	test components.
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
//     $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Common/BackgroundTestComponent.cpp $
// 
// 3     5/08/06 7:06p Cward
// Removed information printfs
//
// 2     9/01/04 8:58p Cward
// Added Wait until done function.
//
// 1     6/09/04 11:49p Cward
// Refactored App layer includes to remove implementation
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif

#include "BackgroundTestComponent.h"

#include "GeneralUtilities.h"
#include "Bepos.h"


BackgroundTestComponent::BackgroundTestComponent() : AtomicThread()
{	// Nothing to do here
}

BackgroundTestComponent::~BackgroundTestComponent(void)
{	// Nothing special to be done
}

void* BackgroundTestComponent::Execute(void)
{	// Keep sending the message until told to stop
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

	status = RunBackgroundComponent();

	// Determine the status of the send
	if (BEP_STATUS_SUCCESS != status)
	{	// Message was not sent properly
		printf("Error monitoring module - status: %s\n", ConvertStatusToResponse(status).c_str());
	}
	// Wait for the thread who terminated us to see our terminated status
	BposSleep( 250);
	// Let the instantiating object know the thread is complete
	return NULL;
}

INT32 BackgroundTestComponent::WaitUntilDone(void)
{
	INT32       err;
	if ((err = pthread_join(GetInitialThreadId(),NULL)) != EOK)
	{
		printf("\n\t*********************************************************************************************\n");
		printf("\t Thread Join failed or thread no longer exists - Thread Number = %d\n", GetInitialThreadId());
		printf("\n\t*********************************************************************************************\n\n");
	}
	return( err);
}

