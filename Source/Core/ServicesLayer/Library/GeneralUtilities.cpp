//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/GeneralUtilities.cpp 7     10/30/07 3:53p Rwiersem $
//
// FILE DESCRIPTION:
//   The is the base component for the RTOS system it will provide the base
//   functionality for the majority of the system.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/GeneralUtilities.cpp $
// 
// 7     10/30/07 3:53p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed the mask for the return value in CalculateCheckSum().
// - Added ApplyForceMultiplier().
// 
// 6     10/26/06 9:56a Rwiersem
// Changes for the 06102701 core release:
// 
// Added methods for determining if a configuration file has been
// modified.
// 
// 5     4/26/06 6:52p Cward
// Moved atob from GenericTC to gerneral Utilities
//
// 4     4/13/06 3:40a Cward
// Added FixUpPathString
//
// 2     2/15/06 3:50p Bmeinke
// Added FixUpPathString() function
//
// 3     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 2     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:53p Khykin
// Initial checkin.
//
// 2     4/30/03 4:15p Khykin
// Updating the CreateMessage method to only have a thread safe version.
//
// 1     4/22/03 9:32a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:41  tbochene
//
//    PooBag
//
//
// 1     1/27/03 6:25a Khykin
// Initial Checkin
//
// 1     1/18/03 7:57a Khykin
// Initial Checkin From DCTC
//
// 26    11/19/02 1:34p Tbochene
// Updated prototype of GetBinaryData() to take void* and size of array in
// bytes.
//
// 25    11/08/02 8:24a Tbochene
// Added GetBinaryData()
//
// 24    10/09/02 9:17a Derickso
// Modified LoadFile to check for file existence for attmepting to
// open.  Will thorw BepException if file can not be accessed.
//
// 23    7/31/02 10:48a Skypig13
// Added SignalCheck()
//
// 22    4/25/02 4:36p Skypig13
// Updated with new changes
//    Revision 1.2  2002/04/24 01:02:29  erickson
//    Version from VSS
//
//    Revision 1.1.1.1  2002/04/08 19:49:05  erickson
//    started
//
//
// 18    4/05/02 7:37a Dan.erickson
// Updated CalculateCheckSum() to take a const std::string reference.
//
// 17    4/03/02 2:14p Dan.erickson
// Added CalculateChecksum() and GetTimeAndDate() methods.
//
// 16    2/05/02 2:56p Skypig13
// Removed compiler warnings
//
// 15    2/04/02 11:40p Skypig13
// Removed compiler warnings
//
// 14    1/15/02 11:24p Skypig13
// Added LoadDataFile() and ParseLine()
//
// 13    9/27/01 11:46a Dan.erickson
// Decreased delay time in LoadFille method from 1000ms to 1ms.
//
// 12    8/17/01 2:35p Kh
// Updating the LoadFile method to make three attempts and to check for an
// EINTR signal so that a file close will be reattempted.
//
// 11    8/01/01 4:37p Kh
// Adding exception handling.
//
// 10    5/08/01 12:48p Kh
// Adding method that will replace the &lt; and &gt; symbols in an XML
// document with "<" and ">".
//
// 9     5/03/00 3:45p Kh
// Adding memset to the LoadFile method.
//
// 7     4/25/01 3:10p Kh
// Adding a utility to be able to compare two files for a match and a
// utility to open a file and verify that a string exists in the file
// (primary use for this function is during testing).
//
// 6     4/20/00 2:51p Kh
// Changing INT8 to char due to changes in code (was returning an INT8
// from CreateMessage)
//
// 5     4/20/00 8:54a Kh
// Changing from a class to functions.  gcc forces the use of the class
// name even for static methods.
//
// 4     4/19/00 10:21a Kh
// Added the Create Message Method.
//
// 3     4/18/00 2:05p Kh
// Adding Create Message Utility.
//
// 2     4/17/01 2:13p Brian.meinke
// Read entire file in one shot instead of one char at a time
//
// 1     4/17/00 1:36p Kh
// Initial Version.
//
//
//*************************************************************************

#include <time.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#include "Bepos.h"
#include "BepDefs.h"
#include "GeneralUtilities.h"

#ifdef _NTO_VERSION
    #if (_NTO_VERSION >= 630)
        #define vsnprintf std::vsnprintf
    #endif
#endif
//==============================================================================

