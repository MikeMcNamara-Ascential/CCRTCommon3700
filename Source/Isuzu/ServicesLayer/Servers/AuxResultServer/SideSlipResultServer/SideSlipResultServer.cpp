//******************************************************************************
// Description:
//  Test result parser for side slip test results
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/SideSlipResultServer/SideSlipResultServer.cpp $
// 
// 6     11/17/05 5:13p Bmeinke
// Initialize various variables to remove compiler warnings
// 
// 5     9/27/05 8:18a Bmeinke
// Modified CheckForFullString() to llok for the { 0x33, 0xBB, 0x10 }
// header block and the number of bytes following this header block to
// determine if a full result has been received
// 
// 4     9/16/05 9:23a Bmeinke
// Changed the Data member of the Ass1300Data_t structure to a
// SideSlipResult_t structure
// Added more Log() statements to ParseStream() and CheckForFullString()
// 
// 3     9/14/05 4:31p Bmeinke
// Removed the constructor that took command line argument style
// parameters
// 
// 2     9/08/05 12:43p Bmeinke
// CheckForFullString now removes the portion of the stream that needs to
// be parsed
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "SideSlipResultServer.h"

SideSlipResultServer::SideSlipResultServer() : AuxResultServer()
{
}

SideSlipResultServer::~SideSlipResultServer()
{
}

bool SideSlipResultServer::ParseStream( const std::string &resultString, std::string &identifier, XmlElement &resultNode)
{
	char						fieldValue[256];
	std::string					fieldTag;
	std::string 				vinStr;
	AuxResultFieldListCItr_t	fieldItr=m_resultTags.begin();
	const Ass1300Data_t	   		*mssgPacket = NULL;
	const SideSlipResult_t		*testResult;
	bool						retVal = false;

	Log( LOG_FN_ENTRY, "Enter SideSlipResultServer::ParseStream()\n");

	Log( LOG_DEV_DATA, "%d bytes of data to process\n", resultString.size());
	// If the buffer is big enough to hold a 
	if( resultString.size() >= sizeof(Ass1300Data_t))
	{
		// Get the full packet data
		mssgPacket = (const Ass1300Data_t*)resultString.c_str();

		Log( LOG_DEV_DATA, "Have a header that says %d bytes total\n", mssgPacket->Datalen);
		// Get the test result data
		testResult = &mssgPacket->Data;

		// Add Front Side Slip value
		///////////////////////////
		sprintf( fieldValue, "%.02f", testResult->FSideslipValue);
		// get the tag for this Aux result field
		if( fieldItr != m_resultTags.end())	fieldTag = (fieldItr++)->GetFieldTag();
		else								fieldTag = "FrontSideSlip";
		Log( LOG_DEV_DATA, "\t%s = %s\n", fieldTag.c_str(), fieldValue);
		(void)resultNode.addChild( fieldTag, fieldValue, ELEMENT_NODE);

		///////////////////////////
		// Add Rear Side Slip value
		///////////////////////////
		sprintf( fieldValue, "%.02f", testResult->BSideslipValue);
		if( fieldItr != m_resultTags.end())	fieldTag = (fieldItr++)->GetFieldTag();
		else								fieldTag = "RearSideSlip";
		Log( LOG_DEV_DATA, "\t%s = %s\n", fieldTag.c_str(), fieldValue);
		(void)resultNode.addChild( fieldTag, fieldValue, ELEMENT_NODE);

		///////////////////////////
		// Add Front speed value
		///////////////////////////
		sprintf( fieldValue, "%.02f", testResult->FSpeedValue);
		if( fieldItr != m_resultTags.end())	fieldTag = (fieldItr++)->GetFieldTag();
		else								fieldTag = "FrontSpeed";
		Log( LOG_DEV_DATA, "\t%s = %s\n", fieldTag.c_str(), fieldValue);
		(void)resultNode.addChild( fieldTag, fieldValue, ELEMENT_NODE);

		///////////////////////////
		// Add Rear speed value
		///////////////////////////
		sprintf( fieldValue, "%.02f", testResult->BSpeedValue);
		if( fieldItr != m_resultTags.end())	fieldTag = (fieldItr++)->GetFieldTag();
		else								fieldTag = "RearSpeed";
		Log( LOG_DEV_DATA, "\t%s = %s\n", fieldTag.c_str(), fieldValue);
		(void)resultNode.addChild( fieldTag, fieldValue, ELEMENT_NODE);

		///////////////////////////
		// Add pass/fail result
		///////////////////////////
		if( 0 == testResult->Determinant)	strcpy( fieldValue, BEP_PASS_RESPONSE);
		else								strcpy( fieldValue, BEP_FAIL_RESPONSE);
		if( fieldItr != m_resultTags.end())	fieldTag = (fieldItr++)->GetFieldTag();
		else								fieldTag = "SideSlipResult";
		Log( LOG_DEV_DATA, "\t%s = %s\n", fieldTag.c_str(), fieldValue);
		(void)resultNode.addChild( fieldTag, fieldValue, ELEMENT_NODE);

		///////////////////////////
		// Add test date
		///////////////////////////
		memset( fieldValue, 0, sizeof( fieldValue));
		strncpy( fieldValue, testResult->Date, sizeof(testResult->Date));
		if( fieldItr != m_resultTags.end())	fieldTag = (fieldItr++)->GetFieldTag();
		else								fieldTag = "TestDate";
		Log( LOG_DEV_DATA, "\t%s = %s\n", fieldTag.c_str(), fieldValue);
		(void)resultNode.addChild( fieldTag, fieldValue, ELEMENT_NODE);

		///////////////////////////
		// Add test time
		///////////////////////////
		memset( fieldValue, 0, sizeof( fieldValue));
		strncpy( fieldValue, testResult->Time, sizeof(testResult->Time));
		if( fieldItr != m_resultTags.end())	fieldTag = (fieldItr++)->GetFieldTag();
		else								fieldTag = "TestTime";
		Log( LOG_DEV_DATA, "\t%s = %s\n", fieldTag.c_str(), fieldValue);
		(void)resultNode.addChild( fieldTag, fieldValue, ELEMENT_NODE);

		///////////////////////////
		// Add VIN
		///////////////////////////
		memset( fieldValue, 0, sizeof( fieldValue));
		strncpy( fieldValue, testResult->Vin, sizeof(testResult->Vin));
		if( fieldItr != m_resultTags.end())	fieldTag = (fieldItr++)->GetFieldTag();
		else								fieldTag = "Vin";
		Log( LOG_DEV_DATA, "\t%s = %s\n", fieldTag.c_str(), fieldValue);
		(void)resultNode.addChild( fieldTag, fieldValue, ELEMENT_NODE);

		// Return the test result VIN
		identifier = std::string( fieldValue);
		
		retVal = true;
	}
	else
	{
		Log( LOG_DEV_DATA, "Not enough data for full result block (%d < %d)\n", 
			 resultString.size(), sizeof(SideSlipResult_t));
	}
	Log( LOG_FN_ENTRY, "Enter SideSlipResultServer::ParseStream(%s)\n", identifier.c_str());

	return( retVal);
}

