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
if [[ -e /usr/local/bin/TaskMon.old ]]; then
    cp /usr/local/bin/TaskMon.old /tmp

    # Make sure the time is displayed correctly
    if [[ -e /etc/TZ ]]; then
	export TZ=$(head -l -n1 /etc/TZ)
    fi

    # Make sure TaskMon is executable
    if [[ -x /tmp/TaskMon.old ]]; then
	/tmp/TaskMon.old -k -l500
    else
        echo /tmp/TaskMon.old is not executable
	exec login
    fi
else
    echo /usr/local/bin/TaskMon.old does not exist
    exec login
fi
