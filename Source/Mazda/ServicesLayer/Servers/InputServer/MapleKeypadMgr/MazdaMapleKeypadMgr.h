//*************************************************************************
// FILE DESCRIPTION:
//  The MazdaMapleKeypadMgr handles vehicle data type inputs and publishes 
//  the data to objects that need the data.
//
//===========================================================================
// Copyright (c) 20013- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef MazdaMapleKeypadMgr_h
#define MazdaMapleKeypadMgr_h
//-------------------------------------------------------------------------------------------------
#include "MapleKeypadManager.h"
//-------------------------------------------------------------------------------------------------
class MazdaMapleKeypadMgr : public MapleKeypadManager
{
public:

	MazdaMapleKeypadMgr();
	virtual ~MazdaMapleKeypadMgr();

	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag and data that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 */
	virtual const string Publish(const XmlNode *node);

	/**
	 * Main Run method to control the process
	 *
	 * @param terminateFlag
	 *               Flag used to terminate the system.
	 */
	virtual void Run(volatile bool *terminateFlag=NULL);






protected:

	/**
	 * Load the additional configuration items needed by the MapleKeypadManager.
	 * The "standard" items will be handled by the call to
	 * InputDeviceBase::Initialize().
	 *
	 * @param configNode A pointer to the MapleKeypadManager config node
	 */
	virtual void LoadAdditionalConfigurationItems(const XmlNode *configNode);






private:

	/**
	 * Get any input from the keypad from the user.
	 * 
	 * @param inputData  Buffer to store the received data bytes into.
	 * @param bufferSize Size of the input data buffer.
	 * 
	 * @return Number of bytes received from the keypad.
	 */
	INT32 GetKeypadInput(UINT8 *inputData, INT32 bufferSize);

	/**
	 * Get/Set the flag indicating the machine is in half-auto mode.
	 * 
	 * @param inHalfAuto Flag indicating the machine is in half-auto mode.
	 * 
	 * @return Flag indicating the machine is in half-auto mode.
	 */
	const bool& InHalfAutoMode(const bool *inHalfAuto = NULL);

	/**
	 * Run through getting data from the user if the machine is in 
	 * half auto mode.
	 */
	void ProcessHalfAutoModeInputs(UINT8 *inputData, INT32 byteCount);

	// Flag to track if the machine has been placed in half-auto mode
	bool m_inHalfAutoMode;

	// Vehicle build items to be entered by the operator when in half auto mode
	XmlNodeMap m_halfAutoItems;
};
//-------------------------------------------------------------------------------------------------
#endif //MazdaMapleKeypadMgr_h
