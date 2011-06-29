//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/TestSelector.h 3     10/30/07 3:27p Rwiersem $
//
// FILE DESCRIPTION:
// The test selector is responsible for requesting the vehicle build 
// information and then determining which test best matches the current
// build and returns it to the calling process.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/Include/TestSelector.h $
// 
// 3     10/30/07 3:27p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/23/04 3:19p Cward
// Last minute high priority changes. Must be added.
// 
// 1     8/21/03 9:02a Derickso
// 05WK CCRT Project
// 
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
// 
// 1     6/20/03 9:43a Khykin
// Initial checkin
// 
// 1     6/20/03 9:19a Khykin
// Initial checkin.
// 
// 1     4/22/03 10:26a Khykin
// Initial Working Checkin.
// 
// 2     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:02:55  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 11:10a Khykin
// Initial Checkin
// 
// 1     1/18/03 7:17a Khykin
// Initial Checkin
// 
// 5     3/06/02 1:51p Kh
// Updating to handle base brake drive curve requests.
// 
// 4     12/20/01 10:19a Kh
// Loading the Loss Compensation Drive Curve name for loading.
// 
// 3     12/06/01 12:29p Kh
// Updating to be a BepComponent.
// 
// 2     11/20/01 11:10a Kh
// Updating for changes to the VehicleTest.
// 
// 1     7/06/01 5:35p Kh
// Just checking in for the weekend.  Will debug next week.
//
//*************************************************************************
#ifndef TESTSELECTOR_H
#define TESTSELECTOR_H

using namespace std;

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <errno.h>
#include <fstream>

//#include "GeneralUtilities.h" // general system utilities
#include "Debuggable.h"
#include "BepException.h"       // base exception definition
#include "BepComponent.h"       // base component definition
#include "BepDefs.h"
#include "Bepos.h"
#include "XmlParser.h"
#include "XmlNode.h"
#include "ILogger.h"
#include "BepTagDefs.h"

/**
 * The TestSelector class that is responsible for receiving
 * a VehicleBuild or test type and returning the DriveCurve
 * that should be used for testing.
 * 
 * @author Kevin Hykin
 * @version Version 1.3
 */
class TestSelector : public BepComponent
{
public:
    /**
     * Constructor that takes a reference to the logger interface from the 
     * parent process.
     */
    TestSelector();
    
    /**
     * Destructor of the Test Selector.
     * @since Version 1.0
     */
    virtual ~TestSelector();
    
    /**
     * Initialize method that loads the selection table and
     * prepares to read vehicle information and determine
     * which drive curve should be used to test with.
     * 
     * @param configuration
     *               The configuration of the Test Selector.
     * @since Version 1.0
     */
    void Initialize(const XmlNode *configuration);
    
    /**
     * Select Drive Curve reads the vehicle build information
     * from the Vehicle Build Server and then determines which
     * Drive Curve should be loaded to test the vehicle.
     * 
     * @param testType The type of test to perform.
     *                 TEST_LOSS_COMPENSATION
     *                 ...
     * @return The drive curve name. Ex DriveCurve1, ...
     * @since Version 1.0
     */
    std::string SelectDriveCurve(const std::string testType);

    /**
     * Select Drive Curve takes the vehicle build information and determines 
     * which Drive Curve should be loaded to test the vehicle.
     * 
     * @param vehicleBuild The vehicle build information.
     * 
     * @return The drive curve name. Ex DriveCurve1, ...
     */
    std::string SelectDriveCurve(const XmlNode *vehicleBuild, string &driveCurveRoot);
    /**
     * Return the tag being used for vehicle body style.
     * 
     * @return Vehicle body style tag.
     * @since Version 1.3
     */
    virtual const string& GetBodyStyleTag(void);
    
protected:
    /**
     * Store the tag being used for vehicle body style.
     * 
     * @param bodyStyleTag
     *               Tag for vehicle body style.
     * @since Version 1.3
     */
    virtual void SetBodyStyleTag(const string& bodyStyleTag);
    /**
     * Tag being used for the vehicle body style.
     * @since Version  1.3
     */
    string m_bodyStyleTag;
    /**
     * Reference to the selection table that has been parsed.
     * @since Version 1.0
     */
    XmlParser *m_parser;
    
    /**
     * Reference to the root node of the selection table.
     * @since Version 1.0
     */
    const XmlNode *m_selectionTable;

    /**
     * List of commandable drive curves.
     * @since Version 1.2
     */
    const XmlNode *m_commandableDriveCurves;
};

#endif //TESTSELECTOR_H

