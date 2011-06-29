#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(CommProxy)
ADD_SUBDIRECTORY(ICM)
ADD_SUBDIRECTORY(PlcMgr)
ADD_SUBDIRECTORY(SerialServer)
ADD_SUBDIRECTORY(TaskMon)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME ManagersTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(managerstestsuite STATIC ${EXE_NAME}Suite.cpp)
TARGET_LINK_LIBRARIES(managerstestsuite
        commproxytestsuite
        icmtestsuite
        plcmgrtestsuite
        serialservertestsuite
        taskmontestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner managerstestsuite)

