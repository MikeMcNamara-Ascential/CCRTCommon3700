#!/bin/sh
#ifdef __USAGE
#
#%C
#  Copies the TaskMon utility from /usr/local/bin to /tmp
#   and runs it from /tmp with '/tmp/TaskMon -l50 -k'
#
#endif

# Make sure that the notaskmon file does not exist
if [[ -e /etc/system/config/notaskmon ]]; then
	/bin/login
	return 0
fi

# Make sure the real TaskMon exists
if [[ -e /usr/local/bin/TaskMonTerm ]]; then
    cp /usr/local/bin/TaskMonTerm /tmp

    # Make sure the time is displayed correctly
    if [[ -e /etc/TZ ]]; then
            export TZ=$(head -l -n1 /etc/TZ)
    fi

    # Make sure TaskMon is executable
    if [[ -x /tmp/TaskMonTerm ]]; then
        # Check for config file
        if [[ -e /home/CCRT/Rewrite/Configuration/Servers/TaskMonConfig.xml ]]; then
            /tmp/TaskMonTerm -f /home/CCRT/Rewrite/Configuration/Servers/TaskMonConfig.xml
        else
            /tmp/TaskMonTerm -c -k -l500
        fi
    else
        echo /tmp/TaskMonTerm is not executable
    fi
else
    echo /usr/local/bin/TaskMonTerm does not exist
fi
