//*************************************************************************
// Description:
//   Tata Test Result Server to handle all test results.
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
//
// $Log$
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef TeslaTestResultServer_h
#define TeslaTestResultServer_h
//-------------------------------------------------------------------------------------------------
#include "INamedDataBroker.h"
#include "TestResultServer.h"

//-------------------------------------------------------------------------------------------------
class TeslaTestResultServer : public TestResultServer
{
public:
	TeslaTestResultServer();
	virtual ~TeslaTestResultServer();

	/**
	 * Set up the final test result server.
	 *
	 * @param document Configuration file for the test result server.
	 */
	virtual void Initialize(const XmlNode *document);

	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag and data that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 */
	virtual const string Publish(const XmlNode *node);


protected:

    /**
     * <b>Description:</b>
     * Read data from NDB.
     *
     * @param tag The data tag to read.
     * 
     * @return The value of the data.  If an error occurs, the value
     *         ERROR will be returned.
     */
    const string NdbRead(const string &tag);



private:
    // interface to NDB
    INamedDataBroker *m_ndb;
};

//-------------------------------------------------------------------------------------------------
#endif //TeslaTestResultServer_h
