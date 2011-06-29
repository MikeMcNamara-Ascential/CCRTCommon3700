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
#ifndef IcmDataSupplier_h
#define IcmDataSupplier_h
//-----------------------------------------------------------------------------
#include "BepDefs.h"
#include "BepSync.h"
#include "XmlNode.h"
#include "IBepCommunication.h"
#include <vector>
//-----------------------------------------------------------------------------
typedef struct
{
    float lf;
    float rf;
    float lr;
    float rr;
} IcmSample;

typedef vector<float>             IcmDataVector;
typedef vector<IcmDataVector>     IcmDataArray;
typedef IcmDataArray::iterator    IcmDataArrayIter;
//-----------------------------------------------------------------------------

class IcmDataSupplier
{
public:
    /**
     * Class constructor.
     * 
     * @param fileNames List of file names to use for the various ICM data to be collected.
     * @param rollerCount
     *                  Number of rollers on the machine.
     */
    IcmDataSupplier(const XmlNode *fileNames, const INT32 &rollerCount);
    /**
     * Class destructor.
     */
    virtual ~IcmDataSupplier();
    /**
     * Open the ICM data files for reading.
     * 
     * @return Status of opening the ICM data files.
     */
    BEP_STATUS_TYPE OpenIcmDataFiles();
    /**
     * Get the current roller speeds.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current wheel speeds from the system.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentSpeeds - Name of the file conatining the current roller speeds. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> IsSpeedFileOpen() </li>
     *      <li> OpenSpeedFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param wheelSpeed The wheel speed array to populate with the current speeds.
     * 
     * @return  Status of reading the roller speeds.
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the wheel speeds were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if the speed file mutex could not be acquired or an exception was thrown. </li>
     *          <li> BEP_STATUS_FAILURE for any other errors. </li>
     *          </ul>
     */
    BEP_STATUS_TYPE GetWheelSpeeds(float wheelSpeed[]);
    /**
     * Get the current wheel distances.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current distance for all wheels.  If the wheel distance file has not yet been opened, it will be opened.
     * The wheel distances will then be read from the file.  The wheel distance file will remain open to expedite
     * further force reads.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentDistances - Name of the current wheel distance file. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetRollerCount() </li>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      <li> OpenWheelDistanceFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param wheelDistance An array for storing the distance values.
     * 
     * @return Status of reading the wheel distances:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the wheel distances were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if an exception is thrown or the wheel distance file mutex could not be acquired. </li>
     *          </ul>
     */
    BEP_STATUS_TYPE GetWheelDistances(float wheelDistance[]);
    /**
     * Get the current forces.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current force for all wheels.  If the force file has not been opened yet, the force file will be opened.
     * The forces will then be read from the file.  The file will remain open to expedite further force reads.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentForces - Name of the file that contains the current forces. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetRollerCount() </li>
     *      <li> IsForceFileOpen() </li>
     *      <li> OpenForceFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param forces An array for storing the force values.
     * 
     * @return Status of reading the wheel forces from the force file:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the forces were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if an exception is thrown or the force file mutex could not be acquired. </li>
     *          </ul>
     */
    BEP_STATUS_TYPE GetForces(float force[]);
    /**
     * Read data from the speed array.
     * The speed array will be read starting at <i>startInex</i> and read up to and including <i>endIndex</i>.
     * The data read from the array will be placed into <i>dataArray</i>.
     * 
     * @param startIndex Index at which to start reading the array.
     * @param endIndex   Index at which to end reading of the array.
     * @param dataArray  Data array to populate with the data.
     * 
     * @return Status of reading the data from the array.
     */
    BEP_STATUS_TYPE ReadSpeedArray(const INT32 &startIndex, const INT32 &endIndex, IcmDataArray &dataArray);
    /**
     * Read data from the force array.
     * The force array will be read starting at <i>startInex</i> and read up to and including <i>endIndex</i>.
     * The data read from the array will be placed into <i>dataArray</i>.
     * 
     * @param startIndex Index at which to start reading the array.
     * @param endIndex   Index at which to end reading of the array.
     * @param dataArray  Data array to populate with the data.
     * 
     * @return Status of reading the data from the array.
     */
    BEP_STATUS_TYPE ReadForceArray(const INT32 &startIndex, const INT32 &endIndex, IcmDataArray &dataArray);
    /**
     * Tag the ICM data arrays with the specified tag.  The current array index will be returned to the calling function.
     * 
     * @param arrayTag Tag to place in the ICM data array.
     * 
     * @return Current array index.
     */
    INT32 TagArray(const string &arrayTag);
    /**
     * Read the current ICM data index and return it to the calling funstion.
     * 
     * @return Current ICM array index.
     */
    INT32 GetCurrentArrayIndex(void);

private:
    /**
     * Read data from the specified array.
     * The specified array will be read starting at <i>startInex</i> and read up to and including <i>endIndex</i>.
     * The data read from the array will be placed into <i>dataArray</i>.
     * 
     * @param arrayFile  ICM data array to read.
     * @param startIndex Index at which to start reading the array.
     * @param endIndex   Index at which to end reading of the array.
     * @param dataArray  Data array to populate with the data.
     * 
     * @return Status of reading the data from the array.
     */
    BEP_STATUS_TYPE ReadIcmArray(const INT32 &arrayFile, const INT32 &startIndex, 
                                 const INT32 &endIndex, IcmDataArray &dataArray);
    /**
     * Open the force file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the ICM force file for reading wheel forces.  A flag will also be set indicating if the file has been 
     * opened successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentForces - Name of the ICM force file to be opened. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetForceFileOpen() </li>
     *      <li> IsForceFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenForceFile(void);
    /**
     * Open the force array file.
     * 
     * @return Status of opening the force array file.
     */
    BEP_STATUS_TYPE OpenForceArrayFile(void);
    /**
     * Close the force file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the force file.  A flag will be set indicating the force file was closed.  If the force file was not opened,
     * nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsForceFileOpen() </li>
     *      <li> SetForceFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the force file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the force file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseForceFile(void);
    /**
     * Close the force array file.  A flag will be set indicating the force array file was closed.  If the force array file
     * was not opened, nothing will be done.
     * 
     * @return Status of closing the force array file.
     */
    BEP_STATUS_TYPE CloseForceArrayFile(void);
    /**
     * Check if the force file has been opened.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the force file is open.
     * <p>
     * @return Flag indicating if the force file is open.
     */
    bool& IsForceFileOpen(void);
    /**
     * Check if the force array file has been opened.
     * 
     * @return Flag indicating if the force array file has been opened.
     */
    bool& IsForceArrayFileOpen(void);
    /**
     * Open the speed file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the speed file for reading wheel speeds.  A flag will also be set indicating if the file has been opened 
     * successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentSpeeds - Name of the current wheels speed file to be opened. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetSpeedFileOpen() </li>
     *      <li> IsSpeedFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenSpeedFile(void);
    /**
     * Open the speed array file.  a flag will be set to indicate the status of opening the spee array file.
     * 
     * @return Status of opening the speed array file.
     */
    BEP_STATUS_TYPE OpenSpeedArrayFile(void);
    /**
     * Close the ICM speed file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the speed file.  A flag will be set indicating the speed file was closed.  If the speed file was not opened,
     * nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsSpeedFileOpen() </li>
     *      <li> SetSpeedFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the speed file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the speed file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseSpeedFile(void);
    /**
     * Close the speed array file.  A flag will be set to indicate if the speed array file has been closed.  If the speed
     * array file has not been opened, nothing will be done.
     * 
     * @return Status of closing the speed array file.
     */
    BEP_STATUS_TYPE CloseSpeedArrayFile(void);
    /**
     * Check if the speed file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the speed file is open.
     * <p>
     * @return Flag indicating if the speed file is open.
     */
    bool& IsSpeedFileOpen(void);
    /**
     * Check if the speed array file is open.
     * 
     * @return Flag indicating if the speed array file is open.
     */
    bool& IsSpeedArrayFileOpen(void);
    /**
     * Open the ICM wheel distance file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the wheel distance file for reading wheel distances.  A flag will also be set indicating of the file has 
     * been opened successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentDistances - Name of the ICM file containing the current wheel distances. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetWheelDistanceFileOpen() </li>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenWheelDistanceFile(void);
    /**
     * Close the ICM wheel distance file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the wheel distance file.  A flag will be set indicating the wheel distance file was closed.  If the wheel
     * distance file was not opened, nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      <li> SetWheelDistanceFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the wheel distance file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the wheel distance file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseWheelDistanceFile(void);
    /**
     * Check if the wheel distance file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the wheel distance file is open.
     * <p>
     * @return The flag indicating if the wheel distance file is open.
     */
    bool& IsWheelDistanceFileOpen(void);
    /**
     * Store the flag indicating if the force file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the force file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of force file.
     */
    void SetForceFileOpen(bool fileOpen);
    /**
     * Set the flag indicating the force array file has been opened.
     * 
     * @param fileOpen Flag indicating the force array file has been opened.
     */
    void SetForceArrayFileOpen(bool fileOpen);
    /**
     * Store the flag indicating if the speed file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the speed file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of speed file.
     */
    void SetSpeedFileOpen(bool fileOpen);
    /**
     * Store the flag indicating the open status of the speed array file.
     * 
     * @param fileOpen Flag indicating if the file is open.
     */
    void SetSpeedArrayFileOpen(bool fileOpen);
    /**
     * Store the flag indicating if the distance file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the wheel distance file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of wheel distance file.
     */
    void SetWheelDistanceFileOpen(bool fileOpen);
    /**
     * Get/Set the number of rollers on the machine.
     * 
     * @param rollerCount
     *               Number of rollers on the machine.
     * 
     * @return Number of rollers on the machine.
     */
    const INT32& RollerCount(const INT32 *rollerCount = NULL);
    /** Number of rollers supported by the system. */
    INT32 m_rollerCount;
    /** Name of the ICM force file. */
    string m_forceFileName;
    /** File descriptor for the force file. */
    INT32 m_forceFile;
    /**
     * Flag indicating the open status of the force file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_forceFileOpen;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_forceFileMutex;
    /** Name of the ICM force array file. */
    string m_forceArrayFileName;
    /** File descriptor for the force array file. */
    INT32 m_forceArrayFile;
    /**
     * Flag indicating the open status of the force array file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_forceArrayFileOpen;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_forceArrayFileMutex;
    /** Name of the ICM speed file. */
    string m_speedFileName;
    /** File descriptor for the speed file. */
    INT32 m_speedFile;
    /**
     * Flag indicating the open status of the speed file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_speedFileOpen;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_speedFileMutex;
    /** Name of the ICM speed array file. */
    string m_speedArrayFileName;
    /** File descriptor for the speed array file. */
    INT32 m_speedArrayFile;
    /**
     * Flag indicating the open status of the speed array file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_speedArrayFileOpen;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_speedArrayFileMutex;
    /** Name of the ICM distance file. */
    string m_distanceFileName;
    /** File descriptor for the wheel distance file. */
    INT32 m_wheelDistanceFile;
    /**
     * Flag indicating the open status of the wheel distance file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_wheelDistanceFileOpen;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_wheelDistanceFileMutex;
    /** Name of the ICM distance array file. */
    string m_distanceArrayFileName;
    /** File descriptor for the distance array file. */
    INT32 m_distanceArrayFile;
    /**
     * Flag indicating the open status of the distance array file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_distanceArrayFileOpen;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_distanceArrayFileMutex;
    /**
     * Communication object to use for tagging the data array.
     */
    IBepCommunication  m_icmComm;
};

//-----------------------------------------------------------------------------
#endif //IcmDataSupplier_h
