#------------------------------------------------------------------------------
# Tell CMake where to find the necessary .h files
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SYS_MON_INC_DIR})
                    
#------------------------------------------------------------------------------
# Tell CMake what is needed for the library
#------------------------------------------------------------------------------
ADD_LIBRARY(mocksystemmonitorlib STATIC MockSystemMonitor.cpp)
TARGET_LINK_LIBRARIES(mocksystemmonitorlib
                          sysmon)

