//*************************************************************************
// Description:
//   Motor object that is used to command a Baldor motor.
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
//    $Log: /Working/Source/Core/ServicesLayer/Servers/MotorController/Electric/Baldor/BaldorMotor.cpp $
// 
// 1     5/11/05 10:06p Cward
// 
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
// 
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:41p Cward
// 2     2/18/05 12:55p Bmeinke
// Changed the loop limit in SendCommand() from "message.length()" to
// "response.length()"
//
// 2     7/09/04 2:16p Bmeinke
// Derived from the new SerialMotor class (added to the inheritance tree
// to remove serial dependencies from the base MotorClass)
//
// 3     6/17/04 11:26p Bmeinke
// Changed parent class from Motor to SerialMotor
//
// 2     6/01/04 9:31p Bmeinke
// Added port locking around serial comm with the motors
//
// 5     10/24/03 1:45p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 4     8/13/03 3:06p Khykin
// Updated with changes from Flatrock.
//
// 19    6/24/03 11:00a Tbochene
// Updated due changes in CreateMessage()
//
// 18    5/20/03 2:10p Tbochene
// Updated to use new inheritance structure
//
// 17    5/08/03 6:40p Tbochene
// Removed fault server interface and reporting (PlcMgr does this) and
// updated enabling communication fault reporting to report based on a
// threshhold of faults before it gets reported
//
// 16    5/08/03 1:49p Tbochene
// Added parameters to enable/disable thread lockup detection and setting
// of communication faults.  Changes to remove race conditions causing
// segmentation faults.
//
// 1     4/25/03 10:21a Tbochene
// My latest files
//
// 14    4/10/03 3:37p Tbochene
// Latest Motor Controller changes
//
// 13    4/08/03 4:30p Tbochene
// Updated for robustness and imporved diagnostics
//
// 12    3/28/03 9:54a Tbochene
// Lastest files after initial testing
//
// 11    3/26/03 2:36p Tbochene
// Remove old baggage
//
// 10    3/26/03 11:19a Tbochene
// Updated for robustness
//
// 8     11/27/02 8:52a Tbochene
// Cleaned up mess from debugging
//
// 7     11/19/02 9:38p Tbochene
// Updaed diagnostics and changed semaphore names to be more descriptive
// (contain motor name).  Fixed drive type detection.  Made line feed
// apped option only available when compiling with TEST_COMM enabled.
//
// 6     11/08/02 12:07p Tbochene
// Added Set/GetFaultStatus() methods and converted to use SerialString_t
// instead of SERIAL_STRING.  Moved Launch() and MotorThread() from
// MotorController.
//
// 5     10/14/02 4:39p Tbochene
// Updated Handling of respomses to commands and debugging information
//
// 4     9/24/02 12:12p Tbochene
// Removed compiler warnings in many places
//
// 3     9/24/02 7:36a Khykin
// Making changes to occomodate configuration changes.
//
// 2     9/10/02 3:40p Khykin
// Updating for Tim to take to CTC.
//
// 1     9/06/02 10:42a Khykin
// Initial checkin.
//
// 4     8/23/02 12:03p Kh
// Updating for changes at the pit machine.
//
// 3     5/03/02 5:17p Kh
// Updating prior to testing.
//
// 2     4/12/02 2:31p Kh
// Final draft prior to debug.
//
// 1     4/03/02 1:24p Kh
// Initial Checkin.
//
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#include "BaldorMotor.h"

BaldorMotor::BaldorMotor() : SerialMotor()
{
}
BaldorMotor::~BaldorMotor(void)
{	// Delete the interfaces if they exist
	Terminate();
}

/**
 * Upload the specified drive parameters to the motor.
 *
 * @param parameters The parameters to upload.
 * @return The status of the operation.
 * @since Version 1.0
 */
