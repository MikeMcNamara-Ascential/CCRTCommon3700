//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/Power/Include/HVBBackgroundComponent.h 2     1/10/08 9:01a Derickso $
//
// FILE DESCRIPTION:
//	High Voltage Battery Test Component Background Thread to allow component to run in background.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// 
//*************************************************************************
#include "BackgroundTestComponent.h"

// Forward declaration of the IsuzuDeltaEmissionsTc class for the compiler
template <class ModuleType>
class IsuzuDeltaEmissionsTc;

template <class ModuleType> 
class DeltaThrottleBackgroundComponent : public BackgroundTestComponent
{
public:
    /**
     * Class constructor.<br>
     * The pointer to the instantiating test component will be saved to a member variable.
     * 
     * @param testComponent
     *               Pointer to the test component that instantiated this object.
     */
    DeltaThrottleBackgroundComponent(IsuzuDeltaEmissionsTc<ModuleType> *testComponent);
    /**
     * Class destructor.
     */
    virtual ~DeltaThrottleBackgroundComponent();
    /**
     * Monitor vehicle speed and time.
     * <p><b>Description:</b><br>
     * The test component will be used to monitor the vehicle speed and time data to ensure the speed above time
     * requirement is satisfied.  The test component will then be called to set the TPM complete flag to the
     * appropriate state.
     * <p>
     * <b>Test Component Functions:</b>
     *      <ul>
     *      <li> MonitorVehicleSpeedAndTime() </li>
     *      <li> SetTPMComplete() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS
     */
	virtual BEP_STATUS_TYPE RunBackgroundComponent();
	/**
     * Wait for the background processing to be completed.
     * <p><b>Description:</b><br>
     * The background component will wait until the thread signals that it has completed execution and is safe to be 
     * deleted.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BackgroundTestComponent::WaitUntilDone() </li>
     *      </ul>
	 */
	virtual INT32 WaitUntilDone(void);
	
protected:
    /**
     * Pointer to the test component that instantiated this object.
     * 
     * @see IsuzuEmissionsTc
     */
    IsuzuDeltaEmissionsTc<ModuleType> *m_testComponent;
};
