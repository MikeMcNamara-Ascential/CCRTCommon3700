#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to root of the servers tree
#------------------------------------------------------------------------------------------
SET(CORE_SERVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}
        CACHE PATH "Path to the root of the core ServicesLayer tree"
        FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(DataServer)
ADD_SUBDIRECTORY(DsaServer)
ADD_SUBDIRECTORY(FaultServer)
ADD_SUBDIRECTORY(GenericServer)
ADD_SUBDIRECTORY(InputServer)
ADD_SUBDIRECTORY(Launcher)
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
ADD_SUBDIRECTORY(WinCcrtInterface)
