//******************************************************************************
// Description:
//   MES Host Interface base lass to handle copying the test result file to the
//   special directory so it can be processed and placed on the MES network.
//
//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef MesInterface_h
#define MesInterface_h
//-------------------------------------------------------------------------------------------------
#include "HostInterface.h"

class MesInterface : public HostInterface
{
public:

	MesInterface();
	virtual  ~MesInterface();

    /**
     * Initialize the host interface using the supplied command line arguments.
     * The command line argumets will be parsed and used for initializing the 
     * host interface.
     * 
     * @param argc   Count of the command line arguments.
     * @param argv Command line arguments to parse and use for initializing the 
     *             host interface.
     */
    virtual void Initialize(int argc, char *argv[]);

    /**
     * Initialization method that retrieves the xml document.
     * 
     * @param fileName Configuration file which contains the object setup
     *                 information.
     */
    virtual void Initialize(const string &fileName);

	/**
     * Method for initialization that is passed a parsed configuration file.
     *
     * @param document Configuration data for setting up the interface.
     */
    virtual void Initialize(const XmlNode *document);

	/**
	 * Main Run method to control the process
	 *
	 * @param terminateFlag Flag used to terminate the system.
	 */
	virtual void Run(volatile bool *terminateFlag=NULL);

	/**
	 * <b>Description:</b>
	 * Request to write a data tag.
	 *
	 * @param node The data tag and new value to be written.
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
	 * Pop the first test result off of the list of test result files.
	 * 
	 * @return Name of the test result file name popped from the list.
	 */
	string PopTestResult(void);

	/**
	 * Push a test result file name onto the list of files to process.
	 * 
	 * @param fileName Name of the test result file to be processed.
	 */
	void PushTestResult(const string &fileName);



private:

	// List of result files to process
	vector<string> m_resultFilesToProcess;
	   /**
     * Vector to hold the pointers to the test results to publish
     *
     * @see StaticTesterServer
     */
    vector<XmlNode *>  m_testResultsToProcess;
    BepMutex m_testResultMutex;

};
//-------------------------------------------------------------------------------------------------
#endif //MesInterface_h
