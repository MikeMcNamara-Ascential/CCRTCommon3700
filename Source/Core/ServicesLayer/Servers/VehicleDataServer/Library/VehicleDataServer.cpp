//*************************************************************************
// Description:
//   Vehicle data server.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/VehicleDataServer/Library/VehicleDataServer.cpp $
// 
// 8     10/31/07 10:12a Rwiersem
// Changes for the 07103101 core release:
// 
// - Added seting VINReadStatus to "Green" when a valid VIN is received.
// - Removed #if 0 blocks.
// 
// 7     2/22/07 11:20a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Removed commented out code.
// 
// 6     10/26/06 1:27p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 5     8/02/06 8:34p Cward
// Assigned result of CreateBuild to result in Read method
//
// 4     5/08/06 7:46p Cward
// Added reload capability
//
// 1     5/04/06 3:34p Rwiersem
// Added HandlePulse() and LoadAdditionalConfigurationItems() to support
// reloading the configuration without restarting the process.
//
// 3     11/11/05 4:57p Cward
// Changes for 0511101 iteration
//
//       8/24/05 4:24p Bmeinke
// Replaced DoHmmaBuildCreation() and DoKoreaBuildCreation() with a
// virtual CreateBuild() method. CreateBuild calls the new
// ProcessBuildItem() method.
// Added ProcessBuildItem()  to customize individual build data items
// based on external inputs (i.e. PLC pushbuttons).
//
// 2     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 5     4/27/05 11:20a Gswope
// added alternate publish method (used by test)
//
// 4     4/15/05 11:05a Gswope
// removed unused local variable and obsolete comment
//
// 3     4/15/05 8:27a Gswope
// Substantial revisions to incorporate both HMMA and Korean modes
// modifications to make this work with Korea, HMMA, and Nissan
// globally...
// name change m_vehicleBuild -> m_vehicleBuilds
// added m_thisBuildData
// individually...
// in destructor, conditionally delete m_thisBuildData
// line 116: fixed logic
//
// 2     3/02/05 2:09p Gswope
// Significant changes to allow HMMA to run without host
//
// 2     11/15/04 11:32a Bmeinke
// Removed the #if 1 / else / endif inside the Read() method
// Added some logging output inside the Read() method
//
// 1     12/21/03 6:29p Cward
//
// 4     10/24/03 2:02p Khykin
// Updated with latest changes from Korea.
//
// 1     10/07/03 11:28a Khykin
// Initial checkin
//
// 3     9/05/03 1:01p Khykin
// Updated for Korea.
//
// 2     8/29/03 5:16p Khykin
// Updated with latest changes to work.  Will test in the begining.
//
// 1     8/29/03 11:27a Khykin
// Initial checkin.
//
// 1     8/22/03 3:19p Rwiersem
// Initial version.
//
//*************************************************************************

#include "VehicleDataServer.h"
#include "INamedDataBroker.h"

//-----------------------------------------------------------------------------
inline const char *b2s (bool value)
{
    return((value) ? "True" : "False");
}


//-----------------------------------------------------------------------------
VehicleDataServer::VehicleDataServer():BepServer(), m_vehicleBuilds(NULL),
m_thisBuildData(NULL), m_dataBroker(NULL)
{
    m_useScalar = false;
    m_useDiscrete = false;

}

//-----------------------------------------------------------------------------
VehicleDataServer::~VehicleDataServer()
{
    if(NULL != m_vehicleBuilds) delete m_vehicleBuilds;
    if(NULL != m_thisBuildData) delete m_thisBuildData;
    if(NULL != m_dataBroker)    delete m_dataBroker;
}

//-----------------------------------------------------------------------------
const std::string VehicleDataServer::Register(void)
{   // Create a new NDB comm object
    if(m_dataBroker != NULL)  delete m_dataBroker;
    m_dataBroker = new INamedDataBroker();
    // Call the base class to complete the registration
    return BepServer::Register();
}

//-----------------------------------------------------------------------------
void VehicleDataServer::Initialize(const XmlNode *document)
{
// go do the parent initialization first, so logging will work?
    BepServer::Initialize(document);

    Log(LOG_FN_ENTRY, "VehicleDataServer::Initialize %s\n",document->ToString().c_str());

    // load the additional configuration items
    LoadAdditionalConfigurationItems(document);
}

void
VehicleDataServer::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    try
    {
        // get a copy of the vehicle build
        m_vehicleBuilds = document->getChild("Setup/VehicleBuildDefinitions")->Copy();
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS,"VehicleDataServer::Initialize exception %s\n",e.what());
    }
