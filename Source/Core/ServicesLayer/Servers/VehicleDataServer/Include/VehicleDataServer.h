//*************************************************************************
// Description:
//  Vehicle data server.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log
//
//*************************************************************************
#ifndef VehicleDataServer_h
#define VehicleDataServer_h
//==============================================================================

#include "BepServer.h"
#include "INamedDataBroker.h"

/**
 * VehicleDataServer processes the vehicle build data
 *
 * @author Ross Wiersema
 */
class VehicleDataServer : public BepServer
{
public:
    friend class VehicleDataServerTest;
    /**
     * VehicleDataServer default constructor
     */
    VehicleDataServer();

    /**
     * VehicleDataServer destructor
     */
    virtual ~VehicleDataServer();

    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
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
    virtual const std::string Register(void);

    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     */
    virtual const std::string Publish(const XmlNode *node);

    /**
    * alternate publish method, using tags
    *
    * @param tag    destination for the data
    * @param value  value to be stored
    *
    * @return Status of the updating of the published tag
    *         Success, Fail
      */
    virtual const std::string Publish(const std::string &tag, const std::string &value);

    /**
      * Request to read a data tag
      *
      * @param tag    Data to be read
      * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
      * @return Value of tag
      */
    virtual const std::string Read(const std::string &tag, const INT32 rate = 0);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);

protected:
    /**
     * Load the additional configuration items needed by the vehicle data
     * server.  The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

    /**
     * Load and process the vehicle build data from the config file for
     * a specific vehicle type and/or wheelbase position.
     *
     * @param wheelbase The wheelbase position for the vehicle to be selected
     * @param vehicleSelected
     *                  The vehicle type identifier for the vehicle build to be loaded
     *
     * @return The string form (i.e. buildData->ToString()) of the vehicle build
     *         data
     */
        virtual std::string CreateBuild(std::string wheelbase, int vehicleSelected);
    /**
     * Processes an individual vehicle build node to override the config
     * file settings due to external inputs (i.e. PLC pushbuttons)
     *
     * @param buildItem An XML node from the vehicle build data selected for. Use
     *                  buildItem->setValue() to override build data based on external inputs.
     *
     * @return The string form (i.e. buildItem->ToString()) of the given
     *         build node
     */
    virtual std::string ProcessBuildItem(XmlNode *buildItem);

    /**
     * converts discrete PLC bits used for wheelbase selection
     * into CCRT style wheelbase and vehicleSelected
     *
     * @param wheelbase       pointer to the string that holds wheelbase
     * @param vehicleSelected pointer to the int that holds the selected vehicle
     */
    void ConvertBitToWheelbase(std::string *wheelbase,int *vehicleSelected);

    /**
     * Internally stored VIN.
     * @since Version 1.0
     */
    string m_storedVIN;
    /**
     * The vehicle build that is used to determine what type of
     * vehicle is being tested.
     */
    XmlNode *m_vehicleBuilds;

    /**
     * The vehicle build that is used to determine what type of
     * vehicle is being tested.
     */
    XmlNode *m_thisBuildData;

    /**
     * Used to decide which method of wheelbase decode
     * to use from the PLC
     */
    bool m_useDiscrete;
    bool m_useScalar;

    /**
     * these two values supply default values
     * Used only if using the discrete wheelbase (bits) method
     */
    std::string m_defaultWheelbase;
    int m_defaultVehicle;

    /**
     * This determines the method for creating build data that is returned.
     *
     * "KOREA" --> This is the default method if none is provided.
     * Causes the data to be dynamically recreated from the specific build
     * items that are hard coded. This method also does a SetData() for each
     * specific field (Not sure if this is used by anyone, but it is
     * maintained for 100% backward campatibility.)
     *
     *
     * "HMMA"
     * Uses a copy of the retrieved build record, then modifies
     * specific fields before returning it. This has the advantage of passing
     * information that is in the configuration file, but is not specifically
     * called out for inclusion in the source code.
     */
    std::string m_buildCreationMethod;
    /** Object to read and write system data */
    INamedDataBroker *m_dataBroker;
};

#endif // VehicleDataServer_h
