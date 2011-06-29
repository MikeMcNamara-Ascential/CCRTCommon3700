#------------------------------------------------------------------------------------------
# Create a list of all the cpp files in this directory to be added to the library
#------------------------------------------------------------------------------------------
SET(TASKMON_LIB_SOURCES
	TaskMonProcTask.cpp
	TaskMonTask.cpp
	TaskMonTaskList.cpp
	TaskMonTimer.cpp)

#------------------------------------------------------------------------------------------
# Create the TaskMon library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(taskmonlib STATIC ${TASKMON_LIB_SOURCES})
