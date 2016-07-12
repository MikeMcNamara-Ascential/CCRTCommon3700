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
//     $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/SideSlipResultServer/SideSlipResultServer.h $
// 
// 3     9/16/05 9:23a Bmeinke
// Changed the Data member of the Ass1300Data_t structure to a
// SideSlipResult_t structure
// 
// 2     9/14/05 4:31p Bmeinke
// Removed the constructor that took command line argument style
// parameters
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _SIDE_SLIP_RESULT_SERVER_H_INCLUDED_
#define _SIDE_SLIP_RESULT_SERVER_H_INCLUDED_

#include "AuxResultServer.h"

/**
 * Start side slip test command (we don't use this)
 */
#define CMD_START	0x01
/**
 * Stop side slip test command (we don't use this)
 */
#define CMD_STOP	0x02
/**
 * Identifies a message as a test result message
 */
#define CMD_RESULT	0x10

// Make sure structs are byte aligned
#include <_pack1.h>

/**
 * Side slip test result format
 */
typedef struct
{
	/**
	 * Front sideslip value
	 */
	float FSideslipValue;
	/**
	 * Rear sideslip value
	 */
	float BSideslipValue;
	/**
	 * Front speed value (reserve)
	 */
	float FSpeedValue; // 
	/**
	 * Rear speed value (reserve)
	 */
	float BSpeedValue;
	/**
	 * Side slip pass/fail result (0=pass, 1=fail)
	 */
	short Determinant;
	/**
	 * Test date
	 */
	char Date[15];
	/**
	 * Test time
	 */
	char Time[15];
	/**
	 * Vin code
	 */
	char Vin[20];

} SideSlipResult_t;

/**
 * Side slip tester test result packet format
 */
typedef struct
{
	/**
	 * Message Header (should be 0x33 0xBB)
	 */
	unsigned char 		Head[2];
	/**
	 * Message Command (should be CMD_RESULT)
	 */
	unsigned char 		Cmd;
	/**
	 * Length of the data block
	 */
	unsigned char 		Datalen;
	/**
	 * Data block (should be a SideSlipResult_t structure)
	 */
	SideSlipResult_t	Data;
} Ass1300Data_t;

// Back to default alignment
#include <_packpop.h>

class SideSlipResultServer : public AuxResultServer
{
public:
		/**
	 * Default constructor. Set debug to false, channel ID's to -1, etc
	 */
	SideSlipResultServer();

	/**
	 * Class destructor
	 */
	virtual ~SideSlipResultServer();

	/**
	 * Parse the given test result string into individual results and
	 * insert each field/value pair as children nodes of the 'resultNode'.
	 * 
	 * @param resultString
	 *                   Test result string to be parsed
	 * @param identifier A unique vehicle identifier (i.e. VIN) that this test is for.
	 * @param resultNode Node to be populated with child nodes representing the result tag/value
	 *                   pairs
	 * 
	 * @return true if the string was fully parsed, false if not fully parsed
	 */
	virtual bool ParseStream( const std::string &resultString, std::string &identifier, XmlElement &resultNode);

	/**
	 * Checks to see if the given buffer contains a full test result string.
	 * If the full string has been received the portion of the string that
	 * was processed is erased from the resultBuff string before returning.
	 * 
	 * @param resultBuff Result string received so far (may or may not be complete)
	 * @param resultString
	 *                   Buffer to put a full result string into (with the "START," and "END"
	 *                   tags removed)
	 * 
	 * @return true if the given string contains a full test result string
	 */
	virtual bool CheckForFullString( std::string &resultBuff, std::string &resultString);
};


#endif // _SIDE_SLIP_RESULT_SERVER_H_INCLUDED_

