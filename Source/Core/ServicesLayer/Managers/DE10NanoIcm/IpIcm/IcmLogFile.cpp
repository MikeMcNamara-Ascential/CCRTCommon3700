//******************************************************************************
// Description:
//  Object to manage data log files for ICM data
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IcmLogFile.cpp $
//
// 2     5/01/07 12:54p Rwiersem
// Changes for the 07050201 core release:
//
// Removed the leading $'s when reading the FTP_ROOT environment variable.
//
// 1     4/13/06 3:11a Cward
// Moved implementation from headers to cpp
//
// 1     2/15/06 12:05p Bmeinke
// Moved code from header to cpp file
//
// 2     2/03/06 1:58p Bmeinke
// Moved implementation to cpp file
//
// 1     12/16/05 1:50p Bmeinke
// Added new IcmLogSample_t typedef to hold a vector of floats to be added
// to a log file
// Moved all source to seperate cpp file.
// Added overloaded LogSample() methods that take IcmLogSample_t objects.
// Added overloaded LogSample() methods that do not take a dataIdx
// parameter (dataIdx is used to add a tag to a sample in the log file)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

/*******************
 include files
 ******************/
#include "IcmLogFile.h"
#include <unistd.h>     // symlink(), unlink()

IcmLogFile::IcmLogFile() : m_tagList(NULL), m_logFileName(""), m_logFileLink(""),
m_file(NULL), m_enabled(false)
{
}

IcmLogFile::~IcmLogFile()
{
}

/**
 * Object initializer from XML node
 *
 * @param tagList    Pointer to the object holding the list of sample indices and
 *                   associated tags to be used when logging tags to the data log
 * @param configNode Node to read log file configuration from
 */
void IcmLogFile::Initialize(IcmTagList *tagList, const XmlNode *configNode)
{
    int err;
    const char *ftpRoot = getenv("FTP_ROOT");

    // Lock list for exclusive access
    if ((err = Lock()) == EOK)
    {
        m_tagList = tagList;

        // Read status of log file objects
        try
        {
            const XmlNode *blah = configNode->getAttributes().getNode(XML_T("status"));
            const XmlString &status = blah->getValue();
            if (status == XmlString(XML_T("Disabled")))
                SetEnabled(false);
            else
                SetEnabled(true);
        }
        catch (XmlException &err)
        {
            // Default to enabled
            SetEnabled(false);
        }

        // Read file name to use for storing ICM data to in ASCII format
        try
        {
            const XmlNode *name = configNode->getAttributes().getNode(CFG_LOG_NAME_TAG);
            const XmlString &fileName = name->getValue();

            // If a file name is specified in the config file
            if (fileName.empty() == false)
            {
                if (ftpRoot != NULL)
                    m_logFileName = ftpRoot + XmlToAscii(fileName);
                else
                    m_logFileName = XmlToAscii(fileName);
            }
            else
            {
                // No name in config file...so disable
                SetEnabled(false);
            }
        }
        catch (XmlException &err)
        {
            SetEnabled(false);
            printf("ICM Error Could Not Set Up Log File\n");
        }

        // Read name of the link to use for the gui to FTP the log file from
        try
        {
            const XmlNode *link = configNode->getAttributes().getNode(CFG_LOG_LINK_TAG);
            if (ftpRoot != NULL)
                m_logFileLink = ftpRoot + XmlToAscii(link->getValue());
            else
                m_logFileLink = XmlToAscii(link->getValue());
        }
        catch (XmlException &err)
        {
            printf("ICM Error Could Not Set Up Link File\n");
        }

        // Unlock the list
        Unlock();
    }
    else
    {
        printf("Error, Could Not Lock To Initialize a Log File: %s\n", strerror(err));
    }
}

/**
 * Object initializer from XML node
 *
 * @param tagList  Pointer to the object holding the list of sample indices and
 *                 associated tags to be used when logging tags to the data log
 * @param fileName Name of the file to log data to
 * @param linkName Path to the file link we need to create at the end of
 *                 the test to notify the GUI that the data file is valid
 */
void IcmLogFile::Initialize(IcmTagList *tagList, const string &fileName, const string &linkName)
{
    int err;
    // Lock list for exclusive access
    if ((err = Lock()) == EOK)
    {
        const char *ftpRoot = getenv("FTP_ROOT");
        m_tagList = tagList;
        if (ftpRoot != NULL)
        {
            m_logFileName = getenv("FTP_ROOT") + fileName;
            m_logFileLink = getenv("FTP_ROOT") + linkName;
        }
        else
        {
            m_logFileName = fileName;
            m_logFileLink = linkName;
        }
        m_enabled = true;

        // Unlock the list
        Unlock();
    }
    else
    {
        printf("Error, Could Not Lock To Initialize a Log File: %s\n", strerror(err));
    }
}

/**
 * Prepares the log file object to start logging new data.
 * Resets the tag list and removes the associated file link.
 */