std::string LoadFile(const std::string &fileName)
{
    char            *fileBuff=NULL;
    std::string     buffer = "";
    int             bytesRead, ii=0;
    int             fileFd;
    struct stat     fileStat;

    // Check for file existence
    if (access(fileName.c_str(), F_OK) == EOK)
    {   // The file exists
        do
        {
            const char *name = fileName.c_str();
            // open the specified file
            fileFd = open( name, O_RDONLY);
            if( fileFd != -1)    // If file opened
            {
                try
                {
                    lseek( fileFd, 0, SEEK_SET);
                    fstat( fileFd, &fileStat);
                    // Allocate buffer
                    fileBuff = new char[ fileStat.st_size+1];
                    // reset the buffer
                    memset(fileBuff, 0, fileStat.st_size + 1);
                    // Read file contents
                    bytesRead = read( fileFd, fileBuff, fileStat.st_size);
                    // Convert to string
                    buffer = string( fileBuff);
                    // Free memoryt for buffer
                    delete[] fileBuff;
                    // Close file (this can be interrupted so try until successfull)
                    while( close( fileFd) == -1)
                    {
                        delay( 1);
                    }
                }
                catch (...)
                {   // for whatever reason there was a problem, close the file
                    if (fileBuff)    delete[] fileBuff;
                    // Close file (this can be interrupted so try until successfull)
                    while( close( fileFd) == -1)
                    throw BepException("Error Reading: %s, Because: %s", fileName.c_str(), strerror(errno));
                }
            }
            else
                delay(1);
        }while ((buffer == "") && (ii++ < 3));
    }
    else
    {   // Throw an exception
        throw BepException("File does not exist: %s - %s", fileName.c_str(), strerror(errno));
    }
    if (buffer == "")
    {
        throw BepException("Unable to open %s, Because: %s", fileName.c_str(), strerror(errno));
        printf("Unable to open %s, Because: %s", fileName.c_str(), strerror(errno));
    }

    return(buffer);
}

