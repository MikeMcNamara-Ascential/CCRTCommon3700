//*************************************************************************
// FILE:
//    $Header: /Core/ApplicationLayer/TestComponents/TransferCase/GenericTransferCase/GenericTcaseTCMain.cpp 3     10/14/04 6:39p Cward $Log:
//
//
//*************************************************************************
#include "GenericTcaseTC.h"
#include "CmdLineProcessor.h"

int main(int argc, char *argv[])
{
    CmdLineProcessor clp;		        // command line processor
    GenericTC *component = NULL;	// Component object

    try
    {
    	// parse the command line
        clp.ParseArguments(argc, argv);
        // create a GenericTcaseTC component object
        if(clp.IsDebugOn())	printf("Creating The Generic Tcase Component\n");
        component = new GenericTcaseTC();    // create a Generic Tcase component
        // Initialize the new test component
		if(clp.IsDebugOn())	printf("Initializing The Generic Tcase Component\n");
		component->Initialize(clp.GetConfigFile());
        // Start the test running
		if(clp.IsDebugOn())	printf("Generic Tcase Component Running\n");
		component->Run();			    // process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("(Generic Tcase -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("Generic Tcase -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("Generic Tcase -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

    if(clp.IsDebugOn())
		printf("Generic Tcase (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// GetRid of the test component
	if (component != NULL)  delete component;
	component = NULL;
   // Return control
    return (0);
}

