#ifndef SerialServerTestSuite_h
#define SerialServerTestSuite_h

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>
#include <string>

class SerialServerTestSuite : public CppUnit::TestSuite
{
    public:
        SerialServerTestSuite();
};

#endif

