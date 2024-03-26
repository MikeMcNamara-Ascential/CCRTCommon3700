//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/TestSelector/TestSelector.cpp 6     10/25/06 4:24p Rwiersem $
//
// FILE DESCRIPTION:
// The drive curve selector is responsible for requesting the vehicle build
// information and then determining which Drive Curve best matches the current
// vehicle build and returns it to the calling process.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/TestSelector/TestSelector.cpp $
// 
// 6     10/25/06 4:24p Rwiersem
// Changes for the 06102701 core release:
// 
// - Added writing of the vehicle build string in SelectDriveCurve()
// 
// 5     5/08/06 7:18p Cward
// Changed printf statements to Log statements.
//
// 4     4/13/06 3:33a Cward
// initialize variables
//
// 2     3/20/06 10:50a Bmeinke
// Initialize some uninitialized variables to remove compiler warnings
//
// 3     5/19/04 9:36p Cward
// Updated Select Drive Curve to check for an empty vehicle build as well
// as the existance of a vehicle build before starting a test
//
// 2     1/23/04 3:19p Cward
// Last minute high priority changes. Must be added.
//
// 4     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 2     8/15/03 2:06p Khykin
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/20/03 9:46a Khykin
// Initial checkin
//
// 1     6/20/03 9:22a Khykin
// Initial checkin.
//
// 2     4/30/03 9:20a Khykin
// Removed the localization of the standard drive curves (done by the
// user) and updated the selector to allow a selection table with no
// matches and no errors be a valid selection.
//
// 1     4/22/03 10:33a Khykin
// Initial Working Checkin.
//
// 1     4/10/03 7:41a Khykin
// Latest PIT machine code
//    Revision 1.1.1.1  2003/02/15 00:03:11  tbochene
//
//    PooBag
//
//
// 12    9/26/02 3:13p Tbochene
// Removed compiler warnings
//
// 11    8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 9     5/24/02 1:50p Kh
// Updating due to debugging.
//
// 8     3/06/02 1:36p Kh
// Making changes for logging and for the ability to sequence a base brake
// test.
//
// 7     2/06/02 12:21a Kh
// Debugging a possible SigSeg condition.
//
// 6     12/20/01 10:19a Kh
// Updating Logged information.
//
// 5     12/06/01 12:29p Kh
// Updating to be a BepComponent.
//
// 4     11/20/01 11:10a Kh
// Updating for changes in the VehicleTest.
//
// 3     8/28/01 11:07a Kh
// Updating for NTO 6.1
//
// 2     7/12/01 3:19p Kh
// Tested and ready to go.  Still need to add the Vehicle Build Server
// Comm. code.  Will do once the INamedDataBroker interface is complete.
//
// 1     7/06/01 5:34p Kh
// Checking in the weeks work.  Not functional just written and in the
// process of debug.
//
//*************************************************************************

#include "TestSelector.h"

TestSelector::TestSelector() : BepComponent(), m_parser(NULL), m_selectionTable(NULL),
    m_commandableDriveCurves(NULL)
{
};

TestSelector::~TestSelector()
{
    Log(LOG_FN_ENTRY, "~TestSelector Terminating\n");
    if (m_parser != NULL)
        delete m_parser;
    Log(LOG_FN_ENTRY, "~TestSelector Terminating done\n");
};