const INT32 BaldorMotor::UploadDriveParameters(const XmlNode *parameters)
{
	INT32 status = EOK, retStatus = status;
	Motor::Log(LOG_FN_ENTRY, "BaldorMotor::UploadDriveParameters(%s)\n", parameters->ToString().c_str());
	try
	{	// determine what type of drive we are using, and retrieve the parameters
		std::string tempString, driveParameters = BposGetEnv("FTP_ROOT") + parameters->getValue();
		Motor::Log(LOG_DEV_DATA, "Parameter file = %s\n", driveParameters.c_str());

		// load and parse the file
		XmlParser driveParser;
		const XmlNode* parameters = driveParser.ReturnXMLDocument(driveParameters);

		// if there were parameters to send
		if(parameters->getChildren().size() > 0)
		{	// upload the drive parameters
			char buffer[MOTOR_BUF_SZ];
			const XmlNode* level, *block, *param;
			for(UINT32 ii = 0; ii < parameters->getChildren().size(); ii++)
			{	// walk throught the levels
				level = parameters->getChildren().getNode(ii);
				Motor::Log(LOG_DEV_DATA, "Level Number: %s\n", level->getName().c_str());
				for(UINT32 jj = 0; jj < level->getChildren().size(); jj++)
				{	// walk through the blocks
					block = level->getChildren().getNode(jj);
					Motor::Log(LOG_DEV_DATA, "Block Number: %s\n", block->getName().c_str());
					for(UINT32 kk = 0; kk < block->getChildren().size(); kk++)
					{	// send the parameter
						param = block->getChildren().getNode(kk);
						Motor::Log(LOG_DEV_DATA, "Parameter Number: %s\n", param->getName().c_str());
						Motor::Log(LOG_DEV_DATA, "Description: %s\n", param->getAttribute("Description")->getValue().c_str());

						tempString = CreateMessage(buffer, sizeof(buffer), "%s %s\r", param->getName().c_str(), param->getValue().c_str());

						Motor::Log(LOG_DEV_DATA, "Sending: '%s'\n", tempString.c_str());
						if((status = SendVariableCommand(tempString, param->getName())) == EOK)
						{
							Motor::Log(LOG_DEV_DATA, "Sent %s\n", tempString.c_str());
						}
						else Motor::Log(LOG_ERRORS, "Failed to send %s because: %s\n", tempString.c_str(), strerror(errno));
					}
				}
			}
		}
		else
		{
			status = EIO;
			Motor::Log(LOG_ERRORS, "%s can not UploadDriveParameters, due they do not exist\n", GetName().c_str());
		}
	}
	catch(BepException &Err)
	{
		status = EIO;
		Motor::Log(LOG_ERRORS, "%s Error Uploading Drive Parameters: %s\n", Err.what());
	}
	Motor::Log(LOG_FN_ENTRY, "BaldorMotor::UploadDriveParameters() done\n");
	return(retStatus);
}


/**
 * Enable the motor.  This must be done prior to commanding the
 * motor.
 *
 * @param enable Whether to enable (true) or disable (false) the drive
 *
 * @return The status of the motor enabling.
 * @since Version 1.0
 */