void IcmLogFile::StartLog()
{
    int err;
    // Lock list for exclusive access
    if ((err = Lock()) == EOK)
    {
        // If logging is enabled
        if (IsEnabled())
        {
            // Clear any buffered tags
            if (m_tagList != NULL)
                m_tagList->Clear();
            // Remove the file link to the log file
            RemoveFileLink();
            // Open a new log file
            OpenLogFile();
        }
        // Unlock the list
        Unlock();
    }
    else
    {
        printf("Could Not Lock To Start Log %s: %s\n", m_logFileName.empty() ? "" : m_logFileName.c_str(),
               strerror(err));
    }
}

/**
 * Finalizes the log file data. Updates the link to the log file
 * used by the GUI to know when the log file is valid
 */
void IcmLogFile::EndLog()
{
    int err;
    // Lock list for exclusive access
    if ((err = Lock()) == EOK)
    {
        // Clear any buffered tags
        if (m_tagList != NULL)
            m_tagList->Clear();

        // Close the log file
        CloseLogFile();

        // Update the file link to the log file
        UpdateFileLink();

        // Unlock the list
        Unlock();
    }
    else
    {
        printf("Could Not Lock To End Log %s: %s\n", m_logFileName.empty() ? "" : m_logFileName.c_str(),
               strerror(err));
    }
}

/**
 * Adds an entry to the log field with the given data values
 *
 * @param dataIdx ICM array index of this sample
 * @param lf      LF data point
 * @param rf      RF data point
 * @param lr      LR data point
 * @param rr      RR data point
 *
 * @return EOK if successful, other values indicate error
 */
int IcmLogFile::LogSample(uint32_t dataIdx, float lf, float rf, float lr, float rr)
{
    int retVal = EOK;

    // Lock list for exclusive access
    if ((retVal = Lock()) == EOK)
    {
        // If logging is enabled
        if (IsEnabled())
        {
            // If the file handle is valid
            if (m_file != NULL)
            {
                // Put data into the file
                /*
                
                This code has not been tested yet. It is intended to verfiy that the data gets written
                to the log file properly by checking how many bytes were written vs. how many bytes
                we tried to write.
                                    // Verify that data was written
                                    char outLine[ 1024];
                                    int outBytes;
                                    outBytes = sprintf( outLine, , "%+7.3f %+7.3f %+7.3f %+7.3f", lf, rf, lr, rr);
                                    if( fwrite( outLine, 1, outBytes, m_file) != outBytes)
                                    {
                                        retVal = errno;
                                    }
                                    else
                                    {
                                        // Append any tags we have for this sample
                                        AddTagToSample( dataIdx);
                
                                        fprintf( m_file, "\n");
                                    }
                */
                fprintf(m_file, "%+7.3f %+7.3f %+7.3f %+7.3f", lf, rf, lr, rr);

                // Append any tags we have for this sample
                AddTagToSample(dataIdx);

                fprintf(m_file, "\n");
            }
            // else if the file has been specified and is not open
            else if (m_logFileName.empty() == false)
            {
                // No such file or device
                retVal = ENOENT;
            }
        }
        // Unlock the list
        Unlock();
    }
    else
    {
        printf("Error, Could Not Lock To Log A Sample: %s\n", strerror(retVal));
    }

    return(retVal);
}

/**
 * Adds an entry to the log field with the given data values
 *
 * @param dataIdx ICM array index of this sample
 * @param lf      LF data point
 * @param rf      RF data point
 * @param lr      LR data point
 * @param rr      RR data point
 * @param lt      LT data point
 * @param rt      RT data point
 *
 * @return EOK if successful, other values indicate error
 */
int IcmLogFile::LogSample(uint32_t dataIdx, float lf, float rf, float lr, float rr,
                          float lt, float rt)
{
    int retVal;

    // Lock list for exclusive access
    if ((retVal = Lock()) == EOK)
    {
        // If logging is enabled
        if (IsEnabled())
        {
            // If the file handle is valid
            if (m_file != NULL)
            {
                // Put data into the file
                /*
                
                This code has not been tested yet. It is intended to verfiy that the data gets written
                to the log file properly by checking how many bytes were written vs. how many bytes
                we tried to write.
                
                                    // Verify that data was written
                                    char outLine[ 1024];
                                    int outBytes;
                                    outBytes = sprintf( outLine, , "%+7.3f %+7.3f %+7.3f %+7.3f %+7.3f %+7.3f", lf, rf, lr, rr, lt, rt);
                                    if( fwrite( outLine, 1, outBytes, m_file) != outBytes)
                                    {
                                        retVal = errno;
                                    }
                                    else
                                    {
                                        // Append any tags we have for this sample
                                        AddTagToSample( dataIdx);
                
                                        fprintf( m_file, "\n");
                                    }
                */
                fprintf(m_file, "%+7.3f %+7.3f %+7.3f %+7.3f %+7.3f %+7.3f",
                        lf, rf, lr, rr, lt, rt);

                // Append any tags we have for this sample
                AddTagToSample(dataIdx);

                fprintf(m_file, "\n");
            }
            else
            {
                // No such file or device
                retVal = ENOENT;
            }
        }
        // Unlock the list
        Unlock();
    }
    else
    {
        printf("Error, Could Not Lock To Log A Sample: %s\n", strerror(retVal));
    }


    return(retVal);
}

