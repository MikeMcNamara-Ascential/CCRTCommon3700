//#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TextTestProgressListener.h>

using std::cout;
using std::endl;
using std::string;

/**
 * Common progress listener class.
 */
//class ProgressListener : public CppUnit::BriefTestProgressListener
class ProgressListener : public CppUnit::TextTestProgressListener
{
public:
    void startTest(CppUnit::Test *test)
    {
        string name = test->getName();
        // cut off first 2 chars
//        name.erase(0,2);
        // Get the test suite name
        int position = name.find(':');
        name.erase(position, name.size() - position);
        //name = name.left(name.find('.'));
        if (m_name != name)
        {
            if (m_name != "")
            {
                cout << endl;
            }

            cout << "Running: " << name << endl;
            m_name = name;
        }
    }
private:
    string m_name;
};



