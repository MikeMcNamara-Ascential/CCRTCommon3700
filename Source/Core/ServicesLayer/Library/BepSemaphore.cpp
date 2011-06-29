//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/BepSemaphore.cpp $
// $Author: Cward $
// $Date: 1/23/04 9:25a $
//
// Description:
//	Semaphore class that provides the methods necessary to ensure critical resource
//  protection in a multi-threaded system.
//
//===========================================================================
// Copyright (c) 2000 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//     $Log: /Core/ServicesLayer/Library/BepSemaphore.cpp $
// 
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "BepSemaphore.h"

#include "BepException.h"
#include "GeneralUtilities.h"

#include <unistd.h>
#include <sys/select.h>
#include <string>
#include <errno.h>
#include <time.h>



BepSemaphore::BepSemaphore(int count /*= 1*/,
                           bool debug /*= false*/,
                           std::string name /*= "Blank"*/)
    : Debuggable(debug), m_semaphore(NULL), m_name(name)
{
    Create(count);
}

BepSemaphore::~BepSemaphore()
{
    Destroy();
}



void BepSemaphore::Create(int initialVal)
{	// if the semaphore already exists, destroy it
	if(m_semaphore != NULL)		Destroy();
	// create and initialize the new semaphore
	m_semaphore = new sem_t;	// create a new semaphore
	if(sem_init(m_semaphore, 0, initialVal) == -1)
	{
		perror("Could Not Initialize m_semCommand");
		throw BepException("VehicleTest: Could Not Initialize m_semCommand %d", errno);
	}
	else if(IsDebugOn())	printf("Created BepSemaphore\n");
}

void BepSemaphore::Destroy(void)
{	// if the semaphore exists, destroy it
	if(m_semaphore != NULL)
	{
		if(IsDebugOn())		printf("Deleting BepSemaphore %s\n", m_name.c_str());
		sem_destroy(m_semaphore);	// destroy it
		delete m_semaphore;		// delete it
		m_semaphore = NULL;		// set to NULL	
	}
	else if(IsDebugOn())	printf("Did Not Delete BepSemaphore %s\n", m_name.c_str());
}

void BepSemaphore::CriticalSectionExit(void)
{
	INT32 status, counter=0;
	// wait here until the semaphore is avaliable
	if(IsDebugOn())	printf("waiting to post semaphore %s\n", m_name.c_str());
	int semValue;
	do
	{	// get the value of the count
		status = sem_getvalue(m_semaphore, &semValue);
		if(status == 0)
		{	// if value successfully read and it is greater than the count
			if(semValue > (m_count -1))
				status = sem_trywait(m_semaphore);	// decrement the count
		}
	}while(((semValue > (m_count -1)) || (status == -1)) && (counter++ < 10));
	// post the semaphore
	counter = 10;
	do
	{
		status = sem_post(m_semaphore);
		// if an error occurrs and it is not a signal interrupt
		if((status == -1) && (errno != EINTR))
			throw BepException("Could Not Post A BepSemaphore: %s, %s\n", m_name.c_str(), strerror( errno ));
		else if((IsDebugOn()) && (status == -1))
			printf("BepSemaphore Post Interruption %s\n", m_name.c_str());
	}
	while(SignalCheck(status, EINTR, counter, IsDebugOn()) == false);	// Do over if interrupted
	if(IsDebugOn())	printf("posting semaphore %s\n", m_name.c_str());
}

BEP_STATUS_TYPE BepSemaphore::CriticalSectionEnter(INT32 timeout /* = -1*/)
{
	INT32 status;			// status flag
	INT32 counter=0;		// counter for retries
	struct timespec tm;		// timeout structure
	
	if(IsDebugOn())	printf("%s waiting for a Bepsemaphore\n", m_name.c_str());
	if(timeout > 0)	// if a timeout is specified, set up the structure
	{
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += timeout;
	}
	do
	{
		if(timeout > 0) status = sem_timedwait( m_semaphore, &tm );
		else			status = sem_wait(m_semaphore);
		// if an error occurrs and it is not a signal interrupt
		if((status == -1) && (errno != EINTR) && (errno != ETIMEDOUT))
			throw BepException("%s Could Not Wait On BepSemaphore: %s", m_name.c_str(), strerror( errno ));
		else if((IsDebugOn()) && (status == -1) && (errno == EINTR))
			printf("%s Semaphore Wait Exception\n", m_name.c_str());
	}
	while((counter++ < 10) && (status != 0) && (status != ETIMEDOUT));
	
	BEP_STATUS_TYPE result;
	if(status == EOK)   			result = BEP_STATUS_SUCCESS;
	else if(status == ETIMEDOUT)    result = BEP_STATUS_TIMEOUT;
	else                            result = BEP_STATUS_FAILURE;
		
	if(IsDebugOn()) 	printf("%s CriticalSectionEnter Status: %d\n", m_name.c_str(), status);
		
	return(result);
}

BEP_STATUS_TYPE BepSemaphore::CriticalSectionTryEnter(void)
{
	BEP_STATUS_TYPE status;
	if(IsDebugOn())	printf("Attempting to get Bepsemaphore %s\n", m_name.c_str());
	// ensure that the semaphore is not already avaliable
	int result;
	do
	{
		result = sem_trywait(m_semaphore);
	}while((result == -1) && (errno == EINTR));
	
	if(result == -1)
		status = BEP_STATUS_FAILURE;
	else
		status = BEP_STATUS_SUCCESS;
	return(status);
}

