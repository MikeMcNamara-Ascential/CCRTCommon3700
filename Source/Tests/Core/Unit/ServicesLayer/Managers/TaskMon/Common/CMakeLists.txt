#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME CommonTaskMonTest)

#------------------------------------------------------------------------------------------
# Where to find the TaskMon .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Managers/TaskMon/Common)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(commontaskmontestsuite STATIC
    ${EXE_NAME}Suite.cpp
    TaskMonTaskTest.cpp
    TaskMonTaskListTest.cpp)

TARGET_LINK_LIBRARIES(commontaskmontestsuite taskmonlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner commontaskmontestsuite)

