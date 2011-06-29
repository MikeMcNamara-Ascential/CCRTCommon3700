#!/bin/sh
#ifdef __USAGE
#
#%C
#  Determines whether the old style or new style of
#   Task Monitor is needed, and invokes the
#   appropriate shell script.
#
#endif

# Make sure that the notaskmon file does not exist
if [[ -e /etc/system/config/notaskmon ]]; then
	/bin/login
	return 0
fi

# If we have a TaskMonConfig.xml file, use the new TaskMon
if [[ -e /home/CCRT/Rewrite/Configuration/Servers/TaskMonConfig.xml ]]; then
	exec /usr/local/bin/TaskMon.new.sh
# Otherwise use the old TaskMon
else
	exec /usr/local/bin/TaskMon.old.sh
fi
