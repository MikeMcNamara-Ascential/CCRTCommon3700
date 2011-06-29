//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
// The ICM data files will be opened to allow instantaneous data to be accessed.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//*************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Bepos.h"
#include "IcmDataSupplier.h"

//-----------------------------------------------------------------------------
IcmDataSupplier::IcmDataSupplier(const XmlNode *fileNames, const INT32 &rollerCount)
{
    try
    {
        m_forceFileName = fileNames->getChild("CurrentForces")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_forceFileName = "/dev/Icm/Block/Force";
    }
    try
    {
        m_speedFileName = fileNames->getChild("CurrentSpeeds")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_speedFileName = "/dev/Icm/Block/Speed";
    }
    try
    {
        m_distanceFileName = fileNames->getChild("CurrentDistances")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_distanceFileName = "/dev/Icm/Block/PulseCount";
    }
    try
    {
        m_forceArrayFileName = fileNames->getChild("ForceArray")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_forceArrayFileName = "/dev/ArrayForce";
    }
    try
    {
        m_speedArrayFileName = fileNames->getChild("SpeedArray")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_speedArrayFileName = "/dev/ArraySpeed";
    }
    try
    {
        m_distanceArrayFileName = fileNames->getChild("DistanceArray")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_distanceArrayFileName = "/dev/ArrayDistance";
    }
    RollerCount(&rollerCount);
}

