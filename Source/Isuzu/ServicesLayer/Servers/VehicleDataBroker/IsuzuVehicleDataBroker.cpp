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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Isuzu/ServicesLayer/Servers/VehicleDataBroker/IsuzuVehicleDataBroker.cpp $
// 
// 2     2/07/08 9:58a Derickso
// Updated to support global use bypass part number list tag.
// 
// 1     11/01/07 2:58p Derickso
// Added capability to support a common module fault list.
// 
// 10    10/26/06 1:24p Rwiersem
// Changes for the 06102701 core release:
// 
// Now clear the XmlNodeMaps in the destructor.
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 9     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/07/06 11:01a Rwiersem
// Removed the old ReloadConfiguration() method, added the HandlePulse()
// method, and added the LoadAdditionalConfigurationItems() method.
//
// 8     7/20/05 9:31p Cward
// Added log calls to register method
//
// 7     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 6     5/09/05 5:00p Gpattison
// Re-arranged the Initialize() method to call the base class initialize
// as soon as possible
//
// 6     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/18/05 12:01p Gswope
// added try/catch around attempt to read build record
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     11/15/04 11:29a Bmeinke
// Added a m_useDefDriverNo data meber to say whether or not to use the
// DefaultDriverNumber from the config file when we reset the drive number
// on rolls down or if we should read the current driver number from the
// system
//
// 4     3/04/04 5:33p Cward
// Removed temporary code that was a work around, to get around a PLC
// problem for wheebase adjust
//
// 3     2/26/04 6:35p Cward
// Changes made for the Feb 26, 2004 iteration
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
//
// 3     10/24/03 2:01p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/18/03 7:53a Khykin
// Updated with Flatrock changes.
//
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 3:57p Khykin
// Initial checkin.
//
// 1     4/22/03 9:52a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:40  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:19a Khykin
// Initial Checkin
//
// 1     1/18/03 8:13a Khykin
// Initial Checkin From DCTC
//
// 21    10/22/02 3:54p Derickso
// Modified to read NextVehicleBuild tag from PlantHostInbound
// and publish VehicleBuild to subscribers.
// Updated LoadDataTags method to use macros out of BepTagDefs.h
//
// 20    9/05/02 3:28p Derickso
// Updated to support J2190ActiveVehicleData.
//
// 19    8/23/02 10:47a Dan.erickson
// Updated Initialize method to set the Default driver number.  Updated to
// handle driver number display processing.
//
// 18    7/29/02 8:11a Dan.erickson
// Added functionality to gather active data from the vehicle
// modules during testing.
//
// 17    6/13/02 3:02p Dan.erickson
// Added diagnostic messages.
//
// 16    6/03/02 1:06p Dan.erickson
// Modified UpdatePLC() to only write SET or CLEAR for boolean
// type items.
//
// 15    5/20/02 4:45p Dan.erickson
// Allow UpdateVehicleBuildTags to continue publishing all tags
// even if a failure is encountered.
// Modified return value of Publish().
//
// 14    4/25/02 4:47p Skypig13
// Updated with new changes
//
// 12    4/16/02 7:26a Dan.erickson
// Added code to set and get the vin display tag.
// Use ConvertStatusToResponse().
// In UpdateVehicleBuildTags(), write VINDisplay to the
// NamedDataBroker.
//
// 11    4/03/02 1:47p Dan.erickson
// Removed support for VinDisplay tag.  MachineDataBroker will
// now support this tag.
//
// 10    3/27/02 3:31p Dan.erickson
// Added support to invalidate the driver number when the
// retaining rolls are lowered.
// Added support to publish the vin to the GUI.
//
// 9     3/25/02 9:08a Dan.erickson
// Overloaded Initialize(const std::string&) method.
//
// 8     3/22/02 1:33p Dan.erickson
// Reworked data that gets sent to PLC.  The tags are now coming
// from the config file so they can be changed at any time.
//
// 7     2/18/02 1:09p Dan.erickson
// Reworked for new version of BepServer.
//
// 6     2/12/02 5:19p Dan.erickson
// Moved retrieval of StartTest tag and VehicleBuild tag to LoadDataTags
// method.
// Pass DataTags node to LoadDataTags method.
//
// 5     2/06/02 2:26p Admin
// Checking in for system testing.
//
// 4     1/16/02 4:24p Dan.erickson
// Modified Initialize method to get node name instead of the value.
//
// 3     1/11/02 5:00p Dan.erickson
// Added UpdatePLC method.
// Cleaned up code and used accessor methods to return data.
//
// 2     1/07/02 9:45a Dan.erickson
// Modified Initialize method to reflect new location of
// configuration data under the Setup node.
//
// 1     12/06/01 3:17p Dan.erickson
// Created.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================

#include "IsuzuVehicleDataBroker.h"
#include "GeneralUtilities.h"

//-----------------------------------------------------------------------------
IsuzuVehicleDataBroker::IsuzuVehicleDataBroker() : VehicleDataBroker()
{
}

//-----------------------------------------------------------------------------
IsuzuVehicleDataBroker::~IsuzuVehicleDataBroker()
{   
}

//-----------------------------------------------------------------------------
void IsuzuVehicleDataBroker::Initialize(const std::string &fileName)
{
    BepServer::Initialize(fileName);
}

//-----------------------------------------------------------------------------
void IsuzuVehicleDataBroker::Initialize(const XmlNode *document)
{
    // Initialize the base server
    BepServer::Initialize(document);
    Log("IsuzuVehicleDataBroker::Initialize(%s)", document->ToString().c_str());

    // load the additional configuration items
    LoadAdditionalConfigurationItems(document);
}

//-----------------------------------------------------------------------------
const std::string IsuzuVehicleDataBroker::UpdateVehicleBuildRecord(const XmlNode *vehicleBuildRecord, bool publishRecord)
{
    const std::string noDriveAxle( "None");

    Log( LOG_FN_ENTRY, "Enter IsuzuVehicleDataBroker::UpdateVehicleBuildRecord()\n");

    // If a front axle test is selected
    if( ReadSubscribeData( "FrontAxleTestSelected") == "1")
    {
        XmlNode *driveAxle = (XmlNode*)vehicleBuildRecord->getChild(DRIVE_AXLE_TAG);

        if( NULL != driveAxle)
        {
            Log( LOG_DEV_DATA, "Setting %s to %s in the build record because FrontAxleTestSelected is 1\n", 
                 DRIVE_AXLE_TAG, noDriveAxle.c_str());
            driveAxle->setValue( noDriveAxle);
        }
        
    }
    
    return(VehicleDataBroker::UpdateVehicleBuildRecord(vehicleBuildRecord, publishRecord));
}
