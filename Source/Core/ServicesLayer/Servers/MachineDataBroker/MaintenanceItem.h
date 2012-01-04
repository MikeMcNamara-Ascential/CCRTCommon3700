//*************************************************************************
// FILE DESCRIPTION:
//   Machine maintenance class. 
//	 The purpose of this class is to manage a specific maintenance item.
//   the machine time in use will be tracked and a fault will be issued for
//   items that excede the maintenance interval.
//
//===========================================================================
// Copyright (c) 2011- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef MaintenanceItem_h
#define MaintenanceItem_h
//---------------------------------------------------------------------------------------------------------------------
#include "MachineDataBroker.h"
#include <time.h>

//---------------------------------------------------------------------------------------------------------------------
class MachineDataBroker;

//---------------------------------------------------------------------------------------------------------------------
class MaintenanceItem
{
public:

	MaintenanceItem(MachineDataBroker *server);
	virtual ~MaintenanceItem();

	/**
	 * Initialize the maintenance data item.
	 * 
	 * @param config Configuration data for the maintenance item.
	 * @param currentInUseTime
	 *               amount of time already tracked for the maintenance item.
	 */
	void Initialize(const XmlNode *config, double &currentInUseTime);

	/**
	 * Return a flag indicating if the timer is running.
	 * 
	 * @return True if the timer is running.
	 */
	bool IsTimerRunning(void);

	/**
	 * Get the name of the maintenance data item.
	 * 
	 * @return Name of the maintenance data item.
	 */
	string MaintenanceItemName(void);

	/**
	 * Report a fault stating that the interval for this maintenance item has expired.
	 */
	void ReportIntervalExpired(void);

	/**
	 * Reset the in use time.
	 * The total time in use will be checked to make sure it is valid to reset the in use time.
	 * If the maintenance interval has not beed exceeded, the in use time will not be reset.
	 */
	void ResetInUseTime(void);

	/**
	 * Set the start time for machine in use.
	 */
	void StartInUseTimer(void);

	/**
	 * Stop the machine in use time.
	 * The time in use will be calculated and updated internally.
	 * If the maintenance interval has been exceeded, a fault will be recorded.
	 */
	double StopInUseTimer(void);



private:

	/**
	 * Get/Set the machine in use start time.
	 * 
	 * @param startTime Machine in use start time
	 * 
	 * @return Machine in use start time
	 */
	time_t& InUseStartTime(const time_t *startTime = NULL);

	/**
	 * Get/Set the fault level to use when reporting the maintenance interval has expired.
	 * 
	 * @param level  Fault level to use when reporting the maintenance interval has expired.
	 * 
	 * @return Fault level to use when reporting the maintenance interval has expired.
	 */
	INT32& FaultLevel(const INT32 *level = NULL);

	/**
	 * Get/Set the fault text to display when the maintenance interval expires.
	 * 
	 * @param faultDesc Fault text to display when the maintenance interval expires.
	 * 
	 * @return Fault text to display when the maintenance interval expires.
	 */
	string& FaultString(const string *faultDesc = NULL);

	/**
	 * Get/Set the flag indicating if the machine is currently in use.
	 * 
	 * @param inUse  Flag indicating if the machine is currently in use.
	 * 
	 * @return Flag indicating if the machine is currently in use.
	 */
	bool& MachineInUse(const bool *inUse = NULL);

	/**
	 * Get/Set the Time interval for this maintenance item.
	 * 
	 * @param interval Time interval for this maintenance item.
	 * 
	 * @return Time interval for this maintenance item.
	 */
	double& MaintenanceInterval(const double *interval = NULL);

	/**
	 * Get/Set the name of the maintenance item.
	 * 
	 * @param tag    Name of the maintenance item.
	 * 
	 * @return Name of the maintenance item.
	 */
	string& MaintenanceTag(const string *tag = NULL);

	/**
	 * Get/Set the pointer to the MachineDataBRoker so convenience functions can be used.
	 * 
	 * @param mdb    Pointer to the MachineDataBRoker so convenience functions can be used.
	 * 
	 * @return Pointer to the MachineDataBRoker so convenience functions can be used.
	 */
	MachineDataBroker* Server(MachineDataBroker *mdb = NULL);

	/**
	 * Get/Set the total time in use for this maintenance item.
	 * If timeInUse is not NULL, the value will be added to the current in use time.
	 * 
	 * @param timeInUse Total time in use for this maintenance item.
	 * 
	 * @return Total time in use for this maintenance item.
	 */
	double& TimeInUse(const double *timeInUse = NULL);

	// Start time of the current in use period.
	time_t m_inUseStartTime;

	// Severuty level to use in reporting the maintenance interval has expired.
	INT32 m_faultLevel;

	// Time interval for this maintenance item.
	double m_maintenanceInterval;

	// Number of hours the machine has been in use for the current maintenance period.
	double m_totalInUseTime;

	// Fault string to display when the interval period has expired.
	string m_faultString;

	// Friendly name of the maintenance item.
	string m_maintenanceTag;

	// Flag to track machine in use status to ensure we track macine in use time properly
	bool m_machineInUse;

	// Pointer to the machine data broker so convenience functions can be used
	MachineDataBroker *m_mdb;
};
//---------------------------------------------------------------------------------------------------------------------
#endif //MaintenanceItem_h
