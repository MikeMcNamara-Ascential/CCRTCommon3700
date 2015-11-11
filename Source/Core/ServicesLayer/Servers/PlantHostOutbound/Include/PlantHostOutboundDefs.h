//******************************************************************************
// Description:
//   PlantHostOutbound definitions.
//
//===========================================================================
// Copyright (c) 2007- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/PlantHostOutbound/Include/PlantHostOutboundDefs.h $
// 
// 1     5/01/07 1:04p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef PlantHostOutboundDefs_h
#define PlantHostOutboundDefs_h
//-----------------------------------------------------------------------------
/**
 * Length of the Vin
 */
const UINT16 vinLength = 17;
const UINT16 serialNumberLength = 14;
/**
 * Yes/No test results.
 */
const string yesResult("Yes");
const string noResult("No");
/**
 * Tags to be used in the PlantHostOutbound server.
 */
const string startIndex("StartIndex");
const string fieldLength("Length");
const string fieldFormat("Format");
const string fieldType("Type");
const string fieldDataType("DataType");
const string fieldMapNames("MapComponentNames");
const string fieldMaxDtcReport("MaximumDtcsToReport");
const string excludeNotTested("ExcludeIfNotTested");
const string detailSectionLength("DetailSectionLength");
const string machineNumber("MachineNumber");
const string equipmentIdentifier("EquipmentIdentifier");
const string testResultDetailSectionLength("TestResultDetailSectionLength");
const string faultCodeTag("FaultCode");
const string fieldScale("Scale");
const string fieldOffset("Offset");
const string driveCurve("DriveCurve");
/**
 * Type attribute definitions for field type.
 */
const string typeCode("Code");
const string typeValue("Value");
const string typeDetail("Detail");
const string typeSeparator("Separator");
const string typeSerialNumber("SerialNumber");
const string typeVehicleBuild("VehicleBuild");
const string typeDetailString("DetailString");
const string typeDtc("DTC");
const string typeDtcAttribute("DtcAttribute");
const string typeDtcData("DtcData");
const string typeComponentName("ComponentName");
const string detailNode("DetailNode");
const string dateTimeNode("DateTime");
const string typeConditionalBlock("ConditionalBlock");
const string typeXmlTag("XmlTag");
const string typeMachineSpecificSeperator("MachineSpecificSeparator");
/**
 * Valid data types for map file.
 */
const string typeInt("Integer");
const string typeFloat("Float");
const string typeString("String");
const string typeASCIIInt("ASCIIInt");
/**
 * Value to signal the interface threads to begin processing the test results.
 */
const string processFaults("ProcessFaults");
const string processTestResults("ProcessResults");
const string doNotProcessTestResults("DoNotProcessResults");
/**
 * Default ACK and NAK characters to be used
 */
#define ACK    0x06
#define NAK    0x15
/**
 * Miscellaneous constants to use.
 */
const string sendResultsToHost("Yes");
#define CR    '\x00D'
#define LF    '\x00A'
#define STX   '\x013'

//-----------------------------------------------------------------------------
#endif //PlantHostOutboundDefs_h
