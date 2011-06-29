#include "PlcMgrTestSuite.h"

// Include the header files for the test fixtures in this dir
#include "PlcMgrTest.h"
#include "PlcDf1Test.h"
#include "IDf1ProtocolTest.h"
#include "IRk512ProtocolTest.h"
#include "PlcRk512Test.h"
#include "MelsecProtocolTest.h"

PlcMgrTestSuite::PlcMgrTestSuite()
{
    // Add the inidividual test fixtures in this directory
    addTest(IDf1ProtocolTest::suite());
    addTest(PlcDf1Test::suite());
    addTest(PlcMgrTest::suite());
    addTest(IRk512ProtocolTest::suite());
    addTest(PlcRk512Test::suite());
    addTest(MelsecProtocolTest::suite());
}