//-----------------------------------------------------------------------------
IcmDataSupplier::~IcmDataSupplier()
{   // Ensure all data files are closed
    CloseForceFile();
    CloseSpeedFile();
    CloseWheelDistanceFile();
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::OpenIcmDataFiles()
{
    // Initialize the comm object for the ICM
    m_icmComm.Initialize("mgr/ICM", "Client");
    // Open the files
    BEP_STATUS_TYPE forceStatus = OpenForceFile();
    BEP_STATUS_TYPE speedStatus = OpenSpeedFile();
    BEP_STATUS_TYPE distStatus  = OpenWheelDistanceFile();
    BEP_STATUS_TYPE forceArrayStatus = OpenForceArrayFile();
    BEP_STATUS_TYPE speedArrayStatus = OpenSpeedArrayFile();
    return ((BEP_STATUS_SUCCESS == forceStatus) && 
            (BEP_STATUS_SUCCESS == speedStatus) && 
            (BEP_STATUS_SUCCESS == distStatus)  &&
            (BEP_STATUS_SUCCESS == forceArrayStatus) &&
            (BEP_STATUS_SUCCESS == speedArrayStatus)) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::GetWheelSpeeds(float wheelSpeed[])
{
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;  // overall status
    int retry = 3;  // the number of retrys to attempt
    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_speedFileMutex.Acquire())
    {
        try
        {   // clear out the wheel speeds
            memset(wheelSpeed, 0, (sizeof(float) * RollerCount()));
            if(!IsSpeedFileOpen())
            {   // Open the speed file
                status = OpenSpeedFile();
            }
            // Ensure the file is open before we attempt to read
            if(IsSpeedFileOpen())
            {
                do
                {   // Read the speed data
                    lseek(m_speedFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_speedFile, (void *) wheelSpeed, (sizeof(float) * RollerCount()));
                    status = (bytes == (sizeof(float) * RollerCount())) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
                    // if the file could not be read, give up the time slice
                    if(status != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while((status != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_speedFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            status = BEP_STATUS_ERROR;
            // release the file mutex
            m_speedFileMutex.Release();
        }
    }
    else
    {
        status = BEP_STATUS_ERROR;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::GetWheelDistances(float wheelDistance[])
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt
    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_wheelDistanceFileMutex.Acquire())
    {
        try
        {   // clear out the wheel distances
            memset(wheelDistance, 0, (sizeof(float) * RollerCount()));
            if(!IsWheelDistanceFileOpen())
            {   // Open the WheelDistance file
                status = OpenWheelDistanceFile();
            }
            // Ensure the file is open before we attempt to read
            if(IsWheelDistanceFileOpen())
            {
                do
                {
                    // Read the WheelDistance data
                    lseek(m_wheelDistanceFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_wheelDistanceFile, (void *) wheelDistance, (sizeof(float) * RollerCount()));
                    status = (bytes == (sizeof(float) * RollerCount())) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
                    // if the file could not be open, give up the time slice
                    if(status != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while( (status != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_wheelDistanceFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            status = BEP_STATUS_ERROR;
            // release the file mutex
            m_wheelDistanceFileMutex.Release();
        }
    }
    else
    {
        status = BEP_STATUS_ERROR;
    }

    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::GetForces(float force[])
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    int retry = 3;  // the number of retrys to attempt
    // Allow only one thread to access SpeedFile at a time
    if(EOK == m_forceFileMutex.Acquire())
    {
        try
        {   // clear out the forces
            memset(force, 0, (sizeof(float) * RollerCount()));
            if(!IsForceFileOpen())
            {   // Open the force file
                status = OpenForceFile();
            }
            // Ensure the file is open before we attempt to read
            if(IsForceFileOpen())
            {
                do
                {   // Read the force data
                    lseek(m_forceFile, 0, SEEK_SET);
                    UINT32 bytes = read(m_forceFile, (void *) force, (sizeof(float) * RollerCount()));
                    status = (bytes == (sizeof(float) * RollerCount())) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
                    // if the file could not be open, give up the time slice
                    if(status != BEP_STATUS_SUCCESS)    BposSleep(1);
                }while( (status != BEP_STATUS_SUCCESS) && (retry--));
                // release the file mutex
                m_forceFileMutex.Release();
            }
        }
        catch(BepException &e)
        {
            status = BEP_STATUS_ERROR;
            // release the file mutex
            m_forceFileMutex.Release();
        }
    }
    else
    {
        status = BEP_STATUS_ERROR;
    }

    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::ReadSpeedArray(const INT32 &startIndex, const INT32 &endIndex, IcmDataArray &dataArray)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // If the file is not open, try to open it
    if(!IsSpeedArrayFileOpen())
    {
        status = OpenSpeedArrayFile();
    }
    // Make sure the file is open
    if(IsSpeedArrayFileOpen())
    {   // File has been opened, lock it
        if(EOK == m_speedArrayFileMutex.Acquire())
        {   // Read the speed data from the array file
            status = ReadIcmArray(m_speedArrayFile, startIndex, endIndex, dataArray);
            // release the mutex
            m_speedArrayFileMutex.Release();
        }
        else
        {   // Could not acquire the mutex
            status = BEP_STATUS_ERROR;
        }
    }
    else
    {   // File is not open
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::ReadForceArray(const INT32 &startIndex, const INT32 &endIndex, IcmDataArray &dataArray)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // If the file is not open, try to open it
    if(!IsForceArrayFileOpen())
    {
        status = OpenForceArrayFile();
    }
    // Make sure the file is open
    if(IsForceArrayFileOpen())
    {   // File has been opened, lock it
        if(EOK == m_forceArrayFileMutex.Acquire())
        {   // Read the force data from the array file
            status = ReadIcmArray(m_forceArrayFile, startIndex, endIndex, dataArray);
            // release the mutex
            m_forceArrayFileMutex.Release();
        }
        else
        {   // Could not acquire the mutex
            status = BEP_STATUS_ERROR;
        }
    }
    else
    {   // File is not open
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
INT32 IcmDataSupplier::TagArray(const string &arrayTag)
{
    INT32 sampleNumber = 0;
    string response;
    if(m_icmComm.Command("TagIndex", arrayTag, response, true) == BEP_STATUS_SUCCESS)
    {   // Extract the data from the response
        string tag, value;
        m_icmComm.GetNext(tag, value, response);
        sampleNumber = BposReadInt(value.c_str());
    }
    return sampleNumber;
}

//-----------------------------------------------------------------------------
INT32 IcmDataSupplier::GetCurrentArrayIndex(void)
{
    INT32 arrayIndex = -1;
    string response;
    if(m_icmComm.Read("TagIndex", response, true) == BEP_STATUS_SUCCESS)
    {   // Extract the data from the response
        string tag, value;
        m_icmComm.GetNext(tag, value, response);
        arrayIndex = BposReadInt(value.c_str());
    }
    return arrayIndex;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::ReadIcmArray(const INT32 &arrayFile, const INT32 &startIndex, 
                                              const INT32 &endIndex, IcmDataArray &dataArray)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Clear the data array so we start fresh
    dataArray.clear();
    // Create a data vector for each roller
    IcmDataVector roller[RollerCount()];
    // Set the seekpoint to the first requested sample
    UINT32 seekPoint = (RollerCount() * startIndex) * sizeof(float);
    ssize_t bytes;          // The number of bytes read from the array file
    struct stat arrayStat;  // Status info of the array file
    memset(&arrayStat, 0, sizeof(arrayStat));
    // Get the status of the array file
    fstat(arrayFile, &arrayStat);
    // Verify we can successfully seek to the desired sample
    if((seekPoint != 0) && (seekPoint < (UINT32)arrayStat.st_size))
    {   // Attempt to set the seek point
        if(lseek(arrayFile, seekPoint, SEEK_SET) != -1)
        {   // Determine the number of samples to read
            INT32 samples = endIndex - startIndex;
            IcmSample dataSample[samples+1];
            bytes = read(arrayFile, (void *)dataSample, (sizeof(IcmSample) * samples));
            // Verify ew read the correct amount of data
            if(bytes == (ssize_t)(sizeof(IcmSample) * samples))
            {   // Store the data
                for(INT32 index = 0; index < samples; index++)
                {
                    roller[LFWHEEL].push_back(dataSample[index].lf);
                    roller[RFWHEEL].push_back(dataSample[index].rf);
                    roller[LRWHEEL].push_back(dataSample[index].lr);
                    roller[RRWHEEL].push_back(dataSample[index].rr);
                }
                // Set the data to be returned
                for(UINT8 index = 0; index < RollerCount(); index++)
                {
                    dataArray.push_back(roller[index]);
                }
                status = BEP_STATUS_SUCCESS;
            }
            else
            {
                status = BEP_STATUS_FAILURE;
            }
        }
        else
        {   // Could not seek to the desired point
            status = BEP_STATUS_FAILURE;
        }
    }
    else
    {   // Invalid seek point
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::OpenForceFile(void)
{
    INT32 attempts = 3;
    do
    {
        m_forceFile = open(m_forceFileName.c_str(), O_RDONLY);
        SetForceFileOpen(m_forceFile != -1);
    } while(!IsForceFileOpen() && attempts--);

    return IsForceFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::OpenForceArrayFile(void)
{
    INT32 attempts = 3;
    do
    {
        m_forceArrayFile = open(m_forceArrayFileName.c_str(), O_RDONLY);
        SetForceArrayFileOpen(m_forceArrayFile != -1);
    } while(!IsForceArrayFileOpen() && attempts--);

    return IsForceArrayFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::CloseForceFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsForceFileOpen())
    {   // Force file is open, close it
        do
        {
            closeStatus = close(m_forceFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                SetForceFileOpen(false);
                m_forceFile = -1;
            }
        } while(IsForceFileOpen() && closeAttempts--);
    }
    else
    {
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return closeStatus;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::CloseForceArrayFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT8 closeAttempts = 3;
    if(IsForceArrayFileOpen())
    {   // Close the force array file
        do
        {
            closeStatus = (close(m_forceArrayFile) == 0) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(BEP_STATUS_SUCCESS == closeStatus)
            {
                SetForceArrayFileOpen(false);
                m_forceArrayFile = -1;
            }
        } while(IsForceArrayFileOpen() && closeAttempts--);
    }
    else
    {
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return closeStatus;
}

//-----------------------------------------------------------------------------
inline bool& IcmDataSupplier::IsForceFileOpen(void)
{
    return m_forceFileOpen;
}

//-----------------------------------------------------------------------------
inline bool& IcmDataSupplier::IsForceArrayFileOpen(void)
{
    return m_forceArrayFileOpen;
}

//-----------------------------------------------------------------------------
inline void IcmDataSupplier::SetForceFileOpen(bool fileOpen)
{
    m_forceFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
inline void IcmDataSupplier::SetForceArrayFileOpen(bool fileOpen)
{
    m_forceArrayFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::OpenSpeedFile(void)
{
    INT32 attempts = 3;
    do
    {
        m_speedFile = open(m_speedFileName.c_str(), O_RDONLY);
        SetSpeedFileOpen(m_speedFile != -1);
    } while(!IsSpeedFileOpen() && attempts--);

    return IsSpeedFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::OpenSpeedArrayFile(void)
{
    INT32 attempts = 3;
    do
    {
        m_speedArrayFile = open(m_speedArrayFileName.c_str(), O_RDONLY);
        SetSpeedArrayFileOpen(m_speedArrayFile != -1);
    } while(!IsSpeedArrayFileOpen() && attempts--);

    return IsSpeedArrayFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::CloseSpeedFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsSpeedFileOpen())
    {   // Speed file is open, close it
        do
        {
            closeStatus = close(m_speedFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                SetSpeedFileOpen(false);
                m_speedFile = -1;
            }
        } while(IsSpeedFileOpen() && closeAttempts--);
    }
    else
    {
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return closeStatus;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::CloseSpeedArrayFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT8 closeAttempts = 3;
    if(IsSpeedArrayFileOpen())
    {
        do
        {
            closeStatus = (close(m_speedArrayFile) == 0) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(BEP_STATUS_SUCCESS == closeStatus)
            {
                SetSpeedArrayFileOpen(false);
                m_speedArrayFile = -1;
            }
        } while(IsSpeedArrayFileOpen() && closeAttempts--);
    }
    else
    {
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return closeStatus;
}

//-----------------------------------------------------------------------------
inline bool& IcmDataSupplier::IsSpeedFileOpen(void)
{
    return m_speedFileOpen;
}

//-----------------------------------------------------------------------------
inline bool& IcmDataSupplier::IsSpeedArrayFileOpen(void)
{
    return m_speedArrayFileOpen;
}

//-----------------------------------------------------------------------------
inline void IcmDataSupplier::SetSpeedFileOpen(bool fileOpen)
{
    m_speedFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
inline void IcmDataSupplier::SetSpeedArrayFileOpen(bool fileOpen)
{
    m_speedArrayFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::OpenWheelDistanceFile(void)
{
    INT32 attempts = 3;
    do
    {
        m_wheelDistanceFile = open(m_distanceFileName.c_str(), O_RDONLY);
        SetWheelDistanceFileOpen(m_wheelDistanceFile != -1);
    } while(!IsWheelDistanceFileOpen() && attempts--);

    return IsWheelDistanceFileOpen() ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE IcmDataSupplier::CloseWheelDistanceFile(void)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    INT32 closeAttempts = 3;
    if(IsWheelDistanceFileOpen())
    {   // WheelDistance file is open, close it
        do
        {
            closeStatus = close(m_wheelDistanceFile) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
            if(closeStatus == BEP_STATUS_SUCCESS)
            {
                SetWheelDistanceFileOpen(false);
                m_wheelDistanceFile = -1;
            }
        } while(IsWheelDistanceFileOpen() && closeAttempts--);
    }
    else
    {
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return closeStatus;
}

//-----------------------------------------------------------------------------
inline bool& IcmDataSupplier::IsWheelDistanceFileOpen(void)
{
    return m_wheelDistanceFileOpen;
}

//-----------------------------------------------------------------------------
inline void IcmDataSupplier::SetWheelDistanceFileOpen(bool fileOpen)
{
    m_wheelDistanceFileOpen = fileOpen;
}

//-----------------------------------------------------------------------------
inline const INT32& IcmDataSupplier::RollerCount(const INT32 *rollerCount /*= NULL*/)
{
    if(rollerCount != NULL)  m_rollerCount = *rollerCount;
    return m_rollerCount;
}
