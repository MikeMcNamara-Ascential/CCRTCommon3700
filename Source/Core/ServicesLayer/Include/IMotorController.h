//*************************************************************************
// Description:
//   Interface class used to control the motor controller
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
//    $Log: /Core/ServicesLayer/Include/IMotorController.h $
// 
// 3     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 4     8/14/03 10:42a Khykin
// Updated method to pass a pointer instead of reference to a poin
//
// 3     8/13/03 3:55p Khykin
// Changes for new motor controller.
//
// 2     8/12/03 2:49p Khykin
// Updated with new directory update changes.
//
// 1     8/07/03 9:34a Khykin
// Updated with changes from Flatrock.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef IMotorController_h
#define IMotorController_h

#include "IBepCommunication.h"
#include "MotorControllerDefs.h"
#include "XmlNode.h"

/**
 * Class used to communicate with and control the
 * motor controller
 *
 * @author Tim Bochenek
 * @version 1.0
 * @see MotorController
 * @see IBepCommunication
 * @see ElectricMotorController
 * @see HydraulicMotorController
 * @since Version 1.0
 */
class IMotorController : protected IBepCommunication
{
public:
	/**
	 * Class constructor (default)
	 *
	 * @param debug  If debugging information should be enabled
	 * @since Version 1.0
	 */
	IMotorController(const bool debug = false);

    /**
	 * Class destructor
	 * @since Version 1.0
	 */
	virtual ~IMotorController(void);

	/**
	 * Read information from motot controller based on the tag
	 *
	 * @param tag     Tag to read
	 * @param IsFinal If this tag is the las tag to be requested
	 *
	 * @return Status of operation
	 *         Defined return types:
	 *         All tags sent and received Ok                == BEP_STATUS_SUCCESS
	 *         Tag was added to list (waiting for last tag, IsFinal == true) == 1
	 *         Read operation failed to read all tags       == -1
	 *         Read operation failed                        == -2
	 * @since Version 1.0
	 */
	virtual INT32 Read(const std::string &tag, const bool IsFinal = false);

	/**
	 * Read information in the system based on the tag value
	 * passed.
	 *
	 * @param node    Information to be read.
	 * @param IsFinal If this tag is the las tag to be requested
	 *
	 * @return Status of operation
	 *         Defined return types:
	 *         All tags sent and received Ok                == BEP_STATUS_SUCCESS
	 *         Tag was added to list (waiting for last tag, IsFinal == true) == 1
	 *         Read operation failed to read all tags       == -1
	 *         Read operation failed                        == -2
	 * @since Version 1.0
	 */
	virtual INT32 Read(const XmlNode *node, const bool IsFinal = false);

	/**
	 * Write the motor controller to perform a task (change mode, update data)
	 *
	 * @param tag     Command to be executed
	 * @param value   Value of the command
	 * @param IsFinal If this tag is the las tag to be requested
	 *
	 * @return Status of operation
	 * @since Version 1.0
	 */
	virtual INT32 Write(const std::string &tag, const std::string &value, const bool IsFinal = false);

	/**
	 * Write the motor controller to perform a task (change mode, update data)
	 *
	 * @param node    Information to be commanded.
	 * @param IsFinal If this tag is the las tag to be requested
	 *
	 * @return Status of operation
	 * @since Version 1.0
	 */
	virtual INT32 Write(const XmlNode *node, const bool IsFinal = false);

	/**
	 * Retrieves the next tag from the response
	 *
	 * @param tag    Data requested
	 * @param value  Value of data
	 *
	 * @return Number of items left to process
	 * @since Version 1.0
	 */
	virtual const INT32 GetNext(std::string& tag, std::string& value);

	/**
	 * Retrieves the next tag from the response
	 *
	 * @param node   Location where data is to be stored
	 *
	 * @return Number of items left to process
	 * @since Version 1.0
	 */
	const INT32 GetNext(XmlNode &node);

};
#endif // IMotorController_h

