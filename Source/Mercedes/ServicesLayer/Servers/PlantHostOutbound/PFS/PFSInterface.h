//******************************************************************************
// Description:
//   PfsServer class to handle communications between the DCX
//   Plant Feedback System(PFS) and the Burke Porter system
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ServicesLayer/Servers/PlantHostOutbound/Include/PFSInterface.h $
// 
// 5     5/10/07 4:49p Derickso
// Updated to allow separate TPM message to be reported to PFS.
// 
// 4     4/04/07 11:06a Derickso
// Updated to have base class call LoadAdditionalConfigurationItems from
// Initialize().
// Removed overloaded Initialize functions and replaced with overloaded
// LoadAdditionalConfigurationItems.
// 
// 3     3/27/07 2:09p Derickso
// Added new Initialize functions.
// Corrected location of data items from config files.
// Corrected errors from testing and debug.
// 
// 2     3/23/07 3:04p Derickso
// Refactored to derive from HostInterface.
// 
// 1     3/23/07 12:30p Derickso
// Moved from Core to Dcx specific.
// 
// 3     1/18/07 3:04p Rwiersem
// Changes for the 07011901 core release:
//                                        
// - Made the test class a friend.
// - Added DoResultProcessing().
// - Moved m_plantHost and m_pfsComm from private to protected.
// - Added m_msgVin.
// - Added AddPfsHeader().
// - Added m_addPfsHeader.
// 
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     11/24/04 12:11p Bmeinke
// Added GetPlantHostComm() method to get a reference to the SerialChannel
// used to communicate with the host system (needed for new QMSInterface
// object)
// Added GetPlantHost() method to get a reference to the PlantHostOutbound
// object that instantiated us (needed for new QMSInterface object)
//
// 1     7/21/04 10:18a Cward
//
// 2     5/25/04 4:34p Cward
// Removed definition of ACK.
//
// 1     1/28/04 12:47p Cward
//
// 1     8/21/03 9:09a Derickso
// 05WK CCRT Project
//
// 1     7/09/03 2:31p Rwiersem
// Initial version.
//
// 1     7/03/03 5:20p Khykin
// Initial checkin
//
// 1     6/12/03 4:19p Khykin
// Initial checkin.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:44  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:08a Khykin
// Initial Checkin
//
// 1     1/18/03 8:05a Khykin
// Initial Checkin From DCTC
//
// 1     7/29/02 8:22a Dan.erickson
// Created.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef PFSInterface_h
#define PFSInterface_h
//==============================================================================

#include "HostInterface.h"

/**
 * This thread is spun up by the PlantHostOutbound if the plant is equipped with PFS.
 * The PFSInterface class will invoke the PlantHostOutbound methods for building the
 * test result string to be sent to PFS after it has been signaled that new test
 * results have been posted.  The PFSInterface class will then add the
 * appropriate wrappers to the message and then call the PlantHostoutbound method for
 * transmitting the message to the external host, which for this class is PFS.
 *
 * @author Dan Erickson
 */
