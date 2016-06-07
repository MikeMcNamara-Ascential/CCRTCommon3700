//*************************************************************************
// FILE DESCRIPTION:
// This file contains the functions necessary to create a MapleKeypad manager
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
//    $Log: /Working/Source/Core/ServicesLayer/Servers/InputServer/MapleKeypadManager/MapleKeypadManagerMain.cpp $
// 
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
//*************************************************************************

#include "MahindraKeypadManager.h"


int main(int argc, char *argv[])
{
	InputDeviceBase *m_Keypad = NULL;	// mapleKeypad object

	try
	{
		printf("Creating The MahindraKeypadManager Object\n");
		m_Keypad = new MahindraKeypadManager();    // create a Mahindra Keypad object
		// create, initialize, and process messages for the named data broker
		printf("Initializing The Mahindra Keypad Object\n");
		m_Keypad->Initialize(argc,argv);
		printf("Mahindra Keypad Object Running\n");
		m_Keypad->Run();							// process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("MahindraKeypadManagerMain: XmlExcepetion: %s", XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("MahindraKeypadManagerMain: BepExcpetion: %s", BepErr.what());
	}
	catch(...)
	{
		printf("MahindraKeypadManagerMain: Unknown Exception\n");
	}

	if(m_Keypad != NULL)	delete m_Keypad;	// delete the Mahindra Keypad object
	printf("MahindraKeypadManagerMain (%d): Terminating\n", BposGetMyTaskId());

	return(0);
}

#ifdef __USAGE

MahindraKeypadManager Version 1.0
%C [options]
options: -fConfigFile.xml -v?
Where ConfigFile.xml is the xml configuration file needed forset up
and -v indicates that prints should be enabled formain loop and ? is any valid whole number.

#endif	// __USAGE

