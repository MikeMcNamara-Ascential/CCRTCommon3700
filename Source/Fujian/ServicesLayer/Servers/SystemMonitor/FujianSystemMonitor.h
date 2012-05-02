//*************************************************************************
// FILE DESCRIPTION:
//  System monitor for Fujian.  The system monitor will determine when
//  conditions are correct for starting a vehicle test as well as determining
//  when a test has completed and handle the system cleanup.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//*************************************************************************
#ifndef FujianSystemMonitor_h
#define FujianSystemMonitor_h
//-------------------------------------------------------------------------------------------------
#include "SystemMonitor.h"
//-------------------------------------------------------------------------------------------------
class FujianSystemMonitor : public SystemMonitor
{
public:
    FujianSystemMonitor();
    FujianSystemMonitor(int argc, char *argv[]);
    virtual ~FujianSystemMonitor();

protected:
    /**
     * Testing control logic
     *
     * @param ctrl Current value of control data needed for system control events
     */
    virtual void CheckTesting( ControlData *ctrl);
    /**
     * Overloaded BepServer Publish method which provides the ability
     * to handle the fault level messages that were requested.
     *
     * @param node the published information to deal with.
     * @return the status of the operation.
     */
    virtual const string Publish(const XmlNode *node);
};
//-------------------------------------------------------------------------------------------------
#endif //FujianSystemMonitor_h
