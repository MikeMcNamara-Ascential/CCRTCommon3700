//******************************************************************************
// Description:
//    Plant host outbound test suite.
//
//==============================================================================
//
//     Copyright (c) 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/PlantHostOutbound/PlantHostOutboundTestSuite.cpp $
// 
// 3     5/01/07 1:36p Rwiersem
// Changes for the 07050201 core release:
// 
// Moved DAQInterfaceTest to its own subdirectory.  Added HostInterface
// subdirectory.  Removed PFSInterfaceTest.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "PlantHostOutboundTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "PlantHostOutboundTest.h"
#include "HostInterface/HostInterfaceTest.h"
#include "DAQ/DAQInterfaceTest.h"

PlantHostOutboundTestSuite::PlantHostOutboundTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(PlantHostOutboundTest::suite());
    addTest(HostInterfaceTest::suite());
    addTest(DAQInterfaceTest::suite());
}

