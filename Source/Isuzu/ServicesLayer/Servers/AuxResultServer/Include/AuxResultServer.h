//******************************************************************************
// Description:
//  Server used to hold the test result string received from an external tester
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
//     $Log: /Tata/Source/Tata/ServicesLayer/Servers/AuxResultServer/Include/AuxResultServer.h $
// 
// 5     3/09/06 1:00a Bmeinke
// Utilitize the new AuxResultFieldList m_visibility member to remove
// result fields from the test results we return to clients
// Added logic to put test result flag field values (Pass/Fail) into a
// "Result" attribute node of the associated data field
// 
// 4     9/16/05 9:28a Bmeinke
// Added a 'recreate' parameter to the OpenResultFile() method in order to
// completely overwrite the file when a new record comes in
// 
// 3     9/14/05 4:34p Bmeinke
// Removed the constructor that took command line argument style
// parameters
// Added m_portName to define which logical port the results will com in
// in
// Added an overloaded Terminate() method in order to shutdown the
// receiver thread
// Added an overloaded Register() method in order to launch the recevier
// thread
// 
// 2     9/08/05 12:42p Bmeinke
// Fixed comment for ProcessStream
// 
// 1     9/08/05 10:40a Bmeinke
// Initial checkin for new server architecture
// 
// 3     8/22/05 2:31p Bmeinke
// Changed ResultTagList_t to HLAResultFieldList_t and moved to its own
// header
// HLAResultFieldList_t now hold a list of HLAResultField objects instead
// of just string tags
// 
// 2     8/19/05 4:36p Bmeinke
// Added methods for loading and saving the HLA results to a file
// Added ability to read HLA test result via the Read() method
// 
// 1     8/18/05 3:43p Bmeinke
// Initial check-in (not compiled)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

class AuxResultServer;

#ifndef _AUX_RESULT_SERVER_H_INCLUDED_
#define _AUX_RESULT_SERVER_H_INCLUDED_

/*******************
 include files
 ******************/
#include "BepDefs.h"
#include "BepServer.h"
#include "INamedDataBroker.h"
#include "IPromptServer.h"
#include "IFaultServer.h"
#include "BepTimer.h"
#include "BepSemaphore.h"
#include "AuxResultReceiverThread.h"
#include "AuxResultFieldList.h"

/**
 * Object used to receive and publish test result strings from an external tester
 */
class AuxResultServer : public BepServer
{
	friend class AuxResultReceiverThread;
	
public:
	/**
	 * Default constructor. Set debug to false, channel ID's to -1, etc
	 */
	AuxResultServer();

	/**
	 * Class destructor
	 */
	virtual ~AuxResultServer();

	/**
	 * Method for initialization.
	 *
	 * @param document Reference to a parsed configuration document.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);
	/**
	 * Method used to terminate the component.  This performs
	 * any cleanup that the destructor would not and sets
	 * the state of the component to terminate.
	 * @since Version 1.0
	 */
	virtual void Terminate(void);
	/**
	 * Request to read a data tag
	 *
	 * @param node    Data to be read
	 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
	 * @return Value of tag
	 * @since Version 2.0
	 */
	virtual const std::string Read(const XmlNode *node, const INT32 rate);
	/**
	 * Request to write a data tag
	 *
	 * @param node    Data to be written
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 2.0
	 */
	virtual const std::string Write(const XmlNode *node);
	/**
	 * Request to Perform an action
	 *
	 * @param node    Action to be performed
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 2.0
	 */
	virtual const std::string Command(const XmlNode *node);
	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
	 *		   BEP_SUCCESS_RESPONSE == Registered
	 * @since Version 2.7
	 */
	virtual const std::string Register(void);
	
	/**
	 * Processes the given test result string which may or may not be a
	 * complete string.
	 * 
	 * @param resultStream
	 *               Test result string to be parsed
	 * 
	 * @return true if the string was fully parsed, false if not fully parsed
	 */
	virtual bool ProcessStream( std::string &resultStream);

	/**
	 * Copies pass/fail flags from result fields to their associated data
	 * fields and removes any privately scoped fields
	 * 
	 * @param resultNode The XML node containing the test result fields to be processed
	 */
	virtual void FinalizeResultRecord( XmlNode *resultNode);
	
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
	virtual bool ParseStream( const std::string &resultString, std::string &identifier, XmlElement &resultNode) = 0;

	/**
	 * Checks to see if the given buffer contains a full test result string.
	 * If the full string has been received the portion of the string that
	 * was processed shoud be  erased from the resultBuff string before returning.
	 * 
	 * @param resultBuff Result string received so far (may or may not be complete)
	 * @param resultString
	 *                   Buffer to put a full result string into (with the "START," and "END"
	 *                   tags removed)
	 * 
	 * @return true if the given string contains a full test result string
	 */
	virtual bool CheckForFullString( std::string &resultBuff, std::string &resultString) = 0;

protected:
	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag and data that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 * @since Version 2.1
	 */
	virtual const std::string Publish(const XmlNode *node);

	/**
	 * Method called by the receiver thread whe a new test result
	 * is received.
	 * 
	 * @param resultNode An XML node containing the external results. The node value is the VIN
	 *                   from the tested vehicle with individual results contained as child
	 *                   nodes
	 * @param vinStr     The VIN this test result is for
	 */
	virtual void NewRecordReceived( const XmlNode *resultNode, const std::string &vinStr);

	/**
	 * Increments the "Index" attribute of each result node by 1
	 */
	void RotateResultNodes();

	/**
	 * Loads the test result file from disk
	 */
	void ReadResultFile();
	
	/**
	 * Writes the current result list to the result file
	 */
	void WriteResultFile();

	/**
	 * Opens the result file creating all necessary directories
	 * 
	 * @param recreate Flag indicating if we want to re-create the file (i.e. overwrite it) or not
	 * 
	 * @return Valid file descriptor to the file if successful
	 *         -1 if the file could not be opened
	 */
	int OpenResultFile(bool recreate=false);

	/**
	 * Creates any missing directories listed in the given full file path
	 * 
	 * @param fileName Full name of the file with directory information
	 * 
	 * @return 0 if successfuk
	 *         -1 if an error occurred
	 */
	int CreateFilePath( const std::string &fileName);
	
	/**
	 * List of tags to use for parsed results. The tags appear in this
	 * list in the same order as their corresponding values appear in the
	 * result string;
	 */
	AuxResultFieldList_t	m_resultTags;

	/**
	 * Thread used to receive the result strings
	 */
	AuxResultReceiverThread	*m_receiverThread;

	/**
	 * Object used to maintain a list of results sorted by vehicle VIN 
	 */
	XmlNode					*m_resultMap;

	/**
	 * Maximum number of results to keep in the m_resultMap
	 */
	uint32_t				m_maxResultMapSize;

	/**
	 * Name of the file to save the results into
	 */
	std::string				m_resultFileName;

	/**
	 * The system tag clients will read to get the auxiliary results
	 */
	std::string				m_resultTag;

	/**
	 * Logical port to receive result on
	 */
	std::string				m_portName;
};



#endif // _AUX_RESULT_SERVER_H_INCLUDED_

