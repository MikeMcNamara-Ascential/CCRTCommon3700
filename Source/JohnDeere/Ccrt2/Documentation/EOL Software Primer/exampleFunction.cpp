template <class VehicleModuleType>
string ExampleTC<VehicleModuleType>::ExampleTestStep(void)
{
	//log function entry for tracing in log file 
    Log(LOG_FN_ENTRY, "ExampleTC::ExampleTestStep() - Enter");
	//set initial variables 
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult(BEP_TESTING_STATUS);
    string code("0000");
	//retrieves description from config file 
    string desc(GetTestStepInfo("Description"));
    bool exampleBool = false;
    //example Module message read 
    status = m_vehicleModule.ReadModuleData("ExampleModuleMEssage" ,exampleBool);
	//check to see if module response was good 
    if (status == BEP_STATUS_SUCCESS )
    {
		//good response log response 
        Log(LOG_FN_ENTRY, "Example bool from module: %s" , (exampleBool ? "True" : "False"));
		//reponse was good so set test to pass
        testResult =testPass;
    }
    else
    {
		//module response was not valid 
        Log(LOG_ERRORS, "ExampleTC::ExampleTestStep - Module Communication Failure");
		//set test ot fail 
        testResult = testFail;
        code = GetFaultCode("Module Comm Failure");
        desc = GetFaultDescription("Module Comm Failure");
    }
    //if cng set cng true or false
    //send test reuslt  
    SendTestResultWithDetail(testResult, desc, code);
	//log funtion exit 
    Log(LOG_FN_ENTRY, "ExampleTC::ExampleTestStep() - Exit");
	//return test Result 
    return testResult;
}
