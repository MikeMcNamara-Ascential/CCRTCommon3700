//*************************************************************************
// Description:
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/GeneralUtilities.h $
// 
// 8     10/30/07 3:36p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added ApplyForceMultiplier().
// 
// 7     10/26/06 9:13a Rwiersem
// Changes for the 06102701 core release:
// 
// Added methods for determining if a configuration file has been
// modified.
// 
// 6     4/26/06 6:52p Cward
// Moved atob from GenericTC to gerneral Utilities
//
// 5     4/13/06 3:35a Cward
// Added Fix Up Path String function
//
// 2     2/15/06 3:50p Bmeinke
// Added FixUpPathString() function
//
// 4     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     2/18/05 12:00p Bmeinke
// Added Dan E's changes to only open the ICM binary data files once per
// component lifetime
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:09p Khykin
// Initial checkin.
//
// 2     4/30/03 4:14p Khykin
//
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:36  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:04a Khykin
// Initial Checkin
//
// 1     1/18/03 8:02a Khykin
// Initial Checkin From DCTC
//
// 18    11/19/02 1:35p Tbochene
// Updated prototype of GetBinaryData() to take void* and size of array in
// bytes.
//
// 17    11/08/02 8:24a Tbochene
// Added GetBinaryData()
//
// 16    7/31/02 11:30a Skypig13
// Added SignalCheck()
//
// 15    5/20/02 4:34p Dan.erickson
// Updated comments for GetTimeAndDate().
//
// 14    4/24/02 3:12p Dan.erickson
// Updated CalculateCheckSum() method to use and return unsigned shorts.
//
// 13    4/16/02 7:27a Dan.erickson
// Added ConvertStatusToResponse().
//
// 12    4/05/02 7:37a Dan.erickson
// Updated CalculateCheckSum() to take a const std::string reference.
//
// 11    4/03/02 2:14p Dan.erickson
// Added CalculateChecksum() and GetTimeAndDate() methods.
//
// 10    1/15/02 11:24p Skypig13
// Added LoadDataFile() and ParseLine()
//
// 9     8/03/01 10:57p Brian.meinke
// Added #include <iostream> for cout (RTP v 6.1)
//
// 8     5/08/01 12:47p Kh
// Adding method that will replace the &lt; and &gt; symbols in an XML
// document with "<" and ">".
//
// 7     4/25/01 3:11p Kh
// Adding a utility to be able to compare two files for a match and a
// utility to open a file and verify that a string exists in the file
// (primary use for this function is during testing).
//
// 6     4/20/00 2:51p Kh
// Changing INT8 to char due to changes in code (was returning an INT8
// from CreateMessage)
//
// 4     4/20/00 8:55a Kh
// Changing from a class to functions.  gcc forces the use of the class
// name even for static methods.
//
// 3     4/19/00 10:20a Kh
// Added the Create Message Method.
//
// 2     4/18/00 2:05p Kh
// Adding Create Message Utility.
//
// 1     4/17/00 1:49p Kh
// Initial Version.
//
//
//*************************************************************************
#ifndef GeneralUtilities_h
#define GeneralUtilities_h

#include "BepException.h"
#include "BepDefs.h"

#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdarg.h>

//==============================================================================

/**
 * Data type definition for a block for controlling data
 * and access to it
 * @since Version 1.0
 */
typedef struct _DataBlock
{
    /**
     * Current index into data array
     * @since Version 1.0
     */
    UINT32 index;
    /**
     * Container where data is located
     * @since Version 1.0
     */
    vector<std::string> data;
} DataBlock;
/**
 * Data type definition of tag/DataBlock pair
 * @since Version 1.0
 */
typedef pair<std::string, DataBlock> DcbPair;
/**
 * Data type definition of a container of blocks used to control
 * data and acces to it
 * @since Version 1.0
 */
typedef vector<DcbPair> DataControlBlock;
/**
 * Data type definition of a iterator for container of blocks used to control
 * data and acces to it
 * @since Version 1.0
 */
typedef DataControlBlock::iterator DataControlBlockIter;
/**
 * Data type definition of a value of blocks used to control
 * data and acces to it
 * @since Version 1.0
 */
