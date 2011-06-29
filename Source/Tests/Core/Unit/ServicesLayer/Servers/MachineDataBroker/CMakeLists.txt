#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME MachineDataBrokerTest)

#------------------------------------------------------------------------------------------
# Where to find the MachineDataBroker .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/MachineDataBroker)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(machinedatabrokertestsuite STATIC
    ${EXE_NAME}Suite.cpp
    MachineDataBrokerTest.cpp)

TARGET_LINK_LIBRARIES(machinedatabrokertestsuite machinedatabrokerlib)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner machinedatabrokertestsuite)

