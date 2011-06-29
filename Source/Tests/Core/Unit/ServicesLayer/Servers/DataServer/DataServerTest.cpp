#include "DataServerTest.h"
#include "DataServer.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
DataServerTest::DataServerTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" 
            << endl;
        exit(1);
    }
}

//-----------------------------------------------------------------------------
DataServerTest::~DataServerTest()
{
}

//-----------------------------------------------------------------------------
void DataServerTest::setUp()
{
    ds = new DataServer();
}

//-----------------------------------------------------------------------------
void DataServerTest::tearDown()
{
    delete ds;
}

//-----------------------------------------------------------------------------
void DataServerTest::TestConstructor(void)
{
    CPPUNIT_ASSERT(ds->m_commConfig == NULL);
    CPPUNIT_ASSERT(ds->m_ndb == NULL);
}
