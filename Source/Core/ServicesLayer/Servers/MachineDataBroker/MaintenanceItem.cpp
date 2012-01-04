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
#include "IFaultServer.h"
#include "MaintenanceItem.h"

//---------------------------------------------------------------------------------------------------------------------
MaintenanceItem::MaintenanceItem(MachineDataBroker *server) : m_totalInUseTime(0), m_machineInUse(false), m_mdb(NULL)
{   // Save the pointer to the server that invoked this class
	Server(server);
}

//---------------------------------------------------------------------------------------------------------------------
MaintenanceItem::~MaintenanceItem()
{   // Nothing special to do
}

//---------------------------------------------------------------------------------------------------------------------
void MaintenanceItem::Initialize(const XmlNode *config, double &currentInUseTime)
{   // Store all the configuration data we will need
	TimeInUse(&currentInUseTime);
	string name(config->getName());
	MaintenanceTag(&name);
	string faultText(config->getAttribute("FaultText")->getValue());
	FaultString(&faultText);
	INT32 faultLevel = BposReadInt(config->getAttribute("FaultLevel")->getValue().c_str());
	FaultLevel(&faultLevel);
	double interval = atof(config->getAttribute("MaintenanceInterval")->getValue().c_str());
	MaintenanceInterval(&interval);
	// Check if the maintenance interval has expired
	if(TimeInUse() > MaintenanceInterval())
	{
		ReportIntervalExpired();
	}
}

//---------------------------------------------------------------------------------------------------------------------
bool MaintenanceItem::IsTimerRunning(void)
{
	return MachineInUse();
}

//---------------------------------------------------------------------------------------------------------------------
string MaintenanceItem::MaintenanceItemName(void)
{
	return MaintenanceTag();
}

//---------------------------------------------------------------------------------------------------------------------
void MaintenanceItem::ReportIntervalExpired(void)
{
	string response;
	Server()->Log(LOG_ERRORS, "MaintenanceItem: Maintenance interval for %s - %s has expired, setting fault",
				  MaintenanceTag().c_str(), FaultString().c_str());
	IFaultServer flt;
	flt.Initialize(FAULT_SERVER_NAME);
	flt.SetFault(MaintenanceTag(), FaultString(), Server()->GetProcessName(), response, true, FaultLevel());
}

//---------------------------------------------------------------------------------------------------------------------
void MaintenanceItem::ResetInUseTime(void)
{
	if(TimeInUse() > MaintenanceInterval())
	{
		m_totalInUseTime = 0;
		Server()->Log(LOG_DEV_DATA, "MaintenanceItem %s: Reset total time in use to 0", MaintenanceTag().c_str());
	}
}

//---------------------------------------------------------------------------------------------------------------------
void MaintenanceItem::StartInUseTimer(void)
{
	if(!MachineInUse())
	{
		time_t currentTime = time(NULL);
		InUseStartTime(&currentTime);
		bool inUse = true;
		MachineInUse(&inUse);
		char buff[128];
		strftime(buff, sizeof(buff), "%D %T", localtime(&currentTime));
		Server()->Log(LOG_DEV_DATA, "MaintenanceItem %s: Machine in use at %s", 
					  MaintenanceTag().c_str(), buff);
					  
	}
	else
	{
		Server()->Log(LOG_ERRORS, "Machine already in use, not restarting the start time");
	}
}

//---------------------------------------------------------------------------------------------------------------------
double MaintenanceItem::StopInUseTimer(void)
{   // Stop the in use timer
	if(MachineInUse())
	{   // Get the current time
		time_t currentTime = time(NULL);
		// Determine the elapsed time.  difftime returns seconds.  Divide by 3600 (number of seconds in an hour) to get hours
		double elapsedTime = difftime(currentTime, InUseStartTime()) / 3600.0;
		// Add the elapsed time to the current time
		TimeInUse(&elapsedTime);
		Server()->Log(LOG_DEV_DATA, "MaintenanceItem %s: Elapsed Time: %.4f  (total time in use: %.4f)", 
					  MaintenanceTag().c_str(), elapsedTime, TimeInUse());
		// Set machine to not in use
		bool inUse = false;
		MachineInUse(&inUse);
		// Check if the maintenance interval has expired
		if(TimeInUse() > MaintenanceInterval())
		{
			ReportIntervalExpired();
		}
	}
	return TimeInUse();
}

//---------------------------------------------------------------------------------------------------------------------
time_t& MaintenanceItem::InUseStartTime(const time_t *startTime /*= NULL*/)
{
	if(startTime != NULL)  m_inUseStartTime = *startTime;
	return m_inUseStartTime;
}

//---------------------------------------------------------------------------------------------------------------------
INT32& MaintenanceItem::FaultLevel(const INT32 *level /*= NULL*/)
{
	if(level != NULL)  m_faultLevel = *level;
	return m_faultLevel;
}

//---------------------------------------------------------------------------------------------------------------------
string& MaintenanceItem::FaultString(const string *faultDesc /*= NULL*/)
{
	if(faultDesc != NULL)   m_faultString = *faultDesc;
	return m_faultString;
}

//---------------------------------------------------------------------------------------------------------------------
bool& MaintenanceItem::MachineInUse(const bool *inUse /*= NULL*/)
{
	if(inUse != NULL)  m_machineInUse = *inUse;
	return m_machineInUse;
}

//---------------------------------------------------------------------------------------------------------------------
double& MaintenanceItem::MaintenanceInterval(const double *interval /*= NULL*/)
{
	if(interval != NULL)  m_maintenanceInterval = *interval;
	return m_maintenanceInterval;
}

//---------------------------------------------------------------------------------------------------------------------
string& MaintenanceItem::MaintenanceTag(const string *tag /*= NULL*/)
{
	if(tag != NULL)  m_maintenanceTag = *tag;
	return m_maintenanceTag;
}

//---------------------------------------------------------------------------------------------------------------------
MachineDataBroker* MaintenanceItem::Server(MachineDataBroker *mdb /*= NULL*/)
{
	if(mdb != NULL)  m_mdb = mdb;
	return m_mdb;
}

//---------------------------------------------------------------------------------------------------------------------
double& MaintenanceItem::TimeInUse(const double *timeInUse /*= NULL*/)
{
	if(timeInUse != NULL)  m_totalInUseTime += *timeInUse;
	return m_totalInUseTime;
}
