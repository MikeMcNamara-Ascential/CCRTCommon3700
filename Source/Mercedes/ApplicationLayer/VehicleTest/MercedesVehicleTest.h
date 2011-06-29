//*************************************************************************
// FILE:
//    $Header: /DCX/CCRT/Common/Core/ApplicationLayer/VehicleTest/MercedesVehicleTest.h 1     11/01/04 3:11p Derickso $
//
// FILE DESCRIPTION:
// The Mercedes Vehicle Test is responsible for execuiting a test of a vehilce.  It
// creates a Selector to choose the appropriate DriveCurve to sequence based
// on the vehicle build and then creates a Sequencer to run the test.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /DCX/CCRT/Common/Core/ApplicationLayer/VehicleTest/MercedesVehicleTest.h $
// 
// 1     11/01/04 3:11p Derickso
// Created.
// 
//*************************************************************************
#ifndef DAIMLER_CHRYSLER_VEHICLETEST_H
#define DAIMLER_CHRYSLER_VEHICLETEST_H
//==============================================================================

#include <queue>
#include "VehicleTest.h"
#include "ITestResultServer.h"
#include "INamedDataBroker.h"

/**
 * Constant used to command test component
 * data to be removed
 * @since 16 August 2004
 */
#define REMOVE_COMPONENTS 	"RemoveComponents"
/**
 * Constant used to command test component
 * data to load
 * @since 16 August 2004
 */
#define LOAD_COMPONENTS 	"LoadComponents"

/**
 * MercedesVehicleTest class.  Responsible for managing a vehicle
 * test.
 * 
 * @author Greg Pattison
 * @version 16 August 2004
 */
class MercedesVehicleTest : public VehicleTest
{
public:
	/**
	 * Constructor for the Vehicle Test Class.
	 * @since Version 1.0
	 */
	MercedesVehicleTest();

	/**
	 * Destructor for the Vehicle Test Class.
	 * @since Version 1.0
	 */
	virtual ~MercedesVehicleTest();

	/**
	 * Initialize method that takes the vehicle test
	 * configuration file and initializes it for operation.
	 *
	 * @param document Root node of the configuration file
	 *                 to use for initialization.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);
	
	/**
	 * Method that runs the vehicle test task until terminated.
	 * It will block until started by a command to perform a
	 * vehicle or machine test. If the test status is set to
	 * Termiante, it will return and the test thread will
	 * terminate and the process will shut down.
	 * @since Version 1.0
	 */
	virtual void Run(volatile bool *terminateFlag=NULL);

	/**
	 * Method to process the BepCommuncation Command message.
	 *
	 * @param node    Action to be performed
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 1.5
	 */
	virtual const std::string Command(const XmlNode *node);

	/**
	 * Method that will handle all published data.  This adds the
	 * functionality of launching the LossCompensation test if
	 * the start event is received.
	 *
	 * @param node    Tag and data that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 * @since Version 1.5
	 */
	const std::string Publish(const XmlNode *node);
	virtual string LoadDriveCurve(void);
	/**
	 * Method to load the drive curve components before
	 * the test is actually started.
	 */
	virtual string LoadComponents(string testType);
	/**
	 * Get the commanded test to perform.  This method will
	 * block until there is a new command to perform.  The
	 * commands will be Start, Rerun, Calibrate, ...
	 *
	 * @return The command to perform.
	 * @since Version 1.0
	 */
	virtual inline std::string GetCommandedTest(void);

	/**
	 * Method used to command a vehicle test.  It will set
	 * the test to be performed and then unblock the vehicle
	 * test thread to perform the test.  Tests include: Start,
	 * Retest, Calibrate, ...
	 *
	 * @param testType The test type to perform.
	 * @return The status of the commanded test.
	 * @since Version 1.0
	 */
	virtual inline std::string CommandTest(const std::string testType);
    /**
     * Debug function to return the size of the mem used by the process 
     * 
     * 
     */
    int ReturnMemArena();

protected:

	private:
	/**
	 * Queue used to keep a list of commands
	 * for controlling and sequencing the loading,
	 * removing and running of test components.
	 */
	std::queue< string > m_commandQueue;
	/**
	 * System data tags to be reset at the beginning of a new test sequence.
	 * @since July 14, 2004
	 */
	XmlNodeMap m_resetSystemTags;
	/**
	 * Overall test result boxes to clear.
	 * @since July 15, 2004
	 */
	XmlNodeMap m_resetOverallResults;
	/**
	 * Object to use to clear the test result boxes on the GUI.
	 * @since July 15, 2004
	 */
	ITestResultServer *m_testResult;

};
#endif // DAIMLER_CHRYSLER_VEHICLETEST_H