// determine which method of wheelbase selection to use
    m_useScalar = false;
    try
    {
        if("1" == document->getChild("Setup/Parameters/UseScalarPlcWheelbase")->getValue())
            m_useScalar = true;
    }
    catch(...)
    {
// this could be intentional
        Log(LOG_DEV_DATA,"VehicleDataServer::Initialize UseScalarPLCWheelbase node not found.\n");
    }
    m_useDiscrete = false;
    try
    {
        if("1" == document->getChild("Setup/Parameters/UseDiscretePlcWheelbase")->getValue())
            m_useDiscrete = true;
    }
    catch(...)
    {
// this could be intentional
        Log(LOG_DEV_DATA,"VehicleDataServer::Initialize: UseDiscretePLCWheelbase node not found.\n");
    }
// determine what to do if neither is selected
    if((false == m_useDiscrete) && (false == m_useScalar))
    {
        Log(LOG_ERRORS,"VehicleDataServer::Initialize: Neither wheelbase method selected.\n");
        m_useDiscrete = true;
    }
// do a log message if both are selected, this could be intentional
    if((true == m_useDiscrete) && (true == m_useScalar))
    {
        Log(LOG_ERRORS,"VehicleDataServer::Initialize: Both wheelbase methods selected\n");
    }

// get the default wheelbases (not implemented for Scalar)
    m_defaultWheelbase = "0";
    m_defaultVehicle = 0;
    if(true == m_useDiscrete)
    {
        try
        {
            m_defaultWheelbase = document->getChild("Setup/Parameters/DefaultWheelbase")->getValue();
        }
        catch(...)
        {
            Log(LOG_ERRORS,"VehicleDataServer::Initialize: DefaultWheelbase node not found.\n");
        }
        try
        {
            m_defaultVehicle = BposReadInt(document->getChild("Setup/Parameters/DefaultVehicle")->getValue().c_str());
        }
        catch(...)
        {
            Log(LOG_ERRORS,"VehicleDataServer::Initialize: DefaultVehicle node not found.\n");
        }
    }
    try
    {
        m_buildCreationMethod = document->getChild("Setup/Parameters/BuildCreationMethod")->getValue();
    }
    catch(...)
    {
        Log(LOG_DEV_DATA,"VehicleDataServer::Initialize: BuildCreationMethod node not found.\n");
        m_buildCreationMethod = "KOREA";
    }

    Log(LOG_DEV_DATA,"VehicleDataServer::Initialize complete. Parameters in use:\n");
    Log(LOG_DEV_DATA,"    m_useScalar = %s\n", b2s(m_useScalar));
    Log(LOG_DEV_DATA,"    m_useDiscrete = %s\n", b2s(m_useDiscrete));
    Log(LOG_DEV_DATA,"    m_defaultWheelbase = \"%s\"\n", m_defaultWheelbase.c_str());
    Log(LOG_DEV_DATA,"    m_defaultVehicle = %d\n", m_defaultVehicle);
    Log(LOG_DEV_DATA,"    m_buildCreationMethod = \"%s\"\n", m_buildCreationMethod.c_str());
}

//-----------------------------------------------------------------------------
const std::string VehicleDataServer::Publish(const std::string &tag, const std::string &value)
{
    XmlNode node(tag, value);
    return(Publish(&node));
}

//-----------------------------------------------------------------------------
const std::string VehicleDataServer::Publish(const XmlNode *node)
{
    Log(LOG_FN_ENTRY, "VehicleDataServer::Publish Received: %s\n",
        node->ToString().c_str());
    string response;

    if(node->getName() == GetDataTag("NextVIN"))
    {
        if(ReadSubscribeData(GetDataTag("TestInProgress")) == "1")
        {
            m_storedVIN = node->getValue();
            Log(LOG_DEV_DATA, "Vin Stored: \"%s\"\n", m_storedVIN.c_str());
        }
        else
        {
            m_storedVIN.erase();
            SetData(GetDataTag("LatestVIN"),node->getValue());
            Log(LOG_DEV_DATA, "Updated LatestVIN: \"%s\"\n", node->getValue().c_str());
            SetData(string("VINReadStatus"),string("Green"));
            // Make sure the rest of the system is aware of the VIN read status
            m_dataBroker->Write(VIN_READ_STATUS_TAG, string("Green"), response, true);
        }
    }
    else if(node->getName() == GetDataTag("TestInProgress"))
    {
        if(node->getValue() == "0")
        {
// TestInProgress is transitioning true to false

// note that if we ever have a problem with a stacked scan being lost,
// it is possible that we recieved two publications of TestInProgress,0
// with no TestInProgress,1 between them. This would be interpreted as
// a change, when in fact no change had taken place
// If we need to verify that this is truely an edge, we need to do a
// ReadSubscribeData() before the call to BepServer::Publish() below

            if(ReadSubscribeData("TestInProgress") == "1")
            {
                if(m_storedVIN != "")
                {   //make sure we don't update stuff for an empty vin
                    SetData(GetDataTag("LatestVIN"),m_storedVIN);
                    Log(LOG_DEV_DATA, "Updated LatestVIN with m_storedVIN: \"%s\"\n", m_storedVIN.c_str());
                    m_storedVIN.erase();
                    SetData(string("VINReadStatus"),string("Green"));
                    // Make sure the rest of the system is aware of the VIN read status
                    m_dataBroker->Write(VIN_READ_STATUS_TAG, string("Green"), response, true);
                }
                else
                {
                    Log(LOG_DEV_DATA, "VIN was blank. Will not update");
                }


            }
        }
        else
        {
// if TestInProgress is true, we do NOT process anything
// we just use the call below to save the info
        }
    }
    return(BepServer::Publish(node));
}

