#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(HandTest)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME LogDaemonTest)

#------------------------------------------------------------------------------------------
# Where to find the LogDaemon .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/LogDaemon)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(logdaemontestsuite STATIC
    ${EXE_NAME}Suite.cpp
        BogusLogFile.cpp
        BogusLogWriter.cpp
        BogusCoreLogFile.cpp
        BogusCoreLogFile2.cpp
        BogusNdb.cpp
        LogDaemonTest.cpp
        LogFileTest.cpp
        LogFileMapTest.cpp
        LogMsgQueueTest.cpp
        LogRotatorTest.cpp
        LogWriterTest.cpp
        LogFileIteratorTest.cpp
        PrivilegedLoggerTest.cpp
        CoreLogFileTest.cpp
        LogMessageTest.cpp)
TARGET_LINK_LIBRARIES(logdaemontestsuite logdaemonlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner logdaemontestsuite)

