//******************************************************************************
// Description:
//  Interface class for the BEP Ndb reporting interface.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/PlantHostOutbound/Include/NdbInterface.h $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef NdbInterface_h
#define NdbInterface_h
//==============================================================================
#include "HostInterface.h"

/**
 * This thread is spun up by the PlantHostOutbound if the plant is equipped 
 * with a DataCollector system.  The NdbInterface class will invoke the 
 * PlantHostOutbound methods for building the test result string to be sent to 
 * the name data broker after it has been signaled that new test results have been 
 * posted.  The NdbInterface class will then add the appropriate wrappers to 
 * the message and then call the PlantHostoutbound method for transmitting the 
 * message to the name data broker.
 *
 * @author Joe Semann
 */
class NdbInterface : public HostInterface
{
    //friend class NdbInterfaceTest;

public:
    /**
     * Class constructor.
     */
    NdbInterface();
    /**
     * Initialize the host interface using the supplied command line arguments.
     * The command line argumets will be parsed and used for initializing the 
     * host interface.
     * 
     * @param argc   Count of the command line arguments.
     * @param argv Command line arguments to parse and use for initializing the 
     *             host interface.
     */
    virtual void Initialize(int argc, char *argv[]);
    /**
     * Initialization method that retrieves the xml document.
     * 
     * @param fileName Configuration file which contains the object setup
     *                 information.
     */
    virtual void Initialize(const string &fileName);
    /**
     * Method for initialization that is passed a parsed configuration file.
     *
     * @param document Configuration data for setting up the interface.
     */
    virtual void Initialize(const XmlNode *document);

protected:
    /**
     * Process the node from the map specifying how to build the result string.  Save the
     * Resulting string in an xmlnode map to be published to the ndb upon result processing.
     *
     * @param resultMapNode Result string field to build.
     * @param testResults
     *                The reported test results.
     * @param vehicleData
     *                The vehicle build information from the tested vehicle.
     * @param resultConversions  The map specifying test result conversion. 
     * @param totalSerialNumbers
     *                The total number of Serial numbers reported in the test 
     *                results.
     * @return The data to place in the current result string field.
     */
    virtual const string ProcessMapNode(const XmlNode *resultMapNode,
                                const XmlNodeMap &testResults,
                                const XmlNodeMap &vehicleData,
                                const XmlNodeMap &resultConversions,
                                INT32 &totalSerialNumbers);

    /**
     * Transmit the result string to the plant host system system.
     *
     * @param result The result string to be sent.
     */
    virtual void SendResultToHost(const string &result);
private:
};

#endif  // NdbInterface_h
