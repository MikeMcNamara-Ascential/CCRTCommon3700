#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the DCX common component headers and .cpp
#------------------------------------------------------------------------------------------
SET(KIA_BRAKE_TC_PATHS
                ${CMAKE_CURRENT_SOURCE_DIR}/Include
                ${CMAKE_CURRENT_SOURCE_DIR}/Common)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES( 
                ${KIA_BRAKE_TC_PATHS}
                ${BRAKE_MODULE_PATHS}
                ${CORE_BRAKE_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_BRAKE_TC_LIB_PATH})
LINK_LIBRARIES(basebrake m)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(Include)
ADD_SUBDIRECTORY(BaseBrake)
ADD_SUBDIRECTORY(Mando)
ADD_SUBDIRECTORY(Bosch8)