void TestSelector::Initialize(const XmlNode *configuration)
{
    BepComponent::Initialize(configuration);

    try
    {   // load the selection table
        if (m_parser != NULL)
            delete m_parser;
        m_parser = new XmlParser();
        m_selectionTable = m_parser->ReturnXMLDocument(getenv("FTP_ROOT") + configuration->getChild("Table")->getValue());
        Log(LOG_DEV_DATA, "TestSelector::Initialize Table: %s\n", m_selectionTable->getValue().c_str());

        // Load the body style tag
        try
        {   // Get the body style from the config file
            SetBodyStyleTag(configuration->getChild("Setup/DataTags/BodyModel")->getValue());
        }
        catch (XmlException &e)
        {   // Body style tag not found, use a default
            Log(LOG_ERRORS, "Could not find BodyModel DataTag, using BodyModel - %s\n", e.what());
            SetBodyStyleTag("BodyModel");
        }
    }
    catch (BepException &e)
    {
        m_selectionTable = NULL;
        Log(LOG_ERRORS, "Error Loading The Selection Table, BepException: %s\n", e.what());
    }

    try
    {
        m_commandableDriveCurves = configuration->getChild("CommandableDriveCurves");
        Log(LOG_DEV_DATA, "Loaded the DriveCurves: %s\n", m_commandableDriveCurves->ToString().c_str());
    }
    catch (BepException &e)
    {
        m_commandableDriveCurves = NULL;
        Log(LOG_ERRORS, "Error Loading DriveCurves, BepException: %s\n", e.what());
    }

};

std::string TestSelector::SelectDriveCurve(const std::string testType)
{
    std::string driveCurve; // the drive curve to return

    if (m_commandableDriveCurves != NULL)
    {
        try
        {   // get the commanded drive curve from the list
            if (!testType.empty())
                driveCurve = m_commandableDriveCurves->getChild(testType)->getValue();
            else
                throw BepException("No testType provided!");
        }
        catch (BepException &e)
        {
            Log(LOG_DEV_DATA, "Not In Loaded the drive curves: %s\n", m_commandableDriveCurves->ToString().c_str());
            throw BepException("Commanded Drive Curve %s Not Supported: %s\n", testType.c_str(), e.what());
        }
    }
    else
        throw BepException("No Commandable Drive Curves Loaded\n");


    return(driveCurve);
}

