#-------------------------------------------------------------------------------
# Tell CMake where to find the necessary .h files
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${CORE_SERVICES_LAYER_DIR}/Servers/PlantHostOutbound/Include)
                    
#-------------------------------------------------------------------------------
# Tell CMake what is needed for the library
#-------------------------------------------------------------------------------
ADD_LIBRARY(mockplanthostoutboundlib STATIC MockPlantHostOutbound.cpp)
TARGET_LINK_LIBRARIES(mockplanthostoutboundlib PlantHostOut) 



