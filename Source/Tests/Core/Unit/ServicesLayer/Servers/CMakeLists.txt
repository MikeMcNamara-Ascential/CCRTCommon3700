#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(DataServer)
ADD_SUBDIRECTORY(InputServer)
ADD_SUBDIRECTORY(LogDaemon)
ADD_SUBDIRECTORY(MachineDataBroker)
ADD_SUBDIRECTORY(MotorController)
ADD_SUBDIRECTORY(NamedDataBroker)
ADD_SUBDIRECTORY(PlantHostInbound)
ADD_SUBDIRECTORY(PlantHostOutbound)
ADD_SUBDIRECTORY(PromptServer)
ADD_SUBDIRECTORY(SystemMonitor)
ADD_SUBDIRECTORY(SystemScreenSaver)
ADD_SUBDIRECTORY(TestResultServer)
ADD_SUBDIRECTORY(VehicleDataBroker)
ADD_SUBDIRECTORY(VehicleDataServer)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME ServersTest)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(serverstestsuite STATIC ${EXE_NAME}Suite.cpp)
TARGET_LINK_LIBRARIES(serverstestsuite
        dataservertestsuite
        inputservertestsuite
        logdaemontestsuite
        machinedatabrokertestsuite
        motorcontrollertestsuite
        nameddatabrokertestsuite
        planthostinboundtestsuite
        planthostoutboundtestsuite
        promptservertestsuite
        systemmonitortestsuite
        systemscreensavertestsuite
        testresultservertestsuite
        vehicledatabrokertestsuite
        vehicledataservertestsuite)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner serverstestsuite)

