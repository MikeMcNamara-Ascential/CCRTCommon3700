//*************************************************************************
// FILE DESCRIPTION:
//  The MahindraKeypadManager handles all keypad entries and publishes the data to
//  all subscribers.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/MahindraKeypadManager.h $
// 
// 3     10/30/07 4:08p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:45p Rwiersem
// Made the test class a friend.  Added a flag to tell if we should open
// the logical port.  This is used to run the unit tests without the full
// system.  Added LoadAdditionalConfigurationItems() which is used to
// reload the configuration without restarting the process.
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 1     4/29/05 8:00a Gpattison
// Refactored the entire MapleKeypad Manager so that the InputServer could
// spawn it as an individual input device instead of having it as an object.
// This allows it to be added or removed from the system by making
// only configuration file changes.  It is now a stand alone BepServer type
// executable.
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:17p Khykin
// Initial checkin.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:43  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:07a Khykin
// Initial Checkin
//
// 1     1/18/03 8:04a Khykin
// Initial Checkin From DCTC
//
// 1     11/26/02 4:55p Derickso
// New Input Device manager.
//
//*************************************************************************
#ifndef MahindraKeypadManager_h
#define MahindraKeypadManager_h
//===========================================================================

#include "MapleKeypadManager.h"

/**
 * Miscellaneous constants to use.
 * @since Version 1.0
 */
//#define CR           '\x00D'    /* Carriage Return */
//#define LF           '\x00A'    /* Line Feed */
//#define ESC          '\x01B'    /* Escape */
//#define TERMINATOR   '\x002'    /* Command terminator */
//#define CLEAR_SCREEN '\x00C'    /* Clear the keypad display */


/**
 * MapleKeyPadManager.
 * The MapleKeypadManager will manage the Maple Keypads.  The manager will determine the
 * type of data entered on the keypad and make it available to all objects that require
 * the data.  The MapleKeypadManager will also display prompts on the maple keypad for
 * test operators.
 *
 * @author Dan Erickson
 * @version Version 1.0
 * @see AtomicThread
 * @see InputServer
 * @since November 20, 2002
 */
class MahindraKeypadManager : public MapleKeypadManager
{
    //friend class MapleKeypadManagerTest;

public:
    /**
     * Default constructor.
     */
    MahindraKeypadManager();
    /**
     * Default destructor.
     * @since Version 1.0
     */
    ~MahindraKeypadManager();
    /**
     * Initializes the manager to talk to the keypad as well as any data that needs to
     * be setup before we begin processing inputs.
     *
     * @param configNode Configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *configNode);
   
    /**
     * Set the current state of the Maple Keypad.
     *
     * @param state  State to set the keypad to.
     * @since Version 1.0
     */
    virtual void SetCurrentState(const std::string &state);

    /**
     * Main Run method to control the process
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);

protected:
    /**
     * Determine the type of data received from the keypad.
     *
     * @param data      Data from the Maple Keypad.
     * @param byteCount Number of bytes received from the keypad.
     * @since Version 1.0
     */
    void EvaluateData(unsigned char *data, const INT32 &byteCount);
    
private:

};

#endif   // MahindraKeypadManager_h