const std::string VehicleDataServer::Read(const std::string &tag, const INT32 rate) /* = 0 */
{
    // Create a node from the data and call the Read method
    XmlNode node(tag,"");
    return(Read(&node, rate));
}

const std::string VehicleDataServer::Read(const XmlNode *node, const INT32 rate)  /*= 0*/
{
    Log(LOG_FN_ENTRY, "Enter VehicleDataServer::Read(%s, %d)\n", node->ToString().c_str(), rate);
    std::string result(BEP_ERROR_RESPONSE);
// value of tag "VehicleBuild" is currently "NextVehicleBuild" at HMMA
    if(node->getName() == GetDataTag("VehicleBuild"))
    {
        Log(LOG_DEV_DATA, "VehicleDataServer: Reading Vehicle Build\n");
//defaults will be overwritten by one of the two methods below
        int vehicleSelected = 0;
        std::string wheelbase("0");
//      std::string thisTag;

// 4/11/2005 - ews
// Using parameters, either of these two methods can be explicitly selected.
// note that if BOTH are called, this is the lower priority method. The
// data will be overwritten if the second method returns any non-zero value.
        if(true == m_useScalar)
        {
// Method 1 copied from Korea (not in use)
            // set the wheelbase to the vehicle type from PLC
//            wheelbase = ReadSubscribeData(GetDataTag("VehicleTypeFromPLC"));
            INamedDataBroker ndb;
            string response;
            ndb.Read(GetDataTag("VehicleTypeFromPLC"), response, true);
            ndb.GetByTag(GetDataTag("VehicleTypeFromPLC"), wheelbase, response);
// note that if the value is invalid, the next line evaluates as 0
            vehicleSelected = atoi(wheelbase.c_str());
        }

        if(true == m_useDiscrete)
        {
// Method 2, used in both Korea and HMMA
            std::string tempWheelbase;
            int tempVehicleSelected;
            ConvertBitToWheelbase(&tempWheelbase, &tempVehicleSelected);
            if(0 != tempVehicleSelected)
            {
// discrete value was valid. always use it
                wheelbase = tempWheelbase;
                vehicleSelected = tempVehicleSelected;
            }

        }
        if(0 == vehicleSelected)
        {
// discrete invalid, and no scalar value
// get the default values, based on the configuration file data
            wheelbase = m_defaultWheelbase;
            vehicleSelected = m_defaultVehicle;
        }
        Log(LOG_DEV_DATA, "VDS: wheelbase = %s; vehicleSelected = %d\n",
            wheelbase.c_str(), vehicleSelected);

        // Process the build data
        result = CreateBuild(wheelbase, vehicleSelected);
    }
    else
    {
// everything else is handled by the parent
        result = BepServer::Read(node, rate);
    }

    Log(LOG_FN_ENTRY, "Exit VehicleDataServer::Read() result: %s\n", result.c_str());

    return(result);
}