class PFSInterface : public HostInterface
{
    friend class PFSInterfaceTest;

public:
    /**
     * Class constructor.
     */
    PFSInterface();
    /**
     * Class destructor.
     */
    virtual ~PFSInterface();
    /**
     * Process the test result node, creating and sending
     * the appropriate messages to plant host
     * 
     * @param testResults pointer to the test result node copied from the file
     */
    virtual void DoResultProcessing(const XmlNode *testResults);

protected:
    /**
     * Process the test results in order to build the TPm result string to send to PFS.
     * 
     * @param testResults
     *               Test results to use to generate the TPM result string.
     */
    void DoTpmResultProcessing(const XmlNode *testResults);
    /**
     * Store additional items from the config file to complete the initialization of the host interface.
     * 
     * @param config Configuration data used to complete the initialization of the host interface.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *config);
    /**
     * Generate the result string to be sent to the host system.
     * 
     * @param testResults
     *               Test result data to be used for generating the result string for the host system.
     * 
     * @return Result string to be transmitted to the host system.
     */
    virtual string GenerateHostResultString(const XmlNode *testResults);
    /**
     * Update the test result string header if any information is dependant on test results.
     *
     * @param data   Data to update the header with.
     */
    virtual void UpdatePFSHeader(const INT32 &data);
    /**
     * Set the information that will be constant for eac PFS record sent out.
     *
     * @param header Header information to use.
     */
    virtual void SetPFSHeader(const XmlNode *header);
    /**
     * Return the header information to be added to each PFS string.
     *
     * @return PFS Header information.
     */
    const string GetPFSHeader(void);
    /**
     * Set/Get the start index for the serial number length field.
     * 
     * @param index  Start index of the serial number length field.
     * 
     * @return Start index of the serial number length field.
     */
    const INT32& SerialNumberIndex(const INT32 *index = NULL);
    /**
     * Set/Get the length of the serial number length field.
     * 
     * @param length Length of the serial number field.
     * 
     * @return Length of the serial number field.
     */
    const INT32& SerialNumberFieldLength(const INT32 *length = NULL);
    /**
     * Get/Set the index for the detail section length in the PFS header.
     * 
     * @param index  Index of the detail section length field in the PFS header.
     * 
     * @return Index of the detail section length field in the PFS header.
     */
    const INT32& DetailSectionFieldIndex(const INT32 *index = NULL);
    /**
     * Set/Get the length of the detail section length field in the PFS header.
     * 
     * @param length Length of the detail section length field in the PFS header.
     * 
     * @return Length of the detail section length field in the PFS header.
     */
    const INT32& DetailSectionFieldLength(const INT32 *length = NULL);
    /**
     * Set the mapping for the base test result message.
     *
     * @param baseMap Test result base string.
     */
    inline void SetPFSBaseMap(const XmlNode *baseMap);
    /**
     * Get/Set the length of the PFS detail section.
     * This must be included in the PFS header that is sent to PFS.
     * 
     * @param sectionLength
     *               Length of the detail section of the PFS result string.
     * 
     * @return Length of the detail section of the PFS result string.
     */
    const INT32& DetailSectionLength(const INT32 *sectionLength = NULL);
    /**
     * Save/Retrieve the result detail map.
     * If the parameter is NULL, just the detail result string map will be returned.  Otherwise, 
     * the result detail map will be stored.
     * 
     * @param detailMap Detail result string map to use when building the result string.
     * 
     * @return Detail result string map to use when building the result string.
     */
    const XmlNodeMap& DetailMap(const XmlNode *detailMap = NULL);
    /**
     * Store/Retrieve the mapping to use to generate the TPM result string to PFS.
     * 
     * @param resultMap Result map to use for generating the TPM result string to PFS.
     * 
     * @return Result map to use for generating the TPM result string to PFS.
     */
    const XmlNodeMap& TpmResultMap(const XmlNode *resultMap = NULL);

private:
    /**
     * Set/Get the flag to add PFS header informatrion to the result string prior to transmitting.
     * 
     * @param addPfsHeader
     *               Flag to add PFS header to result string.
     *               <ul>
     *               <li> True - Add PFS header to the result string. </li>
     *               <li> False - Do not add the PFS header information to the result string. </li>
     *               </ul>
     * 
     * @return Flag to add PFS header information to the result string.
     *         <ul>
     *         <li> True - Add PFS header to the result string. </li>
     *         <li> False - Do not add the PFS header information to the result string. </li>
     *         </ul>
     */
    const bool& AddPfsHeader(const string *addPfsHeader = NULL);
    /**
     * Set/Get the flag indicating if a separate message should be generated to report TPM results to PFS.
     * 
     * @param reportTpmResult
     *               Flag indicating if separate TPM result string should be generated.
     * 
     * @return Flag indicating if separate TPM result string should be generated.
     */
    const bool& ReportTpmResults(const string *reportTpmResult = NULL);
    /**
     * Store/Retrieve the message ID to use for sending the TPM result string.
     * 
     * @param messageId Message ID to use for sending the TPM result string.
     * 
     * @return Message ID to use for sending the TPM result string.
     */
    UINT8* TpmMessageId(const UINT8 *messageId = NULL);
    /**
     * Store/Retrieve the message ID to use for sending the test result string.
     * 
     * @param messageId Message ID to use for sending the test result string.
     * 
     * @return Message ID to use for sending the test result string.
     */
    UINT8* TestResultMessageId(const UINT8 *messageId = NULL);
    /**
     * Store the list of body styles which need a TPM result generated.
     * 
     * @param tpmBodyStyles
     *               List of body styles which require a TPM result string to be reported to PFS.
     */
    void StoreTpmBodyStyles(const XmlNode *tpmBodyStyles);
    /**
     * Determine if a TPM result string should be generated.
     * The supplied body style will be used to determine if TPM data should be reported to PFS.
     * 
     * @param bodyStyle Body style to use for determining if a TPm result should be generated and reported to PFS.
     * 
     * @return Flag indicating if a TPM result string should be generated and sent to PFS.
     */
    const bool ShouldGenerateTpmResultString(const string &bodyStyle);
    /**
     * Message ID to use for the TPM result message.
     */
    UINT8 m_tpmMessageId;
    /**
     * Message ID to use for reporting the test result string to PFS.
     */
    UINT8 m_testResultMessageId;
    /**
     * Stores the start index of the Serial Number length field.
     */
    INT32 m_serialNumberIndex;
    /**
     * Stores the length of the Serial Number length field.
     */
    INT32 m_serialNumberLength;
    /**
     * Index of the detail section length in the PFS header.
     */
    INT32 m_detailSectionFieldIndex;
    /**
     * Length of the detail section length field in the PFS header.
     */
    INT32 m_detailSectionFieldLength;
    /**
     * Length of the detail section to be sent to PFS.
     */
    INT32 m_detailSectionLength;
    /**
     * The header block to add to all PFS strings.
     */
    string  m_pfsHeader;
    /**
     * Flag to add PFS header to the PFS string prior to transmitting.
     */
    bool m_addPfsHeader;
    /**
     * Flag to indicate if a separate message should be generated to report TPM results to PFS.
     */
    bool m_reportTpmResults;
    /**
     * The map for the detail section of the Test result string.  This map is specific to
     * each plant.
     */
    XmlNodeMap   m_pfsDetailMap;
    /**
     * TPM result string format mapping to use to generate the TPM test result string to be sent to PFS.
     */
    XmlNodeMap   m_tpmResultMap;
    /**
     * List of body styles to generate TPM result data for.
     */
    XmlNodeMap   m_tpmBodyStyles;
};
#endif  // PFSInterface_h
