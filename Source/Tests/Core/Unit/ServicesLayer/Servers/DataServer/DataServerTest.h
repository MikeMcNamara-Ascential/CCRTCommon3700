#ifndef DataServerTest_h
#define DataServerTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class DataServer;

class DataServerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DataServerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST_SUITE_END();

public:
    DataServerTest();
    ~DataServerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that verifies the DateServer object is constructed
     * properly
     */ 
    void TestConstructor(void);

    /** The DataServer object under test */
    DataServer *ds;
};
#endif