std::string VehicleDataServer::CreateBuild(std::string wheelbase, int vehicleSelected)
{
    std::string absSelected(ReadSubscribeData(GetDataTag("ABSTestSelectedFromPLC")));
    std::string cableConnect(ReadSubscribeData(GetDataTag("CableConnect")));
    std::string result(BEP_ERROR_RESPONSE);

    if((m_vehicleBuilds != NULL) && (!wheelbase.empty()))
    {
        Log(LOG_DEV_DATA, "VehicleDataServer: Builds found. Wheelbase = %s, VehicleSelected = %d\n",
            wheelbase.c_str(), vehicleSelected);
        try
        {
            if(NULL != m_thisBuildData)
            {
                delete m_thisBuildData;
                m_thisBuildData = NULL;
            }
            m_thisBuildData = (m_vehicleBuilds->getChildren().getNode(vehicleSelected)->Copy());


            Log(LOG_DEV_DATA, "VehicleDataServer: PLC ABS=%s  CableConnect=%s\n",
                absSelected.c_str(), cableConnect.c_str());

// create the build string here
            Log( LOG_DEV_DATA, "VehicleDataServer: BuildingResponseString\n");
            result = "";

            XmlNodeMap  vehicleBuild = m_thisBuildData->getChildren();
            for(XmlNodeMapItr itr=vehicleBuild.begin(); itr!=vehicleBuild.end(); itr++)
            {
                result += ProcessBuildItem( itr->second);
            }

            //std::for_each(vehicleBuild.begin(), vehicleBuild.end(), &VehicleDataServer::ProcessBuildItem);
        }
        catch(XmlException &e)
        {
// default result string will be used.
            Log(LOG_ERRORS, "ERROR: Unable to get Vehicle Build Info for Vehicle: %d\n", vehicleSelected);
        }
    }
    else
    {
        Log(LOG_ERRORS, "ERROR: No Setup/VehicleBuildDefinitions Information In The Configuration File\n");
        // default result string (BEP_ERROR_RESPONSE) will be used.
    }

    Log(LOG_FN_ENTRY,"Exit VehicleDataServer::CreateBuild(), result = %s", result.c_str());

    return(result);
}

std::string VehicleDataServer::ProcessBuildItem(XmlNode *buildItem)
{
    const std::string &tag=buildItem->getName();
    std::string value(buildItem->getValue());
    std::string targetData;

    if(GetDataTag("BrakeType") == tag)
    {
        string absSelected(ReadSubscribeData(GetDataTag("ABSTestSelectedFromPLC")));
        string cableConnect(ReadSubscribeData(GetDataTag("CableConnect")));
        // set the brake type.
        // Use ABS type from the config file if cable connected.
        if((absSelected == "1") || (cableConnect == "1"))
        {
            Log(LOG_DEV_DATA, "VehicleDataServer: Configuration ABS Selected.\n");
        }
        else if(atob(GetDataTag("CableConnectAfterBuildSelect").c_str()))
        {
            Log(LOG_DEV_DATA, "VehicleDataServer: Not overwriting brake type since cable is connected later");
        }
        else
        {
            // otherwise, set it to base brake.
            Log( LOG_DEV_DATA, "VehicleDataServer: Base Brake Selected\n");
            buildItem->setValue(GetDataTag("BaseBrake"));
            Log( LOG_DEV_DATA, "VehicleDataServer: Base Brake Set\n");
        }
    }
    else if(GetDataTag("TractionEnabled") == tag)
    {
        // set the traction control selection, as determined by the PLC
        Log( LOG_DEV_DATA, "VehicleDataServer: Determining Traction\n");
        targetData = ReadSubscribeData(GetDataTag("TractionControlSelected"));
        buildItem->setValue(targetData);
        Log( LOG_DEV_DATA, "VehicleDataServer: Traction set to %s\n", targetData.c_str());

    }
    else if(GetDataTag("VIN") == tag)
    {
        // update the VIN node, from LOCAL data
        Log( LOG_DEV_DATA, "VehicleDataServer: Determining VIN\n");
        targetData = GetData(GetDataTag("LatestVIN"));
        buildItem->setValue(targetData);
        Log( LOG_DEV_DATA, "VehicleDataServer: VIN set to %s\n", targetData.c_str());
    }

    // create the build string here
    Log( LOG_FN_ENTRY, "Exit VehicleDataServer::ProcessBuildItem()");

    return( buildItem->ToString());
}

void VehicleDataServer::ConvertBitToWheelbase(std::string *wheelbase, int *vehicleSelected)
{
    if(ReadSubscribeData(GetDataTag("WheelbasePosition01Selected")) == "1")
    {
        *wheelbase = "1";
        *vehicleSelected = 1;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition02Selected")) == "1")
    {
        *wheelbase = "2";
        *vehicleSelected = 2;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition03Selected")) == "1")
    {
        *wheelbase = "3";
        *vehicleSelected = 3;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition04Selected")) == "1")
    {
        *wheelbase = "4";
        *vehicleSelected = 4;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition05Selected")) == "1")
    {
        *wheelbase = "5";
        *vehicleSelected = 5;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition06Selected")) == "1")
    {
        *wheelbase = "6";
        *vehicleSelected = 6;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition07Selected")) == "1")
    {
        *wheelbase = "7";
        *vehicleSelected = 7;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition08Selected")) == "1")
    {
        *wheelbase = "8";
        *vehicleSelected = 8;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition09Selected")) == "1")
    {
        *wheelbase = "9";
        *vehicleSelected = 9;
    }
    else if(ReadSubscribeData(GetDataTag("WheelbasePosition10Selected")) == "1")
    {
        *wheelbase = "10";
        *vehicleSelected = 10;
    }
    else
    {
        *wheelbase = "0";
        *vehicleSelected = 0;
    }
}

