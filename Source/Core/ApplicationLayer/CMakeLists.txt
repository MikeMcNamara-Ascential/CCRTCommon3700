#------------------------------------------------------------------------------------------
# Define the root of the ApplicationLayer source tree
#------------------------------------------------------------------------------------------
SET(CORE_APP_LAYER_ROOT ${CCRT_ROOT}/Core/ApplicationLayer)

#------------------------------------------------------------------------------------------
# Define the paths to the ModuleInterfaces and ProtocolFilters directories for use
# in the TestComponent make files
#------------------------------------------------------------------------------------------
SET(CORE_MODULES_ROOT ${CORE_APP_LAYER_ROOT}/ModuleInterfaces)
SET(CORE_PROTOCOLS_ROOT ${CORE_APP_LAYER_ROOT}/ProtocolFilters)

#------------------------------------------------------------------------------------------
# Add the common App Layer include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_APP_LAYER_ROOT}/Include
                ${CORE_MODULES_ROOT}/Include
                ${CORE_MODULES_ROOT}/Common
                ${CORE_PROTOCOLS_ROOT}/Include
                ${CORE_PROTOCOLS_ROOT}/Common
                ${CORE_APP_LAYER_ROOT}/TestComponents/Include
                ${CORE_APP_LAYER_ROOT}/TestComponents/Common)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(ProtocolFilters)
ADD_SUBDIRECTORY(ModuleInterfaces)
ADD_SUBDIRECTORY(TestComponents)
ADD_SUBDIRECTORY(VehicleTest)