const INT32 BaldorMotor::Enable(const bool enable)
{	// send the enable command
	Motor::Log(LOG_FN_ENTRY, "SerialMotor::Enable(%d)\n", enable);
	INT32 status = EOK, retStatus = status;
	SerialString_t reply;	// Reply message
	std::string message = enable ? "Enable" : "Disable";

	if(enable)
	{	// If enabling drive check fault
		std::string message = "CheckFault";

		if((retStatus = SendCommand(message)) == EOK)
		{	// If the drive is faulted
			Motor::Log(LOG_DEV_DATA, "%s is faulted\n", GetName().c_str());

			message = "ReadFaultDescription";
			if(WritePort(message))
			{
				SerialString_t buffer;
				if(ReadPort(buffer, 10, 10) > 0)
				{	// We read some data
					std::string faultDescription = (char*)buffer.c_str();

					if((retStatus = SendCommand("ClearFault")) == EOK)
					{	// Clear fault message sent
						Motor::Log(LOG_DEV_DATA, "%s fault cleared\n", faultDescription.c_str());
					}
					else Motor::Log(LOG_ERRORS, "Failed to send message %s because: %s\n", message.c_str(), strerror(retStatus));
				}
				else Motor::Log(LOG_ERRORS, "Failed to read message %s because: %s\n", message.c_str(), strerror(errno));
			}
			else Motor::Log(LOG_ERRORS, "Failed to send message %s because: %s\n", message.c_str(), strerror(errno));
		}
		else Motor::Log(LOG_ERRORS, "Failed to send message %s because: %s\n", message.c_str(), strerror(retStatus));
	}

	message = enable ? "Enable" : "Disable";

	if((retStatus == EOK) || (enable == false))
	{	// If fault properly handled or disabling drive
		retStatus = SendCommand(message);
	}

	Motor::Log(LOG_DEV_DATA, "%s%s %sd\n", GetName().c_str(), (status != EOK) ? " not": "", message.c_str());

	Motor::Log(LOG_FN_ENTRY, "SerialMotor::Enable(%d) done\n", enable);

	return(retStatus);
}
/**
 * Read the type of drive that the motor is commanding.
 *
 * @return The drive type.
 *         S18 - Baldor 18h
 *         S25 - Baldor 25m
 */
const std::string BaldorMotor::ReadDriveType(void)
{
	INT32 status = EOK;
	std::string driveType, message("Status");

	Motor::Log(LOG_FN_ENTRY, "SerialMotor::ReadDriveType(%s)\n", GetName().c_str());

	if(WritePort(message) > 0)
	{
		UINT8 buffer[256];
		INT32 bytesRead = 0;
		memset(buffer, 0, sizeof(buffer));

		if((bytesRead = ReadPort(buffer, sizeof(buffer) - 1, 10, 10)) > 0)
		{
			std::string reply((char*)buffer);

			if(reply.find("S18"))
			{
				Motor::Log(LOG_DEV_DATA, "%s Drive type is 18H\n", GetName().c_str());
			}
			else if(reply.find("S25"))
			{
				Motor::Log(LOG_DEV_DATA, "%s Drive type is 25M\n", GetName().c_str());
			}
			else
			{
				status = ENOMSG;
				Motor::Log(LOG_ERRORS, "Failed to find revision in message '%s'\n", reply.c_str());
			}
		}
		else
		{
			status = errno;
			Motor::Log(LOG_ERRORS, "Failed to get response to %s because: %s\n", message.c_str(), strerror(status));
		}
	}
	else
	{
		status = errno;
		Motor::Log(LOG_ERRORS, "Failed to send message %s because: %s\n", message.c_str(), strerror(status));
	}

	SetCommFault(status != EOK);

	return(driveType);
}

