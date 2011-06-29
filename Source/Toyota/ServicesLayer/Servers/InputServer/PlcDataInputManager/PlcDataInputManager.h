//*************************************************************************
// FILE DESCRIPTION:
//  The PlcDataInputManager handles vehicle data type inputs and publishes 
//  the data to objects that need the data.
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
#ifndef PlcDataInputManager_h
#define PlcDataInputManager_h
//-------------------------------------------------------------------------------------------------
#include "InputServerDefs.h"
#include "InputDeviceBase.h"
#include "INamedDataBroker.h"
//-------------------------------------------------------------------------------------------------
class InputServer;
//-------------------------------------------------------------------------------------------------
class PlcDataInputManager : public InputDeviceBase
{
public:
    PlcDataInputManager();
    virtual ~PlcDataInputManager();

    /**
     * Initialization method that parses the configuration file
     * and initializes the object.
     *
     * @param document Configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * <b>Description:</b>
     * Method that is called to command the component to perform any system 
     * registration functions necessary.
     *
     * @return BEP_SUCCESS_RESPONSE if the registration was successful
     *         BEP_FAILURE_RESPONSE if the registration was not successful
     */
    virtual const string Register(void);

protected:
    /**
     * <b>Description:</b>
     * Write the requested data item from the server.
     *
     * @param node  The data tag and value to write.
     *
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const string Publish(const XmlNode *node);

private:
    /**
     * Lookup the vehicle type and return an ASCII name from the code that was supplied.
     * 
     * @param type   Vehicle type code from the PLC.
     * 
     * @return Body style translation for the specified code.
     */
    string LookupVehicleType(const INT32 &type);

    /** Lookup table for vehicle type codes. */
    XmlNodeMap m_vehicleTypeLookupTable;

    /** Interface object to the Named Data Broker so we can get the vehicle data out to the correct location */
    INamedDataBroker *m_dataBroker;
};
//-------------------------------------------------------------------------------------------------
#endif //PlcDataInputManager_h
