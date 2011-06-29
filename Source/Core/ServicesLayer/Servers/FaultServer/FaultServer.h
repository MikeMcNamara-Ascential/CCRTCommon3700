//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/FaultServer/FaultServer.h 2     3/30/06 1:43a Cward $
//
// FILE DESCRIPTION:
//   This defines the FaultServer object.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/FaultServer/FaultServer.h $
// 
// 2     3/30/06 1:43a Cward
// Updated to add Fault History Log PE#79
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:58p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 3     6/26/03 1:29p Khykin
// Updated with Tims latest changes.
//
// 14    6/24/03 10:37a Tbochene
// Added synchronization object to make access to fault information thread
// safe
//
// 13    5/30/02 4:16p Skypig13
// Removed unnecessary #include
//
// 12    4/25/02 4:40p Skypig13
// Updated with new changes
//
// 10    3/21/02 2:00p Dan.erickson
// Updated prototypes so an IFaultServer object can be passed
// around to be passed to the Notify() method.
//
// 9     3/20/02 9:11a Dan.erickson
// Overloaded Initialize(std::string &fileName) method.
//
// 8     3/11/02 1:16p Dan.erickson
// Removed include of IFaultServer.h.
// Removed SpecialProcessing(), Notify() and Handle() methods.
//
// 7     3/05/02 5:03p Dan.erickson
// Removed SpecialProcessing method.  No longer needed now that Xml nodes
// are better supported.
//
// 6     2/12/02 5:14p Dan.erickson
// Updated after restructure of BepServer.
//
// 5     1/11/02 4:51p Dan.erickson
// Added methods to set and get the all faults tag.
//
// 4     12/03/01 11:37a Dan.erickson
// Added Notify method.
//
// 3     11/20/01 10:57a Dan.erickson
// Modified the publish scheme when faults are set and cleared.
//
// 2     11/20/01 10:13a Dan.erickson
// Finished writing.
//
// 1     11/20/01 8:31a Dan.erickson
// New server files.
//
//
//*************************************************************************
#ifndef FaultServer_h
#define FaultServer_h

#include "BepSync.h"
#include "BepServer.h"
#include "IQnxDataServer.h"

#define SEVERITY_MAP  "SeverityFaultMap"
#define PROCESS_MAP   "ProcessFaultMap"

/**
 * The FaultServer will store all active faults reported.
 * Faults can be retrieved by process name, severity level,
 * by tag, or all faults can be retrieved.
 *
 * @author Dan Erickson
 * @version Version 2.1
 * @since Version 1.0
 */
class FaultServer : public BepServer
{
public:
	/**
	 * Constructor.
	 * @since Version 2.0
	 */
	FaultServer();
	/**
	 * Class destructor.
	 * @since Version 2.0
	 */
	virtual ~FaultServer();
	/**
	 * Method for initialization passing in the file name
	 *
	 * @param fileName Name of the XML configuration file to be used to initialize
	 * @since Version 2.0
	 */
	virtual void Initialize(const std::string &fileName);
	/**
	 * Method for initialization.
	 *
	 * @param document Reference to a parsed configuration document.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);
	/**
     * <b>Description:</b>
     * Method that is called to command the component to perform any system 
     * registration functions necessary.
     *
     * @return BEP_SUCCESS_RESPONSE if the registration was successful
     *         BEP_FAILURE_RESPONSE if the registration was not successful
     */
    virtual const string Register(void);
	/**
	 * Request to read a data tag
	 *
	 * @param dataNode    Data to be read
	 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
	 * @return Value of tag
	 * @since Version 1.0
	 */
	virtual const std::string Read(const XmlNode *dataNode, const INT32 rate=0);

	/**
	 * Request to write a data tag
	 *
	 * @param dataNode   Tag and data to be written
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 1.0
	 */
	virtual const std::string Write(const XmlNode *dataNode);
	/**
	 * Return the tag being used for subscriptions for all faults.
	 *
	 * @return All faults tag.
	 * @since Version 1.2
	 */
	const std::string GetAllFaultsTag(void)
	{
		return m_allFaultsTag;
	}
protected:
	/**
	 * Clears the data sent to the server from the data structure containing it
	 * @since Version 1.0
	 */
	virtual void ClearData(void)
	{
		m_data.clear(true);
		m_processFaults.clear(true);
		m_severityFaults.clear(true);
	};
	/**
	 * Save the tag being used to report all faults.
	 *
	 * @param tag    Tag being used for all faults reporting.
	 * @since Version 1.2
	 */
	void SetAllFaultsTag(const std::string &tag)
	{
		m_allFaultsTag = tag;
	};

private:
	/**
	 * Removes from the faults list all faults which have been cleared.
	 *
	 * @param fault  Fault to be SET or CLEAR.
	 * @param value  SET or CLEAR.
	 * @param notifyComm Communication object to use for Publishing the data to subscribers.
	 * @since Version 1.0
	 */
	void UpdateFaultList(const std::string &fault, const std::string &value);

	/**
	 * Add a fault to the process name fault list.
	 *
	 * @param node   Fault to add to the list.
	 * @param notifyComm Communication object to use for Publishing the data to subscribers.
	 * @param publishFault
	 *               Should a notification be sent out.
	 * @since Version 1.0
	 */
	void AddProcessNameFault(const XmlNode *node, const bool publishFault = false);

	/**
	 * Add a fault to the severity level fault list.
	 *
	 * @param node   Fault to add to the list.
	 * @param notifyComm Communication object to use for Publishing the data to subscribers.
	 * @param publishFault
	 *               Should a notification be sent out.
	 * @since Version 1.0
	 */
	void AddSeverityLevelFault(const XmlNode *node, const bool publishFault = false);


    /**
     * crafts a string that is unique and represents the current time
     * used to modify the fault logger filename to prevent overwriting on reload
     *
     * @return the specially crafted string
     */
    std::string GetFileMod(void);

    /**
     * helper to set up the fault history log
     *
     * @param document configuration document.
     */
    void SetUpFaultLogger(const XmlNode *document);



	/**
	 * Find the fault node in the fault map specified.
	 *
	 * @param mapType Which map to look for the fault in.
	 * @param tag    Fault to look for.
	 * @return Pointer to the fault node.
	 * @since Version 1.0
	 */
	XmlNode * FindFaultNode(const string &mapType, const string &tag);

	/**
     * Get/Set the flag indicating if the QNX data server shold be connected to.
     * 
     * @param connect Flag indicating if the QNX data server should be connected to.
     * 
     * @return Flag indicating if the QNX data server should be connected to.
     */
    const bool& ConnectToQnxDataServer(const bool *connect = NULL);
	/**
	 * All faults listed by process name.
	 * @since Version 1.0
	 */
	XmlNodeMap m_processFaults;
	/**
	 * All faults listed by severity level.
	 * @since Version 1.0
	 */
	XmlNodeMap m_severityFaults;
	/**
	 * The tag being used to report all faults.
	 * @since Version 1.2
	 */
	std::string m_allFaultsTag;
	/**
	 * Access control object for reading and writing fault data
	 * @since Version 2.1
	 */
	BepMutex m_faultDataLock;

    /**
     * second ILogger path, for recording faults to
     * a unique fault log.
     */
    ILogger *m_faultLogger;
    /** Interface to the QNX data server */
    IQnxDataServer m_qnxDataServer;
    /** Flag indicating if we need to connect to the QNX data server */
    bool m_connectToQnxDataServer;
};

#endif //FaultServer_h
