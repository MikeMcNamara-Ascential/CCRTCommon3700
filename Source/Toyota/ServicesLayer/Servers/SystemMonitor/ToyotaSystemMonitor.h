//*************************************************************************
// FILE DESCRIPTION:
//  The ToyotaSystemMonitor monitors the system to determine when a vehicle
//  test should start and what should be done once vehicle testing is complete.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef ToyotaSystemMonitor_h
#define ToyotaSystemMonitor_h
//-------------------------------------------------------------------------------------------------
#include "SystemMonitor.h"
//-------------------------------------------------------------------------------------------------
const bool buildDataLoaded = true;
const bool buildDataNotLoaded = false;
//-------------------------------------------------------------------------------------------------
class ToyotaSystemMonitor : public SystemMonitor
{
public:
    ToyotaSystemMonitor();
    ToyotaSystemMonitor(int argc, char *argv[]);
    virtual ~ToyotaSystemMonitor();

protected:
    /**
     * Overloaded BepServer Publish method which provides the ability
     * to handle the fault level messages that were requested.
     *
     * @param node the published information to deal with.
     * @return the status of the operation.
     */
    virtual const string Publish(const XmlNode *node);

    /**
     * Testing control logic
     *
     * @param ctrl Current value of control data needed for system control events
     */
    virtual void CheckTesting( ControlData *ctrl);

    /**
     * Test ABORT logic
     *
     * @param ctrl Current value of control data needed for system control events
     */
    virtual void CheckAbort( ControlData *ctrl);

private:
    /**
     * Get/Set the flag indicating the build data has been loaded.
     * 
     * @param loaded Flag indicating the build data has been loaded.
     * 
     * @return Flag indicating the build data has been loaded.
     */
    const bool& VehicleBuildDataLoaded(const bool *loaded = NULL);

    /** Flag used to signal vehicle build data has been loaded */
    bool m_buildDataLoaded;
};
//-------------------------------------------------------------------------------------------------
#endif //ToyotaSystemMonitor_h