char *CreateMessage(char *buffer, size_t length, const char *cpFmt, ...)
{
    va_list         vaArgs;

    // clear the print buffer
    memset(buffer, 0 , length);
    // format the input into a String
    va_start( vaArgs, cpFmt);
    // Limit the size of the message so that it fits into the buffer (else SIGSEGV)
    vsnprintf(buffer, length-1, (char *)cpFmt, vaArgs);
    // remove left overs
    va_end( vaArgs);
    // done.  this much printed
    return(buffer);
}
int FileCompare(const char *file1, const char *file2)
{
    string first, second;       // stream objects for both files
    int status = 1;             // default to compare fail

    try
    {
        first = LoadFile(file1);        // load the first file
        second = LoadFile(file2);       // load the second file
        status = first.compare(second); // compare the files
    }
    catch (BepException &e)
    {
        if (DEBUG)
            cout << "Could Not FileCompare: " << e.Reason() << endl;
        status = -1;
    }
    catch (...)
    {
        if (DEBUG)
            cout << "FileCompare Unknown Exception" << endl;
    }

    return(status);
}
int ValidateTextInFile(const char *fileName, const char *text)
{
    int status=0;   // set to not found

    try
    {
        string file = LoadFile(fileName);   // load the file
        unsigned int pos = file.find(text);         // look for the text
        status = (pos != string::npos);
    }
    catch (BepException &e)
    {
        cout << "ValidateTextInFile BepException: " << e.Reason() << endl;
    }
    catch (...)
    {
        cout << "ValidateTextInFile Unknown Exception" << endl;
    }

    return(status);     // return if the text existed
}
int ValidateTextInFile(const std::string &fileName, const std::string &text)
{
    return(ValidateTextInFile(fileName.c_str(), text.c_str()));
}
void ConvertSGandLString(std::string &value)
{
    string tempString;                  // temporary string to manipulate
    string tempString2;                 // temporary string to manipulate
    std::string::size_type startPos=0;  // start position holder
    std::string::size_type endPos=0;    // end position holder

    tempString2 = value;        // use the second string as the first value holder

    // look for the "&lt" symbol
    while (endPos != std::string::npos)
    {
        endPos = tempString2.find("&amp;lt", startPos);         // determine if the less than symbol exists
        tempString += tempString2.substr(startPos, endPos); // copy the substring
        if (endPos != std::string::npos)                    // determine if < exists
            tempString += "<";                              // if so, put one in
        startPos = endPos + 7;                              // increment to the next possible position
    }
    // now lets look for the "&gt" symbol
    startPos = endPos = 0;      // reset the position variables
    tempString2.erase();        // clear the temp string
    while (endPos != std::string::npos)
    {
        endPos = tempString.find("&amp;gt", startPos);          // determine if the less than symbol exists
        tempString2 += tempString.substr(startPos, endPos); // copy the substring
        if (endPos != std::string::npos)                    // determine if < exists
            tempString2 += ">";                             // if so, put one in
        startPos = endPos + 7;                              // increment to the next possible position
    }
    value = tempString2;
}
void LoadDataFile(const std::string &file, DataControlBlock &dcb, const std::string delimiter, const bool debug)
{
    bool start = true;
    std::string error = std::string();
    if (debug) printf("SimulationServer::LoadDataFile(%s)\n", file.c_str());
    ifstream inFile;
    // open the specified file
    inFile.open(file.c_str());
    if (inFile) // If file opened
    {
        try
        {   // Get a line of data
            std::string textLine;
            DataControlBlock tempDcb;
            while (getline(inFile, textLine, '\n'))
            {   // Loop through all lines of the file
                textLine += delimiter;  // Replace '\n' with ','
                if (debug) printf("Line read: %s\n", textLine.c_str());

                ParseLine(textLine, tempDcb, start, delimiter, debug);
                start = false;
            }
            while (inFile != NULL)
                inFile.close(); // Close file (this can be interrupted so try until successfull)
            for (UINT16 ii = 0; ii < tempDcb.size(); ii++)
            {
                dcb.push_back(tempDcb[ii]);
            }
        }
        catch (...)
        {   // for whatever reason there was a problem, close the file
            while (inFile != NULL)
                inFile.close(); // Close file (this can be interrupted so try until successfull)
            error += strerror(errno);
            throw BepException(error);
        }
    }
    else throw BepException("Error Reading: %s  Because: ", file.c_str(), strerror(errno));

    if (debug) printf("SimulationServer::LoadDataFile(%s) done\n", file.c_str());
}
void ParseLine(std::string line, DataControlBlock &dcb, bool start, const std::string delimiter, const bool debug)
{
    if (debug) printf("SimulationServer::ParseLine(%s)\n", line.c_str());
    DcbPair tempPair;
    std::string::size_type endIndex = 0;    // Data pointers
    UINT16 dcbIndex = 0;
    while (line.length() > 1)
    {
        if (debug) printf("Looking for string ending in '%s' string of length %d\n", delimiter.c_str(), line.length());
        endIndex = line.find(delimiter.c_str());    // Find next delimiter
        tempPair.first = line.substr(0, endIndex);
        line = line.substr(endIndex+1, line.length()-1);
        if (debug) printf("Found %s\n", tempPair.first.c_str());
        if (!start) dcb[dcbIndex++].second.data.push_back(tempPair.first);
        else dcb.push_back(tempPair);
    }
    start = false;
    if (debug) printf("SimulationServer::ParseLine(%s) done\n", line.c_str());
}
const UINT16 CalculateCheckSum(const std::string &data)
{
    UINT16 checkSum = 0;
    // Sum up all the bytes
    for (UINT16 ii = 0; ii < data.length(); ii++)
        checkSum += (UINT16)data[ii];
    // Return the checksum
    return(checkSum & 0xFFFF);
}
const std::string GetTimeAndDate(const std::string &dateTime)
{   // Get the current date and time from the system
    time_t systemTime = time(NULL);
    struct tm *currentTime = localtime(&systemTime);
    // Convert the date time string to what the requestor asked for
    std::string result, formatString;
    char timeBuffer[20];
    if (dateTime == "Time")
    {   // Pull the time out of the time date string HHMM
        formatString = std::string("%H%M");
    }
    else if (dateTime == "Date")
    {   // Pull the date out of the date time string MMDDYYYY
        formatString = std::string("%m%d%Y");
    }
    else if (dateTime == "Both" || dateTime.empty())
    {   // Pull the date and time out of the string
        formatString = std::string("%m%d%Y%H%M");
    }
    else
    {   // The caller specified the format
        formatString = dateTime;
    }
    strftime(timeBuffer, sizeof(timeBuffer), formatString.c_str(), currentTime);
    result = timeBuffer;
    return(result);
}
const std::string ConvertStatusToResponse(const INT32 status)
{
    // Set the responses
    std::string result;
    switch( status)
    {
    case BEP_STATUS_ERROR:          result = BEP_ERROR_RESPONSE;            break;
    case BEP_STATUS_SUCCESS:        result = BEP_SUCCESS_RESPONSE;          break;
    case BEP_STATUS_NA:             result = BEP_UNAVAILABLE_RESPONSE;      break;
    case BEP_STATUS_FAILURE:        result = BEP_FAILURE_RESPONSE;          break;
    case BEP_STATUS_ABORT:          result = BEP_ABORT_RESPONSE;            break;
    case BEP_STATUS_TIMEOUT:        result = BEP_TIMEOUT_RESPONSE;          break;
    case BEP_STATUS_SOFTWARE:       result = BEP_SOFTWAREFAIL_RESPONSE;     break;
    case BEP_STATUS_HARDWARE:       result = BEP_HARDWAREFAIL_RESPONSE;     break;
    case BEP_STATUS_SKIP:           result = BEP_SKIP_RESPONSE;             break;
    case BEP_STATUS_END_OF_LIST:    result = BEP_NO_IMPLEMENTED_RESPONSE;   break;
    default:                        result = BEP_NO_IMPLEMENTED_RESPONSE;   break;
    }

    return(result);
}

