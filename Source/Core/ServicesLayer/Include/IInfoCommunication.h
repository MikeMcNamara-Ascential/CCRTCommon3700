//******************************************************************************
//
// FILE:    IInfoCommunication
//     $Archive::  $
//     $Author:: Rwiersem    $
//     $Date:: 10/30/07 3:38 $
//
// Description:
//  Extend IBepCommunication to support reporting of information
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/IInfoCommunication.h $
// 
// 2     10/30/07 3:38p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 1     12/21/03 6:26p Cward
// 
// 2     10/24/03 11:41a Khykin
// Updated with latest changes from Korea and Flatrock.
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
// 2     6/26/03 1:40p Khykin
// Updated with Dans latest changes.
// 
// 14    6/24/03 10:35a Derickso
// Default process name.
// 
// 13    4/05/02 3:01p Dan.erickson
// Updated ValidateMessage() due to changes in 
// IBepCommunication::ValidateMessage().
// 
// 12    2/26/02 2:41p Dan.erickson
// Updated SetDescription and SetProcess methods to return 
// the node that was built.
// 
// 11    2/07/02 7:28p Skypig13
// Reordered include files
//
// 10    11/26/01 3:40p Dan.erickson
// Moved AddDetail and AddDetailUnits methods to IInfoCommunication.
//
// 9     11/13/01 1:28p Dan.erickson
// Added GetDescription and GetProcess methods.
//
// 8     11/06/01 10:23a Dan.erickson
// Moved to IInfoCommunication directory.
// Removed methods which were geared more towards providing information
// for TestRsultServer
//
// 7     11/06/01 7:40a Dan.erickson
// Testing.
//
// 6     11/01/01 6:06p Dan.erickson
// Removed const for BepComponent item in SpecialProcessing prototype.
//
// 5     11/01/01 3:31p Dan.erickson
// Added name to Initialize method prototype for
// IInterProcessCommunication.
//
// 4     11/01/01 2:56p Dan.erickson
// Compiles, not tested.
//
// 3     10/26/01 4:01p Dan.erickson
// Revised to extend IBepCommunication only.
//
// 2     10/22/01 3:48p Dan.erickson
// Re-factored to inherit from BepServer.
//
// 1     10/19/01 2:37p Dan.erickson
// Revised from ITestResult.
//
// 2     10/16/01 4:31p Dan.erickson
// Debugged.
//
// 1     10/16/01 8:41a Dan.erickson
// Created
//
//
//******************************************************************************

#ifndef IInfoCommunication_h
#define IInfoCommunication_h

#include "BepComponent.h"
#include "IBepCommunication.h"

/**
 * Extends the IBepCommunication class.  Provides methods
 * to facilitate passing data which must have certain
 * attributes.
 * 
 * @author Dan Erickson
 * @version Version 1.4
 * @since Version 1.0
 */
class IInfoCommunication : public IBepCommunication
{
public:
    // Default constructor
    IInfoCommunication(void);
    // Default Destructor
    virtual ~IInfoCommunication(void);
    /**
     * Set the Reason/Description for the error, failure,
     * test result, etc.
     * 
     * @param description
     *                  Text description/Reason
     * @param existNode Exisitng XmlNode with the attribute added/updated
     * 
     * @return The node passed in (existNode)
     * @since Version 1.4
     */
    XmlNode * SetDescription(const std::string &description, XmlNode *existNode);
    
    /**
     * Set the process attribute of the given node
     * 
     * @param process   Name of the process the message originated from.
     * @param existNode Exisitng XmlNode to add/update process attribute to
     * 
     * @return The node passed in (existNode)
     * @since Version 1.4
     */
    XmlNode * SetProcess(const std::string &process, XmlNode *existNode);

    /**
     * Get the description attribute from the node specified.
     *
     * @param node   Node to get description from.
     * @return Value of the description attribute.
     * @since Version 1.1
     */
    std::string GetDescription(const XmlNode *node);
    /**
     * Get the process attribute from the node specified.
     *
     * @param node   Node to get process from.
     * @return Value of the process attribute.
     * @since Version 1.1
     */
    std::string GetProcess(const XmlNode *node);
    /**
     * Adds a child to the specified message node.
     * 
     * @param detail    Name of the detail to add.
     * @param value     Data to add.
     * @param existNode Existing node to add/update the attribute for
     * @since Version 1.4
     */
    void AddDetail(const std::string &detail, const std::string &value, XmlNode *existNode);
    /**
     * Add the Units attribute to the detail node.
     * 
     * @param detail    Which detail node to add the attribute to.
     * @param units     Units of the data (mph, kph, lbf, kgf, ...)
     * @param existNode Existing XmlNode to add/update the attribute for
     * @since Version 1.4
     */
    virtual void AddDetailUnits(const std::string &detail, const std::string &units, XmlNode *existNode);
    /**
     * Special processing to be done to each node.
     * Process and description attributes are added to each node.
     *
     * @param node   Node to add attributes to.
     * @param type   Type of operation we are performing.
     * @param server The server requesting the special processing.
     */
    virtual void SpecialProcessing(XmlNode *node, const std::string type, BepComponent *server);
    /**
     * Validate an incomming message and return if it is valid
     * or not.
     *
     * @param doc    Document to be validated
     * @return Status of operation
     * @since Version 1.0
     */
    virtual bool ValidateMessage(const XmlNode* doc);

protected:
    /**
     * Verifies that the attribute is valid
     *
     * @param attribute value of attribute
     * @return If valid
     * @since Version 1.0
     */
    virtual bool CheckAttribute(const XmlNode *attribute);
};

#endif // IInfoCommunication_h

