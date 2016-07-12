//*************************************************************************
// Description:
//   This defines the IsuzuVehicleDataBroker object.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Isuzu/ServicesLayer/Servers/VehicleDataBroker/IsuzuVehicleDataBroker.h $
// 
// 2     2/07/08 9:58a Derickso
// Updated to support global use bypass part number list tag.
// 
// 1     11/01/07 2:58p Derickso
// Added capability to support a common module fault list.
// 
// 6     10/26/06 1:25p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 5     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/07/06 11:00a Rwiersem
// Made the test class a friend, removed the old ReloadConfiguration()
// method, added the HandlePulse() method, and added the
// LoadAdditionalConfigurationItems() method.
//
// 4     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
//
// 2     11/15/04 11:29a Bmeinke
// Added a m_useDefDriverNo data meber to say whether or not to use the
// DefaultDriverNumber from the config file when we reset the drive number
// on rolls down or if we should read the current driver number from the
// system
//
// 2     10/24/03 2:01p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 4:26p Khykin
// Initial checkin.
//
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:48  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:11a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 15    10/22/02 3:52p Derickso
// Added set and get methods for next vehicle build tag.
//
// 14    9/05/02 3:23p Derickso
// Added J2190ActiveVehicleData support.
//
// 13    8/23/02 9:11a Dan.erickson
// Add DriverNumberDisplay items.  Added OverallTest handling.
//
// 12    7/29/02 8:11a Dan.erickson
// Added functionality to gather active data from the vehicle
// modules during testing.
//
// 11    6/03/02 1:05p Dan.erickson
// Added string constant for boolean type PLC items.
//
// 10    4/25/02 4:43p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:03  erickson
//    started
//
//
// 8     4/03/02 1:47p Dan.erickson
// Removed support for VinDisplay tag.  MachineDataBroker will
// now support this tag.
//
// 7     3/27/02 3:31p Dan.erickson
// Added support to invalidate the driver number when the
// retaining rolls are lowered.
// Added support to publish the vin to the GUI.
//
// 6     3/25/02 9:09a Dan.erickson
// Overloaded Initialize(const std::string&) method.
//
// 5     3/22/02 1:36p Dan.erickson
// Removed everything that was dependant on specific tags
// being written to the PLC manager.
//
// 4     2/18/02 1:10p Dan.erickson
// Reworked after restructure of BepServer.
//
// 3     2/06/02 2:25p Admin
// Checking in for testing in the main system.
//
// 2     1/11/02 4:52p Dan.erickson
// Added member variables and accessor and settor methods.
//
// 1     12/06/01 3:16p Dan.erickson
// Created.
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#ifndef IsuzuVehicleDataBroker_h
#define IsuzuVehicleDataBroker_h

#include "VehicleDataBroker.h"


/**
 * Provides the vehicle build record for the vehicle currently being tested.
 *
 * @author Dan Erickson
 * @see BepServer
 */
class IsuzuVehicleDataBroker : public VehicleDataBroker
{
    //friend class VehicleDataBrokerTest;

public:
    /**
     * IsuzuVehicleDataBroker Constructor.
     */
    IsuzuVehicleDataBroker();
    /**
     * IsuzuVehicleDataBroker destructor.
     */
    virtual ~IsuzuVehicleDataBroker();
    /**
     * Method for initialization passing in the file name
     *
     * @param fileName Name of the XML configuration file to be used to initialize
     */
    virtual void IsuzuVehicleDataBroker::Initialize(const std::string &fileName);
    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    
protected:
    
    /**
     * Update the current vehicle build record with a new record to be used for testing.
     *
     * @param vehicleBuildRecord
     *               New vehicle build record.
     * @return Result of the operation - BEP_SUCCESS_RESPONSE, BEP_ERROR_RESPONSE, etc.
     * @since Version 1.0
     */
    const std::string UpdateVehicleBuildRecord(const XmlNode *vehicleBuildRecord,bool publishRecord=true);
};

#endif  //IsuzuVehicleDataBroker_h
