//*************************************************************************
// FILE DESCRIPTION:
//   Vehicle data broker that is responsible for maintaining and publishing
//   all of the information related to the vehicle.
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
#ifndef MazdaVehicleDataBroker_h
#define MazdaVehicleDataBroker_h
//-------------------------------------------------------------------------------------------------
#include "VehicleDataBroker.h"
//-------------------------------------------------------------------------------------------------
class MazdaVehicleDataBroker : public VehicleDataBroker
{
public:
    MazdaVehicleDataBroker();
    virtual ~MazdaVehicleDataBroker();

protected:
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     */
    virtual const string Publish(const XmlNode *node);
    /**
     * Load the additional configuration items needed by the vehicle data
     * broker.  The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

    /**
     * Read the current vehicle build record from the WorkCellController.
     *
     * @return Result of the operation - BEP_SUCCESS_RESPONSE, BEP_ERROR_RESPONSE, etc.
     */
    virtual const string ReadLatestVehicleBuildRecord(bool publishAfterRead=true);

private:
    /**
     * The shift points will be read from the vehicle build data and displayed on the screen.
     */
    void UpdateTransmissionShiftPoints(void);

    /**
     * Get the current engine size selected.
     * 
     * @return Current engine size selected.
     */
    const string& GetEngineSize(void);

    /**
     * Get the current selected speedometer type.
     * 
     * @return Current speedometer type (mph/kph).
     */
    const string& GetSpeedoType(void);

    /**
     * Store the currently selected engine size.
     * 
     * @param engineSize Selected engine size
     */
    void SetEngineSize(const string &engineSize);

    /**
     * Store the currently selected speedometer type.
     * 
     * @param speedoType Selected speedometer type.
     */
    void SetSpeedoType(const string &speedoType);

    /** Type of engine so the correct test parameters can be displayed */
    string m_engineSize;
    /** Type of speedometer (mph/kph) so the correct test parameters can be displayed */
    string m_speedoType;
};
//-------------------------------------------------------------------------------------------------
#endif //MazdaVehicleDataBroker_h
