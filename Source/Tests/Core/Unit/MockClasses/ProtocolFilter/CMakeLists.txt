#------------------------------------------------------------------------------------------
# Tell CMake where to find the necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_PROTOCOLS_INC_PATH}
                    ${CORE_MODULES_INC_PATH})
                    
#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(mockprotocolfilterlib STATIC MockProtocolFilter.cpp)
TARGET_LINK_LIBRARIES(mockprotocolfilterlib
                          protocolfilters 
                          moduleinterfaces)