BEP_STATUS_TYPE ConvertIntToBepStatus(const INT32 &status)
{
    BEP_STATUS_TYPE bepStatus = BEP_STATUS_NA;
    switch(status)
    {
    case -1:   bepStatus = BEP_STATUS_ERROR;           break;
    case 0:    bepStatus = BEP_STATUS_SUCCESS;         break;
    case 1:    bepStatus = BEP_STATUS_NA;              break;
    case 2:    bepStatus = BEP_STATUS_FAILURE;         break;
    case 3:    bepStatus = BEP_STATUS_ABORT;           break;
    case 4:    bepStatus = BEP_STATUS_TIMEOUT;         break;
    case 5:    bepStatus = BEP_STATUS_SOFTWARE;        break;
    case 6:    bepStatus = BEP_STATUS_HARDWARE;        break;
    case 7:    bepStatus = BEP_STATUS_SKIP;            break;
    case 8:    bepStatus = BEP_STATUS_END_OF_LIST;     break;
    default:   bepStatus = BEP_STATUS_NA;              break;
    }
    return bepStatus;
}

const bool SignalCheck(const INT32 stat, const INT32 sig, INT32& count, const bool debug /* = false */)
{
    bool done = true;   // Status checker

    if (debug) printf("SignalCheck(%d, %d, %d, %d)\n", stat, sig, count, debug);
    if (stat == -1)
    {   // If failed check to see if the correct signal was received
        if (debug) printf("stat == fail\n");
        if (errno == sig)
        {   // If correct signal, see if the number of retries has been exceeded
            if (debug) printf("errno = %d, signal = %d\n", errno, sig);
            if (--count < 0)
            {   // If maximum number of retries has been exceeded, we are done
                if (debug) printf("count expired\n");
            }
            else done = false;  // Keep trying
        }
    }
    if (debug) printf("SignalCheck(%d, %d, %d, %d) done = %d\n", stat, sig, count, debug, done);
    return(done);
}
BEP_STATUS_TYPE GetBinaryData(void* data, const UINT32 size, std::string& file, INT32 &fileDescriptor)
{
    BEP_STATUS_TYPE testStatus = BEP_STATUS_FAILURE;      // overall status

    try
    {   // clear out the wheel speeds
        memset(data, 0, size);

        int retry = 3;   // the number of retrys to attempt
        do
        {   // Check if the file has already been opened
            if (fileDescriptor == -1)
            {   // File was not opened, open the file
                fileDescriptor = open(file.c_str(), O_RDONLY);
            }
            // read the data from the file if it is open
            if(fileDescriptor != -1)
            {
                lseek(fileDescriptor, 0, SEEK_SET);
                int bytes = read(fileDescriptor, data, size);
                if(bytes != (int)size)
                {   // if interrupted during a read, read again
                    if(errno == EINTR)
                    {
                        printf("Interrupted Trying To Read %s\n", file.c_str());
                    }
                    else
                    {
                        printf("Error Reading The File Information: %d, %s\n", bytes, strerror(errno)); // else display the error report
                    }
                }
                else
                {
                    testStatus = BEP_STATUS_SUCCESS;
                }
            }
            else
            {
                testStatus = BEP_STATUS_SOFTWARE;
            }
            // if the file could not be open, give up the time slice
            if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
        }
        while((testStatus != BEP_STATUS_SUCCESS) && (retry--));
    }
    catch(BepException &e)
    {
        testStatus = BEP_STATUS_ERROR;
    }
    return(testStatus);
}