typedef DataControlBlock::value_type DataControlBlockValue;
/**
 * Method used to load a specified file and return
 * it as a string.
 *
 * @param fileName The name of the file to read in.
 * @return The specified file as a string.
 */
string LoadFile(const string &fileName);
/**
 * Creates a compound message (text and data) and returns
 * it as a character array. This is thread safe.
 *
 * @param buffer The buffer to create the message in.
 * @param length The length of the provided buffer.
 * @param cpFmt  String with formating characters in it to fill followed
 *               by the data to insert into it.
 * @return Character array with the data pass included.
 */
char *CreateMessage(char *buffer, size_t length, const char *cpFmt, ...);
/**
 * This function will load and compare two text files.
 *
 * @param file1  The name of the first file to load.
 * @param file2  The name of the second file to load.
 * @return The results of the file match.
 *         1  = Files Did Not Match
 *         0  = Files Matched
 *         -1 = Could not open one of the files.
 */
int FileCompare(const char *file1, const char *file2);
/**
 * Utility that opens the specified file and looks for
 * the existence of a string of text.
 *
 * @param fileName The name of the file to search.
 * @param text     The text string to search for.
 * @return If the text was found. 1 = TRUE, 0 = FALSE
 */
int ValidateTextInFile(const char *fileName, const char *text);
/**
 * Utility that opens the specified file and looks for
 * the existence of a string of text.
 *
 * @param fileName The name of the file to search.
 * @param text     The text string to search for.
 * @return If the text was found. 1 = TRUE, 0 = FALSE
 */
int ValidateTextInFile(const std::string &fileName, const std::string &text);
/**
 * Utility that will take a string and convert the
 * Standard Generalized Markup Language characters to
 * the standard string symbols.
 * Ex. "&lt" will be converted to "<" and
 *     "&gt" will be converted to ">".
 * Note: the other option is to hand edit the xml file
 * and put in the &lt; and &gt; symbols.  The parser
 * will convert these to "<" and ">" for you.
 *
 * @param value  The string that should be searched and updated.
 */
void ConvertSGandLString(std::string &value);
/**
 * Load data from a file (tab separated with tags as headers)
 *
 * @param file      Name of file to load
 * @param dcb       Data control block to use
 * @param delimiter Delimiter between strings in the line
 * @param debug     If printouts should be sent to console
 * @since Version 1.0
 */
void LoadDataFile(const std::string &file, DataControlBlock &dcb, const std::string delimiter, const bool debug);
/**
 * Parse line of file
 *
 * @param line      If the data being passed in si first in the file
 * @param dcb       Data control block to use
 * @param start     If this is first run
 * @param delimiter Delimiter between strings in the line
 * @param debug     If printouts should be sent to console
 * @since Version 1.0
 */
void ParseLine(std::string line, DataControlBlock &dcb, bool start, const std::string delimiter, const bool debug);
/**
 * Calculate the cheksum for the specified data by summing up all the bytes.
 *
 * @param data   Data to calculate the checksum for.
 * @return The checksum.
 * @since Version 1.1
 */
const UINT16 CalculateCheckSum(const std::string &data);
/**
 * Method to return the current system date and time.  This method uses the ANSI C strftime to format
 * the time string according to the parameter.
 *
 * @param dateTime Which portion of the date time to return.
 *                 Date - Return just the date [MMDDYYYY].
 *                 Time - Return just the time [HHMM].
 *                 Both - Return the entire date time string [MMDDYYYYHHMM].
 *                 You can also specify your own format using the time format descriptors.
 * @return Date time string.
 * @since Version 1.1
 */
const std::string GetTimeAndDate(const std::string &dateTime);
/**
 * Converts a BEP_STATUS_* to a BEP_*_RESPONSE.
 * For example, BEP_STATUS_SUCCESS would be converted to BEP_SUCCESS_RESPONSE.
 *
 * @param status Status to convert.
 * @return The status converted to a response.
 * @since Version 1.2
 */
const std::string ConvertStatusToResponse(const INT32 status);

/**
 * Converts an INT32 to a BEP_STATUS_TYPE.
 * 
 * @param status Integer status to be converted to BEP_STATUS_TYPE.
 * 
 * @return Equivalent BEP_STATUS_TYPE.
 */
BEP_STATUS_TYPE ConvertIntToBepStatus(const INT32 &status);

