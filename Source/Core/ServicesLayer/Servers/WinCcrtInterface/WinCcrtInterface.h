//*************************************************************************
// Description:
//   Interface to the Windows based CCRT system.
//   This class allows reading and writting data from the Windows based 
//   CCRT system so all data available to the QNX CCRT system can be available
//   to the Windows based CCRT system.  This is mainly intended to assist
//   the migration of CCRT from QNX to Windows.
//
//===========================================================================
// Copyright (c) 2011 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef WinCcrtInterface_h
#define WinCcrtInterface_h
//-------------------------------------------------------------------------------------------------
#include "BepServer.h"
#include "INamedDataBroker.h"
#include "SerialChannel.h"
//-------------------------------------------------------------------------------------------------
class WinCcrtInterface : public BepServer
{
public:
	// Class constructor.
	WinCcrtInterface();

	// Class Destructor.
	virtual ~WinCcrtInterface();

	/**
	 * <b>Description:</b>
	 * Method for initialization passing an XML node.
	 *
	 * <b>Configuration Items:</b>
	 *     <ul>
	 *     <li> Setup/Debug </li>
	 *     <li> Setup/DataTags </li>
	 *     </ul>
	 *
	 * @param document Pointer to a parsed configuration document.
	 */
	virtual void Initialize(const XmlNode *document);

    /**
     * <b>Description:</b>
     * Request to read a data tag.
     *
     * @param node The XML node to be read.
     * @param rate Rate the data should be provided (this is unused)
     * 
     * @return The value of the data tag if it is found.
     *         Otherwise, BEP_UNSUPPORTED_RESPONSE.
     */
    virtual const string Read(const XmlNode *node, const INT32 rate = 0);

	/**
	 * <b>Description:</b>
	 * Method that is called to command the component to perform any system 
	 * registration functions necessary.
	 *
	 * @return BEP_SUCCESS_RESPONSE if the registration was successful
	 *         BEP_FAILURE_RESPONSE if the registration was not successful
	 */
	virtual const string Register(void);

	/**
	 * <b>Description:</b>
	 * Main run method.  This method runs until the status is BEP_TERMINATE.
	 *
	 * @param terminateFlag Flag used to terminate the system.  (this is unused)
	 */
	virtual void Run(volatile bool *terminateFlag=NULL);

	/**
	 * <b>Description:</b>
	 * Request to write a data tag.
	 *
	 * @param node The XML node to be written.
	 * 
	 * @return BEP_SUCCESS_RESPONSE if the new value was written successfully.
	 *         BEP_ERROR_RESPONSE if the data tag was found, but the tag data
	 *         mutex could not be locked.
	 *         BEP_UNAVAILABLE_RESPONSE if the data tag was found and the tag 
	 *         data mutex was locked, but the data could not be written.
	 *         BEP_UNSUPPORTED_RESPONSE if the data tag could not be found.
	 */
	virtual const string Write(const XmlNode *node);



protected:

	/**
	 * <b>Description:</b>
	 * Write the requested data item from the server.
	 *
	 * @param node  The data tag and value to write.
	 * 
	 * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
	 *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
	 *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
	 */
	virtual const string Publish(const XmlNode *node);



private:
	
	/**
	 * Check if there are any messages waiting from Windows CCRT to process.
	 * 
	 * @return Status of checking/processing Windows CCRT messages.
	 */
	BEP_STATUS_TYPE CheckForWinCcrtMessage(void);

	/**
	 * Get/Set the time to wait between checks for incoming messages.
	 * 
	 * @param waitTime Time to wait between checks for incoming messages.
	 * 
	 * @return Time to wait between checks for incoming messages.
	 */
	INT32& MessageWaitTime(const INT32 *waitTime = NULL);

    /**
     * Get/Set the time to wait between checks for incoming messages.
     * 
     * @param waitTime Time to wait between checks for incoming messages.
     * 
     * @return Time to wait between checks for incoming messages.
     */
    INT32& MessageWaitTimeInit(const INT32 *waitTime = NULL);

    /**
     * Get/Set the time to wait between checks for incoming messages.
     * 
     * @param waitTime Time to wait between checks for incoming messages.
     * 
     * @return Time to wait between checks for incoming messages.
     */
    INT32& MessageWaitTimeEnd(const INT32 *waitTime = NULL);


	/**
	 * Process the message received from the Windows CCRT system.
	 * 
	 * @param message Message received from the Windows CCRT system.
	 * 
	 * @return Status of processing the Windows CCRT message.
	 */
	BEP_STATUS_TYPE ProcessWinCcrtMessage(const string &message);

	/**
	 * Read the value of the specified system tag from the system.
	 * 
	 * @param tag    System data tag to read.
	 * 
	 * @return Value of the requested system data tag.
	 */
	string SystemRead(const string &tag);

	/**
	 * Write the specified value to the specified system data tag.
	 * 
	 * @param tag    System data tag to write.
	 * @param value  Value to write to the system data tag.
	 * 
	 * @return Status of writing the value to the system data tag.
	 */
	BEP_STATUS_TYPE SystemWrite(const string &tag, const string &value);

	/**
	 * Get/Set the type of test that was selected.
	 * 
	 * @param type   The type of test that was selected.
	 * 
	 * @return The type of test that was selected.
	 */
	string& TestType(const string *type = NULL);

	/**
	 * Get the flag to determine if the test type should be updated.
	 * 
	 * @return Flag indicating if the test type should be updated:
	 *         - True = update the test type to the Windows Input Server
	 *         - False = do not update the test type to the Windows Input Server.
	 */
	const bool& UpdateTestType();

	/**
	 * Set the flag indicating if the test type should be updated to the Windows Input Server.
	 * 
	 * @param updateType Flag indicating if the test type should be updated to the Windows Input Server:
	 *                   - True = update the test type
	 *                   - False = do not update the test type.
	 */
	void UpdateTestType(const bool &updateType);

	// Flag to indicate if the test type should be updated.
	bool m_updateTestType;
	// Time to wait between checks for incoming messages
	INT32 m_messageWaitTime;
    // Time to wait between checks for incoming messages
    INT32 m_messageWaitTimeInit;
    // Time to wait between checks for incoming messages
    INT32 m_messageWaitTimeEnd;
	// Type of test that was selected
	string m_testType;
	// Notes entered by the operator.
	string m_notes;
	// INamedDataBroker object to use for reading and writing system level data tags
	INamedDataBroker *m_ndb;
	// Communication object to get/send data to Windows CCRT processes.
	SerialChannel m_winCcrtComm;
	// Windows CCRT system communication configuration
	XmlNode *m_winCcrtCommConfig;
	// Belt tensions received from the floor interface system
	XmlNodeMap m_beltTensions;
};

//-------------------------------------------------------------------------------------------------
#endif //WinCcrtInterface_h