BEP_STATUS_TYPE CloseBinaryDataFile(INT32 &fileDescriptor)
{
    BEP_STATUS_TYPE closeStatus = BEP_STATUS_ERROR;
    if (-1 != fileDescriptor)
    {
        closeStatus = close(fileDescriptor) == 0 ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
        fileDescriptor = -1;
    }
    else
    {
        closeStatus = BEP_STATUS_SUCCESS;
    }
    return closeStatus;
}

int FixUpPathString( std::string &path)
{
    std::string::size_type  envStart, envEnd;
    char                    *envValue;
    char                    realPath[ 1024+1];
    int                     status;

    // Look for simple HOME character (~)
    while( (envStart=path.find('~')) != path.npos)
    {
        // Erase the '~' and replace it with $(HOME)
        path.erase( envStart, 1);
        char *homeDir = getenv( "HOME");
        if( homeDir != NULL)    path.insert( envStart, homeDir);
        else                    path.insert( envStart, "/home");
    }

    // Now replace any '$()' delimitted environ vars
    while( (envStart=path.find("$(")) != path.npos)
    {
        // Look for the end of the environ var name
        if( (envEnd = path.find(')', envStart)) != path.npos)
        {
            // Get the environment variable name
            std::string envName( path.substr( envStart+2, envEnd-envStart-2));
            // If it is a valid envirnoment variable name
            if( (envValue=getenv( envName.c_str())) != NULL)
            {
                // Replace it in the string
                path.erase( envStart, (envEnd-envStart+1));
                path.insert( envStart, getenv( envName.c_str()));
            }
        }
    }

    // Now resolve relative path to absolute path
    if(NULL != realpath(path.c_str(), realPath))
    {
        path = std::string(realPath);
        status = EOK;
    }
    else
    {
        status = errno;
    }

    return( status);
}

bool atob( const char *boolString)
{
    bool retVal = false;

    if( NULL != boolString)
    {
        retVal = ( (strnicmp( "1", boolString, 1) == 0) ||
                   (strnicmp( "true", boolString, 4) == 0) ||
                   (strnicmp( "yes", boolString, 3) == 0) ||
                   (strnicmp( "on", boolString, 2) == 0) ||
                   (strnicmp( "enable", boolString, 6) == 0) ||
                   ((strnicmp( "y", boolString, 1) == 0) && (1 == strlen( boolString))) ||
                   ((strnicmp( "t", boolString, 1) == 0) && (1 == strlen( boolString))) );
    }
    return( retVal);
}

std::string Basename( const std::string &fileName)
{
    char junk[ fileName.size() + 1];
    strcpy( junk, fileName.c_str());
    return( std::string( basename( junk)));
}

string CreateUniqueIpcName( std::string progName, std::string cfgFile)
{
    std::string regName( Basename( progName));

    // Resolve relative paths and environment variables
    FixUpPathString( cfgFile);

    // Create a unique IPC channel name based on our qxe name ans config file name
    if( cfgFile[ 0] != '/')     regName = regName + std::string( "/") + cfgFile;
    else                        regName = regName + cfgFile;

    return( regName);
}

time_t GetFileModificationTime( const std::string &fileName)
{
    struct stat fileStat;
    
    memset( &fileStat, 0, sizeof( fileStat));
    if( false == fileName.empty())
    {
        if( -1 == stat( fileName.c_str(), &fileStat))
        {
            fileStat.st_mtime = 0;
        }
    }
    else
    {
        fileStat.st_mtime = 0;
    }

    return( fileStat.st_mtime);
}

bool FileHasChanged( const std::string &fileName, const time_t &lastModTime)
{
    bool    changed = false;
    time_t modTime = GetFileModificationTime( fileName);
    if( modTime > lastModTime)
    {
        changed = true;
    }
    return( changed);
}

int GetProcecssPid( const std::string &ipcName)
{
    struct _server_info serverInfo;
    int     procPid=-1, coId, status;

    coId = BposGetTaskId(ipcName.c_str());
    if( -1 != coId)
    {
        status = ConnectServerInfo( 0, coId, &serverInfo);
        if( -1 != status)
        {
            procPid = serverInfo.pid;
        }
        BposCloseTaskId( coId);
    }

    return( procPid);
}

float ApplyForceMultiplier(const float &rawForce, const float &multiplier)
{
    return (multiplier != 0) ? (rawForce * multiplier) : rawForce;
}