/**
 * Checks to see if a signal has interrupted a blocking kernel call.
 * Returns true if status was Ok, signal did not match test value,
 * or count has been exhausted else false.
 *
 * @param stat   Status returned by kernel call
 * @param sig Signal we are interested in checking for
 * @param count  Number of retries
 * @param debug  Should diagnostics be printed
 * @return If the check is Ok
 * @since Version 1.3
 */
const bool SignalCheck(const INT32 stat, const INT32 sig, INT32& count, const bool debug = false);
/**
 * Read data from file.
 * If the file descriptor passed in is -1, the file will be
 * opened for reading, otherwise, data will just be read from
 * the file descriptor provided.
 * NOTE:  The data file will not be closed until
 *        CloseBinaryDataFile() is called.
 *
 * @param data   Array to hold dta
 * @param size   Size of array (in bytes)
 * @param file   File to read
 * @param fileDescriptor
 *               File descriptor to read data from.
 *               If this is set to -1, the file will be opened and the
 *               new file descriptor returned to the calling program.
 *
 * @return Status of operation
 * @see CloseBinaryDataFile
 * @since Version 2.4
 */
BEP_STATUS_TYPE GetBinaryData(void* data, const UINT32 size, std::string& file, INT32 &fileDescriptor);

/**
 * Close the Binary data file opened by GetBinaryData.
 * The file will be closed and the file descriptor
 * will be reset to -1.
 *
 * @param fileDescriptor
 *               File descriptor to be closed.
 *
 * @return BEP_STATUS_SUCCESS - file was closed properly.
 *         BEP_STATUS_FAILURE - file was not closed properly.
 * @see GetBinaryData
 */
BEP_STATUS_TYPE CloseBinaryDataFile(INT32 &fileDescriptor);

/**
 * Expands environment variable names in the given path
 *
 * @param path   Path to be cleaned up
 *
 * @return EOK if successful, error code if an error occurred
 */
int FixUpPathString( std::string &path);

/**
 * Convert a string to a bool.
 * Recognized values for true are (case insensitive):
 *   1, true, yes, on, enable, enabled, y and t
 *
 * @param boolString String to convert into a bool value
 *
 * @return true or false
 */
bool atob( const char *boolString);

/**
 * Returns the basename (everything after the last '/') of the given file.
 * 
 * @param fileName The file to get the basename for
 * 
 * @return The basename (everything after the last '/') of the given file.
 */
std::string Basename( const std::string &fileName);

/**
 * Creates a unique IPC channel name using an executabel file name and a
 * config file name
 * 
 * @param progName The name of the executable file to be used when creating the IPC
 *                 channel name
 * @param cfgFile  The name of the config file to be used when creating the IPC
 *                 channel name
 * 
 * @return A unique IPC channel name using an executabel file name and a
 *         config file name
 */
std::string CreateUniqueIpcName( std::string progName, std::string cfgFile);

/**
 * Get the modification time of the given file
 * 
 * @param fileName Name of the file to get the last modification time for
 * 
 * @return The number of seconds (since Jan 1, 1970) since the file was last modified
 */
time_t GetFileModificationTime( const std::string &fileName);

/**
 * Check if a file has changed since a given time using the file modification
 * time
 * 
 * @param fileName Name of the file to check
 * @param lastModTime
 *                 Last known modification time of the file
 * 
 * @return true if the file's current modification time is greater than the last known
 *         modification time
 */
bool FileHasChanged( const std::string &fileName, const time_t &lastModTime);

/**
 * Get the process ID for a component given its registered IPC channel name
 * 
 * @param ipcName The IPC channel name registered by the process we want to get the PID for
 * 
 * @return The PID for the desired process or -1 if the PID cannot be determined
 */
int GetProcecssPid( const std::string &ipcName);

/**
 * Apply the specified force multiplier to the raw frce provided.
 * If the multiplier is zero, the rawForce value will be returned.
 * 
 * @param rawForce   Raw force data to apply the force multiplier to.
 * @param multiplier Multiplier to be applied to the rawForce provided.
 * 
 * @return The rawForce with the multiplier applied to it, or the rwForce if the multiplier is zero..
 */
float ApplyForceMultiplier(const float &rawForce, const float &multiplier);

#endif // GeneralUtilities_h

