#------------------------------------------------------------------------------------------
# Tell CMake where to find the necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH}
                    ${CORE_PROTOCOLS_INC_PATH})
                    
#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(mockgenericemissionsmoduletemplatelib STATIC 
                MockGenericEmissionsModuleTemplate.cpp)
                
TARGET_LINK_LIBRARIES(mockgenericemissionsmoduletemplatelib
                          moduleinterfaces)



