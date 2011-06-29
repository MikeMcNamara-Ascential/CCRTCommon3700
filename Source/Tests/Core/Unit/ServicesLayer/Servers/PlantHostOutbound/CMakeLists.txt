#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(HostInterface)
ADD_SUBDIRECTORY(DAQ)

#-------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#-------------------------------------------------------------------------------
SET(EXE_NAME PlantHostOutboundTest)

#-------------------------------------------------------------------------------
# Where to find the PlantHostOutbound .h files
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/PlantHostOutbound/Include
                    ${CCRT_ROOT}/Tests/Core/Unit/MockClasses/PlantHostOutbound)

#-------------------------------------------------------------------------------
# Tell CMake what is needed for the test suite library
#-------------------------------------------------------------------------------
ADD_LIBRARY(planthostoutboundtestsuite STATIC
                ${EXE_NAME}Suite.cpp
                ${EXE_NAME}.cpp)

TARGET_LINK_LIBRARIES(planthostoutboundtestsuite 
                        PlantHostOut
                        mockplanthostoutboundlib
                        hostinterfacetestsuite
                        daqinterfacetestsuite)

#-------------------------------------------------------------------------------
# Tell CMake what is needed for the runner
#-------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}Runner ${EXE_NAME}Runner.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME}Runner planthostoutboundtestsuite)
