//*************************************************************************
// FILE DESCRIPTION:
//  System monitor for Aymesa.  The system monitor will determine when
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
#ifndef AymesaSystemMonitor_h
#define AymesaSystemMonitor_h
//-------------------------------------------------------------------------------------------------
#include "SystemMonitor.h"
//-------------------------------------------------------------------------------------------------
class AymesaSystemMonitor : public SystemMonitor
{
public:
    AymesaSystemMonitor();
    AymesaSystemMonitor(int argc, char *argv[]);
    virtual ~AymesaSystemMonitor();

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
#endif //AymesaSystemMonitor_h
