#------------------------------------------------------------------------------------------
# Tell CMake where to find the necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_COMMON_TC_INC_PATH})
                    
#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(mockgenerictclib STATIC 
                MockGenericTc.cpp)
                
TARGET_LINK_LIBRARIES(mockgenerictclib
                          basebrake)



