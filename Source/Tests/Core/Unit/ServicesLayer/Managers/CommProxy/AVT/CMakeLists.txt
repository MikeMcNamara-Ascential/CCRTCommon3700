#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(CAN)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME AvtCommProxyTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(avtcommproxytestsuite STATIC ${EXE_NAME}Suite.cpp)
TARGET_LINK_LIBRARIES(avtcommproxytestsuite
        avtcanproxytestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner avtcommproxytestsuite)