/**
 * Attempts to lock the object for extended use.
 *
 * @param block  Flag indicating if caller wants to block until the lock can
 *               be accomplished (block=true), or retrurn immediately if the
 *               lock cannot be obtained (block=false).
 * @return EOK if successful, other value if an error occurrs
 */
int IcmLogFile::Lock(bool block)
{
    int retVal;
    if (block)
        retVal = m_logFileLock.Acquire();
    else
        retVal = m_logFileLock.TryAcquire();
    if (retVal != EOK)
        printf("Error Locking The Mutex: %d\n", retVal);
    return(retVal);
}

/**
 * Releases a lock on the object (after a successful call to Lock())
 *
 * @return EOK if successful, other value if an error occurs
 */
int IcmLogFile::Unlock()
{
    return(m_logFileLock.Release());
}

/**
 * Return the status of the log file.
 *
 * @return The status of logging to file.
 *         true - logging, false - not logging
 */
bool IcmLogFile::IsEnabled(void)
{
    return(m_enabled);
}

/**
 * Set the enabled status of the log file.
 *
 * @param enableStatus The status to update the logger with.
 */
void IcmLogFile::SetEnabled(bool enableStatus)
{
    m_enabled = enableStatus;
}

/**
 * Returns the name of the log file we are saving data to
 *
 * @return The name of the log file we are saving data to
 */
const std::string& IcmLogFile::GetName() const
{
    return(m_logFileName);
}

/**
 * Opens a new log file
 *
 * @return true if file opened successfully, false if an error occurred
 */
bool IcmLogFile::OpenLogFile()
{
    bool retVal = false;

    if (Lock() == EOK)
    {
        // Make sure file is closed
        CloseLogFile();

        if (IsEnabled())
        {
            // If file name is valid
            if (m_logFileName.empty() == false)
            {
                // Create the file
                if ((m_file = fopen(m_logFileName.c_str(), "w+")) != NULL)
                {
                    retVal = true;
                }
                else
                {
                    printf("Error Opening Log File: %s\n", m_logFileName.empty() ? "" : m_logFileName.c_str());
                }
            }
            else
                printf("Log File Name is empty\n");
        }

        Unlock();
    }
    else
        printf("Could Not Lock Log File: %s\n", m_logFileName.empty() ? "" : m_logFileName.c_str());

    return(retVal);
}

/**
 * Closes an existing log file
 */
void IcmLogFile::CloseLogFile()
{
    if (Lock() == EOK)
    {
        if (m_file != NULL)
        {
            fclose(m_file);
            m_file = NULL;
        }

        Unlock();
    }
    else
        printf("Error, Could Not Lock To Close the File\n");

}

/**
 * Updates (creates) the file link to the log file at the end of a test.
 * The link is used by the GUI to know when the log files are valid
 *
 * @return -1 if error occurs, any other value indicates success
 */
int IcmLogFile::UpdateFileLink()
{
    int retVal;

    if ((retVal = Lock()) == EOK)
    {
        if (m_logFileLink.empty() == false)
        {
            // Create FTP link to data log file
            retVal = symlink(m_logFileName.c_str(), m_logFileLink.c_str());
            // if not created and does not exist
            if ((retVal != EOK) && (errno != EEXIST))
                printf("Error creating Link: %s, %d\n", m_logFileLink.c_str(), retVal);
        }
        else
        {
            // No such file or device
            errno = ENOENT;
            printf("No Link Name Entered\n");
        }

        Unlock();
    }
    else
        printf("Could Not Update Link: %s\n", m_logFileLink.c_str());

    return(retVal);
}

/**
 * Removes the file link to the log file at the beginning of a test.
 * The link is used by the GUI to know when the log files are valid
 */
void IcmLogFile::RemoveFileLink()
{
    if (Lock() == EOK)
    {
        if (m_logFileLink.empty() == false)
        {
            // Remove the FTP link to the data log file
            unlink(m_logFileLink.c_str());
        }
        else
        {
            // No such file or device
            errno = ENOENT;
        }

        Unlock();
    }
}

/**
 * Appends a sample tag to the log file for the given sample index
 *
 * @param dataIdx Index of the sample to append tags to in the log file
 */
void IcmLogFile::AddTagToSample(uint32_t dataIdx)
{
    if (m_tagList != NULL)
    {
        if (m_tagList->Lock() == EOK)
        {
            try
            {
                // Check if we have any tags for this sample
                if (m_tagList->empty() == false)
                {
                    IcmTagListItr_t itr = m_tagList->FindFirstTag(dataIdx);
                    while (itr != m_tagList->end())
                    {
                        fprintf(m_file, " ~%s", itr->second.c_str());
                        itr = m_tagList->FindNextTag(itr);
                    }
                }
            }
            catch (...)
            {
                printf("ERROR: Exception occurred in IcmLogFile::AddTagToSample()\n");
            }

            m_tagList->Unlock();
        }
    }
}