bool SideSlipResultServer::CheckForFullString( std::string &resultBuff, std::string &resultString)
{
	const char	startBytes[] = { 0x33, 0xBB, CMD_RESULT }; 
	uint32_t	mssgStart;
	bool		retVal = false;
	
	Log( LOG_FN_ENTRY, "Enter SideSlipResultServer::CheckForFullString(), bytes=%d of %d\n", 
		 resultBuff.size(), sizeof(Ass1300Data_t));

	// Look for the result block header signature
	mssgStart = resultBuff.find( startBytes, 0, sizeof( startBytes));
	if( resultBuff.npos != mssgStart)
	{
		Log( LOG_DEV_DATA, "Found side slip result header start signature at byte %d\n",
			 mssgStart);

		// Trim off any leading junk
		if( 0 != mssgStart)
		{
			Log( LOG_DEV_DATA, "Trimming off %d bytes of leading junk from result string\n",
				 mssgStart);
			resultBuff.erase( 0, mssgStart);
			mssgStart = 0;
		}

		// If the buffer contains enough data for a full 
		if( (resultBuff.size()) >= sizeof(Ass1300Data_t))
		{
			Log( LOG_DEV_DATA, "Full result block received (%d bytes of %d bytes)\n", 
				 resultBuff.size(), sizeof( Ass1300Data_t));

			// Extract the full result block
			resultString = resultBuff.substr( 0, sizeof(Ass1300Data_t));
			// Erase this result from the buff
			resultBuff.erase( 0, sizeof(Ass1300Data_t));
			retVal = true;
		}
		else
		{
			Log( LOG_DEV_DATA, "Full result block not received yet (%d bytes of %d bytes)\n", 
				 resultBuff.size(), sizeof( Ass1300Data_t));
		}
	}
	Log( LOG_FN_ENTRY, "Exit SideSlipResultServer::CheckForFullString(%d)\n", retVal);

	return( retVal);
}
