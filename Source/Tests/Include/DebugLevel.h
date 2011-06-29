#ifndef DEBUG_LEVEL_H
#define DEBUG_LEVEL_H
//==============================================================================

#include <string>

#include <iostream>
#include <stdlib.h>

using namespace std;


/**
 * Common Debug Level class.
 */
class DebugLevel
{
public:

    DebugLevel(void) : m_debug(false)
    {
    };

    ~DebugLevel(void)
    {
    };


    void ViaEnv(const std::string &env)
    {

        ViaEnv(env.c_str());

    }

    void ViaEnv(const char *env)
    {

        if(getenv(env) == NULL)
        {
            m_debug = false;
//            cout << endl << env <<" not defined" << endl;
        }
        else if(getenv(env) != string("0"))
        {
            m_debug = true;
//            cout << endl << env << " is non zero" << endl;
        }
        else
        {
            m_debug = false;
//            cout << endl << env << " defined as zero" << endl;
        }
    }

    bool On(void)
    {
        return (m_debug);
    }

    void Set(void)
    {
        m_debug = true;
    }

    void Clear(void)
    {
        m_debug = false;
    }

	void LogIf(const std::string &val, bool forceNl = false)
	{
		if(m_debug)
		{
			if(forceNl)
			{
				cout << endl;
			}
			cout << val << endl;
		}
	}

	void LogIf(const char *val, bool forceNl = false)
	{
		std::string sval = val;
		LogIf(sval, forceNl);
	}

private:

    bool m_debug;
};


#endif
