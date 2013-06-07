//*************************************************************************
// FILE DESCRIPTION:
//  The MazdaSystemMonitor monitors the system to determine when a vehicle
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
#ifndef MazdaSystemMonitor_h
#define MazdaSystemMonitor_h
//-------------------------------------------------------------------------------------------------
#include "SystemMonitor.h"
//-------------------------------------------------------------------------------------------------
const bool buildDataLoaded = true;
const bool buildDataNotLoaded = false;
//-------------------------------------------------------------------------------------------------
class MazdaSystemMonitor : public SystemMonitor
{
public:
    MazdaSystemMonitor();
    MazdaSystemMonitor(int argc, char *argv[]);
    virtual ~MazdaSystemMonitor();



protected:
    /**
     * method for component initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);

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

    /**
     * Write a datatag to the NDB
     * 
     * @param string tag - the NDB tag name
     * @param string value - the value that should be written
     */
    virtual string WriteDataTag(string tag, string value);




private:

    /**
     * Will parse through an xml node and write a bunch of plc system tags which will
     * clear the bits on the plc
     */
    void ClearMazdaAlcBits(void);

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

    /**
     * The list that will map PLC bits to the vehicle build info  
     */
    XmlNodeMap m_plcClearBits;
};
//-------------------------------------------------------------------------------------------------
#endif //MazdaSystemMonitor_h