std::string TestSelector::SelectDriveCurve(const XmlNode *vehicleBuild, string &driveCurveRoot)
{
    std::string driveCurve; // the drive curve with the highest number of matches

    if ((vehicleBuild != NULL) && (vehicleBuild->getChildren().size()))  // if a vehicle build exists
    {
        if (m_selectionTable != NULL)
        {
            try
            {   
                std::string vehicleBuildString;

                vehicleBuild->writeNode( vehicleBuildString);
                // parse the build information
                Log(LOG_DEV_DATA, "Vehicle Build: {%s}\n", vehicleBuildString.c_str());

                // store the list with the highest number of matches without a discrepency
                int matches=-1;             // the number of matches the drive curve had
                int currentMatches=0;       // the current drive curves number of matches
                int difference=0;           // difference flag
                const XmlNode *build=NULL;      // the current vehicle build being looked at
                const XmlNode *entry=NULL;      // the table entry
                std::string buildRecordString;

                // walk through the all of the build information and determine the closest match
                for (UINT32 ii = 0; ii < m_selectionTable->getChildren().size(); ii++)
                {   // walk through each table build and determine how many matches there are
                    build = m_selectionTable->getChildren().getNode(ii);    // get the selection table build info
                    difference = 0;                                         // reset the difference
                    currentMatches = 0;                                     // reset the number equipemnt matches
                    buildRecordString.clear();
                    build->writeNode( buildRecordString);
                    Log( LOG_DEV_DATA, "Evaluating build record: {%s}\n", buildRecordString.c_str());
                    
                    for (UINT32 jj = 0; (jj < build->getChildren().size()) && (!difference); jj++)
                    {   // compare each entry in the selection table build to the vehicle build
                        try
                        {
                            entry = build->getChildren().getNode(jj);
                            if (entry->getValue() == "") // if blank, do not use in assessment
                            {
                                Log(LOG_DEV_DATA, "Blank Entry For: %s\n", entry->getName().c_str());
                            }
                            // if VIN or BodyModel, a character by character comparison must be made
                            else if ((entry->getName() == "VIN") || (entry->getName() == GetBodyStyleTag()))
                            {
                                std::string value = entry->getValue();
                                std::string tableValue = vehicleBuild->getChildren().getNode(entry->getName())->getValue();
                                int VINMatches = 0;
                                for (UINT32 kk = 0; (kk < value.length()) && (!difference); kk++)
                                {
                                    if (value[kk] != '?')
                                    {
                                        Log(LOG_DEV_DATA, "Evaluating %s: %c, %c\n", entry->getName().c_str(), value[kk], tableValue[kk]);
                                        if (value[kk] != tableValue[kk])
                                            difference++;
                                        else
                                            VINMatches++;
                                    }
                                }
                                // if there were no differences
                                if ((!difference) && (VINMatches))   // if the comparison was good
                                    currentMatches += VINMatches;   // increment matches by the number of VIN characters that matched
                            }
                            // else if equal, increment matches
                            else if (entry->getValue() == vehicleBuild->getChildren().getNode(entry->getName())->getValue())
                            {
                                Log(LOG_DEV_DATA, "Match For: %s, %s\n", entry->getName().c_str(), entry->getValue().c_str());
                                currentMatches++;
                            }
                            // else it did not match
                            else
                            {
                                Log(LOG_ERRORS, "Invalid Match: %s, %s\n", entry->getValue().c_str(), vehicleBuild->getChildren().getNode(entry->getName())->getValue().c_str());
                                difference++;
                            }
                        }
                        catch (XmlException &e)
                        {
                            Log(LOG_ERRORS, "Node Does Not Exist In Build: %s\n", entry->getName().c_str());
                        }
                    }

                    // determine if drive curve match is closer than the previous match
                    if (!difference) // if not zero differences
                    {
                        if (currentMatches > matches)    // if a better fit found
                        {
                            // store the new match number and drive curve name
                            matches = currentMatches;
                            driveCurve = build->getAttributes().getNode("DriveCurve")->getValue();
                            Log(LOG_DEV_DATA, "New Match: %s, %d\n", driveCurve.c_str(), matches);
                            try 
                            {
                                driveCurveRoot = build->getAttributes().getNode("DriveCurveRootDir")->getValue();
                                Log(LOG_DEV_DATA, "Updated Rewrite Root:  [%s]\n", driveCurveRoot.c_str());;
                            }
                            catch (XmlException &e)
                            {
                                Log(LOG_ERRORS, "TestSelector get attribute DriveCurveRootDir XmlException: %s", e.what());
                                driveCurveRoot = "";
                            }
                        }
                        else if (currentMatches == matches)  // if same, leave the first one but note to user
                        {
                            Log(LOG_DEV_DATA, "Warning Equal Match: %s, %s\n", driveCurve.c_str(),
                                build->getAttributes().getNode("DriveCurve")->getValue().c_str());
                            Log(LOG_DEV_DATA, "Warning Equal Match: %s, %s, %s\n", entry->getName().c_str(),
                                driveCurve.c_str(), build->getAttributes().getNode("DriveCurve")->getValue().c_str());
                        }
                        else    // else not a better match so go to next entry
                        {
                            Log(LOG_DEV_DATA, "Less Matches: %s, %d\n",
                                build->getAttributes().getNode("DriveCurve")->getValue().c_str(),
                                matches);
                        }
                    }
                    else
                    {
                        Log(LOG_DEV_DATA, "Build Did Not Match: %s\n", build->getName().c_str());
                    }
                }
            }
            catch (XmlException &e)
            {
                Log(LOG_ERRORS, "TestSelector SelectDriveCurve XmlException: %s", e.what());
            }
            catch (BepException &e)
            {
                Log(LOG_ERRORS, "TestSelector SelectDriveCurve BepException: %s", e.what());
            }
            catch (...)
            {
                Log(LOG_ERRORS, "TestSelector SelectDriveCurve Unknown Exception");
            }

        }
        else
        {
            throw BepException("TestSelector No Selection Table Provided");
        }
    }
    else
    {
        throw BepException("TestSelector No Vehicle Build Information");
    }

    return(driveCurve);
};

inline const string& TestSelector::GetBodyStyleTag(void)
{   // Return the tag for vehicle body style
    return m_bodyStyleTag;
}

inline void TestSelector::SetBodyStyleTag(const string& bodyStyleTag)
{   // Store the tag being used for vehicle body style
    m_bodyStyleTag = bodyStyleTag;
}
