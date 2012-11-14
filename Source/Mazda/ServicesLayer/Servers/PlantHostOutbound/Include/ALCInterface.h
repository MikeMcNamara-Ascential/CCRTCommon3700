//******************************************************************************
//
// $Archive: /HMMA/Source/HMMA/ServicesLayer/Servers/PlantHostOutbound/ALCInterface.h $
// $Author: Gswope $
//   $Date: 3/09/06 3:16p $
//
// Description:
//    ALC Host Interface for HMMA
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /HMMA/Source/HMMA/ServicesLayer/Servers/PlantHostOutbound/ALCInterface.h $
// 
// 1     3/09/06 3:16p Gswope
// After changes to bring HMMA up to "current core"
// 
// 1     1/18/05 10:47a Gswope
// Initial Entry in this project in SourceSafe
// 
// 2     11/24/04 12:09p Bmeinke
// PFSInterface derived object for HMMA ALC host system
// 
// 1     11/19/04 3:10p Bmeinke
// 
//
//******************************************************************************
#include "HostInterface.h"

#ifndef ALC_INTERFACE_H_INCLUDED_
#define ALC_INTERFACE_H_INCLUDED_


/**
 * This thread is spun up by HMMAPlantHostOutbound if the plant is equipped with ALC.
 * The ALCInterface class will invoke the PlantHostOutbound methods for building the
 * test result string to be sent to ALC after it has been signaled that new test
 * results have been posted.  The ALCInterface class will then add the
 * appropriate wrappers to the message and then call the PlantHostoutbound method for
 * transmitting the message to the external host, which for this class is ALC.
 */
class ALCInterface : public HostInterface
{
public:
	/**
	 * Class constructor.
	 *
	 * @param server Pointer to the server that created us.
	 * @since Version 1.0
	 */
	ALCInterface();
	/**
	 * Class destructor.
	 * @since Version 1.0
	 */
	virtual ~ALCInterface();
	/**
	 * Return the header information to be added to each PFS string.
	 *
	 * @return PFS Header information.
	 * @since Version 1.0
	 */
	const std::string GetPFSHeader(void);

protected:
    /**
     * Store additional items from the config file to complete the initialization of the host interface.
     * 
     * @param config Configuration data used to complete the initialization of the host interface.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *config);
    /**
     * Generate the result string to be sent to the host system.
     * 
     * @param testResults
     *               Test result data to be used for generating the result string for the host system.
     * 
     * @return Result string to be transmitted to the host system.
     */
    virtual string GenerateHostResultString(const XmlNode *testResults);
	/**
	 * Update the test result string header if any information is dependant on test results.
	 *
	 * @param data   Data to update the header with.
	 * @since Version 1.1
	 */
	virtual void UpdatePFSHeader(const INT32 &data);
	/**
	 * Set the information that will be constant for eac PFS record sent out.
	 *
	 * @param header Header information to use.
	 * @since Version 1.0
	 */
	virtual void SetPFSHeader(const XmlNode *header);
    /**
     * Save/Retrieve the result detail map.
     * If the parameter is NULL, just the detail result string map will be returned.  Otherwise, 
     * the result detail map will be stored.
     * 
     * @param detailMap Detail result string map to use when building the result string.
     * 
     * @return Detail result string map to use when building the result string.
     */
    const XmlNodeMap& DetailMap(const XmlNode *detailMap = NULL);
    /**
     * Transmit the result string to the plant host system system.
     *
     * @param result The result string to be sent.
     */
    virtual void SendResultToHost(const string &result);

private:

    /**
     * The map for the detail section of the Test result string.  This map is specific to
     * each plant.
     */
    XmlNodeMap   m_pfsDetailMap;
};

#endif // ALC_INTERFACE_H_INCLUDED_

