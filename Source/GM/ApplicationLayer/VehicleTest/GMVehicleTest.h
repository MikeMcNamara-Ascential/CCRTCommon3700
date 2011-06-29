//*************************************************************************
// FILE:
//    $Header: /CMake Core/Source/GM/ApplicationLayer/VehicleTest/GMVehicleTest.h 2     4/01/06 5:05p Bmeinke $
//
// FILE DESCRIPTION:
// The Vehicle Test is responsible for execuiting a test of a vehilce.  It
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
//    $Log: /CMake Core/Source/GM/ApplicationLayer/VehicleTest/GMVehicleTest.h $
// 
// 2     4/01/06 5:05p Bmeinke
// Added an overloaded PreTestInit() method
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for GM
// 
// 1     6/04/04 1:08p Bmeinke
// 
// 1     4/12/04 1:20p Bmeinke
// Changing VSS repositories
// 
// 1     3/05/04 2:27p Bmeinke
// Latest release from March 03,2004 trip to Chicago
// 
// 2     8/06/03 12:54p Khykin
// Updated with latest changes from Flatrock.
// 
// 1     6/20/03 9:43a Khykin
// Initial checkin
// 
// 1     6/20/03 9:18a Khykin
// Initial checkin.
// 
//
//*************************************************************************
#ifndef GMVEHICLETEST_H
#define GMVEHICLETEST_H
//==============================================================================

#include "VehicleTest.h"
#include "PhaseCommander.h"


/**
 * Tag used for getting the test type from the drive curve
 */
#define TEST_TYPE_TAG					"TestType"


/**
 * VehicleTest class.  Responsible for managing a vehicle
 * test.
 * 
 * @author Kevin Hykin
 * @version Version 1.6
 * @since 26 July 2003
 */
class GMVehicleTest : public VehicleTest
{
public:
	/**
	 * Constructor for the Vehicle Test Class.
	 * @since Version 1.0
	 */
	GMVehicleTest();

	/**
	 * Destructor for the Vehicle Test Class.
	 * @since Version 1.0
	 */
	virtual ~GMVehicleTest();

	/**
	 * Initialize method that takes the vehicle test
	 * configuration file and initializes it for operation.
	 * 
	 * @param fileName The configuration file to use for initialization.
	 * @since Version 1.4
	 */
	virtual void Initialize(const std::string &fileName);

	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
	 *		   BEP_SUCCESS_RESPONSE == Registered
	 * @since Version 1.5
	 */
	virtual const std::string Register(void);

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

	/**
	 * Method called when an Abort command message is
	 * received.
	 * @since Version 1.0
	 */
	virtual void Abort(void);

	/**
	 * Method called when a Terminate command message is
	 * received.
	 * @since Version 1.0
	 */
	virtual void Terminate(void);

	/**
	 * Method called when a Reset command message is
	 * received.
	 * @since Version 1.0
	 */
	virtual void Reset(void);

	/**
	 * Method called when a Reset command message is
	 * received.
	 * @since Version 1.0
	 */
	virtual void ReloadConfig(void);

	/**
	 * Command a test phase.
	 * 
	 * @param phase  The phase of the test.
	 * @return The status of the operation.
	 */
	inline std::string CommandPhase(std::string phase);
	/**
	 * Get the type of the last test that was performed.
	 * 
	 * @return Type of the last performed test.
	 * @since Version 1.6
	 */
	const string &GetLastTestType(void);
    /**
     * Method used to command a vehicle test.  It will set
     * the test to be performed and then unblock the vehicle
     * test thread to perform the test.  Tests include: Start,
     * Retest, Calibrate, ...
     *
     * @param testType The test type to perform.
     * @return The status of the commanded test.
     */
    virtual inline string CommandTest(const string testType);

protected:  
	/**
	 * Get the type of drive curve that was commanded.
	 * 
	 * @param document The drive curve to look at for the type.
	 * @return The type of drive curve.
	 */
	const string GetDriveCurveType(const XmlNode *document);
	
	/**
	 * Store the current test type that is being performed.
	 * 
	 * @param testType Type of test to perform.
	 * @since Version 1.6
	 */
	void SetCurrentTestType(const string &testType);

	/**
	 * Method called just prior to starting a new test to allow sub-classes to 
	 * install "hooks" when a test is about to begin.
	 * 
	 * @param test       Type of test being performed (i.e. TEST_RETEST, etc)
	 * @param driveCurve Drive cureve to be executed
	 */
	virtual void PreTestInit(const std::string &test, const std::string &driveCurve);

	/**
	 * Phase sequencer that will allow the sequencing of individual phases in the
	 * drive curve.
	 * @since Version 1.0
	 */
	PhaseCommander m_phaseCommander;
	/**
	 * Type of the last test that was performed.
	 * @since Version 1.6
	 */
	string m_lastTestType;         

private:
    //test commanded
    string m_testPhase; 
    //config file selected drive cureve
    string m_driveCurve;
    //xml data
    XmlParser m_parser;
};

#endif //GMVEHICLETEST_H