const INT32 BaldorMotor::SendCommand(const std::string& message)
{
	SerialString_t response;
	INT32 status = EOK, bytes;

	Motor::Log(LOG_FN_ENTRY, "BaldorMotor::SendCommand(%s)\n", message.c_str());

	if( LockPort() == true)
	{
	ResetConnection();
	if((bytes = WritePort(message)) > 0)
	{	// Successfully wrote message
		INT32 bytes;
			Motor::Log(LOG_DEV_DATA, "%s message sent\n", message.c_str());
		if((bytes = ReadPort(message, response)) > 0)
		{	// Got the response Ok
			LogSerialString(LOG_DEV_DATA, message + " reply: ", response);

			if(!IsPositiveResponse(message, response)) status = EBADMSG;
			if(GetCommand() == MOTOR_STATUS_ENQUIRE) SetEnquiryResponse(response);
				Motor::Log(LOG_DEV_DATA, "Response was %s\n", (status == EOK) ? "Positive" : "Negative");
			}
		else
		{
			status = errno;
			char buffer[16];
			std::string temp;
				for(UINT32 ii = 0; ii < response.length(); ii++)
				{
				CreateMessage(buffer, sizeof(buffer), "0x%02X ", response[ii]);
				temp += buffer;
			}
				Motor::Log(LOG_ERRORS, "Failed to get %s response because: %s\n", message.c_str(), strerror(status));
				Motor::Log(LOG_ERRORS, "Response: <%s>\n", temp.c_str());
			}
		}
	else
	{
		status = errno;
			Motor::Log(LOG_ERRORS, "Failed to send %s to %s because: %s\n", message.c_str(), GetName().c_str(), strerror(status));
			Motor::Log(LOG_ERRORS, "%d bytes sent\n", bytes);
		}

		UnlockPort();
	}
	else
	{
		status = EPERM;
		Motor::Log(LOG_ERRORS, "Unable to lock port in order to send %s to %s\n", message.c_str(), GetName().c_str());
	}

	SetCommFault(status != EOK);
	Motor::Log(LOG_FN_ENTRY, "BaldorMotor::SendCommand(%s) done\n", message.c_str());

	return(status);
}
const INT32 BaldorMotor::SendVariableCommand(const std::string message, const std::string tag, const INT32 readResponseDelay) /* = 0 */
{
	std::string temp;
	INT32 status = EOK, bytes;
	char buffer[MOTOR_BUF_SZ];
	SerialString_t response;

	Motor::Log(LOG_FN_ENTRY, "BaldorMotor::SendVariableCommand(%s, %s)\n", message.c_str(), tag.c_str());

	if(Motor::GetLogStatus() && Motor::GetLocalEcho() && (Motor::GetVerboseMask() & LOG_DEV_DATA))
	{
		for(UINT32 ii = 0; ii < message.length(); ii++)
		{
			CreateMessage(buffer, sizeof(buffer), "0x%02X ", message[ii]);
			temp += buffer;
		}
		Motor::Log(LOG_DEV_DATA, "Message to send: %s\n", temp.c_str());
	}

	if( LockPort() == true)
	{
	ResetConnection();
	if((bytes = WritePort((UINT8*)message.c_str(), message.length())) == (INT32)message.length())
	{
			Motor::Log(LOG_DEV_DATA, "%s message sent\n", tag.c_str());
		BposSleep(readResponseDelay);
		if((bytes = ReadPort(tag, response)) > 0)
		{
				Motor::Log(LOG_DEV_DATA, "%s response recveived\n", tag.c_str());
			if(!IsPositiveResponse(message, response)) status = EBADMSG;
				Motor::Log(LOG_DEV_DATA, "Response was %s\n", (status == EOK) ? "Positive" : "Negative");
			}
		else
		{
			status = errno;
			temp.erase();	// Clear string
			for(UINT32 ii = 0; ii < message.length(); ii++)
			{
				CreateMessage(buffer, sizeof(buffer), "0x%02X ", message[ii]);
				temp += buffer;
			}
				Motor::Log(LOG_ERRORS, "Response: %s\n", temp.c_str());
				Motor::Log(LOG_ERRORS, "Failed to get %s response because: %s\n", tag.c_str(), strerror(status));
			}
		}
	else
	{
		status = errno;
			Motor::Log(LOG_ERRORS, "Failed to send %s to %s because: %s\n", tag.c_str(), GetName().c_str(), strerror(status));
			Motor::Log(LOG_ERRORS, "%d bytes sent\n", bytes);
		}

		UnlockPort();
	}
	else
	{
		status = EPERM;
		Motor::Log(LOG_ERRORS, "Unable to lock port in order to send %s to %s\n", message.c_str(), GetName().c_str());
	}

	SetCommFault(status != EOK);

	Motor::Log(LOG_FN_ENTRY, "BaldorMotor::SendVariableCommand(%s, %s) done\n", message.c_str(), tag.c_str());
	return(status);
}

/**
 * Get the string representing how a speed command should be built
 *
 * @return Speed string
 */
const char* BaldorMotor::GetTorqueCommand(void)
{
	return(TORQUE_COMMAND);
}
/**
 * Get the string representing how a torque command should be built
 *
 * @return Torqu string
 */
const char* BaldorMotor::GetSpeedCommand(void)
{
	return(RPM_COMMAND);
}

