
//*************************************************************************
// FILE:
//    $Header: /Isuzu/ApplicationLayer/TestComponents/Axle/LsdTcMain.cpp     11/10/10 12:56p KHarman $Log:
//
//
//*************************************************************************
#include "LsdTc.h"
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
        component = new LsdTc();    // create a Generic Tcase component
        // Initialize the new test component
		if(clp.IsDebugOn())	printf("Initializing The LSD Component\n");
		component->Initialize(clp.GetConfigFile());
        // Start the test running
		if(clp.IsDebugOn())	printf("LSD Component Running\n");
		component->Run();			    // process until terminated
	}
	catch(XmlException &XmlErr)
	{
		printf("(LSD -%s: XmlExcpetion: %s", clp.GetConfigFile().c_str(), XmlErr.what());
	}
	catch(BepException &BepErr)
	{
		printf("LSD -%s: BepExcpetion: %s", clp.GetConfigFile().c_str(), BepErr.what());
	}
	catch(...)
	{
		printf("LSD -%s: Unknown Exception\n", clp.GetConfigFile().c_str());
	}

    if(clp.IsDebugOn())
		printf("LSD (%d, %s): Terminating\n", BposGetMyTaskId(), clp.GetConfigFile().c_str());
	// GetRid of the test component
	if (component != NULL)  delete component;
	component = NULL;
   // Return control
    return (0);
}

